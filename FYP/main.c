/******************************************************************************************************************************************
FYP-Height Estimation
Source code for the Stonyman Vision chip & MSP430F2618 uC.
Programmed using Code Composer studio (old) V5.4.0.00091 (new) V6.1.0.00104

M. Bearlin-Allardice	November 2014	micah.allardice@gmail.com
M. Greenwood			2015			matt.p.greenwood@gmail.com

The code repeatedly takes an TOTAL_ROW*TOTAL_COL image and sends the dimensions and contents via UART (if is DEBUG defined).
This was interfaced with a Serial (uart) to USB dongle and read in via Matlab for debugging.

The code assumes that 4*TOTAL_ROW*TOTAL_COL < ~7kb. This is required since there is only 8kb of RAM.
There are two images stored and each pixel is two bytes.

Recommended Resources:
 * MSP430x2xx User Guide
 * MSP430F2618 Data Sheet
 * Stonyman Vision Chip data sheet
 * http://embeddedeye.com/ forum for Stonyman

Ideal to have optmisation level 3 (-o3) enabled for inlining.
******************************************************************************************************************************************/
#include <msp430.h>
#include <stdint.h>
#include <math.h>
#include "adc12.h"
#include "stonyman.h"
#include "uart.h"
#include "laser.h"

#define DEBUG

#define START_ROW 21	// Row to start reading pixels from
#define TOTAL_ROW 90	// Total number of rows to read
#define START_COL 50	// Column to start reading pixels from
#define TOTAL_COL 11	// Total number of columns to read

static void initialise();

static volatile int16_t image[2][TOTAL_ROW][TOTAL_COL];	// Stores the images
static uint8_t rowCount, colCount, imageCount;				// Used in ADC12 ISR

int main(void){
	initialise();
	/*
	 * TODO: Implement math algorithm (in matlab first)
	 */
	while(1){
		// Capture first image w/ laser
		laserOn();
		imageCount = 0;
		// Go to first row
		setRow(START_ROW);
		// Loop through all rows
		for(rowCount=0; rowCount<TOTAL_ROW; rowCount++){
			// Go to first column
			setCol(START_COL);
			// Loop through all columns
			for(colCount=0; colCount<TOTAL_COL; colCount++){
				// Settling delay for pixel reading
				__delay_cycles(PIXEL_SETTLING_DELAY);

				ADC12CTL0 |= ADC12SC;				// Start Conversion
				__bis_SR_register(LPM0_bits + GIE);	// Enter LPM0, Enable interrupts

				incrementCurrent(); // Move to next column
			}	// End column loop
			// Go to next row
			incrementRow();
		}	// End row loop
		// Capture second image w/o laser
		laserOff();
		imageCount=1;
		// Go to first row
		setRow(START_ROW);
		// Loop through all rows
		for(rowCount=0; rowCount<TOTAL_ROW; rowCount++){
			// Go to first column
			setCol(START_COL);
			// Loop through all columns
			for(colCount=0; colCount<TOTAL_COL; colCount++){
				// Settling delay for pixel reading
				__delay_cycles(PIXEL_SETTLING_DELAY);

				ADC12CTL0 |= ADC12SC;				// Start Conversion
				__bis_SR_register(LPM0_bits + GIE);	// Enter LPM0, Enable interrupts

				incrementCurrent(); // Move to next column
			}	// End column loop
			// Go to next row
			incrementRow();
		}	// End row loop

		// Subtract second from first and store in first image
		uint8_t currRow, currCol;
		// Loop through all rows
		for(currRow=0; currRow<TOTAL_ROW; currRow++){
			// Loop through all columns
			for(currCol=0; currCol<TOTAL_COL; currCol++){
				// Get the postive difference of the two pixels
				image[0][currRow][currCol] =
						abs(image[0][currRow][currCol]-image[1][currRow][currCol]);
			}	// End column loop
		}	// End row loop


#ifdef DEBUG
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
				sendInt(image[0][i][j]);
			}
		}
#endif
	  ledToggle(); // Full image iterated
	}	// End infinite while loop
}

/*
 * ADC12 ISR
 */
#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void){
	image[imageCount][rowCount][colCount] = ADC12MEM0;	// Read Converted Value, IFG is Cleared
	__bic_SR_register_on_exit(LPM0_bits); 				// Clear LPM0
}

static void initialise(){
	// Clock Module
	DCOCTL = CALDCO_16MHZ;					// Set DC0 to 16Mhz
	BCSCTL1 = CALBC1_16MHZ;					// Use MCLK = DC0

	laserInit();
	adc12Init();
	stonymanInit();
#ifdef DEBUG
	uartInit();
#endif
}
