#include "stm32f10x.h"
#include "Init.h"



void Init()
{	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);//72 MHz clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//36 MHz clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);//36 MHz clock;PL_TIMER
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    
}