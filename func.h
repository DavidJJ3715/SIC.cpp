#ifndef _func_
#define _func_
#define SDL_MAIN_HANDLED
#include <iostream>
#include <string>
#include <random>
#include <tuple>
#include <memory>
#include <optional>
#include <chrono>
#include <map>
#include <vector>
#include <fstream>
#include <thread>
#include <functional>
#include <mutex>
#include "../SDL2/include/SDL2/SDL.h"
#include "../SDL2/include/SDL2/SDL_ttf.h"
#include "../SDL2/include/SDL2/SDL_image.h"

//? Type->Type == 1.0x Damage   || Strong->Weak == 2.0x Damage  || Fire->Life == 3.0x Damage
//? Water->Fire || No weakness  || 100% Movement    || 33% stun boss && 9% wipe ads during boss fight && 1% wipe screen
//? Fire->Life  || 1.5x Damage  || 133% Movement    || 7% Projectiles explode on impact
//? Life->None  || Health Regen || 80% Movement     || 42% Chance not to take damage

//Todo: settings menu, element select screen=>finish and add flavor text, boss transition
//Todo: boss class, boss controls
//Todo: experience and level system
//Todo: difficulty change as game goes on (in another thread?)

//* Settings ideas:
//*     -FPS enabler/disabler
//*     -Monotone colorway
//*     -FPS selecter?
//*     -Reset highscore
//*     -Profile selection? (highscores link to specific user's profile)


/********************************
*       Global Variables        *
*********************************/
using intTup = std::tuple<int,int,int>;
const int WIDTH = 800, HEIGHT = 600, chosenFPS = 120, frameDelay = 1000/chosenFPS;
bool toggleFPS = true, isWhole;
std::mutex highScoreLock;


/********************************
*       Save Functions          *
*********************************/
void saveHighScore(int highScore)
{
    std::ofstream scoreFile("save.txt");
    scoreFile << highScore;
}

int loadHighScore()
{
    int highScore = 0;
    std::ifstream scoreFile("save.txt");
    scoreFile >> highScore;
    return highScore;
}

void checkAgainstHighScore(int score, int& highScore)
{
    if(score > highScore)
    {
        highScore = score;
        saveHighScore(score);
    }
}

void periodicSave(int& highScore, bool& playerAlive)
{
    while(playerAlive)
    {
        {
            std::lock_guard<std::mutex> lock(highScoreLock);
            saveHighScore(highScore);
        }
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}


/********************************
*       Core Functionality      *
*********************************/
bool isWholeNumber(double val, double epsilon = 1e-9) 
    {return std::abs(val - std::round(val)) < epsilon;}

intTup getColor()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 255);
    return std::make_tuple(dist(gen), dist(gen), dist(gen));
}

intTup getCompColor(intTup color)
{
    int R = std::get<0>(color);
    int G = std::get<1>(color);
    int B = std::get<2>(color);
    return std::make_tuple(255-R, 255-G, 255-B);
}

template<typename projectileType, typename enemyType>
int updateDrawProjectile(SDL_Renderer* renderer, std::vector<std::shared_ptr<projectileType>>& projList, std::vector<std::shared_ptr<enemyType>>& enemyList)
{
    int enemiesKilled=0;
    for(auto proj = projList.begin(); proj != projList.end(); ++proj)
    {
        enemiesKilled += proj->get()->update(enemyList);
        if(!proj->get()->isItAlive())
                {proj = projList.erase(proj); --proj;}
        else
            {proj->get()->draw(renderer);}
    }
    return enemiesKilled;
}

template<typename enemyType, typename userType>
bool updateDrawEnemy(SDL_Renderer* renderer, std::vector<std::shared_ptr<enemyType>>& enemyList, std::shared_ptr<userType>& player, std::string element)
{
    bool gameOver = false, ignoreDamage = false;
    for(auto en = enemyList.begin(); en != enemyList.end(); ++en)
    {
        en->get()->update();
        if(!en->get()->isItAlive())
        {
            en = enemyList.erase(en);
            --en;
            if(element == "life")
            {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<int> dist(1, 100);
                if(dist(gen) > 42)
                    {ignoreDamage = true;}      
            } 
            if(!ignoreDamage)
            {
                player.get()->damage();
                if(player.get()->isDead())
                    {gameOver = true;}
            }
        }
        else
            {en->get()->draw(renderer);}
    }
    return gameOver;
}

template<typename entityType, typename... Args>
Uint64 spawnEntity(std::vector<std::shared_ptr<entityType>>& entityList, Args&&... args)
{ //Generic spawn function. Function takes in the list of entities and any additional arguments that are needed for constructing the object
    entityList.emplace_back(std::make_shared<entityType>(std::forward<Args>(args)...));
    return SDL_GetTicks64();
} 


/****************************
*       Draw Functions      *
*****************************/
void drawLives(SDL_Renderer* renderer, double lives)
{
    int fpsWidth = 0;
    SDL_Texture* halfHeart = IMG_LoadTexture(renderer, "./spritePNGs/Half Heart.png");
    SDL_Texture* fullHeart = IMG_LoadTexture(renderer, "./spritePNGs/Full Heart.png");
    SDL_Rect heartRect;

    if(toggleFPS) {fpsWidth = 90;}

    if(isWholeNumber(lives))
        {isWhole = true;}
    else
        {isWhole = false;}

    for(int i=0;i<lives;i++)
    {
        int x = (WIDTH-((i+1)*55))-fpsWidth;
        heartRect = {x, 10, 45, 45};
        SDL_RenderCopy(renderer, fullHeart, nullptr, &heartRect);
        if(!isWhole && i+1 >= lives)
            {SDL_RenderCopy(renderer, halfHeart, nullptr, &heartRect);}
    }

    SDL_DestroyTexture(halfHeart);
    SDL_DestroyTexture(fullHeart);
}

void drawSaveScreen(SDL_Renderer* renderer, TTF_Font* font)
{
    char message[10];
    sprintf(message, "Saving Please Wait");

    SDL_Rect textRect = {(WIDTH/2)-150,(HEIGHT/2)-75,300,150};
    SDL_Surface* surface = TTF_RenderText_Solid(font, message, {255,255,255,0});
    SDL_Texture* writing = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_SetRenderDrawColor(renderer,0,0,0,0);
    SDL_RenderClear(renderer);
    SDL_RenderFillRect(renderer, &textRect);
    SDL_RenderCopy(renderer, writing, nullptr, &textRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(writing);
    SDL_RenderPresent(renderer);
}

template <typename entityType>
void drawObjectBehindTextLeft(SDL_Renderer* renderer, std::vector<std::shared_ptr<entityType>> entityList, int x, int y)
{
    for(auto it = entityList.begin(); it != entityList.end(); ++it)
    {
        if(it->get()->left() < x && it->get()->top() < y)
            {it->get()->draw(renderer);}
    }
    //draw stars here
}

template <typename entityType>
void drawObjectBehindTextRight(SDL_Renderer* renderer, std::vector<std::shared_ptr<entityType>> entityList, int x, int y)
{
    for(auto it = entityList.begin(); it != entityList.end(); ++it)
    {
        if(it->get()->right() > x && it->get()->top() < y)
            {it->get()->draw(renderer);}
    }
    //draw stars here
}

template <typename projType, typename enemyType>
void drawFPS(SDL_Renderer* renderer, int fps, intTup color, TTF_Font* font, std::vector<std::shared_ptr<projType>> projList, std::vector<std::shared_ptr<enemyType>> enemyList)
{
    char fpsString[10];
    sprintf(fpsString, "FPS: %d", fps);

    SDL_Rect textRect = {WIDTH-85, 14, 70, 35};
    SDL_Surface* surface = TTF_RenderText_Solid(font, fpsString, {255,255,255,0});
    SDL_Texture* frames = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_SetRenderDrawColor(renderer,std::get<0>(color),std::get<1>(color),std::get<2>(color),0);
    SDL_RenderFillRect(renderer, &textRect);
    drawObjectBehindTextRight(renderer, projList, WIDTH-100, 50);
    drawObjectBehindTextRight(renderer, enemyList, WIDTH-100, 50);
    SDL_RenderCopy(renderer, frames, nullptr, &textRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(frames);
}

template <typename projType, typename enemyType>
void drawScore(SDL_Renderer* renderer, int score, int highScore, int enemiesKilled, intTup color, TTF_Font* font, std::vector<std::shared_ptr<projType>> projList, std::vector<std::shared_ptr<enemyType>> enemyList)
{
    char highScoreString[1000], scoreString[1000], enemiesString[1000];
    sprintf(highScoreString, "High Score: %d", highScore);
    sprintf(scoreString, "Score: %d", score);
    sprintf(enemiesString, "Enemies: %d", enemiesKilled);

    SDL_Rect highRect = {5,5,220,35};
    SDL_Rect scoreRect = {5,35,220,35};
    SDL_Rect killedRect = {5,65,220,35};
    SDL_Surface* surface = TTF_RenderText_Solid(font, highScoreString, {255,255,255,0});
    SDL_Texture* HIGHSCORE = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    surface = TTF_RenderText_Solid(font, scoreString, {255,255,255,0});
    SDL_Texture* SCORE = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    surface = TTF_RenderText_Solid(font, enemiesString, {255,255,255,0});
    SDL_Texture* KILLED = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    SDL_SetRenderDrawColor(renderer,std::get<0>(color),std::get<1>(color),std::get<2>(color),0);
    SDL_RenderFillRect(renderer, &highRect);
    SDL_RenderFillRect(renderer, &scoreRect);
    SDL_RenderFillRect(renderer, &killedRect);
    drawObjectBehindTextLeft(renderer, projList, 220, 105);
    drawObjectBehindTextLeft(renderer, enemyList, 220, 105);
    SDL_RenderCopy(renderer, HIGHSCORE, nullptr, &highRect);
    SDL_RenderCopy(renderer, SCORE, nullptr, &scoreRect);
    SDL_RenderCopy(renderer, KILLED, nullptr, &killedRect);

    SDL_DestroyTexture(HIGHSCORE);
    SDL_DestroyTexture(SCORE);
    SDL_DestroyTexture(KILLED);
}

void drawStartText(SDL_Renderer* renderer, bool& increaseAlpha, double& alpha, double& fadeSpeed, TTF_Font* font)
{
    std::tuple<Uint8,Uint8,Uint8> color = getColor();
    SDL_Color textColor = {std::get<0>(color), std::get<1>(color), std::get<2>(color),0};
    SDL_Surface* surface = TTF_RenderText_Solid(font, "Press Space To Start", textColor);
   
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

void drawElements(SDL_Renderer* renderer, TTF_Font* font, SDL_Texture* flame, SDL_Texture* bubble, SDL_Texture* heart, int state)
{
    SDL_Rect rect, rect2, rect3; //rect(white box), rect2(image), rect3(gold border)
    SDL_Color textColor = {255,255,255,0}; //White color for box

    SDL_Surface* surface = TTF_RenderText_Solid(font, "Fire", textColor); //Render the labels for the elements
    SDL_Texture* flameText = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    surface = TTF_RenderText_Solid(font, "Water", textColor);
    SDL_Texture* waterText = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    surface = TTF_RenderText_Solid(font, "Life", textColor);
    SDL_Texture* lifeText = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    SDL_SetRenderDrawColor(renderer,0,0,0,0); //Draw the boxes and images in the white box
    SDL_RenderClear(renderer);   
    SDL_SetRenderDrawColor(renderer,255,255,255,0);
    rect = {100,175,100,100}; //Rectangle for white box
    rect2 = {125,200,50,50}; //Rectangle for image
    SDL_RenderFillRect(renderer, &rect);
    SDL_RenderCopy(renderer, flame, nullptr, &rect2);
    rect = {350,175,100,100}; //Rectangle for white box
    rect2 = {375,200,50,50}; //Rectangle for image
    SDL_RenderFillRect(renderer, &rect);
    SDL_RenderCopy(renderer, bubble, nullptr, &rect2);
    rect = {600,175,100,100}; //Rectangle for white box
    rect2 = {625,200,50,50}; //Rectangle for image
    SDL_RenderFillRect(renderer, &rect);
    SDL_RenderCopy(renderer, heart, nullptr, &rect2);

    SDL_SetRenderDrawColor(renderer,212,175,55,0); //Make gold border based on what "state" the menu is in
    switch(state)
    {
        case 0:
            {rect3 = {75,150,150,150}; break;}
        case 1:
            {rect3 = {325,150,150,150}; break;}
        case 2:
            {rect3 = {575,150,150,150}; break;}
    }
    SDL_RenderDrawRect(renderer, &rect3);

    SDL_DestroyTexture(flameText); //Clean up our memory
    SDL_DestroyTexture(waterText);
    SDL_DestroyTexture(lifeText);
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

void drawGameOver(SDL_Renderer* renderer, TTF_Font* font, bool selection)
{
    SDL_SetRenderDrawColor(renderer,0,0,0,0);
    SDL_RenderClear(renderer);

    SDL_RenderPresent(renderer);
}


/********************************
*       Screen Functions        *
*********************************/
std::optional<std::string> elementScreen(SDL_Renderer* renderer, TTF_Font* font, const int frameDelay)
{
    std::optional<std::string> retString = std::nullopt;
    Uint64 frameStart;
    bool stayAtSelectionScreen = true;
    int state = 1, frameTime; //state is either left(0), middle(1), right(2)
    SDL_Texture* flame = IMG_LoadTexture(renderer, "./spritePNGs/flame.png");
    SDL_Texture* bubble = IMG_LoadTexture(renderer, "./spritePNGs/bubble.png");
    SDL_Texture* heart = IMG_LoadTexture(renderer, "./spritePNGs/Full Heart.png");

    while(stayAtSelectionScreen)
    {
        frameStart = SDL_GetTicks64();
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    {retString = "quit"; break;}
                case SDL_KEYDOWN:
                {
                    if(event.key.keysym.sym == SDLK_LEFT)
                        {if(state != 0) {state--;}}
                    else if(event.key.keysym.sym == SDLK_RIGHT)
                        {if(state != 2) {state++;}}
                    else if(event.key.keysym.sym == SDLK_RETURN)
                    {
                        stayAtSelectionScreen = false;
                        if(state == 0) {retString = "fire";}
                        else if(state == 1) {retString = "water";}
                        else if(state == 2) {retString = "life";}
                        break;
                    }
                    else if(event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        stayAtSelectionScreen = false; 
                        retString = "escape";
                        break;
                    }
                }
            }
        }
        drawElements(renderer, font, flame, bubble, heart, state);
        frameTime = SDL_GetTicks64() - frameStart;
        if(frameDelay > frameTime)
            {SDL_Delay(frameDelay - frameTime);}
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(flame);
    SDL_DestroyTexture(bubble);
    SDL_DestroyTexture(heart);
    return retString;
}

bool startScreen(SDL_Renderer* renderer, TTF_Font* startFont, const int frameDelay)
{
    Uint64 frameStart;
    bool stayAtStartScreen = true, increaseAlpha = true, running = false;
    int frameTime;
    double fadeSpeed = 1, alpha = 1;
    SDL_Texture* gear = IMG_LoadTexture(renderer, "./spritePNGs/Settings Icon.png");

    SDL_SetRenderDrawColor(renderer,0,0,0,0);
    SDL_RenderClear(renderer);
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
                    {
                        stayAtStartScreen = false;
                        running = true; 
                        break;
                    }
                    else if(event.key.keysym.sym == SDLK_s) //Enter the settings menu
                        {running = true; /*settings = true*/;}
                    else if(event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        //Add a "quit game" confirmation screen like in Destiny
                        stayAtStartScreen = false; 
                        break;
                    }
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

bool selectionMenu(SDL_Renderer* renderer, TTF_Font* font, std::function<void(SDL_Renderer*,TTF_Font*,bool)>func)
{ //Generic two prompt selection menu that takes in a draw function as a parameter
    bool selection = true; //True == "resume" || False == "quit"
    func(renderer, font, selection);
    while(true)
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
                            func(renderer, font, selection); //Only call the draw function whenever there is a change to be made to the screen   
                            break;
                        }
                        case SDLK_DOWN: //User selects "QUIT"
                        {
                            selection = false; 
                            func(renderer, font, selection);    
                            break;
                        }
                    }
               }
            }
        }
    }
    return selection;
}

void settingsScreen(SDL_Renderer* renderer, TTF_Font* font)
{

}

#endif