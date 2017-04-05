/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* cpssDxChNetIfMii.c
*
* DESCRIPTION:
*       Include DxCh MII/CPU Ethernet port network interface management functions.
*
*
* FILE REVISION NUMBER:
*       $Revision: 20 $
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/networkIf/private/prvCpssDxChNetworkIfLog.h>
#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>
#include <cpss/generic/networkIf/cpssGenNetIfMii.h>
#include <cpss/generic/networkIf/private/prvCpssGenNetworkIfMii.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
#include <cpss/dxCh/dxChxGen/networkIf/private/prvCpssDxChNetIf.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/cos/private/prvCpssDxChCoS.h> /* for queue number validity check */

/*******************************************************************************
* internal_cpssDxChNetIfMiiTxPacketSend
*
* DESCRIPTION:
*       This function receives packet buffers & parameters from Application,
*       prepares them for the transmit operation, and
*       transmits a packet through the ethernet port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - Device number.
*       pcktParamsPtr       - The internal packet params to be set into the packet
*                             descriptors.
*       packetBuffsArrPtr   - (pointer to)The packet data buffers list.
*       buffLenArr          - A list of the buffers len in packetBuffsArrPtr.
*       numOfBufs           - Length of packetBuffsArrPtr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_NO_RESOURCE if there is not enough desc. for enqueuing the packet.
*       GT_FAIL otherwise.
*
* COMMENTS:
*       Support both regular and extended DSA Tag.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfMiiTxPacketSend
(
    IN GT_U8                        devNum,
    IN CPSS_DXCH_NET_TX_PARAMS_STC *pcktParamsPtr,
    IN GT_U8                       *packetBuffsArrPtr[],
    IN GT_U32                       buffLenArr[],
    IN GT_U32                       numOfBufs
)
{
    GT_STATUS rc = GT_OK;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(pcktParamsPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetBuffsArrPtr);
    CPSS_NULL_PTR_CHECK_MAC(buffLenArr);

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->netIf.initDone == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }
    else if (PRV_CPSS_PP_MAC(devNum)->cpuPortMode != CPSS_NET_CPU_PORT_MODE_MII_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }
    rc = prvCpssGenNetIfMiiTxModeSet(PRV_CPSS_GEN_NETIF_MII_TX_MODE_ASYNCH_E);
    if (rc != GT_OK)
      return rc;

    return prvCpssDxChNetIfMiiPacketTx(devNum, pcktParamsPtr, packetBuffsArrPtr,
                                       buffLenArr, numOfBufs);
}

/*******************************************************************************
* cpssDxChNetIfMiiTxPacketSend
*
* DESCRIPTION:
*       This function receives packet buffers & parameters from Application,
*       prepares them for the transmit operation, and
*       transmits a packet through the ethernet port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - Device number.
*       pcktParamsPtr       - The internal packet params to be set into the packet
*                             descriptors.
*       packetBuffsArrPtr   - (pointer to)The packet data buffers list.
*       buffLenArr          - A list of the buffers len in packetBuffsArrPtr.
*       numOfBufs           - Length of packetBuffsArrPtr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_NO_RESOURCE if there is not enough desc. for enqueuing the packet.
*       GT_FAIL otherwise.
*
* COMMENTS:
*       Support both regular and extended DSA Tag.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfMiiTxPacketSend
(
    IN GT_U8                        devNum,
    IN CPSS_DXCH_NET_TX_PARAMS_STC *pcktParamsPtr,
    IN GT_U8                       *packetBuffsArrPtr[],
    IN GT_U32                       buffLenArr[],
    IN GT_U32                       numOfBufs
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfMiiTxPacketSend);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, pcktParamsPtr, packetBuffsArrPtr, buffLenArr, numOfBufs));

    rc = internal_cpssDxChNetIfMiiTxPacketSend(devNum, pcktParamsPtr, packetBuffsArrPtr, buffLenArr, numOfBufs);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, pcktParamsPtr, packetBuffsArrPtr, buffLenArr, numOfBufs));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChNetIfMiiSyncTxPacketSend
*
* DESCRIPTION:
*       This function receives packet buffers & parameters from Application,
*       prepares them for the transmit operation, and
*       transmits a packet through the ethernet port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - Device number.
*       pcktParamsPtr       - The internal packet params to be set into the packet
*                             descriptors.
*       packetBuffsArrPtr   - (pointer to)The packet data buffers list.
*       buffLenArr          - A list of the buffers len in packetBuffsArrPtr.
*       numOfBufs           - Length of packetBuffsArrPtr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_NO_RESOURCE if there is not enough desc. for enqueuing the packet.
*       GT_FAIL otherwise.
*
* COMMENTS:
*       Support both regular and extended DSA Tag.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfMiiSyncTxPacketSend
(
    IN GT_U8                        devNum,
    IN CPSS_DXCH_NET_TX_PARAMS_STC *pcktParamsPtr,
    IN GT_U8                       *packetBuffsArrPtr[],
    IN GT_U32                       buffLenArr[],
    IN GT_U32                       numOfBufs
)
{
    GT_STATUS rc = GT_OK;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(pcktParamsPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetBuffsArrPtr);
    CPSS_NULL_PTR_CHECK_MAC(buffLenArr);

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->netIf.initDone == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }
    else if (PRV_CPSS_PP_MAC(devNum)->cpuPortMode != CPSS_NET_CPU_PORT_MODE_MII_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }
    rc = prvCpssGenNetIfMiiTxModeSet(PRV_CPSS_GEN_NETIF_MII_TX_MODE_SYNCH_E);
    if (rc != GT_OK)
      return rc;

    return prvCpssDxChNetIfMiiPacketTx(devNum, pcktParamsPtr, packetBuffsArrPtr,
                                       buffLenArr, numOfBufs);
}

/*******************************************************************************
* cpssDxChNetIfMiiSyncTxPacketSend
*
* DESCRIPTION:
*       This function receives packet buffers & parameters from Application,
*       prepares them for the transmit operation, and
*       transmits a packet through the ethernet port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - Device number.
*       pcktParamsPtr       - The internal packet params to be set into the packet
*                             descriptors.
*       packetBuffsArrPtr   - (pointer to)The packet data buffers list.
*       buffLenArr          - A list of the buffers len in packetBuffsArrPtr.
*       numOfBufs           - Length of packetBuffsArrPtr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_NO_RESOURCE if there is not enough desc. for enqueuing the packet.
*       GT_FAIL otherwise.
*
* COMMENTS:
*       Support both regular and extended DSA Tag.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfMiiSyncTxPacketSend
(
    IN GT_U8                        devNum,
    IN CPSS_DXCH_NET_TX_PARAMS_STC *pcktParamsPtr,
    IN GT_U8                       *packetBuffsArrPtr[],
    IN GT_U32                       buffLenArr[],
    IN GT_U32                       numOfBufs
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfMiiSyncTxPacketSend);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, pcktParamsPtr, packetBuffsArrPtr, buffLenArr, numOfBufs));

    rc = internal_cpssDxChNetIfMiiSyncTxPacketSend(devNum, pcktParamsPtr, packetBuffsArrPtr, buffLenArr, numOfBufs);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, pcktParamsPtr, packetBuffsArrPtr, buffLenArr, numOfBufs));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChNetIfMiiTxBufferQueueGet
*
* DESCRIPTION:
*       This routine returns the caller the TxEnd parameters for a transmitted
*       packet.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       hndl        - Handle got from cpssEventBind.
*
* OUTPUTS:
*       devPtr      - The device the packet was transmitted from.
*       cookiePtr   - The user cookie handed on the transmit request.
*       queuePtr    - The queue from which this packet was transmitted
*       statusPtr   - GT_OK if packet transmission was successful, GT_FAIL on
*                     packet reject.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfMiiTxBufferQueueGet
(
    IN GT_UINTPTR            hndl,
    OUT GT_U8               *devPtr,
    OUT GT_PTR              *cookiePtr,
    OUT GT_U8               *queuePtr,
    OUT GT_STATUS           *statusPtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(devPtr);
    CPSS_NULL_PTR_CHECK_MAC(cookiePtr);
    CPSS_NULL_PTR_CHECK_MAC(queuePtr);
    CPSS_NULL_PTR_CHECK_MAC(statusPtr);

    return prvCpssGenNetIfMiiTxBufferQueueGet(hndl, devPtr, cookiePtr, queuePtr,
                                                statusPtr);
}

/*******************************************************************************
* cpssDxChNetIfMiiTxBufferQueueGet
*
* DESCRIPTION:
*       This routine returns the caller the TxEnd parameters for a transmitted
*       packet.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       hndl        - Handle got from cpssEventBind.
*
* OUTPUTS:
*       devPtr      - The device the packet was transmitted from.
*       cookiePtr   - The user cookie handed on the transmit request.
*       queuePtr    - The queue from which this packet was transmitted
*       statusPtr   - GT_OK if packet transmission was successful, GT_FAIL on
*                     packet reject.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfMiiTxBufferQueueGet
(
    IN GT_UINTPTR            hndl,
    OUT GT_U8               *devPtr,
    OUT GT_PTR              *cookiePtr,
    OUT GT_U8               *queuePtr,
    OUT GT_STATUS           *statusPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfMiiTxBufferQueueGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, hndl, devPtr, cookiePtr, queuePtr, statusPtr));

    rc = internal_cpssDxChNetIfMiiTxBufferQueueGet(hndl, devPtr, cookiePtr, queuePtr, statusPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, hndl, devPtr, cookiePtr, queuePtr, statusPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfMiiRxPacketGet
*
* DESCRIPTION:
*       This function returns packets from PP destined to the CPU port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number throw which packet was received.
*       queueIdx    - The queue from which this packet was received (APPLICABLE RANGES: 0..7).
*       numOfBuffPtr- Num of buffs in packetBuffsArrPtr.
*
* OUTPUTS:
*       numOfBuffPtr        - Num of used buffs in packetBuffsArrPtr.
*       packetBuffsArrPtr   - (pointer to) The received packet buffers list.
*       buffLenArr          - List of buffer lengths for packetBuffsArrPtr.
*       rxParamsPtr         - (Pointer to)information parameters of received packets
*
* RETURNS:
*       GT_NO_MORE  - no more packets on the device/queue
*       GT_OK       - packet got with no error.
*       GT_FAIL     - failed to get the packet
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_BAD_PARAM - wrong devNum,queueIdx.
*       GT_DSA_PARSING_ERROR - DSA tag parsing error.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfMiiRxPacketGet
(
    IN GT_U8                            devNum,
    IN GT_U8                            queueIdx,
    INOUT GT_U32                        *numOfBuffPtr,
    OUT GT_U8                           *packetBuffsArrPtr[],
    OUT GT_U32                          buffLenArr[],
    OUT CPSS_DXCH_NET_RX_PARAMS_STC     *rxParamsPtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(queueIdx);
    CPSS_NULL_PTR_CHECK_MAC(numOfBuffPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetBuffsArrPtr);
    CPSS_NULL_PTR_CHECK_MAC(buffLenArr);
    CPSS_NULL_PTR_CHECK_MAC(rxParamsPtr);

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->netIf.initDone == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }
    else if (PRV_CPSS_PP_MAC(devNum)->cpuPortMode != CPSS_NET_CPU_PORT_MODE_MII_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return prvCpssDxChNetIfMiiRxGet(devNum, queueIdx, numOfBuffPtr,
                                    packetBuffsArrPtr, buffLenArr,rxParamsPtr);
}

/*******************************************************************************
* cpssDxChNetIfMiiRxPacketGet
*
* DESCRIPTION:
*       This function returns packets from PP destined to the CPU port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number throw which packet was received.
*       queueIdx    - The queue from which this packet was received (APPLICABLE RANGES: 0..7).
*       numOfBuffPtr- Num of buffs in packetBuffsArrPtr.
*
* OUTPUTS:
*       numOfBuffPtr        - Num of used buffs in packetBuffsArrPtr.
*       packetBuffsArrPtr   - (pointer to) The received packet buffers list.
*       buffLenArr          - List of buffer lengths for packetBuffsArrPtr.
*       rxParamsPtr         - (Pointer to)information parameters of received packets
*
* RETURNS:
*       GT_NO_MORE  - no more packets on the device/queue
*       GT_OK       - packet got with no error.
*       GT_FAIL     - failed to get the packet
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_BAD_PARAM - wrong devNum,queueIdx.
*       GT_DSA_PARSING_ERROR - DSA tag parsing error.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfMiiRxPacketGet
(
    IN GT_U8                            devNum,
    IN GT_U8                            queueIdx,
    INOUT GT_U32                        *numOfBuffPtr,
    OUT GT_U8                           *packetBuffsArrPtr[],
    OUT GT_U32                          buffLenArr[],
    OUT CPSS_DXCH_NET_RX_PARAMS_STC     *rxParamsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfMiiRxPacketGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, queueIdx, numOfBuffPtr, packetBuffsArrPtr, buffLenArr, rxParamsPtr));

    rc = internal_cpssDxChNetIfMiiRxPacketGet(devNum, queueIdx, numOfBuffPtr, packetBuffsArrPtr, buffLenArr, rxParamsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, queueIdx, numOfBuffPtr, packetBuffsArrPtr, buffLenArr, rxParamsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfMiiRxBufFree
*
* DESCRIPTION:
*       Frees a list of buffers, that where previously passed to the upper layer
*       in an Rx event.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number throw which these buffers where
*                     received.
*       rxQueue     - The Rx queue number throw which these buffers where
*                     received (APPLICABLE RANGES: 0..7).
*       rxBuffList  - List of Rx buffers to be freed.
*       buffListLen - Length of rxBufList.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfMiiRxBufFree
(
    IN GT_U8    devNum,
    IN GT_U8    rxQueue,
    IN GT_U8    *rxBuffList[],
    IN GT_U32   buffListLen
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(rxBuffList);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(rxQueue);

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->netIf.initDone == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }
    else if (PRV_CPSS_PP_MAC(devNum)->cpuPortMode != CPSS_NET_CPU_PORT_MODE_MII_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return prvCpssGenNetIfMiiRxBufFree(devNum, rxQueue, rxBuffList, buffListLen);
}

/*******************************************************************************
* cpssDxChNetIfMiiRxBufFree
*
* DESCRIPTION:
*       Frees a list of buffers, that where previously passed to the upper layer
*       in an Rx event.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number throw which these buffers where
*                     received.
*       rxQueue     - The Rx queue number throw which these buffers where
*                     received (APPLICABLE RANGES: 0..7).
*       rxBuffList  - List of Rx buffers to be freed.
*       buffListLen - Length of rxBufList.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfMiiRxBufFree
(
    IN GT_U8    devNum,
    IN GT_U8    rxQueue,
    IN GT_U8    *rxBuffList[],
    IN GT_U32   buffListLen
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfMiiRxBufFree);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, rxQueue, rxBuffList, buffListLen));

    rc = internal_cpssDxChNetIfMiiRxBufFree(devNum, rxQueue, rxBuffList, buffListLen);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, rxQueue, rxBuffList, buffListLen));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfMiiInit
*
* DESCRIPTION:
*       Initialize the network interface MII/RGMII Ethernet port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - Device number.
*       miiInitPtr   - (Pointer to) initialization parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on fail
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_CPU_MEM        - on memory allocation failure
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Only single device can be connected to CPU by ethernet port, therefor
*          this API must be called only once.
*       2. The application must call only one of the following APIs per device:
*          cpssDxChNetIfMiiInit - for MII/RGMII Ethernet port networkIf initialization.
*          cpssDxChNetIfInit - for SDMA networkIf initialization.
*          In case more than one of the mentioned above API is called
*          GT_FAIL will be return.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfMiiInit
(
    IN GT_U8                        devNum,
    IN CPSS_DXCH_NETIF_MII_INIT_STC *miiInitPtr
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(miiInitPtr);

    if (PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_BOBCAT2_SGMII_TO_CPU_NOT_SUPPORTED_WA_E))
    {
        /* FE-5613945 -
           Internal SGMII connection between switch and CPU is not operational */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    if ((miiInitPtr->rxBufBlockPtr == NULL) ||
        (miiInitPtr->rxBufBlockSize == 0) ||
        (miiInitPtr->txInternalBufBlockPtr == NULL) ||
        (miiInitPtr->txInternalBufBlockSize == 0))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* check if netIf already initialized */
    if(PRV_CPSS_DXCH_PP_MAC(devNum)->netIf.initDone == GT_TRUE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChNetIfMiiInitNetIfDev(devNum,miiInitPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_PP_MAC(devNum)->cpuPortMode = CPSS_NET_CPU_PORT_MODE_MII_E;
    PRV_CPSS_DXCH_PP_MAC(devNum)->netIf.initDone = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfMiiInit
*
* DESCRIPTION:
*       Initialize the network interface MII/RGMII Ethernet port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - Device number.
*       miiInitPtr   - (Pointer to) initialization parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on fail
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_CPU_MEM        - on memory allocation failure
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Only single device can be connected to CPU by ethernet port, therefor
*          this API must be called only once.
*       2. The application must call only one of the following APIs per device:
*          cpssDxChNetIfMiiInit - for MII/RGMII Ethernet port networkIf initialization.
*          cpssDxChNetIfInit - for SDMA networkIf initialization.
*          In case more than one of the mentioned above API is called
*          GT_FAIL will be return.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfMiiInit
(
    IN GT_U8                        devNum,
    IN CPSS_DXCH_NETIF_MII_INIT_STC *miiInitPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfMiiInit);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, miiInitPtr));

    rc = internal_cpssDxChNetIfMiiInit(devNum, miiInitPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, miiInitPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
