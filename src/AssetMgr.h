// ==================================================================
// Filename:    AssetMgr.h
// Description: container for assets: textures, sounds, etc.
//
// Created:     16.04.2025 by DimaSkup
// ==================================================================
#ifndef ASSET_MGR_H
#define ASSET_MGR_H

#include "TextureMgr.h"
#include "EntityMgr.h"
#include <map>
#include <string>

class AssetMgr
{
public:
    AssetMgr(EntityMgr* pEnttMgr);
    ~AssetMgr();

    void ClearData();

    void AddTexture(const char* textureID, const char* filePath);
    SDL_Texture* GetTexture(const char* textureID);

private:
    EntityMgr* pEnttMgr_ = nullptr;
    std::map<std::string, SDL_Texture*> textures_;
};

#endif
