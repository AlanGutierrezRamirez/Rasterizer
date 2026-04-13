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

struct Point3D
{
    float x, y, z;
};

struct Vertex
{
    int x, y;
    Point3D normal;
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

float interpolate(float start, float end, int y, int y_start, int y_end)
{
    if (y_start == y_end) return start;
    return start + (float)(y - y_start) * (end - start) / (float)(y_end - y_start);
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

struct Color
{
    int r, g, b;
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

Point3D normalize(Point3D v)
{
    float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return {v.x/length, v.y/length, v.z/length};
}

Uint32 addBrightness(SDL_Surface *surface, int r, int g, int b, float brightness)
{
    return SDL_MapSurfaceRGB(surface, r * brightness, g * brightness, b * brightness);
}

void fillTrianglePhong(SDL_Surface *surface, Vertex v_one, Vertex v_two, Vertex v_three, Point3D light, Color color)
{
    orderVertices(v_one, v_two, v_three);

    for(int i = v_one.y; i<v_two.y; i++)
    {
        float xL = interpolate(v_one.x, v_two.x, i, v_one.y, v_two.y);
        float nxL = interpolate(v_one.normal.x, v_two.normal.x, i, v_one.y, v_two.y);
        float nyL = interpolate(v_one.normal.y, v_two.normal.y, i, v_one.y, v_two.y);
        float nzL = interpolate(v_one.normal.z, v_two.normal.z, i, v_one.y, v_two.y);

        float xR = interpolate(v_one.x, v_three.x, i, v_one.y, v_three.y);
        float nxR = interpolate(v_one.normal.x, v_three.normal.x, i, v_one.y, v_three.y);
        float nyR = interpolate(v_one.normal.y, v_three.normal.y, i, v_one.y, v_three.y);
        float nzR = interpolate(v_one.normal.z, v_three.normal.z, i, v_one.y, v_three.y);

        int left = std::min(xL, xR);
        int right = std::max(xL, xR);

        for(int j = left; j < right; j++)
        {

            float nx = interpolate(nxL, nxR, j, left, right);
            float ny = interpolate(nyL, nyR, j, left, right);
            float nz = interpolate(nzL, nzR, j, left, right);

            Point3D normal = {nx, ny, nz};

            normal = normalize(normal);

            float dot = dotProduct(normal, light);

            Uint32 bright_cyan = addBrightness(surface, color.r, color.g, color.b, std::max(0.0f, dot));

            putPixel(surface, j, i, bright_cyan);
    
        }
    }

    for(int i=v_two.y; i<v_three.y; i++)
    {
        float xL = interpolate(v_one.x, v_three.x, i, v_one.y, v_three.y);
        float nxL = interpolate(v_one.normal.x, v_three.normal.x, i, v_one.y, v_three.y);
        float nyL = interpolate(v_one.normal.y, v_three.normal.y, i, v_one.y, v_three.y);
        float nzL = interpolate(v_one.normal.z, v_three.normal.z, i, v_one.y, v_three.y);

        float xR = interpolate(v_two.x, v_three.x, i, v_two.y, v_three.y);
        float nxR = interpolate(v_two.normal.x, v_three.normal.x, i, v_two.y, v_three.y);
        float nyR = interpolate(v_two.normal.y, v_three.normal.y, i, v_two.y, v_three.y);
        float nzR = interpolate(v_two.normal.z, v_three.normal.z, i, v_two.y, v_three.y);

        int left = std::min(xL, xR);
        int right = std::max(xL, xR);

        for(int j = left; j < right; j++)
        {

            float nx = interpolate(nxL, nxR, j, left, right);
            float ny = interpolate(nyL, nyR, j, left, right);
            float nz = interpolate(nzL, nzR, j, left, right);

            Point3D normal = {nx, ny, nz};

            normal = normalize(normal);

            float dot = dotProduct(normal, light);

            Uint32 bright_cyan = addBrightness(surface, color.r, color.g, color.b, std::max(0.0f, dot));

            putPixel(surface, j, i, bright_cyan);
    
        }
    }

}

const int latitud = 32;
const int longitud = 64;
const int total_points = (latitud + 1) * longitud;
const int total_faces = (latitud - 1) * longitud;

Point3D  sphere[total_points];

struct Triangle{
    int a, b, c;
};

Triangle sphere_triangles[total_faces * 2];

void Sphere(int r)
{
    int cur = 0;
    for(int i=0; i<=latitud; i++)
    {
        float theta = (float)i / latitud * M_PI;
        for(int j=0; j<longitud; j++)
        {
            float phi = (float)j / longitud * 2 * M_PI;
            float x = r * sin(theta) * cos(phi);
            float y = r * cos(theta);
            float z = r * sin(theta) * sin(phi);
            sphere[i * longitud + j] = {x, y, z + 5.0f};
        }
    }

    int tri = 0;
    for(int i = 0; i < latitud; i++)
    {
        for(int j = 0; j < longitud; j++)
        {

            int current = i * longitud + j;
            int right = i * longitud + (j + 1) % longitud;
            int bottom = (i + 1) * longitud + j;
            int bottom_right = (i + 1) * longitud + (j + 1) % longitud;

            sphere_triangles[tri++] = {current, right, bottom};
            sphere_triangles[tri++] = {right, bottom_right, bottom};
        }
    }


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

    SDL_Event event;

    Point3D light = {-1, 0, 0};

    Color cyan_color = {0, 255, 200};

    int radius = 2;

    Sphere(radius);

    float deltaTime = 0;
    Uint64 currentTime = 0;
    Uint64 previousTime = SDL_GetPerformanceCounter();
    float frequency = SDL_GetPerformanceFrequency();
    
    float update = 0.5f;
    float accumulator = 0;

    bool running = true;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        currentTime = SDL_GetPerformanceCounter();
        deltaTime = (currentTime - previousTime) / frequency;
        float fps = 1.0/deltaTime;
        previousTime = currentTime;


        accumulator += deltaTime;
        if(accumulator > update)
        {
            std::string title = "Rasterizer: " + std::to_string(fps) + " FPS";
            SDL_SetWindowTitle(window, title.c_str());
            accumulator = 0;
        }

        SDL_FillSurfaceRect(surface, nullptr, SDL_MapSurfaceRGB(surface, 15, 15, 35));


        for(Triangle triangle : sphere_triangles)
        {

            Point3D a = sphere[triangle.a];
            Point3D b = sphere[triangle.b];
            Point3D c = sphere[triangle.c];

            Point3D v1 = {b.x - a.x, b.y - a.y, b.z - a.z};
            Point3D v2 = {c.x - a.x, c.y - a.y, c.z - a.z};
            Point3D faceNormal = crossProduct(v1, v2);

            if (faceNormal.z < 0) continue;

            Vertex one = projectPoint(a);
            Vertex two = projectPoint(b);
            Vertex three = projectPoint(c);

            one.normal = {a.x, a.y, a.z - 5.0f};
            two.normal = {b.x, b.y, b.z - 5.0f};
            three.normal = {c.x, c.y, c.z - 5.0f};

            fillTrianglePhong(surface, one, two, three, light, cyan_color);
        }

        SDL_UpdateWindowSurface(window);

    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}