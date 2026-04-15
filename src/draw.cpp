#include "draw.h"
#include "zbuffer.h"
#include <algorithm>
#include <cmath>
#include "vector_3.h"

ProjectedPoint projectPoint(Vec3 vertex)
{
    return {
        (int)(vertex.x / vertex.z * SCALE) + HALF_WIDTH,
        (int)(vertex.y / vertex.z * SCALE) + HALF_HEIGHT,
        vertex.z
    };
}

void putPixel(SDL_Surface* surface, int x, int y, Uint32 color)
{
    if (x < 0 || x >= surface->w || y < 0 || y >= surface->h) return;
    Uint32* pixels = (Uint32*)surface->pixels;
    pixels[x + surface->w * y] = color;
}

void drawLine(SDL_Surface* surface, int x0, int y0, int x1, int y1, Uint32 color)
{
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        putPixel(surface, x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx)  { err += dx; y0 += sy; }
    }
}

static void orderVertices(ProjectedPoint &top, ProjectedPoint &mid, ProjectedPoint &bot)
{
    if (top.y > mid.y) std::swap(top, mid);
    if (top.y > bot.y) std::swap(top, bot);
    if (mid.y > bot.y) std::swap(mid, bot);
}

static int interpolateX(ProjectedPoint a, ProjectedPoint b, int y)
{
    return a.x + (y - a.y) * (b.x - a.x) / (b.y - a.y);
}

static float interpolate(float start, float end, int y, int y_start, int y_end)
{
    if (y_start == y_end) return start;
    return start + (float)(y - y_start) * (end - start) / (float)(y_end - y_start);
}

void fillTriangle(SDL_Surface* surface, ProjectedPoint v_one, ProjectedPoint v_two, ProjectedPoint v_three, Uint32 color)
{
    orderVertices(v_one, v_two, v_three);

    for (int i = v_one.y; i < v_two.y; i++) {
        drawLine(surface, interpolateX(v_one, v_three, i), i, interpolateX(v_one, v_two, i), i, color);
    }
    for (int i = v_two.y; i < v_three.y; i++) {
        drawLine(surface, interpolateX(v_one, v_three, i), i, interpolateX(v_two, v_three, i), i, color);
    }
}

void fillTrianglePhong(SDL_Surface* surface, ProjectedPoint v_one, ProjectedPoint v_two, ProjectedPoint v_three, Vec3 light, Color color)
{
    orderVertices(v_one, v_two, v_three);

    auto scanSpan = [&](int y, ProjectedPoint A, ProjectedPoint B, ProjectedPoint C, ProjectedPoint D)
    {
        float xL  = interpolate(A.x, B.x, y, A.y, B.y);
        float xR  = interpolate(C.x, D.x, y, C.y, D.y);
        float nxL = interpolate(A.normal.x, B.normal.x, y, A.y, B.y);
        float nyL = interpolate(A.normal.y, B.normal.y, y, A.y, B.y);
        float nzL = interpolate(A.normal.z, B.normal.z, y, A.y, B.y);
        float nxR = interpolate(C.normal.x, D.normal.x, y, C.y, D.y);
        float nyR = interpolate(C.normal.y, D.normal.y, y, C.y, D.y);
        float nzR = interpolate(C.normal.z, D.normal.z, y, C.y, D.y);
        float zL  = interpolate(A.z, B.z, y, A.y, B.y);
        float zR  = interpolate(C.z, D.z, y, C.y, D.y);

        if (xL > xR) {
            std::swap(xL, xR);
            std::swap(nxL, nxR); std::swap(nyL, nyR); std::swap(nzL, nzR);
            std::swap(zL, zR);
        }

        int left = (int)xL, right = (int)xR;
        for (int j = left; j < right; j++)
        {
            if (j < 0 || j >= WIDTH || y < 0 || y >= HEIGHT) continue;

            float z = interpolate(zL, zR, j, left, right);
            if (z > zBuffer[y * WIDTH + j]) continue;
            zBuffer[y * WIDTH + j] = z;

            Vec3 normal = {
                interpolate(nxL, nxR, j, left, right),
                interpolate(nyL, nyR, j, left, right),
                interpolate(nzL, nzR, j, left, right)
            };

            normal.Normalize();

            float dot = Vec3::DotProduct(normal, light);
            putPixel(surface, j, y, addBrightness(surface, color.r, color.g, color.b, std::max(0.05f, dot)));
        }
    };

    for (int i = v_one.y; i < v_two.y; i++)
        scanSpan(i, v_one, v_two, v_one, v_three);

    for (int i = v_two.y; i < v_three.y; i++)
        scanSpan(i, v_one, v_three, v_two, v_three);
}
