#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include "../EntityMgr.h"
#include "../../lib/glm/glm.hpp"
#include "../Game.h"
#include <SDL2/SDL.h>

class TransformComponent : public Component
{
public:
    TransformComponent(
        const int posX, 
        const int posY, 
        const int velX, 
        const int velY, 
        const int width, 
        const int height, 
        const int scale) 
        :
        position_(posX, posY),
        velocity_(velX, velY),
        width_(width),
        height_(height),
        scale_(scale) {}

    void Initialize() override {}

    void Update(const float deltaTime) override
    {
        // update the position/velocity as a function of deltaTime
        position += (velocity * deltaTime);
    }

    void Render() override 
    {
        SDL_Rect enttRect = 
        {
            (int)position.x,
            (int)position.y,
            width,
            height
        };

        SDL_SetRenderDrawColor(Game::pRenderer_, 255, 255, 255, 255);
        SDL_RenderFillRect(Game::pRenderer_, &enttRect);
    }

public:
    glm::vec2 position_;
    glm::vec2 velocity_;
    int width_ = 0;
    int height_ = 0;
    int scale_ = 0;
}

#endif
