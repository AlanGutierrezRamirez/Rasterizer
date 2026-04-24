#pragma once

#include <SDL3/SDL.h>
#include "light.h"
#include "config.h"
#include "vector_3.h"

struct ProjectedPoint
{
    int x, y;
    float z;
    Vec3 normal;
    Vec3 worldPos;
};

class Draw
{

    public:
        Draw() {};
        ~Draw() {};

        static ProjectedPoint projectPoint(Vec3 vertex);

        static void putPixel(SDL_Surface* surface, int x, int y, Uint32 color);
        static void drawLine(SDL_Surface* surface, int x0, int y0, int x1, int y1, Uint32 color);
        static void fillTriangle(SDL_Surface* surface, ProjectedPoint v_one, ProjectedPoint v_two, ProjectedPoint v_three, Uint32 color);
        static void fillTrianglePhong(SDL_Surface* surface, ProjectedPoint v_one, ProjectedPoint v_two, ProjectedPoint v_three, SpotLight light, Color color);
        static void fillTriangleGouraud(SDL_Surface* surface, ProjectedPoint v_one, ProjectedPoint v_two, ProjectedPoint v_three, SpotLight light, Color color);

};