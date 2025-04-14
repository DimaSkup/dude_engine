#include "Entity.h"


Entity::Entity(EntityMgr& mgr) : 
    enttMgr_(mgr),
    isActive_(true)
{ 
}

///////////////////////////////////////////////////////////

Entity::Entity(EntityMgr& mgr, const std::string& name) : 
    enttMgr_(mgr),
    isActive_(true),
    name_(name)
{ 
}

/////////////////////////////////////////////////

void Entity::Update(const float deltaTime)
{
    // update each component of this entity
    for (Component* pComponent : components_)
    {
        pComponent->Update(deltaTime);
    }
}

/////////////////////////////////////////////////

void Entity::Render()
{
    // render each component of entity (if necessary)
    for (Component* pComponent : components_)
    {
        pComponent->Render();
    }
}

/////////////////////////////////////////////////

void Entity::Destroy()
{
    isActive_ = false;
}
