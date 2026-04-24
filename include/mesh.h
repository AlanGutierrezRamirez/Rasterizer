#pragma once
#include <vector>

#include <SDL3/SDL.h>

#include "vector_3.h"
#include "camera.h"
#include "light.h"
#include "draw.h"

struct Vertex
{
    Vec3 pos;
    Vec3 normal;
    Vec3 UV;
};

struct Triangle
{
    Vertex a, b, c;
};

struct TransformedTriangle {
    Vec3 a, b, c;
    ProjectedPoint pa, pb, pc;
    bool culled;
};

class Mesh
{
    public:
        Mesh() {};
        ~Mesh() {};

        void loadOBJ(const char* filename);

        void render(SDL_Surface* surface, Camera cam, SpotLight light, Color color);
    
    private:
        std::vector<Vec3> tempPos;
        std::vector<Vec3> tempNorms;
        std::vector<Vec3> tempUVs;
        std::vector<Triangle> mesh;
        std::vector<TransformedTriangle> workBuffer; 


};