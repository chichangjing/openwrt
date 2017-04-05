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


#ifdef _BORLAND
/* suppress BC warnings: */
/* unreachanle code */
#pragma warn -rch
/* code has no effect */
#pragma warn -eff
#endif 

//*****************************************************************************
//                   IMPORTED  DECLARATIONS                                  
//*****************************************************************************

#include "Globals.h"

//****************************************************************************
//                         DEFINITIONS                                       
//****************************************************************************


//****************************************************************************
//                         GLOBAL VARS                                       
//****************************************************************************
//register file of dragonite
 t_Comm_db DB_for_Comm;
//flag that allows only one config msg
 U8   ConfigDisable; 

//for os simulation only
 U8 NumTaskRunning;
 
 
//****************************************************************************
//                         LOCAL FUNCTIONS DECLARATIONS                                       
//****************************************************************************




/********************************************************************
* Task_HostComm()          
*
*   This task is responsible for communication of CPU as slave device with
*   HOST over shared memory. The host places Host command in comm message.
*   The task verify that the receivd command is valid, process command, and build
*   reply message.
*
* Parameters:
*                                                                   
* Return: 
*********************************************************************/
#ifdef DRAGONITE_TYPE_A1

void dafaultsInit(void)
{
    t_config initConfig = 
                  { 0x00000015,
                    0x00000011,
                    0x00000000,
                    0x0000003f,
                    0x00000084,
                    0x00000184,
                    0x000003bc,
                    0x00000313,
                    0x000002b0,
                    0x00000000,
                    0x00000001,
                    0x00000002,
                    0x00000003,
                    0x00000004,
                    0x00000005,
                    0x00000006,
                    0x00000007,
                    0x00000008,
                    0x00000009,
                    0x0000000a,
                    0x0000000b,
                    0x00000100,
                    0x00000101,
                    0x00000102,
                    0x00000103,
                    0x00000104,
                    0x00000105,
                    0x00000106,
                    0x00000107,
                    0x00000108,
                    0x00000109,
                    0x0000010a,
                    0x0000010b,
                    0x00000200,
                    0x00000201,
                    0x00000202,
                    0x00000203,
                    0x00000204,
                    0x00000205,
                    0x00000206,
                    0x00000207,
                    0x00000208,
                    0x00000209,
                    0x0000020a,
                    0x0000020b,
                    0x00000300,
                    0x00000301,
                    0x00000302,
                    0x00000303,
                    0x00000304,
                    0x00000305,
                    0x00000306,
                    0x00000307,
                    0x00000308,
                    0x00000309,
                    0x0000030a,
                    0x0000030b,
                    0x00000400,
                    0x00000401,
                    0x00000402,
                    0x00000403,
                    0x00000404,
                    0x00000405,
                    0x00000406,
                    0x00000407,
                    0x00000408,
                    0x00000409,
                    0x0000040a,
                    0x0000040b,
                    0x00000500,
                    0x00000501,
                    0x00000502,
                    0x00000503,
                    0x00000504,
                    0x00000505,
                    0x00000506,
                    0x00000507,
                    0x00000508,
                    0x00000509,
                    0x0000050a,
                    0x0000050b,
                    0x00000600,
                    0x00000601,
                    0x00000602,
                    0x00000603,
                    0x00000604,
                    0x00000605,
                    0x00000606,
                    0x00000607,
                    0x00000608,
                    0x00000609,
                    0x0000060a,
                    0x0000060b,
                    0x00000700,
                    0x00000701,
                    0x00000702,
                    0x00000703,
                    0x00000704,
                    0x00000705,
                    0x00000706,
                    0x00000707,
                    0x00000708,
                    0x00000709,
                    0x0000070a,
                    0x0000070b,
                    0x00000019,
                    0x00000019};

    t_system  initSystem = 
            { 0x00000000,
            0x00000000,
            0x0001ffff,
            0x0001ffff,
            0x0001ffff,
            0x0001ffff,
            0x0001ffff,
            0x0001ffff,
            0x0001ffff,
            0x0001ffff,
            0x00000000,
            0x00000000,
            0x00000000,
            0x00000000,
            0x00000000,
            0x00000000,
            0x00000000,
            0x00000000,
            0x00000000,
            0x00000000,
            0x00000000,
            0x00000000,
            0x00000000,
            0x00000000,
            0x00000000,
            0x00000000,
            0x00000000,
            0x00000000,
            0x00000000,
            0x00000000,
            0x00000000,
            0x00000000,
            0x20626546,
            0x32203632,
            0x00393030,
            0x333a3730,
            0x32313a34,
            0x00000000,
            0x00000000,
            0x00000000,
            0x00000000,
            0x00000000,
            0x00000000};

    CONFIG_REGS = initConfig;
    //flag that allows only one configuration msg
    ConfigDisable = FALSE;

    SYSTEM_REGS = initSystem;

    /* set shared comm memory owner Dragonite */ 
    MO_FLAG = 1;

    //write data from dragonite db
    memcpy(MSG_DATA_PTR, (U8*)&SYSTEM_REGS, MsgTypeData[System_e].Length);
    ERROR_VECTOR = 0;
    MSG_TYPE = System_e;
    LENGTH = MsgTypeData[System_e].Length; 
    VERSION = MsgTypeData[System_e].Version;

    //prepare for interrupt to host
    COMM_READ_FLAG = 1;

    //send reply
    SendMessage();

    return;
}

unsigned __stdcall Task_HostComm(void* cookie)
#else 
void       Task_HostComm(void)
#endif
{
  U8  Status;

#ifdef DRAGONITE_TYPE_A1
    /* to avoid "not used parameter" warning */
    cookie;
#endif

    // Mark that task finished it's all pre init stage, and enters the while(1) loop, which 
    // will allow the funtion which created this task to continue to next task creation
    //OSAL_TASK_RUNNING_FINISH_INIT_STAGE();  //^^^^ for OS
    //INIT
    InitCommIrq();
    
    dafaultsInit();
    
    while(1)
    {
        //WDG_Report_OK(eFUNC_COMM_TASK); // Report to Watchdog funtionality that this soft section is working OK
  
        //read msg from host
        Status = MessageStatus();
        //process msg only if host finished writing it
        if(Status != MsgNotReady_e)
        {
          HandleIncomingMsg(Status);
        }// end msg not ready
           //interrupt to host
        HandleInterruptToHost();
       
        //after process, go to sleep for 25ms
#ifdef DRAGONITE_TYPE_A1
        OSAL_DELAY_mSec(25000); //25ms
#else 
        OSAL_DELAY_mSec(25); //25ms     @@@@@@there is a field in the DB which defines the frequency of the the communication handler        
#endif 
    } // end while(1)

#ifdef DRAGONITE_TYPE_A1
    return 0;
#endif 
}



//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//---------------- L O C A L  --  F U N C T I O N S ------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------




