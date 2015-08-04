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

enum PIXEL_SKIP {
	NONE=1,
	TWO=2,
	FOUR=4,
	EIGHT=8
};

#define PIXEL_SETTLING_DELAY 24

/****************************
 * Extern Function Declarations
 ***************************/
inline void stonymanInit();
inline void setRow(uint8_t row);
inline void setCol(uint8_t col);
inline void incrementRow();
inline void incrementCol();
inline void incrementCurrent();
void setBinning(enum PIXEL_SKIP rowSkip, enum PIXEL_SKIP colSkip);

#endif /* STONYMAN_H_ */
