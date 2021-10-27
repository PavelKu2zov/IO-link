//**************************************************************************************************
// @Module        PL
// @Filename      PL_cfg.h
//--------------------------------------------------------------------------------------------------
// @Description   Configuration of the required functionality of the PL module.
//                Physical layer of Io-Link
//--------------------------------------------------------------------------------------------------
// @Version       1.0.0
//--------------------------------------------------------------------------------------------------
// @Date          XX.XX.XXXX
//--------------------------------------------------------------------------------------------------
// @History       Version  Author      Comment
// XX.XX.XXXX     1.0.0    XXX         First release.
//**************************************************************************************************

#ifndef PL_CFG_H
#define PL_CFG_H



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



// User can enable/disable...
// Valid values: ON / OFF
#define MODULE_PARAMETER_ZERO                   (OFF)

// User can
// Valid values: [x ; y)
#define MODULE_PARAMETER_ONE                    (0x01U)

// User can specify...
// Valid values: (a ; b]
#define MODULE_PARAMETER_TWO                    (-2)



#endif // #ifndef PL_CFG_H

//****************************************** end of file *******************************************
