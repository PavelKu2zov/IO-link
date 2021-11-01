//**************************************************************************************************
// @Module        MODULE
// @Filename      module.c
//--------------------------------------------------------------------------------------------------
// @Platform      PLATFORM_NAME
//--------------------------------------------------------------------------------------------------
// @Compatible    COMPATIBLE_PROCESSOR_MODULE
//--------------------------------------------------------------------------------------------------
// @Description   Implementation of the MODULE functionality.
//                [Description...]
//
//                Abbreviations:
//                  ABBR0 -
//                  ABBR1 -
//
//                Global (public) functions:
//                  MODULE_functionZero()
//                  MODULE_functionOne()
//
//                Local (private) functions:
//                  MODULE_functionTwo()
//                  MODULE_functionThree()
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
#include "main.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "PL_drv.h"
#include "Init.h"

//**************************************************************************************************
// Definitions of global (public) variables
//**************************************************************************************************

// Semaphore fo DL mode 
xSemaphoreHandle xDL_SetMode;

// PL Queue
xQueueHandle xPL_Queue;



//**************************************************************************************************
// Declarations of local (private) data types
//**************************************************************************************************

//None.

//**************************************************************************************************
// Definitions of local (private) constants
//**************************************************************************************************

#define LEN_PL_QUEUE            (10)

//**************************************************************************************************
// Definitions of static global (private) variables
//**************************************************************************************************

//None.

//**************************************************************************************************
// Declarations of local (private) functions
//**************************************************************************************************

//None.


//**************************************************************************************************
//==================================================================================================
// Definitions of global (public) functions
//==================================================================================================
//**************************************************************************************************



//**************************************************************************************************
// @Function      main()
//--------------------------------------------------------------------------------------------------
// @Description   [description...]
//--------------------------------------------------------------------------------------------------
// @Notes
//--------------------------------------------------------------------------------------------------
// @ReturnValue   returnValue - [description...]
//--------------------------------------------------------------------------------------------------
// @Parameters    parameterZero - [description...]
//**************************************************************************************************

int main(void)
{
	Init();

    /
	
    vSemaphoreCreateBinary( xDL_SetMode );
    
    // Create Queue for PL 
    xPL_Queue = xQueueCreate( LEN_PL_QUEUE,sizeof (PL_MES_QUEUE));
}// end of main()



