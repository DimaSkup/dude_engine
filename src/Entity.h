#ifndef ENTITY_H
#define ENTITY_H

#include "EntityMgr.h"
#include "IComponent.h"
#include <vector>
#include <string>

class Component;
class EntityMgr;

class Entity
{
public:
    Entity(EntityMgr& mgr);
    Entity(EntityMgr& mgr, const std::string& name);

    void Update(const float deltaTime);
    void Render();
    void Destroy();

    inline bool IsActive() const { return isActive_; }

    template <typename T, typename... TArgs>
    T& AddComponent(TArgs&&... args)
    {
        T* pNewComponent(new T(std::forward<TArgs>(args)...));  // create a new component object
        pNewComponent->pOwner_ = this;                           // setup an owner for this new component
        components_.emplace_back(pNewComponent);                // store new component into the array of the entity components
        pNewComponent->Initialize();                             // and simply init this new component

        return *pNewComponent;
    }

    void ListAllComponents() const;

public:
    std::string name_;
    bool isActive_ = false;
    std::vector<IComponent*> components_;

private:
    EntityMgr& enttMgr_;
};

#endif
