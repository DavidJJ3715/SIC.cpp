#ifndef _func_
#define _func_
#define SDL_MAIN_HANDLED
#include <iostream>
#include <string>
#include <random>
#include <tuple>
#include "../SDL2/include/SDL2/SDL.h"
#include "../SDL2/include/SDL2/SDL_ttf.h"
#include "../SDL2/include/SDL2/SDL_image.h"
#include "classes.h"

const int WIDTH = 800;
const int HEIGHT = 600;

std::tuple<int,int,int> getColor()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 255);
    return std::make_tuple(dist(gen), dist(gen), dist(gen));
}

std::tuple<int,int,int> getCompColor(std::tuple<int,int,int> color)
{
    int R = std::get<0>(color);
    int G = std::get<1>(color);
    int B = std::get<2>(color);
    return std::make_tuple(255-R, 255-G, 255-B);
}

void drawStartText(SDL_Renderer* renderer, bool& increaseAlpha, double& alpha, double& fadeSpeed, TTF_Font* startFont)
{
    std::tuple<int,int,int> color = getColor();
    SDL_Color textColor = {std::get<0>(color), std::get<1>(color), std::get<2>(color)};
    SDL_Surface* surface = TTF_RenderText_Solid(startFont, "Press Space To Start", textColor);
    SDL_Texture* text = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect textRect = {(WIDTH-300)/2, (HEIGHT-75)/2, 300, 75};

    SDL_SetRenderDrawColor(renderer,0,0,0,0);
    SDL_RenderFillRect(renderer, &textRect);
    SDL_SetTextureAlphaMod(text, alpha);
    SDL_RenderCopy(renderer, text, nullptr, &textRect);
    
    if(increaseAlpha == true)
    {
        alpha += fadeSpeed;
        if(alpha >= 255)
            {alpha = 255; increaseAlpha = false;}
    }
    else
    {
        alpha -= fadeSpeed;
        if(alpha <= 0)
            {alpha = -400; increaseAlpha = true;}
    }
    SDL_DestroyTexture(text);    
}

bool startScreen(SDL_Renderer* renderer, TTF_Font* startFont, TTF_Font* fpsFont, const int frameDelay)
{
    Uint64 frameStart = SDL_GetTicks64();
    bool stayAtStartScreen = true, increaseAlpha = true, running = false;
    int frameTime;
    double fadeSpeed = 0.1, alpha = -15;

    while(stayAtStartScreen)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT: //Quit the game
                    {stayAtStartScreen = false; break;}
                case SDL_KEYDOWN:
                {
                    if(event.key.keysym.sym == SDLK_SPACE) //Start the game
                        {stayAtStartScreen = false, running = true;}
                    else if(event.key.keysym.sym == SDLK_s) //Enter the settings menu
                        {continue;}
                }
            }
        }
        drawStartText(renderer, increaseAlpha, alpha, fadeSpeed, startFont); 
        frameTime = SDL_GetTicks64() - frameStart;
        if(frameDelay > frameTime)
            {SDL_Delay(frameDelay - frameTime);}
        SDL_RenderPresent(renderer);
    }
    return running;
}

#endif