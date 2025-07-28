// ==================================================================
// Filename:    Game.h
// Description: main class for the game 
//              init/process input/update/render/etc. 
//
// Created:     07.04.2025 by DimaSkup
// ==================================================================
#ifndef GAME_H
#define GAME_H

#include "Entity.h"
#include <SDL2/SDL.h>
#include "../lib/lua/sol.hpp"


class Game
{
public:
    Game();
    ~Game();
    
    void Initialize();
    void ProcessInput();

    void Update();
    void Destroy();

    void Render();
    void RenderFont();

    void LoadLevel(const int levelNumber);
    void HandleCameraMovement();
    void CheckCollisions();

    void ProcessNextLevel(const int levelNumber);
    void ProcessGameOver();
    inline bool IsRunning() const { return m_Running; }

private:
    void RenderColliderAABB() const;

    void HandleEventPlayerShoot(Entity& player);
    void CreateExplosion(Entity& enemy);
    void PlaySound(const char* soundName);

public:
    static SDL_Event ms_Event;
    static SDL_Rect  ms_Camera;
private:
    bool             m_Running        = true;
    bool             m_ShowAABB       = false;
    bool             m_ShowHelpScreen = true;
    bool             m_PlayerIsKilled = false;
    uint32_t         m_PrevTicks      = 0;
    uint32_t         m_FpsCounter     = 0;
    uint32_t         m_FramesDrawn    = 0;
    float            m_FpsValue       = 0;
};

#endif
