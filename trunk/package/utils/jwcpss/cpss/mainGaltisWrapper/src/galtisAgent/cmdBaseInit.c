/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdBaseInit.c
*
* DESCRIPTION:
*       Init for Galtis Agent
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*
*******************************************************************************/

#include <galtisAgent/wrapUtil/cmdCpssSysConfig.h>

extern GT_STATUS cmdCpssLibInit(void);
extern GT_STATUS cmdCpssLibInitAsicSim(void);
extern GT_STATUS cmdCpssLibInitSysconfig(void);
extern GT_STATUS cmdLibInitAppDemoExMxPmExternalTcamConfig(void);
extern GT_STATUS cmdLibInitAppDemoDb(void);
extern GT_STATUS cmdLibInitCpssExMxPmFabricPktSegm(void);
extern GT_STATUS cmdLibInitCpssExMxPmFabricConfig(void);
extern GT_STATUS cmdFx950LibInit(void);
extern GT_STATUS cmdLibInitFe200 (void);
extern GT_STATUS cmdLibInitAppDemoFx950_api_interrupts(void);
extern GT_STATUS cmdLibInitAppDemoLionPort(void);
extern GT_STATUS cmdLibInitAppDemoDxChHsu(void);
extern GT_STATUS cmdLibInitAppDemoExMxPmPort(void);
extern GT_STATUS cmdLibInitAppDemoLion2Fabric(void);
extern GT_STATUS cmdLibInitAppDemoZarlink(void);
extern GT_STATUS cmdLibInitRxPktReveive(void);


/* global database initialization routine */
GT_STATUS cmdCpssInitDatabase(void)
{
    if (cmdCpssLibInitSysconfig() != GT_OK)
        return GT_FAIL;

    if (cmdCpssLibInit() != GT_OK)
        return GT_FAIL;

    if (cmdLibInitAppDemoDb() != GT_OK)
        return GT_FAIL;

#ifdef IMPL_CPSS_EXMXPM
    if (cmdLibInitAppDemoExMxPmExternalTcamConfig() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitCpssExMxPmFabricPktSegm() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitCpssExMxPmFabricConfig() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitAppDemoExMxPmPort() != GT_OK)
        return GT_FAIL;
#endif

#ifdef IMPL_CPSS_DXCHX
    if (cmdLibInitAppDemoLionPort() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitAppDemoDxChHsu() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitAppDemoLion2Fabric() != GT_OK)
        return GT_FAIL;
#if (defined _linux)
    if (cmdLibInitAppDemoZarlink() != GT_OK)
        return GT_FAIL;
#endif
#endif
    if (cmdLibInitRxPktReveive() != GT_OK)
        return GT_FAIL;

#ifdef IMPL_DUNE
#ifdef IMPL_DUNE_FE
    /* the FE board has only FE200 device. Not need wrappers for FX devices */
    if (cmdLibInitFe200() != GT_OK)
        return GT_FAIL;
#else
    if (cmdFx950LibInit() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitAppDemoFx950_api_interrupts() != GT_OK)
        return GT_FAIL;

#endif
#endif

#if defined (ASIC_SIMULATION)
    if (cmdCpssLibInitAsicSim() != GT_OK)
        return GT_FAIL;
#endif /*#if defined (ASIC_SIMULATION)*/

    return GT_OK;
}
