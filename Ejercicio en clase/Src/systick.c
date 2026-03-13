#include "stm32f4xx.h"

#define SYSTICK_LOAD_VAL	16000
#define CTRL_ENABLE	(1U << 0)
#define CTRL_CLKSRC	(1U << 2)
#define CTRL_COUNTFLAG	(1U << 16)

void systickDelayMs(int delay){
	SysTick->LOAD = SYSTICK_LOAD_VAL;
	SysTick->LOAD = 0;
	SysTick->CTRL = CTRL_ENABLE | CTRL_CLKSRC;

	for (int i=0; i<delay; i++){
		while((SysTick->CTRL & CTRL_COUNTFLAG)==0) {}
	}

	sYStICK->CTRL = 0;

}
