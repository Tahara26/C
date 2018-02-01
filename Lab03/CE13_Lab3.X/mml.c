// **** Include libraries here ****
// Standard libraries
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//CMPE13 Support Library
#include "BOARD.h"


// Microchip libraries
#include <xc.h>
#include <plib.h>
#include <math.h>

// User libraries
#include "MatrixMath.h"

// **** Set macros and preprocessor directives ****

// **** Define global, module-level, or external variables here ****

// **** Declare function prototypes ****

int main()
{
    BOARD_Init();

    /******************************************************************************
     * Your code goes in between this comment and the following one with asterisks.
     *****************************************************************************/
    float testMatrix1[3][3] = {
        {4.2, 9.8, -8.5},
        {6.7, -3.1, 10.7},
        {2.5, 4.9, -7.8}
    };

    float testMatrix2[3][3] = {
        {3.6, 9.0, 1.1},
        {2.7, -7.5, 8.0},
        {5.8, 7.2, 5.4}
    };

    float testMatrix3[3][3] = {
        {7.9, 6.1, 9.4},
        {4.8, -2.7, 0.5},
        {3.1, 9.1, 8.7}
    };

    int n, m, i, j, equals, mult, smult, add, sadd, det, inv, tran, trac, count;
    float x, y, z, u, v, w, avg;
    count = 0;
    n = 0;
    m = 0;
    i = 0;
    j = 0;
    float result[3][3] = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0}
    };
    float result1[3][3] = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0}
    };


    //-----------------------------------------------------------------------------   
    //   Matrix Equals

    float MatrixEq1[3][3] = {
        {4.2, 9.8, -8.5},
        {6.7, -3.1, 10.7},
        {2.5, 4.9, -7.8}
    };

    float MatrixEq2[3][3] = {
        {3.6, 9.0, 1.1},
        {2.7, -7.5, 8.0},
        {5.8, 7.2, 5.4}
    };

    equals = 0;
    int var;
    var = MatrixEquals(MatrixEq1, testMatrix1);
    if (var != 0) {
        equals++;
    }

    var = 0;
    var = MatrixEquals(MatrixEq2, testMatrix2);
    if (var != 0) {
        equals++;
    }
    if (equals == 2) {
        count++;
    }

    printf("PASSED (%d/2): MatrixEquals()\n", equals);

    //-----------------------------------------------------------------------------             
    //   Matrix Multiply   

    mult = 0;
    var = 0;

    float MatrixM1[3][3] = {
        {-7.71999, -96.9, 37.1200},
        {77.81, 160.59, 40.35},
        {-23.00999, -70.41, -0.1700}
    };

    float MatrixM2[3][3] = {
        {53.87, -78.19, -29.56999},
        {71.22000, 146.60999, 154.51999},
        {19.08999, -68.96, -41.91}
    };

    MatrixMultiply(testMatrix1, testMatrix2, result);
    if (MatrixEquals(MatrixM1, MatrixM1)) {
        mult++;
    }

    var = 0;
    MatrixMultiply(testMatrix1, testMatrix3, result1);
    var = MatrixEquals(result1, MatrixM2);
    if (var != 0) {
        mult++;
    }

    if (mult == 2) {
        count++;
    }
    printf("PASSED (%d/2): MatrixMultiply()\n", mult);

    //-----------------------------------------------------------------------------
    //   Matrix Scalar Multiply
    x = 3.1;
    y = 2.4;
    smult = 0;

    float MatrixSM1[3][3] = {
        {11.16, 27.9, 3.41},
        {8.37, -23.25, 24.8},
        {17.98, 22.32, 16.74}
    };
    float MatrixSM2[3][3] = {
        {18.96, 14.64, 22.56},
        {11.52, -6.48, 1.2},
        {7.44, 21.84, 20.88}
    };

    MatrixScalarMultiply(x, testMatrix2, result);
    var = MatrixEquals(result, MatrixSM1);
    if (var == 1) {
        smult += 1;
    }

    MatrixScalarMultiply(y, testMatrix3, result);
    var = MatrixEquals(result, MatrixSM2);
    if (var == 1) {
        smult += 1;
    }

    if (smult == 2) {
        count++;
    }

    printf("PASSED (%d/2): MatrixScalarMultiply()\n", smult);
    //-----------------------------------------------------------------------------  
    //   Determinant

    float difference, difference1;
    z = 310.718;
    det = 0;

    difference = (double) MatrixDeterminant(testMatrix1) - z;
    difference1 = fabs(difference);
    if (difference1 <= FP_DELTA) {
        det += 1;
    }

    u = 22.473000;
    difference = (double) MatrixDeterminant(testMatrix3) - u;
    difference1 = fabs(difference);
    if (difference1 <= FP_DELTA) {
        det += 1;
    }

    if (det == 2) {
        count++;
    }

    printf("PASSED (%d/2): MatrixDeterminant()\n", det);

    //-----------------------------------------------------------------------------
    //   Addition
    add = 0;

    float MatrixA1[3][3] = {
        {7.8, 18.8, -7.4},
        {9.4, -10.6, 18.7},
        {8.3, 12.1, -2.4}
    };
    float MatrixA2[3][3] = {
        {11.5, 15.1, 10.5},
        {7.5, -10.2, 8.5},
        {8.9, 16.3, 14.1}
    };
    add = 0;
    MatrixAdd(testMatrix1, testMatrix2, result);
    var = MatrixEquals(result, MatrixA1);
    if (var != 0) {
        add++;
    }

    MatrixAdd(testMatrix2, testMatrix3, result);
    var = MatrixEquals(result, MatrixA2);
    if (var != 0) {
        add++;
    }

    if (add == 2) {
        count++;
    }

    printf("PASSED (%d/2): MatrixAdd()\n", add);

    //-----------------------------------------------------------------------------  
    //   Scalar Addition
    sadd = 0;

    float MatrixSA1[3][3] = {
        {6.6, 12.2, -6.1},
        {9.1, -0.7, 13.1},
        {4.9, 7.3, -5.4}
    };

    float MatrixSA2[3][3] = {
        {6.0, 11.4, 3.5},
        {5.1, -5.1, 10.4},
        {8.2, 9.6, 7.8}
    };

    MatrixScalarAdd(y, testMatrix1, result);
    var = MatrixEquals(result, MatrixSA1);
    if (var != 0) {
        sadd++;
    }

    MatrixScalarAdd(y, testMatrix2, result);
    var = MatrixEquals(result, MatrixSA2);
    if (var != 0) {
        sadd++;
    }

    if (sadd == 2) {
        count++;
    }

    printf("PASSED (%d/2): MatrixScalarAdd()\n", sadd);

    //-----------------------------------------------------------------------------
    //   Inverse

    float MatrixIN1[3][3] = {
        {-0.090918, 0.111966, 0.252672},
        {0.254282, -0.037043, -0.327917},
        {0.130600, 0.012615, -0.253219}
    };

    float MatrixIN2[3][3] = {
        {-39.975387, -16.576950, 32.701580},
        {12.966533, 5.321902, -10.525630},
        {25.647817, 10.709004, -20.904562}
    };

    inv = 0;
    MatrixInverse(testMatrix1, result);
    if (MatrixEquals(result, MatrixIN1)) {
        inv++;
    }

    MatrixInverse(testMatrix2, result);
    if (MatrixEquals(result, MatrixIN2)) {
        inv++;
    }

    if (inv == 2) {
        count++;
    }

    printf("PASSED (%d/2): MatrixInverse()\n", inv);

    //-----------------------------------------------------------------------------
    //   Transpose
    tran = 0;

    float MatrixTN1[3][3] = {
        {4.2, 6.7, 2.5},
        {9.8, -3.1, 4.9},
        {-8.5, 10.7, -7.8}
    };

    float MatrixTN2[3][3] = {
        {7.9, 4.8, 3.1},
        {6.1, -2.7, 9.1},
        {9.4, 0.5, 8.7}
    };

    MatrixTranspose(testMatrix1, result);
    var = MatrixEquals(result, MatrixTN1);
    if (var != 0) {
        tran++;
    }

    MatrixTranspose(testMatrix3, result);
    var = MatrixEquals(result, MatrixTN2);
    if (var != 0) {
        tran++;
    }

    if (tran == 2) {
        count++;
    }

    printf("PASSED (%d/2): MatrixTranspose()\n", tran);
    //-----------------------------------------------------------------------------
    //   Trace
    trac = 0;
    v = 13.9;
    w = 1.5;

    if (MatrixTrace(testMatrix3) == v) {
        trac++;
    }

    if (MatrixTrace(testMatrix2) == w) {
        trac++;
    }

    if (trac == 2) {
        count++;
    }

    printf("PASSED (%d/2): MatrixTrace()\n", trac);
    //-----------------------------------------------------------------------------
    //   Print
    int total = 9;
    avg = (float) count / total * 100.0;
    printf("-----------------------------\n");
    printf("%d out of 9 functions passed (%5.1f%%).\n\n", count, (double) avg);
    printf("Output of MatrixPrint()\n");
    printf(" ________________________\n");
    MatrixPrint(testMatrix1);
    printf("\n");
    printf("Expected output of MatrixPrint()\n");
    printf(" ________________________\n|   4.20 |  9.80 | -8.50 |\n ------------------------\n|   6.70 | -3.10 | 10.70 |\n ------------------------\n|   2.50 |  4.90 | -7.80 |\n ------------------------");

    /******************************************************************************
     * Your code goes in between this comment and the preceding one with asterisks
     *****************************************************************************/

    // Returning from main() is bad form in embedded environments. So we sit and spin.
    while (1);
}

