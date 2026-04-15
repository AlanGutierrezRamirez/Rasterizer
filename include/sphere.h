#pragma once

#include <SDL3/SDL.h>
#include "draw.h"
#include "lights.h"
#include "vector_3.h"

struct Triangle
{
    int a, b, c;
};

struct Sphere
{
    static constexpr int latitud      = 32;
    static constexpr int longitud     = 64;
    static constexpr int total_points = (latitud + 1) * longitud;
    static constexpr int total_faces  = latitud * longitud;

    Vec3  vertices[total_points];
    Triangle triangles[total_faces * 2];

};

struct SphereInstance {
    Sphere sphere;
    float angle;
    float accum;
    float orbitRadius;
    Color color;
};

void InitSphere(Sphere& sphere, float r);

void renderSphere(SDL_Surface* surface, SphereInstance &sphere, Vec3 cameraDir);
