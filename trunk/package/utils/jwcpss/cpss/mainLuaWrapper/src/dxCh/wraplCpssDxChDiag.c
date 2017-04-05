/*******************************************************************************
*              (c), Copyright 2014, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplCpssDxChDiag.c
*
* DESCRIPTION:
*       A lua wrapper for Diag functions
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*******************************************************************************/


/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/diag/cpssDxChDiagDataIntegrity.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsLion2.h>

#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>
 
use_prv_struct(CPSS_DXCH_DIAG_DATA_INTEGRITY_EVENT_STC)

enumDescr enumDescr_MPRV_CPSS_DXCH_DIAG_EVENT_ADDITION_INFO_ENT[] = {
    ENUM_ENTRY(PRV_CPSS_LION2_TCC_LOWER_GEN_TCAM_ERROR_DETECTED_E),
    ENUM_ENTRY(PRV_CPSS_LION2_TCC_UPPER_GEN_TCAM_ERROR_DETECTED_E),
    ENUM_ENTRY(PRV_CPSS_LION2_TXDMA_ERROR_HA_INFO_FIFO_ECC_ONE_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_TXDMA_ERROR_HA_INFO_FIFO_ECC_TWO_OR_MORE_ERRORS_E),
    ENUM_ENTRY(PRV_CPSS_LION2_TXQ_EGRESS_0_GEN_MC_FIFO_0_PARITY_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_TXQ_EGRESS_1_GEN_MC_FIFO_0_PARITY_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_TXQ_EGRESS_0_GEN_MC_FIFO_1_PARITY_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_TXQ_EGRESS_1_GEN_MC_FIFO_1_PARITY_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_TXQ_EGRESS_0_GEN_MC_FIFO_3_PARITY_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_TXQ_EGRESS_1_GEN_MC_FIFO_3_PARITY_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_TXQ_SHT_GEN_EGR_VLAN_TBL_PARITY_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_TXQ_SHT_GEN_ING_VLAN_TBL_PARITY_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_TXQ_SHT_GEN_L2_PORT_ISOLATION_TBL_PARITY_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_TXQ_SHT_GEN_L3_PORT_ISOLATION_TBL_PARITY_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_TXQ_SHT_GEN_VIDX_TBL_PARITY_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_IPLR0_DATA_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_IPLR1_DATA_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_IP_ROUTER_NHE_DIP_DATA_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_IP_ROUTER_NHE_SIP_DATA_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_IP_ROUTER_STG2_DATA_FIFO_ECC_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_IP_ROUTER_STG4_DATA_FIFO_ECC_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_IP_ROUTER_UNUSED_DATA_FIFO_ECC_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_MPPM_0_BK0_ECC_1_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_MPPM_0_BK1_ECC_1_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_MPPM_1_BK0_ECC_1_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_MPPM_1_BK1_ECC_1_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_MPPM_0_BK0_ECC_2_OR_MORE_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_MPPM_0_BK1_ECC_2_OR_MORE_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_MPPM_1_BK0_ECC_2_OR_MORE_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_MPPM_1_BK1_ECC_2_OR_MORE_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_TXQ_DQ_MEMORY_ERROR_TOKEN_BUCKET_PRIO_PARITY_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_TXQ_DQ_MEMORY_ERROR_STATE_VARIABLE_PARITY_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_TXQ_LL_GEN_FBUF_ECC_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_TXQ_QUEUE_QCN_CN_BUFFER_FIFO_PARITY_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP0_COUNTERS_ERR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP1_COUNTERS_ERR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP2_COUNTERS_ERR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP3_COUNTERS_ERR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP4_COUNTERS_ERR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP5_COUNTERS_ERR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP6_COUNTERS_ERR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_TXQ_QUEUE_PFC_PARITY_PORT_GROUP7_COUNTERS_ERR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_BMA_CORE0_MCCNT_PARITY_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_BMA_CORE1_MCCNT_PARITY_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_BMA_CORE2_MCCNT_PARITY_ERROR_E),
    ENUM_ENTRY(PRV_CPSS_LION2_BMA_CORE3_MCCNT_PARITY_ERROR_E),
    { 0, NULL }
};
add_mgm_enum(MPRV_CPSS_DXCH_DIAG_EVENT_ADDITION_INFO_ENT);

/*******************************************************************************
* wrlCpssDxChDiagDataIntegrityEventsGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       returns array of data integrity events.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2
*
* INPUTS:
*       L                     - lua state
* 
* OUTPUTS:
*
* RETURNS:
*       status, CPSS_DXCH_DIAG_DATA_INTEGRITY_EVENT_STC[]
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChDiagDataIntegrityEventsGet
(
    IN lua_State *L
)
{
    GT_U8       devNum;
    GT_U32      portGroupId;
    GT_U32      evType = 0;
    GT_STATUS   status = GT_OK;
    GT_U32      evExtData;
    GT_U32      eventsNum = 20;     /* initial number of ECC/parity events */
    CPSS_DXCH_DIAG_DATA_INTEGRITY_EVENT_STC eventsArr[20];
    GT_BOOL     isScanFinished = GT_FALSE;     /* events scan finish status */
    GT_U32      i, idx;

    PARAM_NUMBER(status, devNum,      1, GT_U8);
    PARAM_NUMBER(status, portGroupId, 2, GT_U32);
    /* PARAM_ENUM  (status,  evType,  3, MPRV_CPSS_DXCH_DIAG_EVENT_ADDITION_INFO_ENT) */
    if (lua_type(L, 3) == LUA_TNUMBER || lua_type(L, 3) == LUA_TSTRING)
    {
        lua_pushvalue(L, 3);
        evType = (GT_U32)prv_lua_to_c_enum(L, enumDescr_MPRV_CPSS_DXCH_DIAG_EVENT_ADDITION_INFO_ENT);
        lua_pop(L, 1);
    } else {
        status = GT_BAD_PARAM;
    }
    
    evExtData = (portGroupId << 16) | evType;

    lua_pushinteger(L, (lua_Integer)status);
    if (status != GT_OK)
    {
        return 1;
    }

    lua_newtable(L);
    idx = 0;

    /* read data */
    while(isScanFinished != GT_TRUE)
    {
        status = cpssDxChDiagDataIntegrityEventsGet(devNum, evExtData, &eventsNum, eventsArr, &isScanFinished);
        if(GT_OK != status)
        {
            break;
        }

        for(i = 0; i < eventsNum; i++)
        {
            prv_c_to_lua_CPSS_DXCH_DIAG_DATA_INTEGRITY_EVENT_STC(L, &(eventsArr[i]));
            lua_rawseti(L, -2, ++idx);
        }
    }

    lua_pushinteger(L, status);
    lua_replace(L, -3);

    return 2;
}
