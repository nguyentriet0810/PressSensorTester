#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_exti.h"

#define ADS1256_CS_LOW()   GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define ADS1256_CS_HIGH()  GPIO_SetBits(GPIOA, GPIO_Pin_4)
#define ADS1256_DRDY()     GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0)

#define ADS1256_CMD_WAKEUP  0x00
#define ADS1256_CMD_RDATA   0x01
#define ADS1256_CMD_RDATAC  0x03
#define ADS1256_CMD_SDATAC  0x0F
#define ADS1256_CMD_RREG    0x10
#define ADS1256_CMD_WREG    0x50
#define ADS1256_CMD_SELFCAL 0xF0
#define ADS1256_CMD_SELFOCAL 0xF1
#define ADS1256_CMD_SELFGCAL 0xF2
#define ADS1256_CMD_SYSOCAL 0xF3
#define ADS1256_CMD_SYSGCAL 0xF4
#define ADS1256_CMD_SYNC    0xFC
#define ADS1256_CMD_STANDBY 0xFD
#define ADS1256_CMD_RESET   0xFE

#define ADS1256_MUX_AIN0    0x08
#define ADS1256_GAIN_1      0x00
#define ADS1256_DR_10SPS    0x03
#define ADS1256_DR_100SPS   0x0A
#define ADS1256_VREF        2.5f

void SPI1_Init(void) {
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_Init(SPI1, &SPI_InitStructure);
    
    SPI_Cmd(SPI1, ENABLE);
}

uint8_t SPI1_Transmit(uint8_t data) {
    while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
    SPI_I2S_SendData(SPI1, data);
    while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
    return SPI_I2S_ReceiveData(SPI1);
}

void ADS1256_WriteReg(uint8_t reg, uint8_t value) {
    ADS1256_CS_LOW();
    SPI1_Transmit(ADS1256_CMD_WREG | (reg & 0x0F));
    SPI1_Transmit(0x00);
    SPI1_Transmit(value);
    ADS1256_CS_HIGH();
}

uint8_t ADS1256_ReadReg(uint8_t reg) {
    uint8_t value;
    ADS1256_CS_LOW();
    SPI1_Transmit(ADS1256_CMD_RREG | (reg & 0x0F));
    SPI1_Transmit(0x00);
    value = SPI1_Transmit(0xFF);
    ADS1256_CS_HIGH();
    return value;
}

void ADS1256_Reset(void) {
    ADS1256_CS_LOW();
    SPI1_Transmit(ADS1256_CMD_RESET);
    for (volatile uint32_t i = 0; i < 50000; i++);
    ADS1256_CS_HIGH();
}

void ADS1256_Init(void) {
    ADS1256_Reset();
    ADS1256_WriteReg(0x00, 0x08); // AINP = AIN0, AINN = GND
    ADS1256_WriteReg(0x02, ADS1256_GAIN_1);
    ADS1256_WriteReg(0x03, ADS1256_DR_10SPS);
    SPI1_Transmit(ADS1256_CMD_SYNC);
    SPI1_Transmit(ADS1256_CMD_WAKEUP);
}

int32_t ADS1256_ReadData(void) {
    int32_t result;
    while (ADS1256_DRDY());
    ADS1256_CS_LOW();
    SPI1_Transmit(ADS1256_CMD_RDATA);
    for (volatile uint32_t i = 0; i < 10; i++);
    result = SPI1_Transmit(0xFF);
    result = (result << 8) | SPI1_Transmit(0xFF);
    result = (result << 8) | SPI1_Transmit(0xFF);
    if (result & 0x800000) result |= 0xFF000000;
    ADS1256_CS_HIGH();
    return result;
}

float ADS1256_GetVoltage(void) {
    int32_t adc_value = ADS1256_ReadData();
    return ((float)adc_value / 8388607.0f) * ADS1256_VREF;
}
float voltage ;
int main(void) {
    SPI1_Init();
    ADS1256_Init();
    while (1) {
        voltage = ADS1256_GetVoltage();
    }
}
