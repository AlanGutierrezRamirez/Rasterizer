#include "sphere.h"
#include <cmath>
#include "vector_3.h"
#include "vector_4.h"
#include "matrix_4.h"
#include "light.h"

void InitSphere(Sphere& sphere, float r)
{
    for (int i = 0; i <= sphere.latitud; i++)
    {
        float theta = (float)i / sphere.latitud * M_PI;
        for (int j = 0; j < sphere.longitud; j++)
        {
            float phi = (float)j / sphere.longitud * 2 * M_PI;
            sphere.vertices[i * sphere.longitud + j] = {
                r * sin(theta) * cos(phi),
                r * cos(theta),
                r * sin(theta) * sin(phi)
            };
        }
    }

    int tri = 0;
    for (int i = 0; i < sphere.latitud; i++)
    {
        for (int j = 0; j < sphere.longitud; j++)
        {
            int current      = i * sphere.longitud + j;
            int right        = i * sphere.longitud + (j + 1) % sphere.longitud;
            int bottom       = (i + 1) * sphere.longitud + j;
            int bottom_right = (i + 1) * sphere.longitud + (j + 1) % sphere.longitud;

            sphere.triangles[tri++] = {current, right, bottom};
            sphere.triangles[tri++] = {right, bottom_right, bottom};
        }
    }
}

void renderSphere(SDL_Surface* surface, SphereInstance& sphere, Vec3 cameraPos, Vec3 cameraTarget, SpotLight light)
{

    Mat4 model = Mat4::Translate(sphere.pos);

    Mat4 lookAt = Mat4::LookAt(cameraPos, cameraTarget, Vec3::up);
    
    Mat4 mv = Mat4().Identity();
    mv = mv.Multiply(lookAt);
    mv = mv.Multiply(model);
    
    for (Triangle& triangle : sphere.sphere.triangles)
    {
        Vec3 a = sphere.sphere.vertices[triangle.a];
        Vec3 b = sphere.sphere.vertices[triangle.b];
        Vec3 c = sphere.sphere.vertices[triangle.c];

        Vec4 awVec = model.Transform(Vec4(a, 1.0f));
        Vec4 bwVec = model.Transform(Vec4(b, 1.0f));
        Vec4 cwVec = model.Transform(Vec4(c, 1.0f));
        Vec3 aWorld = { awVec.x, awVec.y, awVec.z };
        Vec3 bWorld = { bwVec.x, bwVec.y, bwVec.z };
        Vec3 cWorld = { cwVec.x, cwVec.y, cwVec.z };

        Vec3 na = a.Normalized();
        Vec3 nb = b.Normalized();
        Vec3 nc = c.Normalized();

        Vec4 av = mv.Transform(Vec4(a, 1.0f));
        Vec4 bv = mv.Transform(Vec4(b, 1.0f));  
        Vec4 cv = mv.Transform(Vec4(c, 1.0f));

        a.x = av.x; b.x = bv.x; c.x = cv.x;
        a.y = av.y; b.y = bv.y; c.y = cv.y;
        a.z = av.z; b.z = bv.z; c.z = cv.z;

        Vec3 v1 = {b.x - a.x, b.y - a.y, b.z - a.z};
        Vec3 v2 = {c.x - a.x, c.y - a.y, c.z - a.z};

        Vec3 centroide = {
            (a.x + b.x + c.x) / 3.0f,
            (a.y + b.y + c.y) / 3.0f,
            (a.z + b.z + c.z) / 3.0f
        };

        Vec3 faceNormal = Vec3::CrossProduct(v1, v2);
        Vec3 toCamera = {-centroide.x, -centroide.y, -centroide.z};

        if (Vec3::DotProduct(faceNormal, toCamera) < 0) continue;

        if (a.z < 0.5f || b.z < 0.5f || c.z < 0.5f) continue;

        ProjectedPoint one   = Draw::projectPoint(a);
        ProjectedPoint two   = Draw::projectPoint(b);
        ProjectedPoint three = Draw::projectPoint(c);

        one.normal   = na;
        two.normal   = nb;
        three.normal = nc;

        one.worldPos   = aWorld;
        two.worldPos   = bWorld;
        three.worldPos = cWorld;

        Draw::fillTrianglePhong(surface, one, two, three, light, sphere.color);
    }

    sphere.accum += sphere.angle;
}
