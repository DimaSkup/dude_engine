// ==================================================================
// Filename:    EntityMgr.cpp
// Description: implementation of the EntityMgr functional
// ==================================================================
#include "EntityMgr.h"
#include "Log.h"
#include "Components/Collider.h"
#include "Components/Sprite.h"
#include "Components/ProjectileEmmiter.h"
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
// Desc:   main updating function for the entity manager;
//         here we update the all entities states
// Args:   - deltaTime: the time passed since the previous frame
//---------------------------------------------------------
void EntityMgr::Update(const float deltaTime)
{
    for (const Event& e : g_EventMgr.m_Events)
    {
        const EntityID id = e.id;
        Entity* pEntt = GetEnttByID(id);

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
                Entity&          player     = *GetPlayer();
                const Transform* pTransform = player.GetComponent<Transform>();
                const Sprite*    pSprite    = player.GetComponent<Sprite>();

                // compute the projectile's init position, velocity, direction
                const glm::vec2 playerPos = pTransform->GetPosition();

                // define an angle of projectile emitting based on player's direction
                int angleDeg = 0;

                switch (pSprite->GetCurrAnimationType())
                {
                    case ANIMATION_TYPE_DOWN:  angleDeg = 90;  break;
                    case ANIMATION_TYPE_UP:    angleDeg = 270; break;
                    case ANIMATION_TYPE_RIGHT: angleDeg = 0;   break;
                    case ANIMATION_TYPE_LEFT:  angleDeg = 180; break;
                }
                const EntityID projectileID = m_LastEnttID + 1;
                char name[64]{0};
                sprintf(name, "player_projectile_%d", projectileID);

                Entity& entt = AddEntity(name, LAYER_PROJECTILE);

                const     int posX   = (int)playerPos.x;
                const     int posY   = (int)playerPos.y;
                constexpr int velX   = 0;
                constexpr int velY   = 0;
                constexpr int width  = 4;
                constexpr int height = 4;
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
            }
        }
    }

    // we handled all the events so clear the list 
    g_EventMgr.m_Events.clear();

    // update each component of each entity
    for (Entity* pEntt : m_Entities)
    {
        pEntt->Update(deltaTime);
    }

    DestroyInactiveEntts();
}

//---------------------------------------------------------
// Desc:   remove all the inactive entities
//---------------------------------------------------------
void EntityMgr::DestroyInactiveEntts()
{
    // TODO: optimize
   
    std::vector<int> inactiveEnttsIdxs;

    for (int i = 0; i < (int)m_Entities.size(); ++i)
    {
        if (!m_Entities[i]->IsActive())
        {
            Entity* pEntt = m_Entities[i];
            const EntityID   id    = pEntt->GetID();
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

            // release memory from the entity 
            delete pEntt;

            // store this idx so later we will remove record from a map
            inactiveEnttsIdxs.push_back(i);
        }
    }
    
    for (int idx : inactiveEnttsIdxs)
    {
        m_Entities[idx] = m_Entities.back();
        m_Entities.pop_back();
    }
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
                        case ENEMY:          return PLAYER_ENEMY_COLLISION;
                        case PROJECTILE:     return PLAYER_PROJECTILE_COLLISION;
                        case LEVEL_COMPLETE: return PLAYER_LEVEL_COMPLETE_COLLISION;
                    } 
                }

                // else we maybe have collision btw enemy and smth
                else if (cTag == eColliderTag::ENEMY)
                {
                   if (cTag2 == FRIENDLY_PROJECTILE)
                       return ENEMY_PROJECTILE_COLLISION;
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
