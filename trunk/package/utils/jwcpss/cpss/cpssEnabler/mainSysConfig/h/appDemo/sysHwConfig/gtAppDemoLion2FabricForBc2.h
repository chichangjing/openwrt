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
* gtAppDemoLion2FabricForBc2.h
*
* DESCRIPTION:
*       Application demo of using Lion2 device as Switch Fabric connecting
*       Bobcat2 devices.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 2$
*
*******************************************************************************/
#ifndef __gtAppDemoLion2FabricForBc2h
#define __gtAppDemoLion2FabricForBc2h

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cos/cpssCosTypes.h>

/*******************************************************************************
* appDemoLion2FabricInit
*
* DESCRIPTION:
*   The function initializes DB and writes PCL rule for TO_CPU packets
*
* INPUTS:
*    lion2FabricDevNum  - Lion2 CPSS device number
*    cpuLinkPortNum     - number of Lion2 port linked to BC2 devices with CPU port
*
* OUTPUTS:
*    None.
*
* RETURNS:
*    GT_OK on success, others on fail.
*
* COMMENTS:
*    NONE
*
*******************************************************************************/
GT_STATUS appDemoLion2FabricInit
(
    IN GT_U8              lion2FabricDevNum,
    IN GT_PORT_NUM        cpuLinkPortNum
);

/*******************************************************************************
* appDemoLion2FabricForwardAddBc2Device
*
* DESCRIPTION:
*   The function writes PCL rules for UC packets with commands FORWARD, FROM_CPU, TO_ANALYSER
*   that must be forwarded to the given target device.
*
* INPUTS:
*    lion2FabricDevNum  - Lion2 CPSS device number
*    hwDevId            - HW id of BC2 target device
*    linkPortNum        - number of Lion2 port linked to BC2 target device
*
* OUTPUTS:
*    None.
*
* RETURNS:
*    GT_OK on success, others on fail.
*
* COMMENTS:
*    NONE
*
*******************************************************************************/
GT_STATUS appDemoLion2FabricForwardAddBc2Device
(
    IN  GT_U8           lion2FabricDevNum,
    IN  GT_U32          hwDevId,
    IN  GT_PORT_NUM     linkPortNum
);

/*******************************************************************************
* appDemoLion2FabricForwardAddBc2Vidx
*
* DESCRIPTION:
*   The function writes PCL rules for MC packets with the given target VIDX
*   with commands FORWARD, FROM_CPU, TO_ANALYSER
*   that must be egressed from the given set (bitmap) of Lion2 ports.
*
* INPUTS:
*    lion2FabricDevNum  - Lion2 CPSS device number
*    bc2Vidx            - target VIDX (coded by BC2 device in eDSA Tag)
*    targetPortsBmpPtr  - (pointer to) bitmap of target Lion2 ports
*
* OUTPUTS:
*    None.
*
* RETURNS:
*    GT_OK on success, others on fail.
*
* COMMENTS:
*    NONE
*
*******************************************************************************/
GT_STATUS appDemoLion2FabricForwardAddBc2Vidx
(
    IN  GT_U8                 lion2FabricDevNum,
    IN  GT_U32                bc2Vidx,
    IN  CPSS_PORTS_BMP_STC    *targetPortsBmpPtr
);

/*******************************************************************************
* appDemoLion2FabricForwardAddBc2Vlan
*
* DESCRIPTION:
*   The function writes PCL rules for BC packets with the given target VID
*   with commands FORWARD, FROM_CPU
*   that must be egressed from the given set (bitmap) of Lion2 ports.
*
* INPUTS:
*    lion2FabricDevNum  - Lion2 CPSS device number
*    bc2VlanId           - target eVID (coded by BC2 device in eDSA Tag)
*    targetPortsBmpPtr  - (pointer to) bitmap of target Lion2 ports
*
* OUTPUTS:
*    None.
*
* RETURNS:
*    GT_OK on success, others on fail.
*
* COMMENTS:
*    NONE
*
*******************************************************************************/
GT_STATUS appDemoLion2FabricForwardAddBc2Vlan
(
    IN  GT_U8                 lion2FabricDevNum,
    IN  GT_U32                bc2VlanId,
    IN  CPSS_PORTS_BMP_STC    *targetPortsBmpPtr
);

/*******************************************************************************
* appDemoLion2FabricCleanUp
*
* DESCRIPTION:
*   The function restores Lion2 device from all Lion2Fabric configurations.
*
* INPUTS:
*    lion2FabricDevNum  - Lion2 CPSS device number
*
* OUTPUTS:
*    None.
*
* RETURNS:
*    GT_OK on success, others on fail.
*
* COMMENTS:
*    NONE
*
*******************************************************************************/
GT_STATUS appDemoLion2FabricCleanUp
(
    IN GT_U8              lion2FabricDevNum
);

/*******************************************************************************
* appDemoLion2FabricConfigureQos
*
* DESCRIPTION:
*   The function configure Lion2Fabric for TC/DP support inside Lion2 device.
*   The eDSA Tag should remain unchanged.
*
* INPUTS:
*    lion2FabricDevNum  - Lion2 CPSS device number
*    toCpuTc            - TC for all TO_CPU packets
*    toCpuDp            - DP for all TO_CPU packets
*    toAnalyzerTc       - TC for all TO_ANALYZER packets
*    toAnalyzerDp       - DP for all TO_ANALYZER packets
*    fromCpuDp          - DP for all FROM_CPU packets
*
* OUTPUTS:
*    None.
*
* RETURNS:
*    GT_OK on success, others on fail.
*
* COMMENTS:
*    NONE
*
*******************************************************************************/
GT_STATUS appDemoLion2FabricConfigureQos
(
    IN GT_U8              lion2FabricDevNum,
    IN GT_U32             toCpuTc,
    IN CPSS_DP_LEVEL_ENT  toCpuDp,
    IN GT_U32             toAnalyzerTc,
    IN CPSS_DP_LEVEL_ENT  toAnalyzerDp,
    IN CPSS_DP_LEVEL_ENT  fromCpuDp
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* __gtAppDemoLion2FabricForBc2h */
