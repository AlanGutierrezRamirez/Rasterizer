#pragma once 

#include <SDL3/SDL.h>
#include "lights.h"
#include "config.h"
#include "vector_3.h"

struct ProjectedPoint
{
    int x, y;
    float z;
    Vec3 normal;
};

ProjectedPoint projectPoint(Vec3 vertex);

void putPixel(SDL_Surface* surface, int x, int y, Uint32 color);
void drawLine(SDL_Surface* surface, int x0, int y0, int x1, int y1, Uint32 color);
void fillTriangle(SDL_Surface* surface, ProjectedPoint v_one, ProjectedPoint v_two, ProjectedPoint v_three, Uint32 color);
void fillTrianglePhong(SDL_Surface* surface, ProjectedPoint v_one, ProjectedPoint v_two, ProjectedPoint v_three, Vec3 light, Color color);
