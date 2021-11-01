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
    DL_MODE_Idle_0=0,
    DL_MODE_WURQ_5,
    DL_MODE_ComRequestCOM3_6,
    DL_MODE_ComRequestCOM2_7,
    DL_MODE_ComRequestCOM1_8,
    DL_MODE_Retry_9,
    DL_MODE_Startup_2,
    DL_MODE_PreOperate_3,
    DL_MODE_Operate_4
}DL_MODE_StateMachine;



//**************************************************************************************************
// Definitions of local (private) constants
//**************************************************************************************************

// [Description...]
#define MODULE_CONST_TWO    (0x01U)

// [Description...]
#define MODULE_CONST_THREE  (-1)

// [Description...]
static const DATA_TYPE MODULE_constFour = 0x02U;



//**************************************************************************************************
// Definitions of static global (private) variables
//**************************************************************************************************

// [Description...]
static DATA_TYPE MODULE_variableTwo;

// [Description...]
static DATA_TYPE MODULE_variableThree;



//**************************************************************************************************
// Declarations of local (private) functions
//**************************************************************************************************

// [Description...]
static DATA_TYPE MODULE_FunctionTwo(DATA_TYPE parameterZero,
                                    DATA_TYPE parameterOne);

// [Description...]
static DATA_TYPE MODULE_FunctionThree(DATA_TYPE parameterZero,
                                      DATA_TYPE parameterOne,
                                      DATA_TYPE parameterTwo);



//**************************************************************************************************
//==================================================================================================
// Definitions of global (public) functions
//==================================================================================================
//**************************************************************************************************



//**************************************************************************************************
// @Function      MODULE_FunctionZero()
//--------------------------------------------------------------------------------------------------
// @Description   [description...]
//--------------------------------------------------------------------------------------------------
// @Notes
//--------------------------------------------------------------------------------------------------
// @ReturnValue   returnValue - [description...]
//--------------------------------------------------------------------------------------------------
// @Parameters    parameterZero - [description...]
//**************************************************************************************************
DATA_TYPE MODULE_FunctionZero(DATA_TYPE parameterZero)
{
    DATA_TYPE returnValue;
    
    // [Description...]
    if (a > b)
    {
        moreCnt++;
    }
    else if (a < b)
    {
        lessCnt++;
    }
    else
    {
        equalCnt++;
    }
    
    return returnValue;
    
} // end of MODULE_FunctionZero()



//**************************************************************************************************
// @Function      MODULE_FunctionOne()
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
DATA_TYPE MODULE_FunctionOne(DATA_TYPE parameterZero,
                             DATA_TYPE parameterOne)
{
    DATA_TYPE returnValue;
    
    // [Description...]
    DATA_TYPE idx = 0;
    for ( ; idx < MAX; idx++)
    {
        arrayResult[idx] = arrayInitial[idx];
    }
    
    return returnValue;
    
} // end of MODULE_FunctionOne()



//**************************************************************************************************
// @Function      MODULE_ISRx()
//--------------------------------------------------------------------------------------------------
// @Description   [description...]
//--------------------------------------------------------------------------------------------------
// @Notes
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
INTERRUPT(VectorNumber) void MODULE_ISRx()
{

} // end of MODULE_ISR0()



//**************************************************************************************************
//==================================================================================================
// Definitions of local (private) functions
//==================================================================================================
//**************************************************************************************************



//**************************************************************************************************
// @Function      MODULE_FunctionTwo()
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
static DATA_TYPE MODULE_FunctionTwo(DATA_TYPE parameterZero,
                                    DATA_TYPE parameterOne)
{
    DATA_TYPE returnValue;
    
    // [Description...]
    switch (expression)
    {
        case CASE_ONE:
            caseOneCnt++;
            break;

        case CASE_TWO:
            caseTwoCnt++;
            break;

        default:
            caseDefaultCnt++;
            break;
    } // end of switch (expression)
    
    return returnValue;
    
} // end of MODULE_FunctionTwo()



//**************************************************************************************************
// @Function      MODULE_FunctionThree()
//--------------------------------------------------------------------------------------------------
// @Description   [description...]
//--------------------------------------------------------------------------------------------------
// @Notes
//--------------------------------------------------------------------------------------------------
// @ReturnValue   returnValue - [description...]
//--------------------------------------------------------------------------------------------------
// @Parameters    parameterZero - [description...]
//                parameterOne  - [description...]
//                parameterTwo  - [description...]
//**************************************************************************************************
static DATA_TYPE MODULE_FunctionThree(DATA_TYPE parameterZero,
                                      DATA_TYPE parameterOne,
                                      DATA_TYPE parameterTwo)
{
    DATA_TYPE returnValue;
    
    // [Place code here]
    
    return returnValue;
    
} // end of MODULE_FunctionThree()



//****************************************** end of file *******************************************
