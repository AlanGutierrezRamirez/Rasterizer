#pragma once

#include <SDL3/SDL.h>
#include "draw.h"
#include "light.h"
#include "vector_3.h"
#include "light.h"

struct Face
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
    Face triangles[total_faces * 2];

};

struct SphereInstance {
    Sphere sphere;
    Vec3 pos;
    float angle;
    float accum;
    float orbitRadius;
    Color color;
};

void InitSphere(Sphere& sphere, float r);

void renderSphere(SDL_Surface* surface, SphereInstance &sphere, Vec3 cameraPos, Vec3 cameraTarget, SpotLight light);
