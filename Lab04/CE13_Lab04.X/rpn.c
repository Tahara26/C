// **** Include libraries here ****
// Standard libraries
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//CMPE13 Support Library
#include "BOARD.h"

// Microchip libraries
#include <xc.h>
#include <plib.h>

// User libraries
#include "Stack.h"

// **** Set macros and preprocessor directives ****

// **** Define global, module-level, or external variables here ****

// **** Declare function prototypes ****
/*
 * Function prototype for ProcessBackspaces() - This function should be defined
 * below main after the related comment.
 */
int ProcessBackspaces(char *rpn_sentence);

// If this code is used for testing, rename main to something we can call from our testing code. We
// also skip all processor configuration and initialization code.
#ifndef LAB4_TESTING

int main()
{
    BOARD_Init();
#else

int their_main(void)
{
#endif // LAB4_TESTING

    /******************************** Your custom code goes below here *******************************/
    int i;
    char string[61];
    char *token;
    float result;
    float x;
    float y;
    struct Stack stack;
    StackInit(&stack);

    printf("Welcome to Justin's RPN calculator.\n");
        printf("Enter floats and + - / * in RPN format:\n>");
        for (i = 0; i < sizeof(string); i++) {
        string[i] = '\0';
    }
        fgets(string, sizeof (string), stdin);
        token = strtok(string, " ");
        while (token != NULL) {
            printf("%f\n", atof(token));
            if (atof(token) != 0) {
                StackPush(&stack, atof(token));
            }
            
            else if (*token == '+') {
                StackPop(&stack, &y);
                StackPop(&stack, &x);
                printf("%f %f", y, x);
                result = x + y;
                StackPush(&stack, result);
                printf("Result: %5.2f", result);
            }
            /*
            else if (*token == '-') {
                StackPop(&stack, &y);
                StackPop(&stack, &x);
                result = x - y;
                StackPush(&stack, result);
                printf("Result: %5.2f", result);
            }
            else if (*token == '/') {
                StackPop(&stack, &y);
                StackPop(&stack, &x);
                result = x / y;
                StackPush(&stack, result);
                printf("Result: %5.2f", result);
            }
            else if (*token == '*') {
                StackPop(&stack, &y);
                StackPop(&stack, &x);
                result = x * y;
                StackPush(&stack, result);
                printf("Result: %5.2f", result);
            } */
            token = strtok(NULL, " ");
        }

    /*************************************************************************************************/

    // You can never return from main() in an embedded system (one that lacks an operating system).
    // This will result in the processor restarting, which is almost certainly not what you want!
    while (1);
}

/**
 * Extra Credit: Define ProcessBackspaces() here - This function should read through an array of
 * characters and when a backspace character is read it should replace the preceding character with
 * the following character. It should be able to handle multiple repeated backspaces and also
 * strings with more backspaces than characters. It should be able to handle strings that are at
 * least 256 characters in length.
 * @param rpn_sentence The string that will be processed for backspaces. This string is modified in
 *                     place.
 * @return Returns the size of the resultant string in `rpn_sentence`.
 */
int ProcessBackspaces(char *rpn_sentence)
{
    return 0;
}



