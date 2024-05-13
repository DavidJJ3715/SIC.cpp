#ifndef _enemy_
#define _enemy_
#include "func.h"

class enemy
{
    public:
        enemy();
        double top(), bottom(), left(), right();
        void damage(std::string);

    private:
        double yCoord, xCoord, health=100;

};

enemy::enemy() {}

double enemy::top()     {return yCoord;}
double enemy::bottom()  {return yCoord+33;}
double enemy::left()    {return xCoord;}
double enemy::right()   {return xCoord+33;}

void enemy::damage(std::string)
{

}

#endif