// ==================================================================
// Filename:    EntityMgr.cpp
// Description: implementation of the EntityMgr functional
// ==================================================================
#include "EntityMgr.h"
#include "Log.h"
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
    for (Entity* pEntt : entities_)
    {
        pEntt->Destroy();
        delete pEntt;
    }

    entities_.clear();
}

///////////////////////////////////////////////////////////

void EntityMgr::Update(const float deltaTime)
{
    for (Entity* pEntt : entities_)
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

    entities_.emplace_back(pEntt);                // add this entt into the main array of entities
    enttsByLayers_[layer].emplace_back(pEntt);    // add this entt into the map of entities by layers (so we relate this entity to particular layer)

    return *pEntt;
}

///////////////////////////////////////////////////////////

const std::vector<Entity*>& EntityMgr::GetEntts() const
{
    return entities_;
}

///////////////////////////////////////////////////////////

const std::vector<Entity*>* EntityMgr::GetEnttsByLayer(
    const eLayerType layer) const
{
    if (enttsByLayers_.find(layer) != enttsByLayers_.end())
        return &enttsByLayers_.at(layer);
    else
        return nullptr;
}

///////////////////////////////////////////////////////////

void EntityMgr::ListAllEntts() const
{
    // for debug: print the list of all entities and its components

    printf("\n\n");
    LogDbg(LOG_INFO, "components dump");

    for (int i = 0; const Entity* pEntt : entities_)
    {
        printf("Entity[%d]: EntityName: %s\n", i, pEntt->m_Name);
        pEntt->ListAllComponents();
        ++i;
    }
}

