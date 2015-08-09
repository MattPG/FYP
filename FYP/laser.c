#include <msp430.h>
#include <stdint.h>
#include "laser.h"

#define LASER_PORT P5OUT
#define LED_PORT P1OUT
#define LASER_BIT BIT5
#define LED_BIT BIT0
void laserInit(){
	P5DIR |= LASER_BIT;					// P5.5 output mode
	LASER_PORT &= ~LASER_BIT;			// Assert laser low

	P1DIR |= LED_BIT;						// Set LED pin (P1.0) as output
	P1OUT &= ~LED_BIT;						// Assert LED pin low
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

extern void ledToggle(){
	LED_PORT ^= LED_BIT;
}
