#pragma once
#include "vector_3.h"
#include "matrix_4.h"

class Camera
{
    public:
       Camera(Vec3 _pos, float _yaw, float _pitch, float _speed)
            : pos(_pos), yaw(_yaw), pitch(_pitch), speed(_speed) {};

        Vec3 GetForward() const;
        Vec3 GetRight() const;

        void ProcessInput(float deltaTime);
        void ProcessMouse(float deltaX, float deltaY);

        Mat4 GetViewMatrix();

        Vec3 GetCameraPos()    const { return pos; }
        Vec3 GetCameraTarget() const { return pos + GetForward(); }

    private:
        void Translate(const Vec3& displacement);

    private:
        Vec3  pos;
        float yaw;    
        float pitch;  
        float speed;
        float rotSpeed = 2.0f; 
        float mouseSensitivity = 0.002f;
};