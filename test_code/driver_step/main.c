#include <stm32f10x.h>
#include <stdint.h>

#define Step_Port    GPIOA
#define Step_Enable  GPIO_Pin_7
#define Micro_Step_1 GPIO_Pin_6
#define Micro_Step_2 GPIO_Pin_5
#define Micro_Step_3 GPIO_Pin_4
#define Step_Reset   GPIO_Pin_3
#define Step_Sleep   GPIO_Pin_2
#define Step_Pul     GPIO_Pin_1
#define Step_Dir     GPIO_Pin_0

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
void Step_Driver_Init(void)	 {
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;

	//Config PA7, PA6, PA5, PA4, PA3, PA2, PA0 as output push-ull
  GPIO_InitStructure.GPIO_Pin = Step_Enable | Micro_Step_1 | Micro_Step_2 | Micro_Step_3 | Step_Reset | Step_Sleep | Step_Dir;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(Step_Port, &GPIO_InitStructure);
	
	//config PA1 as AFIO PWM
	GPIO_InitStructure.GPIO_Pin = Step_Pul;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(Step_Port, &GPIO_InitStructure);
	
	GPIO_ResetBits(Step_Port, Step_Enable | Micro_Step_1 | Micro_Step_2 | Micro_Step_3 | Step_Dir);
	GPIO_SetBits(Step_Port, Step_Reset | Step_Sleep);
}
void Timer2_Init(void) {
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  TIM_TimeBaseInitTypeDef timerInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;


  timerInitStructure.TIM_Prescaler = 71;
  timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  timerInitStructure.TIM_Period = 999;
  timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  timerInitStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM2, &timerInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 499;             // Duty cycle
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);      // Cho phép ng?t
  TIM_Cmd(TIM2, ENABLE);                          // B?t d?u timer

  NVIC_InitTypeDef nvicStructure;
  nvicStructure.NVIC_IRQChannel = TIM2_IRQn;
  nvicStructure.NVIC_IRQChannelPreemptionPriority = 1;
  nvicStructure.NVIC_IRQChannelSubPriority = 1;
  nvicStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvicStructure);
}
void PWM_Init(void) {
    // 1. B?t Clock cho TIM2 và GPIOA
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // 2. C?u hình chân PA0 (TIM2_CH1) làm Alternate Function Push-Pull
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 3. C?u hình TIM2
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
    TIM_TimeBaseStruct.TIM_Prescaler = 71;  // Không chia clock
    TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_Period = 199;  // ARR = 3599
    TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStruct);

    // 4. C?u hình PWM Mode cho TIM2_CH1
    TIM_OCInitTypeDef TIM_OCInitStruct;
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse = 100;  // 50% Duty Cycle
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM2, &TIM_OCInitStruct);
    
		TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
		
    // 5. Kích ho?t PWM
    TIM_Cmd(TIM2, ENABLE);
}

int main() {
	RCC_Clock_Init();
	Step_Driver_Init();
	Timer2_Init();
	while(1) {
		TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	}
}
