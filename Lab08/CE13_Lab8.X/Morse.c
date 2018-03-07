/*
 * File:   Morse.c
 * Author: JustinT
 *
 * Created on March 2, 2018, 12:49 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "Morse.h"
#include "Tree.h"
#include "Buttons.h"
#include "xc.h"

typedef enum {
    WAITING,
    DOT,
    DASH,
    INTER_LETTER
} states;

Node *tree;
Node *copytree;

/**
 * This function initializes the Morse code decoder. This is primarily the generation of the
 * Morse tree: a binary tree consisting of all of the ASCII alphanumeric characters arranged
 * according to the DOTs and DASHes that represent each character. Traversal of the tree is done by
 * taking the left-child if it is a dot and the right-child if it is a dash. If the morse tree is
 * successfully generated, SUCCESS is returned, otherwise STANDARD_ERROR is returned. This function
 * also initializes the Buttons library so that MorseCheckEvents() can work properly.
 * @return Either SUCCESS if the decoding tree was successfully created or STANDARD_ERROR if not.
 */
int MorseInit(void)
{
    const char Tree[] = {'#', 'E', 'I', 'S', 'H', '5', '4', 'V', '#', '3', 'U', 'F',
        '#', '#', '#', '#', '2', 'A', 'R', 'L', '#', '#', '#', '#', '#', 'W', 'P', '#', '#',
        'J', '#', '1', 'T', 'N', 'D', 'B', '6', '#', 'X', '#', '#', 'K', 'C', '#', '#', 'Y',
        '#', '#', 'M', 'G', 'Z', '7', '#', 'Q', '#', '#', 'O', '#', '8', '#', '#', '9', '0'};

    tree = TreeCreate(6, Tree);
    copytree = tree;
    if (tree == NULL){
        return STANDARD_ERROR;
    }
    else {
        return SUCCESS;
    }
}

/**
 * MorseDecode decodes a Morse string by iteratively being passed MORSE_CHAR_DOT or MORSE_CHAR_DASH.
 * Each call that passes a DOT or DASH returns a SUCCESS if the string could still compose a
 * Morse-encoded character. Passing in the special MORSE_CHAR_END_OF_CHAR constant will terminate
 * decoding and return the decoded character. During that call to MorseDecode() the return value
 * will be the character that was decoded or STANDARD_ERROR if it couldn't be decoded. Another
 * special value exists, MORSE_CHAR_DECODE_RESET, which will clear the stored state. When a
 * MORSE_CHAR_DECODE_RESET is done, SUCCESS will be returned. If the input is not a valid MorseChar
 * then the internal state should be reset and STANDARD_ERROR should be returned.
 * 
 * @param in A value from the MorseChar enum which specifies how to traverse the Morse tree.
 * 
 * @return Either SUCCESS on DECODE_RESET or when the next traversal location is still a valid
 *         character, the decoded character on END_OF_CHAR, or STANDARD_ERROR if the Morse tree
 *         hasn't been initialized, the next traversal location doesn't exist/represent a character,
 *         or `in` isn't a valid member of the MorseChar enum.
 */
char MorseDecode(MorseChar in)
{
    
    if (in == MORSE_CHAR_DOT) {
        if (copytree->leftChild != NULL) {
            return SUCCESS;
        }
        else {
            return NULL;
        }
    }
    else if (in == MORSE_CHAR_DASH) {
        if (copytree->rightChild != NULL) {
            return SUCCESS;
        }
        else {
            return NULL;
        }
    }
    else if (in == MORSE_CHAR_END_OF_CHAR){
        
    }
    else if (in == MORSE_CHAR_DECODE_RESET) {
        
    }
}

/**
 * This function calls ButtonsCheckEvents() once per call and returns which, if any,
 * of the Morse code events listed in the enum above have been encountered. It checks for BTN4
 * events in its input and should be called at 100Hz so that the timing works. The
 * length that BTN4 needs to be held down for a dot is >= 0.25s and < 0.50s with a dash being a button
 * down event for >= 0.5s. The button uptime various between dots/dashes (>= .5s), letters
 * (>= 1s), and words (>= 2s).
 *
 * @note This function assumes that the buttons are all unpressed at startup, so that the first
 *       event it will see is a BUTTON_EVENT_*DOWN.
 *
 * So pressing the button for 0.1s, releasing it for 0.1s, pressing it for 0.3s, and then waiting
 * will decode the string '.-' (A). It will trigger the following order of events:
 * 9 MORSE_EVENT_NONEs, 1 MORSE_EVENT_DOT, 39 MORSE_EVENT_NONEs, a MORSE_EVENT_DASH, 69
 * MORSE_EVENT_NONEs, a MORSE_EVENT_END_CHAR, and then MORSE_EVENT_INTER_WORDs.
 * 
 * @return The MorseEvent that occurred.
 */
MorseEvent MorseCheckEvents(void)
{
    static uint16_t counter = 0;
    static states state = WAITING;
    static uint8_t buttonEvents;
    buttonEvents = ButtonsCheckEvents();
    counter++;
    switch (state) {
    case WAITING:
        if (buttonEvents & BUTTON_EVENT_4DOWN) {
            counter = 0;
            state = DOT;
            break;
        }
        break;

    case DOT:
        if (counter >= MORSE_EVENT_LENGTH_DOWN_DOT) {
            state = DASH;
        }
        if (buttonEvents & BUTTON_EVENT_4UP) {
            counter = 0;
            state = INTER_LETTER;
            return MORSE_EVENT_DOT;
        }
        break;

    case DASH:
        if (buttonEvents & BUTTON_EVENT_4UP) {
            counter = 0;
            state = INTER_LETTER;
            return MORSE_EVENT_DASH;
        }
        break;

    case INTER_LETTER:
        if (counter >= MORSE_EVENT_LENGTH_UP_INTER_LETTER_TIMEOUT) {
            state = WAITING;
            return MORSE_EVENT_INTER_WORD;
            break;
        }
        if (buttonEvents & BUTTON_EVENT_4DOWN) {
            if (counter >= MORSE_EVENT_LENGTH_UP_INTER_LETTER) {
                counter = 0;
                state = DOT;
                return MORSE_EVENT_INTER_LETTER;
            } else {
                counter = 0;
                state = DOT;
            }
        }
        break;

        return MORSE_EVENT_NONE;
    }
}
