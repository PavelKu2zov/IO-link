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

// Tbit - измеряется в тиках TIM5, source 36MHz
#define PL_Tbit_COM3           (156)
#define PL_Tbit_COM2           (937)
#define PL_Tbit_COM1           (7500)

#define PL_Tdmt_K              (30)

#define PL_Tdmt_COM3           (PL_Tdmt_K*PL_Tbit_COM3)
#define PL_Tdmt_COM2           (PL_Tdmt_K*PL_Tbit_COM2)
#define PL_Tdmt_COM1           (PL_Tdmt_K*PL_Tbit_COM1)

// Number hardware timer
#define PL_TIMER		      (TIM5)
#define PL_TIMER_PERIOD		  ()
#define PL_TIMER_PRESCALER    ()
 

#define PL_SIZE_BUFF_DATA_TO_TR     (64)
              
//**************************************************************************************************
// Definitions of static global (private) variables
//**************************************************************************************************

PL_TARGET_MODE PL_TargetMode;
uint8_t dataToTransfer[PL_SIZE_BUFF_DATA_TO_TR]; 


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
    portBASE_TYPE xStatus;
    PL_MES_QUEUE PL_receiveMes;
	for(;;)
	{
		//Wait message
		xStatus = xQueueReceive( xPL_Queue, &PL_receiveMes, portMAX_DELAY );
		
        if ( pdPASS == xStatus )
        {
            if (PL_WAKE_UP == PL_receiveMes)
            {
                PL_WakeUP();
            }
            else if (PL_TRANSFER_COM1 == PL_receiveMes)
            {
                PL_SetMode(COM1);
                PL_Transfer();
            }
            else if (PL_TRANSFER_COM2 == PL_receiveMes)
            {
                PL_SetMode(COM2);
                PL_Transfer();
            }
            else if (PL_TRANSFER_COM3 == PL_receiveMes)
            {
                PL_SetMode(COM3);
                PL_Transfer();
            }
            else if (PL_SET_MODE== PL_receiveMes)
            {
                ;
            }
        }
		
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
    
    // Wait Tdmt_COM3
    PL_Delay( PL_Tdmt_COM3 );
    
    // Set mode COM3
    PL_SetMode(COM3);
    
    // формируем пакет TYPE_0
    dataToTransfer[0] = 0xA2;
    dataToTransfer[1] = PL_CalCKT( dataToTransfer, 1,0 );
    
    // Send data
    PL_Transfer(dataToTransfer,2);
    
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
	PL_TargetMode = mode;
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
void PL_Transfer( uint8_t* data,uint16_t size )
{
    for (uint32_t i=0;i<size;i++)
    {
        while(RESET == USART_GetFlagStatus(PL_USART_IO_LINK, USART_FLAG_TC));
        USART_SendData(PL_USART_IO_LINK, *data);
        data++;
    }
}
// end of PL_Transfer()



//**************************************************************************************************
// @Function      PL_Receive
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
void PL_Receive( void )
{
    for (uint32_t i=0;i<size;i++)
    {
        while(RESET == USART_GetFlagStatus(PL_USART_IO_LINK, USART_FLAG_TC));
        USART_SendData(PL_USART_IO_LINK, *data);
        data++;
    }
}
// end of PL_Receive()



//**************************************************************************************************
// @Function      PL_Delay
//--------------------------------------------------------------------------------------------------
// @Description   Delay for PL. It bases on PL_TIMER
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    time in ticks of PL_Timer
//**************************************************************************************************
void PL_Delay( uint16_t time )
{
	TIM_Cmd(PL_Timer, DISABLE);
	TIM_ClearFlag(PL_Timer, TIM_FLAG_Update);
	TIM_SetAutoreload(PL_TIMER, time);
	//wait
	TIM_Cmd(PL_Timer, ENABLE);
	while(RESET != TIM_GetFlagStatus(PL_Timer, TIM_FLAG_Update));
	TIM_Cmd(PL_Timer, DISABLE);
}
// end of PL_Delay()



//**************************************************************************************************
// @Function      PL_CalCKT
//--------------------------------------------------------------------------------------------------
// @Description   Function calculates check Sum adn then compresses to 6 bit
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    data - pointer to data
//                size - size of data 
//                typeMseq - type of M-sequence   
//**************************************************************************************************
void uint8_t PL_CalCKT( uint8_t* data, const uint16_t size, uint8_t typeMseq )
{
    uint8_t checkSum = 0x52;
    uint8_t checkSum6Bits = 0;
    uint8_t CKT = 0;
    
    for (uint32_t i=0;i<size;i++)
    {
        checkSum = checkSum ^ *data;
        data++;    
    }
    
    // comprassing to 6 bits
    uint8_t tm = checkSum >> 1;
    tm = tm ^ checkSum;
    checkSum6Bits |= tm & 0x1;
    checkSum6Bits |= (tm >> 1) & 0x2;
    checkSum6Bits |= (tm >> 2) & 0x4;
    checkSum6Bits |= (tm >> 3) & 0x8;
    checkSum6Bits |= ((checkSum ^ (checkSum>>2) ^ (checkSum<<2) ^ (checkSum<<4))&0x30;
    
    CKT = checkSum6Bits | ((typeMseq<<6)&0xc0);
    return CKT;
}
// end of PL_CalCKT()



//****************************************** end of file *******************************************
