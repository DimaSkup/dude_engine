// ==================================================================
// Filename:  Animation.cpp
// Created:   19.04.2025 by DimaSkup
// ==================================================================
#include "Animation.h"


Animation::Animation()
{
}

///////////////////////////////////////////////////////////

Animation::Animation(
    const uint index, 
    const uint numFrames, 
    const uint animationSpeed) 
    :
    m_Idx(index),
    m_NumFrames(numFrames),
    m_AnimationSpeed(animationSpeed)
{
}


