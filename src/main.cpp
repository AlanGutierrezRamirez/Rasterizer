#include <SDL3/SDL.h>
#include <iostream>

#define WIDTH 800
#define HEIGHT 600
#define SCALE 2
#define HALF_WIDTH 400
#define HALF_HEIGHT 300

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

struct Point3D
{
    float x, y, z;
};

Vertex projectPoint(Point3D vertice)
{
    return {(int)(vertice.x/vertice.z) * SCALE + HALF_WIDTH, (int)(vertice.y/vertice.z) * SCALE + HALF_HEIGHT};
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

        Point3D top_left_back = { -100, 100, 15 };
        Point3D top_right_back = { 100, 100, 15 };
        Point3D top_left_front = { -100, 100, 8 };
        Point3D top_right_front = { 100, 100, 8 };
        Point3D bottom_left_back = { -100, -100, 15 };
        Point3D bottom_right_back = { 100, -100, 15 };
        Point3D bottom_left_front = {-100, -100, 8};
        Point3D bottom_right_front = {100, -100, 8};

        Vertex tlb = projectPoint(top_left_back);
        Vertex trb = projectPoint(top_right_back);
        Vertex tlf = projectPoint(top_left_front);
        Vertex trf = projectPoint(top_right_front);
        Vertex blb = projectPoint(bottom_left_back);
        Vertex brb = projectPoint(bottom_right_back);
        Vertex blf = projectPoint(bottom_left_front);
        Vertex brf = projectPoint(bottom_right_front);


        drawLine(surface, tlb.x, tlb.y, trb.x, trb.y, cyan);
        drawLine(surface, trb.x, trb.y, trf.x, trf.y, cyan);
        drawLine(surface, tlb.x, tlb.y, tlf.x, tlf.y, cyan);
        drawLine(surface, tlf.x, tlf.y, trf.x, trf.y, cyan);
        drawLine(surface, tlb.x, tlb.y, blb.x, blb.y, cyan);
        drawLine(surface, trb.x, trb.y, brb.x, brb.y, cyan);
        drawLine(surface, tlf.x, tlf.y, blf.x, blf.y, cyan);
        drawLine(surface, trf.x, trf.y, brf.x, brf.y, cyan);
        drawLine(surface, blb.x, blb.y, brb.x, brb.y, cyan);
        drawLine(surface, blb.x, blb.y, blf.x, blf.y, cyan);
        drawLine(surface, brb.x, brb.y, brf.x, brf.y, cyan);
        drawLine(surface, blf.x, blf.y, brf.x, brf.y, cyan);


        SDL_UpdateWindowSurface(window);

    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}