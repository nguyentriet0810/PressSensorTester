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
#include "com.h"

uint8_t buffer_mcu_data[USART1_BUFFER_SIZE];
uint8_t buffer_pc_data[USART3_BUFFER_SIZE];

volatile uint8_t current_mcu_data = 0;
volatile uint8_t current_pc_data = 0;

uint8_t g_buffer_mcu_flag = 0;
uint8_t g_buffer_pc_flag  = 0;

void Usart1_GPIO_Init(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;

  // C?u hình TX (PA9)
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  // C?u hình RX (PA10)
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void Usart1_Init(void) {
	// B?t clock cho USART1
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	Usart1_GPIO_Init();
	
  // C?u hình USART1
  USART_InitTypeDef USART_InitStruct;
  USART_InitStruct.USART_BaudRate = 115200;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Init(USART1, &USART_InitStruct);
	
	// RX Enable IRQ
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	// C?u hình NVIC cho USART1
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	// B?t USART1
  USART_Cmd(USART1, ENABLE);
}

void Usart1_SendChar(char c) {
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
  USART_SendData(USART1, c);
}

void Usart1_SendString(const char *str) {
	if (str == '\0') return;
  while (*str) {
		Usart1_SendChar(*str++);
  }
}

void Usart3_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // Ð?m b?o AFIO du?c b?t d? dùng NVIC

    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10; // TX
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11; // RX
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate = 115200;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART3, &USART_InitStruct);

    // B?t ng?t RXNE
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    // C?u hình NVIC cho USART3
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    USART_Cmd(USART3, ENABLE);
}

// H m g?i k  t? qua UART3
void Usart3_SendChar(char c) {
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    USART_SendData(USART3, c);
}

// H m g?i chu?i qua UART3
void Usart3_SendString(const char *str) {
		if (str == '\0') return;
    while (*str) {
        Usart3_SendChar(*str++);
    }
}

// H m printf
void Usart1_printf(const char *format, ...) {
  char buffer[255];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  Usart1_SendString(buffer);
}

void Usart3_printf(const char *format, ...) {
  char buffer[255];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  Usart3_SendString(buffer);
}

char* float_to_string(float value, int precision) {
    static char buffer[32];  // buffer tinh d? gi? k?t qu?
    char *ptr = buffer;

    int int_part = (int)value;
    float frac_part = fabsf(value - (float)int_part);

    if (value < 0 && int_part == 0) {
        strcpy(ptr, "-0");
    } else {
        sprintf(ptr, "%d", int_part);
    }

    // Di chuy?n con tr? d?n cu?i chu?i d? n?i ti?p
    ptr = buffer + strlen(buffer);
    *ptr++ = '.';

    for (int i = 0; i < precision; i++) {
        frac_part *= 10.0f;
        int digit = (int)frac_part;
        *ptr++ = digit + '0';
        frac_part -= digit;
    }

    *ptr = '\0';  // k?t thúc chu?i
    return buffer;
}

void USART1_IRQHandler(void) {
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		uint8_t byte = USART_ReceiveData(USART1);
		
		if (current_mcu_data < USART1_BUFFER_SIZE) {
			buffer_mcu_data[current_mcu_data] = byte;
			current_mcu_data++;
    } 
		if (current_mcu_data >= USART1_BUFFER_SIZE) {
			g_buffer_mcu_flag = 1;
		  current_mcu_data = 0;
    }
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

void USART3_IRQHandler(void) {
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) {
		uint8_t byte = USART_ReceiveData(USART3);
		
		if (current_pc_data < USART3_BUFFER_SIZE) {
			buffer_pc_data[current_pc_data] = byte;
			current_pc_data++;
    } 
		if (current_pc_data >= USART3_BUFFER_SIZE) {
			g_buffer_pc_flag = 1;
		  current_pc_data = 0;
    }
		
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	}
}

