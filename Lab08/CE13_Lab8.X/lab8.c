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

// **** Declare any data types here ****

// **** Define any module-level, global, or external variables here ****
static MorseEvent morseEvents;

// **** Declare any function prototypes here ****


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
    
    OledInit();
    ButtonsInit();
    
//    while(1) {
//        if(morseEvents == MORSE_EVENT_DOT){
//            OledDrawString(".");
//            OledUpdate();
//        }
//        else if(morseEvents == MORSE_EVENT_DASH){
//            OledDrawString("-");
//            OledUpdate();
//        }
////        else if(morseEvents == MORSE_EVENT_NONE){
////            OledDrawString("#");
////            OledUpdate();
////        }
//        //morseEvents = 0;
//    }
//    MorseInit();
//    
//    static Node *variable;
//    
//    const char Tree[] = {'#','E','I','S','H','5','4','V','#','3','U','F',
//          '#','#','#','#','2','A','R','L','#','#','#','#','#','W','P','#','#',
//          'J','#','1','T','N','D','B','6','#','X','#','#','K','C','#','#','Y',
//          '#','#','M','G','Z','7','#','Q','#','#','O','#','8','#','#','9','0'};
//    
//    variable = TreeCreate(6, Tree);
//    static Node *variable2;
//    variable2 = variable;
//    for(;variable != NULL; variable = variable->leftChild){
//        printf("this should be my tree: %c\n", variable->data);
//    }
//    for(;variable2 != NULL; variable2 = variable2->rightChild){
//        printf("this should be my tree: %c\n", variable2->data);
//    }
    
   

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

void cleartop(void) {
    OledDrawString("\0");
    OledUpdate();
}

void topline(void) {
    
}

void bottomline(void) {
    
}