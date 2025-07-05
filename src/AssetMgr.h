// ==================================================================
// Filename:    AssetMgr.h
// Description: container for assets: textures, sounds, etc.
//
// Created:     16.04.2025 by DimaSkup
// ==================================================================
#ifndef ASSET_MGR_H
#define ASSET_MGR_H

#include "SoundMgr.h"
#include "EntityMgr.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
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
    void AddFont   (const char* fontID, const char* filePath, const int fontSize);

    SDL_Texture* GetTexture(const char* textureID);
    TTF_Font*    GetFont   (const char* fontID);

    SDL_Point    GetTextureSize(SDL_Texture* pTexture);

    // sounds/music related methods
    inline void TogglePlay()                    { m_SoundMgr.TogglePlay(); }
    inline void SetVolume(const int v)          { m_SoundMgr.SetVolume(v); }

    inline int LoadMusic(const char* filename)  { return m_SoundMgr.LoadMusic(filename); }
    inline int LoadSound(const char* filename)  { return m_SoundMgr.LoadSound(filename); }
    
    inline eSoundState PlayMusic(const int m)   
    {   
        return m_SoundMgr.PlayMusic(m);     
    }

    inline eSoundState PlaySound(const int s, const int times)
    {   
        return m_SoundMgr.PlaySound(s, times);   
    }

private:
    EntityMgr*                          m_pEnttMgr = nullptr;
    SoundMgr                            m_SoundMgr;
    std::map<std::string, SDL_Texture*> m_Textures;
    std::map<std::string, TTF_Font*>    m_Fonts;
};


// ==================================================================
// Declare a global instance of the Asset Manager
// ==================================================================
extern AssetMgr g_AssetMgr;

#endif
