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
    LoadLevel(1);

    m_PrevTicks = SDL_GetTicks();

    m_Running = true;

    // setup the game state
    g_GameStates.SetWndDimensions(WINDOW_WIDTH, WINDOW_HEIGHT);

    // setup the camera
    ms_Camera = {0, 0, g_GameStates.windowWidth, g_GameStates.windowHeight };

    // compute the camera's limits
    g_GameStates.cameraMaxX = g_GameStates.levelMapWidth  - ms_Camera.w;
    g_GameStates.cameraMaxY = g_GameStates.levelMapHeight - ms_Camera.h;

    LogMsg(LOG, "The game is initialized!");
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
        case NO_COLLISION:
        {
            return;
        }
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
    LogMsg(LOG, "Next level");
    m_Running = false;
}

///////////////////////////////////////////////////////////

void Game::ProcessGameOver()
{
    LogMsg(LOG, "Game Over");
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

//---------------------------------------------------------
// Desc:   load assets from lua config file
// Args:   - assets:  data for assets initialization
//---------------------------------------------------------
void LoadAssets(sol::table assets)
{
#if 0
    // start loading new assets into the Asset Manager list
    g_AssetMgr.AddTexture("tank-texture-big-left",         "assets/images/tank-big-right.png");
    g_AssetMgr.AddTexture("chopper-texture",      "assets/images/chopper-spritesheet.png");
    g_AssetMgr.AddTexture("bounding-box",       "assets/images/collision-texture.png");
    g_AssetMgr.AddTexture("radar-texture",        "assets/images/radar.png");
    g_AssetMgr.AddTexture("heliport-texture",     "assets/images/heliport.png");
    g_AssetMgr.AddTexture("projectile-texture",   "assets/images/bullet-enemy.png");
    g_AssetMgr.AddTexture("terrain-texture-day", "assets/tilemaps/jungle.png");
#else

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
#endif
}

//---------------------------------------------------------
// Desc:   load a map from lua config file
// Args:   - levelMap: data for map initialization
//---------------------------------------------------------
void LoadMap(sol::table levelMap)
{
    LogMsg(LOG, "start loading of map");

#if 0

    // load tilemap and create tile entities
    constexpr int tileScale = 2;
    constexpr int tileSize = 32;
    s_pMap = new Map("terrain-texture-day", tileScale, tileSize);

    constexpr int tileMapWidth = 25;
    constexpr int tileMapHeight = 20;
    s_pMap->LoadMap("assets/tilemaps/jungle.map", tileMapWidth, tileMapHeight);

    // compute full width and height of the level in pixels
    g_GameStates.levelMapWidth  = tileScale * tileSize * tileMapWidth;
    g_GameStates.levelMapHeight = tileScale * tileSize * tileMapHeight;

    LogMsg(LOG, "map size: %d %d", g_GameStates.levelMapWidth, g_GameStates.levelMapHeight);

#else

    std::string mapTextureId = levelMap["textureAssetId"];
    std::string mapPath      = levelMap["file"];

    const int tileScale     = (int)levelMap["scale"];
    const int tileSize      = (int)levelMap["tileSize"];
    const int tileMapWidth  = (int)levelMap["mapSizeX"];
    const int tileMapHeight = (int)levelMap["mapSizeY"];

    printf("map tex id:               %s\n", mapTextureId.c_str());
    printf("map file:                 %s\n", mapPath.c_str());
    printf("map tile scale:           %d\n", tileScale);
    printf("map tile size:            %d\n", tileSize);
    printf("map size x (tiles count): %d\n", tileMapWidth);
    printf("map size y (tiles count): %d\n", tileMapHeight);

    s_pMap = new Map(mapTextureId.c_str(), tileScale, tileSize);
    s_pMap->LoadMap(mapPath.c_str(), tileMapWidth, tileMapHeight);

    // compute full width and height of the level in pixels
    g_GameStates.levelMapWidth  = tileScale * tileSize * tileMapWidth;
    g_GameStates.levelMapHeight = tileScale * tileSize * tileMapHeight;

    LogMsg(LOG, "map is loaded");

#endif
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
    
    printf("\t\tposX:     %d\n", posX);
    printf("\t\tposY:     %d\n", posY);
    printf("\t\tvelX:     %d\n", velX);
    printf("\t\tvelY:     %d\n", velY);
    printf("\t\twidth:    %d\n", width);
    printf("\t\theight:   %d\n", height);
    printf("\t\tscale:    %d\n", scale);
    //printf("\t\trotation: %d\n", rotation);

    entt.AddComponent<Transform>(
        posX,
        posY,
        velX,
        velY,
        width,
        height,
        scale);
    
    LogDbg(LOG, "component \"transform\" is added");

}


//---------------------------------------------------------
// Desc:   load entities from lua config file
// Args:   - entts: data container for entities initialization
//---------------------------------------------------------
void LoadEntities(const sol::table entts)
{
#if 0 

    // add and setup the "chopper" entity
    Entity& enttChopper = g_EntityMgr.AddEntity("chopper", LAYER_PLAYER);
    enttChopper.AddComponent<Transform>(240, 106, 0, 0, 32, 32, 1);
    enttChopper.AddComponent<Sprite>("chopper-texture", 2, 90, true, false);
    enttChopper.AddComponent<KeyboardControl>("up", "right", "down", "left", "space");
    enttChopper.AddComponent<Collider>(eColliderTag::PLAYER, 240, 106, 32, 32);
    g_EntityMgr.SetPlayer(&enttChopper);

#else

    int         enttIdx = 0;
    eLayerType  enttLayerType;
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
            enttName = enttData["name"];
            enttLayerType = eLayerType(enttData["layer"]);

            SetConsoleColor(YELLOW);
            printf("entt name: %s\n", enttName.c_str());
            printf("entt layer: %d\n", (int)enttData["layer"]);
            SetConsoleColor(RESET);

            // create an entity
            Entity& entt = g_EntityMgr.AddEntity(enttName.c_str(), enttLayerType);
            
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
                const sol::object componentData = keyToValue.second;

                SetConsoleColor(CYAN);
                printf("\tadd component: %s\n", componentName);
                SetConsoleColor(RESET);

                   // add a sprite component to the entity
                if (strcmp(componentName, "sprite") == 0)
                {
                    sol::table sprite = componentData;
 
                    assetId = sprite["textureAssetId"];
                    const bool animated = (bool)sprite["animated"];

                    // if we want to load in data for animated sprite
                    if (animated)
                    {   const int frameCount     = (int)sprite["frameCount"];
                        const int animationSpeed = (int)sprite["animationSpeed"];
                        const bool hasDirections = (bool)sprite["hasDirections"];
                        const bool fixed         = (bool)sprite["fixed"];

                        printf("\t\tassetId:        %s\n", assetId.c_str());
                        printf("\t\tanimated:       %d\n", animated);
                        printf("\t\tframeCount:     %d\n", frameCount);
                        printf("\t\tanimationSpeed: %d\n", animationSpeed);
                        printf("\t\thasDirections:  %d\n", hasDirections);
                        printf("\t\tfixed:          %d\n", fixed);

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
                        printf("\t\tassetId:  %s\n", assetId.c_str());
                        printf("\t\tanimated: %d\n", animated);

                        entt.AddComponent<Sprite>(assetId.c_str());
                    }

                    LogDbg(LOG, "component is added: sprite");
                    continue;
                }

                // add a collider component to the entity
                if (strcmp(componentName, "collider") == 0)
                {
                    sol::table collider = componentData;
                    sol::table tr = components["transform"];
                    
                    // TODO:
                    std::string colliderTag = collider["tag"];
                    eColliderTag tag;

                    if (colliderTag == "PLAYER")
                        tag = eColliderTag::PLAYER;

                    const int colliderBoxPosX = tr["position"]["x"];
                    const int colliderBoxPosY = tr["position"]["y"];
                    const int colliderBoxWidth = tr["width"];
                    const int colliderBoxHeight = tr["height"];

                    // print data into console for debug
                    printf("\t\tcollider tag: %s\n", colliderTag.c_str());
                    printf("\t\tposX:         %d\n", colliderBoxPosX);
                    printf("\t\tposY:         %d\n", colliderBoxPosY);
                    printf("\t\twidth:        %d\n", colliderBoxWidth);
                    printf("\t\theight:       %d\n", colliderBoxHeight);

                    entt.AddComponent<Collider>(
                        tag, 
                        colliderBoxPosX, 
                        colliderBoxPosY,
                        colliderBoxWidth,
                        colliderBoxHeight);
                    
                    LogDbg(LOG, "component is added: collider");
                    continue;
                }

                // add an input control (keyboard/mouse) to this entity
                if (strcmp(componentName, "input") == 0)
                {
                    const sol::table input     = componentData;
                    const sol::table keyboard  = input["keyboard"];

                    const std::string upKey    = keyboard["up"];
                    const std::string leftKey  = keyboard["left"];
                    const std::string downKey  = keyboard["down"];
                    const std::string rightKey = keyboard["right"];
                    const std::string shootKey = keyboard["shoot"];

                    printf("\t\tup key:    %s\n", upKey.c_str());
                    printf("\t\tleft key:  %s\n", leftKey.c_str());
                    printf("\t\tdown key:  %s\n", downKey.c_str());
                    printf("\t\tright key: %s\n", rightKey.c_str());
                    printf("\t\tshoot key: %s\n", shootKey.c_str());

                   
                    entt.AddComponent<KeyboardControl>(
                        upKey.c_str(),
                        rightKey.c_str(),
                        downKey.c_str(),
                        leftKey.c_str(),
                        shootKey.c_str());

                    LogDbg(LOG, "component is added: input (keyboard)");
                    continue;
                }

                if (strcmp(componentName, "projectileEmitter") == 0)
                {
                    LogErr(LOG, "got here");
                    const sol::table emmiter = componentData;
                    const sol::table tr = components["transform"];

                    assetId = emmiter["textureAssetId"];
                    const int  speed    = emmiter["speed"];
                    const int  angleDeg = emmiter["angle"];
                    const int  range    = emmiter["range"];
                    const bool loop     = emmiter["shouldLoop"];
                    const int  width    = emmiter["width"];
                    const int  height   = emmiter["height"];
                
                    printf("\t\tspeed:      %d\n", speed);
                    printf("\t\tangleDeg:   %d\n", angleDeg);
                    printf("\t\trange:      %d\n", range);

                    printf("\t\tloop(bool): %d\n", loop);
                    printf("\t\twidth:      %d\n", width);
                    printf("\t\theight:     %d\n", height);

                    // add a separate projectile emmiter entity
                    char projectileName[64]{0};
                    strcat(projectileName, enttName.c_str());
                    strcat(projectileName, "_projectile");

                    LogDbg(LOG, "add projectile: %s", projectileName);
#if 1
                    Entity& projectile = g_EntityMgr.AddEntity(projectileName, LAYER_PROJECTILE);

                    AddTransformComponent(projectile, components["transform"]);

                    projectile.GetComponent<Transform>()->SetWidth(4);
                    projectile.GetComponent<Transform>()->SetHeight(4);
                    projectile.AddComponent<Sprite>(assetId.c_str());

                    projectile.AddComponent<Collider>(
                        eColliderTag::PROJECTILE, 
                        tr["position"]["x"],
                        tr["position"]["y"],
                        4, 4); 
                    
                    projectile.AddComponent<ProjectileEmmiter>(
                        speed,
                        angleDeg,
                        range,
                        loop);
#endif
                    LogDbg(LOG, "component is added: projectile emiter");
                    continue;
                }
            }
        }

        enttIdx++;
    }

    // setup a pointer to the player's entity
    Entity* pEnttPlayer = g_EntityMgr.GetEnttByName("player");

    if (!pEnttPlayer)
    {
        LogErr(LOG, "there is no entity by name: player");
        exit(-1);
    }

    g_EntityMgr.SetPlayer(pEnttPlayer);
#endif
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
}

///////////////////////////////////////////////////////////

void Game::LoadLevel(const int levelNumber)
{
    LoadLevelFromLuaScript(levelNumber);


    // load in a font
    //constexpr int fontSize = 14;
    //g_AssetMgr.AddFont("charriot-font", "assets/fonts/charriot.ttf", fontSize);

#if 0
    // add and setup the "tank" entity
    Entity& enttTank = g_EntityMgr.AddEntity("tank", LAYER_ENEMY);
    enttTank.AddComponent<Transform>(150, 495, 5, 0, 32, 32, 1);
    enttTank.AddComponent<Sprite>("tank-texture-big-left");
    enttTank.AddComponent<Collider>(eColliderTag::ENEMY, 0, 0, 32, 32);

    // add "projectile" entity
    Entity& projectile = g_EntityMgr.AddEntity("projectile", LAYER_PROJECTILE);
    projectile.AddComponent<Transform>(166, 511, 0, 0, 4, 4, 1);
    projectile.AddComponent<Sprite>("projectile-texture");
    projectile.AddComponent<Collider>(eColliderTag::PROJECTILE, 166, 511, 4, 4); 
    projectile.AddComponent<ProjectileEmmiter>(50, 270, 200, true);
            
    // add "heliport" entity
    Entity& heliport = g_EntityMgr.AddEntity("Heliport", LAYER_OBSTACLE);
    heliport.AddComponent<Transform>(470, 420, 0, 0, 32, 32, 1);
    heliport.AddComponent<Sprite>("heliport-texture");
    heliport.AddComponent<Collider>(eColliderTag::LEVEL_COMPLETE, 470, 420, 32, 32);

    // add and setup the "radar" entity
    Entity& enttRadar = g_EntityMgr.AddEntity("radar", LAYER_UI);
    const int radarOffset = 15;
    const int radarPosX = Render::GetWndWidth() - 64 - radarOffset;
    enttRadar.AddComponent<Transform>(radarPosX, 15, 0, 0, 64, 64, 1);
    constexpr bool radarHasDirections = false;
    constexpr bool radarIsFixed       = true;
    enttRadar.AddComponent<Sprite>("radar-texture", 8, 150, radarHasDirections, radarIsFixed);

#endif
    // create text entities
    Entity& labelLevelName = g_EntityMgr.AddEntity("LabelLevelName", LAYER_UI);
    labelLevelName.AddComponent<TextLabel>(10, 10, "First level...", "charriot-font", WHITE_COLOR); 

    Entity& fpsText = g_EntityMgr.AddEntity("fps", LAYER_UI);
    fpsText.AddComponent<TextLabel>(10, 30, "Fps: 0", "charriot-font", WHITE_COLOR);

    Entity& deltaTimeText = g_EntityMgr.AddEntity("delta time", LAYER_UI);
    deltaTimeText.AddComponent<TextLabel>(10, 50, "Delta time: 0ms", "charriot-font", WHITE_COLOR);
    LogMsg(LOG, "level %d is loaded", levelNumber);
}

