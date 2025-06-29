// ==================================================================
// Filename:    EntityMgr.cpp
// Description: implementation of the EntityMgr functional
// ==================================================================
#include "EntityMgr.h"
#include "Log.h"
#include "Components/Collider.h"
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

///////////////////////////////////////////////////////////

void EntityMgr::Update(const float deltaTime)
{
    for (Entity* pEntt : m_Entities)
    {
        pEntt->Update(deltaTime);
    }

    DestroyInactiveEntts();
}

///////////////////////////////////////////////////////////

void EntityMgr::DestroyInactiveEntts()
{
    // TODO: optimize
    
    for (int i = 0; i < (int)m_Entities.size(); ++i)
    {
        if (!m_Entities[i]->IsActive())
            m_Entities.erase(m_Entities.begin() + i);
    }
}

///////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////

Entity& EntityMgr::AddEntity(const char* enttName, const eLayerType layer)
{
    Entity* pEntt = new Entity(*this, enttName, layer);

    m_Entities.emplace_back(pEntt);                // add this entt into the main array of entities
    m_EnttsByNames.insert({ enttName, pEntt });
    m_EnttsByLayers[layer].emplace_back(pEntt);    // add this entt into the map of entities by layers (so we relate this entity to particular layer)

    return *pEntt;
}

///////////////////////////////////////////////////////////

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
