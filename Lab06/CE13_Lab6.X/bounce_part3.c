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

    // x for setting the leds
    uint8_t x;
    
    // check with the enum
    uint8_t SwitchStored;
    
    // Initialize the Buttons and the LEDs
    ButtonsInit();
    LEDS_INIT();
    
    while (1) {
        // The checker 
        if (buttonEvents != BUTTON_EVENT_NONE) {

            // Store the Switch State to a separate integer
            SwitchStored = SWITCH_STATES();
 
            // Check the first switch and do the corresponding actions for up and down
            if (((SwitchStored & SWITCH_STATE_SW1) == 1) && (buttonEvents & BUTTON_EVENT_1UP)) {  
                x = LEDS_GET() ^ (0x03);
                LEDS_SET(x);
            } 
            else if (((SwitchStored & SWITCH_STATE_SW1) == 0) && (buttonEvents & BUTTON_EVENT_1DOWN)) {
                x = LEDS_GET() ^ (0x03);
                LEDS_SET(x);
            }
            
            // Check the second switch and do the corresponding actions for up and down
            if (((SwitchStored & SWITCH_STATE_SW2) == 2) && (buttonEvents & BUTTON_EVENT_2UP)) {  
                x = LEDS_GET() ^ (0x0C);
                LEDS_SET(x);
            } 
            else if (((SwitchStored & SWITCH_STATE_SW2) == 0) && (buttonEvents & BUTTON_EVENT_2DOWN)) {
                x = LEDS_GET() ^ (0x0C);
                LEDS_SET(x);
            }
            
            // Check the third switch and do the corresponding actions for up and down
            if (((SwitchStored & SWITCH_STATE_SW3) == 4) && (buttonEvents & BUTTON_EVENT_3UP)) {  
                x = LEDS_GET() ^ (0x30);
                LEDS_SET(x);
            } 
            else if (((SwitchStored & SWITCH_STATE_SW3) == 0) && (buttonEvents & BUTTON_EVENT_3DOWN)) {
                x = LEDS_GET() ^ (0x30);
                LEDS_SET(x);
            }
            
            // Check the fourth switch and do the corresponding actions for up and down
            if (((SwitchStored & SWITCH_STATE_SW4) == 8) && (buttonEvents & BUTTON_EVENT_4UP)) {  
                x = LEDS_GET() ^ (0xC0);
                LEDS_SET(x);
            } 
            else if (((SwitchStored & SWITCH_STATE_SW4) == 0) && (buttonEvents & BUTTON_EVENT_4DOWN)) {
                x = LEDS_GET() ^ (0xC0);
                LEDS_SET(x);
            }
            
            // Clears the buttonEvents just in case
            buttonEvents = BUTTON_EVENT_NONE;
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
    // The checker 
    buttonEvents = ButtonsCheckEvents();

    // Clear the interrupt flag.
    INTClearFlag(INT_T1);
}