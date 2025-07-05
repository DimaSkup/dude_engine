// ==================================================================
// Filename:   SoundMgr.h
// Desc:       - initialization of SDL_Mixer 
//               (SDL part related to sounds)
//             - loading of sounds/music
//             - playing sounds/music
// 
// Created:    05.07.2025 by DimaSkup
// ==================================================================
#ifndef SOUND_H
#define SOUND_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <vector>
#include <map>

struct SoundAssetName
{
    char name[16];
};

enum eSoundState 
{
    CHANNEL_STATE_INVALID_ASSET,
    CHANNEL_STATE_BUSY,
    CHANNEL_STATE_START_PLAYING,
};

///////////////////////////////////////////////////////////

class SoundMgr
{
public:
    SoundMgr();
    ~SoundMgr();

    int Initialize();
    void Release();

    void TogglePlay();
    void SetVolume(const int v);

    int LoadMusic(const char* filename);
    int LoadSound(const char* filename);
    
    eSoundState PlayMusic(const int m);
    eSoundState PlaySound(const int s, const int times);

public:
    std::vector<Mix_Chunk*> m_Sounds;
    std::vector<Mix_Music*> m_Music;
    int m_Sound  = 0;
    int m_Song   = 0;
    int m_Volume = 0;
};

#endif
