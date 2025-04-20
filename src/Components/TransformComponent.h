// ==================================================================
// Filename:    TransformComponent.h
// Description: a component of the EC (Entity-Component) which
//              contains and handles data of entity: 
//              1. position 
//              2. velocity
//              3. width and height
//              4. scale
//
// Created:     15.04.2025 by DimaSkup
// ==================================================================
#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include "../IComponent.h"
#include "../Render.h"
#include "../../lib/glm/glm.hpp"


class TransformComponent : public IComponent
{
public:
    TransformComponent(
        const int posX,  
        const int posY, 
        const int velX,  
        const int velY, 
        const int width, 
        const int height, 
        const int scale)
        :
        m_Position(posX, posY),
        m_Velocity(velX, velY),
        m_Width(width),
        m_Height(height),
        m_Scale(scale) {}

    virtual ~TransformComponent() {}

    ///////////////////////////////////////////////////////

    virtual void Initialize() override {}
    virtual void Render() override {}

    ///////////////////////////////////////////////////////////

    virtual void Update(const float deltaTime) override
    {
        // update the position/velocity as a function of deltaTime
        m_Position += (m_Velocity * deltaTime);
    }

    ///////////////////////////////////////////////////////////

    virtual const char* GetName() const override { return "TransformComponent"; }

    ///////////////////////////////////////////////////////////

    inline glm::vec2 GetPosition() const { return m_Position; }
    inline glm::vec2 GetVelocity() const { return m_Velocity; }

    inline int GetOrigWidth()      const { return m_Width; }
    inline int GetOrigHeight()     const { return m_Height; }
    inline int GetWidth()          const { return m_Width * m_Scale; }
    inline int GetHeight()         const { return m_Height * m_Scale; }    

public:
    glm::vec2 m_Position;
    glm::vec2 m_Velocity;
    int m_Width = 0;
    int m_Height = 0;
    int m_Scale = 0;
};

#endif
