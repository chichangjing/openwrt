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

#ifndef _MCOMM_H_
#define _MCOMM_H_

#include "Globals.h"

//enum of errors 
typedef enum
{
  MsgOK_e,
  MsgNotReady_e,
  FirstMsgError,
  ChecksumError_e = FirstMsgError,
  MsgSizeError_e,
  TypeVersionError_e,
  MsgTypeError_e,
  ConfigError_e,
  IrqError_e,
  ParamOutOfScope_e,
  MaxCommMsgStatus
    
} e_CommMsgStatus;


//****************************************************************************
//                         DEFINITIONS                                       
//****************************************************************************
//Here is the definition for the shared memory with the host
#define HOST_MSG_PTR &InputMsg
#define MSG_DATA_PTR  InputMsg.Msg.RawData
//definitions for header fields
#define MO_FLAG InputMsg.CommHeader.MsgType.Bits.MO_Flag
#define RW_FLAG InputMsg.CommHeader.MsgType.Bits.RW_Flag
#define MSG_TYPE  InputMsg.CommHeader.MsgType.Bits.StructureType
#define HDR_CHECKSUM    InputMsg.CommHeader.Checksum
#define LENGTH  InputMsg.CommHeader.StructureLength
#define VERSION InputMsg.CommHeader.Structureversion
#define ERROR_VECTOR  InputMsg.CommHeader.CommError.ErrorLong
#define MSG_TYPE_LONG  InputMsg.CommHeader.MsgType.Long

#define HEADER_SIZE_FOR_CHECKSUM  sizeof(t_CommHeader) - sizeof(U32) //checksum
/****************************************************************************
*                            GLOBAL DATA                                    *
*****************************************************************************/
extern U8 ConfigDisable;

/****************************************************************************
*                  EXPORTED FUNCTIONS PROTOTYPES                            *
*****************************************************************************/
e_CommMsgStatus MessageStatus(void);
void SendMessage(void);
void InitComm(void);
U8 HostWriteFormater(void);
U8 HostReadRequest(void);
void HandleIncomingMsg(U8 MsgStatus);
void HandleMsgError(U8 MsgStatus);

/* task func definition */
#ifdef DRAGONITE_TYPE_A1
unsigned __stdcall Task_HostComm(void* cookie);
#else 
void       Task_HostComm(void);
#endif
 

#endif
