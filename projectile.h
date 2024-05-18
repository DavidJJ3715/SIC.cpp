#ifndef _projectile_
#define _projectile_
#include "func.h"
#include "enemy.h"

class projectile
{
    public:
        projectile(std::string, double, double);
        double left(), right(), top(), bottom();
        bool isItAlive();
        void update(std::vector<std::shared_ptr<enemy>>&), draw(SDL_Renderer*);
        std::string element, png;

    private:
        std::map<std::string,std::string> pics =
        {
            {"fire", "./spritePNGs/fireball.png"},
            {"water", "./spritePNGs/raindrop.png"},
            {"life", "./spritePNGs/Full Heart.png"}
        };
        double xCoord, yCoord, velocity=10;
        bool isAlive = true;
};

projectile::projectile(std::string input, double x, double y)
{
    element = input; 
    png = pics.at(element);
    xCoord = x+12;
    yCoord = y-25;
}

double projectile::left()       {return xCoord;}
double projectile::right()      {return xCoord+5;}
double projectile::bottom()     {return yCoord;}
double projectile::top()        {return yCoord-10;}
bool projectile::isItAlive()    {return isAlive;}

void projectile::update(std::vector<std::shared_ptr<enemy>>& enemyList)
{
    if(yCoord<0) 
        {isAlive = false;}
    for(auto en = enemyList.begin(); en != enemyList.end(); ++en)
    {
        if(en->get()->bottom() >= yCoord && en->get()->left() <= xCoord+12.5 && en->get()->right() >= xCoord)
        {
            if(en->get()->damage(element))
                {en = enemyList.erase(en); --en;}
            isAlive = false;
        }
    }
    if(isAlive == true)
        {yCoord -= velocity;}
}

void projectile::draw(SDL_Renderer* renderer)
{
    SDL_Rect location;
    SDL_Texture* image = IMG_LoadTexture(renderer, png.data());
    
    location = {int(xCoord), int(yCoord), 25, 50};

    SDL_RenderCopy(renderer, image, nullptr, &location);
    SDL_DestroyTexture(image);
}

#endif