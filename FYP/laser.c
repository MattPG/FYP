#include "laser.h"

#define LASER_POUT P5OUT
#define LASER_PDIR P5DIR
#define LASER_BIT BIT5
#define LED_POUT P1OUT
#define LED_PDIR P1DIR
#define LED_BIT BIT0

void laserInit(){
	LASER_PDIR |= LASER_BIT;			// P5.5 output mode
	LASER_POUT &= ~LASER_BIT;			// Assert laser low

	LED_PDIR |= LED_BIT;				// Set LED pin (P1.0) as output
	LED_POUT &= ~LED_BIT;				// Assert LED pin low
}

extern void laserToggle(){
	LASER_POUT ^= LASER_BIT;
}

extern void laserOn(){
	LASER_POUT |= LASER_BIT;
}

extern void laserOff(){
	LASER_POUT &= ~LASER_BIT;
}

extern void ledToggle(){
	LED_POUT ^= LED_BIT;
}
