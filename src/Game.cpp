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
#include "Components/LifeTimer.h"
#include "GameState.h"
#include "EventMgr.h"

#define SHOW_DBG_INFO_WHEN_CREATE_ENTITIES 0

// init Game's static members 
SDL_Event Game::ms_Event;
SDL_Rect  Game::ms_Camera = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

const SDL_Color WHITE_COLOR = { 255, 255, 255, 255 };
const SDL_Color GREEN_COLOR = { 0, 255, 0, 255 };

// static pointer to the tilemap
Map* s_pMap = nullptr;

// init some globals
GameStates g_GameStates;

int s_Sound = 0;
int s_Song = 0;


//==================================================================

Game::Game() : m_Running(false)
{
}

Game::~Game()
{
    g_AssetMgr.ClearData(); 
    
    if (s_pMap)
        delete s_pMap;
}

//---------------------------------------------------------
// Desc:   initialize the game
//---------------------------------------------------------
void Game::Initialize()
{
    LoadLevel(1);

    m_PrevTicks = SDL_GetTicks();

    m_Running = true;

    // setup the game state
    g_GameStates.SetWndDimensions(WINDOW_WIDTH, WINDOW_HEIGHT);

    // setup the camera
    ms_Camera = {0, 0, (int)g_GameStates.windowWidth, (int)g_GameStates.windowHeight };

    // compute the camera's limits
    g_GameStates.cameraMaxX = g_GameStates.levelMapWidth  - ms_Camera.w;
    g_GameStates.cameraMaxY = g_GameStates.levelMapHeight - ms_Camera.h;

    // load sound/music assets
    g_AssetMgr.LoadSound("./assets/sounds/helicopter.wav");
    g_AssetMgr.LoadSound("./assets/sounds/explosion_2.wav");
    g_AssetMgr.LoadMusic("./assets/sounds/Fortunate_Son.mp3");

    const int soundHelicopter = g_AssetMgr.GetSoundIdxByName("helicopter");
    const int musicBackground = g_AssetMgr.GetMusicIdxByName("Fortunate_Son");

    // start playing the background music and helicopter sound
    const int playTimes              = -1;
    const int soundChannelHelicopter = -1;


    g_AssetMgr.PlayMusic(musicBackground);

#if 1
    g_AssetMgr.PlaySound(
        soundChannelHelicopter,
        soundHelicopter, 
        playTimes+1);
#endif

    const int halfWndWidth  = Render::GetWndWidth() / 2;
    const int halfWndHeight = Render::GetWndHeight() / 2;

    const int lifeSpriteWidth = 32;
    const int lifeSpriteHeight = 32;
    
    const int posX[3] =
    {
        halfWndWidth - (int)(1.5f * lifeSpriteWidth) - 10,
        halfWndWidth - lifeSpriteWidth/2,
        halfWndWidth + lifeSpriteWidth/2 + 10
    };
    const int posY = 10;

    TransformInitParams trParams;
    trParams.width  = lifeSpriteWidth;
    trParams.height = lifeSpriteHeight;
    trParams.vel    = {0,0};
    trParams.scale  = 1;

    SpriteInitParams spriteParams;
    spriteParams.numFrames = 1;
    spriteParams.isFixed = true;

    Entity& lifeSprite1 = g_EntityMgr.AddEntity("life_1", LAYER_PROJECTILE);
    Entity& lifeSprite2 = g_EntityMgr.AddEntity("life_2", LAYER_PROJECTILE);
    Entity& lifeSprite3 = g_EntityMgr.AddEntity("life_3", LAYER_PROJECTILE);

    trParams.pos = { posX[0], posY };
    lifeSprite1.AddComponent<Transform>(trParams);

    trParams.pos = { posX[1], posY };
    lifeSprite2.AddComponent<Transform>(trParams);

    trParams.pos = { posX[2], posY };
    lifeSprite3.AddComponent<Transform>(trParams);

    lifeSprite1.AddComponent<Sprite>("chopper-texture", spriteParams);
    lifeSprite2.AddComponent<Sprite>("chopper-texture", spriteParams);
    lifeSprite3.AddComponent<Sprite>("chopper-texture", spriteParams);

    LogMsg(LOG, "The game is initialized!");
}

//---------------------------------------------------------
// Desc:   process input from the devices
//---------------------------------------------------------
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
            else if (key == SDLK_RETURN)  m_ShowHelpScreen = false;
            
            else if (key == SDLK_SPACE)   
            {
                const EntityID id = g_EntityMgr.GetPlayer()->GetID();
                g_EventMgr.AddEvent(EventPlayerShoot(id));
            }

            break;
        }
        default:
        {
            break;
        }
    }
}

//---------------------------------------------------------
// Desc:   main update method
//---------------------------------------------------------
void Game::Update()
{
    if (m_ShowHelpScreen)
        return;

    if (m_PlayerIsKilled)
    { 
        SetConsoleColor(RED);
            LogMsg("\nYOU LOST :(\nTRY AGAIN!\n");
            SetConsoleColor(RESET);
            ProcessGameOver();
    }

    // difference in ticks from last frame converted to seconds
    const uint32_t ticksNow     = SDL_GetTicks();
    const uint32_t deltaTimeMs  = (ticksNow - m_PrevTicks);
    const float    deltaTimeSec = (float)deltaTimeMs * 0.001f; 

    // clamp deltaTime to a maximum value
    //deltaTime = (deltaTime > 16.6f) ? 16.6f : deltaTime;

    // set the new ticks count for the current frame to be used in the next pass
    m_PrevTicks = ticksNow;

    m_TimeMs += deltaTimeMs;
    m_NumDrawnFrames++;

    // compute actual fps value if need
    if (m_TimeMs >= 1000)
    {
        m_FpsValue       = m_NumDrawnFrames;
        m_TimeMs         = 0;
        m_NumDrawnFrames = 0;
    }
 
    HandleEvents();
    g_EntityMgr.Update(deltaTimeSec);

    HandleCameraMovement();
    CheckCollisions();

    UpdateUIText(deltaTimeMs);
}

//---------------------------------------------------------
// Desc:  update text on the screen
//---------------------------------------------------------
void Game::UpdateUIText(const uint32_t dtMs)
{
    Entity* pFpsCount  = g_EntityMgr.GetEnttByName("fps");
    Entity* pDeltaTime = g_EntityMgr.GetEnttByName("delta time");

    char fpsBuf[32]{'\0'};
    char deltaTimeBuf[32]{'\0'};

    sprintf(fpsBuf, "Fps: %d", (int)m_FpsValue);
    sprintf(deltaTimeBuf, "Delta time: %d ms", (int)dtMs);

    pFpsCount->GetComponent<TextLabel>()->SetLabelText(fpsBuf, "charriot-font");
    pDeltaTime->GetComponent<TextLabel>()->SetLabelText(deltaTimeBuf, "charriot-font");
}

//---------------------------------------------------------
// Desc:  handle all the events from the events queue
//---------------------------------------------------------
void Game::HandleEvents()
{
    for (const Event& e : g_EventMgr.m_Events)
    {
        Entity* pEntt = g_EntityMgr.GetEnttByID(e.id);

        switch (e.type)
        {
            case EVENT_TYPE_SWITCH_ANIMATION:
            {
                eAnimationType animType = eAnimationType((int)e.x);
                pEntt->GetComponent<Sprite>()->Play(animType);
                break;
            }
            case EVENT_TYPE_PLAYER_SHOOT:
            {
                Entity* pPlayer = g_EntityMgr.GetPlayer();
                HandleEventPlayerShoot(*pPlayer);
                break;
            }
            case EVENT_TYPE_PLAYER_MOVE:
            {
                pEntt->GetComponent<Transform>()->SetVelocity(e.x, e.y);
                break;
            }
            case EVENT_TYPE_PLAYER_STOP:
            {
                pEntt->GetComponent<Transform>()->SetVelocity(0,0);
                break;
            }
            case EVENT_TYPE_PLAYER_HIT_ENEMY_PROJECTILE:
            {
                char name[16]{'\0'};
                sprintf(name, "%s%d", "life_", m_NumLifes);

                Entity* pEntt = g_EntityMgr.GetEnttByName(name);
                if (pEntt)
                {
                    g_EntityMgr.DestroyEntt(pEntt->GetID());
                }

                m_NumLifes--;
                g_EntityMgr.DestroyEntt(e.id);

                if (m_NumLifes == 0)
                    m_PlayerIsKilled = true;

                break;
            }
            case EVENT_TYPE_DESTROY_ENTITY:
            {
                g_EntityMgr.DestroyEntt(e.id);
                break;
            }
            case EVENT_TYPE_KILL_ENEMY:
            {
                Entity* pEnemyEntt = pEntt;

                PlaySound("explosion_2");

                // also destroy a projectile emmiter of this enemy:
                // find a projectile entity and set that its
                // projectile emitter is not looped anymore (so it will be destroyed)
                char projectileName[64]{'\0'};
                strcat(projectileName, pEntt->GetName());
                strcat(projectileName, "_projectile");

                Entity* pProjectileEntt = g_EntityMgr.GetEnttByName(projectileName);

                if (pProjectileEntt)
                    pProjectileEntt->GetComponent<ProjectileEmmiter>()->SetLooped(false);

                CreateExplosion(*pEnemyEntt);
               
                // destroy the enemy entity
                g_EntityMgr.DestroyEntt(e.id);
                g_GameStates.numEnemies--;

                break;
            }
        }
    }

    // we handled all the events so clear the list 
    g_EventMgr.m_Events.clear(); 
}



//---------------------------------------------------------
// Desc:   a handler for event when the player is shooting
// Args:   - player: player's entity
//---------------------------------------------------------
void Game::HandleEventPlayerShoot(Entity& player)
{
    const Transform* pPlayerTransform = player.GetComponent<Transform>();
    const Sprite*    pPlayerSprite    = player.GetComponent<Sprite>();

    // compute the projectile's init position, velocity, direction
    const glm::vec2 playerPos    = pPlayerTransform->GetPosition();
    const int       playerWidth  = pPlayerTransform->GetWidth();
    const int       playerHeight = pPlayerTransform->GetHeight();

    // position offset of the player's bullet to create it in proper position
    int bulletOffsetX = 0;
    int bulletOffsetY = 0;

    // define an angle of projectile emitting based on player's direction
    int angleDeg = 0;

    switch (pPlayerSprite->GetCurrAnimationType())
    {
        case ANIMATION_TYPE_SINGLE:
        {
            break;
        }
        case ANIMATION_TYPE_DOWN:
        { 
            angleDeg      = 90;  
            bulletOffsetX = playerWidth / 2;
            bulletOffsetY = playerHeight;
            break;
        }
        case ANIMATION_TYPE_UP:    
        {
            angleDeg      = 270; 
            bulletOffsetX = playerWidth / 2;
            bulletOffsetY = 0;
            break;
        }
        case ANIMATION_TYPE_RIGHT: 
        {
            angleDeg      = 0;   
            bulletOffsetX = playerWidth;
            bulletOffsetY = playerHeight / 2;
            break;
        }
        case ANIMATION_TYPE_LEFT:  
        {
            angleDeg      = 180; 
            bulletOffsetX = 0;
            bulletOffsetY = playerHeight / 2;
            break;
        }
    }

    // generate a name for the projectile emmiter entity...
    const EntityID bulletID = g_EntityMgr.m_LastEnttID + 1;
    char name[64]{0};
    sprintf(name, "player_projectile_%d", bulletID);

    // ... and create it
    Entity& bullet = g_EntityMgr.AddEntity(name, LAYER_PROJECTILE);

    // setup player's bullet parameters
    TransformInitParams trParams;
    trParams.width  = 4;
    trParams.height = 4;
    trParams.pos.x  = (int)playerPos.x + bulletOffsetX - trParams.width/2;
    trParams.pos.y  = (int)playerPos.y + bulletOffsetY - trParams.height/2;
    trParams.vel    = {0,0};
    trParams.scale  = 2;

    constexpr int speed = 800;
    constexpr int range = 600;
    constexpr bool loop = false;

    // add components
    bullet.AddComponent<Transform>(trParams);
    bullet.AddComponent<Sprite>("projectile-texture");

    bullet.AddComponent<Collider>(
        eColliderTag::FRIENDLY_PROJECTILE,
        (int)trParams.pos.x,
        (int)trParams.pos.y,
        trParams.width,
        trParams.height);
        
    bullet.AddComponent<ProjectileEmmiter>(speed, angleDeg, range, loop); 
}

//---------------------------------------------------------
// Desc:   when we kill some enemy we draw an explosion in its place
// Args:   - enemy:  an entity of enemy which we kill
//---------------------------------------------------------
void Game::CreateExplosion(Entity& enemy)
{
    const Transform* pEnemyTr  = enemy.GetComponent<Transform>();
    const glm::vec2 pos        = pEnemyTr->GetPosition();

    const int width      = pEnemyTr->GetWidth();
    const int height     = pEnemyTr->GetHeight();
    const int centerX    = pos.x + (width / 2);
    const int centerY    = pos.y + (height / 2);

    // setup explosion's transformation initial params
    TransformInitParams trParams;
    trParams.width    = 96;
    trParams.height   = 96;
    trParams.scale    = 2.0f;
    
    const int scaledWidth  = trParams.width * trParams.scale;
    const int scaledHeight = trParams.height * trParams.scale;

    trParams.pos.x    = centerX - (scaledWidth  >> 1);
    trParams.pos.y    = centerY - (scaledHeight >> 1);
    trParams.vel      = {0,0};

    // setup explosion's sprite initial params
    SpriteInitParams spriteParams;
    spriteParams.numFrames      = 12;
    spriteParams.animationSpeed = 100;
    spriteParams.hasDirections  = false;
    spriteParams.isFixed        = false;

    // compute life time of explosion based on duration of full animation
    const int lifeTimeMs = spriteParams.numFrames * spriteParams.animationSpeed;

    Entity& explosion = g_EntityMgr.AddEntity("explosion", LAYER_OBSTACLE);
    explosion.AddComponent<Transform>(trParams);
    explosion.AddComponent<Sprite>("explosion_2", spriteParams);

    // we play the explosion animation one time and then remove it
    explosion.AddComponent<LifeTimer>(lifeTimeMs);
}

//---------------------------------------------------------
// Desc:   a helper to play sound by its name only once
// Args:   - soundName: a name of the sound from the sound mgr
//---------------------------------------------------------
void Game::PlaySound(const char* soundName)
{
    if (!soundName || soundName[0] == '\0')
    {
        LogErr(LOG, "can't play sound: input name is empty!");
        return;
    }

    // get sound idx
    const int soundIdx = g_AssetMgr.GetSoundIdxByName(soundName);

    // try to play sound
    int channel = 3;
    constexpr int playTimes = 1;
    eSoundState soundState = g_AssetMgr.PlaySound(channel, soundIdx, playTimes);

    // if for any reason the channel is busy we try another one
    while (soundState == CHANNEL_STATE_BUSY)
    {
        channel++;
        soundState = g_AssetMgr.PlaySound(channel, soundIdx, playTimes);
    }
}

//---------------------------------------------------------
// Desc:   compute the camera's position relatively 
//         to the player's position
//---------------------------------------------------------
void Game::HandleCameraMovement()
{
    Entity* pPlayer = g_EntityMgr.GetPlayer();

    if (!pPlayer)
    {
        LogErr(LOG, "ptr to player == nullptr");
        exit(-1);
    }

    Transform* pPlayerTransform = pPlayer->GetComponent<Transform>();

    ms_Camera.x = pPlayerTransform->m_Position.x - HALF_WND_WIDTH;
    ms_Camera.y = pPlayerTransform->m_Position.y - HALF_WND_HEIGHT;

    const uint cameraRight  = ms_Camera.x + ms_Camera.w;
    const uint cameraBottom = ms_Camera.y + ms_Camera.h;

    // clamp the camera's position by X
    if (ms_Camera.x < 0)
        ms_Camera.x = 0;
    
    if (cameraRight > g_GameStates.levelMapWidth)
        ms_Camera.x = g_GameStates.cameraMaxX;

    // clamp the camera's position by Y
    if (ms_Camera.y < 0)
        ms_Camera.y = 0;
    
    if (cameraBottom > g_GameStates.levelMapHeight)
        ms_Camera.y = g_GameStates.cameraMaxY;

    g_GameStates.cameraPosX = ms_Camera.x;
    g_GameStates.cameraPosY = ms_Camera.y;
}

//---------------------------------------------------------
// Desc:  check collisions btw entities
//---------------------------------------------------------
void Game::CheckCollisions()
{
    const eCollisionType cType = g_EntityMgr.CheckCollisions();

    switch (cType)
    {
        case NO_COLLISION:
        {
            return;
        }
        case PLAYER_LEVEL_COMPLETE_COLLISION:
        {
            // if we killed them all
            if (g_GameStates.numEnemies == 0)
            {
                ProcessNextLevel(1);
                SetConsoleColor(CYAN);
                LogMsg("\n\nLOL, YOU WON!\n\n");
                SetConsoleColor(RESET);
            }
            break;
        }
    }
}

//---------------------------------------------------------
// Desc:   load in a level by number
//---------------------------------------------------------
void Game::ProcessNextLevel(const int levelNumber)
{
    LogMsg(LOG, "Next level");
    m_Running = false;
    g_AssetMgr.ClearData(); 
}

//---------------------------------------------------------
// Desc:   shutdown the game
//---------------------------------------------------------
void Game::ProcessGameOver()
{
    m_Running = false;
    Destroy();
    LogMsg(LOG, "Game Over");
}

//---------------------------------------------------------
// Desc:   visualize bounding-box for each entity which has
//         a collider component
//---------------------------------------------------------
void Game::RenderColliderAABB() const
{
    // render visualization of colliders AABB for entities which have the Collider component

    std::vector<Entity*> enttsWithCollider;
    enttsWithCollider.reserve(32);

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

//---------------------------------------------------------
// Desc:   render all the stuff onto the screen
//---------------------------------------------------------
void Game::Render()
{
    if (m_ShowHelpScreen)
    {
        SDL_Texture* pTex = g_AssetMgr.GetTexture("help-screen-texture-level1");
        const SDL_Point texSize = g_AssetMgr.GetTextureSize(pTex);
        const SDL_Rect  srcRect = {0, 0, texSize.x, texSize.y};  
        const SDL_Rect  dstRect = {0, 0, g_GameStates.windowWidth, g_GameStates.windowHeight };

        Render::DrawRectTextured(
            pTex,
            srcRect,
            dstRect,
            SDL_FLIP_NONE);

        return;
    } 

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

//---------------------------------------------------------
// Desc:   render the text stuff onto the screen
//---------------------------------------------------------
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

//---------------------------------------------------------
// Desc:   destroy the current level
//---------------------------------------------------------
void Game::Destroy()
{
    g_AssetMgr.ClearData(); 
}

//---------------------------------------------------------
// Desc:   load assets from lua config file
// Args:   - assets:  data for assets initialization
//---------------------------------------------------------
void LoadAssets(sol::table assets)
{
   // load in assets
    int assetIdx = 0;
    std::string assetType(16, ' ');
    std::string assetId(64, ' ');
    std::string assetPath(128, ' ');

    while (true)
    {
        sol::optional<sol::table> existsAssetIdxNode = assets[assetIdx];

        // we reached the end of table or there is no data at all
        if (existsAssetIdxNode == sol::nullopt)
        {
            break;
        }
        else
        {
            sol::table asset = assets[assetIdx];
            assetType        = asset["type"];
            assetId          = asset["id"];
            assetPath        = asset["file"];

            // we want to load some texture
            if (assetType == "texture")
            {
                g_AssetMgr.AddTexture(assetId.c_str(), assetPath.c_str());
            }

            // load some font
            else if(assetType == "font")
            {
                const int fontSize = asset["fontSize"];
                g_AssetMgr.AddFont(assetId.c_str(), assetPath.c_str(), fontSize);
            }
        }

        assetIdx++;
    }
}

//---------------------------------------------------------
// Desc:   load a map from lua config file
// Args:   - levelMap: data for map initialization
//---------------------------------------------------------
void LoadMap(sol::table levelMap)
{
    LogMsg(LOG, "start loading of map");

    std::string mapTextureId = levelMap["textureAssetId"];
    std::string mapPath      = levelMap["file"];

    const int tileScale     = (int)levelMap["scale"];
    const int tileSize      = (int)levelMap["tileSize"];
    const int tileMapWidth  = (int)levelMap["mapSizeX"];
    const int tileMapHeight = (int)levelMap["mapSizeY"];

#if SHOW_DBG_INFO_WHEN_CREATE_ENTITIES
    printf("map tex id:               %s\n", mapTextureId.c_str());
    printf("map file:                 %s\n", mapPath.c_str());
    printf("map tile scale:           %d\n", tileScale);
    printf("map tile size:            %d\n", tileSize);
    printf("map size x (tiles count): %d\n", tileMapWidth);
    printf("map size y (tiles count): %d\n", tileMapHeight);
#endif

    s_pMap = new Map(mapTextureId.c_str(), tileScale, tileSize);
    s_pMap->LoadMap(mapPath.c_str(), tileMapWidth, tileMapHeight);

    // compute full width and height of the level in pixels
    g_GameStates.levelMapWidth  = tileScale * tileSize * tileMapWidth;
    g_GameStates.levelMapHeight = tileScale * tileSize * tileMapHeight;

    LogMsg(LOG, "map is loaded");
}
//---------------------------------------------------------
// Desc:   add to input entity a transform component by input data
// Args:   - entt: entity to modify
//         - tr:   transformation data for transform component
//---------------------------------------------------------
void AddTransformComponent(Entity& entt, sol::table tr)
{
    const int posX   = (int)tr["position"]["x"];
    const int posY   = (int)tr["position"]["y"];
    const int velX   = (int)tr["velocity"]["x"];
    const int velY   = (int)tr["velocity"]["y"];

    const int width  = (int)tr["width"];
    const int height = (int)tr["height"];
    const int scale  = (int)tr["scale"];
    // const int rotation = (int)tr["rotation"];
    
#if SHOW_DBG_INFO_WHEN_CREATE_ENTITIES
    printf("\t\tposX:     %d\n", posX);
    printf("\t\tposY:     %d\n", posY);
    printf("\t\tvelX:     %d\n", velX);
    printf("\t\tvelY:     %d\n", velY);
    printf("\t\twidth:    %d\n", width);
    printf("\t\theight:   %d\n", height);
    printf("\t\tscale:    %d\n", scale);
    //printf("\t\trotation: %d\n", rotation);
#endif

    entt.AddComponent<Transform>(
        posX,
        posY,
        velX,
        velY,
        width,
        height,
        scale);
}

//---------------------------------------------------------
// Desc:   add to input entity a sprite component by input data
// Args:   - entt:   entity to modify
//         - sprite: a table with data for component
//---------------------------------------------------------
void AddSpriteComponent(Entity& entt, sol::table sprite)
{ 
    const std::string assetId = sprite["textureAssetId"];
    const bool animated       = (bool)sprite["animated"];

    // if we want to load in data for animated sprite
    if (animated)
    {   const int frameCount     = (int)sprite["frameCount"];
        const int animationSpeed = (int)sprite["animationSpeed"];
        const bool hasDirections = (bool)sprite["hasDirections"];
        const bool fixed         = (bool)sprite["fixed"];

#if SHOW_DBG_INFO_WHEN_CREATE_ENTITIES
        printf("\t\tassetId:        %s\n", assetId.c_str());
        printf("\t\tanimated:       %d\n", animated);
        printf("\t\tframeCount:     %d\n", frameCount);
        printf("\t\tanimationSpeed: %d\n", animationSpeed);
        printf("\t\thasDirections:  %d\n", hasDirections);
        printf("\t\tfixed:          %d\n", fixed);
#endif

        entt.AddComponent<Sprite>(
            assetId.c_str(),
            frameCount,
            animationSpeed,
            hasDirections,
            fixed);
    }

    // we want to load in a static sprite (no animation)
    else
    {
#if SHOW_DBG_INFO_WHEN_CREATE_ENTITIES
        printf("\t\tassetId:  %s\n", assetId.c_str());
        printf("\t\tanimated: %d\n", animated);
#endif

        entt.AddComponent<Sprite>(assetId.c_str());
    }
}

//---------------------------------------------------------
// Desc:   add to input entity a collider component by input data
// Args:   - entt:   entity to modify
//         - sprite: a table with data for sprite component
//         - tr:     a table with data related to transformation
//---------------------------------------------------------
void AddColliderComponent(
    Entity& entt, 
    sol::table collider,
    sol::table tr)
{
    const std::string colliderTag = collider["tag"];
    eColliderTag tag = eColliderTag::NONE;

    // define a type of collider
    if (colliderTag == "PLAYER")
        tag = eColliderTag::PLAYER;

    else if (colliderTag == "ENEMY")
    {
        tag = eColliderTag::ENEMY;
        g_GameStates.numEnemies++;
    }

    else if (colliderTag == "PROJECTILE")
        tag = eColliderTag::PROJECTILE;

    else if (colliderTag == "FRIENDLY_PROJECTILE")
        tag = eColliderTag::FRIENDLY_PROJECTILE;

    else if (colliderTag == "LEVEL_COMPLETE")
        tag = eColliderTag::LEVEL_COMPLETE;


    const int colliderBoxPosX = tr["position"]["x"];
    const int colliderBoxPosY = tr["position"]["y"];
    const int colliderBoxWidth = tr["width"];
    const int colliderBoxHeight = tr["height"];

#if SHOW_DBG_INFO_WHEN_CREATE_ENTITIES
    // print data into console for debug
    printf("\t\tAdd collider component:\n");
    printf("\t\tcollider tag: %s\n", colliderTag.c_str());
    printf("\t\tposX:         %d\n", colliderBoxPosX);
    printf("\t\tposY:         %d\n", colliderBoxPosY);
    printf("\t\twidth:        %d\n", colliderBoxWidth);
    printf("\t\theight:       %d\n", colliderBoxHeight);
#endif

    entt.AddComponent<Collider>(
        tag, 
        colliderBoxPosX, 
        colliderBoxPosY,
        colliderBoxWidth,
        colliderBoxHeight);
}

//---------------------------------------------------------
// Desc:   add to input entity a keyboard component by input data
// Args:   - entt:     entity to modify
//         - keyboard: a table with data for component
//---------------------------------------------------------
void AddKeyboardComponent(Entity& entt, sol::table keyboard)
{
    const std::string upKey    = keyboard["up"];
    const std::string leftKey  = keyboard["left"];
    const std::string downKey  = keyboard["down"];
    const std::string rightKey = keyboard["right"];
    const std::string shootKey = keyboard["shoot"];

#if SHOW_DBG_INFO_WHEN_CREATE_ENTITIES
    printf("\t\tAdd keyboard component:\n");
    printf("\t\tup key:    %s\n", upKey.c_str());
    printf("\t\tleft key:  %s\n", leftKey.c_str());
    printf("\t\tdown key:  %s\n", downKey.c_str());
    printf("\t\tright key: %s\n", rightKey.c_str());
    printf("\t\tshoot key: %s\n", shootKey.c_str());
#endif
   
    entt.AddComponent<KeyboardControl>(
        upKey.c_str(),
        rightKey.c_str(),
        downKey.c_str(),
        leftKey.c_str(),
        shootKey.c_str());
}

//---------------------------------------------------------
// Desc:   bind a separate projectile emitter entity
//         to input entity
// Args:   - entt:    bind projectile entity to this entity
//         - emitter: data for the projectile emitter component
//---------------------------------------------------------
void AddProjectileEmitterComponent(Entity& entt, sol::table emitter)
{
    std::string assetId = emitter["textureAssetId"];
    const int  speed    = emitter["speed"];
    const int  angleDeg = emitter["angle"];
    const int  range    = emitter["range"];
    const bool loop     = emitter["shouldLoop"];
    const int  width    = emitter["width"];
    const int  height   = emitter["height"];

#if SHOW_DBG_INFO_WHEN_CREATE_ENTITIES
    printf("\t\tAdd projectile emitter component:\n");
    printf("\t\tspeed:      %d\n", speed);
    printf("\t\tangleDeg:   %d\n", angleDeg);
    printf("\t\trange:      %d\n", range);

    printf("\t\tloop(bool): %d\n", loop);
    printf("\t\twidth:      %d\n", width);
    printf("\t\theight:     %d\n", height);
#endif

    // create a separate projectile emiter entity
    char projectileName[64]{0};
    strcat(projectileName, entt.GetName());
    strcat(projectileName, "_projectile");

    Entity& projectile = g_EntityMgr.AddEntity(projectileName, LAYER_PROJECTILE);

    const     Transform& tr      = *entt.GetComponent<Transform>();
    const     glm::vec2 pos      = tr.GetPosition();
    const     int posX           = (int)pos.x;
    const     int posY           = (int)pos.y;
    constexpr int velocityX      = 0;
    constexpr int velocityY      = 0;
    const     int enttHalfWidth  = tr.GetWidth() / 2;
    const     int enttHalfHeight = tr.GetHeight() / 2;
    constexpr int scale          = 1;

    // add components to the projectile emitter entity
    projectile.AddComponent<Transform>(
        posX + enttHalfWidth,
        posY + enttHalfHeight,
        velocityX,
        velocityY,
        width,
        height,
        scale);

    projectile.AddComponent<Sprite>(assetId.c_str());

    projectile.AddComponent<Collider>(
        eColliderTag::PROJECTILE,
        posX + enttHalfWidth,
        posY + enttHalfHeight, 
        width, 
        height); 
    
    projectile.AddComponent<ProjectileEmmiter>(
        speed,
        angleDeg,
        range,
        loop);
}

//---------------------------------------------------------
// Desc:   load entities from lua config file
// Args:   - entts: data container for entities initialization
//---------------------------------------------------------
void LoadEntities(const sol::table entts)
{
    int         enttIdx = 0;
    eLayerType  layerType;
    std::string enttName(32, ' ');
    std::string assetId(32, ' ');

    // load in all the entities
    while (true)
    {
        sol::optional<sol::table> enttIdxNode = entts[enttIdx];

        // we reached the end of table or there is no data at all
        if (enttIdxNode == sol::nullopt)
        {
            break;
        }
        else
        {
            const sol::table enttData = entts[enttIdx];

            // define a name and layer for a new entity
            enttName  = enttData["name"];
            const char* name = enttName.c_str();
            layerType = eLayerType(enttData["layer"]);

#if SHOW_DBG_INFO_WHEN_CREATE_ENTITIES
            SetConsoleColor(YELLOW);
            printf("entt name:   %s\n", name);
            printf("entt layer:  %d\n", (int)enttData["layer"]);
            SetConsoleColor(RESET);
#endif

            // create an entity
            Entity& entt = g_EntityMgr.AddEntity(name, layerType);
            
            // table of components which we will bind to the entity
            const sol::table components = enttData["components"];

            // since all the other components may depend on transform
            // component we add it to the entity first of all
            AddTransformComponent(entt, components["transform"]);

            // go through the table of components and add 
            // components to the entity
            for (const auto& keyToValue : components)
            {
                const char* componentName = keyToValue.first.as<const char*>();

                   // add a sprite component to the entity
                if (strcmp(componentName, "sprite") == 0)
                {
                    AddSpriteComponent(entt, keyToValue.second);
                    continue;
                }

                // add a collider component to the entity
                if (strcmp(componentName, "collider") == 0)
                {
                    AddColliderComponent(
                        entt, 
                        keyToValue.second,       // data for collider
                        components["transform"]);

                    continue;
                }

                // add an input control (keyboard/mouse) to this entity
                if (strcmp(componentName, "input") == 0)
                {
                    sol::table input = keyToValue.second;
                    AddKeyboardComponent(entt, input["keyboard"]);
                    continue;
                }

                if (strcmp(componentName, "projectileEmitter") == 0)
                {
                    AddProjectileEmitterComponent(entt, keyToValue.second);
                    continue;
                }
            }
        }

        enttIdx++;
    }
}

//---------------------------------------------------------
// Desc:   load level's data from lua script
// Args:   - levelNumber: which level we will load
//---------------------------------------------------------
void LoadLevelFromLuaScript(const int levelNumber)
{
    // define the filename to load level
    char levelName[32]{'\0'};
    char luaScriptPath[64]{'\0'};

    snprintf(levelName, 32, "%s%d", "Level", levelNumber);
    LogDbg(LOG, "Load level: %s", levelName);

    // load level from .lua file
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::os, sol::lib::math);

    snprintf(
        luaScriptPath, 
        64, 
        "%s%s%s", 
        "./assets/scripts/", 
        levelName, 
        ".lua");

    LogDbg(LOG, "Load level from lua file: %s", luaScriptPath);
    lua.script_file(luaScriptPath);

    // load stuff from lua script
    LoadAssets(lua[levelName]["assets"]);
    LoadMap(lua[levelName]["map"]);
    LoadEntities(lua[levelName]["entities"]);

    // setup a pointer to the player's entity
    Entity* pEnttPlayer = g_EntityMgr.GetEnttByName("player");
    if (!pEnttPlayer)
    {
        LogErr(LOG, "there is no entity by name: player");
        exit(-1);
    }

    g_EntityMgr.SetPlayer(pEnttPlayer);

}

//---------------------------------------------------------
// Desc:   execute initialization of level by input number
// Args:   - levelNumber: what level to load
//---------------------------------------------------------
void Game::LoadLevel(const int levelNumber)
{
    LoadLevelFromLuaScript(levelNumber);

    // create text entities
    Entity& labelLevelName = g_EntityMgr.AddEntity("LabelLevelName", LAYER_UI);
    labelLevelName.AddComponent<TextLabel>(10, 10, "First level...", "charriot-font", WHITE_COLOR); 

    Entity& fpsText = g_EntityMgr.AddEntity("fps", LAYER_UI);
    fpsText.AddComponent<TextLabel>(10, 30, "Fps: 0", "charriot-font", WHITE_COLOR);

    Entity& deltaTimeText = g_EntityMgr.AddEntity("delta time", LAYER_UI);
    deltaTimeText.AddComponent<TextLabel>(10, 50, "Delta time: 0ms", "charriot-font", WHITE_COLOR);
    LogMsg(LOG, "level %d is loaded", levelNumber);
}

