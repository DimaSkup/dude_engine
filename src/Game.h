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

    inline bool IsRunning() const { return m_IsRunning; }


public:
    static SDL_Event ms_Event;

private:
    bool             m_IsRunning = true;
    int              m_TicksLastFrame = 0;
};

#endif
