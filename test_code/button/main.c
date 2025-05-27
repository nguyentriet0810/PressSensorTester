#include <main.h>

uint32_t x;

int main (void) {
	RCC_Init();
	button_config();
	test_button();
	while (1) {
		if((GPIOB->IDR & GPIO_IDR_IDR4) == 0 || (GPIOB->IDR & GPIO_IDR_IDR5) == 0 || (GPIOB->IDR & GPIO_IDR_IDR6) == 0
			|| (GPIOB->IDR & GPIO_IDR_IDR7) == 0 || (GPIOB->IDR & GPIO_IDR_IDR8) == 0 || (GPIOB->IDR & GPIO_IDR_IDR9) == 0) {
			GPIOC->ODR |= GPIO_ODR_ODR13;
		}
		else {
			GPIOC->ODR &= ~GPIO_ODR_ODR13;
		}
	}
}
void RCC_Init(void) {
	//step0: Disable JTAG but keep SWD enabled
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
	//setp1: enable HSE
	RCC->CR |= RCC_CR_HSEON;
	while (!(RCC->CR & RCC_CR_HSERDY));
	//step2: setup flash memory
	FLASH->ACR |= FLASH_ACR_PRFTBE;
	while (!(FLASH->ACR & FLASH_ACR_PRFTBS));
	FLASH->ACR |= FLASH_ACR_LATENCY_2;
	//step3: config pll
	RCC->CFGR |= RCC_CFGR_PLLSRC_HSE; //HSE is input PLL
	RCC->CFGR |= RCC_CFGR_PLLMULL9; // PLL x9
	//step4: config bus
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; // AHB1 = SYSCLK/2 = 36M
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1; // AHB2 = SYSCLK/1 = 72M
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;  // AHB = SYSCLK = 72M
	//step5: enable PLL
	RCC->CR |= RCC_CR_PLLON;
	while (!(RCC->CR & RCC_CR_PLLRDY));
	//step6: running in PLL mode
	RCC->CFGR |= RCC_CFGR_SW;
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL); 
}
