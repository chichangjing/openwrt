/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* wrapCpssDxChTmGlueDram.c
*
* DESCRIPTION:
*       Wrapper functions for cpssDxChTmGlueDram.c functions
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 15 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>


/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/tmGlue/cpssDxChTmGlueDram.h>
#include <cpss/dxCh/dxChxGen/config/cpssDxChCfgInit.h>


/*******************************************************************************
* cpssDxChTmGlueDramInit
*
* DESCRIPTION:
*       Traffic Manager DRAM related configurations
*       (DDR, PHY & TM BAP registers).
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum      - physical device number
*       algoType    - DDR configuration type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or algorithem type
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueDramInit
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       result;
    GT_U8                                           devNum;
    CPSS_DXCH_TM_GLUE_DRAM_CFG_STC                  dramCfg;
    CPSS_DXCH_TM_GLUE_DRAM_ALGORITHM_STC            algoParams;
    CPSS_DXCH_CFG_DEV_INFO_STC                      devInfo;
    GT_U32 ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    dramCfg.activeInterfaceNum = (GT_U32)inArgs[1];
    dramCfg.interfaceParams.speedBin = (CPSS_DRAM_SPEED_BIN_ENT)inArgs[2];
    dramCfg.interfaceParams.busWidth = (CPSS_DRAM_BUS_WIDTH_ENT)inArgs[3];
    dramCfg.interfaceParams.memorySize = (CPSS_DRAM_SIZE_ENT)inArgs[4];
    dramCfg.interfaceParams.memoryFreq = (CPSS_DRAM_FREQUENCY_ENT)inArgs[5];
    dramCfg.interfaceParams.casWL = (GT_U32)inArgs[6];
    dramCfg.interfaceParams.casL = (GT_U32)inArgs[7];
    for( ii = 0 ; ii < CPSS_DXCH_TM_GLUE_DRAM_INF_BUS_CNS ; ii++ )
    {
        dramCfg.interfaceParams.busParams[ii].csBitmask = (GT_U32)inArgs[8+ii*4];
        dramCfg.interfaceParams.busParams[ii].mirrorEn = (GT_BOOL)inArgs[9+ii*4];
        dramCfg.interfaceParams.busParams[ii].dqsSwapEn = (GT_BOOL)inArgs[10+ii*4];
        dramCfg.interfaceParams.busParams[ii].ckSwapEn = (GT_BOOL)inArgs[11+ii*4];
    }
    dramCfg.interfaceParams.interfaceTemp = (CPSS_DRAM_TEMPERATURE_ENT)inArgs[24];

    algoParams.algoType = (CPSS_DXCH_TM_GLUE_DRAM_CONFIGURATION_TYPE_ENT)inArgs[25];
    algoParams.performWriteLeveling = (GT_BOOL)inArgs[26];
    
    dramCfg.activeInterfaceMask = BIT_MASK_MAC(dramCfg.activeInterfaceNum);

    /* call cpss api function */
    result = cpssDxChCfgDevInfoGet(devNum, &devInfo); 

    if (result == GT_OK)
    {
        /* if bobk-caelum device then activeInterfaceMask is 0x0B */
        switch (devInfo.genDevInfo.devType)
        {
            case CPSS_BOBK_CAELUM_DEVICES_CASES_MAC:
                dramCfg.activeInterfaceMask = 0x0B;
                break;
            default:
                break;
        }

        /* call cpss api function */
        result = cpssDxChTmGlueDramInit(devNum, &dramCfg, &algoParams);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;

}

/*******************************************************************************
* cpssDxChTmGlueDramInitFlagsSet
*
* DESCRIPTION:
*       Set flags for Traffic Manager DRAM related configurations
*       (DDR, PHY & TM BAP registers).
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum                   - physical device number
*       externalMemoryInitFlags  - External Memory Init Flags
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or configuration parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueDramInitFlagsSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS          result;
    GT_U8              devNum;
    GT_U32             extMemIntFlags;;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum         = (GT_U8)inArgs[0];
    extMemIntFlags = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTmGlueDramInitFlagsSet(
        devNum, extMemIntFlags);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChTmGlueDramInit",
        &wrCpssDxChTmGlueDramInit,
        27, 0},
    {"cpssDxChTmGlueDramInitFlagsSet",
        &wrCpssDxChTmGlueDramInitFlagsSet,
        2, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChTmGlueDram
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
GT_STATUS cmdLibInitCpssDxChTmGlueDram
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


