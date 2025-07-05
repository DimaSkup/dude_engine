#include "SoundMgr.h"
#include "Log.h"


SoundMgr::SoundMgr()
{
    if (Initialize() == -1)
    {
        LogErr(LOG, "can't initialize the sound manager");
        exit(-1);
    }
}

SoundMgr::~SoundMgr()
{
    Release();
}

//---------------------------------------------------------
// Desc:   initialize the SDL_mixer which is responsible for sounds/music
// Ret:    0 if everything is ok, and -1 if we didn't manage to init
//---------------------------------------------------------
int SoundMgr::Initialize()
{
    Mix_Init(MIX_INIT_MP3);
    SDL_Init(SDL_INIT_AUDIO);

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) 
    {
        LogErr(LOG, "SDL_Mixer can't init. Err: %s", Mix_GetError());
        return -1;
    }
    SetVolume(80);
    return 0;
}

//---------------------------------------------------------
// Desc:   release music/sounds data and release the SDL mixer
//---------------------------------------------------------
void SoundMgr::Release()
{
    // relese all the sounds
    for (int s = 0; s < (int)m_Sounds.size(); ++s)
    {
        Mix_FreeChunk(m_Sounds[s]);
        m_Sounds[s] = nullptr;
    }

    // release all the music
    for (int m = 0; m < (int)m_Music.size(); ++m)
    {
        Mix_FreeMusic(m_Music[m]);
        m_Music[m] = nullptr;
    }

    m_Sounds.clear();
    m_Music.clear();

    Mix_Quit();
}

//---------------------------------------------------------
// Desc:   load in .mp3 (music) file by input filename
// Args:   - filename: path to file
// Ret:    index to the music asset
//---------------------------------------------------------
int SoundMgr::LoadMusic(const char* filename)
{
    Mix_Music* m = Mix_LoadMUS(filename);
    if (m == nullptr)
    {
        LogErr(LOG, "failed to load music: %s\nSDL_Mixer err: %s", filename, Mix_GetError());
        return -1;
    }

    m_Music.push_back(m);
    return m_Music.size()-1;
}

//---------------------------------------------------------
// Desc:   load in .wav (sound) file by input filename
// Args:   - filename: path to file
// Ret:    index to the sound asset
//---------------------------------------------------------
int SoundMgr::LoadSound(const char* filename)
{
    Mix_Chunk* m = Mix_LoadWAV(filename);
    if (m == nullptr)
    {
        LogErr(LOG, "failed to load sound: %s\nSDL_Mixer err: %s", filename, Mix_GetError());
        return -1;
    }

    m_Sounds.push_back(m);
    return m_Sounds.size()-1;
}

//---------------------------------------------------------
// Desc:  setup the volume level
// Args:  - v: which volume level to set (from 0 to 100)
//---------------------------------------------------------
void SoundMgr::SetVolume(const int v)
{
    m_Volume = (MIX_MAX_VOLUME * v) / 100;
}

//---------------------------------------------------------
// Desc:   play a music asset by input index
// Args:   - m: an index to the loaded music asset
// Ret:    a state flag
//---------------------------------------------------------
eSoundState SoundMgr::PlayMusic(const int m)
{
    // check if input index is valid
    if (0 < m || m >= (int)m_Music.size())
    {
        LogErr(LOG, "invalid input music idx: %d", m);
        return CHANNEL_STATE_INVALID_ASSET;
    }

    // if the channel is currently playing some music
    if (Mix_PlayingMusic() != 0)
        return CHANNEL_STATE_BUSY;

    // if we aren't playing any music -- PLAY IT!
    Mix_Volume(1, m_Volume);
    Mix_PlayMusic(m_Music[m], -1);

    return CHANNEL_STATE_START_PLAYING;
}

//---------------------------------------------------------
// Desc:   play a sound asset by input index
// Args:   - s: an index to the loaded sound asset
//         - times: how many times to play this sound
//                  (if -1, loop "infinitely" (~65000 times)
// Ret:    a state flag
//---------------------------------------------------------
eSoundState SoundMgr::PlaySound(const int s, const int times)
{
    // check if input index is valid
    if (0 < s || s >= (int)m_Sounds.size())
    {
        LogErr(LOG, "invalid input sound idx: %d", s);
        return CHANNEL_STATE_INVALID_ASSET;
    }

    // if the channel is currently playing some sound
    if (Mix_Playing(-1) != 0)
        return CHANNEL_STATE_BUSY;

    Mix_Volume(-1, m_Volume);
    Mix_PlayChannel(-1, m_Sounds[s], times);

    return CHANNEL_STATE_START_PLAYING;;
}


//---------------------------------------------------------
// Desc:   turn on/off the music playing
//---------------------------------------------------------
void SoundMgr::TogglePlay()
{
    if (Mix_PausedMusic() == 1)
    {
        Mix_ResumeMusic();
    }
    else
    {
        Mix_PauseMusic();
    }
}


