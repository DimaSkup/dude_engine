#ifndef EVENT_H
#define EVENT_H

#include "Types.h"
#include <string.h>

enum eEventType
{
    EVENT_TYPE_SWITCH_ANIMATION,

    EVENT_TYPE_PLAYER_SHOOT,
    EVENT_TYPE_PLAYER_MOVE,
    EVENT_TYPE_PLAYER_STOP,     // doesn't move (velocity is zero)
    EVENT_TYPE_PLAYER_HIT_ENEMY_PROJECTILE,

    EVENT_TYPE_DESTROY_ENTITY,    
    EVENT_TYPE_KILL_ENEMY,
};

// ==================================================================
// Basic event
// ==================================================================
struct Event
{
    eEventType type;
    EntityID id;
    float x;
    float y;
    float z;
    float w;
};

// ==================================================================
// Concrete events
// ==================================================================
struct EventSwitchAnimation : public Event
{
    EventSwitchAnimation(
        const EntityID enttID,
        const eAnimationType inAnimType)
    {
        id   = enttID;
        type = EVENT_TYPE_SWITCH_ANIMATION;
        x    = (float)inAnimType;
    } 
};

///////////////////////////////////////////////////////////

struct EventPlayerShoot : public Event
{
    EventPlayerShoot(const EntityID enttID)
    {
        id = enttID;
        type = EVENT_TYPE_PLAYER_SHOOT;
    }
};

///////////////////////////////////////////////////////////

struct EventPlayerMove : public Event
{
    EventPlayerMove(
        const EntityID enttID, 
        const float velocityX,
        const float velocityY)
    {
        id   = enttID;
        type = EVENT_TYPE_PLAYER_MOVE;
        x    = velocityX;
        y    = velocityY;
    }
};

///////////////////////////////////////////////////////////

struct EventPlayerStop : public Event
{
    EventPlayerStop(const EntityID enttID)
    {
        id = enttID;
        type = EVENT_TYPE_PLAYER_STOP;
    }
};

///////////////////////////////////////////////////////////

struct EventPlayerHitEnemyProjectile : public Event
{
    EventPlayerHitEnemyProjectile(const EntityID enttID)
    {
        id = enttID;
        type = EVENT_TYPE_PLAYER_HIT_ENEMY_PROJECTILE;
    }
};

///////////////////////////////////////////////////////////

struct EventDestroyEntity : public Event
{
    EventDestroyEntity(const EntityID enttID)
    {
        id = enttID;
        type = EVENT_TYPE_DESTROY_ENTITY;
    }
};

///////////////////////////////////////////////////////////

struct EventKillEnemy : public Event
{
    EventKillEnemy(const EntityID enemyID)
    {
        id = enemyID;
        type = EVENT_TYPE_KILL_ENEMY;
    }
};

#endif
