/******************************************************************************************************************************************
FYP-Height Estimation
Source code for the Stonyman Vision chip & MSP430F2618 uC.
Programmed using Code Composer studio (old) V5.4.0.00091 (new) V6.1.0.00104

M. Bearlin-Allardice	November 2014	micah.allardice@gmail.com
M. Greenwood			2015			matt.p.greenwood@gmail.com

The code repeatedly takes an image of size TOTAL_ROW*TOTAL_COL and sends the dimensions and contents via UART (if DEBUG is defined).
This was interfaced with a UART->USB dongle and read in via Matlab for debugging.

The code assumes that 4*TOTAL_ROW*TOTAL_COL < ~7kb. This is required since there is only 8kb of RAM.
There are two images stored and each pixel is two bytes.

Recommended Resources:
 * MSP430x2xx User Guide
 * MSP430F2618 Data Sheet
 * Stonyman Vision Chip data sheet
 * http://embeddedeye.com/ forum for Stonyman

Ideal to have optmisation level 3 (-o3) and speed level 5.
******************************************************************************************************************************************/
#include "main.h"

#define DEBUG			// Enables UART communication

#define ROW_START 21	// Row to start reading pixels from
#define ROW_TOTAL 90	// Total number of rows to read
#define COL_START 50	// Column to start reading pixels from
#define COL_TOTAL 11	// Total number of columns to read
#define IMG_TOTAL 2		// Number of images stored

static struct Pixel{
	uint8_t row;
	uint8_t col;
	int16_t brightness;
} brightestPixel;

static void initialise();

static volatile int16_t images[IMG_TOTAL][ROW_TOTAL][COL_TOTAL];	// Stores the images
static uint8_t rowCount, colCount, imageCount;						// Current pixel parameters

int main(void){
	initialise();
	uint8_t currRow, currCol;	// Faster than using global iterators
	int16_t brightness;		// The pixel brightness
	/*
	 * TODO: Implement math algorithm (in matlab first)
	 */
	while(1){
		// Capture first image w/ laser
		laserOn();
		imageCount = 0;
		// Go to first row
		setRow(ROW_START);
		// Loop through all rows
		for(rowCount=0; rowCount<ROW_TOTAL; rowCount++){
			// Go to first column
			setCol(COL_START);
			// Loop through all columns
			for(colCount=0; colCount<COL_TOTAL; colCount++){
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
		imageCount = 1;
		// Go to first row
		setRow(ROW_START);
		// Loop through all rows
		for(rowCount=0; rowCount<ROW_TOTAL; rowCount++){
			// Go to first column
			setCol(COL_START);
			// Loop through all columns
			for(colCount=0; colCount<COL_TOTAL; colCount++){
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
		brightestPixel.brightness = INT16_MIN;	// Track the brightest pixel
		for(currRow=0; currRow<ROW_TOTAL; currRow++){
			for(currCol=0; currCol<COL_TOTAL; currCol++){
				// Get the postive difference of the two pixels
				brightness = abs(images[0][currRow][currCol] - images[1][currRow][currCol]);
				images[0][currRow][currCol] = brightness;

				// See if the current pixel is brighter
				if(brightness > brightestPixel.brightness){
					brightestPixel.brightness = brightness;
					brightestPixel.row = currRow + ROW_START;
					brightestPixel.col = currCol + COL_START;
				}
			}
		}

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
		sendByte(ROW_TOTAL);
		sendByte(COL_TOTAL);
		/*
		 * Transmit the row and column offsets for this image
		 * Assumes 0<=row<=255 and 0<=col<=255
		 */
		sendByte(ROW_START);
		sendByte(COL_START);
		// Transmit the image
		for(currRow=0; currRow<ROW_TOTAL; currRow++){
			for(currCol=0; currCol<COL_TOTAL; currCol++){
				sendInt(images[0][currRow][currCol]);
			}
		}
		// Transmit the brightest pixel
		sendByte(brightestPixel.row);
		sendByte(brightestPixel.col);
		sendInt(brightestPixel.brightness);
#endif
	  ledToggle(); // Full image iterated
	}	// End infinite while loop
}

/*
 * ADC12 ISR
 */
#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void){
	images[imageCount][rowCount][colCount] = ADC12MEM0;	// Read Converted Value, IFG is Cleared
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
