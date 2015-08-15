#ifndef MAIN_H_
#define MAIN_H_

#include <msp430.h>
#include <stdint.h>
#include <math.h>
#include "adc12.h"
#include "stonyman.h"
#include "uart.h"
#include "laser.h"

/* Image dimensions */
#define ROW_TOTAL 90	// Total number of rows to read
#define COL_TOTAL 11	// Total number of columns to read
#define IMG_TOTAL 2		// Number of images stored

/* Struct Declarations */
struct Pixel{
	uint8_t row;
	uint8_t col;
	int16_t brightness;
};
struct SystemConfig{
	uint8_t rowStart, rowTotal, colStart, colTotal;	// px
	float baseLength, alpha, beta;					// metres, radians
};

/* Calculation Parameters */
#define F 168.0f
#define Oc 55
#ifndef M_PI
#define M_PI 3.1415926535
#endif

/* Global Variables */
extern int16_t volatile adcResult;	// Defined in adc12.c

/* Static Functions */
static void initialise();

#endif /* MAIN_H_ */
