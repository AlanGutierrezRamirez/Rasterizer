#include <SDL3/SDL.h>
#include <iostream>

void putPixel(SDL_Surface* surface, int x, int y, Uint32 color) {
    if (x < 0 || x >= surface->w || y < 0 || y >= surface->h) return;

    Uint32* pixels = (Uint32*)surface->pixels;
    pixels[x + surface->w * y] = color;
}

void drawLine(SDL_Surface* surface, int x0, int y0, int x1, int y1, Uint32 color) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;  // dirección en X
    int sy = (y0 < y1) ? 1 : -1;  // dirección en Y
    int err = dx - dy;

    while (true) {
        putPixel(surface, x0, y0, color);

        if (x0 == x1 && y0 == y1) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
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

    Uint32 cyan = SDL_MapSurfaceRGB(surface, 0, 255, 200);

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        SDL_FillSurfaceRect(surface, nullptr, SDL_MapSurfaceRGB(surface, 15, 15, 35));

        drawLine(surface, 400, 150, 200, 450, cyan);  // punta → izquierda
        drawLine(surface, 200, 450, 600, 450, cyan);  // izquierda → derecha
        drawLine(surface, 600, 450, 400, 150, cyan);  // derecha → punta

        SDL_UpdateWindowSurface(window);

    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}