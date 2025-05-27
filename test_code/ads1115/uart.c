#include <uart.h>

void USART3_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

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
	
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

		USART_Cmd(USART3, ENABLE);
		
		NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}
void USART3_SendChar(char c) {
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    USART_SendData(USART3, c);
}
void USART3_SendString(const char *str) {
    while (*str) {
			USART3_SendChar(*str++);
    }
}
void UART_printf(const char *format, ...) {
    char buffer[128]; //buffer chua chuoi can in
    va_list args; //tao danh sach tham so
    va_start(args, format); //khoi tao cac tham so
    vsnprintf(buffer, sizeof(buffer), format, args); //nhan danh sach, do chieu dai va dien vao buffer
    va_end(args); // ket thuc
    USART3_SendString(buffer); //gui uart
}
void USART3_SendBuffer(uint8_t *data, uint16_t len) {
    for (uint16_t i = 0; i < len; i++) {
        USART3_SendChar(data[i]); // S? d?ng USART3_SendChar() dã có
    }
}
