#include <SDL3/SDL.h>
#include <iostream>
#include <string>

#include "vector_3.h"
#include "config.h"
#include "lights.h"
#include "zbuffer.h"
#include "draw.h"
#include "sphere.h"

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

    sphereInsOne.orbitRadius = 3.0f;
    sphereInsTwo.orbitRadius = -3.0f;

    sphereInsOne.color = green_color;
    sphereInsTwo.color = red_color;

    sphereInsOne.angle = 0.005;
    sphereInsTwo.angle = -0.005;

    light.Normalize();

    Vec3 cameraPos = {0, 0, 0};
    Vec3 cameraTarget = {0, 0, 1};

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

        renderSphere(surface, sphereInsOne, cameraPos, cameraTarget);
        renderSphere(surface, sphereInsTwo, cameraPos, cameraTarget);

        SDL_UpdateWindowSurface(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
