// ==================================================================
// Filename:    AssetMgr.cpp
//
// Created:     16.04.2025 by DimaSkup
// ==================================================================
#include "AssetMgr.h"
#include "Log.h"


AssetMgr::AssetMgr(EntityMgr* pEnttMgr) : pEnttMgr_(pEnttMgr)
{
    if (pEnttMgr == nullptr)
        LogErr(LOG_INFO, "input ptr to the Entity Manager == nullptr");
}

///////////////////////////////////////////////////////////

AssetMgr::~AssetMgr()
{
}

///////////////////////////////////////////////////////////

void AssetMgr::ClearData()
{
    textures_.clear();
}

///////////////////////////////////////////////////////////

void AssetMgr::AddTexture(const char* textureID, const char* filePath)
{
    SDL_Texture* pTex = TextureMgr::LoadTexture(filePath);
    textures_.emplace(textureID, pTex);
}
///////////////////////////////////////////////////////////

SDL_Texture* AssetMgr::GetTexture(const char* textureID)
{
}


