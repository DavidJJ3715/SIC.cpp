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
    
    private:
        double xCoord, yCoord, velocity, health;
        bool userMoveLeftRight = true, userMoveUpDown = false, userIsDead = false;
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

#endif