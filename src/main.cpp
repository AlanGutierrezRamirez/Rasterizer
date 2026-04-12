#include <SDL3/SDL.h>
#include <iostream>

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

    bool running = true;
    SDL_Event event;

    while (running) {
        // 1. Procesar eventos
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }
        // 2. Actualizar lógica (por ahora vacío)
        // 3. Dibujar (por ahora vacío)
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}