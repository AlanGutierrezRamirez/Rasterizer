#include "engine_math.h"
#include <cmath>

Point3D crossProduct(Point3D v1, Point3D v2)
{
    return {
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    };
}

float dotProduct(Point3D v1, Point3D v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Point3D normalize(Point3D v)
{
    float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return {v.x / length, v.y / length, v.z / length};
}

Point3D rotateX(Point3D point, float angle)
{
    return {
        point.x,
        point.y * cos(angle) - point.z * sin(angle),
        point.y * sin(angle) + point.z * cos(angle)
    };
}

Point3D rotateY(Point3D point, float angle)
{
    return {
        point.x * cos(angle) - point.z * sin(angle),
        point.y,
        point.x * sin(angle) + point.z * cos(angle)
    };
}

Point3D rotateZ(Point3D point, float angle)
{
    return {
        point.x * cos(angle) - point.y * sin(angle),
        point.x * sin(angle) + point.y * cos(angle),
        point.z
    };
}
