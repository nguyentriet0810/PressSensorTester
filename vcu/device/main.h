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
#ifndef __MAIN__
#define __MAIN__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_flash.h>

#include "os.h"
#include "timer.h"
#include "tft.h"
#include "touch.h"
#include "button.h"
#include "com.h"
#include "sdcard.h"
#include "fonts.h"

#define quanta 1 //(ms)
	
typedef enum {
	PULL = 0,
	PULL_and_RE = 1,
} Pull_Mode;

typedef enum {
	Blance_Disable = 0,
	Blance_Enable  = 1,
} Auto_Blance;

typedef enum {
	NO_PULL = 0,
	PULL_UP = 1,
	PULL_DW = 2,
} PULL_SETUP;

typedef enum {
	Step_4MM   = 0,
	Step_2MM   = 1,
	Step_1MM   = 2,
	Step_0_5MM = 3,
} Step_Size;

typedef enum {
	Res_Off  = 0,
	Res_Auto = 1,
} Resistor_Mode;

typedef enum {
	Uart_Off = 0,
	Uart_On = 1,
} Uart_Mode;

typedef enum {
	Sd_Off = 0,
	Sd_On = 1,
} Sd_Mode;

typedef struct {
	Page_Type page;
	Pull_Mode func;
	Step_Size step;
	Resistor_Mode res;
	int16_t times;
	int16_t length;
	Sd_Mode sd;
	Uart_Mode uart;
	uint8_t  alarm;
	int8_t speed;
	PULL_SETUP s_pull;
	Auto_Blance blance;
	uint32_t current_force;
	uint32_t current_res;
	uint32_t current_length;
	uint32_t current_times;
	uint8_t mode;
} Enable_Peripheral;

void RCC_Clock_Init (void);
void tach_4_chu_so_dau(float so, uint8_t out[4]);
int Tach_So_Int_Char(int32_t number, char *buffer, uint8_t len);
void Setup(Enable_Peripheral *val);
void int_to_string(int16_t *data, int16_t data_size);
int ascii_to_int(uint8_t *str, int len);

void Draw_Display(void);
void Check_Button(void);
void Transmit_Data(void);
void Receive_Data(void);
void SD_Save_Data(void);
	
int main (void);

#endif /*__MAIN__*/
