#include <msp430.h>
#include "stonyman.h"

/*
 * Stonyman Commands
 */
//static const int PH = 2;			// Amplifier control (never used since it's off)
static const int IV = 3;			// Increment Value
static const int RV = 4;			// Reset value
static const int IP = 5;			// Increment Pointer
static const int RP = 6;			// Reset Pointer

/*
 * Local static functions
 */
static void setPointer(char ptr);
static void setValue(short val);
static void pulsePin(char pinToPulse);

/****************************
 * Public Functions
 ***************************/
/*
 * Settings determined experimentally
 * Assumes 5V setup with no binning
 */
void stonymanSetup(){
	// Configure Stonyman Pin Connections
	P1DIR |= BIT2 + BIT3 + BIT4 + BIT5 + BIT6;
	P1OUT &= ~(BIT2 + BIT3 + BIT4 + BIT5 + BIT6);

	// No binning
	setPointerValue(HSW, 0);
	setPointerValue(VSW, 0);

	// Bias values
	setPointerValue(AOBIAS, 60);
	setPointerValue(NBIAS, 60);

	// Some other thing to do
	setPointerValue(CONFIG,16);		// Disable amplifier
}

/*********************************************************************/
//  setPointerValue
//  Sets the pointer to a register and sets the value of that
//  register
/*********************************************************************/
void setPointerValue(char ptr, short val){
    setPointer(ptr);    //set pointer to register
    setValue(val);    //set value of that register
}

/*
 * Sets the pointer the pointer to a register
 * and increments the value of that register once
 */
void incPointerValue(char ptr){
    setPointer(ptr);    //set pointer to register
    pulsePin(IV);
}

/*********************************************************************/
//  incValue
//  Sets the pointer system register to the desired value.  Value is
//  not reset so the current value must be taken into account
/*********************************************************************/
void incCurrentValue(){
	pulsePin(IV);
}

/****************************
 * Private Functions
 ***************************/
/*********************************************************************/
//  setPointer
//  Sets the pointer system register to the desired value
/*********************************************************************/
static void setPointer(char ptr){
	// clear pointer
	pulsePin(RP); // macro
	short i;
	// increment to required pointer
	for (i=0; i<ptr; ++i)
		pulsePin(IP); // macro
}

/*********************************************************************/
//  setValue
//  Sets the value of the current register
/*********************************************************************/
static void setValue(short val){
  // clear value
  pulsePin(RV); // macro
  short i;
  // increment to required value
  for (i=0; i!=val; i++)
	  pulsePin(IV); // macro
}

/*********************************************************************/
//  pulsePin
//  pulses the pins to the Stonyman vision chip
/*********************************************************************/
static void pulsePin(char pinToPulse){
	if(pinToPulse == IV){
		P1OUT ^= BIT3;
		P1OUT ^= BIT3;
	}else
	if(pinToPulse == IP){
		P1OUT ^= BIT5;
		P1OUT ^= BIT5;
	}else
	if(pinToPulse == RV){
		P1OUT ^= BIT4;
		P1OUT ^= BIT4;
	}else
	if(pinToPulse == RP){
		P1OUT ^= BIT6;
		P1OUT ^= BIT6;
	}
}
