#include "SoundMgr.h"
#include "Log.h"
#include "FileSystem.h"

// global instance of the sound manager
SoundMgr g_SoundMgr;


//---------------------------------------------------------
// Desc: default constructor
//---------------------------------------------------------
SoundMgr::SoundMgr()
{
    if (Initialize() == -1)
    {
        LogErr(LOG, "can't initialize the sound manager");
        exit(-1);
    }
}


//---------------------------------------------------------
// Desc:   initialize the SDL_mixer which is responsible for sounds/music
// Ret:    0 if everything is ok, and -1 if we didn't manage to init
//---------------------------------------------------------
int SoundMgr::Initialize()
{
    Mix_Init(MIX_INIT_MP3);
    SDL_Init(SDL_INIT_AUDIO);

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 1, 2048) < 0) 
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

    // store a music into the array of music assets
    m_Music.push_back(m);
    const int idx = m_Music.size()-1;

    // generate a pair [file_stem, music_idx]
    // so later we will be able to get idx by this stem
    char* stem = g_String;
    FileSys::GetFileStem(filename, stem);
    m_MusicNameToIdx.insert({stem, idx});

    return idx;
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

    // store this sound into the array of sound assets
    m_Sounds.push_back(m);
    const int idx = m_Sounds.size()-1;

    // generate a pair [file_stem, sound_idx]
    // so later we will be able to get idx by this stem
    char* stem = g_String;
    FileSys::GetFileStem(filename, stem);
    m_SoundNameToIdx.insert({stem, idx});

    return idx;
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
    if (0 > m || m >= (int)m_Music.size())
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
// Args:   - channel: sound channel which will be used  
//         - soundIdx: an index to the loaded sound asset
//                  (if -1, loop "infinitely" (~65000 times)
//         - times: how many times to play this sound
// Ret:    a state flag
//---------------------------------------------------------
eSoundState SoundMgr::PlaySound(
    const int channel, 
    const int soundIdx,
    const int times)
{
    // check if input index is valid
    if (0 > soundIdx || soundIdx >= (int)m_Sounds.size())
    {
        LogErr(LOG, "invalid input sound idx: %d", soundIdx);
        return CHANNEL_STATE_INVALID_ASSET;
    }

    // if the channel is currently playing some sound
    if (Mix_Playing(channel) != 0)
        return CHANNEL_STATE_BUSY;

    Mix_Volume(channel, m_Volume);
    Mix_PlayChannel(channel, m_Sounds[soundIdx], times-1);

    return CHANNEL_STATE_START_PLAYING;
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

//---------------------------------------------------------
// Desc:  get an index to the music asset by its name
// Args:  - name: music name
// Ret:   int: an index to the music asset
//             or -1 if there is no such sound
//---------------------------------------------------------
int SoundMgr::GetMusicIdxByName(const char* name)
{
    if (!name || name[0] == '\0')
    {
        LogErr(LOG, "input music name is empty");
        return -1;
    }

    auto& map = m_MusicNameToIdx;
    auto  it  = map.find(name);

    return (it != map.end()) ? it->second : -1;
}

//---------------------------------------------------------
// Desc:  get an index to the sound asset by its name
// Args:  - name: sound name
// Ret:   int: an index to the sound asset
//             or -1 if there is no such sound
//---------------------------------------------------------
int SoundMgr::GetSoundIdxByName(const char* name)
{
    if (!name || name[0] == '\0')
    {
        LogErr(LOG, "input sound name is empty");
        return -1;
    }

    auto& map = m_SoundNameToIdx;
    auto  it  = map.find(name);

    return (it != map.end()) ? it->second : -1;
}
