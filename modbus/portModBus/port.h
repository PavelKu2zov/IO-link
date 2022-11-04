//**************************************************************************************************
// @Module        PORT
// @Filename      port.h
//--------------------------------------------------------------------------------------------------
// @Description   port for STM8S to use FreeModBus
//                
//--------------------------------------------------------------------------------------------------
// @Version       1.0.0
//--------------------------------------------------------------------------------------------------
// @Date          24.08.2021
//--------------------------------------------------------------------------------------------------
// @History       Version  Author      Comment
// XX.XX.XXXX     1.0.0    KPS         First release.
//**************************************************************************************************

#ifndef PORT_H
#define PORT_H


#define assert(x)
//**************************************************************************************************
// Project Includes
//**************************************************************************************************


#include "stm32f10x.h"

#include "platform.h"

#include "general_types.h"


//**************************************************************************************************
// Declarations of global (public) data types
//**************************************************************************************************

 typedef char    BOOL;

 typedef unsigned char UCHAR;
 typedef char    CHAR;

 typedef unsigned short USHORT;
 typedef short   SHORT;

 typedef unsigned long ULONG;
 typedef long    LONG;



//**************************************************************************************************
// Definitions of global (public) constants
//**************************************************************************************************

// macros for enabling interrupts.
#define ENTER_CRITICAL_SECTION( )   vMBPortEnterCritical()

// macros for disabling interrupts.
#define EXIT_CRITICAL_SECTION( )    vMBPortExitCritical()

#ifndef TRUE
#define TRUE                                    1
#endif

#ifndef FALSE
#define FALSE                                   0
#endif

#define MB_PORT_HAS_CLOSE	                	1
#define MB_ASCII_TIMEOUT_WAIT_BEFORE_SEND_MS    2
 
#define  RAK811_MODE_LoRaWAN            ('0')
#define  RAK811_MODE_LORA_P2P           ('1')
#define  RAK811_RECEIVER_MODE           ('1')
#define  RAK811_SENDER_MODE             ('2') 

//**************************************************************************************************
// Declarations of global (public) variables
//**************************************************************************************************

// None.



//**************************************************************************************************
// Declarations of global (public) functions
//**************************************************************************************************

extern void vMBPortEnterCritical( void );
extern void vMBPortExitCritical( void );
extern void MBPortTimerInitHard( void );
extern BOOL vMBPortGetStatusRxEnable( void );
extern BOOL vMBPortGetStatusTxEnable( void );
extern void xMBPortClearTxBuff( void );
extern void xMBPortGetDataTxBuff( S8* pData, const U16 size );
extern U16 xMBPortGetSizeDataTxBuff( void );
extern void xMBPortSerialPutByteInRxBuf( CHAR * pucByte );
#endif // #ifndef PORT_H

//****************************************** end of file *******************************************
