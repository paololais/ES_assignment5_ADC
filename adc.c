#include "adc.h"

void adc_init() {
    // ADC config
    AD1CON1bits.ADON = 0;       // Turn off ADC to configure
    AD1CON1bits.AD12B = 0;      // 10-bit mode
    AD1CON1bits.FORM = 0;       // Integer output
    AD1CON1bits.SSRC = 0;       // Manual conversion trigger
    AD1CON1bits.ASAM = 0;       // Manual sampling

    AD1CON2bits.CHPS = 0;       // Use CH0 only
    AD1CON3bits.ADRC = 0;       // Use system clock
    AD1CON3bits.SAMC = 0;       // Not used for manual sampling
    AD1CON3bits.ADCS = 8;       // ADC conversion clock select (Tad)

    AD1CHS0bits.CH0SA = 11;     // Select AN11 as input

    AD1CON1bits.ADON = 1;       // Turn on ADC
}
