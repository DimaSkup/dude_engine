// ==================================================================
// Filename:    Sprite.h
// Description: a component of EC (Entity-Component)
//              for sprites and sprites animations
//
// Created:     04.2025 by DimaSkup
// ==================================================================
#ifndef SPRITE_H
#define SPRITE_H

#include "../GameState.h"
#include "../Types.h"
#include "../StrHelper.h"
#include "../Log.h"
#include "../IComponent.h"
#include "../AssetMgr.h"
#include "../Render.h"
#include "../Animation.h"
#include "Transform.h"
#include <SDL2/SDL.h>

class Sprite : public IComponent 
{
public:

    Sprite(const char* assetTextureID) :
        m_IsAnimated(false),
        m_IsFixed(false)
    {
        // a constructor for static (not animated sprites)

        if (IsStrEmpty(assetTextureID))
            LogErr(LOG_INFO, "input asset texture ID is empty");

        SetTexture(assetTextureID);
    }

    ///////////////////////////////////////////////////////
    
    Sprite(
        const char* assetTextureID, 
        const uint numFrames,
        const uint animationSpeed,
        const bool hasDirections,
        const bool isFixed)
        :
        m_NumFrames(numFrames),
        m_AnimationSpeed(animationSpeed),
        m_IsAnimated(true),
        m_IsFixed(isFixed)
    {
        // a constructor for animated sprites

        if (IsStrEmpty(assetTextureID))
            LogErr("input asset texture ID is empty");

        if (hasDirections)
        {
            // separate animation data container for each direction
            Animation down(0, numFrames, animationSpeed);
            Animation right(1, numFrames, animationSpeed);
            Animation left(2, numFrames, animationSpeed);
            Animation up(3, numFrames, animationSpeed);

            // add animations into map to be able to switch btw them
            m_Animations.emplace("DownAnimation", down);
            m_Animations.emplace("RightAnimation", right);
            m_Animations.emplace("LeftAnimation", left);
            m_Animations.emplace("UpAnimation", up);
            
            m_AnimationIdx = 0;
            m_CurrAnimationName = "DownAnimation";
        }
        else
        {
            Animation singleAnimation(0, numFrames, animationSpeed);

            const char* animKey = "SingleAnimation";
            const auto& res = m_Animations.emplace(animKey, singleAnimation);
            if (!res.second)
                LogErr("didn't manage to add a single animation into the map of animations by key: %s", animKey);

            m_AnimationIdx = 0;
            m_CurrAnimationName = animKey;
        }

        SetTexture(assetTextureID);
        Play(m_CurrAnimationName);
    }

    ///////////////////////////////////////////////////////

    virtual ~Sprite() {}
    
    ///////////////////////////////////////////////////////

    void Play(const std::string& animationName)
    {
        const Animation& anim = m_Animations[animationName];

        m_NumFrames         = anim.m_NumFrames;
        m_AnimationIdx      = anim.m_Idx; 
        m_AnimationSpeed    = anim.m_AnimationSpeed;
        m_CurrAnimationName = animationName; 
    }

    ///////////////////////////////////////////////////////
    
    void SetTexture(const char* assetTextureID)
    {
        if (IsStrEmpty(assetTextureID))
        {
            LogErr(LOG_INFO, "input asset texture ID is empty");
            return;
        }

        m_pTexture = g_AssetMgr.GetTexture(assetTextureID); 
    }

    ///////////////////////////////////////////////////////

    inline const SDL_Rect& GetSrcRect() const { return m_SrcRect; }
    inline const SDL_Rect& GetDstRect() const { return m_DstRect; }


    // ==============================================================
    // Virtual methods
    // ==============================================================
    virtual void Initialize() override
    {
        m_pTransform = m_pOwner->GetComponent<Transform>();
        m_SrcRect.x = 0;
        m_SrcRect.y = 0;
        m_SrcRect.w = m_pTransform->m_Width;
        m_SrcRect.h = m_pTransform->m_Height;
    }

    ///////////////////////////////////////////////////////
    
    virtual void Update(const float deltaTime) override
    {
        // update the animation
        if (m_IsAnimated)
        {
            m_SrcRect.x = m_SrcRect.w * (int)((SDL_GetTicks() / m_AnimationSpeed) % m_NumFrames);
        }
        m_SrcRect.y = m_AnimationIdx * m_pTransform->m_Height;

        // update the position onto the screen
        m_DstRect.x = (int)(m_pTransform->m_Position.x - g_GameStates.cameraPosX * !m_IsFixed);
        m_DstRect.y = (int)(m_pTransform->m_Position.y - g_GameStates.cameraPosY * !m_IsFixed);

        m_DstRect.w = m_pTransform->m_Width  * m_pTransform->m_Scale;
        m_DstRect.h = m_pTransform->m_Height * m_pTransform->m_Scale;
    }

    ///////////////////////////////////////////////////////

    virtual void Render() override
    {
        Render::DrawRectTextured(m_pTexture, m_SrcRect, m_DstRect, m_SpriteFlip);
    }

    ///////////////////////////////////////////////////////

    virtual const char* GetName() const override 
    { 
        return "Sprite (Component)"; 
    }



public:
    SDL_RendererFlip m_SpriteFlip = SDL_FLIP_NONE;

private:
    Transform*      m_pTransform     = nullptr;
    SDL_Texture*    m_pTexture       = nullptr;
    SDL_Rect        m_SrcRect;
    SDL_Rect        m_DstRect;

    int             m_NumFrames      = 0;
    int             m_AnimationSpeed = 0;
    uint            m_AnimationIdx   = 0;
    
    bool            m_IsAnimated     = false;     
    bool            m_IsFixed        = false;   // is always fixed at the same screen position

    std::string     m_CurrAnimationName;

    std::map<std::string, Animation>    m_Animations;  // key => animation_info
};

#endif
