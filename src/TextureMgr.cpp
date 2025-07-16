#include "TextureMgr.h"
#include "Render.h"
#include "Log.h"
#include "FileSystem.h"
#include <SDL2/SDL_image.h>

// init global instance of the Texture Manager
TextureMgr g_TextureMgr;

///////////////////////////////////////////////////////////

SDL_Texture* TextureMgr::LoadTexture(const char* fileName)
{
    if (!FileSys::Exists(fileName))
    {
        LogErr(LOG, "input path to texture is empty");
        return nullptr;
    }

    SDL_Surface* pSurface = IMG_Load(fileName);
    SDL_Texture* pTexture = SDL_CreateTextureFromSurface(g_pRenderer, pSurface);
    SDL_FreeSurface(pSurface);

    return pTexture;
}

