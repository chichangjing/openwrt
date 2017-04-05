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
* DESCRIPTION:    TM configuration library control interface 
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#ifndef __cpssTmCtlh
#define __cpssTmCtlh

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpss/generic/tm/cpssTmPublicDefs.h>

#define WAIT_LOOP   10
#define SLEEP_MS   100

/*
 * typedef: struct CPSS_TM_CTL_LAD_INF_PARAM_STC
 *
 * Description:
 *      This struct defines custom LAD interface parameters,
 *      its used when needed to overwrite LADs Default configuration.
 *
 * Fields:
 *  minPkgSize            - Minimal DRAM Package size in Chunk (64 bytes) units.
 *                          value range is 0x1(64 Bytes) - 0x80 (8k Bytes). 
 *  pagesPerbank          - Number of DRAM pages per bank that are used by the TM Queue Manager.
 *  PkgesPerbank          - Number of DRAM packeges per bank that are used by the TM Queue Manager.
 *  portChuncksEmitPerSel - Maximal number of chunks (16 bytes) emitted from port per single
 *  						Scheduler selection.
 *  bapIDs                - 2 BAP interfaces connected to current LAD.
 * 
 * Comment:
 *  filed with zeroe value is ignored and the default value is
 *  unchanged for this field.
 *
*/
typedef struct
{
    GT_U32 minPkgSize;
    GT_U32 pagesPerBank;
    GT_U32 pkgesPerBank;
    GT_U32 portChunksEmitPerSel; 
	GT_U8 bapIDs[2];
}
CPSS_TM_CTL_LAD_INF_PARAM_STC;

#define CPSS_TM_CTL_MAX_NUM_OF_LADS_CNS 5 
#define CPSS_TM_CTL_MAX_NUM_OF_BAPS_CNS 10

/*******************************************************************************
* cpssTmCtlLibInit
*
* DESCRIPTION:
*       Initialize the TM configuration library.
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
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmInit
(
    IN GT_U8 devNum
);
/*******************************************************************************
* cpssTmCtlLibInitExt
*
* DESCRIPTION:
*       Customize Initialize of the TM configuration library.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       tmLibInitParams   - TM LIB initialize parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_FAIL                     - on hardware error.
*       GT_NO_RESOURCE              - on out of memory space.
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmInitExt
(
    IN GT_U8 devNum,
    IN CPSS_TM_LIB_INIT_PARAMS_STC *tmLibInitParams
);

/*******************************************************************************
* cpssTmCtlLibClose
*
* DESCRIPTION:
*       Close the TM configuration library.
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
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmClose
(
    IN GT_U8 devNum
);

/*******************************************************************************
* cpssTmCtlReadRegister
*
* DESCRIPTION:
*       Read register.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       regAddr         - Register address.
*       dataPtr         - Pointer to read data.
* 
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*******************************************************************************/
GT_STATUS cpssTmCtlReadRegister
(
    IN GT_U8                   devNum,
    IN GT_U64                  regAddr,
    OUT GT_U64                 *dataPtr
);
/*******************************************************************************
* cpssTmCtlWriteRegister
*
* DESCRIPTION:
*       Write register.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       regAddr         - Register address.
*       dataPtr         - Pointer to write data.
* 
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*******************************************************************************/
GT_STATUS cpssTmCtlWriteRegister
(
    IN GT_U8                   devNum,
    IN GT_U64                  regAddr,
    IN GT_U64                  *dataPtr
);


/*******************************************************************************
* cpssTmCtlLadParamsSet
*
* DESCRIPTION:
*       Set TM LAD parameters to its DB when neccessary to overwrite
*       its default configuration, parameters are used from DB
*       at cpssTmCtlLadInit.  
* 
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum       - physical device number.
*       numOfLads    - number of LADs.
*       ladParamsPtr - (pointer of) CPSS_TM_CTL_LAD_INF_PARAM_STC. 
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
GT_STATUS cpssTmCtlLadParamsSet
(
    IN  GT_U8                         devNum,
    IN  GT_U8                         numOfLads,
    IN  CPSS_TM_CTL_LAD_INF_PARAM_STC *ladParamsPtr
);

/*******************************************************************************
* cpssTmCtlLadParamsGet
*
* DESCRIPTION:
*       Get TM LAD parameters from its DB.  
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum       - physical device number.
*       numOfLads    - number of LADs.
*       ladParamsPtr - (pointer of) CPSS_TM_CTL_LAD_INF_PARAM_STC. 
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
GT_STATUS cpssTmCtlLadParamsGet
(
    IN  GT_U8                         devNum,
    IN  GT_U8                         numOfLads,
    OUT CPSS_TM_CTL_LAD_INF_PARAM_STC *ladParamsPtr
);

/*******************************************************************************
* cpssTmTreeParamsGet
*
* DESCRIPTION:
*       Get TM Tree Hw properties.
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
*       tmTreeParamsPtr  - pointer to CPSS_TM_TREE_PARAMS_STC.    
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_FAIL                     - on hardware error.
*       GT_BAD_PARAM                - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmTreeParamsGet
(
    IN GT_U8 devNum,
    OUT CPSS_TM_TREE_PARAMS_STC *tmTreeParamsPtr
);


/*******************************************************************************
* cpssTmUnitsErrorStatusGet
*
* DESCRIPTION:
*       Get TM Units Global Error Status
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
*       errorStatusInfoPtr- pointer to CPSS_TM_UNITS_ERROR_STATUS_STC.    
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_FAIL                     - on hardware error.
*       GT_BAD_PARAM                - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmUnitsErrorStatusGet
(
    IN GT_U8 devNum,
    OUT CPSS_TM_UNITS_ERROR_STATUS_STC *errorStatusInfoPtr
);

/*******************************************************************************
* cpssTmBapUnitErrorStatusGet
*
* DESCRIPTION:
*       Get TM BAP Unit Global Error Status
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*       bapNum            - BAP interface number. (APPLICABLE RANGES: 0..CPSS_TM_CTL_MAX_NUM_OF_BAPS_CNS).
* OUTPUTS:
*       errorStatusInfoPtr- pointer to CPSS_TM_BAP_UNIT_ERROR_STATUS_STC.    
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_FAIL                     - on hardware error.
*       GT_BAD_PARAM                - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device.
*       GT_NO_RESOURCE              - on not exist BAP interface.
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmBapUnitErrorStatusGet
(
    IN GT_U8 devNum,
    IN GT_U8 bapNum,
    OUT CPSS_TM_BAP_UNIT_ERROR_STATUS_STC *errorStatusInfoPtr
);


#endif 	    /* __cpssTmCtlh */
