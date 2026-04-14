#pragma once

struct Point3D
{
    float x, y, z;
};

struct Vec4
{
    float x, y, z, w;
    Vec4() : x(0), y(0), z(0), w(1) {}
    Vec4(Point3D p) : x(p.x), y(p.y), z(p.z), w(1) {}
};

Point3D crossProduct(Point3D v1, Point3D v2);
float   dotProduct(Point3D v1, Point3D v2);
Point3D normalize(Point3D v);

Point3D rotateX(Point3D point, float angle);
Point3D rotateY(Point3D point, float angle);
Point3D rotateZ(Point3D point, float angle);
