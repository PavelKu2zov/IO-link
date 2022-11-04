//**************************************************************************************************
// @Module        SOFTWARE_TIMER
// @Filename      software_timer.h
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

#ifndef SOFTWARE_TIMER_H
#define SOFTWARE_TIMER_H



//**************************************************************************************************
// Project Includes
//**************************************************************************************************


// Get configuration of the program module
#include "stm32f10x.h"

#include "software_timer_cfg.h"

#include "platform.h"

#include "general_types.h"

//**************************************************************************************************
// Declarations of global (public) data types
//**************************************************************************************************

typedef enum SOFTIMER_EVENT_enum
{
    NOT_EXPIRED=0,
    EXPIRED
}SOFTTIMER_EVENT;



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

// Init timer
extern void SOFTTIMER_HardInit(void);

// Get Event
extern STD_RESULT SOFTTIMER_GetEvent(const uint8_t nTimerHandler, SOFTTIMER_EVENT *const event);
       
// Create softtimer       
extern STD_RESULT SOFTTIMER_Create(uint8_t* const pTimerHandler);

//start timer
extern STD_RESULT SOFTTIMER_StartTimer(const uint8_t     nTimerHandle,
                                const SOFTTIMER_SZ nTimerPeriod);

// wait nTimerPeriod in us
extern void SOFTTIMER_Delay(const uint8_t  nTimerHandle,const SOFTTIMER_SZ nTimerPeriod);
// deinit all sofware timer
extern STD_RESULT SOFTTIMER_DeInit(void);

#endif // #ifndef SOFTWARE_TIMER_H

//****************************************** end of file *******************************************
