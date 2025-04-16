#ifndef SPRITE_COMPONENT_H
#define SPRITE_COMPONENT_H

#include "../Log.h"
#include "../IComponent.h"
#include "../AssetMgr.h"
#include "../Render.h"
#include "TransformComponent.h"
#include <SDL2/SDL.h>


class SpriteComponent : public IComponent 
{
public:
    SpriteComponent(const char* spriteFilePath)
    {
        if (!spriteFilePath || spriteFilePath[0] == '\0')
            LogErr(LOG_INFO, "invalid input file path to sprite");

        sprintf(g_String, "init sprite component with sprite path: %s", spriteFilePath);
        LogDbg(LOG_INFO, g_String);

        SetTexture(spriteFilePath);
    }

    ///////////////////////////////////////////////////////
    
    void SetTexture(const char* assetTextureID)
    {
        if (!assetTextureID || assetTextureID[0] == '\0')
        {
            LogErr(LOG_INFO, "input asset texture ID is empty");
            return;
        }

        m_pTexture = g_AssetMgr.GetTexture(assetTextureID); 
    }

    ///////////////////////////////////////////////////////

    virtual void Initialize() override
    {
        m_pTransform = m_pOwner->GetComponent<TransformComponent>();
        m_SrcRect.x = 0;
        m_SrcRect.y = 0;
        m_SrcRect.w = m_pTransform->m_Width;
        m_SrcRect.h = m_pTransform->m_Height;
    }

    ///////////////////////////////////////////////////////
    
    virtual void Update(const float deltaTime) override
    {
        m_DstRect.x = (int)m_pTransform->m_Position.x;
        m_DstRect.y = (int)m_pTransform->m_Position.y;
        m_DstRect.w = m_pTransform->m_Width  * m_pTransform->m_Scale;
        m_DstRect.h = m_pTransform->m_Height * m_pTransform->m_Scale;
    }

    ///////////////////////////////////////////////////////

    virtual void Render() override
    {
        Render::DrawRectTextured(m_pTexture, m_SrcRect, m_DstRect, m_SpriteFlip);
    }

    ///////////////////////////////////////////////////////

    virtual const char* GetName() const override { return "SpriteComponent"; }


public:
    SDL_RendererFlip m_SpriteFlip = SDL_FLIP_NONE;

private:
    TransformComponent* m_pTransform = nullptr;
    SDL_Texture* m_pTexture = nullptr;
    SDL_Rect m_SrcRect;
    SDL_Rect m_DstRect;
};

#endif
