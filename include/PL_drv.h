//**************************************************************************************************
// @Module        PL
// @Filename      PL_drv.h
//--------------------------------------------------------------------------------------------------
// @Description   Interface of the PL module.
//
//--------------------------------------------------------------------------------------------------
// @Version       1.0.0
//--------------------------------------------------------------------------------------------------
// @Date          30.08.2021
//--------------------------------------------------------------------------------------------------
// @History       Version  Author      Comment
// XX.XX.XXXX     1.0.0    KPS         First release.
//**************************************************************************************************

#ifndef PL_DRV_H
#define PL_DRV_H



//**************************************************************************************************
// Project Includes
//**************************************************************************************************

// None.


//**************************************************************************************************
// Declarations of global (public) data types
//**************************************************************************************************

// Services of Physical layer 
typedef enum 
{
    PL_SETMODE = 0,
    PL_WAKEUP,
	PL_TRANSFER
}PL_SERVICE_enum;

// Target mode
typedef enum PL_TARGET_MODE_enum
{
	INACTIVE=0,
	DI,
	D,
	COM1,
	COM2,
	COM3
}PL_TARGET_MODE;


// type message for queue
typedef enum PL_TYPE_MES_QUEUE_enum
{
    PL_SET_MODE=0,
    PL_WAKE_UP,
    PL_TRANSFER_COM1,
    PL_TRANSFER_COM2,
    PL_TRANSFER_COM3
}PL_TYPE_MES_QUEUE;

// structure mes queue
typedef struct PL_MES_QUEUE_struct
{
    PL_TYPE_MES_QUEUE typeMes;
    uint32_t size;
    uint8_t *p;    
}PL_MES_QUEUE;

//**************************************************************************************************
// Definitions of global (public) constants
//**************************************************************************************************

#define PL_SIZE_DATA		(uint16_t)(255) 		


//**************************************************************************************************
// Declarations of global (public) variables
//**************************************************************************************************

uint8_t PL_aData[PL_SIZE_DATA];


//**************************************************************************************************
// Declarations of global (public) functions
//**************************************************************************************************

// Init Physical layer
void PL_Init(void);

// Task of Physikal layer
void PL_Task(void *pvParameters)

// Wake up service
void PL_WakeUP(void);

// Set target mode service
void PL_SetMode(PL_TARGET_MODE mode);

// Transfer data service
void PL_Transfer( void );


#ifndef PL_DRV_H

//****************************************** end of file *******************************************
