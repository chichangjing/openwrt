/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPrvIpLpm.h
*
* DESCRIPTION:
*       This file includes internal typedefs declarations for controlling the
*       IP tables and structures, and structures definitions for shadow
*       management.
*
* FILE REVISION NUMBER:
*       $Revision: 37 $
*
*******************************************************************************/
#ifndef __cpssDxChPrvIpLpmh
#define __cpssDxChPrvIpLpmh

#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpm.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/cpssDxChPrvIpLpmTypes.h>
#include <cpss/dxCh/dxChxGen/resourceManager/cpssDxChTcamManagerTypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* prvCpssDxChIpLpmDbGetL3
*
* DESCRIPTION:
*       Retrieves a specific shadow's ip Table memory Size needed and info
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       iterationSizePtr   - (pointer to) data size in bytes supposed to be processed
*                            in current iteration.
*       iterPtr            - the iterator, to start - set to 0.
*
* OUTPUTS:
*       tableSizePtr  - (pointer to) the table size calculated (in bytes)
*       tablePtr      - (pointer to) the table size info block
*       iterationSizePtr   - (pointer to) data size in bytes left after iteration .
*       iterPtr       - the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In case the LPM DB uses TCAM manager and creates the TCAM manager
*       internally, then the LPM DB is also responsible for exporting and
*       importing the TCAM manager data. The TCAM manager must be imported
*       before VR, UC and MC entries.
*       If the LPM uses TCAM manager (internally or externally) then the LPM DB
*       must update the TCAM manager client callback functions.
*
*       Data is arranged in entries of different types in the following order:
*        - TCAM manager entries, if needed. Each TCAM manager entry is 1k
*          of the TCAM manager HSU data (last entry is up to 1k).
*        - VR entry per virtual router in the LPM DB. If the LPM DB contains
*          no VR, then this section is empty.
*        - Dummy VR entry. Note that the dummy VR entry will always exists,
*          even if there are no VR in the LPM DB
*        - If the LPM DB supports IPv4 protocol then for each virtual router in
*          the LPM DB the following section will appear. The order of the
*          virtual routers is according to the virtual router ID. In case
*          there are no virtual routers, this section will be empty.
*            - UC entry for each IPv4 UC entry other than the default.
*            - Dummy UC entry.
*            - MC entry for each IPv4 MC entry other than the default.
*            - Dummy MC entry
*        - If the LPM DB supports IPv6 protocol then for each virtual router in
*          the LPM DB the following section will appear. The order of the
*          virtual routers is according to the virtual router ID. In case
*          there are no virtual routers, this section will be empty.
*            - UC entry for each IPv6 UC entry other than the default.
*            - Dummy UC entry.
*            - MC entry for each IPv6 MC entry other than the default.
*            - Dummy MC entry.
*       Note that if the LPM DB doesn't support a protocol stack then the
*       sections related to this protocol stack will be empty (will not include
*       the dummy entries as well).
*       If a VR doesn't support one of the prefix types then the section for
*       this prefix type will be empty but will include dummy.
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpLpmDbGetL3
(
    IN     GT_U32                       lpmDBId,
    OUT    GT_U32                       *tableSizePtr,
    OUT    GT_VOID                      *tablePtr,
    INOUT  GT_U32                       *iterationSizePtr,
    INOUT  GT_UINTPTR                   *iterPtr
);

/*******************************************************************************
* prvCpssDxChIpLpmDbSetL3
*
* DESCRIPTION:
*       Set the data needed for core IP shadow reconstruction used after HSU.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       tablePtr            - (pointer to)the table size info block.
*       iterationSizePtr    - (pointer to) data size in bytes supposed to be processed.
*                             in current iteration.
*       iterPtr             - the iterator, to start - set to 0.
*
* OUTPUTS:
*       iterationSizePtr    - (pointer to) data size in bytes left after iteration .
*       iterPtr             - the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       Refer to prvCpssDxChIpLpmDbGetL3.
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpLpmDbSetL3
(
    IN     GT_U32                       lpmDBId,
    IN     GT_VOID                      *tablePtr,
    INOUT  GT_U32                       *iterationSizePtr,
    INOUT  GT_UINTPTR                   *iterPtr
);

/*******************************************************************************
* prvCpssDxChIpLpmDbRelease
*
* DESCRIPTION:
*       private (internal) function to release the DB of LPM.
*       NOTE: function not 'free' the allocated memory. only detach from it ,
*             and restore DB to 'pre-init' state
*
*             The assumption is that the 'cpssOsMalloc' allocations will be not
*             valid any more by the application , so no need to 'free' each and
*             every allocation !
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2;
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpLpmDbRelease
(
    void
);

/*******************************************************************************
* prvCpssDxChIpLpmDbIdGetNext
*
* DESCRIPTION:
*       This function retrieve next LPM DB ID from LPM DBs Skip List
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2;
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       slIteratorPtr - The iterator Id that was returned from the last call to
*                       this function.
* OUTPUTS:
*       lpmDbIdPtr    - retrieved LPM DB ID
*       slIteratorPtr - The iteration Id to be sent in the next call to this
*                       function to get the next data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_NO_MORE               - on absence of elements in skip list
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpLpmDbIdGetNext
(
    OUT     GT_U32      *lpmDbIdPtr,
    INOUT   GT_UINTPTR  *slIteratorPtr
);

/*******************************************************************************
* prvCpssDxChIpLpmLogRouteEntryTypeGet
*
* DESCRIPTION:
*   This function gets route entry type according to lpmDBId
*
* INPUTS:
*       lpmDBId       - The LPM DB id.
*
* OUTPUTS:
*   entryTypePtr     - GT_TRUE if route entry is pclIpUcAction
*                       GT_FALSE if route entry is ipLttEntry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpLpmLogRouteEntryTypeGet
(
    IN    GT_U32     lpmDBId,
    OUT   GT_BOOL    *entryTypePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChPrvIpLpmh */


