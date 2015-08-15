/******************************************************************************************************************************************
FYP-Height Estimation
Source code for the Stonyman Vision chip (B/W Camera) & MSP430F2618 uC.
Programmed using Code Composer studio V6.1.0.00104

M. Bearlin-Allardice	November 2014	micah.allardice@gmail.com
M. Greenwood			2015			matt.p.greenwood@gmail.com

The code repeatedly takes two images of size TOTAL_ROW*TOTAL_COL.
The laser is off in one image and on in the next.
The two images are subtracted from eachother to determine and changes in light intesnsity.
The resulting image is iterated to find the brightest pixel (assumed to be position of laser).
If DEBUG is enabled the dimensions and contents of image and laser position are sent via UART.
This can be interfaced with a UART->USB dongle and read in via Matlab for debugging.

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

extern int16_t volatile const adcResult;	// Defined in adc12.c

static void initialise();

int main(void){
	initialise();

	int16_t images[IMG_TOTAL][ROW_TOTAL][COL_TOTAL];	// Stores the images
	int16_t brightness;									// The pixel brightness
	uint8_t rowCount, colCount, imgCount;				// Current pixel parameters
	struct Pixel{
		uint8_t row;
		uint8_t col;
		int16_t brightness;
	} brightestPixel;

	/*
	 * TODO: Implement math algorithm (in matlab first)
	 */
	while (1){
		laserOff();	// Capture first image w/o laser and second image w/.
		for (imgCount=0; imgCount<IMG_TOTAL; imgCount++){
			setRow(ROW_START);	// Set stonyman row ptr
			for (rowCount=0; rowCount<ROW_TOTAL; rowCount++){
				setCol(COL_START);	// Set stonyman col ptr
				for (colCount=0; colCount<COL_TOTAL; colCount++){
					__delay_cycles(PIXEL_SETTLING_DELAY);	// Settling delay for pixel reading
					ADC12CTL0 |= ADC12SC;					// Start Conversion
					__bis_SR_register(LPM0_bits + GIE);		// Enter LPM0, Enable interrupts
					images[imgCount][rowCount][colCount] = adcResult;
					incrementCurrent();						// Increment stonyman col ptr
				}	// End column loop
				incrementRow();	// Increment stonyman row ptr
			}	// End row loop
			laserToggle();
		} // End image loop

		// Subtract second from first and store in first image
		brightestPixel.brightness = INT16_MIN;	// Track the brightest pixel
		for (rowCount=0; rowCount<ROW_TOTAL; rowCount++){
			for (colCount=0; colCount<COL_TOTAL; colCount++){
				// Get the postive difference of the two pixels
				brightness = abs(images[0][rowCount][colCount] - images[1][rowCount][colCount]);
				images[0][rowCount][colCount] = brightness;
				// See if the current pixel is brighter
				if (brightness > brightestPixel.brightness){
					brightestPixel.brightness = brightness;
					brightestPixel.row = rowCount + ROW_START;
					brightestPixel.col = colCount + COL_START;
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
		for (rowCount=0; rowCount<ROW_TOTAL; rowCount++){
			for (colCount=0; colCount<COL_TOTAL; colCount++){
				sendInt(images[0][rowCount][colCount]);
			}
		}
		// Transmit the brightest pixel
		sendByte(brightestPixel.row);
		sendByte(brightestPixel.col);
		sendInt(brightestPixel.brightness);
#endif
		ledToggle();
	}	// End infinite while loop
}	// End main

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
