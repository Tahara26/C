//Justin Tahara
//2-16-18
#include "BOARD.h"
#include <xc.h>

#ifndef LEDS_H    /* Guard against multiple inclusion */
#define LEDS_H

#define LEDS_INIT() do{ \
    TRISE = 0x0; \
    LATE = 0x0; \
    } while(0)

#define LEDS_GET() LATE
   
#define LEDS_SET(x) LATE = (x)

#define LD1 PORTEbits.RE0
#define LD2 PORTEbits.RE1
#define LD3 PORTEbits.RE2
#define LD4 PORTEbits.RE3
#define LD5 PORTEbits.RE4
#define LD6 PORTEbits.RE5
#define LD7 PORTEbits.RE6
#define LD8 PORTEbits.RE7

#endif 
