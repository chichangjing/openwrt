/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssTmPublicDefs.h
*
* DESCRIPTION:
*       TM Configuration Library Public Definitions.
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/

#ifndef __cpssTmPublicDefsh
#define __cpssTmPublicDefsh

#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/cpssTypes.h>

/************************* Constants Definitions ******************************/

/************************* Node Quantum Unit (in 256 bytes units) *************/
#define CPSS_TM_NODE_QUANTUM_UNIT_CNS  256

/************************* Port Quantum Unit (in 64 bytes units) **************/
#define CPSS_TM_PORT_QUANTUM_UNIT_CNS  64

/************************* Infinite Profile ***********************************/
#define CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS  0

/************************* No Drop ********************************************/
#define CPSS_TM_NO_DROP_PROFILE_CNS  0

/************************* 32 bit Invalid data indicator **********************/
#define CPSS_TM_INVAL_CNS   0xFFFFFFFF

/************************* Maximum Bandwidth (in Kbits/sec) ********************/
#define CPSS_TM_SHAPING_INFINITE_BW_CNS  0xFFFFFFFF

#define CPSS_TM_SHAPING_MAX_BURST_CNS (128*1024) /* 128 KB */

/*********************** WRED curve probability array size ********************/
#define CPSS_TM_DROP_PROB_ARR_SIZE_CNS 32

/************************* Enumerated Types ***********************************/

/******************************************************************************
* typedef: enum CPSS_TM_LEVEL_ENT
*
* Description: TM levels enumeration
*
* Enumerations:
*
*  	CPSS_TM_LEVEL_Q_E - Q-nodes level
*  	CPSS_TM_LEVEL_A_E - A-nodes level
*  	CPSS_TM_LEVEL_B_E - B-nodes level
*	CPSS_TM_LEVEL_C_E - C-nodes level
*  	CPSS_TM_LEVEL_P_E - Ports level
*
******************************************************************************/
typedef enum
{
    CPSS_TM_LEVEL_Q_E = 0, /* Q_LEVEL */
    CPSS_TM_LEVEL_A_E,
    CPSS_TM_LEVEL_B_E,
    CPSS_TM_LEVEL_C_E,
 	CPSS_TM_LEVEL_P_E
} CPSS_TM_LEVEL_ENT;

/******************************************************************************
* typedef: enum CPSS_TM_PORT_BW_ENT
*
* Description: Port's physical bandwidth
*
* Enumerations:
*
*  	CPSS_TM_PORT_BW_1G_E - 1G bit/sec
*  	CPSS_TM_PORT_BW_2HG_E - 2.5G bit/sec
*  	CPSS_TM_PORT_BW_10G_E - 10G bit/sec
*	CPSS_TM_PORT_BW_40G_E - 40G bit/sec
*  	CPSS_TM_PORT_BW_50G_E - 50G bit/sec
*   CPSS_TM_PORT_BW_100G_E - 100G bit/sec
*
******************************************************************************/
typedef enum
{
    CPSS_TM_PORT_BW_1G_E = 0, /* TM_1G_PORT */
    CPSS_TM_PORT_BW_2HG_E,
    CPSS_TM_PORT_BW_10G_E,
    CPSS_TM_PORT_BW_40G_E,
    CPSS_TM_PORT_BW_50G_E,
    CPSS_TM_PORT_BW_100G_E
} CPSS_TM_PORT_BW_ENT;


/******************************************************************************
* typedef: enum CPSS_TM_ELIG_FUNC_NODE_ENT
*
* Description: Node eligable function Ids
*
* Enumerations:
*
*      CPSS_TM_ELIG_N_PRIO1_E   - Eligible function priority 1
*      CPSS_TM_ELIG_N_PRIO5_E   - Eligible function priority 5
*      CPSS_TM_ELIG_N_SHP_E     - Eligible function shaping
*      CPSS_TM_ELIG_N_MIN_SHP_E - Eligible function min shaping
*      CPSS_TM_ELIG_N_PPA_E     - Eligible function propagated priority
*      CPSS_TM_ELIG_N_PPA_SP_E  - Eligible function propagated priority with
*                                 strict priority (priority = 5)
*      CPSS_TM_ELIG_N_PPA_SHP_E - Eligible function propagated with shaping
*      CPSS_TM_ELIG_N_PPA_SP_MIN_SHP_E - Eligible function propagated priority with
*                                        strict priority (priority = 5) with min shaper
*      CPSS_TM_ELIG_N_PPA_SHP_IGN_E    - Eligible function propagated priority and shaping with
*                                        shaper ignore for propagated priority = 3
*      CPSS_TM_ELIG_N_PPA_MIN_SHP_SP_IGN_E - Eligible function propagated priority with strict priority
*                                            (priority = 5) with min shaper with shaper ignore for
*                                            propagated priority = 3
*      CPSS_TM_ELIG_N_FP0_E - Node is always eligible in priority 0
*      CPSS_TM_ELIG_N_FP1_E - Node is always eligible in priority 1
*      CPSS_TM_ELIG_N_FP2_E - Node is always eligible in priority 2
*      CPSS_TM_ELIG_N_FP3_E - Node is always eligible in priority 3
*      CPSS_TM_ELIG_N_FP4_E - Node is always eligible in priority 4
*      CPSS_TM_ELIG_N_FP5_E - Node is always eligible in priority 5
*      CPSS_TM_ELIG_N_FP6_E - Node is always eligible in priority 6
*      CPSS_TM_ELIG_N_FP7_E - Node is always eligible in priority 7
*      CPSS_TM_ELIG_N_MIN_SHP_FP0_E - Node is eligible in priority 0 when minTB is positive
*      CPSS_TM_ELIG_N_MIN_SHP_FP1_E - Node is eligible in priority 1 when minTB is positive
*      CPSS_TM_ELIG_N_MIN_SHP_FP2_E - Node is eligible in priority 2 when minTB is positive
*      CPSS_TM_ELIG_N_MIN_SHP_FP3_E - Node is eligible in priority 3 when minTB is positive
*      CPSS_TM_ELIG_N_MIN_SHP_FP4_E - Node is eligible in priority 4 when minTB is positive
*      CPSS_TM_ELIG_N_MIN_SHP_FP5_E - Node is eligible in priority 5 when minTB is positive
*      CPSS_TM_ELIG_N_MIN_SHP_FP6_E - Node is eligible in priority 6 when minTB is positive
*      CPSS_TM_ELIG_N_MIN_SHP_FP7_E - Node is eligible in priority 7 when minTB is positive
*      CPSS_TM_ELIG_N_MAX_INC_MIN_SHP_E - Eligible function propagated with MaxTB include MinTB shaping
*      CPSS_TM_ELIG_N_PP_E              - Node is always eligible in propagated priority
*      CPSS_TM_ELIG_N_MIN_SHP_PP_E      - Node is eligible when MinTB is positive
*      CPSS_TM_ELIG_N_PP_SHP_E          - Eligible function with propagated priority and shaping
*      CPSS_TM_ELIG_N_SHP_4P_MIN_4P_MAX_E - Eligible function with min max shaping
*      CPSS_TM_ELIG_N_SHP_PP_TB_E       - Eligible function with min max shaping, the pp influences
*                                         which shaper will be used
*      CPSS_TM_ELIG_N_SHP_PP_MAX_TB_0   - Eligible function with min max shaping,SchedPrio0 = Prop0(Min)+Prop0..7(Max)
*                                         SchedPrio1..7 = SchedPrio1..7(Min), PropOut = Prop
*      CPSS_TM_ELIG_N_DEQ_DIS_E - Eligible function for dequeue disable (the node is not eligible)
*                                 PPA - Propagated priority according to RevA .
*                                 i.e. - only propagated priority 0...3 are supported.
*                                 When node gets propagated priority 4...7 it is not eligible
*
******************************************************************************/
typedef enum
{
    CPSS_TM_ELIG_N_PRIO1_E = 0,
    CPSS_TM_ELIG_N_PRIO5_E = 1,
    CPSS_TM_ELIG_N_SHP_E   = 2,
    CPSS_TM_ELIG_N_MIN_SHP_E = 3,
    CPSS_TM_ELIG_N_PPA_E     = 4,
    CPSS_TM_ELIG_N_PPA_SP_E  = 5,
    CPSS_TM_ELIG_N_PPA_SHP_E = 6,
    CPSS_TM_ELIG_N_PPA_SP_MIN_SHP_E = 7,
    CPSS_TM_ELIG_N_PPA_SHP_IGN_E    = 8,
    CPSS_TM_ELIG_N_PPA_MIN_SHP_SP_IGN_E = 9,
    CPSS_TM_ELIG_N_FP0_E = 10,
    CPSS_TM_ELIG_N_FP1_E = 11,
    CPSS_TM_ELIG_N_FP2_E = 12,
    CPSS_TM_ELIG_N_FP3_E = 13,
    CPSS_TM_ELIG_N_FP4_E = 14,
    CPSS_TM_ELIG_N_FP5_E = 15,
    CPSS_TM_ELIG_N_FP6_E = 16,
    CPSS_TM_ELIG_N_FP7_E = 17,
    CPSS_TM_ELIG_N_MIN_SHP_FP0_E = 18,
    CPSS_TM_ELIG_N_MIN_SHP_FP1_E = 19,
    CPSS_TM_ELIG_N_MIN_SHP_FP2_E = 20,
    CPSS_TM_ELIG_N_MIN_SHP_FP3_E = 21,
    CPSS_TM_ELIG_N_MIN_SHP_FP4_E = 22,
    CPSS_TM_ELIG_N_MIN_SHP_FP5_E = 23,
    CPSS_TM_ELIG_N_MIN_SHP_FP6_E = 24,
    CPSS_TM_ELIG_N_MIN_SHP_FP7_E = 25,
    CPSS_TM_ELIG_N_MAX_INC_MIN_SHP_E = 26,
    CPSS_TM_ELIG_N_PP_E              = 27,
    CPSS_TM_ELIG_N_MIN_SHP_PP_E      = 28,
    CPSS_TM_ELIG_N_PP_SHP_E          = 29,
    CPSS_TM_ELIG_N_SHP_4P_MIN_4P_MAX_E = 30,
    CPSS_TM_ELIG_N_SHP_PP_TB_E       = 31,
    CPSS_TM_ELIG_N_SHP_PP_MAX_TB_0   = 32,
    CPSS_TM_ELIG_N_DEQ_DIS_E         = 63
} CPSS_TM_ELIG_FUNC_NODE_ENT;


/******************************************************************************
* typedef: enum CPSS_TM_ELIG_FUNC_QUEUE_ENT
*
* Description: Node eligable function Ids
*
* Enumerations:
*
*      CPSS_TM_ELIG_Q_PRIO0_E - Eligible function priority 0
*      CPSS_TM_ELIG_Q_PRIO1_E - Eligible function priority 1
*      CPSS_TM_ELIG_Q_PRIO2_E - Eligible function priority 2
*      CPSS_TM_ELIG_Q_PRIO3_E - Eligible function priority 3
*      CPSS_TM_ELIG_Q_MIN_SHP_PRIO0_E - Eligible function min shaping priority 0
*      CPSS_TM_ELIG_Q_MIN_SHP_PRIO1_E - Eligible function min shaping priority 1
*      CPSS_TM_ELIG_Q_MIN_SHP_PRIO2_E - Eligible function min shaping priority 2
*      CPSS_TM_ELIG_Q_MIN_SHP_PRIO3_E - Eligible function min shaping priority 3
*      CPSS_TM_ELIG_Q_PRIO4_E - Eligible function priority 4
*      CPSS_TM_ELIG_Q_PRIO5_E - Eligible function priority 5
*      CPSS_TM_ELIG_Q_PRIO6_E - Eligible function priority 6
*      CPSS_TM_ELIG_Q_PRIO7_E - Eligible function priority 7
*      CPSS_TM_ELIG_Q_MIN_SHP_PRIO4_E - Eligible function min shaping priority 4
*      CPSS_TM_ELIG_Q_MIN_SHP_PRIO5_E - Eligible function min shaping priority 5
*      CPSS_TM_ELIG_Q_MIN_SHP_PRIO6_E - Eligible function min shaping priority 6
*      CPSS_TM_ELIG_Q_MIN_SHP_PRIO7_E - Eligible function min shaping priority 7
*      CPSS_TM_ELIG_Q_SHP_E - Eligible function with shaping
*      CPSS_TM_ELIG_Q_MAX_INC_MIN_SHP_E - Eligible function propagated with MaxTB include MinTB shaping
*      CPSS_TM_ELIG_Q_SHP_SCHED00_PROP00 - Eligible function min max shaping, scheduling priority 0/0, propagated priority 0/0
*      CPSS_TM_ELIG_Q_SHP_SCHED10_PROP10 - Eligible function min max shaping, scheduling priority 1/0, propagated priority 1/0
*      CPSS_TM_ELIG_Q_SHP_SCHED20_PROP20 - Eligible function min max shaping, scheduling priority 2/0, propagated priority 2/0
*      CPSS_TM_ELIG_Q_SHP_SCHED30_PROP30 - Eligible function min max shaping, scheduling priority 3/0, propagated priority 3/0
*      CPSS_TM_ELIG_Q_SHP_SCHED40_PROP40 - Eligible function min max shaping, scheduling priority 4/0, propagated priority 4/0
*      CPSS_TM_ELIG_Q_SHP_SCHED50_PROP50 - Eligible function min max shaping, scheduling priority 5/0, propagated priority 5/0
*      CPSS_TM_ELIG_Q_SHP_SCHED60_PROP60 - Eligible function min max shaping, scheduling priority 6/0, propagated priority 6/0
*      CPSS_TM_ELIG_Q_SHP_SCHED70_PROP70 - Eligible function min max shaping, scheduling priority 7/0, propagated priority 7/0
*      CPSS_TM_ELIG_Q_DEQ_DIS_E - Eligible function for dequeue disable (the queue is not eligible)
*
******************************************************************************/
typedef enum
{
    CPSS_TM_ELIG_Q_PRIO0_E = 0,
    CPSS_TM_ELIG_Q_PRIO1_E = 1,
    CPSS_TM_ELIG_Q_PRIO2_E = 2,
    CPSS_TM_ELIG_Q_PRIO3_E = 3,
    CPSS_TM_ELIG_Q_MIN_SHP_PRIO0_E = 4,
    CPSS_TM_ELIG_Q_MIN_SHP_PRIO1_E = 5,
    CPSS_TM_ELIG_Q_MIN_SHP_PRIO2_E = 6,
    CPSS_TM_ELIG_Q_MIN_SHP_PRIO3_E = 7,
    CPSS_TM_ELIG_Q_PRIO4_E = 8,
    CPSS_TM_ELIG_Q_PRIO5_E = 9,
    CPSS_TM_ELIG_Q_PRIO6_E = 10,
    CPSS_TM_ELIG_Q_PRIO7_E = 11,
    CPSS_TM_ELIG_Q_MIN_SHP_PRIO4_E = 12,
    CPSS_TM_ELIG_Q_MIN_SHP_PRIO5_E = 13,
    CPSS_TM_ELIG_Q_MIN_SHP_PRIO6_E = 14,
    CPSS_TM_ELIG_Q_MIN_SHP_PRIO7_E = 15,
    CPSS_TM_ELIG_Q_SHP_E = 16,
    CPSS_TM_ELIG_Q_MAX_INC_MIN_SHP_E = 17,
    CPSS_TM_ELIG_Q_SHP_SCHED00_PROP00 = 18,
    CPSS_TM_ELIG_Q_SHP_SCHED10_PROP10 = 19,
    CPSS_TM_ELIG_Q_SHP_SCHED20_PROP20 = 20,
    CPSS_TM_ELIG_Q_SHP_SCHED30_PROP30 = 21,
    CPSS_TM_ELIG_Q_SHP_SCHED40_PROP40 = 22,
    CPSS_TM_ELIG_Q_SHP_SCHED50_PROP50 = 23,
    CPSS_TM_ELIG_Q_SHP_SCHED60_PROP60 = 24,
    CPSS_TM_ELIG_Q_SHP_SCHED70_PROP70 = 25,
    CPSS_TM_ELIG_Q_DEQ_DIS_E = 63
} CPSS_TM_ELIG_FUNC_QUEUE_ENT;

/******************************************************************************
* typedef: enum CPSS_TM_TOKEN_BUCKET_ENT
*
* Description: Token bucket usage
*
* Enumerations:
*
*  	CPSS_TM_TOKEN_BUCKET_MIN_E - Use Min token bucket
*  	CPSS_TM_TOKEN_BUCKET_MAX_E - Use Max token bucket
*
******************************************************************************/
typedef enum
{
    CPSS_TM_TOKEN_BUCKET_MIN_E = 0,
    CPSS_TM_TOKEN_BUCKET_MAX_E
} CPSS_TM_TOKEN_BUCKET_ENT;

/******************************************************************************
* typedef: enum CPSS_TM_DROP_MODE_ENT
*
* Description: Drop WRED/CATD mode
*
* Enumerations:
*
*      CPSS_TM_DROP_MODE_CB_CA_WRED_E - CBTD + Color Aware WRED mode
*      CPSS_TM_DROP_MODE_CB_CA_TD_E   - CBTD + Color Aware TD mode
*      CPSS_TM_DROP_MODE_CB_TD_E      - Color Blind TailDrop Only mode
******************************************************************************/
typedef enum
{
    CPSS_TM_DROP_MODE_CB_TD_CA_WRED_E = 0,
    CPSS_TM_DROP_MODE_CB_TD_CA_TD_E,
    CPSS_TM_DROP_MODE_CB_TD_ONLY_E
} CPSS_TM_DROP_MODE_ENT;

/******************************************************************************
* typedef: enum CPSS_TM2TM_CHANNEL_ENT
*
* Description: TM2TM channel
*
* Enumerations:
*
*  	CPSS_TM2TM_CHANNEL_NODE_E - TM2TM Node channel
*  	CPSS_TM2TM_CHANNEL_PORT_E - TM2TM Port channel
******************************************************************************/
typedef enum
{
    CPSS_TM2TM_CHANNEL_NODE_E = 0, /* TM2TM_NODE_CH */
    CPSS_TM2TM_CHANNEL_PORT_E
} CPSS_TM2TM_CHANNEL_ENT;


/******************************************************************************
* typedef: enum CPSS_TM_COLOR_NUM_ENT
*
* Description: Number of colors
*
* Enumerations:
*
*  	CPSS_TM_COLOR_NUM_1_E - One color per level
*  	CPSS_TM_COLOR_NUM_2_E - Two colors per level
*   CPSS_TM_COLOR_NUM_3_E - Three colors per level
******************************************************************************/
typedef enum
{
    CPSS_TM_COLOR_NUM_1_E = 0,
    CPSS_TM_COLOR_NUM_2_E,
    CPSS_TM_COLOR_NUM_3_E
} CPSS_TM_COLOR_NUM_ENT;

/******************************************************************************
* typedef: enum CPSS_TM_DP_SOURCE_ENT
*
* Description: DP source.
*
* Enumerations:
*
*  	CPSS_TM_DP_SOURCE_AQL_E - Average Queue Length is used to calculate DP source.
*  	CPSS_TM_DP_SOURCE_QL_E -  Queue Length is used to calculate DP source.
******************************************************************************/
typedef enum
{
    CPSS_TM_DP_SOURCE_AQL_E = 0,
    CPSS_TM_DP_SOURCE_QL_E
} CPSS_TM_DP_SOURCE_ENT;


/******************************************************************************
* typedef: enum CPSS_TM_SCHD_MODE_ENT
*
* Description: Scheduler mode.
*
* Enumerations:
*
*  	CPSS_TM_SCHD_MODE_RR_E   - RR mode.
*  	CPSS_TM_SCHD_MODE_DWRR_E - DWRR mode.
******************************************************************************/
typedef enum
{
    CPSS_TM_SCHD_MODE_RR_E,
    CPSS_TM_SCHD_MODE_DWRR_E
} CPSS_TM_SCHD_MODE_ENT;


/************************* Global Paramteres Data structures ******************/

/******************************************************************************
* Typedef: struct CPSS_TM_LIB_INIT_PARAMS_STC
*
* Description: Customize TM lib initialization parameters.
*
* Fields:
*      tmMtu -  if 0 then MTU is TM_MTU
*               (APPLICABLE RANGES: 0..16K)
*****************************************************************************/
typedef struct
{
    GT_U32      tmMtu;
} CPSS_TM_LIB_INIT_PARAMS_STC;

/******************************************************************************
* Typedef: struct CPSS_TM_LEVEL_PERIODIC_PARAMS_STC
*
* Description: Periodic Update Rate configuration params per level.
*
* Fields:
*      periodicState       - Periodic shaper update Enable/Disable
*      shaperDecoupling    - Shaper Decoupling Enable/Disable
*****************************************************************************/
typedef struct
{
    GT_BOOL     periodicState;
    GT_BOOL     shaperDecoupling;
} CPSS_TM_LEVEL_PERIODIC_PARAMS_STC;


/* Maximum DROP threshold in bytes */
#define CPSS_TM_MAX_DROP_THRESHOLD_CNS	0xFFFFFFFF

/******************************************************************************
* Typedef: struct CPSS_TM_DROP_PROFILE_CA_TD_PARAMS_STC
*
* Description: Color Aware Drop configuration profile.
*
* Fields:
*      caTdThreshold - Color Aware TD in Bytes Per Color
*                     (APPLICABLE RANGES: 0..CPSS_TM_MAX_DROP_THRESHOLD_CNS)
*****************************************************************************/
typedef struct
{
    GT_U32  caTdThreshold[3];
} CPSS_TM_DROP_PROFILE_CA_TD_PARAMS_STC;


/******************************************************************************
* Typedef: struct CPSS_TM_DROP_PROFILE_WRED_PARAMS_STC
*
* Description: Drop configuration profile.
*
* Fields:
*      aqlExponent             - Exponent value to determine new average queue length
*                                as function of previous average and the current size
*                                of the queue
*                                (APPLICABLE RANGES: 0..15)
*
*      curveIndex[3]           - RED curve index per color
*                                (APPLICABLE RANGES: 0..7)
*
*      dpCurveScale[3]         - Drop Probabiliry Curve scaling:
*                                0 - no scaling, DP[i] = DPCurve[i].
*                                2 colors - scale down from DPCurve[i] * 1/64 to DPCurve[i] * 63/64
*                                3 colors - scale down from DPCurve[i] * 1/16 to DPCurve[i] * 15/16
*                                (APPLICABLE RANGES: 2 colors: 0..64, 3 colors: 0..15)
*
*      caWredTdMinThreshold[3] - Color Aware Wred Min TD in Bytes Per Color
*                               (APPLICABLE RANGES: 0..caWredTdMaxThreshold[i])
*
*      caWredTdMaxThreshold[3] - Color Aware Max Wred TD in Bytes Per Color
*                               (APPLICABLE RANGES: caWredTdMinThreshold[i]..CPSS_TM_MAX_DROP_THRESHOLD_CNS)
*
*****************************************************************************/
typedef struct
{
    GT_U8   aqlExponent;
    GT_U8   curveIndex[3];
    GT_U8   dpCurveScale[3];
    GT_U32  caWredTdMinThreshold[3];
    GT_U32  caWredTdMaxThreshold[3];
} CPSS_TM_DROP_PROFILE_CA_WRED_PARAMS_STC;


/******************************************************************************
* Typedef: struct CPSS_TM_DROP_PROFILE_PARAMS_STC
*
* Description: Structure which groups the 3 drop modes.
*
* Fields:
*      dropMode           - Drop Mode
*      cbTdThresholdBytes - Color Blind TD in Bytes
*                           (APPLICABLE RANGES: 0..CPSS_TM_MAX_DROP_THRESHOLD_CNS)
*      caTdDp             - Color Aware Tail Drop configuration profile
*      caWredDp           - Color Aware WRED Drop configuration profile
*
******************************************************************************/
typedef struct  {
    CPSS_TM_DROP_MODE_ENT                    dropMode;
	/* common data for all drop modes */
    GT_U32									 cbTdThresholdBytes;
	/* extra data for CA mode */
    CPSS_TM_DROP_PROFILE_CA_TD_PARAMS_STC    caTdDp;
	/*  or extra data for WRED  mode */
    CPSS_TM_DROP_PROFILE_CA_WRED_PARAMS_STC  caWredDp;
} CPSS_TM_DROP_PROFILE_PARAMS_STC;


/******************************************************************************
* Typedef: struct CPSS_TM_SHAPING_PROFILE_PARAMS_STC
*
* Description: Shared Shaper Profile configuration structure.
*
* Fields:
*      cirBw           - CIR BW in KBit/Sec
*                        (APPLICABLE RANGES: 0..maxBw)
*      cbs             - CIR Burst Size in Kbytes
*                        (APPLICABLE RANGES: 0..4K)
*      eirBw           - EIR BW in KBit/Sec
*                        (APPLICABLE RANGES: 0..maxBw)
*      ebs             - EIR Burst Size in Kbytes
*                        (APPLICABLE RANGES: 0..4K)
*
******************************************************************************/
typedef struct
{
	GT_U32  cirBw;
	GT_U32  cbs;
	GT_U32  eirBw;
	GT_U32  ebs;
} CPSS_TM_SHAPING_PROFILE_PARAMS_STC;


/************************* Nodes Parameters Data Structures *******************/

/******************************************************************************
* Typedef: struct CPSS_TM_QUEUE_PARAMS_STC
*
* Description: Queue Parameters Data Structure.
*
* Fields:
*
*      shapingProfilePtr         - Index of Shaping profile
*                                  (APPLICABLE RANGES: 1..total tree Q,A,B,C levels nodes (84479 for BC2),
*                                                      CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS)
*                                  number of tree levels nodes can be retrived with cpssTmTreeParamsGet.
*      quantum                   - DWRR Quantum in resolution of CPSS_TM_NODE_QUANTUM_UNIT_CNS (256)
*                                  (APPLICABLE RANGES: 0x40..0x3FFF).
*                                  Quantum Limits depend on system MTU and can be retrieved with
*                                  cpssTmNodeQuantumLimitsGet API. 
*      dropProfileInd            - Index of Queue Drop profile
*                                  (APPLICABLE RANGES: 1..2047,
*                                                      CPSS_TM_NO_DROP_PROFILE_CNS)
*      eligiblePrioFuncId        - Eligible Priority Function pointer
*                                 (APPLICABLE RANGES: (CPSS_TM_ELIG_FUNC_QUEUE_ENT) 0..17, 18..25, 63)
*
******************************************************************************/
typedef struct
{
    GT_U32  shapingProfilePtr;
    GT_U32  quantum;
    GT_U32  dropProfileInd;
    CPSS_TM_ELIG_FUNC_QUEUE_ENT  eligiblePrioFuncId;
} CPSS_TM_QUEUE_PARAMS_STC;

/******************************************************************************
* Typedef: struct CPSS_TM_A_NODE_PARAMS_STC
*
* Description: A-Node Parameters Data Structure.
*
* Fields:
*
*      shapingProfilePtr         - Index of Shaping profile
*                                  (APPLICABLE RANGES: 1..total tree Q,A,B,C levels nodes (84479 for BC2),
*                                                      CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS)
* 
*                                  number of tree levels nodes can be retrived with cpssTmTreeParamsGet.
*      quantum                   - DWRR Quantum in resolution of CPSS_TM_NODE_QUANTUM_UNIT_CNS (256) bytes
*                                  (APPLICABLE RANGES: 0x40..0x3FFF).
*                                  Quantum Limits depend on system MTU and can be retrieved with
*                                  cpssTmNodeQuantumLimitsGet API.
*      dropProfileInd            - Index of A-Level Drop profile
*                                  (APPLICABLE RANGES: 1..255,
*                                                      CPSS_TM_NO_DROP_PROFILE_CNS) 
*      eligiblePrioFuncId        - Eligible Priority Function pointer
*                                  (APPLICABLE RANGES: (CPSS_TM_ELIG_FUNC_NODE_ENT) 0..32, 63).
*      schdModeArr[8]            - RR/DWRR Priority for A-Node Scheduling
*                                  (APPLICABLE RANGES: (CPSS_TM_SCHD_MODE_ENT) 0..1).
*      numOfChildren             - Number of children nodes
*                                  (APPLICABLE RANGES: 1..TM_MAX_QUEUES).
*
******************************************************************************/
typedef struct
{
    GT_U32  shapingProfilePtr;
    GT_U32  quantum;
    GT_U32  dropProfileInd;
    CPSS_TM_ELIG_FUNC_NODE_ENT eligiblePrioFuncId;
    CPSS_TM_SCHD_MODE_ENT  schdModeArr[8];
    GT_U32  numOfChildren;
} CPSS_TM_A_NODE_PARAMS_STC;

/******************************************************************************
* Typedef: struct CPSS_TM_B_NODE_PARAMS_STC
*
* Description: B-Node Parameters Data Structure.
*
* Fields:
*
*      shapingProfilePtr         - Index of Shaping profile
*                                  (APPLICABLE RANGES: 1..total tree Q,A,B,C levels nodes (84479 for BC2),
*                                                      CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS)
*                                  number of tree levels nodes can be retrived with cpssTmTreeParamsGet.
*      quantum                   - DWRR Quantum in resolution of CPSS_TM_NODE_QUANTUM_UNIT_CNS (256) bytes
*                                  (APPLICABLE RANGES: 0x40..0x3FFF).
*                                  Quantum Limits depend on system MTU and can be retrieved with
*                                  cpssTmNodeQuantumLimitsGet API.
*      dropProfileInd            - Index of B-Level Drop profile
*                                  (APPLICABLE RANGES: 1..63,
*                                                      CPSS_TM_NO_DROP_PROFILE_CNS).
*      eligiblePrioFuncId        - Eligible Priority Function pointer
*                                  (APPLICABLE RANGES: (CPSS_TM_ELIG_FUNC_NODE_ENT) 0..32, 63).
*      schdModeArr[8]            - RR/DWRR Priority for B-Node Scheduling
*                                  (APPLICABLE RANGES: (CPSS_TM_SCHD_MODE_ENT) 0..1). 
*      numOfChildren             - Number of children nodes
*                                  (APPLICABLE RANGES: 1..TM_MAX_A_NODES).
*
******************************************************************************/
typedef struct
{
    GT_U32  shapingProfilePtr;
    GT_U32  quantum;
    GT_U32  dropProfileInd;
    CPSS_TM_ELIG_FUNC_NODE_ENT eligiblePrioFuncId;
    CPSS_TM_SCHD_MODE_ENT schdModeArr[8];
    GT_U32  numOfChildren;
} CPSS_TM_B_NODE_PARAMS_STC;

/******************************************************************************
* Typedef: struct CPSS_TM_C_NODE_PARAMS_STC
*
* Description: C-Node Parameters Data Structure.
*
* Fields:
*
*      shapingProfilePtr         - Index of Shaping profile
*                                  (APPLICABLE RANGES: 1..total tree Q,A,B,C levels nodes (84479 for BC2),
*                                                      CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS)
*                                  number of tree levels nodes can be retrived with cpssTmTreeParamsGet.
*      quantum                   - DWRR Quantum in resolution of CPSS_TM_NODE_QUANTUM_UNIT_CNS (256) bytes
*                                  (APPLICABLE RANGES: 0x40..0x3FFF).
*                                  Quantum Limits depend on system MTU and can be retrieved with
*                                  cpssTmNodeQuantumLimitsGet API.
*      dropCosMap                - C-node CoS bit map for Drop profiles
*                                  (APPLICABLE RANGES: 1..255).
*      dropProfileIndArr[8]      - Index of C-Level Drop profile per CoS
*                                  (APPLICABLE RANGES: 0..63,
*                                                      CPSS_TM_NO_DROP_PROFILE_CNS).
*      eligiblePrioFuncId        - Eligible Priority Function pointer
*                                  (APPLICABLE RANGES: (CPSS_TM_ELIG_FUNC_NODE_ENT) 0..32, 63).
*      schdModeArr[8]            - RR/DWRR Priority for C-Node Scheduling
*                                  (APPLICABLE RANGES: (CPSS_TM_SCHD_MODE_ENT) 0..1).
*      numOfChildren             - Number of children nodes
*                                  (APPLICABLE RANGES: 1..TM_MAX_B_NODES).
*
******************************************************************************/
typedef struct
{
    GT_U32  shapingProfilePtr;
    GT_U32  quantum;
    GT_U32  dropCosMap;
    GT_U32  dropProfileIndArr[8];
    CPSS_TM_ELIG_FUNC_NODE_ENT  eligiblePrioFuncId;
    CPSS_TM_SCHD_MODE_ENT schdModeArr[8];
    GT_U32  numOfChildren;
} CPSS_TM_C_NODE_PARAMS_STC;

/******************************************************************************
* Typedef: struct CPSS_TM_PORT_PARAMS_STC
*
* Description: Port Parameters Data Structure.
*
* Fields:
*
*      cirBw                     - CIR BW in KBit/Sec
*                                  (APPLICABLE RANGES: 0..100G,
*                                                      CPSS_TM_SHAPING_INFINITE_BW_CNS)
*      eirBw                     - EIR BW in KBit/Sec
*                                  (APPLICABLE RANGES: 0..100G,
*                                                      CPSS_TM_SHAPING_INFINITE_BW_CNS) 
*      cbs                       - CBS in Kbytes
*      ebs                       - EBS in Kbytes
* 
*      quantumArr[8]             - DWRR Quantum Per Cos in resolution of CPSS_TM_PORT_QUANTUM_UNIT_CNS (64) bytes
*                                  (APPLICABLE RANGES: 0x10..0x1FF).
*                                  Quantum Limits depend on system MTU and can be retrieved with
*                                  cpssTmPortQuantumLimitsGet API.
*
*      schdModeArr[8]            - RR/DWRR Priority for Port Scheduling
*                                  (APPLICABLE RANGES: 0..1)
*      dropProfileInd            - Index of Port-Level Drop profile
*                                  (APPLICABLE RANGES: 0..191,
*                                                      CPSS_TM_NO_DROP_PROFILE_CNS).
*      eligiblePrioFuncId        - Eligible Priority Function pointer
*                                  (APPLICABLE RANGES: (CPSS_TM_ELIG_FUNC_NODE_ENT) 0..32, 63).
*      numOfChildren             - Number of children nodes
*                                  (APPLICABLE RANGES: 1..TM_MAX_C_NODES)
*
******************************************************************************/
typedef struct
{
    GT_U32  cirBw;
	GT_U32  eirBw;
    GT_U32  cbs;
	GT_U32  ebs;
    GT_U32  quantumArr[8];
    CPSS_TM_SCHD_MODE_ENT  schdModeArr[8];
    GT_U32  dropProfileInd;
    CPSS_TM_ELIG_FUNC_NODE_ENT  eligiblePrioFuncId;
    GT_U32  numOfChildren;
} CPSS_TM_PORT_PARAMS_STC;


/******************************************************************************
* Typedef: struct CPSS_TM_PORT_DROP_PER_COS_STC
*
* Description: Port Drop per Cos Parameters Data Structure.
*
* Fields:
*
*      dropCosMap                  - port CoS bit map for Drop profiles
*                                  (APPLICABLE RANGES: 1..255)
*      dropProfileIndArr[8]        - Index of Drop profile per CoS
*                                  (APPLICABLE RANGES: 1..255,
*                                                      CPSS_TM_NO_DROP_PROFILE_CNS)
******************************************************************************/
typedef struct
{
    GT_U32  dropCosMap;
    GT_U32  dropProfileIndArr[8];
} CPSS_TM_PORT_DROP_PER_COS_STC;


/******************************************************************************
* Typedef: struct CPSS_TM_PORT_STATUS_STC
*
* Description: Port status data structure.
*
* Fields:
*
*      maxBucketLevel      - Maximal Shaper Bucket level
*      minBucketLevel      - Minimal Shaper Bucket level
*      deficitArr[8]       - DWRR Deficit per instance
*
******************************************************************************/
typedef struct
{
    GT_U32  maxBucketLevel;
    GT_U32  minBucketLevel;
    GT_U32  deficitArr[8];
} CPSS_TM_PORT_STATUS_STC;


/******************************************************************************
* Typedef: struct CPSS_TM_NODE_STATUS_STC
*
* Description: Node status data structure.
*
* Fields:
*
*      maxBucketLevel                      - Maximal Shaper Bucket level
*      minBucketLevel                      - Minimal Shaper Bucket level
*      deficit                             - DWRR Deficit
*
******************************************************************************/
typedef struct
{
    GT_U32  maxBucketLevel;
    GT_U32  minBucketLevel;
    GT_U32  deficit;
} CPSS_TM_NODE_STATUS_STC;


/******************************************************************************
* Typedef: struct CPSS_TM_QMR_PKT_STATISTICS_STC
*
* Description: QMR Packet Statistics data structure.
*
* Fields:
*
*      numPktsFromSms              - Non-error pkts from SMS
*      numErrsFromSmsDropped       - Pkts with error from SMS that hasn't been written to DRAM
*      numErrsFromSmsPassed        - Packets with error from SMS that has been written to DRAM
*      numPktsToUnInstallQueue     - Packets from SMS that have arrived to un-installed Queue
*
******************************************************************************/
typedef struct
{
#if 0 /* TBD */
    GT_U64  numPktsFromSms;
    GT_U64  numErrsFromSmsDropped;
    GT_U64  numErrsFromSmsPassed;
#endif
    GT_U64  numPktsToUnInstallQueue;
} CPSS_TM_QMR_PKT_STATISTICS_STC;


/******************************************************************************
* Typedef: struct CPSS_TM_RCB_PKT_STATISTICS_STC
*
* Description: RCB Packet Statistics data structure.
*
* Fields:
*
*      numPktsToSms                - Non-error pkts that are passed to SMS
*      numCrcErrPktsToSms          - Packets with CRC error
*      numErrsFromSmsToDram        - Packtes with error from SMS that has been written to DRAM
*      numPortFlushDrpDataPkts     - Dropped Port Flush Packets counter
*
******************************************************************************/
typedef struct
{
    GT_U64  numPktsToSms;
    GT_U64  numCrcErrPktsToSms;
    GT_U64  numErrsFromSmsToDram;
    GT_U64  numPortFlushDrpDataPkts;
} CPSS_TM_RCB_PKT_STATISTICS_STC;


/******************************************************************************
* Typedef: struct CPSS_TM_ERROR_INFO_STC
*
* Description: TM Blocks Error Information.
*
* Fields:
*
*      errorCounter           - TM Block Error counter
*      exceptionCounter       - TM Block Exception Counter
*
******************************************************************************/
typedef struct
{
	GT_U32  errorCounter;
	GT_U32  exceptionCounter;
} CPSS_TM_ERROR_INFO_STC;


/******************************************************************************
* Typedef: struct CPSS_TM2TM_EXTERNAL_HDR_STC
*
* Description: TM2TM External Headers.
*
* Fields:
*
*      size           - Header size - only fixed values - 3, 7, 11 or 15
*                       (APPLICABLE RANGES: 3, 7, 11, 15) - fixed values
*      contentArr[4]  - The header content
*
******************************************************************************/
typedef struct
{
	GT_U32  size;
	GT_U64  contentArr[4];
} CPSS_TM2TM_EXTERNAL_HDR_STC;


/******************************************************************************
* Typedef: struct CPSS_TM2TM_CNTRL_PKT_STRUCT_STC
*
* Description: TM2TM Control Packet Structure.
*
* Fields:
*
*      portsNum   - Number of ports updates in TM to TM packet
*      nodesNum   - Number of nodes updates in TM to TM packet
*
******************************************************************************/
typedef struct
{
	GT_U32   portsNum;
	GT_U32   nodesNum;
} CPSS_TM2TM_CNTRL_PKT_STRUCT_STC;


/******************************************************************************
* Typedef: struct CPSS_TM2TM_DELTA_RANGE_STC
*
* Description: TM2TM Delta Range Mapping to Priority.
*
* Fields:
*
*      upperRange0 - Priority 0 upper range
*      upperRange1 - Priority 1 upper range
*      upperRange2 - Priority 2 upper range
******************************************************************************/
typedef struct
{
	GT_U32   upperRange0;
	GT_U32   upperRange1;
	GT_U32   upperRange2;
} CPSS_TM2TM_DELTA_RANGE_STC;


/******************************************************************************
* Typedef: struct CPSS_TM_TREE_CHANGE_STC
*
* Description: Reshuffling index/range change structure.
*
* Fields:
*
*      type     - Type of change: index - TM_ENABLE, range - TM_DISABLE
*      index    - Index of changed parent node
*      oldIndex - Old index/range
*      newIndex - New index/range
*      nextPtr  - Pointer to the next change
******************************************************************************/
typedef struct CPSS_TM_TREE_CHANGE_STCT
{
	GT_U32   type;
	GT_U32   index;
	GT_U32   oldIndex;
    GT_U32   newIndex;
    struct CPSS_TM_TREE_CHANGE_STCT *nextPtr;
} CPSS_TM_TREE_CHANGE_STC;


/******************************************************************************
* Typedef: struct CPSS_TM_ELIG_PRIO_FUNC_OUT_STC
*
* Description: Eligible Priority Function Data structure.
*
* Fields:
*
*      maxTb     - Use Max Token Bucket
*      minTb     - Use Min Token Bucket
*      propPrio  - Propagated priority
*      schedPrio - Scheduling priority
*      elig      - Eligibility
******************************************************************************/
typedef struct
{
    GT_U32  maxTb;
    GT_U32  minTb;
	GT_U32  propPrio;
    GT_U32  schedPrio;
    GT_U32  elig;
} CPSS_TM_ELIG_PRIO_FUNC_OUT_STC;


typedef union
{
	CPSS_TM_ELIG_PRIO_FUNC_OUT_STC queueEligPrioFunc[4];
    CPSS_TM_ELIG_PRIO_FUNC_OUT_STC nodeEligPrioFunc[8][4];
} CPSS_TM_ELIG_PRIO_FUNC_STC;


/******************************************************************************
* Typedef: struct CPSS_TM_QUANTUM_LIMITS_STC
*
* Description: quantum Limits for port/node.  Used for read these parameters from  TM instance
*
* Fields:
*
*      minQuantum - minimum possible quantum value  for port/node in bytes.
*      maxQuantum - maximum possible quantum value  for port/node in bytes.
*      resolution - quantum value granularioty in bytes.
*
******************************************************************************/
typedef struct
{
    GT_U32 minQuantum;
    GT_U32 maxQuantum;
    GT_U32 resolution;
} CPSS_TM_QUANTUM_LIMITS_STC;


/******************************************************************************
* Typedef: struct CPSS_TM_TREE_PARAMS_STC
*
* Description: TM Tree HW properties and Limits.
*
* Fields:
*
*      maxQueues - max number of Queue Nodes.
*      maxAnodes - max number of A Nodes.
*      maxBnodes - max number of B Nodes.
*      maxCnodes - max number of C Nodes.
*      maxPorts  - max number of TM Ports.
*
******************************************************************************/
typedef struct
{
    GT_U32	maxQueues;
    GT_U32	maxAnodes;
    GT_U32	maxBnodes;
    GT_U32	maxCnodes;
    GT_U32	maxPorts;
} CPSS_TM_TREE_PARAMS_STC;

/******************************************************************************
* Typedef: struct CPSS_TM_UNITS_ERROR_STATUS_STC
*
* Description: TM Units Global Error Status
*
* Fields:
*
*      qmngrStatus - Queue Manager Unit Status.
*      dropStatus  - Drop Unit Status
*      schedStatus - Scheduler Unit Status.
*      rcbStatus   - RCB (Packets Reordering) Unit Status.
*******************************************************************************/
typedef struct
{
    GT_U32	qmngrStatus;
    GT_U32	dropStatus;
    GT_U32	schedStatus;
    GT_U32	rcbStatus;
} CPSS_TM_UNITS_ERROR_STATUS_STC;

/******************************************************************************
* Typedef: struct CPSS_TM_BAP_ERROR_STATUS_STC
*
* Description: TM BAP Unit Global Error Status
*
* Fields:
*
*      bapStatus -  BAP (Dram Access) Unit Status.
*      bapNum    -  BAP device number. 
*******************************************************************************/
typedef struct
{
	GT_U32	bapStatus;
	GT_U8	bapNum;
} CPSS_TM_BAP_UNIT_ERROR_STATUS_STC;

#endif 	    /* __cpssTmPublicDefsh */

