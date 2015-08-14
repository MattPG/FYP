#include <msp430.h>
#include <stdint.h>
#include "uart.h"

void adc12Init(){
	P6SEL |= BIT0;							// Enable A/D channel A0
	ADC12CTL0 = ADC12ON + SHT0_2 + REFON; 	// Turn on, Internal Vref+ = 1.5V, Sample for 16 ADC12OSC cycles.
	ADC12CTL1 = SHP;						// Pulse Mode activated by ADC12SC
	ADC12MCTL0 = SREF_1;					// Use Vr+ = Vref and Vr- = AVss
	ADC12IE = 0x01;                         // Enable ADC12IFG0
	ADC12CTL0 |= ENC;						// Enable Converter
}


