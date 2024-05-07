#ifndef _classes_
#define _classes_
#include "func.h"

class user
{
    public:
        user();
        double left(), right(), top(), bottom();
        bool moveLeftRight(), moveUpDown(), isDead();
        void setX(double), setY(double), setLeftRight(), setUpDown(), killUser();
        void setColor(intTup), draw(SDL_Renderer*);
        std::optional<SDL_KeyCode> update(const SDL_Event&);
    
    private:
        double xCoord = (WIDTH-25)/2, yCoord = HEIGHT-75, velocity = 15, health = 3;
        bool userMoveLeftRight = true, userMoveUpDown = false, userIsDead = false, mirror = false;
        intTup color = {255,255,255};
};

user::user() {}

double user::left()         {return xCoord;}
double user::top()          {return yCoord;}
double user::right()        {return xCoord+50;}
double user::bottom()       {return yCoord+50;}

bool user::moveLeftRight()  {return userMoveLeftRight;}
bool user::moveUpDown()     {return userMoveUpDown;}
bool user::isDead()         {return userIsDead;}

void user::setX(double x)   {xCoord = x;}
void user::setY(double y)   {yCoord = y;}
void user::setLeftRight()   {userMoveLeftRight = !userMoveLeftRight;}
void user::setUpDown()      {userMoveUpDown = !userMoveUpDown;}
void user::killUser()       {userIsDead = !userIsDead;}

void user::setColor(intTup compColor) {color = compColor;}

void user::draw(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, std::get<0>(color), std::get<1>(color), std::get<2>(color), 0);
    SDL_Rect rect = {int(xCoord),int(yCoord),50,50};
    SDL_RenderFillRect(renderer, &rect);
}

std::optional<SDL_KeyCode> user::update(const SDL_Event &event)
{
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
    return std::nullopt;
}

#endif