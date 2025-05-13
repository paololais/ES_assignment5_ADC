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
char buffer[32];
float distance;
float v_battery;
volatile unsigned int sample_count = 0;

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

void readBattery() {
    unsigned int adc_val;
    float v_sense;

    AD1CON1bits.SAMP = 1; // Start sampling
    tmr_wait_ms(TIMER1, 10);
    AD1CON1bits.SAMP = 0; // Start conversion

    while (!AD1CON1bits.DONE); // Wait for conversion complete
    adc_val = ADC1BUF0; // Read ADC result

    //conversion
    v_sense = (adc_val / 1024.0) * 3.3; // Tensione sul BAT-VSENSE
    v_battery = v_sense * 3; // Partitore: BAT-VSENSE = Vbat / 3

    sprintf(buffer, "ADC=%f*", v_battery);
    IEC0bits.U1TXIE = 0;
    for (int i = 0; i < strlen(buffer); i++) {
        cb_push(&cb_tx, buffer[i]);
    }
    IEC0bits.U1TXIE = 1;


    tmr_wait_ms(TIMER1, 500); // Delay between samples
}

void readDistance(){
    AD1CON1bits.SAMP = 1;       // Start sampling
    tmr_wait_ms(TIMER1, 1);    // Sample for 1ms
    AD1CON1bits.SAMP = 0;       // Auto conversion starts

    while (!AD1CON1bits.DONE);  // Wait for conversion complete
    //float voltage = ADC1BUF0;         // Read ADC value
    
    unsigned int adc_val = ADC1BUF0; // Raw ADC value
    float voltage = (ADC1BUF0 / 1024.0) * 3.3;

    float v2 = voltage * voltage;
    float v3 = v2 * voltage;
    float v4 = v3 * voltage;

    distance = 2.34 - 4.74 * voltage + 4.06 * v2 - 1.60 * v3 + 0.24 * v4;
    distance *= 100; //distance in cm

    //sprintf(buffer, "ADC=%u, V=%.2fV\r\n", adc_val, voltage); //check voltage reading
    sprintf(buffer, "IR=%.2f cm\r\n", distance);
    
    IEC0bits.U1TXIE = 0;
    for (int i = 0; i < strlen(buffer); i++) {
        cb_push(&cb_tx, buffer[i]);
    }
    IEC0bits.U1TXIE = 1;

    tmr_wait_ms(TIMER1, 100);
    tmr_wait_ms(TIMER1, 200);
    tmr_wait_ms(TIMER1, 200);
}

void printValues(){
    sprintf(buffer, "$SENS,%.2f,%.2f*\r\n", distance, v_battery);
    
    IEC0bits.U1TXIE = 0;
    for (int i = 0; i < strlen(buffer); i++) {
        cb_push(&cb_tx, buffer[i]);
    }
    IEC0bits.U1TXIE = 1;
}

// ADC Interrupt - triggered after each scan completes
void __attribute__((interrupt, auto_psv)) _AD1Interrupt() {
    IFS0bits.AD1IF = 0; // Clear ADC interrupt flag
    
    // Read the ADC results (we have 2 samples: AN5 and AN11)
    unsigned int ir_adc_val = ADC1BUF0;     // IR sensor on AN5
    unsigned int battery_adc_val = ADC1BUF1; // Battery voltage on AN11
    
    // Process IR sensor reading
    float voltage = (ir_adc_val / 1024.0) * 3.3;
    float v2 = voltage * voltage;
    float v3 = v2 * voltage;
    float v4 = v3 * voltage;
    distance = 2.34 - 4.74 * voltage + 4.06 * v2 - 1.60 * v3 + 0.24 * v4;
    distance *= 100; // Convert to cm 
    
    // Process battery voltage reading
    float v_sense = (battery_adc_val / 1024.0) * 3.3;
    v_battery = v_sense * 3; // Voltage divider: BAT-VSENSE = Vbat / 3
    
    // Increment sample counter for 10 Hz transmission
    sample_count++;
    if (sample_count >= 100) { // 1000Hz / 100 = 10Hz
        sample_count = 0;
        
        // Send data in the required format: $SENS,x,y*
        sprintf(buffer, "$SENS,%.2f,%.2f*\r\n", distance, v_battery);
        
        // Disable TX interrupt while updating the buffer
        IEC0bits.U1TXIE = 0;
        for (int i = 0; i < strlen(buffer); i++) {
            cb_push(&cb_tx, buffer[i]);
        }
        IEC0bits.U1TXIE = 1; // Re-enable TX interrupt
    }
}

int main(void) {   
    adc_init3_v2();
    
    UART1_Init(); // initialize UART1
    
    cb_init(&cb_tx);
    
    tmr_setup_period(TIMER3, 1); // TIMER3 for 1kHz sampling

    while(1){
        
    }
    
    return 0;
}