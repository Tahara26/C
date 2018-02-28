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

typedef enum {
    RESET,
    START,
    COUNTDOWN,
    PENDING_SELECTOR_CHANGE,
    PENDING_RESET
} oven_states;

typedef enum {
    BAKE,
    TOAST,
    BROIL
} cooking_modes;

typedef enum {
    TIME,
    TEMPERATURE
} input_selection;

typedef enum {
    BAKEON,
    BAKEOFF1,
    BAKEOFF2,
    TOASTON,
    TOASTOFF,
    BROILON,
    BROILOFF
} oven_draw;

typedef struct OvenData {
    int CookingTimeLeft;
    oven_states OvenState;
    cooking_modes CookingMode;
    input_selection InputSelection;
    uint16_t InitialCookTime;
    uint16_t Temperature;
    uint16_t ButtonPressCounter;
} OvenData;



// **** Define any module-level, global, or external variables here ****
static OvenData oven;
static uint8_t Timer2hz = 0;
static uint16_t Timer5hz = 0;
//static uint16_t Timer100hz = 0;
static uint16_t counter = 0;
static uint8_t buttonEvents;
static int initial1 = 1;
static int initial2 = 350;
static uint16_t doubletime;
static const char line1[20] = "|\x1\x1\x1\x1\x1|";
static const char line2[20] = "|\x2\x2\x2\x2\x2|";
static const char line3[20] = "|     |";
static const char line4[20] = "|-----|";
static const char line5[20] = "|\x3\x3\x3\x3\x3|";
static const char line6[20] = "|\x4\x4\x4\x4\x4|";
static uint16_t countersave;

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

    char buffer[1000];
    //    sprintf(buffer, "%s  Mode:Bake\n%s >Time:  0:01\n%s  Temp: 350%sF\n%s", line1, line2, line3, degree, line4);
    //    sprintf(buffer, "%s\n%s\n%s\n%s", line1, line2, line3, line4);
    OledDrawString(buffer);
    OledUpdate();

    doubletime = ((AdcRead() >> 2)*2);

    while (1) {
        switch (oven.OvenState) {
        case RESET:
            if (oven.CookingMode == BAKE) {
                oven.InitialCookTime = initial1;
                oven.Temperature = initial2;
                oven.CookingTimeLeft = oven.InitialCookTime;
                OledClear(OLED_COLOR_BLACK);
                DrawOven(BAKEOFF1, buffer);
                OledDrawString(buffer);
                OledUpdate();
            } else if (oven.CookingMode == TOAST) {
                oven.InitialCookTime = initial1;
                oven.CookingTimeLeft = oven.InitialCookTime;
                OledClear(OLED_COLOR_BLACK);
                DrawOven(TOASTOFF, buffer);
                OledDrawString(buffer);
                OledUpdate();
            } else if (oven.CookingMode == BROIL) {
                oven.InitialCookTime = initial1;
                oven.CookingTimeLeft = oven.InitialCookTime;
                OledClear(OLED_COLOR_BLACK);
                DrawOven(BROILOFF, buffer);
                OledDrawString(buffer);
                OledUpdate();
            }
            oven.OvenState = START;
            break;
        case START:
            if (AdcChanged()) {
                if (oven.CookingMode == BAKE) {
                    if (oven.InputSelection == TIME) {
                        oven.InitialCookTime = ((AdcRead() >> 2) + 1);
                        DrawOven(BAKEOFF1, buffer);
                        OledDrawString(buffer);
                        OledUpdate();
                    } else if (oven.InputSelection == TEMPERATURE) {
                        oven.Temperature = ((AdcRead() >> 2) + 300);
                        DrawOven(BAKEOFF2, buffer);
                        OledDrawString(buffer);
                        OledUpdate();
                    }
                } else if (oven.CookingMode == TOAST) {
                    oven.InitialCookTime = ((AdcRead() >> 2) + 1);
                    DrawOven(TOASTOFF, buffer);
                    OledDrawString(buffer);
                    OledUpdate();
                } else if (oven.CookingMode == BROIL) {
                    oven.InitialCookTime = ((AdcRead() >> 2) + 1);
                    DrawOven(BROILOFF, buffer);
                    OledDrawString(buffer);
                    OledUpdate();
                }
            }
            if (buttonEvents == BUTTON_EVENT_3DOWN) {
                countersave = counter;
                buttonEvents = BUTTON_EVENT_NONE;
                oven.OvenState = PENDING_SELECTOR_CHANGE;
            }
            if (buttonEvents == BUTTON_EVENT_4DOWN){
                
                oven.OvenState = COUNTDOWN;
            }
            break;
        case COUNTDOWN:

            break;
        case PENDING_SELECTOR_CHANGE:
            oven.ButtonPressCounter = (countersave - counter);
            if (oven.ButtonPressCounter >= LONG_PRESS) {

            }
            if ((oven.ButtonPressCounter < LONG_PRESS) && (buttonEvents == BUTTON_EVENT_3UP)) {
                if (oven.CookingMode == BAKE) {
                    oven.InitialCookTime = initial1;
                    oven.Temperature = initial2;
                    oven.CookingTimeLeft = oven.InitialCookTime;
                    OledClear(OLED_COLOR_BLACK);
                    DrawOven(TOASTOFF, buffer);
                    OledDrawString(buffer);
                    OledUpdate();
                    oven.CookingMode = TOAST;
                } else if (oven.CookingMode == TOAST) {
                    oven.InitialCookTime = initial1;
                    oven.CookingTimeLeft = oven.InitialCookTime;
                    OledClear(OLED_COLOR_BLACK);
                    DrawOven(BROILOFF, buffer);
                    OledDrawString(buffer);
                    OledUpdate();
                    oven.CookingMode = BROIL;
                } else if (oven.CookingMode == BROIL) {
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
        sprintf(buffer, "%s  Mode: Bake\n%s  Time:  %1d:%02d\n%s  Temp: %d%cF\n%s", line1, line3, oven.CookingTimeLeft / 60,
                oven.CookingTimeLeft % 60, line4, oven.Temperature, 0xF8, line5);
        break;
    case BAKEOFF1:
        sprintf(buffer, "%s  Mode: Bake\n%s >Time:  %1d:%02d\n%s  Temp: %d%cF\n%s", line2, line3, oven.InitialCookTime / 60,
                oven.InitialCookTime % 60, line4, oven.Temperature, 0xF8, line6);
        break;
    case BAKEOFF2:
        sprintf(buffer, "%s  Mode: Bake\n%s  Time:  %1d:%02d\n%s >Temp: %d%cF\n%s", line2, line3, oven.InitialCookTime / 60,
                oven.InitialCookTime % 60, line4, oven.Temperature, 0xF8, line6);
        break;
    case TOASTON:
        sprintf(buffer, "%s  Mode: Toast\n%s  Time:  %1d:%02d\n%s\n%s", line2, line3, oven.CookingTimeLeft / 60,
                oven.CookingTimeLeft % 60, line4, line5);
        break;
    case TOASTOFF:
        sprintf(buffer, "%s  Mode: Toast\n%s  Time:  %1d:%02d\n%s\n%s", line2, line3, oven.InitialCookTime / 60,
                oven.InitialCookTime % 60, line4, line6);
        break;
    case BROILON:
        sprintf(buffer, "%s  Mode: Broil\n%s  Time:  %1d:%02d\n%s  Temp: 500%cF\n%s", line1, line3, oven.CookingTimeLeft / 60,
                oven.CookingTimeLeft % 60, line4, 0xF8, line6);
        break;
    case BROILOFF:
        sprintf(buffer, "%s  Mode: Broil\n%s  Time:  %1d:%02d\n%s  Temp: 500%cF\n%s", line2, line3, oven.InitialCookTime / 60,
                oven.InitialCookTime % 60, line4, 0xF8, line6);
        break;
    }
}