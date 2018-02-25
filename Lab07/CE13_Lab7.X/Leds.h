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

#define LEDS_SET(x) (LATE = (x))

#endif 
