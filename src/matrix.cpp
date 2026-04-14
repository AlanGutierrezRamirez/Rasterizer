#include "matrix.h"
#include <math.h>
Mat4 MatIdentity()
{
    Mat4 mathId = {{
        {1, 0, 0, 0}, 
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    }};

    return mathId;
};

Vec4 Vec4Mult(Mat4 mat, Vec4 vec)
{
    Vec4 final_vec;

    final_vec.x = vec.x*mat.mat[0][0] + vec.y*mat.mat[0][1] + vec.z*mat.mat[0][2] + vec.w*mat.mat[0][3];
    final_vec.y = vec.x*mat.mat[1][0] + vec.y*mat.mat[1][1] + vec.z*mat.mat[1][2] + vec.w*mat.mat[1][3];
    final_vec.z = vec.x*mat.mat[2][0] + vec.y*mat.mat[2][1] + vec.z*mat.mat[2][2] + vec.w*mat.mat[2][3];
    final_vec.w = vec.x*mat.mat[3][0] + vec.y*mat.mat[3][1] + vec.z*mat.mat[3][2] + vec.w*mat.mat[3][3];

    return final_vec;

}

Mat4 MatMult(Mat4 matA, Mat4 matB)
{
    Mat4 result;

    for(int i=0; i < 4; i++)
    {
        for(int j=0; j < 4; j++)
        {
           result.mat[i][j] = matA.mat[i][0]*matB.mat[0][j] 
             + matA.mat[i][1]*matB.mat[1][j] 
             + matA.mat[i][2]*matB.mat[2][j] 
             + matA.mat[i][3]*matB.mat[3][j];
        }
    }
    return result;
}

Mat4 MatTranslate(float x, float y, float z)
{

    Mat4 result = MatIdentity();

    result.mat[0][3] = x;
    result.mat[1][3] = y;
    result.mat[2][3] = z;

    return result;
}

Mat4 MatRotateX(float angle)
{
    Mat4 result = MatIdentity();

    result.mat[1][1] = cos(angle);
    result.mat[1][2] = sin(angle);
    result.mat[2][1] = -sin(angle);
    result.mat[2][2] = cos(angle);

    return result;
}

Mat4 MatRotateY(float angle)
{
    Mat4 result = MatIdentity();

    result.mat[0][0] = cos(angle);
    result.mat[2][0] = sin(angle);
    result.mat[0][2] = -sin(angle);
    result.mat[2][2] = cos(angle);

    return result;
}

Mat4 MatRotateZ(float angle)
{
    Mat4 result = MatIdentity();

    result.mat[0][0] = cos(angle);
    result.mat[0][1] = sin(angle);
    result.mat[1][0] = -sin(angle);
    result.mat[1][1] = cos(angle);

    return result;
}
