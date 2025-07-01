// ==================================================================
// Filename:     GameState.h
// Description:  a common container for different states of the game
// ==================================================================
#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "Types.h"

struct GameStates
{
    uint cameraPosX     = 0.0f;   // camera's current position by X
    uint cameraPosY     = 0.0f;   // camera's current position by Y
    uint cameraMaxX     = 0.0f;   // maximal value for the camera's left side (usually == levelMapWidth  - cameraRect.width)
    uint cameraMaxY     = 0.0f;   // maximal value for the camera's top side  (usually == levelMapHeight - cameraRect.height)
                            
    uint windowWidth    = 800;
    uint windowHeight   = 600;
    uint halfWndWidth   = 400;
    uint halfWndHeight  = 300;

    uint levelMapWidth  = 0;      // width of the current level in pixels
    uint levelMapHeight = 0;      // height of the current level in pixels

    int playerSpeed = 400;

    void SetWndDimensions(const uint wndWidth, const uint wndHeight)
    {

        windowWidth   = (wndWidth > 0)  ? wndWidth  : 800;
        windowHeight  = (wndHeight > 0) ? wndHeight : 600;
        halfWndWidth  = windowWidth / 2;
        halfWndHeight = windowHeight / 2;
    }
};

extern GameStates g_GameStates;


#endif
