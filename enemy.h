#ifndef _enemy_
#define _enemy_
#include "func.h"

class enemy
{
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
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 2);
    std::uniform_int_distribution<int> locationDist(0,14);

    element = elementList[dist(gen)];
    color = elemColor.at(element);

    double tempX = xList[locationDist(gen)];
    for(auto en = enemyList.begin(); en != enemyList.end(); ++en)
    {
        if(tempX == en->get()->left() && en->get()->top() <= 67)
        {
            tempX = xList[locationDist(gen)]; 
            en = enemyList.begin();
        }
    }
    xCoord = tempX;
}

double enemy::top()     {return yCoord;}
double enemy::bottom()  {return yCoord+33;}
double enemy::left()    {return xCoord;}
double enemy::right()   {return xCoord+33;}
bool enemy::isItAlive() {return isAlive;}

bool enemy::damage(std::string projElem)
{
    bool ignoreDamage = false;

    if(element == "life")
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, 99);
        if(dist(gen) < 42)
            {ignoreDamage = true;}      
    }
    if(!ignoreDamage)
    {
        if(projElem == "fire")
        {
            if(element == "life")
                {health -= 3.0*damageVal;}
            else 
                {health -= 1.5*damageVal;}
        }
        else if(projElem == "water")
        {
            if(element == "fire")
                {health -= 2.0*damageVal;}
            else
                {health -= damageVal;}
        }
        else
            {health -= damageVal;}
    }
    if(health <= 0)
        {return true;}
    return false;
}

void enemy::draw(SDL_Renderer* renderer)
{
    SDL_Rect location = {int(xCoord), int(yCoord), 35, 35};
    SDL_SetRenderDrawColor(renderer, std::get<0>(color), std::get<1>(color), std::get<2>(color), 0);
    SDL_RenderFillRect(renderer, &location);
}

void enemy::update()
{
    if(yCoord >= HEIGHT-35)
        {isAlive = false;}
    else if(isAlive)
        {yCoord += velocity;}
}

#endif