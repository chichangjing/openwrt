/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdCpssDxChIncrEntry.h
*
* DESCRIPTION:
*       CPSS Utils for create incremented Entries sequences
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 11 $
*
*******************************************************************************/
#ifndef __cmdCpssDxChIncrEntryh
#define __cmdCpssDxChIncrEntryh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

/* Feature specific includes. */
#include <cpss/dxCh/dxChxGen/oam/cpssDxChOam.h>
#include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>

/*******************************************************************************
* cmdCpssDxChGenIntrfaceInfoIncrement
*
* DESCRIPTION:
*       Inrement the given structure.
*
* INPUTS:
*       basePtr         - (pointer to)the structure with base values.
*       incPtr          - (pointer to)the structure with increment values.
*
*
* OUTPUTS:
*       basePtr         - (pointer to)the structure with result values.
*
* RETURNS:
*       GT_OK - on sucess, GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cmdCpssDxChGenIntrfaceInfoIncrement
(
    INOUT CPSS_INTERFACE_INFO_STC *basePtr,
    IN    CPSS_INTERFACE_INFO_STC *incPtr
);

/*******************************************************************************
* cmdCpssDxChOamEntryIncrement
*
* DESCRIPTION:
*       Inrement the given structure.
*
* INPUTS:
*       basePtr         - (pointer to)the structure with base values.
*       incPtr          - (pointer to)the structure with increment values.
*
*
* OUTPUTS:
*       basePtr         - (pointer to)the structure with result values.
*
* RETURNS:
*       GT_OK - on sucess, GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cmdCpssDxChOamEntryIncrement
(
    INOUT CPSS_DXCH_OAM_ENTRY_STC *basePtr,
    IN    CPSS_DXCH_OAM_ENTRY_STC *incPtr
);

/*******************************************************************************
* cmdCpssDxChOamEntrySequenceWrite
*
* DESCRIPTION:
*       Write sequence of incremented OAM Entries.
*
* INPUTS:
*       devNum            - device number
*       stage             - OAM stage type.
*       baseEntryIndex  - index of entry to be retrieved and used as base values.
*                           this entry also will be start entry of the sequence
*       incrEntryIndex  - index of entry to be retrieved and used as increment values.
*                           retieved at the beginnig and can be overriden by sequence.
*       amountToWrite     - amount of enries to write
*       writeIndexIncr    - signed increment of sequence entry index
*
*
* OUTPUTS:
*       basePtr         - (pointer to)the structure with result values.
*
* RETURNS:
*       GT_OK - on sucess, GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cmdCpssDxChOamEntrySequenceWrite
(
    IN   GT_U8                              devNum,
    IN   CPSS_DXCH_OAM_STAGE_TYPE_ENT       stage,
    IN   GT_U32                             baseEntryIndex,
    IN   GT_U32                             incrEntryIndex,
    IN   GT_U32                             amountToWrite,
    IN   GT_32                              writeIndexIncr
);

/*******************************************************************************
* cmdCpssDxChPclRuleIncrement
*
* DESCRIPTION:
*       Increment all supported fields in the given structure.
*
* INPUTS:
*       ruleFormat      - Rule Format
*       basePtr         - (pointer to)the structure with base values.
*       incrPtr         - (pointer to)the structure with increment values.
*
* OUTPUTS:
*       basePtr         - (pointer to)the structure with result values.
*
* RETURNS:
*       GT_OK - on sucess, GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cmdCpssDxChPclRuleIncrement
(
    IN    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT        ruleFormat,
    INOUT CPSS_DXCH_PCL_RULE_FORMAT_UNT             *basePtr,
    IN    CPSS_DXCH_PCL_RULE_FORMAT_UNT             *incrPtr
);

/*******************************************************************************
* cmdCpssDxChPclActionIncrement
*
* DESCRIPTION:
*       Increment all supported fields in the given structure.
*
* INPUTS:
*       ruleFormat      - Rule Format
*       basePtr         - (pointer to)the structure with base values.
*       incrPtr         - (pointer to)the structure with increment values.
*
* OUTPUTS:
*       basePtr         - (pointer to)the structure with result values.
*
* RETURNS:
*       GT_OK - on sucess, GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cmdCpssDxChPclActionIncrement
(
    INOUT CPSS_DXCH_PCL_ACTION_STC             *basePtr,
    IN    CPSS_DXCH_PCL_ACTION_STC             *incrPtr
);

/*******************************************************************************
* cmdCpssDxChPclRuleSequenceWrite
*
* DESCRIPTION:
*       Write sequence of incremented PclRules.
*       The rule mask copied unchanged, pattern and action incremented.
*
* INPUTS:
*       devNum            - device number
*       ruleFormat        - format of all related rules.
*       baseRuleIndex     - index of rule to be retrieved and used as base values.
*                           this entry also will be start rule of the sequence
*       incrRuleIndex     - index of rule to be retrieved and used as increment values.
*                           retieved at the beginnig and can be overriden by sequence.
*       amountToWrite     - amount of rules to write
*       writeIndexIncr    - signed increment of sequence rule index
*
*
* OUTPUTS:
*       basePtr         - (pointer to)the structure with result values.
*
* RETURNS:
*       GT_OK - on sucess, other on error.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cmdCpssDxChPclRuleSequenceWrite
(
    IN   GT_U8                              devNum,
    IN   CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat,
    IN   GT_U32                             baseRuleIndex,
    IN   GT_U32                             incrRuleIndex,
    IN   GT_U32                             amountToWrite,
    IN   GT_32                              writeIndexIncr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cmdCpssDxChIncrEntryh */


