#include <msp430.h>
#include <stdint.h>
#include "uart.h"

/****************************
 * Extern Function Definitions
 ***************************/
void uartInit(){
	//Setup For UART
	/*
	 * SET UXRXD0 LOW and UXTXD0 HIGH
	 */
	UCA0CTL1 |= UCSSEL1;					// SMCLK
	UCA0BR0 = 138;							// 16MHz 115200 baud
	UCA0BR1 = 0;							// 16MHz 115200
	UCA0MCTL = UCBRS2 + UCBRS1 + UCBRS0;	// Modulation UCBRSx = 7
	P3SEL |= BIT4;							// P3.4 = USCI_A0 TXD
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

