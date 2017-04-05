/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxSChCoS.h
*
* DESCRIPTION:
*       Includes structures definition for the use of Cheetah Prestera core SW.
*
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*
*******************************************************************************/

#ifndef __prvCpssDxChCoSh
#define __prvCpssDxChCoSh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>


#define PRV_CPSS_DXCH_COS_ENTRY_SIZE_CNS (4)


/* macro to convert user priority value to traffic class bit offset
    userPrio - User prority

*/
#define PRV_CPSS_DXCH_USER_PRIO_2_TC_BIT_OFFSET_MAC(userPrio) ((userPrio) * 3)

/* macro to traffic class value to user priority bit offset
    trfClass - trafic class

*/
#define PRV_CPSS_DXCH_TC_2_USER_PRIO_BIT_OFFSET_MAC(trfClass) ((trfClass) * 3)


/* macro to convert user priority value to Drop precedence bit offset
    userPrio - User prority

*/
#define PRV_CPSS_DXCH_USER_PRIO_2_DP_BIT_OFFSET_MAC(userPrio) ((userPrio) * 2)



/* macro to validate the value of user priority parameter (DxCh format)
    userPrio - User prority

*/
#define PRV_CPSS_DXCH_COS_CHECK_UP_MAC(userPrio) \
    if((userPrio) >= CPSS_USER_PRIORITY_RANGE_CNS)  \
    {                                 \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);          \
    }

/* macro to validate the value of trafic class parameter (DxCh format)
    trfClass - traffic class

*/
#define PRV_CPSS_DXCH_COS_CHECK_TC_MAC(trfClass) \
    if((trfClass) >= CPSS_TC_RANGE_CNS)  \
    {                                 \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);          \
    }

/* macro to validate the value of trafic class parameter (DxCh format)
    value4Tc - 4 traffic class

*/
#define PRV_CPSS_DXCH_COS_CHECK_4_TC_MAC(value4Tc) \
    if((value4Tc) >= CPSS_4_TC_RANGE_CNS)  \
    {                                 \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);          \
    }

/* macro to validate the value of trafic class parameter (DxCh format)
   according to fine tunning settings.
   trfClass - traffic class
*/
#define PRV_CPSS_DXCH_COS_CHECK_TX_QUEUES_NUM_MAC(_devNum, _trfClass)                   \
    if((_trfClass) >= PRV_CPSS_DXCH_PP_MAC(_devNum)->fineTuning.tableSize.txQueuesNum)  \
    {                                                                                   \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                            \
    }


/* macro to validate the value of exp field mpls shim header parameter (DxCh format)
    exp - exp feild mpls shim header

*/
#define PRV_CPSS_DXCH_COS_CHECK_EXP_MAC(exp) \
    if((exp) >= CPSS_EXP_RANGE_CNS)  \
    {                                 \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);          \
    }


/* macro to validate the value of DSCP parameter (DxCh format)
    DSCP - DSCP value

*/
#define PRV_CPSS_DXCH_COS_CHECK_DSCP_MAC(dscp) \
    if((dscp) >= CPSS_DSCP_RANGE_CNS)  \
    {                                 \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);          \
    }


#define PRV_CPSS_DXCH_COS_CHECK_DP_MAC(dpLevel) \
    switch(dpLevel)                             \
    {                                           \
        case CPSS_DP_GREEN_E:                   \
        case CPSS_DP_RED_E:                     \
            break;                              \
        default:                                \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                \
    }

/* convert DP level to HW values for DXCH3 and above devices */
#define PRV_CPSS_DXCH3_COS_DP_CONVERT_MAC(_dpLevel, _hwValue) \
    switch(_dpLevel)                                \
    {                                               \
        case CPSS_DP_GREEN_E:  _hwValue = 0; break; \
        case CPSS_DP_YELLOW_E: _hwValue = 1; break; \
        case CPSS_DP_RED_E:    _hwValue = 2; break; \
        default:                                    \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                    \
    }

/* convert HW values to DP level for DXCH3 and above devices */
#define PRV_CPSS_DXCH3_COS_DP_TO_SW_CONVERT_MAC(_hwValue, _dpLevel) \
    switch(_hwValue)                                 \
    {                                                \
        case 0:  _dpLevel = CPSS_DP_GREEN_E;  break; \
        case 1:  _dpLevel = CPSS_DP_YELLOW_E; break; \
        case 2:  _dpLevel = CPSS_DP_RED_E;    break; \
        default:                                     \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);                     \
    }

/* macro to validate the value of DP for DxCh3, dpLevel - DP value */
#define PRV_CPSS_DXCH3_COS_CHECK_DP_MAC(dpLevel)    \
    switch(dpLevel)                                 \
    {                                               \
        case CPSS_DP_GREEN_E:                       \
        case CPSS_DP_YELLOW_E:                      \
        case CPSS_DP_RED_E:                         \
            break;                                  \
        default:                                    \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                    \
    }

/* macro to validate the value of CFI/DEI bit[0..1] */
#define PRV_CPSS_DXCH3_CHECK_CFI_DEI_BIT_MAC(cfiDeiBit) \
    if(cfiDeiBit > BIT_0)                               \
    {                                                   \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                            \
    }

/* macro to validate the value of upProfileIndex -- as KEY */
/* should be checked only on Lion and above */
#define PRV_CPSS_DXCH_CHECK_KEY_UP_PROFILE_INDEX_MAC(_upProfileIndex) \
    if(_upProfileIndex > 1)                             \
    {                                                   \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                            \
    }

/* macro to validate the value of upProfileIndex -- as DATA */
/* should be checked only on Lion and above */
#define PRV_CPSS_DXCH_CHECK_DATA_UP_PROFILE_INDEX_MAC(_upProfileIndex) \
    if(_upProfileIndex > 1)                             \
    {                                                   \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);                         \
    }



/* check and convert DP level to HW values for DXCH3 and above devices */
/* if SW value not supported returns GT_BAD_PARAM                      */
#define PRV_CPSS_DXCH_COS_DP_TO_HW_CHECK_AND_CONVERT_MAC(              \
    _devNum, _swDp, _hwDp)                                             \
    if (0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(_devNum)) \
    {                                                                  \
        PRV_CPSS_DXCH_COS_CHECK_DP_MAC(_swDp);                         \
        _hwDp = (_swDp == CPSS_DP_GREEN_E) ? 0 : 1;                    \
    }                                                                  \
    else                                                               \
    {                                                                  \
        /* Lion and above */                                           \
        PRV_CPSS_DXCH3_COS_DP_CONVERT_MAC(_swDp, _hwDp);               \
    }

/* check and convert HW values to DP level for DXCH and above devices */
/* if HW value not supported returns GT_BAD_STATE                     */
#define PRV_CPSS_DXCH_COS_DP_TO_SW_CHECK_AND_CONVERT_MAC(              \
    _devNum, _hwDp, _swDp)                                             \
    if (0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(_devNum)) \
    {                                                                  \
        if ((_hwDp & (~ 1)) != 0) CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);                 \
        _swDp = (_hwDp == 0) ?  CPSS_DP_GREEN_E : CPSS_DP_RED_E;       \
    }                                                                  \
    else                                                               \
    {                                                                  \
        /* Lion and above */                                           \
        PRV_CPSS_DXCH3_COS_DP_TO_SW_CONVERT_MAC(_hwDp, _swDp);         \
    }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChCoSh */

