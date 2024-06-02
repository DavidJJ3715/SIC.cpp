#ifndef _Star_
#define _Star_
#include "func.h"

class star
{ //A star is an object that scrolls on the backdrop that is stored in a bespoke list
    public:
        star(double, double);
        void update(SDL_Renderer*), draw(SDL_Renderer*);
        std::shared_ptr<star> last = nullptr;
        std::shared_ptr<star> next = nullptr;
        double xCoord, yCoord, velocity = 0.15;
    private:
        SDL_Texture* image = nullptr;
        int twinkle = -1;
        std::map<int, std::string> png =
        {
            {1, "./spritePNGs/Star1.png"},
            {2, "./spritePNGs/Star2.png"},
            {3, "./spritePNGs/Star3.png"},
            {4, "./spritePNGs/Star4.png"},
            {5, "./spritePNGs/Star5.png"}
        };
};

star::star(double x, double y)
{ //Spawn a star object at a given random location
    xCoord = x;
    yCoord = y;
}

void star::draw(SDL_Renderer*renderer)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> twinkleChanger(1,60);
    if(twinkle == -1) //Randomly assign twinkle value to ensure every star isn't moving together
        {twinkle = twinkleChanger(gen);}
    
    int modifier = 1; //Amount of pixels to move for twinkling
    switch(twinkle) //Move the sprite around in the adjacent pixels to simulate twinkling
    {
        case 0:
            xCoord -= modifier;
            ++twinkle;
            break;
        case 20:
            yCoord += modifier;
            ++twinkle;
            break;
        case 40:
            xCoord += modifier;
            ++twinkle;
            break;
        case 60:
            yCoord -= modifier;
            ++twinkle;
            break;
        case 80:
            twinkle = 0;
            break;
        default:
            ++twinkle;
    }

    SDL_Rect location = {int(xCoord), int(yCoord), 35,35};
    SDL_RenderCopy(renderer, image, nullptr, &location);
}

void star::update(SDL_Renderer* renderer) //Move the star down the screen
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> chooseSprite(1,5); //Choose a random sprite to change to 
    std::uniform_int_distribution<int> xDirection(0,WIDTH); //Choose a new spawn location
    if(yCoord > HEIGHT) //Star is below bottom drawline, so take back to the top
    {
        SDL_DestroyTexture(image); //This prevents a slow memory leak
        image = IMG_LoadTexture(renderer, png.at(chooseSprite(gen)).data());
        xCoord = xDirection(gen);
        yCoord = -5;
    }
    else //Move the star down the screen
        {yCoord += velocity;}
    
    if(image == nullptr) //First time being drawn, so assign an image to the star
        {image = IMG_LoadTexture(renderer, png.at(chooseSprite(gen)).data());}
}

/***************************************************************************************/

class starLinkedList
{ //This object is a bespoke container for the star class
    public:
        starLinkedList();
        void draw(SDL_Renderer*);

    private:
        std::shared_ptr<star> head = nullptr;
        std::shared_ptr<star> tail = nullptr;
        void orderedInsert(std::shared_ptr<star>);
    
    friend std::ostream& operator<<(std::ostream&, const starLinkedList&);
};

starLinkedList::starLinkedList()
{ //Initialize the list to give the star a random spawn point and insertion sort into the list
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> xDirection(0,WIDTH);
    std::uniform_int_distribution<int> yDirection(0,HEIGHT);
    for(int i=0; i<43; ++i) //Insert every star into the linked list
        {this->orderedInsert(std::make_shared<star>(xDirection(gen), yDirection(gen)));}
}

void starLinkedList::orderedInsert(std::shared_ptr<star> starInQuestion)
{ //Basic insertion sort function
    if(head == nullptr) //First object in the list
    {
        head = starInQuestion;
        tail = starInQuestion;
    }
    else if(starInQuestion->yCoord < head->yCoord) //Star spawned before the head star
    {
        starInQuestion->next = head;
        head->last = starInQuestion;
        head = starInQuestion;
    }
    else //Star spawn is after the head star
    {
        auto it = head->next;
        while(it != nullptr)
        {
            if(it == tail) 
            {
                if(it->yCoord <= starInQuestion->yCoord) //Larger than last element in list
                {
                    tail->next = starInQuestion;
                    starInQuestion->last = tail;
                    tail = starInQuestion;
                }
                else //Smaller than the last element in the list
                {
                    starInQuestion->last = tail->last;
                    starInQuestion->next = tail;
                    starInQuestion->last->next = starInQuestion;
                    tail->last = starInQuestion;
                }
                break;
            }
            else if(it->yCoord >= starInQuestion->yCoord) //Item is smaller than next sequential item in the list
            {
                starInQuestion->next = it;
                starInQuestion->last = it->last;
                it->last->next = starInQuestion;
                it->last = starInQuestion;
                break;
            }
            else //Move the iterating pointer to the next star
                {it = it->next;}
        }
    }
}

void starLinkedList::draw(SDL_Renderer* renderer)
{ 
    for(auto it = head; it != nullptr; it = it->next) //Update the star's location and draw to the screen
    {
        it->update(renderer);
        it->draw(renderer);
    }
    while(tail->yCoord < 0) //Move negative stars to the beginning of the list to ensure the list always stays ordered
    {
        tail->next = head;
        head->last = tail;
        head = tail;
        tail = tail->last;
        head->last = nullptr;
        tail->next = nullptr;
    }
}

std::ostream& operator<<(std::ostream& os, const starLinkedList& starList)
{ //Allow the user to print out the desired variable from the list. Set to yCoord by default
    for(auto it = starList.head; it != nullptr; it = it->next)
        {os << it->yCoord << ", ";}
    return os;
}

#endif