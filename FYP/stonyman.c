#include <msp430.h>
#include <stdint.h>
#include "stonyman.h"

//Stonymann Chip control values
enum STONY_PINS {
	PH,	// Amplifier Control
	IV,	// Increment Value
	RV,	// Reset Value
	IP,	// Increment Pointer
	RP	// Reset Pointer
};

/****************************
 * Static Function Declarations
 ***************************/
static inline void setPointer(enum STONY_VALS ptr);
static inline void setValue(const uint8_t val);
static inline void setPointerValue(enum STONY_VALS ptr, const uint8_t val);
static inline void pulsePin(enum STONY_PINS pinToPulse);

/****************************
 * Extern Function Definitions
 ***************************/
/*
 * stonymanInit
 * Initialisation function for the stonyman camera
 */
extern void stonymanInit(){
	// Stonyman Pin Connections
	P1DIR |= BIT2 + BIT3 + BIT4 + BIT5 + BIT6;
	P1OUT &= ~(BIT2 + BIT3 + BIT4 + BIT5 + BIT6);

	/*
	 * Assert configuration settings in stonmany registers
	 * Stonyman Vision Chip setup determined via testing
	 * This was for 5V setup to allow biggest voltage swing
	 */
	setPointerValue(AOBIAS, 60);
	setPointerValue(NBIAS, 60);
	setBinning(NONE, NONE);
	setPointerValue(CONFIG,16);				// No amplifier
}

/*
 * setRow
 * Moves to the row number provided
 */
extern void setRow(uint8_t row){
	setPointerValue(ROWSEL, row);
}

/*
 * setCol
 * Moves to the column number provided
 */
extern void setCol(uint8_t col){
	setPointerValue(COLSEL, col);
}

/*
 * incrementRow
 * Points to the next row
 */
extern void incrementRow(){
	setPointer(ROWSEL);
	pulsePin(IV);
}

/*
 * incrementCol
 * Points to the next column
 */
extern void incrementCol(){
	setPointer(COLSEL);
	pulsePin(IV);
}

/*
 * incrementCurrent
 * Increments the value of the current register by val.
 */
extern void incrementCurrent(){
	pulsePin(IV);
}

/****************************
 * Static Function Definitions
 ***************************/

/*
 * setPointer
 * Sets the pointer system register to the desired value
 */
static inline void setPointer(enum STONY_VALS ptr){
	// clear pointer
	pulsePin(RP); // macro
	uint8_t iterator;
	// increment pointer to desired value
	for (iterator=ptr; iterator>0; iterator--)
		pulsePin(IP); // macro
}

/*
 * setValue
 * Sets the value of the current register
 */
static inline void setValue(const uint8_t val){
	// clear pointer
	pulsePin(RV); // macro
	uint8_t iterator;
	// increment pointer
	for (iterator=val; iterator>0; iterator--)
	  pulsePin(IV); // macro
}

/*
 * setPointerValue
 * Sets the pointer to a register and then sets the value of that register
 */
static inline void setPointerValue(enum STONY_VALS ptr, const uint8_t val){
    setPointer(ptr);    //set pointer to register
    setValue(val);    //set value of that register
}

/*
 * pulsePin
 * pulses the pins to the Stonyman vision chip
 */
static inline void pulsePin(enum STONY_PINS pinToPulse){
	if(pinToPulse == IV){
		P1OUT ^= BIT3;
		P1OUT ^= BIT3;
	}else if(pinToPulse == IP){
		P1OUT ^= BIT5;
		P1OUT ^= BIT5;
	}else if(pinToPulse == RP){
		P1OUT ^= BIT6;
		P1OUT ^= BIT6;
	}else if(pinToPulse == RV){
		P1OUT ^= BIT4;
		P1OUT ^= BIT4;
	}else if(pinToPulse == PH){
		P1OUT ^= BIT2;
		__delay_cycles(26); //1 microsecond delay
		P1OUT ^= BIT2;
	}
}

/*********************************************************************/
//  setBinning - DEPRECATED
//  Configures binning in the focal plane using the VSW and HSW
//  system registers. The super pixels are aligned with the top left
//  of the image, e.g. "offset downsampling" is not used. This
//  function is for the Stonyman chip only.
//  VARIABLES:
//  hbin: set to 1, 2, 4, or 8 to bin horizontally by that amount
//  vbin: set to 1, 2, 4, or 8 to bin vertically by that amount
/*********************************************************************/
void setBinning(enum PIXEL_SKIP rowSkip, enum PIXEL_SKIP colSkip){
	uint8_t hsw,vsw;

	// Horizontal binning
	switch (colSkip){
	case NONE:    //no binning
		hsw = 0x00;
		break;
	case TWO:     //downsample by 2
		hsw = 0x55;
		break;
	case FOUR:     //downsample by 4
		hsw = 0x77;
		break;
	case EIGHT:     //downsample by 8
		hsw = 0x7F;
		break;
	}

	// Vertical binning
	switch (rowSkip){
	case NONE:    //no binning
		vsw = 0x00;
		break;
	case TWO:     //downsample by 2
		vsw = 0xAA;
		break;
	case FOUR:     //downsample by 4
		vsw = 0x77;
		break;
	case EIGHT:     //downsample by 8
		vsw = 0x7F;
		break;
	}
	//set switching registers
	setPointerValue(HSW,hsw);
	setPointerValue(VSW,vsw);
}
