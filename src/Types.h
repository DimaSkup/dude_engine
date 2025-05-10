// ==================================================================
// Filename:    Types.h
// Description: contains common typedefs/enums
// Created:     14.04.2025 by DimaSkup
// ==================================================================
#ifndef TYPES_H
#define TYPES_H

using uint = unsigned int;

enum eColliderTag
{
    COLLIDER_TAG_NONE,    // means that we have no collision with anything
    COLLIDER_TAG_PLAYER,
    COLLIDER_TAG_ENEMY,
};

#endif
