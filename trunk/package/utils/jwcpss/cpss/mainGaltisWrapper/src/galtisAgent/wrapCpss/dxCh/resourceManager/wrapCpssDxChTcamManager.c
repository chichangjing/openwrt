/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxChTcamManager.c
*
* DESCRIPTION:
*       Wrapper functions for tcam cpss functions (dxChx).
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/resourceManager/cpssDxChTcamManager.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamCommonTypes.h>
#include <cpssCommon/private/prvCpssSkipList.h>

/* LPM DB skip list (used to get TCAM manager handler that is used by the LPM DB) */
extern GT_VOID      *prvCpssDxChIplpmDbSL;

/*Max tokens callback array size*/
#define TCAM_MAX_TOKEN_CALLBACK_ARR_SIZE_CNS  20

/*Cause there is 4 types of entries and it can't be repeated
  (see CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT description) */
#define TCAM_MAX_ENTRY_TYPES_SIZE_CNS  4

/*Store tcam manager handler pointer*/
static GT_VOID *prvTcamManagerHandlerPtr = NULL;

/*Used to provide callback functions*/
static CPSS_DXCH_TCAM_MANAGER_CLIENT_FUNC_STC prvTcamClientFunc;

/*Array with client Id's*/
static GT_U32 prvTcamClientIdArray[CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS];

/*Struct with token info*/
typedef struct
{
    GT_BOOL     isUsed;
    GT_U32      tokenEntry;
    GT_U32      clientId;
} PRV_TCAM_TOKEN_INFO_STC;

/*Array with tokens info*/
static PRV_TCAM_TOKEN_INFO_STC* prvTcamTokenArray = NULL;

/*Current size of array with tokens*/
static GT_U32 prvTokensArrSize = 0;

/*Additional function to register clients*/
static GT_STATUS prvClientMoveToLocation
(
    IN  CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC    *fromPtr,
    IN  CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC    *toPtr,
    IN  GT_VOID                                     *clientCookiePtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(fromPtr);
    CPSS_NULL_PTR_CHECK_MAC(toPtr);

    clientCookiePtr = clientCookiePtr;

    return GT_OK;
}


/*Additional function to register clients*/
static GT_STATUS prvClientMoveToAnywhere
(
    IN  GT_VOID         *clientCookiePtr
)
{
    GT_STATUS   status;
    GT_U32      tokenIdx;

    CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT entryType = CPSS_DXCH_TCAM_MANAGER_SINGLE_ENTRY_E;
    CPSS_DXCH_TCAM_MANAGER_ALLOC_METHOD_ENT allocMethod = CPSS_DXCH_TCAM_MANAGER_ALLOC_MIN_E;
    GT_BOOL  defragEnable = GT_FALSE;

    /* check input parameters */
    CPSS_NULL_PTR_CHECK_MAC(clientCookiePtr);

    /* get token index */
    tokenIdx = *((GT_U32*) clientCookiePtr);

    /* call allocate function */
    status = cpssDxChTcamManagerEntryAllocate(prvTcamManagerHandlerPtr,
                                              prvTcamTokenArray[tokenIdx].clientId,
                                              entryType,
                                              0, 0,  /* allocation with no boundaries */
                                              allocMethod, defragEnable, clientCookiePtr,
                                             &prvTcamTokenArray[tokenIdx].tokenEntry);

    return status;
}

/*Additional function to register clients*/
static GT_STATUS prvClientCheckIfDefaultLocation
(
    IN  GT_VOID         *clientCookiePtr,
    OUT GT_BOOL         *isDefaultEntryLocationPtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(clientCookiePtr);
    CPSS_NULL_PTR_CHECK_MAC(isDefaultEntryLocationPtr);

    isDefaultEntryLocationPtr = isDefaultEntryLocationPtr;

    return GT_OK;
}


/*******************************************************************************
* prvTcamPrepareTokenArray
*
* DESCRIPTION:
*       Prepare tcam token array (dynamically extended)
*
* INPUTS:
*       entryTokenIdx  - index needed to operate
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_NO_RESOURCE        - if no CPU memory for memory allocation
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvTcamPrepareTokenArray
(
    IN  GT_8   outArgs[CMD_MAX_BUFFER],
    IN  GT_U32 entryTokenIdx
)
{
    /* prepare tokens array (if nessesary) */
    if(entryTokenIdx >= prvTokensArrSize)
    {
        /* allocate memory first */
        if(prvTcamTokenArray == NULL)
        {
            prvTcamTokenArray = (PRV_TCAM_TOKEN_INFO_STC*)
                                cmdOsMalloc(sizeof(PRV_TCAM_TOKEN_INFO_STC));
        }

        if(prvTcamTokenArray == NULL)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return GT_NO_RESOURCE;
        }

        /* extend array */
        prvTcamTokenArray = (PRV_TCAM_TOKEN_INFO_STC*) cmdOsRealloc(prvTcamTokenArray,
                                        sizeof(PRV_TCAM_TOKEN_INFO_STC) * entryTokenIdx);

        if(prvTcamTokenArray == NULL)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return GT_NO_RESOURCE;
        }

        /* clear extended part of array */
        cmdOsMemSet(prvTcamTokenArray + (sizeof(PRV_TCAM_TOKEN_INFO_STC) * prvTokensArrSize),
                0, sizeof(PRV_TCAM_TOKEN_INFO_STC) * (entryTokenIdx - prvTokensArrSize));

        /* store new size of array */
        prvTokensArrSize = entryTokenIdx;
    }

    return GT_OK;
}


/*******************************************************************************
* cpssDxChTcamManagerCreate
*
* DESCRIPTION:
*       Creates TCAM manager and returns handler for the created TCAM manager.
*
* INPUTS:
*       tcamType              - the TCAM type that the TCAM manager will handle
*       rangePtr              - the TCAM range allocated for this TCAM manager
*
* OUTPUTS:
*       prvTcamManagerHandlerPtr  - points to pointer of the created TCAM manager object
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_OUT_OF_CPU_MEM     - if no CPU memory for memory allocation
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTcamManagerCreate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   status;
    GT_U32      tokensMax = 0;

    CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT    tcamType;
    CPSS_DXCH_TCAM_MANAGER_RANGE_STC        range;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(prvTcamManagerHandlerPtr != NULL)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Tcam manager already created  %d", -1);
        return CMD_OK;
    }

    /*map input locals values*/
    tcamType        = (CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT)inArgs[0];
    range.firstLine = (GT_U32)inArgs[1];
    range.lastLine  = (GT_U32)inArgs[2];

    /*create tcam manager*/
    status = cpssDxChTcamManagerCreate(tcamType, &range, &prvTcamManagerHandlerPtr);

    /* check is call failed */
    if (GT_OK != status)
    {
        galtisOutput(outArgs, status, "%d", -1);
        return CMD_OK;
    }

    /*calculate maximum tokens supported*/
    tokensMax = (range.lastLine - range.firstLine + 1) * 4;

    /*prepare token array*/
    status = prvTcamPrepareTokenArray(outArgs, tokensMax);
    if (GT_OK != status)
    {
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerConfigGet
*
* DESCRIPTION:
*       Gets the configuration of the TCAM manager.
*
* INPUTS:
*       prvTcamManagerHandlerPtr - the TCAM manager handler
*
* OUTPUTS:
*       tcamTypePtr           - points to TCAM type managed by the TCAM manager
*       rangePtr              - points to the TCAM range managed by the TCAM manager
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTcamManagerConfigGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   status;

    CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT    tcamType;
    CPSS_DXCH_TCAM_MANAGER_RANGE_STC        range;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* check is tcam manager created */
    if(NULL == prvTcamManagerHandlerPtr)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Tcam manager was not created yet  %d", -1);
        return CMD_OK;
    }

    /* Call cpss api function. Note that the tcam manager must be already created*/
    status = cpssDxChTcamManagerConfigGet(prvTcamManagerHandlerPtr, &tcamType, &range);

    /* check is call failed */
    if (GT_OK != status)
    {
        galtisOutput(outArgs, status, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "%d%d%d", tcamType, range.firstLine, range.lastLine);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerDelete
*
* DESCRIPTION:
*       Deletes the TCAM manager.
*
* INPUTS:
*       prvTcamManagerHandlerPtr - the TCAM manager handler
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_FAIL               - on error
*
* COMMENTS:
*       All entries allocated by the TCAM manager must be freed prior of calling
*       this API. Clients may be registered upon calling the API.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTcamManagerDelete
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   status;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* check is tcam manager created */
    if(NULL == prvTcamManagerHandlerPtr)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Tcam manager was not created yet  %d", -1);
        return CMD_OK;
    }

    /* Call cpss api function. Note that the tcam manager must be already created*/
    status = cpssDxChTcamManagerDelete(prvTcamManagerHandlerPtr);

    /* delete all tokens from array */
    if(GT_OK == status)
    {
        cpssOsFree(prvTcamTokenArray);
        prvTokensArrSize = 0;
    }

    /* check is call failed */
    if (GT_OK != status)
    {
        galtisOutput(outArgs, status, "%d", -1);
        return CMD_OK;
    }

    prvTcamManagerHandlerPtr = NULL;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerRangeUpdate
*
* DESCRIPTION:
*       Updates the range allocated for the TCAM manager.
*
* INPUTS:
*       prvTcamManagerHandlerPtr - the TCAM manager handler
*       rangePtr              - the new TCAM range allocated for this TCAM manager
*       rangeUpdateMethod     - update method to use during the range update
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_FAIL               - on error
*
* COMMENTS:
*       1. Since the ranges for static elients are explicitly stated by the clients,
*          then static entries will never be moved (regardless of the range update
*          method). If any static entry (either allocated or reserved by static
*          client) is outside the new range, the function will fail.
*       2. In case of smaller new range, the new range must be big enough to
*          hold all current allocated and reserved entries.
*       3. In CPSS_DXCH_TCAM_MANAGER_DO_NOT_MOVE_RANGE_UPDATE_METHOD_E update
*          method, if any of existing entries is outside of the new range,
*          the API will fail.
*       4. In CPSS_DXCH_TCAM_MANAGER_MOVE_TOP_AND_KEEP_OFFSETS_RANGE_UPDATE_METHOD_E
*          update method, if any of the existing entries will not fit in the new
*          range then the API will fail.
*       5. Since TCAM manager keeps ordering between same type of entries, then
*          upon range update, ordering between entries of different types may
*          be changed.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTcamManagerRangeUpdate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS status;
    GT_U32    tokensMax = 0;

    CPSS_DXCH_TCAM_MANAGER_RANGE_STC range;
    CPSS_DXCH_TCAM_MANAGER_RANGE_UPDATE_METHOD_ENT rangeUpdateMethod;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* check is tcam manager created */
    if(NULL == prvTcamManagerHandlerPtr)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Tcam manager was not created yet  %d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    rangeUpdateMethod = (CPSS_DXCH_TCAM_MANAGER_RANGE_UPDATE_METHOD_ENT)inArgs[0];
    range.firstLine   = (GT_U32)inArgs[1];
    range.lastLine    = (GT_U32)inArgs[2];

    /* call cpss api function */
    status = cpssDxChTcamManagerRangeUpdate(prvTcamManagerHandlerPtr,
                                            &range, rangeUpdateMethod);

    /* check is call failed */
    if (GT_OK != status)
    {
        galtisOutput(outArgs, status, "%d", -1);
        return CMD_OK;
    }

    /*calculate maximum tokens supported*/
    tokensMax = (range.lastLine - range.firstLine + 1) * 4;

    /*prepare token array*/
    status = prvTcamPrepareTokenArray(outArgs, tokensMax);
    if (GT_OK != status)
    {
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerClientRegister
*
* DESCRIPTION:
*       Register a client in the TCAM manager.
*
* INPUTS:
*       prvTcamManagerHandlerPtr - the TCAM manager handler
*       clientName            - string that represent the client
*       prvTcamClientFuncPtr         - points to callback functions supplied by the client
*                               move callback function is mandatory
*                               relocate callback function is optional
*
* OUTPUTS:
*       clientIdPtr           - points to client Id allocated by the TCAM manager
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_FAIL               - on error
*
* COMMENTS:
*       1. Up to CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS clients is supported.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTcamManagerClientRegister
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS status;
    GT_CHAR   clientName[CPSS_DXCH_TCAM_MANAGER_CLIENT_NAME_NAX_LENGTH_CNS];

    GT_U32 clientIdIdx; /*index client Id in clients Id array*/

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* check is tcam manager created */
    if(NULL == prvTcamManagerHandlerPtr)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Tcam manager was not created yet  %d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    clientIdIdx = (GT_U32)inArgs[0];

    cpssOsMemCpy(clientName, (GT_CHAR*)inArgs[1],
                 CPSS_DXCH_TCAM_MANAGER_CLIENT_NAME_NAX_LENGTH_CNS);

    /* check user input index range */
    if (clientIdIdx >= CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Wrong client index (range 0-15)");
        return CMD_OK;
    }

    /*point func to register client*/
    prvTcamClientFunc.moveToLocationFuncPtr = prvClientMoveToLocation;
    prvTcamClientFunc.moveToAnywhereFuncPtr = prvClientMoveToAnywhere;
    prvTcamClientFunc.checkIfDefaultLocationFuncPtr = prvClientCheckIfDefaultLocation;

    /* call cpss api function */
    status = cpssDxChTcamManagerClientRegister(prvTcamManagerHandlerPtr,
         clientName, &prvTcamClientFunc, &prvTcamClientIdArray[clientIdIdx]);

    /* check is call failed */
    if (GT_OK != status)
    {
        galtisOutput(outArgs, status, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "%d", prvTcamClientIdArray[clientIdIdx]);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerClientUnregister
*
* DESCRIPTION:
*       Unregister a client in the TCAM manager.
*
* INPUTS:
*       prvTcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_NO_SUCH            - the clientId is not registered
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTcamManagerClientUnregister
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS status;
    GT_U32 clientIdIdx; /*index client Id in clients ID array*/

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* check is tcam manager created */
    if(NULL == prvTcamManagerHandlerPtr)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Tcam manager was not created yet  %d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    clientIdIdx = (GT_U32)inArgs[0];

    /* check user input index range */
    if (clientIdIdx >= CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Wrong client index (range 0-15)");
        return CMD_OK;
    }

    /* call cpss api function */
    status = cpssDxChTcamManagerClientUnregister(prvTcamManagerHandlerPtr,
                              prvTcamClientIdArray[clientIdIdx]);

    /* check is call failed */
    if (GT_OK != status)
    {
        galtisOutput(outArgs, status, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/**************cpssDxChTcamManagerEntriesReservationSet table *****************/
/*index for requested entries array */
static GT_U32 prvTcamRequestedEntriesIdx = 0;

/*requested entries array */
static CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC
                    prvTcamRequestedEntriesArray[TCAM_MAX_ENTRY_TYPES_SIZE_CNS];

/*******************************************************************************
* cpssDxChTcamManagerEntriesReservationSet
*
* DESCRIPTION:
*       Reserves entries for a client in the TCAM manager.
*
* INPUTS:
*       prvTcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       reservationType       - type of the reservation
*       prvTcamRequestedEntriesArray - array of amount of entries requested by client
*       sizeOfArray           - number of elemenets in the array
*       reservationRangePtr   - the TCAM range allocated for this reservation;
*                               relevant only for static reservation type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_NO_SUCH            - the clientId is not registered
*       GT_FAIL               - on error
*
* COMMENTS:
*       1. The TCAM manager will save enough space so that at any given time,
*          the client will have enough space for up to the number of reserved
*          entries.
*       2. In case of static entries reservation, the TCAM range allocated to
*          the static reservation must be within the range allocated to the
*          TCAM manager. The requested range can't contain entries reserved for
*          previous static reservation.
*       3. Each client can have only one reservation. New reservation for a
*          client will replace the previous reservation.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTcamManagerEntriesReservationSetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS status = GT_OK;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* check is tcam manager created */
    if(NULL == prvTcamManagerHandlerPtr)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Tcam manager was not created yet  %d", -1);
        return CMD_OK;
    }

    /* reset array */
    cmdOsMemSet(prvTcamRequestedEntriesArray, 0, sizeof(prvTcamRequestedEntriesArray));

    /* reset index first */
    prvTcamRequestedEntriesIdx = 0;

    /* map input arguments to locals */

    prvTcamRequestedEntriesArray[prvTcamRequestedEntriesIdx].entryType =
                            (CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT)inFields[0];
    prvTcamRequestedEntriesArray[prvTcamRequestedEntriesIdx].amount    = (GT_U32)inFields[1];

    prvTcamRequestedEntriesIdx++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerEntriesReservationSet
*
* DESCRIPTION:
*       Reserves entries for a client in the TCAM manager.
*
* INPUTS:
*       prvTcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       reservationType       - type of the reservation
*       prvTcamRequestedEntriesArray - array of amount of entries requested by client
*       sizeOfArray           - number of elemenets in the array
*       reservationRangePtr   - the TCAM range allocated for this reservation;
*                               relevant only for static reservation type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_NO_SUCH            - the clientId is not registered
*       GT_FAIL               - on error
*
* COMMENTS:
*       1. The TCAM manager will save enough space so that at any given time,
*          the client will have enough space for up to the number of reserved
*          entries.
*       2. In case of static entries reservation, the TCAM range allocated to
*          the static reservation must be within the range allocated to the
*          TCAM manager. The requested range can't contain entries reserved for
*          previous static reservation.
*       3. Each client can have only one reservation. New reservation for a
*          client will replace the previous reservation.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTcamManagerEntriesReservationSetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS status = GT_OK;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    prvTcamRequestedEntriesArray[prvTcamRequestedEntriesIdx].entryType =
                            (CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT)inFields[0];
    prvTcamRequestedEntriesArray[prvTcamRequestedEntriesIdx].amount    = (GT_U32)inFields[1];

    prvTcamRequestedEntriesIdx++;

    /* check the end of the table */
    if (prvTcamRequestedEntriesIdx >= TCAM_MAX_ENTRY_TYPES_SIZE_CNS)
    {
        galtisOutput(outArgs, status, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerEntriesReservationSet
*
* DESCRIPTION:
*       Reserves entries for a client in the TCAM manager.
*
* INPUTS:
*       prvTcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       reservationType       - type of the reservation
*       prvTcamRequestedEntriesArray - array of amount of entries requested by client
*       sizeOfArray           - number of elemenets in the array
*       reservationRangePtr   - the TCAM range allocated for this reservation;
*                               relevant only for static reservation type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_NO_SUCH            - the clientId is not registered
*       GT_FAIL               - on error
*
* COMMENTS:
*       1. The TCAM manager will save enough space so that at any given time,
*          the client will have enough space for up to the number of reserved
*          entries.
*       2. In case of static entries reservation, the TCAM range allocated to
*          the static reservation must be within the range allocated to the
*          TCAM manager. The requested range can't contain entries reserved for
*          previous static reservation.
*       3. Each client can have only one reservation. New reservation for a
*          client will replace the previous reservation.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTcamManagerEntriesReservationEndSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS status;

    GT_U32 clientIdIdx;
    CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT reservationType;
    GT_U32 sizeOfArray = TCAM_MAX_ENTRY_TYPES_SIZE_CNS;
    CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC
                prvTcamRequestedEntriesArray[TCAM_MAX_ENTRY_TYPES_SIZE_CNS];
    CPSS_DXCH_TCAM_MANAGER_RANGE_STC reservationRange;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    clientIdIdx = (GT_U32)inArgs[0];

    /* check user input index range */
    if (clientIdIdx >= CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Wrong client index (range 0-15)");
        return CMD_OK;
    }

    reservationType            = (CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT)inArgs[1];
    sizeOfArray                = prvTcamRequestedEntriesIdx;
    reservationRange.firstLine = (GT_U32)inArgs[2];
    reservationRange.lastLine  = (GT_U32)inArgs[3];

    /* call cpss api function */
    status = cpssDxChTcamManagerEntriesReservationSet(prvTcamManagerHandlerPtr,
                                prvTcamClientIdArray[clientIdIdx], reservationType,
                                prvTcamRequestedEntriesArray, sizeOfArray, &reservationRange);

    /* check is call failed */
    if (GT_OK != status)
    {
        galtisOutput(outArgs, status, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/**************cpssDxChTcamManagerEntriesReservationGet table *****************/
/* index for reserved entries array */
static GT_U32 prvTcamReservedEntriesIdx = 0;

/* reserved entries array */
static CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC
                    prvTcamReservedEntriesArray[TCAM_MAX_ENTRY_TYPES_SIZE_CNS];

/* reserved entries array size */
static GT_U32 prvSizeOfArray = TCAM_MAX_ENTRY_TYPES_SIZE_CNS;

/*******************************************************************************
* cpssDxChTcamManagerEntriesReservationGet
*
* DESCRIPTION:
*       Gets number of reserved entries for a client in the TCAM manager.
*
* INPUTS:
*       prvTcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       sizeOfArrayPtr        - points to the allocated size of the array
*
* OUTPUTS:
*       reservationTypePtr    - points to reservation type
*       prvTcamReservedEntriesArray  - array of amount of entries reserved to client
*       sizeOfArrayPtr        - points to number of elements filled in the array
*       reservationRangePtr   - points to the reservation range (relevant only
*                               for static entries reservation)
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_NO_SUCH            - the clientId is not registered
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTcamManagerEntriesReservationGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS status;

    GT_U32 clientIdIdx;

    CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT reservationType;
    CPSS_DXCH_TCAM_MANAGER_RANGE_STC reservationRange;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* check is tcam manager created */
    if(NULL == prvTcamManagerHandlerPtr)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Tcam manager was not created yet  %d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    clientIdIdx = (GT_U32)inArgs[0];

    /* check user input index range */
    if (clientIdIdx >= CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Wrong client index (range 0-15)");
        return CMD_OK;
    }

    /* reset index first */
    prvTcamReservedEntriesIdx = 0;

    /* set array size to get all entries */
    prvSizeOfArray = TCAM_MAX_ENTRY_TYPES_SIZE_CNS;

    /* call cpss api function */
    status = cpssDxChTcamManagerEntriesReservationGet(prvTcamManagerHandlerPtr,
                            prvTcamClientIdArray[clientIdIdx], &reservationType,
                            prvTcamReservedEntriesArray, &prvSizeOfArray, &reservationRange);

    /* check is call failed */
    if (GT_OK != status)
    {
        galtisOutput(outArgs, status, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = prvTcamReservedEntriesArray[prvTcamReservedEntriesIdx].entryType;
    inFields[1] = prvTcamReservedEntriesArray[prvTcamReservedEntriesIdx].amount;

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    prvTcamReservedEntriesIdx++;

    galtisOutput(outArgs, GT_OK, "%f%d%d%d%d", reservationType, prvSizeOfArray,
                reservationRange.firstLine, reservationRange.lastLine);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerEntriesReservationGet
*
* DESCRIPTION:
*       Gets number of reserved entries for a client in the TCAM manager.
*
* INPUTS:
*       prvTcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       sizeOfArrayPtr        - points to the allocated size of the array
*
* OUTPUTS:
*       reservationTypePtr    - points to reservation type
*       prvTcamReservedEntriesArray  - array of amount of entries reserved to client
*       sizeOfArrayPtr        - points to number of elements filled in the array
*       reservationRangePtr   - points to the reservation range (relevant only
*                               for static entries reservation)
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_NO_SUCH            - the clientId is not registered
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTcamManagerEntriesReservationGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    inFields[0] = prvTcamReservedEntriesArray[prvTcamReservedEntriesIdx].entryType;
    inFields[1] = prvTcamReservedEntriesArray[prvTcamReservedEntriesIdx].amount;

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    prvTcamReservedEntriesIdx++;

    /* check the end of the table */
    if (prvTcamReservedEntriesIdx >= prvSizeOfArray)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/***************  cpssDxChTcamManagerUsageGet table  **************************/
/*array with allocated entries*/
static CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC
                    prvAllocatedEntriesArray[TCAM_MAX_ENTRY_TYPES_SIZE_CNS];

/*size of array with reserved entries*/
static GT_U32 prvSizeOfReservedArray  = 0;

/*size of array with allocated entries*/
static GT_U32 prvSizeOfAllocatedArray = 0;

/*entries index*/
static GT_U32 prvTcamEntriesIdx = 0;

/*******************************************************************************
* cpssDxChTcamManagerUsageGet
*
* DESCRIPTION:
*       Returns the current number of entries from each type used by all
*       clients in the TCAM manager.
*
* INPUTS:
*       prvTcamManagerHandlerPtr - the TCAM manager handler
*       usageOfSpecificClient - GT_TRUE:  get usage of specific client
*                               GT_FALSE: get commulative usage of all clients
*       clientId              - the client Id
*                               relevant if usageOfSpecificClient is GT_TRUE
*       sizeOfReservedArrayPtr    - points to the allocated size of the
*                                   reserved entries array
*       sizeofAllocatedArrayPtr   - points to the allocated size of the
*                                   allocated entries array
*
* OUTPUTS:
*       prvTcamReservedEntriesArray      - array of amount of entries reserved
*       sizeOfReservedArrayPtr    - points to the allocated size of the
*                                   reserved entries array
*       allocatedEntriesArray     - array of amount of entries allocated
*       sizeofAllocatedArrayPtr   - points to the allocated size of the
*                                   allocated entries array
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_NO_SUCH            - the clientId is not registered
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTcamManagerUsageGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS status;

    GT_BOOL usageOfSpecificClient;
    GT_U32 clientIdIdx;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* check is tcam manager created */
    if(NULL == prvTcamManagerHandlerPtr)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Tcam manager was not created yet  %d", -1);
        return CMD_OK;
    }

    /* reset index on first */
    prvTcamEntriesIdx = 0;

    prvSizeOfReservedArray = TCAM_MAX_ENTRY_TYPES_SIZE_CNS;
    prvSizeOfAllocatedArray = TCAM_MAX_ENTRY_TYPES_SIZE_CNS;

    /* map input arguments to locals */
    usageOfSpecificClient = (GT_BOOL)inArgs[0];
    clientIdIdx           = (GT_U32)inArgs[1];

    /* check user input index range */
    if (clientIdIdx >= CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Wrong client index (range 0-15)");
        return CMD_OK;
    }

    /* call cpss api function */
    status = cpssDxChTcamManagerUsageGet(prvTcamManagerHandlerPtr,
                    usageOfSpecificClient, prvTcamClientIdArray[clientIdIdx],
                    prvTcamReservedEntriesArray, &prvSizeOfReservedArray,
                    prvAllocatedEntriesArray, &prvSizeOfAllocatedArray);

    /* check is call failed */
    if (GT_OK != status)
    {
        galtisOutput(outArgs, status, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = prvTcamReservedEntriesArray[prvTcamEntriesIdx].entryType;
    inFields[1] = prvTcamReservedEntriesArray[prvTcamEntriesIdx].amount;
    inFields[2] = prvAllocatedEntriesArray[prvTcamEntriesIdx].entryType;
    inFields[3] = prvAllocatedEntriesArray[prvTcamEntriesIdx].amount;

    prvTcamEntriesIdx++;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    galtisOutput(outArgs, GT_OK, "%f%d%d", prvSizeOfReservedArray, prvSizeOfAllocatedArray);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerUsageGet
*
* DESCRIPTION:
*       Returns the current number of entries from each type used by all
*       clients in the TCAM manager.
*
* INPUTS:
*       prvTcamManagerHandlerPtr - the TCAM manager handler
*       usageOfSpecificClient - GT_TRUE:  get usage of specific client
*                               GT_FALSE: get commulative usage of all clients
*       clientId              - the client Id
*                               relevant if usageOfSpecificClient is GT_TRUE
*       sizeOfReservedArrayPtr    - points to the allocated size of the
*                                   reserved entries array
*       sizeofAllocatedArrayPtr   - points to the allocated size of the
*                                   allocated entries array
*
* OUTPUTS:
*       prvTcamReservedEntriesArray      - array of amount of entries reserved
*       sizeOfReservedArrayPtr    - points to the allocated size of the
*                                   reserved entries array
*       allocatedEntriesArray     - array of amount of entries allocated
*       sizeofAllocatedArrayPtr   - points to the allocated size of the
*                                   allocated entries array
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_NO_SUCH            - the clientId is not registered
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTcamManagerUsageGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS status = GT_OK;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* check the end of the table */
    if ((prvTcamEntriesIdx >= prvSizeOfReservedArray) &&
        (prvTcamEntriesIdx >= prvSizeOfAllocatedArray))
    {
        galtisOutput(outArgs, status, "%d", -1);
        return CMD_OK;
    }

    /*print prvTcamReservedEntriesArray*/
    if (prvTcamEntriesIdx < prvSizeOfReservedArray)
    {
        inFields[0] = prvTcamReservedEntriesArray[prvTcamEntriesIdx].entryType;
        inFields[1] = prvTcamReservedEntriesArray[prvTcamEntriesIdx].amount;
    }

    /*print allocatedEntriesArray*/
    if (prvTcamEntriesIdx < prvSizeOfAllocatedArray)
    {
        inFields[2] = prvAllocatedEntriesArray[prvTcamEntriesIdx].entryType;
        inFields[3] = prvAllocatedEntriesArray[prvTcamEntriesIdx].amount;
    }

    prvTcamEntriesIdx++;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}


/******************* cpssDxChTcamManagerTokensTable **************************/
/*token index */
static GT_U32 prvTokenIdIdx = 0;

/*******************************************************************************/
static CMD_STATUS wrCpssDxChTcamManagerTokenTableGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS status = GT_OK;

    CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC tcamLocation;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* check is tcam manager created */
    if(NULL == prvTcamManagerHandlerPtr)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Tcam manager was not created yet  %d", -1);
        return CMD_OK;
    }

    /* reset index on first */
    prvTokenIdIdx = 0;

    /* find first used token in array */
    while((GT_FALSE == prvTcamTokenArray[prvTokenIdIdx].isUsed)
           && (prvTokenIdIdx <= prvTokensArrSize))
    {
        prvTokenIdIdx++;
    }

    /* check user input index range */
    if (prvTokenIdIdx >= prvTokensArrSize)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "There is no used tokens");
        return CMD_OK;
    }

    /*get tcam location for given token*/
    status = cpssDxChTcamManagerTokenToTcamLocation(prvTcamManagerHandlerPtr,
                                         prvTcamTokenArray[prvTokenIdIdx].clientId,
                                         prvTcamTokenArray[prvTokenIdIdx].tokenEntry,
                                         &tcamLocation);

    /* check is call failed */
    if (GT_OK != status)
    {
        galtisOutput(outArgs, status, "Cannot get token tcam location %d", -1);
        return CMD_OK;
    }

    inFields[0] = prvTcamTokenArray[prvTokenIdIdx].isUsed;
    inFields[1] = prvTokenIdIdx;
    inFields[2] = prvTcamTokenArray[prvTokenIdIdx].tokenEntry;
    inFields[3] = prvTcamTokenArray[prvTokenIdIdx].clientId;
    inFields[4] = tcamLocation.row;
    inFields[5] = tcamLocation.column;

    prvTokenIdIdx++;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                              inFields[3], inFields[4], inFields[5]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssDxChTcamManagerTokenTableGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS status = GT_OK;

    CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC tcamLocation;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* check user input index range */
    if (prvTokenIdIdx >= prvTokensArrSize)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* find used tokens in array */
    if(GT_TRUE == prvTcamTokenArray[prvTokenIdIdx].isUsed)
    {
        /*get tcam location for given token*/
        status = cpssDxChTcamManagerTokenToTcamLocation(prvTcamManagerHandlerPtr,
                                             prvTcamTokenArray[prvTokenIdIdx].clientId,
                                             prvTcamTokenArray[prvTokenIdIdx].tokenEntry,
                                             &tcamLocation);

        /* check is call failed */
        if (GT_OK != status)
        {
            galtisOutput(outArgs, status, "Cannot get token tcam location %d", -1);
            return CMD_OK;
        }

        inFields[0] = prvTcamTokenArray[prvTokenIdIdx].isUsed;
        inFields[1] = prvTokenIdIdx;
        inFields[2] = prvTcamTokenArray[prvTokenIdIdx].tokenEntry;
        inFields[3] = prvTcamTokenArray[prvTokenIdIdx].clientId;
        inFields[4] = tcamLocation.row;
        inFields[5] = tcamLocation.column;

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                                  inFields[3], inFields[4], inFields[5]);

        galtisOutput(outArgs, GT_OK, "%f");
    }

    prvTokenIdIdx++;

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerEntryFree
*
* DESCRIPTION:
*       Frees entry.
*
* INPUTS:
*       prvTcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       entryToken            - token representing the entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_OUT_OF_PP_MEM      - if there is no space left between the upper
*                               and lower bounds
*       GT_OUT_OF_RANGE       - if the indexes were out of range
*
* COMMENTS:
*       1. The entry must be previously allocated by this client.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTcamManagerEntryFreeDel
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS status;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    /* note: table contain only used tokens, so only it index can be get */
    prvTokenIdIdx = (GT_U32)inFields[0];

    /* check user input index range */
    if (prvTokenIdIdx >= prvTokensArrSize)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* check is token used */
    if(GT_FALSE == prvTcamTokenArray[prvTokenIdIdx].isUsed)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Trying to free not used token %d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    status = cpssDxChTcamManagerEntryFree(prvTcamManagerHandlerPtr,
                            prvTcamTokenArray[prvTokenIdIdx].clientId,
                            prvTcamTokenArray[prvTokenIdIdx].tokenEntry);

    /* check is call failed */
    if (GT_OK != status)
    {
        galtisOutput(outArgs, status, "%d", -1);
        return CMD_OK;
    }

    /* mark token index as not initialized */
    prvTcamTokenArray[prvTokenIdIdx].isUsed = GT_FALSE;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerEntryFree
*
* DESCRIPTION:
*       Frees entry.
*
* INPUTS:
*       prvTcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       entryToken            - token representing the entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_OUT_OF_PP_MEM      - if there is no space left between the upper
*                               and lower bounds
*       GT_OUT_OF_RANGE       - if the indexes were out of range
*
* COMMENTS:
*       1. The entry must be previously allocated by this client.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTcamManagerEntryFreeClear
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS status = GT_OK;
    GT_U32    entryTokenIdx = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* find first used token in array */
    for(entryTokenIdx = 0; entryTokenIdx < prvTokensArrSize; entryTokenIdx++)
    {
        /* check is token used */
        if(GT_TRUE == prvTcamTokenArray[entryTokenIdx].isUsed)
        {
            /* free token from memory */
            status = cpssDxChTcamManagerEntryFree(prvTcamManagerHandlerPtr,
                                prvTcamTokenArray[prvTokenIdIdx].clientId,
                                prvTcamTokenArray[prvTokenIdIdx].tokenEntry);

            /* mark token index as not initialized */
            prvTcamTokenArray[prvTokenIdIdx].isUsed = GT_FALSE;
        }
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerDump
*
* DESCRIPTION:
*       Dumps the state of the TCAM manager.
*
* INPUTS:
*       prvTcamManagerHandlerPtr - the TCAM manager handler
*       dumpType              - type of dump
*       clientId              - the client Id; relevant only when dump
*                               type is  CPSS_DXCH_TCAM_MANAGER_DUMP_ENTRIES_FOR_SPECIFIC_CLIENT_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_NO_SUCH            - the clientId is not registered
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTcamManagerDump
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS status;

    CPSS_DXCH_TCAM_MANAGER_DUMP_TYPE_ENT dumpType;
    GT_U32 clientIdIdx;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* check is tcam manager created */
    if(NULL == prvTcamManagerHandlerPtr)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Tcam manager was not created yet  %d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    dumpType = (CPSS_DXCH_TCAM_MANAGER_DUMP_TYPE_ENT)inArgs[0];

    clientIdIdx = (GT_U32) inArgs[1];

    /* check user input index range */
    if (clientIdIdx >= CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Wrong client index (range 0-15)");
        return CMD_OK;
    }

    /* call cpss api function */
    status = cpssDxChTcamManagerDump(prvTcamManagerHandlerPtr,
                                    dumpType, prvTcamClientIdArray[clientIdIdx]);

    /* check is call failed */
    if (GT_OK != status)
    {
        galtisOutput(outArgs, status, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerEntryAllocate
*
* DESCRIPTION:
*       Allocates TCAM entry for client. The entry is allocated in a range
*       bounded by two other entries (bounded in the meaning of TCAM search
*       order). The entry is allocated according to specific allocation method.
*
* INPUTS:
*       prvTcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       entryType             - entry type to allocate
*       lowerBoundToken       - token representing the lower boundary; the allocated
*                               entry will reside after this token (in the meaning
*                               of TCAM search order); if 0 then no lower boundary
*       upperBoundToken       - toekn representing the upper boundary; the allocated
*                               entry will reside before this token (in the meaning
*                               of TCAM search order); if 0 then no upper boundary
*       allocMethod           - the allocation method
*       defragEnable          - whether to defrag entries in case there is no
*                               free place for this entry cause of entries
*                               fragmentation;
*       clientCookiePtr       - client cookie; used in client callback functions
*
* OUTPUTS:
*       allocEntryTokenPtr    - token representing the allocated entry
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_OUT_OF_PP_MEM      - if there is no space left between the upper
*                               and lower bounds
*       GT_OUT_OF_RANGE       - if the indexes were out of range
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTcamManagerEntryAllocate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS status;

    GT_U32   clientIdIdx;
    GT_U32   entryTokenIdx;
    CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT entryType;
    GT_U32   lowerBoundTokenIdx;
    GT_U32   upperBoundTokenIdx;
    CPSS_DXCH_TCAM_MANAGER_ALLOC_METHOD_ENT allocMethod;
    GT_BOOL  defragEnable;
    GT_VOID *clientCookiePtr = NULL;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* check is tcam manager created */
    if(NULL == prvTcamManagerHandlerPtr)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Tcam manager was not created yet  %d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */

    clientIdIdx = (GT_U32)inArgs[0];

    /* check user input index range */
    if (clientIdIdx >= CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Wrong client index (range 0-15)");
        return CMD_OK;
    }

    entryTokenIdx = (GT_U32)inArgs[1];

    /* check user input index range */
    if (entryTokenIdx >= prvTokensArrSize)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Wrong token index(out of range)");
        return CMD_OK;
    }

    /* check is token index already busy */
    if (GT_TRUE == prvTcamTokenArray[entryTokenIdx].isUsed)
    {
        galtisOutput(outArgs, GT_BAD_VALUE,
                    "Wrong token index(index already in use, free it first)");
        return CMD_OK;
    }

    entryType = (CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT)inArgs[2];

    lowerBoundTokenIdx = (GT_U32)inArgs[3];

    /* check user input index range */
    if (entryTokenIdx >= prvTokensArrSize)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Wrong token index(out of range)");
        return CMD_OK;
    }

    /* check is token index used */
    if ((GT_FALSE == prvTcamTokenArray[lowerBoundTokenIdx].isUsed) &&
        (0 != prvTcamTokenArray[lowerBoundTokenIdx].tokenEntry))
    {
        galtisOutput(outArgs, GT_BAD_VALUE,
                         "Wrong token index(given index not used)");
        return CMD_OK;
    }

    upperBoundTokenIdx = (GT_U32)inArgs[4];

    /* check user input index range */
    if (entryTokenIdx >= prvTokensArrSize)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Wrong token index(out of range)");
        return CMD_OK;
    }

    /* check is token index used */
    if ((GT_FALSE == prvTcamTokenArray[upperBoundTokenIdx].isUsed) &&
        (0 != prvTcamTokenArray[upperBoundTokenIdx].tokenEntry))
    {
        galtisOutput(outArgs, GT_BAD_VALUE,
                         "Wrong token index(given index not used)");
        return CMD_OK;
    }

    allocMethod  = (CPSS_DXCH_TCAM_MANAGER_ALLOC_METHOD_ENT)inArgs[5];

    defragEnable = (GT_BOOL)inArgs[6];

    /* call cpss api function */
    status = cpssDxChTcamManagerEntryAllocate(prvTcamManagerHandlerPtr,
                   prvTcamClientIdArray[clientIdIdx], entryType,
                   prvTcamTokenArray[lowerBoundTokenIdx].tokenEntry,
                   prvTcamTokenArray[upperBoundTokenIdx].tokenEntry,
                   allocMethod, defragEnable, clientCookiePtr,
                   &prvTcamTokenArray[entryTokenIdx].tokenEntry);

    /* check is call failed */
    if (GT_OK != status)
    {
        galtisOutput(outArgs, status, "%d", -1);
        return CMD_OK;
    }

    /*save clientId for new created token*/
    prvTcamTokenArray[entryTokenIdx].clientId = prvTcamClientIdArray[clientIdIdx];

    /* mark token index as initialized */
    prvTcamTokenArray[entryTokenIdx].isUsed = GT_TRUE;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "%d", prvTcamTokenArray[entryTokenIdx]);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerEntryClientCookieUpdate
*
* DESCRIPTION:
*       Updates client cookie for specific entry token.
*
* INPUTS:
*       prvTcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       entryToken            - token representing the entry to be updated
*       clientCookiePtr       - new client cookie; used in client callback functions
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_OUT_OF_PP_MEM      - if there is no space left between the upper
*                               and lower bounds
*       GT_OUT_OF_RANGE       - if the indexes were out of range
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTcamManagerEntryClientCookieUpdate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS status;

    GT_UINTPTR entryTokenIdx;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* check is tcam manager created */
    if(NULL == prvTcamManagerHandlerPtr)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Tcam manager was not created yet  %d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    entryTokenIdx = inArgs[0];

    status = prvTcamPrepareTokenArray(outArgs, (GT_U32)entryTokenIdx);
    if (entryTokenIdx >= prvTokensArrSize)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Wrong entryToken index");
        return CMD_OK;
    }

    /* call cpss api function */
    status = cpssDxChTcamManagerEntryClientCookieUpdate(prvTcamManagerHandlerPtr,
                                   prvTcamTokenArray[entryTokenIdx].clientId,
                                   prvTcamTokenArray[entryTokenIdx].tokenEntry,
                                   (GT_VOID*)entryTokenIdx);

    /* check is call failed */
    if (GT_OK != status)
    {
        galtisOutput(outArgs, status, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChTcamManagerAvailableEntriesCheck
*
* DESCRIPTION:
*       Checks whether the TCAM manager has enough available entries for the
*       requested amount of entries.
*
* INPUTS:
*       prvTcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       prvTcamRequestedEntriesArray - array of amount of entries requested by client
*       sizeOfArray           - number of elemenets in the array
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_OUT_OF_PP_MEM      - if there is no space left between the upper
*                               and lower bounds
*       GT_OUT_OF_RANGE       - if the indexes were out of range
*
* COMMENTS:
*       1. The number of reserved entries of the specific client is also taken
*          into calculations.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTcamManagerAvailableEntriesCheckSetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS status = GT_OK;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* check is tcam manager created */
    if(NULL == prvTcamManagerHandlerPtr)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Tcam manager was not created yet  %d", -1);
        return CMD_OK;
    }

    /* reset array */
    cmdOsMemSet(prvTcamRequestedEntriesArray, 0, sizeof(prvTcamRequestedEntriesArray));

    prvTcamRequestedEntriesIdx = 0;

    /* map input arguments to locals */
    prvTcamRequestedEntriesArray[prvTcamRequestedEntriesIdx].entryType =
                                    (CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT)inFields[0];
    prvTcamRequestedEntriesArray[prvTcamRequestedEntriesIdx].amount = (GT_U32)inFields[1];

    prvTcamRequestedEntriesIdx++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerAvailableEntriesCheck
*
* DESCRIPTION:
*       Checks whether the TCAM manager has enough available entries for the
*       requested amount of entries.
*
* INPUTS:
*       prvTcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       prvTcamRequestedEntriesArray - array of amount of entries requested by client
*       sizeOfArray           - number of elemenets in the array
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_OUT_OF_PP_MEM      - if there is no space left between the upper
*                               and lower bounds
*       GT_OUT_OF_RANGE       - if the indexes were out of range
*
* COMMENTS:
*       1. The number of reserved entries of the specific client is also taken
*          into calculations.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTcamManagerAvailableEntriesCheckSetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS status = GT_OK;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* check the end of the table */
    if (prvTcamRequestedEntriesIdx >= TCAM_MAX_ENTRY_TYPES_SIZE_CNS)
    {
        galtisOutput(outArgs, status, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    prvTcamRequestedEntriesArray[prvTcamRequestedEntriesIdx].entryType =
                                    (CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT)inFields[0];
    prvTcamRequestedEntriesArray[prvTcamRequestedEntriesIdx].amount    = (GT_U32)inFields[1];

    prvTcamRequestedEntriesIdx++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerAvailableEntriesCheck
*
* DESCRIPTION:
*       Checks whether the TCAM manager has enough available entries for the
*       requested amount of entries.
*
* INPUTS:
*       prvTcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       prvTcamRequestedEntriesArray - array of amount of entries requested by client
*       sizeOfArray           - number of elemenets in the array
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_OUT_OF_PP_MEM      - if there is no space left between the upper
*                               and lower bounds
*       GT_OUT_OF_RANGE       - if the indexes were out of range
*
* COMMENTS:
*       1. The number of reserved entries of the specific client is also taken
*          into calculations.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTcamManagerAvailableEntriesCheckEndSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS status;

    GT_U32    clientIdIdx;
    GT_U32    sizeOfArray = TCAM_MAX_ENTRY_TYPES_SIZE_CNS;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    clientIdIdx = (GT_U32)inArgs[0];

    /* check user input index range */
    if (clientIdIdx >= CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Wrong client index (range 0-15)");
        return CMD_OK;
    }

    sizeOfArray = (GT_U32)inArgs[1];

    /* call cpss api function */
    status = cpssDxChTcamManagerAvailableEntriesCheck(prvTcamManagerHandlerPtr,
               prvTcamClientIdArray[clientIdIdx], prvTcamRequestedEntriesArray, sizeOfArray);

    /* check is call failed */
    if (GT_OK != status)
    {
        galtisOutput(outArgs, status, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerTokenToTcamLocation
*
* DESCRIPTION:
*       Converts entry token to hardware TCAM location.
*
* INPUTS:
*       prvTcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       entryToken            - token representing the entry
*
* OUTPUTS:
*       tcamLocationPtr       - points to the hardware TCAM location
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_OUT_OF_PP_MEM      - if there is no space left between the upper
*                               and lower bounds
*       GT_OUT_OF_RANGE       - if the indexes were out of range
*
* COMMENTS:
*       1. The entry must be previously allocated by the same client.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTcamManagerTokenToTcamLocation
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS status;
    GT_U32    entryTokenIdx;

    CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC tcamLocation;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* check is tcam manager created */
    if(NULL == prvTcamManagerHandlerPtr)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Tcam manager was not created yet  %d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    entryTokenIdx = (GT_U32)inArgs[0];

    /* check user input index range */
    if (entryTokenIdx >= prvTokensArrSize)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Wrong entryToken index");
        return CMD_OK;
    }

    /* call cpss api function */
    status = cpssDxChTcamManagerTokenToTcamLocation(prvTcamManagerHandlerPtr,
                                     prvTcamTokenArray[entryTokenIdx].clientId,
                                     prvTcamTokenArray[entryTokenIdx].tokenEntry,
                                     &tcamLocation);

    /* check is call failed */
    if (GT_OK != status)
    {
        galtisOutput(outArgs, status, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "%d%d", tcamLocation.row, tcamLocation.column);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerTokenCompare
*
* DESCRIPTION:
*       Compares two tokens and returns the relative order (according to TCAM
*       order).
*
* INPUTS:
*       prvTcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       firstToken            - first token to compare
*       secondToken           - second token to compare
*
* OUTPUTS:
*       statusPtr             - points to the compare status,
*                               GT_EQUAL    - tokens represent same tcam location
*                               GT_GREATER  - first token is after second token
*                                             according to TCAM search order
*                               GT_SMALLER  - first tokem is before second token
*                                             according to TCAM search order
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_OUT_OF_PP_MEM      - if there is no space left between the upper
*                               and lower bounds
*       GT_OUT_OF_RANGE       - if the indexes were out of range
*
* COMMENTS:
*       The tokens must be previously allocated by the same client.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTcamManagerTokenCompare
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS status;

    GT_U32 firstTokenIdx;
    GT_U32 secondTokenIdx;
    GT_COMP_RES result;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* check is tcam manager created */
    if(NULL == prvTcamManagerHandlerPtr)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Tcam manager was not created yet  %d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    firstTokenIdx  = (GT_U32)inArgs[0];

    /* check user input index range */
    if (firstTokenIdx >= prvTokensArrSize)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Wrong entryToken index");
        return CMD_OK;
    }

    secondTokenIdx = (GT_U32)inArgs[1];

    /* check user input index range */
    if (secondTokenIdx >= prvTokensArrSize)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Wrong entryToken index");
        return CMD_OK;
    }

    /*check is two tokens allocated by the same client*/
    if (prvTcamTokenArray[firstTokenIdx].clientId !=
        prvTcamTokenArray[secondTokenIdx].clientId)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "Tokens allocated by different clients");
        return CMD_OK;
    }

    /* call cpss api function */
    status = cpssDxChTcamManagerTokenCompare(prvTcamManagerHandlerPtr,
                                   prvTcamTokenArray[firstTokenIdx].clientId,
                                   prvTcamTokenArray[firstTokenIdx].tokenEntry,
                                   prvTcamTokenArray[secondTokenIdx].tokenEntry,
                                   &result);

    /* check is call failed */
    if (GT_OK != status)
    {
        galtisOutput(outArgs, status, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */

    switch(result)
    {
        case GT_EQUAL:
            galtisOutput(outArgs, status, "Result: equal");
            break;
        case GT_GREATER:
            galtisOutput(outArgs, status, "Result: greater");
            break;
        case GT_SMALLER:
            galtisOutput(outArgs, status, "Result: smaller");
            break;
        default:
            /*this should never happen*/
            galtisOutput(outArgs, status, "Wrong result!  %d", -1);
    }

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTcamManagerGetFromLpmDb
*
* DESCRIPTION:
*       Get the TCAM manager handler used by the LPM DB.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTcamManagerGetFromLpmDb
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32                              lpmDBId;
    PRV_CPSS_DXCH_LPM_SHADOW_STC        *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC   *tcamLpmDbPtr;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* verify that no TCAM manager is already in used */
    if (prvTcamManagerHandlerPtr != NULL)
    {
        cmdOsPrintf("TCAM manager already in use\n");
        galtisOutput(outArgs, GT_ALREADY_EXIST, "");
        return CMD_OK;
    }

    lpmDBId = (GT_U32)inArgs[0];

    /* search for the LPM DB */
    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        cmdOsPrintf("LPM DB was not found\n");
        galtisOutput(outArgs, GT_NOT_FOUND, "");
        return CMD_OK;
    }

    tcamLpmDbPtr = (PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow);

    if (tcamLpmDbPtr->tcamManagementMode != PRV_CPSS_DXCH_LPM_TCAM_MANAGEMENT_TCAM_MANAGER_MODE_E)
    {
        cmdOsPrintf("LPM DB doesn't use TCAM manager\n");
        galtisOutput(outArgs, GT_NO_SUCH, "");
        return CMD_OK;
    }

    prvTcamManagerHandlerPtr = tcamLpmDbPtr->tcamManagerHandlerPtr;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChTcamManagerCreate",
        &wrCpssDxChTcamManagerCreate,
        3, 0},
    {"cpssDxChTcamManagerConfigGet",
        &wrCpssDxChTcamManagerConfigGet,
        0, 0},
    {"cpssDxChTcamManagerDelete",
        &wrCpssDxChTcamManagerDelete,
        0, 0},
    {"cpssDxChTcamManagerRangeUpdate",
        &wrCpssDxChTcamManagerRangeUpdate,
        3, 0},
    {"cpssDxChTcamManagerClientRegister",
        &wrCpssDxChTcamManagerClientRegister,
        2, 0},
    {"cpssDxChTcamManagerClientUnregister",
        &wrCpssDxChTcamManagerClientUnregister,
        1, 0},

   /******************************************************/
    {"cpssDxChTcamManagerEntriesReservationSetFirst",
        &wrCpssDxChTcamManagerEntriesReservationSetFirst,
        0, 2},
    {"cpssDxChTcamManagerEntriesReservationSetNext",
        &wrCpssDxChTcamManagerEntriesReservationSetNext,
        0, 2},
    {"cpssDxChTcamManagerEntriesReservationEndSet",
        &wrCpssDxChTcamManagerEntriesReservationEndSet,
        4, 0},

   /******************************************************/
    {"cpssDxChTcamManagerEntriesReservationGetFirst",
        &wrCpssDxChTcamManagerEntriesReservationGetFirst,
        1, 0},
    {"cpssDxChTcamManagerEntriesReservationGetNext",
        &wrCpssDxChTcamManagerEntriesReservationGetNext,
        0, 0},

   /******************************************************/
    {"cpssDxChTcamManagerUsageGet",
        &wrCpssDxChTcamManagerUsageGetFirst,
        2, 0},
    {"cpssDxChTcamManagerUsageGet",
        &wrCpssDxChTcamManagerUsageGetNext,
        0, 0},

   /**************** Entry token table ******************/
    {"cpssDxChTcamManagerTokenTableGetFirst",
        &wrCpssDxChTcamManagerTokenTableGetFirst,
        1, 0},
    {"cpssDxChTcamManagerTokenTableGetNext",
        &wrCpssDxChTcamManagerTokenTableGetNext,
        0, 0},
    {"cpssDxChTcamManagerTokenTableDelete",
        &wrCpssDxChTcamManagerEntryFreeDel,
        1, 0},
    {"cpssDxChTcamManagerTokenTableClear",
        &wrCpssDxChTcamManagerEntryFreeClear,
        0, 0},

   /******************************************************/
    {"cpssDxChTcamManagerDump",
        &wrCpssDxChTcamManagerDump,
        2, 0},
    {"cpssDxChTcamManagerEntryAllocate",
        &wrCpssDxChTcamManagerEntryAllocate,
        7, 0},
    {"cpssDxChTcamManagerEntryClientCookieUpdate",
        &wrCpssDxChTcamManagerEntryClientCookieUpdate,
        1, 0},

   /******************************************************/
    {"cpssDxChTcamManagerAvailableEntriesCheckSetFirst",
        &wrCpssDxChTcamManagerAvailableEntriesCheckSetFirst,
        0, 2},
    {"cpssDxChTcamManagerAvailableEntriesCheckSetNext",
        &wrCpssDxChTcamManagerAvailableEntriesCheckSetNext,
        0, 2},
    {"cpssDxChTcamManagerAvailableEntriesCheckEndSet",
        &wrCpssDxChTcamManagerAvailableEntriesCheckEndSet,
        2, 0},

   /******************************************************/
    {"cpssDxChTcamManagerTokenToTcamLocation",
        &wrCpssDxChTcamManagerTokenToTcamLocation,
        1, 0},
    {"cpssDxChTcamManagerTokenCompare",
        &wrCpssDxChTcamManagerTokenCompare,
        2, 0},

    {"cpssDxChTcamManagerGetFromLpmDb",
        &wrCpssDxChTcamManagerGetFromLpmDb,
        1, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChTcamManager
*
* DESCRIPTION:
*     Library database initialization function.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
GT_STATUS cmdLibInitCpssDxChTcamManager
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}
