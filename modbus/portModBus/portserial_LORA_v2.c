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
#include "stdlib.h"
#include "port.h"
#include "ModuleRAK811.h"
#include "stdio.h"
#include "IO_LINK.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "software_timer.h"

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

#define PORT_UART_HARD		        	(USART3)
#define LORA_UART						USART3	
#define LORA_UART_BAUDRATE				(U32)(115200)
#define LORA_PORT						GPIOB	
#define LORA_PIN_RECEIVE				GPIO_Pin_11
#define LORA_PIN_TRANSMMITER			GPIO_Pin_10



//**************************************************************************************************
// Definitions of static global (private) variables
//**************************************************************************************************

static char ModBusRXByte = 0;
static const S8 startSendMesLora[] = {"at+send=lorap2p:"};

// software timer 
static uint8_t nTimerHandle_pLora = 0;
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
        USART_ITConfig(LORA_UART, USART_IT_RXNE, ENABLE);
    }
    else
    {
        USART_ITConfig(LORA_UART, USART_IT_RXNE, DISABLE);      
    }

    if( xTxEnable )
    {
        // //UART2_ITConfig(UART2_IT_TC, ENABLE);
        // UART2_ITConfig(UART2_IT_TXE, ENABLE);
		USART_ITConfig(LORA_UART, USART_IT_TXE, ENABLE);
    }
    else
    {
        // //UART2_ITConfig(UART2_IT_TC, DISABLE);
        // UART2_ITConfig(UART2_IT_TXE, DISABLE);
		USART_ITConfig(LORA_UART, USART_IT_TXE, DISABLE);
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
	
    //init software timer
    SOFTTIMER_Create(&nTimerHandle_pLora);
   
	//init pins for uart
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = LORA_PIN_RECEIVE;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;   
	GPIO_Init(LORA_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = LORA_PIN_TRANSMMITER;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LORA_PORT, &GPIO_InitStruct);

    
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
    USART_DeInit(LORA_UART);
    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_2;
    USART_InitStruct.USART_Parity = parity;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_BaudRate = ulBaudRate;
    USART_Init(LORA_UART, &USART_InitStruct);
    
    // Enable USART
    USART_Cmd(LORA_UART, ENABLE);
    
    // init RAK811 
	//MODE_LORA_P2P
	RAK811_sendMessage("at+set_config=lora:work_mode:1\r\n");
            //delay
        for(int i=0;i <DELEY_WITHOUT_IRQ;i++);
        for(int i=0;i <DELEY_WITHOUT_IRQ;i++);
	// init P2P prm
	RAK811_sendMessage("at+set_config=lorap2p:869525000:7:0:1:5:5\r\n");
            //delay
        for(int i=0;i <DELEY_WITHOUT_IRQ;i++);
        for(int i=0;i <DELEY_WITHOUT_IRQ;i++);
    // set transmit mode
    RAK811_confTransferMode(RAK811_RECEIVER_MODE);
            //delay
        for(int i=0;i <DELEY_WITHOUT_IRQ;i++);
    //SOFTTIMER_Delay(nTimerHandle_pLora,100000);
    
    
    // Enable Interrupt PL_USART_IO_LINK RX
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    

   
 
    return bStatus;
} // end of xMBPortSerialInit()



//**************************************************************************************************
// @Function      RAK811_confTransferMode()
//--------------------------------------------------------------------------------------------------
// @Description   configure LoRaP2P parameters
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    mode 1: receiver mode
//                     2: sender mode
//**************************************************************************************************
void RAK811_confTransferMode(const U8 mode)
{
    if (RAK811_SENDER_MODE == mode)
    {
        RAK811_sendMessage("at+set_config=lorap2p:transfer_mode:2\r\n");
    }
    else if (RAK811_RECEIVER_MODE == mode)
    {
        RAK811_sendMessage("at+set_config=lorap2p:transfer_mode:1\r\n");
    }
}// end of RAK811_confTransferMode



//**************************************************************************************************
// @Function      RAK811_sendMessage()
//--------------------------------------------------------------------------------------------------
// @Description   Send message to RAK811
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    message - data to send
//**************************************************************************************************
void RAK811_sendMessage(const char* message)
{
	U32 size = strlen(message);
	
	for (U32 i=0;i<size;i++)
    {
        while(RESET == USART_GetFlagStatus(LORA_UART, USART_FLAG_TC));
        USART_SendData(LORA_UART, *message);
        message++;
    }
    
    //wait transmiting last byte
     while(RESET == USART_GetFlagStatus(LORA_UART, USART_FLAG_TC));
	 
}// end of RAK811_sendMessage()



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
    USART_Cmd(LORA_UART, DISABLE);
    
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
    USART_Cmd(LORA_UART, DISABLE);
    
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
   char tempBuf[5];
   
   static U8 cntByte = 0;

   static char Testdata[256];
   
   U8 *end;
    // if start messege
    if (':' == ucByte)
    {
        RAK811_confTransferMode(RAK811_SENDER_MODE);
        //delay
        for(int i=0;i <DELEY_WITHOUT_IRQ;i++);
        const S8 *pData = startSendMesLora;
        for (int i=0; i < strlen(startSendMesLora);i++)
        {
            while(RESET == USART_GetFlagStatus(LORA_UART, USART_FLAG_TC));
            USART_SendData(LORA_UART, *pData);
            pData++;
        }
        while(RESET == USART_GetFlagStatus(LORA_UART, USART_FLAG_TC));
        
        snprintf(tempBuf,3,"%X",ucByte);
        for (int i=0; i < 2;i++)
        {
            while(RESET == USART_GetFlagStatus(LORA_UART, USART_FLAG_TC));
            USART_SendData(LORA_UART, tempBuf[i]);
            Testdata[i] = tempBuf[i];
        }
        cntByte = 1;        
    }
    else
    {
        if ( 0x0D == ucByte)
        {
            tempBuf[0] = '0';
            tempBuf[1] = 'D';
        }
        else if (0x0A == ucByte)
        {
            tempBuf[0] = '0';
            tempBuf[1] = 'A';
        }
        else
        {
            snprintf(tempBuf,3,"%X",ucByte);
        }
        for (int i=0; i < 2;i++)
        {
            while(RESET == USART_GetFlagStatus(LORA_UART, USART_FLAG_TC));
            USART_SendData(LORA_UART, tempBuf[i]);
            Testdata[i+cntByte*2] = tempBuf[i];
        }
        cntByte++;
        
        if ((tempBuf[0] == '0') && (tempBuf[1] == 'A')) 
        {
            while(RESET == USART_GetFlagStatus(LORA_UART, USART_FLAG_TC));
            USART_SendData(LORA_UART, '\r');
            while(RESET == USART_GetFlagStatus(LORA_UART, USART_FLAG_TC));
            USART_SendData(LORA_UART, '\n');
            cntByte = 0;
            
            //delay
            for(int i=0;i <DELEY_WITHOUT_IRQ;i++);
            for(int i=0;i <DELEY_WITHOUT_IRQ;i++);
            RAK811_confTransferMode(RAK811_RECEIVER_MODE);
            //delay
            for(int i=0;i <DELEY_WITHOUT_IRQ;i++);
        }
          
    }
    
  
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
    *pucByte = ModBusRXByte;

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
void USART3_IRQHandler(void)
{
    char receiveByte = 0;
    static BOOL startMes = FALSE;
    static U8 cntByteTwo = 0;
    static U8 uartBuffRec[3];
    
    static U16 TestCounter = 0;
    static U8 TestBuff[256]=0;
    
    U8 *end;
    
	if (SET == USART_GetFlagStatus(LORA_UART, USART_FLAG_TC))
	{
		//USART_ITConfig(LORA_UART, USART_IT_TXE, DISABLE);
        pxMBFrameCBTransmitterEmpty();
        //USART_ITConfig(LORA_UART, USART_IT_TXE, ENABLE);
	}
	if (SET == USART_GetFlagStatus(LORA_UART, USART_FLAG_RXNE))
	{
		receiveByte = USART_ReceiveData(LORA_UART);
        
        if (':' == receiveByte)// Lora's :
        {
            startMes = TRUE;
            cntByteTwo = 0;
        }
        else if (TRUE == startMes)
        {
            if (cntByteTwo == 0)
            {
                uartBuffRec[cntByteTwo] = receiveByte;
                cntByteTwo++;
            }
            else
            {
                uartBuffRec[cntByteTwo] = receiveByte;
                cntByteTwo = 0;
                uartBuffRec[3] = 0;
                ModBusRXByte = (char)strtoul(uartBuffRec,&end,16);
                TestBuff[TestCounter] = ModBusRXByte;
                TestCounter++;
                pxMBFrameCBByteReceived( );
                if ('\n' == ModBusRXByte)
                {
                    cntByteTwo = 0;
                    startMes = FALSE;
                }
            }
        }   
	}
	
    
} // end of USART1_IRQHandler()




//**************************************************************************************************
//==================================================================================================
// Definitions of local (private) functions
//==================================================================================================
//**************************************************************************************************

// None.


//****************************************** end of file *******************************************
