#include <main.h>

#define ADS1115_ADDR  0x48
#define ADS1115_REG_CONVERSION  0x00
#define ADS1115_REG_CONFIG      0x01

void I2C2_Init(void);
void I2C_Write(uint8_t deviceAddr, uint8_t regAddr, uint16_t data);
uint16_t I2C_Read(uint8_t deviceAddr, uint8_t regAddr);
void ADS1115_Config(void);
uint16_t ADS1115_ReadADC(void);
float ADS1115_ReadVoltage(void);
float Calculate_Rx(float Vcc, float Vdiff);

void GPIO_Config(void);

float voltage, res;


int main(void) {
	SystemInit();
	GPIO_Config();
  I2C2_Init();
	float vcc = 5;
  while (1) {
		//GPIOA->BSRR = GPIO_BSRR_BS15;
		uint16_t status;
		I2C_Write(ADS1115_ADDR, ADS1115_REG_CONFIG, 0xE363);
		do {
			status = I2C_Read(ADS1115_ADDR, ADS1115_REG_CONFIG);
		} while ((status & 0x8000) == 0);
		int16_t val = I2C_Read(ADS1115_ADDR, ADS1115_REG_CONVERSION);
		voltage = (val * 4.096) / 32768.0;
		res = ((5 - voltage) * 100000) / voltage;
		for (volatile int i = 0; i < 500000; i++);
	}
}

void I2C2_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;

    // B?t clock cho GPIOB và I2C2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);

    // C?u hình PB10 (SCL) và PB11 (SDA) là Alternate Function Open-Drain
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // C?u hình I2C2
    I2C_DeInit(I2C2);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 100000; // 100kHz
    I2C_Init(I2C2, &I2C_InitStructure);

    // B?t I2C2
    I2C_Cmd(I2C2, ENABLE);
}

void I2C_Write(uint8_t deviceAddr, uint8_t regAddr, uint16_t data) {
    while (I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY));

    I2C_GenerateSTART(I2C2, ENABLE);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C2, deviceAddr << 1, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C2, regAddr);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_SendData(I2C2, (data >> 8) & 0xFF);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_SendData(I2C2, data & 0xFF);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_GenerateSTOP(I2C2, ENABLE);
}

uint16_t I2C_Read(uint8_t deviceAddr, uint8_t regAddr) {
    uint16_t data = 0;

    while (I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY));

    I2C_GenerateSTART(I2C2, ENABLE);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C2, deviceAddr << 1, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C2, regAddr);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_GenerateSTART(I2C2, ENABLE);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C2, deviceAddr << 1, I2C_Direction_Receiver);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    I2C_AcknowledgeConfig(I2C2, ENABLE);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED));
    data = I2C_ReceiveData(I2C2) << 8;

    I2C_AcknowledgeConfig(I2C2, DISABLE);
    I2C_GenerateSTOP(I2C2, ENABLE);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED));
    data |= I2C_ReceiveData(I2C2);

    return data;
}

void ADS1115_Config(void) {
		uint16_t status;
    I2C_Write(ADS1115_ADDR, ADS1115_REG_CONFIG, 0x8503); 
		do {
    status = I2C_Read(ADS1115_ADDR, ADS1115_REG_CONFIG);
	} while ((status & 0x8000) == 0);  // Bit OS == 0 ? chua xong
}

uint16_t ADS1115_ReadADC(void) {
	
		ADS1115_Config();
    return I2C_Read(ADS1115_ADDR, ADS1115_REG_CONVERSION);
}

float ADS1115_ReadVoltage(void) {
    int16_t adc_value = (int16_t)ADS1115_ReadADC();
    float voltage = (adc_value * 2.048) / 32768.0;
    return voltage;
}

float Calculate_Rx(float Vcc, float Vdiff) {
    float Rx = Vdiff * 10000; // 1mA
    return Rx;
}
//output 
void GPIO_Config(void) {
    
	// Enable clock GPIOA and GPIOB
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN;
	
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;  // Enable clock AFIO
	AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_1;   // Disable JTAG, USING SWD
  
	//Config PA8, PA11, PA12, PA15
  GPIOA->CRH &= ~(GPIO_CRH_MODE8 | GPIO_CRH_CNF8);
  GPIOA->CRH |= (GPIO_CRH_MODE8_1 | GPIO_CRH_MODE8_0);
  GPIOA->ODR &= ~GPIO_ODR_ODR8;

	GPIOA->CRH &= ~(GPIO_CRH_MODE11 | GPIO_CRH_CNF11);
  GPIOA->CRH |= (GPIO_CRH_MODE11_1 | GPIO_CRH_MODE11_0);
  GPIOA->ODR &= ~GPIO_ODR_ODR11;
	
	GPIOA->CRH &= ~(GPIO_CRH_MODE12 | GPIO_CRH_CNF12);
  GPIOA->CRH |= (GPIO_CRH_MODE12_1 | GPIO_CRH_MODE12_0);
  GPIOA->ODR &= ~GPIO_ODR_ODR12;
	
	GPIOA->CRH &= ~(GPIO_CRH_MODE15 | GPIO_CRH_CNF15);
  GPIOA->CRH |= (GPIO_CRH_MODE15_1 | GPIO_CRH_MODE15_0);
  GPIOA->ODR &= ~GPIO_ODR_ODR15;
	
	//Config PB1, PB3
	GPIOB->CRL &= ~(GPIO_CRL_MODE1 | GPIO_CRL_CNF1);
  GPIOB->CRL |= (GPIO_CRL_MODE1_1 | GPIO_CRL_MODE1_0);
  GPIOB->ODR &= ~GPIO_ODR_ODR1;
		
	GPIOB->CRL &= ~(GPIO_CRL_MODE3 | GPIO_CRL_CNF3);
  GPIOB->CRL |= (GPIO_CRL_MODE3_1 | GPIO_CRL_MODE3_0);
  GPIOB->ODR &= ~GPIO_ODR_ODR3;
}
