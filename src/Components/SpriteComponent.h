#ifndef SPRITE_COMPONENT_H
#define SPRITE_COMPONENT_H

#include "../Log.h"
#include "../IComponent.h"
#include "../TextureMgr.h"
#include "../AssetMgr.h"
#include <SDL2/SDL.h>

class SpriteComponent : public IComponent 
{
public:
    SpriteComponent(const char* spriteFilePath)
    {
        if (!spriteFilePath || spriteFilePath[0] == '\0')
            LogErr("invalid input file path to sprite");

        SetTexture(spriteFilePath);
    }

    void SetTexture(const char* assetTextureID)
    {
        if (!assetTextureID || assetTextureID[0] == '\0')
        {
            LogErr("invalid input asset texture ID");
            return;
        }

        m_pTexture = g_AssetMgr->GetTexture(assetTextureID); 
    }

    ///////////////////////////////////////////////////////

    virtual void Initialize() override
    {
        m_pTransform = pOwner_->GetComponent<TransformComponent>();
    }
public:
    SDL_RendererFlip m_SpriteFlip = SDL_FLIP_NONE;

private:
    TransformComponent* m_pTransform = nullptr;
    SDL_Texture* m_pTexture = nullptr;
    SDL_Rect m_SrcRect;
    SDL_Rect m_DstRect;
};

#endif
