#ifndef ENTITY_H
#define ENTITY_H

#include "EntityMgr.h"
#include "IComponent.h"
#include <vector>
#include <string>
#include <map>
#include <typeinfo>

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

    inline bool IsActive() const { return m_IsActive; }

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
        return static_cast<T*>(m_ComponentTypeMap[&typeid(T)]);
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
    std::string              m_Name;
    bool                     m_IsActive = false;
    std::vector<IComponent*> m_Components;
    std::map<const std::type_info*, IComponent*> m_ComponentTypeMap; // pairs [component_type => component_ptr]

private:
    EntityMgr& m_EnttMgr;
};

#endif
