#include <msp430.h>
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




