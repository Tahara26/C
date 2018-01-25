// **** Include libraries here ****
// Standard libraries
#include <stdio.h>
#include <math.h>

//CMPE13 Support Library
#include "BOARD.h"

// Microchip libraries
#include <xc.h>
#include <plib.h>

// User libraries


// **** Set macros and preprocessor directives ****

// **** Define global, module-level, or external variables here ****
#define PI 3.14159265
// **** Declare function prototypes ****
double Round(double operand);
double AbsoluteValue(double operand);
double FahrenheitToCelsius(double operand);
double CelsiusToFahrenheit(double operand);
double Average(double a, double b);
double Tangent(double operand);

// If this code is used for testing, rename main to something we can call from our testing code.
#ifndef LAB2_TESTING

int main(void)
{
    BOARD_Init();
#else

int their_main(void)
{
#endif // LAB2_TESTING

    /******************************************************************************
     * Your code goes in between this comment and the following one with asterisks.
     *****************************************************************************/
    char oper;
    char garbage;
    float x, y;
    printf("Welcome to Justin's calculator program!\n");
    while (1) {
        printf("Enter a mathematical operation to perform (*,/,+,-,v,a,c,f,t,r):");
        scanf("%c%c", &oper, &garbage);

        // Addition 
        if (oper == '+') {
            printf("Enter the first operand:");
            scanf("%f%c", &x, &garbage);
            printf("Enter the second operand:");
            scanf("%f%c", &y, &garbage);
            printf("Result of (%f + %f): %f\n", x, y, x + y);
        }// Subtraction
        else if (oper == '-') {
            printf("Enter the first operand:");
            scanf("%f%c", &x, &garbage);
            printf("Enter the second operand:");
            scanf("%f%c", &y, &garbage);
            printf("Result of (%f - %f): %f\n", x, y, x - y);
        }// Multiplication
        else if (oper == '*') {
            printf("Enter the first operand:");
            scanf("%f%c", &x, &garbage);
            printf("Enter the second operand:");
            scanf("%f%c", &y, &garbage);
            printf("Result of (%f * %f): %f\n", x, y, x * y);
        }// Division
        else if (oper == '/') {
            printf("Enter the first operand:");
            scanf("%f%c", &x, &garbage);
            printf("Enter the second operand:");
            scanf("%f%c", &y, &garbage);
            if (y == 0) {
                printf("Result of (%f / %f): %f\n", x, y, 0);
                continue;
            }
            printf("Result of (%f / %f): %f\n", x, y, x / y);
        }// Average
        else if (oper == 'v') {
            printf("Enter the first operand:");
            scanf("%f%c", &x, &garbage);
            printf("Enter the second operand:");
            scanf("%f%c", &y, &garbage);
            printf("Result of (%f v %f): %f\n", x, y, Average(x, y));
        }// Absolute Value
        else if (oper == 'a') {
            printf("Enter the first operand:");
            scanf("%f%c", &x, &garbage);
            printf("Result of AbsoluteValue(%f): %f\n", x, AbsoluteValue(x));
        }// Fahrenheit to Celsius
        else if (oper == 'c') {
            printf("Enter the first operand:");
            scanf("%f%c", &x, &garbage);
            printf("Result of %f deg->C: %f\n", x, FahrenheitToCelsius(x));
        }// Celsius to Fahrenheit
        else if (oper == 'f') {
            printf("Enter the first operand:");
            scanf("%f%c", &x, &garbage);
            printf("Result of %f deg->F: %f\n", x, CelsiusToFahrenheit(x));
        }// Tangent
        else if (oper == 't') {
            printf("Enter the first operand:");
            scanf("%f%c", &x, &garbage);
            printf("Result of tan(%f): %f\n", x, Tangent(x));
        }// Round
        else if (oper == 'r') {
            printf("Enter the first operand:");
            scanf("%f%c", &x, &garbage);
            printf("Result of round(%f): %f\n", x, Round(x));
        } else {
            printf("Error, not a valid operator\n");
        }
    }

    /******************************************************************************
     * Your code goes in between this comment and the preceding one with asterisks
     *****************************************************************************/
    while (1);
}

/********************************************************************************
 * Define the Absolute Value function here.
 ********************************************************************************/
double AbsoluteValue(double operand)
{
    if (operand < 0) {
        operand = (operand)*(-1);
    }
    return operand;
}

/*********************************************************************************
 * Define the Fahrenheit to Celsius function here.
 ********************************************************************************/
double FahrenheitToCelsius(double operand)
{
    double x;
    x = ((5.0 / 9.0)*(operand - 32));
    return x;
}

/*********************************************************************************
 * Define the Celsius to Fahrenheit function here.
 ********************************************************************************/
double CelsiusToFahrenheit(double operand)
{
    double x;
    x = (operand * (9.0 / 5.0) + 32);
    return x;
}

/********************************************************************************
 * Define the Average function here.
 *******************************************************************************/
double Average(double a, double b)
{
    double operand;
    operand = ((a + b) / 2);
    return operand;
}

/*********************************************************************************
 * Define the Tangent function that takes input in degrees (Note: Your tangent 
 * function relies on a call from the tangent function of math.h which uses 
 * radians).
 ********************************************************************************/
double Tangent(double operand)
{
    double x;
    x = tan(operand * PI / 180);
    return x;
}

/*********************************************************************************
 * Define the Round function here.
 * In order to receive the extra credit your calculator MUST ALSO CORRECTLY utilize
 * this function.
 ********************************************************************************/

double Round(double operand)
{
    return operand;
}

