#pragma once
#include "vector_3.h"
#include "matrix_4.h"

class Camera
{
    public:
        Camera(Vec3 _pos, Vec3 _target, float _speed) : pos(_pos), target(_target), speed(_speed) {};
        ~Camera() {};

        Vec3 GetForward();
        Vec3 GetRight(Vec3 forward);

        void ProcessInput(float deltaTime);

        Mat4 GetViewMatrix();

        Vec3 GetCameraPos() { return pos; };
        Vec3 GetCameraTarget() { return target; }

    private:
        void Translate(const Vec3& displacement);

    private:
        Vec3 pos;
        Vec3 target;
        float speed;

};