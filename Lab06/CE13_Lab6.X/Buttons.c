/*
 * File:   Buttons.c
 * Author: JustinT
 *
 * Created on February 20, 2018, 2:08 PM
 */

#include "BOARD.h"
#include "Buttons.h"
#include "Leds.h"
#include "xc.h"

// All of the events defined
#define Button_Event_None 0x00000
#define Button_Event_1 0x11111
#define Button_Event_1Down 0x01111
#define Button_Event_1Up 0x10000
#define Button_Event_2 0x22222
#define Button_Event_2Down 0x02222
#define Button_Event_2Up 0x20000
#define Button_Event_3 0x44444
#define Button_Event_3Down 0x04444
#define Button_Event_3Up 0x40000
#define Button_Event_4 0x88888
#define Button_Event_4Down 0x08888
#define Button_Event_4Up 0x80000


// Buttons Initialize from the lab manual
void ButtonsInit(void)
{
    TRISD |= 0x00E0;
    TRISF |= 0x0002;
}

uint8_t ButtonsCheckEvents(void)
{
    // Make 5 different samples 
    static uint8_t save1, save2, save3, save4, save5;
    
    // Initialize result as none
    uint8_t result = BUTTON_EVENT_NONE;
    
    // Result2 is for the previous result 
    static uint8_t result2;

    // Save the five separate Button States
    save5 = save4;
    save4 = save3;
    save3 = save2;
    save2 = save1;
    save1 = BUTTON_STATES();

    //Hold all the samples in one number
    uint32_t comb = 0x00000000;

    // Shift the saved samples so you can combine them at the end
    comb |= save5;
    comb = comb << 4;
    comb |= save4;
    comb = comb << 4;
    comb |= save3;
    comb = comb << 4;
    comb |= save2;
    comb = comb << 4;
    comb |= save1;

    // If & Else If for button 1 
    if (((comb & Button_Event_1) == Button_Event_1Down) && ((result2 & BUTTON_EVENT_1DOWN)) == 0) {
        result |= BUTTON_EVENT_1DOWN;
    } else if (((comb & Button_Event_1) == Button_Event_1Up) && ((result2 & BUTTON_EVENT_1UP)) == 0) {
        result |= BUTTON_EVENT_1UP;
    }

    // If & Else If for button 2
    if (((comb & Button_Event_2) == Button_Event_2Down) && ((result2 & BUTTON_EVENT_2DOWN)) == 0) {
        result |= BUTTON_EVENT_2DOWN;
    } else if (((comb & Button_Event_2) == Button_Event_2Up) && ((result2 & BUTTON_EVENT_2UP)) == 0) {
        result |= BUTTON_EVENT_2UP;
    }

    // If & Else If for button 3
    if (((comb & Button_Event_3) == Button_Event_3Down) && ((result2 & BUTTON_EVENT_3DOWN)) == 0) {
        result |= BUTTON_EVENT_3DOWN;
    } else if (((comb & Button_Event_3) == Button_Event_3Up) && ((result2 & BUTTON_EVENT_3UP)) == 0) {
        result |= BUTTON_EVENT_3UP;
    }

    // If & Else If for button 4
    if (((comb & Button_Event_4) == Button_Event_4Down) && ((result2 & BUTTON_EVENT_4DOWN)) == 0) {
        result |= BUTTON_EVENT_4DOWN;
    } else if (((comb & Button_Event_4) == Button_Event_4Up) && ((result2 & BUTTON_EVENT_4UP)) == 0) {
        result |= BUTTON_EVENT_4UP;
    }

    // Stores the previous result
    result2 = result;
    return result;
}