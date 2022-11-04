//**************************************************************************************************
// @Module        DL_MES_HANDLER
// @Filename      DL_MessageHandler.c
//--------------------------------------------------------------------------------------------------
// @Platform      stm32f103
//--------------------------------------------------------------------------------------------------
// @Compatible    
//--------------------------------------------------------------------------------------------------
// @Description   Implementation of the DL Message Handler functionality.
//
//
//                Abbreviations:
//                  none.
//
//
//                Global (public) functions:
//						DL_MES_Init()
//						DL_MES_Task()
//						DL_MES_ChangeStateMachine()
//
//
//                Local (private) functions:
//						DL_MES_Inactive_0()
//						DL_MES_AwaitReply_1()
//						DL_MES_StartUp_2()
//						DL_MES_Response_3()
//						DL_MES_Preoperate_6()
//						DL_MES_GetOD_7()
//						DL_MES_Response_8()
//						DL_MES_CheckHandler_11()
//						DL_MES_Operate_12()
//						DL_MES_GetPD_13()
//						DL_MES_GetOD_14()
//						DL_MES_Response_15()
//						DL_MES_CalCKT()
//
//--------------------------------------------------------------------------------------------------
// @Version       1.0.0
//--------------------------------------------------------------------------------------------------
// @Date          30.08.2021
//--------------------------------------------------------------------------------------------------
// @History       Version  Author      Comment
// XX.XX.XXXX     1.0.0    KPS         First release.
//**************************************************************************************************



//**************************************************************************************************
// Project Includes
//**************************************************************************************************

// Native header
#include "DL_MessageHandler.h"
#include "Physical_layer.h"
#include "IO_LINK.h"
#include "software_timer.h"
#include "terminal.h"

// circle buffer
#include "circ_buffer.h"

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

// state of machinestate
typedef enum DL_MES_TypeStateMachine_enum
{
    DL_MES_INACTIVE_0=0,
    DL_MES_AWAITREPLY_1,
    DL_MES_STARTUP_2,
    DL_MES_RESPONSE_3,
    DL_MES_PREOPERATE_6,
    DL_MES_GETOD_7,
    DL_MES_RESPONSE_8,
	DL_MES_CHECKHANDLER_11,
    DL_MES_OPERATE_12,
    DL_MES_GETPD_13,
	DL_MES_GETOD_14,
	DL_MES_RESPONSE_15
}DL_MES_TypeStateMachine;

// type for event req message
typedef enum DL_MES_EVENT_REQ_enum
{
    EXIST=0,
    NOT_EXIST
}DL_MES_EVENT_REQ;

// type for REQ message
typedef struct DL_MES_PRM_REQ_str
{
    U8 addr;
    const U8 *data;
    DL_MES_EVENT_REQ event;
}DL_MES_PRM_REQ;



//**************************************************************************************************
// Definitions of local (private) constants
//**************************************************************************************************

#define DL_MES_SIZE_BUFF_DATA_TO_TR             (64)

#define DL_MES_SIZE_BUFF_DATA_REC               (64)
//**************************************************************************************************
// Definitions of static global (private) variables
//**************************************************************************************************

static DL_MES_TypeStateMachine DL_MES_StateMachine; 

// software timer 
static uint8_t nTimerHandle_MH = 0;

static uint8_t dataToTransfer[DL_MES_SIZE_BUFF_DATA_TO_TR];

static uint8_t dataReceive[DL_MES_SIZE_BUFF_DATA_REC];

// buffer for DL_read request from SM
static DL_MES_PRM_REQ buff_DL_read_req = {0,0,NOT_EXIST};

// buffer for DL_write request from SM
static DL_MES_PRM_REQ buff_DL_write_req = {0,0,NOT_EXIST};

// current COM
static U8 currentBaudrate = BAUDRATE_NONE;

static DL_MES_RESPONSE_TYPE	DL_MES_Response = {NOT_OK,0};
// call from DL_MODE
static U8 currentCall = MH_CONF_NONE;

//**************************************************************************************************
// Declarations of local (private) functions
//**************************************************************************************************

static void DL_MES_Inactive_0(void);
static void DL_MES_AwaitReply_1( void );
static void DL_MES_StartUp_2( void );
static void DL_MES_Response_3( void );
static void DL_MES_Preoperate_6( void );
static void DL_MES_GetOD_7( void );
static void DL_MES_Response_8( void );
static void DL_MES_CheckHandler_11( void );
static void DL_MES_Operate_12( void );
static void DL_MES_GetPD_13( void );
static void DL_MES_GetOD_14( void );
static void DL_MES_Response_15( void );
static U16 GetTbit(U8 baudRate);

//**************************************************************************************************
//==================================================================================================
// Definitions of global (public) functions
//==================================================================================================
//**************************************************************************************************

//**************************************************************************************************
// @Function      DL_MES_Init()
//--------------------------------------------------------------------------------------------------
// @Description   Init DL Message handler
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
STD_RESULT DL_MES_Init(void)
{
    STD_RESULT nFuncResult = RESULT_OK;
    
    currentBaudrate = BAUDRATE_NONE;
    
    DL_MES_StateMachine = DL_MES_INACTIVE_0;
    
    // Create timer nTimerHandle_MH
    if (RESULT_OK == SOFTTIMER_Create(&nTimerHandle_MH))
    {
    #ifdef DEBUG
        TERMINAL_SendMessage("DL_Message Handler was initialized\n\r");
    #endif  
        nFuncResult = RESULT_OK;
    }
    else
    {
        nFuncResult = RESULT_NOT_OK;
    }
    
    return nFuncResult;
    
}// end of DL_MES_Init()



//**************************************************************************************************
// @Function      DL_MES_ChangeStateMachine()
//--------------------------------------------------------------------------------------------------
// @Description   None.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
void DL_MES_ChangeStateMachine( const U8 call )
{
	// to apply call and chose next step of the state machine
    currentCall = call;
    switch (call)
    {
        case MH_CONF_COM1: DL_MES_StateMachine = DL_MES_AWAITREPLY_1; break;
        
        case MH_CONF_COM2: DL_MES_StateMachine = DL_MES_AWAITREPLY_1; break;
        
        case MH_CONF_COM3: DL_MES_StateMachine = DL_MES_AWAITREPLY_1; break;
        
        case MH_CONF_STARTUP: DL_MES_StateMachine = DL_MES_STARTUP_2; break;
        
        case MH_CONF_PREOPERATE: DL_MES_StateMachine = DL_MES_PREOPERATE_6; break;
        
        case MH_CONF_OPERATE: DL_MES_StateMachine = DL_MES_OPERATE_12; break;
        
        case MH_CONF_ACTIVE: break;
        
        case MH_CONF_INACTIVE: DL_MES_StateMachine = DL_MES_INACTIVE_0;break;
        
        default: break;
    }
}// end of DL_MES_ChangeStateMachine()



//**************************************************************************************************
// @Function      DL_MES_Run()
//--------------------------------------------------------------------------------------------------
// @Description   None.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
void DL_MES_Task( void )
{
    // // move in state machine
    switch (DL_MES_StateMachine)
    {
        case DL_MES_INACTIVE_0: DL_MES_Inactive_0();break;
        
        case DL_MES_AWAITREPLY_1: DL_MES_AwaitReply_1();
                if (BAUDRATE_COM3 == currentBaudrate)
                {
                    DL_MES_StateMachine = DL_MES_STARTUP_2;
                }break;
        
        case DL_MES_STARTUP_2: DL_MES_StartUp_2();break;
        
        case DL_MES_RESPONSE_3: DL_MES_Response_3();break;
        
        case DL_MES_PREOPERATE_6: DL_MES_Preoperate_6();break;
        
        case DL_MES_GETOD_7: DL_MES_GetOD_7();break;
        
        case DL_MES_RESPONSE_8: DL_MES_Response_8();break;
        
        case DL_MES_CHECKHANDLER_11: DL_MES_CheckHandler_11();break;
        
        case DL_MES_OPERATE_12: DL_MES_Operate_12();break;
        
        case DL_MES_GETPD_13: DL_MES_GetPD_13();break;
        
        case DL_MES_GETOD_14: DL_MES_GetOD_14();break;
        
        case DL_MES_RESPONSE_15: DL_MES_Response_15();break;
        
        default: break;
    }
    
    
}// end of DL_MES_Run()



//**************************************************************************************************
// @Function      DL_MES_Req_DL_Read
//--------------------------------------------------------------------------------------------------
// @Description   None.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
void DL_MES_Req_DL_Read(const U8* addr)
{
    buff_DL_read_req.addr = *addr;
    buff_DL_read_req.event = EXIST;
}



//**************************************************************************************************
// @Function      DL_MES_Req_DL_Write
//--------------------------------------------------------------------------------------------------
// @Description   None.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
void DL_MES_Req_DL_Write(const U8* addr, const U8* data)
{
    buff_DL_write_req.addr = *addr;
    buff_DL_write_req.data = data;
    buff_DL_write_req.event = EXIST;
}



//**************************************************************************************************
// @Function      DL_MES_GetResponse_DL_Read_req
//--------------------------------------------------------------------------------------------------
// @Description   None.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
DL_MES_RESPONSE_TYPE DL_MES_GetResponse_DL_Read_req( void )
{
    return DL_MES_Response;
}// end of DL_MES_GetResponse_DL_Read_req()



//**************************************************************************************************
// @Function      DL_MES_GetCurrentCom
//--------------------------------------------------------------------------------------------------
// @Description   None.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
U8 DL_MES_GetCurrentCom( void )
{
	return currentBaudrate;
}	
// end of DL_MES_GetCurrentCom()



//**************************************************************************************************
// @Function      DL_MES_GetResponse_DL_Read_req
//--------------------------------------------------------------------------------------------------
// @Description   None.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
void DL_MES_ODreq(const U8* data, const U16* size)
{
    U16 sizeData=0;
    PL_TransferReq( data, *size );
    
    // calculate Tm-sequense
    if ( DL_MES_PREOPERATE_6 == DL_MES_StateMachine ) // TYPE_1_V 8 byte
    {
        if ((*data & MC_READ) == MC_READ)
        {
            // wait 
            
            SOFTTIMER_Delay(nTimerHandle_MH,Tm_sequence(TYPE_1_V_MC_CKT,TYPE_1_V_8_OD_CKS, GetTbit(currentBaudrate)));
            PL_GetTransferInd( dataReceive, TYPE_1_V_8_OD_CKS );
            sizeData = TYPE_1_V_8_OD_CKS;
        }
        else// MC_WRITE
        {
            // wait 
            SOFTTIMER_Delay(nTimerHandle_MH,Tm_sequence(TYPE_1_V_MC_CKT_8_OD,TYPE_1_V_CKS,GetTbit(currentBaudrate)));
            PL_GetTransferInd( dataReceive, TYPE_1_V_CKS );
            sizeData = TYPE_1_V_CKS;
        }      
    }
    else if (DL_MES_OPERATE_12 == DL_MES_StateMachine)
    {
        
    }
    
    // send OD.cnf
    DL_OD_Cnf( dataReceive, sizeData );
    
}// end of DL_MES_ODreq()



//**************************************************************************************************
// @Function      DL_MES_Cnf()
//--------------------------------------------------------------------------------------------------
// @Description   None.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
void DL_MES_Cnf(U8* data, const U16 size)
{
	 for (U8 i=1;i< size;i++)
	{
		*data = dataReceive[i];
        data++;
	}
}// end of DL_MES_Cnf()



//**************************************************************************************************
// @Function      DL_MES_CalCKT
//--------------------------------------------------------------------------------------------------
// @Description   Function calculates check Sum and then compresses to 6 bit
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    data - pointer to data
//                size - size of data 
//                typeMseq - type of M-sequence   
//**************************************************************************************************
U8 DL_MES_CalCKT( uint8_t* data, const uint16_t size )
{
    uint8_t checkSum = 0x52;
    uint8_t checkSum6Bits = 0;
    uint8_t CKT = 0;
    
    for (uint32_t i=0;i<size;i++)
    {
        checkSum = checkSum ^ *data;
        data++;    
    }
    
    
    // comprassing to 6 bits
    uint8_t tm = checkSum >> 1;
    tm = tm ^ checkSum;
    checkSum6Bits |= tm & 0x1;
    checkSum6Bits |= (tm >> 1) & 0x2;
    checkSum6Bits |= (tm >> 2) & 0x4;
    checkSum6Bits |= (tm >> 3) & 0x8;
    checkSum6Bits |= (checkSum ^ (checkSum>>2) ^ (checkSum<<2) ^ (checkSum<<4))&0x30;
    
    CKT = checkSum6Bits;/* | ((typeMseq<<6)&0xc0);*/
    return CKT;
}
// end of DL_MES_CalCKT()



//**************************************************************************************************
//==================================================================================================
// Definitions of local (private) functions
//==================================================================================================
//**************************************************************************************************

//**************************************************************************************************
// @Function	  DL_MES_Inactive_0
//--------------------------------------------------------------------------------------------------
// @Description   None.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
static void DL_MES_Inactive_0(void)
{
	
}
// end of DL_MES_Inactive_0()



//**************************************************************************************************
// @Function      DL_MES_AwaitReply_1
//--------------------------------------------------------------------------------------------------
// @Description   None.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    mode - INACTIVE,DI,DO,COM1,COM2,COM3
//**************************************************************************************************
static void DL_MES_AwaitReply_1( void )
{
    PL_ClearReceiveBuffer();
    
    if (MH_CONF_COM3 == currentCall)
    {
        // read MinCycleTime‚ TYPE_0
        dataToTransfer[0] = 0xA2; // MC
        dataToTransfer[1] = 0x00 | TYPE_0_M_SEQ_CKT_BITS; // CKT
        dataToTransfer[1] |= DL_MES_CalCKT( dataToTransfer, 1 )&0x3F;

        // Send data with mode COM3
        PL_TransferReq( dataToTransfer,2 );
        //wait Tdmt_COM2    
		SOFTTIMER_Delay(nTimerHandle_MH,Tm_sequence(2,2,Tbit_COM3));
		
		U8 datatemp = 0;
		// exist ever one byte 
		if (RESPONSE_OK == PL_GetTransferInd( &datatemp, 1))
		{
			currentBaudrate = BAUDRATE_COM3;
		}
    }
    else if (MH_CONF_COM2 == currentCall)
    {
        // // read MinCycleTime‚ TYPE_0
        // dataToTransfer[0] = 0xA2; // MC
        // dataToTransfer[1] = 0x00 | TYPE_0_M_SEQ_CKT_BITS; // CKT
        // dataToTransfer[1] |= DL_MES_CalCKT( dataToTransfer, 1 )&0x3F;

        // // Send data with mode COM2
        // PL_TransferReq( dataToTransfer,2 );
        
        // // set mode PL_DI to receive data
        // previousPLTargetMode = PL_GetTargetMode();
        // PL_SetMode(PL_DI);
        
        // if (RESULT_OK == SOFTTIMER_StartTimer(nTimerHandle_MH,Tm_sequence(2,2,Tbit_COM2)))
        // {
            // SOFTTIMER_EVENT event = NOT_EXPIRED;
            // //wait Tm_sequence
            // while(EXPIRED != event)
            // {
                // SOFTTIMER_GetEvent(nTimerHandle_MH, &event);
            // }

            // //get data form PL
            // if (RESULT_OK == PL_GetTransferInd( dataReceive, 2 ))
            // {
                
                // *response = RESPONSE_COM2;
            // }
            // else
            // {
                // *response = RESPONSE_NOT_OK;
               
            // }
        // }
        // else
        // {
           // ;
        // }
        // PL_SetMode(previousPLTargetMode);
    }
    else if (MH_CONF_COM1 == currentCall)
    {
        // // read MinCycleTime‚ TYPE_0
        // dataToTransfer[0] = 0xA2; // MC
        // dataToTransfer[1] = 0x00 | TYPE_0_M_SEQ_CKT_BITS; // CKT
        // dataToTransfer[1] |= DL_MES_CalCKT( dataToTransfer, 1 )&0x3F;

        // // Send data with mode COM1
        // PL_TransferReq( dataToTransfer,2 );
        
        // // set mode PL_DI to receive data
        // previousPLTargetMode = PL_GetTargetMode();
        // PL_SetMode(PL_DI);
        
        // if (RESULT_OK == SOFTTIMER_StartTimer(nTimerHandle_MH,Tm_sequence(2,2,Tbit_COM1)))
        // {
            // SOFTTIMER_EVENT event = NOT_EXPIRED;
            // //wait Tm_sequence
            // while(EXPIRED != event)
            // {
                // SOFTTIMER_GetEvent(nTimerHandle_MH, &event);
            // }

            // //get data form PL
            // if (RESULT_OK == PL_GetTransferInd( dataReceive, 2 ))
            // {   
                // *response = RESPONSE_COM1;
            // }
            // else
            // {
                // *response = RESPONSE_NOT_OK;
                
            // }
        // }
        // else
        // {
            // ;
        // }
        // PL_SetMode(previousPLTargetMode);
    }
	
}
// end of DL_MES_AwaitReply_1()



//**************************************************************************************************
// @Function      DL_MES_StartUp_2
//--------------------------------------------------------------------------------------------------
// @Description   None.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
static void DL_MES_StartUp_2( void )
{
    U8 data = 0;
	U8 sizeToread = 1;
    if ( EXIST == buff_DL_read_req.event )
    {
        if (buff_DL_read_req.addr <= 0x1f)
        {
            // reading 
            dataToTransfer[0] = MC_READ | MC_CHANNEL_PAGE | buff_DL_read_req.addr; // MC
            dataToTransfer[1] = 0x00 | TYPE_0_M_SEQ_CKT_BITS; // CKT
            dataToTransfer[1] |= DL_MES_CalCKT( dataToTransfer,2  )&0x3F;
            PL_TransferReq( dataToTransfer, 2);
            
            // wait 
            SOFTTIMER_Delay(nTimerHandle_MH,Tm_sequence(TYPE_0_MC_CKT,TYPE_0_OD_CKS,GetTbit(currentBaudrate)));
            
            if ( RESPONSE_OK == PL_GetTransferInd(&data,sizeToread))
			{
				DL_MES_Response.state = OK;
				DL_MES_Response.data = data;
			}
			else
			{
				DL_MES_Response.state = NOT_OK;
			}

            buff_DL_read_req.event = NOT_EXIST;
        }
    }
    else if ( EXIST == buff_DL_write_req.event )
    {
        sizeToread = 3;
		dataToTransfer[0] = MC_WRITE | MC_CHANNEL_PAGE | buff_DL_write_req.addr; // MC
        dataToTransfer[1] = 0x00 | TYPE_0_M_SEQ_CKT_BITS; // CKT
        dataToTransfer[2] = *buff_DL_write_req.data; 
        dataToTransfer[1] |= DL_MES_CalCKT( dataToTransfer,3 )&0x3F;
        PL_TransferReq( dataToTransfer, sizeToread);
        
        // wait 
        SOFTTIMER_Delay(nTimerHandle_MH,Tm_sequence(TYPE_0_MC_CKT_OD,TYPE_0_CKS,GetTbit(currentBaudrate)));
		
		if ( RESPONSE_OK == PL_GetTransferInd(&data,sizeToread))
		{
			DL_MES_Response.state = OK;
		}
		else
		{
			DL_MES_Response.state = NOT_OK;
		}
      
        buff_DL_write_req.event = NOT_EXIST;
    }
}
// end of DL_MES_StartUp_2()



//**************************************************************************************************
// @Function      DL_MES_Response_3
//--------------------------------------------------------------------------------------------------
// @Description   None.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
static void DL_MES_Response_3( void )
{
	
}
// end of DL_MES_Response_3()



//**************************************************************************************************
// @Function      DL_MES_Preoperate_6
//--------------------------------------------------------------------------------------------------
// @Description   None.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
static void DL_MES_Preoperate_6( void )
{
	
	U8 data = 0;
	U8 sizeToread = 1;
    if ( EXIST == buff_DL_read_req.event )
    {
        // if (buff_DL_read_req.addr <= 0x1f)
        // {
            // // reading 
            // dataToTransfer[0] = MC_READ | MC_CHANNEL_PAGE | buff_DL_read_req.addr; // MC
            // dataToTransfer[1] = 0x00 | TYPE_0_M_SEQ_CKT_BITS; // CKT
            // dataToTransfer[1] |= DL_MES_CalCKT( dataToTransfer,2  )&0x3F;
            // PL_TransferReq( dataToTransfer, 2);
            
            // // wait 
            // SOFTTIMER_Delay(nTimerHandle_MH,Tm_sequence(TYPE_0_MC_CKT,TYPE_0_OD_CKS,GetTbit(currentBaudrate)));
            
            // if ( RESPONSE_OK == PL_GetTransferInd(&data,sizeToread))
			// {
				// DL_MES_Response.state = OK;
				// DL_MES_Response.data = data;
			// }
			// else
			// {
				// DL_MES_Response.state = NOT_OK;
			// }

            // buff_DL_read_req.event = NOT_EXIST;
        // }
    }
    else if ( EXIST == buff_DL_write_req.event )
    {
		dataToTransfer[0] = MC_WRITE | MC_CHANNEL_PAGE | buff_DL_write_req.addr; // MC
        dataToTransfer[1] = 0x00 | TYPE_1_M_SEQ_CKT_BITS; // CKT
        dataToTransfer[2] = *buff_DL_write_req.data;
		dataToTransfer[3] = 0;
		dataToTransfer[4] = 0;
		dataToTransfer[5] = 0;
		dataToTransfer[6] = 0;
		dataToTransfer[7] = 0;
		dataToTransfer[8] = 0;
		dataToTransfer[9] = 0;		
        dataToTransfer[1] |= DL_MES_CalCKT( dataToTransfer,10 )&0x3F;
        PL_TransferReq( dataToTransfer, TYPE_1_V_MC_CKT_8_OD);
        
        // wait 
        SOFTTIMER_Delay(nTimerHandle_MH,Tm_sequence(TYPE_1_V_MC_CKT_8_OD,TYPE_1_V_CKS,GetTbit(currentBaudrate)));
		
		if ( RESPONSE_OK == PL_GetTransferInd(&data,TYPE_1_V_CKS))
		{
			DL_MES_Response.state = OK;
		}
		else
		{
			DL_MES_Response.state = NOT_OK;
		}
      
        buff_DL_write_req.event = NOT_EXIST;
    }
    
    // // Send command DEVICE_OPERATE
    // dataToTransfer[0] = 0x20; // MC
    // dataToTransfer[1] = 0x00 | TYPE_1_M_SEQ_CKT_BITS; // CKT
    // dataToTransfer[2] = DEVICE_OPERATE;
    // dataToTransfer[3] = 0;
    // dataToTransfer[4] = 0;
    // dataToTransfer[5] = 0;
    // dataToTransfer[6] = 0;
    // dataToTransfer[7] = 0;
    // dataToTransfer[8] = 0;
    // dataToTransfer[9] = 0;
    // dataToTransfer[1] |= DL_MES_CalCKT( dataToTransfer, 10 )&0x3F;
    // PL_TransferReq( dataToTransfer,10 );
    
    // //wait 1 s
    // SOFTTIMER_Delay(nTimerHandle_MH,1000000);
    
    // // Send command DEVICE_PROCESS_DATA_OUTPUT_OPERATE
    // dataToTransfer[0] = 0x20; // MC
    // dataToTransfer[1] = 0x00 | TYPE_2_M_SEQ_CKT_BITS; // CKT
    // dataToTransfer[2] = DEVICE_PROCESS_DATA_OUTPUT_OPERATE;
    // dataToTransfer[1] |= DL_MES_CalCKT( dataToTransfer, 3 )&0x3F;
    // PL_TransferReq( dataToTransfer,3);
    
    // //wait 1 s
    // SOFTTIMER_Delay(nTimerHandle_MH,1000000);    
    
    // while(1)
    // {
        // // read PD
        // dataToTransfer[0] = 0x80; // MC
        // dataToTransfer[1] = 0x00 | TYPE_2_M_SEQ_CKT_BITS; // CKT
        // dataToTransfer[1] |= DL_MES_CalCKT( dataToTransfer, 2 )&0x3F;
        // PL_TransferReq( dataToTransfer,2);

        // //wait 1 s
        // SOFTTIMER_Delay(nTimerHandle_MH,1000000);
    // }
    
}
// end of DL_MES_Preoperate_6()



//**************************************************************************************************
// @Function      DL_MES_GetOD_7
//--------------------------------------------------------------------------------------------------
// @Description   None.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
static void DL_MES_GetOD_7( void )
{
	
}
// end of DL_MES_GetOD_7()



//**************************************************************************************************
// @Function      DL_MES_Response_8
//--------------------------------------------------------------------------------------------------
// @Description   None.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
static void DL_MES_Response_8( void )
{
	
}
// end of DL_MES_Response_8()



//**************************************************************************************************
// @Function      DL_MES_CheckHandler_11
//--------------------------------------------------------------------------------------------------
// @Description   None.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
static void DL_MES_CheckHandler_11( void )
{
	
}
// end of DL_MES_CheckHandler_11()



//**************************************************************************************************
// @Function      DL_MES_Operate_12
//--------------------------------------------------------------------------------------------------
// @Description   None.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
static void DL_MES_Operate_12( void )
{
	// if exsit call from DL_read or DL_write
	if (( EXIST == buff_DL_read_req.event ) || ( EXIST == buff_DL_write_req.event ))
	{
		if ( EXIST == buff_DL_read_req.event )
		{
			// if (buff_DL_read_req.addr <= 0x1f)
			// {
				// // reading 
				// dataToTransfer[0] = MC_READ | MC_CHANNEL_PAGE | buff_DL_read_req.addr; // MC
				// dataToTransfer[1] = 0x00 | TYPE_0_M_SEQ_CKT_BITS; // CKT
				// dataToTransfer[1] |= DL_MES_CalCKT( dataToTransfer,2  )&0x3F;
				// PL_TransferReq( dataToTransfer, 2);
				
				// // wait 
				// SOFTTIMER_Delay(nTimerHandle_MH,Tm_sequence(TYPE_0_MC_CKT,TYPE_0_OD_CKS,GetTbit(currentBaudrate)));
				
				// if ( RESPONSE_OK == PL_GetTransferInd(&data,sizeToread))
				// {
					// DL_MES_Response.state = OK;
					// DL_MES_Response.data = data;
				// }
				// else
				// {
					// DL_MES_Response.state = NOT_OK;
				// }

				// buff_DL_read_req.event = NOT_EXIST;
			// }
		}
		else if ( EXIST == buff_DL_write_req.event )
		{
			dataToTransfer[0] = MC_WRITE | MC_CHANNEL_PAGE | buff_DL_write_req.addr; // MC
			dataToTransfer[1] = 0x00 | TYPE_2_M_SEQ_CKT_BITS; // CKT
			dataToTransfer[2] = *buff_DL_write_req.data;	
			dataToTransfer[1] |= DL_MES_CalCKT( dataToTransfer,3 )&0x3F;
			PL_TransferReq( dataToTransfer, TYPE_2_V_MC_CKT_OD);

			// wait 
			SOFTTIMER_Delay(nTimerHandle_MH,Tm_sequence(TYPE_2_V_MC_CKT_OD,TYPE_2_V_8PD_CKS,GetTbit(currentBaudrate)));
            
			
            
			if ( RESPONSE_OK == PL_GetTransferInd(dataReceive,TYPE_2_V_8PD_CKS))
			{
				DL_MES_Response.state = OK;
			}
			else
			{
				DL_MES_Response.state = NOT_OK;
			}

			buff_DL_write_req.event = NOT_EXIST;
		}
	}
	else
	{
		// get PD
		DL_MES_GetPD_13();
		
		// get OD
		DL_MES_GetOD_14();
		
		dataToTransfer[0] = MC_READ | MC_CHANNEL_PROCESS | 0; // MC
		dataToTransfer[1] = 0x00 | TYPE_2_M_SEQ_CKT_BITS; // CKT
		dataToTransfer[1] |= DL_MES_CalCKT( dataToTransfer,2 )&0x3F;
		PL_TransferReq( dataToTransfer, TYPE_2_V_MC_CKT);
        
		// wait 
		SOFTTIMER_Delay(nTimerHandle_MH,Tm_sequence(TYPE_2_V_MC_CKT,TYPE_2_V_OD_8PD_CKS,GetTbit(currentBaudrate)));
             
        
		if ( RESPONSE_OK == PL_GetTransferInd(dataReceive,TYPE_2_V_OD_8PD_CKS))
		{
		DL_MES_Response.state = OK;
		}
		else
		{
		DL_MES_Response.state = NOT_OK;
		}

        
		buff_DL_write_req.event = NOT_EXIST;
		
	}
}
// end of DL_MES_Operate_12()



//**************************************************************************************************
// @Function      DL_MES_GetPD_13
//--------------------------------------------------------------------------------------------------
// @Description   None.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
static void DL_MES_GetPD_13( void )
{
	
}
// end of DL_MES_GetPD_13()



//**************************************************************************************************
// @Function      DL_MES_GetOD_14
//--------------------------------------------------------------------------------------------------
// @Description   None.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
static void DL_MES_GetOD_14( void )
{
	
}
// end of DL_MES_GetOD_14()



//**************************************************************************************************
// @Function      DL_MES_Response_15
//--------------------------------------------------------------------------------------------------
// @Description   None.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.
//**************************************************************************************************
static void DL_MES_Response_15( void )
{
	
}
// end of DL_MES_Response_15()



//**************************************************************************************************
// @Function      GetTbit
//--------------------------------------------------------------------------------------------------
// @Description   None.
//--------------------------------------------------------------------------------------------------
// @Notes         None.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   None.
//--------------------------------------------------------------------------------------------------
// @Parameters    None.         
//**************************************************************************************************
static U16 GetTbit(U8 baudRate)
{
    U16 result = 0;
    
    if (BAUDRATE_COM3 == baudRate)
    {
        result = Tbit_COM3;
    }
    else if (BAUDRATE_COM2 == baudRate)
    {
        result = Tbit_COM2;
    }
        else if (BAUDRATE_COM1 == baudRate)
    {
        result = Tbit_COM1;
    }
    
    return result;
    
}// end of GetTbit
//****************************************** end of file *******************************************
