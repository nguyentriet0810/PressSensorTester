#include <os.h>

//tao bien cho systick den tinh quanta time
uint32_t MILLIS_PRESCALER;

//tao mang kieu tcb cho cac thread
tcbType tcbs[NUM_OF_THREADS];
//tao con tro hien tai de biet task nao dang chay
tcbType *currentPt;

//Khoi tao vung nho Stack bang mang 2 chieu
uint32_t TCB_STACK[NUM_OF_THREADS][STACK_SIZE];

//khoi tao so thread co trong os ban dau bang 0
uint32_t current_num_of_thread = 0;

//chuong trinh con khoi tao nhan cua os
void osKernelInit(void){
	//1ms = 7200 xung
	MILLIS_PRESCALER = (BUS_FREQ/1000);
}

void osKernelLaunch(uint8_t quanta) {
	//reset control register
	SysTick->CTRL = 0;
	//reset value
	SysTick->VAL = 0;
	//load
	SysTick->LOAD = (quanta*MILLIS_PRESCALER-1);
	//
	NVIC_SetPriority(SysTick_IRQn, 0);
	//clock source 
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE;
	//enable counter
	SysTick->CTRL |= SysTick_CTRL_ENABLE;
	//enable IRQ
	SysTick->CTRL |= SysTick_CTRL_TICKINT;
	//Callback to the scheduler to run the first thread
	osSchedulerLaunch();
}

//khoi tao vung stack cho moi thread
void osKernelStackInit(int i){
	//16 o nho dau tien danh cho 16 thanh ghi da dung cua he thong
	tcbs[i].stackPt = &TCB_STACK[i][STACK_SIZE-16];
	/*Set bit21 (T-Bit) in PSR to 1 to operate it*/
	/*Thumb Mode*/
	TCB_STACK[i][STACK_SIZE-1] = 1<<24;	/*PSR*/
	/*Skip the PC*/
	//
	/*Dumming Stack Content*/
	TCB_STACK[i][STACK_SIZE-3] = 0xAAAAAAAA;/*R14 (LR)*/
	TCB_STACK[i][STACK_SIZE-4] = 0xAAAAAAAA;/*R12*/
	TCB_STACK[i][STACK_SIZE-5] = 0xAAAAAAAA;/*R3*/
	TCB_STACK[i][STACK_SIZE-6] = 0xAAAAAAAA;/*R2*/
	TCB_STACK[i][STACK_SIZE-7] = 0xAAAAAAAA;/*R1*/
	TCB_STACK[i][STACK_SIZE-8] = 0xAAAAAAAA;/*R0*/
	
	TCB_STACK[i][STACK_SIZE-9] = 0xAAAAAAAA; /*R11*/
	TCB_STACK[i][STACK_SIZE-10] = 0xAAAAAAAA;/*R10*/
	TCB_STACK[i][STACK_SIZE-11] = 0xAAAAAAAA;/*R9*/
	TCB_STACK[i][STACK_SIZE-12] = 0xAAAAAAAA;/*R8*/
	TCB_STACK[i][STACK_SIZE-13] = 0xAAAAAAAA;/*R7*/
	TCB_STACK[i][STACK_SIZE-14] = 0xAAAAAAAA;/*R6*/
	TCB_STACK[i][STACK_SIZE-15] = 0xAAAAAAAA;/*R5*/
	TCB_STACK[i][STACK_SIZE-16] = 0xAAAAAAAA;/*R4*/
}

uint8_t osKernelAdd1Thread(void (*task)(void)){

	__disable_irq();

	osKernelStackInit(current_num_of_thread);

	TCB_STACK[current_num_of_thread][STACK_SIZE-2] = (uint32_t)(task);

	if(!current_num_of_thread){
		tcbs[current_num_of_thread].nextPt = &tcbs[current_num_of_thread];
	} 
	else {
		tcbs[current_num_of_thread-1].nextPt = &tcbs[current_num_of_thread];
		tcbs[current_num_of_thread].nextPt = &tcbs[0];
	}

	currentPt = &tcbs[0];

	current_num_of_thread++;
	
	for(int i=0; i < current_num_of_thread; i++){
		tcbs[i].Sleep = 0;
	}
	
	__enable_irq();
	return 1;
}
//chuong trinh con chuyen task dang chay sang task tiep theo
void osSchedulerRoundRobin(void){
	//tro con tro hien tai toi stackPt cua thread tiep theo
	currentPt = currentPt->nextPt;
	while(currentPt->Sleep>0){
		currentPt = currentPt->nextPt;
	}
}
//chuong trinh con chu dong bat ngat systick de chuyen task
void osThreadYield(void){
	
	SysTick->VAL = 0;
	INTCTRL |= PENDSTSET ;
}
//chuong trinh con khoi tao semaphore thuong
void osSemaphore_Init(uint8_t *semaphore, uint8_t val){
	*semaphore = val;
}
//chuong trinh con give samephore 
void osSemaphore_Give(uint8_t *semaphore){
	//khong cho phep ngat toan cuc
	__disable_irq();
	//cong gia tri cua semaphore len 1
	*semaphore +=1;
	if (* semaphore > 1) {
		*semaphore = 1;
	}
	//bat lai ngat toan cuc
	__enable_irq();
}
//chuong trinh con doi cap semaphore
void osCooperative_Wait(uint8_t *semaphore){
	//ko cho phep ngat
	__disable_irq();
	//cho neu nhu ngoai vi can dung dang duoc chuong trinh khac su dung
	while(*semaphore <= 0){
		//cho phep ngat
		__enable_irq();
		//chu dong goi ham osThreadYield() de chuyen sang task khac tranh lang phi thoi gian
		osThreadYield();
		//ko cho phep ngat
		__disable_irq();
	}
	//neu duoc phep su dung ngoai vi thi tru semaphore di 1
	*semaphore -=1;
	//cho phep ngat toan cuc
	__enable_irq();
}

void osThreadsSleep(uint32_t sleeptime){
	__disable_irq();
	currentPt->Sleep = sleeptime;
	__enable_irq();
	osThreadYield();
}
