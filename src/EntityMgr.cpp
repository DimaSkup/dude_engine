// ==================================================================
// Filename:    EntityMgr.cpp
// Description: implementation of the EntityMgr functional
// ==================================================================
#include "EntityMgr.h"
#include "Log.h"


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

void EntityMgr::ListAllEntities() const
{
    printf("\n\n");
    LogDbg(LOG_INFO, "components dump");

    for (int i = 0; const Entity* pEntt : entities_)
    {
        printf("Entity[%d]: EntityName: %s\n", i, pEntt->m_Name.c_str());
        pEntt->ListAllComponents();
        ++i;
    }
}

