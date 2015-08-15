#ifndef UART_H_
#define UART_H_

#include <msp430.h>
#include <stdint.h>

/****************************
 * Extern Function Declarations
 ***************************/
void uartInit();
inline void sendByte(uint8_t byte);
inline void sendInt(uint16_t integer);
inline void sendFloat(float val);

#endif /* UART_H_ */
