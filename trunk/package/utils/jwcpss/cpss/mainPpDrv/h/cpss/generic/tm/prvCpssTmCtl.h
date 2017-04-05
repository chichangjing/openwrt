/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssTmCtl.h
*
* DESCRIPTION: TM configuration library private utilities
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#ifndef __prvCpssTmCtlh
#define __prvCpssTmCtlh

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/tm/cpssTmPublicDefs.h>
#include <cpss/generic/cpssHwInit/cpssHwInit.h>
/* for CPSS_TM_CTL_LAD_INF_PARAM_STC definition */
#include <cpss/generic/tm/cpssTmCtl.h>
#include <cpss/generic/tm/prvCpssTmDefs.h>


/******************************************************************************
 * Typedef: struct PRV_CPSS_TM_HW_PARAMS_STC
 *
 * Description: TM HW properties and Limits.
 *
 * Fields:
 *
 *      maxQueues - max number of Queue Nodes.
 *      maxAnodes - max number of A Nodes.
 *      maxBnodes - max number of B Nodes.
 *      maxCnodes - max number of C Nodes.
 *      maxPorts  - max number of TM Ports.
 * 
 *      queuesToAnode - Default number of Queues which are connected to A node. 
 *      aNodesToBnode - Default number of A nodes which are connected to B nodes.
 *      bNodesToCnode - Default number of B nodes which are connected to C nodes.          
 *      cNodesToPort  - Default number of C nodes which are connected to TM ports.
 *      installedQueuesPerPort - Default number of Queues which are installed on
 *      each TM port.
 * 
 ******************************************************************************/
typedef struct
{
    /* TM tree structure */
    GT_U16	maxQueues;
    GT_U16	maxAnodes;
    GT_U16	maxBnodes;
    GT_U16	maxCnodes;
    GT_U16	maxPorts;

    /* default TM tree mapping */
    GT_U8   queuesToAnode;
    GT_U8   aNodesToBnode;
    GT_U8	bNodesToCnode;
    GT_U8	cNodesToPort;
    GT_U8	installedQueuesPerPort;
} PRV_CPSS_TM_HW_PARAMS_STC;


extern GT_U8 prvCpssMinDramInfArr[PRV_CPSS_TM_DEV_LAST_E+1];
extern GT_U8 prvCpssMaxDramInfArr[PRV_CPSS_TM_DEV_LAST_E+1];

/* macro to verify Lad interfaces range */
    /*PRV_CPSS_PP_MAC(devNum)->tmInfo.prvCpssTmDevId can not be used as this macro is called befor initSystem) */
#define PRV_CPSS_TM_CTL_CHECK_DRAM_INF_NUM_MAC(devNum, dramInfNum) \
    if(dramInfNum < prvCpssMinDramInfArr[prvCpssTmGetInternalDevId(devNum)]) \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG); \
    if(dramInfNum > prvCpssMaxDramInfArr[prvCpssTmGetInternalDevId(devNum)]) \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);


/*******************************************************************************
* prvCpssTmGetInternalDevId
*
* DESCRIPTION:
*       Get TM Internal Device ID.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum       - physical device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       PRV_CPSS_TM_DEVS_ENT != PRV_CPSS_TM_DEV_LAST_E - on success
*
* COMMENTS:
*
*******************************************************************************/
PRV_CPSS_TM_DEVS_ENT prvCpssTmGetInternalDevId
(
    IN  GT_U8   devNum
);


/*******************************************************************************
* prvCpssTmDumpPort
*
* DESCRIPTION:
*       print configured tree under a specific port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       portIndex         - port index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_FAIL                     - on hardware error.
*       GT_NO_RESOURCE              - on out of memory space.
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device.
*       GT_BAD_STATE                - on burst mode or init doesnt succeed
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvCpssTmDumpPort
(
    IN GT_U8    devNum,
    IN GT_U32   portIndex
);

/*******************************************************************************
* prvCpssTmCtlDumpPortHW
*
* DESCRIPTION:
*       print from HW configured tree under a specific port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       portIndex         - port index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_FAIL                     - on hardware error.
*       GT_NO_RESOURCE              - on out of memory space.
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device.
*       GT_BAD_STATE                - on burst mode or init doesnt succeed
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvCpssTmCtlDumpPortHW
(
    IN GT_U8                   devNum,
    IN GT_U32                  portIndex
);

/*******************************************************************************
* prvCpssTmCtlLadParamsGetHW
*
* DESCRIPTION:
*       Get TM LAD parameters from HW.  
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum       - physical device number.
*       numOfLadsPtr - (out pointer of)LAD number.
*       ladParamsPtr - (out pointer of) CPSS_TM_CTL_LAD_INF_PARAM_STC. 
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
GT_STATUS prvCpssTmCtlLadParamsGetHW
(
    IN  GT_U8                         devNum,
    OUT GT_U32                        *numOfLadsPtr,
    OUT CPSS_TM_CTL_LAD_INF_PARAM_STC *ladParamsPtr
);


/*******************************************************************************
* prvCpssTmCtlHwInit
*
* DESCRIPTION:
*       Initialize the TM HW configuration.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       numOfLad          - Nubmer of lads.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_FAIL                     - on hardware error.
*       GT_NO_RESOURCE              - on out of memory space.
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device.
*       GT_BAD_STATE                - on burst mode or init doesnt succeed
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvCpssTmCtlHwInit
(
    IN GT_U8                   devNum,
    IN GT_U8                   numOfLad
);

/*******************************************************************************
* prvCpssTmCtlDevInit 
*
* DESCRIPTION:
*       Initialize TM Device Info.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_FAIL                     - on hardware error.
*       GT_NO_RESOURCE              - on out of memory space.
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device.
*       GT_BAD_STATE                - on burst mode or init doesnt succeed
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvCpssTmCtlDevInit 
(
	IN GT_U8	devNum
);

/*******************************************************************************
* prvCpssDxChTMFreqGet 
*
* DESCRIPTION:
*       Get TM Dram Frequency.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*
* OUTPUTS:
*       tmFreqPtr         - (pointer of) tmFreqency
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_FAIL                     - on hardware error.
*       GT_BAD_PARAM                - on bad params.
*
* COMMENTS:
*   tmFreqency 0: tm not supported.
*   tmFreqency 1: CPSS_DRAM_FREQ_800_MHZ_E.
*   tmFreqency 2: CPSS_DRAM_FREQ_933_MHZ_E.
*   tmFreqency 3: CPSS_DRAM_FREQ_667_MHZ_E.    
*******************************************************************************/
GT_STATUS prvCpssDxChTMFreqGet
(
    IN  GT_U8    devNum,
    OUT GT_U32 * tmFreqPtr
);




#endif 	    /* __prvCpssTmCtlh */
