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

#ifndef __HX711__
#define __HX711__

#include <stdio.h>
#include <stdint.h>

#include <stm32f10x.h>
#include <stm32f10x_gpio.h>

#include "timer.h"

#define DT_PIN GPIO_Pin_3
#define DT_PORT GPIOB
#define SCK_PIN GPIO_Pin_4
#define SCK_PORT GPIOB

#define tare                8037224U 
#define knownOriginal       11430000U
#define knownHX711          410262U
#define knownOriginalForce  0U

typedef struct {
	int32_t weight;
	int32_t offset;
	int32_t force;
} LoadCell_typedef;

extern LoadCell_typedef data;

void Force_Sensor_Init(void);
int32_t getHX711(void);
int32_t weigh(void);
int32_t getForce(int32_t offset);

#endif /*__HX711__*/
