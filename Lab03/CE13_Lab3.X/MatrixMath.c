/*
 * File:   MatrixMath.c
 * Author: JustinT
 *
 * Created on January 26, 2018, 11:09 AM
 */


#include "xc.h"
#include "MatrixMath.h"
#include <math.h>

void MatrixMultiply(float mat1[3][3], float mat2[3][3], float result[3][3])
{
    int i, j, k;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            for (k = 0; k < 3; k++) {
                result[i][j] += mat1[i][k] * mat2[k][j];
            }
        }
    }
}

void MatrixAdd(float mat1[3][3], float mat2[3][3], float result[3][3])
{
    int i, j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            result[i][j] = mat1[i][j] + mat2[i][j];

        }
    }
}

int MatrixEquals(float mat1[3][3], float mat2[3][3])
{
    float difference, difference1;
    int i, j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            difference = mat1[i][j] - mat2[i][j];
            difference1 = fabs(difference);
            if (difference1 >= FP_DELTA) {
                return 0;
            }
        }
    }
    return 1;
}

void MatrixScalarMultiply(float x, float mat[3][3], float result[3][3])
{
    int i, j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            result[i][j] = x * mat[i][j];
        }
    }
}

void MatrixScalarAdd(float x, float mat[3][3], float result[3][3])
{
    int i, j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            result[i][j] = x + mat[i][j];
        }
    }
}

float MatrixDeterminant(float mat[3][3])
{
    return (mat[0][0] * mat[1][1] * mat[2][2]) + (mat[0][1] * mat[1][2] * mat[2][0]) + (mat[0][2] * mat[1][0] * mat[2][1]) - (mat[0][2] * mat[1][1] * mat[2][0]) - (mat[0][0] * mat[1][2] * mat[2][1]) - (mat[0][1] * mat[1][0] * mat[2][2]);
    //return mat[0][0] * ((mat[1][1]*mat[2][2]) - (mat[2][1]*mat[1][2])) - mat[0][1] * (mat[1][0]* mat[2][2] - mat[2][0] * mat[1][2]) + mat[0][2] * (mat[1][0] * mat[2][1] - mat[2][0] * mat[1][1]);
}

float MatrixTrace(float mat[3][3])
{
    int i;
    float sum = 0;
    for (i = 0; i < 3; i++)
        sum += mat[i][i];
    return sum;
}

void MatrixTranspose(float mat[3][3], float result[3][3])
{
    int i, j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            result[j][i] = mat[i][j];
        }
    }
}

void MatrixInverse(float mat[3][3], float result[3][3])
{
    float inter[3][3] = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0}
    };
    float det = 0.0;
    det = MatrixDeterminant(mat);
    MatrixAdjugate(mat, inter);
    MatrixScalarMultiply((1 / det), inter, result);

}

void MatrixPrint(float mat[3][3])
{
    int i, j;
    for (i = 0; i < 3; i++) {
        printf("| ");
        for (j = 0; j < 3; j++) {
            printf("%6.2f |", (double) mat[i][j]);
        }
        printf("\n ------------------------\n");
    }
}

void MatrixAdjugate(float mat[3][3], float result[3][3])
{
    float b, c, d, e, f, g, h, k, l;
    b = ((mat[1][1] * mat[2][2])-(mat[1][2] * mat[2][1]));
    c = ((mat[1][2] * mat[2][0])-(mat[1][0] * mat[2][2]));
    d = ((mat[1][0] * mat[2][1])-(mat[1][1] * mat[2][0]));
    e = ((mat[0][2] * mat[2][1])-(mat[0][1] * mat[2][2]));
    f = ((mat[0][0] * mat[2][2])-(mat[0][2] * mat[2][0]));
    g = ((mat[0][1] * mat[2][0])-(mat[0][0] * mat[2][1]));
    h = ((mat[0][1] * mat[1][2])-(mat[0][2] * mat[1][1]));
    k = ((mat[0][2] * mat[1][0])-(mat[0][0] * mat[1][2]));
    l = ((mat[0][0] * mat[1][1])-(mat[0][1] * mat[1][0]));

    float resultant[3][3] = {
        {b, c, d},
        {e, f, g},
        {h, k, l}
    };

    MatrixTranspose(resultant, result);
}


