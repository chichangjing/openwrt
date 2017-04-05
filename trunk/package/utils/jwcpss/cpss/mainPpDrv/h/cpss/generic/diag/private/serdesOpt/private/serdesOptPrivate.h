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
* serdesOptPrivate.h
*
* DESCRIPTION:
*   Serdes optimizer private API's definition.
*   Assumption is that serdes optimizer used in systems which HWS library
*   doesn't support.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
******************************************************************************/

#ifndef __serdesRxOptPrv_H
#define __serdesRxOptPrv_H

#ifdef __cplusplus
extern "C" {
#endif

#include <serdesOpt/SerdesRxOptimizer.h>

#define  DFE_MAX    (0x1f)
#define  DFE_MIN    (0x1)
#define  FFE_C_MAX  (0xf)
#define  FFE_C_MIN  (0x8)
#define  FFE_R_MAX  (0x7)
#define  FFE_R_MIN  (0x1)
#define  SAMPLER_START (0x5a)
#define  SQLCH_VAL  (0)

typedef struct
{
    GT_U32 dfeMinValue;          
    GT_U32 dfeMaxValue;          
}DFE_CFG;

typedef struct
{
    GT_U32 ffeRMinValue;          
    GT_U32 ffeRMaxValue;          
    GT_U32 ffeCMinValue;          
    GT_U32 ffeCMaxValue;          
}FFE_CFG;

/*
 * Typedef: FUNCP_SERDES_ACCESS
 *
 * Description: Pointer to function performing lane access
 *
 * Fields:
 *       devNum  - The device number.
 *       laneNum - The lane number.
 */
typedef void (*FUNCP_SERDES_ACCESS)
(
    GT_U8 devNum,
    GT_U32 portGroupNum,
    GT_U32 laneNum
);

/*
 * Typedef: FUNCP_SERDES_DATA_CFG
 *
 * Description: Pointer to function performing write data to lane
 *
 * Fields:
 *       devNum  - The device number.
 *       laneNum - The lane number.
 */
typedef void (*FUNCP_SERDES_DATA_CFG)
(
    GT_U8 devNum,
    GT_U32 portGroupNum,
    GT_U32 laneNum,
    GT_U32  data
);

/*
 * Typedef: FUNCP_SERDES_PRBS_CFG
 *
 * Description: Pointer to function performing write prbs to lane
 *
 * Fields:
 *       devNum  - The device number.
 *       laneNum - The lane number.
 *       prbs - type of prbs.
 */
typedef void (*FUNCP_SERDES_PRBS_CFG)
(
    GT_U8  devNum,
    GT_U32 portGroupNum,
    GT_U32 laneNum,
    MV_PRBS_TYPE  prbs
);

typedef GT_U32 (*FUNCP_SERDES_DATA_READ)
(
    GT_U8 devNum,
    GT_U32 portGroupNum,
    GT_U32 laneNum
);

typedef void (*FUNCP_SERDES_FFE_CFG)
(
    GT_U8 devNum,
    GT_U32 portGroupNum,
    GT_U32 laneNum,
    GT_U32  ffeR,
    GT_U32  ffeC
);

typedef GT_U32 (*FUNCP_SERDES_CONNECT)
(
    GT_U8 devNum,
    GT_U32 portGroupNum,
    GT_U32 laneNum,
    GT_U32  dfe,
    GT_U32  ffe,
    GT_U32  prbsCode
);

typedef void (*FUNCP_SAVE_RESTORE_CUR_CFG)
(
    GT_U8 devNum,
    GT_U32 portGroupNum,
    GT_U32 laneNum,
    int mode /* 0 - to save data, 1 - to restore prev. saved data */
);

typedef void (*FUNCP_GET_SIGNAL_DETECT)
(
	GT_U8 devNum,
	GT_U32 portGroupNum,
	GT_U32 laneNum,
	GT_U32 *signal 
);

typedef struct
{
    FUNCP_SERDES_PRBS_CFG   prbsConfigFunc;
    FUNCP_SERDES_DATA_READ  getErrorsFunc;
    FUNCP_SERDES_ACCESS     transmitterCfgFunc;
    FUNCP_SERDES_CONNECT    approvLaneConnectFunc;

    FUNCP_SERDES_DATA_CFG   setDfeValueFunc;
    FUNCP_SERDES_FFE_CFG    setFfeValueFunc;
    FUNCP_SERDES_DATA_CFG   setSqlchValueFunc;

    FUNCP_SERDES_DATA_CFG   samplerCfgFunc;
	FUNCP_GET_SIGNAL_DETECT checkSignalDetectFunc;

    FUNCP_SAVE_RESTORE_CUR_CFG saveRestoreCfgFunc;
	FUNCP_SERDES_ACCESS     prbsStopCfgFunc;

}MV_ACCESS_FUNCS;

typedef struct
{
    unsigned char devNum;
    GT_U32 laneNum;
	GT_U32 groupNum;

    MV_PRBS_TYPE  prbsCode;

    /* info needed by algorithm */
    GT_U32  tunningDone; 

	MV_SERDES_OPT_RESULTS result;

    /* functions to perform serdes read / write access */
    MV_ACCESS_FUNCS accIf;

}MV_LANE_DATA;


extern MV_SOPT_OS_EXACT_DELAY_FUNC   hwsSOptOsExactDelayPtr;
extern MV_SOPT_OS_TIME_WK_AFTER_FUNC hwsSOptOsTimerWkFuncPtr;
extern MV_SOPT_OS_MEM_SET_FUNC       hwsSOptOsMemSetFuncPtr;
extern MV_SOPT_OS_FREE_FUNC          hwsSOptOsFreeFuncPtr;
extern MV_SOPT_OS_MALLOC_FUNC        hwsSOptOsMallocFuncPtr;
extern MV_SOPT_OS_MEM_CPY_FUNC       hwsSOptOsMemCpyFuncPtr;
extern MV_SOPT_OS_IO_PRINTF_FUNC     hwsSOptOsPrintfFuncPtr;
extern MV_SOPT_OS_IO_SPRINTF_FUNC    hwsSOptOsSprintfFuncPtr;
extern MV_SOPT_OS_SIG_SEM_BIN_CREATE_FUNC  hwsSOptOsSemBCreateFuncPtr;
extern MV_SOPT_OS_SIG_SEM_DELETE_FUNC  hwsSOptOsSemDelFuncPtr;
extern MV_SOPT_OS_SIG_SEM_WAIT_FUNC  hwsSOptOsSemWaitFuncPtr;
extern MV_SOPT_OS_SIG_SEM_SIGNAL_FUNC  hwsSOptOsSemSignalFuncPtr;
extern MV_SOPT_OS_RAND_FUNC            hwsSOptOsRandFuncPtr;

/* input parameters config functions */
int dfeInfoConfig(GT_U32 dfeMin, GT_U32 dfeMax);
int ffeInfoConfig(GT_U32 ffeRMin, GT_U32 ffeRMax, GT_U32 ffeCMin, GT_U32 ffeCMax);
void addInfoConfig(GT_U32 sampStart, GT_U32 sqlchVal);
void mode1ParamsCfg(GT_U32 iterNum,
					GT_U32 crit,
					GT_32  rCrit,
					GT_32  cCrit,
					GT_U32 smoothDfe, 
					GT_U32 smoothSampler,
					GT_U32 centerDist,
					GT_U32 berRiskCrit);

#ifdef __cplusplus
}
#endif

#endif /* __serdesRxOptPrv_H */


