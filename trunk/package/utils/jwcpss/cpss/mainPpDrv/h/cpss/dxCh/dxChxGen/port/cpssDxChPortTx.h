/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortTx.h
*
* DESCRIPTION:
*       CPSS implementation for configuring the Physical Port Tx Traffic Class
*       Queues.
*       This covers:
*       - queuing enable/disable
*       - transmit enable/disable
*       - scheduling algorithms, bandwidth management
*       - shaping on queues and the logical port
*       - drop algorithms for congestion handling
*
* FILE REVISION NUMBER:
*       $Revision: 62 $
*******************************************************************************/

#ifndef __cpssDxChPortTxh
#define __cpssDxChPortTxh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/port/cpssPortTx.h>
#include <cpss/generic/cos/cpssCosTypes.h>

#define PRV_CPSS_DXCH_TXQREV1_TOKEN_BCKT_UPD_RATE_MIN_CNS        1
#define PRV_CPSS_DXCH_TXQREV1_TOKEN_BCKT_UPD_RATE_MAX_CNS        15
#define PRV_CPSS_DXCH_TXQREV1_SLOW_TOKEN_BCKT_UPD_RATIO_MIN_CNS  1
#define PRV_CPSS_DXCH_TXQREV1_SLOW_TOKEN_BCKT_UPD_RATIO_MAX_CNS  16
#define PRV_CPSS_DXCH_TXQREV1_TOKENS_REFILL_MAX_CNS              0xFFF

/*
 * typedef: enum DXCH_PORT_TX_TOKEN_BUCKET_MTU_ENT
 *
 * Description:  Token bucket maximum transmission unit (MTU)
 *
 * Enumerations:
 *      CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_1_5K_E  - 1.5K
 *      CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_2K_E    - 2K
 *      CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_10K_E   - 10K
 */

typedef enum
{
    CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_1_5K_E,
    CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_2K_E,
    CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_10K_E
}CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_ENT;

/*
 * typedef: CPSS_DXCH_PORT_TX_SHARED_POLICY_ENT
 *
 * Description:  Enumeration of shared pool allocation policy
 *
 * Enumerations:
 *     CPSS_DXCH_PORT_TX_SHARED_POLICY_UNCONSTRAINED_E -
 *          Unconstrained. A port may consume the entire shared pool.
 *     CPSS_DXCH_PORT_TX_SHARED_POLICY_CONSTRAINED_E  -
 *          Constrained. The Port Tail Drop Threshold limits the total
 *          number of resources occupied by a port.
 */

typedef enum
{
    CPSS_DXCH_PORT_TX_SHARED_POLICY_UNCONSTRAINED_E,
    CPSS_DXCH_PORT_TX_SHARED_POLICY_CONSTRAINED_E
}CPSS_DXCH_PORT_TX_SHARED_POLICY_ENT;

/*
 * typedef: CPSS_DXCH_PORT_TX_SHAPER_GRANULARITY_ENT
 *
 * Description:  Enumeration of shaper granularity.
 *
 * Enumerations:
 *     CPSS_DXCH_PORT_TX_SHAPER_GRANULARITY_64_CORE_CLOCKS_E -
 *          Granularity is 64 core clock cycles.
 *              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
 *     CPSS_DXCH_PORT_TX_SHAPER_GRANULARITY_1024_CORE_CLOCKS_E  -
 *          Granularity is 1024 core clock cycles.
 *     CPSS_DXCH_PORT_TX_SHAPER_GRANULARITY_128_CORE_CLOCKS_E  -
 *          Granularity is 128 core clock cycles.
 *              (APPLICABLE DEVICES: Bobcat2)
 *     CPSS_DXCH_PORT_TX_SHAPER_GRANULARITY_256_CORE_CLOCKS_E  -
 *          Granularity is 256 core clock cycles.
 *              (APPLICABLE DEVICES: Bobcat2)
 */

typedef enum
{
    CPSS_DXCH_PORT_TX_SHAPER_GRANULARITY_64_CORE_CLOCKS_E,
    CPSS_DXCH_PORT_TX_SHAPER_GRANULARITY_1024_CORE_CLOCKS_E,
    CPSS_DXCH_PORT_TX_SHAPER_GRANULARITY_128_CORE_CLOCKS_E,
    CPSS_DXCH_PORT_TX_SHAPER_GRANULARITY_256_CORE_CLOCKS_E
}CPSS_DXCH_PORT_TX_SHAPER_GRANULARITY_ENT;


/*
 * typedef: struct CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC
 *
 * Description: Shaper configurations.
 *
 * Fields:
 *       tokensRate - tokens rate for shapers(1..15).
 *                          Granularity of this field defined by portsTokensRateGran.
 *       slowRateRatio - slow rate ratio. Tokens Update rate for ports with
 *                           slow rate divided to the ratio (1..16).
 *       tokensRateGran - Defines the granularity tokensRate.
 *       portsPacketLength - packet length in bytes for decreasing the shaper token
 *                      bucket in packet based mode for non-CPU ports (0..0xFFFFFF).
 *       cpuPacketLength - packet length in bytes for decreasing the
 *              shaper token bucket in packet based mode for CPU port (0..0xFFFFFF).
 *
 * Comments:
 *      None.
 */
typedef struct {
    GT_U32 tokensRate ;
    GT_U32 slowRateRatio ;
    CPSS_DXCH_PORT_TX_SHAPER_GRANULARITY_ENT tokensRateGran;
    GT_U32 portsPacketLength;
    GT_U32 cpuPacketLength;
} CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC;

/*
 * typedef: CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_ENT
 *
 * Description:  Enumeration of how packet buffer consumption is
 *               calculated for Tail Drop and Scheduling.
 *
 * Enumerations:
 *     CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_BYTE_E -
 *          The packet’s buffer count is the actual number of buffers
 *          occupied by the received packet.
 *     CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_PACKET_E  -
 *          The packet’s buffer count is a fixed configurable value.
 */

typedef enum
{
    CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_BYTE_E,
    CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_PACKET_E
}CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_ENT;

/*
 * typedef: enum CPSS_PORT_TX_SHARED_DP_MODE_ENT
 *
 * Description:  Drop Precedence (DP) enable mode to use the shared pool.
 *
 * Enumerations:
 *      CPSS_PORT_TX_SHARED_DP_MODE_DISABLE_E      -
 *                   None of the packets (with any DP) can use the shared pool.
 *                   Compatible with DxCh3, xCat and xCat3 Disable mode.
 *      CPSS_PORT_TX_SHARED_DP_MODE_ALL_E       -
 *                   All packets (with any DP) can use the shared pool.
 *                   Compatible with DxCh3, xCat and xCat3 Enable mode.
 *      CPSS_PORT_TX_SHARED_DP_MODE_DP0_E       -
 *                   Only packets with DP0 can use the shared pool.
 *      CPSS_PORT_TX_SHARED_DP_MODE_DP0_DP1_E   -
 *                   Packets with DP0 and DP1 can use the shared pool.
 */
typedef enum
{
    CPSS_PORT_TX_SHARED_DP_MODE_DISABLE_E = GT_FALSE,
    CPSS_PORT_TX_SHARED_DP_MODE_ALL_E = GT_TRUE,
    CPSS_PORT_TX_SHARED_DP_MODE_DP0_E,
    CPSS_PORT_TX_SHARED_DP_MODE_DP0_DP1_E
}CPSS_PORT_TX_SHARED_DP_MODE_ENT;

/*
 * typedef: enum CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_ENT
 *
 * Description: Enumeration of BC (Byte Count) Change enable options.
 *
 * Enumerations:
 *  CPSS_DXCH_PORT_TX_BC_CHANGE_DISABLE_ALL_E  -
 *     BC change is disabled.
 *  CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_SHAPER_ONLY_E -
 *     BC change is enabled for shaper only.
 *  CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_SCHEDULER_ONLY_E -
 *     BC change is enabled for scheduler only.
 *  CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_SHAPER_AND_SCHEDULER_E -
 *     BC change is enabled for both shaper and scheduler.
 *
 * Comments:
 *         None.
 */
typedef enum
{
    CPSS_DXCH_PORT_TX_BC_CHANGE_DISABLE_ALL_E,
    CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_SHAPER_ONLY_E,
    CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_SCHEDULER_ONLY_E,
    CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_SHAPER_AND_SCHEDULER_E
}CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_ENT;


/*
 * typedef: enum CPSS_PORT_TX_TAIL_DROP_WRTD_ENABLERS_STC
 *
 * Description:  Tail Drop limits enabling for Weigthed Random Tail Drop.
 *
 * Fields:
 *      tcDpLimit       - enable/disable WRTD for {Queue,DP} descriptor/buffer
 *                        limits. (APPLICABLE DEVICES: Lion; xCat2)
 *      portLimit       - enable/disable Port descriptor/buffer limits.
 *                        (APPLICABLE DEVICES: Lion; xCat2)
 *      tcLimit         - enable/disable Queue descriptor/buffer limits.
 *                        (APPLICABLE DEVICES: Lion)
 *      sharedPoolLimit - enable/disable Shared Pool descriptor/buffer limits.
 *                        (APPLICABLE DEVICES: Lion; xCat2)
 */
typedef struct
{
   GT_BOOL  tcDpLimit;
   GT_BOOL  portLimit;
   GT_BOOL  tcLimit;
   GT_BOOL  sharedPoolLimit;
}CPSS_PORT_TX_TAIL_DROP_WRTD_ENABLERS_STC;

/**********************************************************************
 * typedef: CPSS_PORT_TX_WRTD_MASK_LSB_STC
 *
 * Description: The Tail Drop WRTD masked lsbs' for each limit type.
 *              (APPLICABLE RANGES: Lion 0..9; xCat2 0..14)
 *
 * Fields:
 *   tcDp   - The number of lsbs' masked for WRTD for {Queue,DP} limits.
 *            (APPLICABLE DEVICES: Lion; xCat2)
 *   port   - The number of lsbs' masked for WRTD for Port limits.
 *            (APPLICABLE DEVICES: Lion; xCat2)
 *   tc     - The number of lsbs' masked for WRTD for Queue limits.
 *            (APPLICABLE DEVICES: Lion)
 *   pool   - The number of lsbs' masked for WRTD for Shared Pool limits.
 *            (APPLICABLE DEVICES: Lion; xCat2)
 *
 *********************************************************************/
typedef struct
{
    GT_U32  tcDp;
    GT_U32  port;
    GT_U32  tc;
    GT_U32  pool;
} CPSS_PORT_TX_WRTD_MASK_LSB_STC;

/**********************************************************************
 * typedef: CPSS_PORT_TX_MC_FIFO_ARBITER_WEIGHTS_STC
 *
 * Description: The Multicast arbiter weights configured to the
 *              Multicast FIFOs.
 *             (APPLICABLE DEVICES: Lion2; Bobcat2; Caelum; Bobcat3)
 *
 * Fields:
 *   mcFifo0  - the weight given to Multicast descriptors forwarded
 *              to MC FIFO 0.
 *              (APPLICABLE RANGES: 0..255)
 *   mcFifo1  - the weight given to Multicast descriptors forwarded
 *              to MC FIFO 1.
 *              (APPLICABLE RANGES: 0..255)
 *   mcFifo2  - the weight given to Multicast descriptors forwarded
 *              to MC FIFO 2.
 *              (APPLICABLE RANGES: 0..255)
 *              (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *   mcFifo3  - the weight given to Multicast descriptors forwarded
 *              to MC FIFO 3.
 *              (APPLICABLE RANGES: 0..255)
 *              (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 *********************************************************************/
typedef struct
{
    GT_U32  mcFifo0;
    GT_U32  mcFifo1;
    GT_U32  mcFifo2;
    GT_U32  mcFifo3;
} CPSS_PORT_TX_MC_FIFO_ARBITER_WEIGHTS_STC;

/*******************************************************************************
* cpssDxChPortTxInit
*
* DESCRIPTION:
*       Init port Tx configuration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - physical device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In SALSA family the first profile (profile_0) is used for CPU port and
*       the second one (profile_1) for network ports.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxInit
(
    IN  GT_U8    devNum
);

/*******************************************************************************
* cpssDxChPortTxQueueEnableSet
*
* DESCRIPTION:
*       Enable/Disable enqueuing on all Tx queues on a specific device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - physical device number
*       enable - GT_TRUE, Global enqueuing enabled
*                GT_FALSE, Global enqueuing disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQueueEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   enable
);

/*******************************************************************************
* cpssDxChPortTxQueueEnableGet
*
* DESCRIPTION:
*       Get status of enqueuing on all Tx queues on a specific device
*       (enabled/disabled).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       enablePtr - GT_TRUE, Global enqueuing enabled
*                  GT_FALSE, Global enqueuing disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQueueEnableGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr
);

/*******************************************************************************
* cpssDxChPortTxQueueingEnableSet
*
* DESCRIPTION:
*       Enable/Disable enqueuing to a Traffic Class queue
*       on the specified port of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical or CPU port number
*       tcQueue - traffic class queue on this device (0..7)
*       enable  - GT_TRUE, enable enqueuing to the queue
*                 GT_FALSE, disable enqueuing to the queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQueueingEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    IN  GT_U8     tcQueue,
    IN  GT_BOOL   enable
);

/*******************************************************************************
* cpssDxChPortTxQueueingEnableGet
*
* DESCRIPTION:
*       Get the status of enqueuing to a Traffic Class queue
*       on the specified port of specified device (Enable/Disable).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical or CPU port number
*       tcQueue - traffic class queue on this device (0..7)
*
* OUTPUTS:
*       enablePtr  - GT_TRUE, enable enqueuing to the queue
*                    GT_FALSE, disable enqueuing to the queue
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQueueingEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    IN  GT_U8     tcQueue,
    OUT GT_BOOL   *enablePtr
);

/*******************************************************************************
* cpssDxChPortTxQueueTxEnableSet
*
* DESCRIPTION:
*       Enable/Disable transmission from a Traffic Class queue
*       on the specified port of specified device.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical or CPU port number
*       tcQueue - traffic class queue on this Physical Port (0..7)
*       enable  - GT_TRUE, enable transmission from the queue
*                GT_FALSE, disable transmission from the queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQueueTxEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    IN  GT_U8     tcQueue,
    IN  GT_BOOL   enable
);

/*******************************************************************************
* cpssDxChPortTxQueueTxEnableGet
*
* DESCRIPTION:
*       Get the status of transmission from a Traffic Class queue
*       on the specified port of specified device (Enable/Disable).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       tcQueue - traffic class queue on this Physical Port (0..7)
*
* OUTPUTS:
*       enablePtr  - GT_TRUE, enable transmission from the queue
*                    GT_FALSE, disable transmission from the queue
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQueueTxEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    IN  GT_U8     tcQueue,
    OUT GT_BOOL   *enablePtr
);

/*******************************************************************************
* cpssDxChPortTxFlushQueuesSet
*
* DESCRIPTION:
*       Flush all the traffic class queues on the specified port of
*       specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - physical device number
*       portNum- physical or CPU port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxFlushQueuesSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum
);

/*******************************************************************************
* cpssDxChPortTxShaperEnableSet
*
* DESCRIPTION:
*       Enable/Disable Token Bucket rate shaping on specified port of
*       specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical or CPU port number
*       enable  - GT_TRUE, enable Shaping
*                 GT_FALSE, disable Shaping
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Flow control must be disabled before disabling the shaper.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    IN  GT_BOOL   enable
);

/*******************************************************************************
* cpssDxChPortTxShaperEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable Token Bucket rate shaping status on specified port of
*       specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical or CPU port number
*
* OUTPUTS:
*       enablePtr - Pointer to Token Bucket rate shaping status.
*                   - GT_TRUE, enable Shaping
*                   - GT_FALSE, disable Shaping
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters in NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT  GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChPortTxShaperProfileSet
*
* DESCRIPTION:
*       Set Token Bucket Shaper Profile on specified port of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical or CPU port number
*       burstSize  - burst size in units of 4K bytes
*                    (max value is 4K which results in 16M burst size)
*       maxRatePtr - Requested Rate in Kbps or packets per second
*                    according to port shaper mode.
*                    Packet per second relevant only for Lion and above.
*
* OUTPUTS:
*       maxRatePtr   - (pointer to) the actual Rate value in Kbps
*                      or packets per second.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. For DxCh1, DxCh2, DxCh3, xCat and xCat2:
*       For port number in the range 0..23, whether G or XG type, the global
*       shaper used parameters are those referenced as gig ones -
*       "gigPortsTokensRate" and "gigPortsSlowRateRatio" inputs for
*       "cpssDxChPortTxShaperGlobalParamsSet" API.
*       2. For xCat-A3 and xCat2:
*       For port number in the range 24..27, whether G or XG type, the global
*       shaper used parameters are determined by
*       "cpssDxChPortTxShaperOnStackAsGigEnableSet".
*       3. For Lion:
*       If the shaper mode on a given port is packet based and the packet
*       length (portsPacketLength/cpuPacketLength) configuration isn't null,
*       maxRatePtr units are packets per second,
*       otherwise maxRatePtr units are Kbps.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperProfileSet
(
    IN    GT_U8     devNum,
    IN    GT_PHYSICAL_PORT_NUM  portNum,
    IN    GT_U16    burstSize,
    INOUT GT_U32    *maxRatePtr
);

/*******************************************************************************
* cpssDxChPortTxShaperProfileGet
*
* DESCRIPTION:
*       Get Token Bucket Shaper Profile on specified port of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical or CPU port number
*
* OUTPUTS:
*       burstSizePtr - (pointer to) burst size in units of 4K bytes
*       maxRatePtr   - (pointer to) the actual Rate value in Kbps
*                                   or packets per second.
*       enablePtr    - (pointer to) Token Bucket rate shaping status.
*                           - GT_TRUE - Token Bucket rate shaping is enabled.
*                           - GT_FALSE - Token Bucket rate shaping is disabled.
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. For Lion:
*          If the shaper mode on a given port is packet based and the packet
*          length (portsPacketLength/cpuPacketLength) configuration isn't null,
*          maxRatePtr units are packets per second,
*          otherwise maxRatePtr units are Kbps.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperProfileGet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_U16  *burstSizePtr,
    OUT GT_U32  *maxRatePtr,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChPortTxQShaperEnableSet
*
* DESCRIPTION:
*       Enable/Disable shaping of transmitted traffic from a specified Traffic
*       Class Queue and specified port of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical or CPU port number
*       tcQueue - traffic class queue on this port (0..7)
*       enable  - GT_TRUE, enable shaping on this traffic queue
*                 GT_FALSE, disable shaping on this traffic queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQShaperEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    IN  GT_U8     tcQueue,
    IN  GT_BOOL   enable
);

/*******************************************************************************
* cpssDxChPortTxQShaperEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable shaping status
*       of transmitted traffic from a specified Traffic
*       Class Queue and specified port of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical or CPU port number
*       tcQueue - traffic class queue on this port (0..7)
*
* OUTPUTS:
*       enablePtr  - Pointer to  shaping status
*                    - GT_TRUE, enable shaping on this traffic queue
*                    - GT_FALSE, disable shaping on this traffic queue
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQShaperEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    IN  GT_U8     tcQueue,
    OUT GT_BOOL   *enablePtr
);

/*******************************************************************************
* cpssDxChPortTxQShaperProfileSet
*
* DESCRIPTION:
*       Set Shaper Profile for Traffic Class Queue of specified port on
*       specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - physical device number
*       portNum      - physical or CPU port number
*       tcQueue      - traffic class queue on this Logical Port
*       burstSize    - burst size in units of 4K bytes
*                      (max value is 4K which results in 16M burst size)
*       maxRatePtr   - Requested Rate in Kbps or packets per second.
*
* OUTPUTS:
*       maxRatePtr   - (pointer to) the actual Rate value in Kbps or
*                       or packets per second.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. For DxCh1, DxCh2, DxCh3, xCat and xCat2:
*       For port number in the range 0..23, whether G or XG type, the global
*       shaper used parameters are those referenced as gig ones -
*       "gigPortsTokensRate" and "gigPortsSlowRateRatio" inputs for
*       "cpssDxChPortTxShaperGlobalParamsSet" API.
*       2. For xCat-A3 and xCat2:
*       For port number in the range 24..27, whether G or XG type, the global
*       shaper used parameters are determined by
*       "cpssDxChPortTxShaperOnStackAsGigEnableSet".
*       3. For Lion:
*       If the shaper mode on a given port is packet based and the packet
*       length (portsPacketLength/cpuPacketLength) configuration isn't null,
*       maxRatePtr units are packets per second,
*       otherwise maxRatePtr units are Kbps.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQShaperProfileSet
(
    IN    GT_U8     devNum,
    IN    GT_PHYSICAL_PORT_NUM  portNum,
    IN    GT_U8     tcQueue,
    IN    GT_U16    burstSize,
    INOUT GT_U32    *maxRatePtr
);

/*******************************************************************************
* cpssDxChPortTxQShaperProfileGet
*
* DESCRIPTION:
*       Get Shaper Profile for Traffic Class Queue of specified port on
*       specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical or CPU port number
*       tcQueue    - traffic class queue on the port
*
* OUTPUTS:
*       burstSizePtr - (pointer to) burst size in units of 4K bytes
*       maxRatePtr   - (pointer to) the actual Rate value in Kbps
*                                   or packets per second.
*       enablePtr    - (pointer to) Token Bucket rate shaping status.
*                           - GT_TRUE - Token Bucket rate shaping is enabled.
*                           - GT_FALSE - Token Bucket rate shaping is disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. For Lion:
*          If the shaper mode on a given port is packet based and the packet
*          length (portsPacketLength/cpuPacketLength) configuration isn't null,
*          maxRatePtr units are packets per second,
*          otherwise maxRatePtr units are Kbps.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQShaperProfileGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    IN  GT_U8    tcQueue,
    OUT GT_U16   *burstSizePtr,
    OUT GT_U32   *maxRatePtr,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChPortTxBindPortToSchedulerProfileSet
*
* DESCRIPTION:
*       Bind a port to scheduler profile set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical or CPU port number
*       profileSet - The Profile Set in which the scheduler's parameters are
*                    associated.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxBindPortToSchedulerProfileSet
(
    IN  GT_U8                                   devNum,
    IN  GT_PHYSICAL_PORT_NUM                    portNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet
);

/*******************************************************************************
* cpssDxChPortTxBindPortToSchedulerProfileGet
*
* DESCRIPTION:
*       Get scheduler profile set that is binded to the port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical or CPU port number
*
* OUTPUTS:
*       profileSetPtr - The Profile Set in which the scheduler's parameters are
*                        associated.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxBindPortToSchedulerProfileGet
(
    IN  GT_U8                                   devNum,
    IN  GT_PHYSICAL_PORT_NUM                    portNum,
    OUT CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  *profileSetPtr
);

/*******************************************************************************
* cpssDxChPortTx4TcTailDropProfileSet
*
* DESCRIPTION:
*       Set tail drop profiles limits for particular TC.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum             - physical device number
*       profileSet         - the Profile Set in which the Traffic
*                            Class Drop Parameters is associated.
*       trafficClass       - the Traffic Class associated with this set of
*                            Drop Parameters (0..7).
*       tailDropProfileParamsPtr -
*                       the Drop Profile Parameters to associate
*                       with the Traffic Class in this Profile set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, profile set or
*                                  traffic class
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTx4TcTailDropProfileSet
(
    IN    GT_U8                                   devNum,
    IN    CPSS_PORT_TX_DROP_PROFILE_SET_ENT       profileSet,
    IN    GT_U8                                   trafficClass,
    IN    CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS *tailDropProfileParamsPtr
);

/*******************************************************************************
* cpssDxChPortTx4TcTailDropProfileGet
*
* DESCRIPTION:
*       Get tail drop profiles limits for particular TC.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum             - physical device number
*       profileSet         - the Profile Set in which the Traffic
*                            Class Drop Parameters is associated.
*       trafficClass       - the Traffic Class associated with this set of
*                            Drop Parameters (0..7).
*
* OUTPUTS:
*       tailDropProfileParamsPtr - Pointer to
*                       the Drop Profile Parameters to associate
*                       with the Traffic Class in this Profile.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, profile set or
*                                  traffic class
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTx4TcTailDropProfileGet
(
    IN    GT_U8                                   devNum,
    IN    CPSS_PORT_TX_DROP_PROFILE_SET_ENT       profileSet,
    IN    GT_U8                                   trafficClass,
    OUT   CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS *tailDropProfileParamsPtr
);

/*******************************************************************************
* cpssDxChPortTxTailDropProfileSet
*
* DESCRIPTION:
*       Enables/Disables sharing of buffers and descriptors for all queues of an
*       egress port and set maximal port's limits of buffers and descriptors.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - physical device number
*       profileSet    - the Profile Set in which the Traffic
*                       Class Drop Parameters is associated
*       enableSharing - If GT_TRUE Sharing is enabled. If a packet is to be queued
*                      to an empty queue, it is queued even if the number of buffers
*                       or descriptors allocated to this port exceeds the
*                       < portMaxBuffLimit > and < portMaxDescrLimit >.
*                       If GT_FALSE Sharing is disabled. If the sum of buffers or
*                       descriptors allocated per queue is larger than the number of
*                       buffers or descriptors allocated to this port
*                       < portMaxBuffLimit > and < portMaxDescrLimit >, then some of
*                       the queues may be starved.
*                       DxCh2 and above devices support only GT_FALSE value.
*       portMaxBuffLimit  - maximal number of buffers for a port.
*                          (APPLICABLE RANGES: DxCh1,DxCh2 0..4095;
*                           DxCh3,xCat,xCat3,Lion,xCat2 0..16383; Bobcat2,Caelum,Bobcat3 0..0xFFFFF)
*       portMaxDescrLimit - maximal number of descriptors for a port.
*                          (APPLICABLE RANGES: DxCh1,DxCh2 0..4095;
*                           DxCh3,xCat,xCat3,Lion,xCat2 0..16383; Bobcat2,Caelum,Bobcat3 0..0xFFFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTailDropProfileSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet,
    IN  GT_BOOL                             enableSharing,
    IN  GT_U32                              portMaxBuffLimit,
    IN  GT_U32                              portMaxDescrLimit
);

/*******************************************************************************
* cpssDxChPortTxTailDropProfileGet
*
* DESCRIPTION:
*       Get sharing status of buffers and descriptors for all queues of an
*       egress port and get maximal port's limits of buffers and descriptors.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - physical device number
*       profileSet    - the Profile Set in which the Traffic
*                       Class Drop Parameters is associated
*
* OUTPUTS:
*       enableSharingPtr - Pointer to sharing status.
*                      If GT_TRUE Sharing is enabled. If a packet is to be queued
*                      to an empty queue, it is queued even if the number of buffers
*                       or descriptors allocated to this port exceeds the
*                       < portMaxBuffLimit > and < portMaxDescrLimit >.
*                       If GT_FALSE Sharing is disabled. If the sum of buffers or
*                       descriptors allocated per queue is larger than the number of
*                       buffers or descriptors allocated to this port
*                       < portMaxBuffLimit > and < portMaxDescrLimit >, then some of
*                       the queues may be starved.
*                       DxCh2 and above devices support only GT_FALSE value.
*       portMaxBuffLimitPtr - Pointer to maximal number of buffers for a port
*       portMaxDescrLimitPtr - Pointer to maximal number of descriptors for a port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTailDropProfileGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet,
    OUT GT_BOOL                             *enableSharingPtr,
    OUT GT_U32                              *portMaxBuffLimitPtr,
    OUT GT_U32                              *portMaxDescrLimitPtr
);

/*******************************************************************************
* cpssDxChPortTxSniffedPcktDescrLimitSet
*
* DESCRIPTION:
*       Set maximal descriptors limits for mirrored packets.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - physical device number
*       rxSniffMaxDescNum - The number of descriptors allocated for packets
*                           forwarded to the ingress analyzer port due to
*                           mirroring.
*                           For DxCh1,DxCh2: range 0..0xFFF
*                           For DxCh3,xCat,xCat3,xCat2: range 0..0x3FFF
*                           For Lion: range 0..0xFFFF
*       txSniffMaxDescNum - The number of descriptors allocated for packets
*                           forwarded to the egress analyzer port due to
*                           mirroring.
*                           For DxCh,DxCh2: range 0..0xFFF
*                           For DxCh3,xCat,xCat3,xCat2: range 0..0x3FFF
*                           For Lion: range 0..0xFFFF
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on out of range value
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSniffedPcktDescrLimitSet
(
    IN    GT_U8                  devNum,
    IN    GT_U32                 rxSniffMaxDescNum,
    IN    GT_U32                 txSniffMaxDescNum
);

/*******************************************************************************
* cpssDxChPortTxSniffedPcktDescrLimitGet
*
* DESCRIPTION:
*       Get maximal descriptors limits for mirrored packets.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - physical device number
*
* OUTPUTS:
*       rxSniffMaxDescNumPtr - (pointer to) The number of descriptors allocated
*                              for packets forwarded to the ingress analyzer
*                              port due to mirroring.
*       txSniffMaxDescNumPtr - (pointer to) The number of descriptors allocated
*                              for packets forwarded to the egress analyzer
*                              port due to mirroring.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSniffedPcktDescrLimitGet
(
    IN    GT_U8                  devNum,
    OUT   GT_U32                *rxSniffMaxDescNumPtr,
    OUT   GT_U32                *txSniffMaxDescNumPtr
);

/*******************************************************************************
* cpssDxChPortTxMcastPcktDescrLimitSet
*
* DESCRIPTION:
*       Set maximal descriptors limits for multicast packets.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - physical device number
*       mcastMaxDescNum   - The number of descriptors allocated for multicast
*                           packets.
*                           For all devices except Lion, Bobcat2, Caelum: in units of 128
*                           descriptors, the actual number descriptors
*                           allocated will be 128 * mcastMaxDescNum.
*                           For Lion, Bobcat2, Caelum: actual descriptors number (granularity
*                           of 1).
*                           For DxCh1,DxCh2,DxCh3,xCat,xCat3,xCat2: range 0..0xF80
*                           For Lion, Bobcat2, Caelum: range 0..0xFFFF
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxMcastPcktDescrLimitSet
(
    IN    GT_U8                  devNum,
    IN    GT_U32                 mcastMaxDescNum
);

/*******************************************************************************
* cpssDxChPortTxMcastPcktDescrLimitGet
*
* DESCRIPTION:
*       Get maximal descriptors limits for multicast packets.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - physical device number
*
* OUTPUTS:
*       mcastMaxDescNumPtr  - (pointer to) the number of descriptors allocated
*                             for multicast packets.
*                             For all devices except Lion, Bobcat2, Caelum: in units of 128
*                             descriptors, the actual number descriptors
*                             allocated will be 128 * mcastMaxDescNum.
*                             For Lion, Bobcat2, Caelum: actual descriptors number (granularity
*                             of 1).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxMcastPcktDescrLimitGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *mcastMaxDescNumPtr
);

/*******************************************************************************
* cpssDxChPortTxWrrGlobalParamSet
*
* DESCRIPTION:
*       Set global parameters for WRR scheduler.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - physical device number
*       wrrMode - Transmit Scheduler count mode.
*                 (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.)
*       wrrMtu  - MTU for DSWRR, resolution of WRR weights for byte based
*                 Transmit Scheduler count mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        For DxCh1, DxCh2, DxCh3, xCat and xCat2 only CPSS_PORT_TX_WRR_MTU_2K_E
*        and CPSS_PORT_TX_WRR_MTU_8K_E are supported.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxWrrGlobalParamSet
(
    IN    GT_U8                     devNum,
    IN    CPSS_PORT_TX_WRR_MODE_ENT wrrMode,
    IN    CPSS_PORT_TX_WRR_MTU_ENT  wrrMtu
);

/*******************************************************************************
* cpssDxChPortTxWrrGlobalParamGet
*
* DESCRIPTION:
*       Get global parameters for WRR scheduler.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       wrrModePtr - (pointer to) Transmit Scheduler count mode.
*       wrrMtuPtr  - (pointer to) MTU for DSWRR, resolution of WRR weights
*                    for byte based Transmit Scheduler count mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxWrrGlobalParamGet
(
    IN    GT_U8                      devNum,
    OUT   CPSS_PORT_TX_WRR_MODE_ENT *wrrModePtr,
    OUT   CPSS_PORT_TX_WRR_MTU_ENT  *wrrMtuPtr
);

/*******************************************************************************
* cpssDxChPortTxShaperGlobalParamsSet
*
* DESCRIPTION:
*       Set Global parameters for shaper.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum               - physical device number
*       xgPortsTokensRate    - tokens rate for XG ports' shapers (1..15)
*       gigPortsTokensRate   - tokens rate for Tri Speed ports' shapers.(1..15)
*       gigPortsSlowRateRatio - slow rate ratio for Tri Speed ports.
*                               Tokens Update rate for ports with slow rate
*                               divided to the ratio. (1..16)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperGlobalParamsSet
(
    IN    GT_U8                   devNum,
    IN    GT_U32                  xgPortsTokensRate,
    IN    GT_U32                  gigPortsTokensRate,
    IN    GT_U32                  gigPortsSlowRateRatio
);

/*******************************************************************************
* cpssDxChPortTxShaperGlobalParamsGet
*
* DESCRIPTION:
*       Get Global parameters for shaper.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*      devNum                   - device number
*
* OUTPUTS:
*       xgPortsTokensRatePtr     - (pointer to) tokens rate for XG ports' shapers
*       gigPortsTokensRatePtr    - (pointer to) tokens rate for Tri Speed ports' shapers
*       gigPortsSlowRateRatioPtr - (pointer to) slow rate ratio for Tri Speed ports.
*                                  Tokens Update rate for ports with slow rate divided
*                                  to the ratio.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperGlobalParamsGet
(
    IN    GT_U8          devNum,
    OUT   GT_U32         *xgPortsTokensRatePtr,
    OUT   GT_U32         *gigPortsTokensRatePtr,
    OUT   GT_U32         *gigPortsSlowRateRatioPtr
);

/*******************************************************************************
* cpssDxChPortTxQWrrProfileSet
*
* DESCRIPTION:
*       Set Weighted Round Robin profile on the specified port's
*       Traffic Class Queue.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       tcQueue    - traffic class queue on this Port (0..7)
*       wrrWeight  - proportion of bandwidth assigned to this queue
*                    relative to the other queues in this
*                    Arbitration Group - resolution is 1/255.
*       profileSet - the Tx Queue scheduler Profile Set in which the wrrWeight
*                    Parameter is associated.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, profile set or
*                                  traffic class
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT               - previous updated operation not yet completed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*         - if weight will be less then port's MTU (maximum transmit unit) there
*           is possibility for empty WRR loops for given TC queue, but to not
*           tight user and for future ASIC's where this problem will be solved
*           check of wrrWeight * 256 > MTU not implemented
*         - There is errata for Cheetah FEr#29. weigh cannot be = 255 for some
*           modes. The functions limits weight accordingly.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQWrrProfileSet
(
    IN  GT_U8                                   devNum,
    IN  GT_U8                                   tcQueue,
    IN  GT_U8                                   wrrWeight,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet
);

/*******************************************************************************
* cpssDxChPortTxQWrrProfileGet
*
* DESCRIPTION:
*       Get Weighted Round Robin profile on the specified port's
*       Traffic Class Queue.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       tcQueue    - traffic class queue on this Port (0..7)
*       profileSet - the Tx Queue scheduler Profile Set in which the wrrWeight
*                    Parameter is associated.
* OUTPUTS:
*       wrrWeightPtr  - Pointer to proportion of bandwidth assigned to this queue
*                       relative to the other queues in this
*                       Arbitration Group - resolution is 1/255.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, profile set or
*                                  traffic class
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT               - previous updated operation not yet completed
*       GT_BAD_PTR               - one of the parameters in NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQWrrProfileGet
(
    IN  GT_U8                                   devNum,
    IN  GT_U8                                   tcQueue,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    OUT  GT_U8                                  *wrrWeightPtr
);

/*******************************************************************************
* cpssDxChPortTxQArbGroupSet
*
* DESCRIPTION:
*       Set Traffic Class Queue scheduling arbitration group on
*       specificed profile of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       tcQueue    - traffic class queue (0..7)
*       arbGroup   - scheduling arbitration group:
*                     1) Strict Priority
*                     2) WRR Group 1
*                     3) WRR Group 0
*       profileSet - the Tx Queue scheduler Profile Set in which the arbGroup
*                    parameter is associated.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong profile number, device or arbGroup
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT               - previous updated operation not yet completed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQArbGroupSet
(
    IN  GT_U8                                   devNum,
    IN  GT_U8                                   tcQueue,
    IN  CPSS_PORT_TX_Q_ARB_GROUP_ENT            arbGroup,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet
);

/*******************************************************************************
* cpssDxChPortTxQArbGroupGet
*
* DESCRIPTION:
*       Get Traffic Class Queue scheduling arbitration group on
*       specificed profile of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       tcQueue    - traffic class queue (0..7)
*       profileSet - the Tx Queue scheduler Profile Set in which the arbGroup
*                    parameter is associated.
*
* OUTPUTS:
*       arbGroupPtr   - Pointer to scheduling arbitration group:
*                       1) Strict Priority
*                       2) WRR Group 1
*                       3) WRR Group 0
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong profile number, device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters in NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQArbGroupGet
(
    IN  GT_U8                                   devNum,
    IN  GT_U8                                   tcQueue,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    OUT CPSS_PORT_TX_Q_ARB_GROUP_ENT            *arbGroupPtr
);

/*******************************************************************************
* cpssDxChPortTxBindPortToDpSet
*
* DESCRIPTION:
*       Bind a Physical Port to a specific Drop Profile Set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - physical device number
*       portNum       - physical or CPU port number
*       profileSet    - the Profile Set in which the Traffic
*                       Class Drop Parameters is associated
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxBindPortToDpSet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT  profileSet
);

/*******************************************************************************
* cpssDxChPortTxBindPortToDpGet
*
* DESCRIPTION:
*       Get Drop Profile Set according to a Physical Port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical or CPU port number
*
* OUTPUTS:
*       profileSetPtr    - Pointer to the Profile Set in which the Traffic
*                          Class Drop Parameters is associated
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxBindPortToDpGet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    OUT CPSS_PORT_TX_DROP_PROFILE_SET_ENT   *profileSetPtr
);

/*******************************************************************************
* cpssDxChPortTxDescNumberGet
*
* DESCRIPTION:
*       Gets the current number of descriptors allocated per specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical or CPU port number
*
* OUTPUTS:
*       numberPtr - number of descriptors
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxDescNumberGet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_U16      *numberPtr
);

/*******************************************************************************
* cpssDxChPortTxBufNumberGet
*
* DESCRIPTION:
*       Gets the current number of buffers allocated per specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical or CPU port number
*
* OUTPUTS:
*       numPtr   - number of buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxBufNumberGet
(
    IN   GT_U8       devNum,
    IN   GT_PHYSICAL_PORT_NUM  portNum,
    OUT  GT_U16      *numPtr
);

/*******************************************************************************
* cpssDxChPortTx4TcDescNumberGet
*
* DESCRIPTION:
*       Gets the current number of descriptors allocated on specified port
*       for specified Traffic Class queues.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical or CPU port number
*       trafClass - trafiic class (0..7)
*
* OUTPUTS:
*       numberPtr  - (pointer to) the number of descriptors
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Note: Do not call this API under traffic for Lion devices
*
*******************************************************************************/
GT_STATUS cpssDxChPortTx4TcDescNumberGet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    IN  GT_U8       trafClass,
    OUT GT_U16      *numberPtr
);

/*******************************************************************************
* cpssDxChPortTx4TcBufNumberGet
*
* DESCRIPTION:
*       Gets the current number of buffers allocated on specified port
*       for specified Traffic Class queues.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical or CPU port number
*       trafClass - trafiic class (0..7)
*
* OUTPUTS:
*       numPtr    - (pointer to) the number of buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Note: Do not call this API under traffic for Lion devices
*
*******************************************************************************/
GT_STATUS cpssDxChPortTx4TcBufNumberGet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    IN  GT_U8       trafClass,
    OUT GT_U16      *numPtr
);

/*******************************************************************************
* cpssDxChPortTxToCpuShaperModeSet
*
* DESCRIPTION:
*       Set Shaper mode packet or byte based for CPU port shapers
*       Shapers are configured by cpssDxChPortTxShaperProfileSet or
*       cpssDxChPortTxQShaperProfileSet
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       mode    - shaper mode: byte count or packet number based ackets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxToCpuShaperModeSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_SHAPER_MODE_ENT   mode
);

/*******************************************************************************
* cpssDxChPortTxToCpuShaperModeGet
*
* DESCRIPTION:
*       Get Shaper mode packet or byte based for CPU port shapers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       modePtr - (pointer to) shaper mode: byte count or packet number based ackets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxToCpuShaperModeGet
(
    IN  GT_U8                               devNum,
    OUT CPSS_PORT_TX_DROP_SHAPER_MODE_ENT  *modePtr
);

/*******************************************************************************
* cpssDxChPortTxSharingGlobalResourceEnableSet
*
* DESCRIPTION:
*       Enable/Disable sharing of resources for enqueuing of packets.
*       The shared resources configuration is set by
*       cpssDxChPortTxSharedGlobalResourceLimitsSet
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       enable  - GT_TRUE   - enable TX queue resourses sharing
*                 GT_FALSE - disable TX queue resources sharing
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSharingGlobalResourceEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
);

/*******************************************************************************
* cpssDxChPortTxSharingGlobalResourceEnableGet
*
* DESCRIPTION:
*       Get enable/disable sharing of resources for enqueuing of packets.
*       The shared resources configuration is set by
*       cpssDxChPortTxSharedGlobalResourceLimitsSet
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE   - enable TX queue resourses sharing
*                    GT_FALSE - disable TX queue resources sharing
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSharingGlobalResourceEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL    *enablePtr
);

/*******************************************************************************
* cpssDxChPortTxSharedGlobalResourceLimitsSet
*
* DESCRIPTION:
*       Configuration of shared resources for enqueuing of packets.
*       The using of shared resources is set
*       by cpssDxChPortTxSharingGlobalResourceEnableSet
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum             - physical device number
*       sharedBufLimit     - The number of buffers in all Transmit queues that
*                            can be shared between eligible packets.
*                            For DxCh2: range 0..4095
*                            For DxCh3,xCat,xCat3,xCat2: range 0..16380
*       sharedDescLimit    - The number of descriptors that can be shared
*                            between all eligible packets.
*                            For DxCh2: range 0..4095
*                            For DxCh3,xCat,xCat3,xCat2: range 0..16380
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For DxCh3,xCat,xCat3,xCat2: sharedBufLimit and sharedDescLimit are rounded up
*       to the nearest multiple of 4 since the corresponding fields in the
*       Transmit Queue Resource Sharing register are in 4 buffers\descriptors
*       resolution.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSharedGlobalResourceLimitsSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  sharedBufLimit,
    IN  GT_U32  sharedDescLimit
);

/*******************************************************************************
* cpssDxChPortTxSharedGlobalResourceLimitsGet
*
* DESCRIPTION:
*       Get the configuration of shared resources for enqueuing of packets.
*       The using of shared resources is set
*       by cpssDxChPortTxSharingGlobalResourceEnableSet
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum             - physical device number
*
* OUTPUTS:
*       sharedBufLimitPtr  - (pointer to) The number of buffers in all
*                            Transmit queues that
*                            can be shared between eligible packets.
*       sharedDescLimitPtr - (pointer to) The number of descriptors that
*                            can be shared between all eligible packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSharedGlobalResourceLimitsGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *sharedBufLimitPtr,
    OUT GT_U32  *sharedDescLimitPtr
);

/*******************************************************************************
* cpssDxChPortTxSharedPolicySet
*
* DESCRIPTION:
*       Sets shared pool allocation policy for enqueuing of packets.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum     - device number.
*       policy     - shared pool allocation policy.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or wrong policy
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSharedPolicySet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PORT_TX_SHARED_POLICY_ENT  policy
);

/*******************************************************************************
* cpssDxChPortTxSharedPolicyGet
*
* DESCRIPTION:
*       Gets shared pool allocation policy for enqueuing of packets.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       policyPtr  - (pointer to) shared pool allocation policy.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSharedPolicyGet
(
    IN  GT_U8                                devNum,
    OUT CPSS_DXCH_PORT_TX_SHARED_POLICY_ENT  *policyPtr
);

/*******************************************************************************
* cpssDxChPortTxWatchdogEnableGet
*
* DESCRIPTION:
*       Get Tx Port Watchdog status and timeout
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical or CPU port number
*
* OUTPUTS:
*       enablePtr     - (pointer to) the enable / disable state
*       timeoutPtr    - (pointer to) the watchdog timeout
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - enablePtr or timeoutPtr is a null pointer
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxWatchdogEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_BOOL     *enablePtr,
    OUT GT_U16      *timeoutPtr
);

/*******************************************************************************
* cpssDxChPortTxWatchdogEnableSet
*
* DESCRIPTION:
*       Enable/Disable Watchdog on specified port of specified port of
*       specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical or CPU port number
*       enable  - GT_TRUE, enable Watchdog
*                 GT_FALSE, disable Watchdog
*       timeout - Watchdog timeout in milliseconds (1..4K ms)
*                 If disabling the Watchdog, this parameter is ignored
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxWatchdogEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    IN  GT_BOOL   enable,
    IN  GT_U16    timeout
);

/*******************************************************************************
* prvCpssDxChPortIpmBridgeCopyGetDroppedWaTailDropMaxLimitsSet
*
* DESCRIPTION:
*       Set tail drop max limit for WA of IPM Bridge Copy Dropped errata
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK    - success
*       GT_FAIL  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortIpmBridgeCopyGetDroppedWaTailDropMaxLimitsSet
(
    IN GT_U8 devNum
);

/*******************************************************************************
* prvCpssDxChPortIpmBridgeCopyGetDroppedWaTailDropLimitsRestore
*
* DESCRIPTION:
*       Restore tail drop limit for WA of IPM Bridge Copy Dropped errata
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK    - success
*       GT_FAIL  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortIpmBridgeCopyGetDroppedWaTailDropLimitsRestore
(
    IN GT_U8 devNum
);

/*******************************************************************************
* cpssDxChPortTxTailDropUcEnableSet
*
* DESCRIPTION:
*       Enable/Disable tail-dropping for all packets based on the profile limits.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   -  device number.
*       enable   -  GT_TRUE  - Tail Drop is enabled.
*                   GT_FALSE - The Tail Drop limits for all packets are
*                              ignored and packet is dropped only
*                              when the Tx Queue has reached its global
*                              descriptors limit.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTailDropUcEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
);

/*******************************************************************************
* cpssDxChPortTxTailDropUcEnableGet
*
* DESCRIPTION:
*       Get enable/disable tail-dropping for all packets based on the profile limits.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      -  device number.
*
* OUTPUTS:
*       enablePtr   -  pointer to tail drop status:
*                          GT_TRUE  - Tail Drop is enabled.
*                          GT_FALSE - The Tail Drop limits for all packets are
*                                     ignored and packet is dropped only
*                                     when the Tx Queue has reached its global
*                                     descriptors limit.
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTailDropUcEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChPortTxBufferTailDropEnableSet
*
* DESCRIPTION:
*       Enable/disable Tail Drop according to the number of buffers in the
*       queues.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - device number.
*       enable   -  GT_TRUE  -  Enables Tail Drop according to the number of
*                               buffers in the queues. Tail drop use both
*                               decsriptiors and buffers limits.
*                   GT_FALSE - Tail drop use only descriptor limits.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxBufferTailDropEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
);

/*******************************************************************************
* cpssDxChPortTxBufferTailDropEnableGet
*
* DESCRIPTION:
*       Get enable/disable Tail Drop status according to the number of buffers
*       in the queues.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number.
*
* OUTPUTS:
*       enablePtr   -  pointer to Tail Drop status:
*                       GT_TRUE  - Enables Tail Drop according to the number of
*                                  buffers in the queues. Tail drop use both
*                                  decsriptiors and buffers limits.
*                       GT_FALSE - Tail drop use only descriptor limits.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxBufferTailDropEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChPortTxBuffersSharingMaxLimitSet
*
* DESCRIPTION:
*       Sets the maximal number of shared buffers in a Tail Drop system.
*       When the total number of buffers exceeds this threshold, all of the
*       shared buffers are currently used and packets are queued according to
*       their target queue guaranteed limits.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       limit   - maximal number of shared buffers in a Tail Drop system.
*                 For DxCh2: range 0..0xFFF
*                          To disable Buffers Sharing for Transmit queues
*                          set this field to 0xFFF.
*                 For DxCh3,xCat,xCat3,xCat2: range 0..0x3FFF
*                          To disable Buffers Sharing for Transmit queues
*                          set this field to 0.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxBuffersSharingMaxLimitSet
(
    IN  GT_U8       devNum,
    IN  GT_U32      limit
);

/*******************************************************************************
* cpssDxChPortTxBuffersSharingMaxLimitGet
*
* DESCRIPTION:
*       Gets the maximal number of shared buffers in a Tail Drop system.
*       When the total number of buffers exceeds this threshold, all of the
*       shared buffers are currently used and packets are queued according to
*       their target queue guaranteed limits.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*
*
* OUTPUTS:
*       limitPtr   -  pointer to maximal number of shared buffers
*                     in a Tail Drop system
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxBuffersSharingMaxLimitGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *limitPtr
);

/*******************************************************************************
* cpssDxChPortTxDp1SharedEnableSet
*
* DESCRIPTION:
*       Enable/Disable packets with DP1 (Red) to use the shared
*       descriptors / buffers pool.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       enable  - GT_TRUE   - Allow DP1 (Red) in shared pool.
*                 GT_FALSE  - Disallow DP1 to be shared.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxDp1SharedEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
);

/*******************************************************************************
* cpssDxChPortTxDp1SharedEnableGet
*
* DESCRIPTION:
*       Gets current status of  shared  descriptors / buffer pool usage
*       for packets with DP1 (Red).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       enablePtr  - pointer to current status of
*                    shared  descroptors / Buffers pool usage:
*                  - GT_TRUE   - Allow DP1 (Red) in shared pool.
*                  - GT_FALSE  - Allow DP0 (Green) only in shared pool.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxDp1SharedEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
);

/*******************************************************************************
* cpssDxChPortTxTcSharedProfileEnableSet
*
* DESCRIPTION:
*       Enable/Disable usage of the shared  descriptors / buffer pool for
*       packets with the traffic class (tc) that are transmited via a
*       port that is associated with the Profile (pfSet).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum   - physical device number
*       pfSet    - the Profile Set in which the Traffic
*                  Class Drop Parameters is associated
*       tc       - the Traffic Class, range 0..7.
*       enableMode  - Drop Precedence (DPs) enabled mode for sharing.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, pfSet
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTcSharedProfileEnableSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT       pfSet,
    IN  GT_U8                                   tc,
    IN  CPSS_PORT_TX_SHARED_DP_MODE_ENT         enableMode
);

/*******************************************************************************
* cpssDxChPortTxTcSharedProfileEnableGet
*
* DESCRIPTION:
*       Gets usage of the shared  descriptors / buffer pool status for
*       packets with the traffic class (tc) that are transmited via a
*       port that is associated with the Profile (pfSet).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum   - physical device number
*       pfSet    - the Profile Set in which the Traffic
*                  Class Drop Parameters is associated
*       tc       - the Traffic Class, range 0..7.
*
* OUTPUTS:
*       enableModePtr   - (pointer to) Drop Precedence (DPs) enabled mode for
*                         sharing.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, pfSet
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTcSharedProfileEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT       pfSet,
    IN  GT_U8                                   tc,
    OUT CPSS_PORT_TX_SHARED_DP_MODE_ENT         *enableModePtr
);

/*******************************************************************************
* cpssDxChPortTxShaperTokenBucketMtuSet
*
* DESCRIPTION:
*       Set Token bucket maximum transmission unit (MTU).
*       The token bucket MTU defines the minimum number of tokens required to
*       permit a packet to be transmitted (i.e., conforming).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - physical device number
*       mtu      - MTU for egress rate shaper
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, mtu
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperTokenBucketMtuSet
(
    IN GT_U8                                  devNum,
    IN CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_ENT mtu
);

/*******************************************************************************
* cpssDxChPortTxShaperTokenBucketMtuGet
*
*       Get Token bucket maximum transmission unit (MTU).
*       The token bucket MTU defines the minimum number of tokens required to
*       permit a packet to be transmitted (i.e., conforming).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - physical device number
*
* OUTPUTS:
*       mtuPtr   -  pointer to MTU for egress rate shaper
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperTokenBucketMtuGet
(
    IN  GT_U8                                  devNum,
    OUT CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_ENT *mtuPtr
);

/*******************************************************************************
* cpssDxChPortTxRandomTailDropEnableSet
*
* DESCRIPTION:
*       Enable/disable Random Tail drop Threshold, to overcome synchronization.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - device number.
*       enable   -  GT_TRUE  -  Enable Random Tail drop Threshold.
*                   GT_FALSE -  Disable Random Tail drop Threshold.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on invalid input paramters value
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxRandomTailDropEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
);

/*******************************************************************************
* cpssDxChPortTxRandomTailDropEnableGet
*
* DESCRIPTION:
*       Get Random Tail drop Threshold status.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number.
*
* OUTPUTS:
*       enablePtr   -  pointer to Random Tail drop Threshold status:
*                       GT_TRUE  - Random Tail drop Threshold enabled.
*                       GT_FALSE - Random Tail drop Threshold disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on invalid input paramters value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxRandomTailDropEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChTxPortAllShapersDisable
*
* DESCRIPTION:
*       Disables all ports and queues shapers for specified device in minimum delay.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - physical device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTxPortAllShapersDisable
(
    IN  GT_U8    devNum
);

/*******************************************************************************
* cpssDxChTxPortShapersDisable
*
* DESCRIPTION:
*       Disable Shaping on Port and all it's queues.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - physical device number.
*       port    - physical or CPU port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTxPortShapersDisable
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM  port
);

/*******************************************************************************
* cpssDxChPortTxSchedulerDeficitModeEnableSet
*
* DESCRIPTION:
*       Enable/Disable scheduler deficit mode.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       enable  - GT_TRUE: Enable Scheduler Deficit mode.
*                 GT_FALSE: Disable Scheduler Deficit mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Deficit mode should be enabled to support line-rate scheduling
*       in 40/100 Gbps ports.
*       2. Request Masks must be properly configured if deficit mode
*          is enabled, see:
*          cpssDxChPortTxRequestMaskSet
*          cpssDxChPortTxRequestMaskIndexSet.
*       3. If Deficit scheduling is enabled, the shaper’s baseline must be
*          at least 8*MTU, see:
*          cpssDxChPortTxShaperBaselineSet
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSchedulerDeficitModeEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   enable
);

/*******************************************************************************
* cpssDxChPortTxSchedulerDeficitModeEnableGet
*
* DESCRIPTION:
*       Gets the current status of scheduler deficit mode.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       enablePtr  - (pointer to) status of scheduler deficit mode
*                    GT_TRUE: Scheduler deficit mode enabled.
*                    GT_FALSE: Scheduler Deficit mode disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSchedulerDeficitModeEnableGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChPortTxRequestMaskSet
*
* DESCRIPTION:
*       Sets request mask value to given mask index.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*       maskIndex    - request mask index (0..3).
*       maskValue    - request mask value defines the minimum number of core clock
*                      cycles between two consecutive service grants
*                      to the same port (3..255).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or mask index
*       GT_OUT_OF_RANGE          - on out of range mask value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Request masks used only when deficit mode is enabled, see:
*          cpssDxChPortTxSchedulerDeficitModeEnableSet.
*       2. To bind a port to request mask index use:
*          cpssDxChPortTxRequestMaskIndexSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxRequestMaskSet
(
    IN GT_U8    devNum,
    IN GT_U32   maskIndex,
    IN GT_U32   maskValue
);

/*******************************************************************************
* cpssDxChPortTxRequestMaskGet
*
* DESCRIPTION:
*       Gets request mask value to given mask index.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*       maskIndex    - request mask index (0..3).
*
* OUTPUTS:
*       maskValuePtr - (pointer to) request mask value defines the minimum number
*                      of core clock cycles between two consecutive service
*                      grants to the same port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number or mask index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxRequestMaskGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   maskIndex,
    OUT GT_U32  *maskValuePtr
);

/*******************************************************************************
* cpssDxChPortTxRequestMaskIndexSet
*
* DESCRIPTION:
*       Binds a port to a request mask.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - port number.
*       maskIndex    - mask index (0..3).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_OUT_OF_RANGE          - on out of range mask index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxRequestMaskIndexSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM    portNum,
    IN GT_U32   maskIndex
);

/*******************************************************************************
* cpssDxChPortTxRequestMaskIndexGet
*
* DESCRIPTION:
*       Gets the port's request mask.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - port number.
*
* OUTPUTS:
*       maskIndexPtr - (pointer to) mask index.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxRequestMaskIndexGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_U32   *maskIndexPtr
);
/*******************************************************************************
* cpssDxChPortTxShaperModeSet
*
* DESCRIPTION:
*       Set Shaper mode, packet or byte based for given port.
*       Shapers are configured by cpssDxChPortTxShaperProfileSet or
*       cpssDxChPortTxQShaperProfileSet
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum  - device number.
*       portNum - port number (CPU port supported as well as regular ports).
*       mode    - shaper mode: byte count or packet number based.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperModeSet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    IN  CPSS_PORT_TX_DROP_SHAPER_MODE_ENT   mode
);

/*******************************************************************************
* cpssDxChPortTxShaperModeGet
*
* DESCRIPTION:
*       Get Shaper mode, packet or byte based for given port.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum  - device number.
*       portNum - port number (CPU port supported as well as regular ports).
*
* OUTPUTS:
*       modePtr - (pointer to) shaper mode: byte count or packet number based.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperModeGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT CPSS_PORT_TX_DROP_SHAPER_MODE_ENT   *modePtr
);

/*******************************************************************************
* cpssDxChPortTxShaperBaselineSet
*
* DESCRIPTION:
*       Set Token Bucket Baseline.
*       The Token Bucket Baseline is the "zero" level of the token bucket.
*       When the token bucket fill level < Baseline, the respective queue/port
*       is not served.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum   - device number.
*       baseline - Token Bucket Baseline value in bytes(0..0xFFFFFF).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range baseline
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Token Bucket Baseline must be configured as follows:
*        1. At least MTU.
*        2. If PFC response is enabled, the Baseline must be
*           at least 0x3FFFC0, see:
*           cpssDxChPortPfcEnableSet.
*        3. If Deficit scheduling is enabled, the Baseline must be
*           at least 8*MTU, see:
*           cpssDxChPortTxSchedulerDeficitModeEnableSet.
*        4. When packet based shaping is enabled, the following used as
*           shaper’s MTU:
*           CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC. portsPacketLength
*           CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC. cpuPacketLength
*           see:
*           cpssDxChPortTxShaperConfigurationSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperBaselineSet
(
    IN GT_U8    devNum,
    IN GT_U32   baseline
);

/*******************************************************************************
* cpssDxChPortTxShaperBaselineGet
*
* DESCRIPTION:
*       Get Token Bucket Baseline.
*       The Token Bucket Baseline is the "zero" level of the token bucket.
*       When the token bucket fill level < Baseline, the respective queue/port
*       is not served.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum  - device number.
*
* OUTPUTS:
*       baselinePtr - (pointer to) Token Bucket Baseline value in bytes.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperBaselineGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *baselinePtr
);

/*******************************************************************************
* cpssDxChPortTxShaperConfigurationSet
*
* DESCRIPTION:
*       Set global configuration for shaper.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*      devNum     - device number.
*      configsPtr - (pointer to) shaper configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        When packet based shaping is enabled, the following used as shaper’s MTU:
*        configsPtr->portsPacketLength
*        configsPtr->cpuPacketLength
*        see:
*        cpssDxChPortTxShaperBaselineSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperConfigurationSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC *configsPtr
);

/*******************************************************************************
* cpssDxChPortTxShaperConfigurationGet
*
* DESCRIPTION:
*       Get global configuration for shaper.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*      devNum     - device number.
*
* OUTPUTS:
*      configsPtr - (pointer to) shaper configuration.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperConfigurationGet
(
    IN  GT_U8                   devNum,
    OUT CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC *configsPtr
);

/*******************************************************************************
* cpssDxChPortTxTailDropBufferConsumptionModeSet
*
* DESCRIPTION:
*       Sets the packet buffer consumption mode and its parameter.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum  - device number.
*       mode    - packet buffer consumption mode.
*                 (APPLICABLE DEVICES: Lion; Lion2.)
*       length  - Defines the number of buffers consumed by a packet for
*                 Tail Drop and Scheduling.
*                 Range: 0..63
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or wrong mode
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on length out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The function should be called only when traffic disabled
*       Mode configuration in Bobcat2,Caelum,Bobcat3 performed per profile.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTailDropBufferConsumptionModeSet
(
    IN  GT_U8                                                   devNum,
    IN  CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_ENT mode,
    IN  GT_U32                                                  length
);

/*******************************************************************************
* cpssDxChPortTxTailDropBufferConsumptionModeGet
*
* DESCRIPTION:
*       Gets the packet buffer consumption mode and its parameter.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum  - device number.
*
* OUTPUTS:
*       modePtr     - (pointer to) packet buffer consumption mode.
*                     (Not applicable for Bobcat2,Caelum,Bobcat3)
*       lengthPtr   - (pointer to ) the number of buffers consumed by a packet
*                     for Tail Drop and Scheduling. Relevant only for mode
*                 CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_PACKET_E.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or wrong mode
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTailDropBufferConsumptionModeGet
(
    IN  GT_U8                                                   devNum,
    OUT CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_ENT *modePtr,
    OUT GT_U32                                                  *lengthPtr
);

/*******************************************************************************
* cpssDxChPortTxTcProfileSharedPoolSet
*
* DESCRIPTION:
*       Sets the shared pool associated for traffic class and Profile.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum  - physical device number
*       pfSet   - the Profile Set in which the Traffic
*                 Class Drop Parameters is associated
*       tc      - the Traffic Class, range 0..7.
*       poolNum - shared pool associated, range 0..7.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, pfSet
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTcProfileSharedPoolSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   pfSet,
    IN  GT_U8                               tc,
    IN  GT_U32                              poolNum
);

/*******************************************************************************
* cpssDxChPortTxTcProfileSharedPoolGet
*
* DESCRIPTION:
*       Gets the shared pool associated for traffic class and Profile.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum      - physical device number
*       pfSet       - the Profile Set in which the Traffic
*                     Class Drop Parameters is associated
*       tc          - the Traffic Class, range 0..7.
*
* OUTPUTS:
*       poolNumPtr  - (pointer to) shared pool associated.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, pfSet
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTcProfileSharedPoolGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   pfSet,
    IN  GT_U8                               tc,
    OUT GT_U32                              *poolNumPtr
);

/*******************************************************************************
* cpssDxChPortTxMcastBuffersLimitSet
*
* DESCRIPTION:
*       Set maximal buffers limits for multicast packets.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum            - physical device number
*       mcastMaxBufNum    - The number of buffers allocated for multicast
*                           packets. Range 0..0xFFFF
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range mcastMaxBufNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxMcastBuffersLimitSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  mcastMaxBufNum
);


/*******************************************************************************
* cpssDxChPortTxMcastBuffersLimitGet
*
* DESCRIPTION:
*       Get maximal buffers limits for multicast packets.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum              - physical device number
*
* OUTPUTS:
*       mcastMaxBufNumPtr   - (pointer to) the number of buffers allocated
*                             for multicast packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxMcastBuffersLimitGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *mcastMaxBufNumPtr
);

/*******************************************************************************
* cpssDxChPortTxSniffedBuffersLimitSet
*
* DESCRIPTION:
*       Set maximal buffers limits for mirrored packets.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum            - physical device number
*       rxSniffMaxBufNum  - The number of buffers allocated for packets
*                           forwarded to the ingress analyzer port due to
*                           mirroring.
*                           (APPLICABLE RANGES: Lion, Lion2: 0..0xFFFF,
*                            Bobcat2, Caelum, Bobcat3: 0..0xFFFFF)
*
*       txSniffMaxBufNum  - The number of buffers allocated for packets
*                           forwarded to the egress analyzer port due to
*                           mirroring.
*                           (APPLICABLE RANGES: Lion, Lion2: 0..0xFFFF,
*                            Bobcat2, Caelum, Bobcat3: 0..0xFFFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on out of range value
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSniffedBuffersLimitSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  rxSniffMaxBufNum,
    IN  GT_U32  txSniffMaxBufNum
);

/*******************************************************************************
* cpssDxChPortTxSniffedBuffersLimitGet
*
* DESCRIPTION:
*       Get maximal buffers limits for mirrored packets.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum              - physical device number
*
* OUTPUTS:
*       rxSniffMaxBufNumPtr - (pointer to) The number of buffers allocated
*                              for packets forwarded to the ingress analyzer
*                              port due to mirroring.
*       txSniffMaxBufNumPtr - (pointer to) The number of buffers allocated
*                              for packets forwarded to the egress analyzer
*                              port due to mirroring.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSniffedBuffersLimitGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *rxSniffMaxBufNumPtr,
    OUT GT_U32  *txSniffMaxBufNumPtr
);

/*******************************************************************************
* cpssDxChPortTxSharedPoolLimitsSet
*
* DESCRIPTION:
*       Set maximal descriptors and buffers limits for shared pool.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - physical device number
*       poolNum    - Shared pool number. Range 0..7
*       maxBufNum  - The number of buffers allocated for a shared pool.
*                    Range 0..0x3FFF.
*       maxDescNum - The number of descriptors allocated for a shared pool.
*                    Range 0..0x3FFF.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on out of range value
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSharedPoolLimitsSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  poolNum,
    IN  GT_U32  maxBufNum,
    IN  GT_U32  maxDescNum
);

/*******************************************************************************
* cpssDxChPortTxSharedPoolLimitsGet
*
* DESCRIPTION:
*       Get maximal descriptors and buffers limits for shared pool.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum        - physical device number
*       poolNum       - Shared pool number. Range 0..7
*
* OUTPUTS:
*       maxBufNumPtr  - (pointer to) The number of buffers allocated for a
*                       shared pool.
*       maxDescNumPtr - (pointer to) The number of descriptors allocated
*                       for a shared pool.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSharedPoolLimitsGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  poolNum,
    OUT GT_U32  *maxBufNumPtr,
    OUT GT_U32  *maxDescNumPtr
);

/*******************************************************************************
* cpssDxChPortTxProfileWeightedRandomTailDropEnableSet
*
* DESCRIPTION:
*       Enable/disable Weighted Random Tail Drop Threshold to overcome
*       synchronization.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum      - device number.
*       pfSet       - the Profile Set in which the Traffic
*                     Class Drop Parameters is associated.
*       dp          - Drop Precedence
*                     (APPLICABLE RANGES: Lion, Lion2, Bobcat2, Caelum, Bobcat3 0..2; xCat2 0..1)
*       tc          - the Traffic Class, range 0..7.
*                     (APPLICABLE DEVICES: xCat2, Bobcat2, Caelum, Bobcat3)
*       enablersPtr - (pointer to) Tail Drop limits enabling
*                     for Weigthed Random Tail Drop
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on invalid input paramters value
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxProfileWeightedRandomTailDropEnableSet
(
    IN GT_U8                                    devNum,
    IN CPSS_PORT_TX_DROP_PROFILE_SET_ENT        pfSet,
    IN GT_U32                                   dp,
    IN GT_U8                                    tc,
    IN CPSS_PORT_TX_TAIL_DROP_WRTD_ENABLERS_STC *enablersPtr
);

/*******************************************************************************
* cpssDxChPortTxProfileWeightedRandomTailDropEnableGet
*
* DESCRIPTION:
*       Get Random Tail drop Threshold status.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum      - device number.
*       pfSet       - the Profile Set in which the Traffic
*                     Class Drop Parameters is associated.
*       dp          - Drop Precedence
*                     (APPLICABLE RANGES: Lion, Lion2, Bobcat2, Caelum, Bobcat3 0..2; xCat2 0..1)
*       tc          - the Traffic Class, range 0..7.
*                     (APPLICABLE DEVICES: xCat2, Bobcat2, Caelum, Bobcat3)
*
* OUTPUTS:
*       enablersPtr - (pointer to) Tail Drop limits enabling
*                     for Weigthed Random Tail Drop.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on invalid input paramters value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxProfileWeightedRandomTailDropEnableGet
(
    IN  GT_U8                                    devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT        pfSet,
    IN  GT_U32                                   dp,
    IN  GT_U8                                    tc,
    OUT CPSS_PORT_TX_TAIL_DROP_WRTD_ENABLERS_STC *enablersPtr
);

/*******************************************************************************
* cpssDxChPortTxTailDropWrtdMasksSet
*
* DESCRIPTION:
*       Sets Weighted Random Tail Drop (WRTD) masks.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum     - physical device number
*       maskLsbPtr - WRTD masked least significant bits.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTailDropWrtdMasksSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORT_TX_WRTD_MASK_LSB_STC  *maskLsbPtr
);

/*******************************************************************************
* cpssDxChPortTxTailDropWrtdMasksGet
*
* DESCRIPTION:
*       Gets Weighted Random Tail Drop (WRTD) masks.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum     - physical device number
*
* OUTPUTS:
*       maskLsbPtr - (pointer to) WRTD masked least significant bits.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTailDropWrtdMasksGet
(
    IN  GT_U8                            devNum,
    OUT CPSS_PORT_TX_WRTD_MASK_LSB_STC  *maskLsbPtr
);

/*******************************************************************************
* cpssDxChPortTxResourceHistogramThresholdSet
*
* DESCRIPTION:
*       Sets threshold for Histogram counter increment.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum      - physical device number
*       cntrNum     - Histogram Counter number. Range 0..3.
*       threshold   - If the Global Descriptors Counter exceeds this Threshold,
*                     the Histogram Counter is incremented by 1.
*                     Range 0..0x3FFF.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, cntrNum
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxResourceHistogramThresholdSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  cntrNum,
    IN  GT_U32  threshold
);

/*******************************************************************************
* cpssDxChPortTxResourceHistogramThresholdGet
*
* DESCRIPTION:
*       Gets threshold for Histogram counter increment.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum          - physical device number
*       cntrNum         - Histogram Counter number. Range 0..3.
*
* OUTPUTS:
*       thresholdPtr    - (pointer to) If the Global Descriptors Counter
*                         exceeds this Threshold, the Histogram Counter
*                         is incremented by 1.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, cntrNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxResourceHistogramThresholdGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  cntrNum,
    OUT GT_U32  *thresholdPtr
);

/*******************************************************************************
* cpssDxChPortTxResourceHistogramCounterGet
*
* DESCRIPTION:
*       Gets Histogram Counter.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum  - physical device number
*       cntrNum - Histogram Counter number. Range 0..3.
*
* OUTPUTS:
*       cntrPtr - (pointer to) Histogram Counter value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, cntrNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Histogram Counter is cleared on read.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxResourceHistogramCounterGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  cntrNum,
    OUT GT_U32  *cntrPtr
);

/*******************************************************************************
* cpssDxChPortTxGlobalDescNumberGet
*
* DESCRIPTION:
*       Gets total number of descriptors allocated.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum    - physical device number
*
* OUTPUTS:
*       numberPtr - number of descriptors
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxGlobalDescNumberGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *numberPtr
);

/*******************************************************************************
* cpssDxChPortTxGlobalBufNumberGet
*
* DESCRIPTION:
*       Gets total number of virtual buffers enqueued.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       numberPtr   - number of buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The buffers associated with enqueued multi-target descriptors are
*       counted multiple times, once for each multi-target descriptor instance.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxGlobalBufNumberGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *numberPtr
);

/*******************************************************************************
* cpssDxChPortTxMcastDescNumberGet
*
* DESCRIPTION:
*       Gets total number of multi-target descriptors allocated.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum    - physical device number
*
* OUTPUTS:
*       numberPtr - number of descriptors
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Multi-target descriptors are descriptors that are duplicated to
*       multiple egress targets, e.g. Multicast, Broadcast, or unknown Unicast
*       flooded packets.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxMcastDescNumberGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *numberPtr
);

/*******************************************************************************
* cpssDxChPortTxMcastBufNumberGet
*
* DESCRIPTION:
*       Gets total number of multi-target buffers allocated (virtual buffers).
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       numberPtr   - number of buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The buffers associated with enqueued multi-target descriptors are
*       counted multiple times, once for each multi-target descriptor instance.
*       Multi-target descriptors are descriptors that are duplicated to
*       multiple egress targets, e.g. Multicast, Broadcast, or unknown Unicast
*       flooded packets.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxMcastBufNumberGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *numberPtr
);

/*******************************************************************************
* cpssDxChPortTxSniffedDescNumberGet
*
* DESCRIPTION:
*       Gets total number of mirrored descriptors allocated.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum    - physical device number
*
* OUTPUTS:
*       rxNumberPtr - number of ingress mirrored descriptors.
*       txNumberPtr - number of egress mirrored descriptors.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSniffedDescNumberGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *rxNumberPtr,
    OUT GT_U32  *txNumberPtr
);

/*******************************************************************************
* cpssDxChPortTxSniffedBufNumberGet
*
* DESCRIPTION:
*       Gets total number of mirrored buffers allocated.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       rxNumberPtr - number of ingress mirrored buffers.
*       txNumberPtr - number of egress mirrored buffers.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSniffedBufNumberGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *rxNumberPtr,
    OUT GT_U32  *txNumberPtr
);

/*******************************************************************************
* cpssDxChPortTxSharedResourceDescNumberGet
*
* DESCRIPTION:
*       Gets total number of descriptors allocated from the shared descriptors
*       pool.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum    - physical device number
*       poolNum   - shared pool number, range 0..7
*
* OUTPUTS:
*       numberPtr - number of descriptors
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong poolNum or devNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSharedResourceDescNumberGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  poolNum,
    OUT GT_U32  *numberPtr
);

/*******************************************************************************
* cpssDxChPortTxSharedResourceBufNumberGet
*
* DESCRIPTION:
*       Gets the number of virtual buffers enqueued from the shared buffer pool.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum    - physical device number
*       poolNum   - shared pool number, range 0..7
*
* OUTPUTS:
*       numberPtr - number of buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong poolNum or devNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSharedResourceBufNumberGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  poolNum,
    OUT GT_U32  *numberPtr
);

/*******************************************************************************
* cpssDxChPortTxShaperOnStackAsGigEnableSet
*
* DESCRIPTION:
*       Enables/Disables the shaper on stacking ports (24-27) to operate as
*       Tri Speed ports' shapers.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       enable  - GT_TRUE, stacking ports shapers operates
*                          as Tri Speed ports' shapers.
*                 GT_FALSE, stacking ports shapers operates as XG ports.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       When "Stack As Gig" enabled, for a port number in the range 24..27,
*       whether Gig or XG type, the global shaper used parameters are those
*       referenced as gig ones -
*       "gigPortsTokensRate" and "gigPortsSlowRateRatio" inputs for
*       "cpssDxChPortTxShaperGlobalParamsSet" API.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperOnStackAsGigEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   enable
);

/*******************************************************************************
* cpssDxChPortTxShaperOnStackAsGigEnableGet
*
* DESCRIPTION:
*       Gets the current state (enable/disable) of the shaper's mode on
*       stacking ports (24-27).
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                 GT_TRUE, stacking ports shapers operates
*                          as Tri Speed ports' shapers.
*                 GT_FALSE, stacking ports shapers operates as XG ports.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperOnStackAsGigEnableGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChPortProfileTxByteCountChangeEnableSet
*
* DESCRIPTION:
*       Enables/Disables profile Byte Count Change of the packet length by per port
*       constant for shaping and/or scheduling rate calculation.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       profile    - scheduler profile index
*       bcMode     - byte count change enable options
*       bcOp       - addition or subtraction of the per port value.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal profile, bcMode, bcOp
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortProfileTxByteCountChangeEnableSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profile,
    IN  CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_ENT  bcMode,
    IN  CPSS_ADJUST_OPERATION_ENT               bcOp
);

/*******************************************************************************
* cpssDxChPortProfileTxByteCountChangeEnableGet
*
* DESCRIPTION:
*       Get profile Byte Count Change mode.
*       The byte count change mode changes the length of a packet by per port
*       constant for shaping and/or scheduling rate calculation.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       profile    - scheduler profile index
*
* OUTPUTS:
*       bcModePtr  - (pointer to) byte count change enable options.
*       bcOpPtr    - (pointer to) addition or subtraction of the per port value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal profile
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortProfileTxByteCountChangeEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profile,
    OUT CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_ENT  *bcModePtr,
    OUT CPSS_ADJUST_OPERATION_ENT               *bcOpPtr
);


/*******************************************************************************
* cpssDxChPortTxByteCountChangeEnableSet
*
* DESCRIPTION:
*       Enables/Disables Byte Count Change of the packet length by per port
*       constant for shaping and/or scheduling rate calculation.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum - physical device number.
*       bcChangeEnable - Byte Count Change enable opition.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number or BC change option
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxByteCountChangeEnableSet
(
    IN  GT_U8     devNum,
    IN  CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_ENT bcChangeEnable
);

/*******************************************************************************
* cpssDxChPortTxByteCountChangeEnableGet
*
* DESCRIPTION:
*       Gets the Byte Count Change enable opition.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum - physical device number.
*
* OUTPUTS:
*       bcChangeEnablePtr - (pointer to) Byte Count Change enable opition.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxByteCountChangeEnableGet
(
    IN  GT_U8     devNum,
    OUT CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_ENT *bcChangeEnablePtr
);


/*******************************************************************************
* cpssDxChPortTxByteCountChangeValueSet
*
* DESCRIPTION:
*       Sets the number of bytes added/subtracted to the length of every frame
*       for shaping/scheduling rate calculation for given port when
*       Byte Count Change is enabled.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical or CPU port number
*       bcValue - number of bytes added to the length of every frame
*                 for shaping/scheduling rate calculation.
*                 (APPLICABLE RANGES: xCat, xCat3, xCat2 -63..63;
*                                     Bobcat2 0..63)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. To globally enable/disable Byte Count Change,
*          use cpssDxChPortTxByteCountChangeEnableSet
*       2. To update bcValue for given port Byte Count Change globally disabled.
*          This can cause side effects to shaper/scheduler behavior
*          on other ports if performed under traffic.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxByteCountChangeValueSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    IN  GT_32     bcValue
);

/*******************************************************************************
* cpssDxChPortTxByteCountChangeValueGet
*
* DESCRIPTION:
*       Gets the number of bytes added/subtracted to the length of every frame
*       for shaping/scheduling rate calculation for given port when
*       Byte Count Change is enabled.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical or CPU port number
*
* OUTPUTS:
*       bcValuePtr - (pointer to) number of bytes added to the length of
*                    every frame for shaping/scheduling rate calculation.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxByteCountChangeValueGet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_32    *bcValuePtr
);

/*******************************************************************************
* cpssDxChPortTxGlobalDescLimitSet
*
* DESCRIPTION:
*       Set limits of total descriptors in all egress queues
*
* APPLICABLE DEVICES:
*       Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum          - device number.
*       limit           - total descriptor limit value
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on invalid input paramters value
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_OUT_OF_RANGE          - on limit out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Improper configuration of descriptors limit may affect entire system behavior
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxGlobalDescLimitSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  limit
);

/*******************************************************************************
* cpssDxChPortTxGlobalDescLimitGet
*
* DESCRIPTION:
*       Get limits of total descriptors in all egress queues
*
* APPLICABLE DEVICES:
*       Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum      - device number.
*
* OUTPUTS:
*       limitPtr   -  pointer to total descriptor limit value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on invalid input paramters value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxGlobalDescLimitGet
(
    IN  GT_U8   devNum,
    OUT GT_U32 *limitPtr
);

/*******************************************************************************
* cpssDxChPortTxMcFifoSet
*
* DESCRIPTION:
*      Enable/Disable selected MC FIFO for specified egress port.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
*       mcFifo   - MC FIFO number (APPLICABLE RANGES: 0..1)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev, port or mcFifo
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxMcFifoSet
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    IN GT_U32        mcFifo
);

/*******************************************************************************
* cpssDxChPortTxMcFifoGet
*
* DESCRIPTION:
*      Gets state (Enable/Disable) of selected MC FIFO for specified egress port.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
*
* OUTPUTS:
*       mcFifoPtr   - (pointer to) MC FIFO number
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - wrong HW configuration
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxMcFifoGet
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    OUT GT_U32       *mcFifoPtr
);

/*******************************************************************************
* cpssDxChPortTxMcFifoArbiterWeigthsSet
*
* DESCRIPTION:
*      Configure the Multicast arbiter weights for MC FIFOs.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum      - device number
*       weigthsPtr  - (pointer to) Multicast arbiter weights
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev
*       GT_OUT_OF_RANGE          - FIFO weight out of range
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxMcFifoArbiterWeigthsSet
(
    IN GT_U8        devNum,
    IN CPSS_PORT_TX_MC_FIFO_ARBITER_WEIGHTS_STC  *weigthsPtr
);

/*******************************************************************************
* cpssDxChPortTxMcFifoArbiterWeigthsGet
*
* DESCRIPTION:
*      Get the Multicast arbiter weights for MC FIFOs configuration.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       weigthsPtr  - (pointer to) Multicast arbiter weights
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxMcFifoArbiterWeigthsGet
(
    IN GT_U8        devNum,
    OUT CPSS_PORT_TX_MC_FIFO_ARBITER_WEIGHTS_STC  *weigthsPtr
);

/*******************************************************************************
* cpssDxChPortTxQueueGlobalTxEnableSet
*
* DESCRIPTION:
*       Enable/Disable transmission of specified device.
*
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum  - physical device number
*       enable  - GT_TRUE, enable transmission
*                 GT_FALSE, disable transmission
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQueueGlobalTxEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   enable
);

/*******************************************************************************
* cpssDxChPortTxQueueGlobalTxEnableGet
*
* DESCRIPTION:
*       Get the status of transmission of specified device (Enable/Disable).
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)
*                     GT_TRUE, enable transmission
*                     GT_FALSE, disable transmission
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQueueGlobalTxEnableGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr
);


/*******************************************************************************
* cpssDxChPortTxMcShaperMaskSet
*
* DESCRIPTION:
*       Function sets the multicast shaper mask value.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion.
*
* INPUTS:
*       devNum  - physical device number
*       mask    - mask value (APPLICABLE RANGE: 0..255)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_OUT_OF_RANGE          - on wrong mask
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxMcShaperMaskSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  mask
);


/*******************************************************************************
* cpssDxChPortTxMcShaperMaskGet
*
* DESCRIPTION:
*       Function gets the multicast shaper mask value.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion.
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       maskPtr - (pointer to) mask value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxMcShaperMaskGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *maskPtr
);

/*******************************************************************************
* cpssDxChPortTxTailDropProfileBufferConsumptionModeSet
*
* DESCRIPTION:
*       Sets the packet buffer consumption mode per Tail Drop profile.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number.
*       profile - tail drop profile
*       mode    - buffer consumption mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device, profile or wrong mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTailDropProfileBufferConsumptionModeSet
(
    IN  GT_U8                                                   devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT                       profile,
    IN  CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_ENT mode
);


/*******************************************************************************
* cpssDxChPortTxTailDropProfileBufferConsumptionModeGet
*
* DESCRIPTION:
*       Gets the packet buffer consumption mode per Tail Drop profile.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number.
*       profile - tail drop profile
*
* OUTPUTS:
*       modePtr - (pointer to) buffer consumption mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device, profile
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTailDropProfileBufferConsumptionModeGet
(
    IN  GT_U8                                                   devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT                       profile,
    OUT CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_ENT *modePtr
);

/*******************************************************************************
* cpssDxChPortTxShaperAvbModeEnableSet
*
* DESCRIPTION:
*       Set Shaper AVB Mode of specified port.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number.
*       portNum         - physical or CPU port number
*       avbModeEnable   - AVB mode
*                          GT_TRUE - AVB mode enabled
*                          GT_FALSE - AVB mode disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperAvbModeEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 avbModeEnable
);


/*******************************************************************************
* cpssDxChPortTxQShaperAvbModeEnableGet
*
* DESCRIPTION:
*       Get status of Shaper AVB Mode of specified port.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number.
*       portNum         - physical or CPU port number
*
* OUTPUTS:
*       avbModeEnablePtr- (pointer to) AVB mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxShaperAvbModeEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *avbModeEnablePtr
);


/*******************************************************************************
* cpssDxChPortTxQShaperAvbModeEnableSet
*
* DESCRIPTION:
*       Set Shaper AVB Mode for Traffic Class Queue of specified port.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number.
*       portNum         - physical or CPU port number
*       tcQueue         - traffic class queue on this Logical Port
*       avbModeEnable   - AVB mode
*                          GT_TRUE - AVB mode enabled
*                          GT_FALSE - AVB mode disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device, portNum, tc
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQShaperAvbModeEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U8                   tcQueue,
    IN  GT_BOOL                 avbModeEnable
);

/*******************************************************************************
* cpssDxChPortTxQShaperAvbModeEnableGet
*
* DESCRIPTION:
*       Get status of Shaper AVB Mode for Traffic Class Queue of specified port.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number.
*       portNum         - physical or CPU port number
*       tcQueue         - traffic class queue on this Logical Port
*
* OUTPUTS:
*       avbModeEnablePtr- (pointer to) AVB mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device, portNum, tc
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxQShaperAvbModeEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U8                   tcQueue,
    OUT GT_BOOL                 *avbModeEnablePtr
);


/*******************************************************************************
* cpssDxChPortTxSchedulerProfileCountModeSet
*
* DESCRIPTION:
*       Sets the counting mode for  scheduler profile.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number.
*       profile - scheduler profile
*       wrrMode - scheduler count mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device, profile or wrong wrrMode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSchedulerProfileCountModeSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profile,
    IN  CPSS_PORT_TX_WRR_MODE_ENT               wrrMode
);

/*******************************************************************************
* cpssDxChPortTxSchedulerProfileCountModeGet
*
* DESCRIPTION:
*       Sets the counting mode for  scheduler profile.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number.
*       profile - scheduler profile
*
* OUTPUTS:
*       wrrModePtr - (pointer to) scheduler count mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device, profile or wrong wrrMode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxSchedulerProfileCountModeGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profile,
    OUT CPSS_PORT_TX_WRR_MODE_ENT               *wrrModePtr
);


/*******************************************************************************
* cpssDxChPortTxTailDropMaskSharedBuffEnableSet  
*
* DESCRIPTION:
*       Enable/Disable shared buffer indication masking.
*       When enabled, the tail drop mechanism will ignore
*       the buffer fill level indication.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - physical device number
*       enable  - GT_TRUE - Enable buffer fill level masking.
*                 GT_FALSE -Disable buffer fill level masking.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/ 
GT_STATUS cpssDxChPortTxTailDropMaskSharedBuffEnableSet  
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
);

/*******************************************************************************
* cpssDxChPortTxTailDropMaskSharedBuffEnableGet  
*
* DESCRIPTION:
*       Gets status of shared buffer indication masking.
*       When enabled, the tail drop mechanism will ignore
*       the buffer fill level indication.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       enablePtr - (pointer to) shared buffer indication masking.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/ 
GT_STATUS cpssDxChPortTxTailDropMaskSharedBuffEnableGet  
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
);


/*******************************************************************************
* cpssDxChPortTxTailDropSharedBuffMaxLimitSet  
*
* DESCRIPTION:
*       Set max shared buffer limit value.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - physical device number
*       maxSharedBufferLimit - maximal shared buffer limit
*                             (APPLICABLE RANGES: 0..0xFFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_OUT_OF_RANGE          - on illegal maxSharedBufferLimit
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/ 
GT_STATUS cpssDxChPortTxTailDropSharedBuffMaxLimitSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  maxSharedBufferLimit
);


/*******************************************************************************
* cpssDxChPortTxTailDropSharedBuffMaxLimitGet  
*
* DESCRIPTION:
*       Get max shared buffer limit value.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - physical device number
*
* OUTPUTS:
*       maxSharedBufferLimitPtr - (pointer to) maximal shared buffer limit
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/ 
GT_STATUS cpssDxChPortTxTailDropSharedBuffMaxLimitGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *maxSharedBufferLimitPtr
);

/*******************************************************************************
* cpssDxChPortTx4TcMcBufNumberGet
*
* DESCRIPTION:
*       Gets the current number of MC buffers allocated on specified port
*       for specified Traffic Class queues.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical or CPU port number
*       trafClass - trafiic class
*
* OUTPUTS:
*       numPtr    - (pointer to) the number of buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortTx4TcMcBufNumberGet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  GT_U8       trafClass,
    OUT GT_U32      *numPtr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChPortTxh */

