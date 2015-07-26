#ifndef STONYMAN_H_
#define STONYMAN_H_


/****************************
 * Stonyman Pointers/Registers
 ***************************/
enum STONY_VALS {
	COLSEL=0,
	ROWSEL,
	VSW,
	HSW,
	VREF,
	CONFIG,
	NBIAS,
	AOBIAS
};

/****************************
 * Extern Function Declarations
 ***************************/
void setPointerValue(enum STONY_VALS ptr, const uint8_t val);
void setPointer(enum STONY_VALS ptr);
void incValue();

#endif /* STONYMAN_H_ */
