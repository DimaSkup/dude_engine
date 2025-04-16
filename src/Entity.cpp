#include "Entity.h"


Entity::Entity(EntityMgr& mgr) : 
    m_EnttMgr(mgr),
    m_IsActive(true)
{ 
}

///////////////////////////////////////////////////////////

Entity::Entity(EntityMgr& mgr, const std::string& name) : 
    m_EnttMgr(mgr),
    m_IsActive(true),
    m_Name(name)
{ 
}

/////////////////////////////////////////////////

void Entity::Update(const float deltaTime)
{
    // update each component of this entity
    for (IComponent* pComponent : m_Components)
    {
        pComponent->Update(deltaTime);
    }
}

/////////////////////////////////////////////////

void Entity::Render()
{
    // render each component of entity (if necessary)
    for (IComponent* pComponent : m_Components)
    {
        pComponent->Render();
    }
}

/////////////////////////////////////////////////

void Entity::Destroy()
{
    m_IsActive = false;
}

///////////////////////////////////////////////////////////

void Entity::ListAllComponents() const
{
    printf("Components:\n");

    for (IComponent* pComp : m_Components)
    {
        printf("\t%s\n", pComp->GetName()); 
    }
    printf("\n");
}
