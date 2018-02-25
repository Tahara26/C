// **** Include libraries here ****
// Standard libraries

//CMPE13 Support Library
#include "BOARD.h"
#include "Leds.h"

// Microchip libraries
#include <xc.h>
#include <plib.h>

// User libraries

// **** Set macros and preprocessor directives ****

// **** Declare any datatypes here ****
typedef struct TimerResult {
    uint8_t event;
    uint8_t value;
} TimerResult;

// **** Define global, module-level, or external variables here ****
static TimerResult Eventdata;
#define LEFT 1
#define RIGHT 0

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

    // Initialize the LEDs
    LEDS_INIT();
    uint8_t i, j;
    i = 0x01;
    // Set the LEDs so the far right one is the only one on
    LEDS_SET(i);

    // Loop that keeps going in a cycle bouncing back and forth infinitely
    while (1) {
        // Checks the event 
        if (Eventdata.event) {
            Eventdata.event = 0;

            // If the far left LED is on then switch direction to right
            if (LEDS_GET() == 0x80) {
                j = RIGHT;
            }
            
            // If the far right LED is on then switch the direction to left
            if (LEDS_GET() == 0x01) {
                j = LEFT;
            }
            
            // If the value was set to right then iterate the led to the right and set it
            if (j == RIGHT) {
                i = i >> 1;
                LEDS_SET(i);
            }
            
            // If the value was set to right then iterate the led to the left and set it 
            if (j == LEFT) {
                i = i << 1;
                LEDS_SET(i);
            }
        }
    }

    /***************************************************************************************************
     * Your code goes in between this comment and the preceding one with asterisks
     **************************************************************************************************/

    while (1);
}

/**
 * This is the interrupt for the Timer1 peripheral. During each call it increments a counter (the
 * value member of a module-level TimerResult struct). This counter is then checked against the
 * binary values of the four switches on the I/O Shield (where SW1 has a value of 1, SW2 has a value
 * of 2, etc.). If the current counter is greater than this switch value, then the event member of a
 * module-level TimerResult struct is set to true and the value member is cleared.
 */
void __ISR(_TIMER_1_VECTOR, IPL4AUTO) Timer1Handler(void)
{
    // Iterate the value
    Eventdata.value++;

    // The checker for an event 
    if (Eventdata.value >= SWITCH_STATES()) {
        Eventdata.event = 1;
        Eventdata.value = 0;
    }
    
    // Clear the interrupt flag.
    INTClearFlag(INT_T1);


}