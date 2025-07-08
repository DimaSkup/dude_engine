#ifndef PROJECTILE_EMMITER_COMPONENT_H
#define PROJECTILE_EMMITER_COMPONENT_H

#include "../Log.h"
#include "../IComponent.h"
#include "../EntityMgr.h"
#include "../EventMgr.h"
#include "Transform.h"
#include "../../lib/glm/glm.hpp"

class ProjectileEmmiter : public IComponent
{
public:
    ProjectileEmmiter(
        const int speed,
        const int angleDeg,
        const int range, 
        const bool loop) 
        :
        m_Speed(speed),
        m_Range(range),
        m_AngleRad(glm::radians((float)angleDeg)),
        m_IsLoop(loop)
    {
    }

    virtual ~ProjectileEmmiter()
    {
    }

    ///////////////////////////////////////////////////////

    virtual void Initialize() override
    {
        m_pTransform = m_pOwner->GetComponent<Transform>();
        m_Origin = m_pTransform->m_Position;

        // compute velocity vector
        const float velocityX = cosf(m_AngleRad) * m_Speed;
        const float velocityY = sinf(m_AngleRad) * m_Speed;
        m_pTransform->m_Velocity = glm::vec2(velocityX, velocityY); 
    }

    ///////////////////////////////////////////////////////

    virtual void Update(const float deltaTime) override
    {
        const glm::vec2 distanceVec = m_pTransform->m_Position - m_Origin;
        const int distanceSqr       = (int)glm::dot(distanceVec, distanceVec);
        const int rangeSqr          = m_Range*m_Range; 

        // if we need to destroy/move this projectile
        if (distanceSqr > rangeSqr)
        {
            if (m_IsLoop)
                m_pTransform->m_Position = m_Origin;

            else
            {
                const EntityID id = m_pOwner->GetID();
                g_EventMgr.AddEvent(EventDestroyEntity(id));
            }
        }
    }

    ///////////////////////////////////////////////////////

    virtual const char* GetName() const { return "ProjectileEmmiter (Component)"; }

private:
    Transform* m_pTransform = nullptr;
    glm::vec2  m_Origin;               // position where appears
    int        m_Speed      = 0;       // speed in pixels
    int        m_Range      = 0;       // range in pixels where projectile is destroyed automatically
    float      m_AngleRad   = 0;       // angle in radians
    bool       m_IsLoop     = false;   // should emmiting repeat?
};

#endif
