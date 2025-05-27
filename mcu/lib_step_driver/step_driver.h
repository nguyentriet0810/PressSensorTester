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

#ifndef __STEP_DRIVER__
#define __STEP_DRIVER__

#include <stdint.h>

#include <stm32f10x.h>
#include <stm32f10x_gpio.h>

#include "timer.h"

#define Step_Port    GPIOA
#define Step_Enable  GPIO_Pin_7
#define Micro_Step_1 GPIO_Pin_6
#define Micro_Step_2 GPIO_Pin_5
#define Micro_Step_3 GPIO_Pin_4
#define Step_Reset   GPIO_Pin_3
#define Step_Sleep   GPIO_Pin_2
#define Step_Pul     GPIO_Pin_1
#define Step_Dir     GPIO_Pin_0

void Step_Driver_Init(void);

#endif /*__STEP_DRIVER__*/
