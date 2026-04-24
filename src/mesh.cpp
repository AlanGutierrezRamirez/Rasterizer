#include "mesh.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <SDL3/SDL.h>
#include "light.h"
#include "matrix_4.h"
#include "draw.h"
#include "profiler.h"


void Mesh::loadOBJ(const char* filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "No se pudo abrir: " << filename << std::endl;
        return;
    }
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;
        
        if (prefix == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            tempPos.push_back({x, y, z});
        }
        if(prefix == "vn"){
            float x, y, z;
            iss >> x >> y >> z;
            tempNorms.push_back({x, y, z});
        }
        if(prefix == "vt"){
            float x, y, z;
            iss >> x >> y >> z;
            tempUVs.push_back({x, y, z});
        }
        if (prefix == "f") {
            std::vector<std::string> group;
            std::string token;
            while (iss >> token) {
                group.push_back(token);
            }

            std::vector<int> posIndices;
            for (int i = 0; i < group.size(); i++) {
                int slashPos = group[i].find('/');
                std::string numStr = group[i].substr(0, slashPos);
                int posIdx = std::stoi(numStr) - 1;
                posIndices.push_back(posIdx);
            }

            for (int i = 1; i < posIndices.size() - 1; i++) {
                Vertex vA = { tempPos[posIndices[0]],     Vec3::zero };
                Vertex vB = { tempPos[posIndices[i]],     Vec3::zero };
                Vertex vC = { tempPos[posIndices[i + 1]], Vec3::zero };
                mesh.push_back({ vA, vB, vC });
            }
        }
    }

    for (Triangle& tri : mesh) {
        Vec3 edge1 = tri.b.pos - tri.a.pos;
        Vec3 edge2 = tri.c.pos - tri.a.pos;
        Vec3 faceNormal = Vec3::CrossProduct(edge1, edge2).Normalized();
        
        tri.a.normal = faceNormal;
        tri.b.normal = faceNormal;
        tri.c.normal = faceNormal;
    }
    std::cout << "Cargado " << filename 
          << ": " << mesh.size() << " triangulos"
          << std::endl;
}

void Mesh::render(SDL_Surface* surface, Camera cam, SpotLight light, Color color)
{

    Mat4 view = cam.GetViewMatrix();

    workBuffer.resize(mesh.size());


    {
        ScopedTimer transformTimer("transform");
        for(size_t i=0; i<mesh.size(); i++)
        {
            const Triangle& tri = mesh[i];
            Vec4 av = view.Transform(Vec4(tri.a.pos, 1.0f));
            Vec4 bv = view.Transform(Vec4(tri.b.pos, 1.0f));
            Vec4 cv = view.Transform(Vec4(tri.c.pos, 1.0f));

            workBuffer[i].a = { av.x, av.y, av.z };
            workBuffer[i].b = { bv.x, bv.y, bv.z };
            workBuffer[i].c = { cv.x, cv.y, cv.z };

            workBuffer[i].culled = false;
        }
    }


    {
       ScopedTimer frustumCull("frustum_cull");

        for(size_t i=0; i<mesh.size(); i++)
        {
            const Vec3& a = workBuffer[i].a;
            const Vec3& b = workBuffer[i].b;
            const Vec3& c = workBuffer[i].c;


            if (a.z < 0.5f && b.z < 0.5f && c.z < 0.5f) { workBuffer[i].culled = true; continue; };

            if (a.x * SCALE > HALF_WIDTH * a.z &&
                b.x * SCALE > HALF_WIDTH * b.z &&
                c.x * SCALE > HALF_WIDTH * c.z) { workBuffer[i].culled = true; continue; }; 

            if (a.x * SCALE < -HALF_WIDTH * a.z &&
                b.x * SCALE < -HALF_WIDTH * b.z &&
                c.x * SCALE < -HALF_WIDTH * c.z) { workBuffer[i].culled = true; continue; };

            if (a.y * SCALE > HALF_WIDTH * a.z &&
                b.y * SCALE > HALF_WIDTH * b.z &&
                c.y * SCALE > HALF_WIDTH * c.z) { workBuffer[i].culled = true; continue; };

            if (a.y * SCALE < -HALF_WIDTH * a.z &&
                b.y * SCALE < -HALF_WIDTH * b.z &&
                c.y * SCALE < -HALF_WIDTH * c.z) { workBuffer[i].culled = true; continue; };
        }

    }

    {

        ScopedTimer backfaceCulling("backface_cull");

        for(size_t i=0; i<mesh.size(); i++)
        {
            const Vec3& a = workBuffer[i].a;
            const Vec3& b = workBuffer[i].b;
            const Vec3& c = workBuffer[i].c;

            Vec3 edge1 = b - a;
            Vec3 edge2 = c - a;
            Vec3 faceNormal = Vec3::CrossProduct(edge1, edge2);

            Vec3 centroid = { (a.x+b.x+c.x)/3, (a.y+b.y+c.y)/3, (a.z+b.z+c.z)/3 };
            Vec3 toCamera = { -centroid.x, -centroid.y, -centroid.z };

            if (Vec3::DotProduct(faceNormal, toCamera) < 0) { workBuffer[i].culled = true; continue; };
            if (a.z < 0.5f || b.z < 0.5f || c.z < 0.5f)  {workBuffer[i].culled = true; continue; };
        }

    }

    {

        ScopedTimer project("project");

        for(size_t i=0; i<mesh.size(); i++)
        {
            TransformedTriangle& curTriangle =  workBuffer[i];

            if(curTriangle.culled) continue;

            const Vec3& a = curTriangle.a;
            const Vec3& b = curTriangle.b;
            const Vec3& c = curTriangle.c;

            curTriangle.pa = Draw::projectPoint(a);
            curTriangle.pb = Draw::projectPoint(b);
            curTriangle.pc = Draw::projectPoint(c);

            curTriangle.pa.normal = mesh[i].a.normal;
            curTriangle.pb.normal  = mesh[i].b.normal;
            curTriangle.pc.normal  = mesh[i].c.normal;

            curTriangle.pa.worldPos = mesh[i].a.pos;
            curTriangle.pb.worldPos = mesh[i].b.pos;
            curTriangle.pc.worldPos = mesh[i].c.pos;

        }
    }

    {
        ScopedTimer fill("fill");
        for(size_t i=0; i<mesh.size(); i++)
        {
            TransformedTriangle& curTriangle =  workBuffer[i];
            Draw::fillTriangleGouraud(surface, curTriangle.pa, curTriangle.pb, curTriangle.pc, light, color);

        }
        
    }


}