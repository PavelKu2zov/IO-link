//**************************************************************************************************
// @Module        PORT_SERIAL
// @Filename      portserial.c
//--------------------------------------------------------------------------------------------------
// @Platform      STM8S
//--------------------------------------------------------------------------------------------------
// @Compatible    STM8S-discovery board
//--------------------------------------------------------------------------------------------------
// @Description   Implementation of the PORT_SERIAL functionality.
//                
//
//                Abbreviations:
//                  None.
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
// @Date          25.08.2021
//--------------------------------------------------------------------------------------------------
// @History       Version  Author      Comment
// XX.XX.XXXX     1.0.0    KPS         First release.
//**************************************************************************************************



//**************************************************************************************************
// Project Includes
//**************************************************************************************************

/* ----------------------- Platform includes --------------------------------*/
//#include <stdlib.h>
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"


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

#define PORT_UART_HARD			(USART1)



//**************************************************************************************************
// Definitions of static global (private) variables
//**************************************************************************************************

//None.


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
// @Function      voidvMBPortSerialEnable()
//--------------------------------------------------------------------------------------------------
// @Description   Open serial port
//--------------------------------------------------------------------------------------------------
// @Notes         None.  
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    xRxEnable - enable RX
//                xTxEnable - enable TX  
//**************************************************************************************************
void vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{

    if( xRxEnable )
    {
		USART_ITConfig(PORT_UART_HARD, USART_IT_RXNE, ENABLE);
    }
    else
    {
        USART_ITConfig(PORT_UART_HARD, USART_IT_RXNE, DISABLE);      
    }

    if( xTxEnable )
    {
        // //UART2_ITConfig(UART2_IT_TC, ENABLE);
        // UART2_ITConfig(UART2_IT_TXE, ENABLE);
		USART_ITConfig(PORT_UART_HARD, USART_IT_TXE, ENABLE);
    }
    else
    {
        // //UART2_ITConfig(UART2_IT_TC, DISABLE);
        // UART2_ITConfig(UART2_IT_TXE, DISABLE);
		USART_ITConfig(PORT_UART_HARD, USART_IT_TXE, DISABLE);
    }
}// end of voidvMBPortSerialEnable



//**************************************************************************************************
// @Function      xMBPortSerialInit()
//--------------------------------------------------------------------------------------------------
// @Description   Initial serial port
//--------------------------------------------------------------------------------------------------
// @Notes         None.  
//--------------------------------------------------------------------------------------------------
// @ReturnValue   true - not initialized
//                flase - initialized  
//--------------------------------------------------------------------------------------------------
// @Parameters    ucPort - 
//                ulBaudRate - baudrate
//                ucDataBits - number data bits
//                eParity - party  
//**************************************************************************************************
BOOL xMBPortSerialInit( UCHAR ucPort, 
                        ULONG ulBaudRate,
                        UCHAR ucDataBits, 
                        eMBParity eParity )
{
	
	BOOL bStatus = TRUE;
    U8 parity = USART_Parity_No;
	
	// Init GPIO
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;//TX1
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;   
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;//RX1
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;   
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    
	switch ( eParity )
    {
    case MB_PAR_NONE:
        parity = USART_Parity_No;
        break;
    case MB_PAR_ODD:
        parity = USART_Parity_Odd;
        break;
    case MB_PAR_EVEN:
        parity = USART_Parity_Even;
        break;
    default:
        bStatus = FALSE;
    }


	// Init TERMINAL_UART
    USART_DeInit(USART1);
    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_2;
    USART_InitStruct.USART_Parity = parity;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_BaudRate = ulBaudRate;
    USART_Init(USART1, &USART_InitStruct);
    
    // Enable USART
    USART_Cmd(USART1, ENABLE);
    
    // Enable Interrupt PL_USART_IO_LINK RX
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
 
    return bStatus;
} // end of xMBPortSerialInit()



//**************************************************************************************************
// @Function      vMBPortSerialClose()
//--------------------------------------------------------------------------------------------------
// @Description   Close serial port
//--------------------------------------------------------------------------------------------------
// @Notes         None.  
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
void vMBPortSerialClose( void )
{
	// Disable USART
    USART_Cmd(USART1, DISABLE);
    
}// end of vMBPortSerialClose



//**************************************************************************************************
// @Function      vMBPortClose()
//--------------------------------------------------------------------------------------------------
// @Description   Close serial port
//--------------------------------------------------------------------------------------------------
// @Notes         None.  
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
void vMBPortClose( void )
{
    // Disable USART
    USART_Cmd(USART1, DISABLE);
    
}// end of vMBPortClose




//**************************************************************************************************
// @Function      xMBPortSerialPutByte()
//--------------------------------------------------------------------------------------------------
// @Description   Send data
//--------------------------------------------------------------------------------------------------
// @Notes         None.  
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    ucByte - byte to send
//**************************************************************************************************
BOOL xMBPortSerialPutByte( CHAR ucByte )
{
     
	//while(SET != USART_GetFlagStatus(USART1, USART_FLAG_TXE));
	
	USART_SendData(USART1, ucByte);
  
  return TRUE;
}// end of xMBPortSerialPutByte



//**************************************************************************************************
// @Function      xMBPortSerialGetByte()
//--------------------------------------------------------------------------------------------------
// @Description   Get byte from serial port
//--------------------------------------------------------------------------------------------------
// @Notes         None.  
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    pucByte - pointer where store byte
//**************************************************************************************************
BOOL xMBPortSerialGetByte( CHAR * pucByte )
{
    *pucByte = USART_ReceiveData(USART1);

    return TRUE;
}// end of xMBPortSerialGetByte



//**************************************************************************************************
// @Function      USART1_IRQHandler()
//--------------------------------------------------------------------------------------------------
// @Description   Interrupt TC or RXNE from UART1 
//--------------------------------------------------------------------------------------------------
// @Notes         None.  
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
void USART1_IRQHandler(void)
{
	if (SET == USART_GetFlagStatus(USART1, USART_FLAG_TC))
	{
		pxMBFrameCBTransmitterEmpty();
	}
	if (SET == USART_GetFlagStatus(USART1, USART_FLAG_RXNE))
	{
		pxMBFrameCBByteReceived( );
	}
	
    
} // end of USART1_IRQHandler()




//**************************************************************************************************
//==================================================================================================
// Definitions of local (private) functions
//==================================================================================================
//**************************************************************************************************

// None.


//****************************************** end of file *******************************************
