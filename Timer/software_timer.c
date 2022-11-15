//**************************************************************************************************
// @Module        SOFTWARE_TIMER
// @Filename      software_timer.c
//--------------------------------------------------------------------------------------------------
// @Platform      PLATFORM_NAME
//--------------------------------------------------------------------------------------------------
// @Compatible    COMPATIBLE_PROCESSOR_MODULE
//--------------------------------------------------------------------------------------------------
// @Description   Implementation of the MODULE functionality.
//                [Description...]
//
//                Abbreviations:
//                  
//                  
//
//                Global (public) functions:
//                  
//                  
//
//                Local (private) functions:
//                  
//                  
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
#include <stm32f10x_tim.h>
#include <misc.h>
#include "software_timer.h"
//#include "intrinsics.h"


//**************************************************************************************************
// Verification of the imported configuration parameters
//**************************************************************************************************

// None.



//**************************************************************************************************
// Definitions of global (public) variables
//**************************************************************************************************

// None.



//**************************************************************************************************
// Declarations of local (private) data types
//**************************************************************************************************

typedef struct SOFTTIMER_STATUS_str
{
    // Software timer initialization state
    uint8_t bCreated :1;
    // Software timer state
    uint8_t nState:7;
}SOFTTIMER_STATUS;

typedef struct SW_PARAM_str
{
    SOFTTIMER_STATUS   Status;
    
    SOFTTIMER_SZ       Old_Value_Counter; 
    
    SOFTTIMER_SZ       nPeriod;
    
    SOFTTIMER_EVENT    Event;  
    
}SOFTTIMER_PARAM;



//**************************************************************************************************
// Definitions of local (private) constants
//**************************************************************************************************

// Software timer states
#define SOFTTIMER_STATE_STOPPED     (0U)
#define SOFTTIMER_STATE_RUNNING     (1U)
#define SOFTTIMER_STATE_BLOCKED     (2U)



//**************************************************************************************************
// Definitions of static global (private) variables
//**************************************************************************************************

static SOFTTIMER_SZ SOFTTIMER_Timer_MSB_Bits = 0;

// Software timer parameters
static SOFTTIMER_PARAM SOFTTIMER_Param[SOFTTIMER_QTY];

static U8 SOFTTIMER_IRQ_exist = 0;

//**************************************************************************************************
// Declarations of local (private) functions
//**************************************************************************************************

static BOOLEAN SOFTTIMER_IsTimerNumberValid(const uint8_t nTimerHandle);



//**************************************************************************************************
//==================================================================================================
// Definitions of global (public) functions
//==================================================================================================
//**************************************************************************************************



//**************************************************************************************************
// @Function      SOFTTIMER_HardInit()
//--------------------------------------------------------------------------------------------------
// @Description   Timer starts to count with power on. Resolution is microsecond. Timer finishs to 
//                count with power off
//--------------------------------------------------------------------------------------------------
// @Notes
//--------------------------------------------------------------------------------------------------
// @ReturnValue   returnValue - [description...]
//--------------------------------------------------------------------------------------------------
// @Parameters    parameterZero - [description...]
//**************************************************************************************************
void SOFTTIMER_HardInit(void)
{
    SOFTTIMER_Timer_MSB_Bits = 0;
    SOFTTIMER_IRQ_exist = 0;
    // Init Hardware timer
    TIM_DeInit(SOFTTIMER_HARD_NUM);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseInitStruct.TIM_Period = SOFTTIMER_HARD_TIMER_PERIOD;
    TIM_TimeBaseInitStruct.TIM_Prescaler = SOFTTIMER_HARD_TIMER_PRESCALER;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x0000;
    TIM_TimeBaseInit(SOFTTIMER_HARD_NUM, &TIM_TimeBaseInitStruct);
    TIM_ARRPreloadConfig(SOFTTIMER_HARD_NUM,DISABLE);
    
    // Enable interrupt
    TIM_ITConfig(SOFTTIMER_HARD_NUM, TIM_IT_Update, ENABLE);
    
    // NVIC configuration
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Configure the preemption priority and subpriority:
     - 1 bits for pre-emption priority: possible value are 0 or 1 
     - 3 bits for subpriority: possible value are 0..7
     - Lower values gives higher priority  
    */

    /* Enable the TIM2_IRQn Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_Cmd(SOFTTIMER_HARD_NUM, ENABLE);
    
}//end SOFTTIMER_HardInit()



//**************************************************************************************************
// @Function      SOFTTIMER_Create()
//--------------------------------------------------------------------------------------------------
// @Description   Creates new timer and returns it's number.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   RESULT_OK     - function succeeded
//                RESULT_NOT_OK - function NOT succeeded
//--------------------------------------------------------------------------------------------------
// @Parameters    pTimerHandle - pointer to the output timer number               
//**************************************************************************************************
STD_RESULT SOFTTIMER_Create(uint8_t* const pTimerHandler)
{
    STD_RESULT nFuncResult = RESULT_OK;
    
    // Check pointer
    if (0 != pTimerHandler) 
    {
        uint8_t nTimerNum;
        // Find free timer
        for (nTimerNum = 0U; nTimerNum < SOFTTIMER_QTY; ++nTimerNum)
        {
            if (FALSE == SOFTTIMER_Param[nTimerNum].Status.bCreated)
            {
                // Initialize timer
                SOFTTIMER_Param[nTimerNum].Status.bCreated   = TRUE;
                SOFTTIMER_Param[nTimerNum].Status.nState     = SOFTTIMER_STATE_STOPPED;
                SOFTTIMER_Param[nTimerNum].nPeriod           = 0U;
                
                // Return timer number
                *pTimerHandler = nTimerNum;

                nFuncResult = RESULT_OK;
                break;
            }
        }
    }
    else
    {
        nFuncResult = RESULT_NOT_OK;
    }
    
    return nFuncResult;
}// end of SOFTIMER_Create()



//**************************************************************************************************
// @Function      SOFTTIMER_Create()
//--------------------------------------------------------------------------------------------------
// @Description   Creates new timer and returns it's number.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   RESULT_OK     - function succeeded
//                RESULT_NOT_OK - function NOT succeeded
//--------------------------------------------------------------------------------------------------
// @Parameters    pTimerHandle - pointer to the output timer number               
//**************************************************************************************************
STD_RESULT SOFTTIMER_DeInit(void)
{
    STD_RESULT nFuncResult = RESULT_OK;
    

	uint8_t nTimerNum;
	// Find free timer
	for (nTimerNum = 0U; nTimerNum < SOFTTIMER_QTY; ++nTimerNum)
	{
		// Initialize timer
		SOFTTIMER_Param[nTimerNum].Status.bCreated   = FALSE;
		SOFTTIMER_Param[nTimerNum].Status.nState     = SOFTTIMER_STATE_STOPPED;
		SOFTTIMER_Param[nTimerNum].nPeriod           = 0U;
	}

    
    return nFuncResult;
}// end of SOFTIMER_Create()



//**************************************************************************************************
// @Function      SOFTTIMER_StartTimer()
//--------------------------------------------------------------------------------------------------
// @Description   Sets the software timer period and starts the timer.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   RESULT_OK     - function succeeded
//                RESULT_NOT_OK - function NOT succeeded
//--------------------------------------------------------------------------------------------------
// @Parameters    nTimerHandle - timer number to start
//                nTimerPeriod - timer period value to set
//**************************************************************************************************
STD_RESULT SOFTTIMER_StartTimer(const uint8_t     nTimerHandle,
                                const SOFTTIMER_SZ nTimerPeriod)
{
    STD_RESULT nFuncResult = RESULT_OK;
    
    // Check timer number
    if (TRUE == SOFTTIMER_IsTimerNumberValid(nTimerHandle))
    {
        // Checking whether initialization is done or not
        if (TRUE == SOFTTIMER_Param[nTimerHandle].Status.bCreated)
        {
            SOFTTIMER_SZ timeCurrent=0;
            if ( RESULT_OK == SOFTTIMER_GetCurrentTime(&timeCurrent) )
            {
                // Store current value time
                SOFTTIMER_Param[nTimerHandle].Old_Value_Counter = timeCurrent;
                 
                // Initialize timer parameters
                SOFTTIMER_Param[nTimerHandle].nPeriod = nTimerPeriod;
               
                // Start the timer
                SOFTTIMER_Param[nTimerHandle].Status.nState = SOFTTIMER_STATE_RUNNING;
                
                SOFTTIMER_Param[nTimerHandle].Event = NOT_EXPIRED;
                        
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
    }
    else
    {
        nFuncResult = RESULT_NOT_OK;
    }
    
    return nFuncResult;
}// end of SOFTTIMER_StartTimer()



//**************************************************************************************************
// @Function      SOFTTIMER_GetEvent()
//--------------------------------------------------------------------------------------------------
// @Description   [description...]
//--------------------------------------------------------------------------------------------------
// @Notes
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
STD_RESULT SOFTTIMER_GetEvent(const uint8_t nTimerHandler, SOFTTIMER_EVENT *const event)
{
    STD_RESULT nFuncResult = RESULT_OK;
    
    // Check timer number
    if (TRUE == SOFTTIMER_IsTimerNumberValid(nTimerHandler))
    {
        // Checking whether initialization is done or not
        if (TRUE == SOFTTIMER_Param[nTimerHandler].Status.bCreated)
        {
            SOFTTIMER_SZ timeCurrent=0;
            if ( RESULT_OK == SOFTTIMER_GetCurrentTime(&timeCurrent))
            {
                if ( SOFTTIMER_STATE_RUNNING == SOFTTIMER_Param[nTimerHandler].Status.nState )
                {
                    if ( SOFTTIMER_Param[nTimerHandler].nPeriod <= (timeCurrent - SOFTTIMER_Param[nTimerHandler].Old_Value_Counter))
                    {
                        SOFTTIMER_Param[nTimerHandler].Event = EXPIRED;
                        *event = EXPIRED;
                        SOFTTIMER_Param[nTimerHandler].Status.nState = SOFTTIMER_STATE_STOPPED;
                    }
                    else
                    {
                        SOFTTIMER_Param[nTimerHandler].Event = NOT_EXPIRED;
                    }
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
}// end of SOFTTIMER_GetEvent()


//**************************************************************************************************
// @Function      TIM2_IRQHandler()
//--------------------------------------------------------------------------------------------------
// @Description   [description...]
//--------------------------------------------------------------------------------------------------
// @Notes
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
void TIM2_IRQHandler(void)
{
    // increment msb timer
    SOFTTIMER_Timer_MSB_Bits++;
    
    //clear flag
    TIM_ClearFlag(SOFTTIMER_HARD_NUM, TIM_IT_Update);
    SOFTTIMER_IRQ_exist = 1;
    
} // end of TIM2_IRQHandler()


//**************************************************************************************************
// @Function      SOFTTIMER_Delay
//--------------------------------------------------------------------------------------------------
// @Description   None.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.         
//**************************************************************************************************
void SOFTTIMER_Delay(const uint8_t  nTimerHandle,const SOFTTIMER_SZ nTimerPeriod)
{
    if (RESULT_OK == SOFTTIMER_StartTimer(nTimerHandle,nTimerPeriod))
    {
        SOFTTIMER_EVENT event = NOT_EXPIRED;
       
        while(EXPIRED != event)
        {
            SOFTTIMER_GetEvent(nTimerHandle, &event);
        }
    } 
    
}// end of SOFTTIMER_Delay()



//**************************************************************************************************
// @Function      SOFTTIMER_GetCurrentTime()
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
STD_RESULT SOFTTIMER_GetCurrentTime(SOFTTIMER_SZ *const timeCurrent)
{
    STD_RESULT nFuncResult = RESULT_OK;



    if ( (SOFTTIMER_HARD_NUM->CR1 & TIM_CR1_CEN) == TIM_CR1_CEN )
    {
        SOFTTIMER_IRQ_exist = 0;
        *timeCurrent = (SOFTTIMER_Timer_MSB_Bits << SOFTTIMER_HARD_TIMER_RESOLUTION) + SOFTTIMER_HARD_NUM->CNT;

        if (SOFTTIMER_IRQ_exist == 1)
        {
            *timeCurrent = (SOFTTIMER_Timer_MSB_Bits << SOFTTIMER_HARD_TIMER_RESOLUTION) + SOFTTIMER_HARD_NUM->CNT;
            SOFTTIMER_IRQ_exist = 0;
        }
        nFuncResult = RESULT_OK;
    }
    else
    {
        nFuncResult = RESULT_NOT_OK;
    }



    return nFuncResult;
} // end of SOFTTIMER_GetCurrentTime()



//**************************************************************************************************
//==================================================================================================
// Definitions of local (private) functions
//==================================================================================================
//**************************************************************************************************



//**************************************************************************************************
// @Function      SOFTTIMER_IsTimerNumberValid()
//--------------------------------------------------------------------------------------------------
// @Description   Checks the timer number value.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   TRUE  - returns if the timer number IS valid
//                FALSE - returns if the timer number is NOT valid
//--------------------------------------------------------------------------------------------------
// @Parameters    nTimerHandle - timer number
//**************************************************************************************************

static BOOLEAN SOFTTIMER_IsTimerNumberValid(const uint8_t nTimerHandle)
{
    return (BOOLEAN)(nTimerHandle < SOFTTIMER_QTY);
} // end of SOFTTIMER_IsTimerNumberValid()



//****************************************** end of file *******************************************
