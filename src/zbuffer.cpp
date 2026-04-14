#include "zbuffer.h"
#include <algorithm>

float zBuffer[WIDTH * HEIGHT];

void resetZBuffer()
{
    std::fill(zBuffer, zBuffer + WIDTH * HEIGHT, INFINITY);
}
