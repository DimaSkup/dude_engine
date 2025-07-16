// ==================================================================
// Filename:    IComponent.h
// Description: an interface for EC (Entity-Component) components
//
// Created:     15.04.2025 by DimaSkup
// ==================================================================
#ifndef ICOMPONENT_H
#define ICOMPONENT_H

class Entity;

class IComponent
{
public:
    Entity* m_pOwner = nullptr;

    virtual ~IComponent() {}
    virtual void Initialize() {}
    virtual void Update(const float deltaTime) {}
    virtual void Render() {}

    virtual Entity* GetOwner() const { return m_pOwner; }

    // return a name of the component (for example: "Component<TransformComponent>")
    virtual const char* GetName() const = 0;
};

#endif
