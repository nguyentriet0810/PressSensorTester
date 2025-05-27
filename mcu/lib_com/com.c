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

void Usart1_GPIO_Init(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;

  // C?u h�nh TX (PA9)
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  // C?u h�nh RX (PA10)
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
}
void Usart1_Init(void) {
	// B?t clock cho USART1
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	Usart1_GPIO_Init();
	
  // C?u h�nh USART1
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
 

	//config NVIC for USART1
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =3;
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


// H m printf
void Usart1_printf(const char *format, ...) {
  char buffer[255];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  Usart1_SendString(buffer);
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

    *ptr = '\0';  // k?t th�c chu?i
    return buffer;
}
void Update_Data(int32_t *p_Force, float *p_Resistor, uint8_t *p_Length, uint16_t *p_Times) {
	Usart1_printf(":%d,%s,%d,%d:",*p_Force,float_to_string(*p_Resistor,4),*p_Length,*p_Times);
}


