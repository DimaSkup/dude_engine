// ==================================================================
// Filename:    Collider.h
// Description: a component of EC (Entity-Component) to handle 
//              collision of entities
//
// Created:     08.05.2025 by DimaSkup
// ==================================================================
#ifndef COLLIDER_H
#define COLLIDER_H

#include "../Types.h"
#include "../Log.h"
#include "../GameState.h"
#include "../IComponent.h"
#include "Transform.h"
#include <SDL2/SDL.h>


class Collider : public IComponent
{
public:
    Collider(
        const eColliderTag colliderTag, 
        const int x,
        const int y,
        const int width,
        const int height)
        :
        m_ColliderTag{ colliderTag },
        m_ColliderRect{ x, y, width, height }
    {
    }

    virtual ~Collider() {}

    ///////////////////////////////////////////////////////

    virtual void Initialize() override
    {
        if (m_pOwner->HasComponent<Transform>())
        {
            m_pTransform = m_pOwner->GetComponent<Transform>();
            m_SrcRect = {0, 0, m_pTransform->m_Width, m_pTransform->m_Height};
            m_DstRect = m_ColliderRect;
        }
        else
        {
            LogErr(LOG, "can't init collider because the entity (name: %s) doesn't have the Transform component", m_pOwner->m_Name);
        }
    }

    ///////////////////////////////////////////////////////
   
    virtual void Update(const float deltaTime) override
    {
        Transform* pT = m_pTransform;
        m_ColliderRect.x = (int)pT->m_Position.x;
        m_ColliderRect.y = (int)pT->m_Position.y;
        m_ColliderRect.w = pT->m_Width * pT->m_Scale;
        m_ColliderRect.h = pT->m_Height * pT->m_Scale;

        m_DstRect.x = m_ColliderRect.x - g_GameStates.cameraPosX;
        m_DstRect.y = m_ColliderRect.y - g_GameStates.cameraPosY;
    }

    ///////////////////////////////////////////////////////

    virtual const char* GetName() const override 
    { 
        return "Collider (Component)"; 
    }

public:
    eColliderTag m_ColliderTag;
    SDL_Rect     m_ColliderRect;
    SDL_Rect     m_SrcRect;
    SDL_Rect     m_DstRect;
    Transform*   m_pTransform = nullptr;

};

#endif

