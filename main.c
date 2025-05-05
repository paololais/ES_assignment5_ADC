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

CircularBuffer cb_tx;

// Interrupt UART TX
void __attribute__((__interrupt__, __auto_psv__)) _U1TXInterrupt() {
    IFS0bits.U1TXIF = 0; // Clear the TX interrupt flag
    
    char c;
    
    while(U1STAbits.UTXBF == 0){
        // If there are characters in the TX buffer, send them
        if (!cb_is_empty(&cb_tx)) {
            cb_pop(&cb_tx, &c); // Pop a character from the TX buffer
            U1TXREG = c;        // Write the character to the UART TX register
        } else {
            IEC0bits.U1TXIE = 0;
            break;
        }
    }
}

int main(void) {
    // Disable all analog inputs first
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
    
    // Enable AN11 (RB11) as analog input
    ANSELBbits.ANSB11 = 1;
    
    adc_init();
    
    UART1_Init(); // initialize UART1
    
    cb_init(&cb_tx);
    
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

        sprintf(buffer, "ADC=%f*", v_battery);
        IEC0bits.U1TXIE = 0;
        for (int i = 0; i < strlen(buffer); i++) {
            cb_push(&cb_tx, buffer[i]);
        }
        IEC0bits.U1TXIE = 1;
        

        tmr_wait_ms(TIMER1, 500);           // Delay between samples
    }
    
    return 0;
}
