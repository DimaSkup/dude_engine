// ==================================================================
// Filename:    Game.h
// Description: 
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
    
    void Initialize(const int wndWidth, const int wndHeight);
    void ProcessInput();
    void Update();
    void Render();
    void Destroy();

    inline bool IsRunning() const { return isRunning_; }

private:
    bool isRunning_ = true;
    int  ticksLastFrame_ = 0;

    SDL_Window*   pWindow_   = nullptr;
    SDL_Renderer* pRenderer_ = nullptr;
};

#endif
