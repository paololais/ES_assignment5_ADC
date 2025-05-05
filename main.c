/*
 * File:   main.c
 * Author: paolo
 *
 * Created on May 5, 2025, 9:29 AM
 */


#include "xc.h"
#include "adc.h"
#include "timer.h"
#include "uart.h"
#include "stdio.h"
#include "string.h"

int main(void) {
    // Disable all analog inputs first
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
    
    // Enable AN11 (RB11) as analog input
    ANSELBbits.ANSB11 = 1;
    
    adc_init();
    
    unsigned int adc_val;
    char buffer[32];
    float v_sense;
    float v_battery;
    
    while(1){
         AD1CHS0bits.CH0SA = 11;    // Ensure AN11 is selected
        AD1CON1bits.SAMP = 1;      // Start sampling
        tmr_wait_ms(TIMER1, 10);
        AD1CON1bits.SAMP = 0;      // Start conversion

        while (!AD1CON1bits.DONE); // Wait for conversion complete
        adc_val = ADC1BUF0;        // Read ADC result

        //conversion
        v_sense = (adc_val / 1023.0) * 3.3; // Tensione sul BAT-VSENSE
        v_battery = v_sense * 3; // Partitore: BAT-VSENSE = Vbat / 3

        sprintf(buffer, "ADC=%u*", adc_val);
        for (int i = 0; i < strlen(buffer); i++) {
            UART1_WriteChar(buffer[i]);
        }
        memset(buffer, 0, sizeof(buffer));
        

        tmr_wait_ms(500);           // Delay between samples
    }
    
    return 0;
}
