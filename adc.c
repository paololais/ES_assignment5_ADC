#include "adc.h"

void adc_init1() {
    // ADC config
    // Disable all analog inputs first
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
    
    // Enable AN11 (RB11) as analog input
    ANSELBbits.ANSB11 = 1;
    
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

void adc_init2() {
    // ADC config
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;

    ANSELBbits.ANSB4 = 1;
    
    TRISBbits.TRISB4 = 1; // Imposta RB4 come input

    AD1CON1bits.ADON = 0;       // Turn off ADC to configure
    AD1CON1bits.AD12B = 0;      // 10-bit mode
    AD1CON1bits.FORM = 0;       // Integer output
    AD1CON1bits.SSRC = 7;       // Auto conversion after sampling
    AD1CON1bits.ASAM = 0;       // Manual sampling

    AD1CON2bits.CHPS = 0;       // Use CH0 only
    AD1CON3bits.ADRC = 0;       // Use system clock
    AD1CON3bits.SAMC = 16;       // Not used for manual sampling
    AD1CON3bits.ADCS = 8;       // ADC conversion clock select (Tad)

    //AD1CHS0bits.CH0SA = 0;     // Select AN11 as input
    AD1CHS0bits.CH0SA = 4; // Seleziona AN4 (RB4)

    AD1CON1bits.ADON = 1;       // Turn on ADC
}