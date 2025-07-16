#include "Collision.h"

bool Collision::CheckRectCollision(
    const SDL_Rect& rectA, 
    const SDL_Rect& rectB)
{
    // check collision of two input rectangles
    return (
        (rectA.x + rectA.w >= rectB.x) &&
        (rectB.x + rectB.w >= rectA.x) &&
        (rectA.y + rectA.h >= rectB.y) &&
        (rectB.y + rectB.h >= rectA.y));
}
