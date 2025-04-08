// ==================================================================
// Filename: Game.cpp
// Created:  08.04.2025 by DimaSkup
// ==================================================================
#include "Game.h"
#include "Constants.h"
#include "Log.h"

Game::Game() : isRunning_(false)
{
}

Game::~Game()
{
}

///////////////////////////////////////////////////////////

float projectilePosX = 0.0f;
float projectilePosY = 0.0f;
constexpr float projectileVelocityX = 0.1f;
constexpr float projectileVelocityY = 0.1f;

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
    projectilePosX += projectileVelocityX;
    projectilePosY += projectileVelocityY;
}

///////////////////////////////////////////////////////////

void Game::Render()
{
    SDL_SetRenderDrawColor(pRenderer_, 21, 21, 21, 255);
    SDL_RenderClear(pRenderer_);

    SDL_Rect projectile = {(int)projectilePosX, (int)projectilePosY, 10, 10 };

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



