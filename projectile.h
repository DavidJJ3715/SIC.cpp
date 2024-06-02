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
        int update(std::vector<std::shared_ptr<enemy>>&);
        void draw(SDL_Renderer*);
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

int projectile::update(std::vector<std::shared_ptr<enemy>>& enemyList)
{ //Move the projectile along the screen until it is off the screen, or colliding with an enemy
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> explosionChance(1,100);
    std::uniform_int_distribution<int> clearChance(1,125);

    int enemiesKilled = 0;
    double explosionLocation = -1;
    bool clearList = false;

    if(yCoord<0) //At the top of the screen. Delete
        {isAlive = false;}
    for(auto en = enemyList.begin(); en != enemyList.end(); ++en) //Check every projectile against every enemy to make sure there are no collisions
    {
        if(en->get()->bottom() >= yCoord && en->get()->left() <= xCoord+12.5 && en->get()->right() >= xCoord) //Collision detected
        {
            if(element == "fire" && explosionChance(gen) <= 7) //Projectile is fire, test if there is an explosion
                {explosionLocation = (en->get()->left() + en->get()->right())/2;}
            else if(element == "water" && clearChance(gen) == 42)
                {clearList = true; break;}
            if(en->get()->damage(element)) //If the enemy is killed, delete from list and increment the enemiesKilled counter
            {
                en = enemyList.erase(en);
                --en;
                enemiesKilled+=1;
            }
            isAlive = false; //Kill the projectile since it hit an enemy
        }
    }

    if(explosionLocation > 0) //If there is an explosion, check a 100x100 square around the center of the projectile's top
    {
        for(auto it = enemyList.begin(); it != enemyList.end(); ++it)
        {
            if((it->get()->right() > explosionLocation-100) && (it->get()->left() < explosionLocation+100) && (it->get()->top() < explosionLocation+100) && (it->get()->bottom() > explosionLocation-100))
            {
                it = enemyList.erase(it);
                --it;
                enemiesKilled+=1;
            }
        }
    }
    else if(clearList)
        {enemiesKilled+=enemyList.size(); enemyList.clear();}

    if(isAlive == true) //Projectile is still alive, move across the screen
        {yCoord -= velocity;}
    return enemiesKilled; //Return the amount of enemies killed during this function call
}

void projectile::draw(SDL_Renderer* renderer)
{ //Draw the projectile to the screen
    SDL_Rect location;
    SDL_Texture* image = IMG_LoadTexture(renderer, png.data());
    
    location = {int(xCoord), int(yCoord), 25, 50};

    SDL_RenderCopy(renderer, image, nullptr, &location);
    SDL_DestroyTexture(image); //This prevents a very fast memory leak
}

#endif