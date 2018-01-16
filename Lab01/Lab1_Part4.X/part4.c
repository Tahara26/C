/*
 * File:   part4.c
 * Author: JustinT
 *
 * Created on January 16, 2018, 1:15 PM
 */


#include "xc.h"
#include "Ascii.h"
#include "BOARD.h"
#include "Oled.h"
#include "OledDriver.h"

int main(void) {
    BOARD_Init();
    OledInit();
    OledDrawString("Hello, World!\n");
    OledUpdate();
    while (1);
}
