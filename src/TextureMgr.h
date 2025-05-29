// ==================================================================
// Filename:    AssetMgr.h
// Description: container for assets: textures, sounds, etc.
//
// Created:     16.04.2025 by DimaSkup
// ==================================================================
#ifndef TEXTURE_MGR_H
#define TEXTURE_MGR_H

#include <SDL2/SDL.h>

class TextureMgr
{
public:
    static SDL_Texture* LoadTexture(const char* fileName);
};

// ==================================================================
// Declare a global instance of the Texture Manager
// ==================================================================
extern TextureMgr g_TextureMgr;

#endif
