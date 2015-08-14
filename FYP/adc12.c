#include "adc12.h"

volatile int16_t adcResult;

void adc12Init(){
	P6SEL |= BIT0;							// Enable A/D channel A0
	ADC12CTL0 = ADC12ON + SHT0_2 + REFON; 	// Turn on, Internal Vref+ = 1.5V, Sample for 16 ADC12OSC cycles.
	ADC12CTL1 = SHP;						// Pulse Mode activated by ADC12SC
	ADC12MCTL0 = SREF_1;					// Use Vr+ = Vref and Vr- = AVss
	ADC12IE = 0x01;                         // Enable ADC12IFG0
	ADC12CTL0 |= ENC;						// Enable Converter
}

/*
 * ADC12 ISR
 */
#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void){
	adcResult = ADC12MEM0;					// Read Converted Value, IFG is Cleared
	__bic_SR_register_on_exit(LPM0_bits); 	// Clear LPM0
}
