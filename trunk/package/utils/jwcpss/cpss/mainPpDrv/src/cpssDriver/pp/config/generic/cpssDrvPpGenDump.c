/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpGenDump.c
*
* DESCRIPTION:
*       PP Registers and PP memory dump functions implementation
*
* FILE REVISION NUMBER:
*       $Revision: 24 $
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpssDriver/log/private/prvCpssDriverLog.h>
#include <cpssDriver/pp/config/generic/cpssDrvPpGenDump.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>
#include <cpss/generic/cpssHwInit/private/prvCpssHwInit.h>
/* get the device info and common info */
#if (defined CHX_FAMILY)
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#endif

extern GT_STATUS prvCpssDrvPpDump(GT_U8   devNum);

/* define family independent function to get registers. */
extern GT_STATUS prvCpssExMxHwRegAddrStcInfoGet
(
    IN  GT_U8     devNum,
    OUT GT_U32  **regAddrPtr,
    OUT GT_U32   *sizePtr
);

extern GT_STATUS prvCpssExMxPmHwRegAddrStcInfoGet
(
    IN  GT_U8     devNum,
    OUT GT_U32  **regAddrPtrPtr,
    OUT GT_U32   *sizePtr
);

extern GT_STATUS prvCpssDxChHwRegAddrStcInfoGet
(
    IN  GT_U8     devNum,
    OUT GT_U32  **regAddrPtrPtr,
    OUT GT_U32   *sizePtr
);

extern GT_STATUS prvCpssDxChHwTblAddrStcInfoGet
(
    IN  GT_U8     devNum,
    OUT GT_U32  **tblAddrPtrPtr,
    OUT GT_U32   *sizePtr
);

extern GT_STATUS prvCpssDxSalHwRegAddrStcInfoGet
(
    IN  GT_U8     devNum,
    OUT GT_U32  **regAddrPtr,
    OUT GT_U32   *sizePtr
);

#ifdef ASIC_SIMULATION
static GT_U32   uartTimeToSleep = 0;
/* used print routine ,with sleep every 16 times to let dump go into uart  */
    #define DUMP_PRINT(x) cpssOsPrintf x ; \
        if(((uartTimeToSleep++)&0xf) == 0) cpssOsTimerWkAfter(1)
#else
    #define DUMP_PRINT(x) cpssOsPrintf x
#endif

/* size of dump text line in words */
#define PRV_WORDS_IN_DUMP_LINE_CNS  4

/* 16 bytes alignment for memory dump start address */
#define PRV_16_BYTE_ALIGNMENT_CNS   0xF  /* 00001111 */

/* if register address >= this const - it treated as incorrect   */
#define MAX_PP_REG_ADDR_CNS  0xFFFFFFF0


/*
 * Typedef: union PRV_DUMP_LINE_UNT
 *
 * Description: stores information for one text line of memory dump (16 bytes)
 *
 * Fields:
 *      words  - as 4  WORDs
 *      shorts - as 8  SHORTs
 *      bytes  - as 16 BYTEs
 */
typedef union
{
    GT_U32  words[PRV_WORDS_IN_DUMP_LINE_CNS];
    GT_U16 shorts[2 * PRV_WORDS_IN_DUMP_LINE_CNS];
    GT_U8   bytes[4 * PRV_WORDS_IN_DUMP_LINE_CNS];
}PRV_DUMP_LINE_UNT;

/*******************************************************************************
* getRegInfo
*
* DESCRIPTION:
*       local routine which prints information about Packet processor
*
* INPUTS:
*       devNum - PP's device number .
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - invalid devNum
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS getRegInfo
(
    IN  GT_U8     devNum,
    OUT GT_U32  **regAddrPtrPtr,
    OUT GT_U32   *sizePtr
)
{
    GT_STATUS rc=GT_FAIL;

    if(CPSS_IS_EXMX_FAMILY_MAC(PRV_CPSS_PP_MAC(devNum)->devFamily))
    {
#if ( ((defined EX_FAMILY) || (defined MX_FAMILY)) && !(defined CPSS_PRODUCT))
        rc = prvCpssExMxHwRegAddrStcInfoGet(devNum, regAddrPtrPtr, sizePtr);
#endif /*(defined EX_FAMILY) || (defined MX_FAMILY)*/
    }
    else
    if(CPSS_IS_DXSAL_FAMILY_MAC(PRV_CPSS_PP_MAC(devNum)->devFamily))
    {
#if (defined SAL_FAMILY)
        rc = prvCpssDxSalHwRegAddrStcInfoGet(devNum, regAddrPtrPtr, sizePtr);
#endif /*(defined SAL_FAMILY)*/
    }
    else
    if(CPSS_IS_DXCH_FAMILY_MAC(PRV_CPSS_PP_MAC(devNum)->devFamily))
    {
#if (defined CHX_FAMILY)
        rc = prvCpssDxChHwRegAddrStcInfoGet(devNum, regAddrPtrPtr, sizePtr);
#endif /*(defined CHX_FAMILY) */
    }
    else
    if(CPSS_IS_EXMXPM_FAMILY_MAC(PRV_CPSS_PP_MAC(devNum)->devFamily))
    {
#if (defined EXMXPM_FAMILY)
        rc = prvCpssExMxPmHwRegAddrStcInfoGet(devNum, regAddrPtrPtr, sizePtr);
#endif /*(defined EXMXPM_FAMILY)*/
    }

    return rc;

}

/*******************************************************************************
* getTblInfo
*
* DESCRIPTION:
*       local routine which prints information about Packet processor
*
* INPUTS:
*       devNum - PP's device number .
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - invalid devNum
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS getTblInfo
(
    IN  GT_U8     devNum,
    OUT GT_U32  **regAddrPtr,
    OUT GT_U32   *sizePtr
)
{
    GT_STATUS rc=GT_FAIL;

    /* avoid warning */
    regAddrPtr = regAddrPtr;
    sizePtr = sizePtr;

    if(CPSS_IS_DXCH_FAMILY_MAC(PRV_CPSS_PP_MAC(devNum)->devFamily))
    {
#if (defined CHX_FAMILY)
        rc = prvCpssDxChHwTblAddrStcInfoGet(devNum, regAddrPtr, sizePtr);
#endif /*(defined CHX_FAMILY) */
    }
    else
    {
        rc = GT_NOT_SUPPORTED;
    }

    return rc;
}

/*******************************************************************************
* cpssPpDumpRegisters
*
* DESCRIPTION:
*       This routine print dump of PP registers.
*
* INPUTS:
*       devNum   - PP's device number .
*       regArray - array of reg addresses
*       regArray - count of registers to dump.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success  (all Registers dumped successfully)
*       GT_FAIL         - on failure. (at least one register read error)
*       GT_BAD_PARAM    - invalid device number
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS printPpDumpRegisters
(
    IN GT_U8     devNum,
    IN GT_U32    portGroupId,
    IN GT_U32   *regArray,
    IN GT_U32    regCount
)
{
    GT_U32      regAddr;    /* current register data        */
    GT_U32      regData;    /* current register data        */
    GT_STATUS   rc = GT_OK; /* func return code             */
    GT_U32      i;
    PRV_CPSS_BOOKMARK_STC *bookmark;
    GT_U32  actualNumReg = 0;

    for (i = 0; i < regCount; i++)
    {
        regAddr = regArray[i];

        /* if regAddr Ok - read info from Pp    */
        if (regArray[i] == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            continue; /* Skip unused entry */
        }
        if (regArray[i] == PRV_CPSS_SW_PTR_BOOKMARK_CNS)
        {
            bookmark = (PRV_CPSS_BOOKMARK_STC *)(&(regArray[i]));

            /*Check for write only data*/
            if (bookmark->type & PRV_CPSS_SW_TYPE_WRONLY_CNS)
            {
                /* if the pointer field data is write only skip the pointer, else skip the whole range*/
                if (bookmark->type & PRV_CPSS_SW_TYPE_PTR_FIELD_CNS)
                {
                    i += sizeof(*bookmark)/sizeof(GT_U32) - 1;
                }
                else
                {
                    i += 1 + bookmark->size;
                }
            }
            else
            {
                if((i+sizeof(*bookmark)/sizeof(GT_U32)-1) < regCount)
                {
                    printPpDumpRegisters (devNum, portGroupId, bookmark->nextPtr, bookmark->size/4 );
                    i += sizeof(*bookmark)/sizeof(GT_U32) - 1;
                }
            }
        }
        else
        {
            actualNumReg++;/* increment the number of actual registers that are used */
            rc = prvCpssHwPpPortGroupReadRegister(devNum,portGroupId,regAddr,&regData);
            /* print register address and it's value if read ok   */
            if (rc == GT_OK)
            {
                DUMP_PRINT(("0x%8.8x : %8.8x\n", regAddr, regData));
            }
            else
            {
                DUMP_PRINT(("0x%8.8x : ERROR\n", regAddr));
            }
        }
    }
    DUMP_PRINT(("\n"));
    DUMP_PRINT((" Actual number of registers dump: %d\n\n", actualNumReg));
    DUMP_PRINT(("\n"));

    return rc;
}

/*******************************************************************************
* internal_cpssPpDumpRegisters
*
* DESCRIPTION:
*       This routine print dump of PP registers.
*
* INPUTS:
*       devNum - PP's device number .
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success  (all Registers dumped successfully)
*       GT_FAIL         - on failure. (at least one register read error)
*       GT_BAD_PARAM    - invalid device number
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssPpDumpRegisters
(
    IN GT_U8                    devNum
)
{
    GT_U32      regCount;   /* count of registers to dump           */
    GT_U32      *regArray, size;  /* array of pointers to reg addresses and its size   */
    GT_STATUS   rc = GT_OK; /* func return code             */

    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        DUMP_PRINT(("bad device number [%d] \n",devNum));
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(GT_OK != getRegInfo(devNum, &regArray, &size))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    regCount = size / sizeof(GT_U32);


    DUMP_PRINT(("\nstart Dump Registers for device number [%d]: \n",devNum));

    /* print information about Packet processor */
    prvCpssDrvPpDump(devNum);

    DUMP_PRINT((" Array Size for Registers to dump: %d\n\n", regCount));
    DUMP_PRINT((" Reg addr  : reg data\n--------------------\n"));

    rc = printPpDumpRegisters(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, regArray, regCount);

    return rc;
}

/*******************************************************************************
* cpssPpDumpRegisters
*
* DESCRIPTION:
*       This routine print dump of PP registers.
*
* INPUTS:
*       devNum - PP's device number .
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success  (all Registers dumped successfully)
*       GT_FAIL         - on failure. (at least one register read error)
*       GT_BAD_PARAM    - invalid device number
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssPpDumpRegisters
(
    IN GT_U8                    devNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssPpDumpRegisters);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum));

    rc = internal_cpssPpDumpRegisters(devNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
    /* debug limit: */
    /*if (regCount > 25) regCount = 25;*/

/*******************************************************************************
* cpssPpDumpPortGroupRegisters
*
* DESCRIPTION:
*       This routine print dump of PP registers.
*
* INPUTS:
*       devNum - PP's device number .
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success  (all Registers dumped successfully)
*       GT_FAIL         - on failure. (at least one register read error)
*       GT_BAD_PARAM    - invalid device number
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssPpDumpPortGroupRegisters
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId
)
{
    GT_U32      regCount;   /* count of registers to dump           */
    GT_U32      *regArray, size;  /* array of pointers to reg addresses and its size   */
    GT_STATUS   rc = GT_OK; /* func return code             */

    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        DUMP_PRINT(("bad device number [%d] \n",devNum));
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(GT_OK != getRegInfo(devNum, &regArray, &size))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    regCount = size / sizeof(GT_U32);


    DUMP_PRINT(("\nstart Dump Registers for device number [%d]: \n",devNum));

    /* print information about Packet processor */
    prvCpssDrvPpDump(devNum);

    DUMP_PRINT((" Registers to dump: %d\n\n", regCount));
    DUMP_PRINT((" Reg addr  : reg data\n--------------------\n"));

    rc = printPpDumpRegisters(devNum, portGroupId, regArray, regCount);

    return rc;
}

static const char *tableNamesArray[] = {
    "PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E",
    "PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E",
    "PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E",
    "PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E",
    "PRV_CPSS_DXCH_TABLE_CPU_CODE_E",
    "PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E",
    "PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E",
    "PRV_CPSS_DXCH_TABLE_REMARKING_E",
    "PRV_CPSS_DXCH_TABLE_STG_E",
    "PRV_CPSS_DXCH_TABLE_VLAN_E",
    "PRV_CPSS_DXCH_TABLE_MULTICAST_E",
    "PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E",
    "PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E",
    "PRV_CPSS_DXCH_TABLE_FDB_E",
    "PRV_CPSS_DXCH_TABLE_POLICER_E",
    "PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E",
    "PRV_CPSS_DXCH2_TABLE_EGRESS_PCL_CONFIG_E",
    "PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E",
    "PRV_CPSS_DXCH2_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E",
    "PRV_CPSS_DXCH2_TABLE_ROUTE_ACCESS_MATRIX_E",
    "PRV_CPSS_DXCH2_LTT_TT_ACTION_E",
    "PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E",
    "PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_E",
    "PRV_CPSS_DXCH3_TABLE_MAC2ME_E",
    "PRV_CPSS_DXCH3_TABLE_INGRESS_VLAN_TRANSLATION_E",
    "PRV_CPSS_DXCH3_TABLE_EGRESS_VLAN_TRANSLATION_E",
    "PRV_CPSS_DXCH3_TABLE_VRF_ID_E",
    "PRV_CPSS_DXCH3_LTT_TT_ACTION_E",
    "PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_LOOKUP1_CONFIG_E",
    "PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_UDB_CONFIG_E",
    "PRV_CPSS_DXCH_XCAT_TABLE_LOGICAL_TARGET_MAPPING_E",
    "PRV_CPSS_DXCH_XCAT_TABLE_BCN_PROFILES_E",
    "PRV_CPSS_DXCH_XCAT_TABLE_EGRESS_POLICER_REMARKING_E"
};

/*******************************************************************************
* printPpDumpTables
*
* DESCRIPTION:
*       This routine print dump of PP registers.
*
* INPUTS:
*       devNum   - PP's device number .
*       portGroupId
*       regArray - array of reg addresses
*       regCount - count of registers to dump.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success  (all Registers dumped successfully)
*       GT_FAIL         - on failure. (at least one register read error)
*       GT_BAD_PARAM    - invalid device number
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS printPpDumpTables
(
    IN GT_U8     devNum,
    IN GT_U32    portGroupId,
    IN GT_U32   *regArray,
    IN GT_U32    tblsCount
)
{
#if (defined CHX_FAMILY)
    GT_U32      regData[1024];    /* current register data        */
    GT_STATUS   rc = GT_OK; /* func return code             */
    GT_U32      i, j, k;
    PRV_CPSS_DXCH_TABLES_INFO_STC tblInfo;

    for (i = 0; i < tblsCount; i++)
    {
        tblInfo = ((PRV_CPSS_DXCH_TABLES_INFO_STC*)regArray)[i];

        if(i < (sizeof(tableNamesArray)/sizeof(char * )))
            DUMP_PRINT(("\n--------- %s -------------\n", tableNamesArray[i]));

        if(tblInfo.readAccessType == PRV_CPSS_DXCH_DIRECT_ACCESS_E)
            DUMP_PRINT(("--------- baseAddr=0x%8.8x -------------\n",
                        ((PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC*)tblInfo.readTablePtr)->baseAddress));

        for(j = 0; j < tblInfo.maxNumOfEntries; j++)
        {
            DUMP_PRINT(("%d : ", j));

            if((rc = prvCpssDxChPortGroupReadTableEntry(devNum, portGroupId, i, j, regData)) != GT_OK)
            {
                DUMP_PRINT(("ERROR:0x%x\n", rc));
                break;
            }

            for(k = 0; k < tblInfo.entrySize; k++)
            {
                DUMP_PRINT(("0x%8.8x ", regData[k]));
            }
            DUMP_PRINT(("\n"));
        }
    }

    DUMP_PRINT(("\n"));
    return rc;
#else

    /* avoid warnings */
    devNum = devNum;
    portGroupId = portGroupId;
    regArray = regArray;
    tblsCount = tblsCount;
    DUMP_PRINT(("%s\n",tableNamesArray[0]));

    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

#endif
}

/*******************************************************************************
* cpssPpDumpPortGroupTables
*
* DESCRIPTION:
*       This routine print dump of PP tables.
*
* INPUTS:
*       devNum - PP's device number .
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success  (all Registers dumped successfully)
*       GT_FAIL         - on failure. (at least one register read error)
*       GT_BAD_PARAM    - invalid device number
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssPpDumpPortGroupTables
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId
)
{
    GT_U32      *regArray, size;  /* array of pointers to reg addresses and its size   */
    GT_STATUS   rc = GT_OK; /* func return code             */

    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        DUMP_PRINT(("bad device number [%d] \n",devNum));
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(GT_OK != getTblInfo(devNum, &regArray, &size))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    DUMP_PRINT(("\nstart Dump Tables for device number [%d]: \n",devNum));

    /* print information about Packet processor */
    prvCpssDrvPpDump(devNum);

    DUMP_PRINT((" Number of tables to dump : %d\n\n", size));
    DUMP_PRINT((" Tbl addr  : tbl data\n--------------------\n"));

    rc = printPpDumpTables(devNum,
                            (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum) == 0) ? CPSS_PORT_GROUP_UNAWARE_MODE_CNS : portGroupId,
                            regArray, size);
    return rc;
}

/*******************************************************************************
* prvPrintDumpLine
*
* DESCRIPTION:
*       local routine which prints one dump line (16 bytes) by given data.
*
* INPUTS:
*       lineStartAddr  - memory addr of first dumped element
*                        (printed at the beginning of line)
*       dumpType       - dump line format (16 bytes / 8 shorts / or 4 words)
*       dataPtr        - pointer to 16 bytes of data to be dumped as one text line
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - invalid dumpType
*       GT_BAD_PTR      - dataPtr is NULL
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvPrintDumpLine
(
    IN GT_U32                       lineStartAddr,
    IN CPSS_MEMORY_DUMP_TYPE_ENT    dumpType,
    IN PRV_DUMP_LINE_UNT            *dataPtr
)
{
    GT_U32      i;
    char        wordStr[50];       /* one word text representation (20 chars enough) */
    char        lineStr[15 + 20*PRV_WORDS_IN_DUMP_LINE_CNS] = "";

    if (NULL == dataPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    /* iterate words in data    */
    for (i = 0; i < PRV_WORDS_IN_DUMP_LINE_CNS; i++)
    {
        /* attention: if changing something -           */
        /*    be sure that length of lineStr is enough! */
        switch(dumpType)
        {
        case CPSS_MEMORY_DUMP_WORD_E:
             cpssOsSprintf(wordStr, "  %8.8x", dataPtr->words[i]);
             break;
        case CPSS_MEMORY_DUMP_SHORT_E:
             cpssOsSprintf(wordStr, "  %4.4x %4.4x", dataPtr->shorts[2*i + 1],
                                                 dataPtr->shorts[2*i]);
             break;
        case CPSS_MEMORY_DUMP_BYTE_E:
             cpssOsSprintf(wordStr, "  %2.2x %2.2x %2.2x %2.2x",
                                                 dataPtr->bytes[4*i+3],
                                                 dataPtr->bytes[4*i+2],
                                                 dataPtr->bytes[4*i+1],
                                                 dataPtr->bytes[4*i]);
             break;
        default:
             CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        cpssOsStrCat(lineStr, wordStr);  /* append text form of current word to lineStr   */

    }   /* iterate words in data    */

    DUMP_PRINT(("0x%8.8x :%s\n", lineStartAddr, lineStr));
    return GT_OK;
}

/*******************************************************************************
* cpssPpPortGroupDumpMemory
*
* DESCRIPTION:
*       This routine print dump of PP memory.
*
* INPUTS:
*       devNum      - PP's device number .
*       portGroupId - The port group Id.
*                     relevant only to 'multi-port-groups' devices.
*                     supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       startAddr   - dump start address (will be aligned to 16 bytes)
*       dumpType    - dump line format (16 bytes / 8 shorts / or 4 words)
*       dumpLength  - dump length in bytes
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success  (memory dumped successfully)
*       GT_FAIL         - on failure. (memory read error)
*       GT_BAD_PARAM    - invalid device number
*
* COMMENTS:
*       4 lower bits of startAddr will be zeroed (ignored) for alignment
*
*******************************************************************************/
GT_STATUS cpssPpPortGroupDumpMemory
(
    IN GT_U8                        devNum,
    IN GT_U32                       portGroupId,
    IN GT_U32                       startAddr,
    IN CPSS_MEMORY_DUMP_TYPE_ENT    dumpType,
    IN GT_U32                       dumpLength
)
{
    PRV_DUMP_LINE_UNT   dumpLineData;   /* data for current dump line (16 bytes)  */
    GT_U32      lineAddr;               /* current dump line - addr of first byte */
    GT_U32      addrAlignMask;          /* 16 bytes alignment mask   */
    GT_U32      startAddrAligned;       /* start addr aligned to 16 bytes   */
    GT_U32      dumpEnd;                /* first byte after requested dump range    */
    GT_U32      regAddr;                /* word address to be read from Pp memory   */
    GT_U32      regData;                /* data read from Pp memory */
    GT_U32      i;
    GT_STATUS   st;


    if (0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        DUMP_PRINT(("bad device number [%d] \n",devNum));
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch(dumpType)
    {
        case CPSS_MEMORY_DUMP_WORD_E:
        case CPSS_MEMORY_DUMP_SHORT_E:
        case CPSS_MEMORY_DUMP_BYTE_E:
             break;
        default:
             CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    DUMP_PRINT(("Start memory dump for device number [%d]: \n",devNum));

    /* print information about Packet processor */
    prvCpssDrvPpDump(devNum);
    DUMP_PRINT(("\n"));

    addrAlignMask = ~((GT_U32)PRV_16_BYTE_ALIGNMENT_CNS);   /* all bits except 4 last are "1" */
    startAddrAligned = startAddr & addrAlignMask;
    dumpEnd = startAddr + dumpLength;   /* first byte after requested dump range */

    for (lineAddr = startAddrAligned; lineAddr < dumpEnd; lineAddr += (4 * PRV_WORDS_IN_DUMP_LINE_CNS) )
    {
        /* Read words from Pp for current dump line (4 words or 16 bytes)  */
        for (i = 0; i < PRV_WORDS_IN_DUMP_LINE_CNS; i++)
        {
            /* read word from HW */
            regAddr = lineAddr + 4*i;
            st = prvCpssDrvHwPpPortGroupReadRegister(
                devNum, portGroupId, regAddr, &regData);
            if (GT_OK == st)
            {
                dumpLineData.words[i] = regData;
            }
            else
            {
                DUMP_PRINT(("Pp memory read error at address 0x%8.8x\n", regAddr));
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }
        }
        /* print current dump line  */
        prvPrintDumpLine(lineAddr, dumpType, &dumpLineData);
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssPpDumpMemory
*
* DESCRIPTION:
*       This routine print dump of PP memory.
*
* INPUTS:
*       devNum      - PP's device number .
*       startAddr   - dump start address (will be aligned to 16 bytes)
*       dumpType    - dump line format (16 bytes / 8 shorts / or 4 words)
*       dumpLength  - dump length in bytes
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success  (memory dumped successfully)
*       GT_FAIL         - on failure. (memory read error)
*       GT_BAD_PARAM    - invalid device number
*
* COMMENTS:
*       4 lower bits of startAddr will be zeroed (ignored) for alignment
*
*******************************************************************************/
static GT_STATUS internal_cpssPpDumpMemory
(
    IN GT_U8                        devNum,
    IN GT_U32                       startAddr,
    IN CPSS_MEMORY_DUMP_TYPE_ENT    dumpType,
    IN GT_U32                       dumpLength
)
{
    return cpssPpPortGroupDumpMemory(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        startAddr, dumpType, dumpLength);
}

/*******************************************************************************
* cpssPpDumpMemory
*
* DESCRIPTION:
*       This routine print dump of PP memory.
*
* INPUTS:
*       devNum      - PP's device number .
*       startAddr   - dump start address (will be aligned to 16 bytes)
*       dumpType    - dump line format (16 bytes / 8 shorts / or 4 words)
*       dumpLength  - dump length in bytes
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success  (memory dumped successfully)
*       GT_FAIL         - on failure. (memory read error)
*       GT_BAD_PARAM    - invalid device number
*
* COMMENTS:
*       4 lower bits of startAddr will be zeroed (ignored) for alignment
*
*******************************************************************************/
GT_STATUS cpssPpDumpMemory
(
    IN GT_U8                        devNum,
    IN GT_U32                       startAddr,
    IN CPSS_MEMORY_DUMP_TYPE_ENT    dumpType,
    IN GT_U32                       dumpLength
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssPpDumpMemory);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, startAddr, dumpType, dumpLength));

    rc = internal_cpssPpDumpMemory(devNum, startAddr, dumpType, dumpLength);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, startAddr, dumpType, dumpLength));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* cpssPpDumpCpuMemory
*
* DESCRIPTION:
*       This routine print dump of CPU memory.
*
* INPUTS:
*       startAddr    - dump start address (will be aligned to 16 bytes)
*       dumpLength   - dump length in bytes
*       dumpType     - dump line format (16 bytes / 8 shorts / or 4 words)
*       wordsPerLine - words Per Line
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success  (memory dumped successfully)
*       GT_FAIL         - on failure. (memory read error)
*       GT_BAD_PARAM    - invalid device number
*
* COMMENTS:
*       4 lower bits of startAddr will be zeroed (ignored) for alignment
*
*******************************************************************************/
GT_STATUS cpssPpDumpCpuMemory
(
    IN GT_U8*                       startAddr,
    IN GT_U32                       dumpLength,
    IN CPSS_MEMORY_DUMP_TYPE_ENT    dumpType,
    IN GT_U32                       wordsPerLine
)
{
    GT_U8*      lineAddr;               /* current dump line - addr of first byte */
    GT_U8*      wordAddr;               /* current dump word - addr of first byte */
    GT_U8*      startAddrAligned;       /* start addr aligned to 16 bytes   */
    GT_U8*      dumpEnd;                /* first byte after requested dump range    */
    GT_U32      i;
    GT_U32      words;
    GT_U16      shorts[2];
    GT_U8       bytes[4];


    DUMP_PRINT(("Start CPU memory dump: \n"));

    startAddrAligned = (GT_U8*)(startAddr - (GT_U32)((startAddr - (GT_U8*)0) % 4));
    dumpEnd = startAddr + dumpLength;

    for (lineAddr = startAddrAligned; lineAddr < dumpEnd; lineAddr += (4 * wordsPerLine))
    {
        DUMP_PRINT(("0x%8.8x :", lineAddr));
        for (i = 0; (i < wordsPerLine); i++)
        {
            wordAddr = lineAddr + (i * 4);
            switch(dumpType)
            {
                case CPSS_MEMORY_DUMP_WORD_E:
                    words = *(GT_U32*)wordAddr;
                    DUMP_PRINT(("  %8.8x", words));
                    break;
                case CPSS_MEMORY_DUMP_SHORT_E:
                    shorts[0] = *(GT_U16*)wordAddr;
                    shorts[1] = *(GT_U16*)(wordAddr + 2);
                    DUMP_PRINT((
                        "  %4.4x %4.4x", shorts[0], shorts[1]));
                    break;
                case CPSS_MEMORY_DUMP_BYTE_E:
                    bytes[0] = *(GT_U8*)wordAddr;
                    bytes[1] = *(GT_U8*)(wordAddr + 1);
                    bytes[2] = *(GT_U8*)(wordAddr + 2);
                    bytes[3] = *(GT_U8*)(wordAddr + 3);
                    DUMP_PRINT((
                        "  %2.2x %2.2x %2.2x %2.2x",
                        bytes[0], bytes[1], bytes[2], bytes[3]));
                    break;
                default:
                     CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }
        DUMP_PRINT(("\n"));
    }

    return GT_OK;
}

/*******************************************************************************
* cpssPpPortGroupMemoryFieldsDump
*
* DESCRIPTION:
*       This routine print dump of PP memory fields.
*
* INPUTS:
*       devNum       - PP's device number .
*       portGroupId       - port group Id relevant for multi port group devices only
*       startAddr    - dump start address
*       bitStart     - start bit for the first bit group
*       bitGroupNum  - amount of bit groups
*       bitGroupStep - step to the next bit group in bits
*       fieldsNum    - amount of printed fields in each bit group
*       fieldOff0    - offset of the first field from the start
*                      of each bit group.
*       fieldLen0    - length of field in bits must follow offset
*                      for each field.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success  (memory dumped successfully)
*       GT_FAIL         - on failure. (memory read error)
*       GT_BAD_PARAM    - invalid device number
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssPpPortGroupMemoryFieldsDump
(
    IN GT_U8                        devNum,
    IN GT_U32                       portGroupId,
    IN GT_U32                       startAddr,
    IN GT_U32                       bitStart,
    IN GT_U32                       bitGroupNum,
    IN GT_U32                       bitGroupStep,
    IN GT_U32                       fieldsNum,
    IN GT_U32                       fieldOff0,
    IN GT_U32                       fieldLen0,
    IN GT_U32                       fieldOff1,
    IN GT_U32                       fieldLen1,
    IN GT_U32                       fieldOff2,
    IN GT_U32                       fieldLen2,
    IN GT_U32                       fieldOff3,
    IN GT_U32                       fieldLen3,
    IN GT_U32                       fieldOff4,
    IN GT_U32                       fieldLen4,
    IN GT_U32                       fieldOff5,
    IN GT_U32                       fieldLen5,
    IN GT_U32                       fieldOff6,
    IN GT_U32                       fieldLen6,
    IN GT_U32                       fieldOff7,
    IN GT_U32                       fieldLen7
)
{
    GT_STATUS   rc;        /* return code                        */
    GT_U32 bitGroupIndex;  /* bit Group Index                    */
    GT_U32 bitGroupdAddr;  /* bit Group Address                  */
    GT_U32 bitGroupdStart; /*  bit Group Start bit               */
    GT_U32 fieldAddr;      /* field Address                      */
    GT_U32 fieldIndex;     /* field Index                        */
    GT_U32 fieldStart;     /* field Start bit                    */
    GT_U16 fldOff[8];      /* field bit offsets                  */
    GT_U8  fldLen[8];      /* field bit lenghts                  */
    GT_U32 fldLen0;        /* lendth of the field in first word  */
    GT_U32 fldLen1;        /* lendth of the field in second word */
    GT_U32 fieldVal0;      /* value  of the field in first word  */
    GT_U32 fieldVal1;      /* value  of the field in second word */

    DUMP_PRINT(("Dev: %2.2X portGroupId %X addr %8.8X bitStart %4.4X\n",
        devNum, portGroupId, startAddr, bitStart));
    DUMP_PRINT(("bitGroupNum %2.2X bitGroupStep %4.4X fieldsNum %2.2X\n",
        bitGroupNum, bitGroupStep, fieldsNum));

    if (fieldsNum > 8)
    {
        DUMP_PRINT((
            "Pp memory fiels amount 0x%8.8x > 8\n",
            fieldsNum));
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    /* copy field offsets and lengths to local arrays */
    fldOff[0] = (GT_U16)fieldOff0;
    fldLen[0] = (GT_U8) fieldLen0;
    fldOff[1] = (GT_U16)fieldOff1;
    fldLen[1] = (GT_U8) fieldLen1;
    fldOff[2] = (GT_U16)fieldOff2;
    fldLen[2] = (GT_U8) fieldLen2;
    fldOff[3] = (GT_U16)fieldOff3;
    fldLen[3] = (GT_U8) fieldLen3;
    fldOff[4] = (GT_U16)fieldOff4;
    fldLen[4] = (GT_U8) fieldLen4;
    fldOff[5] = (GT_U16)fieldOff5;
    fldLen[5] = (GT_U8) fieldLen5;
    fldOff[6] = (GT_U16)fieldOff6;
    fldLen[6] = (GT_U8) fieldLen6;
    fldOff[7] = (GT_U16)fieldOff7;
    fldLen[7] = (GT_U8) fieldLen7;

    for (fieldIndex = 0; (fieldIndex < fieldsNum); fieldIndex++)
    {
        DUMP_PRINT(("%2.2X: offset %4.4X, length %2.2X\n",
            fieldIndex, fldOff[fieldIndex], fldLen[fieldIndex]));
        if (fldLen[fieldIndex] > 32)
        {
            DUMP_PRINT((
                "Pp memory field length 0x%8.8x > 32 (index 0x%2.2x)\n",
                fldLen[fieldIndex], fieldIndex));
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
    }

    for (bitGroupIndex = 0;
          (bitGroupIndex < bitGroupNum);
          bitGroupIndex++)
    {
        DUMP_PRINT(("%2.2X: ", bitGroupIndex));

        bitGroupdStart =
            ((bitStart + (bitGroupStep * bitGroupIndex)) % 32);
        bitGroupdAddr = startAddr +
            (((bitStart + (bitGroupStep * bitGroupIndex)) / 32) * 4);

        for (fieldIndex = 0; (fieldIndex < fieldsNum); fieldIndex++)
        {
            fieldStart =
                ((bitGroupdStart + fldOff[fieldIndex]) % 32);
            fieldAddr = bitGroupdAddr +
                (((bitGroupdStart + fldOff[fieldIndex]) / 32 ) * 4);

            /* default - all field in one word */
            fldLen0 = fldLen[fieldIndex];
            fldLen1 = 0;
            if (fldLen0 > (32 - fieldStart))
            {
                /* field begins in one word and finished in the next */
                fldLen0 = (32 - fieldStart);
                fldLen1 = fldLen[fieldIndex] - fldLen0;
            }

            rc = prvCpssDrvHwPpPortGroupReadRegister(
                devNum, portGroupId, fieldAddr, &fieldVal0);
            if (GT_OK != rc)
            {
                DUMP_PRINT((
                    "Pp memory read error at address 0x%8.8x, port group 0x%8.8x\n",
                    fieldAddr, portGroupId));
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }

            fieldVal0 >>= fieldStart;

            if (fldLen0 < 32)
            {
                fieldVal0 &= ((1 << fldLen0) - 1);
            }

            if (fldLen1 != 0)
            {
                rc = prvCpssDrvHwPpPortGroupReadRegister(
                    devNum, portGroupId, (fieldAddr + 4), &fieldVal1);
                if (GT_OK != rc)
                {
                    DUMP_PRINT((
                        "Pp memory read error at address 0x%8.8x, port group 0x%8.8x\n",
                        fieldAddr, portGroupId));
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                }

                fieldVal1 &= ((1 << fldLen1) - 1);

                fieldVal0 |= (fieldVal1 << fldLen0);
            }

            DUMP_PRINT(("%X ", fieldVal0));

        }

        DUMP_PRINT(("\n"));
    }

    return GT_OK;
}

/*******************************************************************************
* cpssPpFillMemory
*
* DESCRIPTION:
*       This routine fills PP memory or register space.
*
* INPUTS:
*       devNum           - PP's device number .
*       memoryOrReg      - GT_TRUE - memory, GT_FALSE - registers
*       wordsAmount      - amount of words to write
*       startAddr        - write start address
*       startValue       - write start value
*       addressIncrement - address Increment
*       valueIncrement   - value Increment
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success  (memory dumped successfully)
*       GT_FAIL         - on failure. (memory read error)
*       GT_BAD_PARAM    - invalid device number
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssPpFillMemoryOrRegs
(
    IN GT_U8                        devNum,
    IN GT_BOOL                      memoryOrReg,
    IN GT_U32                       wordsAmount,
    IN GT_U32                       startAddr,
    IN GT_U32                       startValue,
    IN GT_U32                       addressIncrement,
    IN GT_U32                       valueIncrement
)
{
    GT_U32    i;     /* loop index    */
    GT_U32    addr;  /* write address */
    GT_U32    value; /* write value   */
    GT_STATUS rc;    /* return code   */

    for (i = 0; (i < wordsAmount); i++)
    {
        addr  = startAddr + (addressIncrement * i);
        value = startValue + (valueIncrement * i);
        if (memoryOrReg == GT_FALSE)
        {
            rc = cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addr, value);
        }
        else
        {
            rc = cpssDrvPpHwRamWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addr, 1/*length*/, &value);
        }
        if (rc != GT_OK)
        {
            DUMP_PRINT(("Pp memory/reg write error at address 0x%8.8x\n", addr));
            return rc;
        }
    }

    return GT_OK;
}
