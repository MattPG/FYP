/*
 * adc10.c
 *
 *  Created on: 3 Aug 2015
 *      Author: matthew
 */
#include <msp430.h>
#include <stdint.h>
#include "uart.h"

void adc10Init(){
	P6SEL |= BIT0;							// Enable A/D channel A0
	ADC12CTL0 = ADC12ON + SHT0_2 + REFON; 	// Turn on, Internal Vref+ = 1.5V, Sample for 16 ADC12OSC cycles.
	ADC12CTL1 = SHP;						// Pulse Mode activated by ADC12SC
	ADC12MCTL0 = SREF_1;					// Use Vr+ = Vref and Vr- = AVss
	ADC12IE = 0x01;                         // Enable ADC12IFG0
	ADC12CTL0 |= ENC;						// Enable Converter

	// Vref settling delay
	volatile uint16_t i;
	for ( i=0; i<0x3600; i++);                // Delay for reference start-up
}


