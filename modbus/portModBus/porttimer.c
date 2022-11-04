//**************************************************************************************************
// @Module        PORT_TIMER
// @Filename      porttimer.c
//--------------------------------------------------------------------------------------------------
// @Platform      STM8S
//--------------------------------------------------------------------------------------------------
// @Compatible    STM8S-discovery board
//--------------------------------------------------------------------------------------------------
// @Description   Implementation of the PORT_TIMER functionality.
//                
//
//                Abbreviations:
//                  None.
//                  
//
//                Global (public) functions:
//                  
//                  
//
//                Local (private) functions:
//                  None.
//                  
//
//--------------------------------------------------------------------------------------------------
// @Version       1.0.0
//--------------------------------------------------------------------------------------------------
// @Date          24.08.2021
//--------------------------------------------------------------------------------------------------
// @History       Version  Author      Comment
// XX.XX.XXXX     1.0.0    KPS         First release.
//**************************************************************************************************



//**************************************************************************************************
// Project Includes
//**************************************************************************************************

#include "port.h"
#include "mb.h"
#include "mbport.h"



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

// None.



//**************************************************************************************************
// Definitions of local (private) constants
//**************************************************************************************************

#define PORT_TIMER_HARD							  TIM1	
#define PORT_TIMER_PERIOD	                     (U16)(0xffff)
#define PORT_TIMER_PRS                           (U16)(3600)
#define PORT_TIMER_REPETION                      (U8)(0) 



//**************************************************************************************************
// Definitions of static global (private) variables
//**************************************************************************************************

// None.


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
// @Function      MBPortTimerInitHard()
//--------------------------------------------------------------------------------------------------
// @Description   HARD Init timer
//--------------------------------------------------------------------------------------------------
// @Notes         None.  
//--------------------------------------------------------------------------------------------------
// @ReturnValue   
//--------------------------------------------------------------------------------------------------
// @Parameters    
//**************************************************************************************************
void MBPortTimerInitHard( void )
{
    
	// Init Hardware timer
    TIM_DeInit(PORT_TIMER_HARD);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseInitStruct.TIM_Period = PORT_TIMER_PERIOD;
    TIM_TimeBaseInitStruct.TIM_Prescaler = PORT_TIMER_PRS;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = PORT_TIMER_REPETION;
    TIM_TimeBaseInit(PORT_TIMER_HARD, &TIM_TimeBaseInitStruct);
    TIM_ARRPreloadConfig(PORT_TIMER_HARD,DISABLE);
	
    
    // TIM1_TimeBaseInit(PORT_TIMER_PRS,
                      // TIM1_COUNTERMODE_UP,                            
                      // usTim1Timerout50us,
                      // PORT_TIMER_REPETION);   
                      
    // TIM1_ITConfig(TIM1_IT_UPDATE, DISABLE);
    
    // // update prescaler
    // TIM1_Cmd(ENABLE);
    // //wait update flag 
    // while (TIM1_GetFlagStatus(TIM1_FLAG_UPDATE) == RESET);
    // TIM1_Cmd(DISABLE);
    

}// end of MBPortTimerInitHard()



//**************************************************************************************************
// @Function      xMBPortTimersInit()
//--------------------------------------------------------------------------------------------------
// @Description   Init timer
//--------------------------------------------------------------------------------------------------
// @Notes         None.  
//--------------------------------------------------------------------------------------------------
// @ReturnValue   true - initialized OK
//                false - initialized not OK
//--------------------------------------------------------------------------------------------------
// @Parameters    usTim1Timerout50us  - 
//**************************************************************************************************
BOOL xMBPortTimersInit( USHORT usTim1Timerout50us  )
{
    // Disable TIM
	TIM_Cmd(PORT_TIMER_HARD, DISABLE);
	// Disable IT
	TIM_ITConfig(PORT_TIMER_HARD, TIM_IT_Update, DISABLE);
	// Set value to Autoreload
	TIM_SetAutoreload(PORT_TIMER_HARD, usTim1Timerout50us);
	// Clear counter
	TIM_SetCounter(PORT_TIMER_HARD, 0);
    //Clear IT flag
	TIM_ClearFlag(PORT_TIMER_HARD, TIM_IT_Update);
	//Enable IT
    TIM_ITConfig(PORT_TIMER_HARD, TIM_IT_Update, ENABLE);
    //Enable TIM
    //TIM_Cmd(PORT_TIMER_HARD, ENABLE);
	
    return TRUE;
}// end of xMBPortTimersInit()



//**************************************************************************************************
// @Function      vMBPortTimersEnable()
//--------------------------------------------------------------------------------------------------
// @Description   Start timer
//--------------------------------------------------------------------------------------------------
// @Notes         None.  
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//                
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
void vMBPortTimersEnable(  )
{
	// Disable TIM
	TIM_Cmd(PORT_TIMER_HARD, DISABLE);
	// Clear counter
	TIM_SetCounter(PORT_TIMER_HARD, 0);
	//Enable TIM
    TIM_Cmd(PORT_TIMER_HARD, ENABLE);
      
}// end of vMBPortTimersEnable



//**************************************************************************************************
// @Function      vMBPortTimersDisable()
//--------------------------------------------------------------------------------------------------
// @Description   Start timer
//--------------------------------------------------------------------------------------------------
// @Notes         None.  
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//                
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
void vMBPortTimersDisable(  )
{
    // Disable TIM
	TIM_Cmd(PORT_TIMER_HARD, DISABLE);
	// Clear counter
	TIM_SetCounter(PORT_TIMER_HARD, 0);

}//end of vMBPortTimersDisable



//**************************************************************************************************
// @Function      vMBPortTimersDelay()
//--------------------------------------------------------------------------------------------------
// @Description   delay ms
//--------------------------------------------------------------------------------------------------
// @Notes		  None. 	
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    usTimeOutMS - value for delay ms
//**************************************************************************************************
void vMBPortTimersDelay( USHORT usTimeOutMS )
{
	// Disable TIM
	TIM_Cmd(PORT_TIMER_HARD, DISABLE);
	// Disable IT
	TIM_ITConfig(PORT_TIMER_HARD, TIM_IT_Update, DISABLE);
	// Set value to Autoreload
	U32 time = usTimeOutMS*20;
	if (time <= 0xffff)
	{
		TIM_SetAutoreload(PORT_TIMER_HARD, (U16)time);
	}
	else
	{
		TIM_SetAutoreload(PORT_TIMER_HARD, 0xffff);
	}
	//Clear IT flag
	TIM_ClearFlag(PORT_TIMER_HARD, TIM_IT_Update);
	//Enable TIM
    TIM_Cmd(PORT_TIMER_HARD, ENABLE);
	
	while( SET != TIM_GetFlagStatus(PORT_TIMER_HARD, TIM_IT_Update));
	
	// Disable TIM
	TIM_Cmd(PORT_TIMER_HARD, DISABLE);
	//Clear IT flag
	TIM_ClearFlag(PORT_TIMER_HARD, TIM_IT_Update);
	
}//end of vMBPortTimersDelay()



//**************************************************************************************************
// @Function      PORT_TIMER_TIM1_ISRx()
//--------------------------------------------------------------------------------------------------
// @Description   interrupt TIMER 1
//--------------------------------------------------------------------------------------------------
// @Notes		  None. 	
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
 void TIM1_UP_IRQHandler()
{   
	pxMBPortCBTimerExpired(  );  
	
	// Disable TIM
	TIM_Cmd(PORT_TIMER_HARD, DISABLE);
	// Disable IT
	TIM_ITConfig(PORT_TIMER_HARD, TIM_IT_Update, DISABLE);
	//Clear IT flag
	TIM_ClearFlag(PORT_TIMER_HARD, TIM_IT_Update);
}
 // end of PORT_TIMER_TIM1_ISRx()



//**************************************************************************************************
//==================================================================================================
// Definitions of local (private) functions
//==================================================================================================
//**************************************************************************************************


// None.


//****************************************** end of file *******************************************
