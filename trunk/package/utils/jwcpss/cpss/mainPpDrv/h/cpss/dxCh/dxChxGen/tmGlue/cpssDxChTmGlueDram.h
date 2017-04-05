/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChTmGlueDram.h
*
* DESCRIPTION:
*       TM related DRAM APIs H file
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*******************************************************************************/

#ifndef __cpssDxChTmGlueDramh
#define __cpssDxChTmGlueDramh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssHwInit/cpssHwInit.h>

/* Number of buses per interface */
#define CPSS_DXCH_TM_GLUE_DRAM_INF_BUS_CNS  4

/* External Memory Initialization flags         */
/* Initialize DRAM and PHY only, don't touch TM */
#define CPSS_DXCH_TM_GLUE_DRAM_INIT_DDR_PHY_CNS  1

 /*
 * typedef: enum CPSS_DXCH_TM_GLUE_DRAM_CONFIGURATION_TYPE_ENT
 *
 * Description: TM DDR configuration type.
 *
 * Enumerations:
 *      CPSS_DXCH_TM_GLUE_DRAM_CONFIGURATION_TYPE_DYNAMIC_E -
 *                                                      Dynamic configuration.
 *      CPSS_DXCH_TM_GLUE_DRAM_CONFIGURATION_TYPE_STATIC_E -
 *                                                      Static configuration.
 *
 */
typedef enum
{
    CPSS_DXCH_TM_GLUE_DRAM_CONFIGURATION_TYPE_DYNAMIC_E,
    CPSS_DXCH_TM_GLUE_DRAM_CONFIGURATION_TYPE_STATIC_E
}CPSS_DXCH_TM_GLUE_DRAM_CONFIGURATION_TYPE_ENT;

/*
 * typedef: CPSS_DXCH_TM_GLUE_DRAM_ALGORITHM_STC
 *
 * Description:
 *      This struct defines the DRAM configuration algorithm parameters.
 *
 * Fields:
 *  algoType                - static or dynamic configuration.
 *  performWriteLeveling    - GT_TRUE:  enable performing write leveling.
 *                            GT_FALSE: disable performing write leveling.
 * Comment:
 *
*/
typedef struct
{
    CPSS_DXCH_TM_GLUE_DRAM_CONFIGURATION_TYPE_ENT   algoType;
    GT_BOOL                                         performWriteLeveling;
}CPSS_DXCH_TM_GLUE_DRAM_ALGORITHM_STC;


/*
 * typedef: struct CPSS_DXCH_TM_GLUE_DRAM_BUS_PARAM_STC
 *
 * Description:
 *      This struct defines specific DRAM bus interface parameters.
 *
 * Fields:
 *  csBitmask       - Chip Select (CS) bitmask, bit<n> for CS<n>.
 *  mirrorEnBitmask - Mirror enabling.
 *  dqsSwapEn       - DQS Swap (polarity) enabling.
 *  ckSwapEn        - CK Swap (polarity) enabling.
 * Comment:
 *
*/
typedef struct
{
   GT_U32   csBitmask;
   GT_BOOL  mirrorEn;
   GT_BOOL  dqsSwapEn;
   GT_BOOL  ckSwapEn;
}
CPSS_DXCH_TM_GLUE_DRAM_BUS_PARAM_STC;

/*
 * typedef: struct CPSS_DXCH_TM_GLUE_DRAM_INF_PARAM_STC
 *
 * Description:
 *      This struct defines specific DRAM interface parameters.
 *
 * Fields:
 *  busParams       - Interface bus parameters.
 *  speedBin        - Speed bining (JEDEC standard name).
 *  busWidth        - Memory bus width.
 *  memorySize      - Memory Size (in MByte).
 *  memoryFreq      - Memory interface frequency.
 *  casWL           - Delay CAS Write Latency.
 *                    (0 for using default value - jedec suggested).
 *  casL            - Delay CAS Latency.
 *                    (0 for using default value - jedec suggested).
 *  interfaceTemp   - operating temperature.
 *
 * Comment:
 *
*/
typedef struct
{
   CPSS_DXCH_TM_GLUE_DRAM_BUS_PARAM_STC busParams[CPSS_DXCH_TM_GLUE_DRAM_INF_BUS_CNS];
   CPSS_DRAM_SPEED_BIN_ENT              speedBin;
   CPSS_DRAM_BUS_WIDTH_ENT              busWidth;
   CPSS_DRAM_SIZE_ENT                   memorySize;
   CPSS_DRAM_FREQUENCY_ENT              memoryFreq;
   GT_U32                               casWL;
   GT_U32                               casL;
   CPSS_DRAM_TEMPERATURE_ENT            interfaceTemp;
}
CPSS_DXCH_TM_GLUE_DRAM_INF_PARAM_STC;

/*
 * typedef: struct CPSS_DXCH_TM_GLUE_DRAM_CFG_STC
 *
 * Description:
 *      This struct defines specific DRAM intrface hardware parameters relevant
 *      for the Traffic Manager.
 *
 * Fields:
 *  activeInterfaceNum  -  Number of active DDR interfaces.
 *                         (APPLICABLE RANGES: 2..5.)
 *  activeInterfaceMask -  Custom Order of active DDR interfaces.
 *                         APPLICABLE RANGES:
 *                           0x0        - mask is calculated from activeInterfaceNum.
 *                           0x01..0x1F - custom mask is specified.
 *  interfaceParams     -  Controller interface configurations.
 *
 * Comment:
 *
*/
typedef struct
{
    GT_U32                               activeInterfaceNum;
    GT_U32                               activeInterfaceMask;
    CPSS_DXCH_TM_GLUE_DRAM_INF_PARAM_STC interfaceParams;
}
CPSS_DXCH_TM_GLUE_DRAM_CFG_STC;

/*******************************************************************************
* cpssDxChTmGlueDramInit
*
* DESCRIPTION:
*       Traffic Manager DRAM related configurations
*       (DDR, PHY & TM BAP registers).
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum      - physical device number
*       dramCfgPtr  - (pointer to) DRAM interface configuration parameters
*       dramAlgoPtr - (pointer to) DRAM configuration algorithm parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or configuration parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_CPU_MEM        - no CPU memory allocation fail
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueDramInit
(
    IN  GT_U8                                  devNum,
    IN  CPSS_DXCH_TM_GLUE_DRAM_CFG_STC         *dramCfgPtr,
    IN  CPSS_DXCH_TM_GLUE_DRAM_ALGORITHM_STC   *dramAlgoPtr
);

/*******************************************************************************
* cpssDxChTmGlueDramInitFlagsSet
*
* DESCRIPTION:
*       Set flags for Traffic Manager DRAM related configurations
*       (DDR, PHY & TM BAP registers).
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum                   - physical device number
*       externalMemoryInitFlags  - External Memory Init Flags
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or configuration parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueDramInitFlagsSet
(
    IN  GT_U8                                 devNum,
    IN  GT_U32                                externalMemoryInitFlags
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChTmGlueDramh */


