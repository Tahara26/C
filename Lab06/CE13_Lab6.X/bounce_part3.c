// **** Include libraries here ****
// Standard libraries

//CMPE13 Support Library
#include "BOARD.h"
#include "Buttons.h"
#include "Leds.h"

// Microchip libraries
#include <xc.h>
#include <plib.h>

// User libraries


// **** Set macros and preprocessor directives ****

// **** Declare any datatypes here ****

// **** Define global, module-level, or external variables here ****
static uint8_t buttonEvents;

// **** Declare function prototypes ****

int main(void)
{
    BOARD_Init();

    // Configure Timer 1 using PBCLK as input. This default period will make the LEDs blink at a
    // pretty reasonable rate to start.
    OpenTimer1(T1_ON | T1_SOURCE_INT | T1_PS_1_8, 0xFFFF);

    // Set up the timer interrupt with a priority of 4.
    INTClearFlag(INT_T1);
    INTSetVectorPriority(INT_TIMER_1_VECTOR, INT_PRIORITY_LEVEL_4);
    INTSetVectorSubPriority(INT_TIMER_1_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
    INTEnable(INT_T1, INT_ENABLED);

    /***************************************************************************************************
     * Your code goes in between this comment and the following one with asterisks.
     **************************************************************************************************/

    ButtonsInit();
    LEDS_INIT();
    while (1) {
        if (SWITCH_STATES() == 0x01 && BUTTON_EVENT_1UP){
            
            LEDS_SET();
        }
        else if (SWITCH_STATES() == 0x00 && BUTTON_EVENT_1DOWN){
            
            LEDS_SET();
        }
        if (SWITCH_STATES() == 0x02 && BUTTON_EVENT_2UP){
            
            LEDS_SET();
        }
        else if (SWITCH_STATES() == 0x00 && BUTTON_EVENT_2DOWN){
            
            LEDS_SET();
        }
        if (SWITCH_STATES() == 0x04 && BUTTON_EVENT_3UP){
            
            LEDS_SET();
        }
        else if (SWITCH_STATES() == 0x00 && BUTTON_EVENT_3DOWN){
            
            LEDS_SET();
        }
        if (SWITCH_STATES() == 0x08 && BUTTON_EVENT_4UP){
            
            LEDS_SET();
        }
        else if (SWITCH_STATES() == 0x00 && BUTTON_EVENT_4DOWN){
            
            LEDS_SET();
        }
        
    }
    
    

    /***************************************************************************************************
     * Your code goes in between this comment and the preceding one with asterisks
     **************************************************************************************************/

    while (1);
}

/**
 * This is the interrupt for the Timer1 peripheral. It checks for button events and stores them in a
 * module-level variable.
 */
void __ISR(_TIMER_1_VECTOR, IPL4AUTO) Timer1Handler(void)
{
    buttonEvents = ButtonsCheckEvents();
    
    // Clear the interrupt flag.
    INTClearFlag(INT_T1);
}