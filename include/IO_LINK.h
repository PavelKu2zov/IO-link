//**************************************************************************************************
// @Module        IO_LINK
// @Filename      IO_Link.h
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

#ifndef IO_LINK_H
#define IO_LINK_H



//**************************************************************************************************
// Project Includes
//**************************************************************************************************

#include "stm32f10x.h"


//**************************************************************************************************
// Declarations of global (public) data types
//**************************************************************************************************

// calls
typedef enum CALLS_enum
{
	MH_CONF_COM1=0,
	MH_CONF_COM2,
	MH_CONF_COM3,
	MH_CONF_STARTUP,
	MH_CONF_PREOPERATE,
	MH_CONF_OPERATE,
	MH_CONF_ACTIVE,
	MH_CONF_INACTIVE,
	OH_CONF_ACTIVE,
	OH_CONF_INACTIVE,
	IH_CONF_ACTIVE,
	IH_CONF_INACTIVE,
	CH_CONF_ACTIVE,
	CH_CONF_INACTIVE,	
	EH_CONF_ACTIVE,
	EH_CONF_INACTIVE,
	
	START_UP,
	PREOPERATE,
	OPERATE,
	INACTIVE,
	
	PD_CONF_INACTIVE,
	PD_CONF_SINGLE,
	PD_CONF_INTERLEAVE
}CALLS;

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

// None.


#endif // IO_LINK_H

//****************************************** end of file *******************************************
