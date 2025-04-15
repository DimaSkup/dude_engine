// ==================================================================
// Filename:    TransformComponent.h
// Description: a component of the EC (Entity-Component) which
//              contains and handles data of entity: 
//              1. position 
//              2. velocity
//              3. width and height
//              4. scale
//
// Created:     15.04.2025 by DimaSkup
// ==================================================================
#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include "../IComponent.h"
#include "../Render.h"
#include "../../lib/glm/glm.hpp"


class TransformComponent : public IComponent
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

    virtual ~TransformComponent() {}

    ///////////////////////////////////////////////////////

    virtual void Initialize() override {}

    ///////////////////////////////////////////////////////////

    virtual void Update(const float deltaTime) override
    {
        // update the position/velocity as a function of deltaTime
        position_ += (velocity_ * deltaTime);
    }

    ///////////////////////////////////////////////////////////

    virtual void Render() override
    {
        SDL_Rect enttRect = 
        {
            (int)position_.x,
            (int)position_.y,
            width_,
            height_
        };

        SDL_SetRenderDrawColor(g_pRenderer, 255, 255, 255, 255);
        SDL_RenderFillRect(g_pRenderer, &enttRect);
    }

    virtual const char* GetName() const override { return "Component<TransformComponent>"; }

public:
    glm::vec2 position_;
    glm::vec2 velocity_;
    int width_ = 0;
    int height_ = 0;
    int scale_ = 0;

    char componentName_[48]{'\0'};
};

#endif
