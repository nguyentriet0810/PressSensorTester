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

uint8_t Sema_Draw, Sema_COM, Sema_SD, Sema_Value, Sema_MCU;
uint8_t g_draw_state = 0;
Enable_Peripheral device, g_old_device;

void Draw_Display(void) {
	while (1) {
		char buffer[12];
		osCooperative_Wait(&Sema_Draw);
		if(device.page == Page_Defaut) {
			if (g_draw_state == 1) {
				Draw_Page_default();
				X_Coordinate = 30; 
				Y_Coordinate = 195;
				g_draw_state = 3;
			}
			if (g_draw_state == 3) {
				if ((Y_Coordinate > 124) && (Y_Coordinate < 196)) {
					ILI9341_DrawText( X_Coordinate, Y_Coordinate - 35, " " ,YELLOW, BLACK, 3);
					ILI9341_DrawText( X_Coordinate, Y_Coordinate + 35, " " ,YELLOW, BLACK, 3);
				}
				ILI9341_DrawText(X_Coordinate, Y_Coordinate, ">" ,YELLOW, BLACK, 3);
				g_draw_state = 0;
			}
		} else if (device.page == Page_Settings) {
			if (g_draw_state == 1) {
				Draw_Page_Settings();
				
				X_Coordinate = 20; 
				Y_Coordinate = 55;
				g_draw_state = 2;
			} 
			if (g_draw_state == 2) {
				if (device.func == PULL) {
					ILI9341_DrawText(220, 55, " PULL  ", RED   , BLACK, 3);
				} else {
					ILI9341_DrawText(220, 55, " P&R   ", RED   , BLACK, 3);
				}
				if (device.res == Res_Off) {
					ILI9341_DrawText(220, 90, " OFF   ", RED   , BLACK, 3);
				} else if (device.res == Res_Auto){
					ILI9341_DrawText(220, 90, " ON  "  , RED   , BLACK, 3);
				}
				Tach_So_Int_Char(device.force, buffer, 4);
				ILI9341_DrawText(238, 125, buffer, RED   , BLACK, 3);
				Tach_So_Int_Char(device.times , buffer, 4);
				ILI9341_DrawText(238, 160, buffer, RED   , BLACK, 3);
				g_draw_state = 3;
			}		
			if (g_draw_state == 3) {
				if ((Y_Coordinate > 55) && (Y_Coordinate < 161)) {
					ILI9341_DrawText( X_Coordinate, Y_Coordinate - 35, " " ,YELLOW, BLACK, 3);
				}
				ILI9341_DrawText(X_Coordinate, Y_Coordinate, ">" ,YELLOW, BLACK, 3);
				ILI9341_DrawText( X_Coordinate, Y_Coordinate + 35, " " ,YELLOW, BLACK, 3);
				g_draw_state = 0;
			}
		} else if (device.page == Page_Options) {
			if (g_draw_state == 1) {
				Draw_Page_Options();
				
				X_Coordinate = 20; 
				Y_Coordinate = 55;
				g_draw_state = 2;
			} 
			if (g_draw_state == 2) {
				if (device.s_pull == PULL_UP) {
					ILI9341_DrawText(40, 55, "PULL DOWN"  , GREEN, BLACK, 3);
					ILI9341_DrawText(40, 90, "PULL UP"   , RED  , BLACK, 3);
				} else if (device.s_pull == PULL_DW) {
					ILI9341_DrawText(40, 55, "PULL DOWN"  , RED  , BLACK, 3);
					ILI9341_DrawText(40, 90, "PULL UP"   , GREEN, BLACK, 3);
				} else {
					ILI9341_DrawText(40, 55, "PULL DOWN"  , RED  , BLACK, 3);
					ILI9341_DrawText(40, 90, "PULL UP"   , RED  , BLACK, 3);
				} 
				if (device.uart == Uart_On) {
					ILI9341_DrawText(40,125, "COM  PORT: ON ", GREEN, BLACK, 3);
				} else {
					ILI9341_DrawText(40,125, "COM  PORT: OFF", RED  , BLACK, 3);
				}
//				if (device.sd == Sd_On) {
//					ILI9341_DrawText(40,160, "SAVE DATA: ON ", GREEN, BLACK, 3);
//				} else {
//					ILI9341_DrawText(40,160, "SAVE DATA: OFF", RED, BLACK, 3);
//				}
				g_draw_state = 3;
			}
			if (g_draw_state == 3) {
				if ((Y_Coordinate > 55) && (Y_Coordinate < 161)) {
					ILI9341_DrawText(X_Coordinate, Y_Coordinate - 35, " " ,YELLOW, BLACK, 3);
				}
				ILI9341_DrawText(X_Coordinate, Y_Coordinate     , ">" ,YELLOW, BLACK, 3);
				ILI9341_DrawText(X_Coordinate, Y_Coordinate + 35, " " ,YELLOW, BLACK, 3);
				g_draw_state = 0;
			} 
		} else if (device.page == Page_Start) {
			if (g_draw_state == 1) {
				Draw_Page_Start();
				g_draw_state = 2;
			}
			if (g_draw_state == 2) {
				//hien thi so lan keo duoc
				Tach_So_Int_Char(device.current_times     , buffer, 4);
				ILI9341_DrawText(170, 55, buffer , WHITE, BLACK, 3);
				//hien thi luc dang do duoc
				Tach_So_Int_Char(device.current_force     ,buffer,  5);
				ILI9341_DrawText(170, 90, buffer , WHITE, BLACK, 3);
				ILI9341_DrawText(280, 90, "mN" , WHITE, BLACK, 3);
				//hien thi dien tro dang do duoc
				char data[7];
				if (device.current_res > 99999999) {
					data[0] = (device.current_res / 10000000000) % 10 + '0';
					data[1] = (device.current_res / 1000000000)  % 10 + '0';
					data[2] = (device.current_res / 100000000)   % 10 + '0';
					data[3] = '.';
					data[4] = (device.current_res / 10000000)    % 10 + '0';
					data[5] = (device.current_res / 1000000)     % 10 + '0';
					data[6] = '\0';
					ILI9341_DrawText(280, 125, "MR" , WHITE, BLACK, 3);
				} else if ((device.current_res > 99999) && (device.current_res <= 99999999)) {
					data[0] = (device.current_res / 10000000)  % 10 + '0';
					data[1] = (device.current_res / 1000000)   % 10 + '0';
					data[2] = (device.current_res / 100000)    % 10 + '0';
					data[3] = '.';
					data[4] = (device.current_res / 10000)     % 10 + '0';
					data[5] = (device.current_res / 1000)      % 10 + '0';
					data[6] = '\0';
					ILI9341_DrawText(280, 125, "KR" , WHITE, BLACK, 3);
				} else if (device.current_res <= 99999) {
					data[0] = (device.current_res / 10000)      % 10 + '0';
					data[1] = (device.current_res / 1000)       % 10 + '0';
					data[2] = (device.current_res / 100)        % 10 + '0';
					data[3] = '.';
					data[4] = (device.current_res / 10)         % 10 + '0';
					data[5] = (device.current_res / 1)          % 10 + '0';
					data[6] = '\0';
					ILI9341_DrawText(280, 125, " R" , WHITE, BLACK, 3);
				}
				ILI9341_DrawText(170, 125, data, WHITE, BLACK, 3);
				
				//hien thi khoang cach do duoc
				//Tach_So_Int_Char(device.current_length    ,buffer, 3);
				//ILI9341_DrawText(170, 160, buffer, WHITE, BLACK, 3);
			}
		} else if (device.page == Page_Alarm) {
			if (g_draw_state == 1) {
				Draw_Warning();
				g_draw_state = 2;
			}
		}
	}
}

void Check_Button(void) {
	while (1) {
		if        ( GPIO_ReadInputDataBit(Button_Port, Button_Hold  ) == 0) {
			device.page = Page_Defaut;
			g_draw_state = 1;
			osSemaphore_Give(&Sema_Draw);
		} 
		if ( GPIO_ReadInputDataBit(Button_Port, Button_Choose) == 0) {
			for (int i = 0; i < 3; ++i) {
				if (device.page == Page_Defaut) {
					if (Y_Coordinate == page_actions[i].y_coord) {
						device.page = page_actions[i].next_page;
						if (device.page == Page_Start) {
							osSemaphore_Give(&Sema_MCU);
						}
						g_draw_state = 1;
            break;
          }
        }
      }
			osSemaphore_Give(&Sema_Draw);
		}
		if ( GPIO_ReadInputDataBit(Button_Port, Button_Top   ) == 0) {
			for (int i = 0; i < 3; ++i) {
				if (device.page == top_actions[i].current_page) {
					if (Y_Coordinate <= top_actions[i].y_coord_top_limit) {
						Y_Coordinate = top_actions[i].y_coord_top_limit;
						break;
					} else {
						top_actions[i].action(&Y_Coordinate);  // G?i hàm n?u c?n
						break;
					}
        }
      }
			g_draw_state = 3;
			osSemaphore_Give(&Sema_Draw);
		} 
		if ( GPIO_ReadInputDataBit(Button_Port, Button_Bottom) == 0) {
			for (int i = 0; i < 3; ++i) {
				if (device.page == bottom_actions[i].current_page) {
					if (Y_Coordinate >= bottom_actions[i].y_coord_bot_limit) {
						Y_Coordinate = bottom_actions[i].y_coord_bot_limit;
						break;
					} else {
						bottom_actions[i].action(&Y_Coordinate);  // G?i hàm n?u c?n
						break;
					}
        }
      }
			g_draw_state = 3;
			osSemaphore_Give(&Sema_Draw);
		} 
		if ( GPIO_ReadInputDataBit(Button_Port, Button_Plus  ) == 0) {
			if ((device.page == Page_Settings) && (Y_Coordinate == 125)) {
				device.force += 1;
				if (device.force > 50) {
					device.force = 50;
				}
			} else if ((device.page == Page_Settings) && (Y_Coordinate == 160)) {
				device.times += 1;
				if (device.times > 9999) {
					device.times = 9999;
				}
			} else if ((device.page == Page_Settings) && (Y_Coordinate == 55)) {
				device.func = PULL_and_RE;
			} else if ((device.page == Page_Settings) && (Y_Coordinate == 90)) {
				device.res = Res_Auto;
			}	else if ((device.page == Page_Options) && (Y_Coordinate == 125)) {
				device.uart = Uart_On;
			} else if ((device.page == Page_Options) && (Y_Coordinate == 160)) {
				device.sd = Sd_On;
			} else if ((device.page == Page_Options) && (Y_Coordinate == 90)) {
				device.s_pull = PULL_DW;
			} else if ((device.page == Page_Options) && (Y_Coordinate == 55)) {
				device.s_pull = PULL_UP;
			} 
			g_draw_state = 2;
			osSemaphore_Give(&Sema_Draw);
			osSemaphore_Give(&Sema_MCU);
		} 
		if ( GPIO_ReadInputDataBit(Button_Port, Button_Sub ) == 0) {
			if ((device.page == Page_Settings) && (Y_Coordinate == 125)) {
				device.force -= 1;
				if (device.force < 1) {
					device.force = 1;
				}
			} else if ((device.page == Page_Settings) && (Y_Coordinate == 160)) {
				device.times -= 1;
				if (device.times < 1) {
					device.times = 0;
				}
			} else if ((device.page == Page_Settings) && (Y_Coordinate == 55)) {
				device.func = PULL;
			} else if ((device.page == Page_Settings) && (Y_Coordinate == 90)) {
				device.res = Res_Off;
			}	else if ((device.page == Page_Options) && (Y_Coordinate == 125)) {
				device.uart = Uart_Off;
			} else if ((device.page == Page_Options) && (Y_Coordinate == 160)) {
				device.sd = Sd_Off;
			} else if ((device.page == Page_Options) && (Y_Coordinate == 55)) {
				device.s_pull = NO_PULL;
			} else if ((device.page == Page_Options) && (Y_Coordinate == 90)) {
				device.s_pull = NO_PULL;
			} 
			g_draw_state = 2;
			osSemaphore_Give(&Sema_MCU);
			osSemaphore_Give(&Sema_Draw);
		} 
		osThreadsSleep(20);
	}
}

void Transmit_Data(void) {
	while (1) {
		osCooperative_Wait(&Sema_MCU);
		char l_length_buffer[6];
		char l_times_buffer[6];
		char l_speed_buffer[6];
		if (device.s_pull == PULL_UP) {
			Usart1_printf("UUUUUUUUUUUUUUUU");
		} else if (device.s_pull == PULL_DW) {
			Usart1_printf("DDDDDDDDDDDDDDDD");
		} else if (device.s_pull == NO_PULL) {
			Usart1_printf("TTTTTTTTTTTTTTTT");
		}
		if (device.page == Page_Start) {
			if ((device.force == 0) || (device.times == 0)) {
				g_draw_state = 2;
			} else {
				Tach_So_Int_Char(device.force,l_length_buffer, 4);
				Tach_So_Int_Char(device.times ,l_times_buffer , 4);
				Tach_So_Int_Char(device.speed ,l_speed_buffer , 4);
				Usart1_printf("S%d%d%s%s%d%s",device.func,device.res,l_length_buffer,l_times_buffer,device.blance,l_speed_buffer);
			}
		}
		osThreadYield();
	}
}

void Receive_Data(void) {
	while (1) {
		if (g_buffer_mcu_flag == 1) {
			if       (buffer_mcu_data[0] == 'D') {
				device.current_force  = ascii_to_int(&buffer_mcu_data[1],5);
				device.current_res    = ascii_to_int(&buffer_mcu_data[6],10);
				device.current_length = ascii_to_int(&buffer_mcu_data[16],5);
				device.current_times  = ascii_to_int(&buffer_mcu_data[21],5);
				device.mode           = buffer_mcu_data[26];
				g_buffer_mcu_flag = 0;
				if (device.uart == Uart_On) {
					Usart3_printf("%d,%d,0,%d,%d,%d,0,%d\n",device.func,device.current_times,device.force,device.current_force,device.current_res,device.current_length);
				}
				g_draw_state = 2;
				osSemaphore_Give(&Sema_Draw);
			}
//			} else if (buffer_mcu_data[0] == 'R') {
//				device.page = Page_Alarm;
//				g_draw_state = 1;
//				osSemaphore_Give(&Sema_Draw);
//			} else if (buffer_mcu_data[0] == 'L') {
//				device.page = Page_Alarm;
//				g_draw_state = 1;
//				osSemaphore_Give(&Sema_Draw);
//			} else if (buffer_mcu_data[0] == 'F') {
//				device.page = Page_Alarm;
//				g_draw_state = 1;
//				osSemaphore_Give(&Sema_Draw);
//			} else {
//				
//			}
		}
		if (g_buffer_pc_flag == 1) {
			g_buffer_pc_flag = 0;
		}
		osThreadYield();
	}
}

void SD_Save_Data(void) {
	while (1) {
		osThreadYield();
	}
}

//test tounch
//uint16_t axis_x, axis_y;

int main (void) {
	
	Setup(&device);
	
	RCC_Clock_Init();
	
	Timer2_Init();
	
	Timer3_Init();

	Usart1_Init();
	Usart3_Init();
	Button_GPIO_Init();
	test_button_Init();
	Spi1_Init();
  ILI9341_Reset();
	ILI9341_Init();  // Kh?i t?o màn hình
	//ILI9341_FillScreen(BLACK);  // Ð?t n?n màu d?
	
	g_draw_state = 1;
	//Draw_Warning();
	
	osSemaphore_Init(&Sema_Draw , 1);
	osSemaphore_Init(&Sema_COM  , 0);
	osSemaphore_Init(&Sema_SD   , 0);
	osSemaphore_Init(&Sema_Value, 0);
	osSemaphore_Init(&Sema_MCU  , 0);
	
	osKernelInit();
	osKernelAdd1Thread(*(Check_Button));
	osKernelAdd1Thread(*(Draw_Display));
	osKernelAdd1Thread(*(Transmit_Data));
	osKernelAdd1Thread(*(Receive_Data));
	//osKernelAdd1Thread(*(SD_Save_Data));
	osKernelLaunch(quanta);
	
//	Touch_Init();
	
	while (1){
		if (GPIO_ReadInputDataBit(Button_Port, GPIO_Pin_8 ) == 0) {
			GPIO_SetBits(GPIOC,GPIO_Pin_13);
		}
		else{
			GPIO_ResetBits(GPIOC,GPIO_Pin_13);
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
void tach_4_chu_so_dau(float so, uint8_t out[4]) {
    uint32_t so_nguyen = (uint32_t)so;
    float phan_thap_phan = so - (float)so_nguyen;
    int count = 0;

    // X? lý ph?n nguyên
    if (so_nguyen == 0) {
        out[count++] = 0;
    } else {
        // Ð?m s? ch? s? trong ph?n nguyên
        uint32_t temp = so_nguyen;
        int so_chu_so = 0;
        while (temp > 0) {
            temp /= 10;
            so_chu_so++;
        }
        // Trích t?ng ch? s? t? trái sang
        for (int i = so_chu_so - 1; i >= 0 && count < 4; i--) {
            uint32_t mu_10 = 1;
            for (int j = 0; j < i; j++) mu_10 *= 10;
            out[count++] = (so_nguyen / mu_10) % 10;
        }
    }
    // B? sung ph?n th?p phân n?u chua d? 4 ch? s?
    while (count < 4) {
        phan_thap_phan *= 10.0f;
        out[count++] = (uint8_t)phan_thap_phan;
        phan_thap_phan -= (uint8_t)phan_thap_phan;
    }
}
int Tach_So_Int_Char(int32_t number, char *buffer, uint8_t len) {
    if (buffer == NULL || len == 0) return -1;

    for (int8_t i = len - 1; i >= 0; --i) {
        buffer[i] = (number % 10) + '0';
        number /= 10;
    }

    buffer[len] = '\0';  // Null-terminate

    return 1;  // Thành công
}

int ascii_to_int(uint8_t *data, int len) {
  uint32_t value = 0;
  for (int i = 0; i < len; i++) {
		if (data[i] < '0' || data[i] > '9') {
    // Ký t? không h?p l?
			return 0xFFFFFFFF;  // Giá tr? báo l?i
      }
      value = value * 10 + (data[i] - '0');
  }
  return value;
}

void Setup(Enable_Peripheral *val) {
	val->page   = Page_Defaut;
	val->uart   = Uart_Off;
	val->sd     = Sd_Off;
	val->force  = 1;
	val->times  = 1;
	val->func   = PULL;
	val->step   = Step_1MM;
	val->res    = Res_Off;
	val->alarm  = 0;
	val->speed  = 0;
	val->s_pull = NO_PULL;
	val->blance = Blance_Enable;
}
