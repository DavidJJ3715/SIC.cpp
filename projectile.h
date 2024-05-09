#ifndef _projectile_
#define _projectile_
#include "func.h"

class projectile
{
    public:
        projectile(std::string);
        double left(), right(), top(), bottom();
        bool isDead(), update();
        std::string element, png;

    private:
        std::map<std::string,std::string> pics =
        {
            {"fire", "./spritePNGs/fireball.png"},
            {"water", "./spritePNGs/raindrop.png"},
            {"life", "./spritePNGs/Full Heart.png"}
        };
        double xCoord, yCoord, health, velocity;
};

projectile::projectile(std::string input)
    {element = input; png = pics.at(element);}

double projectile::left() {return xCoord;}

#endif