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

struct Vertex
{
    int x, y;
};

void orderVertices(Vertex &top, Vertex &mid, Vertex &bot)
{
    if (top.y > mid.y) std::swap(top, mid);
    if (top.y > bot.y) std::swap(top, bot);
    if (mid.y > bot.y) std::swap(mid, bot);
}

int interpolateX(Vertex v_one, Vertex v_two, int i)
{
    return v_one.x + (i - v_one.y) * (v_two.x - v_one.x) / (v_two.y - v_one.y);
}

void fillTriangle(SDL_Surface *surface, Vertex v_one, Vertex v_two, Vertex v_three, Uint32 color)
{
    orderVertices(v_one, v_two, v_three);

    for(int i = v_one.y; i<v_two.y; i++)
    {
        int x = interpolateX(v_one, v_three, i);
        int x1 = interpolateX(v_one, v_two, i); 
        drawLine(surface, x, i, x1, i, color);
    }
    for(int i=v_two.y; i<v_three.y; i++)
    {
        int x = interpolateX(v_one, v_three, i);
        int x1 = interpolateX(v_two, v_three, i); 
        drawLine(surface, x, i, x1, i, color);
    }
}

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
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

        Vertex one = {400, 150};
        Vertex two = {200, 450};
        Vertex three = {600, 450};

        fillTriangle(surface, one, two, three, cyan);

        SDL_UpdateWindowSurface(window);

    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}