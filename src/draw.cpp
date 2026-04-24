#include <iostream>

#include "draw.h"
#include "zbuffer.h"
#include <algorithm>
#include <cmath>
#include "vector_3.h"
#include "light.h"

ProjectedPoint Draw::projectPoint(Vec3 vertex)
{
    return {
        (int)(vertex.x / vertex.z * SCALE) + HALF_WIDTH,
        (int)(-vertex.y / vertex.z * SCALE) + HALF_HEIGHT,
        vertex.z
    };
}

void Draw::putPixel(SDL_Surface* surface, int x, int y, Uint32 color)
{
    if (x < 0 || x >= surface->w || y < 0 || y >= surface->h) return;
    Uint32* pixels = (Uint32*)surface->pixels;
    pixels[x + surface->w * y] = color;
}

void Draw::drawLine(SDL_Surface* surface, int x0, int y0, int x1, int y1, Uint32 color)
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

void Draw::fillTriangle(SDL_Surface* surface, ProjectedPoint v_one, ProjectedPoint v_two, ProjectedPoint v_three, Uint32 color)
{
    orderVertices(v_one, v_two, v_three);

    for (int i = v_one.y; i < v_two.y; i++) {
        drawLine(surface, interpolateX(v_one, v_three, i), i, interpolateX(v_one, v_two, i), i, color);
    }
    for (int i = v_two.y; i < v_three.y; i++) {
        drawLine(surface, interpolateX(v_one, v_three, i), i, interpolateX(v_two, v_three, i), i, color);
    }
}

void Draw::fillTrianglePhong(SDL_Surface* surface, ProjectedPoint v_one, ProjectedPoint v_two, ProjectedPoint v_three, SpotLight light, Color color)
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
        float wxL = interpolate(A.worldPos.x, B.worldPos.x, y, A.y, B.y);
        float wyL = interpolate(A.worldPos.y, B.worldPos.y, y, A.y, B.y);
        float wzL = interpolate(A.worldPos.z, B.worldPos.z, y, A.y, B.y);
        float wxR = interpolate(C.worldPos.x, D.worldPos.x, y, C.y, D.y);
        float wyR = interpolate(C.worldPos.y, D.worldPos.y, y, C.y, D.y);
        float wzR = interpolate(C.worldPos.z, D.worldPos.z, y, C.y, D.y);
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

            Vec3 worldPos{
                interpolate(wxL, wxR, j, left, right),
                interpolate(wyL, wyR, j, left, right),
                interpolate(wzL, wzR, j, left, right)

            };

            Vec3 normal{
                interpolate(nxL, nxR, j, left, right),
                interpolate(nyL, nyR, j, left, right),
                interpolate(nzL, nzR, j, left, right)
            };

            float spotAtt = light.spotLight(worldPos, normal);

            float finalBrightness = spotAtt + 0.05f;
            putPixel(surface, j, y, addBrightness(surface, color.r, color.g, color.b,
                                                std::min(1.0f, finalBrightness)));
        }
    };

    for (int i = v_one.y; i < v_two.y; i++)
        scanSpan(i, v_one, v_two, v_one, v_three);

    for (int i = v_two.y; i < v_three.y; i++)
        scanSpan(i, v_one, v_three, v_two, v_three);


}

void Draw::fillTriangleGouraud(SDL_Surface* surface, ProjectedPoint v_one, ProjectedPoint v_two, ProjectedPoint v_three, SpotLight light, Color color)
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
        float wxL = interpolate(A.worldPos.x, B.worldPos.x, y, A.y, B.y);
        float wyL = interpolate(A.worldPos.y, B.worldPos.y, y, A.y, B.y);
        float wzL = interpolate(A.worldPos.z, B.worldPos.z, y, A.y, B.y);
        float wxR = interpolate(C.worldPos.x, D.worldPos.x, y, C.y, D.y);
        float wyR = interpolate(C.worldPos.y, D.worldPos.y, y, C.y, D.y);
        float wzR = interpolate(C.worldPos.z, D.worldPos.z, y, C.y, D.y);
        float zL  = interpolate(A.z, B.z, y, A.y, B.y);
        float zR  = interpolate(C.z, D.z, y, C.y, D.y);

        if (xL > xR) {
            std::swap(xL, xR);
            std::swap(nxL, nxR); std::swap(nyL, nyR); std::swap(nzL, nzR);
            std::swap(zL, zR);
            std::swap(wxL, wxR); std::swap(wyL, wyR); std::swap(wzL, wzR); 
        }

        int left = (int)xL, right = (int)xR;
        if (right <= left) return; 
            Vec3 worldPosLeft {
                wxL, wyL, wzL
            };

            Vec3 worldPosRight {
                wxR, wyR, wzR
            };


            Vec3 normalLeft = {
                nxL, nyL, nzL
            };

            Vec3 normalRight = {
                nxR, nyR, nzR
            };

        float brightL = light.spotLight(worldPosLeft, normalLeft);
        float brightR = light.spotLight(worldPosRight, normalRight);

        int jStart = std::max(left, 0);
        int jEnd   = std::min(right, WIDTH);

        float brightnessD = (brightR - brightL) / (right - left);
        float brightness = brightL;

        float dz = (zR - zL) / (right - left);  

        float skipped = (float)(jStart - left);
        float z = zL + dz * skipped;
        brightness = brightL + brightnessD * skipped;

        for (int j = jStart; j < jEnd; j++)
        {
            if (z < zBuffer[y * WIDTH + j]) {
                zBuffer[y * WIDTH + j] = z;
                putPixel(surface, j, y, addBrightness(surface, color.r, color.g, color.b,
                                                 std::min(1.0f, brightness + 0.2f)));
            }
            z += dz;
            brightness += brightnessD;
        }
    };

    for (int i = v_one.y; i < v_two.y; i++)
    {
        if (i < 0 || i >= HEIGHT) continue;
        scanSpan(i, v_one, v_two, v_one, v_three);
    }
      

    for (int i = v_two.y; i < v_three.y; i++)
    {
        if (i < 0 || i >= HEIGHT) continue;
        scanSpan(i, v_one, v_three, v_two, v_three);
    }

}
