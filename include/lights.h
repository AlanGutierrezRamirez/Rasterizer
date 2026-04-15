#pragma once

#include <SDL3/SDL.h>
#include "vector_3.h"

struct Color
{
    int r, g, b;
};

extern Vec3 light;

Uint32 addBrightness(SDL_Surface* surface, int r, int g, int b, float brightness);
