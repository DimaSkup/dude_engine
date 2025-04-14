// ==================================================================
// Filename:    EntityMgr.cpp
// Description: implementation of the EntityMgr functional
// ==================================================================
#include "EntityMgr.h"

void EntityMgr::ClearData()
{
    for (Entity* pEntt : entities_)
        pEntt->Destroy();

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
    for (Entity* pEntt : entities_)
    {
        pEntt->Render();
    }
}

///////////////////////////////////////////////////////////

Entity& EntityMgr::AddEntity(const std::string& enttName)
{
    Entity* pEntt = new Entity(*this, enttName);
    entities_.emplace_back(pEntt);

    return *pEntt;
}

///////////////////////////////////////////////////////////

const std::vector<Entity*>& EntityMgr::GetEntities() const
{
    return entities_;
}

///////////////////////////////////////////////////////////


