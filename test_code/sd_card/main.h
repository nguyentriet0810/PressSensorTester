#ifndef __MAIN_H__
#define __MAIN_H__

#include <stm32f10x.h>
#include <stdint.h>

#include "sd_card.h"
#define SD_CS_LOW()    (GPIOB->BSRR = GPIO_BSRR_BR12)  // CS = 0, ch?n th? SD
#define SD_CS_HIGH()   (GPIOB->BSRR = GPIO_BSRR_BS12)  // CS = 1, b? ch?n th? SD

void SPI2_Init(void);
void SPI2_GPIO_Init(void);
uint8_t SPI2_Transmit(uint8_t data);
void SD_SendDummyClock(void);
void RCC_Init(void);
int main(void);

#endif /*__MAIN_H__*/
