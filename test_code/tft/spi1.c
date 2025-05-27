#include "spi1.h"

void Spi1_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    SPI_InitTypeDef SPI_InitStruct;
    
    // 1. Enable Clock cho SPI1 và GPIOA
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA, ENABLE);
    
    // 2. C?u hình chân SPI1 (SCK, MISO, MOSI)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;  // Ch? d? Alternate Function Push-Pull
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // 3. C?u hình các chân CS, RESET, D/C là OUTPUT
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;  // Ch? d? Output Push-Pull
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // 4. C?u hình SPI1
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;  // Ði?u khi?n CS b?ng ph?n m?m
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; // 72MHz / 8 = 9MHz
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStruct);
    
    // 5. Kích ho?t SPI1
    SPI_Cmd(SPI1, ENABLE);
    
    // 6. Thi?t l?p tr?ng thái m?c d?nh cho GPIO di?u khi?n
    GPIO_SetBits(GPIOA, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3); // CS, RESET, D/C = HIGH
}
uint8_t Spi1_SendByte(uint8_t data) {
    // Ð?i TXE (TX buffer empty) d? d?m b?o có th? g?i d? li?u
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    
    // G?i d? li?u qua SPI
    SPI_I2S_SendData(SPI1, data);
    
    // Ð?i RXNE (RX buffer not empty) d? d?m b?o d? li?u dã du?c nh?n
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    
    // Ð?c và tr? v? d? li?u nh?n du?c t? SPI
    return SPI_I2S_ReceiveData(SPI1);
}
void Spi1_SendBuffer(uint8_t *buffer, uint16_t length) {
    for (uint16_t i = 0; i < length; i++) {
        Spi1_SendByte(buffer[i]);
    }
}
uint8_t Spi1_ReceiveByte(void) {
    return Spi1_SendByte(0xFF); // G?i 0xFF d? nh?n d? li?u
}
void Spi1_ReceiveBuffer(uint8_t *buffer, uint16_t length) {
    for (uint16_t i = 0; i < length; i++) {
        buffer[i] = Spi1_ReceiveByte();
    }
}
