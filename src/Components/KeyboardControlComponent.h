// ==================================================================
// Filename:    KeyboardControlComponent.h
// Description: component to control entity position using keyboard
//
// Created:     19.04.2025 by DimaSkup
// ==================================================================
#ifndef KEYBOARD_CONTROL_COMPONENT_H
#define KEYBOARD_CONTROL_COMPONENT_H

#include "../GameState.h"
#include "../StrHelper.h"
#include "../Render.h"
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

    virtual ~KeyboardControlComponent() {};

    ///////////////////////////////////////////////////////
    
    virtual void Initialize() override
    {
        m_pTransform = m_pOwner->GetComponent<TransformComponent>();
        m_pSprite = m_pOwner->GetComponent<SpriteComponent>();
    }
    
    ///////////////////////////////////////////////////////
    
    bool HandleKeysPressing()
    {
        // return: a flag to define if we pressed any key for moving
        
        const uint8_t* keys = SDL_GetKeyboardState(NULL);
        constexpr float speed = 400;
        bool isMoving = false;

        glm::vec2 velocity = { 0, 0 };

        if (keys[SDL_SCANCODE_UP])
        {
            velocity.y = -1;
            m_pSprite->Play("UpAnimation");
            isMoving = true;
        }
        if (keys[SDL_SCANCODE_DOWN])
        {
            velocity.y = 1;
            m_pSprite->Play("DownAnimation");
            isMoving = true;
        } 
        if (keys[SDL_SCANCODE_RIGHT])
        {
            velocity.x = 1;
            m_pSprite->Play("RightAnimation");
            isMoving = true;
        }
        if (keys[SDL_SCANCODE_LEFT])
        {
            velocity.x = -1;
            m_pSprite->Play("LeftAnimation");
            isMoving = true;
        }

        // normalize the direction to prevent moving faster by diagonal
        if (isMoving)
        {
            m_pTransform->m_Velocity = glm::normalize(velocity) * speed;
        }

        return isMoving;
    }

    ///////////////////////////////////////////////////////

    void ClampPosition(const float deltaTime)
    {
        // if we are moving we need to clamp the position of the 
        // main player to the visible width & height of the window

        const glm::vec2 playerPos  = m_pTransform->GetPosition();
        const glm::vec2 playerVel  = m_pTransform->GetVelocity();
        const int halfPlayerW      = m_pTransform->GetWidth() >> 1;
        const int halfPlayerH      = m_pTransform->GetHeight() >> 1;

        // compute the player's next position
        const float nextPosX = playerPos.x + (playerVel.x * deltaTime);
        const float nextPosY = playerPos.y + (playerVel.y * deltaTime);

        // check if we touch the window left/right edge
        if ((nextPosX - halfPlayerW) <= 0)
        {
            m_pTransform->m_Velocity.x = 0;
        }
        else if ((nextPosX + halfPlayerW) >= g_GameStates.levelMapWidth)
        { 
            m_pTransform->m_Velocity.x = 0;
        }

        // check if we touch the window top/bottom edge
        if ((nextPosY - halfPlayerH) <= 0)
        {
            m_pTransform->m_Velocity.y = 0;
        }
        else if ((nextPosY + halfPlayerH) >= g_GameStates.levelMapHeight)
        {
            m_pTransform->m_Velocity.y = 0;
        }
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
                    m_pTransform->m_Velocity.x = 0;
                    m_pTransform->m_Velocity.y = 0;
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

        bool isMoving = HandleKeysPressing();

        if (isMoving)
            ClampPosition(deltaTime);

        HandleKeysReleasing();
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
