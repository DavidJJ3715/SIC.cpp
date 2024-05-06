#include "func.h"

int main()
{
    const int chosenFPS = 120, frameDelay = 1000/chosenFPS;
    Uint64 frameStart;
    int frameTime, fps;

    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);

    SDL_Window* window = SDL_CreateWindow("<Insert Name Here>", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* fpsFont = TTF_OpenFont("DejeVuSans.ttf", 40);
    TTF_Font* startFont = TTF_OpenFont("DejaVuSans.ttf", 75);

    bool running = startScreen(renderer, startFont, fpsFont, frameDelay);
    while(running)
    {
        frameStart = SDL_GetTicks64();
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    {running = false; break;}
                case SDL_KEYDOWN:
                {
                    if(event.key.keysym.sym == SDLK_ESCAPE)
                        {running = false; break;}
                    else if(event.key.keysym.sym == SDLK_SPACE)
                    {
                        std::tuple color = getColor();
                        std::tuple compColor = getCompColor(color);
                        SDL_SetRenderDrawColor(renderer, std::get<0>(color), std::get<1>(color), std::get<2>(color), 0);
                        SDL_RenderClear(renderer);
                    }
                }
            }
        }
        frameTime = SDL_GetTicks64() - frameStart;
        if(frameDelay > frameTime)
            {SDL_Delay(frameDelay - frameTime);}
        fps = (1000/SDL_GetTicks64() - frameStart);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(startFont);
    TTF_CloseFont(fpsFont);
    TTF_Quit();
    SDL_Quit();
    return 0;
}