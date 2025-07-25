// ==================================================================
// Filename:     Entity.h
// Description:  entity of the Entity-Component;
//               each entity:
//               - contains a set of its components
//               - has a name
//               - has an activity flag
// ==================================================================
#ifndef ENTITY_H
#define ENTITY_H

#include "Constants.h"
#include "EventMgr.h"
#include "EntityMgr.h"
#include "IComponent.h"
#include "Types.h"
#include <vector>
#include <map>
#include <typeinfo>

class Component;
class EntityMgr;

class Entity
{
public:
    //Entity(EntityMgr& mgr);
    Entity(
        EntityMgr& mgr, 
        const char* name, 
        const eLayerType layer,
        const EntityID id);

    ~Entity();

    void Update(const float deltaTime);
    void Render();

    //-----------------------------------------------------
    // Desc:   destroy this entity on the next update of entity manager
    //-----------------------------------------------------
    inline void Destroy()
    {
        m_IsActive = false;
        g_EventMgr.AddEvent(EventDestroyEntity(m_ID));
    }

    inline bool        IsActive()  const { return m_IsActive; }
    inline EntityID    GetID()     const { return m_ID; }
    inline const char* GetName()   const { return m_Name; }
    inline eLayerType  GetLayer()  const { return m_Layer; }

    ///////////////////////////////////////////////////////

    template <typename T, typename... TArgs>
    T& AddComponent(TArgs&&... args)
    {
        T* pNewComponent(new T(std::forward<TArgs>(args)...));   // create a new component object
        pNewComponent->m_pOwner = this;                          // setup an owner for this new component
        m_Components.emplace_back(pNewComponent);                // store new component into the array of the entity components
        m_ComponentTypeMap[&typeid(*pNewComponent)] = pNewComponent; // make pair: [component_type => component_ptr]                                                            
        pNewComponent->Initialize();                             // and simply init this new component

        return *pNewComponent;
    }

    ///////////////////////////////////////////////////////
    
    template <typename T>
    T* GetComponent() 
    {
        if (HasComponent<T>())
            return static_cast<T*>(m_ComponentTypeMap[&typeid(T)]);
        else
            return nullptr;
    }

    ///////////////////////////////////////////////////////
    
    template <typename T>
    bool HasComponent() const 
    {

        return m_ComponentTypeMap.find(&typeid(T)) != m_ComponentTypeMap.end();
    }

    ///////////////////////////////////////////////////////
    
    void ListAllComponents() const;

public:
    EntityID                 m_ID = 0;
    eLayerType               m_Layer;
    char                     m_Name[32]{'\0'};
    bool                     m_IsActive = false;
    std::vector<IComponent*> m_Components;
    std::map<const std::type_info*, IComponent*> m_ComponentTypeMap; // pairs [component_type => component_ptr]

private:
    EntityMgr& m_EnttMgr;
};

#endif
