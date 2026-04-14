#include "lights.h"
#include "engine_math.h"

Point3D light = {-1, 0, -1.75f};

Uint32 addBrightness(SDL_Surface* surface, int r, int g, int b, float brightness)
{
    return SDL_MapSurfaceRGB(surface, r * brightness, g * brightness, b * brightness);
}
