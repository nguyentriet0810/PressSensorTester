/*
MIT License

Copyright (c) 2025 Nguyen Hoang Minh Triet

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

-----------------------------------------------------------------------
Author : Nguyen Hoang Minh Triet 
Email  : 23trietminh23@gmail.com  
GitHub : https://github.com/nguyentriet0810  
YouTube: https://www.youtube.com/@hoangtriet9999
-----------------------------------------------------------------------
*/

#include "vl6180x.h"

void I2C1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef  I2C_InitStructure;

    // 1. B?t clock cho GPIOB, AFIO và I2C1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    // 2. Set m?c cao cho PB6 và PB7 d? tránh I2C b? treo
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  // Open-drain output
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_6 | GPIO_Pin_7);  // Ð?t m?c cao

    // 3. Reset I2C1 d? d?m b?o không b? treo
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);

    // 4. C?u hình l?i PB6 và PB7 ? ch? d? Alternate Function Open-Drain
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 5. C?u hình I2C1
    I2C_InitStructure.I2C_ClockSpeed = 100000;  // 100kHz
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

    I2C_Init(I2C1, &I2C_InitStructure);

    // 6. B?t I2C1
    I2C_Cmd(I2C1, ENABLE);
}
void VL6180X_WriteByte(uint16_t reg, uint8_t data)
{
    // Ch? n?u bus dang b?n
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET);

    I2C_GenerateSTART(I2C1, ENABLE);
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == RESET);

    I2C_Send7bitAddress(I2C1, VL6180X_ADDR, I2C_Direction_Transmitter);
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == RESET);

    I2C_SendData(I2C1, (reg >> 8) & 0xFF); // MSB c?a d?a ch? thanh ghi
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == RESET);

    I2C_SendData(I2C1, reg & 0xFF); // LSB
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == RESET);

    I2C_SendData(I2C1, data); // D? li?u
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == RESET);

    I2C_GenerateSTOP(I2C1, ENABLE);
}
uint8_t VL6180X_ReadByte(uint16_t reg)
{
    uint8_t received_data = 0;

    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET);

    I2C_GenerateSTART(I2C1, ENABLE);
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == RESET);

    I2C_Send7bitAddress(I2C1, VL6180X_ADDR, I2C_Direction_Transmitter);
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == RESET);

    I2C_SendData(I2C1, (reg >> 8) & 0xFF); // MSB
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == RESET);

    I2C_SendData(I2C1, reg & 0xFF); // LSB
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == RESET);

    I2C_GenerateSTART(I2C1, ENABLE); // repeated START
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == RESET);

    I2C_Send7bitAddress(I2C1, VL6180X_ADDR, I2C_Direction_Receiver);
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == RESET);

    I2C_AcknowledgeConfig(I2C1, DISABLE); // Không ACK vì ch? d?c 1 byte

    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) == RESET);
    received_data = I2C_ReceiveData(I2C1);

    I2C_GenerateSTOP(I2C1, ENABLE);

    I2C_AcknowledgeConfig(I2C1, ENABLE); // B?t l?i ACK cho l?n d?c sau

    return received_data;
}
int VL6180X_Init(void) {
    char reset;
    reset = VL6180X_ReadByte(0x016);
		if (reset==1){      
		// check to see has it be Initialised already
		///////////////////////////////////////////////////////////////////
		// Added latest settings here - see Section 8 
		// Mandatory : private registers
		VL6180X_WriteByte(0x0207, 0x01);
		VL6180X_WriteByte(0x0208, 0x01);
		VL6180X_WriteByte(0x0096, 0x00);
		VL6180X_WriteByte(0x0097, 0xfd);
		VL6180X_WriteByte(0x00e3, 0x00);
		VL6180X_WriteByte(0x00e4, 0x04);
		VL6180X_WriteByte(0x00e5, 0x02);
		VL6180X_WriteByte(0x00e6, 0x01);
		VL6180X_WriteByte(0x00e7, 0x03);
		VL6180X_WriteByte(0x00f5, 0x02);
		VL6180X_WriteByte(0x00d9, 0x05);
		VL6180X_WriteByte(0x00db, 0xce);
		VL6180X_WriteByte(0x00dc, 0x03);
		VL6180X_WriteByte(0x00dd, 0xf8);
		VL6180X_WriteByte(0x009f, 0x00);
		VL6180X_WriteByte(0x00a3, 0x3c);
		VL6180X_WriteByte(0x00b7, 0x00);
		VL6180X_WriteByte(0x00bb, 0x3c);
		VL6180X_WriteByte(0x00b2, 0x09);
		VL6180X_WriteByte(0x00ca, 0x09);
		VL6180X_WriteByte(0x0198, 0x01);
		VL6180X_WriteByte(0x01b0, 0x17);
		VL6180X_WriteByte(0x01ad, 0x00);
		VL6180X_WriteByte(0x00ff, 0x05);
		VL6180X_WriteByte(0x0100, 0x05);
		VL6180X_WriteByte(0x0199, 0x05);
		VL6180X_WriteByte(0x01a6, 0x1b);
		VL6180X_WriteByte(0x01ac, 0x3e);
		VL6180X_WriteByte(0x01a7, 0x1f);
		VL6180X_WriteByte(0x0030, 0x00);
		// Recommended : Public registers - See data sheet for more detail
		VL6180X_WriteByte(0x0011, 0x10); // Enables polling for ‘New Sample ready’ 
		// when measurement completes
		VL6180X_WriteByte(0x010a, 0x30); // Set the averaging sample period
		// (compromise between lower noise and 
		// increased execution time)
		VL6180X_WriteByte(0x003f, 0x46); // Sets the light and dark gain (upper 
		// nibble). Dark gain should not be 
		// changed.
		VL6180X_WriteByte(0x0031, 0xFF); // sets the # of range measurements after 
		// which auto calibration of system is
		// performed 
		VL6180X_WriteByte(0x0040, 0x63); // Set ALS integration time to 100ms
		VL6180X_WriteByte(0x002e, 0x01); // perform a single temperature calibration
		// of the ranging sensor 
		//Optional: Public registers - See data sheet for more detail
		VL6180X_WriteByte(0x001b, 0x09); // Set default ranging inter-measurement 
		// period to 100ms
		VL6180X_WriteByte(0x003e, 0x31); // Set default ALS inter-measurement period 
		// to 500ms
		VL6180X_WriteByte(0x0014, 0x24); // Configures interrupt on ‘New Sample 
		// Ready threshold event’
		///////////////////////////////////////////////////////////////////
		VL6180X_WriteByte(0x016, 0x00); //change fresh out of set status to 0
	}
	return 0;
}
 
int VL6180X_Start_Range(void)
{
    VL6180X_WriteByte(0x0018, 0x01);  // SYSRANGE__START = 0x01
    return 0;
}
int VL6180X_Poll_Range(void)
{
    uint8_t status = 0;
    uint8_t range_status = 0;

    do {
        status = VL6180X_ReadByte(0x004F);  // RESULT__INTERRUPT_STATUS_GPIO
        range_status = status & 0x07;
        for (volatile int i = 0; i < 7200; i++); // delay thô, tuong duong ~1ms @72MHz
    } while (range_status != 0x04);  // 0x04 = New Sample Ready

    return 0;
}
int VL6180X_Read_Range(void)
{
    return VL6180X_ReadByte(0x0062);  // RESULT__RANGE_VAL
}
int VL6180X_Clear_Interrupts(void)
{
    VL6180X_WriteByte(0x0015, 0x07);  // SYSTEM__INTERRUPT_CLEAR = 0x07
    return 0;
}
int VL6180X_MeasureRange(void)
{
    VL6180X_Start_Range();
    VL6180X_Poll_Range();
    int range = VL6180X_Read_Range();
    VL6180X_Clear_Interrupts();
    return range;
}
