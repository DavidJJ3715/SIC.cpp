#ifndef _Star_
#define _Star_
#include "func.h"

class star
{
    public:
        star(double, double);
        void draw(SDL_Renderer*), update();
        std::shared_ptr<star> last = nullptr;
        std::shared_ptr<star> next = nullptr;
        double xCoord, yCoord, velocity = 0.25;
};

star::star(double x, double y)
{
    xCoord = x;
    yCoord = y;
}

void star::draw(SDL_Renderer* renderer)
{
    SDL_Rect location = {int(xCoord), int(yCoord), 5, 5};
    SDL_SetRenderDrawColor(renderer,255,255,255,0);
    SDL_RenderFillRect(renderer, &location);
}

void star::update()
{
    if(yCoord > HEIGHT)
        {yCoord = -5;}
    else
        {yCoord += velocity;}
}

class starLinkedList
{
    public:
        starLinkedList();
        void drawStarUpdateList(SDL_Renderer*);
    private:
        std::shared_ptr<star> head;
        std::shared_ptr<star> tail;
};

starLinkedList::starLinkedList()
{
    //randomly assign 43 stars across the screen going from top left to bottom right
}

void starLinkedList::drawStarUpdateList(SDL_Renderer* renderer)
{
    for(auto it = head; it != nullptr; it = head->next)
    {
        it->update();  
        it->draw(renderer);
    }
    for(auto it = tail; it->yCoord < 0; it = tail->last)
    {
        //start at tail, if the yCoord is below zero, put star at head
        //      continue looping until there are no more reset stars
    }
}

#endif