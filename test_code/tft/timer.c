#include "timer.h"

void Timer3_Init(void) {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // B?t clock cho TIM3

  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_TimeBaseStructure.TIM_Prescaler = 71;  // Chia t?n s? 72 MHz xu?ng còn 1 MHz (1 tick = 1 µs)
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 0xFFFF; // Giá tr? t?i da
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  TIM_Cmd(TIM3, ENABLE); // B?t d?u Timer3
}

// Hàm delay theo micro giây (µs)
void Timer3_Delay_us(uint16_t us) {
  TIM3->CNT = 0; // Reset b? d?m
  while (TIM3->CNT < us); // Ð?i d?n khi d?t s? micro giây mong mu?n
}

// Hàm delay theo mili giây (ms)
void Timer3_Delay_ms(uint16_t ms) {
  while (ms--) {
		Timer3_Delay_us(1000); // M?i l?n delay 1000 µs = 1 ms
  }
}
