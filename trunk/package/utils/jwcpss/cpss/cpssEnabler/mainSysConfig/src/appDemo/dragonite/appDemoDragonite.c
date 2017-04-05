/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtDbDxBobcat2.c
*
* DESCRIPTION:
*       Initialization functions for the Bobcat2 - SIP5 - board.
*
* FILE REVISION NUMBER:
*       $Revision: 118 $
*
*******************************************************************************/
#include <appDemo/boardConfig/appDemoBoardConfig.h>

#ifdef _linux
#ifdef CPU_MSYS


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <gtExtDrv/drivers/gtDragoniteDrv.h>

#define APP_DEMO_DRAGONITE_FW_FILE_SIZE     65536
#define APP_DEMO_DRAGONITE_FW_FILE_PATH     "/usr/bin/dragonite.bin"

typedef struct POE_CUSTOMER_INFO_DATA_STC
{
  GT_U32 version_number; /* struct version */
  GT_U32 struct_size; /* struct sizeof */
  GT_U8  *sign_date; /* date of generate the version  */
  GT_U8  *sign_time; /* time of generate the version  */
  GT_U8  *chr_array_ptr; /* TBD string */
  GT_U16 *software_pn; /* this value is the same as in the 15 BYTE communication protocol doc  */
  GT_U8  *software_build; /* this value is the same as in the 15 BYTE communication protocol doc */ 
  GT_U8  *product_number; /* this value is the same as in the 15 BYTE communication protocol doc  */
  GT_U16 *kernel_version; /* this value is the same as in the 15 BYTE communication protocol doc  */
  GT_U8  *salad_param_code; /* this value is the same as in the 15 BYTE communication protocol doc  */
 
}POE_CUSTOMER_INFO_DATA_STC;


GT_STATUS appDemoDragoniteMessageCheckSumCalc
(
    IN  GT_U8   *bufferPtr,
    IN  GT_U32  msgLen,
    OUT GT_U16  *checkSumPtr
)
{
    GT_U16  checkSum = 0;

    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);
    CPSS_NULL_PTR_CHECK_MAC(checkSumPtr);

    while (msgLen > 0)
    {
        msgLen--;
        checkSum += bufferPtr[msgLen];
    }

    *checkSumPtr = checkSum;

    return GT_OK;
}


/******************************************************************************
* appDemoDragoniteSowfwareDownload
*
* DESCRIPTION:
*       Example to show Dragonite FW download.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PTR - on NULL pointer
*
* COMMENTS:
*       None.
*
******************************************************************************/
GT_STATUS appDemoDragoniteSowfwareDownload()
{

    GT_STATUS rc = GT_OK;
    char *buf = NULL;
    GT_U32 nr;
    GT_BOOL msg;
    GT_32 rfd = 0, i;
    GT_U8 msg_reply[DRAGONITE_DATA_MSG_LEN] = {0};

    /* call to init Dragonite function */
    rc = extDrvDragoniteDriverInit();
    if(rc != GT_OK)
    {
        return rc;
    }

    buf = cpssOsMalloc(APP_DEMO_DRAGONITE_FW_FILE_SIZE); 
    if (buf == NULL)
    {
        cpssOsPrintf("appDemoDragoniteSowfwareDownload: malloc failed\r\n");
        return GT_OUT_OF_CPU_MEM;
    }

    /* read FW file */
    rfd = open(APP_DEMO_DRAGONITE_FW_FILE_PATH, O_RDONLY);
    if (rfd <= 0)
    {
        cpssOsPrintf("appDemoDragoniteSowfwareDownload: open file failed\r\n");
        rc = GT_BAD_STATE;
        goto fail_label;
    }

    nr = read(rfd, buf, APP_DEMO_DRAGONITE_FW_FILE_SIZE);
    if (nr != APP_DEMO_DRAGONITE_FW_FILE_SIZE)
    {
        cpssOsPrintf("appDemoDragoniteSowfwareDownload: read file failed\r\n");
        rc = GT_BAD_STATE;
        goto fail_label;
    }

    rc = extDrvDragoniteFwDownload(buf, nr);
    if (rc != GT_OK)
    {
        cpssOsPrintf("appDemoDragoniteSowfwareDownload: download FW failed\r\n");
        goto fail_label;
    }

    /* restore mem type to DTCM */
    rc = extDrvDragoniteMemoryTypeSet(DTCM_DIR);
    if(rc != GT_OK)
    {
        cpssOsPrintf("appDemoDragoniteSowfwareDownload: set memory type failed\r\n");
        goto fail_label;
    }

    /* Init DTCM with 0xFF */
    for (i = 0; i < 0xB00; i++)
    {
        rc = extDrvDragoniteRegisterWrite(HOST_OUTPUT_TO_POE_REG_ADDR + i, 0xFF);
        if (rc != GT_OK)
        {
            cpssOsPrintf("appDemoDragoniteSowfwareDownload: write register failed, addr=0x%x\r\n", (HOST_OUTPUT_TO_POE_REG_ADDR + i));
            goto fail_label;
        }
    }

    /* Init Host protocol version to '1' in order to 'speak' with Dragonite with the same 'language' */
    rc = extDrvDragoniteRegisterWrite(HOST_PROT_VER_REG_ADDR, 0x1);
    if(rc != GT_OK)
    {
        goto fail_label;
    }

    msg = GT_TRUE;
    rc = extDrvDragoniteUnresetSet(msg);
    if(rc != GT_OK)
    {
        cpssOsPrintf("appDemoDragoniteSowfwareDownload: Dragonite UnReset failed\r\n");
        goto fail_label;
    }

    cpssOsTimerWkAfter(500);

    /* Get system status message from POE that is ready after unreset */
    rc = extDrvDragoniteMsgRead(msg_reply);
    if(rc != GT_OK)
    {
        cpssOsPrintf("appDemoDragoniteSowfwareDownload: MSG receive was failed\r\n");
        goto fail_label;
    }

    cpssOsPrintf("REC_DATA[0] = 0x%x\r\n", msg_reply[0]);
    cpssOsPrintf("REC_DATA[1] = 0x%x\r\n", msg_reply[1]);
    cpssOsPrintf("REC_DATA[2] = 0x%x\r\n", msg_reply[2]);
    cpssOsPrintf("REC_DATA[3] = 0x%x\r\n", msg_reply[3]);
    cpssOsPrintf("REC_DATA[4] = 0x%x\r\n", msg_reply[4]);
    cpssOsPrintf("REC_DATA[5] = 0x%x\r\n", msg_reply[5]);
    cpssOsPrintf("REC_DATA[6] = 0x%x\r\n", msg_reply[6]);
    cpssOsPrintf("REC_DATA[7] = 0x%x\r\n", msg_reply[7]);
    cpssOsPrintf("REC_DATA[8] = 0x%x\r\n\r\n\r\n", msg_reply[8]);



/* fallthrough in case ok */
fail_label:
    if(buf != NULL)
    {
        cpssOsFree(buf);
    }

    close(rfd);

    return rc;
}


/******************************************************************************
* appDemoDragoniteProtocolVersionShow
*
* DESCRIPTION:
*       Print version info from FW binary image.
*
* INPUTS:
*       buf - (pointer to) FW buffer
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PTR - on NULL pointer
*
* COMMENTS:
*       None.
*
******************************************************************************/
GT_STATUS appDemoDragoniteProtocolVersionShow
(   
    IN char *buf
)
{
    POE_CUSTOMER_INFO_DATA_STC *customerInfoPtr;
    GT_U32 verInfoOffset;

    CPSS_NULL_PTR_CHECK_MAC(buf);

    verInfoOffset = *(GT_U32*)(buf + APP_DEMO_DRAGONITE_FW_FILE_SIZE - 8);
    customerInfoPtr = (POE_CUSTOMER_INFO_DATA_STC*)(buf + verInfoOffset);

    osPrintf("buf 0x%x, offset: 0x%x, ptr: 0x%x\n", buf, verInfoOffset, customerInfoPtr);

    osPrintf("sw build: %d\n", *(buf + (GT_32)customerInfoPtr->software_build) );
    osPrintf("sw pn: %d\n", *(GT_U16*)(buf + (GT_32)customerInfoPtr->software_pn) );
    osPrintf("date: %.15s\n", buf + (GT_32)customerInfoPtr->sign_date);

    return GT_OK;
}



/******************************************************************************
* appDemoDragoniteMcuVersionRead
*
* DESCRIPTION:
*       Read MCU Protocol Version register.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       versionPtr - (pointer to) version value
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PTR - on NULL pointer
*
* COMMENTS:
*       None.
*
******************************************************************************/
GT_STATUS appDemoDragoniteMcuVersionRead
(   
    IN GT_U32 *versionPtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(versionPtr);

    return extDrvDragoniteRegisterRead(PROT_VER_REG_ADDR ,versionPtr);
}

/******************************************************************************
* appDemoDragoniteMcuResetCauseUpdate
*
* DESCRIPTION:
*       Inform the POE software about the reason that the host decided to
*       reset the POE MCU
*
* INPUTS:
*       cause - cause reset
*                   0xFF - power reset
*                   0x01 - POE Not responding
*                   0x02 - Application request 
*                   0x03 - Communication errors
*                   0x04 - Host Application Reboot
*                   0x05 - Undefined cause
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PTR - on NULL pointer
*       GT_FAIL - otherwise
*
* COMMENTS:
*       None.
*
******************************************************************************/
GT_STATUS appDemoDragoniteMcuResetCauseUpdate
(   
    GT_U32 cause
)
{
    GT_STATUS  rc;
    GT_U32     regVal;
    GT_U16     counter;
    GT_U32     data;

    rc = extDrvDragoniteRegisterRead(HOST_RST_CAUSE_REG_ADDR, &regVal);
    if (rc != GT_OK)
    {
        return rc; 
    }

    counter = (regVal & 0xFF);

    if (counter == MEM_INIT_VAL)
    {
        counter = 1;
    }
    else
    {
        counter++;
    }
    data = cause<<24 | counter;

    rc = extDrvDragoniteRegisterWrite(HOST_RST_CAUSE_REG_ADDR, data);
    return rc;
}

/******************************************************************************
* appDemoDragoniteReadRegister
*
* DESCRIPTION:
*       Print value of Dragonite register
*
* INPUTS:
*       addr - address to read.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*       None.
*
******************************************************************************/
GT_STATUS appDemoDragoniteReadRegister
(
    GT_U32 addr
)
{
    GT_STATUS rc;
    GT_U32 regValue;

    rc = extDrvDragoniteRegisterRead(addr, &regValue);
    if(rc != GT_OK)
    {
        cpssOsPrintf("\r\n\r\n extDrvDragoniteRegisterRead: FAILURE: rc = %d\r\n", rc);
        return rc;
    }
    cpssOsPrintf("\r\n\r\nextDrvDragoniteRegisterRead: value = 0x%x\r\n", regValue);

    return GT_OK;
}


/******************************************************************************
* appDemoDragoniteWriteRegister
*
* DESCRIPTION:
*       Write value to Dragonite register.
*
* INPUTS:
*       addr - address to write.
*       regValue - value to write.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*       None.
*
******************************************************************************/
GT_STATUS appDemoDragoniteWriteRegister
(
    GT_U32 addr,
    GT_U32 regValue
)
{
    GT_STATUS rc;

    rc = extDrvDragoniteRegisterWrite(addr, regValue);
    if(rc != GT_OK)
    {
        cpssOsPrintf("\r\n\r\n appDemoDragoniteReadRegister: FAILURE: rc = %d\r\n", rc);
        return rc;
    }

    return GT_OK;
}

/******************************************************************************
* appDemoDragoniteMemTypeSet
*
* DESCRIPTION:
*       COnfigure memory type to Dragonite access
*
* INPUTS:
*       memType - 0 -> ITCM
*                 1 -> DTCM
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*       None.
*
******************************************************************************/
GT_STATUS appDemoDragoniteMemTypeSet(GT_BOOL memType)
{
    GT_STATUS rc;

    rc = extDrvDragoniteMemoryTypeSet(memType);
    if(rc != GT_OK)
    {
        cpssOsPrintf("appDemoDragoniteMemTypeSet: FAIL: rc = %d\r\n", rc);
        return rc;
    }

    return GT_OK;
}


/******************************************************************************
* appDemoDragoniteTestExample1
*
* DESCRIPTION:
*       POE 15B protocol command example
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*       None.
*
******************************************************************************/
GT_STATUS appDemoDragoniteTestExample1()
{
    GT_STATUS rc;
    GT_U8 msg[DRAGONITE_DATA_MSG_LEN] = {0};
    GT_U8 msg_reply[DRAGONITE_DATA_MSG_LEN] = {0};
    GT_U16  checkSum = 0;

    /*
        Host Request:
            Get System Status 
            DATA[0] = 0x2 - Request
            DATA[2] = 0x7 - Global
            DATA[3] = 0x3d - SystemStatus

        Controller Response:
            DATA[0] = 0x3 - Telemetry
            DATA[2] = CPU status 1
            DATA[3] = CPU status 2
            DATA[4] = Factory Default
            DATA[5] = GIE
            DATA[6] = Private Label
            DATA[7] = User byte
            DATA[8] = Device Fail
            DATA[9] = Temperature Disconnect
            DATA[10] = Temperature Alarm
            DATA[11] = Interrupt register
            DATA[12] = Interrupt register

        CPU status-1: Bit0 = ‘1’ indicates PoE controller error. Bit1 = ‘1’ indicates that firmware download is required.
        CPU status-2: Bit0 = ‘1’ indicates PoE controller memory error. Bit1 = ‘1’ indicates error (if there are less than eight
        PoE devices in the system, then Bit1= ‘1’). 
        Factory default: Bit0 = ‘1’ indicates that factory default parameters are currently set. 
        GIE (General Internal Error): When different from 0x00,it  indicates a general internal error. 
        Private Label: Saved in the RAM. Equals 0x00 after reset. Refer to "Set System Status" command. 
        User Byte - Saved in memory. Equals 0xFF, once set to factory default. 
        Device Fail:  (1) Bits 0 to 7 indicate a failed PoE device(s).
            ’1’ = Fail or missing PoE Device, ‘0’ = PoE Device is OK.
        Temperature disconnect:  (1)  Bits 0 to 7 indicate over - heated PoE device(s).
        This over-heating causes disconnection of all ports.
            '1' = This PoE device caused disconnection due to high temperature, '0' = Temperature is OK.
        Temperature alarm:  (1)  Bits 0 to 7 indicate over heated PoE device(s).
        if temperature exceeds a pre-defined user defined limit, (Set PoE Device Params command),
        then the appropriate bit changes to ‘1’.
            '1' = High temperature, '0' = Temperature is OK.
        Interrupt register latches a transition when an event occurs. The transition might be one or more of several port
        status changes, PoE device status event/s or system event/s, depending on event definition.
        (1)  Bit 0 – PoE Device-‘0’, Bit 1 – PoE Device-‘1’, Bit 2 – PoE Device-2, Bit 7 – PoE Device-7.
        Device number is set according to its connectivity to the controller.

    */
    msg[0] = 0x2;
    msg[2] = 0x7;
    msg[3] = 0x3d;
    /* Fill byte[14] and byte[15] in 15B protocol with checksum */
    appDemoDragoniteMessageCheckSumCalc(msg, 13, &checkSum);
    msg[13] = U32_GET_FIELD_MAC(checkSum, 8, 8);
    msg[14] = U32_GET_FIELD_MAC(checkSum, 0, 8);

    rc = extDrvDragoniteMsgWrite(msg);
    if(rc != GT_OK)
    {
        cpssOsPrintf("extDrvDragoniteMsgWrite: MSG send was failed\r\n");
        return rc;
    }

    cpssOsTimerWkAfter(100);

    rc = extDrvDragoniteMsgRead(msg_reply);
    if(rc != GT_OK)
    {
        cpssOsPrintf("extDrvDragoniteMsgRead: MSG receive was failed\r\n");
        return rc;
    }

    cpssOsPrintf("REC_DATA[0] = 0x%x\r\n", msg_reply[0]);
    cpssOsPrintf("REC_DATA[1] = 0x%x\r\n", msg_reply[1]);
    cpssOsPrintf("REC_DATA[2] = 0x%x\r\n", msg_reply[2]);
    cpssOsPrintf("REC_DATA[3] = 0x%x\r\n", msg_reply[3]);
    cpssOsPrintf("REC_DATA[4] = 0x%x\r\n", msg_reply[4]);
    cpssOsPrintf("REC_DATA[5] = 0x%x\r\n", msg_reply[5]);
    cpssOsPrintf("REC_DATA[6] = 0x%x\r\n", msg_reply[6]);
    cpssOsPrintf("REC_DATA[7] = 0x%x\r\n", msg_reply[7]);
    cpssOsPrintf("REC_DATA[8] = 0x%x\r\n", msg_reply[8]);
    cpssOsPrintf("REC_DATA[9] = 0x%x\r\n", msg_reply[9]);
    cpssOsPrintf("REC_DATA[10] = 0x%x\r\n", msg_reply[10]);
    cpssOsPrintf("REC_DATA[11] = 0x%x\r\n", msg_reply[11]);
    cpssOsPrintf("\r\n\r\n");

    return GT_OK;
}


/******************************************************************************
* appDemoDragoniteTestExample2
*
* DESCRIPTION:
*       POE 15B protocol command example
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*       None.
*
******************************************************************************/
GT_STATUS appDemoDragoniteTestExample2()
{
    GT_STATUS rc;
    GT_U8 msg[DRAGONITE_DATA_MSG_LEN] = {0};
    GT_U8 msg_reply[DRAGONITE_DATA_MSG_LEN] = {0};
    GT_U16  checkSum = 0;

    /*
        Host Request:
            Get System Status 
            DATA[0] = 0x2 - Request
            DATA[2] = 0x7 - Global
            DATA[3] = 0xb - Supply
            DATA[4] = 0x17 - Main

        Controller Response:
            DATA[0] = 0x3 - Telemetry
            DATA[2] = Power Consumption
            DATA[3] = Power Consumption
            DATA[4] = Max ShutDown Voltage
            DATA[5] = Max ShutDown Voltage
            DATA[6] = Min ShutDown Voltage
            DATA[7] = Min ShutDown Voltage
            DATA[8] = N
            DATA[9] = Power Bank
            DATA[10] = Power Limit
            DATA[11] = Power Limit
            DATA[12] = N

            Telemetry for main power supply parameters (within the working power budget): 
            Power Consumption: Actual momentary total power consumption (units are in watts). 
            Max Shutdown Voltage: Maximum voltage level set; above this level, PoE ports shutdown. Units are in decivolts. 
            Min Shutdown Voltage: Minimum voltage level set; below this level, PoE ports shutdown. Units are given in decivolts. 
            Power Bank: The current active Power Bank. 
            Power Limit: If power consumption exceeds this level, lowest priority ports will be disconnected. To set the desired value, refer to 
            (units are in watts).

    */
    msg[0] = 0x2;
    msg[2] = 0x7;
    msg[3] = 0xb;
    msg[4] = 0x17;
    /* Fill byte[14] and byte[15] in 15B protocol with checksum */
    appDemoDragoniteMessageCheckSumCalc(msg, 13, &checkSum);
    msg[13] = U32_GET_FIELD_MAC(checkSum, 8, 8);
    msg[14] = U32_GET_FIELD_MAC(checkSum, 0, 8);

    rc = extDrvDragoniteMsgWrite(msg);
    if(rc != GT_OK)
    {
        cpssOsPrintf("extDrvDragoniteMsgWrite: MSG send was failed\r\n");
        return rc;
    }

    cpssOsTimerWkAfter(100);

    rc = extDrvDragoniteMsgRead(msg_reply);
    if(rc != GT_OK)
    {
        cpssOsPrintf("extDrvDragoniteMsgRead: MSG receive was failed\r\n");
        return rc;
    }

    cpssOsPrintf("REC_DATA[0] = 0x%x\r\n", msg_reply[0]);
    cpssOsPrintf("REC_DATA[1] = 0x%x\r\n", msg_reply[1]);
    cpssOsPrintf("REC_DATA[2] = 0x%x\r\n", msg_reply[2]);
    cpssOsPrintf("REC_DATA[3] = 0x%x\r\n", msg_reply[3]);
    cpssOsPrintf("REC_DATA[4] = 0x%x\r\n", msg_reply[4]);
    cpssOsPrintf("REC_DATA[5] = 0x%x\r\n", msg_reply[5]);
    cpssOsPrintf("REC_DATA[6] = 0x%x\r\n", msg_reply[6]);
    cpssOsPrintf("REC_DATA[7] = 0x%x\r\n", msg_reply[7]);
    cpssOsPrintf("REC_DATA[8] = 0x%x\r\n", msg_reply[8]);
    cpssOsPrintf("REC_DATA[9] = 0x%x\r\n", msg_reply[9]);
    cpssOsPrintf("REC_DATA[10] = 0x%x\r\n", msg_reply[10]);
    cpssOsPrintf("REC_DATA[11] = 0x%x\r\n", msg_reply[11]);
    cpssOsPrintf("\r\n\r\n");

    return GT_OK;
}


#endif
#endif

