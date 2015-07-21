//******************************************************************************************************************************************//
// This is Micah Bearlin-Allardice's source code for the Stonyman Vision chip for MSP430F2618 it was programmed using Code Composer studio V5.4.0.00091
//  M. Bearlin-Allardice
//  FYP-Height Esstimation
//  November 2014
// If you would like to contact me for help or understanding email me at: micah.allardice@gmail.com
// This code is for the microc-controller (uC) MSP430F2618 it controls the stonyman vision chip taking x number of images of resolution x by x
// and then transfers the data via UART to matlab
// The code was designed to store each pixel value in an array however the uC's RAM can not store all of the values due to size. The current code
// stores the values in an array of length MAX_PIXELS. This was used for debugging purposes to view the pixel values during testing.
//
// When the ADC is read I normally received values between 1500-3500. Values of 0, 2048, 4095 can all indicate some kind of error so be wary
//
// Loading code with large arrays on an MSP board cause the watchdog timer to create an error. Therefore to use this code it must be disabled
// Put the pre_init.c file in the project file containg the camera code - Hoam Chung Should have this file. If he doesn't the code is
//****************************************************************
//#include <msp430.h>
//
//int _system_pre_init(void)
//{
//	WDTCTL = WDTPW + WDTHOLD;    /* disable watchdog timer  */
//	return 1;
//}
//****************************************************************
//
// MATT: Large global arrays can be declared with the "__no_init" prefix to prevent WDT triggering before main() is reached.
//
// To understand this code you should first definately read the Stonyman Vision Chip data sheet
// There is also sample code that this was designed off at http://www.ardueye.com/pmwiki.php?n=Main.StonymanLens
// And some information on the forums at http://embeddedeye.com/
// Also read the MSP data sheet and User guide if you dont understand how they work
// (Final development of the code should just bin all but the bright values and store the row and column numbers)
//******************************************************************************************************************************************//

#include <msp430.h>
#include <stdint.h>
#include "stonyman.h"

#define DEBUG

static void initialise();

static uint_fast16_t rawPixel;

int main(void){
	initialise();

	static const uint_fast8_t rowStart = 1;
	static const uint_fast8_t colStart = 1;
	static const uint_fast8_t numRows = 110;
	static const uint_fast8_t numCols = 110;
	static uint_fast8_t rowCount, colCount;

	while (1){
#ifdef DEBUG
		/*
		 * Transmit number representing start of image. Since this number
		 * could be the remainder of the division we send it twice, consecutively.
		 */
		UCA0TXBUF = 254;
		__delay_cycles(16);
		while (!(IFG2&UCA0TXIFG));
		UCA0TXBUF = 254;
		__delay_cycles(16);
		while (!(IFG2&UCA0TXIFG));
#endif
		// Go to first row
		setPointerValue(ROWSEL,rowStart);
		// Loop through all rows
		for (rowCount=numRows; rowCount>0; rowCount--) {
			// Go to first column
			setPointerValue(COLSEL,colStart);
			// Loop through all columns
			for (colCount=numCols; colCount>0; colCount--) {
				// Settling delay for pixel reading
				__delay_cycles(16);

				ADC12CTL0 |= ADC12SC;				// Start Conversion
				__bis_SR_register(LPM0_bits + GIE);	// Enter LPM0, Enable interrupts
#ifdef DEBUG
				UCA0TXBUF = rawPixel/20;		// Transmit the first section of the pixel to matlab
				__delay_cycles(16);
				while (!(IFG2&UCA0TXIFG));
				UCA0TXBUF = rawPixel%20;		// Transmit the second section of the pixel to matlab
				__delay_cycles(16);
				while (!(IFG2&UCA0TXIFG));
#endif
				incValue(); // Move to next pixel in this row
			}
			// Go to next row
			setPointer(ROWSEL);
			incValue();
		}
	  P1OUT ^= BIT0;
	}
}

static void initialise(){
	DCOCTL = 0;								// Select lowest DCOx and MODx settings
	BCSCTL1 = CALBC1_16MHZ;					// Set range
	DCOCTL = CALDCO_16MHZ;					// Set DCO step + modulation*/

	WDTCTL = WDTPW + WDTHOLD;				// Stop the watchdog timer because were going to sit in a loop

	// Enable LED
	P1DIR |= BIT0;
	P1OUT &= ~BIT0;

	// Stonyman Pin Connections
	P1DIR |= BIT2 + BIT3 + BIT4 + BIT5 + BIT6;
	P1OUT &= ~(BIT2 + BIT3 + BIT4 + BIT5 + BIT6);

	//Setting Up Pins for ADC conversion
	P6SEL |= BIT0;							// Enable A/D channel A0
	ADC12CTL0 &= ~ENC;						// Disable Converter (allows to modify registers)
	ADC12CTL0 = ADC12ON + SHT0_2 + REFON; 	// Turn on, Internal Vref+ = 1.5V, Sample for 16 ADC12OSC cycles.
	ADC12CTL1 = SHP;						// Pulse Mode activated by ADC12SC
	ADC12MCTL0 = SREF_1;					// Use Vr+ = Vref and Vr- = AVss
	ADC12IE = 0x01;                         // Enable ADC12IFG0
	ADC12CTL0 |= ENC;						// Re-Enable Converter

#ifdef DEBUG
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
#endif

	//Stonyman Vision Chip setup determined via testing
	//This was for 5V setup to allow biggest voltage swing
	setPointerValue(AOBIAS, 60);
	setPointerValue(NBIAS, 60);
	setPointerValue(HSW, 0);				// No horz. binning
	setPointerValue(VSW, 0);				// No vert. binning
	setPointerValue(CONFIG,16);				// No amplifier
}

/*********************************************************************/
//  Interrupt service routine
//  the was not used in the final code but was during testing
/*********************************************************************/
#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void)
{
	rawPixel = ADC12MEM0;			// Read Converted Value, IFG is Cleared
	__bic_SR_register_on_exit(LPM0_bits); // Clear LPM0
}


