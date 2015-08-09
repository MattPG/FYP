#include <msp430.h>

int _system_pre_init(void){
	// Disable WDT
	WDTCTL = WDTPW + WDTHOLD;

	// Initialise all ports
	P1DIR |= 0xFF;
	P2DIR |= 0xFF;
	P3DIR |= 0xFF;
	P4DIR |= 0xFF;
	P5DIR |= 0xFF;
	P6DIR |= 0xFF;
	P7DIR |= 0xFF;
	P8DIR |= 0xFF;
	P1OUT |= 0x00;
	P2OUT |= 0x00;
	P3OUT |= 0x00;
	P4OUT |= 0x00;
	P5OUT |= 0x00;
	P6OUT |= 0x00;
	P7OUT |= 0x00;
	P8OUT |= 0x00;

	return 1;
}
