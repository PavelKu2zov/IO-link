//**************************************************************************************************
// @Module        IO_LINK
// @Filename      IO_Link.h
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

#ifndef IO_LINK_H
#define IO_LINK_H



//**************************************************************************************************
// Project Includes
//**************************************************************************************************

// times


// MinCycleTime in us
#define MIN_CYCLE_TIME				        (uint16_t)(3200)

// Tbit - time of one bit in us 
#define Tbit_COM3                           (4)//4,34 us        230400 kbit/s
#define Tbit_COM2                           (26)//26,04 us      38400 kbit/s
#define Tbit_COM1                           (208)//208,33 us    4800 kbit/s    

#define Tdmt_K                              (27)// 27...37

// Master message delay
// min,max 117,18..159,47 us
#define Tdmt_COM3                           (Tdmt_K*Tbit_COM3)//132 us

// min,max 703,08..963,48 us
#define Tdmt_COM2                           (Tdmt_K*Tbit_COM2)//858 us - 290 us

// min,max 5624,91..7708,21 us
#define Tdmt_COM1                           (Tdmt_K*Tbit_COM1)//6864 us - 2000 us

// Duration of Master's wake-up current pulse 75..85 us
#define Twu				                    (75)

// Receive enable delay max 500 us
#define Tren				                (400)

//Wake-up retry delay 30..50 ms
#define Tdwu				                (40000)

//Wake-up retry count 
#define Nwu				                    (3)	

// time reaction of device in us
#define TaK                                 (8)//1..10
#define Ta(Tbit)                            (uint16_t)(TaK*Tbit)

#define T1                                  (10)
#define T2                                  (10)

// m - number frames UART
// n - number frames UART from device
#define Tm_sequence(m,n,Tbit)               ((m+n)*11*Tbit+Ta(Tbit)+(m-1)*T1+(n-1)*T2)                 

// Data later mode
#define DL_START_UP                    (U8)(0x00)

#define DL_PREOPERATE                  (U8)(0x01)

#define DL_OPERATE                     (U8)(0x02)

#define DL_INACTIVE                    (U8)(0x03)

// DL Message Hanadler's calls
#define MH_CONF_NONE                   (U8)(0x00)
                                       
#define MH_CONF_COM1                   (U8)(0x01) 
                                       
#define MH_CONF_COM2                   (U8)(0x02) 
                                       
#define MH_CONF_COM3                   (U8)(0x03) 
                                      
#define MH_CONF_STARTUP                (U8)(0x04)
                                       
#define MH_CONF_PREOPERATE             (U8)(0x05)  
                                       
#define MH_CONF_OPERATE                (U8)(0x06)
                                       
#define MH_CONF_ACTIVE                 (U8)(0x07)
                                       
#define MH_CONF_INACTIVE               (U8)(0x08)

// DL OD_Data Hnadler's calls
#define OH_CONF_INACTIVE               (U8)(0x09)

#define OH_CONF_ACTIVE                 (U8)(0x0A)

// DL OD_Data_ISDU Hanadler's calls
#define IH_CONF_ACTIVE                 (U8)(0x0B)

#define IH_CONF_INACTIVE               (U8)(0x0C)

// Responses 
#define RESPONSE_NOT_OK                (U8)(0x00)
#define RESPONSE_OK            		   (U8)(0x01)

// Baudrate IO-LINK 
#define BAUDRATE_NONE                  (U8)(0x01)
#define BAUDRATE_COM1                  (U8)(0x02)
#define BAUDRATE_COM2                  (U8)(0x03)
#define BAUDRATE_COM3                  (U8)(0x04)

// Physical layer mode
#define PL_INACTIVE                       (0x00) 
#define PL_DI                             (0x01)
#define PL_D                              (0x02)  
#define PL_COM1                           (0x03)
#define PL_COM2                           (0x04)
#define PL_COM3                           (0x05) 

// I_SERVICE ISDU
#define ISDU_I_SERVICE_INDEX8_WR				(U8)(0x01)
#define ISDU_I_SERVICE_INDEX8_SUBINDEX_WR		(U8)(0x02)
#define ISDU_I_SERVICE_INDEX16_SUBINDEX_WR		(U8)(0x03)


#define ISDU_I_SERVICE_INDEX_8_BIT              (U8)(0x09)
#define ISDU_I_SERVICE_INDEX_8_BIT_SUBINDEX     (U8)(0x0A)
#define ISDU_I_SERVICE_INDEX_16_BIT_SUBINDEX    (U8)(0x0B)    

// FlowCTRL ISDU
#define ISDU_FLOW_CTRL_START					(U8)(0x10)
#define ISDU_FLOW_CTRL_IDLE						(U8)(0x11)
#define ISDU_FLOW_CTRL_ABORT					(U8)(0x1F)
#define ISDU_FLOW_CTRL_COUNT(cnt)				(U8)(cnt)// from 0x00 to 0x0F

// Length data ISDU with different I_SERVICE
#define ISDU_LEN_INDEX_8_BIT                    (U8)(0x03)
#define ISDU_LEN_NOT_EX_INDEX8_SUBINDEX_WR(len) (U8)(len)
#define ISDU_LEN_8_BIT_SUBINDEX                 (U8)(0x04)

// TYPE M-seq CKT bits
#define TYPE_0_M_SEQ_CKT_BITS                   (U8)(0x00)
#define TYPE_1_M_SEQ_CKT_BITS                   (U8)((0x01) << 6)
#define TYPE_2_M_SEQ_CKT_BITS                   (U8)((0x02) << 6)

// Control byte MC                                        
#define MC_READ                                 (U8)(0x01 << 7)
#define MC_WRITE                                (U8)(0x00 << 7)
#define MC_CHANNEL_PROCESS                      (U8)(0x00 << 5)
#define MC_CHANNEL_PAGE                         (U8)(0x01 << 5) 
#define MC_CHANNEL_DIAGNOSTIC                   (U8)(0x02 << 5)                                        
#define MC_CHANNEL_ISDU                         (U8)(0x03 << 5)
                                        
// some parameters IO_link specification
#define IO_LINK_SIZE_PAGE_1                     (U8)(0x10)  
#define IO_LINK_SIZE_PAGE_2                     (U8)(0x10)

// address parameters page 1
#define MASTER_COMMAND                          (U8)(0x00)    
#define MASTER_CYCLE_TIME_ADDR                  (U8)(0x01)
#define MIN_CYCLE_TIME_ADDR                     (U8)(0x02)
#define M_SEQUENCE_CAPABILITY                   (U8)(0x03)
#define REVISION_ID                             (U8)(0x04)
#define PROCESS_DATA_IN                         (U8)(0x05)
#define PROCESS_DATA_OUT                        (U8)(0x06)
#define VENDOR_ID_1                             (U8)(0x07)
#define VENDOR_ID_2                             (U8)(0x08)
#define DEVICE_ID_1                             (U8)(0x09)
#define DEVICE_ID_2                             (U8)(0x0A)
#define DEVICE_ID_3                             (U8)(0x0B)
#define FUNCTION_ID_1                           (U8)(0x0C)
#define FUNCTION_ID_2                           (U8)(0x0D)
#define SYSTEM_COMMAND                          (U8)(0x0F)    

// Command IO_LINC spec
#define DEVICE_MASTERIDENT                      (U8)(0x95) 
#define DEVICE_PREOPERATE                       (U8)(0x9A)  
#define DEVICE_OPERATE                          (U8)(0x99)  
#define DEVICE_PROCESS_DATA_OUTPUT_OPERATE      (U8)(0x98)

// Parameters M-seq
#define TYPE_0_MC_CKT                		 	(U8)(0x02)
#define TYPE_0_MC_CKT_OD  		               	(U8)(0x03)
#define TYPE_0_OD_CKS	  		               	(U8)(0x02)
#define TYPE_0_CKS		  		               	(U8)(0x01)

#define TYPE_1_V_MC_CKT			                (U8)(0x02)
#define TYPE_1_V_MC_CKT_8_OD			        (U8)(0x0A)
#define TYPE_1_V_CKS			                (U8)(0x01)
#define TYPE_1_V_8_OD_CKS			            (U8)(0x09)

#define TYPE_2_V_MC_CKT			                (U8)(0x02)//master read
#define TYPE_2_V_OD_8PD_CKS				        (U8)(0x0A)//device answer
#define TYPE_2_V_MC_CKT_OD				        (U8)(0x03)//master write
#define TYPE_2_V_8PD_CKS					  	(U8)(0x09)//device answer

// ISDU
#define ISDU_WRITE								(U8)(0x01)
#define ISDU_READ								(U8)(0x02)


#define DELEY_WITHOUT_IRQ                      (0x7782f)//(0x7772f) // delay without IRQ

#define COM_STATE_LOST							(U8)(0x01)
#define COM_STATE_NOT_LOST							(U8)(0x02)				
//**************************************************************************************************
// Declarations of global (public) data types
//**************************************************************************************************

// None.



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

// None.



#endif // IO_LINK_H

//****************************************** end of file *******************************************
