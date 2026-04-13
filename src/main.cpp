#include <SDL3/SDL.h>
#include <iostream>
#include <cmath>

#define WIDTH 800
#define HEIGHT 600
#define SCALE 200
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
    return {
        (int)(vertice.x / vertice.z * SCALE) + HALF_WIDTH,
        (int)(vertice.y / vertice.z * SCALE) + HALF_HEIGHT
    };
}
Point3D rotateX(Point3D point, float angle)
{

    Point3D newPoint;

    newPoint.x = point.x;
    newPoint.y = point.y * cos(angle) - point.z * sin(angle);
    newPoint.z = point.y * sin(angle) + point.z * cos(angle);

    return newPoint;

}

Point3D rotateY(Point3D point, float angle)
{

    Point3D newPoint;

    newPoint.x = point.x * cos(angle) - point.z * sin(angle);
    newPoint.y = point.y;
    newPoint.z = point.x * sin(angle) + point.z * cos(angle);

    return newPoint;

}

Point3D rotateZ(Point3D point, float angle)
{

    Point3D newPoint;

    newPoint.x = point.x * cos(angle) - point.y * sin(angle);
    newPoint.y = point.x * sin(angle) + point.y * cos(angle);
    newPoint.z = point.z;

    return newPoint;

}

struct Edge {
    int start, end;
};

struct Face {
    int a, b, c, d;
};


Point3D crossProduct(Point3D v1, Point3D v2)
{
    Point3D cross;
    cross.x = v1.y * v2.z - v1.z * v2.y;
    cross.y = v1.z * v2.x - v1.x * v2.z;
    cross.z = v1.x * v2.y - v1.y * v2.x;

    return cross;
}

float dotProduct(Point3D v1, Point3D v2)
{

    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    
}

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Error al iniciar SDL3: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Rasterizer",
        WIDTH, HEIGHT,
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

    Point3D cube_vertices[8] = 
    {
        { -1,  1,  1 }, 
        {  1,  1,  1 }, 
        { -1,  1, -1 },
        {  1,  1, -1 },
        { -1, -1,  1 },
        {  1, -1,  1 },
        { -1, -1, -1 },
        {  1, -1, -1 }
    };

    Edge edges[12] = 
    {
        {0, 1}, {0, 2}, {0, 4}, 
        {1, 3}, {2, 3}, {1, 5},
        {4, 5}, {5, 7}, {4, 6}, 
        {6, 7}, {6, 2}, {7, 3},
    };

    Point3D new_vertices[8];

    float angleX = 0.01;
    float angleY = 0.05;
    float angleZ = 0.005;

    Face faces[6] = 
    {
        {0, 4, 5, 1},  // frente  (z=1)
        {3, 7, 6, 2},  // atrás   (z=-1)
        {0, 1, 3, 2},  // arriba  (y=1)
        {4, 6, 7, 5},  // abajo   (y=-1)
        {0, 2, 6, 4},  // izquierda (x=-1)
        {1, 5, 7, 3},  // derecha (x=1)
    };


    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        SDL_FillSurfaceRect(surface, nullptr, SDL_MapSurfaceRGB(surface, 15, 15, 35));

        angleX += 0.01;
        angleY += 0.05;
        angleZ += 0.005;

        int i = 0;
        for(Point3D point : cube_vertices)
        {
            Point3D new_point = rotateX(point, angleX);
            new_point = rotateY(new_point, angleY);
            new_point = rotateZ(new_point, angleZ);
            new_point.z += 3;

            new_vertices[i] = new_point;
            i++;
        }

        for(Face face : faces)
        {
            Point3D a = new_vertices[face.a];
            Point3D b = new_vertices[face.b];
            Point3D c = new_vertices[face.c];
            Point3D d = new_vertices[face.d];

            Point3D v1 = { b.x - a.x, b.y - a.y, b.z - a.z };
            Point3D v2 = { c.x - a.x, c.y - a.y, c.z - a.z };

            Point3D normal = crossProduct(v1, v2);

            if(normal.z < 0)
            {

                Vertex one = projectPoint(a);
                Vertex two = projectPoint(b);
                Vertex three = projectPoint(c);
                Vertex four = projectPoint(d);
                fillTriangle(surface, one, two, three, cyan);
                fillTriangle(surface, one, three, four, cyan);
            }

        }


        SDL_UpdateWindowSurface(window);

    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}