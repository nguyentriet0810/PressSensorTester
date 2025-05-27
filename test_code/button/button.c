#include "button.h"

void button_config(void) {
		RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;  // B?t clock AFIO
		AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_1;   // T?t JTAG, gi? SWD
    // Bu?c 1: B?t clock cho GPIOB
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
		
    GPIOB->CRL &= ~(GPIO_CRL_MODE4 | GPIO_CRL_CNF4);
    GPIOB->CRL |= GPIO_CRL_CNF4_1;
    GPIOB->ODR |= GPIO_ODR_ODR4;

    GPIOB->CRL &= ~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5);
    GPIOB->CRL |= GPIO_CRL_CNF5_1;
    GPIOB->ODR |= GPIO_ODR_ODR5;

    GPIOB->CRL &= ~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6);
    GPIOB->CRL |= GPIO_CRL_CNF6_1;
    GPIOB->ODR |= GPIO_ODR_ODR6;

    GPIOB->CRL &= ~(GPIO_CRL_MODE7 | GPIO_CRL_CNF7);
    GPIOB->CRL |= GPIO_CRL_CNF7_1;
    GPIOB->ODR |= GPIO_ODR_ODR7;

    GPIOB->CRH &= ~(GPIO_CRH_MODE8 | GPIO_CRH_CNF8); // Xóa c?u hình PB8
    GPIOB->CRH |= GPIO_CRH_CNF8_1;  // Input pull-up/pull-down
    GPIOB->ODR |= GPIO_ODR_ODR8;    // Kéo lên
		
		GPIOB->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9); // Xóa c?u hình PB8
    GPIOB->CRH |= GPIO_CRH_CNF9_1;  // Input pull-up/pull-down
    GPIOB->ODR |= GPIO_ODR_ODR9;    // Kéo lên
}

void test_button(void) {
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	
	GPIOC->CRH &= ~GPIO_CRH_CNF13_0;
	GPIOC->CRH &= ~GPIO_CRH_CNF13_1;
	GPIOC->CRH |= GPIO_CRH_MODE13_0;
	GPIOC->CRH |= GPIO_CRH_MODE13_1;
	GPIOC->ODR |= GPIO_ODR_ODR13;
}
