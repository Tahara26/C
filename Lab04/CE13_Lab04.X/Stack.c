/*
 * File:   Stack.c
 * Author: JustinT
 *
 * Created on February 1, 2018, 9:45 AM
 */


#include "xc.h"
#include "Stack.h"

void StackInit(struct Stack *stack)
{
    stack ->currentItemIndex = -1;
    stack ->initialized = 1;
}

int StackPush(struct Stack *stack, float value)
{
    if (stack ->initialized == 0) {
        return STANDARD_ERROR;
    } else if (stack ->currentItemIndex == (STACK_SIZE - 1)) {
        return STANDARD_ERROR;
    } else {
        (stack ->currentItemIndex)++;
        stack ->stackItems[stack ->currentItemIndex] = value;
        return SUCCESS;
    }
}

int StackPop(struct Stack *stack, float *value)
{
    if (stack ->initialized == 0) {
        return STANDARD_ERROR;
    } else if (stack ->currentItemIndex == -1) {
        return STANDARD_ERROR;
    } else {
        *value = stack ->stackItems[stack ->currentItemIndex];
        stack ->stackItems[stack ->currentItemIndex] = 0.0;
        (stack ->currentItemIndex)--;
        return SUCCESS;
    }
}

int StackIsEmpty(const struct Stack *stack)
{
    if (stack ->initialized == 0) {
        return 0;
    } else if (stack ->currentItemIndex == -1) {
        return 1;
    } else {
        return 0;
    }
}

int StackIsFull(const struct Stack *stack)
{
    if (stack ->initialized == 0) {
        return 0;
    } else if (stack ->currentItemIndex == (STACK_SIZE - 1)) {
        return 1;
    } else {
        return 0;
    }
}

int StackGetSize(const struct Stack *stack)
{
    if (stack ->initialized == 0) {
        return SIZE_ERROR;
    } else if (stack ->currentItemIndex == -1) {
        return 0;
    } else {
        return (stack ->currentItemIndex) + 1;
    }
}
