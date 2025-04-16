// ==================================================================
// Filename:    AssetMgr.h
// Description: container for assets: textures, sounds, etc.
//
// Created:     16.04.2025 by DimaSkup
// ==================================================================
#ifndef TEXTURE_MGR_H
#define TEXTURE_MGR_H

#include "Game.h"

class TextureMgr
{
public:
    static SDL_Texture* LoadTexture(const char* fileName);

    static void Draw(
        SDL_Texture* pTexture, 
        const SDL_Rect& srcRect,
        const SDL_Rect& dstRect,
        const SDL_RendererFlip& flip);
};

// ==================================================================
// Declare global instance of the Texture Manager
// ==================================================================
extern TextureMgr g_TextureMgr;

#endif
