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
#include "Physical_layer.h"
#include "IO_LINK.h"
#include "software_timer.h"
#include "terminal.h"
// circle buffer
#include "circ_buffer.h"
#include "intrinsics.h"


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
#define PL_PORT_IO_LINK        GPIOA 
#define PL_PIN_TRANSMMITER     GPIO_Pin_2 
#define PL_PIN_RECEIVE         GPIO_Pin_3

#define PL_PIN_EN_CQ           GPIO_Pin_6

#define PL_USART_IO_LINK       USART2
#define CR1_UE_Set             ((uint16_t)0x2000)  /*!< USART Enable Mask */
#define CR1_UE_Reset            ((uint16_t)0xDFFF)  /*!< USART Disable Mask */

// Baudrate Io-Link interface
// COM1 4800    bit/s
// COM2 38400   bit/s
// COM3 230400  bit/s 
#define PL_BAUDRATE_COM1       (4800)              
#define PL_BAUDRATE_COM2       (38400)              
#define PL_BAUDRATE_COM3       (230400)//(326797)//

#define PL_BAUDRATE_COM1_BRR   (0x1D4C) 
#define PL_BAUDRATE_COM2_BRR   (0x3A9) 
#define PL_BAUDRATE_COM3_BRR   (0x9C)//(0x6E)//( 

#define PL_SIZE_BUFF_DATA_TO_TR     (64)
#define PL_SIZE_BUFF_DATA_REC       (64)
              
//**************************************************************************************************
// Definitions of static global (private) variables
//**************************************************************************************************

uint8_t dataToTransfer[PL_SIZE_BUFF_DATA_TO_TR]; 
uint8_t dataReceive[PL_SIZE_BUFF_DATA_REC]; 

// circ_buffer
static stCIRCBUF PL_CIRCBUF;

// buff for circle buf
static uint8_t PL_buff[PL_SIZE_BUFF];

// status circ_buffer
static enCIRCBUF_ERR PL_StatusCircBuf;

// software timer Tdmt
static uint8_t nTimerHandler_PL = 0;

// Current mode
static uint8_t PL_TargetMode;

static U8 nmbrItemsTest=0;

static U8 stateCom = COM_STATE_LOST;


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
STD_RESULT PL_Init(void)
{
    STD_RESULT nFuncResult = RESULT_OK;
    
    // Create timer Tdmt
    if (RESULT_OK == SOFTTIMER_Create(&nTimerHandler_PL))
    {
         // Init circle buffer
        PL_StatusCircBuf = CIRCBUF_Init(&PL_CIRCBUF,PL_buff,PL_SIZE_BUFF);
        if (CIRCBUF_NO_ERR == PL_StatusCircBuf)
        {    
            // set PIN_EN_CQ in output
            GPIO_InitTypeDef GPIO_InitStruct;
            GPIO_InitStruct.GPIO_Pin = PL_PIN_EN_CQ;
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
            GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;   
            GPIO_Init(PL_PORT_IO_LINK, &GPIO_InitStruct);
            
          
            // set trnsmiter in FLOATING
            GPIO_InitStruct.GPIO_Pin = PL_PIN_RECEIVE;
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
            GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;   
            GPIO_Init(PL_PORT_IO_LINK, &GPIO_InitStruct);
            
            USART_DeInit(PL_USART_IO_LINK);
            USART_InitTypeDef USART_InitStruct;
            USART_InitStruct.USART_WordLength = USART_WordLength_9b;
            USART_InitStruct.USART_StopBits = USART_StopBits_1;
            USART_InitStruct.USART_Parity = USART_Parity_Even;
            USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
            USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
            USART_InitStruct.USART_BaudRate = PL_BAUDRATE_COM3;
            USART_Init(PL_USART_IO_LINK, &USART_InitStruct);
            
            // Enable Interrupt PL_USART_IO_LINK RX
            NVIC_InitTypeDef NVIC_InitStructure;
            NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
            
            // Enable USART
            USART_Cmd(PL_USART_IO_LINK, ENABLE);
            PL_TargetMode = PL_INACTIVE;
            #ifdef DEBUG
                TERMINAL_SendMessage( "Physical layer was initialized\n\r");
            #endif 
            
            nFuncResult = RESULT_OK;
        }
        else
        {
           nFuncResult = RESULT_NOT_OK;
        }
    }
    else
    {
        nFuncResult = RESULT_NOT_OK;
    }
    
    return nFuncResult;

}// end of PL_Init()



//**************************************************************************************************
// @Function      PL_WakeUp
//--------------------------------------------------------------------------------------------------
// @Description   The PL-WakeUp service initiates or indicates a specific sequence which prepares 
//			      the Physical Layer to send and receive communication requests.
//--------------------------------------------------------------------------------------------------
// @Notes        NOTE! We control output signal after IO-Link driver!
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
STD_RESULT PL_WakeUP( void )
{
	STD_RESULT nFuncResult = RESULT_OK;
    
	// Set mode PL_DI to read level input 
	PL_SetMode(PL_DI);
	
    // Disable CQ output
    GPIO_SetBits(PL_PORT_IO_LINK, PL_PIN_EN_CQ);
    
    // Read input level
    // Bit_SET means that C/Q is LOW, will we make impuls _/-\_
    if (Bit_SET == GPIO_ReadInputDataBit(PL_PORT_IO_LINK, PL_PIN_RECEIVE))
    {
        // Set mode PL_D to set level
        PL_SetMode(PL_D);
        
        // Enable CQ output
        GPIO_ResetBits(PL_PORT_IO_LINK, PL_PIN_EN_CQ);
        
        // Set C/Q in high
        GPIO_ResetBits(PL_PORT_IO_LINK, PL_PIN_TRANSMMITER);
        
        // wait Twu
        SOFTTIMER_Delay(nTimerHandler_PL,Twu);
        
        // Set C/Q in low
        GPIO_SetBits(PL_PORT_IO_LINK, PL_PIN_TRANSMMITER);
    }
    else//  or make impuls  -\____
    {
        // Set mode PL_D to set level
        PL_SetMode(PL_D);
        
        // Enable CQ output
        GPIO_ResetBits(PL_PORT_IO_LINK, PL_PIN_EN_CQ);
        
        // Set C/Q in low
        GPIO_SetBits(PL_PORT_IO_LINK, PL_PIN_TRANSMMITER);
        
        // wait Twu
        SOFTTIMER_Delay(nTimerHandler_PL,Twu);
    }
    
    // Disable CQ output
    GPIO_SetBits(PL_PORT_IO_LINK, PL_PIN_EN_CQ);
    
    // wait Twu
    SOFTTIMER_Delay(nTimerHandler_PL,Tren-Twu);
    
    return nFuncResult;
    
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
STD_RESULT PL_SetMode(const uint8_t mode)
{
    STD_RESULT nFuncResult = RESULT_OK;
    
    if ((PL_COM1 == mode) || (PL_COM2 == mode) || (PL_COM3 == mode))
	{        
        if ((PL_TargetMode != PL_COM1) && (PL_TargetMode != PL_COM2) && (PL_TargetMode != PL_COM3))
        {
            GPIO_InitTypeDef GPIO_InitStruct;
            GPIO_InitStruct.GPIO_Pin = PL_PIN_RECEIVE;
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
            GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;   
            GPIO_Init(PL_PORT_IO_LINK, &GPIO_InitStruct);
            
            GPIO_InitStruct.GPIO_Pin = PL_PIN_TRANSMMITER;
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
            GPIO_Init(PL_PORT_IO_LINK, &GPIO_InitStruct);
		}
        
		if (PL_COM1 == mode)
		{
			PL_USART_IO_LINK->BRR = PL_BAUDRATE_COM1_BRR;
		}
		else if (PL_COM2 == mode)
		{
			PL_USART_IO_LINK->BRR = PL_BAUDRATE_COM2_BRR;
		}
		else if (PL_COM3 == mode)
		{
			PL_USART_IO_LINK->BRR = PL_BAUDRATE_COM3_BRR;
		}

    }
    else if (PL_INACTIVE == mode)
    {
        GPIO_InitTypeDef GPIO_InitStruct;
        GPIO_InitStruct.GPIO_Pin = PL_PIN_TRANSMMITER | PL_PIN_RECEIVE;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;   
        GPIO_Init(PL_PORT_IO_LINK, &GPIO_InitStruct);
    }
	else if (PL_DI == mode)
	{
		GPIO_InitTypeDef GPIO_InitStruct;
        GPIO_InitStruct.GPIO_Pin = PL_PIN_RECEIVE;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;   
        GPIO_Init(PL_PORT_IO_LINK, &GPIO_InitStruct);
		
        GPIO_InitStruct.GPIO_Pin = PL_PIN_TRANSMMITER;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
        GPIO_Init(PL_PORT_IO_LINK, &GPIO_InitStruct);
	}
	else if (PL_D == mode)
	{
		GPIO_InitTypeDef GPIO_InitStruct;
        GPIO_InitStruct.GPIO_Pin = PL_PIN_RECEIVE;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;   
        GPIO_Init(PL_PORT_IO_LINK, &GPIO_InitStruct);
				
        GPIO_InitStruct.GPIO_Pin = PL_PIN_TRANSMMITER;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; 
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(PL_PORT_IO_LINK, &GPIO_InitStruct);
	}
	
	PL_TargetMode = mode;
    
    return nFuncResult;
}
// end of PL_SetMode()



//**************************************************************************************************
// @Function      PL_TransferReq
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
STD_RESULT PL_TransferReq( const uint8_t *data, const uint16_t size )
{
	STD_RESULT nFuncResult = RESULT_OK;
    
   // __disable_interrupt();
    // Disable IRQ request Rx usart2
    USART_ITConfig(PL_USART_IO_LINK, USART_IT_RXNE, DISABLE);
    
    // Enable CQ output
    GPIO_ResetBits(PL_PORT_IO_LINK, PL_PIN_EN_CQ);
    
    // set trnsmiter in AF
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = PL_PIN_TRANSMMITER;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;   
	GPIO_Init(PL_PORT_IO_LINK, &GPIO_InitStruct);
	
    const uint8_t* pData = data;
    
    for (uint32_t i=0;i<size;i++)
    {
        while(RESET == USART_GetFlagStatus(PL_USART_IO_LINK, USART_FLAG_TC));
        USART_SendData(PL_USART_IO_LINK, *pData);
        pData++;
    }
    
    //wait transmiting last byte
     while(RESET == USART_GetFlagStatus(PL_USART_IO_LINK, USART_FLAG_TC));
    
    // Disable CQ output
    GPIO_SetBits(PL_PORT_IO_LINK, PL_PIN_EN_CQ);
    
    // Clear RXNE flag
    USART_ReceiveData(PL_USART_IO_LINK);
    
    // Enable IRQ request Rx usart2
    USART_ITConfig(PL_USART_IO_LINK, USART_IT_RXNE, ENABLE);
    //__enable_interrupt();
    return nFuncResult;
}
// end of PL_TransferReq()



//**************************************************************************************************
// @Function      PL_GetTransferInd
//--------------------------------------------------------------------------------------------------
// @Description   Get data from device
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    data - var contains received data
//                size - number data to receive  
//**************************************************************************************************
U8 PL_GetTransferInd( uint8_t *data, const uint16_t size )
{
	U8 response = RESPONSE_NOT_OK;
    
    uint16_t nmbrItems=0;
    
    nmbrItems = CIRCBUF_GetNumberOfItems(&PL_CIRCBUF); 
    nmbrItemsTest = nmbrItems;
	if ( nmbrItems >= size )
    {
        for (int i = 0;i < size;i++)
        {
            CIRCBUF_GetData(data,&PL_CIRCBUF);
            data++;
        }
        response = RESPONSE_OK;
		stateCom = COM_STATE_NOT_LOST;
    }
    else
    {
        stateCom = COM_STATE_LOST;
    }        
        
    return response;
}
// end of PL_GetTransferReq()



//**************************************************************************************************
// @Function      PL_GetTransferInd
//--------------------------------------------------------------------------------------------------
// @Description   Get data from device
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    data - var contains received data
//                size - number data to receive  
//**************************************************************************************************
U8 PL_GetCOMState( void )
{
	return stateCom;
}// end of PL_GetCOMState()



//**************************************************************************************************
// @Function      PL_ClearReceiveBuffer
//--------------------------------------------------------------------------------------------------
// @Description   None.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    mode - INACTIVE,DI,DO,COM1,COM2,COM3
//**************************************************************************************************
void PL_ClearReceiveBuffer(void)
{
    CIRCBUF_Purge(&PL_CIRCBUF);
}// end of PL_ClearReceiveBuffer()



//**************************************************************************************************
//==================================================================================================
// Definitions of Interrupt
//==================================================================================================
//**************************************************************************************************
void USART2_IRQHandler(void)
{
    #ifdef DEBUG
        TERMINAL_SendMessage("Interrupt request USART2\r\n");
    #endif   
    //GPIO_SetBits(GPIOB,GPIO_Pin_12);

  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        uint8_t recData;
        /* Read one byte from the receive data register */
        recData = USART_ReceiveData(USART2);
        
        //put data in circle buffer
        PL_StatusCircBuf = CIRCBUF_PutData(&recData,&PL_CIRCBUF);
        
    }
  
 // GPIO_ResetBits(GPIOB,GPIO_Pin_12);
}



//**************************************************************************************************
// @Function      PL_GetTransferInd
//--------------------------------------------------------------------------------------------------
// @Description   Get data from device
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    data - var contains received data
//                size - number data to receive  
//**************************************************************************************************
uint8_t PL_GetTargetMode(void)
{
    return PL_TargetMode;
}



//**************************************************************************************************
//==================================================================================================
// Definitions of local (private) functions
//==================================================================================================
//**************************************************************************************************

// None.





//****************************************** end of file *******************************************
