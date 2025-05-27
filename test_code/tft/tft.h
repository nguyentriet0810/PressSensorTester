#ifndef __TFT__
#define __TFT__

#include <stm32f10x.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_spi.h>

#include "stdlib.h"
#include "string.h"

#include "fonts.h"

// Ð?nh nghia chân GPIO và macro t? các do?n tru?c
#define ILI9341_RST_LOW()  GPIO_ResetBits(GPIOA, GPIO_Pin_2)
#define ILI9341_RST_HIGH() GPIO_SetBits(GPIOA, GPIO_Pin_2)
#define ILI9341_CS_LOW()   GPIO_ResetBits(GPIOA, GPIO_Pin_1)
#define ILI9341_CS_HIGH()  GPIO_SetBits(GPIOA, GPIO_Pin_1)
#define ILI9341_DC_LOW()   GPIO_ResetBits(GPIOA, GPIO_Pin_3)
#define ILI9341_DC_HIGH()  GPIO_SetBits(GPIOA, GPIO_Pin_3)

// Ð?nh nghia màu s?c
#define BLACK       0x0000
#define NAVY        0x000F
#define DARKGREEN   0x03E0
#define DARKCYAN    0x03EF
#define MAROON      0x7800
#define PURPLE      0x780F
#define OLIVE       0x7BE0
#define LIGHTGREY   0xC618
#define DARKGREY    0x7BEF
#define BLUE        0x001F
#define GREEN       0x07E0
#define CYAN        0x07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define YELLOW      0xFFE0
#define WHITE       0xFFFF
#define ORANGE      0xFD20
#define GREENYELLOW 0xAFE5
#define PINK        0xF81F

#define HORIZONTAL_IMAGE 0
#define VERTICAL_IMAGE   1
#define SCREEN_VERTICAL_1  0
#define SCREEN_HORIZONTAL_1 1
#define SCREEN_VERTICAL_2  2
#define SCREEN_HORIZONTAL_2 3

void Timer3_Init(void);
void Timer3_Delay_us(uint16_t us);
void Timer3_Delay_ms(uint16_t ms);


void Spi1_Init(void);
uint8_t Spi1_SendByte(uint8_t data);
void Spi1_SendBuffer(uint8_t *buffer, uint16_t length);
uint8_t Spi1_ReceiveByte(void);
void Spi1_ReceiveBuffer(uint8_t *buffer, uint16_t length);

void ILI9341_SPI_Tx(uint8_t data);
void ILI9341_SPI_TxBuffer(uint8_t *buffer, uint16_t len);

void ILI9341_WriteCommand(uint8_t cmd);
void ILI9341_WriteData(uint8_t data);
void ILI9341_WriteBuffer(uint8_t *buffer, uint16_t len);

void ILI9341_Reset(void);
void ILI9341_Enable(void);
void ILI9341_Init(void);
void ILI9341_SetRotation(uint8_t rotation);
void ILI9341_SetAddress(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void ILI9341_DrawColor(uint16_t color);
void ILI9341_DrawColorBurst(uint16_t color, uint32_t size);
void ILI9341_FillScreen(uint16_t color);
void ILI9341_DrawPixel(uint16_t x,uint16_t y,uint16_t color);
void ILI9341_DrawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void ILI9341_DrawHLine(uint16_t x, uint16_t y, uint16_t width, uint16_t color);
void ILI9341_DrawVLine(uint16_t x, uint16_t y, uint16_t height, uint16_t color);

#define HORIZONTAL_IMAGE	0
#define VERTICAL_IMAGE		1

void ILI9341_DrawHollowCircle(uint16_t X, uint16_t Y, uint16_t radius, uint16_t color);
void ILI9341_DrawFilledCircle(uint16_t X, uint16_t Y, uint16_t radius, uint16_t color);
void ILI9341_DrawHollowRectangleCoord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t color);
void ILI9341_DrawFilledRectangleCoord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t color);
void ILI9341_DrawChar(char ch, const uint8_t font[], uint16_t X, uint16_t Y, uint16_t color, uint16_t bgcolor);
void ILI9341_DrawText(const char* str, const uint8_t font[], uint16_t X, uint16_t Y, uint16_t color, uint16_t bgcolor);
void ILI9341_DrawImage(const uint8_t* image, uint8_t orientation);

#endif /*__TFT__*/
