/*******************************************************************************
*              Copyright 2002, GALILEO TECHNOLOGY, LTD.
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
* gtAppDemoFaConfig.c
*
* DESCRIPTION:
*       Application demo for the Fabric Adapter initialization and
*       configuration.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.2.1 $
*
*******************************************************************************/

#include <appDemo/sysHwConfig/gtAppDemoFaConfig.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>


/*******************************************************************************
* appDemoSetFaPhase1Params
*
* DESCRIPTION:
*       Initialize core level Fa phase1 initialization function.
*
* INPUTS:
*       faPhase1Params  - Fa phase1 initialization params.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoSetFaPhase1Params
(
    IN GT_FA_PHASE1_INIT_PARAMS     *faPhase1Params
)
{
    GT_FA_DEVICE            faDeviceType;
    GT_STATUS               retVal;
    CPSS_REG_VALUE_INFO_STC *regCfgList;
    GT_U32                  regCfgListSize;

    retVal = coreFaHwPhase1Init(faPhase1Params,&faDeviceType);
    if(retVal != GT_OK)
    {
        return retVal;
    }


    /* Set Fa Start-Init configuration parameters.  */
    /* the device type here doesn't matter as long as it's not an XG board */
    retVal = appDemoGetFaRegCfgList(faDeviceType, CPSS_98EX128B_CNS,
                                    &regCfgList, &regCfgListSize);
    if(retVal != GT_OK)
    {
        return retVal;
    }

    retVal = coreFaHwStartInit(faPhase1Params->devNum, GT_FALSE,
                               (GT_REG_VALUE_INFO*)regCfgList,
                               regCfgListSize);
    if(retVal != GT_OK)
    {
        return retVal;
    }

    return GT_OK;
}


/*******************************************************************************
* appDemoSetFaPhase2Params
*
* DESCRIPTION:
*       Initialize core level Fa phase2 initialization function.
*
* INPUTS:
*       faPhase2Params  - Fa phase2 initialization params.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoSetFaPhase2Params
(
    IN GT_FA_PHASE2_INIT_PARAMS     *faPhase2Params
)
{
    return coreFaHwPhase2Init(faPhase2Params);
}


