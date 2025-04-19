// ==================================================================
// Filename:    AssetMgr.cpp
//
// Created:     16.04.2025 by DimaSkup
// ==================================================================
#include "AssetMgr.h"
#include "TextureMgr.h"
#include "Log.h"
#include "StrHelper.h"

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
    // load a texture by filePath and set ID to textureID

    if (IsStrEmpty(textureID))
        LogErr(LOG_INFO, "input texture ID is empty");

    if (IsStrEmpty(filePath))
        LogErr(LOG_INFO, "input path to texture is empty");

    // try to load texture file
    SDL_Texture* pTex = TextureMgr::LoadTexture(filePath);
    if (!pTex)
    {
        sprintf(g_String, "didn't manage to load texture: %s", filePath);
        LogErr(g_String);
        return;
    }

    m_Textures.emplace(textureID, pTex);
}
///////////////////////////////////////////////////////////

SDL_Texture* AssetMgr::GetTexture(const char* textureID)
{
    if (IsStrEmpty(textureID))
    {
        LogErr(LOG_INFO, "input texture ID is empty");
        return nullptr;
    }

    const bool hasTexture = (m_Textures.find(textureID) != m_Textures.end());

    return (hasTexture) ? m_Textures[textureID] : nullptr;
}


