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

    int i; // Made an integer so I can set all of the strings to Null before doing my code.
    int error; // Just an integer for error handling later on
    error = 0;
    char string[61]; // 61 because of the return character
    char *token;
    float out1;
    float out2;
    float result;
    struct Stack stack;
    StackInit(&stack);
    // Sets all the string values to null
    for (i = 0; i < sizeof (string); i++) {
        string[i] = '\0';
    }

    /* TEST CODE
    struct Stack testStack;
    StackInit(&testStack);
    printf("Stack has %d elements!\n", StackGetSize(&testStack));
    StackPush(&testStack, 3.14159);
    printf("Stack has %d elements!\n", StackGetSize(&testStack));
    float testFloat;
    StackPop(&testStack, &testFloat);
    printf("%f = 3.14159!\n", testFloat);
    printf("Stack is empty: %d!\n", StackIsEmpty(&testStack));
     */

    // User greetings
    printf("Welcome to Justin's RPN calculator.\n");
    printf("Enter floats and + - / * in RPN format:\n>");
    while (1) {

        // Fgets to take in the user input
        fgets(string, 61, stdin);
        // Parsing the string
        token = strtok(string, " ");
        StackInit(&stack);
        error = 0;

        // While loop for all of the calculations 
        //Token has to not equal null so it can iterate through the code
        while (token != NULL) {

            // Checks to see that the token is valid and also passes through a zero
            if (atof(token) || *token == '0') {
                // Push if a token but return STANDARD_ERROR if the stack is full    
                if (StackPush(&stack, atof(token)) == STANDARD_ERROR) {
                    printf("ERROR: No more room on the stack.\n>");
                    error++;
                    break;
                }
            }
                // Have the tokens be popped into the stack 
                // If either token is not a number return error
                // If the stack is full return error as well
            else if (*token == '+') {
                if (StackPop(&stack, &out2) == STANDARD_ERROR) {
                    printf("ERROR: Not enough operands before operator.\n>");
                    error++;
                    break;
                }
                if (StackPop(&stack, &out1) == STANDARD_ERROR) {
                    printf("ERROR: Not enough operands before operator.\n>");
                    error++;
                    break;
                }
                if (StackPush(&stack, (out1 + out2)) == STANDARD_ERROR) {
                    printf("ERROR: No more room on the stack.\n>");
                    error++;
                    break;
                }
            }

            else if (*token == '-') {
                if (StackPop(&stack, &out2) == STANDARD_ERROR) {
                    printf("ERROR: Not enough operands before operator.\n>");
                    error++;
                    break;
                }
                if (StackPop(&stack, &out1) == STANDARD_ERROR) {
                    printf("ERROR: Not enough operands before operator.\n>");
                    error++;
                    break;
                }
                if (StackPush(&stack, (out1 - out2)) == STANDARD_ERROR) {
                    printf("ERROR: No more room on the stack.\n>");
                    error++;
                    break;
                }
            } else if (*token == '/') {
                if (StackPop(&stack, &out2) == STANDARD_ERROR) {
                    printf("ERROR: Not enough operands before operator.\n>");
                    error++;
                    break;
                }
                if (StackPop(&stack, &out1) == STANDARD_ERROR) {
                    printf("ERROR: Not enough operands before operator.\n>");
                    error++;
                    break;
                }
                // Special case for dividing by zero because that is not a valid operation
                if (out2 == 0.0) {
                    printf("ERROR: Cannot divide by zero.\n>");
                    error++;
                    break;
                }

                if (StackPush(&stack, (out1 / out2)) == STANDARD_ERROR) {
                    printf("ERROR: No more room on the stack.\n>");
                    error++;
                    break;
                }
            } else if (*token == '*') {
                if (StackPop(&stack, &out2) == STANDARD_ERROR) {
                    printf("ERROR: Not enough operands before operator.\n>");
                    error++;
                    break;
                }
                if (StackPop(&stack, &out1) == STANDARD_ERROR) {
                    printf("ERROR: Not enough operands before operator.\n>");
                    error++;
                    break;
                }
                if (StackPush(&stack, (out1 * out2)) == STANDARD_ERROR) {
                    printf("ERROR: No more room on the stack.\n>");
                    error++;
                    break;
                }
            }
                // If the user input has letters it will be caught by this error handling
            else {
                printf("ERROR: Invalid character in RPN string.\n>");
                error++;
                break;
            }

            token = strtok(NULL, " ");

        }
        // Error handling to make sure not to print result after error message. Basically a flag
        if (error == 1) {
            continue;
        }

        // Last check to see if there are less than one or more than one items on the stack
        if (StackGetSize(&stack) != 1) {
            printf("ERROR: Invalid RPN calculation: more or less than one item in the stack.\n>");
            continue;
        }

        // Print result
        StackPop(&stack, &result);
        printf("Result: %.2f\n>", result);

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



