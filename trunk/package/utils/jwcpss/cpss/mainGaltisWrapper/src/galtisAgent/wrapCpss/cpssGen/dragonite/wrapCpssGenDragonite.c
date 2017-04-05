/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssGenDragonite.c
*
* DESCRIPTION:
*       CPSS wrappers for generic Dragonite APIs.
*
*       DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/dragonite/cpssGenDragonite.h>

static CPSS_GEN_DRAGONITE_DATA_STC data={0};

static GT_U32  chipsCntr = 0;
static GT_U32  portsCntr = 0;
static GT_U32  budgetsCntr = 0;

static CMD_STATUS wrCpssDragoniteReadData_CONFIG_E
(
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    if(data.dataType != CPSS_GEN_DRAGONITE_DATA_TYPE_CONFIG_E)
    {
        galtisOutput(outArgs, GT_FAIL, "%d", -1);
        return CMD_OK;
    }

    if (portsCntr >= CPSS_GEN_DRAGONITE_PORTS_NUM_CNS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }
    
    if(portsCntr == 0)
    {
        inFields[0] = data.dragoniteData.config.dcDisconnectEn;
        inFields[1] = data.dragoniteData.config.externalSyncDis;
        inFields[2] = data.dragoniteData.config.capDis;
        inFields[3] = data.dragoniteData.config.disPortsOverride;
        inFields[4] = data.dragoniteData.config.rprDisable;
        inFields[5] = data.dragoniteData.config.vmainAtPolicyEn;
        inFields[6] = data.dragoniteData.config.class0EqAf;
        inFields[7] = data.dragoniteData.config.class123EqAf;
        inFields[8] = data.dragoniteData.config.classBypass2ndError;
        inFields[9] = data.dragoniteData.config.classErrorEq0;
        inFields[10] = data.dragoniteData.config.classErrorEq4;
        inFields[11] = data.dragoniteData.config.layer2En;
        inFields[12] = data.dragoniteData.config.portPrioritySetByPD;
        inFields[13] = data.dragoniteData.config.privateLableBit;
        inFields[14] = data.dragoniteData.config.lowPriDiscoForHiPriStartupEn;
        inFields[15] = data.dragoniteData.config.portMethodLimitMode;
        inFields[16] = data.dragoniteData.config.portMethodCalcMode;
        inFields[17] = data.dragoniteData.config.guardBandValue;
        inFields[18] = data.dragoniteData.config.startupHiPrOverride;
        inFields[19] = data.dragoniteData.config.IcutMaxAt;
        inFields[20] = data.dragoniteData.config.tempAlarmTh;
        inFields[21] = data.dragoniteData.config.vmainHighTh;
        inFields[22] = data.dragoniteData.config.vmainAtLowTh;
        inFields[23] = data.dragoniteData.config.vmainAfLowTh;
    }

    inFields[24] = data.dragoniteData.config.matrixPort[portsCntr].physicalPortNumber;
    inFields[25] = data.dragoniteData.config.matrixPort[portsCntr].chipNumber;

    if(portsCntr == 0)
    {
        inFields[26] = data.dragoniteData.config.commPollingTime;
        inFields[27] = data.dragoniteData.config.irqGenTime;
    }

    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                            inFields[0], inFields[1], inFields[2], inFields[3],
                            inFields[4], inFields[5], inFields[6], inFields[7],
                            inFields[8], inFields[9], inFields[10], inFields[11],
                            inFields[12], inFields[13], inFields[14], inFields[15],
                            inFields[16], inFields[17], inFields[18], inFields[19],
                            inFields[20], inFields[21], inFields[22], inFields[23],
                            inFields[24], inFields[25], inFields[26], inFields[27]);

    /* pack output arguments to galtis string */    
    galtisOutput(outArgs, GT_OK, "%f");

    portsCntr++;

    return CMD_OK;
}

static CMD_STATUS wrCpssDragoniteReadData_SYSTEM_E
(
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8 tmp[256];
    GT_U32  i;

    if(data.dataType != CPSS_GEN_DRAGONITE_DATA_TYPE_SYSTEM_E)
    {
        galtisOutput(outArgs, GT_FAIL, "%d", -1);
        return CMD_OK;
    }

    if (budgetsCntr >= CPSS_GEN_DRAGONITE_POWER_BUDGETS_NUM_CNS)
    {
        galtisOutput(outArgs, CMD_OK, "%d", -1);
        return CMD_OK;
    }

    if(budgetsCntr == 0)
    {
        inFields[0] = data.dragoniteData.system.systemMask0;
        inFields[1] = data.dragoniteData.system.activeBudget;
        inFields[2] = data.dragoniteData.system.vmain;
    }
    else
    {
        inFields[0] = 0;
        inFields[1] = 0;
        inFields[2] = 0;
    }

    inFields[3] = data.dragoniteData.system.powerBudgetArr[budgetsCntr].availablePower;
    inFields[4] = data.dragoniteData.system.powerBudgetArr[budgetsCntr].powerSourceType;

    if(budgetsCntr == 0)
    {
        inFields[5] = data.dragoniteData.system.vmainHigh;
        inFields[6] = data.dragoniteData.system.vmainLowAT;
        inFields[7] = data.dragoniteData.system.vmainLowAF;
        inFields[8] = data.dragoniteData.system.tempAlarm;
        inFields[9] = data.dragoniteData.system.overTemp;
        inFields[10] = data.dragoniteData.system.disablePortsActive;
        inFields[11] = data.dragoniteData.system.osStatus; 
    }
    else
    {
        inFields[5] = 0;
        inFields[6] = 0;
        inFields[7] = 0;
        inFields[8] = 0;
        inFields[9] = 0;
        inFields[10] = 0;
        inFields[11] = 0;
    }

    if (chipsCntr < CPSS_GEN_DRAGONITE_CHIPS_NUM_CNS)
    {
        inFields[12] = data.dragoniteData.system.chipStatusArr[chipsCntr];
        chipsCntr++;
    }
    else
    {
        inFields[12] = 0;
    }

    if(budgetsCntr == 0)
    {       
        inFields[13] = data.dragoniteData.system.sysTotalCriticalCons;
        inFields[14] = data.dragoniteData.system.sysTotalHighCons;
        inFields[15] = data.dragoniteData.system.sysTotalLowCons;
        inFields[16] = data.dragoniteData.system.sysTotalCriticalReq;
        inFields[17] = data.dragoniteData.system.sysTotalHighReq;
        inFields[18] = data.dragoniteData.system.sysTotalLowReq;
        inFields[19] = data.dragoniteData.system.sysTotalCalcPowerCons;
        inFields[20] = data.dragoniteData.system.sysTotalPowerRequest;
        inFields[21] = data.dragoniteData.system.sysTotalDeltaPower;
        inFields[22] = data.dragoniteData.system.sysTotalRealPowerCons;
        inFields[23] = data.dragoniteData.system.minorVersion;
        inFields[24] = data.dragoniteData.system.majorVersion;   

        for(i = 0; i < CPSS_GEN_DRAGONITE_BLD_TIME_STRING_LEN_CNS; i++)
        {
            if(data.dragoniteData.system.bldDateTime[i] == 0)
                tmp[i] = ' ';
            else
                tmp[i] = data.dragoniteData.system.bldDateTime[i];
        }
        tmp[i] = '\0';
   
        inFields[26] = data.dragoniteData.system.checkSumErrorCounter;
        inFields[27] = data.dragoniteData.system.lengthErrorCounter;
        inFields[28] = data.dragoniteData.system.structVersionErrorCounter;
        inFields[29] = data.dragoniteData.system.typeErrorCounter;
        inFields[30] = data.dragoniteData.system.configErrorCounter;
        inFields[31] = data.dragoniteData.system.irqErrorCounter;
    }
    else
    {
        inFields[13] = 0;
        inFields[14] = 0;
        inFields[15] = 0;
        inFields[16] = 0;
        inFields[17] = 0;
        inFields[18] = 0;
        inFields[19] = 0;
        inFields[20] = 0;
        inFields[21] = 0;
        inFields[22] = 0;
        inFields[23] = 0;
        inFields[24] = 0;
        inFields[25] = 0;
        inFields[26] = 0;
        inFields[27] = 0;
        inFields[28] = 0;
        inFields[29] = 0;
        inFields[30] = 0;
        inFields[31] = 0;
    }

    if(budgetsCntr == 0)
    {
        fieldOutput("%d%d%d%d%d%d%d%d%d%d %d%d%d%d%d%d%d%d%d%d %d%d%d%d%d%s%d%d%d%d %d%d",
                                inFields[0], inFields[1], inFields[2], inFields[3],
                                inFields[4], inFields[5], inFields[6], inFields[7],
                                inFields[8], inFields[9], inFields[10], inFields[11],
                                inFields[12], inFields[13], inFields[14], inFields[15],
                                inFields[16], inFields[17], inFields[18], inFields[19],
                                inFields[20], inFields[21], inFields[22], inFields[23],
                                inFields[24], strDuplicate((GT_CHAR *)tmp), inFields[26], inFields[27],
                                inFields[28], inFields[29], inFields[30], inFields[31]);
    }
    else
    {
        fieldOutput("%d%d%d%d%d%d%d%d%d%d %d%d%d%d%d%d%d%d%d%d %d%d%d%d%d%s%d%d%d%d %d%d",
                                inFields[0], inFields[1], inFields[2], inFields[3],
                                inFields[4], inFields[5], inFields[6], inFields[7],
                                inFields[8], inFields[9], inFields[10], inFields[11],
                                inFields[12], inFields[13], inFields[14], inFields[15],
                                inFields[16], inFields[17], inFields[18], inFields[19],
                                inFields[20], inFields[21], inFields[22], inFields[23],
                                inFields[24], " ", inFields[26], inFields[27],
                                inFields[28], inFields[29], inFields[30], inFields[31]);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    budgetsCntr++;

    return CMD_OK;
}

static CMD_STATUS wrCpssDragoniteReadData_CHIP_E
(
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    if(data.dataType != CPSS_GEN_DRAGONITE_DATA_TYPE_CHIP_E)
    {
        galtisOutput(outArgs, GT_FAIL, "%d", -1);
        return CMD_OK;
    }

    if (chipsCntr >= CPSS_GEN_DRAGONITE_CHIPS_NUM_CNS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = data.dragoniteData.chip[chipsCntr].chipInfo; 
    inFields[1] = data.dragoniteData.chip[chipsCntr].measuredTemp; 
    inFields[2] = data.dragoniteData.chip[chipsCntr].maxMeasuredTemp; 
    inFields[3] = data.dragoniteData.chip[chipsCntr].numOfPorts; 
    inFields[4] = data.dragoniteData.chip[chipsCntr].vmainHighError; 
    inFields[5] = data.dragoniteData.chip[chipsCntr].overTempError; 
    inFields[6] = data.dragoniteData.chip[chipsCntr].disablePortsActiveError;
    inFields[7] = data.dragoniteData.chip[chipsCntr].vmainLowAfError;
    inFields[8] = data.dragoniteData.chip[chipsCntr].vmainLowAtError;
    inFields[9] = data.dragoniteData.chip[chipsCntr].tempAlarm;
    inFields[10] = data.dragoniteData.chip[chipsCntr].chipDevId; 

    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d",
                            inFields[0], inFields[1], inFields[2], inFields[3],
                            inFields[4], inFields[5], inFields[6], inFields[7],
                            inFields[8], inFields[9], inFields[10]);



    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    chipsCntr++;

    return CMD_OK;
}

static CMD_STATUS wrCpssDragoniteReadData_PORT_COMMANDS_E
(
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    if(data.dataType != CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_COMMANDS_E)
    {
        galtisOutput(outArgs, GT_FAIL, "%d", -1);
        return CMD_OK;
    }

    if (portsCntr >= CPSS_GEN_DRAGONITE_PORTS_NUM_CNS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = data.dragoniteData.portCmd[portsCntr].portCtrl.pseEnable;
    inFields[1] = data.dragoniteData.portCmd[portsCntr].portCtrl.pairControl;
    inFields[2] = data.dragoniteData.portCmd[portsCntr].portCtrl.portMode;
    inFields[3] = data.dragoniteData.portCmd[portsCntr].portCtrl.portPriority;
    inFields[4] = data.dragoniteData.portCmd[portsCntr].portPpl;
    inFields[5] = data.dragoniteData.portCmd[portsCntr].portTppl;

    fieldOutput("%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                  inFields[4], inFields[5]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    portsCntr++;

    return CMD_OK;
}

static CMD_STATUS wrCpssDragoniteReadData_PORT_CNTRS_E
(
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    if(data.dataType != CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_CNTRS_E)
    {
        galtisOutput(outArgs, GT_FAIL, "%d", -1);
        return CMD_OK;
    }

    if (portsCntr >= CPSS_GEN_DRAGONITE_PORTS_NUM_CNS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = data.dragoniteData.portCntrs.invalidSignatureCounter[portsCntr];
    inFields[1] = data.dragoniteData.portCntrs.powerDeniedCounter[portsCntr];
    inFields[2] = data.dragoniteData.portCntrs.overloadCounter[portsCntr];
    inFields[3] = data.dragoniteData.portCntrs.shortCyrcuitCounter[portsCntr];
    inFields[4] = data.dragoniteData.portCntrs.underloadCounter[portsCntr];
    inFields[5] = data.dragoniteData.portCntrs.classErrorCounter[portsCntr];

    fieldOutput("%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                  inFields[4], inFields[5]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    portsCntr++;

    return CMD_OK;
}

static CMD_STATUS wrCpssDragoniteReadData_PORT_STATUS_E
(
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    if(data.dataType != CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_STATUS_E)
    {
        galtisOutput(outArgs, GT_FAIL, "%d", -1);
        return CMD_OK;
    }

    if (portsCntr >= CPSS_GEN_DRAGONITE_PORTS_NUM_CNS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = data.dragoniteData.portStat[portsCntr].portSr.status;
    inFields[1] = data.dragoniteData.portStat[portsCntr].portSr.extStatus;
    inFields[2] = data.dragoniteData.portStat[portsCntr].portSr.portClass;
    inFields[3] = data.dragoniteData.portStat[portsCntr].portSr.portAtBehavior;
    inFields[4] = data.dragoniteData.portStat[portsCntr].lastDisconnect;
    inFields[5] = data.dragoniteData.portStat[portsCntr].indications.underload;
    inFields[6] = data.dragoniteData.portStat[portsCntr].indications.overload;
    inFields[7] = data.dragoniteData.portStat[portsCntr].indications.shortCircuit;
    inFields[8] = data.dragoniteData.portStat[portsCntr].indications.invalidSignature;
    inFields[9] = data.dragoniteData.portStat[portsCntr].indications.validSignature;
    inFields[10] = data.dragoniteData.portStat[portsCntr].indications.powerDenied;
    inFields[11] = data.dragoniteData.portStat[portsCntr].indications.validCapacitor;
    inFields[12] = data.dragoniteData.portStat[portsCntr].indications.backoff;
    inFields[13] = data.dragoniteData.portStat[portsCntr].indications.classError;

    fieldOutput("%d%d%d%d%d%d%d%d%d%d %d%d%d%d",
                            inFields[0], inFields[1], inFields[2], inFields[3],
                            inFields[4], inFields[5], inFields[6], inFields[7],
                            inFields[8], inFields[9], inFields[10], inFields[11],
                            inFields[12], inFields[13]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    portsCntr++;

    return CMD_OK;
}

static CMD_STATUS wrCpssDragoniteReadData_PORT_MEASURE_E
(
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    if(data.dataType != CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_MEASURE_E)
    {
        galtisOutput(outArgs, GT_FAIL, "%d", -1);
        return CMD_OK;
    }

    if (portsCntr >= CPSS_GEN_DRAGONITE_PORTS_NUM_CNS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = data.dragoniteData.portMeasure[portsCntr].powerConsumption;
    inFields[1] = data.dragoniteData.portMeasure[portsCntr].current;
    inFields[2] = data.dragoniteData.portMeasure[portsCntr].volt;
    inFields[3] = data.dragoniteData.portMeasure[portsCntr].calcPowerConsumption;

    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");
    
    portsCntr++;

    return CMD_OK;
}

static CMD_STATUS wrCpssDragoniteReadData_LAYER2_E
(
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    if(data.dataType != CPSS_GEN_DRAGONITE_DATA_TYPE_LAYER2_E)
    {
        galtisOutput(outArgs, GT_FAIL, "%d", -1);
        return CMD_OK;
    }

    if (portsCntr >= CPSS_GEN_DRAGONITE_PORTS_NUM_CNS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = data.dragoniteData.layer2[portsCntr].layer2PdInfo.pdRequestedPower;
    inFields[1] = data.dragoniteData.layer2[portsCntr].layer2PdInfo.pdPriority;
    inFields[2] = data.dragoniteData.layer2[portsCntr].layer2PdInfo.portCableLen;
    inFields[3] = data.dragoniteData.layer2[portsCntr].layer2PseInfo.pseAllocPower;
    inFields[4] = data.dragoniteData.layer2[portsCntr].layer2PseInfo.psePriority;
    inFields[5] = data.dragoniteData.layer2[portsCntr].layer2PseInfo.pseType;

    fieldOutput("%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                  inFields[4], inFields[5]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    portsCntr++;

    return CMD_OK;
}

/*----------------------------- Dragonite write data table begin ---------------------------*/

static CMD_STATUS wrCpssDragoniteWrite_CONFIG_E
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    if (portsCntr >= CPSS_GEN_DRAGONITE_PORTS_NUM_CNS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    if(portsCntr == 0)
    {
        data.dataType = CPSS_GEN_DRAGONITE_DATA_TYPE_CONFIG_E;
        data.dragoniteData.config.dcDisconnectEn        = (GT_BOOL)inFields[0];
        data.dragoniteData.config.externalSyncDis       = (GT_BOOL)inFields[1];
        data.dragoniteData.config.capDis                = (GT_BOOL)inFields[2];
        data.dragoniteData.config.disPortsOverride      = (GT_BOOL)inFields[3];
        data.dragoniteData.config.rprDisable            = (GT_BOOL)inFields[4];
        data.dragoniteData.config.vmainAtPolicyEn       = (GT_BOOL)inFields[5];
        data.dragoniteData.config.class0EqAf            = (GT_BOOL)inFields[6];
        data.dragoniteData.config.class123EqAf          = (GT_BOOL)inFields[7];
        data.dragoniteData.config.classBypass2ndError   = (GT_BOOL)inFields[8];
        data.dragoniteData.config.classErrorEq0         = (GT_BOOL)inFields[9];
        data.dragoniteData.config.classErrorEq4         = (GT_BOOL)inFields[10];
        data.dragoniteData.config.layer2En              = (GT_BOOL)inFields[11];
        data.dragoniteData.config.portPrioritySetByPD   = (GT_BOOL)inFields[12];
        data.dragoniteData.config.privateLableBit       = (GT_BOOL)inFields[13];
        data.dragoniteData.config.lowPriDiscoForHiPriStartupEn = (GT_BOOL)inFields[14];
        data.dragoniteData.config.portMethodLimitMode   = (GT_U32)inFields[15];
        data.dragoniteData.config.portMethodCalcMode    = (GT_U32)inFields[16];
        data.dragoniteData.config.guardBandValue        = (GT_U32)inFields[17];
        data.dragoniteData.config.startupHiPrOverride   = (GT_BOOL)inFields[18];
        data.dragoniteData.config.IcutMaxAt             = (GT_U32)inFields[19];
        data.dragoniteData.config.tempAlarmTh           = (GT_U32)inFields[20];
        data.dragoniteData.config.vmainHighTh           = (GT_U32)inFields[21];
        data.dragoniteData.config.vmainAtLowTh          = (GT_U32)inFields[22];
        data.dragoniteData.config.vmainAfLowTh          = (GT_U32)inFields[23];
    }

    data.dragoniteData.config.matrixPort[portsCntr].physicalPortNumber = (GT_U8)inFields[24];
    data.dragoniteData.config.matrixPort[portsCntr].chipNumber = (GT_U8)inFields[25];

    if(portsCntr == 0)
    {
        data.dragoniteData.config.commPollingTime       = (GT_U32)inFields[26];
        data.dragoniteData.config.irqGenTime            = (GT_U32)inFields[27];
    }

    portsCntr++;

    return CMD_OK;
}

static CMD_STATUS wrCpssDragoniteWrite_SYSTEM_E
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    if (budgetsCntr >= CPSS_GEN_DRAGONITE_POWER_BUDGETS_NUM_CNS)
    {
        galtisOutput(outArgs, CMD_OK, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    /* Note: just dataType and powerBudgetArr important all other fields are read only */
    if(budgetsCntr == 0)
    {
        data.dataType                                   = CPSS_GEN_DRAGONITE_DATA_TYPE_SYSTEM_E;
        data.dragoniteData.system.systemMask0           = 0;
        data.dragoniteData.system.activeBudget          = 0;
        data.dragoniteData.system.vmain                 = 0;
    }

    data.dragoniteData.system.powerBudgetArr[budgetsCntr].availablePower = (GT_U32)inFields[3];
    data.dragoniteData.system.powerBudgetArr[budgetsCntr].powerSourceType = 
                        (CPSS_GEN_DRAGONITE_POWER_SOURCE_TYPE_ENT)inFields[4];

    if(budgetsCntr == 0)
    {
        data.dragoniteData.system.vmainHigh             = 0;
        data.dragoniteData.system.vmainLowAT            = 0;
        data.dragoniteData.system.vmainLowAF            = 0;
        data.dragoniteData.system.tempAlarm             = 0;
        data.dragoniteData.system.overTemp              = 0;
        data.dragoniteData.system.disablePortsActive    = 0;
        data.dragoniteData.system.osStatus              = 0;
    }

    if (chipsCntr < CPSS_GEN_DRAGONITE_CHIPS_NUM_CNS)
    {
        data.dragoniteData.system.chipStatusArr[chipsCntr] = 0;
        chipsCntr++;
    }

    if(budgetsCntr == 0)
    {
        data.dragoniteData.system.sysTotalCriticalCons  = 0;
        data.dragoniteData.system.sysTotalHighCons      = 0;
        data.dragoniteData.system.sysTotalLowCons       = 0;
        data.dragoniteData.system.sysTotalCriticalReq   = 0;
        data.dragoniteData.system.sysTotalHighReq       = 0;
        data.dragoniteData.system.sysTotalLowReq        = 0;
        data.dragoniteData.system.sysTotalCalcPowerCons = 0;
        data.dragoniteData.system.sysTotalPowerRequest  = 0;
        data.dragoniteData.system.sysTotalDeltaPower    = 0;
        data.dragoniteData.system.sysTotalRealPowerCons = 0;
        data.dragoniteData.system.minorVersion          = 0;
        data.dragoniteData.system.majorVersion          = 0;
        cpssOsMemSet(data.dragoniteData.system.bldDateTime, 0, 
                                    CPSS_GEN_DRAGONITE_BLD_TIME_STRING_LEN_CNS);
        data.dragoniteData.system.checkSumErrorCounter  = 0;
        data.dragoniteData.system.lengthErrorCounter    = 0;
        data.dragoniteData.system.structVersionErrorCounter = 0;
        data.dragoniteData.system.typeErrorCounter      = 0;
        data.dragoniteData.system.configErrorCounter    = 0;
        data.dragoniteData.system.irqErrorCounter       = 0;
    }

    budgetsCntr++;

    return CMD_OK;
}

static CMD_STATUS wrCpssDragoniteWrite_CHIP_E
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    if (chipsCntr >= CPSS_GEN_DRAGONITE_CHIPS_NUM_CNS)
    {
        galtisOutput(outArgs, CMD_OK, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    data.dataType = CPSS_GEN_DRAGONITE_DATA_TYPE_CHIP_E;
    data.dragoniteData.chip[chipsCntr].chipInfo         = (GT_U32)inFields[0];
    data.dragoniteData.chip[chipsCntr].measuredTemp     = (GT_U32)inFields[1];
    data.dragoniteData.chip[chipsCntr].maxMeasuredTemp  = (GT_U32)inFields[2]; 
    data.dragoniteData.chip[chipsCntr].numOfPorts       = (GT_U32)inFields[3]; 
    data.dragoniteData.chip[chipsCntr].vmainHighError   = (GT_BOOL)inFields[4]; 
    data.dragoniteData.chip[chipsCntr].overTempError    = (GT_BOOL)inFields[5]; 
    data.dragoniteData.chip[chipsCntr].disablePortsActiveError = (GT_BOOL)inFields[6];
    data.dragoniteData.chip[chipsCntr].vmainLowAfError  = (GT_BOOL)inFields[7];
    data.dragoniteData.chip[chipsCntr].vmainLowAtError  = (GT_BOOL)inFields[8];
    data.dragoniteData.chip[chipsCntr].tempAlarm        = (GT_BOOL)inFields[9];
    data.dragoniteData.chip[chipsCntr].chipDevId        = (GT_U32)inFields[10]; 

    chipsCntr++;

    return CMD_OK;
}

static CMD_STATUS wrCpssDragoniteWrite_PORT_COMMANDS_E
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    if (portsCntr >= CPSS_GEN_DRAGONITE_PORTS_NUM_CNS)
    {
        galtisOutput(outArgs, CMD_OK, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    data.dataType = CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_COMMANDS_E;
    data.dragoniteData.portCmd[portsCntr].portCtrl.pseEnable = 
                        (CPSS_GEN_DRAGONITE_PORT_CTRL_STATUS_ENT)inFields[0];
    data.dragoniteData.portCmd[portsCntr].portCtrl.pairControl =
                        (CPSS_GEN_DRAGONITE_PORT_CTRL_PAIRCTRL_ENT)inFields[1];
    data.dragoniteData.portCmd[portsCntr].portCtrl.portMode =
                        (CPSS_GEN_DRAGONITE_PORT_CTRL_MODE_ENT)inFields[2];
    data.dragoniteData.portCmd[portsCntr].portCtrl.portPriority = 
                        (CPSS_GEN_DRAGONITE_PORT_CTRL_PRIORITY_ENT)inFields[3];
    data.dragoniteData.portCmd[portsCntr].portPpl = (GT_U32)inFields[4];
    data.dragoniteData.portCmd[portsCntr].portTppl = (GT_U32)inFields[5];

    portsCntr++;

    return CMD_OK;
}

static CMD_STATUS wrCpssDragoniteWrite_LAYER2_E
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    if (portsCntr >= CPSS_GEN_DRAGONITE_PORTS_NUM_CNS)
    {
        galtisOutput(outArgs, CMD_OK, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    data.dataType = CPSS_GEN_DRAGONITE_DATA_TYPE_LAYER2_E;
    data.dragoniteData.layer2[portsCntr].layer2PdInfo.pdRequestedPower = (GT_U32)inFields[0];
    data.dragoniteData.layer2[portsCntr].layer2PdInfo.pdPriority = 
                        (CPSS_GEN_DRAGONITE_PRIORITY_ENT)inFields[1];
    data.dragoniteData.layer2[portsCntr].layer2PdInfo.portCableLen = (GT_U32)inFields[2];
    data.dragoniteData.layer2[portsCntr].layer2PseInfo.pseAllocPower = (GT_U32)inFields[3];
    data.dragoniteData.layer2[portsCntr].layer2PseInfo.psePriority = 
                        (CPSS_GEN_DRAGONITE_PRIORITY_ENT)inFields[4];
    data.dragoniteData.layer2[portsCntr].layer2PseInfo.pseType = 
                        (CPSS_GEN_DRAGONITE_POWER_SOURCE_TYPE_ENT)inFields[5];

    portsCntr++;

    return CMD_OK;
}

/* -------------------------------------------- */
static CMD_STATUS dragoniteReadData
(
    IN CPSS_GEN_DRAGONITE_DATA_TYPE_ENT type,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;   
    GT_U32 timeout;

    timeout = 100;
    do
    {
        result = cpssDragoniteReadTrigger(type);
        if(result == GT_NOT_READY)
        {
            if((timeout--) == 0)
            {
                galtisOutput(outArgs, GT_NOT_READY, "%d", -1);
                return CMD_OK;
            }
        }
        else if(result != GT_OK)
        {
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }
        else
            break;
    }while(timeout > 0);

    (void)cpssOsTimerWkAfter(200);

    timeout = 100;
    do
    {
        result = cpssDragoniteReadData(&data);
        if(result == GT_NOT_READY)
        {
            if((timeout--) == 0)
            {
                galtisOutput(outArgs, GT_NOT_READY, "%d", -1);
                return CMD_OK;
            }
        }
        else if(result != GT_OK)
        {
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }
        else
            break;
    }while(timeout > 0);

    return CMD_OK;
}

/* chip */
static CMD_STATUS wrDragonite_ChipGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    dragoniteReadData(CPSS_GEN_DRAGONITE_DATA_TYPE_CHIP_E, outArgs);

    chipsCntr = 0;

    wrCpssDragoniteReadData_CHIP_E(inFields, outArgs);

    return CMD_OK;
}

static CMD_STATUS wrDragonite_ChipGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    wrCpssDragoniteReadData_CHIP_E(inFields, outArgs);

    return CMD_OK;
}

static CMD_STATUS wrDragonite_ChipSetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    chipsCntr = 0;    

    wrCpssDragoniteWrite_CHIP_E(inArgs, inFields, numFields, outArgs);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

static CMD_STATUS wrDragonite_ChipSetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    wrCpssDragoniteWrite_CHIP_E(inArgs, inFields, numFields, outArgs);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/* config */
static CMD_STATUS wrDragonite_ConfigGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    dragoniteReadData(CPSS_GEN_DRAGONITE_DATA_TYPE_CONFIG_E, outArgs);

    portsCntr = 0;    

    wrCpssDragoniteReadData_CONFIG_E(inFields, outArgs);

    return CMD_OK;
}

static CMD_STATUS wrDragonite_ConfigGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    wrCpssDragoniteReadData_CONFIG_E(inFields, outArgs);

    return CMD_OK;
}

static CMD_STATUS wrDragonite_ConfigSetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    portsCntr = 0;    

    wrCpssDragoniteWrite_CONFIG_E(inArgs, inFields, numFields, outArgs);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

static CMD_STATUS wrDragonite_ConfigSetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    wrCpssDragoniteWrite_CONFIG_E(inArgs, inFields, numFields, outArgs);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/* port command */
static CMD_STATUS wrDragonite_Port_commandGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    dragoniteReadData(CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_COMMANDS_E, outArgs);

    portsCntr = 0;    

    wrCpssDragoniteReadData_PORT_COMMANDS_E(inFields, outArgs);

    return CMD_OK;
}

static CMD_STATUS wrDragonite_Port_commandGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    wrCpssDragoniteReadData_PORT_COMMANDS_E(inFields, outArgs);

    return CMD_OK;
}

static CMD_STATUS wrDragonite_Port_commandSetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    portsCntr = 0;    

    wrCpssDragoniteWrite_PORT_COMMANDS_E(inArgs, inFields, numFields, outArgs);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

static CMD_STATUS wrDragonite_Port_commandSetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    wrCpssDragoniteWrite_PORT_COMMANDS_E(inArgs, inFields, numFields, outArgs);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/* layer2 */
static CMD_STATUS wrDragonite_Port_Layer2GetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    dragoniteReadData(CPSS_GEN_DRAGONITE_DATA_TYPE_LAYER2_E, outArgs);

    portsCntr = 0;    

    wrCpssDragoniteReadData_LAYER2_E(inFields, outArgs);

    return CMD_OK;
}

static CMD_STATUS wrDragonite_Port_Layer2GetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    wrCpssDragoniteReadData_LAYER2_E(inFields, outArgs);

    return CMD_OK;
}

static CMD_STATUS wrDragonite_Port_Layer2SetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    portsCntr = 0;    

    wrCpssDragoniteWrite_LAYER2_E(inArgs, inFields, numFields, outArgs);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

static CMD_STATUS wrDragonite_Port_Layer2SetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    wrCpssDragoniteWrite_LAYER2_E(inArgs, inFields, numFields, outArgs);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}


/* system */
static CMD_STATUS wrDragonite_SystemGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    dragoniteReadData(CPSS_GEN_DRAGONITE_DATA_TYPE_SYSTEM_E, outArgs);

    chipsCntr = 0;
    budgetsCntr = 0;
    wrCpssDragoniteReadData_SYSTEM_E(inFields, outArgs);

    return CMD_OK;
}

static CMD_STATUS wrDragonite_SystemGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    wrCpssDragoniteReadData_SYSTEM_E(inFields, outArgs);

    return CMD_OK;
}

static CMD_STATUS wrDragonite_SystemSetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    chipsCntr = 0;
    budgetsCntr = 0;
    wrCpssDragoniteWrite_SYSTEM_E(inArgs, inFields, numFields, outArgs);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

static CMD_STATUS wrDragonite_SystemSetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    wrCpssDragoniteWrite_SYSTEM_E(inArgs, inFields, numFields, outArgs);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/* port counters */
static CMD_STATUS wrDragonite_Port_CountersGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    dragoniteReadData(CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_CNTRS_E, outArgs);

    portsCntr = 0;    

    wrCpssDragoniteReadData_PORT_CNTRS_E(inFields, outArgs);

    return CMD_OK;
}

static CMD_STATUS wrDragonite_Port_CountersGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    wrCpssDragoniteReadData_PORT_CNTRS_E(inFields, outArgs);

    return CMD_OK;
}

/* port measurements */
static CMD_STATUS wrDragonite_Port_MeasurmentsGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    dragoniteReadData(CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_MEASURE_E, outArgs);

    portsCntr = 0;    

    wrCpssDragoniteReadData_PORT_MEASURE_E(inFields, outArgs);

    return CMD_OK;
}

static CMD_STATUS wrDragonite_Port_MeasurmentsGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    wrCpssDragoniteReadData_PORT_MEASURE_E(inFields, outArgs);

    return CMD_OK;
}

/* port statuses */
static CMD_STATUS wrDragonite_Port_StatusesGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    dragoniteReadData(CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_STATUS_E, outArgs);

    portsCntr = 0;    

    wrCpssDragoniteReadData_PORT_STATUS_E(inFields, outArgs);

    return CMD_OK;
}

static CMD_STATUS wrDragonite_Port_StatusesGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    wrCpssDragoniteReadData_PORT_STATUS_E(inFields, outArgs);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDragoniteEndSet
*
* DESCRIPTION:
*           Write Dragonite table of any kind
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_FAIL       - on BSP/HW error
*
* COMMENT: 
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDragoniteEndSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;

    /* call cpss api function */
    if((rc = cpssDragoniteWrite(&data)) != GT_OK)
    {
        galtisOutput(outArgs, rc, "");
    }
    else
    {
        (void)cpssOsTimerWkAfter(50); /* give time to prepare answer */
        rc = cpssDragoniteReadData(&data);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

/*----------------------------- Dragonite write data table end ---------------------------*/



/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"Dragonite_ChipGetFirst",
        &wrDragonite_ChipGetFirst,
        0, 0},
    
    {"Dragonite_ChipGetNext",
        &wrDragonite_ChipGetNext,
        0, 0},
    
    {"Dragonite_ChipSetFirst",
        &wrDragonite_ChipSetFirst,
        0, 11},
    
    {"Dragonite_ChipSetNext",
        &wrDragonite_ChipSetNext,
        0, 11},
    
    {"Dragonite_ChipEndSet",
        &wrCpssDragoniteEndSet,
        0, 0},

    {"Dragonite_ConfigGetFirst",
        &wrDragonite_ConfigGetFirst,
        0, 0},
    
    {"Dragonite_ConfigGetNext",
        &wrDragonite_ConfigGetNext,
        0, 0},
    
    {"Dragonite_ConfigSetFirst",
        &wrDragonite_ConfigSetFirst,
        0, 26},
    
    {"Dragonite_ConfigSetNext",
        &wrDragonite_ConfigSetNext,
        0, 26},
    
    {"Dragonite_ConfigEndSet",
        &wrCpssDragoniteEndSet,
        0, 0},

    {"Dragonite_Port_commandGetFirst",
        &wrDragonite_Port_commandGetFirst,
        0, 0},
    
    {"Dragonite_Port_commandGetNext",
        &wrDragonite_Port_commandGetNext,
        0, 0},
    
    {"Dragonite_Port_commandSetFirst",
        &wrDragonite_Port_commandSetFirst,
        0, 6},
    
    {"Dragonite_Port_commandSetNext",
        &wrDragonite_Port_commandSetNext,
        0, 6},
    
    {"Dragonite_Port_commandEndSet",
        &wrCpssDragoniteEndSet,
        0, 0},

    {"Dragonite_Port_CountersGetFirst",
        &wrDragonite_Port_CountersGetFirst,
        0, 0},
    
    {"Dragonite_Port_CountersGetNext",
        &wrDragonite_Port_CountersGetNext,
        0, 0},
    

    {"Dragonite_Port_Layer2GetFirst",
        &wrDragonite_Port_Layer2GetFirst,
        0, 0},
    
    {"Dragonite_Port_Layer2GetNext",
        &wrDragonite_Port_Layer2GetNext,
        0, 0},
    
    {"Dragonite_Port_Layer2SetFirst",
        &wrDragonite_Port_Layer2SetFirst,
        0, 6},
    
    {"Dragonite_Port_Layer2SetNext",
        &wrDragonite_Port_Layer2SetNext,
        0, 6},
    
    {"Dragonite_Port_Layer2EndSet",
        &wrCpssDragoniteEndSet,
        0, 0},

    {"Dragonite_Port_MeasurmentsGetFirst",
        &wrDragonite_Port_MeasurmentsGetFirst,
        0, 0},
    
    {"Dragonite_Port_MeasurmentsGetNext",
        &wrDragonite_Port_MeasurmentsGetNext,
        0, 0},

    {"Dragonite_Port_StatusesGetFirst",
        &wrDragonite_Port_StatusesGetFirst,
        0, 0},
    
    {"Dragonite_Port_StatusesGetNext",
        &wrDragonite_Port_StatusesGetNext,
        0, 0},

    {"Dragonite_SystemGetFirst",
        &wrDragonite_SystemGetFirst,
        0, 0},
    
    {"Dragonite_SystemGetNext",
        &wrDragonite_SystemGetNext,
        0, 0},
    
    {"Dragonite_SystemSetFirst",
        &wrDragonite_SystemSetFirst,
        0, 32},
    
    {"Dragonite_SystemSetNext",
        &wrDragonite_SystemSetNext,
        0, 32},
    
    {"Dragonite_SystemEndSet",
        &wrCpssDragoniteEndSet,
        0, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssGenDragonite
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
GT_STATUS cmdLibInitCpssGenDragonite
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

