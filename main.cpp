#include "func.h"
#include "user.h"
#include "projectile.h"

int main()
{
    const int chosenFPS = 120, frameDelay = 1000/chosenFPS;
    Uint64 frameStart, beforePause, timePaused;
    int frameTime, fps, score = 0, highScore = 0, enemiesKilled = 0;
    std::string element = "life";
    bool running = true;
    std::optional<SDL_KeyCode> postUpdate;
    std::optional<std::string> elementSelection;
    intTup color = {0,0,0};

    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);

    SDL_Window* window = SDL_CreateWindow("<Insert Name Here>", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("DejaVuSans.ttf", 75);

    std::shared_ptr<user> player(new user(element));

    while(running)
    {
        running = startScreen(renderer, font, frameDelay);
        if(!running)
            {player.get()->killUser();}

        elementSelection = elementScreen(renderer, font, frameDelay);
        if(elementSelection == "quit")
            {player.get()->killUser();}
        if(elementSelection)
        {
            running = false;
            element = elementSelection.value();
        }
    }
    std::chrono::_V2::system_clock::time_point startTime = std::chrono::high_resolution_clock::now();
    while(!player.get()->isDead())
    {
        frameStart = SDL_GetTicks64();
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
                        beforePause = SDL_GetTicks64();
                        if(!pauseMenu(renderer, font))
                            {player.get()->killUser(); break;}
                        timePaused += SDL_GetTicks64() - beforePause;
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
        if(toggleFPS) //Only draw the FPS if the user leaves the fps setting on
            {drawFPS(renderer, fps, color, font);} 
        drawLives(renderer, player.get()->getHealth()); //Draw the amount of hearts remaining
        drawScore(renderer, score, highScore, color, font);
        drawKilled(renderer, enemiesKilled, color, font);
        player.get()->draw(renderer);
        SDL_RenderPresent(renderer);
        frameTime = SDL_GetTicks64() - frameStart;
        if(frameDelay > frameTime)
            {SDL_Delay(frameDelay - frameTime);}
        fps = (1000/(SDL_GetTicks64() - frameStart));
        score = (std::chrono::duration_cast<std::chrono::milliseconds>(((std::chrono::high_resolution_clock::now()) - startTime)/100)).count();
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 0;
}