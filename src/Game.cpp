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
#include "GameState.h"


// init Game's static members 
SDL_Event Game::ms_Event;
SDL_Rect  Game::ms_Camera = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

Map* s_pMap = nullptr;

// init some globals
GameStates g_GameStates;
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

    // setup the game state
    g_GameStates.SetWndDimensions(WINDOW_WIDTH, WINDOW_HEIGHT);

    // setup the camera
    ms_Camera = {0, 0, g_GameStates.windowWidth, g_GameStates.windowHeight };

    // compute the camera's limits
    g_GameStates.cameraMaxX = g_GameStates.levelMapWidth  - ms_Camera.w;
    g_GameStates.cameraMaxY = g_GameStates.levelMapHeight - ms_Camera.h;

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

    HandleCameraMovement();
}

///////////////////////////////////////////////////////////

void Game::HandleCameraMovement()
{
    Entity* pPlayer = g_EntityMgr.GetPlayer();
    TransformComponent* pPlayerTransform = pPlayer->GetComponent<TransformComponent>();

    ms_Camera.x = pPlayerTransform->m_Position.x - HALF_WND_WIDTH;
    ms_Camera.y = pPlayerTransform->m_Position.y - HALF_WND_HEIGHT;

#if 0

    // clamp the camera's position
    ms_Camera.x = (ms_Camera.x < 0) ? 0 : ms_Camera.x;
    ms_Camera.y = (ms_Camera.y < 0) ? 0 : ms_Camera.y;

    const uint cameraRight  = ms_Camera.x + ms_Camera.w;
    const uint cameraBottom = ms_Camera.y + ms_Camera.h;

    // limit maximal values for top left corner of the camera rectangle
    ms_Camera.x = (cameraRight  >= g_GameStates.levelMapWidth)  ? g_GameStates.cameraMaxX : ms_Camera.x;
    ms_Camera.y = (cameraBottom >= g_GameStates.levelMapHeight) ? g_GameStates.cameraMaxY : ms_Camera.y;

#else
    const uint cameraRight  = ms_Camera.x + ms_Camera.w;
    const uint cameraBottom = ms_Camera.y + ms_Camera.h;

    // clamp the camera's position by X
    if (ms_Camera.x < 0)
    {
        ms_Camera.x = 0;
    }
    else if (cameraRight > g_GameStates.levelMapWidth)
    {
        ms_Camera.x = g_GameStates.cameraMaxX;
    }

    // clamp the camera's position by Y
    if (ms_Camera.y < 0)
    {
        ms_Camera.y = 0;
    }
    else if (cameraBottom > g_GameStates.levelMapHeight)
    {
        ms_Camera.y = g_GameStates.cameraMaxY;
    }
#endif

    g_GameStates.cameraPosX = ms_Camera.x;
    g_GameStates.cameraPosY = ms_Camera.y;
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
    constexpr int tileScale = 4;
    constexpr int tileSize = 32;
    s_pMap = new Map("jungle-tiletexture", tileScale, tileSize);

    constexpr int tileMapWidth = 25;
    constexpr int tileMapHeight = 20;
    s_pMap->LoadMap("assets/tilemaps/jungle.map", tileMapWidth, tileMapHeight);

    // compute full width and height of the level in pixels
    g_GameStates.levelMapWidth = tileScale * tileSize * tileMapWidth;
    g_GameStates.levelMapHeight = tileScale * tileSize * tileMapHeight;

    LogMsg("map size: %d %d", g_GameStates.levelMapWidth, g_GameStates.levelMapHeight);

    // add entities and add components to the entities
    Entity& enttTank = g_EntityMgr.AddEntity("tank", LAYER_ENEMY);
    enttTank.AddComponent<TransformComponent>(0, 0, 20, 20, 32, 32, 1);
    enttTank.AddComponent<SpriteComponent>("tank-image");

    Entity& enttChopper = g_EntityMgr.AddEntity("chopper", LAYER_PLAYER);
    enttChopper.AddComponent<TransformComponent>(240, 106, 0, 0, 32, 32, 1);
    enttChopper.AddComponent<SpriteComponent>("chopper-image", 2, 90, true, false);
    enttChopper.AddComponent<KeyboardControlComponent>("up", "right", "down", "left", "space");
    g_EntityMgr.SetPlayer(&enttChopper);

    Entity& enttRadar = g_EntityMgr.AddEntity("radar", LAYER_UI);
    const int radarOffset = 15;
    const int radarPosX = Render::GetWndWidth() - 64 - radarOffset;
    enttRadar.AddComponent<TransformComponent>(radarPosX, 15, 0, 0, 64, 64, 1);
    enttRadar.AddComponent<SpriteComponent>("radar-image", 8, 150, false, true);

    sprintf(g_String, "level %d is loaded", levelNumber);
    LogMsg(g_String);
}

