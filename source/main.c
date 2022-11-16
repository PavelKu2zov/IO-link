//**************************************************************************************************
// @Module        MODULE
// @Filename      module.c
//--------------------------------------------------------------------------------------------------
// @Platform      PLATFORM_NAME
//--------------------------------------------------------------------------------------------------
// @Compatible    COMPATIBLE_PROCESSOR_MODULE
//--------------------------------------------------------------------------------------------------
// @Description   Implementation of the MODULE functionality.
//      if ( Bit_RESET == GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_12))
//    {
//        GPIO_SetBits(GPIOB,GPIO_Pin_12);
//    }
//    else
//    {
//        GPIO_ResetBits(GPIOB,GPIO_Pin_12);
//    }
//
//                Abbreviations:
//                  ABBR0 -
//                  ABBR1 -
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

/* Scheduler includes. */
#include <stm32f10x_gpio.h>
#include "stm32f10x.h"
#include "platform.h"
#include "general_types.h"
#include "Init.h"
#include "IO_LINK.h"
#include "terminal.h"
#include "System_management.h"
#include "DL_MessageHandler.h"
#include "DL_OnRequestDataHandler.h"
#include "Data_layer.h"
#include "Physical_layer.h"
#include "software_timer.h"
#include "checksum.h"

#include "ModuleRAK811.h"


void TestUart(uint32_t baudrate);



//**************************************************************************************************
// Definitions of global (public) variables
//**************************************************************************************************

// None.



//**************************************************************************************************
// Declarations of local (private) data types
//**************************************************************************************************

typedef union FloatU16_union
{
    float f1;
    U16 data[2];
}FloatU16;

//**************************************************************************************************
// Definitions of local (private) constants
//**************************************************************************************************

#define TIME_BEFORE_START       (500000)

#define MODBUS_SLAVE_ADDR		(0x01)

// Size time stamp
#define SIZE_TIME_STAMP         (6U)

// Size payload
#define SIZE_PAYLOAD            (8U)

// Size CRC
#define SIZE_CRC                (1U)

// Size raw data
#define SIZE_RAW_DATA           (SIZE_TIME_STAMP + SIZE_PAYLOAD + SIZE_CRC)


// Step per revolution
static const float StepPerRevolution = 16384;
//Length of wire pulled out per revolution in mm
static const float LenPerRevolution = 332.4;
// factor for calculating the position in mm
static float FactorOfposition; 


//**************************************************************************************************
// Definitions of static global (private) variables
//**************************************************************************************************

static U8 nTimerHandle = 0;

static U8 nTimerHandle_PollEncoder = 0;

static U8 dataPD[25];

static U8 dataISDU[25];

static U8 dataRaw[SIZE_RAW_DATA];

static float position=0;


static U16 reg[64];
static U16 adr = 0;
static U8 coil = 0 ;

FloatU16 floatToU16;

// Current time
static U64 currentTime = 0U;



//**************************************************************************************************
// Declarations of local (private) functions
//**************************************************************************************************

//None.


//**************************************************************************************************
//==================================================================================================
// Definitions of global (public) functions
//==================================================================================================
//**************************************************************************************************



//**************************************************************************************************
// @Function      main()
//--------------------------------------------------------------------------------------------------
// @Description   [description...]
//--------------------------------------------------------------------------------------------------
// @Notes
//--------------------------------------------------------------------------------------------------
// @ReturnValue   returnValue - [description...]
//--------------------------------------------------------------------------------------------------
// @Parameters    parameterZero - [description...]
//**************************************************************************************************

int main(void)
{
	Init();
    SOFTTIMER_HardInit();
	RAK811_init();
	//init Hard timer for ModBus
//	MBPortTimerInitHard();
    GPIO_SetBits(GPIOB,GPIO_Pin_12);// LED COM_LOST not light
    GPIO_SetBits(GPIOB,GPIO_Pin_13);// LED communication MoDBus not light
    FactorOfposition = LenPerRevolution/StepPerRevolution; 
		
    // wait 500 ms
    SOFTTIMER_Create(&nTimerHandle);
    SOFTTIMER_Create(&nTimerHandle_PollEncoder);
	SOFTTIMER_Delay(nTimerHandle,500000);
    SOFTTIMER_EVENT eventPollEncoder = NOT_EXPIRED;
    
     //init ModBus
//    eMBInit( MB_ASCII, MODBUS_SLAVE_ADDR, 2, 115200, MB_PAR_NONE );
    // Enable the Modbus Protocol Stack.
//    eMBEnable();
    
    BOOLEAN recOK = FALSE;
    
    // try to change mode three times
    for (U8 i=0;i<3;i++)
    {
        //Set P2P mode
        RAK811_sendMessage(RAK811_MesP2P_CMD);
        // Did it receive OK?
        SOFTTIMER_StartTimer(nTimerHandle, 500000);
        SOFTTIMER_EVENT event = NOT_EXPIRED;

        while(EXPIRED != event)
        {
            SOFTTIMER_GetEvent(nTimerHandle, &event);
            if ( TRUE == RAK811_IsReceiveOK() )
            {
                recOK = TRUE;
                break;
            }
        }
        if (TRUE == recOK)
        {
            break;
        }
    }
    
    // try to change mode three times
    recOK = FALSE;
    for (U8 i=0;i<3;i++)
    {
        //Set prm RAK811
        RAK811_sendMessage(RAK811_MesPrm);
        // Did it receive OK?
        SOFTTIMER_StartTimer(nTimerHandle, 500000);
        SOFTTIMER_EVENT event = NOT_EXPIRED;

        while(EXPIRED != event)
        {
            SOFTTIMER_GetEvent(nTimerHandle, &event);
            if ( TRUE == RAK811_IsReceiveOK() )
            {
                recOK = TRUE;
                break;
            }
        }
        if (TRUE == recOK)
        {
            break;
        }
    }
    
    // try to change mode three times
    recOK = FALSE;
    for (U8 i=0;i<3;i++)
    {
        //Set transmit mode RAK811
//        RAK811_confTransferMode(RAK811_RECEIVER_MODE);
        RAK811_confTransferMode(RAK811_SENDER_MODE);
        // Did it receive OK?
        SOFTTIMER_StartTimer(nTimerHandle, 500000);
        SOFTTIMER_EVENT event = NOT_EXPIRED;

        while(EXPIRED != event)
        {
            SOFTTIMER_GetEvent(nTimerHandle, &event);
            if ( TRUE == RAK811_IsReceiveOK() )
            {
                recOK = TRUE;
                break;
            }
        }
        if (TRUE == recOK)
        {
            break;
        }
    }
    
    while(1)
    {
		SOFTTIMER_DeInit();
        SOFTTIMER_Create(&nTimerHandle);
        SOFTTIMER_Create(&nTimerHandle_PollEncoder);

         
        //Init System menegemanrt
        SM_Init();    
        //Init Data layer
        DL_Init(); 
        //Init Physical layer
        PL_Init();
        
    
        // start 
        SM_SetPortConfig();
        if (PL_GetCOMState() != COM_STATE_LOST )
        {
            SOFTTIMER_Delay(nTimerHandle,10000);
            // config something   
            //write PDswitch to 0
            dataISDU[0] = 0;
            DL_OD_ISDU_Transport(ISDU_I_SERVICE_INDEX8_SUBINDEX_WR, 120,0,dataISDU,1);
            
            SOFTTIMER_Delay(nTimerHandle,10000);
            // write Counting derection
            dataISDU[0] = 1;
            DL_OD_ISDU_Transport(ISDU_I_SERVICE_INDEX8_SUBINDEX_WR, 85,0,dataISDU,1);
            
            SOFTTIMER_Delay(nTimerHandle,10000);
            // read 
            DL_OD_ISDU_Transport(ISDU_I_SERVICE_INDEX_8_BIT_SUBINDEX, 84,0,dataISDU,0);
            
            SOFTTIMER_Delay(nTimerHandle,10000);

            // Set OPERATE mode
            SM_Operate();    
        }

        
        eventPollEncoder = NOT_EXPIRED;
        SOFTTIMER_StartTimer(nTimerHandle_PollEncoder, 100000);
        do
        {   
            GPIO_SetBits(GPIOB,GPIO_Pin_12); // turn off led
            GPIO_SetBits(GPIOB,GPIO_Pin_13);// turn off led
            
			SOFTTIMER_GetEvent(nTimerHandle_PollEncoder, &eventPollEncoder);
            if (EXPIRED == eventPollEncoder)
            {
                if (PL_GetCOMState() != COM_STATE_LOST )
                {
                    eventPollEncoder = NOT_EXPIRED;
                    PL_ClearReceiveBuffer();
                    DL_MES_Task();
                    DL_MES_Cnf(dataPD, 10);
                    
                    // check com state
    //                if (PL_GetCOMState() == COM_STATE_LOST )
    //                {
    //                    break;
    //                }

                    U32 temp = dataPD[7] | dataPD[6]<<8 | dataPD[5]<<16 | dataPD[4]<<24;
                    position = FactorOfposition * temp;
                    floatToU16.f1 = position;
                    
                    reg[0] = dataPD[4] | (((U16)dataPD[5])<<8);
                    reg[1] = dataPD[6] | (((U16)dataPD[7])<<8);
                    reg[2] = dataPD[0] | (((U16)dataPD[1])<<8);
                    reg[3] = dataPD[2] | (((U16)dataPD[3])<<8);

                    // Get current time
                    SOFTTIMER_GetCurrentTime(&currentTime);
                    
                    currentTime = currentTime / 1000U;

                    // Fill dataRaw by time stamp data
                    for (int nByteNumber = 0U; nByteNumber < SIZE_TIME_STAMP; nByteNumber++)
                    {
                        dataRaw[nByteNumber] = (currentTime >> (((SIZE_TIME_STAMP - 1U) - nByteNumber) * 8U)) & 0xFF;
                    }
                    
                    // Fill dataRaw by dataPD
                    for (int nByteNumber = 0U; nByteNumber < SIZE_PAYLOAD; nByteNumber++)
                    {
                        dataRaw[SIZE_TIME_STAMP + nByteNumber] = dataPD[nByteNumber];
                    }

                    // Calculate CRC8
                    dataRaw[SIZE_TIME_STAMP + SIZE_PAYLOAD] = CH_SUM_CalculateCRC8(dataRaw,
                                                                                SIZE_TIME_STAMP + SIZE_PAYLOAD);
                    
                    // Send data to Lora
                    RAK811_SendRawData(dataRaw, SIZE_RAW_DATA);

//                    // Did it receive OK?
//                    SOFTTIMER_StartTimer(nTimerHandle, 120000);
//                    SOFTTIMER_EVENT event = NOT_EXPIRED;
//                    recOK = FALSE;
//
//                    while(EXPIRED != event)
//                    {
//                        SOFTTIMER_GetEvent(nTimerHandle, &event);
//                        if ( TRUE == RAK811_IsReceiveOK() )
//                        {
//                            recOK = TRUE;
//                            break;
//                        }
//                    }


                    SOFTTIMER_StartTimer(nTimerHandle_PollEncoder, 150000);
                }
            }

        }while((EXPIRED != eventPollEncoder)||(PL_GetCOMState() != COM_STATE_LOST ));
    }
    
}// end of main()
