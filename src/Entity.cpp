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
    for (IComponent* pComponent : components_)
    {
        pComponent->Update(deltaTime);
    }
}

/////////////////////////////////////////////////

void Entity::Render()
{
    // render each component of entity (if necessary)
    for (IComponent* pComponent : components_)
    {
        pComponent->Render();
    }
}

/////////////////////////////////////////////////

void Entity::Destroy()
{
    isActive_ = false;
}

///////////////////////////////////////////////////////////

void Entity::ListAllComponents() const
{
    printf("Components:\n");

    for (IComponent* pComp : components_)
    {
        printf("\t%s\n", pComp->GetName()); 
    }
    printf("\n");
}
