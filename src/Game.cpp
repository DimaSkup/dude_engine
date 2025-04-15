// ==================================================================
// Filename: Game.cpp
// Created:  08.04.2025 by DimaSkup
// ==================================================================
#include "Game.h"
#include "../lib/glm/glm.hpp"
#include "Constants.h"
#include "Log.h"
#include "EntityMgr.h"
#include "Components/TransformComponent.h"


///////////////////////////////////////////////////////////

Game::Game() : isRunning_(false)
{
}

Game::~Game()
{
}

///////////////////////////////////////////////////////////

void Game::Initialize()
{
    LoadLevel(0);

    isRunning_ = true; 

    LogMsg(LOG_INFO, "The game is initialized!");
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

    // call the EntityMgr::Update() to update all the entities
    g_EntityMgr.Update(deltaTime);
}

///////////////////////////////////////////////////////////

void Game::Render()
{
    if (g_EntityMgr.HasNoEntities())
        return;
    
    // call the EntityMgr::Render() to render all the entities
    g_EntityMgr.Render();
}

///////////////////////////////////////////////////////////

void Game::Destroy()
{
}

///////////////////////////////////////////////////////////

void Game::LoadLevel(const int levelNumber)
{
    // add entities and add components to the entities
    Entity& entt1 = g_EntityMgr.AddEntity("projectile");
    entt1.AddComponent<TransformComponent>(0,0, 20,20, 32,32,1);

    Entity& entt2 = g_EntityMgr.AddEntity("projectile2");
    entt2.AddComponent<TransformComponent>(100,0, 10,10, 16,16,1);

    Entity& entt3 = g_EntityMgr.AddEntity("projectile3");
    entt3.AddComponent<TransformComponent>(500,500, -10,-10, 40,40,1);
}

