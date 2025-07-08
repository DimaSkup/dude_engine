// ==================================================================
// Filename:    Transform.h
// Description: a component of the EC (Entity-Component) which
//              contains and handles data of entity: 
//              1. position 
//              2. velocity
//              3. width and height
//              4. scale
//
// Created:     15.04.2025 by DimaSkup
// ==================================================================
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "../GameState.h"
#include "../EventMgr.h"
#include "../EntityMgr.h"
#include "../Game.h"
#include "../IComponent.h"
#include "../Render.h"
#include "../../lib/glm/glm.hpp"


class Transform : public IComponent
{
public:
    Transform(
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

    virtual ~Transform() {}

    ///////////////////////////////////////////////////////

    virtual void Initialize() override {}
    virtual void Render()     override {}

    ///////////////////////////////////////////////////////////

    void ClampPosition(const float deltaTime)
    {
        // if we are moving we need to clamp the position of the 
        // main player to the visible width & height of the window

        glm::vec2& pos   = m_Position;
        glm::vec2& vel   = m_Velocity;

        const int width  = m_Width * m_Scale;
        const int height = m_Height * m_Scale;
        const int halfW  = width  >> 1;
        const int halfH  = height >> 1;

        // compute the player's next position
        const float nextPosX = pos.x + (vel.x * deltaTime);
        const float nextPosY = pos.y + (vel.y * deltaTime);

        const float maxX = g_GameStates.levelMapWidth;
        const float maxY = g_GameStates.levelMapHeight;

        // check if we touch the window left/right edge
        if (nextPosX <= 0)
        {
            pos = {1, pos.y};
        }
        else if ((nextPosX + width) >= maxX)
        {
            pos = { maxX-width-1, pos.y };
        }

        // check if we touch the window top/bottom edge
        if ((nextPosY - height) <= 0)
        {
            pos = { pos.x, 1 };
        }
        else if ((nextPosY + height) >= maxY)
        {
            pos = { pos.x, maxY-height-1 };
        }
    }


    virtual void Update(const float deltaTime) override
    {
        // update the position/velocity as a function of deltaTime
        m_Position += (m_Velocity * deltaTime);

        ClampPosition(deltaTime);
    }

    ///////////////////////////////////////////////////////////

    virtual const char* GetName() const override { return "Transform (Component)"; }

    ///////////////////////////////////////////////////////////

    //-----------------------------------------------------
    // Desc:  setup the velocity value
    // Args:  - velX, velY: velocity value by X and Y
    //-----------------------------------------------------
    inline void SetVelocity(const float velX, const float velY)
    {   m_Velocity = { velX, velY };    }

    inline void SetWidth(const float w)
    {   if (w > 0) m_Width = w;    }

    inline void SetHeight(const float h)
    {   if (h > 0) m_Height = h;    }

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
