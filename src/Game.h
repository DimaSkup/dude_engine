// ==================================================================
// Filename:    Game.h
// Description: main class for the game 
//              init/process input/update/render/etc. 
//
// Created:     07.04.2025 by DimaSkup
// ==================================================================
#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>


class Game
{
public:
    Game();
    ~Game();
    
    void Initialize();
    void ProcessInput();

    void Update();
    void Render();
    void Destroy();

    void LoadLevel(const int levelNumber);
    void HandleCameraMovement();
    void CheckCollisions();

    inline bool IsRunning() const { return m_Running; }

private:
    void RenderColliderAABB() const;

public:
    static SDL_Event ms_Event;
    static SDL_Rect  ms_Camera;
private:
    bool             m_Running = true;
    bool             m_ShowAABB = false;
    int              m_TicksLastFrame = 0;
};

#endif
