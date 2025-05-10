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
    LogDbg("constructor");
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
}

///////////////////////////////////////////////////////////

void EntityMgr::Update(const float deltaTime)
{
    for (Entity* pEntt : m_Entities)
    {
        pEntt->Update(deltaTime);
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
    m_EnttsByLayers[layer].emplace_back(pEntt);    // add this entt into the map of entities by layers (so we relate this entity to particular layer)

    return *pEntt;
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
    LogDbg(LOG_INFO, "components dump");

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
        LogErr("can't set a player: input ptr == nullptr");
    }
}

///////////////////////////////////////////////////////////

eColliderTag EntityMgr::CheckEnttCollisions(Entity* pInEntt) const
{
    const Collider* pCollider = pInEntt->GetComponent<Collider>();
    const SDL_Rect& colliderRect = pCollider->m_Collider;

    for (Entity* pEntity : m_Entities)
    {
        // avoid collision test with itself and with tilemaps
        if (strcmp(pEntity->m_Name, pInEntt->m_Name) == 0 &&
            strcmp(pEntity->m_Name, "Tile"))
            continue;

        if (pEntity->HasComponent<Collider>())
        {
            const Collider* pOtherCollider = pEntity->GetComponent<Collider>();
           
            // if we have collision btw input entt and the current one
            if (Collision::CheckRectCollision(colliderRect, pOtherCollider->m_Collider))
            {
                return pOtherCollider->m_ColliderTag;
            }
        }
    }

    return eColliderTag::COLLIDER_TAG_NONE;
}
