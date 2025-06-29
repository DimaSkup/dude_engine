// ==================================================================
// Filename:    Render.cpp
// Description: implement functional of the Render class
//
// Created:     15.04.2025 by DimaSkup
// ==================================================================
#include "Render.h"
#include "Log.h"
#include <SDL2/SDL_ttf.h>

// init some globals 
SDL_Window*   g_pWindow   = nullptr;
SDL_Renderer* g_pRenderer = nullptr;

// init some static members
int Render::ms_WndWidth = 800;
int Render::ms_WndHeight = 600;


///////////////////////////////////////////////////////////

bool Render::Initialize(
    const int wndWidth,       // window width for windowed mode
    const int wndHeight,      // window height for windowed mode
    const bool isFullscreen)
{
    LogMsg(LOG, "Start of the game initialization");

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        LogErr(LOG, "Error initializing SDL");
        return false;        
    }

    if (TTF_Init() != 0)
    {
        LogErr(LOG, "Error initializing SDL TTF");
        return false;
    }

    int width = 0;
    int height = 0;

    if (isFullscreen)
    {
        // use SDL to query what is the fullscreen max width and height
        SDL_DisplayMode displayMode;
        SDL_GetCurrentDisplayMode(0, &displayMode);

        width  = displayMode.w;
        height = displayMode.h;
    }

    // windowed mode
    else
    {
        width  = wndWidth;
        height = wndHeight;
    }
    

    // create a SDL window
    g_pWindow = SDL_CreateWindow(
        "dude_engine",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_BORDERLESS);

    if (g_pWindow == nullptr)
    {
        LogErr(LOG, "Error initializing window");
        return false;
    }
   
    // create a SDL renderer 
    g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, 0);
    if (g_pRenderer == nullptr)
    {
        LogErr(LOG, "Error initializing renderer");
        return false;
    }

    // go to the fullscreen mode and hide cursor
    if (isFullscreen)
    {
        SDL_SetWindowFullscreen(g_pWindow, SDL_WINDOW_FULLSCREEN);
        SDL_ShowCursor(SDL_DISABLE);
    }

    ms_WndWidth  = width;
    ms_WndHeight = height;

    LogMsg(LOG, "Render is initialized!");
    return true;
}

///////////////////////////////////////////////////////////

void Render::Shutdown()
{
    SDL_DestroyRenderer(g_pRenderer);
    SDL_DestroyWindow(g_pWindow);
    SDL_Quit();
}

///////////////////////////////////////////////////////////

void Render::Begin()
{
    SDL_SetRenderDrawColor(g_pRenderer, 21, 21, 21, 255);
    SDL_RenderClear(g_pRenderer);
}

///////////////////////////////////////////////////////////

void Render::End()
{
    SDL_RenderPresent(g_pRenderer);
}

///////////////////////////////////////////////////////////

void Render::DrawRectFilled(
    const int posX,
    const int posY,
    const int width,
    const int height,
    const int r,             // red   channel of RGBA
    const int g,             // green channel of RGBA
    const int b,             // blue  channel of RGBA
    const int a)             // alpha channel of RGBA
{
    SDL_Rect rect = { posX, posY, width, height };
    SDL_SetRenderDrawColor(g_pRenderer, r, g, b, a);
    SDL_RenderFillRect(g_pRenderer, &rect);
}

///////////////////////////////////////////////////////////

void Render::DrawRectTextured(
    SDL_Texture* pTexture,
    const SDL_Rect& srcRect,
    const SDL_Rect& dstRect,
    const SDL_RendererFlip& flip)
{
    if (!pTexture)
        LogErr(LOG, "input texture == nullptr");

    SDL_RenderCopyEx(g_pRenderer, pTexture, &srcRect, &dstRect, 0.0, NULL, flip); 
}
