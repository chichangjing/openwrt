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


//*****************************************************************************
//                   IMPORTED  DECLARATIONS                                  
//*****************************************************************************
#include "MCom.h"


//****************************************************************************
//                         DEFINITIONS                                       
//****************************************************************************


//****************************************************************************
//                         LOCAL FUNCTIONS DECLARATIONS                                       
//****************************************************************************
U32 Checksum(U8 *Ptr);

//=============================================================================
//Function      :   HandleIncomingMsg
//Description   :   this function handles an incoming message from host
//Parameters    :   MsgStatus
//=============================================================================
void HandleIncomingMsg(U8 MsgStatus)
{

  if(RW_FLAG) //write request
  {
    if(MsgStatus == MsgOK_e)
    {
      //process according to msg type
      MsgStatus = HostWriteFormater(); 
    }
    else if((MSG_TYPE == Config_e) && (!ConfigDisable))
    {
      //if wasn't configured yet & there is a msg error, config error should also be reported
      HandleMsgError(ConfigError_e);
    }
    //prepare for interrupt to host
    COMM_WRITE_FLAG = 1;
  }
  else // read request
  {
    if(MsgStatus == MsgOK_e)
    {
      //process according to msg type
      MsgStatus = HostReadRequest(); 
    }
    //prepare for interrupt to host
    COMM_READ_FLAG = 1;
  }
  //report error to host
  HandleMsgError(MsgStatus);
  //send reply
  SendMessage();
  
}
//=============================================================================
//Function      :   HandleMsgError
//Description   :   this function handles a message error
//Parameters    :   MsgStatus
//=============================================================================
void HandleMsgError(U8 MsgStatus)
{
  if(MsgStatus != MsgOK_e)
  {
    if(MsgStatus != ParamOutOfScope_e)
    //update status counter
    COMM_ERROR(MsgStatus)++;
    //update msg error field
    ERROR_VECTOR |= (1 << (MsgStatus - FirstMsgError));
  }
  else
    ERROR_VECTOR = 0;
}
//=============================================================================
//Function      :   MessageStatus
//Description   :   this function handles a message error
//Parameters    :   none
//=============================================================================
e_CommMsgStatus MessageStatus(void)
{    
  U32  Type; /* must be 32 bits to avoid wanring in Type = MSG_TYPE; 
              * I prefere not to use there customization to avoid possible
              * problems with different endianess
              */
  
  //check possible errors & return status accordingly
  if(!MO_FLAG)
    return MsgNotReady_e; //host didn't write anything
  
  //send pointer for start of checksum
  if(HDR_CHECKSUM != Checksum((U8*)&MSG_TYPE_LONG))
    return ChecksumError_e;

  Type = MSG_TYPE;
  //to avoid different versions of dragonite & host check the next parameters:
  if(Type >= MaxCommMsgType_e)
    return MsgTypeError_e;
  
  if(LENGTH != MsgTypeData[Type].Length)
    return MsgSizeError_e;
  
  if(VERSION != MsgTypeData[Type].Version)
    return TypeVersionError_e;  
  
  return MsgOK_e;
}
//=============================================================================
//Function      :   HandleMsgError
//Description   :   this function handles a message error
//Parameters    :   MsgStatus
//=============================================================================
//header & data info are already in place when calling this function
void SendMessage(void)
{
  
  //calc checksum
  HDR_CHECKSUM = Checksum((U8*)&MSG_TYPE_LONG);
  //take off value of MO_Flag, that is cleared next
  HDR_CHECKSUM -= 0x80;     //@@@@@@@find out the true location of the flag to reduce the true value from the CS@@@@@
  //clear flag, so host can read
  MO_FLAG = 0;
}
//=============================================================================
//Function      :   HandleMsgError
//Description   :   this function handles a message error
//Parameters    :   MsgStatus
//=============================================================================
U32 Checksum(U8 *Ptr)
{
    U32 Len, Cnt;
    U32 Sum = 0;

    //total size for checksum
    Len = HEADER_SIZE_FOR_CHECKSUM + LENGTH;
    //do checksum
    /* printf("\ncnt - ptr - sum\n"); */
    for(Cnt = 0; Cnt < Len; Cnt++, Ptr++)
    {
      Sum += *Ptr;
      /* printf("%d - 0x%x - 0x%x\n", Cnt, PtrTmp, Sum); */
    }

    return Sum;
}
//=============================================================================
//Function      :   HostWriteFormater
//Description   :   the function processes the msg data according to 
//                  the msg type. after processing each parameter, it
//                  will clear it if it was ok, or set to 1 if it was 
//                  out of range.
//Parameters    :   none
//Output        :   msg ok or error type. 
//=============================================================================
U8 HostWriteFormater(void)
{
    U8 Status   = MsgOK_e;
    U8 *DataPtr = NULL;
    U32 Size    = 0;
    
    
    //do switch to show where each msg type has to be handled
    switch(MSG_TYPE)
    {
    case Config_e:
        Size = MsgTypeData[Config_e].Length;
        if(!ConfigDisable) //didn't have config msg yet
        {
            ConfigDisable = TRUE;
            DataPtr = (U8*)&CONFIG_REGS;
        }
        else
        {
            //can't change configuration, so report error
            Status = ConfigError_e;
        }
        break;
    case System_e:
        Size = MsgTypeData[System_e].Length;
        DataPtr = (U8 *)&SYSTEM_REGS;
        break;
    case Chip_e:
        Size = MsgTypeData[Chip_e].Length;
        DataPtr = (U8 *)&CHIP_REGS;
        break;
    case PortCmd_e:
        Size = MsgTypeData[PortCmd_e].Length;
        DataPtr = (U8 *)&PORT_CMD_REGS;
        break;
    case PortCounters_e:
        Size = MsgTypeData[PortCounters_e].Length;
        DataPtr = (U8 *)&PORT_CNT_REGS;
        break;
    case PortStatuses_e:
        Size = MsgTypeData[PortStatuses_e].Length;
        DataPtr = (U8 *)&PORT_STAT_REGS;
        break;
    case PortMeasurments_e:
        Size = MsgTypeData[PortMeasurments_e].Length;
        DataPtr = (U8 *)&PORT_MSR_REGS;
        break;
    case PortL2_e:
        Size = MsgTypeData[PortL2_e].Length;
        DataPtr = (U8 *)&PORT_L2_REGS;
        break;
    case DebugMsg_e:
        Size = MsgTypeData[DebugMsg_e].Length;
        DataPtr = (U8 *)&DEBUG_REGS;
        break;
    case Other_e:
        Size = MsgTypeData[Other_e].Length;
        DataPtr = (U8 *)&OTHER_REGS;
        break;
    default:
        return MsgTypeError_e;
    }
    if(Status == MsgOK_e)
    {
        _ENTER_CRITICAL_SECTION_;
        //write data to dragonite db
        memcpy(DataPtr, MSG_DATA_PTR, Size);
        _EXIT_CRITICAL_SECTION_;
    }
    // clear field
    memset(MSG_DATA_PTR, 0, Size);
    return Status;
}
//=============================================================================
//Function      :   HostReadRequest
//Description   :   the function processes a read request
//Parameters    :   none
//Output        :   msg ok or error type. 
//=============================================================================
U8 HostReadRequest(void)
{
  U8 Status     = 0; //init to ok
  U8 *DataPtr   = NULL;
  U32 Size      = 0;
  
  //do switch to show where each msg type has to be handled
  switch(MSG_TYPE)
  {
    case Config_e:
      Size = MsgTypeData[Config_e].Length;
      DataPtr = (U8*)&CONFIG_REGS;
    break;

    case System_e:
      Size = MsgTypeData[System_e].Length;
      DataPtr = (U8*)&SYSTEM_REGS;
    break;
    
    case Chip_e:
      Size = MsgTypeData[Chip_e].Length;
      DataPtr = (U8*)&CHIP_REGS;
    break;
    
    case PortCmd_e:
      Size = MsgTypeData[PortCmd_e].Length;
      DataPtr = (U8 *)&PORT_CMD_REGS;
    break;
    
    case PortCounters_e:
      Size = MsgTypeData[PortCounters_e].Length;
      DataPtr = (U8 *)&PORT_CNT_REGS;
    break;
    
    case PortStatuses_e:
      Size = MsgTypeData[PortStatuses_e].Length;
      DataPtr = (U8 *)&PORT_STAT_REGS;
    break;
    
    case PortMeasurments_e:
      Size = MsgTypeData[PortMeasurments_e].Length;
      DataPtr = (U8 *)&PORT_MSR_REGS;
    break;
    
    case PortL2_e:
      Size = MsgTypeData[PortL2_e].Length;
      DataPtr = (U8 *)&PORT_L2_REGS;
    break;
    
    case DebugMsg_e:
      Size = MsgTypeData[DebugMsg_e].Length;
      DataPtr = (U8 *)&DEBUG_REGS;
    break;
    
    case Other_e:
      Size = MsgTypeData[Other_e].Length;
      DataPtr = (U8 *)&OTHER_REGS;
    break;

    default:
        return MsgTypeError_e;    
  }
  
  _ENTER_CRITICAL_SECTION_;
  //write data from dragonite db
  memcpy(MSG_DATA_PTR, DataPtr, Size);
  _EXIT_CRITICAL_SECTION_;
  //status could change when msg handling is more developed
  return Status;
}


