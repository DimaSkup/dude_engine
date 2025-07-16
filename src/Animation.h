// ==================================================================
// Filename:    Animation.h
// Description: data container for sprite animation
//
// Created:     19.04.2025 by DimaSkup
// ==================================================================
#ifndef ANIMATION_H
#define ANIMATION_H

#include "Types.h"

class Animation
{
public:
    Animation();
    Animation(
        const uint index, 
        const uint numFrames, 
        const uint animationSpeed);

public:
    uint m_Idx = 0; 
    uint m_NumFrames = 0;
    uint m_AnimationSpeed = 0;
};

#endif
