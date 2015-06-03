#include <msp430.h>
#include "stonyman.h"

//Stonymann Chip control values
//static const int PH = 2;			//Amplifier control (Unused)
static const int IV = 3;			//Increment Value
static const int RV = 4;			//Reset value
static const int IP = 5;			//Increment Pointer
static const int RP = 6;			//Reset Pointer


/****************************
 * Static Function Declarations
 ***************************/
static void pulsePin(const short pinToPulse);
static void setValue(const short val);

/****************************
 * Extern Function Definitions
 ***************************/
/*
 * setPointerValue
 * Sets the pointer to a register and then sets the value of that register
 */
void setPointerValue(const short ptr, const short val){
    setPointer(ptr);    //set pointer to register
    setValue(val);    //set value of that register
}

/*
 * setPointer
 * Sets the pointer system register to the desired value
 */
void setPointer(const short ptr){
	// clear pointer
	pulsePin(RP); // macro
	short iterator;
	// increment pointer to desired value
	for (iterator=ptr; iterator>0; iterator--)
		pulsePin(IP); // macro
}

/*
 * incValue
 * Increments the value of the current register by val.
 */
void incValue(){
	pulsePin(IV);
}

/****************************
 * Static Function Definitions
 ***************************/
/*
 * setValue
 * Sets the value of the current register
 */
static void setValue(const short val){
	// clear pointer
	pulsePin(RV); // macro
	short iterator;
	// increment pointer
	for (iterator=val; iterator>0; iterator--)
	  pulsePin(IV); // macro
}

/*********************************************************************/
//  pulsePin
//  pulses the pins to the Stonyman vision chip
/*********************************************************************/
static void pulsePin(const short pinToPulse){
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
	}/*else if(pinToPulse == PH){
		P1OUT ^= BIT2;
		__delay_cycles(26); //1 microsecond delay
		P1OUT ^= BIT2;
	}*/
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
void setBinning(const short hbin, const short vbin){
   short hsw,vsw;

   switch (hbin) //horizontal binning
   {
    case 2:     //downsample by 2
      hsw = 0xAA;
      break;
    case 4:     //downsample by 4
      hsw = 0xEE;
      break;
    case 8:     //downsample by 8
      hsw = 0xFE;
      break;
    default:    //no binning
      hsw = 0x00;
   }

   switch (vbin)    //vertical binning
   {
    case 2:     //downsample by 2
      vsw = 0xAA;
      break;
    case 4:     //downsample by 4
      vsw = 0xEE;
      break;
    case 8:     //downsample by 8
      vsw = 0xFE;
      break;
    default:    //no binning
      vsw = 0x00;
    }

	//set switching registers
	setPointerValue(HSW,hsw);
	setPointerValue(VSW,vsw);
}
