/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDiag.h
*
* DESCRIPTION:
*       General diagnostic definitions and data structures for PP.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*
*******************************************************************************/
#ifndef __cpssDiagh
#define __cpssDiagh

#include <cpss/generic/cpssTypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Typedef: CPSS_DIAG_TEST_PROFILE_ENT
 *
 * Description: Test profile.
 *
 * Enumerations:
 *      CPSS_DIAG_TEST_RANDOM_E            - write random values
 *      CPSS_DIAG_TEST_INCREMENTAL_E       - write incremental values
 *      CPSS_DIAG_TEST_BIT_TOGGLE_E        - toggles all the bits
 *      CPSS_DIAG_TEST_AA_55_E             - write 0xAAAAAAAA/0x55555555
 */
typedef enum
{
    CPSS_DIAG_TEST_RANDOM_E,
    CPSS_DIAG_TEST_INCREMENTAL_E,
    CPSS_DIAG_TEST_BIT_TOGGLE_E,
    CPSS_DIAG_TEST_AA_55_E
}CPSS_DIAG_TEST_PROFILE_ENT;

/*
 * Typedef: CPSS_DIAG_PP_MEM_TYPE_ENT
 *
 * Description: Packet Process memory type.
 *
 * Enumerations:
 *      CPSS_DIAG_PP_MEM_BUFFER_DRAM_E            - The external packet buffer
 *                                                  SDRAM.
 *                                                  (APPLICABLE DEVICES: ExMx; ExMxPm.)
 *                                                  (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.)
 *      CPSS_DIAG_PP_MEM_WIDE_SRAM_E              - The external Wide SRAM, or
 *                                                  internal Control SRAM if the
 *                                                  device was configured
 *                                                  without external wide-SRAM.
 *                                                  (APPLICABLE DEVICES: ExMx.)
 *      CPSS_DIAG_PP_MEM_NARROW_SRAM_E            - the external Narrow SRAM, or
 *                                                  internal IP SRAM if the
 *                                                  device was configured
 *                                                  without external wide-SRAM.
 *                                                  Not relevant for EX1x6.
 *                                                  (APPLICABLE DEVICES: ExMx; ExMxPm.)
 *      CPSS_DIAG_PP_MEM_INTERNAL_NARROW_SRAM_0_E - Internal Narrow SRAM 0
 *                                                  (EX1x6 only)
 *      CPSS_DIAG_PP_MEM_INTERNAL_NARROW_SRAM_1_E - Internal Narrow SRAM 1
 *                                                  (EX1x6 only)
 *      CPSS_DIAG_PP_MEM_EXTERNAL_NARROW_SRAM_0_E - External Narrow SRAM 0
 *                                                  (EX1x6 only)
 *      CPSS_DIAG_PP_MEM_EXTERNAL_NARROW_SRAM_1_E - External Narrow SRAM 1
 *                                                  (EX1x6 only)
 *      CPSS_DIAG_PP_MEM_FLOW_DRAM_E              - The external flow DRAM.
 *                                                  (APPLICABLE DEVICES: ExMx.)
 *      CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E            - Internal MAC table memory.
 *                                                  (APPLICABLE DEVICES: ExMx; ExMxPm.)
 *                                                  (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.)
 *      CPSS_DIAG_PP_MEM_VLAN_TBL_MEM_E           - Internal VLAN table memory.
 *                                                  (APPLICABLE DEVICES: ExMx; ExMxPm.)
 *                                                  (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.)
 */
typedef enum
{
    CPSS_DIAG_PP_MEM_BUFFER_DRAM_E,
    CPSS_DIAG_PP_MEM_WIDE_SRAM_E,
    CPSS_DIAG_PP_MEM_NARROW_SRAM_E,
    CPSS_DIAG_PP_MEM_INTERNAL_NARROW_SRAM_0_E,
    CPSS_DIAG_PP_MEM_INTERNAL_NARROW_SRAM_1_E,
    CPSS_DIAG_PP_MEM_EXTERNAL_NARROW_SRAM_0_E,
    CPSS_DIAG_PP_MEM_EXTERNAL_NARROW_SRAM_1_E,
    CPSS_DIAG_PP_MEM_FLOW_DRAM_E,
    CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E,
    CPSS_DIAG_PP_MEM_VLAN_TBL_MEM_E
}CPSS_DIAG_PP_MEM_TYPE_ENT;

/*
 * Typedef: CPSS_DIAG_PP_REG_TYPE_ENT
 *
 * Description: Packet Process registers type.
 *
 * Enumerations:
 *      CPSS_DIAG_PP_REG_INTERNAL_E - PP internal register space.
 *      CPSS_DIAG_PP_REG_PCI_CFG_E  - PP pci config and PEX register space.
 */
typedef enum
{
    CPSS_DIAG_PP_REG_INTERNAL_E,
    CPSS_DIAG_PP_REG_PCI_CFG_E
}CPSS_DIAG_PP_REG_TYPE_ENT;


/*
 * Typedef: enum CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_ENT
 *
 * Description:
 *      This enum defines memory error injection type
 *
 * Enumerations:
 *      CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_SINGLE_E - single bit of the data
 *              will be inverted in the next write transaction.
 *
 *      CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_MULTIPLE_E - two bits of the data
 *              will be inverted in the next write transaction.
 *
 *  Comments:
 */

typedef enum
{
    CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_SINGLE_E,
    CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_MULTIPLE_E
}CPSS_DIAG_DATA_INTEGRITY_ERROR_INJECT_MODE_ENT;


/*
 * Typedef: enum CPSS_DIAG_DATA_INTEGRITY_ERROR_PROTECTION_TYPE_ENT
 *
 * Description:
 *      This enum defines memory error protection type
 *
 * Enumerations:
 *      CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_NONE_E - not protected
 *
 *      CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_ECC_E -  ECC protected
 *
 *      CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_PARITY_E - parity protected
 *
 *  Comments:
 */

typedef enum
{
    CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_NONE_E,
    CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_ECC_E,
    CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_PARITY_E
}CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_ENT;


/*
 * Typedef: enum CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT
 *
 * Description:
 *      This enum defines ECC/parity error cause type
 * Enumerations:
 *      CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E       - parity errors.
 *
 *      CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E   - single ECC errors.
 *
 *      CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E - multiple ECC errors.
 *
 *      CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_AND_MULTIPLE_ECC_E - single and multiple ECC errors.
 *
 *  Comments:
 */
typedef enum
{
    CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E,
    CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_ECC_E,
    CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_MULTIPLE_ECC_E,
    CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_AND_MULTIPLE_ECC_E
}CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT;


/*
 * Typedef: struct CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_STC
 *
 * Description: Memory location indexes
 *
 * Fields:
 *      dfxPipeId     - DFX pipe ID
 *      dfxClientId   - DFX client ID
 *      dfxMemoryId   - DFX memory ID
 *
 * Comments:
 *      None
 */
typedef struct
{
    GT_U32 dfxPipeId;
    GT_U32 dfxClientId;
    GT_U32 dfxMemoryId;
}CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_STC;

/*
 * Typedef: struct CPSS_DIAG_DATA_INTEGRITY_MPPM_MEMORY_LOCATION_STC
 *
 * Description: MPPM Memory location indexes
 *
 * Fields:
 *      portGroupId - port group ID
 *      mppmId      - MPPM ID
 *      bankId      - MPPM bank ID
 *
 * Comments:
 *      None
 */
typedef struct
{
    GT_U32 portGroupId;
    GT_U32 mppmId;
    GT_U32 bankId;
}CPSS_DIAG_DATA_INTEGRITY_MPPM_MEMORY_LOCATION_STC;


/*
 * Typedef: CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_ENT
 *
 * Description: TCAM array type.
 *
 * Enumerations:
 *      CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_X_E - Array X.
 *      CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_Y_E  - Array Y.
 */
typedef enum
{
    CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_X_E,
    CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_Y_E
}CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_ENT;

/*
 * Typedef: struct CPSS_DIAG_DATA_INTEGRITY_TCAM_MEMORY_LOCATION_STC
 *
 * Description: TCAM Memory location indexes
 *
 * Fields:
 *      arrayType  - The type of the array that the parity error was detected on
 *      ruleIndex  - for Policy TCAM: index of the standard rule in the TCAM.
 *                   See cpssDxChPclRuleSet
 *                   for Router TCAM: line index in TCAM to write to (same as TTI index)
 *
 * Comments:
 *      None
 */
typedef struct
{
    CPSS_DIAG_DATA_INTEGRITY_TCAM_ARRAY_TYPE_ENT    arrayType;
    GT_U32                                          ruleIndex;
}CPSS_DIAG_DATA_INTEGRITY_TCAM_MEMORY_LOCATION_STC;


/*
 * typedef: struct CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_UNT
 *
 * Description:
 *      Holds the two possible memory location formats.
 *
 * Fields:
 *  memLocation      - regular memory location indexes
 *  mppmMemLocation  - special memory location indexes for MPPM memory
 *  tcamMemLocation  - special memory location indexes for TCAM memory
 *
 */
typedef union
{
    CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_STC        memLocation;
    CPSS_DIAG_DATA_INTEGRITY_MPPM_MEMORY_LOCATION_STC   mppmMemLocation;
    CPSS_DIAG_DATA_INTEGRITY_TCAM_MEMORY_LOCATION_STC   tcamMemLocation;

}CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_UNT;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDiagh */

