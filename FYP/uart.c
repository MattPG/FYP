#include <msp430.h>
#include <stdint.h>
#include "uart.h"

/****************************
 * Extern Function Definitions
 ***************************/
void uartInit(){
	//Setup For UART
	DCOCTL = 0;								// Select lowest DCOx and MODx settings
	BCSCTL1 = CALBC1_16MHZ;					// Set DCO
	DCOCTL = CALDCO_16MHZ;
	P3SEL = 0x30;							// P3.4,5 = USCI_A0 TXD/RXD
	UCA0CTL1 |= UCSSEL_2;					// SMCLK
	UCA0BR0 = 139;							// 16MHz 115200 baud
	UCA0BR1 = 0;							// 16MHz 115200
	UCA0MCTL = UCBRS2 + UCBRS0;				// Modulation UCBRSx = 5
	UCA0CTL1 &= ~UCSWRST;					// **Initialize USCI state machine**
}

/**
 * Sends a single byte out through UART
 **/
extern void sendByte(uint8_t byte){
	while (!(IFG2&UCA0TXIFG));			// USCI_A0 TX buffer ready?
	UCA0TXBUF = byte;					// TX -> RXed character
}

/*
 * Sends an integer
 */
extern void sendInt(uint16_t integer){
	while (!(IFG2&UCA0TXIFG));
	UCA0TXBUF = integer>>8;			// Transmit the 8 MSB
	while (!(IFG2&UCA0TXIFG));
	UCA0TXBUF = integer;			// Transmit the 8 LSBs
}

/*
 * Sends an integer
 */
extern void sendInts(uint16_t *ints, uint8_t total){
	uint16_t integer;
	uint8_t i;
	for(i=total; i>0; i--){
		integer = *ints++;
		while (!(IFG2&UCA0TXIFG));
		UCA0TXBUF = integer>>8;		// Transmit the 8 MSB
		while (!(IFG2&UCA0TXIFG));
		UCA0TXBUF = integer;			// Transmit the 8 LSBs
	}
}
