/*===============================================================================================*/
/*  THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF                                               */
/*  MICROSEMI COP.- ANALOG MIXED SIGNAL GROUP LTD. ("AMSG")                                      */
/*  AND IS SUBJECT TO A NON-DISCLOSURE AGREEMENT                                                 */
/*  NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT,                                               */
/*  MASK WORK RIGHT OR COPYRIGHT OF AMSG OR ANY THIRD PARTY.                                     */
/*  AMSG RESERVES THE RIGHT AT ITS SOLE                                                          */
/*  DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO AMSG.                        */
/*  THIS CODE IS PROVIDED "AS IS". AMSG MAKES NO WARRANTIES, EXPRESSED,                          */
/*  IMPLIEDOR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.                    */
/*  THIS SOURCE CODE, MAY NOT BE DISCLOSED TO ANY THIRD PARTY OR USED IN ANY OTHER MANNER,       */
/*  AND KNOWLEDGE DERIVED THEREFROM OR CANNOT BE USED TO WRITE ANY PROGRAM OR CODE.              */
/*  USE IS PERMITTED ONLY PURSUANT TO WRITTEN AGREEMENT SIGNED BY AMSG.                          */
/*  KNOWLEDGE OF THIS FILE MAY UNDER NO CIRCUMSTANCES BE USED TO WRITE ANY PROGRAM.              */
/*===============================================================================================*/

#ifndef _MNGR_HOST_IF_H_
#define _MNGR_HOST_IF_H_

#ifdef _VISUALC
    /* Disable warning messages */
    #pragma warning( disable : 4214 )
#endif /* _VISUALC */

#include "Globals.h"
//****************************************************************************
//                         DEFINITIONS                                       
//****************************************************************************
#define MAX_DATA_SIZE 2304 //maximum size of data from host, in bytes
#define IRQ_TO_HOST IRQTOHOST



//enum of all msg types between host & dragonite
typedef enum
{
  Config_e = 0,
  System_e,
  Chip_e,
  PortCmd_e,
  PortCounters_e,
  PortStatuses_e,
  PortMeasurments_e,
  PortL2_e,
  DebugMsg_e,
  Other_e,
  MaxCommMsgType_e
} e_CommMsgType;

//union of the second DWORD in the header. holds 3 fields
typedef union
{
    struct
    {
        U32 StructureType :30;      
        U32 RW_Flag     :1;
        U32 MO_Flag     :1;
    } Bits;
    U32 Long;
} t_MsgType;

//each bit represent a different error type
typedef union
{
  struct
  {
    U32 ChecksumError     :1;
    U32 LengthError       :1;
    U32 TypeVersionError  :1;
    U32 TypeUnknownError  :1;
    U32 ParamValueError   :1;
  } ErrorBitFields;
  U32 ErrorLong;
} t_CommError;
  
typedef struct
{
  U16 Length;
  U8 Version;
} TypeData;

// Header struct for all comm messages host <-> dragonite
typedef struct 
{
    U32           Checksum;
    t_MsgType     MsgType;
    U32           StructureLength;
    U32           Structureversion; // 
    t_CommError   CommError;
    U32           Reserved;

}t_CommHeader;

// this union holds all the structs of possible messages from host or to host.
// the actual type that is used will be specified in the header
typedef struct
{
    U8 RawData[MAX_DATA_SIZE];
}t_CommMsgData;


//the complete msg
typedef struct host_rxtx
{
    t_CommHeader     CommHeader;                    
    t_CommMsgData    Msg;         

}t_HostMsg;


extern const TypeData MsgTypeData[];
//actual shared memory
extern t_HostMsg InputMsg;

#endif
