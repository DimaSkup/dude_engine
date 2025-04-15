// ==================================================================
// Description: init SDL specific stuff (Initialize), 
//              prepare frame for rendering (Begin)and
//              present this frame onto the screen (End)
//
// Created:     15.04.2025 by DimaSkup              
// ==================================================================
#include <SDL2/SDL.h>

class Render
{
public:
    bool Initialize(const int wndWidth, const int wndHeight);
    void Shutdown();

    void Begin();
    void End();


};    

// ==================================================================
// Declare some globals
// ==================================================================
extern SDL_Window*   g_pWindow;
extern SDL_Renderer* g_pRenderer;
