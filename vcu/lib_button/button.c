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
#include "button.h"

uint16_t X_Coordinate = 20, Y_Coordinate = 195;

void action_nothing(void) {
	return;
}
void action_start(void) {

}

void action_top(uint16_t *coordinate) {
	*coordinate -= 35;
}

void action_bottom(uint16_t *coordinate) {
	*coordinate += 35;
}

void action_plus(uint16_t *value) {
	*value += 1;
}

void action_sub(uint16_t *value) {
	*value -= 1;
}

const Button_Choose_Page_Action page_actions[] = {  
	{125, Page_Start},
	{160, Page_Settings},
	{195, Page_Options},
};

const Button_Top_Action top_actions[] = {
	{Page_Defaut  , 125, action_top},
	{Page_Settings,  55, action_top},
	{Page_Options ,  55, action_top},
};

const Button_Bottom_Action bottom_actions[] = {
	{Page_Defaut  , 195, action_bottom},
	{Page_Settings, 160, action_bottom},
	{Page_Options , 160, action_bottom},
};

const Button_Plus_Action plus_actions[] = {
	{Page_Settings, 125, action_plus},
	{Page_Settings, 195, action_plus},
};

const Button_Sub_Action sub_actions[] = {
	{Page_Settings, 125, action_sub},
	{Page_Settings, 195, action_sub},
};

void Button_GPIO_Init(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  // B?t clock GPIOB

  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;  // Ch? d? input pull-up
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
  GPIO_Init(GPIOB, &GPIO_InitStruct);  // C?u hình PB4 -> PB7
}

void test_button_Init(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);  

  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
  GPIO_Init(GPIOC, &GPIO_InitStruct); 
}



