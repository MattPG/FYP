#include <msp430.h>
#include "stonyman.h"

static void takeImage();

/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    // TODO: UNDERSTAND THIS
	DCOCTL = 0;                 // Select lowest DCOx and MODx settings
	BCSCTL1 = CALBC1_16MHZ;     // Set range
	DCOCTL = CALDCO_16MHZ;      // Set DCO step + modulation*/
	
	// Enable LED
	P1DIR |= BIT0;
	P1OUT &= ~BIT0;

	// Configure ADC
	P6SEL |= BIT0;                          // Enable A/D channel A0
	ADC12CTL0 = ADC12ON + SHT0_2 + REFON; 	// Turn on, Internal Vref+ = 1.5V, Sample for 16 ADC12OSC cycles.
	ADC12CTL1 = SHP;                        // Pulse Mode activated by ADC12SC
	ADC12MCTL0 = SREF_1;                    // Use Vr+ = Vref and Vr- = AVss

	// Configure UART
	DCOCTL = 0;                            	// Select lowest DCOx and MODx settings
	BCSCTL1 = CALBC1_16MHZ;                 // Set DCO
	DCOCTL = CALDCO_16MHZ;
	P3SEL = 0x30;                           // P3.4,5 = USCI_A0 TXD/RXD
	UCA0CTL1 |= UCSSEL_2;                   // SMCLK
	UCA0BR0 = 139;                          // 16MHz 115200 baud
	UCA0BR1 = 0;                            // 16MHz 115200
	UCA0MCTL = UCBRS2 + UCBRS0;             // Modulation UCBRSx = 5
	UCA0CTL1 &= ~UCSWRST;                   // **Initialize USCI state machine**

	// Configure Stonyman Vision Chip
	stonymanSetup();

	while(1){
		  takeImage();	//Run take image
		  P1OUT ^= BIT0;
	}
}

static void takeImage(){
	const int rowStart = 1;
	const int colStart = 1;
	const int numRows = 110;
	const int numCols = 110;

	unsigned int rawPixel;

	/*
	 * Transmit number representing start of image. Since this number
	 * could be the remainder of the division we send it twice.
	 */
	UCA0TXBUF = 254;
	__delay_cycles(16);
	while (!(IFG2&UCA0TXIFG));
	UCA0TXBUF = 254;
	__delay_cycles(16);
	while (!(IFG2&UCA0TXIFG));

	//  unsigned char chigh,clow;
	unsigned char row, col;

	// Go to first row
	setPointerValue(ROWSEL, rowStart);
	__delay_cycles(16);

	// Loop through all rows
	for (row=0; row<numRows; ++row) {

		// Go to first column
		setPointerValue(COLSEL, colStart);

		// Loop through all columns
		for (col=0; col<numCols; ++col) {

		  // settling delay for pixel reading
			__delay_cycles(16);

			// Decode the current pixel
			ADC12CTL0 |= ENC + ADC12SC;	// Enable ADC and Start Conversion
			while ((ADC12IFG & BIT0)==0); // Wait until conversion is stored in ADC12MEM0
			_NOP();
			rawPixel = ADC12MEM0;			//ADC value to array

			// Send pixel over UART
			UCA0TXBUF = rawPixel/20;		//Transmit the first section of the pixel to matlab
			__delay_cycles(16);
			while (!(IFG2&UCA0TXIFG));
			UCA0TXBUF = rawPixel%20;		//Transmit the second section of the pixel to matlab
			__delay_cycles(16);
			while (!(IFG2&UCA0TXIFG))

			// Go to next pixel in this row
			incCurrentValue();
		}
	// Go to next row
	incPointerValue(ROWSEL);
	}

	__delay_cycles(16);
}

/*********************************************************************/
//  Interrupt service routine
//  the was not used in the final code but was during testing
/*********************************************************************/
#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void){

}
