#include <msp430.h>
#include <stdint.h>
#include "laser.h"

#define LASER_PORT P5OUT
#define LASER_BIT BIT5

void laserInit(){
	P5DIR |= LASER_BIT;					// P5.5 output mode
	LASER_PORT &= ~LASER_BIT;			// Assert laser low
}

extern void laserToggle(){
	LASER_PORT ^= LASER_BIT;
}

extern void laserOn(){
	LASER_PORT |= LASER_BIT;
}

extern void laserOff(){
	LASER_PORT &= ~LASER_BIT;
}
