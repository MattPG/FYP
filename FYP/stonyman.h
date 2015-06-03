#ifndef STONYMAN_H_
#define STONYMAN_H_

/*
 * Stonyman Pointers/Registers
 */
#define COLSEL 0    //select column
#define ROWSEL 1    //select row
#define VSW 2       //vertical switching
#define HSW 3       //horizontal switching
#define CONFIG 5    //configuration register
#define VREF 4      //voltage reference
#define NBIAS 6     //nbias
#define AOBIAS 7    //analog out bias

/*
 * Non-static function declarations
 */
void setPointerValue(char ptr, short val);
void incPointerValue(char ptr);
void incCurrentValue();
void stonymanSetup();

#endif /* STONYMAN_H_ */
