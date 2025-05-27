#include "main.h"

uint32_t x, y, z;

void task1 (void) {
	while (1) {
		if (!(GPIOB->IDR & GPIO_IDR_IDR3)) {
    // Hành d?ng khi nh?n nút
			x++;  
		}
		else {
			x = 0;
		}
	}
}
void task2 (void) {
	while (1) {
		y++;
	}
}
void task3 (void) {
	while (1) {
		z++;
	}
}
int main (void) {
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
	RCC_Init();
	button_config();
	osKernelInit();
	osKernelAdd1Thread(task1);
	osKernelAdd1Thread(task2);
	osKernelAdd1Thread(task3);
	osKernelLaunch(QUANTA);
	while (1) {
	
	}
}

void RCC_Init(void) 
{
	// Step 1: Kích ho?t HSE (High-Speed External clock)
	RCC->CR |= RCC_CR_HSEON;
  while (!(RCC->CR & RCC_CR_HSERDY)) {
      // Ch? d?n khi HSE s?n sàng
  }

  // Step 2: Thi?t l?p Flash memory
  FLASH->ACR |= FLASH_ACR_PRFTBE;    // B?t prefetch buffer
  while (!(FLASH->ACR & FLASH_ACR_PRFTBS)) {
      // Ch? d?n khi prefetch buffer du?c b?t
  }
  FLASH->ACR &= ~FLASH_ACR_LATENCY;  // Xóa bits latency cu
  FLASH->ACR |= FLASH_ACR_LATENCY_2; // Thi?t l?p d? tr? 2 wait states

	// Step 3: C?u hình PLL (Phase-Locked Loop)
  RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL);  // Xóa c?u hình cu
  RCC->CFGR |= (RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9); // HSE làm ngu?n PLL, nhân v?i 9

	// Step 4: C?u hình các b? chia t?n h? th?ng
  RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2); // Xóa c?u hình cu
  RCC->CFGR |= (RCC_CFGR_HPRE_DIV1    // AHB prescaler: không chia
              | RCC_CFGR_PPRE1_DIV2   // APB1 prescaler: chia 2
              | RCC_CFGR_PPRE2_DIV1); // APB2 prescaler: không chia

  // Step 5: Kích ho?t PLL
  RCC->CR |= RCC_CR_PLLON;
  while (!(RCC->CR & RCC_CR_PLLRDY)) {
      // Ch? d?n khi PLL s?n sàng
  }

	// Step 6: Chuy?n clock h? th?ng sang PLL
  RCC->CFGR &= ~RCC_CFGR_SW;          // Xóa c?u hình ngu?n clock cu
  RCC->CFGR |= RCC_CFGR_SW_PLL;       // Ch?n PLL làm ngu?n clock h? th?ng
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {
      // Ch? d?n khi PLL du?c s? d?ng làm clock h? th?ng
  }
}
