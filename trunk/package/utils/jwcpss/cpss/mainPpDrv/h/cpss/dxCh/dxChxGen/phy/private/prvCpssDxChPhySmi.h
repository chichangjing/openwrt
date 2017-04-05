/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPhySmi.h
*
* DESCRIPTION:
*        Private definitions for PHY SMI.
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*
*******************************************************************************/
#ifndef __prvCpssDxChPhySmih
#define __prvCpssDxChPhySmih

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/phy/cpssGenPhySmi.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* offset of PHY SMI Group 1 registers */
#define CPSS_DX_PHY_ADDR_REG_OFFSET_CNS     0x0800000

/* offset of SMI Control registers */
#define CPSS_DX_SMI_MNG_CNTRL_OFFSET_CNS    0x1000000

/* number of tri-speed or FE ports with PHY poling support */
#define PRV_CPSS_DXCH_SMI_PPU_PORTS_NUM_CNS     24

/* number of network ports per SMI interface */
#define PRV_CPSS_DXCH_E_ARCH_SMI_PORTS_NUM_CNS          24

/* number of network ports with PHY poling support */
#define PRV_CPSS_DXCH_E_ARCH_SMI_PPU_PORTS_NUM_CNS      48

/******************************************************************************
* prvCpssDxChPhySmiObjInit
*
* DESCRIPTION:
*       Initialise SMI service function pointers : SMI Ctrl Reg. Read/Write.
*       The generic SMI functions cpssSmiRegisterReadShort, 
*       cpssSmiRegisterWriteShort use these pointers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - the device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       none.
*
* COMMENTS:
*******************************************************************************/
GT_VOID prvCpssDxChPhySmiObjInit
(
    IN  GT_U8     devNum
);

/*******************************************************************************
* prvCpssDxChPortSMIAutoPollingMappingGet
*
* DESCRIPTION:
*        this function returns SMI instance , SMI local port and autopolling 
*        automaton at SMI controller
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - physical device number.
*       portNum         - port number.
*
* OUTPUTS:
*       smiInterfacePtr     - smi IF
*       smiLocalPortPtr     - smi local port (logical)
*       autoPollingPlacePtr - hw place at SMI controller (physical) 
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. applicable to BC2-B0 and higher, just for SMI controller (i.e. SMI unit and not LMS unit)
*       2. BC2 B0/CAELUM : at SMI IF 0,2 autopolling place equal to smi logical port
*                    at SMI IF 1,3 autopolling place is (16 - number of autopolling ports + logical) (adjusted to 16
*                       ex : autopolling = 8 , than HW places are 8-15 corrsponts to logical 0-7
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortSMIAutoPollingMappingGet
(
    IN  GT_U8                        devNum,
    IN  GT_PHYSICAL_PORT_NUM         portNum,
    OUT CPSS_PHY_SMI_INTERFACE_ENT  *smiInterfacePtr,
    OUT GT_U32                      *smiLocalPortPtr,
    OUT GT_U32                      *autoPollingPlacePtr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChPhySmih */

