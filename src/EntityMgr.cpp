// ==================================================================
// Filename:    EntityMgr.cpp
// Description: implementation of the EntityMgr functional
// ==================================================================
#include "EntityMgr.h"
#include "Log.h"
#include "Components/Collider.h"
#include "Components/Sprite.h"
#include "Components/ProjectileEmmiter.h"
#include "Components/LifeTimer.h"
#include "AssetMgr.h"
#include "EventMgr.h"
#include <stdio.h>

// init a global instance of the Entity manager
EntityMgr g_EntityMgr;


///////////////////////////////////////////////////////////

EntityMgr::EntityMgr()
{
    LogDbg(LOG, "constructor");
}

///////////////////////////////////////////////////////////

void EntityMgr::ClearData()
{
    for (Entity* pEntt : m_Entities)
    {
        pEntt->Destroy();
        delete pEntt;
    }

    m_Entities.clear();
    m_EnttsByNames.clear();
    m_EnttsByLayers.clear();
}


//---------------------------------------------------------
// Desc:   a handler for event when the player is shooting
// Args:   - player: player's entity
//         - mgr:    entity manager
//---------------------------------------------------------
void HandleEventPlayerShoot(Entity& player, EntityMgr& mgr)
{
    const Transform* pTransform = player.GetComponent<Transform>();
    const Sprite*    pSprite    = player.GetComponent<Sprite>();

    // compute the projectile's init position, velocity, direction
    const glm::vec2 playerPos    = pTransform->GetPosition();
    const int       playerWidth  = pTransform->GetWidth();
    const int       playerHeight = pTransform->GetHeight();

    int projectileOffsetX = 0;
    int projectileOffsetY = 0;

    // define an angle of projectile emitting based on player's direction
    int angleDeg = 0;

    switch (pSprite->GetCurrAnimationType())
    {
        case ANIMATION_TYPE_SINGLE:
        {
            break;
        }
        case ANIMATION_TYPE_DOWN:
        { 
            angleDeg = 90;  
            projectileOffsetX = playerWidth / 2;
            projectileOffsetY = playerHeight;
            break;
        }
        case ANIMATION_TYPE_UP:    
        {
            angleDeg = 270; 
            projectileOffsetX = playerWidth / 2;
            projectileOffsetY = 0;
            break;
        }
        case ANIMATION_TYPE_RIGHT: 
        {
            angleDeg = 0;   
            projectileOffsetX = playerWidth;
            projectileOffsetY = playerHeight / 2;
            break;
        }
        case ANIMATION_TYPE_LEFT:  
        {
            angleDeg = 180; 
            projectileOffsetX = 0;
            projectileOffsetY = playerHeight / 2;
            break;
        }
    }

    // generate a name for the projectile emmiter entity...
    const EntityID projectileID = mgr.m_LastEnttID + 1;
    char name[64]{0};
    sprintf(name, "player_projectile_%d", projectileID);

    // ... and create it
    Entity& entt = mgr.AddEntity(name, LAYER_PROJECTILE);

    
    // setup a projectile parameters
    constexpr int width  = 4;
    constexpr int height = 4;
    const     int posX   = (int)playerPos.x + projectileOffsetX - width/2;
    const     int posY   = (int)playerPos.y + projectileOffsetY - height/2;
    constexpr int velX   = 0;
    constexpr int velY   = 0;
    constexpr int scale  = 1;

    entt.AddComponent<Transform>(
        posX,
        posY,
        velX,
        velY,
        width,
        height,
        scale);
   
    entt.AddComponent<Sprite>("projectile-texture");

    entt.AddComponent<Collider>(
        eColliderTag::FRIENDLY_PROJECTILE,
        posX,
        posY,
        width,
        height);
        
    constexpr int speed = 600;
    constexpr int range = 300;
    constexpr bool loop = false;

    entt.AddComponent<ProjectileEmmiter>(
        speed,
        angleDeg,
        range,
        loop); 
}

//---------------------------------------------------------
// Desc:   a helper to play sound by its name only once
// Args:   - soundName: a name of the sound from the sound mgr
//---------------------------------------------------------
void PlaySound(const char* soundName)
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
// Desc:   main updating function for the entity manager;
//         here we update the all entities states
// Args:   - deltaTime: the time passed since the previous frame
//---------------------------------------------------------
void EntityMgr::Update(const float deltaTime)
{
    for (const Event& e : g_EventMgr.m_Events)
    {
        Entity* pEntt = GetEnttByID(e.id);

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
                Entity& player = *GetPlayer();
                HandleEventPlayerShoot(player, *this);
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
                m_PlayerIsKilled = true;
                break;
            }
            case EVENT_TYPE_DESTROY_ENTITY:
            {
                DestroyEntt(e.id);
                break;
            }
            case EVENT_TYPE_KILL_ENEMY:
            {
                Entity* pEnemyEntt = pEntt;

                PlaySound("explosion_2");

                // also destroy a projectile emmiter of this enemy
                const char* name = pEntt->GetName();
                char projectileName[64]{'\0'};
                sprintf(projectileName, "%s%s", name, "_projectile");

                // find a projectile entity and set that its
                // projectile emitter is not looped anymore
                Entity* pProjectile = GetEnttByName(projectileName);
                pProjectile->GetComponent<ProjectileEmmiter>()->SetLooped(false);

                const int numFrames        = (int)12;
                const int animationSpeed   = (int)100;
                const int lifeTimeMs       = numFrames * animationSpeed;
                const bool hasDirections   = false;
                const bool fixed           = false;

                const Transform* pEnemyTr  = pEnemyEntt->GetComponent<Transform>();
                const glm::vec2 pos        = pEnemyTr->GetPosition();
                const int       width      = pEnemyTr->GetWidth();
                const int       height     = pEnemyTr->GetHeight();
                const int       halfWidth  = width >> 1;
                const int       halfHeight = height >> 1;
                const int       centerX    = pos.x + halfWidth;
                const int       centerY    = pos.y + halfHeight;

                // setup explosion's transformation initial params
                TransformInitParams explTransform;
                explTransform.width      = 96;
                explTransform.height     = 96;
                explTransform.position.x = centerX - (explTransform.width  >> 1);
                explTransform.position.y = centerY - (explTransform.height >> 1);
                explTransform.velocity   = {0,0};
                explTransform.scale      = 1.0f;

                // setup explosion's sprite initial params
                SpriteInitParams explSprite;
                explSprite.

                Entity& explosionEntt = AddEntity("explosion", LAYER_OBSTACLE);
 
                explosionEntt.AddComponent<Transform>(explTransform);
                explosionEntt.AddComponent<Sprite>(
                    "explosion_2",
                    numFrames,
                    animationSpeed,
                    hasDirections,
                    fixed);

                // after explosion will play its animation one time we remove it
                explosionEntt.AddComponent<LifeTimer>(lifeTimeMs);

                // destroy the enemy entity
                DestroyEntt(e.id);

                break;
            }
        }
    }

    // we handled all the events so clear the list 
    g_EventMgr.m_Events.clear();
    
    //DestroyInactiveEntts();

    // update each component of each entity
    for (Entity* pEntt : m_Entities)
    {
        pEntt->Update(deltaTime);
    }

}

void EntityMgr::DestroyEntt(const EntityID id)
{
    Entity* pEntt = GetEnttByID(id);
    const char*      name  = pEntt->GetName();
    const eLayerType layer = pEntt->GetLayer();

    
    // remove a record from map of ids
    const auto& itID    = m_EnttsByIDs.find(id);
    if (itID != m_EnttsByIDs.end())
        m_EnttsByIDs.erase(itID);
    else
        LogErr(LOG, "there is no entt by ID: %d", id);

    // remove a record from map of names
    const auto& itName  = m_EnttsByNames.find(name);
    if (itName != m_EnttsByNames.end())
        m_EnttsByNames.erase(itName);
    else
        LogErr(LOG, "there is no entt by name: %s", name);

    // remove a record from map of layers
    const auto& itLayerVector = std::find(
        m_EnttsByLayers[layer].begin(),
        m_EnttsByLayers[layer].end(),
        pEntt);

    if (itLayerVector != m_EnttsByLayers[layer].end())
    {
        // remove a rectord from std::vector
        *itLayerVector = m_EnttsByLayers[layer].back();
        m_EnttsByLayers[layer].pop_back();
    }
    else
    {
        LogErr(LOG, "there is no entt by layer: %d", (int)layer);
    }

    // TODO: for debug
    LogMsg("entt is destroyed: %s", name);

    // find an idx of this entity in the entities array
    const auto& it = std::find(m_Entities.begin(), m_Entities.end(), pEntt);
    const int idx = std::distance(m_Entities.begin(), it);
   
    // release memory from the entity 
    delete pEntt;
    pEntt = nullptr;

    // remove a record from entities array
    m_Entities[idx] = m_Entities.back();
    m_Entities.pop_back();
}

//---------------------------------------------------------
// Desc:   go through each rendering layer type and 
//         render in order all the entities;
//         so entities with a higher layer type will be rendered
//         over the entts with a lower layer type
//---------------------------------------------------------
void EntityMgr::Render()
{
    for (int layerIdx = 0; layerIdx < eLayerType::NUM_LAYERS; ++layerIdx)
    { 
        const std::vector<Entity*>* pEntts = GetEnttsByLayer(eLayerType(layerIdx));

        if (!pEntts)  // we have no entts by the current layer
            continue;

        for (Entity* pEntt : *pEntts)
            pEntt->Render();
    }
}

//---------------------------------------------------------
// Desc:   create a new entity and add it into the entities manager
// Args:   - enttName: a name for new entity
//         - layerType: to which rendering layer the entt will belong
//
// Ret:    a reference to the created entity
//---------------------------------------------------------
Entity& EntityMgr::AddEntity(const char* enttName, const eLayerType layer)
{
    // define an ID for the new entity
    m_LastEnttID++;
    EntityID id = m_LastEnttID;

    Entity* pEntt = new Entity(*this, enttName, layer, id);

    // add this entt into the main array of entities
    m_Entities.emplace_back(pEntt);                 

    // add this entt into specific groups
    m_EnttsByIDs.insert({ id, pEntt });    
    m_EnttsByNames.insert({ enttName, pEntt });
    m_EnttsByLayers[layer].emplace_back(pEntt);    // add this entt into the map of entities by layers (so we relate this entity to particular layer)

    return *pEntt;
}

//---------------------------------------------------
// Desc:   get a ptr to entity by its ID
// Args:   - id: an id of searched entity
// Ret:    ptr to entity if we found it or nullptr if we didn't
//---------------------------------------------------
Entity* EntityMgr::GetEnttByID(const EntityID id)
{
    if (m_EnttsByIDs.find(id) != m_EnttsByIDs.end())
    {
        return m_EnttsByIDs[id];
    }
    else
    {
        LogErr(LOG, "there is no entity by ID: %d", id);
        return nullptr;
    }
}

//---------------------------------------------------
// Desc:   get a ptr to entity by its name
// Args:   - name: a name of searched entity
// Ret:    ptr to entity if we found it or nullptr if we didn't
//---------------------------------------------------
Entity* EntityMgr::GetEnttByName(const char* name)
{
    if (!name || name[0] == '\0')
    {
        LogErr(LOG, "input entity name is empty");
        return nullptr;
    }

    if (m_EnttsByNames.find(name) != m_EnttsByNames.end())
    {
        return m_EnttsByNames[name];
    }
    else
    {
        LogErr(LOG, "There is no entity by name: %s", name);
        return nullptr;
    }
}

///////////////////////////////////////////////////////////

const std::vector<Entity*>& EntityMgr::GetEntts() const
{
    return m_Entities;
}

///////////////////////////////////////////////////////////

const std::vector<Entity*>* EntityMgr::GetEnttsByLayer(
    const eLayerType layer) const
{
    if (m_EnttsByLayers.find(layer) != m_EnttsByLayers.end())
        return &m_EnttsByLayers.at(layer);
    else
        return nullptr;
}

///////////////////////////////////////////////////////////

void EntityMgr::ListAllEntts() const
{
    // for debug: print the list of all entities and its components

    printf("\n\n");
    LogDbg(LOG, "components dump");

    for (int i = 0; const Entity* pEntt : m_Entities)
    {
        printf("Entity[%d]: EntityName: %s\n", i, pEntt->m_Name);
        pEntt->ListAllComponents();
        ++i;
    }
}

///////////////////////////////////////////////////////////

void EntityMgr::SetPlayer(Entity* pEntt)
{
    // set entity as a player

    if (pEntt)
    {
        m_pPlayer = pEntt;
        LogDbg(LOG, "set player's entity to entt by name: %s", pEntt->GetName());
    }
    else
    {
        LogErr(LOG, "can't set a player: input ptr == nullptr");
    }
}

///////////////////////////////////////////////////////////

eCollisionType EntityMgr::CheckCollisions() const
{
    std::vector<Entity*> entitiesWithCollider;
    entitiesWithCollider.reserve(64);

    // get entities which have collider component
    for (Entity* pEntt : m_Entities)
    {
        if (pEntt->HasComponent<Collider>())
            entitiesWithCollider.push_back(pEntt);
    }

    // test each entity to collision with each other
    for (Entity* pEntt : entitiesWithCollider)
    {
        const Collider* pCollider1    = pEntt->GetComponent<Collider>();
        const SDL_Rect& colliderRect1 = pCollider1->m_ColliderRect;     // collider rectangle of the curr entity
        const eColliderTag cTag       = pCollider1->m_ColliderTag;      // collider tag of the curr entity

        // test with other entities
        for (Entity* pOtherEntt : entitiesWithCollider)
        {
            const Collider* pCollider2    = pOtherEntt->GetComponent<Collider>();
            const SDL_Rect& colliderRect2 = pCollider2->m_ColliderRect;

            if (Collision::CheckRectCollision(colliderRect1, colliderRect2))
            {
                const eColliderTag cTag2 = pCollider2->m_ColliderTag;

                // if the current entity is the player
                if (cTag == eColliderTag::PLAYER)
                {
                    switch (cTag2)
                    {
                //        case ENEMY:          return PLAYER_ENEMY_COLLISION;
                        case PROJECTILE:    
                        {
                            const EntityID playerID = pCollider1->GetOwner()->GetID();
                            g_EventMgr.AddEvent(EventPlayerHitEnemyProjectile(playerID));
                            break;
                        }
                //        case LEVEL_COMPLETE: return PLAYER_LEVEL_COMPLETE_COLLISION;
                    } 
                }

                // else we maybe have collision btw enemy and smth
                else if (cTag == eColliderTag::ENEMY)
                {
                    if (cTag2 == FRIENDLY_PROJECTILE)
                    {
                        const EntityID enemyID      = pCollider1->GetOwner()->GetID();
                        const EntityID projectileID = pCollider2->GetOwner()->GetID();

                        g_EventMgr.AddEvent(EventKillEnemy(enemyID));
                        g_EventMgr.AddEvent(EventDestroyEntity(projectileID)); 
                    }
                }
            }
        }
    }

    return NO_COLLISION;
}

///////////////////////////////////////////////////////////

eColliderTag EntityMgr::CheckEnttCollisions(Entity* pInEntt) const
{
    // check input entity to collision with the other entities
    // which have the Collider component;
    // if we found any collision we return the collider tag

    const Collider* pCollider = pInEntt->GetComponent<Collider>();
    const SDL_Rect& colliderRect = pCollider->m_ColliderRect;
    const char* name = pInEntt->m_Name;

    // go through all the entities
    for (Entity* pEntity : m_Entities)
    {
        // avoid collision test with itself and with tilemaps
        if (strcmp(pEntity->m_Name, name) == 0 ||
            strcmp(pEntity->m_Name, "Tile"))
            continue;

        if (pEntity->HasComponent<Collider>())
        {
            const Collider* pOtherCollider = pEntity->GetComponent<Collider>();
           
            // if we have collision btw input entt and the current one
            if (Collision::CheckRectCollision(colliderRect, pOtherCollider->m_ColliderRect))
                return pOtherCollider->m_ColliderTag;
        }
    }

    return eColliderTag::NONE;
}
