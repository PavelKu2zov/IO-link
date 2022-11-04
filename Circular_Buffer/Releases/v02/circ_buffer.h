//**************************************************************************************************
// @Module        Circular buffer (CIRCBUF)
// @Filename      circ_buffer.h
//--------------------------------------------------------------------------------------------------
// @Description   This file contains all defines, data structures and function prototypes for the
//                circular buffer.
//--------------------------------------------------------------------------------------------------
// @Version       1.0.1
//--------------------------------------------------------------------------------------------------
// @Date          14.06.2011
//--------------------------------------------------------------------------------------------------
// @History       Version  Author    Comment
//                1.0.0    ASD       First release.
// 14.06.2011     1.0.1    ASD       Adapted for a lot of MISRA-C 2004 rules.
//**************************************************************************************************

#ifndef CIRC_BUFFER_H
#define CIRC_BUFFER_H



//**************************************************************************************************
// Project Includes
//**************************************************************************************************

// Define data types
#include "compiler.h"



//**************************************************************************************************
// Verification of the imported configuration parameters
//**************************************************************************************************

// None.



//**************************************************************************************************
// Declarations of global (public) data types
//**************************************************************************************************

// Define type for circular buffer size
typedef U16 CIRCBUF_SZ;

// Define type of the structure of a circular buffer
typedef struct stCIRCBUF_struct 
{
    const void*     pData;      // Pointer to data of the buffer
    U8              itemSize;   // Item size in bytes
    CIRCBUF_SZ      size;       // Size of the buffer in items
    CIRCBUF_SZ      head;       // Head of the buffer, where data is prepended
    CIRCBUF_SZ      tail;       // Tail of the buffer, where data is removed from
} stCIRCBUF;

// Definition of all errors for a circular buffer
typedef enum 
{
    CIRCBUF_NO_ERR,         // No error occurred
    CIRCBUF_EMPTY,          // Buffer is empty
    CIRCBUF_OVERFLOW,       // Buffer overflow
    CIRCBUF_SIZE_TOO_SMALL, // Size of the buffer is too small
    CIRCBUF_NOT_EXIST,      // Buffer does not exist
    CIRCBUF_DATA_NOT_EXIST  // Data of the buffer do not exist
} enCIRCBUF_ERR;



//**************************************************************************************************
// Definitions of global (public) constants
//**************************************************************************************************

// None.



//**************************************************************************************************
// Declarations of global (public) variables
//**************************************************************************************************

// None.



//**************************************************************************************************
// Declarations of global (public) functions
//**************************************************************************************************

extern enCIRCBUF_ERR CIRCBUF_Init(stCIRCBUF* const pBuf,
                                  const void* const pData,
                                  const CIRCBUF_SZ size);

extern enCIRCBUF_ERR CIRCBUF_PutData(const void* const pData,
                                     stCIRCBUF* const pBuf);

extern enCIRCBUF_ERR CIRCBUF_GetData(const void* const pData,
                                     stCIRCBUF* const pBuf);

extern enCIRCBUF_ERR CIRCBUF_Purge(stCIRCBUF* const pBuf);

extern CIRCBUF_SZ CIRCBUF_GetNumberOfItems(const stCIRCBUF* const pBuf);

extern CIRCBUF_SZ CIRCBUF_GetFreeSize(const stCIRCBUF* const pBuf);



//**************************************************************************************************
// Declarations of interrupt service routines
//**************************************************************************************************

// None.



#endif // #ifndef CIRC_BUFFER_H

//****************************************** end of file *******************************************
