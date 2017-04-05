/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssGenDragonite.c
*
* DESCRIPTION:
*       CPSS generic Dragonite APIs.
*
*
* FILE REVISION NUMBER:
*       $Revision: 16 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/generic/dragonite/private/prvCpssGenDragoniteLog.h>
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/dragonite/cpssGenDragonite.h>
#include <cpss/generic/dragonite/private/prvCpssGenDragonite.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

#ifdef ASIC_SIMULATION
#include <Globals.h>
#endif 

GT_BOOL dragoniteDbgDumpsOn = GT_FALSE;
#define DBG_INFO(x)     if(dragoniteDbgDumpsOn) cpssOsPrintf x

/*****************************************************************************
*                         DEFINITIONS                                       
******************************************************************************/

/* CPU is working in big-endian mode now */
GT_BOOL bigEndian = GT_FALSE;

GT_U32 *commMsgPtr = NULL;

static GT_BOOL initDone = GT_FALSE;

#define PRV_CPSS_DRAGONITE_INIT_CHECK_MAC   \
        if(GT_FALSE == initDone)            \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG)

/* read data decode functions definition */
static GT_VOID prvCpssDragoniteDecodeConfig(IN CPSS_GEN_DRAGONITE_DATA_STC *msgPtr);
static GT_VOID prvCpssDragoniteDecodeSystem(IN CPSS_GEN_DRAGONITE_DATA_STC *msgPtr);
static GT_VOID prvCpssDragoniteDecodeChip(IN CPSS_GEN_DRAGONITE_DATA_STC *msgPtr);
static GT_VOID prvCpssDragoniteDecodePortCmd(IN CPSS_GEN_DRAGONITE_DATA_STC *msgPtr);
static GT_VOID prvCpssDragoniteDecodePortCntr(IN CPSS_GEN_DRAGONITE_DATA_STC *msgPtr);
static GT_VOID prvCpssDragoniteDecodePortStatus(IN CPSS_GEN_DRAGONITE_DATA_STC *msgPtr);
static GT_VOID prvCpssDragoniteDecodePortMeasure(IN CPSS_GEN_DRAGONITE_DATA_STC *msgPtr);
static GT_VOID prvCpssDragoniteDecodePortLayer2(IN CPSS_GEN_DRAGONITE_DATA_STC *msgPtr);
static GT_VOID prvCpssDragoniteDecodeDebug(IN CPSS_GEN_DRAGONITE_DATA_STC *msgPtr);
static GT_VOID prvCpssDragoniteDecodeOther(IN CPSS_GEN_DRAGONITE_DATA_STC *msgPtr);

/* write data encode functions definition */
static GT_STATUS prvCpssDragoniteEncodeConfig(IN CPSS_GEN_DRAGONITE_DATA_STC *msgPtr);
static GT_STATUS prvCpssDragoniteEncodeSystem(IN CPSS_GEN_DRAGONITE_DATA_STC *msgPtr);
static GT_STATUS prvCpssDragoniteEncodePortCmd(IN CPSS_GEN_DRAGONITE_DATA_STC *msgPtr);
static GT_STATUS prvCpssDragoniteEncodePortLayer2(IN CPSS_GEN_DRAGONITE_DATA_STC *msgPtr);

/* this const array has to be updated for each msg type */
const PRV_CPSS_DRAGONITE_COMM_MSG_STRUCT_VALID_STC msgTypeData[] = {
    { PRV_CPSS_DRAGONITE_CONFIG_STRUCT_LENGTH_CNS<<2,       0, prvCpssDragoniteDecodeConfig,        prvCpssDragoniteEncodeConfig},
    { PRV_CPSS_DRAGONITE_SYSTEM_STRUCT_LENGTH_CNS<<2,       0, prvCpssDragoniteDecodeSystem,        prvCpssDragoniteEncodeSystem},
    { PRV_CPSS_DRAGONITE_CHIP_STRUCT_LENGTH_CNS<<2,         0, prvCpssDragoniteDecodeChip,          NULL},
    { PRV_CPSS_DRAGONITE_PORT_CMD_STRUCT_LENGTH_CNS<<2,     0, prvCpssDragoniteDecodePortCmd,       prvCpssDragoniteEncodePortCmd},
    { PRV_CPSS_DRAGONITE_PORT_CNTR_STRUCT_LENGTH_CNS<<2,    0, prvCpssDragoniteDecodePortCntr,      NULL},
    { PRV_CPSS_DRAGONITE_PORT_STATUS_STRUCT_LENGTH_CNS<<2,  0, prvCpssDragoniteDecodePortStatus,    NULL},
    { PRV_CPSS_DRAGONITE_PORT_MEASURE_STRUCT_LENGTH_CNS<<2, 0, prvCpssDragoniteDecodePortMeasure,   NULL},
    { PRV_CPSS_DRAGONITE_PORT_LAYER2_STRUCT_LENGTH_CNS<<2,  0, prvCpssDragoniteDecodePortLayer2,    prvCpssDragoniteEncodePortLayer2},
    { PRV_CPSS_DRAGONITE_DEBUG_STRUCT_LENGTH_CNS<<2,        0, prvCpssDragoniteDecodeDebug,         NULL},
    { PRV_CPSS_DRAGONITE_OTHER_STRUCT_LENGTH_CNS<<2,        0, prvCpssDragoniteDecodeOther,         NULL}
};

/*******************************************************************************
* calcChecksum
*
* DESCRIPTION:
*           Calculate communication message checksum
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*    msgPtr - pointer to communication message without checksum field
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       checksum calculated value
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_U32 calcChecksum
(
     IN const GT_VOID *msgPtr
)
{
    GT_U32 len, cnt;
    GT_U32 sum = 0;
    GT_U8 *ptr = (GT_U8*)msgPtr;

    /* total size for checksum */
    len = PRV_CPSS_DRAGONITE_HEADER_SIZE_FOR_CHECKSUM_CNS + PRV_CPSS_DRAGONITE_LENGTH_GET_MAC;

    /* calculate checksum */
    for(cnt = 0; cnt < len; cnt++, ptr++)
        sum += (GT_U8)*ptr;

    return sum;
}

/*******************************************************************************
* endiannessSet
*
* DESCRIPTION:
*     Check what is endianness of current system and save it in global variable
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID endiannessSet
(
    GT_VOID
)
{
    volatile GT_U32 i = 1;
    
    if((*(GT_U8*)&i)==1)
        bigEndian = GT_FALSE;
    else
        bigEndian = GT_TRUE;

    DBG_INFO(("endiannessSet:bigEndian=%d\n", bigEndian));

    return;
}

/*******************************************************************************
* dataSwap
*
* DESCRIPTION:
*     swap communication message data part for big-endian system
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID dataSwap
(
    GT_VOID
)
{
    GT_U32 i;

    if(bigEndian)
    {
        DBG_INFO(("dataSwap\n"));
        for(i = 0; i < PRV_CPSS_DRAGONITE_MAX_DATA_SIZE_CNS>>2; i++)
            PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i] = BYTESWAP_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i]);
    }

    return;
}

/*******************************************************************************
* headerDump
*
* DESCRIPTION:
*     dragonite communication message header dump
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       debug function
*
*******************************************************************************/
static GT_VOID headerDump(GT_VOID)
{
    GT_U32 i;

    for(i = 0; i < 6; i++)
    {
        DBG_INFO(("[0x%08x] 0x%08x\n", commMsgPtr+i, commMsgPtr[i]));
    }
    
    return;
}

/*******************************************************************************
* internal_cpssDragoniteInit
*
* DESCRIPTION:
*           Initialize DRAGONITE management library:
*               get shared memory base pointer from BSP; 
*               get system structure and check version compatibility;
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*    intVectNum - DRAGONITE interrupt vector number
*    intMask    - DRAGONITE interrupt mask
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_INIT_ERROR - if major version of firmware isn't
*                       equal to CPSS_GEN_DRAGONITE_MAJOR_VERSION_CNS
*       GT_NOT_READY  - DRAGONITE controller not finished yet 
*                       copy system structure to shared memory
*       GT_BAD_STATE  - data received, but not of System type
*       GT_BAD_PTR    - if wrong shared mem base addr
*
* COMMENTS:
*       This call may return NOT_READY if DRAGONITE manager still not initialized 
*       application must set some timeout for this case and try again,
*       it can't do nothing DRAGONITE related till this call not succeeded.
*
*******************************************************************************/
static GT_STATUS internal_cpssDragoniteInit
(
    IN GT_U32 intVectNum,
    IN GT_U32 intMask
)
{
    CPSS_GEN_DRAGONITE_DATA_STC     dragoniteData;
    GT_U32                  timeoutCounter = 0;
    GT_STATUS               rc;

    rc = cpssExtDrvDragoniteShMemBaseAddrGet((GT_U32*)(GT_VOID*)&irqMemPtr); /* take from BSP the DTCM base address to be
                                                                              * able to read/write it by direct pointer */
    if(rc != GT_OK)
    {
        return rc;
    }

    CPSS_NULL_PTR_CHECK_MAC(irqMemPtr);

    /* Init interrupts mechanism */
#ifdef ASIC_SIMULATION    
    commMsgPtr = (GT_U32*)&InputMsg;
#else
    commMsgPtr = irqMemPtr + PRV_CPSS_DRAGONITE_COMM_MSG_MEM_OFFSET_CNS;
#endif 

    DBG_INFO(("commMsgPtr=0x%08x,irqMemPtr=0x%08x\n", commMsgPtr, irqMemPtr));

    endiannessSet();

    /* set it here, because cpssDragoniteReadData checks it and indeed most
     * important variables already initialized 
     */
    initDone = GT_TRUE;

    /* This ReadData call doesn't require ReadTrigger - DRAGONITE controller sends 
     * system data first time automatically. 
     */
    do
    {
        rc = cpssDragoniteReadData(&dragoniteData);
        if(rc == GT_NOT_READY)
            timeoutCounter++;
        else if(rc != GT_OK)
        {
            return rc;
        }
        else
            break;
    }while(timeoutCounter <= 3);
    
    if(timeoutCounter == 3)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_READY, LOG_ERROR_NO_MSG);
    }

    DBG_INFO(("dragoniteData.dataType=%d,CPSS_GEN_DRAGONITE_DATA_TYPE_SYSTEM_E=%d\n", 
                dragoniteData.dataType, CPSS_GEN_DRAGONITE_DATA_TYPE_SYSTEM_E));
    if(dragoniteData.dataType != CPSS_GEN_DRAGONITE_DATA_TYPE_SYSTEM_E)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    else
    {        
        GT_U32  i;
                        
        cpssOsPrintf("Dragonite FW Version: ");
        for(i = 0; i < CPSS_GEN_DRAGONITE_BLD_TIME_STRING_LEN_CNS; i++)
        {
            if(dragoniteData.dragoniteData.system.bldDateTime[i] == 0)
                cpssOsPrintf(" ");
            else
                cpssOsPrintf("%c", dragoniteData.dragoniteData.system.bldDateTime[i]);
        }
        cpssOsPrintf(" Ver %d.%d\n", dragoniteData.dragoniteData.system.majorVersion, dragoniteData.dragoniteData.system.minorVersion);
    }

    DBG_INFO(("CPSS_GEN_DRAGONITE_MAJOR_VERSION_CNS=%d,majorVersion=%d\n", 
                CPSS_GEN_DRAGONITE_MAJOR_VERSION_CNS, dragoniteData.dragoniteData.system.majorVersion));

    /* Check firmware and CPSS code versions compatibility */
    if(CPSS_GEN_DRAGONITE_MAJOR_VERSION_CNS != dragoniteData.dragoniteData.system.majorVersion)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);  

    if((rc = dragoniteInterruptsInit(intVectNum,intMask)) != GT_OK)
        return rc;    

    DBG_INFO(("cpssDragoniteInit-OK\n"));

    return GT_OK;
}

/*******************************************************************************
* cpssDragoniteInit
*
* DESCRIPTION:
*           Initialize DRAGONITE management library:
*               get shared memory base pointer from BSP; 
*               get system structure and check version compatibility;
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*    intVectNum - DRAGONITE interrupt vector number
*    intMask    - DRAGONITE interrupt mask
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_INIT_ERROR - if major version of firmware isn't
*                       equal to CPSS_GEN_DRAGONITE_MAJOR_VERSION_CNS
*       GT_NOT_READY  - DRAGONITE controller not finished yet 
*                       copy system structure to shared memory
*       GT_BAD_STATE  - data received, but not of System type
*       GT_BAD_PTR    - if wrong shared mem base addr
*
* COMMENTS:
*       This call may return NOT_READY if DRAGONITE manager still not initialized 
*       application must set some timeout for this case and try again,
*       it can't do nothing DRAGONITE related till this call not succeeded.
*
*******************************************************************************/
GT_STATUS cpssDragoniteInit
(
    IN GT_U32 intVectNum,
    IN GT_U32 intMask
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDragoniteInit);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, intVectNum, intMask));

    rc = internal_cpssDragoniteInit(intVectNum, intMask);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, intVectNum, intMask));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDragoniteReadTrigger
*
* DESCRIPTION:
*      Trigger DRAGONITE controller to put in shared memory data of requested type
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*       type - type of data structure application wants to read 
*                  from DRAGONITE MCU
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM   - on unknown type
*       GT_NOT_READY   - DRAGONITE controller not finished yet 
*                                          previous operation
*       GT_NOT_INITIALIZED - DragoniteInit not executed
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDragoniteReadTrigger
(
    IN CPSS_GEN_DRAGONITE_DATA_TYPE_ENT type
)
{
    PRV_CPSS_DRAGONITE_INIT_CHECK_MAC;

/* in user application base addresses will be aquired by cpssDragoniteInit, but
 * during debug may be needed to call this function independently
 */
    cpssExtDrvDragoniteShMemBaseAddrGet((GT_U32*)(GT_VOID*)&irqMemPtr); /* take from BSP the DTCM base address to be
                                                            * able to read/write it by direct pointer 
                                                            */
    CPSS_NULL_PTR_CHECK_MAC(irqMemPtr);

    /* Init interrupts mechanism */
#ifdef ASIC_SIMULATION    
    commMsgPtr = (GT_U32*)&InputMsg;
#else
    commMsgPtr = irqMemPtr + PRV_CPSS_DRAGONITE_COMM_MSG_MEM_OFFSET_CNS;
#endif 

    DBG_INFO(("cpssDragoniteReadTrigger:commMsgPtr=0x%08x,irqMemPtr=0x%08x\n", commMsgPtr, irqMemPtr));

    if(PRV_CPSS_DRAGONITE_MO_FLAG_GET_MAC)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_READY, LOG_ERROR_NO_MSG);

    if(type > CPSS_GEN_DRAGONITE_DATA_TYPE_MAX_E)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    else
        PRV_CPSS_DRAGONITE_MSG_TYPE_SET_MAC(type);

    PRV_CPSS_DRAGONITE_LENGTH_SET_MAC(msgTypeData[type].structureLength);
    PRV_CPSS_DRAGONITE_VERSION_SET_MAC(msgTypeData[type].structureVersion);
    PRV_CPSS_DRAGONITE_ERROR_VECTOR_CLEAR_MAC;
    PRV_CPSS_DRAGONITE_RW_FLAG_READ_SET_MAC; /* trigger read operation */

    {
        GT_U32 tmpChsum;
    
        /* send pointer for start of checksum */
        tmpChsum = calcChecksum(PRV_CPSS_DRAGONITE_MSG_TYPE_LONG_PTR_MAC);
        /* add value of DRAGONITE_MO_FLAG, that is set on message send */
        tmpChsum += 0x80;

        PRV_CPSS_DRAGONITE_HDR_CHECKSUM_SET_MAC(tmpChsum);
    }

    headerDump();

    /* pass message to DRAGONITE controller */
    PRV_CPSS_DRAGONITE_MO_FLAG_DRAGONITE_SET_MAC;

    return GT_OK;
}

/*******************************************************************************
* cpssDragoniteReadTrigger
*
* DESCRIPTION:
*      Trigger DRAGONITE controller to put in shared memory data of requested type
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*       type - type of data structure application wants to read 
*                  from DRAGONITE MCU
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM   - on unknown type
*       GT_NOT_READY   - DRAGONITE controller not finished yet 
*                                          previous operation
*       GT_NOT_INITIALIZED - DragoniteInit not executed
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDragoniteReadTrigger
(
    IN CPSS_GEN_DRAGONITE_DATA_TYPE_ENT type
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDragoniteReadTrigger);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, type));

    rc = internal_cpssDragoniteReadTrigger(type);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, type));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDragoniteReadData
*
* DESCRIPTION:
*      Read DRAGONITE data structure
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*       None
*
* OUTPUTS:
*    dataPtr  - pointer to structure, that is actually union,
*             where to put data read from shared memory.
*             Pay attention dataPtr is just memory pointer - any setting in it 
*             will be ignorred - this function just returns to application 
*             what it has read from shared memory, it doesn't check if it's 
*             what application asked in ReadTrigger or Write called before
*
* RETURNS:
* --------------------
*  Return codes generated by CPSS API itself:
* --------------------
* GT_OK                    - on success
* GT_NOT_READY   - DRAGONITE controller not finished yet 
*                                    copy data to shared memory
* GT_BAD_PTR         - Illegal pointer value
* ------------------------
* Following errors returned by DRAGONITE controller communication
* protocol:
* ------------------------
* GT_CHECKSUM_ERROR - checksum error. 
* GT_BAD_SIZE - Structure length error.
* GT_NOT_SUPPORTED - Structure version mismatch.
* GT_BAD_PARAM - Unknown Type. When this bit is set,
*         there will be no data structure returned, only the
*         header.
* GT_OUT_OF_RANGE - Out of range value in the structure
*        data. The DRAGONITE manager will fill the data in the structure
*        with zeros as long as the received data was in range. In
*        case of data out of range, the received out of range
*        data will be returned.
* GT_BAD_STATE - Config is not allowed - Application can
*         send config structure to DRAGONITE controller only once after
*         reset.
* GT_ERROR - could happen only if there is something in protocol
*               that cpss is not aware of 
* GT_NOT_INITIALIZED - DragoniteInit not executed
*
* COMMENT:
*     Just one error code returned at once - error priority list:
*     GT_CHECKSUM_ERROR - highest priority
*     GT_BAD_PARAM
*     GT_NOT_SUPPORTED
*     GT_BAD_SIZE
*     GT_BAD_STATE (specific for Config structure)
*     GT_OUT_OF_RANGE (fields with wrong size returned in
*                                           dataPtr)
*
*******************************************************************************/
static GT_STATUS internal_cpssDragoniteReadData
(
    OUT CPSS_GEN_DRAGONITE_DATA_STC *dataPtr
)
{
    GT_U32 errorArray;

    CPSS_NULL_PTR_CHECK_MAC(dataPtr);

    PRV_CPSS_DRAGONITE_INIT_CHECK_MAC;

    if(PRV_CPSS_DRAGONITE_MO_FLAG_GET_MAC)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_READY, LOG_ERROR_NO_MSG);

    DBG_INFO(("DRAGONITE_HDR_CHECKSUM=%d,calcChecksum=%d\n", 
                PRV_CPSS_DRAGONITE_HDR_CHECKSUM_GET_MAC, calcChecksum(PRV_CPSS_DRAGONITE_MSG_TYPE_LONG_PTR_MAC)));

    if(PRV_CPSS_DRAGONITE_HDR_CHECKSUM_GET_MAC != calcChecksum(PRV_CPSS_DRAGONITE_MSG_TYPE_LONG_PTR_MAC))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_CHECKSUM_ERROR, LOG_ERROR_NO_MSG);

    DBG_INFO(("DRAGONITE_MSG_TYPE=%d\n", PRV_CPSS_DRAGONITE_MSG_TYPE_GET_MAC));

    if(PRV_CPSS_DRAGONITE_MSG_TYPE_GET_MAC > CPSS_GEN_DRAGONITE_DATA_TYPE_MAX_E)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    else
        dataPtr->dataType = PRV_CPSS_DRAGONITE_MSG_TYPE_GET_MAC;

    DBG_INFO(("dataPtr->dataType=%d\n", dataPtr->dataType));

    DBG_INFO(("DRAGONITE_LENGTH=%d,structureLength=%d\n", PRV_CPSS_DRAGONITE_LENGTH_GET_MAC, 
                msgTypeData[dataPtr->dataType].structureLength));
    if(PRV_CPSS_DRAGONITE_LENGTH_GET_MAC != msgTypeData[dataPtr->dataType].structureLength)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_SIZE, LOG_ERROR_NO_MSG);

    DBG_INFO(("DRAGONITE_VERSION=%d,structureVersion=%d\n", PRV_CPSS_DRAGONITE_VERSION_GET_MAC, 
                msgTypeData[dataPtr->dataType].structureVersion));
    if(PRV_CPSS_DRAGONITE_VERSION_GET_MAC != msgTypeData[dataPtr->dataType].structureVersion)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    errorArray = PRV_CPSS_DRAGONITE_ERROR_VECTOR_GET_MAC;
    DBG_INFO(("DRAGONITE_ERROR_VECTOR=0x%08x\n", errorArray));
    if(errorArray & PRV_CPSS_DRAGONITE_ERROR_CHECKSUM_CNS)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_CHECKSUM_ERROR, LOG_ERROR_NO_MSG);

    if(errorArray & PRV_CPSS_DRAGONITE_ERROR_UNKNOWN_TYPE_CNS)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    if(errorArray & PRV_CPSS_DRAGONITE_ERROR_VERSION_MISMATCH_CNS)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    if(errorArray & PRV_CPSS_DRAGONITE_ERROR_BAD_SIZE_CNS)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_SIZE, LOG_ERROR_NO_MSG);

    if(errorArray & PRV_CPSS_DRAGONITE_ERROR_BAD_STATE_CNS)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);

    dataSwap();

    msgTypeData[dataPtr->dataType].pt2DecodeFunc(dataPtr);

    /* check out of range here, because for this error 
     * problematic value saved in data
     */
    if(errorArray & PRV_CPSS_DRAGONITE_ERROR_OUT_OF_RANGE_CNS)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    if(errorArray != 0)
    {
        DBG_INFO(("cpssDragoniteReadData-unknown error\n"));
        CPSS_LOG_ERROR_AND_RETURN_MAC(((GT_STATUS)GT_ERROR), LOG_ERROR_NO_MSG); /* could happen only if there is something in protocol
                          * that cpss is not aware of 
                          */
    }        

    DBG_INFO(("cpssDragoniteReadData-OK\n"));

    return GT_OK;
}

/*******************************************************************************
* cpssDragoniteReadData
*
* DESCRIPTION:
*      Read DRAGONITE data structure
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*       None
*
* OUTPUTS:
*    dataPtr  - pointer to structure, that is actually union,
*             where to put data read from shared memory.
*             Pay attention dataPtr is just memory pointer - any setting in it 
*             will be ignorred - this function just returns to application 
*             what it has read from shared memory, it doesn't check if it's 
*             what application asked in ReadTrigger or Write called before
*
* RETURNS:
* --------------------
*  Return codes generated by CPSS API itself:
* --------------------
* GT_OK                    - on success
* GT_NOT_READY   - DRAGONITE controller not finished yet 
*                                    copy data to shared memory
* GT_BAD_PTR         - Illegal pointer value
* ------------------------
* Following errors returned by DRAGONITE controller communication
* protocol:
* ------------------------
* GT_CHECKSUM_ERROR - checksum error. 
* GT_BAD_SIZE - Structure length error.
* GT_NOT_SUPPORTED - Structure version mismatch.
* GT_BAD_PARAM - Unknown Type. When this bit is set,
*         there will be no data structure returned, only the
*         header.
* GT_OUT_OF_RANGE - Out of range value in the structure
*        data. The DRAGONITE manager will fill the data in the structure
*        with zeros as long as the received data was in range. In
*        case of data out of range, the received out of range
*        data will be returned.
* GT_BAD_STATE - Config is not allowed - Application can
*         send config structure to DRAGONITE controller only once after
*         reset.
* GT_ERROR - could happen only if there is something in protocol
*               that cpss is not aware of 
* GT_NOT_INITIALIZED - DragoniteInit not executed
*
* COMMENT:
*     Just one error code returned at once - error priority list:
*     GT_CHECKSUM_ERROR - highest priority
*     GT_BAD_PARAM
*     GT_NOT_SUPPORTED
*     GT_BAD_SIZE
*     GT_BAD_STATE (specific for Config structure)
*     GT_OUT_OF_RANGE (fields with wrong size returned in
*                                           dataPtr)
*
*******************************************************************************/
GT_STATUS cpssDragoniteReadData
(
    OUT CPSS_GEN_DRAGONITE_DATA_STC *dataPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDragoniteReadData);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dataPtr));

    rc = internal_cpssDragoniteReadData(dataPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dataPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDragoniteWrite
*
* DESCRIPTION:
*           Write DRAGONITE data structure of requested type
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*       dataPtr  - pointer to structure that is actually union,
*                  contain data to send to DRAGONITE controller
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_READY     - if shared memory isn't free yet
*       GT_BAD_VALUE     - if one of fields exceeds number of bits for field
*       GT_BAD_PARAM     - wrong structure type
*       GT_NOT_SUPPORTED - on try to write to Read Only structure type
*       GT_NOT_INITIALIZED - DragoniteInit not executed
*
* COMMENT: 
*       after cpssDxChDragoniteWrite application should 
*       call cpssDxChDragoniteReadData to ensure there was no 
*       communication error - it could be done by polling after 
*       delay or upon write_done interrupt.
*
*******************************************************************************/
static GT_STATUS internal_cpssDragoniteWrite
(
    IN CPSS_GEN_DRAGONITE_DATA_STC *dataPtr
)
{
    GT_STATUS rc = GT_OK;

    PRV_CPSS_DRAGONITE_INIT_CHECK_MAC;

    if(PRV_CPSS_DRAGONITE_MO_FLAG_GET_MAC)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_READY, LOG_ERROR_NO_MSG);

    if(dataPtr->dataType > CPSS_GEN_DRAGONITE_DATA_TYPE_MAX_E)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    /* clear shared memory before write */
    cpssOsMemSet(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC, 0, msgTypeData[dataPtr->dataType].structureLength);

    PRV_CPSS_DRAGONITE_MSG_TYPE_SET_MAC(dataPtr->dataType);

    /* check if requested structure has writable fields */
    if(msgTypeData[dataPtr->dataType].pt2EncodeFunc == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    PRV_CPSS_DRAGONITE_LENGTH_SET_MAC(msgTypeData[dataPtr->dataType].structureLength);
    PRV_CPSS_DRAGONITE_VERSION_SET_MAC(msgTypeData[dataPtr->dataType].structureVersion);
    PRV_CPSS_DRAGONITE_ERROR_VECTOR_CLEAR_MAC;
    PRV_CPSS_DRAGONITE_RW_FLAG_WRITE_SET_MAC; /* execute write operation */

    if((rc = msgTypeData[dataPtr->dataType].pt2EncodeFunc(dataPtr)) != GT_OK)
        return rc;

    {
        GT_U32 tmpChsum;
    
        /* send pointer for start of checksum */
        tmpChsum = calcChecksum(PRV_CPSS_DRAGONITE_MSG_TYPE_LONG_PTR_MAC);
        /* add value of DRAGONITE_MO_FLAG, that is set on message send */
        tmpChsum += 0x80;

        PRV_CPSS_DRAGONITE_HDR_CHECKSUM_SET_MAC(tmpChsum);
    }

    dataSwap();

    /* pass message to DRAGONITE controller */
    PRV_CPSS_DRAGONITE_MO_FLAG_DRAGONITE_SET_MAC;

    return rc;
}

/*******************************************************************************
* cpssDragoniteWrite
*
* DESCRIPTION:
*           Write DRAGONITE data structure of requested type
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*       dataPtr  - pointer to structure that is actually union,
*                  contain data to send to DRAGONITE controller
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_READY     - if shared memory isn't free yet
*       GT_BAD_VALUE     - if one of fields exceeds number of bits for field
*       GT_BAD_PARAM     - wrong structure type
*       GT_NOT_SUPPORTED - on try to write to Read Only structure type
*       GT_NOT_INITIALIZED - DragoniteInit not executed
*
* COMMENT: 
*       after cpssDxChDragoniteWrite application should 
*       call cpssDxChDragoniteReadData to ensure there was no 
*       communication error - it could be done by polling after 
*       delay or upon write_done interrupt.
*
*******************************************************************************/
GT_STATUS cpssDragoniteWrite
(
    IN CPSS_GEN_DRAGONITE_DATA_STC *dataPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDragoniteWrite);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dataPtr));

    rc = internal_cpssDragoniteWrite(dataPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dataPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/******************************************************************************
 *                            private functions
 ******************************************************************************/

/******************************************************************************
 *                      Write data encode functions
 ******************************************************************************/


/*******************************************************************************
* prvCpssDragoniteEncodeConfig
*
* DESCRIPTION:
*           translate config structure from SW format to HW
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*    msgPtr - pointer to communication message SW structure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_BAD_VALUE - if wrong value provided for some fields
*       GT_OK - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDragoniteEncodeConfig
(
    IN CPSS_GEN_DRAGONITE_DATA_STC *msgPtr
)
{
    CPSS_GEN_DRAGONITE_CONFIG_STC *structPtr;
    GT_U32  i;

    structPtr = &msgPtr->dragoniteData.config;

    U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0], 0, 1, BOOL2BIT_MAC(structPtr->dcDisconnectEn));
    U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0], 1, 1, BOOL2BIT_MAC(structPtr->externalSyncDis));
    U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0], 2, 1, BOOL2BIT_MAC(structPtr->capDis));
    U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0], 3, 1, BOOL2BIT_MAC(structPtr->disPortsOverride));
    U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0], 4, 3, 1); /* reserved0 */
    U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0], 7, 1, BOOL2BIT_MAC(structPtr->rprDisable));    
    U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0], 8, 1, 0); /* reserved1 */
    U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0], 9, 1, BOOL2BIT_MAC(structPtr->vmainAtPolicyEn));
    U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0], 10, 1, BOOL2BIT_MAC(structPtr->class0EqAf));
    U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0], 11, 1, BOOL2BIT_MAC(structPtr->class123EqAf));
    U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0], 12, 1, BOOL2BIT_MAC(structPtr->classBypass2ndError));
    U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0], 13, 1, BOOL2BIT_MAC(structPtr->classErrorEq0));
    U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0], 14, 1, BOOL2BIT_MAC(structPtr->classErrorEq4));
    U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0], 15, 1, BOOL2BIT_MAC(structPtr->layer2En));
    U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0], 16, 1, BOOL2BIT_MAC(structPtr->portPrioritySetByPD));
    U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0], 17, 1, BOOL2BIT_MAC(structPtr->privateLableBit));
    U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0], 18, 1, BOOL2BIT_MAC(structPtr->lowPriDiscoForHiPriStartupEn));
    U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0], 19, 13, 0); /* reserved2 */

    if(structPtr->portMethodLimitMode >= BIT_4)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_VALUE, LOG_ERROR_NO_MSG);
    else
        U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[1], 0, 4, structPtr->portMethodLimitMode);

    if(structPtr->portMethodCalcMode >= BIT_4)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_VALUE, LOG_ERROR_NO_MSG);
    else
        U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[1], 4, 4, structPtr->portMethodCalcMode);

    PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[2] = structPtr->guardBandValue;

    U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[3], 0, 6, 0x3f); /* StartupCR.reserved0 */
    U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[3], 6, 1, BOOL2BIT_MAC(structPtr->startupHiPrOverride)); 

    PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[4] = structPtr->IcutMaxAt;
    PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[5] = structPtr->tempAlarmTh;
    PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[6] = structPtr->vmainHighTh;
    PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[7] = structPtr->vmainAtLowTh;
    PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[8] = structPtr->vmainAfLowTh;

    for(i = 0; i < CPSS_GEN_DRAGONITE_PORTS_NUM_CNS; i++)
    {
        if(structPtr->matrixPort[i].physicalPortNumber >= CPSS_GEN_DRAGONITE_PORTS_NUM_CNS)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_VALUE, LOG_ERROR_NO_MSG);
        else
            U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[9+i], 0, 8, structPtr->matrixPort[i].physicalPortNumber);

        if(structPtr->matrixPort[i].chipNumber >= CPSS_GEN_DRAGONITE_CHIPS_NUM_CNS)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_VALUE, LOG_ERROR_NO_MSG);
        else
            U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[9+i], 8, 4, structPtr->matrixPort[i].chipNumber);
    }

    PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[105] = structPtr->commPollingTime;
    PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[106] = structPtr->irqGenTime;    

    return GT_OK;
}

/*******************************************************************************
* prvCpssDragoniteEncodeSystem
*
* DESCRIPTION:
*           translate system structure from SW format to HW
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*    msgPtr - pointer to communication message SW structure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_BAD_VALUE - if wrong value provided for some fields
*       GT_OK - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDragoniteEncodeSystem
(
    IN CPSS_GEN_DRAGONITE_DATA_STC *msgPtr
)
{
    CPSS_GEN_DRAGONITE_SYSTEM_STC *structPtr;
    GT_U32 i;

    structPtr = &msgPtr->dragoniteData.system;

    for(i = 0; i < CPSS_GEN_DRAGONITE_POWER_BUDGETS_NUM_CNS; i++)
    {
        if(structPtr->powerBudgetArr[i].availablePower >= BIT_16)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_VALUE, LOG_ERROR_NO_MSG);
        else
            U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[2+i], 0, 16, structPtr->powerBudgetArr[i].availablePower); 

        if(structPtr->powerBudgetArr[i].powerSourceType >= BIT_2)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_VALUE, LOG_ERROR_NO_MSG);
        else
            U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[2+i], 16, 2, structPtr->powerBudgetArr[i].powerSourceType); 
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDragoniteEncodePortCmd
*
* DESCRIPTION:
*           translate port command structure from SW format to HW
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*    msgPtr - pointer to communication message SW structure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_BAD_VALUE - if wrong value provided for some fields
*       GT_OK - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDragoniteEncodePortCmd
(
    IN CPSS_GEN_DRAGONITE_DATA_STC *msgPtr
)
{
    CPSS_GEN_DRAGONITE_PORT_COMMAND_STC *structPtr;
    GT_U32 i;

    structPtr = msgPtr->dragoniteData.portCmd;
    
    for(i = 0; i < CPSS_GEN_DRAGONITE_PORTS_NUM_CNS; i++)
    {
        if(structPtr[i].portCtrl.pseEnable >= BIT_2)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_VALUE, LOG_ERROR_NO_MSG);
        else
            U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i], 0, 2, structPtr[i].portCtrl.pseEnable);

        if(structPtr[i].portCtrl.pairControl >= BIT_2)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_VALUE, LOG_ERROR_NO_MSG);
        else
            U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i], 2, 2, structPtr[i].portCtrl.pairControl);

        if(structPtr[i].portCtrl.portMode >= BIT_2)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_VALUE, LOG_ERROR_NO_MSG);
        else
            U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i], 4, 2, structPtr[i].portCtrl.portMode);

        if(structPtr[i].portCtrl.portPriority >= BIT_2)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_VALUE, LOG_ERROR_NO_MSG);
        else
            U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i], 6, 2, structPtr[i].portCtrl.portPriority);

        PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+CPSS_GEN_DRAGONITE_PORTS_NUM_CNS] = structPtr[i].portPpl;
        PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+(CPSS_GEN_DRAGONITE_PORTS_NUM_CNS*2)] = structPtr[i].portTppl;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDragoniteEncodePortLayer2
*
* DESCRIPTION:
*           translate port layer2 structure from SW format to HW
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*    msgPtr - pointer to communication message SW structure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_BAD_VALUE - if wrong value provided for some fields
*       GT_OK - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDragoniteEncodePortLayer2
(
    IN CPSS_GEN_DRAGONITE_DATA_STC *msgPtr
)
{
    CPSS_GEN_DRAGONITE_PORT_LAYER2_STC *structPtr;
    GT_U32 i;

    structPtr = msgPtr->dragoniteData.layer2;
    
    for(i = 0; i < CPSS_GEN_DRAGONITE_PORTS_NUM_CNS; i++)
    {
        if(structPtr[i].layer2PdInfo.pdRequestedPower >= BIT_16)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_VALUE, LOG_ERROR_NO_MSG);
        else
            U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i], 0, 16, structPtr[i].layer2PdInfo.pdRequestedPower);

        if(structPtr[i].layer2PdInfo.pdPriority >= BIT_2)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_VALUE, LOG_ERROR_NO_MSG);
        else
            U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i], 16, 2, structPtr[i].layer2PdInfo.pdPriority);

        if(structPtr[i].layer2PdInfo.portCableLen >= BIT_8)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_VALUE, LOG_ERROR_NO_MSG);
        else
            U32_SET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i], 18, 8, structPtr[i].layer2PdInfo.portCableLen);
    }

    return GT_OK;
}

/******************************************************************************
 *                        Read data decode functions
 ******************************************************************************/

/*******************************************************************************
* prvCpssDragoniteDecodeConfig
*
* DESCRIPTION:
*           translate config structure from HW format to SW
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*    msgPtr - pointer to communication message SW structure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_VOID prvCpssDragoniteDecodeConfig
(
    IN CPSS_GEN_DRAGONITE_DATA_STC *msgPtr
)
{
    CPSS_GEN_DRAGONITE_CONFIG_STC *structPtr;
    GT_U32  i;

    structPtr = &msgPtr->dragoniteData.config;
    
    structPtr->dcDisconnectEn       = BIT2BOOL_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0] & 1);
    structPtr->externalSyncDis      = BIT2BOOL_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0]>>1 & 1);
    structPtr->capDis               = BIT2BOOL_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0]>>2 & 1);
    structPtr->disPortsOverride     = BIT2BOOL_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0]>>3 & 1);
    /* bit 6 reserved */
    structPtr->rprDisable           = BIT2BOOL_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0]>>7 & 1);
    structPtr->vmainAtPolicyEn      = BIT2BOOL_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0]>>9 & 1);
    structPtr->class0EqAf           = BIT2BOOL_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0]>>10 & 1);
    structPtr->class123EqAf         = BIT2BOOL_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0]>>11 & 1);
    structPtr->classBypass2ndError  = BIT2BOOL_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0]>>12 & 1);
    structPtr->classErrorEq0        = BIT2BOOL_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0]>>13 & 1);
    structPtr->classErrorEq4        = BIT2BOOL_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0]>>14 & 1);
    structPtr->layer2En             = BIT2BOOL_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0]>>15 & 1);
    structPtr->portPrioritySetByPD  = BIT2BOOL_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0]>>16 & 1);
    structPtr->privateLableBit      = BIT2BOOL_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0]>>17 & 1);
    structPtr->lowPriDiscoForHiPriStartupEn = BIT2BOOL_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[0]>>18 & 1);

    structPtr->portMethodLimitMode  = U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[1], 0, 4);
    structPtr->portMethodCalcMode   = U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[1], 4, 4); 
    structPtr->guardBandValue       = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[2];
    structPtr->startupHiPrOverride  = BIT2BOOL_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[3]>>6 & 1);

    structPtr->IcutMaxAt            = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[4];
    structPtr->tempAlarmTh          = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[5];
    structPtr->vmainHighTh          = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[6];
    structPtr->vmainAtLowTh         = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[7];
    structPtr->vmainAfLowTh         = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[8];

    for(i = 0; i < CPSS_GEN_DRAGONITE_PORTS_NUM_CNS; i++)
    {
        structPtr->matrixPort[i].physicalPortNumber = 
            (GT_U8)U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[9+i], 0, 8);
        structPtr->matrixPort[i].chipNumber = 
            (GT_U8)U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[9+i], 8, 4);
    }

    structPtr->commPollingTime      = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[105];
    structPtr->irqGenTime           = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[106];

    return;
}

/*******************************************************************************
* prvCpssDragoniteDecodeSystem
*
* DESCRIPTION:
*           translate system structure from HW format to SW
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*    msgPtr - pointer to communication message SW structure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_VOID prvCpssDragoniteDecodeSystem
(
    IN CPSS_GEN_DRAGONITE_DATA_STC *msgPtr
)
{
    CPSS_GEN_DRAGONITE_SYSTEM_STC *structPtr;
    GT_U32 i;

    structPtr = &msgPtr->dragoniteData.system;

    structPtr->activeBudget = U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[1], 0, 4); 
    structPtr->vmain = U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[1], 4, 10);  

    for(i = 0; i < CPSS_GEN_DRAGONITE_POWER_BUDGETS_NUM_CNS; i++)
    {
        structPtr->powerBudgetArr[i].availablePower = U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[2+i], 0, 16); 
        structPtr->powerBudgetArr[i].powerSourceType = U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[2+i], 16, 2); 
    }
    
    structPtr->vmainHigh            = BIT2BOOL_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[18] & 1);
    structPtr->vmainLowAT           = BIT2BOOL_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[18]>>1 & 1);
    structPtr->vmainLowAF           = BIT2BOOL_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[18]>>2 & 1);
    structPtr->tempAlarm            = BIT2BOOL_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[18]>>3 & 1);
    structPtr->overTemp             = BIT2BOOL_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[18]>>4 & 1);
    structPtr->disablePortsActive   = BIT2BOOL_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[18]>>5 & 1);
    structPtr->osStatus             = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[19];

    for(i = 0; i < CPSS_GEN_DRAGONITE_CHIPS_NUM_CNS; i++)
    {
        structPtr->chipStatusArr[i] = U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[20], i<<1, 2); 
    }
    
    structPtr->sysTotalCriticalCons = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[21];
    structPtr->sysTotalHighCons     = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[22];
    structPtr->sysTotalLowCons      = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[23];
    structPtr->sysTotalCriticalReq  = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[24];
    structPtr->sysTotalHighReq      = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[25];
    structPtr->sysTotalLowReq       = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[26];
    structPtr->sysTotalCalcPowerCons = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[27];
    structPtr->sysTotalPowerRequest = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[28];
    structPtr->sysTotalDeltaPower   = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[29];
    structPtr->sysTotalRealPowerCons = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[30];
    structPtr->minorVersion         = U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[31], 0, 16);
    structPtr->majorVersion         = U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[31], 16, 16); 

    if(bigEndian)
    { /* bldDateTime is bytes array, so it doesn't need swap - but I execute swap of all data before structure
       * decode while working with big-endian host, so here I must do swap twice to keep right format */
        for(i = 0; i < CPSS_GEN_DRAGONITE_BLD_TIME_STRING_LEN_CNS>>2; i++)
            PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[32+i] = BYTESWAP_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[32+i]);
    }
    cpssOsMemCpy(structPtr->bldDateTime, &PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[32], CPSS_GEN_DRAGONITE_BLD_TIME_STRING_LEN_CNS);

    structPtr->checkSumErrorCounter = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[37];
    structPtr->lengthErrorCounter   = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[38];
    structPtr->structVersionErrorCounter = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[39];
    structPtr->typeErrorCounter     = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[40];
    structPtr->configErrorCounter   = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[41];
    structPtr->irqErrorCounter      = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[42];

    return;
}

/*******************************************************************************
* prvCpssDragoniteDecodeChip
*
* DESCRIPTION:
*           translate chip info structure from HW format to SW
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*    msgPtr - pointer to communication message SW structure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_VOID prvCpssDragoniteDecodeChip
(
    IN CPSS_GEN_DRAGONITE_DATA_STC *msgPtr
)
{
    CPSS_GEN_DRAGONITE_CHIP_STC *structPtr;
    GT_U32 i;

    structPtr = msgPtr->dragoniteData.chip;
    for(i = 0; i < CPSS_GEN_DRAGONITE_CHIPS_NUM_CNS; i++)
    {        
        structPtr[i].chipInfo = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i];
        structPtr[i].measuredTemp = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+(CPSS_GEN_DRAGONITE_CHIPS_NUM_CNS)];
        structPtr[i].maxMeasuredTemp = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+(CPSS_GEN_DRAGONITE_CHIPS_NUM_CNS*2)];
        structPtr[i].numOfPorts = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+(CPSS_GEN_DRAGONITE_CHIPS_NUM_CNS*3)];
        structPtr[i].vmainHighError = BIT2BOOL_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+(CPSS_GEN_DRAGONITE_CHIPS_NUM_CNS*4)] & 1);
        structPtr[i].overTempError = BIT2BOOL_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+(CPSS_GEN_DRAGONITE_CHIPS_NUM_CNS*4)]>>1 & 1);
        structPtr[i].disablePortsActiveError = BIT2BOOL_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+(CPSS_GEN_DRAGONITE_CHIPS_NUM_CNS*4)]>>2 & 1);
        structPtr[i].vmainLowAfError = BIT2BOOL_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+(CPSS_GEN_DRAGONITE_CHIPS_NUM_CNS*4)]>>3 & 1);
        structPtr[i].vmainLowAtError = BIT2BOOL_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+(CPSS_GEN_DRAGONITE_CHIPS_NUM_CNS*4)]>>4 & 1);
        structPtr[i].tempAlarm = BIT2BOOL_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+(CPSS_GEN_DRAGONITE_CHIPS_NUM_CNS*4)]>>5 & 1);
        structPtr[i].chipDevId = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+(CPSS_GEN_DRAGONITE_CHIPS_NUM_CNS*5)];
    }

    return;
}

/*******************************************************************************
* prvCpssDragoniteDecodePortCmd
*
* DESCRIPTION:
*           translate port command structure from HW format to SW
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*    msgPtr - pointer to communication message SW structure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_VOID prvCpssDragoniteDecodePortCmd
(
    IN CPSS_GEN_DRAGONITE_DATA_STC *msgPtr
)
{
    CPSS_GEN_DRAGONITE_PORT_COMMAND_STC *structPtr;
    GT_U32 i;

    structPtr = msgPtr->dragoniteData.portCmd;
    
    for(i = 0; i < CPSS_GEN_DRAGONITE_PORTS_NUM_CNS; i++)
    {
        structPtr[i].portCtrl.pseEnable = U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i], 0, 2);
        structPtr[i].portCtrl.pairControl = U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i], 2, 2);
        structPtr[i].portCtrl.portMode = U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i], 4, 2);
        structPtr[i].portCtrl.portPriority = U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i], 6, 2);
        structPtr[i].portPpl = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+CPSS_GEN_DRAGONITE_PORTS_NUM_CNS];
        structPtr[i].portTppl = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+(CPSS_GEN_DRAGONITE_PORTS_NUM_CNS*2)];
    }

    return;
}

/*******************************************************************************
* prvCpssDragoniteDecodePortCntr
*
* DESCRIPTION:
*           translate port counters structure from HW format to SW
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*    msgPtr - pointer to communication message SW structure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_VOID prvCpssDragoniteDecodePortCntr
(
    IN CPSS_GEN_DRAGONITE_DATA_STC *msgPtr
)
{
    CPSS_GEN_DRAGONITE_PORT_COUNTERS_STC *structPtr;
    GT_U32 i;

    structPtr = &msgPtr->dragoniteData.portCntrs;
    
    for(i = 0; i < CPSS_GEN_DRAGONITE_PORTS_NUM_CNS; i++)
    {
        structPtr->invalidSignatureCounter[i]   = (GT_U8)U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i], 0, 8);
        structPtr->powerDeniedCounter[i]        = (GT_U8)U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+CPSS_GEN_DRAGONITE_PORTS_NUM_CNS], 0, 8);
        structPtr->overloadCounter[i]           = (GT_U8)U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+(CPSS_GEN_DRAGONITE_PORTS_NUM_CNS*2)], 0, 8);
        structPtr->shortCyrcuitCounter[i]       = (GT_U8)U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+(CPSS_GEN_DRAGONITE_PORTS_NUM_CNS*3)], 0, 8);
        structPtr->underloadCounter[i]          = (GT_U8)U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+(CPSS_GEN_DRAGONITE_PORTS_NUM_CNS*4)], 0, 8);
        structPtr->classErrorCounter[i]         = (GT_U8)U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+(CPSS_GEN_DRAGONITE_PORTS_NUM_CNS*5)], 0, 8);
    }

    return;
}

/*******************************************************************************
* prvCpssDragoniteDecodePortStatus
*
* DESCRIPTION:
*           translate port status structure from HW format to SW
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*    msgPtr - pointer to communication message SW structure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_VOID prvCpssDragoniteDecodePortStatus
(
    IN CPSS_GEN_DRAGONITE_DATA_STC *msgPtr
)
{
    CPSS_GEN_DRAGONITE_PORT_STATUSES_STC *structPtr;
    GT_U32 i;

    structPtr = msgPtr->dragoniteData.portStat;
    
    for(i = 0; i < CPSS_GEN_DRAGONITE_PORTS_NUM_CNS; i++)
    {
        structPtr[i].portSr.status              = U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i], 0, 8);
        structPtr[i].portSr.extStatus           = U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i], 8, 4);
        structPtr[i].portSr.portClass           = U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i], 12, 3);
        structPtr[i].portSr.portAtBehavior      = BIT2BOOL_MAC(U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i], 15, 1));
        structPtr[i].lastDisconnect             = U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+CPSS_GEN_DRAGONITE_PORTS_NUM_CNS], 0, 8);
        structPtr[i].indications.underload      = BIT2BOOL_MAC(U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+(CPSS_GEN_DRAGONITE_PORTS_NUM_CNS*2)], 0, 1));
        structPtr[i].indications.overload       = BIT2BOOL_MAC(U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+(CPSS_GEN_DRAGONITE_PORTS_NUM_CNS*2)], 1, 1));
        structPtr[i].indications.shortCircuit   = BIT2BOOL_MAC(U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+(CPSS_GEN_DRAGONITE_PORTS_NUM_CNS*2)], 2, 1));
        structPtr[i].indications.invalidSignature = BIT2BOOL_MAC(U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+(CPSS_GEN_DRAGONITE_PORTS_NUM_CNS*2)], 3, 1));
        structPtr[i].indications.validSignature = BIT2BOOL_MAC(U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+(CPSS_GEN_DRAGONITE_PORTS_NUM_CNS*2)], 4, 1));
        structPtr[i].indications.powerDenied    = BIT2BOOL_MAC(U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+(CPSS_GEN_DRAGONITE_PORTS_NUM_CNS*2)], 5, 1));
        structPtr[i].indications.validCapacitor = BIT2BOOL_MAC(U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+(CPSS_GEN_DRAGONITE_PORTS_NUM_CNS*2)], 6, 1));
        structPtr[i].indications.backoff        = BIT2BOOL_MAC(U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+(CPSS_GEN_DRAGONITE_PORTS_NUM_CNS*2)], 7, 1));
        structPtr[i].indications.classError     = BIT2BOOL_MAC(U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+(CPSS_GEN_DRAGONITE_PORTS_NUM_CNS*2)], 8, 1));
    }

    return;
}

/*******************************************************************************
* prvCpssDragoniteDecodePortMeasure
*
* DESCRIPTION:
*           translate port measure structure from HW format to SW
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*    msgPtr - pointer to communication message SW structure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_VOID prvCpssDragoniteDecodePortMeasure
(
    IN CPSS_GEN_DRAGONITE_DATA_STC *msgPtr
)
{
    CPSS_GEN_DRAGONITE_PORT_MEASUREMENTS_STC *structPtr;
    GT_U32 i;

    structPtr = msgPtr->dragoniteData.portMeasure;
    
    for(i = 0; i < CPSS_GEN_DRAGONITE_PORTS_NUM_CNS; i++)
    {
        structPtr[i].powerConsumption   = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i];
        structPtr[i].current            = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+CPSS_GEN_DRAGONITE_PORTS_NUM_CNS];
        structPtr[i].volt               = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+(CPSS_GEN_DRAGONITE_PORTS_NUM_CNS*2)];
        structPtr[i].calcPowerConsumption = PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+(CPSS_GEN_DRAGONITE_PORTS_NUM_CNS*3)];
    }

    return;
}

/*******************************************************************************
* prvCpssDragoniteDecodePortLayer2
*
* DESCRIPTION:
*           translate port layer2 structure from HW format to SW
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*    msgPtr - pointer to communication message SW structure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_VOID prvCpssDragoniteDecodePortLayer2
(
    IN CPSS_GEN_DRAGONITE_DATA_STC *msgPtr
)
{
    CPSS_GEN_DRAGONITE_PORT_LAYER2_STC *structPtr;
    GT_U32 i;

    structPtr = msgPtr->dragoniteData.layer2;
    
    for(i = 0; i < CPSS_GEN_DRAGONITE_PORTS_NUM_CNS; i++)
    {
        structPtr[i].layer2PdInfo.pdRequestedPower  = U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i], 0, 16);
        structPtr[i].layer2PdInfo.pdPriority        = U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i], 16, 2);
        structPtr[i].layer2PdInfo.portCableLen      = U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i], 18, 8);
        structPtr[i].layer2PseInfo.pseAllocPower    = U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+CPSS_GEN_DRAGONITE_PORTS_NUM_CNS], 0, 16);
        structPtr[i].layer2PseInfo.psePriority      = U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+CPSS_GEN_DRAGONITE_PORTS_NUM_CNS], 16, 2);
        structPtr[i].layer2PseInfo.pseType          = U32_GET_FIELD_MAC(PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC[i+CPSS_GEN_DRAGONITE_PORTS_NUM_CNS], 18, 2);
    }

    return;
}

/*******************************************************************************
* prvCpssDragoniteDecodeDebug
*
* DESCRIPTION:
*           translate debug structure from HW format to SW
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*    msgPtr - pointer to communication message SW structure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_VOID prvCpssDragoniteDecodeDebug
(
    IN CPSS_GEN_DRAGONITE_DATA_STC *msgPtr
)
{
/* For future possible use
    GT_U32 *structPtr;
    GT_U32 i;

    structPtr = msgPtr->dragoniteData.debugDataPtr;
    
    cpssOsMemCpy(structPtr, PRV_CPSS_DRAGONITE_MSG_DATA_PTR_MAC, PRV_CPSS_DRAGONITE_DEBUG_STRUCT_LENGTH_CNS<<2);
*/
    msgPtr = msgPtr; /* to avoid warning */

    return;
}

/*******************************************************************************
* prvCpssDragoniteDecodeOther
*
* DESCRIPTION:
*           translate structure of type "Other" from HW format to SW
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*    msgPtr - pointer to communication message SW structure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_VOID prvCpssDragoniteDecodeOther
(
    IN CPSS_GEN_DRAGONITE_DATA_STC *msgPtr
)
{
    msgPtr = msgPtr; /* to avoid warning */

    return;
}

