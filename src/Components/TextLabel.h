// ==================================================================
// Filename:  TextLabel.h
// Desc:      a component to hold and handle text on the screen
// 
// Created:   29.05.2025 by DimaSkup 
// ==================================================================
#ifndef TEXT_LABEL_H
#define TEXT_LABEL_H

#include "../StrHelper.h"
#include "../IComponent.h"
#include "../AssetMgr.h"
#include "../Log.h"
#include "../Render.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>


class TextLabel : public IComponent
{
public:
    TextLabel(
        const int x,
        const int y,
        const char* text,
        const char* fontFamily,
        const SDL_Color color) 
    {
        if (IsStrEmpty(text))
        {
            LogErr(LOG, "input text str is empty");
            return;
        }

        if (IsStrEmpty(fontFamily))
        {
            LogErr(LOG, "input font family str is empty");
            return;
        }

        m_Position.x = x;
        m_Position.y = y;
        m_Color      = color;
        SetLabelText(text, fontFamily);
    }

    ///////////////////////////////////////////////////////

    void SetLabelText(const char* text, const char* fontFamily)
    {
        TTF_Font* pFont = g_AssetMgr.GetFont(fontFamily);

        // create a new text texture surface
        SDL_Surface* pSurface = TTF_RenderText_Blended(pFont, text, m_Color);
        
        // release the previous texture if we had any
        if (m_pTexture)
        {
            SDL_DestroyTexture(m_pTexture);
            m_pTexture = nullptr;
        }

        m_pTexture = SDL_CreateTextureFromSurface(g_pRenderer, pSurface);
        SDL_FreeSurface(pSurface);
        SDL_QueryTexture(m_pTexture, NULL, NULL, &m_Position.w, &m_Position.h);

        // update data fields
        m_Text       = text;
        m_FontFamily = fontFamily;

    }

    ///////////////////////////////////////////////////////

    virtual const char* GetName() const override 
    { 
        return "TextLabel (Component)"; 
    }
    
    ///////////////////////////////////////////////////////
    
    inline const SDL_Rect&    GetPosition() const { return m_Position; }
    inline const std::string& GetText()     const { return m_Text; }
    inline const std::string& GetFontName() const { return m_FontFamily; }
    inline const SDL_Color&   GetColor()    const { return m_Color; }
    inline       SDL_Texture* GetTexture()  const { return m_pTexture; }

private:    
    SDL_Rect     m_Position;
    std::string  m_Text;
    std::string  m_FontFamily;
    SDL_Color    m_Color;               // RGBA color
    SDL_Texture* m_pTexture = nullptr;
};

#endif
