#include "light.h"
#include "vector_3.h"
#include <algorithm>

#define STEP_SIZE 0.4f
#define SIGMA_S 0.2f
#define SIGMA_T 0.1f

float SpotLight::radians(float angle)
{
    float out = angle * M_PI / 180;
    return out;
}

float SpotLight::spotLight(Vec3 pixel, Vec3 normal)
{
    return intensity * distAttenuation(pixel) * angularAttenuation(pixel) * Lamber(pixel, normal);
}

float SpotLight::distAttenuation(Vec3 pixel)
{
    Vec3 pixelDir   = pixel - pos;
    float dSq        = Vec3::DotProduct(pixelDir, pixelDir);
    float rmaxSq    =  rmax * rmax;

    if(dSq >= rmaxSq) return 0.0f;

    float window = 1.0f - dSq / rmaxSq;
    return (window * window);
}

float SpotLight::angularAttenuation(Vec3 pixel)
{
    Vec3 toPixel = (pixel - pos).Normalized();

    float cos_theta = Vec3::DotProduct(dir, toPixel);

    if (cos_theta <= cos_outer) return 0.0f;

    if (cos_theta >= cos_inner) return 1.0f;

    return (cos_theta - cos_outer) / (cos_inner - cos_outer);
}

float SpotLight::Lamber(Vec3 pixel, Vec3 normal)
{
    Vec3 D = (pixel - pos).Normalized();
    Vec3 L(-D.x, -D.y, -D.z); 
    float lambert = std::max(0.0f, Vec3::DotProduct(normal, L));

    return lambert;
}

Uint32 SpotLight::addBrightness(SDL_Surface* surface, int r, int g, int b, float brightness)
{
    return SDL_MapSurfaceRGB(surface, r * brightness, g * brightness, b * brightness);
}

// light.cpp
float SpotLight::rayMarch(Vec3 rayOrigin, Vec3 rayDir,
                          Vec3 lightPos, Vec3 lightDir,
                          float tMax)
{
    float fogAmount = 0.0f;
    float t = 0.0f;

    while (t < tMax) {
        Vec3 samplePos = rayOrigin + rayDir * t;

        Vec3 lightToSample = samplePos - lightPos;
        float distToLight  = lightToSample.Magnitude();

        Vec3 toSample = (distToLight > 1e-5f)
                      ? lightToSample / distToLight
                      : Vec3::zero;

        float cosAngle = Vec3::DotProduct(lightDir, toSample);

        if (cosAngle >= cos_outer) {
            float edge = (cosAngle >= cos_inner)
                ? 1.0f
                : (cosAngle - cos_outer) / (cos_inner - cos_outer);

            if (distToLight < rmax) {
                float window  = 1.0f - (distToLight * distToLight)
                                     / (rmax * rmax);
                float distAtt = window * window;

                float transmittance = expf(-SIGMA_T * t);

                fogAmount += transmittance * SIGMA_S * STEP_SIZE
                           * edge * distAtt * intensity;
            }
        }

        t += STEP_SIZE;
    }

    return fogAmount;
}