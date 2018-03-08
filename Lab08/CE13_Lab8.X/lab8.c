// **** Include libraries here ****
// Standard C libraries
#include <stdlib.h>

//CMPE13 Support Library
#include "BOARD.h"

// Microchip libraries
#include <xc.h>
#include <plib.h>

// User libraries
#include "Leds.h"
#include "Morse.h"
#include "Oled.h"
#include "Buttons.h"
#include "Tree.h"

// **** Set any macros or preprocessor directives here ****
// Specify a bit mask for setting/clearing the pin corresponding to BTN4. Should only be used when
// unit testing the Morse event checker.
#define BUTTON4_STATE_FLAG (1 << 7)
#define SPACE ' '

// **** Declare any data types here ****

// **** Define any module-level, global, or external variables here ****
static MorseEvent morseEvents;
static char string1[100];

// **** Declare any function prototypes here ****
static void cleartop(void);
static void topline(char morse);
static void bottomline(char letter);

int main()
{
    BOARD_Init();


    // Configure Timer 2 using PBCLK as input. We configure it using a 1:16 prescalar, so each timer
    // tick is actually at F_PB / 16 Hz, so setting PR2 to F_PB / 16 / 100 yields a .01s timer.
    OpenTimer2(T2_ON | T2_SOURCE_INT | T2_PS_1_16, BOARD_GetPBClock() / 16 / 100);

    // Set up the timer interrupt with a medium priority of 4.
    INTClearFlag(INT_T2);
    INTSetVectorPriority(INT_TIMER_2_VECTOR, INT_PRIORITY_LEVEL_4);
    INTSetVectorSubPriority(INT_TIMER_2_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
    INTEnable(INT_T2, INT_ENABLED);

    /******************************************************************************
     * Your code goes in between this comment and the following one with asterisks.
     *****************************************************************************/

    // Initialize Oled
    OledInit();
    
    // Set all of the locations to space in the array
    int i;
    for(i=0; i<100; i++) {
        string1[i] = SPACE;
    }

    // Check if MorseInit returns a Fatal error 
    if (!MorseInit()) {
        OledDrawString("Could not initialize");
        OledUpdate();
        FATAL_ERROR();
    }
    char x = '\0';

    // Enter loop for all events 
    while (1) {
        if (morseEvents) {
            
            // For the Dot event
            if (morseEvents == MORSE_EVENT_DOT) {
                // Reset event
                morseEvents = MORSE_EVENT_NONE;
                
                // If returned a standard error
                if (MorseDecode(MORSE_CHAR_DOT) == STANDARD_ERROR) {
                    
                    // Reset decode, clear top, and print Pound
                    MorseDecode(MORSE_CHAR_DECODE_RESET);
                    cleartop();
                    bottomline(MORSE_CHAR_END_OF_CHAR);
                } else {
                    // Print dot
                    topline(MORSE_CHAR_DOT);
                }
            } 
            
            // For the Dash event
            else if (morseEvents == MORSE_EVENT_DASH) {
                // Reset event
                morseEvents = MORSE_EVENT_NONE;
                
                // If returned a standard error
                if (MorseDecode(MORSE_CHAR_DASH) == STANDARD_ERROR) {
                    
                    // Reset decode, clear top, and print Pound 
                    MorseDecode(MORSE_CHAR_DECODE_RESET);
                    cleartop();
                    bottomline(MORSE_CHAR_END_OF_CHAR);
                } else {
                    // Print dash
                    topline(MORSE_CHAR_DASH);
                }
            } 
            
            // For the inter letter event
            else if (morseEvents == MORSE_EVENT_INTER_LETTER) {
                // Reset event
                morseEvents = MORSE_EVENT_NONE;
                if ((x = MorseDecode(MORSE_CHAR_END_OF_CHAR)) > SUCCESS) {
                    
                    // Reset decode, clear top, and print letter
                    MorseDecode(MORSE_CHAR_DECODE_RESET);
                    cleartop();
                    bottomline(x);
                } else {
                    
                    // Reset decode, clear top, and print Pound
                    MorseDecode(MORSE_CHAR_DECODE_RESET);
                    cleartop();
                    bottomline(MORSE_CHAR_END_OF_CHAR);
                }
            } 
            
            // For the inter letter word
            else if (morseEvents == MORSE_EVENT_INTER_WORD) {
                // Reset event
                morseEvents = MORSE_EVENT_NONE;
                if (((x = MorseDecode(MORSE_CHAR_END_OF_CHAR)) != STANDARD_ERROR) && ((x = MorseDecode(MORSE_CHAR_END_OF_CHAR)) != SUCCESS)) {
                    
                    // Reset decode, clear top, and print Word with space after it 
                    MorseDecode(MORSE_CHAR_DECODE_RESET);
                    cleartop();
                    bottomline(x);
                    bottomline(SPACE);
                } else { 
                    
                    // Reset decode, clear top, and print Pound
                    MorseDecode(MORSE_CHAR_DECODE_RESET);
                    cleartop();
                    bottomline(MORSE_CHAR_END_OF_CHAR);
                }
            }
        }
    }

    /******************************************************************************
     * Your code goes in between this comment and the preceding one with asterisks.
     *****************************************************************************/

    while (1);
}

void __ISR(_TIMER_2_VECTOR, IPL4AUTO) TimerInterrupt100Hz(void)
{
    // Clear the interrupt flag.
    IFS0CLR = 1 << 8;

    //******** Put your code here *************//
    morseEvents = MorseCheckEvents();
}

static void cleartop(void)
{
    // Iterate through the top of the array and clear it out 
    int i;
    for (i = 0; i < 21; i++) {
        string1[i] = ' ';
    }
    OledDrawString(string1);
    OledUpdate();
}

static void topline(char morse)
{
    // Look for space and iterate
    int i = 0;
    while (string1[i] != SPACE){
        i++;
    }
    
    // Set string to morse
    string1[i] = morse;
    OledDrawString(string1);
    OledUpdate();
}

static void bottomline(char letter)
{
    // Make static to skip spaces and just iterate
    static int i = 21;
    string1[i] = letter;
    i++;
    OledDrawString(string1);
    OledUpdate();
}
