/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChDiagDataIntegrity.c
*
* DESCRIPTION:
*       CPSS DXCH Diagnostic Data Integrity API
*
* FILE REVISION NUMBER:
*       $Revision: 15 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/diag/private/prvCpssDxChDiagLog.h>
#include <cpss/dxCh/dxChxGen/diag/cpssDxChDiagDataIntegrity.h>
#include <cpss/dxCh/dxChxGen/diag/private/prvCpssDxChDiag.h>
#include <cpss/dxCh/dxChxGen/diag/private/prvCpssDxChDiagDataIntegrityMainMappingDb.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwTables.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsLion2.h>



#define PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_ADDR_TYPE_BASE_ADDR_CNS                       0x0   /* base address */
#define PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_ADDR_TYPE_MEMORY_INT_CAUSE_REG_CNS            0x1C  /* memory interrupt cause register */
#define PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_ADDR_TYPE_MEMORY_INT_MASK_REG_CNS             0x18  /* memory interrupt mask register */
#define PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_ADDR_TYPE_MEMORY_CONTROL_REG_CNS              0x14  /* memory control register */
#define PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_ADDR_TYPE_MEMORY_ERRONEOUS_ADDR_REG_CNS       0x8   /* memory erroneous address register */
#define PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_ADDR_TYPE_MEMORY_ERRONEOUS_SEGMENT_REG_CNS    0x10  /* memory erroneous segment register */
#define PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_ADDR_TYPE_MEMORY_ECC_SYNDROME_REG_CNS         0xC   /* memory ECC syndrome register */
#define PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_ADDR_TYPE_MEMORY_ERRORS_COUNTER_REG_CNS       0x4   /* memory error counter register */
#define PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_ADDR_TYPE_DFX_CLIENT_INT_STATUS0_REG_CNS      0x50  /* client status 0 register */
#define PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_ADDR_TYPE_DFX_CLIENT_INT_STATUS1_REG_CNS      0x54  /* client status 1 register */
#define PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_ADDR_TYPE_DFX_CLIENT_INT_STATUS2_REG_CNS      0x58  /* client status 2 register */
#define PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_ADDR_TYPE_DFX_CLIENT_INT_STATUS3_REG_CNS      0x5C  /* client status 3 register */

/* Macro for NON DFX protected memories */
#define PRV_CPSS_DXCH_LION2_DIAG_DATA_INTEGRITY_EXT_PROTECTION_MEMORY_TYPE_CASES_MAC \
    /*caller start with 'case' */                                                    \
         CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXDMA_HA_INFO_DESC_PREFETCH_E:       \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ROUTER_TCAM_E:                       \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_POLICY_TCAM_E:                       \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_EGR_MC_FIFO_0_E:                 \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_EGR_MC_FIFO_1_E:                 \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_EGR_MC_FIFO_3_E:                 \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_EGR_VLAN_E:                  \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_ING_VLAN_E:                  \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_PORT_ISO_L2_E:               \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_PORT_ISO_L3_E:               \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_VIDX_E:                      \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IP_NHE_TABLE_E:                      \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IP_STG2_DESC_RETURN_TWO_FIFO_E:      \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IP_STG4_DESC_RETURN_TWO_FIFO_E:      \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IP_UNUSED_DATA_FIFO_E:               \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PLR_DESCRIPTOR_FIFO_E:               \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_DATA_BUFFER_MEMORY_E:           \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_DATA_BUFFER_MEMORY_ECC_E:       \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_DQ_TB_GIGA_PRIO_E:               \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_DQ_WRR_STATE_VARIABLES_E:        \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_LL_FREE_BUFS_E:                  \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_BUFFER_FIFO_E:                 \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_0_COUNTERS_E:     \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_1_COUNTERS_E:     \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_2_COUNTERS_E:     \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_3_COUNTERS_E:     \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_4_COUNTERS_E:     \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_5_COUNTERS_E:     \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_6_COUNTERS_E:     \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_7_COUNTERS_E:     \
    case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_BMA_MULTICAST_COUNTERS_E /*caller end with ':' */


/*
 * Typedef: struct PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_CLIENT_STATUS_STC
 *
 * Description: Bitmap of memories errors in DFX Client
 *
 * Fields:
 *      bitmap     - client bitmap array
 *
 * Comments:
 *      None
 */
typedef struct
{
    GT_U32 bitmap[4];
}PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_CLIENT_STATUS_STC;


/*
 * Typedef: struct PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_DFX_ADDR_STC
 *
 * Description: Compare memory BIST data
 *
 * Fields:
 *      dfxUnitClientIdx             - bits [19:15]
 *                                     0-27 - DFX Clients 0 to 27
 *                                     28 - DFX Clients Broadcast
 *                                     29 - DFX Clients Multicast
 *                                     30 - DFX Link Data Logging
 *      dfxServerRegisterOffset      - bits [14:0]
 *                                     Relevant only if dfxUnitClientIdx = 31
 *      dfxXsbSelect                 - bit [14]
 *                                     XSB Select
 *                                          1 - XSB
 *                                          0 - Memories and Client Registers
 *                                     Relevant only if dfxUnitClientIdx = 0-29
 *      dfxXsbNumber                 - bits [13:9]
 *                                     XSB Number
 *                                     Relevant only if dfxUnitClientIdx = 0-29 and
 *                                     XSB Select = 1
 *      dfxXsbRegOffset              - bits [8:0]
 *                                     XSB Register Offset
 *                                     Relevant only if dfxUnitClientIdx = 0-29 and
 *                                     XSB Select = 1
 *      dfxClientSelect              - bit [13]
 *                                     DFX Client Select
 *                                          0 - Memory
 *                                          1 - Client Registers
 *                                     Relevant only if dfxUnitClientIdx = 0-29 and
 *                                     XSB Select = 0
 *      dfxClientRegisterOffset      - bits [12:0]
 *                                     DFX Client Register Offset
 *                                     Relevant only if dfxUnitClientIdx = 0-29 and
 *                                     XSB Select = 0 and
 *                                     DFX Client Select = 1
 *      dfxClientMemoryNumber        - bits [12:6]
 *                                     DFX Client Memory Number
 *                                          0-124   - memories  from 0 to 124
 *                                          125-126 - memories Multicast
 *                                          127     - memories Broadcast
 *                                     Relevant only if dfxUnitClientIdx = 0-29 and
 *                                     XSB Select = 0 and
 *                                     DFX Client Select = 0
 *      dfxClientMemoryRegisterOffset- bits [5:0]
 *                                     DFX Client Memory Register Offset
 *                                     Relevant only if dfxUnitClientIdx = 0-29 and
 *                                     XSB Select = 0 and
 *                                     DFX Client Select = 0
 *
 * Comments:
 *      None
 */
typedef struct
{
    GT_U32 dfxUnitClientIdx;
    GT_U32 dfxServerRegisterOffset;
    GT_U32 dfxXsbSelect;
    GT_U32 dfxXsbNumber;
    GT_U32 dfxXsbRegOffset;
    GT_U32 dfxClientSelect;
    GT_U32 dfxClientRegisterOffset;
    GT_U32 dfxClientMemoryNumber;
    GT_U32 dfxClientMemoryRegisterOffset;
}PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_DFX_ADDR_STC;


static GT_STATUS prvCpssDxChDiagDataIntegrityPortGroupMemoryIndexesGet
(
    IN  GT_U8                                           devNum,
    IN  GT_U32                                          portGroupId,
    IN  CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT      memType,
    OUT CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_STC    *memLocation
);

/*******************************************************************************
* prvCpssDxChDiagDataIntegrityDbPointerSet
*
* DESCRIPTION:
*       Function initializes current pointer to DB and size.
*
* APPLICABLE DEVICES:
*       Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       dbArrayPtrPtr       - (pointer to pointer to) current DB array
*       dbArrayEntryNumPtr  - (pointer to) current DB array size
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID prvCpssDxChDiagDataIntegrityDbPointerSet
(
    IN  GT_U8                                           devNum,
    OUT PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_MAP_DB_STC    **dbArrayPtrPtr,
    OUT GT_U32                                          *dbArrayEntryNumPtr
)
{
    switch(PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        case CPSS_PP_FAMILY_DXCH_LION2_E:
            switch (PRV_CPSS_PP_MAC(devNum)->devType)
            {
                case CPSS_LION2_HOOPER_PORT_GROUPS_0123_DEVICES_CASES_MAC:
                    *dbArrayPtrPtr = hooperDataIntegrityDbArray;
                    *dbArrayEntryNumPtr = hooperDataIntegrityDbArrayEntryNum;
                    break;
                default:
                    *dbArrayPtrPtr = lion2DataIntegrityDbArray;
                    *dbArrayEntryNumPtr = lion2DataIntegrityDbArrayEntryNum;
                    break;
            }

            break;
        case CPSS_PP_FAMILY_DXCH_BOBCAT2_E:
            if (PRV_CPSS_PP_MAC(devNum)->devSubFamily == CPSS_PP_SUB_FAMILY_BOBCAT2_BOBK_E)
            {/* Caelum based devices */
                *dbArrayPtrPtr = caelumDataIntegrityDbArray;
                *dbArrayEntryNumPtr = caelumDataIntegrityDbArrayEntryNum;            
            }
            else
            {/* Bobcat2 devices */
                if (!PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
                {
                    *dbArrayPtrPtr = bobcat2DataIntegrityDbArray;
                    *dbArrayEntryNumPtr = bobcat2DataIntegrityDbArrayEntryNum;
                }
                else
                {
                    *dbArrayPtrPtr = bobcat2B0DataIntegrityDbArray;
                    *dbArrayEntryNumPtr = bobcat2B0DataIntegrityDbArrayEntryNum;
                }
            }
            break;
        default:
             *dbArrayEntryNumPtr = 0;
             *dbArrayPtrPtr = NULL;
             break;
    }

    return;
}


/*******************************************************************************
* prvCpssDxChDiagDataIntegrityExtMemoryProtectionEventMask
*
* DESCRIPTION:
*       Function configures mask/unmask for non DFX protected memories.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - device number
*       memType - memory type
*       errorType - error type
*       operation - mask/unmask operation
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS prvCpssDxChDiagDataIntegrityExtMemoryProtectionEventMask
(
    IN  GT_U8                                           devNum,
    IN  CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT      memType,
    IN  CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT   errorType,
    IN  CPSS_EVENT_MASK_SET_ENT                         operation
)
{
    GT_STATUS   rc; /* return code */
    GT_BOOL     intEnable; /* interrupt enable/disable */
    GT_U32      intIndex; /* HW interrupt index */
    GT_32       intKey;   /* interrupt key */

    intEnable = (operation == CPSS_EVENT_UNMASK_E) ? GT_FALSE : GT_TRUE;

    switch(memType)
    {
        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ROUTER_TCAM_E:
            intIndex = PRV_CPSS_LION2_TCC_UPPER_GEN_TCAM_ERROR_DETECTED_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_POLICY_TCAM_E:
            intIndex = PRV_CPSS_LION2_TCC_LOWER_GEN_TCAM_ERROR_DETECTED_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXDMA_HA_INFO_DESC_PREFETCH_E:
            if(errorType == CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E)
            {
                intIndex = PRV_CPSS_LION2_TXDMA_ERROR_HA_INFO_FIFO_ECC_ONE_ERROR_E;
            }
            else if(errorType == CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E)
            {
                intIndex = PRV_CPSS_LION2_TXDMA_ERROR_HA_INFO_FIFO_ECC_TWO_OR_MORE_ERRORS_E;
            }
            else
            {
                intIndex = PRV_CPSS_LION2_TXDMA_ERROR_HA_INFO_FIFO_ECC_ONE_ERROR_E;
                (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &intKey);
                rc = prvCpssDrvEventsMask(devNum, intIndex, intEnable);
                (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
                if(GT_OK != rc)
                {
                    return rc;
                }

                intIndex = PRV_CPSS_LION2_TXDMA_ERROR_HA_INFO_FIFO_ECC_TWO_OR_MORE_ERRORS_E;
            }
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_EGR_MC_FIFO_0_E:
            /* TxQ EGR 0 */
            intIndex = PRV_CPSS_LION2_TXQ_EGRESS_0_GEN_MC_FIFO_0_PARITY_ERROR_E;
            (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &intKey);
            rc = prvCpssDrvEventsMask(devNum, intIndex, intEnable);
            (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
            if(GT_OK != rc)
            {
                return rc;
            }

            /* TxQ EGR 1 */
            intIndex = PRV_CPSS_LION2_TXQ_EGRESS_1_GEN_MC_FIFO_0_PARITY_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_EGR_MC_FIFO_1_E:
            /* TxQ EGR 0 */
            intIndex = PRV_CPSS_LION2_TXQ_EGRESS_0_GEN_MC_FIFO_1_PARITY_ERROR_E;
            (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &intKey);
            rc = prvCpssDrvEventsMask(devNum, intIndex, intEnable);
            (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
            if(GT_OK != rc)
            {
                return rc;
            }

            /* TxQ EGR 1 */
            intIndex = PRV_CPSS_LION2_TXQ_EGRESS_1_GEN_MC_FIFO_1_PARITY_ERROR_E;

            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_EGR_MC_FIFO_3_E:
            /* TxQ EGR 0 */

            /* false alarm for IPMC routed packets */
            intIndex = PRV_CPSS_LION2_TXQ_EGRESS_0_GEN_MC_FIFO_3_PARITY_ERROR_E;
            (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &intKey);
            rc = prvCpssDrvEventsMask(devNum, intIndex, GT_TRUE);
            (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
            if(GT_OK != rc)
            {
                return rc;
            }

            /* TxQ EGR 1 */
            intIndex = PRV_CPSS_LION2_TXQ_EGRESS_1_GEN_MC_FIFO_3_PARITY_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_EGR_VLAN_E:
            intIndex = PRV_CPSS_LION2_TXQ_SHT_GEN_EGR_VLAN_TBL_PARITY_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_ING_VLAN_E:
            intIndex = PRV_CPSS_LION2_TXQ_SHT_GEN_ING_VLAN_TBL_PARITY_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_PORT_ISO_L2_E:
            intIndex = PRV_CPSS_LION2_TXQ_SHT_GEN_L2_PORT_ISOLATION_TBL_PARITY_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_PORT_ISO_L3_E:
            intIndex = PRV_CPSS_LION2_TXQ_SHT_GEN_L3_PORT_ISOLATION_TBL_PARITY_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_VIDX_E:
            intIndex = PRV_CPSS_LION2_TXQ_SHT_GEN_VIDX_TBL_PARITY_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PLR_DESCRIPTOR_FIFO_E:
            /* ingress PLR0 */
            intIndex = PRV_CPSS_LION2_IPLR0_DATA_ERROR_E;

            (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &intKey);
            rc = prvCpssDrvEventsMask(devNum, intIndex, intEnable);
            (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
            if(GT_OK != rc)
            {
                return rc;
            }

            /* ingress PLR1 */
            intIndex = PRV_CPSS_LION2_IPLR1_DATA_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IP_NHE_TABLE_E:
            intIndex = PRV_CPSS_LION2_IP_ROUTER_NHE_DIP_DATA_ERROR_E;
            (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &intKey);
            rc = prvCpssDrvEventsMask(devNum, intIndex, intEnable);
            (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
            if(GT_OK != rc)
            {
                return rc;
            }

            intIndex = PRV_CPSS_LION2_IP_ROUTER_NHE_SIP_DATA_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IP_STG2_DESC_RETURN_TWO_FIFO_E:
            intIndex = PRV_CPSS_LION2_IP_ROUTER_STG2_DATA_FIFO_ECC_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IP_STG4_DESC_RETURN_TWO_FIFO_E:
            intIndex = PRV_CPSS_LION2_IP_ROUTER_STG4_DATA_FIFO_ECC_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IP_UNUSED_DATA_FIFO_E:
            intIndex = PRV_CPSS_LION2_IP_ROUTER_UNUSED_DATA_FIFO_ECC_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_DATA_BUFFER_MEMORY_E:
        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_DATA_BUFFER_MEMORY_ECC_E:
            if(errorType == CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E)
            {
                intIndex = PRV_CPSS_LION2_MPPM_0_BK0_ECC_1_ERROR_E;
                (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &intKey);
                rc = prvCpssDrvEventsMask(devNum, intIndex, intEnable);
                if(GT_OK != rc)
                {
                    (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
                    return rc;
                }

                intIndex = PRV_CPSS_LION2_MPPM_0_BK1_ECC_1_ERROR_E;
                rc  = prvCpssDrvEventsMask(devNum, intIndex, intEnable);
                if(GT_OK != rc)
                {
                    (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
                    return rc;
                }

                intIndex = PRV_CPSS_LION2_MPPM_1_BK0_ECC_1_ERROR_E;
                rc = prvCpssDrvEventsMask(devNum, intIndex, intEnable);
                if(GT_OK != rc)
                {
                    (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
                    return rc;
                }

                intIndex = PRV_CPSS_LION2_MPPM_1_BK1_ECC_1_ERROR_E;
                rc = prvCpssDrvEventsMask(devNum, intIndex, intEnable);
                (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
                if(GT_OK != rc)
                {
                    return rc;
                }
                return GT_OK;
            }
            else if(errorType == CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E)
            {
                intIndex = PRV_CPSS_LION2_MPPM_0_BK0_ECC_2_OR_MORE_ERROR_E;
                (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &intKey);
                rc = prvCpssDrvEventsMask(devNum, intIndex, intEnable);
                if(GT_OK != rc)
                {
                    (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
                    return rc;
                }

                intIndex = PRV_CPSS_LION2_MPPM_0_BK1_ECC_2_OR_MORE_ERROR_E;
                rc = prvCpssDrvEventsMask(devNum, intIndex, intEnable);
                if(GT_OK != rc)
                {
                    (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
                    return rc;
                }

                intIndex = PRV_CPSS_LION2_MPPM_1_BK0_ECC_2_OR_MORE_ERROR_E;
                rc = prvCpssDrvEventsMask(devNum, intIndex, intEnable);
                if(GT_OK != rc)
                {
                    (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
                    return rc;
                }

                intIndex = PRV_CPSS_LION2_MPPM_1_BK1_ECC_2_OR_MORE_ERROR_E;
                rc = prvCpssDrvEventsMask(devNum, intIndex, intEnable);
                (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
                if(GT_OK != rc)
                {
                    return rc;
                }
                return GT_OK;
            }
            else /* CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_AND_MULTIPLE_ECC_E */
            {
                rc = prvCpssDxChDiagDataIntegrityExtMemoryProtectionEventMask(devNum, memType, CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E, operation);
                if(GT_OK != rc)
                {
                    return rc;
                }

                return prvCpssDxChDiagDataIntegrityExtMemoryProtectionEventMask(devNum, memType, CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E, operation);
            }

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_DQ_TB_GIGA_PRIO_E:

            /* MASK false interrupt */
            intEnable = GT_TRUE;
            intIndex = PRV_CPSS_LION2_TXQ_DQ_MEMORY_ERROR_TOKEN_BUCKET_PRIO_PARITY_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_DQ_WRR_STATE_VARIABLES_E:
            intIndex = PRV_CPSS_LION2_TXQ_DQ_MEMORY_ERROR_STATE_VARIABLE_PARITY_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_LL_FREE_BUFS_E:
            intIndex = PRV_CPSS_LION2_TXQ_LL_GEN_FBUF_ECC_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_BUFFER_FIFO_E:
            intIndex = PRV_CPSS_LION2_TXQ_QUEUE_QCN_CN_BUFFER_FIFO_PARITY_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_0_COUNTERS_E:
            intIndex = PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP0_COUNTERS_ERR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_1_COUNTERS_E:
            intIndex = PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP1_COUNTERS_ERR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_2_COUNTERS_E:
            intIndex = PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP2_COUNTERS_ERR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_3_COUNTERS_E:
            intIndex = PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP3_COUNTERS_ERR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_4_COUNTERS_E:
            intIndex = PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP4_COUNTERS_ERR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_5_COUNTERS_E:
            intIndex = PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP5_COUNTERS_ERR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_6_COUNTERS_E:
            intIndex = PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP6_COUNTERS_ERR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_7_COUNTERS_E:
            intIndex = PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP7_COUNTERS_ERR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_BMA_MULTICAST_COUNTERS_E:
            intIndex = PRV_CPSS_LION2_BMA_CORE0_MCCNT_PARITY_ERROR_E;
            (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &intKey);
            rc = prvCpssDrvEventsMask(devNum, intIndex, intEnable);
            if(GT_OK != rc)
            {
                (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
                return rc;
            }

            intIndex = PRV_CPSS_LION2_BMA_CORE1_MCCNT_PARITY_ERROR_E;
            rc = prvCpssDrvEventsMask(devNum, intIndex, intEnable);
            if(GT_OK != rc)
            {
                (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
                return rc;
            }

            intIndex = PRV_CPSS_LION2_BMA_CORE2_MCCNT_PARITY_ERROR_E;
            rc = prvCpssDrvEventsMask(devNum, intIndex, intEnable);
            if(GT_OK != rc)
            {
                (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
                return rc;
            }

            intIndex = PRV_CPSS_LION2_BMA_CORE3_MCCNT_PARITY_ERROR_E;
            rc = prvCpssDrvEventsMask(devNum, intIndex, intEnable);
            (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
            if(GT_OK != rc)
            {
                return rc;
            }
            return GT_OK;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &intKey);
    rc = prvCpssDrvEventsMask(devNum, intIndex, intEnable);
    (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
    if(GT_OK != rc)
    {
        return rc;
    }

    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChDiagDataIntegrityExtMemoryProtectionEventMask
*
* DESCRIPTION:
*       Function gets mask state for non DFX protected memories.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - device number
*       memType   - memory type
*
* OUTPUTS:
*       errorTypePtr - (pointer to) error type
*       operationPtr - (pointer to) mask/unmask operation
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS prvCpssDxChDiagDataIntegrityExtMemoryProtectionEventMaskGet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT      memType,
    OUT CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT   *errorTypePtr,
    OUT CPSS_EVENT_MASK_SET_ENT                         *operationPtr
)
{
    GT_BOOL     intEnable; /* interrupt enable/disable */
    GT_BOOL     intEnable1; /* interrupt enable/disable */
    GT_U32      intIndex; /* HW interrupt index */
    GT_32       intKey;   /* interrupt key */

    *errorTypePtr = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;

    switch(memType)
    {
        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ROUTER_TCAM_E:
            intIndex = PRV_CPSS_LION2_TCC_UPPER_GEN_TCAM_ERROR_DETECTED_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_POLICY_TCAM_E:
            intIndex = PRV_CPSS_LION2_TCC_LOWER_GEN_TCAM_ERROR_DETECTED_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXDMA_HA_INFO_DESC_PREFETCH_E:

            (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &intKey);

            intIndex = PRV_CPSS_LION2_TXDMA_ERROR_HA_INFO_FIFO_ECC_ONE_ERROR_E;
            prvCpssDrvEventsMaskGet(devNum, intIndex, &intEnable);

            intIndex = PRV_CPSS_LION2_TXDMA_ERROR_HA_INFO_FIFO_ECC_TWO_OR_MORE_ERRORS_E;
            prvCpssDrvEventsMaskGet(devNum, intIndex, &intEnable1);

            (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);

            /* reversed logic from MaskSet */
            if(intEnable == GT_TRUE)
            {
                if(intEnable1 == GT_TRUE)
                {
                    *errorTypePtr = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_AND_MULTIPLE_ECC_E;
                    *operationPtr = CPSS_EVENT_UNMASK_E;
                }
                else
                {
                    *errorTypePtr = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E;
                    *operationPtr = CPSS_EVENT_UNMASK_E;
                }
            }
            else
            {
                if(intEnable1 == GT_TRUE)
                {
                    *errorTypePtr = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E;
                    *operationPtr = CPSS_EVENT_UNMASK_E;
                }
                else
                {
                    *errorTypePtr = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_AND_MULTIPLE_ECC_E;
                    *operationPtr = CPSS_EVENT_MASK_E;
                }
            }
            return GT_OK;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_EGR_MC_FIFO_0_E:
            /* TxQ EGR 0 and TxQ EGR 1 - the same value */
            intIndex = PRV_CPSS_LION2_TXQ_EGRESS_0_GEN_MC_FIFO_0_PARITY_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_EGR_MC_FIFO_1_E:
            /* TxQ EGR 0 and TxQ EGR 1 - the same value */
            intIndex = PRV_CPSS_LION2_TXQ_EGRESS_0_GEN_MC_FIFO_1_PARITY_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_EGR_MC_FIFO_3_E:
            /* TxQ EGR 0 and TxQ EGR 1 - the same value */
            intIndex = PRV_CPSS_LION2_TXQ_EGRESS_0_GEN_MC_FIFO_3_PARITY_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_EGR_VLAN_E:
            intIndex = PRV_CPSS_LION2_TXQ_SHT_GEN_EGR_VLAN_TBL_PARITY_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_ING_VLAN_E:
            intIndex = PRV_CPSS_LION2_TXQ_SHT_GEN_ING_VLAN_TBL_PARITY_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_PORT_ISO_L2_E:
            intIndex = PRV_CPSS_LION2_TXQ_SHT_GEN_L2_PORT_ISOLATION_TBL_PARITY_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_PORT_ISO_L3_E:
            intIndex = PRV_CPSS_LION2_TXQ_SHT_GEN_L3_PORT_ISOLATION_TBL_PARITY_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_VIDX_E:
            intIndex = PRV_CPSS_LION2_TXQ_SHT_GEN_VIDX_TBL_PARITY_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PLR_DESCRIPTOR_FIFO_E:
            intIndex = PRV_CPSS_LION2_IPLR0_DATA_ERROR_E;
            *errorTypePtr = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_AND_MULTIPLE_ECC_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IP_NHE_TABLE_E:
            /* NHE_DIP_DATA and NHE_SIP_DATA - the same value */
            intIndex = PRV_CPSS_LION2_IP_ROUTER_NHE_DIP_DATA_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IP_STG2_DESC_RETURN_TWO_FIFO_E:
            intIndex = PRV_CPSS_LION2_IP_ROUTER_STG2_DATA_FIFO_ECC_ERROR_E;
            *errorTypePtr = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_AND_MULTIPLE_ECC_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IP_STG4_DESC_RETURN_TWO_FIFO_E:
            *errorTypePtr = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_AND_MULTIPLE_ECC_E;
            intIndex = PRV_CPSS_LION2_IP_ROUTER_STG4_DATA_FIFO_ECC_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IP_UNUSED_DATA_FIFO_E:
            *errorTypePtr = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_AND_MULTIPLE_ECC_E;
            intIndex = PRV_CPSS_LION2_IP_ROUTER_UNUSED_DATA_FIFO_ECC_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_DATA_BUFFER_MEMORY_E:
        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_DATA_BUFFER_MEMORY_ECC_E:

            (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &intKey);
            intIndex = PRV_CPSS_LION2_MPPM_0_BK0_ECC_1_ERROR_E;
            prvCpssDrvEventsMaskGet(devNum, intIndex, &intEnable);

            intIndex = PRV_CPSS_LION2_MPPM_0_BK0_ECC_2_OR_MORE_ERROR_E;
            prvCpssDrvEventsMaskGet(devNum, intIndex, &intEnable1);
            (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);

            /* reversed logic from MaskSet */
            if(intEnable == GT_TRUE)
            {
                if(intEnable1 == GT_TRUE)
                {
                    *errorTypePtr = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_AND_MULTIPLE_ECC_E;
                    *operationPtr = CPSS_EVENT_UNMASK_E;
                }
                else
                {
                    *errorTypePtr = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E;
                    *operationPtr = CPSS_EVENT_UNMASK_E;
                }
            }
            else
            {
                if(intEnable1 == GT_TRUE)
                {
                    *errorTypePtr = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E;
                    *operationPtr = CPSS_EVENT_UNMASK_E;
                }
                else
                {
                    *errorTypePtr = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_AND_MULTIPLE_ECC_E;
                    *operationPtr = CPSS_EVENT_MASK_E;
                }
            }
            return GT_OK;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_DQ_TB_GIGA_PRIO_E:
            intIndex = PRV_CPSS_LION2_TXQ_DQ_MEMORY_ERROR_TOKEN_BUCKET_PRIO_PARITY_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_DQ_WRR_STATE_VARIABLES_E:
            intIndex = PRV_CPSS_LION2_TXQ_DQ_MEMORY_ERROR_STATE_VARIABLE_PARITY_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_LL_FREE_BUFS_E:
            intIndex = PRV_CPSS_LION2_TXQ_LL_GEN_FBUF_ECC_ERROR_E;
            *errorTypePtr = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_AND_MULTIPLE_ECC_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_BUFFER_FIFO_E:
            intIndex = PRV_CPSS_LION2_TXQ_QUEUE_QCN_CN_BUFFER_FIFO_PARITY_ERROR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_0_COUNTERS_E:
            intIndex = PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP0_COUNTERS_ERR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_1_COUNTERS_E:
            intIndex = PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP1_COUNTERS_ERR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_2_COUNTERS_E:
            intIndex = PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP2_COUNTERS_ERR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_3_COUNTERS_E:
            intIndex = PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP3_COUNTERS_ERR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_4_COUNTERS_E:
            intIndex = PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP4_COUNTERS_ERR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_5_COUNTERS_E:
            intIndex = PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP5_COUNTERS_ERR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_6_COUNTERS_E:
            intIndex = PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP6_COUNTERS_ERR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_7_COUNTERS_E:
            intIndex = PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP7_COUNTERS_ERR_E;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_BMA_MULTICAST_COUNTERS_E:
            intIndex = PRV_CPSS_LION2_BMA_CORE0_MCCNT_PARITY_ERROR_E;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &intKey);
    prvCpssDrvEventsMaskGet(devNum, intIndex, &intEnable);
    (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);

    *operationPtr = (intEnable == GT_TRUE) ? CPSS_EVENT_UNMASK_E : CPSS_EVENT_MASK_E;

    return GT_OK;
}


/*******************************************************************************
* dataIntegrityDbCmp
*
* DESCRIPTION:
*       Function for compare two DB entries for binary search algorithm.
*
* INPUTS:
*       aPtr - (pointer to) key
*       bPtr - (pointer to) DB item key
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0   - equal
*       else - not equal
*
* COMMENTS:
*       None
*
*******************************************************************************/
static int dataIntegrityDbCmp
(
    IN const GT_VOID *aPtr,
    IN const GT_VOID *bPtr
)
{
    int temp1 = *((int*)aPtr);
    int temp2 = (int)(((PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_MAP_DB_STC*)bPtr)->key);

    return temp1 -  temp2;
}

/*******************************************************************************
* searchMemType
*
* DESCRIPTION:
*       Function performs binary search in map DB.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       dfxPipeIndex   - DFX pipe index
*       dfxClientIndex - DFX client index
*       dfxMemoryIndex - DFX memory index
*
* OUTPUTS:
*       dbItemPtrPtr   - (pointer to) DB entry
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS searchMemType
(
    IN  GT_U8                                           devNum,
    IN  GT_U32                                          dfxPipeIndex,
    IN  GT_U32                                          dfxClientIndex,
    IN  GT_U32                                          dfxMemoryIndex,
    OUT PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_MAP_DB_STC    **dbItemPtrPtr
)
{
    GT_STATUS                           rc;    /* return code */
    GT_U32                              key;   /* compare key */
    GT_U32                              itemSize; /* size of compared item */
    PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_MAP_DB_STC   *dbItemTmpPtr; /* pointer to found item */
    CPSS_OS_COMPARE_ITEMS_FUNC_PTR      cmpFuncPtr; /* pointer to compare function */
    PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_MAP_DB_STC    *dbArrayPtr; /* pointer to data integrity DB */
    GT_U32                                          dbArrayEntryNum; /* size of data integrity DB */

    prvCpssDxChDiagDataIntegrityDbPointerSet(devNum, &dbArrayPtr, &dbArrayEntryNum);

    cmpFuncPtr = dataIntegrityDbCmp;

    key = (dfxPipeIndex << 12) | (dfxClientIndex << 7) | (dfxMemoryIndex);

    itemSize = sizeof(PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_MAP_DB_STC);

    rc = cpssOsBsearch((void *)&key, (void *)dbArrayPtr, dbArrayEntryNum, itemSize, cmpFuncPtr, (void*)&dbItemTmpPtr);
    if(GT_OK != rc)
    {
        return rc;
    }

    *dbItemPtrPtr = dbItemTmpPtr;

    return GT_OK;
}


/*******************************************************************************
* buildDfxAddr
*
* DESCRIPTION:
*       Function calculates DFX client/memory address.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       addrStrPtr  - (pointer to) DFX address structure
*
* OUTPUTS:
*       addrPtr     - (pointer to) calculated address
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong addrStrPtr members
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS buildDfxAddr
(
    IN  PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_DFX_ADDR_STC  *addrStrPtr,
    OUT GT_U32                                          *addrPtr
)
{
    GT_U32 tmpData; /* local temp variable */

    tmpData = 0x18 << 20;

    if(addrStrPtr->dfxUnitClientIdx >= BIT_5)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* bits [19:15] */
    tmpData |= (addrStrPtr->dfxUnitClientIdx << 15);

    if(addrStrPtr->dfxUnitClientIdx == 31)
    {
        if(addrStrPtr->dfxServerRegisterOffset >= BIT_15)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        /* bits [14:0] */
        tmpData |= addrStrPtr->dfxServerRegisterOffset;
    }
    else if(/* (addrStrPtr->dfxUnitClientIdx >= 0) && */(addrStrPtr->dfxUnitClientIdx <= 29))
    {
        /* bit [14] */
        tmpData |= (addrStrPtr->dfxXsbSelect << 14);

        if(addrStrPtr->dfxXsbSelect == 1)
        {
            if((addrStrPtr->dfxXsbNumber >= BIT_6) || (addrStrPtr->dfxXsbRegOffset >= BIT_9))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            /* bits [8:0] */
            tmpData |= addrStrPtr->dfxXsbRegOffset;
            /* bits [13:9] */
            tmpData |= (addrStrPtr->dfxXsbNumber) << 9;
        }
        else
        {
            /* bit [13] */
            tmpData |= (addrStrPtr->dfxClientSelect << 13);
            if(addrStrPtr->dfxClientSelect == 1)
            {
                if(addrStrPtr->dfxClientRegisterOffset >= BIT_13)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                /* bits [12:0] */
                tmpData |= (addrStrPtr->dfxClientRegisterOffset);
            }
            else
            {
                if((addrStrPtr->dfxClientMemoryNumber >= BIT_7) || (addrStrPtr->dfxClientMemoryRegisterOffset >= BIT_6))
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                /* bits [12:6] */
                tmpData |= (addrStrPtr->dfxClientMemoryNumber) << 6;
                /* bits [5:0] */
                tmpData |= (addrStrPtr->dfxClientMemoryRegisterOffset);
            }
        }
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    *addrPtr = tmpData;

    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChDiagDataIntegrityEventGetLion2
*
* DESCRIPTION:
*       Function performs DFX interrupt tree scan for Lion2.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - PP device number
*       evExtData     - event external data
*       eventsNumPtr  - (pointer to) max num of enabled events to
*                                retrieve - this value refer to the number of
*                                members that the array of eventsTypeArr[] and
*                                memTypeArr[] can retrieve.
*
* OUTPUTS:
*       eventsNumPtr      - (pointer to) the actual num of found events
*       eventsArr         - (array of) events
*       isNoMoreEventsPtr - (pointer to) status of events scan process
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_STATE - illegal HW state
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS prvCpssDxChDiagDataIntegrityEventGetLion2
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      evExtData,
    INOUT GT_U32                                    *eventsNumPtr,
    OUT CPSS_DXCH_DIAG_DATA_INTEGRITY_EVENT_STC     eventsArr[],
    OUT GT_BOOL                                     *isNoMoreEventsPtr
)
{
    GT_STATUS rc;    /* return code */
    GT_U32 regAddr;     /* register address */
    GT_U32 regData;     /* register data */
    GT_U32 regData1;    /* register data */
    GT_U32 ruleOffset;  /* variable for policy rule offset calculations */
    GT_U32 i, j, k;     /* loop iterators */
    GT_U32 pipeIndex;   /* pipe index */
    GT_U32 clientIndex; /* client index */
    GT_U32 memoryNumber;/* memory index */
    PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_DFX_ADDR_STC      addrStr;    /* DFX address struct */
    PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_CLIENT_STATUS_STC clientBmp = {{0,0,0,0}};  /* client status struct */
    PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_MAP_DB_STC        *dbItemPtr; /* pointer to map DB item */
    GT_U32 eventCounter = 0; /* found event counter */
    GT_U32 portGroupId; /* port group id */
    GT_U32 eventAdditionInfo; /* event additional info - HW interrupt code */
    PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_FIRST_STAGE_MAPPING_DATA_STC (*firstStageMappingArrayPtr)[MAX_DFX_INT_CAUSE_NUM_CNS];

    portGroupId = evExtData >> 16;
    eventAdditionInfo = evExtData & 0xFFFF;

    switch (PRV_CPSS_PP_MAC(devNum)->devType)
    {
        case CPSS_LION2_HOOPER_PORT_GROUPS_0123_DEVICES_CASES_MAC:
            firstStageMappingArrayPtr = hooperDataIntegrityFirstStageMappingArray;
            break;
        default:
            firstStageMappingArrayPtr = lion2DataIntegrityFirstStageMappingArray;
            break;
    }

    /* NON DFX memories */
    if(eventAdditionInfo != 0)
    {
        *isNoMoreEventsPtr = GT_TRUE;

        switch(eventAdditionInfo)
        {
            case PRV_CPSS_LION2_TCC_LOWER_GEN_TCAM_ERROR_DETECTED_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_POLICY_TCAM_E;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;

                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.pclTccRegs.policyTccErrorInformation;
                rc = prvCpssHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &regData);
                if(GT_OK != rc)
                {
                    return rc;
                }

                eventsArr[0].location.tcamMemLocation.arrayType = (U32_GET_FIELD_MAC(regData, 30, 1) == 0) ? CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_X_E : CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_Y_E;

                if(U32_GET_FIELD_MAC(regData, 12, 4) != 0)
                {
                    ruleOffset = 3;
                }
                else if(U32_GET_FIELD_MAC(regData, 8, 4) != 0)
                {
                    ruleOffset = 2;
                }
                else if(U32_GET_FIELD_MAC(regData, 4, 4) != 0)
                {
                    ruleOffset = 1;
                }
                else
                {
                    ruleOffset = 0;
                }
                eventsArr[0].location.tcamMemLocation.ruleIndex = (U32_GET_FIELD_MAC(regData, 16, 14)) * 4 + ruleOffset;

                return GT_OK;

            case PRV_CPSS_LION2_TCC_UPPER_GEN_TCAM_ERROR_DETECTED_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ROUTER_TCAM_E;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;

                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.ipTccRegs.ipTccErrorInformation;
                rc = prvCpssHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &regData);
                if(GT_OK != rc)
                {
                    return rc;
                }

                eventsArr[0].location.tcamMemLocation.arrayType = (U32_GET_FIELD_MAC(regData, 30, 1) == 0) ? CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_X_E : CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_Y_E;
                eventsArr[0].location.tcamMemLocation.ruleIndex = U32_GET_FIELD_MAC(regData, 16, 14);
                return GT_OK;

            case PRV_CPSS_LION2_TXDMA_ERROR_HA_INFO_FIFO_ECC_ONE_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXDMA_HA_INFO_DESC_PREFETCH_E;

                break;

            case PRV_CPSS_LION2_TXDMA_ERROR_HA_INFO_FIFO_ECC_TWO_OR_MORE_ERRORS_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXDMA_HA_INFO_DESC_PREFETCH_E;
                break;

            case PRV_CPSS_LION2_TXQ_EGRESS_0_GEN_MC_FIFO_0_PARITY_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_EGR_MC_FIFO_0_E;
                break;

            case PRV_CPSS_LION2_TXQ_EGRESS_1_GEN_MC_FIFO_0_PARITY_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_EGR_MC_FIFO_0_E;
                break;

            case PRV_CPSS_LION2_TXQ_EGRESS_0_GEN_MC_FIFO_1_PARITY_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_EGR_MC_FIFO_1_E;

                break;

            case PRV_CPSS_LION2_TXQ_EGRESS_1_GEN_MC_FIFO_1_PARITY_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_EGR_MC_FIFO_1_E;
                break;

            case PRV_CPSS_LION2_TXQ_EGRESS_0_GEN_MC_FIFO_3_PARITY_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_EGR_MC_FIFO_3_E;
                break;

            case PRV_CPSS_LION2_TXQ_EGRESS_1_GEN_MC_FIFO_3_PARITY_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_EGR_MC_FIFO_3_E;
                break;

            case PRV_CPSS_LION2_TXQ_SHT_GEN_EGR_VLAN_TBL_PARITY_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_EGR_VLAN_E;
                break;

            case PRV_CPSS_LION2_TXQ_SHT_GEN_ING_VLAN_TBL_PARITY_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_ING_VLAN_E;
                break;

            case PRV_CPSS_LION2_TXQ_SHT_GEN_L2_PORT_ISOLATION_TBL_PARITY_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_PORT_ISO_L2_E;
                break;

            case PRV_CPSS_LION2_TXQ_SHT_GEN_L3_PORT_ISOLATION_TBL_PARITY_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_PORT_ISO_L3_E;
                break;

            case PRV_CPSS_LION2_TXQ_SHT_GEN_VIDX_TBL_PARITY_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_VIDX_E;
                break;

            case PRV_CPSS_LION2_IPLR0_DATA_ERROR_E:
            case PRV_CPSS_LION2_IPLR1_DATA_ERROR_E:

                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;

                if(eventAdditionInfo == PRV_CPSS_LION2_IPLR0_DATA_ERROR_E)
                {
                    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[0].policerErrorReg;
                }
                else
                {
                    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[1].policerErrorReg;
                }

                rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, 24, 2, &regData);
                if(GT_OK != rc)
                {
                    return rc;
                }

                if(regData == 1)
                {
                    eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E;
                }
                else if(regData == 2)
                {
                    eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E;
                }
                else
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }

                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PLR_DESCRIPTOR_FIFO_E;

                if(eventAdditionInfo == PRV_CPSS_LION2_IPLR0_DATA_ERROR_E)
                {
                    eventsArr[0].location.memLocation.dfxPipeId = portGroupId; /* por PLR PipeId = PortGroupId */
                    eventsArr[0].location.memLocation.dfxClientId = 8;
                    eventsArr[0].location.memLocation.dfxMemoryId = 34;
                }
                else
                {
                    eventsArr[0].location.memLocation.dfxPipeId = portGroupId; /* por PLR PipeId = PortGroupId */
                    eventsArr[0].location.memLocation.dfxClientId = 8;
                    eventsArr[0].location.memLocation.dfxMemoryId = 32;
                }

                return GT_OK;

            case PRV_CPSS_LION2_IP_ROUTER_NHE_DIP_DATA_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IP_NHE_TABLE_E;
                break;

            case PRV_CPSS_LION2_IP_ROUTER_NHE_SIP_DATA_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IP_NHE_TABLE_E;
                break;

            case PRV_CPSS_LION2_IP_ROUTER_STG2_DATA_FIFO_ECC_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;

                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerDataError;
                rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, 18, 2, &regData);
                if(GT_OK != rc)
                {
                    return rc;
                }

                if(regData == 1)
                {
                    eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E;
                }
                else if(regData == 2)
                {
                    eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E;
                }
                else
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }

                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IP_STG2_DESC_RETURN_TWO_FIFO_E;
                break;

            case PRV_CPSS_LION2_IP_ROUTER_STG4_DATA_FIFO_ECC_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;

                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerDataError;
                rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, 20, 2, &regData);
                if(GT_OK != rc)
                {
                    return rc;
                }

                if(regData == 1)
                {
                    eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E;
                }
                else if(regData == 2)
                {
                    eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E;
                }
                else
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }

                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IP_STG4_DESC_RETURN_TWO_FIFO_E;
                break;

            case PRV_CPSS_LION2_IP_ROUTER_UNUSED_DATA_FIFO_ECC_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;

                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerDataError;
                rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, 16, 2, &regData);
                if(GT_OK != rc)
                {
                    return rc;
                }

                if(regData == 1)
                {
                    eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E;
                }
                else if(regData == 2)
                {
                    eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E;
                }
                else
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }

                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IP_UNUSED_DATA_FIFO_E;
                break;

            /* all MPPM cases should return here because of different location structure */
            case PRV_CPSS_LION2_MPPM_0_BK0_ECC_1_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_DATA_BUFFER_MEMORY_E;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E;
                eventsArr[0].location.mppmMemLocation.portGroupId = portGroupId;
                eventsArr[0].location.mppmMemLocation.mppmId = 0;
                eventsArr[0].location.mppmMemLocation.bankId = 0;
                return GT_OK;

            case PRV_CPSS_LION2_MPPM_0_BK1_ECC_1_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_DATA_BUFFER_MEMORY_E;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E;
                eventsArr[0].location.mppmMemLocation.portGroupId = portGroupId;
                eventsArr[0].location.mppmMemLocation.mppmId = 0;
                eventsArr[0].location.mppmMemLocation.bankId = 1;
                return GT_OK;

            case PRV_CPSS_LION2_MPPM_1_BK0_ECC_1_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_DATA_BUFFER_MEMORY_E;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E;
                eventsArr[0].location.mppmMemLocation.portGroupId = portGroupId;
                eventsArr[0].location.mppmMemLocation.mppmId = 1;
                eventsArr[0].location.mppmMemLocation.bankId = 0;
                return GT_OK;

            case PRV_CPSS_LION2_MPPM_1_BK1_ECC_1_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_DATA_BUFFER_MEMORY_E;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E;
                eventsArr[0].location.mppmMemLocation.portGroupId = portGroupId;
                eventsArr[0].location.mppmMemLocation.mppmId = 1;
                eventsArr[0].location.mppmMemLocation.bankId = 1;
                return GT_OK;

            case PRV_CPSS_LION2_MPPM_0_BK0_ECC_2_OR_MORE_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_DATA_BUFFER_MEMORY_E;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E;
                eventsArr[0].location.mppmMemLocation.portGroupId = portGroupId;
                eventsArr[0].location.mppmMemLocation.mppmId = 0;
                eventsArr[0].location.mppmMemLocation.bankId = 0;
                return GT_OK;

            case PRV_CPSS_LION2_MPPM_0_BK1_ECC_2_OR_MORE_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_DATA_BUFFER_MEMORY_E;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E;
                eventsArr[0].location.mppmMemLocation.portGroupId = portGroupId;
                eventsArr[0].location.mppmMemLocation.mppmId = 0;
                eventsArr[0].location.mppmMemLocation.bankId = 1;
                return GT_OK;

            case PRV_CPSS_LION2_MPPM_1_BK0_ECC_2_OR_MORE_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_DATA_BUFFER_MEMORY_E;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E;
                eventsArr[0].location.mppmMemLocation.portGroupId = portGroupId;
                eventsArr[0].location.mppmMemLocation.mppmId = 1;
                eventsArr[0].location.mppmMemLocation.bankId = 0;
                return GT_OK;

            case PRV_CPSS_LION2_MPPM_1_BK1_ECC_2_OR_MORE_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_DATA_BUFFER_MEMORY_E;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E;
                eventsArr[0].location.mppmMemLocation.portGroupId = portGroupId;
                eventsArr[0].location.mppmMemLocation.mppmId = 1;
                eventsArr[0].location.mppmMemLocation.bankId = 1;
                return GT_OK;

            case PRV_CPSS_LION2_TXQ_DQ_MEMORY_ERROR_TOKEN_BUCKET_PRIO_PARITY_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_DQ_TB_GIGA_PRIO_E;
                break;

            case PRV_CPSS_LION2_TXQ_DQ_MEMORY_ERROR_STATE_VARIABLE_PARITY_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_DQ_WRR_STATE_VARIABLES_E;
                break;

            case PRV_CPSS_LION2_TXQ_LL_GEN_FBUF_ECC_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;

                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.ll.linkList.linkListEccControl.fbufRamEccStatus;
                rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, 12, 2, &regData);
                if(GT_OK != rc)
                {
                    return rc;
                }

                if(regData == 1)
                {
                    eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E;
                }
                else if(regData == 2)
                {
                    eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E;
                }
                else
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }

                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_LL_FREE_BUFS_E;
                break;

            case PRV_CPSS_LION2_TXQ_QUEUE_QCN_CN_BUFFER_FIFO_PARITY_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_BUFFER_FIFO_E;
                break;

            case PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP0_COUNTERS_ERR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_0_COUNTERS_E;
                break;

            case PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP1_COUNTERS_ERR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_1_COUNTERS_E;
                break;

            case PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP2_COUNTERS_ERR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_2_COUNTERS_E;
                break;

            case PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP3_COUNTERS_ERR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_3_COUNTERS_E;
                break;

            case PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP4_COUNTERS_ERR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_4_COUNTERS_E;
                break;

            case PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP5_COUNTERS_ERR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_5_COUNTERS_E;
                break;

            case PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP6_COUNTERS_ERR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_6_COUNTERS_E;
                break;

            case PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP7_COUNTERS_ERR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_7_COUNTERS_E;
                break;

            case PRV_CPSS_LION2_BMA_CORE0_MCCNT_PARITY_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_BMA_MULTICAST_COUNTERS_E;
                eventsArr[0].location.memLocation.dfxPipeId = 4;
                eventsArr[0].location.memLocation.dfxClientId = 1;
                eventsArr[0].location.memLocation.dfxMemoryId = 51;
                return GT_OK;

            case PRV_CPSS_LION2_BMA_CORE1_MCCNT_PARITY_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_BMA_MULTICAST_COUNTERS_E;
                eventsArr[0].location.memLocation.dfxPipeId = 4;
                eventsArr[0].location.memLocation.dfxClientId = 1;
                eventsArr[0].location.memLocation.dfxMemoryId = 43;
                return GT_OK;

            case PRV_CPSS_LION2_BMA_CORE2_MCCNT_PARITY_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_BMA_MULTICAST_COUNTERS_E;
                eventsArr[0].location.memLocation.dfxPipeId = 4;
                eventsArr[0].location.memLocation.dfxClientId = 1;
                eventsArr[0].location.memLocation.dfxMemoryId = 35;
                return GT_OK;

            case PRV_CPSS_LION2_BMA_CORE3_MCCNT_PARITY_ERROR_E:
                *eventsNumPtr = 1;
                eventsArr[0].causePortGroupId = portGroupId;
                eventsArr[0].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                eventsArr[0].memType = CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_BMA_MULTICAST_COUNTERS_E;
                eventsArr[0].location.memLocation.dfxPipeId = 4;
                eventsArr[0].location.memLocation.dfxClientId = 1;
                eventsArr[0].location.memLocation.dfxMemoryId = 27;
                return GT_OK;

            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        rc = prvCpssDxChDiagDataIntegrityPortGroupMemoryIndexesGet(devNum,
                                                                   eventsArr[0].causePortGroupId,
                                                                   eventsArr[0].memType,
                                                                   &(eventsArr[0].location.memLocation));
        return rc;
    }


    /* DFX Interrupts Summary Cause */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.globalInterrupt.dfxInterruptCause;

    rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, 0, 27, &regData);
    if(GT_OK != rc)
    {
        return rc;
    }

    /* Ignore bits [1:0] */
    regData &= ~0x3;

    *isNoMoreEventsPtr = GT_TRUE;

    for(i = 2; i <= 26; i++)
    {
        /* ECC/Parity Interrupt Cause Register scanning algorithm */
        /*
             - Read DFX Interrupt cause register
             - For every bit which is set - find in first stage map array DFX pipe index and Client index.
             - Set "Pipe Select" register according to DFX pipe index
             - Read 4 memory bitmap registers according to Client index in current DFX pipe.
             - For every bit which is set (memory number) calculate interrupt cause register address:
                    pipe index + client index + memory number => cause/mask register address.
             - In order to return CPSS Table + entry number date go to second stage map array to make conversion.
        */

        if(U32_GET_FIELD_MAC(regData, i, 1))
        {
            pipeIndex = firstStageMappingArrayPtr[portGroupId][i].dfxPipeIndex;
            clientIndex = firstStageMappingArrayPtr[portGroupId][i].dfxClientIndex;

            if((pipeIndex == DATA_INTEGRITY_ENTRY_NOT_USED_CNS) || (clientIndex == DATA_INTEGRITY_ENTRY_NOT_USED_CNS))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }

            /* Set Pipe Select Register */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->dfxUnits.server.pipeSelect;
            rc = prvCpssHwPpWriteRegister(devNum, regAddr, (1 << pipeIndex));
            if(GT_OK != rc)
            {
                return rc;
            }

            /* Read DFX Client Memory BitMap 0-3 */
            addrStr.dfxUnitClientIdx = clientIndex;
            addrStr.dfxXsbSelect = 0;
            addrStr.dfxClientSelect = 1;

            for(k = 0; k < 4; k++)
            {
                addrStr.dfxClientRegisterOffset = PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_ADDR_TYPE_DFX_CLIENT_INT_STATUS0_REG_CNS + k * 4;

                rc = buildDfxAddr(&addrStr, &regAddr);
                if(GT_OK != rc)
                {
                    return rc;
                }
                rc = prvCpssHwPpReadRegister(devNum, regAddr, &(clientBmp.bitmap[k]));
                if(GT_OK != rc)
                {
                    return rc;
                }
            }

            for(k = 0; k < 4; k++)
            {
                for(j = 0; j < 32; j++)
                {
                    if(U32_GET_FIELD_MAC(clientBmp.bitmap[k], j, 1))
                    {
                        memoryNumber = k*32 + j;

                        /* Read Memory Interrupt Cause Register */
                        addrStr.dfxUnitClientIdx = clientIndex;
                        addrStr.dfxXsbSelect = 0;
                        addrStr.dfxClientSelect = 0;
                        addrStr.dfxClientMemoryNumber = memoryNumber;
                        addrStr.dfxClientMemoryRegisterOffset = PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_ADDR_TYPE_MEMORY_INT_CAUSE_REG_CNS;

                        rc = buildDfxAddr(&addrStr, &regAddr);
                        if(GT_OK != rc)
                        {
                            return rc;
                        }

                        rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 4, &regData);
                        if(GT_OK != rc)
                        {
                            return rc;
                        }

                        rc = searchMemType(devNum, pipeIndex, clientIndex, memoryNumber, &dbItemPtr);
                        if(GT_OK != rc)
                        {
                            return rc;
                        }

                        /* update output parameters */
                        eventsArr[eventCounter].memType = dbItemPtr->memType;
                        eventsArr[eventCounter].causePortGroupId = dbItemPtr->causePortGroupId;
                        eventsArr[eventCounter].location.memLocation.dfxPipeId = pipeIndex;
                        eventsArr[eventCounter].location.memLocation.dfxClientId = clientIndex;
                        eventsArr[eventCounter].location.memLocation.dfxMemoryId = memoryNumber;

                        if(U32_GET_FIELD_MAC(regData, 1, 1))
                        {
                            eventsArr[eventCounter].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                        }
                        else if(U32_GET_FIELD_MAC(regData, 2, 1))
                        {
                            eventsArr[eventCounter].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E;
                        }
                        else if(U32_GET_FIELD_MAC(regData, 3, 1))
                        {
                            eventsArr[eventCounter].eventsType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E;
                        }
                        else
                        {
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                        }

                        if(eventCounter == (*eventsNumPtr - 1))
                        {
                            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.globalInterrupt.dfxInterruptCause;

                            rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, 31, 1, &regData);
                            if(GT_OK != rc)
                            {
                                return rc;
                            }

                            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.globalInterrupt.dfxInterruptMask;

                            rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, 31, 1, &regData1);
                            if(GT_OK != rc)
                            {
                                return rc;
                            }


                            if((regData & regData1) != 0)
                            {
                                *isNoMoreEventsPtr = GT_FALSE;
                            }

                            return GT_OK;
                        }

                        eventCounter++;
                    }
                }
            }

        }
    }
    *eventsNumPtr = eventCounter;

    return GT_OK;
}


/*******************************************************************************
* internal_cpssDxChDiagDataIntegrityEventsGet
*
* DESCRIPTION:
*       Function returns array of data integrity events.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - PP device number
*       evExtData     - event external data
*       eventsNumPtr  - (pointer to) max num of enabled events to
*                                retrieve - this value refer to the number of
*                                members that the array of eventsTypeArr[] and
*                                memTypeArr[] can retrieve.
*
* OUTPUTS:
*       eventsNumPtr      - (pointer to) the actual num of found events
*       eventsArr         - array of ECC/parity events
*       isNoMoreEventsPtr - (pointer to) status of events scan process
*                              GT_TRUE - no more events found
*                              GT_FALSE - there are more events found
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, portGroup
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChDiagDataIntegrityEventsGet
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      evExtData,
    INOUT GT_U32                                    *eventsNumPtr,
    OUT CPSS_DXCH_DIAG_DATA_INTEGRITY_EVENT_STC     eventsArr[],
    OUT GT_BOOL                                     *isNoMoreEventsPtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
                                            CPSS_XCAT2_E | CPSS_LION_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);
    CPSS_NULL_PTR_CHECK_MAC(eventsNumPtr);
    CPSS_NULL_PTR_CHECK_MAC(eventsArr);
    CPSS_NULL_PTR_CHECK_MAC(isNoMoreEventsPtr);

    return prvCpssDxChDiagDataIntegrityEventGetLion2(devNum, evExtData, eventsNumPtr, eventsArr, isNoMoreEventsPtr);
}

/*******************************************************************************
* cpssDxChDiagDataIntegrityEventsGet
*
* DESCRIPTION:
*       Function returns array of data integrity events.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - PP device number
*       evExtData     - event external data
*       eventsNumPtr  - (pointer to) max num of enabled events to
*                                retrieve - this value refer to the number of
*                                members that the array of eventsTypeArr[] and
*                                memTypeArr[] can retrieve.
*
* OUTPUTS:
*       eventsNumPtr      - (pointer to) the actual num of found events
*       eventsArr         - array of ECC/parity events
*       isNoMoreEventsPtr - (pointer to) status of events scan process
*                              GT_TRUE - no more events found
*                              GT_FALSE - there are more events found
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, portGroup
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChDiagDataIntegrityEventsGet
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      evExtData,
    INOUT GT_U32                                    *eventsNumPtr,
    OUT CPSS_DXCH_DIAG_DATA_INTEGRITY_EVENT_STC     eventsArr[],
    OUT GT_BOOL                                     *isNoMoreEventsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChDiagDataIntegrityEventsGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, evExtData, eventsNumPtr, eventsArr, isNoMoreEventsPtr));

    rc = internal_cpssDxChDiagDataIntegrityEventsGet(devNum, evExtData, eventsNumPtr, eventsArr, isNoMoreEventsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, evExtData, eventsNumPtr, eventsArr, isNoMoreEventsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChDiagDataIntegrityEventMaskSet
*
* DESCRIPTION:
*       Function sets mask/unmask for ECC/Parity event.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - PP device number
*       memType     - type of memory(table)
*       errorType   - type of error interrupt
*                     relevant only for ECC protected memories
*       operation   - mask/unmask interrupt
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, portGroup, memType, errorType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChDiagDataIntegrityEventMaskSet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT      memType,
    IN  CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT   errorType,
    IN  CPSS_EVENT_MASK_SET_ENT                         operation
)
{
    GT_STATUS                   rc;          /* return code */
    GT_U32                      regAddr;     /* register address */
    GT_U32                      regData;     /* register data */
    GT_U32                      pipeIndex;   /* pipe index */
    GT_U32                      clientIndex; /* client index */
    GT_U32                      fieldOffset; /* register field offset */
    GT_U32                      fieldLength; /* register field length */
    GT_U32                      i;           /* loop iterator */
    PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_DFX_ADDR_STC  addrStr;     /* DFX address struct */
    GT_BOOL                     memoryFound = GT_FALSE; /* memory found status */
    PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_MAP_DB_STC    *dbArrayPtr; /* pointer to data integrity DB */
    GT_U32                                          dbArrayEntryNum; /* size of data integrity DB */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
                                            CPSS_XCAT2_E | CPSS_LION_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    if(operation != CPSS_EVENT_MASK_E &&
       operation != CPSS_EVENT_UNMASK_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    prvCpssDxChDiagDataIntegrityDbPointerSet(devNum, &dbArrayPtr, &dbArrayEntryNum);

    for(i = 0; i < dbArrayEntryNum; i++)
    {
        if(dbArrayPtr[i].memType == memType)
        {
            memoryFound = GT_TRUE;

            if((dbArrayPtr[i].protectionType == CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_NONE_E) &&
               (dbArrayPtr[i].externalProtectionType == CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_NONE_E))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            /* there are no interrupts for MLL memories */
            if((memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MLL_TABLE_FIFO_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MLL_REPLICATIONS_IN_USE_FIFO_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MLL_CONF_TABLE_E))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }

            /* non-DFX memory mask operation */
            switch(memType)
            {
                case PRV_CPSS_DXCH_LION2_DIAG_DATA_INTEGRITY_EXT_PROTECTION_MEMORY_TYPE_CASES_MAC:

                    rc = prvCpssDxChDiagDataIntegrityExtMemoryProtectionEventMask(devNum, memType, errorType, operation);
                    if(GT_OK != rc)
                    {
                        return rc;
                    }
                    break;

                default:
                    pipeIndex = U32_GET_FIELD_MAC(dbArrayPtr[i].key, 12, 3);
                    clientIndex = U32_GET_FIELD_MAC(dbArrayPtr[i].key, 7, 5);

                    /* Set Pipe Select Register */
                    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->dfxUnits.server.pipeSelect;
                    rc = prvCpssHwPpWriteRegister(devNum, regAddr, (1 << pipeIndex));
                    if(GT_OK != rc)
                    {
                        return rc;
                    }

                    /* build Memory Control Register address */
                    addrStr.dfxUnitClientIdx = clientIndex;
                    addrStr.dfxXsbSelect = 0;
                    addrStr.dfxClientSelect = 0;
                    addrStr.dfxClientMemoryNumber = U32_GET_FIELD_MAC(dbArrayPtr[i].key, 0, 7);
                    addrStr.dfxClientMemoryRegisterOffset = PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_ADDR_TYPE_MEMORY_INT_MASK_REG_CNS;

                    rc = buildDfxAddr(&addrStr, &regAddr);
                    if(GT_OK != rc)
                    {
                        return rc;
                    }

                    if(dbArrayPtr[i].protectionType == CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_ECC_E)
                    {
                        switch(errorType)
                        {
                            case CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E:
                                fieldOffset = 2;
                                fieldLength = 1;
                                regData = operation;
                                break;

                            case CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E:
                                fieldOffset = 3;
                                fieldLength = 1;
                                regData = operation;
                                break;

                            case CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_AND_MULTIPLE_ECC_E:
                                fieldOffset = 2;
                                fieldLength = 2;
                                regData = (operation == CPSS_EVENT_MASK_E) ? 0 : 3;
                                break;

                            default:
                                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                        }
                    }
                    else if(dbArrayPtr[i].protectionType == CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_PARITY_E)
                    {
                        fieldOffset = 1;
                        fieldLength = 1;
                        regData = operation;
                    }
                    else
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                    }

                    /* write to Interrupt Mask Register */
                    rc = prvCpssHwPpSetRegField(devNum, regAddr, fieldOffset, fieldLength, regData);
                    if(GT_OK != rc)
                    {
                        return rc;
                    }
                    break;
            }
        }
    }

    if((i == dbArrayEntryNum) && (memoryFound == GT_FALSE))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChDiagDataIntegrityEventMaskSet
*
* DESCRIPTION:
*       Function sets mask/unmask for ECC/Parity event.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - PP device number
*       memType     - type of memory(table)
*       errorType   - type of error interrupt
*                     relevant only for ECC protected memories
*       operation   - mask/unmask interrupt
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, portGroup, memType, errorType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChDiagDataIntegrityEventMaskSet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT      memType,
    IN  CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT   errorType,
    IN  CPSS_EVENT_MASK_SET_ENT                         operation
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChDiagDataIntegrityEventMaskSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, memType, errorType, operation));

    rc = internal_cpssDxChDiagDataIntegrityEventMaskSet(devNum, memType, errorType, operation);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, memType, errorType, operation));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChDiagDataIntegrityEventMaskGet
*
* DESCRIPTION:
*       Function gets mask/unmask for ECC/Parity interrupt.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - PP device number
*       memType     - type of memory(table)
*
*
* OUTPUTS:
*       errorTypePtr   - (pointer to) type of error interrupt
*       operationPtr   - (pointer to) mask/unmask interrupt
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, portGroup, memType, errorType
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChDiagDataIntegrityEventMaskGet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT      memType,
    OUT CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT   *errorTypePtr,
    OUT CPSS_EVENT_MASK_SET_ENT                         *operationPtr
)
{
    GT_STATUS                   rc;          /* return code */
    GT_U32                      regAddr;     /* register address */
    GT_U32                      regData;     /* register data */
    GT_U32                      pipeIndex;   /* pipe index */
    GT_U32                      clientIndex; /* client index */
    GT_U32                      i;           /* loop iterator */
    PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_DFX_ADDR_STC  addrStr;     /* DFX address struct */
    GT_BOOL                     memoryFound = GT_FALSE; /* memory found status */
    PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_MAP_DB_STC    *dbArrayPtr; /* pointer to data integrity DB */
    GT_U32                                          dbArrayEntryNum; /* size of data integrity DB */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
                                            CPSS_XCAT2_E | CPSS_LION_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);
    CPSS_NULL_PTR_CHECK_MAC(errorTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(operationPtr);

    prvCpssDxChDiagDataIntegrityDbPointerSet(devNum, &dbArrayPtr, &dbArrayEntryNum);

    for(i = 0; i < dbArrayEntryNum; i++)
    {
        if(dbArrayPtr[i].memType == memType)
        {
            memoryFound = GT_TRUE;

            if((dbArrayPtr[i].protectionType == CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_NONE_E) &&
               (dbArrayPtr[i].externalProtectionType == CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_NONE_E))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            /* there are no interrupts for MLL memories */
            if((memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MLL_TABLE_FIFO_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MLL_REPLICATIONS_IN_USE_FIFO_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MLL_CONF_TABLE_E))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }

            /* non-DFX memory mask operation */
            switch(memType)
            {
                case PRV_CPSS_DXCH_LION2_DIAG_DATA_INTEGRITY_EXT_PROTECTION_MEMORY_TYPE_CASES_MAC:

                    rc = prvCpssDxChDiagDataIntegrityExtMemoryProtectionEventMaskGet(devNum, memType, errorTypePtr, operationPtr);
                    if(GT_OK != rc)
                    {
                        return rc;
                    }
                    break;

                default:

                    pipeIndex = U32_GET_FIELD_MAC(dbArrayPtr[i].key, 12, 3);
                    clientIndex = U32_GET_FIELD_MAC(dbArrayPtr[i].key, 7, 5);

                    /* Set Pipe Select Register */
                    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->dfxUnits.server.pipeSelect;
                    rc = prvCpssHwPpWriteRegister(devNum, regAddr, (1 << pipeIndex));
                    if(GT_OK != rc)
                    {
                        return rc;
                    }

                    /* build Memory Control Register address */
                    addrStr.dfxUnitClientIdx = clientIndex;
                    addrStr.dfxXsbSelect = 0;
                    addrStr.dfxClientSelect = 0;
                    addrStr.dfxClientMemoryNumber = U32_GET_FIELD_MAC(dbArrayPtr[i].key, 0, 7);
                    addrStr.dfxClientMemoryRegisterOffset = PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_ADDR_TYPE_MEMORY_INT_MASK_REG_CNS;

                    rc = buildDfxAddr(&addrStr, &regAddr);
                    if(GT_OK != rc)
                    {
                        return rc;
                    }

                    /* write to Interrupt Mask Register */
                    rc = prvCpssHwPpReadRegister(devNum, regAddr, &regData);
                    if(GT_OK != rc)
                    {
                        return rc;
                    }

                    if(dbArrayPtr[i].protectionType == CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_ECC_E)
                    {
                        regData = U32_GET_FIELD_MAC(regData, 2, 2);
                        switch(regData)
                        {
                            case 0:
                                *errorTypePtr = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_AND_MULTIPLE_ECC_E;
                                *operationPtr = CPSS_EVENT_MASK_E;
                                break;
                            case 1:
                                *errorTypePtr = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E;
                                *operationPtr = CPSS_EVENT_MASK_E;
                                break;
                            case 2:
                                *errorTypePtr = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E;
                                *operationPtr = CPSS_EVENT_MASK_E;
                                break;
                            default:
                                *errorTypePtr = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_AND_MULTIPLE_ECC_E;
                                *operationPtr = CPSS_EVENT_UNMASK_E;
                                break;
                        }
                    }
                    else if(dbArrayPtr[i].protectionType == CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_PARITY_E)
                    {
                        *operationPtr = U32_GET_FIELD_MAC(regData, 1, 1);

                        *errorTypePtr = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                    }
                    else
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                    }

                    break;
            }

            /* it's enough to read the first instance of certain memType */
            break;
        }
    }

    if((i == dbArrayEntryNum) && (memoryFound == GT_FALSE))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChDiagDataIntegrityEventMaskGet
*
* DESCRIPTION:
*       Function gets mask/unmask for ECC/Parity interrupt.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - PP device number
*       memType     - type of memory(table)
*
*
* OUTPUTS:
*       errorTypePtr   - (pointer to) type of error interrupt
*       operationPtr   - (pointer to) mask/unmask interrupt
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, portGroup, memType, errorType
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChDiagDataIntegrityEventMaskGet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT      memType,
    OUT CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT   *errorTypePtr,
    OUT CPSS_EVENT_MASK_SET_ENT                         *operationPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChDiagDataIntegrityEventMaskGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, memType, errorTypePtr, operationPtr));

    rc = internal_cpssDxChDiagDataIntegrityEventMaskGet(devNum, memType, errorTypePtr, operationPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, memType, errorTypePtr, operationPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}



/*******************************************************************************
* internal_cpssDxChDiagDataIntegrityErrorInfoGet
*
* DESCRIPTION:
*       Function gets ECC/Parity error info.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum         - PP device number
*       memType        - type of memory(table)
*       locationPtr    - (pointer to) memory location indexes
*
* OUTPUTS:
*       errorCounterPtr    - (pointer to) error counter
*       failedRowPtr       - (pointer to) failed raw
*       failedSyndromePtr  - (pointer to) failed syndrome
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, portGroup, memType
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Error status or counter that isn't supported returns 0xFFFFFFFF
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChDiagDataIntegrityErrorInfoGet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT      memType,
    IN  CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_UNT    *locationPtr,
    OUT GT_U32                                          *errorCounterPtr,
    OUT GT_U32                                          *failedRowPtr,
    OUT GT_U32                                          *failedSyndromePtr
)
{
    GT_STATUS                   rc;          /* return code */
    GT_U32                      regAddr;     /* register address */
    GT_U32                      regAddr1;    /* register address */
    GT_U32                      regData;     /* register data */
    GT_U32                      regOffset;   /* register offset */
    GT_U32                      pipeIndex;   /* pipe index */
    GT_U32                      clientIndex; /* client index */
    GT_U32                      memoryIndex; /* memory index */
    PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_DFX_ADDR_STC  addrStr;     /* DFX address struct */
    PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_MAP_DB_STC    *dbItemPtr; /* pointer to map DB item */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
                                            CPSS_XCAT2_E | CPSS_LION_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);
    CPSS_NULL_PTR_CHECK_MAC(locationPtr);
    CPSS_NULL_PTR_CHECK_MAC(errorCounterPtr);
    CPSS_NULL_PTR_CHECK_MAC(failedRowPtr);
    CPSS_NULL_PTR_CHECK_MAC(failedSyndromePtr);

    if((memType >= CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_LION2_LAST_E) || (memType < 0))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* init counter/status */
    *errorCounterPtr = 0xFFFFFFFF;
    *failedRowPtr = 0xFFFFFFFF;
    *failedSyndromePtr = 0xFFFFFFFF;

    if((memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_DATA_BUFFER_MEMORY_E) ||
       (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_DATA_BUFFER_MEMORY_ECC_E))
    {
        if(locationPtr->mppmMemLocation.mppmId == 0)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->mppm[0].ecc.eccLastFailedAddress;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->mppm[1].ecc.eccLastFailedAddress;
        }
        regOffset = (locationPtr->mppmMemLocation.bankId == 0) ? 0 : 12;

        return prvCpssHwPpPortGroupGetRegField(devNum, locationPtr->mppmMemLocation.portGroupId, regAddr, regOffset, 12, failedRowPtr);
    }
    else
    {
        /* Get portgroup for non-DFX memories */
        rc = searchMemType(devNum,
                           locationPtr->memLocation.dfxPipeId,
                           locationPtr->memLocation.dfxClientId,
                           locationPtr->memLocation.dfxMemoryId,
                           &dbItemPtr);
        if(GT_OK != rc)
        {
            return rc;
        }
    }

    switch(memType)
    {
        /* there are no counters or row statuses for these memories */
        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXDMA_HA_INFO_DESC_PREFETCH_E:
        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_EGR_MC_FIFO_0_E:
        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_EGR_MC_FIFO_1_E:
        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_EGR_MC_FIFO_3_E:
        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ROUTER_TCAM_E:
        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_POLICY_TCAM_E:
            return GT_OK;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_EGR_VLAN_E:

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.sht.global.egrVlanParityErrorCounter;
            rc = prvCpssHwPpPortGroupReadRegister(devNum, dbItemPtr->causePortGroupId, regAddr, errorCounterPtr);
            if(GT_OK != rc)
            {
                return rc;
            }

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.sht.global.vlanParityErrorLastEntries;
            regOffset = 12;
            return prvCpssHwPpPortGroupGetRegField(devNum, dbItemPtr->causePortGroupId, regAddr, regOffset, 12, failedRowPtr);

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_ING_VLAN_E:

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.sht.global.ingVlanParityErrorCounter;
            rc = prvCpssHwPpPortGroupReadRegister(devNum, dbItemPtr->causePortGroupId, regAddr, errorCounterPtr);
            if(GT_OK != rc)
            {
                return rc;
            }

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.sht.global.vlanParityErrorLastEntries;
            regOffset = 0;
            return prvCpssHwPpPortGroupGetRegField(devNum, dbItemPtr->causePortGroupId, regAddr, regOffset, 12, failedRowPtr);

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_PORT_ISO_L2_E:

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.sht.global.l2PortIsolationParityErrorCounter;
            rc = prvCpssHwPpPortGroupReadRegister(devNum, dbItemPtr->causePortGroupId, regAddr, errorCounterPtr);
            if(GT_OK != rc)
            {
                return rc;
            }

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.sht.global.portIsolationParityErrorLastEntries;
            regOffset = 0;
            return prvCpssHwPpPortGroupGetRegField(devNum, dbItemPtr->causePortGroupId, regAddr, regOffset, 12, failedRowPtr);

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_PORT_ISO_L3_E:

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.sht.global.l3PortIsolationParityErrorCounter;
            rc = prvCpssHwPpPortGroupReadRegister(devNum, dbItemPtr->causePortGroupId, regAddr, errorCounterPtr);
            if(GT_OK != rc)
            {
                return rc;
            }

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.sht.global.portIsolationParityErrorLastEntries;
            regOffset = 12;
            return prvCpssHwPpPortGroupGetRegField(devNum, dbItemPtr->causePortGroupId, regAddr, regOffset, 12, failedRowPtr);

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_SHT_VIDX_E:

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.sht.global.vidxParityErrorCounter;
            rc = prvCpssHwPpPortGroupReadRegister(devNum, dbItemPtr->causePortGroupId, regAddr, errorCounterPtr);
            if(GT_OK != rc)
            {
                return rc;
            }

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.sht.global.vidxParityErrorLastEntry;
            regOffset = 0;
            return prvCpssHwPpPortGroupGetRegField(devNum, dbItemPtr->causePortGroupId, regAddr, regOffset, 12, failedRowPtr);

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IP_NHE_TABLE_E:

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerDataError;
            rc = prvCpssHwPpPortGroupReadRegister(devNum, dbItemPtr->causePortGroupId, regAddr, &regData);
            if(GT_OK != rc)
            {
                return rc;
            }

            *errorCounterPtr = U32_GET_FIELD_MAC(regData, 24, 8);

            /* NHE Data Error Address Valid */
            /* This bit indicates whether the 'NHE Data Error Address' field
               is valid. This bit is set to one by the design upon every data
               error that occurs in the NHE table. This bit should be reset by
               software after reading the register content. */
            if((regData & 0x1) != 0)
            {
                *failedRowPtr = U32_GET_FIELD_MAC(regData, 1, 15);

                rc = prvCpssHwPpPortGroupSetRegField(devNum, dbItemPtr->causePortGroupId, regAddr, 0, 1, 0);
            }

            return GT_OK;


        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IP_STG2_DESC_RETURN_TWO_FIFO_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerDataError;
            return prvCpssHwPpPortGroupGetRegField(devNum, dbItemPtr->causePortGroupId, regAddr, 24, 8, errorCounterPtr);

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_PLR_DESCRIPTOR_FIFO_E:

            if(locationPtr->memLocation.dfxMemoryId == 34)
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[0].policerErrorReg;
                regAddr1 = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[0].policerErrorCntrReg;
            }
            else if(locationPtr->memLocation.dfxMemoryId == 32)
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[1].policerErrorReg;
                regAddr1 = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->PLR[1].policerErrorCntrReg;
            }
            else
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }

            rc = prvCpssHwPpPortGroupReadRegister(devNum, dbItemPtr->causePortGroupId, regAddr, &regData);
            if(GT_OK != rc)
            {
                return rc;
            }

            if(U32_GET_FIELD_MAC(regData, 31, 1) != 0)
            {
                *failedRowPtr = U32_GET_FIELD_MAC(regData, 0, 24);

                /* get error counter */
                rc = prvCpssHwPpPortGroupGetRegField(devNum, dbItemPtr->causePortGroupId, regAddr1, 0, 8, errorCounterPtr);
                if(GT_OK != rc)
                {
                    return rc;
                }
            }

            return GT_OK;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IP_STG4_DESC_RETURN_TWO_FIFO_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerDataError;
            return prvCpssHwPpPortGroupGetRegField(devNum, dbItemPtr->causePortGroupId, regAddr, 24, 8, errorCounterPtr);

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_IP_UNUSED_DATA_FIFO_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerDataError;
            return prvCpssHwPpPortGroupGetRegField(devNum, dbItemPtr->causePortGroupId, regAddr, 24, 8, errorCounterPtr);

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_DQ_TB_GIGA_PRIO_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.global.memoryParityError.tokenBucketPriorityParityErrorCounter;
            rc = prvCpssHwPpPortGroupGetRegField(devNum, dbItemPtr->causePortGroupId, regAddr, 0, 16, errorCounterPtr);
            if(rc != GT_OK)
            {
                return rc;
            }

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.global.memoryParityError.parityErrorBadAddress;
            return prvCpssHwPpPortGroupGetRegField(devNum, dbItemPtr->causePortGroupId, regAddr, 0, 5, failedRowPtr);

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_DQ_WRR_STATE_VARIABLES_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.global.memoryParityError.stateVariablesParityErrorCounter;
            rc = prvCpssHwPpPortGroupGetRegField(devNum, dbItemPtr->causePortGroupId, regAddr, 0, 16, errorCounterPtr);
            if(rc != GT_OK)
            {
                return rc;
            }

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.global.memoryParityError.parityErrorBadAddress;
            return prvCpssHwPpPortGroupGetRegField(devNum, dbItemPtr->causePortGroupId, regAddr, 5, 5, failedRowPtr);

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_LL_FREE_BUFS_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.ll.linkList.linkListEccControl.fbufRamEccStatus;
            rc = prvCpssHwPpPortGroupGetRegField(devNum, dbItemPtr->causePortGroupId, regAddr, 0, 12, failedRowPtr);
            if(GT_OK != rc)
            {
                return rc;
            }

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.ll.linkList.linkListEccControl.fbufRamEccErrorCounter;
            return prvCpssHwPpPortGroupGetRegField(devNum, dbItemPtr->causePortGroupId, regAddr, 0, 16, errorCounterPtr);

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_BUFFER_FIFO_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.congestNotification.cnBufferFifoParityErrorsCnt;
            return prvCpssHwPpPortGroupGetRegField(devNum, dbItemPtr->causePortGroupId, regAddr, 0, 31, errorCounterPtr);

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_0_COUNTERS_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.llfcRegs.pfcPipeCountersParityErrorsCounter[0];
            return prvCpssHwPpPortGroupGetRegField(devNum, dbItemPtr->causePortGroupId, regAddr, 0, 31, errorCounterPtr);

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_1_COUNTERS_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.llfcRegs.pfcPipeCountersParityErrorsCounter[1];
            return prvCpssHwPpPortGroupGetRegField(devNum, dbItemPtr->causePortGroupId, regAddr, 0, 31, errorCounterPtr);

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_2_COUNTERS_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.llfcRegs.pfcPipeCountersParityErrorsCounter[2];
            return prvCpssHwPpPortGroupGetRegField(devNum, dbItemPtr->causePortGroupId, regAddr, 0, 31, errorCounterPtr);

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_3_COUNTERS_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.llfcRegs.pfcPipeCountersParityErrorsCounter[3];
            return prvCpssHwPpPortGroupGetRegField(devNum, dbItemPtr->causePortGroupId, regAddr, 0, 31, errorCounterPtr);

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_4_COUNTERS_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.llfcRegs.pfcPipeCountersParityErrorsCounter[4];
            return prvCpssHwPpPortGroupGetRegField(devNum, dbItemPtr->causePortGroupId, regAddr, 0, 31, errorCounterPtr);

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_5_COUNTERS_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.llfcRegs.pfcPipeCountersParityErrorsCounter[5];
            return prvCpssHwPpPortGroupGetRegField(devNum, dbItemPtr->causePortGroupId, regAddr, 0, 31, errorCounterPtr);

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_6_COUNTERS_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.llfcRegs.pfcPipeCountersParityErrorsCounter[6];
            return prvCpssHwPpPortGroupGetRegField(devNum, dbItemPtr->causePortGroupId, regAddr, 0, 31, errorCounterPtr);

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_7_COUNTERS_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.llfcRegs.pfcPipeCountersParityErrorsCounter[7];
            return prvCpssHwPpPortGroupGetRegField(devNum, dbItemPtr->causePortGroupId, regAddr, 0, 31, errorCounterPtr);

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_BMA_MULTICAST_COUNTERS_E:
            if(locationPtr->memLocation.dfxMemoryId == 51)
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bma.bmaCore[0].mcCntParityErrorCounter;
            }
            else if(locationPtr->memLocation.dfxMemoryId == 43)
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bma.bmaCore[1].mcCntParityErrorCounter;
            }
            else if(locationPtr->memLocation.dfxMemoryId == 35)
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bma.bmaCore[2].mcCntParityErrorCounter;
            }
            else if(locationPtr->memLocation.dfxMemoryId == 27)
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bma.bmaCore[3].mcCntParityErrorCounter;
            }
            else
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }

            return prvCpssHwPpPortGroupGetRegField(devNum, dbItemPtr->causePortGroupId, regAddr, 0, 8, errorCounterPtr);

        default:

            pipeIndex = locationPtr->memLocation.dfxPipeId;
            clientIndex = locationPtr->memLocation.dfxClientId;
            memoryIndex = locationPtr->memLocation.dfxMemoryId;

            /* Set Pipe Select Register */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->dfxUnits.server.pipeSelect;
            rc = prvCpssHwPpWriteRegister(devNum, regAddr, (1 << pipeIndex));
            if(GT_OK != rc)
            {
                return rc;
            }

            /* build Memory Control Register address */
            addrStr.dfxUnitClientIdx = clientIndex;
            addrStr.dfxXsbSelect = 0;
            addrStr.dfxClientSelect = 0;
            addrStr.dfxClientMemoryNumber = memoryIndex;
            addrStr.dfxClientMemoryRegisterOffset = PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_ADDR_TYPE_MEMORY_ERRORS_COUNTER_REG_CNS;

            rc = buildDfxAddr(&addrStr, &regAddr);
            if(GT_OK != rc)
            {
                return rc;
            }
            /* read Error Counter Register */
            rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 5, errorCounterPtr);
            if(GT_OK != rc)
            {
                return rc;
            }

            addrStr.dfxClientMemoryRegisterOffset = PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_ADDR_TYPE_MEMORY_ERRONEOUS_ADDR_REG_CNS;
            rc = buildDfxAddr(&addrStr, &regAddr);
            if(GT_OK != rc)
            {
                return rc;
            }
            /* read Error Counter Register */
            rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 16, failedRowPtr);
            if(GT_OK != rc)
            {
                return rc;
            }


            addrStr.dfxClientMemoryRegisterOffset = PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_ADDR_TYPE_MEMORY_ECC_SYNDROME_REG_CNS;
            rc = buildDfxAddr(&addrStr, &regAddr);
            if(GT_OK != rc)
            {
                return rc;
            }
            /* read Error Counter Register */
            rc =  prvCpssHwPpGetRegField(devNum, regAddr, 0, 8, failedSyndromePtr);
            if(GT_OK != rc)
            {
                return rc;
            }

            break;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChDiagDataIntegrityErrorInfoGet
*
* DESCRIPTION:
*       Function gets ECC/Parity error info.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum         - PP device number
*       memType        - type of memory(table)
*       locationPtr    - (pointer to) memory location indexes
*
* OUTPUTS:
*       errorCounterPtr    - (pointer to) error counter
*       failedRowPtr       - (pointer to) failed raw
*       failedSyndromePtr  - (pointer to) failed syndrome
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, portGroup, memType
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Error status or counter that isn't supported returns 0xFFFFFFFF
*
*******************************************************************************/
GT_STATUS cpssDxChDiagDataIntegrityErrorInfoGet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT      memType,
    IN  CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_UNT    *locationPtr,
    OUT GT_U32                                          *errorCounterPtr,
    OUT GT_U32                                          *failedRowPtr,
    OUT GT_U32                                          *failedSyndromePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChDiagDataIntegrityErrorInfoGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, memType, locationPtr, errorCounterPtr, failedRowPtr, failedSyndromePtr));

    rc = internal_cpssDxChDiagDataIntegrityErrorInfoGet(devNum, memType, locationPtr, errorCounterPtr, failedRowPtr, failedSyndromePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, memType, locationPtr, errorCounterPtr, failedRowPtr, failedSyndromePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}



/*******************************************************************************
* internal_cpssDxChDiagDataIntegrityErrorInjectionConfigSet
*
* DESCRIPTION:
*       Function enables/disable injection of error during next write operation.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - PP device number
*       memType      - type of memory(table)
*       injectMode   - error injection mode
*                      relevant only for ECC protected memories
*       injectEnable - enable/disable error injection
*                       GT_TRUE - enable
*                       GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, memType, injectMode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Memory with parity can not be configured with multiple error injection
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChDiagDataIntegrityErrorInjectionConfigSet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT      memType,
    IN  CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_ENT  injectMode,
    IN  GT_BOOL                                         injectEnable
)
{
    GT_STATUS                   rc;          /* return code */
    GT_U32                      regAddr;     /* register address */
    GT_U32                      regData;     /* register data */
    GT_U32                      pipeIndex;   /* pipe index */
    GT_U32                      clientIndex; /* client index */
    GT_U32                      memoryIndex; /* memory index */
    GT_U32                      i;           /* loop iterator */
    PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_DFX_ADDR_STC  addrStr;     /* DFX address struct */
    GT_BOOL                     memoryFound = GT_FALSE; /* memory found status */
    CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT prevMemType =
                CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_LION2_LAST_E; /* temp memType for skip injection */
    PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_MAP_DB_STC    *dbArrayPtr; /* pointer to data integrity DB */
    GT_U32                                          dbArrayEntryNum; /* size of data integrity DB */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
                                            CPSS_XCAT2_E | CPSS_LION_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    prvCpssDxChDiagDataIntegrityDbPointerSet(devNum, &dbArrayPtr, &dbArrayEntryNum);

    for(i = 0; i < dbArrayEntryNum; i++)
    {
        if(dbArrayPtr[i].memType == memType)
        {
            memoryFound = GT_TRUE;

            if((dbArrayPtr[i].protectionType == CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_NONE_E) &&
               (dbArrayPtr[i].externalProtectionType == CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_NONE_E))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            /* there are no interrupts for MLL memories */
            if((memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MLL_TABLE_FIFO_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MLL_REPLICATIONS_IN_USE_FIFO_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MLL_CONF_TABLE_E))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }

            pipeIndex = U32_GET_FIELD_MAC(dbArrayPtr[i].key, 12, 3);
            clientIndex = U32_GET_FIELD_MAC(dbArrayPtr[i].key, 7, 5);
            memoryIndex = U32_GET_FIELD_MAC(dbArrayPtr[i].key, 0, 7);

            /* BMA: special memory vendor (MISL_SR512) - no error injection via DFX
               TXQ Q Counters: special memory vendor (MISL_SR512) - no error injection via DFX
               TXQ Q Index FIFO: useless(spare) memory - no access with traffic and/or MG
               TXQ: false interrupts that cause storming */
            if((memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_BMA_MULTICAST_COUNTERS_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_0_COUNTERS_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_1_COUNTERS_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_2_COUNTERS_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_3_COUNTERS_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_4_COUNTERS_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_5_COUNTERS_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_6_COUNTERS_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_7_COUNTERS_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_IND_FIFO_PIPE_0_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_IND_FIFO_PIPE_1_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_IND_FIFO_PIPE_2_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_IND_FIFO_PIPE_3_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_IND_FIFO_PIPE_4_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_IND_FIFO_PIPE_5_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_IND_FIFO_PIPE_6_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_IND_FIFO_PIPE_7_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_DQ_TB_GIGA_PRIO_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_EGR_MC_FIFO_3_E))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }

            /* MPPM Error Injection:
                There is no data integity error per specific RAM.
                Only BANK0/1 indication supported. Bank reflects state 8 data
                and 1 ecc memories.
                Therefore in order to simulate sigle ECC error injection it is
                needed to skip 7 memories and ECC memory block.

                All other Non-DFX memories:
                In order to simulate and catch data integrity event for the
                memories with more then one RAM in table entry, but one interrupt
                per table(not RAM) line, it is needed to skip all RAMs in line
                except the first one.
            */
            switch(memType)
            {
                case PRV_CPSS_DXCH_LION2_DIAG_DATA_INTEGRITY_EXT_PROTECTION_MEMORY_TYPE_CASES_MAC:
                    if((dbArrayPtr[i].memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ROUTER_TCAM_E) ||
                      (dbArrayPtr[i].memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_POLICY_TCAM_E))
                    {
                        if(PRV_CPSS_DXCH_PP_MAC(devNum)->diagInfo.tcamParityCalcEnable)
                        {
                            if(injectEnable == GT_FALSE)
                            {
                                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                            }

                            if(dbArrayPtr[i].memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_POLICY_TCAM_E)
                            {
                                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.tcamReadMaskParityBase;
                            }
                            else
                            {
                                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.ipTccRegs.ipTcamReadMaskParityBase;
                            }

                            /* Read Parity_Y table */
                            rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, (regAddr), &regData);
                            if (rc != GT_OK)
                            {
                                return rc;
                            }

                            /* invert first bit in Y parity */
                            regData ^= 1;

                            /* Write Parity_Y table */
                            return prvCpssDrvHwPpPortGroupWriteRegister(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, (regAddr), regData);
                        }
                        else
                        {
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                        }
                    }

                    if(dbArrayPtr[i].memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_DATA_BUFFER_MEMORY_ECC_E)
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                    }
                    if(memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_DATA_BUFFER_MEMORY_E)
                    {
                        if(memoryIndex % 8 != 7)
                        {
                            continue;
                        }
                    }
                    else
                    {
                        if(dbArrayPtr[i].firstTableDataBit == 0)
                        {
                            prevMemType = memType;
                        }
                        else if ((dbArrayPtr[i].memType == prevMemType))
                        {
                            continue;
                        }
                    }

                    break;

                default:
                    break;
            }

            /* Set Pipe Select Register */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->dfxUnits.server.pipeSelect;
            rc = prvCpssHwPpWriteRegister(devNum, regAddr, (1 << pipeIndex));
            if(GT_OK != rc)
            {
                return rc;
            }

            /* build Memory Control Register address */
            addrStr.dfxUnitClientIdx = clientIndex;
            addrStr.dfxXsbSelect = 0;
            addrStr.dfxClientSelect = 0;
            addrStr.dfxClientMemoryNumber = memoryIndex;
            addrStr.dfxClientMemoryRegisterOffset = PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_ADDR_TYPE_MEMORY_CONTROL_REG_CNS;

            rc = buildDfxAddr(&addrStr, &regAddr);
            if(GT_OK != rc)
            {
                return rc;
            }

            /* NOTE: In order to switch error injection modes
               <Error Injection Enable> must be set to Disable */

            /* write to Memory Control Register - disable error injection */
            rc = prvCpssHwPpSetRegField(devNum, regAddr, 1, 1, 0);
            if(GT_OK != rc)
            {
                return rc;
            }


            if(dbArrayPtr[i].protectionType == CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_PARITY_E)
            {
                regData = 0;
            }
            else
            {
                regData = (injectMode == CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_MULTIPLE_E) ? 1 : 0;
            }

            /* write to Memory Control Register - error injection mode configuration */
            rc = prvCpssHwPpSetRegField(devNum, regAddr, 2, 1, regData);
            if(GT_OK != rc)
            {
                return rc;
            }


            if(injectEnable == GT_TRUE)
            {
                /* write to Memory Control Register - enable error injection */
                rc = prvCpssHwPpSetRegField(devNum, regAddr, 1, 1, 1);
                if(GT_OK != rc)
                {
                    return rc;
                }
            }

        }
    }

    if((i == dbArrayEntryNum) && (memoryFound == GT_FALSE))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChDiagDataIntegrityErrorInjectionConfigSet
*
* DESCRIPTION:
*       Function enables/disable injection of error during next write operation.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - PP device number
*       memType      - type of memory(table)
*       injectMode   - error injection mode
*                      relevant only for ECC protected memories
*       injectEnable - enable/disable error injection
*                       GT_TRUE - enable
*                       GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, memType, injectMode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Memory with parity can not be configured with multiple error injection
*
*******************************************************************************/
GT_STATUS cpssDxChDiagDataIntegrityErrorInjectionConfigSet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT      memType,
    IN  CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_ENT  injectMode,
    IN  GT_BOOL                                         injectEnable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChDiagDataIntegrityErrorInjectionConfigSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, memType, injectMode, injectEnable));

    rc = internal_cpssDxChDiagDataIntegrityErrorInjectionConfigSet(devNum, memType, injectMode, injectEnable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, memType, injectMode, injectEnable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}



/*******************************************************************************
* internal_cpssDxChDiagDataIntegrityErrorInjectionConfigGet
*
* DESCRIPTION:
*       Function gets status of error injection.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - PP device number
*       memType      - type of memory(table)
*
* OUTPUTS:
*       injectModePtr   - (pointer to) error injection mode
*       injectEnablePtr - (pointer to) enable/disable error injection
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, memType
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChDiagDataIntegrityErrorInjectionConfigGet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT      memType,
    OUT CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_ENT  *injectModePtr,
    OUT GT_BOOL                                         *injectEnablePtr
)
{
    GT_STATUS                   rc;          /* return code */
    GT_U32                      regAddr;     /* register address */
    GT_U32                      regData;     /* register data */
    GT_U32                      pipeIndex;   /* pipe index */
    GT_U32                      clientIndex; /* client index */
    GT_U32                      memoryIndex; /* memory index */
    GT_U32                      i;           /* loop iterator */
    PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_DFX_ADDR_STC  addrStr;     /* DFX address struct */
    GT_BOOL                     memoryFound = GT_FALSE; /* memory found status */
    PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_MAP_DB_STC    *dbArrayPtr; /* pointer to data integrity DB */
    GT_U32                                          dbArrayEntryNum; /* size of data integrity DB */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
                                            CPSS_XCAT2_E | CPSS_LION_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);
    CPSS_NULL_PTR_CHECK_MAC(injectModePtr);
    CPSS_NULL_PTR_CHECK_MAC(injectEnablePtr);

    prvCpssDxChDiagDataIntegrityDbPointerSet(devNum, &dbArrayPtr, &dbArrayEntryNum);

    for(i = 0; i < dbArrayEntryNum; i++)
    {
        if(dbArrayPtr[i].memType == memType)
        {
            memoryFound = GT_TRUE;

            if((dbArrayPtr[i].protectionType == CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_NONE_E) &&
               (dbArrayPtr[i].externalProtectionType == CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_NONE_E))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            /* BMA: special memory vendor (MISL_SR512) - no error injection via DFX
               TXQ Q Counters: special memory vendor (MISL_SR512) - no error injection via DFX
               TXQ Q Index FIFO: useless(spare) memory - no access with traffic and/or MG
               TXQ: false interrupts that cause storming */
            if((memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_BMA_MULTICAST_COUNTERS_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_0_COUNTERS_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_1_COUNTERS_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_2_COUNTERS_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_3_COUNTERS_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_4_COUNTERS_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_5_COUNTERS_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_6_COUNTERS_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_SRC_PIPE_7_COUNTERS_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_IND_FIFO_PIPE_0_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_IND_FIFO_PIPE_1_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_IND_FIFO_PIPE_2_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_IND_FIFO_PIPE_3_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_IND_FIFO_PIPE_4_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_IND_FIFO_PIPE_5_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_IND_FIFO_PIPE_6_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_Q_PFC_IND_FIFO_PIPE_7_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_DQ_TB_GIGA_PRIO_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_TXQ_EGR_MC_FIFO_3_E))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }

            /* there are no interrupts for MLL memories */
            if((memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MLL_TABLE_FIFO_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MLL_REPLICATIONS_IN_USE_FIFO_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MLL_CONF_TABLE_E))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }

            pipeIndex = U32_GET_FIELD_MAC(dbArrayPtr[i].key, 12, 3);
            clientIndex = U32_GET_FIELD_MAC(dbArrayPtr[i].key, 7, 5);
            memoryIndex = U32_GET_FIELD_MAC(dbArrayPtr[i].key, 0, 7);

            if((memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ROUTER_TCAM_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_POLICY_TCAM_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_DATA_BUFFER_MEMORY_ECC_E))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }

            if(memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MPPM_DATA_BUFFER_MEMORY_E)
            {
                if(memoryIndex % 8 != 7)
                {
                    continue;
                }
            }


            /* Set Pipe Select Register */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->dfxUnits.server.pipeSelect;
            rc = prvCpssHwPpWriteRegister(devNum, regAddr, (1 << pipeIndex));
            if(GT_OK != rc)
            {
                return rc;
            }

            /* build Memory Control Register address */
            addrStr.dfxUnitClientIdx = clientIndex;
            addrStr.dfxXsbSelect = 0;
            addrStr.dfxClientSelect = 0;
            addrStr.dfxClientMemoryNumber = memoryIndex;
            addrStr.dfxClientMemoryRegisterOffset = PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_ADDR_TYPE_MEMORY_CONTROL_REG_CNS;

            rc = buildDfxAddr(&addrStr, &regAddr);
            if(GT_OK != rc)
            {
                return rc;
            }

            /* read Memory Control Register */
            rc = prvCpssHwPpReadRegister(devNum, regAddr, &regData);
            if(GT_OK != rc)
            {
                return rc;
            }

            *injectModePtr = (U32_GET_FIELD_MAC(regData, 2, 1) == 1) ? CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_MULTIPLE_E : CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_SINGLE_E;

            *injectEnablePtr = (U32_GET_FIELD_MAC(regData, 1, 1) == 1) ? GT_TRUE : GT_FALSE;

            break;
        }

    }

    if((i == dbArrayEntryNum) && (memoryFound == GT_FALSE))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChDiagDataIntegrityErrorInjectionConfigGet
*
* DESCRIPTION:
*       Function gets status of error injection.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - PP device number
*       memType      - type of memory(table)
*
* OUTPUTS:
*       injectModePtr   - (pointer to) error injection mode
*       injectEnablePtr - (pointer to) enable/disable error injection
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, memType
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChDiagDataIntegrityErrorInjectionConfigGet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT      memType,
    OUT CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_ENT  *injectModePtr,
    OUT GT_BOOL                                         *injectEnablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChDiagDataIntegrityErrorInjectionConfigGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, memType, injectModePtr, injectEnablePtr));

    rc = internal_cpssDxChDiagDataIntegrityErrorInjectionConfigGet(devNum, memType, injectModePtr, injectEnablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, memType, injectModePtr, injectEnablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChDiagDataIntegrityErrorCountEnableSet
*
* DESCRIPTION:
*       Function enables/disable error counter.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - PP device number
*       memType     - type of memory(table)
*       errorType   - error type
*       countEnable - enable/disable error counter
*                       GT_TRUE - enable
*                       GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, memType, errorType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - on not suppoted memory type
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChDiagDataIntegrityErrorCountEnableSet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT      memType,
    IN  CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT   errorType,
    IN  GT_BOOL                                         countEnable
)
{
    GT_STATUS                   rc;    /* return code */
    GT_U32                      regAddr;     /* register address */
    GT_U32                      regData;     /* register data */
    GT_U32                      pipeIndex;   /* pipe index */
    GT_U32                      clientIndex; /* client index */
    GT_U32                      fieldOffset; /* register field offset */
    GT_U32                      fieldLength; /* register field length */
    GT_U32                      i;           /* loop iterator */
    PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_DFX_ADDR_STC  addrStr;     /* DFX address struct */
    GT_BOOL                     memoryFound = GT_FALSE; /* memory found status */
    PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_MAP_DB_STC    *dbArrayPtr; /* pointer to data integrity DB */
    GT_U32                                          dbArrayEntryNum; /* size of data integrity DB */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
                                            CPSS_XCAT2_E | CPSS_LION_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    prvCpssDxChDiagDataIntegrityDbPointerSet(devNum, &dbArrayPtr, &dbArrayEntryNum);

    for(i = 0; i < dbArrayEntryNum; i++)
    {
        if(dbArrayPtr[i].memType == memType)
        {
            memoryFound = GT_TRUE;

            if((dbArrayPtr[i].protectionType == CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_NONE_E) &&
               (dbArrayPtr[i].externalProtectionType == CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_NONE_E))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            /* there are no interrupts for MLL memories */
            if((memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MLL_TABLE_FIFO_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MLL_REPLICATIONS_IN_USE_FIFO_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MLL_CONF_TABLE_E))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }

            /* non-DFX memories not support counter disable */
            switch(memType)
            {
                case PRV_CPSS_DXCH_LION2_DIAG_DATA_INTEGRITY_EXT_PROTECTION_MEMORY_TYPE_CASES_MAC:
                    if((memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ROUTER_TCAM_E) ||
                       (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_POLICY_TCAM_E))
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                    }

                    if(countEnable == GT_TRUE)
                    {
                        return GT_OK;
                    }
                    else
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                    }
                default:
                    break;
            }


            pipeIndex = U32_GET_FIELD_MAC(dbArrayPtr[i].key, 12, 3);
            clientIndex = U32_GET_FIELD_MAC(dbArrayPtr[i].key, 7, 5);

            /* Set Pipe Select Register */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->dfxUnits.server.pipeSelect;
            rc = prvCpssHwPpWriteRegister(devNum, regAddr, (1 << pipeIndex));
            if(GT_OK != rc)
            {
                return rc;
            }

            /* build Memory Control Register address */
            addrStr.dfxUnitClientIdx = clientIndex;
            addrStr.dfxXsbSelect = 0;
            addrStr.dfxClientSelect = 0;
            addrStr.dfxClientMemoryNumber = U32_GET_FIELD_MAC(dbArrayPtr[i].key, 0, 7);
            addrStr.dfxClientMemoryRegisterOffset = PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_ADDR_TYPE_MEMORY_CONTROL_REG_CNS;

            rc = buildDfxAddr(&addrStr, &regAddr);
            if(GT_OK != rc)
            {
                return rc;
            }

            /* NOTE: In order to set configuration to other value then Disable,
                     the follow configuration bits must be set to Disable in
                     advanced: <Address Monitoring Int Select> */


            if(countEnable == GT_TRUE)
            {
                /* write to Memory Control Register - disable address monitoring mechanism */
                rc = prvCpssHwPpSetRegField(devNum, regAddr, 12, 2, 0);
                if(GT_OK != rc)
                {
                    return rc;
                }
            }

            if(dbArrayPtr[i].protectionType == CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_ECC_E)
            {
                fieldOffset = 5;
                fieldLength = 2;

                switch(errorType)
                {
                    case CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E:
                        regData = BOOL2BIT_MAC(countEnable);
                        break;

                    case CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E:
                        regData = BOOL2BIT_MAC(countEnable) ? 2 : 0;
                        break;

                    case CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_AND_MULTIPLE_ECC_E:
                        regData = BOOL2BIT_MAC(countEnable) ? 3 : 0;
                        break;

                    default:
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
            }
            else if(dbArrayPtr[i].protectionType == CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_PARITY_E)
            {
                if(errorType == CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E)
                {
                    fieldOffset = 6;
                    fieldLength = 1;
                    regData = BOOL2BIT_MAC(countEnable);
                }
                else
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
            }
            else
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            /* write to Memory Control Register - enable/disable error counter */
            rc = prvCpssHwPpSetRegField(devNum, regAddr, fieldOffset, fieldLength, regData);
            if(GT_OK != rc)
            {
                return rc;
            }
        }
    }

    if((i == dbArrayEntryNum) && (memoryFound == GT_FALSE))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChDiagDataIntegrityErrorCountEnableSet
*
* DESCRIPTION:
*       Function enables/disable error counter.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - PP device number
*       memType     - type of memory(table)
*       errorType   - error type
*       countEnable - enable/disable error counter
*                       GT_TRUE - enable
*                       GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, memType, errorType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - on not suppoted memory type
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagDataIntegrityErrorCountEnableSet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT      memType,
    IN  CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT   errorType,
    IN  GT_BOOL                                         countEnable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChDiagDataIntegrityErrorCountEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, memType, errorType, countEnable));

    rc = internal_cpssDxChDiagDataIntegrityErrorCountEnableSet(devNum, memType, errorType, countEnable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, memType, errorType, countEnable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChDiagDataIntegrityErrorCountEnableGet
*
* DESCRIPTION:
*       Function gets status of error counter.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - PP device number
*       memType     - type of memory(table)
*
* OUTPUTS:
*       errorTypePtr   - (pointer to) error type
*       countEnablePtr - (pointer to) status of error counter
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, portGroup, memType, errorType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChDiagDataIntegrityErrorCountEnableGet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT      memType,
    OUT CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT   *errorTypePtr,
    OUT GT_BOOL                                         *countEnablePtr
)
{
    GT_STATUS                   rc;          /* return code */
    GT_U32                      regAddr;     /* register address */
    GT_U32                      regData;     /* register data */
    GT_U32                      pipeIndex;   /* pipe index */
    GT_U32                      clientIndex; /* client index */
    GT_U32                      i;           /* loop iterator */
    PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_DFX_ADDR_STC  addrStr;     /* DFX address struct */
    GT_BOOL                     memoryFound = GT_FALSE; /* memory found status */
    PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_MAP_DB_STC    *dbArrayPtr; /* pointer to data integrity DB */
    GT_U32                                          dbArrayEntryNum; /* size of data integrity DB */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
                                            CPSS_XCAT2_E | CPSS_LION_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);
    CPSS_NULL_PTR_CHECK_MAC(countEnablePtr);
    CPSS_NULL_PTR_CHECK_MAC(errorTypePtr);

    prvCpssDxChDiagDataIntegrityDbPointerSet(devNum, &dbArrayPtr, &dbArrayEntryNum);

    for(i = 0; i < dbArrayEntryNum; i++)
    {
        if(dbArrayPtr[i].memType == memType)
        {
            memoryFound = GT_TRUE;

            if((dbArrayPtr[i].protectionType == CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_NONE_E) &&
               (dbArrayPtr[i].externalProtectionType == CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_NONE_E))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            /* there are no interrupts for MLL memories */
            if((memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MLL_TABLE_FIFO_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MLL_REPLICATIONS_IN_USE_FIFO_E) ||
               (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MLL_CONF_TABLE_E))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }

            /* non-DFX memories not support counter disable */
            switch(memType)
            {
                case PRV_CPSS_DXCH_LION2_DIAG_DATA_INTEGRITY_EXT_PROTECTION_MEMORY_TYPE_CASES_MAC:
                    if((memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ROUTER_TCAM_E) ||
                       (memType == CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_POLICY_TCAM_E))
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                    }

                    if(dbArrayPtr[i].externalProtectionType == CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_PARITY_E)
                    {
                        *errorTypePtr = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                    }
                    else
                    {
                        *errorTypePtr = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_AND_MULTIPLE_ECC_E;
                    }
                    *countEnablePtr = GT_TRUE;
                    return GT_OK;

                default:
                    break;
            }


            pipeIndex = U32_GET_FIELD_MAC(dbArrayPtr[i].key, 12, 3);
            clientIndex = U32_GET_FIELD_MAC(dbArrayPtr[i].key, 7, 5);

            /* Set Pipe Select Register */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->dfxUnits.server.pipeSelect;
            rc = prvCpssHwPpWriteRegister(devNum, regAddr, (1 << pipeIndex));
            if(GT_OK != rc)
            {
                return rc;
            }

            /* build Memory Control Register address */
            addrStr.dfxUnitClientIdx = clientIndex;
            addrStr.dfxXsbSelect = 0;
            addrStr.dfxClientSelect = 0;
            addrStr.dfxClientMemoryNumber = U32_GET_FIELD_MAC(dbArrayPtr[i].key, 0, 7);
            addrStr.dfxClientMemoryRegisterOffset = PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_ADDR_TYPE_MEMORY_CONTROL_REG_CNS;

            rc = buildDfxAddr(&addrStr, &regAddr);
            if(GT_OK != rc)
            {
                return rc;
            }


            /* read from Memory Control Register - enable/disable error counter */
            rc = prvCpssHwPpGetRegField(devNum, regAddr, 5, 2, &regData);
            if(GT_OK != rc)
            {
                return rc;
            }


            if(dbArrayPtr[i].protectionType == CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_ECC_E)
            {
                switch(regData)
                {
                    case 0:
                        *errorTypePtr = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_AND_MULTIPLE_ECC_E;
                        *countEnablePtr = GT_FALSE;
                        break;

                    case 1:
                        *countEnablePtr = GT_TRUE;
                        *errorTypePtr = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E;
                        break;

                    case 2:
                        *countEnablePtr = GT_TRUE;
                        *errorTypePtr = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E;
                        break;

                    default:
                        *errorTypePtr = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_AND_MULTIPLE_ECC_E;
                        *countEnablePtr = GT_TRUE;
                        break;
                }
            }
            else if(dbArrayPtr[i].protectionType == CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_PARITY_E)
            {
                *errorTypePtr = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
                *countEnablePtr = BIT2BOOL_MAC((regData >> 1) & 0x1);
            }
            else
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            break;
        }
    }

    if((i == dbArrayEntryNum) && (memoryFound == GT_FALSE))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChDiagDataIntegrityErrorCountEnableGet
*
* DESCRIPTION:
*       Function gets status of error counter.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - PP device number
*       memType     - type of memory(table)
*
* OUTPUTS:
*       errorTypePtr   - (pointer to) error type
*       countEnablePtr - (pointer to) status of error counter
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, portGroup, memType, errorType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagDataIntegrityErrorCountEnableGet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT      memType,
    OUT CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT   *errorTypePtr,
    OUT GT_BOOL                                         *countEnablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChDiagDataIntegrityErrorCountEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, memType, errorTypePtr, countEnablePtr));

    rc = internal_cpssDxChDiagDataIntegrityErrorCountEnableGet(devNum, memType, errorTypePtr, countEnablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, memType, errorTypePtr, countEnablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChDiagDataIntegrityProtectionTypeGet
*
* DESCRIPTION:
*       Function gets memory protection type.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - PP device number
*       memType     - type of memory(table)
*
* OUTPUTS:
*       protectionTypePtr - (pointer to) memory protection type
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, portGroup, memType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChDiagDataIntegrityProtectionTypeGet
(
    IN  GT_U8                                                   devNum,
    IN  CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT              memType,
    OUT CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_ENT  *protectionTypePtr
)
{
    GT_U32  i; /* loop iterator */
    PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_MAP_DB_STC    *dbArrayPtr; /* pointer to data integrity DB */
    GT_U32                                          dbArrayEntryNum; /* size of data integrity DB */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
                                            CPSS_XCAT2_E | CPSS_LION_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);
    CPSS_NULL_PTR_CHECK_MAC(protectionTypePtr);


    prvCpssDxChDiagDataIntegrityDbPointerSet(devNum, &dbArrayPtr, &dbArrayEntryNum);

    for(i = 0; i < dbArrayEntryNum; i++)
    {
        if(dbArrayPtr[i].memType == memType)
        {
            switch(memType)
            {
                case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MLL_TABLE_FIFO_E:
                case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MLL_REPLICATIONS_IN_USE_FIFO_E:
                case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_MLL_CONF_TABLE_E:
                    /* there are no interrupts for these memory types */
                    *protectionTypePtr = CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_NONE_E;
                    break;
                case PRV_CPSS_DXCH_LION2_DIAG_DATA_INTEGRITY_EXT_PROTECTION_MEMORY_TYPE_CASES_MAC:
                    *protectionTypePtr = dbArrayPtr[i].externalProtectionType;
                    break;
                default:
                    *protectionTypePtr = dbArrayPtr[i].protectionType;
                    break;
            }

            break;
        }
    }

    if(i == dbArrayEntryNum)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    else
    {
        return GT_OK;
    }
}

/*******************************************************************************
* cpssDxChDiagDataIntegrityProtectionTypeGet
*
* DESCRIPTION:
*       Function gets memory protection type.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - PP device number
*       memType     - type of memory(table)
*
* OUTPUTS:
*       protectionTypePtr - (pointer to) memory protection type
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, portGroup, memType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagDataIntegrityProtectionTypeGet
(
    IN  GT_U8                                                   devNum,
    IN  CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT              memType,
    OUT CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_ENT  *protectionTypePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChDiagDataIntegrityProtectionTypeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, memType, protectionTypePtr));

    rc = internal_cpssDxChDiagDataIntegrityProtectionTypeGet(devNum, memType, protectionTypePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, memType, protectionTypePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChDiagDataIntegrityTcamParityDaemonEnableSet
*
* DESCRIPTION:
*       Function enables/disables TCAM parity daemon.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - PP device number
*       memType     - type of memory(table)
*                     only Router TCAM and Policy TCAM supported
*       enable      - GT_TRUE - enable daemon
*                     GT_FALSE - disable daemon
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, memType
*       GT_BAD_STATE             - on tcamParityCalcEnable is disabled
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       API is supported only if tcamParityCalcEnable is enabled by cpssDxChPpPhase1Init
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChDiagDataIntegrityTcamParityDaemonEnableSet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT  memType,
    IN  GT_BOOL                                     enable
)
{
    GT_U32  regAddr; /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
                                            CPSS_XCAT2_E | CPSS_LION_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    /* validity check */
    if((PRV_CPSS_DXCH_PP_MAC(devNum)->diagInfo.tcamParityCalcEnable == GT_FALSE) && (enable == GT_TRUE))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    switch(memType)
    {
        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ROUTER_TCAM_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.ipTccRegs.ipTcamControl;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_POLICY_TCAM_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.pclTccRegs.policyTcamControl;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssHwPpSetRegField(devNum, regAddr, 28, 1, BOOL2BIT_MAC(enable));
}

/*******************************************************************************
* cpssDxChDiagDataIntegrityTcamParityDaemonEnableSet
*
* DESCRIPTION:
*       Function enables/disables TCAM parity daemon.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - PP device number
*       memType     - type of memory(table)
*                     only Router TCAM and Policy TCAM supported
*       enable      - GT_TRUE - enable daemon
*                     GT_FALSE - disable daemon
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, memType
*       GT_BAD_STATE             - on tcamParityCalcEnable is disabled
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       API is supported only if tcamParityCalcEnable is enabled by cpssDxChPpPhase1Init
*
*******************************************************************************/
GT_STATUS cpssDxChDiagDataIntegrityTcamParityDaemonEnableSet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT  memType,
    IN  GT_BOOL                                     enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChDiagDataIntegrityTcamParityDaemonEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, memType, enable));

    rc = internal_cpssDxChDiagDataIntegrityTcamParityDaemonEnableSet(devNum, memType, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, memType, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChDiagDataIntegrityTcamParityDaemonEnableGet
*
* DESCRIPTION:
*       Function gets status of TCAM parity daemon.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - PP device number
*       memType     - type of memory(table)
*                     only Router TCAM and Policy TCAM supported
*
* OUTPUTS:
*       enablePtr   - (pointer to) daemon status
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, memType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChDiagDataIntegrityTcamParityDaemonEnableGet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT  memType,
    OUT GT_BOOL                                     *enablePtr
)
{
    GT_STATUS rc; /* return code */
    GT_U32  regAddr; /* register address */
    GT_U32  regData; /* register data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
                                            CPSS_XCAT2_E | CPSS_LION_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);


    switch(memType)
    {
        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ROUTER_TCAM_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.ipTccRegs.ipTcamControl;
            break;

        case CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_POLICY_TCAM_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.pclTccRegs.policyTcamControl;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 28, 1, &regData);
    if(GT_OK != rc)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(regData);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChDiagDataIntegrityTcamParityDaemonEnableGet
*
* DESCRIPTION:
*       Function gets status of TCAM parity daemon.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - PP device number
*       memType     - type of memory(table)
*                     only Router TCAM and Policy TCAM supported
*
* OUTPUTS:
*       enablePtr   - (pointer to) daemon status
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, memType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagDataIntegrityTcamParityDaemonEnableGet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT  memType,
    OUT GT_BOOL                                     *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChDiagDataIntegrityTcamParityDaemonEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, memType, enablePtr));

    rc = internal_cpssDxChDiagDataIntegrityTcamParityDaemonEnableGet(devNum, memType, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, memType, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* prvCpssDfxMemoryRegRead
*
* DESCRIPTION:
*       Function reads DFX memory registers.
*
* APPLICABLE DEVICES:
*       Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion.
*
* INPUTS:
*       devNum      - PP device number
*       pipeId      - DFX pipe ID
*       clientId    - DFX client ID
*       memNumber   - DFX memory ID
*       memReg      - DFX memory register address
*
* OUTPUTS:
*       regDataPtr  - (pointer to) register data
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*       GT_BAD_PTR               - on NULL pointer.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDfxMemoryRegRead
(
    IN GT_U8 devNum,
    IN GT_U32 pipeId,
    IN GT_U32 clientId,
    IN GT_U32 memNumber,
    IN GT_U32 memReg,
    OUT GT_U32 *regDataPtr
)
{
    GT_STATUS rc;    /* return code */
    GT_U32 regAddr;     /* register address */
    PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_DFX_ADDR_STC      addrStr;     /* DFX address struct */

    CPSS_NULL_PTR_CHECK_MAC(regDataPtr);

    /* Set Pipe Select Register */
    rc = prvCpssDfxMemoryPipeIdSet(devNum, pipeId);
    if(GT_OK != rc)
    {
        return rc;
    }

    if( CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily )
    {
        /* Read Memory ID STATUS 0x34 */
        addrStr.dfxUnitClientIdx = clientId;
        addrStr.dfxXsbSelect = 0;
        addrStr.dfxClientSelect = 0;
        addrStr.dfxClientMemoryNumber = memNumber;
        addrStr.dfxClientMemoryRegisterOffset = memReg;

        rc = buildDfxAddr(&addrStr, &regAddr);
        if(GT_OK != rc)
        {
            return rc;
        }

        rc = prvCpssHwPpReadRegister(devNum, regAddr, regDataPtr);
    }
    else /* CPSS_PP_FAMILY_DXCH_BOBCAT2_E */
    {
        rc = prvCpssDfxMemoryRegAddressGet(clientId,
                                           memNumber,
                                           memReg,
                                           &regAddr);
        if(GT_OK != rc)
        {
            return rc;
        }

        rc = prvCpssDrvHwPpResetAndInitControllerReadReg(devNum,
                                                         regAddr,
                                                         regDataPtr);
    }

    return rc;
}


/*******************************************************************************
* prvCpssDfxMemoryRegWrite
*
* DESCRIPTION:
*       Function writes DFX memory registers.
*
* APPLICABLE DEVICES:
*       Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion.
*
* INPUTS:
*       devNum      - PP device number
*       pipeId      - DFX pipe ID
*       clientId    - DFX client ID
*       memNumber   - DFX memory ID
*       memReg      - DFX memory register address
*       regData     - register data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*       GT_BAD_PTR               - on NULL pointer.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDfxMemoryRegWrite
(
    IN GT_U8 devNum,
    IN GT_U32 pipeId,
    IN GT_U32 clientId,
    IN GT_U32 memNumber,
    IN GT_U32 memReg,
    IN GT_U32 regData
)
{
    GT_STATUS rc;    /* return code */
    GT_U32 regAddr;     /* register address */
    PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_DFX_ADDR_STC      addrStr;     /* DFX address struct */

    /* Set Pipe Select Register */
    rc = prvCpssDfxMemoryPipeIdSet(devNum, pipeId);
    if(GT_OK != rc)
    {
        return rc;
    }
    if( CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily )
    {
        /* Read Memory ID STATUS 0x34 */
        addrStr.dfxUnitClientIdx = clientId;
        addrStr.dfxXsbSelect = 0;
        addrStr.dfxClientSelect = 0;
        addrStr.dfxClientMemoryNumber = memNumber;
        addrStr.dfxClientMemoryRegisterOffset = memReg;

        rc = buildDfxAddr(&addrStr, &regAddr);
        if(GT_OK != rc)
        {
            return rc;
        }

        rc = prvCpssHwPpWriteRegister(devNum, regAddr, regData);
    }
    else /* CPSS_PP_FAMILY_DXCH_BOBCAT2_E */
    {
        rc = prvCpssDfxMemoryRegAddressGet(clientId,
                                           memNumber,
                                           memReg,
                                           &regAddr);
        if(GT_OK != rc)
        {
            return rc;
        }

        rc = prvCpssDrvHwPpResetAndInitControllerWriteReg(devNum,
                                                          regAddr,
                                                          regData);
    }

    return rc;
}

/*******************************************************************************
* prvCpssDfxMemoryErrorInjection
*
* DESCRIPTION:
*       Inject error to a selected DFX client RAM.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum      - PP device number
*       pipeId      - DFX pipe ID
*       clientId    - DFX client ID
*       memNumber   - DFX memory ID
*       mode        - error injection mode
*       enable      - enable/disable error injection
*                         GT_TRUE - enable
*                         GT_FALSE - disable

*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDfxMemoryErrorInjectionSet
(
    IN GT_U8                                            devNum,
    IN GT_U32                                           pipeId,
    IN GT_U32                                           clientId,
    IN GT_U32                                           memNumber,
    IN CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_ENT   mode,
    IN GT_BOOL                                          enable
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      regData;    /* register data */

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dfxServer.supported == GT_FALSE)
    {
        return GT_OK;
    }

    rc = prvCpssDfxMemoryRegRead(devNum, pipeId, clientId, memNumber,
                  PRV_CPSS_DXCH_DEV_RESET_AND_INIT_CONTROLLER_REGS_MAC(devNum)->
                                                           DFXRam.memoryControl,
                                         &regData);
    if(GT_OK != rc)
    {
        return rc;
    }

    /* Error Injection Mode */
    U32_SET_FIELD_MAC(regData, 2, 1,
      ((CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_SINGLE_E == mode) ? 0 : 1));

    if( GT_TRUE == enable )
    {
        /* Memory write stop (used by other units) */
        U32_SET_FIELD_MAC(regData, 10, 2, 0x3);
        /* Error Injection Enable */
        U32_SET_FIELD_MAC(regData, 1, 1, 1);

    }
    else /* GT_FALSE == enable */
    {
        /* Memory write stop disable */
        U32_SET_FIELD_MAC(regData, 10, 2, 0x0);
        /* Error Injection Disable */
        U32_SET_FIELD_MAC(regData, 1, 1, 0);
    }

    return prvCpssDfxMemoryRegWrite(devNum, pipeId, clientId, memNumber,
                  PRV_CPSS_DXCH_DEV_RESET_AND_INIT_CONTROLLER_REGS_MAC(devNum)->
                                                           DFXRam.memoryControl,
                                         regData);
}

/*******************************************************************************
* prvCpssDfxClientRegRead
*
* DESCRIPTION:
*       Function reads DFX client registers.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - PP device number
*       pipeId      - DFX pipe ID
*       clientId    - DFX client ID
*       clientReg   - DFX client register address
*
* OUTPUTS:
*       regDataPtr  - (pointer to) register data
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*       GT_BAD_PTR               - on NULL pointer.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDfxClientRegRead
(
    IN GT_U8 devNum,
    IN GT_U32 pipeId,
    IN GT_U32 clientId,
    IN GT_U32 clientReg,
    OUT GT_U32 *regDataPtr
)
{
    GT_STATUS rc;    /* return code */
    GT_U32 regAddr;     /* register address */
    GT_U32 regData;     /* register data */
    PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_DFX_ADDR_STC      addrStr;     /* DFX address struct */

    /* Set Pipe Select Register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->dfxUnits.server.pipeSelect;
    rc = prvCpssHwPpWriteRegister(devNum, regAddr, (1 << pipeId));
    if(GT_OK != rc)
    {
        return rc;
    }

    addrStr.dfxUnitClientIdx = clientId;
    addrStr.dfxXsbSelect = 0;
    addrStr.dfxClientSelect = 1;
    addrStr.dfxClientRegisterOffset = clientReg;

    rc = buildDfxAddr(&addrStr, &regAddr);
    if(GT_OK != rc)
    {
        return rc;
    }

    rc = prvCpssHwPpReadRegister(devNum, regAddr, &regData);
    if(GT_OK != rc)
    {
        return rc;
    }

    *regDataPtr = regData;

    return GT_OK;
}


/*******************************************************************************
* prvCpssDfxMemoryPipeIdSet
*
* DESCRIPTION:
*       Function sets DFX pipe select register.
*
* APPLICABLE DEVICES:
*       Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion.
*
* INPUTS:
*       devNum      - PP device number
*       pipeId      - DFX pipe ID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDfxMemoryPipeIdSet
(
    IN GT_U8 devNum,
    IN GT_U32 pipeId
)
{
    GT_STATUS rc;   /* return code */
    GT_U32 regAddr; /* register address */

    if( CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily )
    {
        /* Set Pipe Select Register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->dfxUnits.server.pipeSelect;
        rc = prvCpssHwPpWriteRegister(devNum, regAddr, (1 << pipeId));
    }
    else /* CPSS_PP_FAMILY_DXCH_BOBCAT2_E */
    {
        regAddr = PRV_CPSS_DXCH_DEV_RESET_AND_INIT_CONTROLLER_REGS_MAC(devNum)->
                                    DFXServerUnits.DFXServerRegs.pipeSelect;
        rc = prvCpssDrvHwPpResetAndInitControllerWriteReg(devNum,
                                                         regAddr,
                                                         (1 << pipeId));
    }

    return rc;
}


/*******************************************************************************
* prvCpssDfxMemoryPipeIdGet
*
* DESCRIPTION:
*       Function sets DFX pipe select register.
*
* APPLICABLE DEVICES:
*       Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion.
*
* INPUTS:
*       devNum      - PP device number
*
* OUTPUTS:
*       pipeIdPtr   - (pointer to) current DFX pipe ID
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDfxMemoryPipeIdGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *pipeIdPtr
)
{
    GT_STATUS rc;   /* return code */
    GT_U32 regAddr; /* register address */
    GT_U32 regData; /* register data */

    if( CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily )
    {
        /* Set Pipe Select Register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->dfxUnits.server.pipeSelect;
        rc = prvCpssHwPpReadRegister(devNum, regAddr, &regData);
    }
    else /* CPSS_PP_FAMILY_DXCH_BOBCAT2_E */
    {
        regAddr = PRV_CPSS_DXCH_DEV_RESET_AND_INIT_CONTROLLER_REGS_MAC(devNum)->
                                    DFXServerUnits.DFXServerRegs.pipeSelect;
        rc = prvCpssDrvHwPpResetAndInitControllerReadReg(devNum,
                                                         regAddr,
                                                         &regData);
    }

    if(GT_OK != rc)
    {
        return rc;
    }

    *pipeIdPtr = regData;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDfxMemoryAllPipesSet
*
* DESCRIPTION:
*       Function sets all pipes in DFX pipe select register.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum      - PP device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDfxMemoryAllPipesSet
(
    IN GT_U8 devNum
)
{
    GT_STATUS rc;       /* return code */
    GT_U32 regAddr;     /* register address */
    GT_U32 allPipesBmp; /* bitmap of all pipes */

    regAddr = PRV_CPSS_DXCH_DEV_RESET_AND_INIT_CONTROLLER_REGS_MAC(devNum)->
                                    DFXServerUnits.DFXServerRegs.pipeSelect;
    if (!PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        /* pipes 0 & 1 */
        allPipesBmp = 3;
    }
    else
    {
        /* pipes 0, 1, 2 */
        allPipesBmp = 7;
    }

    /* Active all pipes */
    rc = prvCpssDrvHwPpResetAndInitControllerWriteReg(devNum, regAddr, allPipesBmp);

    return rc;
}

/*******************************************************************************
* prvCpssDfxClientRegAddressGet
*
* DESCRIPTION:
*       Get DFX client register address
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       client           - DFX client number.
*       regAddr          - Relative register address in client memory space.
*
* OUTPUTS:
*       clientRegAddrPtr - (pointer to) register addrress in specific client
*                          memory space.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong client number
*       GT_BAD_PTR   - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDfxClientRegAddressGet
(
    IN  GT_U32   client,
    IN  GT_U32   regAddr,
    OUT GT_U32   *clientRegAddrPtr
)
{
    /* There are 31 clients:    */
    /* 0-27 simple pipe clients */
    /* 28 - BC client           */
    /* 29 - MC client           */
    /* 30 - data logging client */
    if( client > 0x1E )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    *clientRegAddrPtr = (client << 15) + regAddr;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDfxMemoryRegAddressGet
*
* DESCRIPTION:
*       Get DFX memory register address
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       client  - DFX client number.
*       memory  - DFX client RAM number.
*       regAddr - Relative register address in RAM space.
*
* OUTPUTS:
*       memoryRegAddrPtr - (pointer to) register addrress in for specific client
*                          memory.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong client or RAM number
*       GT_BAD_PTR   - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDfxMemoryRegAddressGet
(
    IN  GT_U32   client,
    IN  GT_U32   memory,
    IN  GT_U32   regAddr,
    OUT GT_U32   *memoryRegAddrPtr
)
{
    /* There are 31 clients:    */
    /* 0-27 simple pipe clients */
    /* 28 - BC client           */
    /* 29 - MC client           */
    /* 30 - data logging client */
    if( client > 0x1E )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* There are up to 128 RAMs per client */
    if( memory > 0x7F )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    *memoryRegAddrPtr = (client << 15) + (memory << 6) + regAddr;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDfxClientSetRegField
*
* DESCRIPTION:
*       Function set a field in a DFX client register.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - The PP device number to write to.
*       pipe            - DFX pipe the client belongs to.
*                         (not relevant for BC, MC or Data logging clients).
*       client          - DFX client to write to.
*       clientRegAddr   - The client register's address to write to.
*       fieldOffset     - The start bit number in the register.
*       fieldLength     - The number of bits to be written to register.
*       fieldData       - Data to be written into the register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       Since reading BC, MC or Data logging DFX clients registers is prohibited
*       client 0 value is read to emulate the default or already existing value.
*
*******************************************************************************/
GT_STATUS prvCpssDfxClientSetRegField
(
    IN GT_U8    devNum,
    IN GT_U32   pipe,
    IN GT_U32   client,
    IN GT_U32   clientRegAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    IN GT_U32   fieldData
)
{
    GT_U32  regAddr = 0;    /* register address */
    GT_U32  regData;    /* register data */
    GT_STATUS rc;       /* return code */

    if( (0x1C == client) || (0x1D == client) || (0x1E == client) )
    {
        /* BC, MC or Data logging DFX client */

        if(fieldLength != 32)
        {
            /* read register's data from reference client.
               calculate address */
            rc = prvCpssDfxClientRegAddressGet(PRV_CPSS_DXCH_PP_MAC(devNum)->diagInfo.dfxMcAccessGetClient, clientRegAddr, &regAddr);
            if( GT_OK != rc )
            {
                return rc;
            }        

            /* Activate reference pipe */
            rc = prvCpssDfxMemoryPipeIdSet(devNum, PRV_CPSS_DXCH_PP_MAC(devNum)->diagInfo.dfxMcAccessGetPipe);
            if( GT_OK != rc )
            {
                return rc;
            }

            /* Read register value in reference client to use as deafult */
            rc = prvCpssDrvHwPpResetAndInitControllerReadReg(devNum, regAddr, &regData);
            if( GT_OK != rc )
            {
                return rc;
            }
        }
        else
        {
            /* full register update. Need not to read data from reference client. */
            regData = 0;
        }

        /* Enable all pipes */
        rc = prvCpssDfxMemoryAllPipesSet(devNum);
        if( GT_OK != rc )
        {
            return rc;
        }

        U32_SET_FIELD_MAC(regData, fieldOffset, fieldLength, fieldData);

        rc = prvCpssDfxClientRegAddressGet(client, clientRegAddr, &regAddr);
        if( GT_OK != rc )
        {
            return rc;
        }

        /* Write to requested client */
        rc = prvCpssDrvHwPpResetAndInitControllerWriteReg(devNum, regAddr, regData);
    }
    else /* "Regular" DFX client*/
    {
        rc = prvCpssDfxClientRegAddressGet(client, clientRegAddr, &regAddr);
        if( GT_OK != rc )
        {
            return rc;
        }

        /* Activate pipe */
        rc = prvCpssDfxMemoryPipeIdSet(devNum, pipe);
        if( GT_OK != rc )
        {
            return rc;
        }

        rc = prvCpssDrvHwPpResetAndInitControllerSetRegField(devNum,
                                                             regAddr,
                                                             fieldOffset,
                                                             fieldLength,
                                                             fieldData);
    }

    return rc;
}

/*******************************************************************************
* prvCpssDxChDiagDataIntegrityMemoryIndexesGet
*
* DESCRIPTION:
*       Function gets all memory location coordinates for given memory type.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum         - PP device number
*       memType        - memory type
*       isPerPortGroup - filtering flag
*                        GT_TRUE - only per given portgroup id
*                        GT_FALSE - all
*       portGroupId    - portgroup id
*                        relevant only if isPerPortGroup = GT_TRUE
*       arraySizePtr   - size of input array
*
* OUTPUTS:
*       arraySizePtr   - actual size of input array
*       memLocationArr - array of memory locations
*       protectionTypePtr - (pointer to) protection memory type
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChDiagDataIntegrityMemoryIndexesGet
(
    IN  GT_U8                                                   devNum,
    IN  CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT              memType,
    IN  GT_BOOL                                                 isPerPortGroup,
    IN  GT_U32                                                  portGroupId,
    INOUT GT_U32                                                *arraySizePtr,
    OUT CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_STC            *memLocationArr,
    OUT CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_ENT  *protectionTypePtr
)
{
    GT_U32                      i;
    GT_U32                      moduleCount = 0;
    GT_BOOL                     memoryFound = GT_FALSE; /* memory found status */
    PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_MAP_DB_STC    *dbArrayPtr; /* pointer to data integrity DB */
    GT_U32                                          dbArrayEntryNum; /* size of data integrity DB */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
                                            CPSS_XCAT2_E | CPSS_LION_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    prvCpssDxChDiagDataIntegrityDbPointerSet(devNum, &dbArrayPtr, &dbArrayEntryNum);

    for(i = 0; i < dbArrayEntryNum; i++)
    {
        if(dbArrayPtr[i].memType == memType)
        {
            if(isPerPortGroup == GT_TRUE)
            {
                if(portGroupId != dbArrayPtr[i].causePortGroupId)
                {
                    continue;
                }
            }

            if(0 == (PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.activePortGroupsBmp & (1<<(dbArrayPtr[i].causePortGroupId))))
            {
                continue;
            }

            memoryFound = GT_TRUE;
            if(moduleCount >= *arraySizePtr)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_SIZE, LOG_ERROR_NO_MSG);
            }

            *protectionTypePtr = dbArrayPtr[i].protectionType;
            memLocationArr[moduleCount].dfxPipeId = U32_GET_FIELD_MAC(dbArrayPtr[i].key, 12, 3);
            memLocationArr[moduleCount].dfxClientId = U32_GET_FIELD_MAC(dbArrayPtr[i].key, 7, 5);
            memLocationArr[moduleCount].dfxMemoryId = U32_GET_FIELD_MAC(dbArrayPtr[i].key, 0, 7);
            moduleCount++;
        }
    }

    *arraySizePtr = moduleCount;

    if(i == dbArrayEntryNum)
    {
        if(memoryFound == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChDiagDataIntegrityMemoryInstancesCountGet
*
* DESCRIPTION:
*       Function returns number of memory instances in DB.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - PP device number
*       memType     - memory type
*
* OUTPUTS:
*       counterPtr  - (pointer to) memory counter
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChDiagDataIntegrityMemoryInstancesCountGet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT      memType,
    OUT GT_U32                                          *counterPtr
)
{
    GT_U32                      i;
    GT_U32                      moduleCount = 0;
    GT_BOOL                     memoryFound = GT_FALSE; /* memory found status */
    PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_MAP_DB_STC    *dbArrayPtr; /* pointer to data integrity DB */
    GT_U32                                          dbArrayEntryNum; /* size of data integrity DB */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
                                            CPSS_XCAT2_E | CPSS_LION_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    prvCpssDxChDiagDataIntegrityDbPointerSet(devNum, &dbArrayPtr, &dbArrayEntryNum);

    for(i = 0; i < dbArrayEntryNum; i++)
    {
        if(dbArrayPtr[i].memType == memType)
        {
            memoryFound = GT_TRUE;
            moduleCount++;
        }
    }

    *counterPtr = moduleCount;

    if(i == dbArrayEntryNum)
    {
        if(memoryFound == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChDiagDataIntegrityPortGroupMemoryIndexesGet
*
* DESCRIPTION:
*       Function returns first instance of specific memory type from DB per
*       given port group.
*
* APPLICABLE DEVICES:
*       Lion2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - PP device number
*       portGroupId - port group id
*       memType     - memory type
*
* OUTPUTS:
*       memLocationPtr - (pointer to) location
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChDiagDataIntegrityPortGroupMemoryIndexesGet
(
    IN  GT_U8                                           devNum,
    IN  GT_U32                                          portGroupId,
    IN  CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT      memType,
    OUT CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_STC    *memLocationPtr
)
{
    GT_STATUS rc; /* return code */
    GT_U32 arraySize;
    CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_ENT  protectionType;
    CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_STC    *memLocationArr;

    rc = prvCpssDxChDiagDataIntegrityMemoryInstancesCountGet(devNum, memType, &arraySize);
    if(GT_OK != rc)
    {
        return rc;
    }

    memLocationArr = (CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_STC*)cpssOsMalloc(arraySize*sizeof(CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_STC));
    cpssOsMemSet(memLocationArr, 0, arraySize*sizeof(CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_STC));

    rc = prvCpssDxChDiagDataIntegrityMemoryIndexesGet(devNum,
                                                      memType,
                                                      GT_TRUE, portGroupId,
                                                      &arraySize, memLocationArr, &protectionType);
    if(GT_OK != rc)
    {
        cpssOsFree(memLocationArr);
        return rc;
    }

    memLocationPtr->dfxPipeId = memLocationArr[0].dfxPipeId;
    memLocationPtr->dfxClientId = memLocationArr[0].dfxClientId;
    memLocationPtr->dfxMemoryId = memLocationArr[0].dfxMemoryId;

    cpssOsFree(memLocationArr);

    return GT_OK;
}


