// ==================================================================
// Filename:  FontMgr.h
// Desc:      a font manager class
//
// Created:   29.05.2025 by DimaSkup
// ==================================================================
#ifndef FONT_MGR_H
#define FONT_MGR_H

#include <SDL2/SDL_ttf.h>

class FontMgr
{
public:
    static TTF_Font* LoadFont(const char* fileName, const int fontSize)
    {
        return TTF_OpenFont(fileName, fontSize);
    }
};


// ==================================================================
// Declare a global instance of the Font Manager
// ==================================================================
extern FontMgr g_FontMgr;

#endif
