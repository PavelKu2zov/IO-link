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
//#include "main.h"


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

typedef enum 
{
	RESULT_NOT_OK=0,
	RESULT_OK
}RESULT_FUN;


// Services of Physical layer 
typedef enum PL_TIMER_STATE_enum
{
    TIMEOUT = 0,
    RUN,
	STOP
}PL_TIMER_STATE;

// Target mode
typedef enum PL_TARGET_MODE_enum
{
	INACTIVE=0,
	DI,
	D,
	COM1,
	COM2,
	COM3
}PL_TARGET_MODE;

//**************************************************************************************************
// Definitions of local (private) constants
//**************************************************************************************************

// gpio for Io-Link
#define PL_PORT_IO_LINK        GPIOA 
#define PL_PIN_TRANSMMITER     GPIO_Pin_2 
#define PL_PIN_RECEIVE         GPIO_Pin_3

#define PL_USART_IO_LINK       USART2

// Baudrate Io-Link interface
// COM1 4800    bit/s
// COM2 38400   bit/s
// COM3 230400  bit/s 
#define PL_BAUDRATE_COM1       (4800)              
#define PL_BAUDRATE_COM2       (38400)              
#define PL_BAUDRATE_COM3       (230400)

// Times

// Duration of Master's wake-up current pulse 75..85 us
#define PL_TWU				   (80)

// Receive enable delay max 500 us
#define PL_TREN				   (500)

// Tbit - time of one bit in us 
#define PL_Tbit_COM3           (4)//4,34 us
#define PL_Tbit_COM2           (26)//26,04 us
#define PL_Tbit_COM1           (208)//208,33 us

#define PL_Tdmt_K              (33)// 27...37

// Master message delay
// min,max 117,18..159,47 us
#define PL_Tdmt_COM3           (PL_Tdmt_K*PL_Tbit_COM3)//132 us

// min,max 703,08..963,48 us
#define PL_Tdmt_COM2           (PL_Tdmt_K*PL_Tbit_COM2)//858 us

// min,max 5624,91..7708,21 us
#define PL_Tdmt_COM1           (PL_Tdmt_K*PL_Tbit_COM1)//6864 us

//Wake-up retry delay 30..50 ms
#define PL_Tdwu				   (40000)

//Wake-up retry count 
#define PL_Nwu				  (3)	

//Device detection time 0,5...1 s
#define PL_Tds				   ()	

// Number hardware timer tick = 1 us
#define PL_TIMER		      (TIM2)
#define PL_TIMER_PERIOD		  (0xffff)
#define PL_TIMER_PRESCALER    (72)

#define PL_SIZE_BUFF_DATA_TO_TR     (64)
#define PL_SIZE_BUFF_DATA_REC       (64)
              
//**************************************************************************************************
// Definitions of static global (private) variables
//**************************************************************************************************

PL_TARGET_MODE PL_TargetMode;
uint8_t dataToTransfer[PL_SIZE_BUFF_DATA_TO_TR]; 
uint8_t dataReceive[PL_SIZE_BUFF_DATA_REC]; 

PL_TIMER_STATE PL_timerState = STOP;


//**************************************************************************************************
// Declarations of local (private) functions
//**************************************************************************************************

// Init PL
static void PL_Init(void);

// Send Wake Up sequences
static RESULT_FUN PL_WakeUP(void);

// Set mode PL
static void PL_SetMode(PL_TARGET_MODE mode);

// Transfer message 
static void PL_Transfer( uint8_t* data,uint16_t size );

// receive message
static RESULT_FUN PL_Receive( uint8_t* data, uint16_t size, uint16_t timeOut );

// start timer
static void PL_StartTimer( uint16_t time );

// Get status timer
static void PL_GetStatusTimer( void );

// calculate CKT
static uint8_t PL_CalCKT( uint8_t* data, const uint16_t size, uint8_t typeMseq );


//**************************************************************************************************
//==================================================================================================
// Definitions of global (public) functions
//==================================================================================================
//**************************************************************************************************



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
    //PL_MES_QUEUE PL_receiveMes;
    PL_Init();
	for(;;)
	{
		//Wait message
		// xStatus = xQueueReceive( xPL_Queue, &PL_receiveMes, portMAX_DELAY );
		
        // if ( pdPASS == xStatus )
        // {
            // if (PL_WAKE_UP == PL_receiveMes)
            // {
				// for (int32_t i=0;i<PL_Nwu;i++)
				// {
					// if (RESULT_OK == PL_WakeUP())
					// {
						// break;
					// }
				// }
				
            // }
 
        // }
		
		for (int32_t i=0;i<PL_Nwu;i++)
		{
			if (RESULT_OK == PL_WakeUP())
			{
				break;
			}
		}
		
		vTaskDelay( 1 / portTICK_RATE_MS );
		
	}
}// end of PL_Task()



//**************************************************************************************************
//==================================================================================================
// Definitions of local (private) functions
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
static void PL_Init(void)
{
    TIM_DeInit(PL_TIMER);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseInitStruct.TIM_Period = PL_TIMER_PERIOD;
	TIM_TimeBaseInitStruct.TIM_Prescaler = PL_TIMER_PRESCALER;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x0000;
	TIM_TimeBaseInit(PL_TIMER, &TIM_TimeBaseInitStruct);
	//TIM_Cmd(PL_TIMER, ENABLE);
}// end of PL_Init()



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
static RESULT_FUN PL_WakeUP( void )
{
	RESULT_FUN result = RESULT_NOT_OK;
	// Set mode SIO
	PL_SetMode(D);
	
	// Make impuls WURQ
	if (Bit_SET == GPIO_ReadOutputDataBit(PL_PORT_IO_LINK, PL_PIN_TRANSMMITER))
	{
        GPIO_ResetBits(PL_PORT_IO_LINK, PL_PIN_TRANSMMITER);
	}
	else if (Bit_RESET == GPIO_ReadOutputDataBit(PL_PORT_IO_LINK, PL_PIN_TRANSMMITER))
	{
		GPIO_SetBits(PL_PORT_IO_LINK, PL_PIN_TRANSMMITER);
		PL_StartTimer( PL_TWU );
		while( RUN == PL_timerState )
		{
			PL_GetStatusTimer();
		}
		GPIO_ResetBits(PL_PORT_IO_LINK, PL_PIN_TRANSMMITER);
		
		PL_StartTimer( PL_TREN );
		while( RUN == PL_timerState )
		{
			PL_GetStatusTimer();
		}
	}
    
	// Send test messages
	
    // Wait Tdmt_COM3
	PL_StartTimer( PL_Tdmt_COM3 );
	while( RUN == PL_timerState )
	{
		PL_GetStatusTimer();
	}
    
    // Set mode COM3
    PL_SetMode(COM3); 
    
    // формируем пакет TYPE_0
    dataToTransfer[0] = 0xA2;
    dataToTransfer[1] = PL_CalCKT( dataToTransfer, 1,0 );
    
    // Send data with mode COM3
    PL_Transfer( dataToTransfer,2 );
	if ( RESULT_OK == PL_Receive( dataReceive,2,PL_Tdmt_COM2 ))
	{
		result = RESULT_OK;
	}
	else
	{
		// Set mode COM2
		PL_SetMode(COM2);
		
		// Send data with mode COM2
		PL_Transfer( dataToTransfer,2 );
		if ( RESULT_OK == PL_Receive( dataReceive,2,PL_Tdmt_COM1 ))
		{
			result = RESULT_OK;
		}
		else
		{
			// Set mode COM1
			PL_SetMode(COM1);
			
			// Send data with mode COM1
			PL_Transfer( dataToTransfer,2 );
			if ( RESULT_OK == PL_Receive( dataReceive,2,PL_Tdwu ))
			{
				result = RESULT_OK;
			}
		}
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
static void PL_SetMode(PL_TARGET_MODE mode)
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
		
		
		USART_DeInit(PL_USART_IO_LINK);
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
		
		USART_Init(PL_USART_IO_LINK, &USART_InitStruct);
		USART_Cmd(PL_USART_IO_LINK, ENABLE);
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
static void PL_Transfer( uint8_t* data,uint16_t size )
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
// @Parameters    timeout - in Tbit
//				  size - size to receive	
//**************************************************************************************************
static RESULT_FUN PL_Receive( uint8_t* data, uint16_t size, uint16_t timeOut )
{
	RESULT_FUN result = RESULT_NOT_OK;
	
	PL_StartTimer( timeOut );
	if (size < PL_SIZE_BUFF_DATA_REC)
	{
		while((size != 0) && (PL_timerState != TIMEOUT)) 
		{
			if (SET == USART_GetFlagStatus(PL_USART_IO_LINK, USART_FLAG_RXNE))
			{
				*data = USART_ReceiveData(PL_USART_IO_LINK);
				data++;
				size--;
			}
            PL_GetStatusTimer();
		}
	}
	else
	{
		result = RESULT_NOT_OK;
	}
	
	if (size == 0)
	{
		result = RESULT_OK;
	}
	else
	{
		result = RESULT_NOT_OK;
	}
	
	return result;
}
// end of PL_Receive()



//**************************************************************************************************
// @Function      PL_StartTimer
//--------------------------------------------------------------------------------------------------
// @Description   Timer for PL.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    time in us. 1..0xffff
//**************************************************************************************************
static void PL_StartTimer( uint16_t time )
{
	TIM_Cmd(PL_TIMER, DISABLE);
	TIM_ClearFlag(PL_TIMER, TIM_FLAG_Update);
	TIM_SetAutoreload(PL_TIMER, time);
	//wait
	TIM_Cmd(PL_TIMER, ENABLE);
	PL_timerState = RUN;
}
// end of PL_StartTimer()



//**************************************************************************************************
// @Function      PL_GetStatusTimer
//--------------------------------------------------------------------------------------------------
// @Description   Delay for PL. It bases on PL_TIMER
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
static void PL_GetStatusTimer( void )
{
	if(SET == TIM_GetFlagStatus(PL_TIMER, TIM_FLAG_Update))
    {
        PL_timerState = TIMEOUT;
        TIM_Cmd(PL_TIMER, DISABLE);
    }
}
// end of PL_GetStatusTimer()



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
static uint8_t PL_CalCKT( uint8_t* data, const uint16_t size, uint8_t typeMseq )
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
    checkSum6Bits |= (checkSum ^ (checkSum>>2) ^ (checkSum<<2) ^ (checkSum<<4))&0x30;
    
    CKT = checkSum6Bits | ((typeMseq<<6)&0xc0);
    return CKT;
}
// end of PL_CalCKT()



//****************************************** end of file *******************************************
