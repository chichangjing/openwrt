/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssTmStatistics.c
*
* DESCRIPTION:
*       TM statistics gathering APIs
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/generic/tm/private/prvCpssGenTmLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <cpss/generic/tm/cpssTmServices.h>
#include <cpss/generic/tm/cpssTmStatistics.h>
#include <tm_statistics.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>


/*******************************************************************************
* internal_cpssTmQmrPktStatisticsGet
*
* DESCRIPTION:
*       Read QMR Packet Statistics.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*
* OUTPUTS:
*       pktCntPtr       - (pointer to) QMR Pkt Statistics structure.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssTmQmrPktStatisticsGet
(
   IN   GT_U8                           devNum,
   OUT  CPSS_TM_QMR_PKT_STATISTICS_STC  *pktCntPtr
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    GT_U64  pkt_counter;

    ret = tm_qmr_get_num_pkts_to_unins_queue(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle,(U64*)&pkt_counter);
    rc = XEL_TO_CPSS_ERR_CODE(ret);
    if(rc)
        return rc;

    pktCntPtr->numPktsToUnInstallQueue.l[0] = pkt_counter.l[0];
    pktCntPtr->numPktsToUnInstallQueue.l[1] = pkt_counter.l[1];

    return rc;
}

/*******************************************************************************
* cpssTmQmrPktStatisticsGet
*
* DESCRIPTION:
*       Read QMR Packet Statistics.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*
* OUTPUTS:
*       pktCntPtr       - (pointer to) QMR Pkt Statistics structure.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmQmrPktStatisticsGet
(
   IN   GT_U8                           devNum,
   OUT  CPSS_TM_QMR_PKT_STATISTICS_STC  *pktCntPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmQmrPktStatisticsGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, pktCntPtr));

    rc = internal_cpssTmQmrPktStatisticsGet(devNum, pktCntPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, pktCntPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmRcbPktStatisticsGet
*
* DESCRIPTION:
*       Read RCB Packet Statistics.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*
* OUTPUTS:
*       pktCntPtr       - (pointer to) RCB Pkt Statistics structure.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssTmRcbPktStatisticsGet
(
   IN   GT_U8                           devNum,
   OUT  CPSS_TM_RCB_PKT_STATISTICS_STC  *pktCntPtr
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    GT_U64  pkt_counter;
    GT_U32  mask;

    ret = tm_rcb_get_num_pkts_to_sms(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle,(U64*) &pkt_counter);
    rc = XEL_TO_CPSS_ERR_CODE(ret);
    if(rc)
        return rc;
    pktCntPtr->numPktsToSms.l[0] = pkt_counter.l[0];
    pktCntPtr->numPktsToSms.l[1] = pkt_counter.l[1];

    ret = tm_rcb_get_num_crc_err_pkts_to_sms(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, (U64*)&pkt_counter);
    rc = XEL_TO_CPSS_ERR_CODE(ret);
    if(rc)
        return rc;
    pktCntPtr->numCrcErrPktsToSms.l[0] = pkt_counter.l[0];
    pktCntPtr->numCrcErrPktsToSms.l[1] = pkt_counter.l[1];

    ret = tm_rcb_get_num_errs_from_sms_to_dram(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, (U64*)&pkt_counter);
    rc = XEL_TO_CPSS_ERR_CODE(ret);
    if(rc)
        return rc;
    pktCntPtr->numErrsFromSmsToDram.l[0] = pkt_counter.l[0];
    pktCntPtr->numErrsFromSmsToDram.l[1] = pkt_counter.l[1];

    ret = tm_rcb_get_num_port_flush_drp_pkts(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, (U64*)&pkt_counter);
    rc = XEL_TO_CPSS_ERR_CODE(ret);
    if(rc)
        return rc;
    pktCntPtr->numPortFlushDrpDataPkts.l[0] = pkt_counter.l[0];
    pktCntPtr->numPortFlushDrpDataPkts.l[1] = pkt_counter.l[1];

    /* TBD: temporary solution */
    mask = 0x1FFFFFFF; /* [0:28] bits - Pkt counter */
    pktCntPtr->numPortFlushDrpDataPkts.l[0] = pkt_counter.l[0] & mask;
    pktCntPtr->numPortFlushDrpDataPkts.l[1] = 0;

    return rc;
}

/*******************************************************************************
* cpssTmRcbPktStatisticsGet
*
* DESCRIPTION:
*       Read RCB Packet Statistics.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*
* OUTPUTS:
*       pktCntPtr       - (pointer to) RCB Pkt Statistics structure.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmRcbPktStatisticsGet
(
   IN   GT_U8                           devNum,
   OUT  CPSS_TM_RCB_PKT_STATISTICS_STC  *pktCntPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmRcbPktStatisticsGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, pktCntPtr));

    rc = internal_cpssTmRcbPktStatisticsGet(devNum, pktCntPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, pktCntPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
