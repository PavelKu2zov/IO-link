//**************************************************************************************************
// @Module        SYSTEM_MANAGEMENT
// @Filename      module.h
//--------------------------------------------------------------------------------------------------
// @Description   Interface of the MODULE module.
//                [Description...]
//--------------------------------------------------------------------------------------------------
// @Version       1.0.0
//--------------------------------------------------------------------------------------------------
// @Date          XX.XX.XXXX
//--------------------------------------------------------------------------------------------------
// @History       Version  Author      Comment
// XX.XX.XXXX     1.0.0    XXX         First release.
//**************************************************************************************************

#ifndef SYSTEM_MANAGEMENT_H
#define SYSTEM_MANAGEMENT_H



//**************************************************************************************************
// Project Includes
//**************************************************************************************************

#include "stm32f10x.h"

#include "platform.h"

#include "general_types.h"

//**************************************************************************************************
// Declarations of global (public) data types
//**************************************************************************************************

// None.


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

// Init System management
extern STD_RESULT SM_Init(void);

// Start System management
extern void SM_Task(void);
// read prm, set PREOPERATE mode
extern void SM_SetPortConfig(void);
// Set OPERATE mode
extern void SM_Operate(void);   


#endif // #ifndef SYSTEM_MANAGEMENT_H

//****************************************** end of file *******************************************
