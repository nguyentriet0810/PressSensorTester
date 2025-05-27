#ifndef __SPI1__
#define __SPI1__

#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_spi.h>

void Spi1_Init(void);
uint8_t Spi1_SendByte(uint8_t data);
void Spi1_SendBuffer(uint8_t *buffer, uint16_t length);
uint8_t Spi1_ReceiveByte(void);
void Spi1_ReceiveBuffer(uint8_t *buffer, uint16_t length);

#endif /*__SPI1__*/
