// ==================================================================
// Filename:     EntityMgr.h
// Description:  entity manager for the EC (Entity-Component);
// Created:      14.04.2025 by DimaSkup
// ==================================================================
#ifndef ENTITYMGR_H
#define ENTITYMGR_H

#include "Types.h"
#include "Entity.h"
#include "Collision.h"           // collision math tests
#include "IComponent.h"
#include <vector>
#include <map>
#include <string>

class EntityMgr
{
public:
    EntityMgr();

    void ClearData();
    void Update(const float deltaTime);
    void DestroyInactiveEntts();
    void Render();

    inline bool HasNoEntts()  const { return m_Entities.empty(); }
    inline uint GetNumEntts() const { return m_Entities.size(); }

    Entity& AddEntity(const char* enttName, const eLayerType layer);

    const std::vector<Entity*>& GetEntts() const;
    const std::vector<Entity*>* GetEnttsByLayer(const eLayerType layer) const;

    void ListAllEntts() const;

    void           SetPlayer(Entity* pEntt);
    inline Entity* GetPlayer() const { return m_pPlayer; }

    Entity* GetEnttByID(const EntityID id);
    Entity* GetEnttByName(const char* name);

    // collision tests
    eCollisionType CheckCollisions() const;
    eColliderTag   CheckEnttCollisions(Entity* pEntt) const;

    EntityID m_LastEnttID = 0;
    bool m_PlayerIsKilled = false;

private:
    Entity*              m_pPlayer = nullptr;
    std::vector<Entity*> m_Entities;

    std::map<EntityID, Entity*>                m_EnttsByIDs;
    std::map<std::string, Entity*>             m_EnttsByNames;
    std::map<eLayerType, std::vector<Entity*>> m_EnttsByLayers;
};

// =================================================================================
// Declare a global instance of the Entity manager
// =================================================================================
extern EntityMgr g_EntityMgr;

#endif
