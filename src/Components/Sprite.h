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
#include <stdexcept>


struct SpriteInitParams
{
    const uint numFrames        = 0;
    const uint animationSpeed   = 0;
    const bool hasDirections    = false;
    const bool isFixed          = true;
};

//---------------------------------------------------------

class Sprite : public IComponent 
{
public:

    Sprite(const char* assetTexId, const SpriteInitParams& params) :
        m_NumFrames(params.numFrames),
        m_AnimationSpeed(params.animationSpeed),
        m_IsAnimated(true),
        m_IsFixed(params.isFixed)
    {
        // a constructor for animated sprites

        if (IsStrEmpty(assetTexId))
            LogErr(LOG, "input asset texture ID is empty");

        if (params.hasDirections)
        {
            // separate animation data container for each direction
            Animation down (0, m_NumFrames, m_AnimationSpeed);
            Animation right(1, m_NumFrames, m_AnimationSpeed);
            Animation left (2, m_NumFrames, m_AnimationSpeed);
            Animation up   (3, m_NumFrames, m_AnimationSpeed);

            // add animations into map to be able to switch btw them
            m_Animations.insert({ ANIMATION_TYPE_DOWN,  down  });
            m_Animations.insert({ ANIMATION_TYPE_RIGHT, right });
            m_Animations.insert({ ANIMATION_TYPE_LEFT,  left  });
            m_Animations.insert({ ANIMATION_TYPE_UP,    up    });
            
            m_AnimationIdx = 0;
            m_CurrAnimationType = ANIMATION_TYPE_RIGHT;
        }
        else
        {
            Animation singleAnimation(0, params.numFrames, params.animationSpeed);

            const auto& res = m_Animations.insert({ ANIMATION_TYPE_SINGLE, singleAnimation });
            if (!res.second)
                LogErr(LOG, "didn't manage to add a single animation into the map of animations");

            m_AnimationIdx = 0;
            m_CurrAnimationType = ANIMATION_TYPE_SINGLE;
        }

        SetTexture(assetTexId);
        Play(m_CurrAnimationType);

    }

    //-----------------------------------------------------
    // Desc:   a constructor for fixed, not animated sprite
    // Args:   - assetTexId:  name of the texture asset
    //-----------------------------------------------------
    Sprite(const char* assetTexId) :
        m_IsAnimated(false),
        m_IsFixed(false)
    {
        // a constructor for static (not animated sprites)

        if (IsStrEmpty(assetTexId))
            LogErr(LOG, "input asset texture ID is empty");

        SetTexture(assetTexId);
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
            LogErr(LOG, "input asset texture ID is empty");

        if (hasDirections)
        {
            // separate animation data container for each direction
            Animation down (0, numFrames, animationSpeed);
            Animation right(1, numFrames, animationSpeed);
            Animation left (2, numFrames, animationSpeed);
            Animation up   (3, numFrames, animationSpeed);

            // add animations into map to be able to switch btw them
            m_Animations.insert({ ANIMATION_TYPE_DOWN,  down  });
            m_Animations.insert({ ANIMATION_TYPE_RIGHT, right });
            m_Animations.insert({ ANIMATION_TYPE_LEFT,  left  });
            m_Animations.insert({ ANIMATION_TYPE_UP,    up    });
            
            m_AnimationIdx = 0;
            m_CurrAnimationType = ANIMATION_TYPE_RIGHT;
        }
        else
        {
            Animation singleAnimation(0, numFrames, animationSpeed);

            //const char* animKey = "SingleAnimation";

            const auto& res = m_Animations.insert({ ANIMATION_TYPE_SINGLE, singleAnimation });
            if (!res.second)
                LogErr(LOG, "didn't manage to add a single animation into the map of animations");

            m_AnimationIdx = 0;
            m_CurrAnimationType = ANIMATION_TYPE_SINGLE;
        }

        SetTexture(assetTextureID);
        Play(m_CurrAnimationType);
    }

    ///////////////////////////////////////////////////////

    virtual ~Sprite() 
    {
        m_Animations.clear();
        m_pTransform = nullptr;
        m_pTexture = nullptr;
    }
    
    //-----------------------------------------------------
    // Desc:  get the current animation type
    //        (using example: to define the player's direction)
    // Ret:   animation type value
    //-----------------------------------------------------
    inline eAnimationType GetCurrAnimationType() const
    {    return m_CurrAnimationType;    }

    // ----------------------------------------------------
    // Desc:  switch to the animation of input type 
    //        (if we have any data for it)
    // Args:  - type: animation type to switch to
    // ----------------------------------------------------
    void Play(const eAnimationType type)
    {
#if 1
        if (m_Animations.empty())
        {
            LogErr(LOG, "std::map of animations is empty!");
            return;
        }

        const auto& it = m_Animations.find(type);

        if (it != m_Animations.end())
        {
            const Animation& anim = it->second;

            m_NumFrames         = anim.m_NumFrames;
            m_AnimationIdx      = anim.m_Idx; 
            m_AnimationSpeed    = anim.m_AnimationSpeed;
            m_CurrAnimationType = type; 
        }
        else
        {
            LogErr(LOG, "there is no animation by type: %d", (int)type);
        }
    }

    ///////////////////////////////////////////////////////
    
    void SetTexture(const char* assetTextureID)
    {
        if (IsStrEmpty(assetTextureID))
        {
            LogErr(LOG, "input asset texture ID is empty");
            return;
        }

        m_pTexture = g_AssetMgr.GetTexture(assetTextureID); 

        // check if we got a valid texture
        if (m_pTexture == nullptr)
        {
            LogErr(LOG, "there is no texture is asset mgr by ID: %s", assetTextureID);
        }
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
        m_AnimationTime += (deltaTime * 1000.0f);

        // update the animation
        if (m_IsAnimated)
        {
            m_SrcRect.x = m_SrcRect.w * (int)(((int)m_AnimationTime / m_AnimationSpeed) % m_NumFrames);
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
    float           m_AnimationTime  = 0;       // time since the start of the animation
    
    bool            m_IsAnimated     = false;     
    bool            m_IsFixed        = false;   // is always fixed at the same screen position

    eAnimationType  m_CurrAnimationType = ANIMATION_TYPE_SINGLE;
    //std::string     m_CurrAnimationName;

    std::map<eAnimationType, Animation> m_Animations;
//    std::map<std::string, Animation>    m_Animations;  // key => animation_info
};

#endif
