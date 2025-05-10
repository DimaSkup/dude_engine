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
        m_Collider{ x, y, width, height }
    {
    }

    virtual ~Collider() {}

    ///////////////////////////////////////////////////////

    virtual void Initialize() override
    {
        //if (m_pOwner->HasComponent<Transform>())
        //{
            m_pTransform = m_pOwner->GetComponent<Transform>();
            m_SrcRect = {0, 0, m_pTransform->m_Width, m_pTransform->m_Height};
            m_DstRect = m_Collider;
        //}
        //else
        //{
            //sprintf(g_String, "can't init collider because the entity (name: %s) doesn't have the Transform component", m_pOwner->m_Name);
            //LogErr(LOG_INFO, g_String);
        //}
    }

    ///////////////////////////////////////////////////////
   
    virtual void Update(const float deltaTime) override
    {
        Transform* pT = m_pTransform;
        m_Collider.x = (int)pT->m_Position.x;
        m_Collider.y = (int)pT->m_Position.y;
        m_Collider.w = pT->m_Width * pT->m_Scale;
        m_Collider.h = pT->m_Height * pT->m_Scale;

        m_DstRect.x = m_Collider.x - g_GameStates.cameraPosX;
        m_DstRect.y = m_Collider.y - g_GameStates.cameraPosY;
    }

    virtual const char* GetName() const override { return "Collider (Component)"; }

public:
    eColliderTag m_ColliderTag;
    SDL_Rect     m_Collider;
    SDL_Rect     m_SrcRect;
    SDL_Rect     m_DstRect;
    Transform*   m_pTransform = nullptr;

};

#endif

