#include "func.h"
#include "user.h"
#include "projectile.h"
#include "enemy.h"

int main()
{
    const int chosenFPS = 120, frameDelay = 1000/chosenFPS;
    Uint64 frameStart;
    int frameTime, fps, score = 0, maxSpawns = 14, highScore = 0, enemiesKilled = 0, timePaused;
    Uint64 timeSinceLastShot = 0, timeSinceLastSpawn = 0;
    std::string element = "life";
    bool running = true, beginning = true;
    std::optional<SDL_KeyCode> postUpdate;
    std::optional<std::string> elementSelection;
    std::vector<std::shared_ptr<projectile>> projList;
    std::vector<std::shared_ptr<enemy>> enemyList;
    intTup color = {0,0,0};

    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);

    SDL_Window* window = SDL_CreateWindow("<Insert Name Here>", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("DejaVuSans.ttf", 75);

    std::shared_ptr<user> player(new user(element));
    std::chrono::_V2::system_clock::time_point startTime, beforePause;
    
    while(!player.get()->isDead())
    {
        frameStart = SDL_GetTicks64();
        if(beginning)
        {
            enemyList.clear();
            projList.clear();
            running = startScreen(renderer, font, frameDelay);
            if(!running)
                {player.get()->killUser();}
            if(!player.get()->isDead())
            {
                elementSelection = elementScreen(renderer, font, frameDelay);
                if(elementSelection == "quit")
                    {player.get()->killUser();}
                else if(elementSelection == "escape")
                    {continue;}
                if(elementSelection)
                {
                    element = elementSelection.value();
                    player.get()->changeElement(element);
                }
            } 
            startTime = std::chrono::high_resolution_clock::now();
            timePaused = 0;
            beginning = false;           
        }

        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    {player.get()->killUser(); break;}
                default:
                    postUpdate = player.get()->update(event);
                    if(!postUpdate.has_value()) 
                        {break;}
                    else if(postUpdate.value() == SDLK_ESCAPE)
                    {
                        beforePause = std::chrono::high_resolution_clock::now();
                        if(!pauseMenu(renderer, font))
                            {beginning = true; break;}
                        timePaused += (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - beforePause)/200).count();
                    }
                    else if(postUpdate.value() == SDLK_SPACE)
                    {
                        color = getColor();
                        player.get()->setColor(getCompColor(color));
                    }
            }
        }
        SDL_SetRenderDrawColor(renderer, std::get<0>(color), std::get<1>(color), std::get<2>(color), 0);
        SDL_RenderClear(renderer);
        
        player.get()->draw(renderer);
        if(timeSinceLastShot+75 <= SDL_GetTicks64())
        {
            double userX = player.get()->left();
            double userY = player.get()->top();
            std::shared_ptr<projectile> temp(new projectile(element, userX, userY));
            projList.emplace_back(temp);
            timeSinceLastShot = SDL_GetTicks64();
        }
        if((timeSinceLastSpawn+250 <= SDL_GetTicks64()) && int(enemyList.size()+1) <= maxSpawns)
        {
            std::shared_ptr<enemy> temp(new enemy(enemyList));
            enemyList.emplace_back(temp);
            timeSinceLastSpawn = SDL_GetTicks64();
        }
        updateDrawProjectile(renderer, projList, enemyList);
        updateDrawEnemy(renderer, enemyList);
        if(toggleFPS) //Only draw the FPS if the user leaves the fps setting on
            {drawFPS(renderer, fps, color, font);} 
        drawLives(renderer, player.get()->getHealth()); //Draw the amount of hearts remaining
        drawScore(renderer, score, highScore, color, font);
        drawKilled(renderer, enemiesKilled, color, font);
        
        SDL_RenderPresent(renderer);
        frameTime = SDL_GetTicks64() - frameStart;
        if(frameDelay > frameTime)
            {SDL_Delay(frameDelay - frameTime);}
        fps = (1000/(SDL_GetTicks64() - frameStart));
        score = (std::chrono::duration_cast<std::chrono::milliseconds>(((std::chrono::high_resolution_clock::now()) - startTime)/200)).count() - timePaused;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 0;
}