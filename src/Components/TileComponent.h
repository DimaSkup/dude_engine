#ifndef TILE_COMPONENT_H
#define TILE_COMPONENT_H

#include "../IComponent.h"
#include "../Render.h"
#include "../AssetMgr.h"
#include <SDL2/SDL.h>
#include "../../lib/glm/glm.hpp"


class TileComponent : public IComponent
{
public:
    TileComponent(
        const int srcRectX,         // X-position of tile on tile texture 
        const int srcRectY,         // Y-position of tile on tile texture
        const int x,                // X-position of tile on the screen
        const int y,                // Y-position of tile on the screen
        const int tileSize, 
        const int tileScale, 
        const char* assetTextureID)
        :
        m_SrcRect {srcRectX, srcRectY, tileSize, tileSize},
        m_DstRect {x, y, tileSize * tileScale, tileSize * tileScale},
        m_Position {x, y}
    {
        m_pTexture = g_AssetMgr.GetTexture(assetTextureID);
    }

    ///////////////////////////////////////////////////////

    virtual ~TileComponent() 
    {
        SDL_DestroyTexture(m_pTexture);
    }

    ///////////////////////////////////////////////////////
    
    virtual void Initialize() override {}

    ///////////////////////////////////////////////////////
    
    virtual void Update(const float deltaTime) override {}

    ///////////////////////////////////////////////////////

    virtual void Render() override
    {
        Render::DrawRectTextured(m_pTexture, m_SrcRect, m_DstRect, SDL_FLIP_NONE);
    }

    ///////////////////////////////////////////////////////

    virtual const char* GetName() const override { return "TileComponent"; }


public:
    SDL_Texture* m_pTexture = nullptr;
    SDL_Rect m_SrcRect;
    SDL_Rect m_DstRect;
    glm::vec2 m_Position;
};

#endif
