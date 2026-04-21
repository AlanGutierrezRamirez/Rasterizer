#include "camera.h"
#include <SDL3/SDL.h>

Vec3 Camera::GetForward()
{
    Vec3 forward = target - pos;
    return forward.Normalized();
}

Vec3 Camera::GetRight(Vec3 forward)
{
    return Vec3::CrossProduct(Vec3::up, forward).Normalized();
}

void Camera::ProcessInput(float deltaTime)
{
    const bool* keys = SDL_GetKeyboardState(nullptr);

    Vec3 forward = GetForward();
    Vec3 right = GetRight(forward);

    float distance = speed * deltaTime;

    if (keys[SDL_SCANCODE_W]) {
       Translate(forward * distance);
    }
    if (keys[SDL_SCANCODE_S]) {
       Translate(forward * -distance);
    }
    if (keys[SDL_SCANCODE_D]) {
       Translate(right * distance);
    }
    if (keys[SDL_SCANCODE_A]) {
        Translate(right * -distance);
    }

}
Mat4 Camera::GetViewMatrix()
{
     return Mat4::LookAt(pos, target, Vec3::up);
}

void Camera::Translate(const Vec3& displacement)
{
    pos    += displacement;
    target += displacement;
}