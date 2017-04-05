/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChSmiUnitDrv.h
*
* DESCRIPTION:
*       SMI unit interface (Switching Core - GOP - SMI).
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __PRV_CPSS_DXCH_SMI_UNIT_DRV_H
#define __PRV_CPSS_DXCH_SMI_UNIT_DRV_H

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define NUMBER_OF_SMI_PER_LMS_UNIT_CNS      2
#define NUMBER_OF_PORTS_PER_LMS_UNIT_CNS    24

#define NUMBER_OF_PORTS_PER_SMI_UNIT_CNS    16

/*******************************************************************************
* prvCpssDxChSMIInfoByPPSet
*
* DESCRIPTION:
*       set SMI info 
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChSMIInfoByPPSet
(
    GT_U8 devNum
);

/*******************************************************************************
* prvCpssDxChSMIDrvInit
*
* DESCRIPTION:
*       Mac SMI driver init
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     it uses prv_dxChMacTG as single-tone
*
*******************************************************************************/
GT_STATUS prvCpssDxChSMIDrvInit
(
    IN    GT_U8                   devNum
);

GT_STATUS prvCpssDxChSMIDrvManagementRegAddrGet
(
    IN  GT_U8 devNum,
    IN  GT_U32 smiInstance,
    OUT GT_U32 * regAddrPtr
);

GT_STATUS prvCpssDxChSMIDrvMiscConfigRegAddrGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  smiInstance,
    OUT GT_U32 *regAddrPtr
);


GT_STATUS prvCpssDxChSMIDrvPHYAutoNegConfigRegAddrGet
(
    IN  GT_U8 devNum,
    IN  GT_U32 smiInstance,
    OUT GT_U32 * regAddrPtr
);


/*******************************************************************************
* prvCpssDxChSMIInvertMDCSet
*
* DESCRIPTION:
*       Enable/Disable InvertMDC
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       smiInstance     - smi instance
*       state           - GT_TRUE:  invert MDC
*                         GT_FALSE: don't invert MDC
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - on port is not initialized (interface/speed)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Before calling this function the port must be connected to packet
*          generator (cpssDxChDiagPacketGeneratorConnectSet),
*          otherwise GT_BAD_STATE is returned.
*
*******************************************************************************/
extern GT_STATUS prvCpssDxChSMIInvertMDCSet
(
    IN  GT_U8                devNum,
    IN  GT_U32               portGroupId,
    IN  GT_U32               smiInstance,
    IN  GT_BOOL              state
);




/*******************************************************************************
* prvCpssDxChSMIAutoNegStateSet
*
* DESCRIPTION:
*       Phy auto negaotiation set
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       smiInstance     - smi instance
*       state           - GT_TRUE:  Auto-Negotiation NotPerformed
*                         GT_FALSE: Auto-Negotiation Performed
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - on port is not initialized (interface/speed)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChSMIAutoNegStateSet
(
    IN  GT_U8                devNum,
    IN  GT_U32               portGroupId,
    IN  GT_U32               smiInstance,
    OUT GT_BOOL              state
);


/*******************************************************************************
* prvCpssDxChSMIAutoNegStateGet
*
* DESCRIPTION:
*       Phy auto negaotiation get
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       smiInstance     - smi instance
*
* OUTPUTS:
*       statePtr        - GT_TRUE:  Auto-Negotiation NotPerformed
*                         GT_FALSE: Auto-Negotiation Performed
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - on port is not initialized (interface/speed)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Before calling this function the port must be connected to packet
*          generator (cpssDxChDiagPacketGeneratorConnectSet),
*          otherwise GT_BAD_STATE is returned.
*
*******************************************************************************/
GT_STATUS prvCpssDxChSMIAutoNegStateGet
(
    IN  GT_U8                devNum,
    IN  GT_U32               portGroupId,
    IN  GT_U32               smiInstance,
    OUT GT_BOOL             *statePtr
);

/*******************************************************************************
* prvCpssDxChSMIAutoPollNumOfPortsSet
*
* DESCRIPTION:
*      Set the SMI Auto Polling number of ports.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       smiInstance     - smi instance
*       portGroupId     - portGroupId
*       state           - SMI Auto Polling number of ports.
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - on port is not initialized (interface/speed)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChSMIAutoPollNumOfPortsSet
(
    IN  GT_U8                devNum,
    IN  GT_U32               portGroupId,
    IN  GT_U32               smiInstance,
    OUT GT_U32               state
);


/*******************************************************************************
* prvCpssDxChSMIAutoPollNumOfPortsGet
*
* DESCRIPTION:
*      Get the SMI Auto Polling number of ports.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       smiInstance     - smi instance
*       portGroupId     - portGroupId
* OUTPUTS:
*       statePtr        - SMI Auto Polling number of ports.
*                         
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - on port is not initialized (interface/speed)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChSMIAutoPollNumOfPortsGet
(
    IN  GT_U8                devNum,
    IN  GT_U32               portGroupId,
    IN  GT_U32               smiInstance,
    OUT GT_U32              *statePtr
);

/*******************************************************************************
* prvCpssDxChSMIPortAutopollingPlaceGet
*
* DESCRIPTION:
*      Get the SMI HW port (Autopolling)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portGroupId     - portGroupId
*       smiInstance     - smi instance
*       smiLocalPort    - smi local port
* OUTPUTS:
*       autoPollingPlacePtr       - SMI Auto Polling number of ports.
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - on port is not initialized (interface/speed)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChSMIPortAutopollingPlaceGet
(
    IN  GT_U8                devNum,
    IN  GT_U32               portGroupId,
    IN  GT_U32               smiInstance,
    IN  GT_U32               smiLocalPort,
    OUT GT_U32              *autoPollingPlacePtr
);


/*******************************************************************************
* prvCpssDxChSMIPortPhyAddSet
*
* DESCRIPTION:
*      Set the SMI local port phy adderss
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portGroupId     - portGroupId
*       smiInstance     - smi instance
*       smiLocalPort    - smi local port
*       phyAddr         - SMI Auto Polling number of ports.
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - on port is not initialized (interface/speed)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChSMIPortPhyAddSet
(
    IN  GT_U8                devNum,
    IN  GT_U32               portGroupId,
    IN  GT_U32               smiInstance,
    IN  GT_U32               smiLocalPort,
    OUT GT_U32               phyAddr
);

/*******************************************************************************
* prvCpssDxChSMIPortPhyAddGet
*
* DESCRIPTION:
*      Get the SMI local port phy adderss
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portGroupId     - portGroupId
*       smiInstance     - smi instance
*       smiLocalPort    - smi local port
*
* OUTPUTS:
*       phyAddrPtr      - phy address
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - on port is not initialized (interface/speed)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChSMIPortPhyAddGet
(
    IN  GT_U8                devNum,
    IN  GT_U32               portGroupId,
    IN  GT_U32               smiInstance,
    IN  GT_U32               smiLocalPort,
    OUT GT_U32              *phyAddrPtr
);

/*******************************************************************************
* prvCpssDxChSMIPortAutoMediaDetectStatusSet
*
* DESCRIPTION:
*      Set the SMI local port Auto Media Detect Status
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portGroupId     - portGroupId
*       smiInstance     - smi instance
*       smiLocalPort    - smi local port
*       status          - Auto Media Detect Status
*
* OUTPUTS:
*       phyAddrPtr         - SMI Auto Polling number of ports.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - on port is not initialized (interface/speed)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChSMIPortAutoMediaDetectStatusSet
(
    IN  GT_U8                devNum,
    IN  GT_U32               portGroupId,
    IN  GT_U32               smiInstance,
    IN  GT_U32               smiLocalPort,
    OUT GT_U32               status
);

/*******************************************************************************
* prvCpssDxChSMIPortAutoMediaDetectStatusGet
*
* DESCRIPTION:
*      Get the SMI local port Auto Media Detect Status
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portGroupId     - portGroupId
*       smiInstance     - smi instance
*       smiLocalPort    - smi local port
*
* OUTPUTS:
*       statusPtr       - Auto Media Detect Status
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - on port is not initialized (interface/speed)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChSMIPortAutoMediaDetectStatusGet
(
    IN  GT_U8                devNum,
    IN  GT_U32               portGroupId,
    IN  GT_U32               smiInstance,
    IN  GT_U32               smiLocalPort,
    OUT GT_U32              *statusPtr
);


GT_STATUS prvCpssDxChSMIMdcDivisionFactorSet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portGroupId,
    IN  GT_U32   smiInstance,
    IN  CPSS_PHY_SMI_MDC_DIVISION_FACTOR_ENT divisionFactor
);

GT_STATUS prvCpssDxChSMIMdcDivisionFactorGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portGroupId,
    IN  GT_U32   smiInstance,
    OUT CPSS_PHY_SMI_MDC_DIVISION_FACTOR_ENT *divisionFactorPtr
);



typedef struct PRV_CPSS_DXCH_SMI_STATE_STC
{
    GT_U32 autoPollNumOfPortsArr[CPSS_PHY_SMI_INTERFACE_MAX_E];
    GT_U32 phyAddrRegArr        [CPSS_PHY_SMI_INTERFACE_MAX_E][NUMBER_OF_PORTS_PER_SMI_UNIT_CNS];
}PRV_CPSS_DXCH_SMI_STATE_STC;

GT_STATUS prvCpssDxChSMIStateGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portGroupId,
    OUT PRV_CPSS_DXCH_SMI_STATE_STC * statePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
