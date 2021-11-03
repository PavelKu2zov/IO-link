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

//    GPIO_InitTypeDef GPIO_InitStruct;
//    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
//    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
//    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;   
//    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    //RCC_MCOConfig(RCC_MCO_SYSCLK);
}