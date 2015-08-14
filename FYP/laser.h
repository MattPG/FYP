#ifndef LASER_H_
#define LASER_H_

#include <msp430.h>

void laserInit();
inline void laserToggle();
inline void laserOn();
inline void laserOff();
inline void ledToggle();

#endif /* LASER_H_ */
