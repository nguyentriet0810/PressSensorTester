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
#include "hx711.h"
#include "step_driver.h"
#include "adc.h"
#include "com.h"
#include "vl6180x.h"

#define quanta 1 //(ms)

typedef enum {
	NO_PULL = 0,
	PULL_RE = 1,
	PULL    = 2,
} Pull_Mode;

typedef enum {
	Current_pull = 1,
	Current_re   = 2,
} Current_Mode;

typedef enum {
	Blance_Disable = 0,
	Blance_Enable  = 1,
} Auto_Blance;

typedef enum {
	Res_Off  = 0,
	Res_Auto = 1,
} Resistor_Mode;

//typedef struct {
//	uint32_t position;  // in micromet
//	uint8_t direction; // one clock step
//} STEP_AXIS;

typedef struct {
	Current_Mode  pull;
	Pull_Mode	    current_pull;
	int32_t       offset;
	int32_t       current_force;
	Resistor_Mode res;
	uint32_t      current_resistor;
	int16_t       times;
	uint32_t      current_times;
	int16_t       set_force;
	uint16_t      current_length;
	uint8_t       speed;
	Auto_Blance   blance;
} Enable_Peripheral;

void RCC_Clock_Init (void);
void system_setup(Enable_Peripheral *val);
void system_reset(Enable_Peripheral *val);
void Tach_So_Thuc_7_Int_2_Float(float so, char *buffer);
int Tach_So_Int_Char(uint32_t number, char *buffer, uint8_t len);
int ascii_to_int(uint8_t *str, int len);

void Read_Resistor(void);
void Read_Force(void);
void Control_Motor(void);
void Transmit_To_VCU(void);
void Receive_Form_VCU(void);
void System_Blance(void);

int main (void);

#endif /*__MAIN__*/
