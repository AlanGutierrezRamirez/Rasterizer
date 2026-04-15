#include "lights.h"
#include "vector_3.h"

Vec3 light = {0, 0, 1};

Uint32 addBrightness(SDL_Surface* surface, int r, int g, int b, float brightness)
{
    return SDL_MapSurfaceRGB(surface, r * brightness, g * brightness, b * brightness);
}
