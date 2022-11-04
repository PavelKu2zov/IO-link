//**************************************************************************************************
// @Module        TERMINAL
// @Filename      terminal.c
//--------------------------------------------------------------------------------------------------
// @Platform      PLATFORM_NAME
//--------------------------------------------------------------------------------------------------
// @Compatible    COMPATIBLE_PROCESSOR_MODULE
//--------------------------------------------------------------------------------------------------
// @Description   Implementation of the MODULE functionality.
//                 #ifdef DEBUG
//                  TERMINAL_SendMessage( const char *message);
//                 #endif 
//                Abbreviations:
//                  
//                  
//
//                Global (public) functions:
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
// @Date          XX.XX.XXXX
//--------------------------------------------------------------------------------------------------
// @History       Version  Author      Comment
// XX.XX.XXXX     1.0.0    XXX         First release.
//**************************************************************************************************



//**************************************************************************************************
// Project Includes
//**************************************************************************************************

// Native header
#include "terminal.h"
#include "circ_buffer.h"


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

// gpio for terminal
#define TERMINAL_PORT                   GPIOA 
#define TERMINAL_PIN_TRANSMMITER        GPIO_Pin_9 
//#define TERMINAL_PIN_RECEIVE            GPIO_Pin_3


// define hardware UART
#define TERMINAL_UART                   USART1
// Terminal baudrate 
#define TERMINAL_BAUDRATE               (230400)
// define DMA for UART TX
#define TERMINAL_DMA_CHANNEL			DMA1_Channel4


// Size transmit buffer 
#define TERMINAL_SIZE_TX_BUF            (200)


//**************************************************************************************************
// Definitions of static global (private) variables
//**************************************************************************************************

static char TERMINAL_TXBuff[TERMINAL_SIZE_TX_BUF];

static char TERMINAL_TXtempBuff[TERMINAL_SIZE_TX_BUF];

static stCIRCBUF TERMINAL_circbufTX;



//**************************************************************************************************
// Declarations of local (private) functions
//**************************************************************************************************

static void TERMINAL_SetDataInDMA(void);



//**************************************************************************************************
//==================================================================================================
// Definitions of global (public) functions
//==================================================================================================
//**************************************************************************************************



//**************************************************************************************************
// @Function      TERMINAL_Init()
//--------------------------------------------------------------------------------------------------
// @Description   [description...]
//--------------------------------------------------------------------------------------------------
// @Notes
//--------------------------------------------------------------------------------------------------
// @ReturnValue   returnValue - [description...]
//--------------------------------------------------------------------------------------------------
// @Parameters    parameterZero - [description...]
//**************************************************************************************************
STD_RESULT TERMINAL_Init(void)
{
    STD_RESULT nFunction = RESULT_NOT_OK;
	
    // Init GPIO
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = TERMINAL_PIN_TRANSMMITER;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;   
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TERMINAL_PORT, &GPIO_InitStruct);

	// Init TERMINAL_UART
    USART_DeInit(TERMINAL_UART);
    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_Mode = USART_Mode_Tx;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_BaudRate = TERMINAL_BAUDRATE;
    USART_Init(TERMINAL_UART, &USART_InitStruct);
    
	USART_DMACmd(TERMINAL_UART, USART_DMAReq_Tx, ENABLE);
	
    // Enable USART
    USART_Cmd(TERMINAL_UART, ENABLE);
    
    // Init DMA
    DMA_DeInit(TERMINAL_DMA_CHANNEL);
	
	DMA_InitTypeDef DMA_InitStruct;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&TERMINAL_UART->DR;
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&TERMINAL_TXBuff;
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStruct.DMA_BufferSize = TERMINAL_SIZE_TX_BUF;
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(TERMINAL_DMA_CHANNEL, &DMA_InitStruct);
	
	DMA_ITConfig(TERMINAL_DMA_CHANNEL,DMA_IT_TC,ENABLE);
	
	DMA_Cmd(TERMINAL_DMA_CHANNEL, DISABLE);
    
	if ( CIRCBUF_NO_ERR == CIRCBUF_Init(&TERMINAL_circbufTX,TERMINAL_TXBuff,TERMINAL_SIZE_TX_BUF))
	{
		nFunction = RESULT_OK;
	}
	else
	{
		nFunction = RESULT_NOT_OK;
	}
	
	// Interrupt DMA
	NVIC_InitTypeDef NVIC_InitStructure;
    /* Enable the TIM2_IRQn Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	return nFunction;
	
} // end of TERMINAL_Init()



//**************************************************************************************************
// @Function      TERMINAL_SendMessage()
//--------------------------------------------------------------------------------------------------
// @Description   [description...]
//--------------------------------------------------------------------------------------------------
// @Notes
//--------------------------------------------------------------------------------------------------
// @ReturnValue   returnValue - [description...]
//--------------------------------------------------------------------------------------------------
// @Parameters    parameterZero - [description...]
//                parameterOne  - [description...]
//**************************************************************************************************
STD_RESULT TERMINAL_SendMessage( const char *message)
{
    STD_RESULT nFunction = RESULT_NOT_OK;
	
	// Calculate message length
	uint16_t len = 0;
	const char *pdata;
    pdata = message;
	
	while (len < TERMINAL_SIZE_TX_BUF)
	{
		if (0 == *pdata)
		{
			nFunction = RESULT_OK;
            break;
		}
		else
		{
			len++;
			pdata++;
			if (len >= TERMINAL_SIZE_TX_BUF)
			{
				nFunction = RESULT_NOT_OK;
				break;
			}
		}
	}
	
	if ( RESULT_OK == nFunction )
	{
		if ( len <= CIRCBUF_GetFreeSize(&TERMINAL_circbufTX))
		{
			pdata = message; 
			for(int i=0;i<len;i++)
			{
				if (CIRCBUF_NO_ERR == CIRCBUF_PutData(pdata,&TERMINAL_circbufTX))
				{
					pdata++;
				}
				else
				{
					nFunction = RESULT_NOT_OK;
					break;
				}                   
			}
			if ( (TERMINAL_DMA_CHANNEL->CCR & DMA_CCR1_EN) == 0)
			{
				TERMINAL_SetDataInDMA();
			}
		}
		else
		{
			nFunction = RESULT_NOT_OK;
		}
	}
	
	return nFunction;
	
} // end of TERMINAL_SendMessage()



//**************************************************************************************************
// @Function      DMA1_Channel4_IRQHandler()
//--------------------------------------------------------------------------------------------------
// @Description   [description...]
//--------------------------------------------------------------------------------------------------
// @Notes
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
void DMA1_Channel4_IRQHandler(void)
{
    //disable DMA
	DMA_Cmd(TERMINAL_DMA_CHANNEL, DISABLE);
    
    // Does data exist in circle buffer?
	if (CIRCBUF_GetNumberOfItems(&TERMINAL_circbufTX) > 0 )
	{
		TERMINAL_SetDataInDMA();
	}
	else
    {
        ;
    }
	
	DMA_ClearFlag(DMA1_FLAG_TC4);
	
} // end of DMA1_Channel4_IRQHandler()



//**************************************************************************************************
//==================================================================================================
// Definitions of local (private) functions
//==================================================================================================
//**************************************************************************************************



//**************************************************************************************************
// @Function      TERMINAL_SetDataInDMA()
//--------------------------------------------------------------------------------------------------
// @Description   [description...]
//--------------------------------------------------------------------------------------------------
// @Notes
//--------------------------------------------------------------------------------------------------
// @ReturnValue   returnValue - [description...]
//--------------------------------------------------------------------------------------------------
// @Parameters    parameterZero - [description...]
//                parameterOne  - [description...]
//**************************************************************************************************
static void TERMINAL_SetDataInDMA(void)
{
	// get data from circle buffer
	uint16_t numItems = CIRCBUF_GetNumberOfItems(&TERMINAL_circbufTX);
	char data=0;
	
	for (int i=0;i<numItems;i++)
	{
		CIRCBUF_GetData(&data,&TERMINAL_circbufTX);
                              
		TERMINAL_TXtempBuff[i] = data; 
	}
	
	// set adr memory
	TERMINAL_DMA_CHANNEL->CMAR = (uint32_t)TERMINAL_TXtempBuff;
	// set CNDTR
	TERMINAL_DMA_CHANNEL->CNDTR = numItems;
	//enable DMA
	DMA_Cmd(TERMINAL_DMA_CHANNEL, ENABLE);
	
} // end of TERMINAL_SetDataInDMA()


//****************************************** end of file *******************************************
