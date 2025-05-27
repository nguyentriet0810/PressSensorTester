#include "lcd_tft.h"

volatile uint32_t sysTickCounter = 0;

#define CS_LOW()    (GPIOA->BRR = GPIO_BRR_BR1)
#define CS_HIGH()   (GPIOA->BSRR = GPIO_BSRR_BS1)
#define RESET_LOW() (GPIOA->BRR = GPIO_BRR_BR2)
#define RESET_HIGH() (GPIOA->BSRR = GPIO_BSRR_BS2)
#define DC_LOW()    (GPIOA->BRR = GPIO_BRR_BR3)
#define DC_HIGH()   (GPIOA->BSRR = GPIO_BSRR_BS3)

void RCC_Init(void) {
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY));
    FLASH->ACR |= FLASH_ACR_PRFTBE;
    FLASH->ACR |= FLASH_ACR_LATENCY_2;
    RCC->CFGR |= RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9;
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE1_DIV2 | RCC_CFGR_PPRE2_DIV1;
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

void SysTick_Init(void) {
    SysTick->LOAD = 72000 - 1;
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Handler(void) {
    if (sysTickCounter > 0) sysTickCounter--;
}

void Delay_ms(uint32_t ms) {
    sysTickCounter = ms;
    while (sysTickCounter != 0);
}

void SPI1_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN | RCC_APB2ENR_IOPAEN;
    GPIOA->CRL &= ~(GPIO_CRL_CNF5 | GPIO_CRL_CNF7 | GPIO_CRL_MODE5 | GPIO_CRL_MODE7);
    GPIOA->CRL |= (GPIO_CRL_CNF5_1 | GPIO_CRL_MODE5_1 | GPIO_CRL_MODE5_0);
    GPIOA->CRL |= (GPIO_CRL_CNF7_1 | GPIO_CRL_MODE7_1 | GPIO_CRL_MODE7_0);
    GPIOA->CRL &= ~(GPIO_CRL_CNF6 | GPIO_CRL_MODE6);
    GPIOA->CRL |= GPIO_CRL_CNF6_1;
    GPIOA->ODR |= GPIO_ODR_ODR6;
    GPIOA->CRL &= ~(GPIO_CRL_CNF1 | GPIO_CRL_CNF2 | GPIO_CRL_CNF3 |
                    GPIO_CRL_MODE1 | GPIO_CRL_MODE2 | GPIO_CRL_MODE3);
    GPIOA->CRL |= (GPIO_CRL_MODE1_1 | GPIO_CRL_MODE1_0);
    GPIOA->CRL |= (GPIO_CRL_MODE2_1 | GPIO_CRL_MODE2_0);
    GPIOA->CRL |= (GPIO_CRL_MODE3_1 | GPIO_CRL_MODE3_0);
    CS_HIGH();
    RESET_HIGH();
    DC_HIGH();
    SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_BR_2; // 4.5MHz
    SPI1->CR1 |= SPI_CR1_CPOL | SPI_CR1_CPHA;
    SPI1->CR1 |= SPI_CR1_SPE;
}

void SPI1_Write(uint8_t data) {
    while (!(SPI1->SR & SPI_SR_TXE));
    SPI1->DR = data;
    while (SPI1->SR & SPI_SR_BSY);
}

void TFT_SendCommand(uint8_t cmd) {
    DC_LOW();
    CS_LOW();
    SPI1_Write(cmd);
    CS_HIGH();
}

void TFT_SendData(uint16_t data) {
    DC_HIGH();
    CS_LOW();
    SPI1_Write(data >> 8);
    SPI1_Write(data & 0xFF);
    CS_HIGH();
}

void TFT_Reset(void) {
    RESET_HIGH();
    Delay_ms(5);
    RESET_LOW();
    Delay_ms(10);
    RESET_HIGH();
    Delay_ms(120);
}

void TFT_Init(void) {
    TFT_Reset();
    TFT_SendCommand(0x01); Delay_ms(50);
    TFT_SendCommand(0x11); Delay_ms(120);
    TFT_SendCommand(0x3A); TFT_SendData(0x55); Delay_ms(10);
    TFT_SendCommand(0x36); TFT_SendData(0x48); Delay_ms(10);
    TFT_SendCommand(0x29); Delay_ms(50);
}

void TFT_SetBackgroundColor(uint16_t color) {
    TFT_SendCommand(0x2A); TFT_SendData(0x00); TFT_SendData(0x00); TFT_SendData(0x00); TFT_SendData(0xEF); Delay_ms(5);
    TFT_SendCommand(0x2B); TFT_SendData(0x00); TFT_SendData(0x00); TFT_SendData(0x01); TFT_SendData(0x3F); Delay_ms(5);
    TFT_SendCommand(0x2C);
    DC_HIGH();
    CS_LOW();
    for (uint32_t i = 0; i < 76800; i++) {
        while (!(SPI1->SR & SPI_SR_TXE));
        SPI1->DR = color >> 8;
        while (!(SPI1->SR & SPI_SR_TXE));
        SPI1->DR = color & 0xFF;
    }
    while (SPI1->SR & SPI_SR_BSY);
    CS_HIGH();
}

int main(void) {
    RCC_Init();
    SysTick_Init();
    SPI1_Init();
    TFT_Init();
		TFT_Reset();
    TFT_SetBackgroundColor(0xF800);  // Ð?
    while (1) {}
}
