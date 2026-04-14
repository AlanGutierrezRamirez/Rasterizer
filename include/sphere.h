#pragma once

#include <SDL3/SDL.h>
#include "engine_math.h"
#include "draw.h"
#include "lights.h"

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

    Point3D  vertices[total_points];
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

void renderSphere(SDL_Surface* surface, SphereInstance &sphere, Point3D cameraDir);
