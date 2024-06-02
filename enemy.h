#ifndef _enemy_
#define _enemy_
#include "func.h"

class enemy
{ //An enemy is an object that the player is meant to kill to progress in the game
    public:
        enemy(std::vector<std::shared_ptr<enemy>>);
        double top(), bottom(), left(), right();
        bool damage(std::string), isItAlive();
        void draw(SDL_Renderer*), update();
        std::string element;

    private:
        double yCoord=-35, xCoord, health=100, velocity=0.33, damageVal=34;
        bool isAlive = true;
        intTup color = {};
        std::map<std::string,intTup>elemColor =
        {
            {"fire", {178,34,34}},
            {"water", {28,107,160}},
            {"life", {34,139,34}}
        };
        std::string elementList[3] = {"fire","water","life"};
        double xList[15] = {50,100,150,200,250,300,350,400,450,500,550,600,650,700,750};
};

enemy::enemy(std::vector<std::shared_ptr<enemy>> enemyList) 
{ //Initialize the enemy and put it into the vector<enemy> enemyList
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 2);
    std::uniform_int_distribution<int> locationDist(0,14);

    element = elementList[dist(gen)]; //Give a random element to the enemy
    color = elemColor.at(element); //Color aligns with element index

    double tempX = xList[locationDist(gen)]; //Determine the spawn location based on random distribution
    for(auto en = enemyList.begin(); en != enemyList.end(); ++en)
    {
        if(tempX == en->get()->left() && en->get()->top() <= 67) //Make sure enemies are not stacked on top of each other
        {
            tempX = xList[locationDist(gen)]; //The chosen location is stacked, so choose a new spawn location
            en = enemyList.begin();
        }
    }
    xCoord = tempX;
}

double enemy::top()             {return yCoord;}
double enemy::bottom()          {return yCoord+33;}
double enemy::left()            {return xCoord;}
double enemy::right()           {return xCoord+33;}
bool enemy::isItAlive()         {return isAlive;}

bool enemy::damage(std::string projElem) //Determine how much damage to take depending on projectile's element
{
    if(projElem == "fire") //Fire base damage is 1.5*damageVal. Double damage to life enemies
    {
        if(element == "life")   {health -= 3.0*damageVal;}
        else                    {health -= 1.5*damageVal;}
    }
    else if(projElem == "water") //Water base damage is 1.0*damageVal. Double damage to fire enemies
    {
        if(element == "fire")   {health -= 2.0*damageVal;}
        else                    {health -= damageVal;}
    }
    else if(projElem == "life") {health -= damageVal;} //No damage boosts

    if(health <= 0)             {return true;} //If the enemy is dead, return true
    else                        {return false;} //Enemy is not dead
}

void enemy::draw(SDL_Renderer* renderer) //Draw the enemy based on enemy's position coordinates
{
    SDL_Rect location = {int(xCoord), int(yCoord), 35, 35};
    SDL_SetRenderDrawColor(renderer, std::get<0>(color), std::get<1>(color), std::get<2>(color), 0);
    SDL_RenderFillRect(renderer, &location);
}

void enemy::update() //Determine where the enemy is and either kill or move the enemy
{
    if(yCoord >= HEIGHT-35)     {isAlive = false;}
    else if(isAlive)            {yCoord += velocity;}
}

#endif