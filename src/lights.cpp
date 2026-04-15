#include "lights.h"
#include "vector_3.h"

Vec3 light = {-1, 0, -1.75f};

Uint32 addBrightness(SDL_Surface* surface, int r, int g, int b, float brightness)
{
    return SDL_MapSurfaceRGB(surface, r * brightness, g * brightness, b * brightness);
}
