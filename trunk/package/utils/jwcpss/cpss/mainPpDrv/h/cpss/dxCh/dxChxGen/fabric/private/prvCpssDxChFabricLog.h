/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChFabricLog.h
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __prvCpssDxChFabricLogh
#define __prvCpssDxChFabricLogh
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>


/********* manually implemented declarations *********/

void prvCpssLogParamFunc_CPSS_DXCH_FABRIC_HGL_RX_ERROR_CNTRS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/********* enums *********/

PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_FABRIC_HGL_CELL_COUNTER_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_FABRIC_HGL_DESCRIPTOR_TYPE_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_FABRIC_HGL_DIRECTION_ENT);
PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_FABRIC_HGL_ECC_TYPE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_FABRIC_HGL_FC_RX_E2E_HEADER_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_FABRIC_HGL_FC_RX_LINK_LAYER_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_FABRIC_HGL_RX_DSA_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);
void prvCpssLogParamFuncStc_CPSS_DXCH_FABRIC_HGL_RX_ERROR_CNTRS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_FABRIC_HGL_CELL_COUNTER_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_FABRIC_HGL_CELL_COUNTER_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_FABRIC_HGL_DESCRIPTOR_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_FABRIC_HGL_DESCRIPTOR_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_FABRIC_HGL_DIRECTION_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_FABRIC_HGL_ECC_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_FABRIC_HGL_ECC_TYPE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_FABRIC_HGL_FC_RX_E2E_HEADER_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_FABRIC_HGL_FC_RX_LINK_LAYER_PARAMS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_FABRIC_HGL_RX_DSA_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/********* API fields DB *********/

extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_FABRIC_HGL_CELL_COUNTER_TYPE_ENT_cellType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_FABRIC_HGL_DESCRIPTOR_TYPE_ENT_type;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_FABRIC_HGL_DIRECTION_ENT_cellDirection;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_FABRIC_HGL_ECC_TYPE_ENT_eccType;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_FABRIC_HGL_FC_RX_E2E_HEADER_STC_PTR_e2eParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_FABRIC_HGL_FC_RX_LINK_LAYER_PARAMS_STC_PTR_llParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_FABRIC_HGL_RX_DSA_STC_PTR_dsaParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_idleCntEnable;
extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_maxIdleCnt;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_FABRIC_HGL_CELL_COUNTER_TYPE_ENT_PTR_cellTypePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_FABRIC_HGL_DESCRIPTOR_TYPE_ENT_PTR_typePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_FABRIC_HGL_ECC_TYPE_ENT_PTR_eccTypePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_FABRIC_HGL_FC_RX_E2E_HEADER_STC_PTR_e2eParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_FABRIC_HGL_FC_RX_LINK_LAYER_PARAMS_STC_PTR_llParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_FABRIC_HGL_RX_DSA_STC_PTR_dsaParamsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_FABRIC_HGL_RX_ERROR_CNTRS_STC_PTR_rxErrorsCntrsPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_idleCntEnablePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_laneSyncOkArr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_cellCntrPtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_maxIdleCntPtr;


/********* lib API DB *********/

enum {
    PRV_CPSS_LOG_FUNC_cpssDxChFabricHGLinkPcsLoopbackEnableSet_E = (CPSS_LOG_LIB_FABRIC_E << 16),
    PRV_CPSS_LOG_FUNC_cpssDxChFabricHGLinkPcsLoopbackEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChFabricHGLinkPcsMaxIdleCntEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChFabricHGLinkPcsMaxIdleCntEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChFabricHGLinkPcsMaxIdleCntSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChFabricHGLinkPcsMaxIdleCntGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChFabricHGLinkPcsRxStatusGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChFabricHGLinkRxErrorCntrGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChFabricHGLinkConfigEccTypeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChFabricHGLinkConfigEccTypeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChFabricHGLinkRxCrcCheckEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChFabricHGLinkRxCrcCheckEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChFabricHGLinkLbiEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChFabricHGLinkLbiEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChFabricHGLinkCellsCntrTypeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChFabricHGLinkCellsCntrTypeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChFabricHGLinkCellsCntrGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChFabricHGLinkFcMacSaSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChFabricHGLinkFcMacSaGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChFabricHGLinkRxDsaParamsSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChFabricHGLinkRxDsaParamsGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChFabricHGLinkFcRxE2eParamsSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChFabricHGLinkFcRxE2eParamsGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChFabricHGLinkRxLinkLayerFcParamsSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChFabricHGLinkRxLinkLayerFcParamsGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChFabricHGLinkDescriptorTypeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChFabricHGLinkDescriptorTypeGet_E
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __prvCpssDxChFabricLogh */
