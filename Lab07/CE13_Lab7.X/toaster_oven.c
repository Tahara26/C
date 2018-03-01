// **** Include libraries here ****
// Standard libraries

//CMPE13 Support Library
#include "BOARD.h"
#include "Oled.h"
#include "Adc.h"
#include "Buttons.h"
#include "Leds.h"

// Microchip libraries
#include <xc.h>
#include <plib.h>



// **** Set any macros or preprocessor directives here ****
// Set a macro for resetting the timer, makes the code a little clearer.
#define TIMER_2HZ_RESET() (TMR1 = 0)

// **** Declare any datatypes here ****
#define LONG_PRESS 5

// Type define all of the states

typedef enum {
    RESET,
    START,
    COUNTDOWN,
    PENDING_SELECTOR_CHANGE,
    PENDING_RESET
} oven_states;

// Type define all of the modes

typedef enum {
    BAKE,
    TOAST,
    BROIL
} cooking_modes;

// Type define for the two types in Bake

typedef enum {
    TIME,
    TEMPERATURE
} input_selection;

// Type define for drawing oven

typedef enum {
    BAKEON,
    BAKEOFF1,
    BAKEOFF2,
    TOASTON,
    TOASTOFF,
    BROILON,
    BROILOFF
} oven_draw;

// Struct for all of the different variables

typedef struct OvenData {
    uint16_t CookingTimeLeft;
    oven_states OvenState;
    cooking_modes CookingMode;
    input_selection InputSelection;
    uint16_t InitialCookTime;
    uint16_t Temperature;
    uint16_t ButtonPressCounter;
    uint16_t DoubleTime;
} OvenData;

// **** Define any module-level, global, or external variables here ****
static OvenData oven;
static uint8_t Timer2hz = 0;
static uint16_t counter = 0;
static uint8_t buttonEvents;
static int initial1 = 1;
static int initial2 = 350;
static const char line1[20] = "|\x1\x1\x1\x1\x1|";
static const char line2[20] = "|\x2\x2\x2\x2\x2|";
static const char line3[20] = "|     |";
static const char line4[20] = "|-----|";
static const char line5[20] = "|\x3\x3\x3\x3\x3|";
static const char line6[20] = "|\x4\x4\x4\x4\x4|";
static uint16_t countersave;
static uint8_t led;
void DrawOven(oven_draw type, char *buffer);

// Configuration Bit settings

int main()
{
    BOARD_Init();

    // Configure Timer 1 using PBCLK as input. We configure it using a 1:256 prescalar, so each timer
    // tick is actually at F_PB / 256 Hz, so setting PR1 to F_PB / 256 / 2 yields a 0.5s timer.
    OpenTimer1(T1_ON | T1_SOURCE_INT | T1_PS_1_256, BOARD_GetPBClock() / 256 / 2);

    // Set up the timer interrupt with a medium priority of 4.
    INTClearFlag(INT_T1);
    INTSetVectorPriority(INT_TIMER_1_VECTOR, INT_PRIORITY_LEVEL_4);
    INTSetVectorSubPriority(INT_TIMER_1_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
    INTEnable(INT_T1, INT_ENABLED);

    // Configure Timer 2 using PBCLK as input. We configure it using a 1:16 prescalar, so each timer
    // tick is actually at F_PB / 16 Hz, so setting PR2 to F_PB / 16 / 100 yields a .01s timer.
    OpenTimer2(T2_ON | T2_SOURCE_INT | T2_PS_1_16, BOARD_GetPBClock() / 16 / 100);

    // Set up the timer interrupt with a medium priority of 4.
    INTClearFlag(INT_T2);
    INTSetVectorPriority(INT_TIMER_2_VECTOR, INT_PRIORITY_LEVEL_4);
    INTSetVectorSubPriority(INT_TIMER_2_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
    INTEnable(INT_T2, INT_ENABLED);

    // Configure Timer 3 using PBCLK as input. We configure it using a 1:256 prescalar, so each timer
    // tick is actually at F_PB / 256 Hz, so setting PR3 to F_PB / 256 / 5 yields a .2s timer.
    OpenTimer3(T3_ON | T3_SOURCE_INT | T3_PS_1_256, BOARD_GetPBClock() / 256 / 5);

    // Set up the timer interrupt with a medium priority of 4.
    INTClearFlag(INT_T3);
    INTSetVectorPriority(INT_TIMER_3_VECTOR, INT_PRIORITY_LEVEL_4);
    INTSetVectorSubPriority(INT_TIMER_3_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
    INTEnable(INT_T3, INT_ENABLED);

    /***************************************************************************************************
     * Your code goes in between this comment and the following one with asterisks.
     **************************************************************************************************/

    OledInit();
    LEDS_INIT();
    ButtonsInit();
    AdcInit();

    // Set array length for buffer
    char buffer[1000];

    // While loop for infinite cycles
    while (1) {
        // Switch statement for all states
        switch (oven.OvenState) {

        case RESET:
            // If mode is bake 
            if (oven.CookingMode == BAKE) {
                oven.InitialCookTime = initial1;
                oven.Temperature = initial2;
                oven.CookingTimeLeft = oven.InitialCookTime;
                OledClear(OLED_COLOR_BLACK);
                DrawOven(BAKEOFF1, buffer);
                OledDrawString(buffer);
                OledUpdate();
            }// If mode is toast
            else if (oven.CookingMode == TOAST) {
                oven.InitialCookTime = initial1;
                oven.CookingTimeLeft = oven.InitialCookTime;
                OledClear(OLED_COLOR_BLACK);
                DrawOven(TOASTOFF, buffer);
                OledDrawString(buffer);
                OledUpdate();
            }// If mode is broil
            else if (oven.CookingMode == BROIL) {
                oven.InitialCookTime = initial1;
                oven.CookingTimeLeft = oven.InitialCookTime;
                OledClear(OLED_COLOR_BLACK);
                DrawOven(BROILOFF, buffer);
                OledDrawString(buffer);
                OledUpdate();
            }
            // Go to Start state
            oven.OvenState = START;
            break;

        case START:

            // If the adc value is altered by user 
            if (AdcChanged()) {

                // Changing values for the bake mode
                if (oven.CookingMode == BAKE) {

                    // Changing the time value for Bake
                    if (oven.InputSelection == TIME) {
                        oven.InitialCookTime = ((AdcRead() >> 2) + 1);
                        DrawOven(BAKEOFF1, buffer);
                        OledDrawString(buffer);
                        OledUpdate();
                    }// Changing the temperature value for Bake
                    else if (oven.InputSelection == TEMPERATURE) {
                        oven.Temperature = ((AdcRead() >> 2) + 300);
                        DrawOven(BAKEOFF2, buffer);
                        OledDrawString(buffer);
                        OledUpdate();
                    }
                }                    // Changing the time value for Toast
                else if (oven.CookingMode == TOAST) {
                    oven.InitialCookTime = ((AdcRead() >> 2) + 1);
                    DrawOven(TOASTOFF, buffer);
                    OledDrawString(buffer);
                    OledUpdate();
                }                    // Changing the time value for Broil
                else if (oven.CookingMode == BROIL) {
                    oven.InitialCookTime = ((AdcRead() >> 2) + 1);
                    DrawOven(BROILOFF, buffer);
                    OledDrawString(buffer);
                    OledUpdate();
                }
            }

            // If button 3 is pressed store the snapshot and move on
            if (buttonEvents == BUTTON_EVENT_3DOWN) {
                countersave = counter;
                buttonEvents = BUTTON_EVENT_NONE;
                oven.OvenState = PENDING_SELECTOR_CHANGE;
            }

            // If button 4 is pressed go to countdown 
            if (buttonEvents == BUTTON_EVENT_4DOWN) {
                Timer2hz = FALSE;
                buttonEvents = BUTTON_EVENT_NONE;
                oven.CookingTimeLeft = oven.InitialCookTime;
                oven.DoubleTime = (oven.CookingTimeLeft * 2);
                oven.OvenState = COUNTDOWN;
            }
            break;

        case COUNTDOWN:

            if (oven.CookingTimeLeft >= 0) {
                led = 0x00;
            }

            if ((oven.CookingTimeLeft*1000) >= ((oven.InitialCookTime)*1000) >> 3) {
                led = 0x80;
            }

            if ((oven.CookingTimeLeft*1000) >= ((oven.InitialCookTime)*2000) >> 3) {
                led = 0xC0;
            }

            if ((oven.CookingTimeLeft*1000) >= ((oven.InitialCookTime)*3000) >> 3) {
                led = 0xE0;
            }

            if ((oven.CookingTimeLeft*1000) >= ((oven.InitialCookTime)*4000) >> 3) {
                led = 0xF0;
            }

            if ((oven.CookingTimeLeft*1000) >= ((oven.InitialCookTime)*5000) >> 3) {
                led = 0xF8;
            }

            if ((oven.CookingTimeLeft*1000) >= ((oven.InitialCookTime)*6000) >> 3) {
                led = 0xFC;
            }

            if ((oven.CookingTimeLeft*1000) >= ((oven.InitialCookTime)*7000) >> 3) {
                led = 0xFE;
            }

            if ((oven.CookingTimeLeft*1000) >= ((oven.InitialCookTime)*8000) >> 3) {
                led = 0xFF;
            }

            // Save the snapshot of the counter
            if (buttonEvents == BUTTON_EVENT_4DOWN) {
                countersave = counter;
                buttonEvents = BUTTON_EVENT_NONE;
                oven.OvenState = PENDING_RESET;
            }

            // Begin the countdown on the Oled display 
            if ((Timer2hz == TRUE) && (oven.CookingTimeLeft > 0)) {

                // Reset the 2hz flag
                Timer2hz = FALSE;

                // Decrement the timer
                oven.DoubleTime -= 1;

                // Make sure the cooking time left is divided by two
                oven.CookingTimeLeft = (oven.DoubleTime >> 1);

                // If Bake print bake on
                if (oven.CookingMode == BAKE) {
                    OledClear(OLED_COLOR_BLACK);
                    DrawOven(BAKEON, buffer);
                    OledDrawString(buffer);
                    OledUpdate();
                }                    // If Toast print toast on
                else if (oven.CookingMode == TOAST) {
                    OledClear(OLED_COLOR_BLACK);
                    DrawOven(TOASTON, buffer);
                    OledDrawString(buffer);
                    OledUpdate();
                }                    // If Broil print broil on
                else if (oven.CookingMode == BROIL) {
                    OledClear(OLED_COLOR_BLACK);
                    DrawOven(BROILON, buffer);
                    OledDrawString(buffer);
                    OledUpdate();
                }
                oven.OvenState = COUNTDOWN;
            }

            // If the time runs out go back to reset
            if ((Timer2hz == TRUE) && (oven.CookingTimeLeft == 0)) {
                Timer2hz = FALSE;
                oven.OvenState = RESET;
            }
            break;

        case PENDING_SELECTOR_CHANGE:

            // Stores the held down value of the button
            oven.ButtonPressCounter = (counter - countersave);

            // If there is a long press
            if (oven.ButtonPressCounter >= LONG_PRESS) {

                // For the bake mode initialize everything 
                if (oven.CookingMode == BAKE) {

                    // If state is at Time then move to temperature state
                    if (oven.InputSelection == TIME) {
                        oven.InputSelection = TEMPERATURE;
                        oven.CookingTimeLeft = oven.InitialCookTime;
                        OledClear(OLED_COLOR_BLACK);
                        DrawOven(BAKEOFF2, buffer);
                        OledDrawString(buffer);
                        OledUpdate();
                    }                        // If state is at Temperature then move to time state
                    else if (oven.InputSelection == TEMPERATURE) {
                        oven.InputSelection = TIME;
                        oven.CookingTimeLeft = oven.InitialCookTime;
                        OledClear(OLED_COLOR_BLACK);
                        DrawOven(BAKEOFF1, buffer);
                        OledDrawString(buffer);
                        OledUpdate();
                    }
                    oven.OvenState = START;
                }                    // If Toast just move on 
                else if (oven.CookingMode == TOAST) {
                    oven.OvenState = START;
                }                    // If Broil just move on 
                else if (oven.CookingMode == BROIL) {
                    oven.OvenState = START;
                }
            }

            // If short press then move on to the next mode
            if (buttonEvents == BUTTON_EVENT_3UP) {

                // If short press was done during bake mode it should move to Toast mode 
                if (oven.CookingMode == BAKE) {
                    oven.InitialCookTime = initial1;
                    oven.Temperature = initial2;
                    oven.CookingTimeLeft = oven.InitialCookTime;
                    OledClear(OLED_COLOR_BLACK);
                    DrawOven(TOASTOFF, buffer);
                    OledDrawString(buffer);
                    OledUpdate();
                    oven.CookingMode = TOAST;
                }                    // If short press was done during toast mode it should move to Broil mode
                else if (oven.CookingMode == TOAST) {
                    oven.InitialCookTime = initial1;
                    oven.CookingTimeLeft = oven.InitialCookTime;
                    OledClear(OLED_COLOR_BLACK);
                    DrawOven(BROILOFF, buffer);
                    OledDrawString(buffer);
                    OledUpdate();
                    oven.CookingMode = BROIL;
                }                    // If short press was done during broil mode it should go back to Bake mode
                else if (oven.CookingMode == BROIL) {
                    oven.InitialCookTime = initial1;
                    oven.CookingTimeLeft = oven.InitialCookTime;
                    OledClear(OLED_COLOR_BLACK);
                    DrawOven(BAKEOFF1, buffer);
                    OledDrawString(buffer);
                    OledUpdate();
                    oven.CookingMode = BAKE;
                }
                oven.OvenState = START;
            }
            break;

        case PENDING_RESET:

            // Stores the held down value of the button
            oven.ButtonPressCounter = (counter - countersave);

            // If short press then move to next mode 
            if (buttonEvents == BUTTON_EVENT_4UP) {
                buttonEvents = BUTTON_EVENT_NONE;
                oven.OvenState = COUNTDOWN;
            }                // Continue the countdown 
            else if ((Timer2hz == TRUE) && (oven.CookingTimeLeft > 0)) {

                // Decrement the timer
                oven.DoubleTime -= 1;

                // Make sure the cooking time left is divided by two
                oven.CookingTimeLeft = (oven.DoubleTime >> 1);

                // Reset the flag
                Timer2hz = FALSE;

                // If Bake print bake on
                if (oven.CookingMode == BAKE) {
                    OledClear(OLED_COLOR_BLACK);
                    DrawOven(BAKEON, buffer);
                    OledDrawString(buffer);
                    OledUpdate();
                }                    // If Toast print toast on
                else if (oven.CookingMode == TOAST) {
                    OledClear(OLED_COLOR_BLACK);
                    DrawOven(TOASTON, buffer);
                    OledDrawString(buffer);
                    OledUpdate();
                }                    // If Broil print broil on
                else if (oven.CookingMode == BROIL) {
                    OledClear(OLED_COLOR_BLACK);
                    DrawOven(BROILON, buffer);
                    OledDrawString(buffer);
                    OledUpdate();
                }
            }                // If long press go to reset
            else if (oven.ButtonPressCounter >= LONG_PRESS) {
                Timer2hz = FALSE;
                oven.OvenState = RESET;
            }
            break;
        }
    }

    /***************************************************************************************************
     * Your code goes in between this comment and the preceding one with asterisks
     **************************************************************************************************/
    while (1);
}

void __ISR(_TIMER_1_VECTOR, ipl4auto) TimerInterrupt2Hz(void)
{
    Timer2hz = TRUE;

    // Clear the interrupt flag.
    IFS0CLR = 1 << 4;

}

void __ISR(_TIMER_3_VECTOR, ipl4auto) TimerInterrupt5Hz(void)
{
    counter++;

    // Clear the interrupt flag.
    IFS0CLR = 1 << 12;

}

void __ISR(_TIMER_2_VECTOR, ipl4auto) TimerInterrupt100Hz(void)
{
    buttonEvents = ButtonsCheckEvents();

    // Clear the interrupt flag.
    IFS0CLR = 1 << 8;

}

void DrawOven(oven_draw type, char *buffer)
{
    switch (type) {
    case BAKEON:
        sprintf(buffer, "%s  Mode: Bake\n%s  Time:  %1d:%02d\n%s  Temp: %d%cF\n%s", line1, line3,
                oven.CookingTimeLeft / 60, oven.CookingTimeLeft % 60, line4, oven.Temperature, 0xF8, line5);
        LEDS_SET(led);
        break;
    case BAKEOFF1:
        sprintf(buffer, "%s  Mode: Bake\n%s >Time:  %1d:%02d\n%s  Temp: %d%cF\n%s", line2, line3,
                oven.InitialCookTime / 60, oven.InitialCookTime % 60, line4, oven.Temperature, 0xF8, line6);
        break;
    case BAKEOFF2:
        sprintf(buffer, "%s  Mode: Bake\n%s  Time:  %1d:%02d\n%s >Temp: %d%cF\n%s", line2, line3,
                oven.InitialCookTime / 60, oven.InitialCookTime % 60, line4, oven.Temperature, 0xF8, line6);
        break;
    case TOASTON:
        sprintf(buffer, "%s  Mode: Toast\n%s  Time:  %1d:%02d\n%s\n%s", line2, line3,
                oven.CookingTimeLeft / 60, oven.CookingTimeLeft % 60, line4, line5);
        LEDS_SET(led);
        break;
    case TOASTOFF:
        sprintf(buffer, "%s  Mode: Toast\n%s  Time:  %1d:%02d\n%s\n%s", line2, line3,
                oven.InitialCookTime / 60, oven.InitialCookTime % 60, line4, line6);
        break;
    case BROILON:
        sprintf(buffer, "%s  Mode: Broil\n%s  Time:  %1d:%02d\n%s  Temp: 500%cF\n%s", line1, line3,
                oven.CookingTimeLeft / 60, oven.CookingTimeLeft % 60, line4, 0xF8, line6);
        LEDS_SET(led);
        break;
    case BROILOFF:
        sprintf(buffer, "%s  Mode: Broil\n%s  Time:  %1d:%02d\n%s  Temp: 500%cF\n%s", line2, line3,
                oven.InitialCookTime / 60, oven.InitialCookTime % 60, line4, 0xF8, line6);
        break;
    }
}



