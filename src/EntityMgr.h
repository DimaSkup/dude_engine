// ==================================================================
// Filename:     EntityMgr.h
// Description:  entity manager for the EC (Entity-Component);
// Created:      14.04.2025 by DimaSkup
// ==================================================================
#ifndef ENTITYMGR_H
#define ENTITYMGR_H

#include "Types.h"
#include "Entity.h"
#include "Component.h"

#include <string>
#include <vector>

class EntityMgr
{
public:
    void ClearData();
    void Update(const float deltaTime);
    void Render();

    inline bool HasNoEntities()  const { return entities_.size() == 0; }
    inline uint GetEntityCount() const { return entities_.size(); }

    Entity& AddEntity(const std::string& enttName);
    const std::vector<Entity*>& GetEntities() const;

private:
    std::vector<Entity*> entities_;
};

#endif
