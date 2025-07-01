// ==================================================================
// Filename:    KeyboardComponent.h
// Description: a component to control entity position using keyboard
//
// Created:     19.04.2025 by DimaSkup
// ==================================================================
#ifndef KEYBOARD_CONTROL_H
#define KEYBOARD_CONTROL_H

#include "../EntityMgr.h"
#include "../GameState.h"
#include "../Game.h"
#include "../StrHelper.h"
#include "../IComponent.h"
#include "../EventMgr.h"
#include "../../lib/glm/glm.hpp"


class KeyboardControl : public IComponent
{
public:
    KeyboardControl();

    KeyboardControl(
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

    virtual ~KeyboardControl() {};

    ///////////////////////////////////////////////////////
    
    virtual void Initialize() override {}
    virtual void Render()     override {}
    
    ///////////////////////////////////////////////////////
    
    bool HandleKeysPressing()
    {
        // return: a flag to define if we pressed any key for moving
        
        const uint8_t* keys = SDL_GetKeyboardState(NULL);
        constexpr float speed = 400;
        bool isMoving = false;

        glm::vec2 velocity = { 0, 0 };
        const EntityID enttID = m_pOwner->GetID();

        if (keys[SDL_SCANCODE_UP])
        {
            velocity.y = -1;
            EventSwitchAnimation e(enttID, ANIMATION_TYPE_UP);
            g_EventMgr.AddEvent(e);
            isMoving = true;
        }
        if (keys[SDL_SCANCODE_DOWN])
        {
            velocity.y = 1;
            EventSwitchAnimation e(enttID, ANIMATION_TYPE_DOWN);
            g_EventMgr.AddEvent(e);
            isMoving = true;
        } 
        if (keys[SDL_SCANCODE_RIGHT])
        {
            velocity.x = 1;
            EventSwitchAnimation e(enttID, ANIMATION_TYPE_RIGHT);
            g_EventMgr.AddEvent(e);
            isMoving = true;
        }
        if (keys[SDL_SCANCODE_LEFT])
        {
            velocity.x = -1;
            EventSwitchAnimation e(enttID, ANIMATION_TYPE_LEFT);
            g_EventMgr.AddEvent(e);
            isMoving = true;
        }

        // normalize the velocity vector to prevent moving faster by diagonal
        // and add an event for player moving
        if (isMoving)
        {
            const float speed = g_GameStates.playerSpeed;
            velocity = glm::normalize(velocity) * speed;

            EventPlayerMove e(enttID, velocity.x, velocity.y);
            g_EventMgr.AddEvent(e);
        }

        return isMoving;
    }

    ///////////////////////////////////////////////////////
    
    void HandleKeysReleasing()
    {
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
                    const EntityID id = m_pOwner->GetID();
                    g_EventMgr.AddEvent(EventPlayerStop(id));
                    break;
                }
            }
        }
    }
    
    ///////////////////////////////////////////////////////

    void Update(const float deltaTime) override
    {
        // update the sprite velocity and animation 
        // based on the keyboard input
        HandleKeysPressing();
        HandleKeysReleasing();
    }
    
    ///////////////////////////////////////////////////////
    
    virtual const char* GetName() const { return "KeyboardControlComponent"; }
    

private:
    SDL_Keycode m_UpKey = 0;
    SDL_Keycode m_DownKey = 0;
    SDL_Keycode m_RightKey = 0;
    SDL_Keycode m_LeftKey = 0;
    SDL_Keycode m_ShootKey = 0;
};

#endif
