// ==================================================================
// Filename:     EntityMgr.h
// Description:  entity manager for the EC (Entity-Component);
// Created:      14.04.2025 by DimaSkup
// ==================================================================
#ifndef ENTITYMGR_H
#define ENTITYMGR_H

#include "Types.h"
#include "Entity.h"
#include "IComponent.h"
#include <vector>
#include <map>

class EntityMgr
{
public:
    EntityMgr();

    void ClearData();
    void Update(const float deltaTime);
    void Render();

    inline bool HasNoEntts()  const { return entities_.size() == 0; }
    inline uint GetNumEntts() const { return entities_.size(); }

    Entity& AddEntity(const char* enttName, const eLayerType layer);

    const std::vector<Entity*>& GetEntts() const;
    const std::vector<Entity*>* GetEnttsByLayer(const eLayerType layer) const;

    void ListAllEntts() const;

private:
    std::vector<Entity*>                      entities_;
    std::map<eLayerType, std::vector<Entity*>> enttsByLayers_;
};

// =================================================================================
// Declare a global instance of the Entity manager
// =================================================================================
extern EntityMgr g_EntityMgr;

#endif
