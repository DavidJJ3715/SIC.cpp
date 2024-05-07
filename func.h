#ifndef _func_
#define _func_
#define SDL_MAIN_HANDLED
#include <iostream>
#include <string>
#include <random>
#include <tuple>
#include <memory>
#include <optional>
#include "../SDL2/include/SDL2/SDL.h"
#include "../SDL2/include/SDL2/SDL_ttf.h"
#include "../SDL2/include/SDL2/SDL_image.h"

using intTup = std::tuple<int,int,int>;

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
    std::tuple<Uint8,Uint8,Uint8> color = getColor();
    SDL_Color textColor = {std::get<0>(color), std::get<1>(color), std::get<2>(color),0};
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
            {alpha = -150; increaseAlpha = true;}
    }
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(text);
}

void drawGears(SDL_Renderer* renderer, TTF_Font* startFont, SDL_Texture* gear)
{
        SDL_Rect gearLarge = {0,0,105,105};
        SDL_Rect gearSmall = {105/2,105/2,65,65};
        SDL_Rect total = {0,0,200,200};
        SDL_Rect textRect = {12,100,100,50};
        SDL_Color textColor = {255,255,255,0};
        SDL_Surface* surface = TTF_RenderText_Solid(startFont, "(S)ettings", textColor);
        SDL_Texture* text = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_SetRenderDrawColor(renderer,0,0,0,0);
        SDL_RenderFillRect(renderer, &total);
        SDL_RenderCopy(renderer, gear, nullptr, &gearLarge);
        SDL_RenderCopy(renderer, gear, nullptr, &gearSmall);
        SDL_RenderCopy(renderer, text, nullptr, &textRect);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(text);
}

bool startScreen(SDL_Renderer* renderer, TTF_Font* startFont, const int frameDelay)
{
    Uint64 frameStart;
    bool stayAtStartScreen = true, increaseAlpha = true, running = false;
    int frameTime;
    double fadeSpeed = 1, alpha = 1;
    SDL_Texture* gear = IMG_LoadTexture(renderer, "./spritePNGs/Settings Icon.png");

    while(stayAtStartScreen)
    {
        frameStart = SDL_GetTicks64();
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
                        {stayAtStartScreen = false, running = true; break;}
                    else if(event.key.keysym.sym == SDLK_s) //Enter the settings menu
                        {continue;}
                }
            }
        }
        drawGears(renderer, startFont, gear);
        drawStartText(renderer, increaseAlpha, alpha, fadeSpeed, startFont); 
        frameTime = SDL_GetTicks64() - frameStart;
        if(frameDelay > frameTime)
            {SDL_Delay(frameDelay - frameTime);}
        SDL_RenderPresent(renderer);
    }
    SDL_SetRenderDrawColor(renderer,0,0,0,0);
    SDL_RenderClear(renderer);
    SDL_DestroyTexture(gear);
    return running;
}

void drawPause(SDL_Renderer* renderer, TTF_Font* font, bool selection)
{
    SDL_SetRenderDrawColor(renderer,255,255,255,0); //Draw white background
    SDL_RenderClear(renderer);

    int menuWidth = 500, menuHeight = 500; //Bounds of the black box
    int menuX = (WIDTH - menuWidth)/2, menuY = (HEIGHT - menuHeight)/2; //Starting points of the black box
    SDL_Rect rect = {menuX, menuY, menuWidth, menuHeight}, border; //Black rectangle rect

    SDL_SetRenderDrawColor(renderer,0,0,0,0); //Choose black color
    SDL_RenderFillRect(renderer, &rect); //Fill in black rectangle

    SDL_Surface* surface = TTF_RenderText_Solid(font, "GAME PAUSED", {255,255,255,0}); //Game pause text
    SDL_Texture* pauseText = SDL_CreateTextureFromSurface(renderer, surface); //Make it a texture
    SDL_FreeSurface(surface); //Free the surface to prevent memory leaks
    surface = TTF_RenderText_Solid(font, "RESUME", {255,255,255,0}); //Resume text
    SDL_Texture* resumeText = SDL_CreateTextureFromSurface(renderer, surface); //Make it a texture
    SDL_FreeSurface(surface); //Free the surface to prevent memory leaks
    surface = TTF_RenderText_Solid(font, "QUIT", {255,255,255,0}); //Quit text
    SDL_Texture* quitText = SDL_CreateTextureFromSurface(renderer, surface); //Make it a texture
    SDL_FreeSurface(surface); //Free the surface to prevent memory leaks

    SDL_Rect pause = {(WIDTH/2)-(menuWidth/5)-50, (HEIGHT/2)-(menuHeight/2)+25, 310, 140}; //Location of "GAME PAUSED"
    SDL_Rect resume = {(WIDTH/2)-(menuWidth/5)+5, (HEIGHT)-(menuHeight)+150, 200, 100}; //Location of "RESUME"
    SDL_Rect quit = {(WIDTH/2)-(menuWidth/5)+5, HEIGHT-menuHeight+300, 200, 100}; //Location of "QUIT"
    SDL_RenderCopy(renderer, pauseText, nullptr, &pause); //Render "GAME PAUSED" to the screen
    SDL_RenderCopy(renderer, resumeText, nullptr, &resume); //Render "RESUME" to the screen
    SDL_RenderCopy(renderer, quitText, nullptr, &quit); //Render "QUIT" to the screen

    if(selection) //Change where border is located based on which choice is selected
        {border = {(WIDTH/2)-145, (HEIGHT/2)-50, 300, 100};} //Resume
    else
        {border = {(WIDTH/2)-145, (HEIGHT/2)+100, 300, 100};} //Quit

    SDL_SetRenderDrawColor(renderer,212,175,55,0); //Gold color for border
    SDL_RenderDrawRect(renderer, &border); //Draw the golden rectangle
    SDL_RenderPresent(renderer); //Draw everything to the screen
    SDL_DestroyTexture(pauseText); //Destroy every texture to prevent memory leaks
    SDL_DestroyTexture(resumeText);
    SDL_DestroyTexture(quitText);
}

bool pauseMenu(SDL_Renderer* renderer, TTF_Font* font)
{
    bool selection = true; //True == "resume" || False == "quit"
    drawPause(renderer, font, selection); //Separate function used to draw the pause menu to keep this function cleaner
    while(true) //Loop until the user resumes or quits the game
    {
        SDL_Event event;
        while(SDL_PollEvent(&event)) //Typical event handler loop
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    {return selection;}
                case SDL_KEYDOWN:
                {
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            {return true;}
                        case SDLK_RETURN:
                            {return selection;}
                        case SDLK_UP: //User selects "RESUME"
                        {
                            selection = true; 
                            drawPause(renderer, font, selection); //Only call the drawPause() function whenever there is a change to be made to the screen   
                            break;
                        }
                        case SDLK_DOWN: //User selects "QUIT"
                        {
                            selection = false; 
                            drawPause(renderer, font, selection);    
                            break;
                        }
                    }
               }
            }
        }
    }
}

#endif