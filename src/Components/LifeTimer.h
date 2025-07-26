//===================================================================
// Filename:  LifeTimer.h
// Desc:      a component of the EC (Entity-Component) which
//            is used to define the entity lifetime since its creation
//
// Created:   14.07.2025  by DimaSkup
//===================================================================
#ifndef LIFE_TIMER_H
#define LIFE_TIMER_H

#include "../IComponent.h"
#include "../EventMgr.h"
#include "../EntityMgr.h"


class LifeTimer : public IComponent
{
public:

    //-----------------------------------------------------
    // Desc:  a constructor of the LifeTimer component
    // Args:  - lifeTimeDurationMs: time in ms how long the 
    //                              entity will live
    //-----------------------------------------------------
    LifeTimer(const int lifeTimeDurationMs) :
        m_FullLifeTimeMs((float)lifeTimeDurationMs),
        m_CurrLifeMs(0) 
    {
    }

    virtual ~LifeTimer()      {}
    virtual void Initialize() {}
    virtual void Render()     {}
    
    //-----------------------------------------------------
    // Desc:  update the entity current lifetime; if it is bigger
    //        than its full life time then we will destroy this entt
    // Args:  - deltaTime: the time (in sec) passed since the prev frame     
    //-----------------------------------------------------
    virtual void Update(const float deltaTime)
    {
        m_CurrLifeMs += (int)(ceil(deltaTime*1000.0f));        

        if (m_CurrLifeMs >= m_FullLifeTimeMs)
        {
            const EntityID id = GetOwner()->GetID();
            g_EventMgr.AddEvent(EventDestroyEntity(id));
        }
    }

    //-----------------------------------------------------
    // Desc:  get a name of this component
    //-----------------------------------------------------
    virtual const char* GetName() const override { return "LifeTimer (Component)"; }

private:
    int m_FullLifeTimeMs = 0;
    int m_CurrLifeMs = 0;   // the time spent since entity creation
};

#endif
