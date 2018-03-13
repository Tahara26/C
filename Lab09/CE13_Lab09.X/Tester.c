// **** Include libraries here ****
// Standard libraries

//CMPE13 Support Library
#include "BOARD.h"

// Microchip libraries
#include <xc.h>
#include <plib.h>

// User libraries
#include "Buttons.h"
#include "Oled.h"
#include "Protocol.h"
#include "Agent.h"
#include "Field.h"
#include "FieldOled.h"
#include "Leds.h"

// **** Set any macros or preprocessor directives here ****

// **** Declare any data types here ****

// **** Define any module-level, global, or external variables here ****
static uint32_t counter;
static uint8_t buttonEvents;

// **** Declare any function prototypes here ****



int main()
{
    BOARD_Init();

    // Configure Timer 2 using PBCLK as input. We configure it using a 1:16 prescalar, so each timer
    // tick is actually at F_PB / 16 Hz, so setting PR2 to F_PB / 16 / 100 yields a 10ms timer.
    OpenTimer2(T2_ON | T2_SOURCE_INT | T2_PS_1_16, BOARD_GetPBClock() / 16 / 100);

    // Set up the timer interrupt with a medium priority of 4.
    INTClearFlag(INT_T2);
    INTSetVectorPriority(INT_TIMER_2_VECTOR, INT_PRIORITY_LEVEL_4);
    INTSetVectorSubPriority(INT_TIMER_2_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
    INTEnable(INT_T2, INT_ENABLED);

    // Disable buffering on stdout
    setbuf(stdout, NULL);

    ButtonsInit();

    OledInit();

    // Prompt the user to start the game and block until the first character press.
    OledDrawString("Press BTN4 to start.");
    OledUpdate();
    while ((buttonEvents & BUTTON_EVENT_4UP) == 0);


/******************************************************************************
 * Your code goes in between this comment and the following one with asterisks.
 *****************************************************************************/
////PROTOCOL DECODE tests
//    
//    
//    char *n11 = "$CHA,37348,117*46\n";
//    char *n12 = "$DET,9578,46222*66\n";          
//    char *n21 = "$CHA,54104,139*45\n";
//    char *n22 = "$DET,32990,21382*5e\n";       
//    char *n31 = "$CHA,62132,70*79\n";
//    char *n32 = "$DET,52343,16067*50\n";         
//    char *n41 = "$CHA,36027,55*7a\n";
//    char *n42 = "$DET,7321,36898*6e\n";
//            
//    char *h1 = "$HIT,3,8,1*43\n";
//    char *h2 = "$HIT,0,2,0*4b\n";
//    char *h3 = "$HIT,2,3,1*49\n";
//    char *h4 = "$HIT,5,6,4*4e\n";
//    char *h5 = "$HIT,0,3,0*4a\n";
//    char *h6 = "$HIT,1,7,1*4e\n";
//    char *h7 = "$HIT,4,8,0*45\n";
//    char *h8 = "$HIT,5,3,3*4c\n";
//    char *h9 = "$HIT,0,5,0*4c\n";
//    char *h10 = "$HIT,5,6,1*4b\n";
//    char *h11 = "$HIT,1,1,1*48\n";
//    char *h12 = "$HIT,1,0,0*48\n";
//    char *h13 = "$HIT,5,2,5*4b\n";
//    char *h14 = "$HIT,2,8,0*43\n";
//    char *h15 = "$HIT,0,6,0*4f\n";
//    char *h16 = "$HIT,5,9,0*45\n";
//    char *h17 = "$HIT,2,8,2*41\n";
//            
//    char *c1 = "$COO,0,2*41\n";
//    char *c2 = "$COO,5,5*43\n";
//    char *c3 = "$COO,1,6*44\n";
//    char *c4 = "$COO,0,4*47\n";
//    char *c5 = "$COO,0,5*46\n";
//    char *c6 = "$COO,1,2*40\n";
//    char *c7 = "$COO,3,8*48\n";
//    char *c8 = "$COO,4,0*47\n";
//    char *c9 = "$COO,1,7*45\n";
//    char *c10 = "$COO,0,8*4b\n";
//    char *c11 = "$COO,2,2*43\n";
//    char *c12 = "$COO,4,1*46\n";
//    char *c13 = "$COO,0,0*43\n";
//    char *c14 = "$COO,2,9*48\n";
//    
//    char *bs1 = "$REF,1,2*2a\n"; //wrong msgID
//    char *bs2 = "$COO,6,6*g8\n"; //wrong hex CheckSums
//    char *bs3 = "$HIT,2,3,4,5*24\n"; //one too many arguments
//    char *bs4 = "helloWorld!"; //this is wrong
//    
//    static char *testMe;
//    //@@@@@@@@@@@@@
//    testMe = n11;
//    //@@@@@@@@@@@@@
//    static int i;
//    static ProtocolParserStatus fsmOUT;
//    printf("\nStart Test\n");
//    for(i = 0; i < strlen(testMe); i++){
//        fsmOUT = ProtocolDecode(testMe[i], nData, gData);
//        printf("%d\n", fsmOUT);
//    }
    
// FIELD tests
    
    

/******************************************************************************
 * Your code goes in between this comment and the preceeding one with asterisks
 *****************************************************************************/

    while (1);
}



/**
 * This is the interrupt for the Timer2 peripheral. It just keeps incrementing a counter used to
 * track the time until the first user input.
 */
void __ISR(_TIMER_2_VECTOR, IPL4AUTO) TimerInterrupt100Hz(void)
{
    // Clear the interrupt flag.
    IFS0CLR = 1 << 8;

    // Increment a counter to see the srand() function.
    counter++;

    // Also check for any button events
    buttonEvents = ButtonsCheckEvents();
}