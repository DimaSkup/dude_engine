#ifndef COMPONENT_H
#define COMPONENT_H

class Entity;

class Component
{
public:
    Entity* pOwner_ = nullptr;

    virtual ~Component() {}
    virtual void Initialize() {}
    virtual void Update(const float deltaTime) {}
    virtual void Render() {}
};

#endif
