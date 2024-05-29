#ifndef _classes_
#define _classes_
#include "func.h"
#include "projectile.h"

class user
{
    public:
        user(std::string);
        double left(), right(), top(), bottom(), getHealth();
        bool moveLeftRight(), moveUpDown(), isDead();
        void setX(double), setY(double), setLeftRight(), setUpDown(), killUser(), damage(), restart(), revive();
        void setColor(intTup), draw(SDL_Renderer*), changeElement(std::string);
        std::optional<SDL_KeyCode> update(const SDL_Event&);
        std::string element, png;
    
    private:
        double xCoord = (WIDTH-25)/2, yCoord = HEIGHT-75, velocity = 15, health = 3;
        bool userMoveLeftRight = true, userMoveUpDown = false, userIsDead = false, mirror = false;
        intTup color = {255,255,255};
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

void user::setColor(intTup compColor) 
    {color = compColor;}

void user::restart()
{
    health = 3;
    xCoord = (WIDTH-25)/2;
    yCoord = HEIGHT-75;
}

void user::changeElement(std::string elem)
{
    element = elem;
    png = pics.at(elem);
    if(element == "fire")
        {velocity = 1.33 * velocity;}
    else if(element == "life")
        {velocity = 0.80 * velocity;}
}

void user::damage()
{
    health -= 1;
    if(health <= 0)
        {this->killUser();}
}

void user::draw(SDL_Renderer* renderer)
{
    SDL_Rect imageLocation;

    SDL_SetRenderDrawColor(renderer, std::get<0>(color), std::get<1>(color), std::get<2>(color), 0);
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