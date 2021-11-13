//**************************************************************************************************
// @Module        DL_ON_REQ_DATA_HANDLER
// @Filename      DL_OnReqDataHandler.c
//--------------------------------------------------------------------------------------------------
// @Platform      PLATFORM_NAME
//--------------------------------------------------------------------------------------------------
// @Compatible    COMPATIBLE_PROCESSOR_MODULE
//--------------------------------------------------------------------------------------------------
// @Description   Implementation of the DL_MODE_HANDLER functionality.
//                
//
//                Abbreviations:
//                  DL - Data link layer
//                  
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

// Native header
#include "DL_OnReqDataHandler_drv.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "IO_LINK.h"



//**************************************************************************************************
// Verification of the imported configuration parameters
//**************************************************************************************************

//None.

//**************************************************************************************************
// Definitions of global (public) variables
//**************************************************************************************************

//None.



//**************************************************************************************************
// Declarations of local (private) data types
//**************************************************************************************************

// State machine of DL mode handler
typedef enum DL_ON_REQ_DATA_TypeStateMachine_enum
{
    DL_ON_REQ_DATA_INACTIVE_0=0,
    DL_ON_REQ_DATA_ISDU_1_INACTIVE_0,
	DL_ON_REQ_DATA_ISDU_1_IDLE_1,
	DL_ON_REQ_DATA_ISDU_1_REQUEST_2,
	DL_ON_REQ_DATA_ISDU_1_ERROR_4,
	DL_ON_REQ_DATA_ISDU_1_WAIT_3,
	DL_ON_REQ_DATA_ISDU_1_RESPONSE_5,
    DL_ON_REQ_DATA_COMMAND_INACTIVE_0,
	DL_ON_REQ_DATA_COMMAND_IDLE_1,
	DL_ON_REQ_DATA_COMMAND_MASTER_COMMAND_2,
	DL_ON_REQ_DATA_EVENT_INACTIVE_0,
	DL_ON_REQ_DATA_EVENT_IDLE_1,
	DL_ON_REQ_DATA_EVENT_CONFIRMATION_4,
	DL_ON_REQ_DATA_EVENT_READ_EVENT_2,
	DL_ON_REQ_DATA_EVENT_SIGNAL_EVENT_3,
}DL_ON_REQ_DATA_TypeStateMachine;


//**************************************************************************************************
// Definitions of local (private) constants
//**************************************************************************************************

// None.


//**************************************************************************************************
// Definitions of static global (private) variables
//**************************************************************************************************

DL_ON_REQ_DATA_TypeStateMachine DL_ON_REQ_DATA_StateMachine;

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
// @Function      DL_ON_REQ_DATA_Task()
//--------------------------------------------------------------------------------------------------
// @Description   task of DL mode handler
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
void DL_ON_REQ_DATA_Task(void *pvParameters)
{
	portBASE_TYPE xStatus;
	CALLS DL_OD_State;
	
	for(;;)
	{
		// Get data from System Management
		xStatus = xQueueReceive( xDL_OD_Queue,&DL_OD_State,0);
		if ( pdPASS == xStatus )
		{
			switch( DL_PD_State )
			{
				case OH_CONF_ACTIVE: DL_ON_REQ_DATA_StateMachine = DL_ON_REQ_DATA_ISDU_1_INACTIVE_0;break;
				
				case OH_CONF_INACTIVE: DL_ON_REQ_DATA_StateMachine = DL_ON_REQ_DATA_INACTIVE_0;break;
				
				case IH_CONF_ACTIVE: DL_ON_REQ_DATA_StateMachine = DL_ON_REQ_DATA_ISDU_1_IDLE_1;break;
				
				case IH_CONF_INACTIVE: DL_ON_REQ_DATA_StateMachine = DL_ON_REQ_DATA_ISDU_1_INACTIVE_0;break;
				
				default: break;
			}
		}
        
        // move in state machine
        switch(DL_ON_REQ_DATA_StateMachine)
        {
            case DL_ON_REQ_DATA_INACTIVE_0: DL_ON_REQ_DATA_Inactive_0();break;
			
			case DL_ON_REQ_DATA_ISDU_1_INACTIVE_0: DL_ON_REQ_DATA_ISDU_1_Inactive_0();break;
			
			case DL_ON_REQ_DATA_ISDU_1_IDLE_1: DL_ON_REQ_DATA_ISDU_1_Idle_1();break;
			
			default:break;
        }
		
		vTaskDelay( 250 / portTICK_RATE_MS );
		
	}
}// end of DL_ON_REQ_DATA_Task()



//**************************************************************************************************
//==================================================================================================
// Definitions of local (private) functions
//==================================================================================================
//**************************************************************************************************



//**************************************************************************************************
// @Function      DL_ON_REQ_DATA_Inactive_0()
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
static void DL_ON_REQ_DATA_Inactive_0(void)
{   
	;
} // end of DL_ON_REQ_DATA_Inactive_0()




//**************************************************************************************************
// @Function      DL_ON_REQ_DATA_ISDU_1_Inactive_0()
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
static void DL_ON_REQ_DATA_ISDU_1_Inactive_0(void)
{   
	;
} // end of DL_ON_REQ_DATA_ISDU_1_Inactive_0()



//**************************************************************************************************
// @Function      DL_ON_REQ_DATA_ISDU_1_Idle_1()
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
static void DL_ON_REQ_DATA_ISDU_1_Idle_1(void)
{   
	;
} // end of DL_ON_REQ_DATA_ISDU_1_Idle_1()



//****************************************** end of file *******************************************
