// ==================================================================
// Filename: Game.cpp
// Created:  08.04.2025 by DimaSkup
// ==================================================================
#include "Game.h"
#include "Constants.h"
#include "Log.h"
#include "../lib/glm/glm.hpp"


Game::Game() : isRunning_(false)
{
}

Game::~Game()
{
}

///////////////////////////////////////////////////////////

glm::vec2 projectilePos      = glm::vec2(0, 0);
glm::vec2 projectileVelocity = glm::vec2(20, 20);

///////////////////////////////////////////////////////////

void Game::Initialize(const int wndWidth, const int wndHeight)
{
    LogPrint(LOG_INFO, "Start of the game initialization");

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        LogError(LOG_INFO, "Error initializing SDL");
        return;        
    }

    pWindow_ = SDL_CreateWindow(
        "dude_engine",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        wndWidth,
        wndHeight,
        SDL_WINDOW_BORDERLESS);

    if (pWindow_ == nullptr)
    {
        LogError(LOG_INFO, "Error initializing window");
        return;
    }
    
    pRenderer_ = SDL_CreateRenderer(pWindow_, -1, 0);
    if (pRenderer_ == nullptr)
    {
        LogError(LOG_INFO, "Error initializing renderer");
        return;
    }

    isRunning_ = true;

    LogPrint(LOG_INFO, "The game is initialized!");
}

///////////////////////////////////////////////////////////

void Game::ProcessInput()
{
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type)
    {
        case SDL_QUIT:
        {
            isRunning_ = false;
            break;
        }
        case SDL_KEYDOWN:
        {
            if (event.key.keysym.sym == SDLK_ESCAPE)
                isRunning_ = false;
            break;
        }
        default:
        {
            break;
        }
    }
}

///////////////////////////////////////////////////////////

void Game::Update()
{
    // speep the execution until we reach the target frame time in ms
    const int timeToWait = FRAME_TARGET_TIME - ((SDL_GetTicks() - ticksLastFrame_));

    // only call delay if we are too fast to process this frame
    if (timeToWait > 0 && timeToWait <= FRAME_TARGET_TIME)
        SDL_Delay(timeToWait);

    // difference in ticks from last frame converted to seconds
    float deltaTime = (SDL_GetTicks() - ticksLastFrame_) * 0.001f; 

    // set the new ticks count for the current frame to be used in the next pass
    ticksLastFrame_ = SDL_GetTicks();
 
    // clamp deltaTime to a maximum value
    deltaTime = (deltaTime > FRAME_TARGET_TIME) ? FRAME_TARGET_TIME : deltaTime;


    projectilePos = glm::vec2(
        projectilePos.x + projectileVelocity.x * deltaTime,
        projectilePos.y + projectileVelocity.y * deltaTime);
}

///////////////////////////////////////////////////////////

void Game::Render()
{
    SDL_SetRenderDrawColor(pRenderer_, 21, 21, 21, 255);
    SDL_RenderClear(pRenderer_);

    SDL_Rect projectile = {(int)projectilePos.x, (int)projectilePos.y, 10, 10 };

    SDL_SetRenderDrawColor(pRenderer_, 255, 255, 255, 255);
    SDL_RenderFillRect(pRenderer_, &projectile);

    SDL_RenderPresent(pRenderer_);
}

///////////////////////////////////////////////////////////

void Game::Destroy()
{
    SDL_DestroyRenderer(pRenderer_);
    SDL_DestroyWindow(pWindow_);
    SDL_Quit();
}



