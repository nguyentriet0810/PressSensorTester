#include "stm32f10x.h"
#include "tft.h"

void RCC_Clock_Init (void);

int main(void) {
	RCC_Clock_Init();
	Timer3_Init();
	Spi1_Init();

	ILI9341_Reset();
	ILI9341_Init();  // Kh?i t?o màn hình
	
	ILI9341_FillScreen(RED);
	
  while (1) {
        
  }
}

void RCC_Clock_Init (void) {
	// Enable clock AFIO
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;  
	//Disable JTAG but keep SWD enabled (free pb3 pb4 pa15)
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
	// Step 1: Enable HSE and wait for it to be ready
  RCC_HSEConfig(RCC_HSE_ON);
  while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);

  // Step 2: Configure Flash memory latency and enable Prefetch Buffer
  FLASH_SetLatency(FLASH_Latency_2);
  FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

  // Step 3: Configure PLL (HSE as source, PLL multiplier x9)
  RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
  RCC_PLLCmd(ENABLE);
  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

  // Step 4: Configure bus clocks (after PLL is ready)
  RCC_HCLKConfig(RCC_SYSCLK_Div1);   // AHB = SYSCLK = 72MHz
  RCC_PCLK1Config(RCC_HCLK_Div2);    // APB1 = AHB / 2 = 36MHz
  RCC_PCLK2Config(RCC_HCLK_Div1);    // APB2 = AHB / 1 = 72MHz

  // Step 5: Switch system clock to PLL
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  while (RCC_GetSYSCLKSource() != 0x08); // 0x08: PLL used as system clock
}
