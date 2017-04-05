/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChDiagDataIntegrityMainMappingDb.h
*
* DESCRIPTION:
*       Internal header with DFX Data Integrity module main mapping batabase.
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*
*******************************************************************************/
#ifndef __prvCpssDxChDiagDataIntegrityMainMappingDbh
#define __prvCpssDxChDiagDataIntegrityMainMappingDbh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/dxCh/dxChxGen/diag/cpssDxChDiagDataIntegrity.h>

/* Lion2 max port group number */
#define LION2_MAX_PORT_GROUP_NUM_CNS                    8
/* Lion2 max number of DFX pipes */
#define LION2_MAX_NUMBER_OF_PIPES_CNS                   8
/* Maximal DFX cause index  */
#define MAX_DFX_INT_CAUSE_NUM_CNS                       27
/* indicator of unused DB entry */
#define DATA_INTEGRITY_ENTRY_NOT_USED_CNS               0xCAFECAFE  
/* Lion2 Hooper max port group number */
#define HOOPER_MAX_PORT_GROUP_NUM_CNS                   4



/*
 * Typedef: struct PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_MAP_DB_STC
 *
 * Description: Bitmap of memories in DFX Client
 *
 * Fields:
 *      key     - DB key - consist of DFX pipe number, Client Number and Memory number
 *      memType - memory type 
 *      protectionType - memory protection type
 *      externalProtectionType - memory external protection type (non-DFX mechanism)
 *      causePortGroupId - port group ID of event has happened
 *      firstTableDataBit - first table data bit
 *      lastTableDataBit - last table data bit
 *
 * Comments:
 *      None
 */
typedef struct
{
    GT_U32                                                  key;
    CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT              memType;
    CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_ENT  protectionType;
    CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_ENT  externalProtectionType;
    GT_U32                                                  causePortGroupId;
    GT_U32                                                  firstTableDataBit;
    GT_U32                                                  lastTableDataBit;
}PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_MAP_DB_STC;

extern PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_MAP_DB_STC lion2DataIntegrityDbArray[];
extern GT_U32 lion2DataIntegrityDbArrayEntryNum;

extern PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_MAP_DB_STC hooperDataIntegrityDbArray[];
extern GT_U32 hooperDataIntegrityDbArrayEntryNum;

extern PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_MAP_DB_STC bobcat2DataIntegrityDbArray[];
extern GT_U32 bobcat2DataIntegrityDbArrayEntryNum;

extern PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_MAP_DB_STC bobcat2B0DataIntegrityDbArray[];
extern GT_U32 bobcat2B0DataIntegrityDbArrayEntryNum;

extern PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_MAP_DB_STC caelumDataIntegrityDbArray[];
extern GT_U32 caelumDataIntegrityDbArrayEntryNum;

/*
 * Typedef: struct PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_FIRST_STAGE_MAPPING_DATA_STC
 *
 * Description: First stage map entry.
 *
 * Fields:
 *      dfxPipeIndex     - DFX pipe index
 *      dfxClientIndex   - DXF client index
 *
 * Comments:
 *      None
 */
typedef struct
{
    GT_U32 dfxPipeIndex;
    GT_U32 dfxClientIndex;
}PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_FIRST_STAGE_MAPPING_DATA_STC;

extern PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_FIRST_STAGE_MAPPING_DATA_STC lion2DataIntegrityFirstStageMappingArray[LION2_MAX_PORT_GROUP_NUM_CNS][MAX_DFX_INT_CAUSE_NUM_CNS];
extern PRV_CPSS_DXCH_DIAG_DATA_INTEGRITY_FIRST_STAGE_MAPPING_DATA_STC hooperDataIntegrityFirstStageMappingArray[HOOPER_MAX_PORT_GROUP_NUM_CNS][MAX_DFX_INT_CAUSE_NUM_CNS];

extern GT_STATUS prvCpssDxChDiagDataIntegrityMemoryIndexesGet
(
    IN  GT_U8                                                   devNum,
    IN  CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT              memType,
    IN  GT_BOOL                                                 isPerPortGroup,
    IN  GT_U32                                                  portGroupId,
    INOUT GT_U32                                                *arraySizePtr,
    OUT CPSS_DIAG_DATA_INTEGRITY_MEMORY_LOCATION_STC            *memLocationArr,
    OUT CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_ENT  *protectionTypePtr
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
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChDiagDataIntegrityMainMappingDbh */
    

