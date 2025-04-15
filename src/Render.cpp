// ==================================================================
// Filename:    Render.cpp
// Description: implement functional of the Render class
//
// Created:     15.04.2025 by DimaSkup
// ==================================================================
#include "Render.h"
#include "Log.h"


// init some globals 
SDL_Window*   g_pWindow   = nullptr;
SDL_Renderer* g_pRenderer = nullptr;


///////////////////////////////////////////////////////////

bool Render::Initialize(const int wndWidth, const int wndHeight)
{
    LogMsg(LOG_INFO, "Start of the game initialization");

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        LogErr(LOG_INFO, "Error initializing SDL");
        return false;        
    }

    g_pWindow = SDL_CreateWindow(
        "dude_engine",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        wndWidth,
        wndHeight,
        SDL_WINDOW_BORDERLESS);

    if (g_pWindow == nullptr)
    {
        LogErr(LOG_INFO, "Error initializing window");
        return false;
    }
    
    g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, 0);
    if (g_pRenderer == nullptr)
    {
        LogErr(LOG_INFO, "Error initializing renderer");
        return false;
    }

    LogMsg("Render is initialized!");
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
