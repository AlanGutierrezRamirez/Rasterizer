#include <SDL3/SDL.h>
#include <iostream>
#include <string>
#include <algorithm>

#include "vector_3.h"
#include "config.h"
#include "zbuffer.h"
#include "draw.h"
#include "sphere.h"
#include "camera.h"
#include "light.h"

Vec3 pixelToWorld(int x, int y)
{
    return Vec3(
        (x - HALF_WIDTH) / (float)SCALE,
        (y - HALF_HEIGHT) / (float)SCALE,
        1
    );
}



int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Error al iniciar SDL3: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Rasterizer", WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "Error al crear ventana: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    SDL_Event event;

    Color red_color = {189, 17, 74};
    Color green_color {47, 160, 132};

    float radius = 2.0f;

    SphereInstance sphereInsOne;
    SphereInstance sphereInsTwo;

    InitSphere(sphereInsOne.sphere, radius);
    InitSphere(sphereInsTwo.sphere, radius);

    sphereInsOne.orbitRadius = 0.0f;
    sphereInsTwo.orbitRadius = 0.0f;

    sphereInsOne.color = green_color;
    sphereInsTwo.color = red_color;

    sphereInsOne.angle = 0.0;
    sphereInsTwo.angle = -0.0;

    sphereInsOne.pos = {0.0f, 0.0f, 10.0f};
    sphereInsTwo.pos = { 5.0f, 0.0f, 10.0f};


    float cos_in  = cosf(SpotLight::radians(15));
    float cos_out = cosf(SpotLight::radians(20));

    Vec3 spotLightPos = {-20, -15, 0};

    Vec3 spotLightDir = sphereInsOne.pos - spotLightPos;

    SpotLight spotLight(spotLightPos, cos_in, spotLightDir, cos_out, 40, 10, {225, 120, 120});

    Camera camera({0, 0, 0 }, {0, 0, 1}, 3.0f);

    float   deltaTime    = 0;
    Uint64  currentTime  = 0;
    Uint64  previousTime = SDL_GetPerformanceCounter();
    float   frequency    = SDL_GetPerformanceFrequency();
    float   update       = 0.5f;
    float   accumulator  = 0;

    bool running = true;
    while (running)
    {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = false;
        }

        currentTime = SDL_GetPerformanceCounter();
        deltaTime   = (currentTime - previousTime) / frequency;
        float fps   = 1.0f / deltaTime;
        previousTime = currentTime;

        accumulator += deltaTime;
        if (accumulator > update) {
            SDL_SetWindowTitle(window, ("Rasterizer: " + std::to_string(fps) + " FPS").c_str());
            accumulator = 0;
        }

        SDL_FillSurfaceRect(surface, nullptr, SDL_MapSurfaceRGB(surface, 15, 15, 35));
        resetZBuffer();

        camera.ProcessInput(deltaTime);
        renderSphere(surface, sphereInsOne, camera.GetCameraPos(), camera.GetCameraTarget(), spotLight);
        renderSphere(surface, sphereInsTwo, camera.GetCameraPos(), camera.GetCameraTarget(), spotLight);

        const SDL_PixelFormatDetails* fmt = SDL_GetPixelFormatDetails(surface->format);
        Uint32* pixels = (Uint32*)surface->pixels;

        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                Vec3 dir = pixelToWorld(x, y);
                float fogAmount = spotLight.rayMarch(camera.GetCameraPos(), dir, 30);
   
                float lr = spotLight.color.r / 255.0f;   // 0.88
                float lg = spotLight.color.g / 255.0f;   // 0.47
                float lb = spotLight.color.b / 255.0f;   // 0.47

                float fogStrength = 0.1f;  // ajustar a mano
                float fogR = fogAmount * fogStrength * lr * 255.0f;
                float fogG = fogAmount * fogStrength * lg * 255.0f;
                float fogB = fogAmount * fogStrength * lb * 255.0f;

                Uint8 rOld, gOld, bOld;
                SDL_GetRGB(pixels[y * WIDTH + x], 
                        SDL_GetPixelFormatDetails(surface->format), 
                        nullptr, &rOld, &gOld, &bOld);

                int rNew = std::min(255, (int)(rOld + fogR));
                int gNew = std::min(255, (int)(gOld + fogG));
                int bNew = std::min(255, (int)(bOld + fogB));

                pixels[y * WIDTH + x] = SDL_MapSurfaceRGB(surface, rNew, gNew, bNew);
                
                
            }
        }

        SDL_UpdateWindowSurface(window);

    }
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
