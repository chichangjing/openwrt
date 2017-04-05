/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortIfModeCfgBobKResource.h
*
* DESCRIPTION:
*       CPSS BobK implementation for Port interface mode resource configuration.
*
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __PRV_CPSS_DXCH_PORT_IF_MODE_CFG_BOBK_RESOURCE_H
#define __PRV_CPSS_DXCH_PORT_IF_MODE_CFG_BOBK_RESOURCE_H

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/port/cpssPortCtrl.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define PRV_CPSS_DXCH_PORT_RESOURCE_BAD_VALUE_CNS               0xFFFFFBAD

typedef struct
{
    GT_U32  txdmaCreditValue;
    GT_U32  txfifoHeaderCounterThresholdScdma;
    GT_U32  txfifoPayloadCounterThresholdScdma;
    GT_U32  txfifoScdmaPayloadThreshold;
    GT_U32  txfifoScdmaShiftersOutgoingBusWidth;
    GT_U32  rxdmaScdmaIncomingBusWidth;
    GT_U32  ethTxfifoOutgoingBusWidth;
    GT_U32  ethTxfifoScdmaPayloadThreshold;
}PRV_CPSS_DXCH_CAELUM_PORT_RESOURCE_STC;


typedef struct
{
    GT_BOOL                                 valid;
    PRV_CPSS_DXCH_CAELUM_PORT_RESOURCE_STC  res;
}PRV_CPSS_DXCH_CAELUM_PORT_RESOURCE_DATABASE_STC;


typedef enum
{
    PRV_CAELUM_REG_FIELDS_TXQ_Q_PORT_DESC_COUNTER_E,
    PRV_CAELUM_REG_FIELDS_TXQ_DQ_TXDMA_PORT_CREDIT_COUNTER_E,
    PRV_CAELUM_REG_FIELDS_TXQ_DQ_TXDMA_CREDIT_COUNTERS_RESET_E,
    PRV_CAELUM_REG_FIELDS_TXDMA_DESC_CREDITS_SCDMA_E,
    PRV_CAELUM_REG_FIELDS_TXDMA_SCDMA_HEADER_TXFIFO_COUNTER_E,
    PRV_CAELUM_REG_FIELDS_TXDMA_SCDMA_PAYLOAD_TXFIFO_COUNTER_E,
    PRV_CAELUM_REG_FIELDS_TXDMA_SCDMA_RESET_DESC_CREDITS_COUNTER_E,
    PRV_CAELUM_REG_FIELDS_TXDMA_SCDMA_TXFIFO_HEADER_THRESHOLD_E,
    PRV_CAELUM_REG_FIELDS_TXDMA_SCDMA_TXFIFO_PAYLOAD_THRESHOLD_E,
    PRV_CAELUM_REG_FIELDS_TXFIFO_SCDMA_PAYLOAD_THRESHOLD_E,
    PRV_CAELUM_REG_FIELDS_TXFIFO_SCDMA_SHIFTERS_OUTGOING_BUS_WIDTH_E,
    PRV_CAELUM_REG_FIELDS_RXDMA_SCDMA_INCOMING_BUS_WIDTH_E,
    PRV_CAELUM_REG_FIELDS_ETH_TXFIFO_OUTGOING_BUS_WIDTH_E,
    PRV_CAELUM_REG_FIELDS_ETH_TXFIFO_SCDMA_PAYLOAD_THRESHOLD_E,
    PRV_CAELUM_REG_FIELDS_LAST_E

}PRV_CAELUM_REG_FIELDS_ENT;


/*******************************************************************************
* prvCpssDxChCaelumPortResourcesConfig
*
* DESCRIPTION:
*       Caelum port resourse configuration
*
* APPLICABLE DEVICES:
*       Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Bobcat3.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       ifMode      - port interface mode
*       speed       - port interface speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, device, ifMode, speed
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChCaelumPortResourcesConfig
(
    IN GT_U8                           devNum,
    IN GT_PHYSICAL_PORT_NUM            portNum,
    IN CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN CPSS_PORT_SPEED_ENT             speed
);

/*******************************************************************************
* prvCpssDxChCaelumPortResourcesInit
*
* DESCRIPTION:
*       Initialize data structure for port resource allocation
*
* APPLICABLE DEVICES:
*       Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChCaelumPortResourcesInit
(
    IN    GT_U8                   devNum
);

/*******************************************************************************
* prvCpssDxChPortCaelumRegAddrGet
*
* DESCRIPTION:
*       Caelum port resourse registers get
*
* APPLICABLE DEVICES:
*       Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Bobcat3.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       fieldCode   - port field code name
*
* OUTPUTS:
*       regAddrPtr  - (pointer to) register address
*       fieldOffsetPtr - (pointer to) register field offset
*       fieldLenPtr - (pointer to) register field length
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, device, ifMode, speed
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChCaelumPortRegAddrGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  PRV_CAELUM_REG_FIELDS_ENT   fieldCode,
    OUT GT_U32                      *regAddrPtr,
    OUT GT_U32                      *fieldOffsetPtr,
    OUT GT_U32                      *fieldLenPtr
);

/*******************************************************************************
* prvCpssDxChCaelumPortRegFieldSet
*
* DESCRIPTION:
*       Caelum port resourse register field set
*
* APPLICABLE DEVICES:
*       Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Bobcat3.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       fieldCode   - field code name
*       fieldValue  - field value
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, device, ifMode, speed
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChCaelumPortRegFieldSet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  PRV_CAELUM_REG_FIELDS_ENT   fieldCode,
    IN  GT_U32                      fieldValue
);

/*******************************************************************************
* prvCpssDxChCaelumPortRegFieldGet
*
* DESCRIPTION:
*       Caelum port resourse register field get
*
* APPLICABLE DEVICES:
*       Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Bobcat3.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       fieldCode   - field code name
*
* OUTPUTS:
*       fieldValuePtr  - (pointer to) field value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, device, ifMode, speed
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChCaelumPortRegFieldGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  PRV_CAELUM_REG_FIELDS_ENT   fieldCode,
    OUT GT_U32                      *fieldValuePtr
);

/*******************************************************************************
* prvCpssDxChCaelumPortResourcesRelease
*
* DESCRIPTION:
*       Remove resources allocated to the port
*
* APPLICABLE DEVICES:
*       Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChCaelumPortResourcesRelease
(
    IN GT_U8                            devNum,
    IN GT_PHYSICAL_PORT_NUM             portNum
);

/*******************************************************************************
* prvCpssDxChCaelumPortResourceTmSet
*
* DESCRIPTION:
*       Caelum TM port resourse configuration
*
* APPLICABLE DEVICES:
*       Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Bobcat3.
*
*
* INPUTS:
*       devNum          - physical device number
*       tmBandwidthMbps - system TM bandwidth
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChCaelumPortResourceStaticTmSet
(
    IN GT_U8    devNum,
    IN  GT_U32  tmBandwidthMbps
);

/*******************************************************************************
* prvCpssDxChCaelumPortResourceDynamicTmSet
*
* DESCRIPTION:
*       Caelum TM port resourse configuration
*
* APPLICABLE DEVICES:
*       Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Bobcat3.
*
*
* INPUTS:
*       devNum          - physical device number
*       tmBandwidthMbps - system TM bandwidth
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChCaelumPortResourceDynamicTmSet
(
    IN GT_U8    devNum,
    IN  GT_U32  tmBandwidthMbps
);


/*******************************************************************************
* prvCpssDxChCaelumPortResourceTmGet
*
* DESCRIPTION:
*       Get Caelum TM port bandwidth
*
* APPLICABLE DEVICES:
*       Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Bobcat3.
*
*
* INPUTS:
*       devNum          - physical device number
*
* OUTPUTS:
*       tmBandwidthMbpsPtr - (pointer to) system TM bandwidth
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
******************************************************************************/
GT_STATUS prvCpssDxChCaelumPortResourceTmGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *tmBandwidthMbpsPtr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
