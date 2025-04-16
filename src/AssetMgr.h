// ==================================================================
// Filename:    AssetMgr.h
// Description: container for assets: textures, sounds, etc.
//
// Created:     16.04.2025 by DimaSkup
// ==================================================================
#ifndef ASSET_MGR_H
#define ASSET_MGR_H

#include "EntityMgr.h"
#include <SDL2/SDL.h>
#include <map>
#include <string>

class AssetMgr
{
public:
    AssetMgr() {}
    AssetMgr(EntityMgr* pEnttMgr);
    ~AssetMgr();

    void SetEntityMgr(EntityMgr* pEnttMgr);
    void ClearData();

    void AddTexture(const char* textureID, const char* filePath);
    SDL_Texture* GetTexture(const char* textureID);

private:
    EntityMgr*                          m_pEnttMgr = nullptr;
    std::map<std::string, SDL_Texture*> m_Textures;
};

// ==================================================================
// Declare global instance of the Asset Manager
// ==================================================================
extern AssetMgr g_AssetMgr;

#endif
