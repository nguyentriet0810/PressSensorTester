#ifndef __MAIN_H__
#define __MAIN_H__

#include <stm32f10x.h>

#include <stdio.h>
#include <stdint.h>

#include "oskernel.h"
#include "button.h"

#define QUANTA 1
void task1 (void);
void task2 (void);
void task3 (void);

void RCC_Init(void);
int main(void);

#endif /*__MAIN_H__*/
