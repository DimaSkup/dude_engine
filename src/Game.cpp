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
#include "Map.h"
#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/KeyboardControlComponent.h"

// init Game's static members 
SDL_Event Game::ms_Event;

Map* s_pMap = nullptr;


///////////////////////////////////////////////////////////

Game::Game() : m_IsRunning(false)
{
}

Game::~Game()
{
    if (s_pMap)
        delete s_pMap;
}

///////////////////////////////////////////////////////////

void Game::Initialize()
{
    LoadLevel(0);

    m_IsRunning = true; 

    LogMsg(LOG_INFO, "The game is initialized!");
}

///////////////////////////////////////////////////////////

void Game::ProcessInput()
{
    SDL_PollEvent(&ms_Event);

    switch (ms_Event.type)
    {
        case SDL_QUIT:
        {
            m_IsRunning = false;
            break;
        }
        case SDL_KEYDOWN:
        {
            if (ms_Event.key.keysym.sym == SDLK_ESCAPE)
                m_IsRunning = false;
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
    const int timeToWait = FRAME_TARGET_TIME - ((SDL_GetTicks() - m_TicksLastFrame));

    // only call delay if we are too fast to process this frame
    if (timeToWait > 0 && timeToWait <= FRAME_TARGET_TIME)
        SDL_Delay(timeToWait);

    // difference in ticks from last frame converted to seconds
    float deltaTime = (SDL_GetTicks() - m_TicksLastFrame) * 0.001f; 

    // set the new ticks count for the current frame to be used in the next pass
    m_TicksLastFrame = SDL_GetTicks();
 
    // clamp deltaTime to a maximum value
    deltaTime = (deltaTime > FRAME_TARGET_TIME) ? FRAME_TARGET_TIME : deltaTime;

    // call the EntityMgr::Update() to update all the entities
    g_EntityMgr.Update(deltaTime);
}

///////////////////////////////////////////////////////////

void Game::Render()
{
    if (g_EntityMgr.HasNoEntts())
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
    g_AssetMgr.AddTexture("tank-image",         "assets/images/tank-big-right.png");
    g_AssetMgr.AddTexture("chopper-image",      "assets/images/chopper-spritesheet.png");
    g_AssetMgr.AddTexture("radar-image",        "assets/images/radar.png");
    g_AssetMgr.AddTexture("jungle-tiletexture", "assets/tilemaps/jungle.png");

    // load tilemap and create tile entities
    constexpr int tileScale = 2;
    constexpr int tileSize = 32;
    s_pMap = new Map("jungle-tiletexture", tileScale, tileSize);

    constexpr int tileMapWidth = 25;
    constexpr int tileMapHeight = 20;
    s_pMap->LoadMap("assets/tilemaps/jungle.map", tileMapWidth, tileMapHeight);


    // add entities and add components to the entities
    Entity& enttTank = g_EntityMgr.AddEntity("tank", LAYER_ENEMY);
    enttTank.AddComponent<TransformComponent>(0, 0, 20, 20, 32, 32, 1);
    enttTank.AddComponent<SpriteComponent>("tank-image");

    Entity& enttChopper = g_EntityMgr.AddEntity("chopper", LAYER_PLAYER);
    enttChopper.AddComponent<TransformComponent>(240, 106, 0, 0, 32, 32, 1);
    enttChopper.AddComponent<SpriteComponent>("chopper-image", 2, 90, true, false);
    enttChopper.AddComponent<KeyboardControlComponent>("up", "right", "down", "left", "space");

    Entity& enttRadar = g_EntityMgr.AddEntity("radar", LAYER_UI);
    const int radarOffset = 15;
    const int radarPosX = Render::GetWndWidth() - 64 - radarOffset;
    enttRadar.AddComponent<TransformComponent>(radarPosX, 15, 0, 0, 64, 64, 1);
    enttRadar.AddComponent<SpriteComponent>("radar-image", 8, 150, false, true);

    sprintf(g_String, "level %d is loaded", levelNumber);
    LogMsg(g_String);
}

