//**************************************************************************************************
// @Module        PL
// @Filename      PL_drv.c
//--------------------------------------------------------------------------------------------------
// @Platform      stm32f103
//--------------------------------------------------------------------------------------------------
// @Compatible    
//--------------------------------------------------------------------------------------------------
// @Description   Implementation of the PL functionality.
//
//
//                Abbreviations:
//                  PL - Physical layer
//
//
//                 Global (public) functions:
//
//
//
//                Local (private) functions:
//
//
//
//--------------------------------------------------------------------------------------------------
// @Version       1.0.0
//--------------------------------------------------------------------------------------------------
// @Date          30.08.2021
//--------------------------------------------------------------------------------------------------
// @History       Version  Author      Comment
// XX.XX.XXXX     1.0.0    KPS         First release.
//**************************************************************************************************



//**************************************************************************************************
// Project Includes
//**************************************************************************************************

// Native header
#include "PL_drv.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"


//**************************************************************************************************
// Verification of the imported configuration parameters
//**************************************************************************************************

// None.


//**************************************************************************************************
// Definitions of global (public) variables
//**************************************************************************************************

// None.


//**************************************************************************************************
// Declarations of local (private) data types
//**************************************************************************************************

// None.


//**************************************************************************************************
// Definitions of local (private) constants
//**************************************************************************************************

// gpio for Io-Link

#defein PL_PORT_IO_LINK        GPIOA 
#define PL_PIN_TRANSMMITER     GPIO_Pin_ 
#define PL_PIN_RECEIVE         GPIO_Pin_ 

#define PL_USART_IO_LINK       USART2

// Baudrate Io-Link interface
// COM1 4800    bit/s
// COM2 38400   bit/s
// COM3 230400  bit/s 
#define PL_BAUDRATE_COM1       (4800)              
#define PL_BAUDRATE_COM2       (38400)              
#define PL_BAUDRATE_COM3       (230400)

// Time settings in us
#define PL_TWU				   (80)
#define PL_TREN				   (500)

// Number hardware timer
#define PL_TIMER		      (TIM5)
#define PL_TIMER_PERIOD		  ()
#define PL_TIMER_PRESCALER    ()
              
//**************************************************************************************************
// Definitions of static global (private) variables
//**************************************************************************************************

// None.


//**************************************************************************************************
// Declarations of local (private) functions
//**************************************************************************************************

// None.


//**************************************************************************************************
//==================================================================================================
// Definitions of global (public) functions
//==================================================================================================
//**************************************************************************************************

//**************************************************************************************************
// @Function      PL_Init()
//--------------------------------------------------------------------------------------------------
// @Description   Init PL
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
void PL_Init(void)
{
    TIM_DeInit(PL_Timer);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseInitStruct.TIM_Period = PL_TIMER_PERIOD;
	TIM_TimeBaseInitStruct.TIM_Prescaler = PL_TIMER_PRESCALER;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x0000;
	TIM_TimeBaseInit(PL_Timer, &TIM_TimeBaseInitStruct);
	
}// end of PL_Init()



//**************************************************************************************************
// @Function      PL_Task()
//--------------------------------------------------------------------------------------------------
// @Description   task of Physical layer
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
void PL_Task(void *pvParameters)
{
	for(;;)
	{
		//Wait SemaphorePL
		xSemaphoreTake( xSemaphorePL, portMAX_DELAY );
		
		
	}
}// end of PL_Task()



//**************************************************************************************************
//==================================================================================================
// Definitions of local (private) functions
//==================================================================================================
//**************************************************************************************************

//**************************************************************************************************
// @Function      PL_WakeUp
//--------------------------------------------------------------------------------------------------
// @Description   The PL-WakeUp service initiates or indicates a specific sequence which prepares 
//			      the Physical Layer to send and receive communication requests.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
void PL_WakeUP( void )
{
	// Set mode SIO
	PL_SetMode(D);
	
	if (Bit_SET == GPIO_ReadOutputDataBit(PL_PORT_IO_LINK, PL_PIN_TRANSMMITER))
	{
		GPIO_WriteBit(PL_PORT_IO_LINK, PL_PIN_TRANSMMITER, Bit_RESET);
	}
	else if (Bit_RESET == GPIO_ReadOutputDataBit(PL_PORT_IO_LINK, PL_PIN_TRANSMMITER))
	{
		GPIO_WriteBit(PL_PORT_IO_LINK, PL_PIN_TRANSMMITER, Bit_RES);
		PL_Delay(PL_TWU);
		GPIO_WriteBit(PL_PORT_IO_LINK, PL_PIN_TRANSMMITER, Bit_RESET);
	}
}
// end of PL_WakeUP()



//**************************************************************************************************
// @Function      PL_SetMode
//--------------------------------------------------------------------------------------------------
// @Description   SetMode service is used to setup the electrical characteristics and configurations 
//				  of the Physical Layer.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    mode - INACTIVE,DI,DO,COM1,COM2,COM3
//**************************************************************************************************
void PL_SetMode(PL_TARGET_MODE mode);
{
    if (INACTIVE == mode)
    {
        GPIO_InitTypeDef GPIO_InitStruct;
        GPIO_InitStruct.GPIO_Pin = PL_PIN_TRANSMMITER | PL_PIN_RECEIVE;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;   
        GPIO_Init(PL_PORT_IO_LINK, &GPIO_InitStruct);
    }
	else if (DI == mode)
	{
		GPIO_InitTypeDef GPIO_InitStruct;
        GPIO_InitStruct.GPIO_Pin = PL_PIN_RECEIVE;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;   
        GPIO_Init(PL_PORT_IO_LINK, &GPIO_InitStruct);
		
        GPIO_InitStruct.GPIO_Pin = PL_PIN_TRANSMMITER;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
        GPIO_Init(PL_PORT_IO_LINK, &GPIO_InitStruct);
	}
	else if (D == mode)
	{
		GPIO_InitTypeDef GPIO_InitStruct;
        GPIO_InitStruct.GPIO_Pin = PL_PIN_RECEIVE;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;   
        GPIO_Init(PL_PORT_IO_LINK, &GPIO_InitStruct);
				
        GPIO_InitStruct.GPIO_Pin = PL_PIN_TRANSMMITER;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; 
        GPIO_Init(PL_PORT_IO_LINK, &GPIO_InitStruct);
	}
	else if ((COM1 == mode) || (COM2 == mode) || (COM3 == mode))
	{
		GPIO_InitTypeDef GPIO_InitStruct;
        GPIO_InitStruct.GPIO_Pin = PL_PIN_RECEIVE;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;   
        GPIO_Init(PL_PORT_IO_LINK, &GPIO_InitStruct);
		
		GPIO_InitStruct.GPIO_Pin = PL_PIN_TRANSMMITER;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(PL_PORT_IO_LINK, &GPIO_InitStruct);
		
		
		USART_DeInit(USART_IO_LINK);
		USART_InitTypeDef USART_InitStruct;
		USART_InitStruct.USART_WordLength = USART_WordLength_8b;
		USART_InitStruct.USART_StopBits = USART_StopBits_1;
		USART_InitStruct.USART_Parity = USART_Parity_Even;
		USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
		
		if (COM1 == mode)
		{
			USART_InitStruct.USART_BaudRate = PL_BAUDRATE_COM1;
		}
		else if (COM2 == mode)
		{
			USART_InitStruct.USART_BaudRate = PL_BAUDRATE_COM2;
		}
		else if (COM3 == mode)
		{
			USART_InitStruct.USART_BaudRate = PL_BAUDRATE_COM3;
		}
		
		USART_Init(USART_IO_LINK, &USART_InitStruct);
	}
	
}
// end of PL_SetMode()


//**************************************************************************************************
// @Function      PL_Transfer
//--------------------------------------------------------------------------------------------------
// @Description   The PL-Transfer service is used to exchange the SDCI data between Data Link Layer 
//				  and  Physical Layer
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
void PL_Transfer( void )
{
	
}
// end of PL_Transfer()



//**************************************************************************************************
// @Function      PL_Delay
//--------------------------------------------------------------------------------------------------
// @Description   Delay for PL. It bases on PL_TIMER
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    time in us
//**************************************************************************************************
void PL_Delay( uint16_t time )
{
	TIM_Cmd(PL_Timer, DISABLE);
	TIM_ClearFlag(PL_Timer, TIM_FLAG_Update);
	TIM_SetAutoreload(PL_TIMER, time*PL_K);
	//wait
	TIM_Cmd(PL_Timer, ENABLE);
	while(RESET != TIM_GetFlagStatus(PL_Timer, TIM_FLAG_Update));
	TIM_Cmd(PL_Timer, DISABLE);
}
// end of PL_Delay()




//****************************************** end of file *******************************************
