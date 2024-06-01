#ifndef _Star_
#define _Star_
#include "func.h"

class star
{
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
{
    xCoord = x;
    yCoord = y;
}

void star::draw(SDL_Renderer*renderer)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> whatAmIDoingWithMyLife(1,60);
    if(twinkle == -1)
        {twinkle = whatAmIDoingWithMyLife(gen);}
    
    int modifier = 1;
    switch(twinkle)
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

void star::update(SDL_Renderer* renderer)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> chooseSprite(1,5);
    if(yCoord > HEIGHT)
    {
        image = IMG_LoadTexture(renderer, png.at(chooseSprite(gen)).data());
        yCoord = -5;
    }
    else
        {yCoord += velocity;}
    
    if(image == nullptr)
        {image = IMG_LoadTexture(renderer, png.at(chooseSprite(gen)).data());}
}

/***************************************************************************************/

class starLinkedList
{
    public:
        starLinkedList();
        void draw(SDL_Renderer*);
        std::shared_ptr<star> operator[](int) const;

    private:
        std::shared_ptr<star> head = nullptr;
        std::shared_ptr<star> tail = nullptr;
        void orderedInsert(std::shared_ptr<star>);
};

starLinkedList::starLinkedList()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> xDirection(0,800);
    std::uniform_int_distribution<int> yDirection(0,600);
    for(int i=0; i<43; ++i) //Insert every star into the linked list
        {this->orderedInsert(std::make_shared<star>(xDirection(gen), yDirection(gen)));}
}

void starLinkedList::orderedInsert(std::shared_ptr<star> starInQuestion)
{
    if(head == nullptr)
    {
        head = starInQuestion;
        tail = starInQuestion;
    }
    else if(starInQuestion->yCoord < head->yCoord)
    {
        starInQuestion->next = head;
        head->last = starInQuestion;
        head = starInQuestion;
    }
    else
    {
        auto it = head->next;
        while(it != nullptr)
        {
            if(it == tail)
            {
                if(it->yCoord <= starInQuestion->yCoord)
                {
                    tail->next = starInQuestion;
                    starInQuestion->last = tail;
                    tail = starInQuestion;
                }
                else
                {
                    starInQuestion->last = tail->last;
                    starInQuestion->next = tail;
                    starInQuestion->last->next = starInQuestion;
                    tail->last = starInQuestion;
                }
                break;
            }
            else if(it->yCoord >= starInQuestion->yCoord)
            {
                starInQuestion->next = it;
                starInQuestion->last = it->last;
                it->last->next = starInQuestion;
                it->last = starInQuestion;
                break;
            }
            else
                {it = it->next;}
        }
    }
}

void starLinkedList::draw(SDL_Renderer* renderer)
{
    for(auto it = head; it != nullptr; it = it->next)
    {
        it->update(renderer);
        it->draw(renderer);
    }
    while(tail->yCoord < 0)
    {
        tail->next = head;
        head->last = tail;
        head = tail;
        tail = tail->last;
        head->last = nullptr;
        tail->next = nullptr;
    }
}

std::shared_ptr<star> starLinkedList::operator[](int index) const
{
    std::shared_ptr<star> it = head;
    for(int i=0; i<index; ++i)
        {it = it->next;}
    return it;
}

#endif