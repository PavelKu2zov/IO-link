//**************************************************************************************************
// @Module        DL_MODE_HANDLER
// @Filename      DL_ModeHandler.c
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
#include "DL_ModeHandler_drv.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"



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
typedef enum DL_MODE_TypeStateMachine_enum
{
    DL_MODE_IDLE_0=0,
    DL_MODE_ESTABLISH_COM,
    DL_MODE_STARTUP_2,
    DL_MODE_PREOPERATE_3,
    DL_MODE_OPERATE_4
}DL_MODE_TypeStateMachine;



//**************************************************************************************************
// Definitions of local (private) constants
//**************************************************************************************************

// None.


//**************************************************************************************************
// Definitions of static global (private) variables
//**************************************************************************************************

DL_MODE_TypeStateMachine DL_MODE_StateMachine;

DL_MODE_Mode DL_MODE_CurrentMode;

DL_MODE_Mode DL_MODE_NextMode;


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
// @Function      DL_MODE_Task()
//--------------------------------------------------------------------------------------------------
// @Description   task of DL mode handler
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
void DL_MODE_Task(void *pvParameters)
{
	portBASE_TYPE xStatus;
	DL_SET_MODE_QUEUE = DL_State;
	
	for(;;)
	{
		// Get data from System Management
		xStatus = xQueueReceive( xDL_SetModeQueue,&DL_State,0);
		if ( pdPASS == xStatus )
		{
			switch( DL_State )
			{
				case START_UP: 
				{
					if ( DL_MODE_IDLE_0 == DL_MODE_StateMachine )
					{
						DL_MODE_StateMachine = DL_MODE_ESTABLISH_COM;
					}
					else if ( DL_MODE_PREOPERATE_3 == DL_MODE_StateMachine )
					{
						DL_MODE_StateMachine = DL_MODE_STARTUP_2;
					}
					else if ( DL_MODE_OPERATE_4 == DL_MODE_StateMachine )
					{
						DL_MODE_StateMachine = DL_MODE_STARTUP_2;
					}
					break;
				}
				
				case PREOPERATE: DL_MODE_StateMachine = DL_MODE_PREOPERATE_3;break;
				
				case OPERATE: DL_MODE_StateMachine = DL_MODE_OPERATE_4;break;
				
				default: break;
			}
		}
        
        // move in state machine
        switch(DL_MODE_StateMachine)
        {
            case DL_MODE_Idle_0: DL_MODE_Idle_0();break;
			
			case DL_MODE_ESTABLISH_COM: DL_MODE_EstablishCom();break;
			
			case DL_MODE_STARTUP_2; DL_MODE_StartUP_2();break;
			
			case DL_MODE_PREOPERATE_3: DL_MODE_Preoperate_3();break;
			
			case DL_MODE_OPERATE_4: DL_MODE_Operate_4();break;
			
			default:break;
        }
		
		vTaskDelay( 250 / portTICK_RATE_MS );
		
	}
}// end of DL_MODE_Task()



//**************************************************************************************************
//==================================================================================================
// Definitions of local (private) functions
//==================================================================================================
//**************************************************************************************************



//**************************************************************************************************
// @Function      DL_MODE_Idle_0()()
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
static void DL_MODE_Idle_0(void)
{   
	;
} // end of DL_MODE_Idle_0()()



//**************************************************************************************************
// @Function      DL_MODE_EstablishCom()()
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
static void DL_MODE_EstablishCom(void)
{   
	;
} // end of DL_MODE_EstablishCom()



//**************************************************************************************************
// @Function      DL_MODE_StartUP_2()
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
static void DL_MODE_StartUP_2(void)
{   
	;
} // end of DL_MODE_StartUP_2()



//**************************************************************************************************
// @Function      DL_MODE_Preoperate_3()
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
static void DL_MODE_Preoperate_3(void)
{   
	;
} // end of DL_MODE_Preoperate_3()



//**************************************************************************************************
// @Function      DL_MODE_Preoperate_3()
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
static void DL_MODE_Preoperate_3(void)
{   
	;
} // end of DL_MODE_Preoperate_3()



//**************************************************************************************************
// @Function      DL_MODE_Operate_4()
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
static void DL_MODE_Operate_4(void)
{   
	;
} // end of DL_MODE_Operate_4()
//****************************************** end of file *******************************************
