#include "TextureMgr.h"
#include "Render.h"

SDL_Texture* TextureMgr::LoadTexture(const char* fileName)
{
    SDL_Surface* pSurface = IMG_Load(fileName);
    SDL_Texture* pTexture = SDL_CreateTextureFromSurface(g_pRenderer, pSurface);
    SDL_FreeSurface(pSurface);

    return pTexture;
}

///////////////////////////////////////////////////////////

void TextureMgr::Draw(
    SDL_Texture* pTexture,
    const SDL_Rect& srcRect,
    const SDL_Rect& dstRect,
    const SDL_RendererFlip& flip)
{
    SDL_RenderCopyEx(g_pRenderer, pTexture, &srcRect, &dstRect, 0.0, NULL, flip); 
}
