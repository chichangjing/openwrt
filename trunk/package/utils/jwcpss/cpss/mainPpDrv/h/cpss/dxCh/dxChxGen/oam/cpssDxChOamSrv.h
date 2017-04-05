/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChOamSrv.h
*
* DESCRIPTION:
*       CPSS DxCh OAM service CPU API.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#ifndef __cpssDxChOamSrvh
#define __cpssDxChOamSrvh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>

/* maximum flow id that is supported by the service CPU */
#define CPSS_DXCH_OAM_SRV_MAX_FLOW_ID_CNS     2047
/* number of reserved Rx queues for the service CPU */
#define CPSS_DXCH_OAM_RX_SDMA_QUEUE_NUM_CNS   2
/* number of reserved Tx queues for the service CPU */
#define CPSS_DXCH_OAM_TX_SDMA_QUEUE_NUM_CNS   1
/* maximum local mac address index that is supported by the service CPU */
#define CPSS_DXCH_OAM_SRV_MAX_LOCAL_MAC_ADDR_INDEX_CNS     127

/*
 *  Typedef: struct CPSS_DXCH_OAM_SRV_LB_CONFIG_STC
 *
 *  Description: Structure of OAM Service CPU Loopback configuration.
 *
 *  Fields:
 *      outOfSequenceEnable - When enabled, the packet’s sequence number field is
 *                            compared to the previous packet sequence number field.
 *                            GT_FALSE - disable out of sequence checking.
 *                            GT_TRUE - enable out of sequence checking.
 *      invalidTlvEnable    - When enabled, CRC32 is computed on the packet's
 *                            data TLV and compared to tlvDataCrc32Val.
 *                            GT_FALSE - disable invalid data TLV checking.
 *                            GT_TRUE - enable invalid data TLV checking.
 *      tlvDataCrc32Val     - The expected TLV data CRC32 value.
 *
 */
typedef struct
{
    GT_BOOL outOfSequenceEnable;
    GT_BOOL invalidTlvEnable;
    GT_U32  tlvDataCrc32Val;
}CPSS_DXCH_OAM_SRV_LB_CONFIG_STC;

/*
 *  Typedef: struct CPSS_DXCH_OAM_SRV_LB_COUNTERS_STC
 *
 *  Description: Structure of OAM Service CPU Loopback statistical counters
 *
 *  Fields:
 *      outOfSequenceCount  - The number of LBR packets that are out of sequence.
 *      totalCheckedDataTlv - The total number of LBR packets that were checked
 *                            for invalid TLV data CRC32.
 *      totalInvalidDataTlv - The total number of LBR packets that their data
 *                            TLV CRC32 value was wrong.
 */
typedef struct
{
    GT_U32  outOfSequenceCount;
    GT_U32  totalCheckedDataTlv;
    GT_U32  totalInvalidDataTlv;
}CPSS_DXCH_OAM_SRV_LB_COUNTERS_STC;

/*
 *  Typedef: struct CPSS_DXCH_OAM_SRV_SYS_PARAMS_STC
 *
 *  Description: Structure of OAM Service CPU system paramaters.
 *
 *  Fields:
 *      rxQueueNum             -  array of Rx Queues reserved for OAM firmware.
 *                                first value is used as the queue for highest priority
 *                                and last value is lowest priority
 *                                (APPLICABLE RANGES: 0..7)
 *      txQueueNum              - array of Tx Queues reserved for OAM firmware.
 *                                first value is used as the queue for highest priority
 *                                and last value is lowest priority
 *                                (APPLICABLE RANGES: 0..7)
 *      downMepsNum             - Number of Down MEPs.
 *                                (APPLICABLE RANGES: 0..2047)
 *      upMepsNum               - Number of Up MEPs.
 *                                (APPLICABLE RANGES: 0..2047)
 *      rxCcmFlowsNum           - Number of Rx CCM flows.
 *                                (APPLICABLE RANGES: 0..2047)
 */
typedef struct
{
    GT_U32  rxQueueNum[CPSS_DXCH_OAM_RX_SDMA_QUEUE_NUM_CNS];
    GT_U32  txQueueNum[CPSS_DXCH_OAM_TX_SDMA_QUEUE_NUM_CNS];
    GT_U32  downMepsNum;
    GT_U32  upMepsNum;
    GT_U32  rxCcmFlowsNum;
}CPSS_DXCH_OAM_SRV_SYS_PARAMS_STC;

/*
 *  Typedef: struct CPSS_DXCH_OAM_SRV_DM_CONFIG_STC
 *
 *  Description: Structure of OAM Service CPU Delay Measurement configuration.
 *
 *  Fields:
 *      countersEnable - GT_TRUE - DM statistical counters enabled and calculated in
 *                             service CPU after each DM frame.
 *                       GT_FALSE - DM statistical counters disabled
 */
typedef struct
{
    GT_BOOL countersEnable;

}CPSS_DXCH_OAM_SRV_DM_CONFIG_STC;

/*
 *  Typedef: struct CPSS_DXCH_OAM_SRV_DM_BASIC_COUNTERS_STC
 *
 *  Description: Structure of OAM Service CPU Delay Measurement basic counters.
 *
 *  Fields:
 *  Single-Ended Delay Measurement Counters (DMR):
 *    twoWayFrameDelay          - A 32-bit Two-Way Frame Delay Measurement
 *    oneWayFrameDelayForward   - A 32-bit One-Way Frame Delay Measurement in the forward direction
 *    oneWayFrameDelayBackward  - A 32-bit One-Way Frame Delay Measurement in the backward direction
 *
 */
typedef struct
{
    GT_U32 twoWayFrameDelay;
    GT_U32 oneWayFrameDelayForward;
    GT_U32 oneWayFrameDelayBackward;


} CPSS_DXCH_OAM_SRV_DM_BASIC_COUNTERS_STC;

/*
 *  Typedef: struct CPSS_DXCH_OAM_SRV_DM_COUNTERS_STC
 *
 *  Description: Structure of OAM Service CPU Delay Measurement counters.
 *
 *  Fields:
 *      basicCounters - basic DM counters.
 *
 */
typedef struct
{
    CPSS_DXCH_OAM_SRV_DM_BASIC_COUNTERS_STC 	basicCounters;

} CPSS_DXCH_OAM_SRV_DM_COUNTERS_STC;

/*
 *  Typedef: struct CPSS_DXCH_OAM_SRV_LM_CONFIG_STC
 *
 *  Description: Structure of OAM Service CPU Loss Measurement configuration.
 *
 *  Fields:
 *      countersEnable - GT_TRUE - LM statistical counters enabled and calculated in
 *                             service CPU after each LM frame.
 *                       GT_FALSE - LM statistical counters disabled.
 */
typedef struct
{
    GT_BOOL countersEnable;

}CPSS_DXCH_OAM_SRV_LM_CONFIG_STC;

/*
 *  Typedef: struct CPSS_DXCH_OAM_SRV_LM_BASIC_COUNTERS_STC
 *
 *  Description: Structure of OAM Service CPU Loss Measurement basec counters.
 *
 *  Fields:
 *      txFrameCntForward - tx frame count in the forward direction
 *      rxFrameCntForward - rx frame count in the forward direction
 *      txFrameCntBackward - tx frame count in the backward direction
 *      rxFrameCntBackward - rx frame count in the backward direction
 *      farEndFrameLoss - calculation of far end frame loss
 *                        |TxFCf[tc] – TxFCf[tp]| – |RxFCf[tc] – RxFCf[tp]|
 *      nearEndFrameLoss - calculation of near end frame loss
 *                         |TxFCb[tc] – TxFCb[tp]| – |RxFCl[tc] – RxFCl[tp]|
 */
typedef struct
{
    GT_U32 txFrameCntForward;
    GT_U32 rxFrameCntForward;
    GT_U32 txFrameCntBackward;
    GT_U32 rxFrameCntBackward;

    GT_U32  farEndFrameLoss;
    GT_U32  nearEndFrameLoss;

} CPSS_DXCH_OAM_SRV_LM_BASIC_COUNTERS_STC;

/*
 *  Typedef: struct CPSS_DXCH_OAM_SRV_LM_COUNTERS_STC
 *
 *  Description: Structure of OAM Service CPU Loss Measurement counters.
 *
 *  Fields:
 *      basicCounters - basic LM statistical counters.
 *
 */
typedef struct
{
    CPSS_DXCH_OAM_SRV_LM_BASIC_COUNTERS_STC	basicCounters;

} CPSS_DXCH_OAM_SRV_LM_COUNTERS_STC;

/*
 *  Typedef: enum CPSS_DXCH_OAM_SRV_FRAME_TYPE_ENT
 *
 *  Description: Enumeration of OAM Service frame type
 *
 *  Enumerations:
 *      CPSS_DXCH_OAM_SRV_FRAME_TYPE_1731_E - the flow is of type Y.1731 protocol
 *      CPSS_DXCH_OAM_SRV_FRAME_TYPE_1711_E - the flow is of type Y.1711 protocol
 *
 */
typedef enum
{
    CPSS_DXCH_OAM_SRV_FRAME_TYPE_1731_E,
    CPSS_DXCH_OAM_SRV_FRAME_TYPE_1711_E
} CPSS_DXCH_OAM_SRV_FRAME_TYPE_ENT;

/*
 *  Typedef: enum CPSS_DXCH_OAM_SRV_MP_TYPE_ENT
 *
 *  Description: Enumeration of OAM Service Maintenance-Point type
 *
 *  Enumerations:
 *          CPSS_DXCH_OAM_SRV_MP_TYPE_UP_MEP_E - the flow is of up-mep typedef
 *          CPSS_DXCH_OAM_SRV_MP_TYPE_DOWN_MEP_E - the flow is of down-mep type
 *          CPSS_DXCH_OAM_SRV_MP_TYPE_MIP_E - the flow is of mip type
 *          CPSS_DXCH_OAM_SRV_MP_TYPE_OTHER_E - the flow is nor up-mp, down-mep or mip
 *
 */
typedef enum
{
    CPSS_DXCH_OAM_SRV_MP_TYPE_UP_MEP_E,
    CPSS_DXCH_OAM_SRV_MP_TYPE_DOWN_MEP_E,
    CPSS_DXCH_OAM_SRV_MP_TYPE_MIP_E,
    CPSS_DXCH_OAM_SRV_MP_TYPE_OTHER_E
} CPSS_DXCH_OAM_SRV_MP_TYPE_ENT;

/*
 *  Typedef: struct CPSS_DXCH_OAM_SRV_TRANSMIT_PARAMS_STC
 *
 *  Description: Transmission Frames Parameters: (for Tx responding)
 *
 *  Fields:
 *      l2HeaderValid   - if l2HeaderValid==GT_TRUE - l2Header parameter is used
 *                          and the L2 header for Tx responding is built from l2Header parameter.
 *                        if l2HeaderValid==GT_FALSE - l2Header parameter is ignored
 *                          and the L2 header for Tx responding is built from the received OAM frame.
 *      l2Header		- L2 header parameters.
 *          header[64]  - Full L2 header not including the OAM frame.
 *                        The L2 header might include any packet header that encapsulates the CFM frame.
 *                        For example CFM header can be encapsulated in simple L2 ethernet header with multiple VLANs
 *                        Or MPLS header including the transport and PW header.
 *          macAddr     - This MAC SA is used in Tx responding L2 header frame (For example, the received OAM
 *                        frame is a MC and the responding is UC using the configured MAC SA of the specified OAM flow id)
 *      dsaParamValid   - if dsaParamValid==GT_TRUE - the DSA tag of the packet is
 *                          taken from dsaParam parameter.
 *                        if dsaParamValid==GT_FALSE - dsaParam parameter is ignored
 *                          and the DSA Tag is build based on the received OAM frame parameters.
 *      dsaParam		- DSA header. used if dsaParamValid==GT_TRUE.
 *
 *
 */
typedef struct
{
    GT_BOOL                             l2HeaderValid;
    union
    {
        GT_U8                           header[64];
        GT_ETHERADDR     		        macAddr;
    } l2Header;

    GT_BOOL                             dsaParamValid;
    CPSS_DXCH_NET_DSA_PARAMS_STC        dsaParam;

} CPSS_DXCH_OAM_SRV_TRANSMIT_PARAMS_STC;

/*
 *  Typedef: struct CPSS_DXCH_OAM_SRV_1731_CONFIG_STC
 *
 *  Description: Structure of OAM Service CPU 1731 parameters configuration.
 *
 *  Fields:
 *    Maintenance Point parameters:
 *      mpType		- Maintenance Point type.
 *    Reception Frames Parameters:
 *    (following parameters are used for OAM packets that need to be send from service CPU)
 *      cfmOffset		- Represent the beginning of OAM payload in the received
 *                        and transmitted frame.
 *                        If cfmOffset==0 - this parameter is ignored and the service CPU
 *                        assumes that OAM ethertype starts after the vlan tag.
 *                        (APPLICABLE RANGES: 12..127)
 *   	transmitParams 	- Transmission Frames Parameters for Tx responding.
 *
 */
typedef struct
{
    CPSS_DXCH_OAM_SRV_MP_TYPE_ENT	        mpType;
    GT_U32				                    cfmOffset;
    CPSS_DXCH_OAM_SRV_TRANSMIT_PARAMS_STC	transmitParams;

}CPSS_DXCH_OAM_SRV_1731_CONFIG_STC;

/*
 *  Typedef: struct CPSS_DXCH_OAM_SRV_RX_FLOW_FRAME_PARAMS_STC
 *
 *  Description: Structure of OAM rx-flow parameters configuration in service CPU.
 *
 *  Fields:
 *    frameType - OAM protocol frame Type. Type for the next union.
 *    frame - frame parameters according to frameType parameter.
 *    frame1731 - frame parameters if frameType == CPSS_DXCH_OAM_SRV_FRAME_TYPE_1731_E.
 *
 */
typedef struct
{
    CPSS_DXCH_OAM_SRV_FRAME_TYPE_ENT		 frameType;
    union{
        CPSS_DXCH_OAM_SRV_1731_CONFIG_STC    frame1731;

    }frame;

}CPSS_DXCH_OAM_SRV_RX_FLOW_FRAME_PARAMS_STC;

/*******************************************************************************
* cpssDxChOamSrvLoopbackConfigSet
*
* DESCRIPTION:
*      Set OAM service CPU Loopback configuration per flow ID.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; xCat3.
*
* INPUTS:
*       devNum                  - device number.
*       flowId                  - OAM MEP flow ID.
*                                 (APPLICABLE RANGES: 1..2048)
*       lbConfigPtr             - (pointer to) struct of LB configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamSrvLoopbackConfigSet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          flowId,
    IN  CPSS_DXCH_OAM_SRV_LB_CONFIG_STC *lbConfigPtr
);

/*******************************************************************************
* cpssDxChOamSrvLoopbackConfigGet
*
* DESCRIPTION:
*      Get OAM service CPU Loopback configuration per flow ID.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; xCat3.
*
* INPUTS:
*       devNum                  - device number.
*       flowId                  - OAM MEP flow ID.
*                                 (APPLICABLE RANGES: 1..2048)
*
* OUTPUTS:
*       lbConfigPtr             - (pointer to) struct of LB configuration.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamSrvLoopbackConfigGet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          flowId,
    OUT CPSS_DXCH_OAM_SRV_LB_CONFIG_STC *lbConfigPtr
);

/*******************************************************************************
* cpssDxChOamSrvLoopbackStatisticalCountersGet
*
* DESCRIPTION:
*      Get the OAM service CPU LB statistical counters per flow ID.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; xCat3.
*
* INPUTS:
*       devNum                   - device number.
*       flowId                   - OAM MEP flow ID 
*                                  (APPLICABLE RANGES: 1..2048)
*       clearOnRead              - perform clear on read.
*                                  GT_TRUE: The flows Loopback statistical 
*                                           counters will be cleared.
*                                  GT_FALSE: The flows Loopback statistical
*                                            counters will not be cleared.
*       timeout                  - time to wait for the service CPU to response
*                                  0 - no wait.
*                                  1 to 999 - defined time in miliseconds. 
*                                  0xFFFFFFFF - wait forever.
*
* OUTPUTS:
*       lbCountersPtr            - (pointer to) struct of current counter values.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamSrvLoopbackStatisticalCountersGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              flowId,
    IN  GT_BOOL                             clearOnRead,
    IN  GT_U32                              timeout,
    OUT CPSS_DXCH_OAM_SRV_LB_COUNTERS_STC   *lbCountersPtr  
);

/*******************************************************************************
* cpssDxChOamSrvLoopbackStatisticalCountersClear
*
* DESCRIPTION:
*      Clear the LB statistical counters per flow ID.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; xCat3.
*
* INPUTS:
*       devNum                   - device number.
*       flowId                   - OAM MEP flow ID.
*                                  (APPLICABLE RANGES: 1..2048)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamSrvLoopbackStatisticalCountersClear
(
    IN  GT_U8   devNum,
    IN  GT_U32  flowId
);

/*******************************************************************************
* cpssDxChOamSrvDmConfigSet
*
* DESCRIPTION:
*      Set OAM service CPU Delay Measurement configuration per flow ID.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; xCat3.
*
* INPUTS:
*       devNum                  - device number.
*       flowId                  - OAM MEP flow ID
*                                 (APPLICABLE RANGES: 1..2048).
*       dmConfigPtr             - (pointer to) struct of LM configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamSrvDmConfigSet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          flowId,
    IN  CPSS_DXCH_OAM_SRV_DM_CONFIG_STC *dmConfigPtr
);

/*******************************************************************************
* cpssDxChOamSrvDmConfigGet
*
* DESCRIPTION:
*      Get OAM service CPU Delay Measurement configuration per flow ID.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; xCat3.
*
* INPUTS:
*       devNum                  - device number.
*       flowId                  - OAM MEP flow ID.
*                                 (APPLICABLE RANGES: 1..2048).
*
* OUTPUTS:
*       dmConfigPtr             - pointer to struct of DM configuration.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamSrvDmConfigGet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          flowId,
    OUT CPSS_DXCH_OAM_SRV_DM_CONFIG_STC *dmConfigPtr
);

/*******************************************************************************
* cpssDxChOamSrvDmStatisticalCountersGet
*
* DESCRIPTION:
*      Get the Delay Measurement statistical counters per flow ID from service CPU .
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; xCat3.
*
* INPUTS:
*       devNum                   - device number.
*       flowId                   - OAM MEP flow ID
*                                 (APPLICABLE RANGES: 1..2048).
*       clearOnRead              - GT_TRUE - clear the counter.
*                                  GT_FALSE - don't clear the counter. 
*
* OUTPUTS:
*       dmCountersPtr            - (pointer to) struct of current counter values.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamSrvDmStatisticalCountersGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              flowId,
    IN  GT_BOOL                             clearOnRead,
    OUT CPSS_DXCH_OAM_SRV_DM_COUNTERS_STC   *dmCountersPtr  
);

/*******************************************************************************
* cpssDxChOamSrvDmStatisticalCountersClear
*
* DESCRIPTION:
*      Clear the Delay Measurement statistical counters per flow ID in service CPU.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; xCat3.
*
* INPUTS:
*       devNum                   - device number.
*       flowId                   - OAM MEP flow ID
*                                 (APPLICABLE RANGES: 1..2048).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamSrvDmStatisticalCountersClear
(
    IN  GT_U8   			        devNum,
    IN  GT_U32  			        flowId
);

/*******************************************************************************
* cpssDxChOamSrvLmConfigSet
*
* DESCRIPTION:
*      Set OAM service CPU Loss Measurement configuration per flow ID.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; xCat3.
*
* INPUTS:
*       devNum                  - device number.
*       flowId                  - OAM MEP flow ID
*                                 (APPLICABLE RANGES: 1..2048).
*       lmConfigPtr             - (pointer to) struct of LM configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamSrvLmConfigSet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          flowId,
    IN  CPSS_DXCH_OAM_SRV_LM_CONFIG_STC *lmConfigPtr
);

/*******************************************************************************
* cpssDxChOamSrvLmConfigGet
*
* DESCRIPTION:
*      Get OAM service CPU Loss Measurement configuration per flow ID.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; xCat3.
*
* INPUTS:
*       devNum                  - device number.
*       flowId                  - OAM MEP flow ID
*                                 (APPLICABLE RANGES: 1..2048).
*
* OUTPUTS:
*       lmConfigPtr             - pointer to struct of LM configuration.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamSrvLmConfigGet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          flowId,
    OUT CPSS_DXCH_OAM_SRV_LM_CONFIG_STC *lmConfigPtr
);

/*******************************************************************************
* cpssDxChOamSrvLmStatisticalCountersGet
*
* DESCRIPTION:
*      Get the OAM service CPU Loss Measurement statistical counters per flow ID.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; xCat3.
*
* INPUTS:
*       devNum                   - device number.
*       flowId                   - OAM MEP flow ID
*                                  (APPLICABLE RANGES: 1..2048).
*       clearOnReadEnable        - GT_TRUE - clear the counter on read.
*                                  GT_FALSE - don't clear the counter.
*
* OUTPUTS:
*       lmCountersPtr            - (pointer to) struct of current counter values.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamSrvLmStatisticalCountersGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              flowId,
    IN  GT_BOOL                             clearOnReadEnable,
    OUT CPSS_DXCH_OAM_SRV_LM_COUNTERS_STC   *lmCountersPtr  
);

/*******************************************************************************
* cpssDxChOamSrvLmStatisticalCountersClear
*
* DESCRIPTION:
*      Clear the Loss Measurement statistical counters per flow ID.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; xCat3.
*
* INPUTS:
*       devNum                   - device number.
*       flowId                   - OAM MEP flow ID
*                                 (APPLICABLE RANGES: 1..2048).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamSrvLmStatisticalCountersClear
(
    IN  GT_U8   			        devNum,
    IN  GT_U32  			        flowId
);

/*******************************************************************************
* cpssDxChOamSrvLocalMacAddressSet
*
* DESCRIPTION:
*      Globally configure the MAC address table in the service CPU.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; xCat3.
*
* INPUTS:
*       devNum         - device number.
*       index 		   - Local MAC Address Entry.
*                        This index will be used in cpssDxChOamSrvLocalInterfaceMacIndexSet.
*                        (APPLICABLE RANGES:0..127)
*       macAddr 	   - mac address.
*     
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamSrvLocalMacAddressSet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          index,    
    IN  GT_ETHERADDR     		        macAddr
);

/*******************************************************************************
* cpssDxChOamSrvLocalMacAddressGet
*
* DESCRIPTION:
*      Get the MAC address table
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; xCat3.
*
* INPUTS:
*       devNum         - device number.
*       index 		   - Local MAC Address Entry
*                        (APPLICABLE RANGES:0..127)
*     
* OUTPUTS:
*       macAddrPtr	   - pointer to mac address.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamSrvLocalMacAddressGet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          index,    
    OUT GT_ETHERADDR     		        *macAddrPtr
);

/*******************************************************************************
* cpssDxChOamSrvLocalInterfaceMacIndexSet
*
* DESCRIPTION:
*      Globally configure the MAC address index of a local interface.
*      If OAM packet that come to service CPU is multicast packet, and this packet
*      need to be send back from the CPU - the source mac for this packet will be
*      taken from this table according to source interface that packet comes from.
*      the destination mac will be the incoming source mac.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; xCat3.
*
* INPUTS:
*       devNum         - device number.
*       portNum        - port or E-port number.
*       isPhysicalPort - defines type of port parameter to be either
*                        physical port or ePort.
*                        GT_TRUE  - port is physical port
*                        GT_FALSE - port is ePort
*       macAddIndex	   - mac address index.
*                        (the mac address index that configured in cpssDxChOamSrvLocalMacAddressSet)
*                        (APPLICABLE RANGES:0..127)
*     
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamSrvLocalInterfaceMacIndexSet
(
    IN  GT_U8                            	devNum,
    IN  GT_PORT_NUM                         portNum,
    IN  GT_BOOL                             isPhysicalPort,
    IN  GT_U32	     		            	macAddIndex
);

/*******************************************************************************
* cpssDxChOamSrvLocalInterfaceMacIndexGet
*
* DESCRIPTION:
*      Get the MAC address index of a local interface
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; xCat3.
*
* INPUTS:
*       devNum         - device number.
*       portNum        - port or E-port number.
*       isPhysicalPort - defines type of port parameter to be either
*                        physical port or ePort.
*                        GT_TRUE  - port is physical port
*                        GT_FALSE - port is ePort
*     
* OUTPUTS:
*       macAddIndexPtr  - mac address index.
*                        (the mac address index that configured in cpssDxChOamSrvLocalMacAddressSet).
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamSrvLocalInterfaceMacIndexGet
(
    IN  GT_U8                            	devNum,
    IN  GT_PORT_NUM                         portNum,
    IN  GT_BOOL                             isPhysicalPort,
    OUT GT_U32	     		             	*macAddIndexPtr
);

/*******************************************************************************
* cpssDxChOamSrvRxFlowEntrySet
*
* DESCRIPTION:
*      Add new Rx OAM entry to OAM service CPU database
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; xCat3.
*
* INPUTS:
*       devNum         - device number.
*       flowId	   	   - Flow ID mapped to the peer MEP/MIP.
*                        (APPLICABLE RANGES: 1..2048)
*       cookiePtr      - for application use
*       frameParamsPtr - (pointer to) parameters per frame type to define how the packet will be sent
*     
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamSrvRxFlowEntrySet
(
    IN  GT_U8                               	    devNum,
    IN  GT_U32                               	    flowId,
    IN  GT_PTR				                        cookiePtr,
    IN  CPSS_DXCH_OAM_SRV_RX_FLOW_FRAME_PARAMS_STC	*frameParamsPtr
);

/*******************************************************************************
* cpssDxChOamSrvRxFlowEntryGet
*
* DESCRIPTION:
*      Add new Rx OAM entry to OAM service CPU database
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; xCat3.
*
* INPUTS:
*       devNum         - device number.
*       flowId	   	   - Flow ID mapped to the peer MEP/MIP.
*                        (APPLICABLE RANGES: 1..2048)
*
* OUTPUTS:
*       cookiePtr      - for application use
*       frameParamsPtr - parameters per frame type to define how the packet will be sent
*     
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChOamSrvRxFlowEntryGet
(
    IN  GT_U8                            	    	devNum,
    IN  GT_U32                           	    	flowId,
    OUT GT_PTR					                    *cookiePtr,
    OUT CPSS_DXCH_OAM_SRV_RX_FLOW_FRAME_PARAMS_STC	*frameParamsPtr
);

/*******************************************************************************
* cpssDxChOamSrvSystemInit
*
* DESCRIPTION:
*      Initialize the service CPU OAM Firmware.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; xCat3.
*
* INPUTS:
*       devNum                   - device number.
*       sysParamsPtr             - (pointer to) struct of system paramaters. 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Must be called after cpssDxChNetIfInit Is called.
*
*******************************************************************************/
GT_STATUS cpssDxChOamSrvSystemInit
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_OAM_SRV_SYS_PARAMS_STC   *sysParamsPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChOamSrvh */
