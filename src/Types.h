// ==================================================================
// Filename:    Types.h
// Description: contains common typedefs/enums
// Created:     14.04.2025 by DimaSkup
// ==================================================================
#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

// typedefs
using EntityID = uint32_t;
using uint     = unsigned int;


enum eColliderTag
{
    NONE                 = 0,         // means that we have no collision with anything
    PLAYER               = 1,
    ENEMY                = 2,
    PROJECTILE           = 3,
    FRIENDLY_PROJECTILE  = 4,
    LEVEL_COMPLETE       = 5,
};

enum eCollisionType 
{
    NO_COLLISION,
    PLAYER_ENEMY_COLLISION,           // when player collides with any enemy
    PLAYER_PROJECTILE_COLLISION,      // when player collides with any projectile (for instance: bullet)
    ENEMY_PROJECTILE_COLLISION,       // when enemy collides with any projectile (for instance: bullet)
    PLAYER_VEGETATION_COLLISION,      // when player collides with any vegetation
    PLAYER_LEVEL_COMPLETE_COLLISION,  // if we got this type of collision it means that we complete the level
};

enum eLayerType
{
    LAYER_TILEMAP       = 0,     // terrain, water
    LAYER_VEGETATION    = 1,     // trees, grass
    LAYER_ENEMY         = 2,
    LAYER_OBSTACLE      = 3,
    LAYER_PLAYER        = 4,
    LAYER_PROJECTILE    = 5,  
    LAYER_UI            = 6,
    NUM_LAYERS,
};

// is used for sprites animations
enum eAnimationType
{
    ANIMATION_TYPE_DOWN = 0,
    ANIMATION_TYPE_UP,
    ANIMATION_TYPE_RIGHT,
    ANIMATION_TYPE_LEFT,
    ANIMATION_TYPE_SINGLE,
};


#endif
