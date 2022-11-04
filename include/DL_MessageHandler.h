//**************************************************************************************************
// @Module        DL_MESSAGE_HANDLER.h
// @Filename      DL_MessageHandler_drv.h
//--------------------------------------------------------------------------------------------------
// @Description   Interface of the PL module.
//
//--------------------------------------------------------------------------------------------------
// @Version       1.0.0
//--------------------------------------------------------------------------------------------------
// @Date          30.08.2021
//--------------------------------------------------------------------------------------------------
// @History       Version  Author      Comment
// XX.XX.XXXX     1.0.0    KPS         First release.
//**************************************************************************************************

#ifndef DL_MES_HANDLER_H
#define DL_MES_HANDLER_H



//**************************************************************************************************
// Project Includes
//**************************************************************************************************

#include "stm32f10x.h"

#include "platform.h"

#include "general_types.h"

//**************************************************************************************************
// Declarations of global (public) data types
//**************************************************************************************************


typedef enum DL_MES_RESPONSE_STATE_enum
{
	OK,
	NOT_OK
}DL_MES_RESPONSE_STATE;

typedef struct DL_MES_RESPONSE_TYPE_str
{
	DL_MES_RESPONSE_STATE	state;
	U8 data;
}DL_MES_RESPONSE_TYPE;



//**************************************************************************************************
// Definitions of global (public) constants
//**************************************************************************************************

// None.


//**************************************************************************************************
// Declarations of global (public) variables
//**************************************************************************************************

// None.


//**************************************************************************************************
// Declarations of global (public) functions
//**************************************************************************************************


// Init DL_MES
extern STD_RESULT DL_MES_Init(void);

// Change state machine
extern void DL_MES_ChangeStateMachine( const U8 call );

// Task
extern void DL_MES_Task( void );

// Get response of DL_Read request
extern DL_MES_RESPONSE_TYPE DL_MES_GetResponse_DL_Read_req( void );

// Get current COM
extern U8 DL_MES_GetCurrentCom( void );

extern void DL_MES_Req_DL_Read(const U8* addr);

extern void DL_MES_Req_DL_Write(const U8* addr, const U8* data);

extern U8 DL_MES_CalCKT( uint8_t* data, const uint16_t size );

extern void DL_MES_Cnf(U8* data, const U16 size);

#endif // DL_MES_HANDLER_H

//****************************************** end of file *******************************************
