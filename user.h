#ifndef _classes_
#define _classes_
#include "func.h"
#include "projectile.h"

class user
{ //The player object that the user is controlling
    public:
        user(std::string);
        double left(), right(), top(), bottom(), getHealth();
        bool moveLeftRight(), moveUpDown(), isDead();
        void setX(double), setY(double), setLeftRight(), setUpDown(), killUser(), damage(), restart(), revive();
        void draw(SDL_Renderer*), changeElement(std::string);
        std::optional<SDL_KeyCode> update(const SDL_Event&);
        std::string element, png;
    
    private:
        double xCoord = (WIDTH-25)/2, yCoord = HEIGHT-75, velocity = 15, health = 3;
        bool userMoveLeftRight = true, userMoveUpDown = false, userIsDead = false, mirror = false;
        std::map<std::string,std::string> pics =
        {
            {"fire", "./spritePNGs/flame.png"},
            {"water", "./spritePNGs/bubble.png"},
            {"life", "./spritePNGs/Full Heart.png"}
        };
};

user::user(std::string elem) {changeElement(elem);}

double user::left()         {return xCoord;}
double user::top()          {return yCoord;}
double user::right()        {return xCoord+50;}
double user::bottom()       {return yCoord+50;}
double user::getHealth()    {return health;}

bool user::moveLeftRight()  {return userMoveLeftRight;}
bool user::moveUpDown()     {return userMoveUpDown;}
bool user::isDead()         {return userIsDead;}

void user::setX(double x)   {xCoord = x;}
void user::setY(double y)   {yCoord = y;}
void user::setLeftRight()   {userMoveLeftRight = !userMoveLeftRight;}
void user::setUpDown()      {userMoveUpDown = !userMoveUpDown;}
void user::killUser()       {userIsDead = true;}
void user::revive()         {userIsDead = false;}

void user::restart()
{ //Reset the user to beginning of game stats
    health = 3;
    xCoord = (WIDTH-25)/2;
    yCoord = HEIGHT-75;
}

void user::changeElement(std::string elem)
{ //Change the element of the user when the game is restarted
    element = elem;
    png = pics.at(elem);
    if(element == "fire")
        {velocity = 1.33 * velocity;}
    else if(element == "life")
        {velocity = 0.80 * velocity;}
}

void user::damage()
{ //A damage event is triggered, lose health
    health -= 1;
    if(health <= 0)
        {this->killUser();}
}

void user::draw(SDL_Renderer* renderer)
{ //Draw the user to the screen using a square and the emblem associated with the chosen element
    SDL_Rect imageLocation;

    SDL_SetRenderDrawColor(renderer,255,255,255,0);
    SDL_Rect rect = {int(xCoord),int(yCoord),50,50};
    SDL_RenderFillRect(renderer, &rect);

    SDL_Texture* image = IMG_LoadTexture(renderer, png.data());
    if(element == "life") {imageLocation = {int(left()+4), int(top()+4), 40, 40};}
    else if(element == "water") {imageLocation = {int(left()+5), int(top()+5), 40, 40};}
    else if(element == "fire") {imageLocation = {int(left()+7), int(top()+5), 40, 40};}
    SDL_RenderCopy(renderer, image, nullptr, &imageLocation);

    SDL_DestroyTexture(image);
}

std::optional<SDL_KeyCode> user::update(const SDL_Event &event)
{ //Read the keys pressed and return the keypressed or make a change to the user's stats
    if(event.type == SDL_KEYDOWN)
    {
        switch(event.key.keysym.sym)
        {
            case SDLK_ESCAPE: 
                {return SDLK_ESCAPE;}
            case SDLK_SPACE:
                {return SDLK_SPACE;}
            case SDLK_a:
            {
                if(!mirror && left() >= 25)
                    {setX(xCoord - velocity);}
                break;
            }
            case SDLK_d:
            {
                if(!mirror && right() <= WIDTH-25)
                    {setX(xCoord + velocity);}
                break;
            }
        }
    }
    return std::nullopt; //Not escape or space, so return an optional null
}

#endif