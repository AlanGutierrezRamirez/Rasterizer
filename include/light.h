#pragma once
#include "vector_3.h"
#include <SDL3/SDL.h>

struct Color
{
    int r, g, b;
};

class SpotLight
{
    public:
        SpotLight(Vec3 _pos, float _inner, Vec3 _dir, float _outer, float _max, float _intensity, Color _color) 
        : pos(_pos), cos_inner(_inner), dir(_dir), cos_outer(_outer), rmax(_max), intensity(_intensity), color(_color){};
        ~SpotLight() {};
        
        float spotLight(Vec3 pixel, Vec3 normal);

        static float radians(float angle);

        float distAttenuation(Vec3 pixel);
        float angularAttenuation(Vec3 pixel);
        float Lamber(Vec3 pixel, Vec3 normal);

        Uint32 addBrightness(SDL_Surface* surface, int r, int g, int b, float brightness);

        float rayMarch(Vec3 cameraPos, Vec3 pixel, float coneAngle);

        Vec3 pos;
        float cos_inner;
        Vec3 dir;
        float cos_outer;
        float rmax;
        float intensity;
        Color color;
};