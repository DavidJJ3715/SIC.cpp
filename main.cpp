#include "func.h"
#include "user.h"
#include "projectile.h"
#include "enemy.h"

int main()
{
    int frameTime, fps, score = 0, maxSpawns = 14, enemiesKilled = 0, timePaused, highScore = loadHighScore();
    Uint64 frameStart, timeSinceLastShot = 0, timeSinceLastSpawn = 0;
    std::string element = "life";
    bool beginning = true, running = true;
    std::optional<SDL_KeyCode> postUpdate;
    std::optional<std::string> elementSelection;
    std::vector<std::shared_ptr<projectile>> projList;
    std::vector<std::shared_ptr<enemy>> enemyList;
    intTup color = {0,0,0};

    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);

    SDL_Window* window = SDL_CreateWindow("SIC.cpp", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("DejaVuSans.ttf", 75);

    std::shared_ptr<user> player(new user(element)); //Player character being controlled
    std::chrono::_V2::system_clock::time_point startTime, beforePause; //Time point starting when the game is beginning
    std::thread saveThread(periodicSave, std::ref(highScore), std::ref(running)); //Spawn a second process that saves the game on an interval, keeping the main game loop unaffected from any slowdowns when saving the game
    
    while(!player.get()->isDead())
    {
        frameStart = SDL_GetTicks64();
        if(beginning)
        {
            enemyList.clear();
            projList.clear();
            enemiesKilled = 0;
            player.get()->restart();
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
                    {player.get()->killUser(); running = false; break;}
                default:
                    postUpdate = player.get()->update(event);
                    if(!postUpdate.has_value()) 
                        {break;}
                    else if(postUpdate.value() == SDLK_ESCAPE)
                    {
                        beforePause = std::chrono::high_resolution_clock::now();
                        if(!pauseMenu(renderer, font))
                            {beginning = true; break;}
                        timePaused += (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - beforePause)/200).count(); //Calculate how much time was spent paused so it can be subtracted from the score value
                    }
                    else if(postUpdate.value() == SDLK_SPACE)
                    {
                        color = getColor(); //Grab a color tuble
                        player.get()->setColor(getCompColor(color)); //Set the user to a complementary color
                    }
            }
        }
        SDL_SetRenderDrawColor(renderer, std::get<0>(color), std::get<1>(color), std::get<2>(color), 0); //Set the background color
        SDL_RenderClear(renderer); //Clear the screen to draw new stuff over it
        
        player.get()->draw(renderer); //Draw the user to the screen
        Uint64 tempTicks = SDL_GetTicks64(); //Get a temp value to synchronize multiple comparisons
        if(timeSinceLastShot+75 <= tempTicks)
            {timeSinceLastShot = spawnEntity(projList,element,player.get()->left(),player.get()->top());} //Spawn a projectile if the timer condition is met
        if((timeSinceLastSpawn+250 <= tempTicks) && int(enemyList.size()+1) <= maxSpawns)
            {timeSinceLastSpawn = spawnEntity(enemyList,enemyList);} //Spawn an enemy if the timer condition is met
        enemiesKilled += updateDrawProjectile(renderer, projList, enemyList); //Update projectiles, draw to screen, and increment counter if enemy is killed
        updateDrawEnemy(renderer, enemyList, player); //Update the enemies and draw to screen
        if(toggleFPS) //Only draw the FPS if the user leaves the fps setting on
            {drawFPS(renderer, fps, color, font);} //Draw the FPS to the screen
        drawLives(renderer, player.get()->getHealth()); //Draw the amount of hearts remaining
        drawScore(renderer, score, highScore, color, font); //Draw the score to the screen
        drawKilled(renderer, enemiesKilled, color, font); //Draw the amount of enemies killed
        
        SDL_RenderPresent(renderer);
        frameTime = SDL_GetTicks64() - frameStart;
        if(frameDelay > frameTime)
            {SDL_Delay(frameDelay - frameTime);}
        fps = (1000/(SDL_GetTicks64() - frameStart));
        score = (std::chrono::duration_cast<std::chrono::milliseconds>(((std::chrono::high_resolution_clock::now()) - startTime)/200)).count() - timePaused + enemiesKilled;
        { //Create a new temp scope for the scope bound lock_guard to be effective
            std::lock_guard<std::mutex> lock(highScoreLock);
            checkAgainstHighScore(score, highScore);
        }
    }
    drawSaveScreen(renderer, font);
    if(saveThread.joinable())
        {saveThread.join();}
    saveHighScore(highScore);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 0;
}