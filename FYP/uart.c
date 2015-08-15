#include "uart.h"

/****************************
 * Extern Function Definitions
 ***************************/
void uartInit(){
	UCA0CTL1 |= UCSSEL1;					// SMCLK
	UCA0BR0 = 138;							// 16MHz 115200 baud
	UCA0BR1 = 0;							// 16MHz 115200
	UCA0MCTL = UCBRS2 + UCBRS1 + UCBRS0;	// Modulation UCBRSx = 7
	P3SEL |= BIT4;							// P3.4 = USCI_A0 TXD
	UCA0CTL1 &= ~UCSWRST;					// **Initialize USCI state machine**
}

/*
 * Sends a single byte out through UART
 */
extern void sendByte(uint8_t byte){
	while (!(IFG2&UCA0TXIFG));			// Buffer ready?
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

extern void sendFloat(float f){
	int16_t integer = f;					// Integer component
	while (!(IFG2&UCA0TXIFG));
	UCA0TXBUF = integer>>8;			// Transmit the 8 MSB
	while (!(IFG2&UCA0TXIFG));
	UCA0TXBUF = integer;			// Transmit the 8 LSBs
	f -= integer;
	integer = (int) (f * 10000.0f);		// Fractional Component (4 digits)
	while (!(IFG2&UCA0TXIFG));
	UCA0TXBUF = integer>>8;			// Transmit the 8 MSB
	while (!(IFG2&UCA0TXIFG));
	UCA0TXBUF = integer;			// Transmit the 8 LSBs
}
