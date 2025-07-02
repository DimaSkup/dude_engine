#include "Entity.h"
#include "StrHelper.h"
#include "Log.h"
#include <stdio.h>
#include <string.h>

#if 0
Entity::Entity(EntityMgr& mgr) : 
    m_EnttMgr(mgr),
    m_IsActive(true)
{ 
    strcpy(m_Name, "no_name");
}
#endif

///////////////////////////////////////////////////////////

Entity::Entity(
    EntityMgr& mgr, 
    const char* name, 
    const eLayerType layer,
    const EntityID id) 
    :
    m_EnttMgr(mgr),
    m_IsActive(true),
    m_Layer(layer),
    m_ID(id)
{ 
    if (IsStrEmpty(name))
    {
        // print an error msg and set default name
        LogErr(LOG, "input name for entity is empty!");
        strcpy(m_Name, "invalid");
    }
    else
    {
        strncpy(m_Name, name, 32);
    }
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

    for (IComponent* pComponent : m_Components)
    {
        delete pComponent;
    }
}

///////////////////////////////////////////////////////////

void Entity::ListAllComponents() const
{
    printf("Components:\n");

    for (IComponent* pComp : m_Components)
    {
        printf("\tComponent<%s>\n", pComp->GetName()); 
    }
    printf("\n");
}
