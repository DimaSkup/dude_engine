// ==================================================================
// Filename:    AssetMgr.cpp
//
// Created:     16.04.2025 by DimaSkup
// ==================================================================
#include "AssetMgr.h"
#include "Log.h"


// init global instance of the AssetMgr
AssetMgr g_AssetMgr;

///////////////////////////////////////////////////////////

AssetMgr::AssetMgr(EntityMgr* pEnttMgr) : m_pEnttMgr(pEnttMgr)
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
    m_Textures.clear();
}

///////////////////////////////////////////////////////////

void AssetMgr::AddTexture(const char* textureID, const char* filePath)
{
    SDL_Texture* pTex = TextureMgr::LoadTexture(filePath);
    m_Textures.emplace(textureID, pTex);
}
///////////////////////////////////////////////////////////

SDL_Texture* AssetMgr::GetTexture(const char* textureID)
{
    return m_Textures[textureID];
}


