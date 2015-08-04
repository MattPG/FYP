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
/*
 * Must have optmisation level 3 (-o3) enabled!
 */
#include <msp430.h>
#include <stdint.h>

#include "adc12.h"
#include "stonyman.h"
#include "uart.h"

#define DEBUG

#define START_ROW 21	// Row to start reading pixels from
#define TOTAL_ROW 90	// Total number of rows to read
#define START_COL 50	// Column to start reading pixels from
#define TOTAL_COL 11	// Total number of columns to read

static void initialise();

static volatile uint16_t rawPixel;
static volatile uint16_t image[2][TOTAL_ROW][TOTAL_COL];
static uint8_t rowCount, colCount, imageCount; // Used in ADC12 ISR

int main(void){
	initialise();

	while (1){
		imageCount = 0;
		// Go to first row
		setRow(START_ROW);
		// Loop through all rows
		for (rowCount=0; rowCount<TOTAL_ROW; rowCount++) {
			// Go to first column
			setCol(START_COL);
			// Loop through all columns
			for (colCount=0; colCount<TOTAL_COL; colCount++) {
				// Settling delay for pixel reading
				__delay_cycles(PIXEL_SETTLING_DELAY);

				ADC12CTL0 |= ADC12SC;				// Start Conversion
				__bis_SR_register(LPM0_bits + GIE);	// Enter LPM0, Enable interrupts

				incrementCurrent(); // Move to next column
			}	// End column loop
			// Go to next row
			incrementRow();
		}	// End row loop

		imageCount=1;
		// Go to first row
		setRow(START_ROW);
		// Loop through all rows
		for (rowCount=0; rowCount<TOTAL_ROW; rowCount++) {
			// Go to first column
			setCol(START_COL);
			// Loop through all columns
			for (colCount=0; colCount<TOTAL_COL; colCount++) {
				// Settling delay for pixel reading
				__delay_cycles(PIXEL_SETTLING_DELAY);

				ADC12CTL0 |= ADC12SC;				// Start Conversion
				__bis_SR_register(LPM0_bits + GIE);	// Enter LPM0, Enable interrupts

				incrementCurrent(); // Move to next column
			}	// End column loop
			// Go to next row
			incrementRow();
		}	// End row loop

#ifdef DEBUG
		uint8_t q;
		for(q=0; q<2; q++){
			/*
			 * Transmit number representing start of image. Since this number
			 * could be the remainder of the division we send it twice, consecutively.
			 */
			sendByte(254);
			sendByte(254);
			/*
			 * Transmit the row and column dimensions for this image
			 * Assumes 0<=row<=255 and 0<=col<=255
			 */
			sendByte(TOTAL_ROW);
			sendByte(TOTAL_COL);
			/*
			 * Transmit the row and column offsets for this image
			 * Assumes 0<=row<=255 and 0<=col<=255
			 */
			sendByte(START_ROW);
			sendByte(START_COL);
			/*
			 * Transmit the image
			 */
			uint8_t i,j;
			for(i=0;i<TOTAL_ROW;i++){
				for(j=0;j<TOTAL_COL;j++){
					sendInt(image[q][i][j]);
				}
			}
		}
#endif
	  P1OUT ^= BIT0; // Full image iterated
	}	// End infinite while loop
}

/*
 * ADC12 ISR
 */
#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void){
	image[imageCount][rowCount][colCount] = ADC12MEM0;			// Read Converted Value, IFG is Cleared
	__bic_SR_register_on_exit(LPM0_bits); // Clear LPM0
}

static void initialise(){
	WDTCTL = WDTPW + WDTHOLD;				// Stop the watchdog timer because were going to sit in a loop

	// Clock Module
	DCOCTL = CALDCO_16MHZ;					// Set DC0 to 16Mhz
	BCSCTL1 = CALBC1_16MHZ;					// Use MCLK = DC0

	// Enable LED
	P1DIR |= BIT0;
	P1OUT &= ~BIT0;

	adc10Init();
	stonymanInit();
	uartInit();
}
