// **** Include libraries here ****
// Standard libraries
#include <stdio.h>

//Class specific libraries
#include "BOARD.h"

// Microchip libraries
#include <xc.h>

// User libraries
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    BOARD_Init();
    /***************************************************************************************************
     * Your code goes in between this comment and the following one with asterisks.
     **************************************************************************************************/

    char f,c,k;
f = 'F';
c = 'C';
k = 'K';

    // Declare Variables
    float kelvin, fahr;
    int lower, upper, step;

    // Initialize Variables
    lower = 0; // lower limit of temperature
    upper = 300; // upper limit
    step = 20; // step size
    kelvin = lower;

    printf("%5c %5c\n", f, c);
    // Print out table
    while (kelvin <= upper) {
        fahr = (5.0 / 9.0)*(kelvin - 32.0);
        printf("%7.1f %04.0f\n", (double) kelvin, (double) fahr);
        kelvin = kelvin + step;

    }

    printf("\n");
    printf("%6c %5c\n", k, f);
    kelvin = lower;
    // Print out table
    while (kelvin <= upper) {
        fahr = (9.0 / 5.0)*(kelvin - 273.15) + 32.0;
        printf("%3.3f %5f\n", (double) kelvin, (double) fahr);
        kelvin = kelvin + step;

    }
    /***************************************************************************************************
     * Your code goes in between this comment and the preceding one with asterisks.
     **************************************************************************************************/

    // Returning from main() is bad form in embedded environments. So we sit and spin.
    while (1);
}
