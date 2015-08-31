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
******************************************************************************************************************************************/
#include "main.h"

#define DEBUG			// Enables UART communication

int main(void){
	initialise();

	struct Pixel brightestPixel, prevPixel = {
			.brightness = 0, .row = 55, .col = 55
	};
	const struct SystemConfig systems[2] = {
			[0]={ .rowStart = 21, .rowTotal = 90, .colStart = 50, .colTotal = 11,
					.baseLength = 0.06, .alpha = 1.343904, .beta = M_PI }
	};

	uint8_t rowCount, colCount, imgCount, currSystem;	// Current pixel parameters
	int16_t brightness;									// The pixel brightness
	int16_t images[2][90][11];							// Stores the images
	float height, pitch = 0, roll = 0;					// Orientation of sensor
	float r1, r2, r3;									// Rotation matrix vals
	float numer, denom;									// Calculation buffers

	/* Calculation Constants */
	const float X = F*cosf(systems[0].beta);
	const float Ta = tanf(systems[0].alpha);
	const float Cb = cosf(systems[0].beta);
	const float Sb = sinf(systems[0].beta);

	/*
	 * TODO: Implement math algorithm (in matlab first)
	 */
	while (1){
		currSystem = 0;	// TODO: Delete when adding more systems
		laserOff();	// Capture first image w/o laser and second image w/.
		for (imgCount=0; imgCount<IMG_TOTAL; imgCount++){
			setRow(systems[currSystem].rowStart);	// Set stonyman row ptr
			for (rowCount=0; rowCount<systems[currSystem].rowTotal; rowCount++){
				setCol(systems[currSystem].colStart);	// Set stonyman col ptr
				for (colCount=0; colCount<systems[currSystem].colTotal; colCount++){
					toggleAmp();							// Amplifier must be pulsed before sampling
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
		for (rowCount=0; rowCount<systems[currSystem].rowTotal; rowCount++){
			for (colCount=0; colCount<systems[currSystem].colTotal; colCount++){
				// Get the postive difference of the two pixels
				brightness = abs(images[0][rowCount][colCount] - images[1][rowCount][colCount]);
				images[0][rowCount][colCount] = brightness;
				// See if the current pixel is brighter
				if (brightness > brightestPixel.brightness){
					brightestPixel.brightness = brightness;
					brightestPixel.row = rowCount + systems[currSystem].rowStart;
					brightestPixel.col = colCount + systems[currSystem].colStart;
				}
			}
		}
		prevPixel = brightestPixel;
		// TODO: source pitch & roll from GINA
		// Determine the height from the brightest pixel
		r1 = -sinf(pitch);
		r2 = cosf(pitch)*sinf(roll);
		r3 = cosf(pitch)*cosf(roll);
		numer = r3*Ta - (r1*Cb + r2*Sb);
		numer *= X*systems[currSystem].baseLength;
		denom = (Oc - brightestPixel.row)*Ta + X;
		height = numer / denom;
#ifdef DEBUG
		/*
		 * Transmit number representing start of image. Since this number
		 * could be the remainder of the division we send it twice, consecutively.
		 */
		sendByte(254);
		sendByte(254);
		// Transmit the row and column dimensions for this image
		sendByte(systems[currSystem].rowTotal);
		sendByte(systems[currSystem].colTotal);
		// Transmit the row and column offsets for this image
		sendByte(systems[currSystem].rowStart);
		sendByte(systems[currSystem].colStart);
		// Transmit the image
		for (rowCount=0; rowCount<systems[currSystem].rowTotal; rowCount++){
			for (colCount=0; colCount<systems[currSystem].colTotal; colCount++){
				sendInt(images[0][rowCount][colCount]);
			}
		}
		// Transmit the brightest pixel
		sendByte(brightestPixel.row);
		sendByte(brightestPixel.col);
		sendInt(brightestPixel.brightness);
		sendFloat(height);
#endif
		ledToggle();
		currSystem = 1-currSystem;
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
