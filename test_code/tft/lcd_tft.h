#ifndef __LCDTFT_H__
#define __LCDTFT_H__

#include <stm32f10x.h>
#include <stdint.h>

#define CS_LOW()    (GPIOA->BRR = GPIO_BRR_BR1)
#define CS_HIGH()   (GPIOA->BSRR = GPIO_BSRR_BS1)
#define RESET_LOW() (GPIOA->BRR = GPIO_BRR_BR2)
#define RESET_HIGH() (GPIOA->BSRR = GPIO_BSRR_BS2)
#define DC_LOW()    (GPIOA->BRR = GPIO_BRR_BR3)
#define DC_HIGH()   (GPIOA->BSRR = GPIO_BSRR_BS3)

// Ð?nh nghia các hu?ng xoay
typedef enum {
    ROTATION_0   = 0,  // Portrait
    ROTATION_90  = 1,  // Landscape
    ROTATION_180 = 2,  // Portrait d?o ngu?c
    ROTATION_270 = 3   // Landscape d?o ngu?c
} TFT_Rotation;

void SysTick_Init(void);
void SysTick_Handler(void);
void Delay_ms(uint32_t ms);
void SPI1_Init(void);
void SPI1_Write(uint8_t data);
void TFT_SendCommand(uint8_t cmd);
void TFT_SendData(uint16_t data);
void TFT_Reset(void);
void TFT_Init(void);
void TFT_SetRotation(TFT_Rotation rotation);
void TFT_SetBackgroundColor(uint16_t color);
#endif /*__LCDTFT_H__*/
