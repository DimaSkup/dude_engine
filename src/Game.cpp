// ==================================================================
// Filename: Game.cpp
// Created:  08.04.2025 by DimaSkup
// ==================================================================
#include "Game.h"
#include "../lib/glm/glm.hpp"
#include "Constants.h"
#include "Log.h"
#include "EntityMgr.h"
#include "AssetMgr.h"
#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"

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
    // start loading new assets into the Asset Manager list
    const char* textureFilePath = "assets/images/tank-big-right.png";
    g_AssetMgr.AddTexture("tank-image", textureFilePath);

    // add entities and add components to the entities
    Entity& enttTank = g_EntityMgr.AddEntity("tank");
    enttTank.AddComponent<TransformComponent>(0,0, 20,20, 32,32,1);
    enttTank.AddComponent<SpriteComponent>("tank-image");
}

