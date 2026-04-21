#pragma once
#include <vector>
#include "vector_3.h"

class Mesh
{
    public:
        Mesh() {};
        ~Mesh() {};

        void loadOBJ(const char* filename);

    private:
        std::vector<Vec3> vertices;
        std::vector<Vec3> faces;
        std::vector<Vec3> normals;

};