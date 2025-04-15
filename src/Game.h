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
    
    void Initialize();
    void ProcessInput();
    void Update();
    void Render();
    void Destroy();
    void LoadLevel(const int levelNumber);

    inline bool IsRunning() const { return isRunning_; }

private:
    bool isRunning_ = true;
    int  ticksLastFrame_ = 0;
};

#endif
