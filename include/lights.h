#pragma once

#include <SDL3/SDL.h>
#include "engine_math.h"

struct Color
{
    int r, g, b;
};

extern Point3D light;

Uint32 addBrightness(SDL_Surface* surface, int r, int g, int b, float brightness);
