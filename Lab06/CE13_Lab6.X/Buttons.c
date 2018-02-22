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

void ButtonsInit(void)
{
    TRISD |= 0x00E0;
    TRISF |= 0x0002;
}

uint8_t ButtonsCheckEvents(void)
{
    static uint8_t save1, save2, save3, save4, save5;
    
    save5 = save4;
    save4 = save3;
    save3 = save2;
    save2 = save1;
    save1 = BUTTON_STATES();
    
    //Hold all the samples in one number
    uint32_t comb = 0x00000000; 
    
    comb |= save5;
    comb = comb << 4;
    comb |= save4;
    comb = comb << 4;
    comb |= save3;
    comb = comb << 4;
    comb |= save2;
    comb = comb << 4;
    comb |= save1;
    
    if (comb == Button_Event_None) {
        return BUTTON_EVENT_NONE;
    }
    
    if (comb == Button_Event_1) {
        return BUTTON_EVENT_NONE;
    }
    
    if (comb == Button_Event_2) {
        return BUTTON_EVENT_NONE;
    }
    
    if (comb == Button_Event_3) {
        return BUTTON_EVENT_NONE;
    }
    
    if (comb == Button_Event_4) {
        return BUTTON_EVENT_NONE;
    }
    
    if (comb == Button_Event_1Down) {
        return BUTTON_EVENT_1DOWN;
    }
    
    if (comb == Button_Event_1Up) {
        return BUTTON_EVENT_1UP;
    }
    
    if (comb == Button_Event_2Down) {
        return BUTTON_EVENT_2DOWN;
    }
    
    if (comb == Button_Event_2Up) {
        return BUTTON_EVENT_2UP;
    }
    
    if (comb == Button_Event_3Down) {
        return BUTTON_EVENT_3DOWN;
    }
    
    if (comb == Button_Event_3Up) {
        return BUTTON_EVENT_3UP;
    }
    
    if (comb == Button_Event_4Down) {
        return BUTTON_EVENT_4DOWN;
    }
    
    if (comb == Button_Event_4Up) {
        return BUTTON_EVENT_4UP;
    }
    
}