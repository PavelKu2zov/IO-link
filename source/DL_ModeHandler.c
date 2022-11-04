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
//                  DL_MODE_Init()
//                  DL_MODE_Task()                 
//                  
//
//
//                Local (private) functions:
//                  DL_MODE_Idle_0()
//                  DL_MODE_EstablishCom_1()
//                  DL_MODE_WuRq_5()
//                  DL_MODE_Request_COM3_6()
//                  DL_MODE_Request_COM2_7()
//                  DL_MODE_Request_COM1_8()
//                  DL_MODE_StartUP_2()
//                  DL_MODE_Preoperate_3()
//                  DL_MODE_Operate_4()
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
#include "IO_LINK.h"
#include "DL_ModeHandler.h"
#include "DL_MessageHandler.h"
#include "Physical_layer.h"
#include "software_timer.h"
#include "terminal.h"



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
    DL_MODE_ESTABLISH_COM_1,
    DL_MODE_WURQ_5,
    DL_MODE_REQUEST_COM3_6,
	DL_MODE_REQUEST_COM2_7,
	DL_MODE_REQUEST_COM1_8,
	DL_MODE_RETRY_9,
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

// static machine DL Mode handler
static DL_MODE_TypeStateMachine DL_MODE_StateMachine;


// software timer Tdmt
static U8 nTimerHandle_Tdmt = 0;
// software timer Tdwu
static U8 nTimerHandle_Tdwu = 0;
// current mode
static U8 currentMode;

//**************************************************************************************************
// Declarations of local (private) functions
//**************************************************************************************************

static void DL_MODE_Idle_0(void);

static STD_RESULT DL_MODE_EstablishCom_1(U8 *const response);

static STD_RESULT DL_MODE_WuRq_5(void);

static STD_RESULT DL_MODE_Request_COM3_6(void);

static STD_RESULT DL_MODE_Request_COM2_7(void);

static STD_RESULT DL_MODE_Request_COM1_8(void);

static void DL_MODE_StartUP_2(void);

static void DL_MODE_Preoperate_3(void);

static void DL_MODE_Operate_4(void);

//**************************************************************************************************
//==================================================================================================
// Definitions of global (public) functions
//==================================================================================================
//**************************************************************************************************



//**************************************************************************************************
// @Function      DL_MODE_Init()
//--------------------------------------------------------------------------------------------------
// @Description   None.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
STD_RESULT DL_MODE_Init(void)
{
    STD_RESULT nFuncResult = RESULT_OK;
    
    DL_MODE_StateMachine = DL_MODE_IDLE_0;
	
	currentMode = DL_INACTIVE;
    
    // Create timer Tdmt
    if (RESULT_OK == SOFTTIMER_Create(&nTimerHandle_Tdmt))
    {
        if (RESULT_OK == SOFTTIMER_Create(&nTimerHandle_Tdwu))
        {
            #ifdef DEBUG
                TERMINAL_SendMessage("DL_MODE Handler was initialized\n\r");
            #endif
            nFuncResult = RESULT_OK;
        }
        else
        {
            nFuncResult = RESULT_NOT_OK;
        }
    }
    else
    {
        nFuncResult = RESULT_NOT_OK;
    }        
    
    return nFuncResult;
}// end of DL_MODE_Init



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
void DL_MODE_ChangeMode(const U8 mode)
{
	// to apply mode and chose next step of the state machine
	switch( mode )
	{
		case DL_INACTIVE:break;
        
        case DL_START_UP: 
		{
			if ( DL_MODE_IDLE_0 == DL_MODE_StateMachine )
			{
				DL_MODE_StateMachine = DL_MODE_ESTABLISH_COM_1;
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
		
		case DL_PREOPERATE: DL_MODE_StateMachine = DL_MODE_PREOPERATE_3;break;
		
		case DL_OPERATE: DL_MODE_StateMachine = DL_MODE_OPERATE_4;break;
		
		default: ;
	}
}// end of DL_MODE_ChangeMode()



//**************************************************************************************************
// @Function      DL_MODE_GetCurrentMode()
//--------------------------------------------------------------------------------------------------
// @Description   None.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
U8 DL_MODE_GetCurrentMode( void )
{
	return currentMode;
}// end of DL_MODE_GetCurrentMode()



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
void DL_MODE_Task( void )
{
    U8 response = 0;
	// move in state machine
	switch(DL_MODE_StateMachine)
	{
		case DL_MODE_IDLE_0: DL_MODE_Idle_0();break;
		
		case DL_MODE_ESTABLISH_COM_1:
			
			
            DL_MODE_EstablishCom_1(&response);
            if (( BAUDRATE_COM1 == response) || ( BAUDRATE_COM2 == response) || ( BAUDRATE_COM3 == response))
            {
                DL_MODE_StateMachine = DL_MODE_STARTUP_2;
                currentMode = DL_START_UP;
            }
            break;
		
        case DL_MODE_STARTUP_2: DL_MODE_StartUP_2();break;
		
		case DL_MODE_PREOPERATE_3: 
            
            DL_MODE_Preoperate_3();
            currentMode = DL_PREOPERATE;
            break;
		
		case DL_MODE_OPERATE_4: 
            DL_MODE_Operate_4();
            currentMode = DL_OPERATE;
            break;
		
		default:break;
	}
		
	
}// end of DL_MODE_Task()



//**************************************************************************************************
//==================================================================================================
// Definitions of local (private) functions
//==================================================================================================
//**************************************************************************************************



//**************************************************************************************************
// @Function      DL_MODE_Idle_0()
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
	DL_MODE_StateMachine = DL_MODE_ESTABLISH_COM_1;
} // end of DL_MODE_Idle_0()



//**************************************************************************************************
// @Function      DL_MODE_ESTABLISH_COM_1()
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
static STD_RESULT DL_MODE_EstablishCom_1(U8 *const response)
{
    STD_RESULT nFuncResult = RESULT_NOT_OK;
    #ifdef DEBUG
        TERMINAL_SendMessage( "DL_MODE -> EstablishCom_1\n\r");
    #endif
    
    for (uint8_t n=0; n < Nwu; n++)
    {
        // Start timer Tdwu
        if (RESULT_OK == SOFTTIMER_StartTimer(nTimerHandle_Tdwu,Tdwu))
        {
    //-------------------  Wake Up  ----------------------------------------------------        
            if ( RESULT_OK == DL_MODE_WuRq_5())
            {
                //wait Tdmt_COM3        
				SOFTTIMER_Delay(nTimerHandle_Tdmt,Tdmt_COM3);
            }
            else
            {
                nFuncResult = RESULT_NOT_OK;
                continue;
            }
    //------------------- COM3 Req -----------------------------------------------------
            if ( RESULT_OK == DL_MODE_Request_COM3_6())
            {
                if (BAUDRATE_COM3 == DL_MES_GetCurrentCom())
                {
                    nFuncResult = RESULT_OK;
                    #ifdef DEBUG
                        TERMINAL_SendMessage( "DL_MODE -> Response equal COM3\n\r");
                    #endif
					*response = BAUDRATE_COM3;
                    break;
                }
                else
                {
                    //wait Tdmt_COM2    
					SOFTTIMER_Delay(nTimerHandle_Tdmt,Tdmt_COM2-Tm_sequence(2,2,Tbit_COM3));
                }
            }
            else
            {
                nFuncResult = RESULT_NOT_OK;
                continue;
            }
    //------------------- COM2 Req -----------------------------------------------------
//            if ( RESULT_OK == DL_MODE_Request_COM2_7(response))
//            {
//                if (RESPONSE_COM2 == DL_MES_GetCurrentCom())
//                {
//                    nFuncResult = RESULT_OK;
//                    #ifdef DEBUG
//                        TERMINAL_SendMessage( "DL_MODE -> Response equal COM2\n\r");
//                    #endif
//                    break;
//                }
//                else
//                {
//                    //wait Tdmt_COM1
//                    if (RESULT_OK == SOFTTIMER_StartTimer(nTimerHandle_Tdmt,Tdmt_COM1-Tm_sequence(2,2,Tbit_COM2)))
//                    {
//                        SOFTTIMER_EVENT event = NOT_EXPIRED;
//                        while(EXPIRED != event)
//                        {
//                            SOFTTIMER_GetEvent(nTimerHandle_Tdmt, &event);
//                        }
//                    }
//                    else
//                    {
//                        nFuncResult = RESULT_NOT_OK;
//                        continue;
//                    }
//                }
//            }
//            else
//            {
//                nFuncResult = RESULT_NOT_OK;
//                continue;
//            }
            
    //------------------- COM1 Req -----------------------------------------------------
//            if ( RESULT_OK == DL_MODE_Request_COM1_8(response))
//            {
//                if (RESPONSE_COM1 == DL_MES_GetCurrentCom())
//                {
//                    nFuncResult = RESULT_OK;
//                    #ifdef DEBUG
//                        TERMINAL_SendMessage( "DL_MODE -> Response equal COM1\n\r");
//                    #endif
//                    break;
//                }
//                else
//                {
//                    //wait Tdmt_COM1
//                    if (RESULT_OK == SOFTTIMER_StartTimer(nTimerHandle_Tdmt,Tdmt_COM1))
//                    {
//                        SOFTTIMER_EVENT event = NOT_EXPIRED;
//                        while(EXPIRED != event)
//                        {
//                            SOFTTIMER_GetEvent(nTimerHandle_Tdmt, &event);
//                        }
//                    }
//                    else
//                    {
//                        nFuncResult = RESULT_NOT_OK;
//                        continue;
//                    }
//                }
//            }
//            else
//            {
//                nFuncResult = RESULT_NOT_OK;
//                continue;
//            }
            
            // wait Tdwu time
            SOFTTIMER_EVENT event = NOT_EXPIRED;
            while(EXPIRED != event)
            {
                SOFTTIMER_GetEvent(nTimerHandle_Tdwu, &event);
            }
}
        else
        {
            nFuncResult = RESULT_NOT_OK;
        }
    }
    
    return nFuncResult;
}// end of DL_MODE_ESTABLISH_COM_1



//**************************************************************************************************
// @Function      DL_MODE_WuRq_5()
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
static STD_RESULT DL_MODE_WuRq_5(void)
{   
	STD_RESULT nFuncResult = RESULT_OK;
    
    #ifdef DEBUG
        TERMINAL_SendMessage( "DL_MODE -> wake up\n\r");
    #endif
    
    // call PL_WakeUpReq
    if (RESULT_OK == PL_WakeUP())
    {
        nFuncResult = RESULT_OK; 
    }
    else
    {
        nFuncResult = RESULT_NOT_OK;
    }
    
    return nFuncResult;
} // end of DL_MODE_WuRq_5()



//**************************************************************************************************
// @Function      DL_MODE_Request_COM3_6()
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
static STD_RESULT DL_MODE_Request_COM3_6(void)
{   
	STD_RESULT nFuncResult = RESULT_OK;
    
    #ifdef DEBUG
        TERMINAL_SendMessage( "DL_MODE -> Request COM3\n\r");
    #endif
    
    // set mode COM3
    if (RESULT_OK == PL_SetMode(PL_COM3))
    {
        DL_MES_ChangeStateMachine( MH_CONF_COM3 );
		DL_MES_Task();
        nFuncResult = RESULT_OK;
    }
    else
    {
        nFuncResult = RESULT_NOT_OK;
    }
    

    return nFuncResult;
} // end of DL_MODE_Request_COM3_6()



//**************************************************************************************************
// @Function      DL_MODE_Request_COM2_7()
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
static STD_RESULT DL_MODE_Request_COM2_7(void)
{   
	STD_RESULT nFuncResult = RESULT_OK;
    #ifdef DEBUG
        TERMINAL_SendMessage( "DL_MODE -> Request COM2\n\r");
    #endif
    // set mode COM2
    if (RESULT_OK == PL_SetMode(PL_COM2))
    {
        DL_MES_ChangeStateMachine( MH_CONF_COM2 );
		DL_MES_Task();
        nFuncResult = RESULT_OK;
    }
    else
    {
        nFuncResult = RESULT_NOT_OK;
    }
    

    return nFuncResult;
} // end of DL_MODE_Request_COM2_7()



//**************************************************************************************************
// @Function      DL_MODE_Request_COM1_8()
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
static STD_RESULT DL_MODE_Request_COM1_8(void)
{   
	STD_RESULT nFuncResult = RESULT_OK;
    #ifdef DEBUG
        TERMINAL_SendMessage( "DL_MODE -> Request COM1\n\r");
    #endif
    // set mode COM1
    if (RESULT_OK == PL_SetMode(PL_COM1))
    {
        DL_MES_ChangeStateMachine( MH_CONF_COM1 );
		DL_MES_Task();
        nFuncResult = RESULT_OK;
    }
    else
    {
        nFuncResult = RESULT_NOT_OK;
    }
    

    return nFuncResult;
} // end of DL_MODE_Request_COM1_8()



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
    DL_MES_ChangeStateMachine( MH_CONF_PREOPERATE );
	//DL_MES_Task(MH_CONF_PREOPERATE,&response);
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
	DL_MES_ChangeStateMachine( MH_CONF_OPERATE );
	//DL_MES_Task(MH_CONF_OPERATE,&response);
} // end of DL_MODE_Operate_4()
//****************************************** end of file *******************************************
