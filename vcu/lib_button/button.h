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
#ifndef __BUTTON__
#define __BUTTON__

#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_exti.h>

#define Button_Port   GPIOB
#define Button_Hold   GPIO_Pin_3  
#define Button_Choose GPIO_Pin_4
#define Button_Top    GPIO_Pin_5
#define Button_Bottom GPIO_Pin_6
#define Button_Plus   GPIO_Pin_7
#define Button_Sub    GPIO_Pin_8

typedef enum {
	Page_Defaut   = 0,
	Page_Start    = 1,
	Page_Settings = 2,
	Page_Options  = 3,
	Page_Alarm    = 4,
} Page_Type;

typedef struct {
  uint8_t y_coord;              // Current Y_coord value
  Page_Type next_page;          // Page      
} Button_Choose_Page_Action;

//typedef struct {
//  uint8_t y_coord;              // Current Y_coord value      
//	void (*action)(uint8_t *);         // action function
//} Button_Choose_Options_Action;

typedef struct {
  Page_Type current_page;       // Page     
	uint8_t y_coord_top_limit;
	void (*action)(uint16_t *);   // action function
} Button_Top_Action;

typedef struct {
  Page_Type current_page;       // Page     
	uint8_t y_coord_bot_limit;
	void (*action)(uint16_t *);   // action function
} Button_Bottom_Action;

typedef struct {
	uint8_t y_coord;              // Current Y_coord value
  Page_Type current_page;       // Page     
	void (*action)(uint16_t *);         // action function
} Button_Plus_Action;

typedef struct {
	uint8_t y_coord;              // Current Y_coord value
  Page_Type current_page;       // Page     
	void (*action)(uint16_t *);         // action function
} Button_Sub_Action;

extern uint16_t X_Coordinate, Y_Coordinate;

extern const Button_Choose_Page_Action page_actions[];
extern const Button_Top_Action         top_actions[];
extern const Button_Bottom_Action      bottom_actions[];
extern const Button_Plus_Action        plus_actions[];
extern const Button_Sub_Action         sub_actions[];

void action_nothing(void);
void action_start(void);
void action_top(uint16_t *coordinate);
void action_bottom(uint16_t *coordinate);
void action_plus(uint16_t *value);
void action_sub(uint16_t *value);

void Button_GPIO_Init(void);
void test_button_Init(void);

#endif /*__BUTTON__*/
