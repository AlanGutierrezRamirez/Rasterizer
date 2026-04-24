#include "camera.h"
#include <SDL3/SDL.h>
#include <algorithm>

Vec3 Camera::GetForward() const
{
    float cp = cosf(pitch);
    return Vec3(
        sinf(yaw) * cp,
        sinf(pitch),
        cosf(yaw) * cp
    ).Normalized();
}

Vec3 Camera::GetRight() const
{
    return Vec3::CrossProduct(Vec3::up, GetForward()).Normalized();
}
void Camera::ProcessInput(float deltaTime)
{
    const bool* keys = SDL_GetKeyboardState(nullptr);

    float rotStep = rotSpeed * deltaTime;
    if (keys[SDL_SCANCODE_LEFT])  yaw   -= rotStep;
    if (keys[SDL_SCANCODE_RIGHT]) yaw   += rotStep;
    if (keys[SDL_SCANCODE_UP])    pitch += rotStep;
    if (keys[SDL_SCANCODE_DOWN])  pitch -= rotStep;


    const float PITCH_LIMIT = 1.553f;
    pitch = std::clamp(pitch, -PITCH_LIMIT, PITCH_LIMIT);

    Vec3 forward = GetForward();
    Vec3 right   = GetRight();

    float distance = speed * deltaTime;

    if (keys[SDL_SCANCODE_W]) Translate(forward *  distance);
    if (keys[SDL_SCANCODE_S]) Translate(forward * -distance);
    if (keys[SDL_SCANCODE_D]) Translate(right   *  distance);
    if (keys[SDL_SCANCODE_A]) Translate(right   * -distance);
}

void Camera::ProcessMouse(float deltaX, float deltaY)
{
    yaw   += deltaX * mouseSensitivity;
    pitch -= deltaY * mouseSensitivity;

    const float PITCH_LIMIT = 1.553f;
    if (pitch >  PITCH_LIMIT) pitch =  PITCH_LIMIT;
    if (pitch < -PITCH_LIMIT) pitch = -PITCH_LIMIT;
}

Mat4 Camera::GetViewMatrix()
{
    return Mat4::LookAt(pos, pos + GetForward(), Vec3::up);
}

void Camera::Translate(const Vec3& displacement)
{
    pos += displacement;
}