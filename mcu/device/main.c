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

#include "main.h"

uint8_t Sema_Blance,Sema_COM, Sema_SD, Sema_RES, Sema_Motor;
Enable_Peripheral device;

void Receive_Form_VCU(void) {
	while (1) {
		osCooperative_Wait(&Sema_usart1_handler);
		if (buffer_usart1_data[0] == 'S') {
			system_reset(&device);
			Timer3_Delay_us(10);
			if (buffer_usart1_data[1] == '0') {
				device.current_pull = PULL;
			} else if (buffer_usart1_data[1] == '1') {
				device.current_pull = PULL_RE;
			}
			if (buffer_usart1_data[2] == '1') {
				device.res = Res_Auto;
			} else {
				device.res = Res_Off;
			}
			if (buffer_usart1_data[11] == '1') {
				device.blance = Blance_Enable;
				osSemaphore_Give(&Sema_Blance);
			} else {
				device.blance = Blance_Disable;
			}
			device.length = ascii_to_int(&buffer_usart1_data[3],4);
			device.times  = ascii_to_int(&buffer_usart1_data[7],4);
			device.speed = 1;
		} else if (buffer_usart1_data[0] == 'T') {
			GPIO_SetBits(Step_Port, Step_Enable);
			GPIO_ResetBits(Step_Port, Step_Dir);
		} else if (buffer_usart1_data[0] == 'U') {
			GPIO_ResetBits(Step_Port, Step_Enable);
			GPIO_ResetBits(Step_Port, Step_Dir);
		} else if (buffer_usart1_data[0] == 'D') {
			GPIO_ResetBits(Step_Port, Step_Enable);
			GPIO_SetBits(Step_Port, Step_Dir);
		}
	}
}

void System_Blance(void) {
	while (1) {
		osCooperative_Wait(&Sema_Blance);
		if (device.blance == Blance_Enable) {
			while (device.current_force < 2000) {
				GPIO_ResetBits(Step_Port, Step_Dir);
				GPIO_ResetBits(Step_Port, Step_Enable);
			} 
			while (device.current_force > 50) {
				GPIO_SetBits(Step_Port, Step_Dir);
			}
		}
		GPIO_ResetBits(Step_Port, Step_Dir);
		GPIO_SetBits(Step_Port, Step_Enable);
		device.blance = Blance_Disable;
		osThreadYield();
	}
}

void Read_Distance (void) {
	while (1) {
		uint32_t delay = 0;
		if ((device.blance == Blance_Disable) && (device.current_pull != NO_PULL)) {
			device.pull = Current_pull;
			if (device.pull == Current_pull) {
				while (device.current_length < device.length) {
					GPIO_ResetBits(Step_Port, Step_Dir);
					GPIO_ResetBits(Step_Port, Step_Enable);
					delay = g_Timer_Tick;
					while((g_Timer_Tick - delay) < 1370) {};
					device.current_length += 1;
					if (device.current_length >= device.length) {
					device.pull = Current_re;
					}
				}
			} 
			if (device.pull == Current_re) {
				while (device.current_length > 0) {
					GPIO_SetBits(Step_Port, Step_Dir);
					GPIO_ResetBits(Step_Port, Step_Enable);
					delay = g_Timer_Tick;
					while((g_Timer_Tick - delay) < 1370) {};
					device.current_length -=1;
					if (device.current_length <= 0) {
						device.pull = Current_pull;
						device.current_times += 1;
						if (device.current_times >= device.times) {
							GPIO_SetBits(Step_Port, Step_Enable);
							device.current_pull = NO_PULL;
						}
					}
				}
			}
		}
		osThreadYield();
	}
}

void Read_Force(void) {
	while (1) {
//		if (device.res == Res_Auto) {
//			osSemaphore_Give(&Sema_RES);
//		}
		device.current_force = -getForce(device.offset);
//		if (device.blance == Blance_Disable) {
//			if ((device.current_force > 55000) || (device.current_force < -5000)) {
//				GPIO_SetBits(Step_Port, Step_Enable);
//			}
//		}
//		osSemaphore_Give(&Sema_COM);
	}
}

void Read_Resistor(void) {
	while (1) {
		int8_t count = 0;
		float value = 0;
		//osCooperative_Wait(&Sema_RES);
		switch (count) {
			case 0:
				value = ADC_Range7_Resistance(2);
				count++;
				if (value > 2000000) {
					device.current_resistor = (uint32_t)((value) / 1 );
					count = 0;
					break;
				}
			case 1:
				value = ADC_Range6_Resistance(2);
				count++;
				if (value > 200000) {
					device.current_resistor = (uint32_t)((value) / 1 );
					count = 0;
					break;
				}
			case 2:
				value = ADC_Range5_Resistance(3, pga_2V);
				count++;
				if (value > 20000) {
					device.current_resistor = (uint32_t)((value) / 1 );
					count = 0;
					break;
				}
			case 3:
				value = ADC_Range4_Resistance(3, pga_2V);
				count++;
				if (value > 2000) {
					device.current_resistor = (uint32_t)((value) / 1 );
					count = 0;
					break;
				}
			case 4:
				value = ADC_Range3_Resistance(3, pga_2V);
				count++;
				if (value > 200) {
					device.current_resistor = (uint32_t)((value) / 1 );
					count = 0;
					break;
				}
			case 5:
				value = ADC_Range2_Resistance(3, pga_2V);
				count++;
				if (value > 20) {
					device.current_resistor = (uint32_t)((value) / 1 );
					count = 0;
					break;
				}
			case 6:
				value = ADC_Range1_Resistance(3, pga_2V);
				count++;
				if (value > 0.4) {
					device.current_resistor = (uint32_t)((value) / 1 );
					count = 0;
					break;
				}
			default:
				break;
		}
		osThreadYield();
	}
}

void Transmit_To_VCU(void) {
	while (1) {
		osCooperative_Wait(&Sema_COM);
		char buffer_force[6];
		char buffer_res[11];
		char buffer_length[6];
		char buffer_times[6];
		if (buffer_usart1_data[0] == 'S') {
			if ((device.length <= 0) || (device.times <= 0)) { 
				Usart1_printf("EEEEEEEEEEEEEEEEEEEEEEEEEEE");
			}	else if (device.current_force < -5000 ) {
				Usart1_printf("RRRRRRRRRRRRRRRRRRRRRRRRRRR");
			}	else if (device.current_force > 55000 ) {
				Usart1_printf("OOOOOOOOOOOOOOOOOOOOOOOOOOO");
			}	else {
				if (device.current_force <= 0) {
					device.current_force = 0;
				}
				buffer_force[0] = (device.current_force / 10000) % 10 + '0';
				buffer_force[1] = (device.current_force / 1000)  % 10 + '0';
				buffer_force[2] = (device.current_force / 100)   % 10 + '0';
				buffer_force[3] = (device.current_force / 10)    % 10 + '0';
				buffer_force[4] = (device.current_force / 1)     % 10 + '0';
				buffer_force[5] = '\0';
				
				buffer_res[0] = (device.current_resistor / 1000000000) % 10 + '0';
				buffer_res[1] = (device.current_resistor / 100000000)  % 10 + '0';
				buffer_res[2] = (device.current_resistor / 10000000)   % 10 + '0';
				buffer_res[3] = (device.current_resistor / 1000000)    % 10 + '0';
				buffer_res[4] = (device.current_resistor / 100000)     % 10 + '0';
				buffer_res[5] = (device.current_resistor / 10000)      % 10 + '0';
				buffer_res[6] = (device.current_resistor / 1000)       % 10 + '0';
				buffer_res[7] = (device.current_resistor / 100)        % 10 + '0';
				buffer_res[8] = (device.current_resistor / 10)         % 10 + '0';
				buffer_res[9] = (device.current_resistor / 1)          % 10 + '0';
				buffer_res[10] = '\0';
				
				buffer_length[0] = (device.current_length / 10000) % 10 + '0';
				buffer_length[1] = (device.current_length / 1000)  % 10 + '0';
				buffer_length[2] = (device.current_length / 100)   % 10 + '0';
				buffer_length[3] = (device.current_length / 10)    % 10 + '0';
				buffer_length[4] = (device.current_length / 1)     % 10 + '0';
				buffer_length[5] = '\0';
				
				buffer_times[0] = (device.current_times / 10000) % 10 + '0';
				buffer_times[1] = (device.current_times / 1000)  % 10 + '0';
				buffer_times[2] = (device.current_times / 100)   % 10 + '0';
				buffer_times[3] = (device.current_times / 10)    % 10 + '0';
				buffer_times[4] = (device.current_times / 1)     % 10 + '0';
				buffer_times[5] = '\0';
			
				char temp = device.pull + '0';
				
				Usart1_printf("D%s%s%s%s%c",buffer_force,buffer_res,buffer_length,buffer_times,temp);
			}
		}
		osThreadYield();
	}
}
int32_t l_set_system;
int main (void) {
	
	RCC_Clock_Init();
	system_setup(&device);
	
	IIC2_Init();
	ADC_GPIO_Measurement_Range();
	
	//Step_Driver_Init();
	Timer2_Init();
	
	Timer3_Init();

	Force_Sensor_Init();
	
//	Usart1_Init();
//	
//	GPIO_SetBits(SCK_PORT, SCK_PIN);
//  Timer3_Delay_us(10);
//  GPIO_ResetBits(SCK_PORT, SCK_PIN);
//  Timer3_Delay_us(10);
	
	for(int z = 0; z < 3; z++) {
		device.offset = weigh();
		if (device.offset == 0) {
			device.offset = weigh();
		}
	}	
//	I2C1_Init();
//	VL6180X_Init();
	
	osKernelInit();
	//osKernelAdd1Thread(*(Receive_Form_VCU));
	//osKernelAdd1Thread(*(System_Blance));
	osKernelAdd1Thread(*(Read_Force));
	osKernelAdd1Thread(*(Read_Resistor));
	//osKernelAdd1Thread(*(Read_Distance));
	//osKernelAdd1Thread(*(Transmit_To_VCU));
	osKernelLaunch(quanta);
  	
	while (1){
		// quay sang trai
		l_set_system = getForce(device.offset);
		GPIO_ResetBits(Step_Port, Step_Enable);
		GPIO_SetBits(Step_Port, Step_Dir);
		 
		if (l_set_system < -3000) {
			GPIO_ResetBits(Step_Port, Step_Dir);
			Timer3_Delay_ms(27400);
			GPIO_SetBits(Step_Port, Step_Enable);
			for(int z = 0; z < 3; z++) {
				device.offset = weigh();
				if (device.offset == 0) {
					device.offset = weigh();
				}
			}
			osSemaphore_Init(&Sema_Blance, 0);
			osSemaphore_Init(&Sema_Motor , 0);
			osSemaphore_Init(&Sema_COM   , 0);
			osSemaphore_Init(&Sema_SD    , 0);
			osSemaphore_Init(&Sema_RES   , 0);
			
			osKernelInit();
			osKernelAdd1Thread(*(Receive_Form_VCU));
			osKernelAdd1Thread(*(System_Blance));
			osKernelAdd1Thread(*(Read_Distance));
			osKernelAdd1Thread(*(Read_Force));
			osKernelAdd1Thread(*(Read_Resistor));
			osKernelAdd1Thread(*(Transmit_To_VCU));
			osKernelLaunch(quanta);
		}
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

void system_setup(Enable_Peripheral *val) {
	val->pull = Current_pull;
	val->current_pull = NO_PULL;
	val->times = 0;
	val->current_times = 0;
	val->length = 0;
	val->current_length = 0;
	val->offset = 0;
	val->current_force = 0;
	val->res = Res_Off;
	val->current_resistor = 0;
	val->blance = Blance_Disable;
}

void system_reset(Enable_Peripheral *val) {
	val->pull = Current_pull;
	val->current_pull = NO_PULL;
	val->current_times = 0;
	val->current_length = 0;
	val->current_force = 0;
	val->current_resistor = 0;
}

void Tach_So_Thuc_7_Int_2_Float(float so, char *buffer) {
	//tach phan nguyen va phan thap phan
  uint32_t so_nguyen = (uint32_t)so;
  uint32_t phan_thap_phan = (so - (float)so_nguyen)*100;
		
	buffer[0] = (so_nguyen/1000000) % 10 + '0';
	buffer[1] = (so_nguyen/100000)  % 10 + '0';
	buffer[2] = (so_nguyen/10000)   % 10 + '0';
	buffer[3] = (so_nguyen/1000)    % 10 + '0';
	buffer[4] = (so_nguyen/100)     % 10 + '0';
	buffer[5] = (so_nguyen/10)      % 10 + '0';
	buffer[6] = (so_nguyen/1)       % 10 + '0';
	buffer[7] = '.';
	
	buffer[8] = (phan_thap_phan/10) % 10 + '0';
	buffer[9] = (phan_thap_phan/1)  % 10 + '0';
	
	buffer[10] = '\0';
}

int Tach_So_Int_Char(uint32_t number, char *buffer, uint8_t len) {
    if (buffer == NULL || len == 0) return -1;

    for (int8_t i = len - 1; i >= 0; --i) {
        buffer[i] = (number % 10) + '0';
        number /= 10;
    }

    buffer[len] = '\0';  // Null-terminate

    return 1;  // Thành công
}

int ascii_to_int(uint8_t *str, int len) {
  int result = 0;
  int sign = 1;
  int i = 0;

  // Ki?m tra d?u âm n?u có
  if (len > 0 && str[0] == '-') {
    sign = -1;
    i = 1;
  }

  for (; i < len; i++) {
    if (str[i] >= '0' && str[i] <= '9') {
      result = result * 10 + (str[i] - '0');
    } else {
      // N?u g?p ký t? không ph?i s?, thoát (có th? tùy ch?n x? lý)
      break;
    }
  }

  return sign * result;
}

/*
########################################################
#																											 #
#          OPERATING SYSTEM EXCEPTION HANDLER          #
#																											 #
########################################################
*/
//chuong trinh con xu ly ngat systick
void SysTick_Handler(void) {
	//bat ngat cho pendsv de thuc hien chuyen task
	SCB->ICSR |= SCB_ICSR_PENDSVSET;
}

void TIM2_IRQHandler(void) {
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		g_Timer_Tick++;
    for(int i = 0; i < current_num_of_thread; i++){
			if(tcbs[i].Sleep > 0){
				tcbs[i].Sleep--;
			}
		}
	}
}

uint8_t buffer_usart1_data[USART1_BUFFER_SIZE];
volatile uint8_t current_data  = 0;
uint8_t Sema_usart1_handler = 0;

void USART1_IRQHandler(void) {
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		uint8_t byte = USART_ReceiveData(USART1);
		
		if (current_data < USART1_BUFFER_SIZE) {
			buffer_usart1_data[current_data] = byte;
			current_data++;
    } 
		if (current_data >= USART1_BUFFER_SIZE) {
			osSemaphore_Init(&Sema_usart1_handler, 1);
		  current_data = 0;
    }
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}
