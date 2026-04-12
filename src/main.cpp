#include <SDL3/SDL.h>
#include <iostream>

void putPixel(SDL_Surface* surface, int x, int y, Uint32 color) {
    if (x < 0 || x >= surface->w || y < 0 || y >= surface->h) return;

    Uint32* pixels = (Uint32*)surface->pixels;
    pixels[x + surface->w * y] = color;
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Error al iniciar SDL3: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Rasterizer",
        800, 600,
        SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        std::cerr << "Error al crear ventana: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Surface* surface = SDL_GetWindowSurface(window);

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        SDL_FillSurfaceRect(surface, nullptr, 0);

        Uint32 red = SDL_MapSurfaceRGB(surface, 255, 0, 0);
        putPixel(surface, 400, 300, red);

        SDL_UpdateWindowSurface(window);

    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}