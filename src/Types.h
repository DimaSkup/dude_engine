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
    NONE,         // means that we have no collision with anything
    PLAYER,
    ENEMY,
    PROJECTILE,
    FRIENDLY_PROJECTILE,
    LEVEL_COMPLETE,
};

enum eCollisionType 
{
    NO_COLLISION,
    PLAYER_ENEMY_COLLISION,
    PLAYER_PROJECTILE_COLLISION,
    ENEMY_PROJECTILE_COLLISION,
    PLAYER_VEGETATION_COLLISION,
    PLAYER_LEVEL_COMPLETE_COLLISION,  // if we got this type of collision it means that we complete the level
};

enum eLayerType
{
    LAYER_TILEMAP       = 0,     // terrain, water
    LAYER_VEGETATION    = 1,     // trees, grass
    LAYER_ENEMY         = 2,
    LAYER_PLAYER        = 3,
    LAYER_PROJECTILE    = 4,  
    LAYER_UI            = 5,
    LAYER_OBSTACLE      = 6,
    NUM_LAYERS,
};


#endif
