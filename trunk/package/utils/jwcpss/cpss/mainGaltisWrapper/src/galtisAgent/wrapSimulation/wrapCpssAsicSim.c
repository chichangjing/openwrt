/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapAsicSim.c
*
* DESCRIPTION:
*       This file implements wrapper functions for the ASIC simulation library;
*       It also defines the command database entries for the commander engine.
*
* FILE REVISION NUMBER:
*       $Revision: 2.1.2.1 $
*
*******************************************************************************/

#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>
#include <galtisAgent/wrapSimulation/wrapCpssAsicSim.h>
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <asicSimulation/SCIB/scib.h>

extern GT_STATUS skernelPolicerConformanceLevelForce(GT_U32 dp);

#define ENTRY_WORD_PATTERN_1(index) (index % 2) ? 0xAAAAAAAA : 0x55555555
#define ENTRY_WORD_PATTERN_2(index) (index % 2) ? 0x55555555 : 0xAAAAAAAA

/* Make word from control register common fields offset */
#define SET_CTRL_FIELDS(trigger, action, line, len) \
    (trigger | action << 4 | line << 8 | len << 12)

/* Retrieve control register common fields offset from word */
#define GET_CTRL_TRIG_FLD(word)     (word & 0xf)
#define GET_CTRL_ACT_FLD(word)      ((word >> 4) & 0xf)
#define GET_CTRL_LINE_FLD(word)     ((word >> 8) & 0xf)
#define GET_CTRL_LEN_FLD(word)      ((word >> 12) & 0x3fff)

/* Table Control Register Info */
typedef struct {
    GT_U32 ctrlRegAddr;             /* Tables Control Register Address */
    GT_U32 targetTbl;               /* Destination Table Index */
    GT_U32 tblDataRegAddr;          /* Table's Data Registers Address */
    GT_U32 tblEntrySize;            /* Table's Entry Size In Words*/
    GT_U32 tblSize;                 /* Table Size In Entries */
    GT_U32 commonFields;            /* Set of common bits */
    GT_U8 tblName[80];              /* Table Name */
} TABLE_CTRL_REG_INFO;

typedef struct {
    GT_U32 memRangeBaseAddr;        /* base address for memory ranges */
    GT_U32 memRangeRegsNum;         /* registers numbers for memory ranges */
    GT_U32 memRangeStep;            /* addresses steps for memory ranges */
} REG_MEMORY_INFO;

TABLE_CTRL_REG_INFO tblCtrlRegInfoArr[] =
{
    {0x0A00000C, 0, 0x0A000000,  3, 4096, SET_CTRL_FIELDS(15, 12, 0, 12),
        "VLAN table"},
    {0x0A00000C, 1, 0x0A000000,  1, 4096, SET_CTRL_FIELDS(15, 12, 0, 12),
        "MCAST group table"},
    {0x0A00000C, 2, 0x0A000000,  2,  256, SET_CTRL_FIELDS(15, 12, 0, 12),
        "STP group table"},
    {0x0B800130, 0, 0x0B800118,  6, 1024, SET_CTRL_FIELDS(0, 1, 2, 13),
        "Policy TCAM table"},
    {0x0B800130, 1, 0x0B800124,  3, 1024, SET_CTRL_FIELDS(0, 1, 2, 13),
        "Action Table"},
    {0x0B800204, 0, 0x0B800200,  1, 1152, SET_CTRL_FIELDS(0, 1, 2, 13),
        "PCL-ID Table"},
    {0x0B800328, 0, 0x0B800320,  2,   63, SET_CTRL_FIELDS(0, 1, 2, 6),
        "Port VLAN and QoS Configuration Table"},
    {0x0B800328, 1, 0x0B800320,  8,   63, SET_CTRL_FIELDS(0, 1, 2, 6),
        "Port Protocol VID and QoS Configuration Table"},
    {0x06000064, 0, 0x06000054,  4,16384, SET_CTRL_FIELDS(0, 1, 2, 14),
        "FDB Access Data Table"},
    {0x0C000014, 0, 0x0C00000C,  2,  256, SET_CTRL_FIELDS(0, 1, 2, 12),
        "Policers Table"},
    {0x0C000024, 0, 0x0C000020,  1,   72, SET_CTRL_FIELDS(0, 1, 2, 7),
        "Policers QoS Remarking and Initial DP Table"},
    {0x0C000038, 0, 0x0C000030,  2,   16, SET_CTRL_FIELDS(0, 1, 2, 4),
        "Policers Counters Table"},
    {0x0B000028, 0, 0x0B001000,  1,  127, SET_CTRL_FIELDS(0, 1, 5, 7),
        "Trunk Table"},
    {0x0B00002C, 0, 0x0B002000,  1,   72, SET_CTRL_FIELDS(0, 1, 2, 7),
        "QoSProfile to QoS Table"},
    {0x0B000030, 0, 0x0B003000,  1,  256, SET_CTRL_FIELDS(0, 1, 2, 8),
        "CPU Code Table"},
    {0x0B000034, 0, 0x0B004000,  1,   32, SET_CTRL_FIELDS(0, 1, 2, 5),
        "Statistical Rate Limits Table"},
    {0x0B000038, 0, 0x0B005000,  3,   27, SET_CTRL_FIELDS(0, 1, 2, 5),
        "Statistical Rate Limits Table"}
};
#define MAX_TABLES_NUM (sizeof(tblCtrlRegInfoArr)/sizeof(TABLE_CTRL_REG_INFO))

REG_MEMORY_INFO regMemInfo[] =
{
    {0x0000003C,1,0x0},     {0x00000038,1,0x0},     {0x00000058,1,0x0},
    {0x00000030,1,0x0},     {0x00000034,1,0x0},     {0x01800130,1,0x0},
    {0x01800134,1,0x0},     {0x07800004,1,0x0},     {0x07800020,1,0x0},
    {0x0B800000,1,0x0},     {0x0B800004,1,0x0},     {0x0B800060,1,0x0},
    {0x0B800050,1,0x0},     {0x0B800520,1,0x0},     {0x0B800500,1,0x0},
    {0x000000C4,1,0x0},     {0x06000000,1,0x0},     {0x06000038,1,0x0},
    {0x06000040,1,0x0},     {0x02040010,1,0x0},     {0x02012800,1,0x0},
    {0x02040000,1,0x0},     {0x02040004,1,0x0},     {0x020400A8,1,0x0},
    {0x02040104,1,0x0},     {0x02040108,1,0x0},     {0x020401A0,1,0x0},
    {0x020401A4,1,0x0},     {0x020400B0,3,0x4},     {0x020400C0,1,0x0},
    {0x020400D0,1,0x0},     {0x020400CC,1,0x0},     {0x020400BC,1,0x0},
    {0x020400D4,1,0x0},     {0x020400DC,1,0x0},     {0x020400E0,1,0x0},
    {0x020400E4,1,0x0},     {0x020400E8,1,0x0},     {0x020400EC,1,0x0},
    {0x06000038,1,0x0},     {0x0600001C,1,0x0},     {0x06000018,1,0x0},
    {0x06000030,1,0x0},     {0x0C000000,1,0x0},     {0x01CC0008,1,0x0},
    {0x0B800320,2,0x4},     {0x0B800100,12,0x4},    {0x06000054,4,0x4},
    {0x0A800000,63,0x100},  {0x0A800004,63,0x100},  {0x0A800008,63,0x100},
    {0x0A80180C,3,0x100},   {0x0B800320,1,0x0},     {0x0B810000,1024,0x20},
    {0x0B810004,1024,0x20}, {0x0B810008,1024,0x20}, {0x0B810010,1024,0x20},
    {0x0B810014,1024,0x20}, {0x0B810018,1024,0x20}, {0x0B81800C,1024,0x20},
    {0x0B81801C,1024,0x0},  {0x0B818000,1024,0x20}, {0x0B818004,1024,0x20},
    {0x0B818008,1024,0x20}, {0x0B818010,1024,0x20}, {0x0B818014,1024,0x20},
    {0x0B818018,1024,0x20}, {0x0B800400,16,0x4},    {0x0B800450,16,0x4},
    {0x0B8004A0,2,0x0},     {0x02000804,16,0x1000}, {0x0A80180C,3,0x1000},
    {0x0A800010,63,0x100},  {0x02000000,63,0x1000}, {0x02000010,63,0x1000},
    {0x02000800,16,0x1000}, {0x02010800,2,0x1000},  {0x02020800,8,0x1000},
    {0x02028800,8,0x1000},  {0x0A801814,3,0x100},   {0x0A801818,3,0x100},
    {0x01800180,1,0x0},     {0x04004020,1,0x0},     {0x01C00000,30,0x4},
    {0x01C40000,30,0x4},    {0x01C80000,30,0x4},    {0x00000070,1,0x0},
    {0x00000078,1,0x0},     {0x0B800200,1,0x0},     {0x0C000020,1,0x0},
    {0x0C00000C,1,0x0},     {0x0C000030,2,0x04},    {0x0B001000,1,0x0},
    {0x0B002000,1,0x0},     {0x0B003000,1,0x0},     {0x0B004000,1,0x0},
    {0x0B005000,1,0x0},     {0x0A000000,1,0x0},     {0x000026C0,8,0x4},
    {0x02040134,1,0x0},     {0x0A801800,3,0x100}
};

#define MAX_RANGE       (sizeof(regMemInfo)/sizeof(REG_MEMORY_INFO))

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] = {
        {"scibWriteRegister",
          &wrScibWriteRegister,
          3, 0},
        {"scibReadRegister",
          &wrScibReadRegister,
          2, 0},
        {"scibWriteMemory",
          &wrScibWriteMemory,
          4, 0},
        {"scibReadMemory",
          &wrScibReadMemory,
          3, 0},
        {"indirectTblMemoryTest",
          &wrIndirectTblMemoryTest,
          2, 0},
        {"searchMemoryTest",
          &wrSearchMemoryTest,
          1, 0},

        {"skernelPolicerConformanceLevelForce",
          &wrSkernelPolicerConformanceLevelForce,
          1, 0},

        {"rtosOnSimRemoteConnectionParametersSet",
          &wrCpssAsicSimRtosOnSimRemoteConnectionParametersSet,
          3, 0},

        {"rtosOnSimIniFileEmulateFill",
          &wrCpssAsicSimRtosOnSimIniFileEmulateFill,
          3, 0},

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* scibWriteRegister (general command)
*
* DESCRIPTION:
*     Write data to register in the simulation
*
* INPUTS:
*     [0] GT_32 deviceNum -
*          PP device number to write to.
*
*     [1] GT_U32 regAddr -
*          Register address to write to.
*
*     [2] GT_U32 regData -
*          Data to be written to register.
*
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrScibWriteRegister
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32 lDevNum;
    GT_U32 lRegAddr;
    GT_U32 lValue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lDevNum = (GT_U32)inArgs[0];
    lRegAddr = (GT_U32)inArgs[1];
    lValue = (GT_U32)inArgs[2];

    /* write to simulation device memory */
    scibWriteMemory(lDevNum, lRegAddr, 1, &lValue);

    return CMD_OK;
}

/*******************************************************************************
* scibReadRegister (general command)
*
* DESCRIPTION:
*     Reads the unmasked bits of a register.
*
* INPUTS:
*     [0] GT_U32 lDevNum -
*          PP device number to read from.
*
*     [1] GT_U32 lRegAddr -
*          Register address to read from.
*
*
* OUTPUTS:
*     [0] GT_U32 lDataPtr -
*          Data read from register.
*
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrScibReadRegister
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32 lDevNum;
    GT_U32 lRegAddr;
    GT_U32 lData;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lDevNum = (GT_U32)inArgs[0];
    lRegAddr = (GT_U32)inArgs[1];

    /* read from simulation device memory */
    scibReadMemory(lDevNum, lRegAddr, 1, &lData);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, CMD_OK, "%d", lData);

    return CMD_OK;
}

/*******************************************************************************
* scibWriteMemory (general command)
*
* DESCRIPTION:
*     Write data to memory in the simulation. Up to 8 words may be written
*
* INPUTS:
*     [0] GT_32 lDevNum -
*          PP device number to write to.
*
*     [1] GT_U32 lRegAddr -
*          Memory address to write to.
*
*     [2] GT_U32 lDataSize -
*          Size of data to be written to memory.
*
*     [3] GT_U32 memDataWord0  -
           Data to be written to register words0
*     [4] GT_U32 memDataWord1  -
           Data to be written to register words1
*     [5] GT_U32 memDataWord2  -
           Data to be written to register words2
*     [6] GT_U32 memDataWord3  -
           Data to be written to register words3
*     [7] GT_U32 memDataWord4  -
           Data to be written to register words4
*     [8] GT_U32 memDataWord5  -
           Data to be written to register words5
*     [9] GT_U32 memDataWord6  -
           Data to be written to register words6
*    [10] GT_U32 memDataWord7  -
           Data to be written to register words7
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrScibWriteMemory
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32 lDevNum;
    GT_U32 lRegAddr;
    GT_U32 lDataSize;
    GT_U32 * lDataPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lDevNum = (GT_U32)inArgs[0];
    lRegAddr = (GT_U32)inArgs[1];
    lDataSize = (GT_U32)inArgs[2];
    lDataPtr = (GT_U32 *)&inArgs[3];

    /* write to simulation device memory */
    scibWriteMemory(lDevNum, lRegAddr, lDataSize, lDataPtr);

    return CMD_OK;
}


/*******************************************************************************
* wrScibReadMemory (general command)
*
* DESCRIPTION:
*     Read data from memory in the simulation. Up to 8 words may be read.
*
* INPUTS:
*     [0] GT_U32 lDevNum -
*          PP device number to read from.
*
*     [1] GT_U32 lRegAddr -
*          Register address to read from.
*
*     [2] GT_U32 lMemSize -
*          Size of data to be written to memory.
*
* OUTPUTS:
*     [3] GT_U32 memDataWord0  -
           Data to be written to register words0
*     [4] GT_U32 memDataWord1  -
           Data to be written to register words1
*     [5] GT_U32 memDataWord2  -
           Data to be written to register words2
*     [6] GT_U32 memDataWord3  -
           Data to be written to register words3
*     [7] GT_U32 memDataWord4  -
           Data to be written to register words4
*     [8] GT_U32 memDataWord5  -
           Data to be written to register words5
*     [9] GT_U32 memDataWord6  -
           Data to be written to register words6
*    [10] GT_U32 memDataWord7  -
*
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrScibReadMemory
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32 lDevNum;
    GT_U32 lRegAddr;
    GT_U32 lMemSize;
    GT_U32 lData[8];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lDevNum = (GT_U32)inArgs[0];
    lRegAddr = (GT_U32)inArgs[1];
    lMemSize = (GT_U32)inArgs[2];

    /* read from simulation device memory */
    scibReadMemory(lDevNum, lRegAddr, lMemSize, lData);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, CMD_OK, "%d%d%d%d%d%d%d%d", lData[0], lData[1],
                                                      lData[2], lData[3],
                                                      lData[4], lData[5],
                                                      lData[6], lData[7]);

    return CMD_OK;
}

/*******************************************************************************
* indirectTblMemoryTest (general command)
*
* DESCRIPTION:
*     Check proper read/write procedures for tables with indirect access
*
* INPUTS:
*     [0] GT_U32 lDevNum -
*          PP device number to read from.
*
*     [1] GT_32 lTblCtrlReg -
*          Tables access control register address
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrIndirectTblMemoryTest
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32 lDevNum;
    GT_U32 lTblCtrlReg;

    TABLE_CTRL_REG_INFO tblCtrlRegInfo; /* Control register data info */

    GT_U32 hWords[12], wordsPattern[12];/* Register's entry and pattern */
    GT_U32 i;                           /* Table entry index */
    GT_U32 j;                           /* Entry word index */
    GT_U32 ctrlRegData;                 /* Control register entry */
    GT_U32 tbl;                         /* Current table index */
    GT_U32 fieldVal;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lDevNum = inArgs[0];
    lTblCtrlReg = inArgs[1];

    for(tbl = 0; tbl < MAX_TABLES_NUM; tbl++)
    {
        if (lTblCtrlReg != tblCtrlRegInfoArr[tbl].ctrlRegAddr)
        {
            continue;
        }

        cmdOsMemCpy(&tblCtrlRegInfo,
                 &tblCtrlRegInfoArr[tbl], sizeof(TABLE_CTRL_REG_INFO));

        /* write whole table */
        for (i = 0; i < tblCtrlRegInfo.tblSize; i++)
        {
            /* change pattern for next entry */
            for (j = 0; j < tblCtrlRegInfo.tblEntrySize; j++)
            {
                wordsPattern[j] = (i % 2) ? ENTRY_WORD_PATTERN_1(j) + i :
                                            ENTRY_WORD_PATTERN_2(j) + i ;
            }

            /* Write table entry */
            scibWriteMemory(lDevNum, tblCtrlRegInfo.tblDataRegAddr,
                            tblCtrlRegInfo.tblEntrySize, wordsPattern);

            /* Read control register */
            scibReadMemory(lDevNum, lTblCtrlReg, 1, &ctrlRegData);

            /* Write entry number */
            U32_SET_FIELD_MAC(ctrlRegData,
                GET_CTRL_LINE_FLD(tblCtrlRegInfo.commonFields),
                GET_CTRL_LEN_FLD(tblCtrlRegInfo.commonFields), i);

            /* Write "Write" operation to control register */
            U32_SET_FIELD_MAC(ctrlRegData,
                GET_CTRL_ACT_FLD(tblCtrlRegInfo.commonFields), 1, 1);

            scibWriteMemory(lDevNum, lTblCtrlReg, 1, &ctrlRegData);

            /* Check that transaction was done */

            /* Read control register */
            scibReadMemory(lDevNum, lTblCtrlReg, 1, &ctrlRegData);

            fieldVal = U32_GET_FIELD_MAC(ctrlRegData,
                GET_CTRL_TRIG_FLD(tblCtrlRegInfo.commonFields), 1);

            if (fieldVal)
            {
                /* pack output arguments to galtis string */
                galtisOutput(outArgs, GT_FAIL,
                            "Fail writing table memory: %s, table %d\n",
                            tblCtrlRegInfo.tblName, tblCtrlRegInfo.targetTbl);

                return GT_FAIL;
            }
        }

        /* read whole table */
        for (i = 0; i < tblCtrlRegInfo.tblSize; i++)
        {
            /* change pattern for next entry */
            for (j = 0; j < tblCtrlRegInfo.tblEntrySize; j++)
            {
                wordsPattern[j] = (i % 2) ? ENTRY_WORD_PATTERN_1(j) + i :
                                            ENTRY_WORD_PATTERN_2(j) + i ;
            }

            /* Read control register */
            scibReadMemory(lDevNum, lTblCtrlReg, 1, &ctrlRegData);

            /* Write entry number */
            U32_SET_FIELD_MAC(ctrlRegData,
                GET_CTRL_LINE_FLD(tblCtrlRegInfo.commonFields),
                GET_CTRL_LEN_FLD(tblCtrlRegInfo.commonFields), i);

            /* Write "Read" operation to control register */
            U32_SET_FIELD_MAC(ctrlRegData,
                GET_CTRL_ACT_FLD(tblCtrlRegInfo.commonFields), 1, 0);

            scibWriteMemory(lDevNum, lTblCtrlReg, 1, &ctrlRegData);

            /* Read control register */
            scibReadMemory(lDevNum, lTblCtrlReg, 1, &ctrlRegData);

            fieldVal = U32_GET_FIELD_MAC(ctrlRegData,
                GET_CTRL_TRIG_FLD(tblCtrlRegInfo.commonFields), 1);

            if (fieldVal)
            {
                /* pack output arguments to galtis string */
                galtisOutput(outArgs, GT_FAIL,
                            "Fail reading table memory: %s, table %d\n",
                             tblCtrlRegInfo.tblName, tblCtrlRegInfo.targetTbl);

                return GT_FAIL;
            }

            /* Compare pattern with entries regs*/

            /* Read and Compare entries regs with pattern */
            scibReadMemory(lDevNum, tblCtrlRegInfo.tblDataRegAddr,
                           tblCtrlRegInfo.tblEntrySize, hWords);

            if (cmdOsMemCmp(hWords, wordsPattern, tblCtrlRegInfo.tblEntrySize) != 0)
            {
                /* pack output arguments to galtis string */
                galtisOutput(outArgs, GT_FAIL,
                            "Fail comparing entry with pattern: %s, table %d",
                             tblCtrlRegInfo.tblName, tblCtrlRegInfo.targetTbl);

                return GT_FAIL;
            }
        }
    }

    galtisOutput(outArgs, GT_OK, "wrIndirectTblMemoryTest is OK\n");

    return GT_OK;
}
#if 0
/*******************************************************************************
* tableCtrlRegInfoGet
*
* DESCRIPTION:
*     Look up in the control register's data array by register address
*     and destination table index
*
* INPUTS:
*     IN GT_U32 ctrRegAddr  - control register address
*     IN GT_U32 dstTableIdx - destination table index
*
* OUTPUTS:
*     OUT TABLE_CTRL_REG_INFO * tblCtrlRegInfoPtr   -
*                    pointer to control register's info structure
*
* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static GT_BOOL tableCtrlRegInfoGet
(
    IN GT_U32 ctrRegAddr,
    IN GT_U32 targetTbl,
    OUT TABLE_CTRL_REG_INFO * tblCtrlRegInfoPtr
)
{
    GT_U32 i;

    cmdOsMemSet(tblCtrlRegInfoPtr, 0, sizeof(TABLE_CTRL_REG_INFO));

    for(i = 0; i < MAX_TABLES_NUM; i++)
    {
        if ((ctrRegAddr != tblCtrlRegInfoArr[i].ctrlRegAddr) ||
            targetTbl != tblCtrlRegInfoArr[i].targetTbl)
        {
            continue;
        }

        cmdOsMemCpy(tblCtrlRegInfoPtr,
                 &tblCtrlRegInfoArr[i], sizeof(TABLE_CTRL_REG_INFO));

        return GT_TRUE;
    }

    return GT_FALSE;
}
#endif /*0*/
/*******************************************************************************
* searchMemoryTest
*
* DESCRIPTION:
*    The goal of the test is to check that all registers and tables
*    with direct access are searched properly. The test should check:
*       1.  All register and tables may be accessed by simulation
*       2.  No overlapping of memory for registers and tables.
*
*
* INPUTS:
*     [0] GT_U32 lDevNum -
*          PP device number to read from.
*
* OUTPUTS:

* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrSearchMemoryTest
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32 lDevNum;

    GT_U32 i,j;                         /* iterators */
    GT_U32 memAddr;                     /* address of memory */
    GT_U32 memData;                     /* data to write/read to/from memory */

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lDevNum = inArgs[0];

    /* Write all ranges */
    for (i = 0; i < MAX_RANGE; i++) /* ranges iterator */
    {
        for (j = 0; j < regMemInfo[i].memRangeRegsNum; j++) /* ranges iterator */
        {
            /* calculate memory address and data, data = address */
            memAddr = regMemInfo[i].memRangeBaseAddr +
                     (j * regMemInfo[i].memRangeStep);
            memData = memAddr;

            /* write data to address */
            scibWriteMemory(lDevNum, memAddr, 1, &memData);
        }
    }

    /* Read and compare all ranges */
    for (i = 0; i < MAX_RANGE; i++) /* ranges iterator */
    {
        for (j = 0; j < regMemInfo[i].memRangeRegsNum; j++) /* ranges iterator */
        {
            /* calculate memory address and data */
            memAddr = regMemInfo[i].memRangeBaseAddr +
                     (j * regMemInfo[i].memRangeStep);

            /* read data from address */
            scibReadMemory(lDevNum, memAddr, 1, &memData);
            if (memData != memAddr)
            {
                /* pack output arguments to galtis string */
                galtisOutput(outArgs, GT_FAIL,
                            "Fail comparing entry with pattern: 0x%x",
                             memAddr);

                return GT_FAIL;
            }
        }

    }

    galtisOutput(outArgs, GT_OK, "wrSearchMemoryTest is OK\n");

    return GT_OK;
}

/*******************************************************************************
* wrSkernelPolicerConformanceLevelForce
*
* DESCRIPTION:
*    force the conformance level for the packets entering the policer
*       (traffic cond)
*
*
* INPUTS:
*     [0] GT_DP_LEVEL    conformance level - green/yellow/red
*
*
* OUTPUTS:

* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrSkernelPolicerConformanceLevelForce
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   rc;
    GT_U32      dp;

    dp = inArgs[0];

#if (defined RTOS_ON_SIM) || (defined APPLICATION_SIDE_ONLY)
    rc = GT_NO_SUCH;
#else
    rc = skernelPolicerConformanceLevelForce(dp);
#endif

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

/*******************************************************************************
* wrCpssAsicSimRtosOnSimRemoteConnectionParametersSet
*
* DESCRIPTION:
*    set RTOS on simulation parameters , this emulate the INI file of simulation
*
*
* INPUTS:
*     inArgs[0] - simulation IP - the "server side" (Note: currently the PP side is
*               the "server side" , it will be changed ... , and then this
*               parameter will be meaningless)
*               format is "10.6.150.92"
*     inArgs[1] - tcp_port_synchronic - the TCP port of the "synchronic connection"
*     inArgs[2] - tcp_port_asynchronous - the TCP port of the "asynchronous connection"
*
* OUTPUTS:
*       none
* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssAsicSimRtosOnSimRemoteConnectionParametersSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
#ifdef RTOS_ON_SIM
    extern GT_STATUS simOsSetCnfValue(IN char *chapNamePtr,IN char *valNamePtr,IN char *valueBufPtr);
    #define CHAPTER_NAME_DISTRIBUTION    "distribution"
    #define VALUE_NAME_DISTRIBUTION_IP    "server_ip_addr"
    #define VALUE_NAME_DISTRIBUTION_TCP_PORT_SYNCHRONIC    "tcp_port_synchronic"
    #define VALUE_NAME_DISTRIBUTION_TCP_PORT_ASYNCHRONOUS  "tcp_port_asynchronous"
    GT_U8   tmpStrBuff[256];
    GT_U32  tmpValue;
    GT_STATUS   rc = GT_OK;
    GT_BYTE_ARRY                        tmpIp;

    galtisBArray(&tmpIp, (GT_U8*)inArgs[0]);

    /* build the IP as human string , as should be in "ini file" */
    cmdOsSprintf(tmpStrBuff,"%d.%d.%d.%d",
                    tmpIp.data[0],
                    tmpIp.data[1],
                    tmpIp.data[2],
                    tmpIp.data[3]);

    /* save info to the "emulated INI file of RTOS" */
    rc += simOsSetCnfValue(CHAPTER_NAME_DISTRIBUTION , VALUE_NAME_DISTRIBUTION_IP , tmpStrBuff);

    tmpValue = inArgs[1];
    /* build the TCP port as string */
    cmdOsSprintf(tmpStrBuff,"%d",tmpValue);

    /* save info to the "emulated INI file of RTOS" */
    rc += simOsSetCnfValue(CHAPTER_NAME_DISTRIBUTION , VALUE_NAME_DISTRIBUTION_TCP_PORT_SYNCHRONIC , tmpStrBuff);

    tmpValue = inArgs[2];
    /* build the TCP port as string */
    cmdOsSprintf(tmpStrBuff,"%d",tmpValue);

    /* save info to the "emulated INI file of RTOS" */
    rc += simOsSetCnfValue(CHAPTER_NAME_DISTRIBUTION , VALUE_NAME_DISTRIBUTION_TCP_PORT_ASYNCHRONOUS , tmpStrBuff);

    if(rc != GT_OK)
    {
        rc = GT_FAIL;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");
#else
    /* this function has meaning only on "RTOS on simulation" */
    galtisOutput(outArgs, GT_NOT_IMPLEMENTED, "");
#endif /*RTOS_ON_SIM*/
    return CMD_OK;
}

/*******************************************************************************
* wrCpssAsicSimRtosOnSimIniFileEmulateFill
*
* DESCRIPTION:
*    set RTOS on simulation parameters , this emulate the INI file of simulation
*
*
* INPUTS:
*     inArgs[0] - chapter name (string)
*     inArgs[1] - value name (string)
*     inArgs[2] - value (string)
*
* OUTPUTS:
*       none
* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssAsicSimRtosOnSimIniFileEmulateFill
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
#ifdef RTOS_ON_SIM
    extern GT_STATUS simOsSetCnfValue(IN char *chapNamePtr,IN char *valNamePtr,IN char *valueBufPtr);
    GT_STATUS   rc ;

    rc = simOsSetCnfValue((char*)inArgs[0],(char*)inArgs[1],(char*)inArgs[2]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");
#else
    /* this function has meaning only on "RTOS on simulation" */
    galtisOutput(outArgs, GT_NOT_IMPLEMENTED, "");
#endif /*RTOS_ON_SIM*/
    return CMD_OK;
}

/*******************************************************************************
* cmdCpssLibInitAsicSim
*
* DESCRIPTION:
*     Library database initialization function.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
GT_STATUS cmdCpssLibInitAsicSim()
{
    return cmdInitLibrary(dbCommands, numCommands);
}


