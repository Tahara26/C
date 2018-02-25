// **** Include libraries here ****
// Standard libraries

//CMPE13 Support Library
#include "BOARD.h"
#include "Buttons.h"
#include "Leds.h"
#include "Oled.h"
#include "OledDriver.h"
#include "Ascii.h"

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

typedef struct AdcResult {
    uint8_t event;
    uint16_t value;
} AdcResult;

// **** Define global, module-level, or external variables here ****
static TimerResult Eventdata;
#define LEFT 1
#define RIGHT 0

static AdcResult eventdata;

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



    // Enable interrupts for the ADC
    ConfigIntADC10(ADC_INT_PRI_2 | ADC_INT_SUB_PRI_0 | ADC_INT_ON);

    // Set B2 to an input so AN0 can be used by the ADC.
    TRISBCLR = 1 << 2;

    // Configure and start the ADC
    // Read AN0 as sample a. We don't use alternate sampling, so setting sampleb is pointless.
    SetChanADC10(ADC_CH0_NEG_SAMPLEA_NVREF | ADC_CH0_POS_SAMPLEA_AN2);
    OpenADC10(
            ADC_MODULE_ON | ADC_IDLE_CONTINUE | ADC_FORMAT_INTG16 | ADC_CLK_AUTO | ADC_AUTO_SAMPLING_ON,
            ADC_VREF_AVDD_AVSS | ADC_OFFSET_CAL_DISABLE | ADC_SCAN_OFF | ADC_SAMPLES_PER_INT_8 |
            ADC_BUF_16 | ADC_ALT_INPUT_OFF,
            ADC_SAMPLE_TIME_29 | ADC_CONV_CLK_PB | ADC_CONV_CLK_51Tcy2,
            ENABLE_AN2_ANA,
            SKIP_SCAN_ALL);
    EnableADC10();

    /***************************************************************************************************
     * Your code goes in between this comment and the following one with asterisks.
     **************************************************************************************************/

    // Initialize the LEDs
    LEDS_INIT();
    uint8_t i, j, k;
    i = 0x00;
                


    // Set a to 1023    
    uint16_t a = 0x03FF;

    // Set b to 100
    uint16_t b = 0x0064;

    // Sets up a string 
    char string[1000];

    // Initializes the OLED
    OledInit();

    // x for setting the leds
    uint8_t x;

    // check with the enum
    uint8_t SwitchStored;

    // Initialize the Buttons
    ButtonsInit();
LEDS_SET(i);
    // Loop that keeps going in a cycle bouncing back and forth infinitely
    while (1) {

        // While loop that keeps updating to display the current value of the dial 

        OledDrawString("Potentiometer value:\n");
        sprintf(string, "\n%4d\n%3d%%", eventdata.value, (((eventdata.value) * b) / a));
        OledDrawString(string);
        OledUpdate();
        
        // Checks the event 
        if (Eventdata.event) {
            Eventdata.event = 0;
            // Set the LEDs so the far right one is the only one on
            //LEDS_SET(i);
            // If the far left LED is on then switch direction to right
            if (LEDS_GET() == 0x80) {
                j = RIGHT;
            }

            // If the far right LED is on then switch the direction to left
            if ((LEDS_GET() == 0x01) || (LEDS_GET() == 0x00)) {
                j = LEFT;
            }

            // If the value was set to right then iterate the led to the right and set it
            if (j == RIGHT) {
                i = i >> 1;
                
                //LEDS_SET(i);
            }

            // If the value was set to right then iterate the led to the left and set it 
            if (j == LEFT) {
                i = i << 1;
                
                //LEDS_SET(i);
            }
        }
        
        // The checker 
        if (buttonEvents != BUTTON_EVENT_NONE) {

            // Store the Switch State to a separate integer
            SwitchStored = SWITCH_STATES();

            // Check the first switch and do the corresponding actions for up and down
            if (((SwitchStored & SWITCH_STATE_SW1) == 1) && (buttonEvents & BUTTON_EVENT_1UP)) {
                x = LEDS_GET() ^ (0x03);
                k = i | x;
                LEDS_SET(k);
            } else if (((SwitchStored & SWITCH_STATE_SW1) == 0) && (buttonEvents & BUTTON_EVENT_1DOWN)) {
                x = LEDS_GET() ^ (0x03);
                k = i | x;
                LEDS_SET(k);
            }

            // Check the second switch and do the corresponding actions for up and down
            if (((SwitchStored & SWITCH_STATE_SW2) == 2) && (buttonEvents & BUTTON_EVENT_2UP)) {
                x = LEDS_GET() ^ (0x0C);
                k = i | x;
                LEDS_SET(k);
            } else if (((SwitchStored & SWITCH_STATE_SW2) == 0) && (buttonEvents & BUTTON_EVENT_2DOWN)) {
                x = LEDS_GET() ^ (0x0C);
                k = i | x;
                LEDS_SET(k);
            }

            // Check the third switch and do the corresponding actions for up and down
            if (((SwitchStored & SWITCH_STATE_SW3) == 4) && (buttonEvents & BUTTON_EVENT_3UP)) {
                x = LEDS_GET() ^ (0x30);
                k = i | x;
                LEDS_SET(k);
            } else if (((SwitchStored & SWITCH_STATE_SW3) == 0) && (buttonEvents & BUTTON_EVENT_3DOWN)) {
                x = LEDS_GET() ^ (0x30);
                k = i | x;
                LEDS_SET(k);
            }

            // Check the fourth switch and do the corresponding actions for up and down
            if (((SwitchStored & SWITCH_STATE_SW4) == 8) && (buttonEvents & BUTTON_EVENT_4UP)) {
                x = LEDS_GET() ^ (0xC0);
                k = i | x;
                LEDS_SET(k);
            } else if (((SwitchStored & SWITCH_STATE_SW4) == 0) && (buttonEvents & BUTTON_EVENT_4DOWN)) {
                x = LEDS_GET() ^ (0xC0);
                k = i | x;
                LEDS_SET(k);
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
 * module-level variable. Additionally during each call it increments a counter (the value member of
 * a module-level TimerResult struct). This counter is then checked against the top four bits of the
 * ADC result, and if it's greater, then the event member of a module-level TimerResult struct is
 * set to true and the value member is cleared.
 */
void __ISR(_TIMER_1_VECTOR, IPL4AUTO) Timer1Handler(void)
{
    buttonEvents = ButtonsCheckEvents();

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

/**
 * This is the ISR for the ADC1 peripheral. It has been enabled to run continuously. Reads all 8
 * samples from the ADC, averages them, and stores them in a module-level variable for use in the
 * main event loop.
 */
void __ISR(_ADC_VECTOR, IPL2AUTO) AdcHandler(void)
{
    // uint8_t values that are set at the beginning to use later on in the function 
    uint16_t i, j;

    // Add up all the ADC values
    i = (ADC1BUF0 + ADC1BUF1 + ADC1BUF2 + ADC1BUF3 + ADC1BUF4 + ADC1BUF5 + ADC1BUF6 + ADC1BUF7);

    // Divide by 8 to get value 
    j = (i / 8);

    // Checker for an event 
    if (eventdata.value != j) {
        eventdata.event = 1;
        eventdata.value = j;
    }

    // Clear the interrupt flag.
    INTClearFlag(INT_AD1);

}