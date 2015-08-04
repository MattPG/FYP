/*
 * uart.h
 *
 *  Created on: 3 Aug 2015
 *      Author: matthew
 */

#ifndef UART_H_
#define UART_H_


/****************************
 * Extern Function Declarations
 ***************************/
void uartInit();
inline void sendByte(uint8_t byte);
inline void sendInt(uint16_t integer);
inline void sendInts(uint16_t *ints, uint8_t total);

#endif /* UART_H_ */
