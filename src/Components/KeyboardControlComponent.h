// ==================================================================
// Filename:    KeyboardControlComponent.h
// Description: component to control entity position using keyboard
//
// Created:     19.04.2025 by DimaSkup
// ==================================================================
#ifndef KEYBOARD_CONTROL_COMPONENT_H
#define KEYBOARD_CONTROL_COMPONENT_H

#include "../StrHelper.h"
#include "../IComponent.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"


class KeyboardControlComponent : public IComponent
{
public:
    KeyboardControlComponent();

    KeyboardControlComponent(
        const char* upKey,
        const char* rightKey,
        const char* downKey,
        const char* leftKey,
        const char* shootKey)
    {
        if (upKey && strcmp(upKey, "up") == 0)
            m_UpKey = SDL_GetKeyFromName(upKey);
        
        if (rightKey && strcmp(rightKey, "right") == 0)
            m_RightKey = SDL_GetKeyFromName(rightKey);

        if (downKey && strcmp(downKey, "down") == 0)
            m_DownKey = SDL_GetKeyFromName(downKey);

        if (leftKey && strcmp(leftKey, "left") == 0)
            m_LeftKey = SDL_GetKeyFromName(leftKey);
        
        if (shootKey && strcmp(shootKey, "space") == 0)
            m_ShootKey = SDL_GetKeyFromName(shootKey);
    }

    ///////////////////////////////////////////////////////
    
    void Initialize() override
    {
        m_pTransform = m_pOwner->GetComponent<TransformComponent>();
        m_pSprite = m_pOwner->GetComponent<SpriteComponent>();
    }
    
    ///////////////////////////////////////////////////////
    
    void Update(const float deltaTime) override
    {
        // update the sprite velocity and animation 
        // based on the keyboard input

        const uint8_t* keys = SDL_GetKeyboardState(NULL);

        if (keys[SDL_SCANCODE_UP])
        {
            m_pTransform->m_Velocity.x = 0;
            m_pTransform->m_Velocity.y = -50;
            m_pSprite->Play("UpAnimation");
        }
        if (keys[SDL_SCANCODE_DOWN])
        {
            m_pTransform->m_Velocity.x = 0;
            m_pTransform->m_Velocity.y = 50;
            m_pSprite->Play("DownAnimation");
        }
        
        if (keys[SDL_SCANCODE_RIGHT])
        {
            m_pTransform->m_Velocity.x = 50;
            m_pTransform->m_Velocity.y = 0;
            m_pSprite->Play("RightAnimation");
        }
        if (keys[SDL_SCANCODE_LEFT])
        {
            m_pTransform->m_Velocity.x = -50;
            m_pTransform->m_Velocity.y = 0;
            m_pSprite->Play("LeftAnimation");
        }

        // if we released some key we set velocity to 0
        if(Game::ms_Event.type == SDL_KEYUP)
        {
            switch(Game::ms_Event.key.keysym.sym)
            {
                case SDLK_UP:
                case SDLK_DOWN:
                case SDLK_RIGHT:
                case SDLK_LEFT:
                {
                    m_pTransform->m_Velocity.x = 0;
                    m_pTransform->m_Velocity.y = 0;
                    break;
                }

            }
        }
    }
    
    ///////////////////////////////////////////////////////
    
    virtual void Render() override {}
    
    ///////////////////////////////////////////////////////
    
    virtual const char* GetName() const { return "KeyboardControlComponent"; }
    
    ///////////////////////////////////////////////////////

private:
    SDL_Keycode m_UpKey = 0;
    SDL_Keycode m_DownKey = 0;
    SDL_Keycode m_RightKey = 0;
    SDL_Keycode m_LeftKey = 0;
    SDL_Keycode m_ShootKey = 0;

    TransformComponent* m_pTransform = nullptr;
    SpriteComponent*    m_pSprite = nullptr;
};

#endif
