#pragma once

#include "engine_math.h"

struct Mat4{
    float mat[4][4];
};

Mat4 MatIdentity();

Vec4 Vec4Mult(Mat4 mat, Vec4 vec);

Mat4 MatMult(Mat4 matA, Mat4 matB);

Mat4 MatTranslate(float x, float y, float z);

Mat4 MatRotateX(float angle);

Mat4 MatRotateY(float angle);

Mat4 MatRotateZ(float angle);