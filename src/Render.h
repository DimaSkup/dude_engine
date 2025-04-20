// ==================================================================
// Description: init SDL specific stuff (Initialize), 
//              prepare frame for rendering (Begin)and
//              present this frame onto the screen (End)
//
// Created:     15.04.2025 by DimaSkup              
// ==================================================================
#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>

class Render
{
public:
    bool Initialize(const int wndWidth, const int wndHeight);
    void Shutdown();

    static void DrawRectFilled(
        const int posX,
        const int posY,
        const int width,
        const int height,
        const int r,             // red   channel of RGBA
        const int g,             // green channel of RGBA
        const int b,             // blue  channel of RGBA
        const int a);            // alpha channel of RGBA

    static void DrawRectTextured(
        SDL_Texture* pTexture,
        const SDL_Rect& srcRect,
        const SDL_Rect& dstRect,
        const SDL_RendererFlip& flip);

    void Begin();   // clear the screen before the next frame
    void End();     // present all the rendered stuff onto the screen

    inline static int GetWndWidth()  { return ms_WndWidth; }
    inline static int GetWndHeight() { return ms_WndHeight; }

private:
    static int ms_WndWidth;
    static int ms_WndHeight;
};    

// ==================================================================
// Declare some globals
// ==================================================================
extern SDL_Window*   g_pWindow;
extern SDL_Renderer* g_pRenderer;

#endif
