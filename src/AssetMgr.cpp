// ==================================================================
// Filename:    AssetMgr.cpp
//
// Created:     16.04.2025 by DimaSkup
// ==================================================================
#include "AssetMgr.h"
#include "TextureMgr.h"
#include "FontMgr.h"
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
    for (auto& it : m_Textures)
        SDL_DestroyTexture(it.second);

    for (auto& it : m_Fonts)
        TTF_CloseFont(it.second);

    m_Textures.clear();
    m_Fonts.clear();
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

void AssetMgr::AddFont(
    const char* fontID, 
    const char* filePath, 
    int fontSize)
{
    // load a font by the filePath, and set an ID and size for it

    if (IsStrEmpty(fontID))
    {
        LogErr(LOG_INFO, "input font ID is empty");
        return;
    }

    if (IsStrEmpty(filePath))
    {
        LogErr(LOG_INFO, "input file path is empty");
        return;
    }

    if (fontSize <= 0)
    {
        LogErr(LOG_INFO, "input font size must be > 0");
        return;
    }

    TTF_Font* pFont = FontMgr::LoadFont(filePath, fontSize);
    if (!pFont)
    {
        sprintf(g_String, "can't to load font from file: %s", filePath);
        LogErr(LOG_INFO, g_String);
        return;
    }

    m_Fonts.emplace(fontID, pFont);
}

///////////////////////////////////////////////////////////

SDL_Texture* AssetMgr::GetTexture(const char* textureID)
{
    // get a texture by input ID

    if (IsStrEmpty(textureID))
    {
        LogErr(LOG_INFO, "input texture ID is empty");
        return nullptr;
    }

    const bool hasTexture = (m_Textures.find(textureID) != m_Textures.end());

    return (hasTexture) ? m_Textures[textureID] : nullptr;
}

///////////////////////////////////////////////////////////

TTF_Font* AssetMgr::GetFont(const char* fontID)
{
    // get a font by input ID

    if (IsStrEmpty(fontID))
    {
        LogErr(LOG_INFO, "input font ID is empty");
        return nullptr;
    }

    const bool hasFont = (m_Fonts.find(fontID) != m_Fonts.end());

    return (hasFont) ? m_Fonts[fontID] : nullptr;
}

///////////////////////////////////////////////////////////

SDL_Point AssetMgr::GetTextureSize(SDL_Texture* pTexture)
{
    // get texture width and height
    SDL_Point size{0,0};

    if (!pTexture)
        LogErr("input ptr to texture == nullptr");
    else
        SDL_QueryTexture(pTexture, NULL, NULL, &size.x, &size.y);

    return size;
}

