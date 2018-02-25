// **** Include libraries here ****
// Standard libraries

//CMPE13 Support Library
#include "BOARD.h"
#include "Oled.h"
#include "OledDriver.h"

// Microchip libraries
#include <xc.h>
#include <plib.h>

// User libraries


// **** Set macros and preprocessor directives ****

// **** Declare any datatypes here ****
typedef struct AdcResult {
    uint8_t event;
    uint16_t value;
} AdcResult;

// **** Define global, module-level, or external variables here ****
static AdcResult eventdata;
        
// **** Declare function prototypes ****

int main(void)
{
    BOARD_Init();

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
            SKIP_SCAN_ALL
            );
    EnableADC10();

    /***************************************************************************************************
     * Your code goes in between this comment and the following one with asterisks.
     **************************************************************************************************/
    
    // Set a to 1023    
    uint16_t a = 0x03FF;
    
    // Set b to 100
    uint16_t b = 0x0064;
    
    // Sets up a string 
    char string[1000];
    
    // Initializes the OLED
    OledInit();
    
    // While loop that keeps updating to display the current value of the dial 
    while (1) {
        OledDrawString("Potentiometer value:\n");
        sprintf(string,"\n%4d\n%3d%%",eventdata.value, (((eventdata.value)*b)/a));
        OledDrawString(string);
        OledUpdate();
  }

    /***************************************************************************************************
     * Your code goes in between this comment and the preceding one with asterisks
     **************************************************************************************************/

    while (1);
}

/**
 * This is the ISR for the ADC1 peripheral. It runs continuously triggered by the hardware directly
 * and not called by any code, as all interrupts are. During each run it reads all 8 samples from
 * the ADC (registers ADC1BUF0 through ADC1BUF7), averages them, and stores them in a module-level
 * AdcResult struct. If this averaged ADC value has changed between this run and the last one, the
 * event flag in the AdcResult struct is set to true.
 */
void __ISR(_ADC_VECTOR, IPL2AUTO) AdcHandler(void)
{
    // uint8_t values that are set at the beginning to use later on in the function 
    uint16_t i, j;
       
    // Add up all the ADC values
    i = (ADC1BUF0+ADC1BUF1+ADC1BUF2+ADC1BUF3+ADC1BUF4+ADC1BUF5+ADC1BUF6+ADC1BUF7);
    
    // Divide by 8 to get value 
    j = (i/8);
    
    // Checker for an event 
    if (eventdata.value != j) {
        eventdata.event = 1;
        eventdata.value = j; 
    }
    
    // Clear the interrupt flag.
    INTClearFlag(INT_AD1);


}