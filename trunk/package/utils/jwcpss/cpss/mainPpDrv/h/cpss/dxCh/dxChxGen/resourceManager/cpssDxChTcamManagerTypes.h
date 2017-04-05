/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChTcamManagerTypes.h
*
* DESCRIPTION:
*       This file contains types for TCAM manager object.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*
*******************************************************************************/

#ifndef __cpssDxChTcamManagerTypesh
#define __cpssDxChTcamManagerTypesh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

/*
 * typedef: struct CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC
 *
 * Description:
 *      Holds representation of TCAM location.
 *
 * Fields:
 *      row               - TCAM row
 *      column            - TCAM column
 */
typedef struct
{
    GT_U32  row;
    GT_U32  column;

} CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC;

/*******************************************************************************
* CPSS_DXCH_TCAM_MANAGER_MOVE_TO_LOCATION_FUNC
*
* DESCRIPTION:
*       Client callback function used by the TCAM manager to request the client
*       to copy single entry from one TCAM location to another TCAM location
*       and to invalidate the entry in the original location.
*       The TCAM manager decides the target location for the entry based only
*       on the TCAM search order. It uses the client because the TCAM manager
*       is not aware of the data in the entry.
*
* INPUTS:
*       fromPtr           - points to the original hardware TCAM location
*       toPtr             - points to the new hardware TCAM location
*       clientCookiePtr   - points to the client cookie associated with the entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - if allocation succedded
*       GT_OUT_OF_PP_MEM  - if there is no space left between the upper
*                           and lower bounds
*       GT_OUT_OF_RANGE   - if the indexes were out of range
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_DXCH_TCAM_MANAGER_MOVE_TO_LOCATION_FUNC)
(
    IN  CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC    *fromPtr,
    IN  CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC    *toPtr,
    IN  GT_VOID                                     *clientCookiePtr
);

/*******************************************************************************
* CPSS_DXCH_TCAM_MANAGER_MOVE_TO_ANYWHERE_FUNC
*
* DESCRIPTION:
*       Client callback function used by the TCAM manager to request the client
*       to move single entry from TCAM location and to invalidate the entry in
*       the original location.
*       The TCAM manager leave it to the client to choose different location
*       for the entry, possibly by taking into consideration the data in the
*       entry.
*       This callback function is optional. If it is not supplied by the client,
*       then the TCAM manager choose different location based on TCAM search
*       order only and uses the copy callback function to move the entry.
*
* INPUTS:
*       clientCookiePtr   - points to the client cookie associated with the entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - if allocation succedded
*       GT_OUT_OF_PP_MEM  - if there is no space left between the upper
*                           and lower bounds
*       GT_OUT_OF_RANGE   - if the indexes were out of range
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_DXCH_TCAM_MANAGER_MOVE_TO_ANYWHERE_FUNC)
(
    IN  GT_VOID         *clientCookiePtr
);

/*******************************************************************************
* CPSS_DXCH_TCAM_MANAGER_CHECK_IF_DEFAULT_LOCATION_FUNC
*
* DESCRIPTION:
*       Client callback function used by the TCAM manager to request the client
*       to find if an entry from TCAM is the default entry location
*
* INPUTS:
*       clientCookiePtr   - points to the client cookie associated with the entry
*
* OUTPUTS:
*       isDefaultEntryLocationPtr - (pointer to)
*                                GT_TRUE: clientCookiePtr is a default entry
*                                GT_FALSE:clientCookiePtr is not a default entry
*
* RETURNS:
*       GT_OK             - if allocation succedded
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_DXCH_TCAM_MANAGER_CHECK_IF_DEFAULT_LOCATION_FUNC)
(
    IN  GT_VOID         *clientCookiePtr,
    OUT GT_BOOL         *isDefaultEntryLocationPtr
);

/*
 * typedef: enum CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT
 *
 * Description:
 *      TCAM types that can be managed by the TCAM manager.
 *
 * Enumerations:
 *      CPSS_DXCH_TCAM_MANAGER_XCAT_AND_ABOVE_ROUTER_TCAM_E
 *                    - xCat and above router TCAM
 *                      Has 4 columns per line, row based search order,
 *      CPSS_DXCH_TCAM_MANAGER_XCAT_AND_ABOVE_PCL_TCAM_E
 *                    - xCat and above PCL TCAM
 *                      Has 4 columns per line, row based search order,
 */
typedef enum
{
    CPSS_DXCH_TCAM_MANAGER_XCAT_AND_ABOVE_ROUTER_TCAM_E = 0,
    CPSS_DXCH_TCAM_MANAGER_XCAT_AND_ABOVE_PCL_TCAM_E    = 1

} CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT;

/*
 * typedef: struct CPSS_DXCH_TCAM_MANAGER_RANGE_STC
 *
 * Description:
 *      Holds physical TCAM range.
 *
 * Fields:
 *      firstLine     - first line;             range 0..(2^18-1)
 *      lastLine      - last line (including);  range 0..(2^18-1)
 */
typedef struct CPSS_DXCH_TCAM_MANAGER_RANGE_STCT
{
    GT_U32  firstLine;
    GT_U32  lastLine;

} CPSS_DXCH_TCAM_MANAGER_RANGE_STC;

/*
 * typedef: enum CPSS_DXCH_TCAM_MANAGER_RANGE_UPDATE_METHOD_ENT
 *
 * Description:
 *      Defines how to rearrange the already allocated entries when updating
 *      the TCAM range.
 *
 * Enumerations:
 *      CPSS_DXCH_TCAM_MANAGER_DO_NOT_MOVE_RANGE_UPDATE_METHOD_E
 *                    - Don't move existing entries at all.
 *      CPSS_DXCH_TCAM_MANAGER_MOVE_TOP_AND_COMPRESS_RANGE_UPDATE_METHOD_E
 *                    - Align entries to the top of the new range and compress
 *      CPSS_DXCH_TCAM_MANAGER_MOVE_BOTTOM_AND_COMPRESS_RANGE_UPDATE_METHOD_E
 *                    - Align entries to bottom of the new range and compress
 *      CPSS_DXCH_TCAM_MANAGER_MOVE_MIDDLE_AND_COMPRESS_RANGE_UPDATE_METHOD_E
 *                    - Align entries to middle of the new range and compress
 *                      around the middle
 *      CPSS_DXCH_TCAM_MANAGER_MOVE_TOP_AND_KEEP_OFFSETS_RANGE_UPDATE_METHOD_E
 *                    - Keeps the exact offsets from the top of the range, meaning
 *                      that for each entry, the offset from the top of the new
 *                      range will be the same as the offset form the top of the
 *                      old range.
 */
typedef enum
{
    CPSS_DXCH_TCAM_MANAGER_DO_NOT_MOVE_RANGE_UPDATE_METHOD_E                = 0,
    CPSS_DXCH_TCAM_MANAGER_MOVE_TOP_AND_COMPRESS_RANGE_UPDATE_METHOD_E      = 1,
    CPSS_DXCH_TCAM_MANAGER_MOVE_BOTTOM_AND_COMPRESS_RANGE_UPDATE_METHOD_E   = 2,
    CPSS_DXCH_TCAM_MANAGER_MOVE_MIDDLE_AND_COMPRESS_RANGE_UPDATE_METHOD_E   = 3,
    CPSS_DXCH_TCAM_MANAGER_MOVE_TOP_AND_KEEP_OFFSETS_RANGE_UPDATE_METHOD_E  = 4

} CPSS_DXCH_TCAM_MANAGER_RANGE_UPDATE_METHOD_ENT;

/*
 * typedef: enum CPSS_DXCH_TCAM_MANAGER_CLIENT_TYPE_ENT
 *
 * Description:
 *      Client type.
 *
 * Enumerations:
 *      CPSS_DXCH_TCAM_MANAGER_STATIC_ENTRIES_RESERVATION_E
 *                    - Static entries reservation.
 *      CPSS_DXCH_TCAM_MANAGER_DYNAMIC_ENTRIES_RESERVATION_E
 *                    - Dynamic entries reservation.
 */
typedef enum
{
    CPSS_DXCH_TCAM_MANAGER_STATIC_ENTRIES_RESERVATION_E     = 0,
    CPSS_DXCH_TCAM_MANAGER_DYNAMIC_ENTRIES_RESERVATION_E    = 1

} CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT;

/*
 * typedef: struct CPSS_DXCH_TCAM_MANAGER_CLIENT_FUNC_STC
 *
 * Description:
 *      Callback functions supplied by clients of the TCAM manager and used by
 *      the TCAM manager to manipulate entries.
 *
 * Fields:
 *      moveToLocationFuncPtr - callback function to move single entry from
 *                              one TCAM location to another TCAM location
 *                              (destination location is choosen by TCAM
 *                              manager)
 *      moveToAnywhereFuncPtr - callback function to move single entry
 *                              (destination location is choosen by the client)
 *      checkIfDefaultLocationFuncPtr -  callback function to check if a single
 *                               entry is a default entry located in default location
 *
 */
typedef struct CPSS_DXCH_TCAM_MANAGER_CLIENT_FUNC_STCT
{
    CPSS_DXCH_TCAM_MANAGER_MOVE_TO_LOCATION_FUNC            moveToLocationFuncPtr;
    CPSS_DXCH_TCAM_MANAGER_MOVE_TO_ANYWHERE_FUNC            moveToAnywhereFuncPtr;
    CPSS_DXCH_TCAM_MANAGER_CHECK_IF_DEFAULT_LOCATION_FUNC   checkIfDefaultLocationFuncPtr;
} CPSS_DXCH_TCAM_MANAGER_CLIENT_FUNC_STC;

/*
 * typedef: enum CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT
 *
 * Description:
 *      Entry types that can be allocated by the TCAM manager.
 *
 * Enumerations:
 *      CPSS_DXCH_TCAM_MANAGER_SINGLE_ENTRY_E - single entry
 *      CPSS_DXCH_TCAM_MANAGER_DUAL_ENTRY_E   - dual entry aligned to even column
 *      CPSS_DXCH_TCAM_MANAGER_TRIPLE_ENTRY_E - triple entry aligned to row start
 *      CPSS_DXCH_TCAM_MANAGER_QUAD_ENTRY_E   - quad entry aligned to row start
 */
typedef enum
{
    CPSS_DXCH_TCAM_MANAGER_SINGLE_ENTRY_E       = 0,
    CPSS_DXCH_TCAM_MANAGER_DUAL_ENTRY_E         = 1,
    CPSS_DXCH_TCAM_MANAGER_TRIPLE_ENTRY_E       = 2,
    CPSS_DXCH_TCAM_MANAGER_QUAD_ENTRY_E         = 3

} CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT;

/*
 * typedef: enum CPSS_DXCH_TCAM_MANAGER_ALLOC_METHOD_ENT
 *
 * Description:
 *      TCAM location allocation method.
 *
 * Enumerations:
 *      CPSS_DXCH_TCAM_MANAGER_ALLOC_MIN_E    - allocate location as close as possible
 *                                              to the lower boundary
 *      CPSS_DXCH_TCAM_MANAGER_ALLOC_MID_E    - allocate location near the middle
 *                                              of the range between upper and lower
 *                                              boundaries
 *      CPSS_DXCH_TCAM_MANAGER_ALLOC_MAX_E    - allocate location as close as possible
 *                                              to the upper boundary
 */
typedef enum
{
    CPSS_DXCH_TCAM_MANAGER_ALLOC_MIN_E      = 0,
    CPSS_DXCH_TCAM_MANAGER_ALLOC_MID_E      = 1,
    CPSS_DXCH_TCAM_MANAGER_ALLOC_MAX_E      = 2

} CPSS_DXCH_TCAM_MANAGER_ALLOC_METHOD_ENT;

/*
 * typedef: struct CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC
 *
 * Description:
 *      Holds amount of specific entry type.
 *
 * Fields:
 *      entryType     - entry type
 *      amount        - number of entries of that type; range 0..(2^20-1)
 *
 */
typedef struct CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STCT
{
    CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT   entryType;
    GT_U32                                  amount;

} CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC;

/*
 * typedef: enum CPSS_DXCH_TCAM_MANAGER_DUMP_TYPE_ENT
 *
 * Description:
 *      Type of data dump.
 *
 * Enumerations:
 *      CPSS_DXCH_TCAM_MANAGER_DUMP_CLIENTS_E
 *                    - dump the list of clients currently registered to
 *                      the TCAM manager
 *      CPSS_DXCH_TCAM_MANAGER_DUMP_ENTRIES_FOR_SPECIFIC_CLIENT_E
 *                    - dump list of entries allocated for a specific client
 *                      in the TCAM manager
 *      CPSS_DXCH_TCAM_MANAGER_DUMP_ENTRIES_FOR_ALL_CLIENTS_E
 *                    - dump list of entries allocated for all clients that
 *                      are currently registered in the TCAM manager
 *      CPSS_DXCH_TCAM_MANAGER_DUMP_CLIENTS_AND_ENTRIES_FOR_ALL_CLIENTS_E
 *                    - dump the list of clients surrently registered and also
 *                      the list of entries allocated for all those clients
 *      CPSS_DXCH_TCAM_MANAGER_DUMP_INTERNAL_INFO_E
 *                    - dump the content of internal data structures
 */
typedef enum
{
    CPSS_DXCH_TCAM_MANAGER_DUMP_CLIENTS_E                               = 0,
    CPSS_DXCH_TCAM_MANAGER_DUMP_ENTRIES_FOR_SPECIFIC_CLIENT_E           = 1,
    CPSS_DXCH_TCAM_MANAGER_DUMP_ENTRIES_FOR_ALL_CLIENTS_E               = 2,
    CPSS_DXCH_TCAM_MANAGER_DUMP_CLIENTS_AND_ENTRIES_FOR_ALL_CLIENTS_E   = 3,
    CPSS_DXCH_TCAM_MANAGER_DUMP_INTERNAL_INFO_E                         = 4

} CPSS_DXCH_TCAM_MANAGER_DUMP_TYPE_ENT;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChTcamManagerTypesh */
