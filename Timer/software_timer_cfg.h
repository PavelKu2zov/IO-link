//**************************************************************************************************
// @Module        SOFTWARE_TIMER
// @Filename      software_timer_cfg.h
//--------------------------------------------------------------------------------------------------
// @Description   Configuration of the required functionality of the MODULE module.
//                [Description...]
//--------------------------------------------------------------------------------------------------
// @Version       1.0.0
//--------------------------------------------------------------------------------------------------
// @Date          XX.XX.XXXX
//--------------------------------------------------------------------------------------------------
// @History       Version  Author      Comment
// XX.XX.XXXX     1.0.0    XXX         First release.
//**************************************************************************************************

#ifndef SOFTWARE_TIMER_CFG_H
#define SOFTWARE_TIMER_CFG_H



//**************************************************************************************************
// Definitions of global (public) constants
//**************************************************************************************************

// Enable/disable the development error detection feature of the program module.
// Used only for debug purposes and should be disabled in the final release.
// Valid values: ON / OFF
#define MODULE_DEVELOPMENT_ERROR_DETECTION      (OFF)

// User can enable/disable the internal diagnostic of the program module.
// Used only for debug purposes and should be disabled in the final release.
// Valid values: ON / OFF
#define MODULE_INTERNAL_DIAGNOSTICS             (OFF)



// Type timer 
#define SOFTTIMER_SZ                             U64
        
// number hard timer
#define SOFTTIMER_HARD_NUM                        TIM2

// timer period
#define SOFTTIMER_HARD_TIMER_PERIOD              (0xFFFF)

// timer presceler
#define SOFTTIMER_HARD_TIMER_PRESCALER           (71)

// hardware timer resolution
#define SOFTTIMER_HARD_TIMER_RESOLUTION          (16)

// Number soft timers
#define SOFTTIMER_QTY                            (20)


#endif // #ifndef SOFTWARE_TIMER_CFG_H

//****************************************** end of file *******************************************
