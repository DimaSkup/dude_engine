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
#include "Map.h"                         // for tilemaps
#include "Components/Transform.h"
#include "Components/Sprite.h"
#include "Components/KeyboardControl.h"
#include "Components/Collider.h"
#include "Components/TextLabel.h"
#include "Components/ProjectileEmmiter.h"
#include "GameState.h"


// init Game's static members 
SDL_Event Game::ms_Event;
SDL_Rect  Game::ms_Camera = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

const SDL_Color WHITE_COLOR = { 255, 255, 255, 255 };
const SDL_Color GREEN_COLOR = { 0, 255, 0, 255 };


// static pointer to the tilemap
Map* s_pMap = nullptr;

// init some globals
GameStates g_GameStates;

///////////////////////////////////////////////////////////

Game::Game() : m_Running(false)
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

    m_PrevTicks = SDL_GetTicks();

    m_Running = true;

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
            m_Running = false;
            break;
        }
        case SDL_KEYDOWN:
        {
            const SDL_Keycode key = ms_Event.key.keysym.sym;

            if      (key == SDLK_ESCAPE)  m_Running = false;          // shutdown and exit from the game
            else if (key == SDLK_F2)      m_ShowAABB = !m_ShowAABB;   // switch on/off the visualization of the AABB

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
    //const int timeToWait = FRAME_TARGET_TIME - ((SDL_GetTicks() - m_TicksLastFrame));

    // only call delay if we are too fast to process this frame
    //if (timeToWait > 0 && timeToWait <= FRAME_TARGET_TIME)
    //    SDL_Delay(timeToWait);

    // difference in ticks from last frame converted to seconds
    const uint32_t ticksNow = SDL_GetTicks();
    const uint32_t diff = (ticksNow - m_PrevTicks);

    m_FpsCounter += diff;
    m_FramesDrawn++;

    float deltaTime = (float)diff * 0.001f; 

    // set the new ticks count for the current frame to be used in the next pass
    m_PrevTicks = ticksNow;

    // compute actual fps value if necessary
    if (m_FpsCounter >= 1000)
    {
        m_FpsValue = (float)m_FramesDrawn / (float)(m_FpsCounter * 0.001f);
        m_FpsCounter = 0;
        m_FramesDrawn = 0;
    }
 
    // clamp deltaTime to a maximum value
    deltaTime = (deltaTime > 16.6f) ? 16.6f : deltaTime;

    // update all the entities
    g_EntityMgr.Update(deltaTime);

    HandleCameraMovement();
    CheckCollisions();

    // update UI text
    Entity* pFpsCount  = g_EntityMgr.GetEnttByName("fps");
    Entity* pDeltaTime = g_EntityMgr.GetEnttByName("delta time");

    char fpsBuf[32]{'\0'};
    char deltaTimeBuf[32]{'\0'};

    sprintf(fpsBuf, "Fps: %d", (int)m_FpsValue);
    sprintf(deltaTimeBuf, "Delta time: %d ms", diff);

    pFpsCount->GetComponent<TextLabel>()->SetLabelText(fpsBuf, "charriot-font");
    pDeltaTime->GetComponent<TextLabel>()->SetLabelText(deltaTimeBuf, "charriot-font");
}

///////////////////////////////////////////////////////////

void Game::HandleCameraMovement()
{
    Entity* pPlayer = g_EntityMgr.GetPlayer();
    Transform* pPlayerTransform = pPlayer->GetComponent<Transform>();

    ms_Camera.x = pPlayerTransform->m_Position.x - HALF_WND_WIDTH;
    ms_Camera.y = pPlayerTransform->m_Position.y - HALF_WND_HEIGHT;

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

    g_GameStates.cameraPosX = ms_Camera.x;
    g_GameStates.cameraPosY = ms_Camera.y;
}

///////////////////////////////////////////////////////////

void Game::CheckCollisions()
{
    const eCollisionType cType = g_EntityMgr.CheckCollisions();

    switch (cType)
    {
        case PLAYER_ENEMY_COLLISION:
        case PLAYER_PROJECTILE_COLLISION:
        {
            ProcessGameOver();
            break;
        }
        case PLAYER_LEVEL_COMPLETE_COLLISION:
        {
            ProcessNextLevel(1);
            break;
        }
    }
}

///////////////////////////////////////////////////////////

void Game::ProcessNextLevel(const int levelNumber)
{
    LogMsg("Next level");
    m_Running = false;
}

///////////////////////////////////////////////////////////

void Game::ProcessGameOver()
{
    LogMsg("Game Over");
    m_Running = false;
}

///////////////////////////////////////////////////////////

void Game::RenderColliderAABB() const
{
    // render visualization of colliders AABB for entities which have the Collider component

    std::vector<Entity*> enttsWithCollider;
    enttsWithCollider.reserve(8);

    // get only entities with collider
    for (Entity* pEntt : g_EntityMgr.GetEntts())
    {
        if (pEntt->HasComponent<Collider>())
            enttsWithCollider.push_back(pEntt);
    }

    // because we want to render AABB over the sprite, but not the actual collider position we get sprite's dest rect
    std::vector<SDL_Rect> dstRects(enttsWithCollider.size());

    for (int i = 0; Entity* pEntt : enttsWithCollider)
       dstRects[i++] = pEntt->GetComponent<Sprite>()->GetDstRect();

    // src rectangle of the AABB texture
    SDL_Texture*    pTexAABB = g_AssetMgr.GetTexture("bounding-box");
    const SDL_Point texSize  = g_AssetMgr.GetTextureSize(pTexAABB);
    const SDL_Rect  srcRect  = {0, 0, texSize.x, texSize.y};  

    // render AABB for each entt with collider    
    for (const SDL_Rect& dstRect : dstRects)
        Render::DrawRectTextured(pTexAABB, srcRect, dstRect, SDL_FLIP_NONE);
}

///////////////////////////////////////////////////////////

void Game::Render()
{
    if (g_EntityMgr.HasNoEntts())
        return;
    
    // render all the entities
    g_EntityMgr.Render();

    // render visualization of AABB if need (call it after the main rendering process)
    if (m_ShowAABB)
        RenderColliderAABB();

    // render UI text onto the screen
    RenderFont();
}

///////////////////////////////////////////////////////////

void Game::RenderFont()
{
    // render the text (UI elements) onto the screen

    // get entities with TextLabel component
    Entity* enttsWithTextLabel[32]{nullptr};
    int numEntts = 0;

    for (Entity* pEntt : g_EntityMgr.GetEntts())
    {
        enttsWithTextLabel[numEntts] = pEntt;
        numEntts += (pEntt->HasComponent<TextLabel>());
    }

    for (int i = 0; i < numEntts; ++i)
    {
        TextLabel* pComponent = enttsWithTextLabel[i]->GetComponent<TextLabel>();

        SDL_RenderCopy(
            g_pRenderer, 
            pComponent->GetTexture(), 
            NULL, 
            &pComponent->GetPosition());
    }
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
    g_AssetMgr.AddTexture("bounding-box",       "assets/images/collision-texture.png");
    g_AssetMgr.AddTexture("radar-image",        "assets/images/radar.png");
    g_AssetMgr.AddTexture("heliport-image",     "assets/images/heliport.png");
    g_AssetMgr.AddTexture("projectile-image",   "assets/images/bullet-enemy.png");
    g_AssetMgr.AddTexture("jungle-tiletexture", "assets/tilemaps/jungle.png");

    // load in a font
    constexpr int fontSize = 14;
    g_AssetMgr.AddFont("charriot-font", "assets/fonts/charriot.ttf", fontSize);

    // load tilemap and create tile entities
    constexpr int tileScale = 2;
    constexpr int tileSize = 32;
    s_pMap = new Map("jungle-tiletexture", tileScale, tileSize);

    constexpr int tileMapWidth = 25;
    constexpr int tileMapHeight = 20;
    s_pMap->LoadMap("assets/tilemaps/jungle.map", tileMapWidth, tileMapHeight);

    // compute full width and height of the level in pixels
    g_GameStates.levelMapWidth  = tileScale * tileSize * tileMapWidth;
    g_GameStates.levelMapHeight = tileScale * tileSize * tileMapHeight;

    LogMsg("map size: %d %d", g_GameStates.levelMapWidth, g_GameStates.levelMapHeight);

    // add and setup the "tank" entity
    Entity& enttTank = g_EntityMgr.AddEntity("tank", LAYER_ENEMY);
    enttTank.AddComponent<Transform>(150, 495, 5, 0, 32, 32, 1);
    enttTank.AddComponent<Sprite>("tank-image");
    enttTank.AddComponent<Collider>(eColliderTag::ENEMY, 0, 0, 32, 32);

    // add "projectile" entity
    Entity& projectile = g_EntityMgr.AddEntity("projectile", LAYER_PROJECTILE);
    projectile.AddComponent<Transform>(166, 511, 0, 0, 4, 4, 1);
    projectile.AddComponent<Sprite>("projectile-image");
    projectile.AddComponent<Collider>(eColliderTag::PROJECTILE, 166, 511, 4, 4); 
    projectile.AddComponent<ProjectileEmmiter>(50, 270, 200, true);
            
    // add "heliport" entity
    Entity& heliport = g_EntityMgr.AddEntity("Heliport", LAYER_OBSTACLE);
    heliport.AddComponent<Transform>(470, 420, 0, 0, 32, 32, 1);
    heliport.AddComponent<Sprite>("heliport-image");
    heliport.AddComponent<Collider>(eColliderTag::LEVEL_COMPLETE, 470, 420, 32, 32);

    // add and setup the "chopper" entity
    Entity& enttChopper = g_EntityMgr.AddEntity("chopper", LAYER_PLAYER);
    enttChopper.AddComponent<Transform>(240, 106, 0, 0, 32, 32, 1);
    constexpr bool chopperHasDirections = true;
    constexpr bool chopperIsFixed       = false;
    enttChopper.AddComponent<Sprite>("chopper-image", 2, 90, true, false);
    enttChopper.AddComponent<KeyboardControl>("up", "right", "down", "left", "space");
    enttChopper.AddComponent<Collider>(eColliderTag::PLAYER, 240, 106, 32, 32);
    g_EntityMgr.SetPlayer(&enttChopper);

    // add and setup the "radar" entity
    Entity& enttRadar = g_EntityMgr.AddEntity("radar", LAYER_UI);
    const int radarOffset = 15;
    const int radarPosX = Render::GetWndWidth() - 64 - radarOffset;
    enttRadar.AddComponent<Transform>(radarPosX, 15, 0, 0, 64, 64, 1);
    constexpr bool radarHasDirections = false;
    constexpr bool radarIsFixed       = true;
    enttRadar.AddComponent<Sprite>("radar-image", 8, 150, radarHasDirections, radarIsFixed);

    // create text entities
    Entity& labelLevelName = g_EntityMgr.AddEntity("LabelLevelName", LAYER_UI);
    labelLevelName.AddComponent<TextLabel>(10, 10, "First level...", "charriot-font", WHITE_COLOR); 

    Entity& fpsText = g_EntityMgr.AddEntity("fps", LAYER_UI);
    fpsText.AddComponent<TextLabel>(10, 30, "Fps: 0", "charriot-font", WHITE_COLOR);

    Entity& deltaTimeText = g_EntityMgr.AddEntity("delta time", LAYER_UI);
    deltaTimeText.AddComponent<TextLabel>(10, 50, "Delta time: 0ms", "charriot-font", WHITE_COLOR);

    sprintf(g_String, "level %d is loaded", levelNumber);
    LogMsg(g_String);
}

