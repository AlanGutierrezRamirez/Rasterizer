#include "sphere.h"
#include <cmath>
#include "matrix.h"

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

void renderSphere(SDL_Surface* surface, SphereInstance& sphere, Point3D cameraDir)
{
    float offsetZ = 10.0f;

    Mat4 translate = MatTranslate(0, 0, 15);
    Mat4 transform = MatMult(translate, MatRotateY(sphere.accum));
    transform = MatMult(transform, MatTranslate(sphere.orbitRadius, 0, offsetZ));

    for (Triangle& triangle : sphere.sphere.triangles)
    {
        Point3D a = sphere.sphere.vertices[triangle.a];
        Point3D b = sphere.sphere.vertices[triangle.b];
        Point3D c = sphere.sphere.vertices[triangle.c];

        Point3D na = normalize(a);
        Point3D nb = normalize(b);
        Point3D nc = normalize(c);

        Vec4 av(a);
        Vec4 bv(b);
        Vec4 cv(c);

        av = Vec4Mult(transform, av);
        bv = Vec4Mult(transform, bv);
        cv = Vec4Mult(transform, cv);

        a.x = av.x; b.x = bv.x; c.x = cv.x;
        a.y = av.y; b.y = bv.y; c.y = cv.y;
        a.z = av.z; b.z = bv.z; c.z = cv.z;

        Point3D v1 = {b.x - a.x, b.y - a.y, b.z - a.z};
        Point3D v2 = {c.x - a.x, c.y - a.y, c.z - a.z};

        Point3D centroide = {
            (a.x + b.x + c.x) / 3.0f,
            (a.y + b.y + c.y) / 3.0f,
            (a.z + b.z + c.z) / 3.0f
        };

        Point3D faceNormal = crossProduct(v1, v2);
        Point3D toCamera = {-centroide.x, -centroide.y, -centroide.z};

        if (dotProduct(faceNormal, toCamera) < 0) continue;

        if (a.z < 0.5f || b.z < 0.5f || c.z < 0.5f) continue;

        ProjectedPoint one   = projectPoint(a);
        ProjectedPoint two   = projectPoint(b);
        ProjectedPoint three = projectPoint(c);

        one.normal   = na;
        two.normal   = nb;
        three.normal = nc;

        fillTrianglePhong(surface, one, two, three, light, sphere.color);
    }
    sphere.accum += sphere.angle;
}
