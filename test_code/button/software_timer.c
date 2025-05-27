#include <software_timer.h>

void TIM4_Config(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    TIM4->PSC = 7200 - 1;    // 72MHz / 7200 = 10kHz (chu k? = 0.1ms)
    TIM4->ARR = 10 - 1;      // Ð?m d? 10 l?n -> 1ms
    TIM4->CR1 |= TIM_CR1_CEN;
    while (!(TIM4->SR & TIM_SR_UIF));
    TIM4->SR &= ~TIM_SR_UIF;
}

void Delay_ms(uint32_t ms) {
    while (ms--) {
        TIM4->CNT = 0;
        TIM4->SR &= ~TIM_SR_UIF;
        while (!(TIM4->SR & TIM_SR_UIF));
    }
}

