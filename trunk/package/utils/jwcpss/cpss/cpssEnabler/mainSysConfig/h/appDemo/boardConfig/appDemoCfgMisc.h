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
* appDemoCfgMisc.h
*
* DESCRIPTION:
*       Includes misc functions definitions to be used by the application demo
*       configuration functions.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*
*******************************************************************************/
#ifndef __appDemoCfgMisch
#define __appDemoCfgMisch

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <appDemo/os/appOs.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>

#ifdef IMPL_FA
#include <cpssFa/generic/cpssFaHwInit/gtCoreFaHwInit.h>
#endif

/*******************************************************************************
* appDemoRxBuffsCacheableSet
*
* DESCRIPTION:
*       This function set status of RX buffers - cacheable or not.
*       Should be called one time BEFORE cpssInitSystem
*
* INPUTS:
*       buffersCacheable - Should be GT_TRUE if RX buffers are in cacheable memory.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - always,
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoRxBuffsCacheableSet
(
    IN GT_BOOL buffersCacheable
);

/*******************************************************************************
* appDemoRawSocketModeSet
*
* DESCRIPTION:
*       Instruct the system to use raw packets in MII mode.
*
* INPUTS:
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - always,
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoRawSocketModeSet
(
    IN GT_VOID
);

/*******************************************************************************
* appDemoLinuxModeSet
*
* DESCRIPTION:
*       Set port <portNum> to Linux Mode (Linux Only)
*
* INPUTS:
*       portNum         - The port number to be defined for Linux mode
*       ip1, ip2, ip3, ip4 - The ip address to assign to the port, 4 numbers
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - always,
*
* COMMENTS:
*       None.
*
*******************************************************************************/

GT_STATUS appDemoLinuxModeSet
(
 IN GT_U32  portNum,
 IN GT_U32 ip1,
 IN GT_U32 ip2,
 IN GT_U32 ip3,
 IN GT_U32 ip4
);

/*******************************************************************************
* appDemoLinuxModeGet
*
* DESCRIPTION:
*       Get port <portNum> Linux Mode indication (Linux Only)
*
* INPUTS:
*       portNum         - The port number to be defined for Linux mode
* OUTPUTS:
*
* RETURNS:
*       GT_TRUE if Linux mode, or
*       GT_FALSE otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/

GT_BOOL appDemoLinuxModeGet
(
 IN GT_U32  portNum
);

/*******************************************************************************
* appDemoAllocateDmaMem
*
* DESCRIPTION:
*       This function allocates memory for phase2 initialization stage, the
*       allocations include: Rx Descriptors / Buffer, Tx descriptors, Address
*       update descriptors.
*
* INPUTS:
*       devType         - The Pp device type to allocate the memory for.
*       rxDescNum       - Number of Rx descriptors (and buffers) to allocate.
*       rxBufSize       - Size of each Rx Buffer to allocate.
*       rxBufAllign     - Ammount of allignment required on the Rx buffers.
*       txDescNum       - Number of Tx descriptors to allocate.
*       auDescNum       - Number of address update descriptors to allocate.
*       ppPhase2Params  - The device's Phase2 parameters.
*
* OUTPUTS:
*       ppPhase2Params  - The device's Phase2 parameters including the required
*                         allocations.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoAllocateDmaMem
(
    IN      CPSS_PP_DEVICE_TYPE         devType,
    IN      GT_U32                      rxDescNum,
    IN      GT_U32                      rxBufSize,
    IN      GT_U32                      rxBufAllign,
    IN      GT_U32                      txDescNum,
    IN      GT_U32                      auDescNum,
    INOUT   CPSS_PP_PHASE2_INIT_PARAMS    *ppPhase2Params
);

/*******************************************************************************
* appDemoGetPpRegCfgList
*
* DESCRIPTION:
*       This function returns the list of registers to be configured to a given
*       device before and after the startInit operation. (Passed to
*       corePpHwStartInit() ).
*
* INPUTS:
*       devNum      - The Pp device type.
*       isB2bSystem - GT_TRUE, it's a B2B system, GT_FALSE otherwise.
*
* OUTPUTS:
*       regCfgList      - A pointer to the register list.
*       regCfgListSize  - Number of elements in regListPtr.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoGetPpRegCfgList
(
    IN  CPSS_PP_DEVICE_TYPE         devType,
    IN  GT_BOOL                     isB2bSystem,
    OUT CPSS_REG_VALUE_INFO_STC     **regCfgList,
    OUT GT_U32                      *regCfgListSize
);

#ifdef IMPL_FA
/*******************************************************************************
* appDemoGetFaRegCfgList
*
* DESCRIPTION:
*       This function returns the list of registers to be configured to a given
*       FA device before and after the startInit operation. (Passed to
*       coreFaHwStartInit() ).
*
* INPUTS:
*       faDevType   - The Fa device type.
*       devType     - The PP device type. See comment.

* OUTPUTS:
*       regCfgList      - A pointer to the register list.
*       regCfgListSize  - Number of elements in regListPtr.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       The input of PP device type is needed as an indication to the BOARD
*       type. XG Boards frequency is different then other boards so FA has
*       a different configuration.
*
*******************************************************************************/
GT_STATUS appDemoGetFaRegCfgList
(
    IN  GT_FA_DEVICE                faDevType,
    IN  CPSS_PP_DEVICE_TYPE         devType,  /* See comment */
    OUT CPSS_REG_VALUE_INFO_STC     **regCfgList,
    OUT GT_U32                      *regCfgListSize
);
#endif

/*******************************************************************************
* appDemoSetPortPhyAddr
*
* DESCRIPTION:
*       Configure the port's default phy address, this function should be used
*       to change the default port's phy address.
*
* INPUTS:
*       devNum  - The Pp device number.
*       portNum - The port number to update the phy address for.
*       phyAddr - The new phy address, only the lower 5 bits of this param are
*                 relevant.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       This function should be called after corePpHwPhase1Init(), and before
*       gtPortSmiInit() (in Tapi level).
*
*******************************************************************************/
GT_STATUS appDemoSetPortPhyAddr
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_U8   phyAddr
);

/*******************************************************************************
* appDemoGetRegisterWriteTime
*
* DESCRIPTION: Get the time needed for register write consecutive operations 
*              as stated in the request.
*
* INPUTS:
*       devNum      - the device number to write to.
*       regAddr1    - first register address to write to.
*       regAddr2    - second register address to write to.
*       regAddr3    - third register address to write to.
*       regData     - the data to write to the register.
*       numOfWrites - the number of times to perform the register write operation.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS appDemoGetRegisterWriteTime
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr1,
    IN GT_U32 regAddr2,
    IN GT_U32 regAddr3,
    IN GT_U32 regData,
    IN GT_U32 numOfWrites
);

/*******************************************************************************
* appDemoGetRegisterReadTime
*
* DESCRIPTION: Get the time needed for register read consecutive operations 
*              as stated in the request.
*
* INPUTS:
*       devNum      - the device number to read from.
*       regAddr1    - first register address to read.
*       regAddr2    - second register address to read.
*       regAddr3    - third register address to read.
*       numOfReads  - the number of times to perform the register read operation.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS appDemoGetRegisterReadTime
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr1,
    IN GT_U32 regAddr2,
    IN GT_U32 regAddr3,
    IN GT_U32 numOfReads
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __appDemoCfgMisch */

