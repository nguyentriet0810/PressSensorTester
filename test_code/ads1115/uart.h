#ifndef __UART_H__
#define __UART_H__

#include <stm32f10x.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>

void USART3_Init(void);
void USART3_SendChar(char c);
void USART3_SendString(const char *str);
void UART_printf(const char *format, ...);
void USART3_SendBuffer(uint8_t *data, uint16_t len);

#endif
