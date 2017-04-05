/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplCpssDxChBrgFdb.c
*
* DESCRIPTION:
*       A lua wrapper for FDB tables facility CPSS DxCh implementation
*
* DEPENDENCIES:
*       A lua type wrapper
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 22 $
*******************************************************************************/

#include <cpssCommon/wraplCpssTypes.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgMc.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpssCommon/wraplCpssDeviceInfo.h>
#include <cpssCommon/wrapCpssDebugInfo.h>
#include <generic/private/prvWraplGeneral.h>
#include <generic/wraplCpssGenBrgFdb.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>

#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgGen.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdbHash.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>
#include <extUtils/tgf/tgfBridgeGen.h>

#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>

/*******************************************************************************
*   FDB tables facility wrapper errors                                         *
*******************************************************************************/
GT_CHAR aAMessagesDropingRiskWarningMessage[]   =
        "Aging mode operation is not supported because of AA messages " \
        "dropping risk";
GT_CHAR multicastIPv6BytesReadingError[]        =
                                        "Could not read Multicast IPv6 bytes";
GT_CHAR multicastIPv6BytesWritingError[]        =
                                        "Could not write Multicast IPv6 bytes";
GT_CHAR fdbMacEntryFromLuaGettingErrorMessage[] =
                                        "Could not get Fdb Mac Entry from lua";
GT_CHAR fdbMacEntryConvertCpssToTgfMessage[] =
                       "Could not convert FDB MAC entry from CPSS to TGF view";
GT_CHAR fdbMacEntryReadErrorMessage[]           =
                                                "Could not read Fdb Mac Entry";
GT_CHAR fdbMacEntryWriteErrorMessage[]          =
                                                "Could not write Fdb Mac Entry";


/***** declarations ********/

use_prv_struct(CPSS_MAC_ENTRY_EXT_STC)
use_prv_print_struct(CPSS_MAC_ENTRY_EXT_STC)
use_prv_struct(CPSS_MAC_ENTRY_EXT_KEY_STC)
use_prv_print_struct(CPSS_MAC_ENTRY_EXT_KEY_STC)
/***** declarations ********/

static GT_STATUS local_fdbNextEntryGet_fromFdbShadow(
    IN GT_U8    devNum,
    IN CPSS_DXCH_BRG_FDB_DEL_MODE_ENT mode,
    INOUT GT_U32   *cookiePtr,
    OUT GT_U32  *indexPtr,
    OUT CPSS_MAC_ENTRY_EXT_STC  **foundMacEntryPtrPtr
);
/* when using the FDB shadow : the cookie needed for iterations */
static GT_U32   cookie__for_fdb_shadow = 0;
/* when using the FDB shadow : indication that the 'index=0' is 'start of iterations'
   or 'really index 0' */
static GT_BOOL  zeroRealIndex__for_fdb_shadow = GT_FALSE;

/*#define DO_DEBUG_PRINT*/
#ifdef DO_DEBUG_PRINT
    #define DEBUG_PRINT(x) cpssOsPrintf x
#else
    #define DEBUG_PRINT(x)
#endif

static void prvCpssPrintFdbKey(
    IN GT_CHAR*name,
    IN CPSS_MAC_ENTRY_EXT_KEY_STC   *keyPtr,
    IN GT_BOOL forcePrint)
{
    if (forcePrint == GT_TRUE)
    {
        cpssOsPrintf("%s: macEntry : entryType = %d ,"
                "vlanId[%d] , mac[%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x] vid1 = %d \n",
                name,
                keyPtr->entryType,
                keyPtr->key.macVlan.vlanId,
                keyPtr->key.macVlan.macAddr.arEther[0],
                keyPtr->key.macVlan.macAddr.arEther[1],
                keyPtr->key.macVlan.macAddr.arEther[2],
                keyPtr->key.macVlan.macAddr.arEther[3],
                keyPtr->key.macVlan.macAddr.arEther[4],
                keyPtr->key.macVlan.macAddr.arEther[5],
                keyPtr->vid1
                );
    }
    else
    {
        DEBUG_PRINT(("%s: macEntry : entryType = %d ,"
                "vlanId[%d] , mac[%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x] vid1 = %d \n",
                name,
                keyPtr->entryType,
                keyPtr->key.macVlan.vlanId,
                keyPtr->key.macVlan.macAddr.arEther[0],
                keyPtr->key.macVlan.macAddr.arEther[1],
                keyPtr->key.macVlan.macAddr.arEther[2],
                keyPtr->key.macVlan.macAddr.arEther[3],
                keyPtr->key.macVlan.macAddr.arEther[4],
                keyPtr->key.macVlan.macAddr.arEther[5],
                keyPtr->vid1
                ));
    }
}
/*******************************************************************************
* prvCpssDxChBrgFdbMacEntryFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of CPSS_MAC_ENTRY_EXT_STC from lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                  - lua state
*        L_index            - entry lua stack index
*
* OUTPUTS:
*       isMacEntryTaken     - property, that mac entry was in stack
*       macEntry            - taken mac entry
*       error_message      - error message
*
* RETURNS:
*       GT_OK, if success otherwise error code and message
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgFdbMacEntryFromLuaGet
(
    IN  lua_State               *L,
    IN  GT_32                   L_index,
    OUT GT_BOOL                 *isMacEntryTaken,
    OUT CPSS_MAC_ENTRY_EXT_STC  *macEntry,
    OUT GT_CHAR_PTR             *error_message
)
{
    GT_STATUS                   status          = GT_OK;

    *error_message = NULL;


    cpssOsMemSet(macEntry, 0, sizeof(*macEntry));

    if (0 != lua_istable(L, L_index))
    {
        lua_pushvalue(L, L_index);
        prv_lua_to_c_CPSS_MAC_ENTRY_EXT_STC(L, macEntry);
        lua_pop(L, 1);
        *isMacEntryTaken = GT_TRUE;

        prvCpssPrintFdbKey("MacEntryFromLuaGet : ",&macEntry->key,GT_FALSE);

    }
    else
    {
        *isMacEntryTaken = GT_FALSE;

        prvCpssPrintFdbKey("MacEntryFromLuaGet - no explicit : ",&macEntry->key,GT_FALSE);
    }

    return status;
}


/*******************************************************************************
* prvCpssDxChBrgFdbMacEntryKeyFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of CPSS_MAC_ENTRY_EXT_KEY_STC from lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                  - lua state
*        L_index            - entry lua stack index
*
* OUTPUTS:
*       isMacEntryTaken     - property, that mac entry was in stack
*       macEntry            - taken mac entry
*       error_message      - error message
*
* RETURNS:
*       GT_OK, if success otherwise error code and message
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgFdbMacEntryKeyFromLuaGet
(
    IN  lua_State               *L,
    IN  GT_32                   L_index,
    OUT GT_BOOL                 *isMacEntryTaken,
    OUT CPSS_MAC_ENTRY_EXT_KEY_STC  *macEntryKeyPtr,
    OUT GT_CHAR_PTR             *error_message
)
{
    GT_STATUS                   status          = GT_OK;

    *error_message = NULL;


    cpssOsMemSet(macEntryKeyPtr, 0, sizeof(*macEntryKeyPtr));

    if (0 != lua_istable(L, L_index))
    {
        lua_pushvalue(L, L_index);
        prv_lua_to_c_CPSS_MAC_ENTRY_EXT_KEY_STC(L, macEntryKeyPtr);
        lua_pop(L, 1);
        *isMacEntryTaken = GT_TRUE;
    }
    else
    {
        *isMacEntryTaken = GT_FALSE;
    }

    return status;
}
/*******************************************************************************
* prvCpssDxChBrgFdbMacEntryMaskFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of CPSS_MAC_ENTRY_EXT_STC comparision mask from lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                  - lua state
*        L_index            - entry lua stack index
*
* OUTPUTS:
*       macEntryMask        - taken mac entry
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgFdbMacEntryMaskFromLuaGet
(
    IN  lua_State               *L,
    IN  GT_32                   L_index,
    OUT CPSS_MAC_ENTRY_EXT_STC  *macEntry,
    OUT GT_CHAR_PTR             *error_message
)
{
    GT_STATUS                   status          = GT_OK;
    *error_message = NULL;

    if (0 != lua_istable(L, L_index))
    {
        lua_pushvalue(L, L_index);
        prv_lua_to_c_mask_CPSS_MAC_ENTRY_EXT_STC(L, macEntry);
        lua_pop(L, 1);
    }
    else
    {
        cpssOsMemSet(macEntry, 0xff, sizeof(*macEntry));
    }

    prvCpssPrintFdbKey("MaskFromLuaGet",&macEntry->key,GT_FALSE);

    return status;
}


/*******************************************************************************
* prvCpssDxChBrgAgedAndMacEntryFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of aged property and mac-entry from lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                                  - lua state
*        L_index                            - entry lua stack index
*        isSearchedAgedPropertyTakenPtr     - property, that searched pattern
*                                             and mask of aged property was
*                                             found
*        patternAgedPtr                     - searched pattern of aged property
*        isSearchedMacEntryTakenPtr         - property, that searched pattern
*                                             and mask of mac entry was found
*        patternMacEtnryPtr                 - searched pattern of mac entry
*        maskMacEtnryPtr                    - searched mask of mac entry
*        error_message                      - error message
*
* OUTPUTS:
*       status and FDB enrty index, if exists, to lua stack
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgAgedAndMacEntryFromLuaGet
(
    IN lua_State                            *L,
    IN GT_32                                L_index,
    OUT GT_BOOL                             *isSearchedAgedPropertyTakenPtr,
    OUT GT_BOOL                             *patternAgedPtr,
    OUT GT_BOOL                             *maskAgedPtr,
    OUT GT_BOOL                             *isSearchedMacEntryTakenPtr,
    OUT CPSS_MAC_ENTRY_EXT_STC              *patternMacEtnryPtr,
    OUT CPSS_MAC_ENTRY_EXT_STC              *maskMacEtnryPtr,
    OUT GT_CHAR_PTR                         *error_message
)
{
    GT_STATUS                               status  = GT_OK;

    *error_message = NULL;

    status = pvrCpssBooleanFromLuaGet(L, L_index,
                                      isSearchedAgedPropertyTakenPtr,
                                      patternAgedPtr, error_message);

    if ((GT_OK == status) && (GT_TRUE == *isSearchedAgedPropertyTakenPtr))
    {
        L_index++;
        *maskAgedPtr = (GT_BOOL) 0;
    }

    if ((GT_OK == status) && (GT_FALSE == *isSearchedAgedPropertyTakenPtr))
    {
        status = prvCpssDxChBrgFdbMacEntryFromLuaGet(L, L_index,
                                                     isSearchedMacEntryTakenPtr,
                                                     patternMacEtnryPtr,
                                                     error_message);
    }

    if ((GT_OK == status) && (GT_FALSE == *isSearchedMacEntryTakenPtr))
    {
        status = prvCpssDxChBrgFdbMacEntryFromLuaGet(L, ++L_index,
                                                     isSearchedMacEntryTakenPtr,
                                                     patternMacEtnryPtr,
                                                     error_message);
    }

    if ((GT_OK == status) && (GT_TRUE == *isSearchedMacEntryTakenPtr))
    {
        status = prvCpssDxChBrgFdbMacEntryMaskFromLuaGet(L, L_index,
                                                         maskMacEtnryPtr,
                                                         error_message);
    }

    return status;
}


/*******************************************************************************
* pvrCpssMacAddressTableCountStatisticFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of mac address table count statistic from lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                                  - lua state
*        L_index                            - entry lua stack index
*        statisticPtr                       - mac address table count statistic
*        error_message                      - error message
*
* OUTPUTS:
*       status and FDB enrty index, if exists, to lua stack
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssMacAddressTableCountStatisticFromLuaGet
(
    IN  lua_State                                       *L,
    IN  GT_32                                           L_index,
    OUT WRAPL_CPSS_MAC_ADDRESS_TABLE_COUT_STATICTIC     *statisticPtr,
    OUT GT_CHAR_PTR                                     *error_message
)
{
    GT_STATUS                                           status = GT_OK;

    *error_message  = NULL;

    if (0 != lua_istable(L, L_index))
    {
        lua_getfield(L, L_index, "total");
        if (LUA_TNUMBER == lua_type(L, -1))
        {
            statisticPtr->total_entries_count   = (GT_32) lua_tointeger(L, -1);
        }
        else
        {
            statisticPtr->total_entries_count   = 0;
        }
        lua_pop(L, 1);

        lua_getfield(L, L_index, "free");
        if (LUA_TNUMBER == lua_type(L, -1))
        {
            statisticPtr->free_entries_count    = (GT_32) lua_tointeger(L, -1);
        }
        else
        {
            statisticPtr->free_entries_count    = 0;
        }
        lua_pop(L, 1);

        lua_getfield(L, L_index, "used");
        if (LUA_TNUMBER == lua_type(L, -1))
        {
            statisticPtr->used_entries_count    = (GT_32) lua_tointeger(L, -1);
        }
        else
        {
            statisticPtr->used_entries_count    = 0;
        }
        lua_pop(L, 1);

        lua_getfield(L, L_index, "static");
        if (LUA_TNUMBER == lua_type(L, -1))
        {
            statisticPtr->static_entries_count  = (GT_32) lua_tointeger(L, -1);
        }
        else
        {
            statisticPtr->static_entries_count  = 0;
        }
        lua_pop(L, 1);

        lua_getfield(L, L_index, "dynamic");
        if (LUA_TNUMBER == lua_type(L, -1))
        {
            statisticPtr->dynamic_entries_count = (GT_32) lua_tointeger(L, -1);
        }
        else
        {
            statisticPtr->dynamic_entries_count = 0;
        }
        lua_pop(L, 1);

        lua_getfield(L, L_index, "skipped");
        if (LUA_TNUMBER == lua_type(L, -1))
        {
            statisticPtr->skipped_entries_count = (GT_32) lua_tointeger(L, -1);
        }
        else
        {
            statisticPtr->skipped_entries_count = 0;
        }
        lua_pop(L, 1);
    }

    return status;
}


/*******************************************************************************
* pvrCpssValidSkipAgedToLuaPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of pushing valid skip and aged values to lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       status              - caller status
*       L                   - lua state
*       valid               - valid property
*       skip                - skipped property
*       aged                - aged property
*
* OUTPUTS:
*
* RETURNS:
*       1 if data was pushed to stack otherwise 0
*
* COMMENTS:
*
*******************************************************************************/
int pvrCpssValidSkipAgedToLuaPush
(
    IN GT_STATUS                status,
    IN lua_State                *L,
    IN GT_BOOL                  valid,
    IN GT_BOOL                  skip,
    IN GT_BOOL                  aged
)
{
    if (GT_OK == status)
    {
        int t;
        lua_newtable(L);
        t = lua_gettop(L);
        lua_pushboolean(L, (valid == GT_TRUE) ? 1 : 0);
        lua_setfield(L, t, "valid");
        lua_pushboolean(L, (skip  == GT_TRUE) ? 1 : 0);
        lua_setfield(L, t, "skip");
        lua_pushboolean(L, (aged  == GT_TRUE) ? 1 : 0);
        lua_setfield(L, t, "aged");
        lua_settop(L, t);

        return 1;
    }
    else
    {
        return 0;
    }
}


/*******************************************************************************
* pvrCpssValidSkipAgedToLuaOnConditionPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of conditional pushing valid skip and aged values
*       to lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       status              - caller status
*       L                   - lua state
*       valid               - valid property
*       skip                - skipped property
*       aged                - aged property
*       condition           - pushing condition
*
* OUTPUTS:
*
* RETURNS:
*       1 if data was pushed to stack otherwise 0
*
* COMMENTS:
*
*******************************************************************************/
int pvrCpssValidSkipAgedToLuaOnConditionPush
(
    IN GT_STATUS                status,
    IN lua_State                *L,
    IN GT_BOOL                  valid,
    IN GT_BOOL                  skip,
    IN GT_BOOL                  aged,
    IN GT_BOOL                  condition
)
{
    if ((GT_OK == status) && (GT_TRUE == condition))
    {
        int t;
        lua_newtable(L);
        t = lua_gettop(L);
        lua_pushboolean(L, (valid == GT_TRUE) ? 1 : 0);
        lua_setfield(L, t, "valid");
        lua_pushboolean(L, (skip  == GT_TRUE) ? 1 : 0);
        lua_setfield(L, t, "skip");
        lua_pushboolean(L, (aged  == GT_TRUE) ? 1 : 0);
        lua_setfield(L, t, "aged");
        lua_settop(L, t);

        return 1;
    }
    else
    {
        return 0;
    }
}


/*******************************************************************************
* pvrCpssMacAddressTableCountStatisticToLuaPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of pushing WRAPL_CPSS_MAC_ADDRESS_TABLE_COUT_STATICTIC to
*       lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       status              - caller status
*       L                   - lua state
*       statistic           - structure with statistic
*
* OUTPUTS:
*
* RETURNS:
*       1 if data was pushed to stack otherwise 0
*
* COMMENTS:
*
*******************************************************************************/
int pvrCpssMacAddressTableCountStatisticToLuaPush
(
    IN GT_STATUS                                        status,
    IN lua_State                                        *L,
    IN WRAPL_CPSS_MAC_ADDRESS_TABLE_COUT_STATICTIC      *statistic
)
{
    if (GT_OK == status)
    {
        int t;
        lua_newtable(L);
        t = lua_gettop(L);
        lua_pushinteger(L, statistic->total_entries_count);
        lua_setfield(L, t, "total");
        lua_pushinteger(L, statistic->free_entries_count);
        lua_setfield(L, t, "free");
        lua_pushinteger(L, statistic->used_entries_count);
        lua_setfield(L, t, "used");
        lua_pushinteger(L, statistic->static_entries_count);
        lua_setfield(L, t, "static");
        lua_pushinteger(L, statistic->dynamic_entries_count);
        lua_setfield(L, t, "dynamic");
        lua_pushinteger(L, statistic->skipped_entries_count);
        lua_setfield(L, t, "skipped");
        lua_settop(L, t);

        return 1;
    }
    else
    {
        return 0;
    }
}


/*******************************************************************************
* pvrCpssMacEntryToLuaPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of pushing CPSS_MAC_ENTRY_EXT_STC to lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       status              - caller status
*       L                   - lua state
*       macEntryPtr         - pushed mac entry
*
* OUTPUTS:
*
* RETURNS:
*       1 if data was pushed to stack otherwise 0
*
* COMMENTS:
*
*******************************************************************************/
int pvrCpssMacEntryToLuaPush
(
    IN GT_STATUS                status,
    IN lua_State                *L,
    IN CPSS_MAC_ENTRY_EXT_STC   *macEntryPtr
)
{
    if (GT_OK == status)
    {
        prv_c_to_lua_CPSS_MAC_ENTRY_EXT_STC(L, macEntryPtr);

        return 1;
    }
    else
    {
        return 0;
    }
}


/*******************************************************************************
* pvrCpssMacEntryToLuaOnConditionPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of conditional pushing CPSS_MAC_ENTRY_EXT_STC to lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       status              - caller status
*       L                   - lua state
*       macEntryPtr         - pushed mac entry
*       condition           - pushing condition
*
* OUTPUTS:
*
* RETURNS:
*       1 if data was pushed to stack otherwise 0
*
* COMMENTS:
*
*******************************************************************************/
int pvrCpssMacEntryToLuaOnConditionPush
(
    IN GT_STATUS                status,
    IN lua_State                *L,
    IN CPSS_MAC_ENTRY_EXT_STC   *macEntryPtr,
    IN GT_BOOL                  condition
)
{
    if ((GT_OK == status) && (GT_TRUE == condition))
    {
        prv_c_to_lua_CPSS_MAC_ENTRY_EXT_STC(L, macEntryPtr);

        return 1;
    }
    else
    {
        return 0;
    }
}


/*******************************************************************************
* prvCpssDxChBrgFdbTableSizeGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of FDB tables facility size.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number
*       fdbTableSizePrt     - pointer to FDB tables facility size
*
* OUTPUTS:
*       GT_OK
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgFdbTableSizeGet
(
    IN  GT_U8               devNum,
    OUT GT_U32_PTR          fdbTableSizePrt,
    OUT GT_CHAR_PTR         *error_message
)
{
    GT_STATUS               status          = GT_OK;
    *error_message = NULL;

    *fdbTableSizePrt = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.fdb;

    return status;
}


/*******************************************************************************
* prvCpssDxChFdbModeGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of FDB mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number
*       fdbModePrt          - fdb mode
*
* OUTPUTS:
*       GT_OK
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChFdbModeGet
(
    IN  GT_U8                                       devNum,
    OUT PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT *fdbModePrt,
    OUT GT_CHAR_PTR                                 *error_message
)
{
    GT_STATUS               status          = GT_OK;
    *error_message = NULL;

    *fdbModePrt = PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.fdbMode;

    return status;
}


/*******************************************************************************
* wrlDxChMacAddressAgingModeConditionChecking
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Checking of condition for mac-address aging mode setting.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                   - lua state
*       L[1]                - device number
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK and true are pused to lua stack if no errors occurs and checking
*       was successful
*       GT_OK, true and warning message are pused to lua stack if no errors
*       occurs and checking was not successful
*       error status and error message it error occures
*
* COMMENTS:
*
*******************************************************************************/
int wrlDxChMacAddressAgingModeConditionChecking
(
    IN lua_State *L
)
{
    GT_U8                       devNum                  =
                                                    (GT_U8) lua_tonumber(L, 1);
                                                                    /* devId */
    CPSS_FDB_ACTION_MODE_ENT    actionMode              =
                                            CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E;
    PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT fdbMode =
                              PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E;
    GT_BOOL                     chekingResult           = GT_TRUE;
    GT_CHAR_PTR                 conditionWarningString  = NULL;
    GT_STATUS                   status                  = GT_OK;
    GT_CHAR_PTR                 error_message           = NULL;
    int                         returned_param_count    = 0;

    status = pvrCpssFdbActionModeFromLuaGet(L, 2, &actionMode, &error_message);

    if (GT_OK == status)
    {
        status = prvCpssDxChFdbModeGet(devNum, &fdbMode, &error_message);
    }

    if (GT_OK == status)
    {
        chekingResult   =
            (CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E != actionMode) ||
            ((PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E != fdbMode) &&
             (PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_LINKED_E
                                                                != fdbMode));
        if ((GT_OK == status) && (GT_FALSE == chekingResult))
        {
            conditionWarningString  = aAMessagesDropingRiskWarningMessage;
        }
    }

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssBooleanToLuaPush(status, L, chekingResult);
    returned_param_count    +=
        pvrCpssStringToLuaPush(status, L, conditionWarningString);
    return returned_param_count;
}


/*******************************************************************************
* prvDxChBrgFdbMacEntryRead
*
* DESCRIPTION:
*        Wrapper of mac entry reading to hardware.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                - device number
*       index                 - hw mac entry index
*       P_CALLING_FORMAL_DATA - general debug information (environment variables
*                               etc); could be empty
*
* OUTPUTS:
*       validPtr              - (pointer to) is entry valid
*       skipPtr               - (pointer to) is entry skip control
*       agedPtr               - (pointer to) is entry aged
*       associatedHwDevNumPtr - (pointer to) is HW device number associated with
*                               the entry (even for vidx/trunk entries the field
*                               is used by PP for aging/flush/transplant
*                               purpose). Relevant only in case of Mac Address
*                               entry.
*       macEntryPtr          - (pointer to) extended Mac table entry
*
* RETURNS:
*       execution status (same as cpssDxChBrgFdbMacEntryRead)
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvDxChBrgFdbMacEntryRead
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  index,
    OUT GT_BOOL                 *validPtr,
    OUT GT_BOOL                 *skipPtr,
    OUT GT_BOOL                 *agedPtr,
    OUT GT_HW_DEV_NUM           *associatedHwDevNumPtr,
    OUT CPSS_MAC_ENTRY_EXT_STC  *macEntryPtr,
    OUT GT_CHAR_PTR             *error_message
    P_CALLING_FORMAL_DATA
)
{
    GT_STATUS                   status          = GT_OK;

    *error_message = NULL;

    if (GT_OK == status)
    {
        CPSS_API_CALL_LOCK();
        status = cpssDxChBrgFdbMacEntryRead(devNum, index, validPtr, skipPtr,
                                            agedPtr, associatedHwDevNumPtr,
                                            macEntryPtr);
        CPSS_API_CALL_UNLOCK();
        P_CALLING_API(
            cpssDxChBrgFdbMacEntryRead,
            PC_NUMBER(devNum,               devNum,                      GT_U8,
            PC_NUMBER(index,                index,                      GT_U32,
            PC_BOOL  (*validPtr,            valid,
            PC_BOOL  (*skipPtr,             skip,
            PC_BOOL  (*agedPtr,             aged,
            PC_NUMBER(*associatedHwDevNumPtr, associatedHwDevNum,   GT_HW_DEV_NUM,

            PC_STRUCT(macEntryPtr,          entry,      CPSS_MAC_ENTRY_EXT_STC,
            PC_LAST_PARAM))))))),
            PC_STATUS);
        if (GT_OK != status)
        {
            *error_message = fdbMacEntryReadErrorMessage;
        }

        if (GT_OK == status)
        {
            status = prvCpssPortsDevConvertDataBack(
                &(macEntryPtr->dstInterface.devPort.hwDevNum),
                &(macEntryPtr->dstInterface.devPort.portNum), error_message);
        }
    }

    /* Bug: Unclear behavore of cpssDxChBrgMcIpv6BytesSelectGet...
    if ((GT_OK == status) &&
        (CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E == macEntryPtr->key.entryType))
    {
        GT_U8  dipBytesSelectMapArr[4];
        GT_U8  sipBytesSelectMapArr[4];

        CPSS_API_CALL_LOCK();
        status = cpssDxChBrgMcIpv6BytesSelectGet(devNum, dipBytesSelectMapArr,
                                                 sipBytesSelectMapArr);
        CPSS_API_CALL_UNLOCK();

        if (GT_OK == status)
        {
        status = prv_swap_byte_array(dipBytesSelectMapArr, 4,
                                     macEntryPtr->key.key.ipMcast.dip);
        }

        if (GT_OK == status)
        {
            status = prv_swap_byte_array(sipBytesSelectMapArr, 4,
                                         macEntryPtr->key.key.ipMcast.sip);
        }

        if (GT_OK != status)
        {
            *error_message = multicastIPv6BytesReadingError;
        }
    }*/

    return status;
}


/*******************************************************************************
* prvDxChBrgFdbMacEntryWrite
*
* DESCRIPTION:
*        Wrapper of mac entry writing to hardware.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                - device number
*       index                 - hw mac entry index
*       skip                  - entry skip control
*                               GT_TRUE - used to "skip" the entry, the HW will
*                               treat this entry as "not used"
*                               GT_FALSE - used for valid/used entries.
*       macEntryPtr           - pointer to MAC entry parameters.
*       P_CALLING_FORMAL_DATA - general debug information (environment variables
*                               etc); could be empty
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       execution status (same as cpssDxChBrgFdbMacEntryWrite)
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvDxChBrgFdbMacEntryWrite
(
    IN GT_U8                    devNum,
    IN GT_U32                   index,
    IN GT_BOOL                  skip,
    IN CPSS_MAC_ENTRY_EXT_STC   *macEntryPtr,
    OUT GT_CHAR_PTR             *error_message
    P_CALLING_FORMAL_DATA
)
{
    GT_STATUS                   status          = GT_OK;
    PRV_TGF_BRG_MAC_ENTRY_STC   tgfMacEntry;

    *error_message = NULL;

    /* Bug: Unclear behavore of cpssDxChBrgMcIpv6BytesSelectSet...
    if (CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E == macEntryPtr->key.entryType)
    {
        GT_U8  dipBytesSelectMapArr[4];
        GT_U8  sipBytesSelectMapArr[4];

        status = prv_swap_byte_array(macEntryPtr->key.key.ipMcast.dip, 4,
                                     dipBytesSelectMapArr);

        if (GT_OK == status)
        {
            status = prv_swap_byte_array(macEntryPtr->key.key.ipMcast.sip, 4,
                                         sipBytesSelectMapArr);
        }

        if (GT_OK == status)
        {
            CPSS_API_CALL_LOCK();
            status = cpssDxChBrgMcIpv6BytesSelectSet(devNum,
                                                     dipBytesSelectMapArr,
                                                     sipBytesSelectMapArr);
            CPSS_API_CALL_UNLOCK();
        }

        if (GT_OK != status)
        {
            *error_message = multicastIPv6BytesWritingError;
        }
    }*/

    if (GT_OK == status && prvTgfBrgFdbDoNaShadow_part2)
    {
        /* prepare call to prvTgfBrgFdbMacEntryWrite_register */
        cpssOsMemSet(&tgfMacEntry, 0, sizeof(PRV_TGF_BRG_MAC_ENTRY_STC));

        status = prvTgfConvertCpssToGenericMacEntry(macEntryPtr, &tgfMacEntry);
        if (GT_OK != status)
        {
            *error_message = fdbMacEntryConvertCpssToTgfMessage;
        }
        else
        {
            prvCpssPrintFdbKey("prvTgfBrgFdbMacEntryWrite_register",&macEntryPtr->key,GT_FALSE);

            prvTgfBrgFdbMacEntryWrite_register(devNum, index, skip, &tgfMacEntry);
        }
    }

    if (GT_OK == status)
    {

        CPSS_API_CALL_LOCK();
        status = cpssDxChBrgFdbMacEntryWrite(devNum, index, skip, macEntryPtr);
        CPSS_API_CALL_UNLOCK();
        P_CALLING_API(
            cpssDxChBrgFdbMacEntryWrite,
            PC_NUMBER(devNum,               devNum,                      GT_U8,
            PC_NUMBER(index,                index,                      GT_U32,
            PC_BOOL  (skip,                 skip,
            PC_STRUCT(macEntryPtr,          macEntry,   CPSS_MAC_ENTRY_EXT_STC,
            PC_LAST_PARAM)))),
            PC_STATUS);
        if (GT_OK != status)
        {
            *error_message = fdbMacEntryWriteErrorMessage;
        }
    }

    return status;
}

/*******************************************************************************
* prvDxChBrgFdbMacEntrySet
*
* DESCRIPTION:
*        Wrapper of mac entry writing to hardware.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                - device number
*       macEntryPtr           - pointer to MAC entry parameters.
*       P_CALLING_FORMAL_DATA - general debug information (environment variables
*                               etc); could be empty
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       execution status (same as cpssDxChBrgFdbMacEntrySet)
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvDxChBrgFdbMacEntrySet
(
    IN GT_U8                    devNum,
    IN CPSS_MAC_ENTRY_EXT_STC   *macEntryPtr,
    OUT GT_CHAR_PTR             *error_message
    P_CALLING_FORMAL_DATA
)
{
    GT_STATUS                   status          = GT_OK;
    PRV_TGF_BRG_MAC_ENTRY_STC   tgfMacEntry;

    *error_message = NULL;

    /*Bug: Unclear behavore of cpssDxChBrgMcIpv6BytesSelectSet.
    if (CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E == macEntryPtr->key.entryType)
    {
       ... <call cpssDxChBrgMcIpv6BytesSelectSet>...
       See prvDxChBrgFdbMacEntryWrite implementation
    }
    */
    if (GT_OK == status && prvTgfBrgFdbDoNaShadow_part2)
    {
        /* prepare call to prvTgfBrgFdbMacEntrySet_register */
        cpssOsMemSet(&tgfMacEntry, 0, sizeof(PRV_TGF_BRG_MAC_ENTRY_STC));

        status = prvTgfConvertCpssToGenericMacEntry(macEntryPtr, &tgfMacEntry);
        if (GT_OK != status)
        {
            *error_message = fdbMacEntryConvertCpssToTgfMessage;
        }
        else
        {
            prvCpssPrintFdbKey("prvTgfBrgFdbMacEntrySet_register",&macEntryPtr->key,GT_FALSE);
            prvTgfBrgFdbMacEntrySet_register(devNum, 0xFFFFFFFF, &tgfMacEntry);
        }
    }

    if (GT_OK == status)
    {

        CPSS_API_CALL_LOCK();
        status = cpssDxChBrgFdbMacEntrySet(devNum, macEntryPtr);
        CPSS_API_CALL_UNLOCK();
        P_CALLING_API(
            cpssDxChBrgFdbMacEntrySet,
            PC_NUMBER(devNum,               devNum,                      GT_U8,
            PC_STRUCT(macEntryPtr,          macEntry,   CPSS_MAC_ENTRY_EXT_STC,
            PC_LAST_PARAM)),
            PC_STATUS);
        if (GT_OK != status)
        {
            *error_message = fdbMacEntryWriteErrorMessage;
        }
    }

    return status;
}

/*******************************************************************************
* prvDxChBrgFdbMacEntryDelete
*
* DESCRIPTION:
*        Wrapper of mac entry delete from hardware. (by message)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                - device number
*       macEntryKeyPtr        - pointer to MAC entry key parameters.
*       P_CALLING_FORMAL_DATA - general debug information (environment variables
*                               etc); could be empty
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       execution status (same as cpssDxChBrgFdbMacEntryDelete)
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvDxChBrgFdbMacEntryDelete
(
    IN GT_U8                    devNum,
    IN CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr,
    OUT GT_CHAR_PTR             *error_message
    P_CALLING_FORMAL_DATA
)
{
    GT_STATUS                   status          = GT_OK;
    PRV_TGF_MAC_ENTRY_KEY_STC   tgfKey;

    *error_message = NULL;

    if (GT_OK == status && prvTgfBrgFdbDoNaShadow_part2)
    {
        /* prepare call to prvTgfBrgFdbMacEntryDelete_unregister */
        cpssOsMemSet(&tgfKey, 0, sizeof(tgfKey));

        status = prvTgfConvertCpssToGenericMacKey(macEntryKeyPtr, &tgfKey);
        if (GT_OK != status)
        {
            *error_message = fdbMacEntryConvertCpssToTgfMessage;
        }
        else
        {
            prvCpssPrintFdbKey("prvTgfBrgFdbMacEntryDelete_unregister",macEntryKeyPtr,GT_FALSE);

            prvTgfBrgFdbMacEntryDelete_unregister(devNum, &tgfKey);

            if(cookie__for_fdb_shadow > 0)
            {
                /* if entry actually removed from the list , we need to allow to
                   get to the item after it.

                   NOTE: if item was not in the list ... then we just took 'reverse'.
                    (I hope that no real side effects)
                */
                cookie__for_fdb_shadow--;
            }
        }
    }

    if (GT_OK == status)
    {

        CPSS_API_CALL_LOCK();
        status = cpssDxChBrgFdbMacEntryDelete(devNum, macEntryKeyPtr);
        CPSS_API_CALL_UNLOCK();
        P_CALLING_API(
            cpssDxChBrgFdbMacEntryDelete,
            PC_NUMBER(devNum,               devNum,                      GT_U8,
            PC_STRUCT(macEntryKeyPtr,       macEntryKey,   CPSS_MAC_ENTRY_EXT_KEY_STC,
            PC_LAST_PARAM)),
            PC_STATUS);
        if (GT_OK != status)
        {
            *error_message = fdbMacEntryWriteErrorMessage;
        }
    }

    return status;
}


/*******************************************************************************
* prvCpssDxChBrgFdbNextEntryIndexGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of next fdb entry index.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum            - device number
*       searchOnlyValid   - property of searching only valid entries
*       index             - FDB entry index
*       indexWasFound     - invalid index founding property
*       error_message     - error message
*       P_CALLING_FORMAL_DATA
*                         - general debug information (environment variables
*                           etc); could be empty
*
* OUTPUTS:
*       status and FDB enrty index, if exists, to lua stack
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgFdbNextEntryIndexGet
(
    IN    GT_U8             devNum,
    IN    GT_BOOL           searchOnlyValid,
    INOUT GT_U32_PTR        index,
    OUT   GT_BOOL           *indexWasFound,
    OUT   GT_CHAR_PTR       *error_message
    P_CALLING_FORMAL_DATA
)
{
    GT_U32                  fdbTableSize    = 0;
    GT_BOOL                 valid           = GT_FALSE;
    GT_BOOL                 skip            = GT_TRUE;
    GT_STATUS               status          = GT_OK;

    if(prvTgfFdbShadowUsed == GT_TRUE)
    {
        CPSS_MAC_ENTRY_EXT_STC  *foundMacEntryPtr;


        if((*index) == 0 &&
            zeroRealIndex__for_fdb_shadow == GT_FALSE)
        {
            cookie__for_fdb_shadow = 0;
        }

        zeroRealIndex__for_fdb_shadow = GT_FALSE;
        DEBUG_PRINT(("local_fdbNextEntryGet_fromFdbShadow: cookie__for_fdb_shadow = %d \n",
                cookie__for_fdb_shadow));
        status = local_fdbNextEntryGet_fromFdbShadow(
            devNum,CPSS_DXCH_BRG_FDB_DEL_MODE_ALL_E,
                &cookie__for_fdb_shadow,
                index,&foundMacEntryPtr);
        if (GT_OK == status)
        {
            *indexWasFound = GT_TRUE;
            prvCpssPrintFdbKey("getNext",&foundMacEntryPtr->key,GT_FALSE);
            DEBUG_PRINT(("getNext: index = %d \n",
                (*index)));
        }
        else
        {
            DEBUG_PRINT(("getNext: NO MORE \n"));
            *indexWasFound = GT_FALSE;
        }

        return GT_OK;
    }

    status = prvCpssDxChBrgFdbTableSizeGet(devNum, &fdbTableSize,
                                           error_message);

    if ((GT_OK == status) && (GT_TRUE == searchOnlyValid))
    {
        while ((GT_OK == status) &&
               ((GT_FALSE == valid) || (GT_TRUE == skip)) &&
               (*index < fdbTableSize))
        {
            CPSS_API_CALL_LOCK();
            status =  cpssDxChBrgFdbMacEntryStatusGet(devNum, (*index)++, &valid,
                                                       &skip);
            CPSS_API_CALL_UNLOCK();
            P_CALLING_API(
                cpssDxChBrgFdbMacEntryStatusGet,
                PC_NUMBER(devNum,               devNum,                  GT_U8,
                PC_NUMBER(*index,               index,                  GT_U32,
                PC_BOOL  (valid,                valid,
                PC_BOOL  (skip,                 skip,
                PC_LAST_PARAM)))),
                PC_STATUS);
            if (GT_OK != status)
            {
                *error_message = fdbMacEntryReadErrorMessage;
                break;
            }
        }
    }

    if ((GT_OK == status) && (*index < fdbTableSize))
    {
        if (GT_TRUE == searchOnlyValid)
        {
            (*index)--;
        }

        *indexWasFound = GT_TRUE;
    }
    else
    {
        *indexWasFound = GT_FALSE;
    }

    return status;
}


/*******************************************************************************
* prvCpssDxChBrgNextEntryIndexAndAgedAndMacEntryFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of nex entry index and aged property and mac-entry from lua
*       stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                         - lua state
*       L_index                   - entry lua stack index
*       devNum                    - device number
*       searchOnlyValid           - property of searching only valid entries
*       macEntryIndexPtr          - next mac entry index
*       P_CALLING_FORMAL_DATA     - general debug information (environment
*                                   variables etc); could be empty
*
* OUTPUTS:
*       indexWasFoundPtr          - propety, that next mac entry was found
*       isSearchedAgedPropertyTakenPtr
*                                 - property, that searched pattern and mask of
*                                   aged property was found
*       patternAgedPtr            - searched pattern of aged property
*       isSearchedMacEntryTakenPtr
*                                 - property, that searched pattern and mask of
*                                   mac entry was found
*       patternMacEtnryPtr        - searched pattern of mac entry
*       maskMacEtnryPtr           - searched mask of mac entry
*       error_message             - error message
*
* RETURNS:
*       status and FDB enrty index, if exists, to lua stack
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgNextEntryIndexAndAgedAndMacEntryFromLuaGet
(
    IN    lua_State                 *L,
    IN    GT_32                     L_index,
    IN    GT_U8                     devNum,
    IN    GT_BOOL                   searchOnlyValid,
    INOUT GT_U32_PTR                macEntryIndexPtr,
    OUT   GT_BOOL                   *indexWasFoundPtr,
    OUT   GT_BOOL                   *isSearchedAgedPropertyTakenPtr,
    OUT   GT_BOOL                   *patternAgedPtr,
    OUT   GT_BOOL                   *maskAgedPtr,
    OUT   GT_BOOL                   *isSearchedMacEntryTakenPtr,
    OUT   CPSS_MAC_ENTRY_EXT_STC    *patternMacEtnryPtr,
    OUT   CPSS_MAC_ENTRY_EXT_STC    *maskMacEtnryPtr,
    OUT   GT_CHAR_PTR               *error_message
    P_CALLING_FORMAL_DATA
)
{
    GT_STATUS                       status = GT_OK;

    status = prvCpssDxChBrgFdbNextEntryIndexGet(devNum, searchOnlyValid,
                                                macEntryIndexPtr,
                                                indexWasFoundPtr,
                                                error_message
                                                P_CALLING_ACTUAL_DATA);

    if ((GT_OK == status) && (GT_TRUE == *indexWasFoundPtr))
    {
        status = prvCpssDxChBrgAgedAndMacEntryFromLuaGet
                    (L, L_index, isSearchedAgedPropertyTakenPtr, patternAgedPtr,
                     maskAgedPtr, isSearchedMacEntryTakenPtr, patternMacEtnryPtr,
                     maskMacEtnryPtr, error_message);
    }

    return status;
}


/*******************************************************************************
* prvCpssDxChBrgFdbNextMaskedEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of next fdb entry. Founded entry and its aged property
*       is masked and compared with pattern.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                      - device number
*       searchOnlyValid             - property of searching only valid entries
*       patternAged                 - pattern of aged property
*       maskAged                    - mask of aged property
*       patternMacEntry             - pattern fdb mac entry
*       maskMacEntry                - mask fdb mac entry
*       index                       - current FDB entry index
*       searchedValidPtr            - searched valid property
*       searchedSkipPtr             - searched skip property
*       searchedAgedSkipPtr         - searched aged property
*       searchedMacEnrtyPtr         - searched mac entry
*       entryWasFound               - mac entry founding property
*       error_message               - error message
*       P_CALLING_FORMAL_DATA       - general debug information (environment
*                                     variables etc); could be empty
*
* OUTPUTS:
*       status and FDB enrty index, if exists, to lua stack
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgFdbNextMaskedEntryGet
(
    IN    GT_U8                     devNum,
    IN    GT_BOOL                   searchOnlyValid,
    IN    GT_BOOL                   patternAged,
    IN    GT_BOOL                   maskAged,
    IN    CPSS_MAC_ENTRY_EXT_STC    *patternMacEntryPtr,
    IN    CPSS_MAC_ENTRY_EXT_STC    *maskMacEntryPtr,
    INOUT GT_U32_PTR                macEntryIndexPtr,
    OUT   GT_BOOL                   *searchedValidPtr,
    OUT   GT_BOOL                   *searchedSkipPtr,
    OUT   GT_BOOL                   *searchedAgedPtr,
    OUT   CPSS_MAC_ENTRY_EXT_STC    *searchedMacEnrtyPtr,
    OUT   GT_BOOL                   *entryWasFound,
    OUT   GT_CHAR_PTR               *error_message
    P_CALLING_FORMAL_DATA
)
{
    GT_BOOL                 nonLastIndex        = GT_TRUE;
    GT_HW_DEV_NUM           associatedHwDevNum  = 0;
    GT_STATUS               status              = GT_OK;

    *searchedValidPtr                           = GT_FALSE;
    *searchedSkipPtr                            = GT_TRUE;
    *entryWasFound                              = GT_FALSE;

    while ((GT_OK == status) && (GT_FALSE == *entryWasFound) &&
           (GT_TRUE == nonLastIndex))
    {
        status =  prvDxChBrgFdbMacEntryRead(devNum, (*macEntryIndexPtr)++,
                                            searchedValidPtr,
                                            searchedSkipPtr,
                                            searchedAgedPtr,
                                            &associatedHwDevNum,
                                            searchedMacEnrtyPtr,
                                            error_message
                                            P_CALLING_ACTUAL_DATA);

        if (GT_OK == status)
        {
            if (((GT_FALSE == searchOnlyValid) ||
                ((GT_TRUE  == *searchedValidPtr) &&
                 (GT_FALSE == *searchedSkipPtr))) &&
                (((GT_BOOL) -1 == maskAged) ||
                 ((*searchedAgedPtr == patternAged) &&
                 ((GT_BOOL) 0 == maskAged))))
            {
                *entryWasFound =
                    prv_lua_compare_mask(searchedMacEnrtyPtr,
                                         patternMacEntryPtr,
                                         maskMacEntryPtr,
                                         sizeof(*searchedMacEnrtyPtr))?
                    GT_TRUE:GT_FALSE;
            }
        }

        if ((GT_OK == status) && (GT_FALSE == *entryWasFound))
        {
            status = prvCpssDxChBrgFdbNextEntryIndexGet(devNum,
                                                        searchOnlyValid,
                                                        macEntryIndexPtr,
                                                        &nonLastIndex,
                                                        error_message
                                                        P_CALLING_ACTUAL_DATA);
        }
    }

    if (GT_TRUE == *entryWasFound)
    {
        (*macEntryIndexPtr)--;
    }

    return status;
}


/*******************************************************************************
* prvCpssDxChBrgFdbNextEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of next fdb entry index. Founded entry and its aged property
*       is masked and compared with pattern.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                      - device number
*       maskedSearch                - search with mask
*       searchOnlyValid             - property of searching only valid entries
*       patternAged                 - pattern of aged property
*       maskAged                    - mask of aged property
*       patternMacEntry             - pattern fdb mac entry
*       maskMacEntryPtr             - mask fdb mac entry
*       macEntryIndexPtr            - current FDB entry index
*       searchedValidPtr            - searched valid property
*       searchedSkipPtr             - searched skip property
*       searchedAgedSkipPtr         - searched aged property
*       searchedMacEnrtyPtr         - searched mac entry
*       entryWasFoundPtr            - mac entry founding property
*       error_message               - error message
*       P_CALLING_FORMAL_DATA       - general debug information (environment
*                                     variables etc); could be empty
*
* OUTPUTS:
*       status and FDB enrty index, if exists, to lua stack
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgFdbNextEntryGet
(
    IN    GT_U8                     devNum,
    IN    GT_BOOL                   maskedSearch,
    IN    GT_BOOL                   searchOnlyValid,
    IN    GT_BOOL                   patternAged,
    IN    GT_BOOL                   maskAged,
    IN    CPSS_MAC_ENTRY_EXT_STC    *patternMacEntryPtr,
    IN    CPSS_MAC_ENTRY_EXT_STC    *maskMacEntryPtr,
    INOUT GT_U32_PTR                macEntryIndexPtr,
    OUT   GT_BOOL                   *searchedValidPtr,
    OUT   GT_BOOL                   *searchedSkipPtr,
    OUT   GT_BOOL                   *searchedAgedPtr,
    OUT   CPSS_MAC_ENTRY_EXT_STC    *searchedMacEnrtyPtr,
    OUT   GT_BOOL                   *entryWasFoundPtr,
    OUT   GT_CHAR_PTR               *error_message
    P_CALLING_FORMAL_DATA
)
{
    GT_STATUS               status              = GT_OK;
    GT_HW_DEV_NUM           associatedHwDevNum    = 0;

    if (GT_TRUE == maskedSearch)
    {
        status = prvCpssDxChBrgFdbNextMaskedEntryGet(devNum, searchOnlyValid,
                                                     patternAged, maskAged,
                                                     patternMacEntryPtr,
                                                     maskMacEntryPtr,
                                                     macEntryIndexPtr,
                                                     searchedValidPtr,
                                                     searchedSkipPtr,
                                                     searchedAgedPtr,
                                                     searchedMacEnrtyPtr,
                                                     entryWasFoundPtr,
                                                     error_message
                                                     P_CALLING_ACTUAL_DATA);
    }
    else
    {
        status =  prvDxChBrgFdbMacEntryRead(devNum, *macEntryIndexPtr,
                                            searchedValidPtr, searchedSkipPtr,
                                            searchedAgedPtr, &associatedHwDevNum,
                                            searchedMacEnrtyPtr,
                                            error_message
                                            P_CALLING_ACTUAL_DATA);
        *entryWasFoundPtr = GT_TRUE;
    }

    return status;
}


/*******************************************************************************
* prvCpssDxChBrgFdbNextInvalidEntryIndexGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of next fdb invalid entry index.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum            - device number
*       index             - FDB entry index
*       indexWasFound     - invalid index founding property
*       error_message     - error message
*       P_CALLING_FORMAL_DATA
*                         - general debug information (environment variables
*                           etc); could be empty
*
* OUTPUTS:
*       status and FDB enrty index, if exists, to lua stack
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgFdbNextInvalidEntryIndexGet
(
    IN    GT_U8             devNum,
    INOUT GT_U32_PTR        index,
    OUT   GT_BOOL           *indexWasFound,
    OUT   GT_CHAR_PTR       *error_message
    P_CALLING_FORMAL_DATA
)
{
    GT_U32                  fdbTableSize    = 0;
    GT_BOOL                 valid           = GT_TRUE;
    GT_BOOL                 skip            = GT_TRUE;
    GT_STATUS               status          = GT_OK;

    status = prvCpssDxChBrgFdbTableSizeGet(devNum, &fdbTableSize,
                                           error_message);

    if (GT_OK == status)
    {
        while (((GT_TRUE == valid) || (GT_TRUE == skip)) &&
               (*index < fdbTableSize))
        {
            CPSS_API_CALL_LOCK();
            status =  cpssDxChBrgFdbMacEntryStatusGet(devNum, (*index)++, &valid,
                                                       &skip);
            CPSS_API_CALL_UNLOCK();
            P_CALLING_API(
                cpssDxChBrgFdbMacEntryStatusGet,
                PC_NUMBER(devNum,               devNum,                  GT_U8,
                PC_NUMBER(*index,               index,                  GT_U32,
                PC_BOOL  (valid,                valid,
                PC_BOOL  (skip,                 skip,
                PC_LAST_PARAM)))),
                PC_STATUS);
            if (GT_OK != status)
            {
                *error_message = fdbMacEntryReadErrorMessage;
                break;
            }
        }
    }

    if ((GT_FALSE == valid) && (GT_FALSE == skip))
    {
        (*index)--;
        *indexWasFound = GT_TRUE;
    }
    else
    {
        *indexWasFound = GT_FALSE;
    }

    return status;
}


/*******************************************************************************
* prvCpssDxChBrgFdbNextEntryAndItsIndexGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of next fdb entry and its index with getting data from lua.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                         - lua state
*       L_index                   - entry lua stack index
*       devNum                    - device number
*       searchOnlyValid           - property of searching only valid entries
*       macEntryIndexPtr          - next mac entry index
*       P_CALLING_FORMAL_DATA     - general debug information (environment
*                                   variables etc); could be empty
*
* OUTPUTS:
*       indexWasFoundPtr          - propety, that next mac entry index was found
*       entryWasFoundPtr          - propety, that next mac entry was found
*       searchedValidPtr          - searched valid property
*       searchedSkipPtr           - searched skip property
*       searchedAgedPtr           - searched aged property
*       patternMacEtnryPtr        - pattern mac entry from lua stack
*       maskMacEtnryPtr           - mask mac entry from lua stack
*       searchedMacEtnryPtr       - searched mac entry
*       errorMessagePtr           - error message
*
* OUTPUTS:
*       status and FDB enrty index, if exists, to lua stack
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int prvCpssDxChBrgFdbNextEntryAndItsIndexGet
(
    IN    lua_State                 *L,
    IN    GT_32                     L_index,
    IN    GT_U8                     devNum,
    IN    GT_BOOL                   searchOnlyValid,
    INOUT GT_U32_PTR                macEntryIndexPtr,
    OUT   GT_BOOL                   *indexWasFoundPtr,
    OUT   GT_BOOL                   *entryWasFoundPtr,
    OUT   GT_BOOL                   *searchedValidPtr,
    OUT   GT_BOOL                   *searchedSkipPtr,
    OUT   GT_BOOL                   *searchedAgedPtr,
    OUT   CPSS_MAC_ENTRY_EXT_STC    *patternMacEtnryPtr,
    OUT   CPSS_MAC_ENTRY_EXT_STC    *maskMacEtnryPtr,
    OUT   CPSS_MAC_ENTRY_EXT_STC    *searchedMacEtnryPtr,
    OUT   GT_CHAR_PTR               *errorMessagePtr
    P_CALLING_FORMAL_DATA
)
{
    GT_BOOL                         isSearchedAgedPropertyTaken = GT_FALSE;
    GT_BOOL                         patternAged                 = GT_FALSE;
    GT_BOOL                         maskAged                    = (GT_BOOL) - 1;
    GT_BOOL                         isSearchedMacEntryTaken     = GT_FALSE;
    GT_STATUS                       status                      = GT_OK;

    cpssOsMemSet(searchedMacEtnryPtr, 0xFF, sizeof(CPSS_MAC_ENTRY_EXT_STC));

    status = prvCpssDxChBrgNextEntryIndexAndAgedAndMacEntryFromLuaGet
                    (L, L_index, devNum, searchOnlyValid, macEntryIndexPtr,
                     indexWasFoundPtr, &isSearchedAgedPropertyTaken,
                     &patternAged, &maskAged, &isSearchedMacEntryTaken,
                     patternMacEtnryPtr, maskMacEtnryPtr, errorMessagePtr
                     P_CALLING_ACTUAL_DATA);

    if ((GT_OK == status) && (GT_TRUE == *indexWasFoundPtr))
    {
        status = prvCpssDxChBrgFdbNextEntryGet(devNum,
                                               isSearchedAgedPropertyTaken ||
                                                   isSearchedMacEntryTaken,
                                               searchOnlyValid, patternAged,
                                               maskAged, patternMacEtnryPtr,
                                               maskMacEtnryPtr,
                                               macEntryIndexPtr,
                                               searchedValidPtr,
                                               searchedSkipPtr,
                                               searchedAgedPtr,
                                               searchedMacEtnryPtr,
                                               entryWasFoundPtr,
                                               errorMessagePtr
                                               P_CALLING_ACTUAL_DATA);
        *indexWasFoundPtr = *entryWasFoundPtr;
    }

    return status;
}


/*******************************************************************************
* prvCpssDxChBrgFdbNextEntryAndItsIndexPushToLua
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of next fdb entry and its index with getting data from lua,
*       pushing result to lua stack.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                   - lua state (in stack should be device number,
*                             starting index, optional parrenrs and masks)
*       searchOnlyValid     - property of searching only valid entries
*       P_CALLING_FORMAL_DATA
*                           - general debug information (environment variables
*                             etc); could be empty
*
* OUTPUTS:
*       status and FDB enrty index, if exists, to lua stack
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int prvCpssDxChBrgFdbNextEntryAndItsIndexPushToLua
(
    IN  lua_State           *L,
    IN GT_BOOL              searchOnlyValid
    P_CALLING_FORMAL_DATA
)
{
    GT_U8                   devNum                  =
                                                    (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_U32                  index                   =
                                                    (GT_U32) lua_tonumber(L, 2);
                                                                    /* index */
    GT_BOOL                 indexWasFound           = GT_FALSE;
    GT_BOOL                 searchedValid           = GT_FALSE;
    GT_BOOL                 searchedSkip            = GT_TRUE;
    GT_BOOL                 searchedAged            = GT_FALSE;
    CPSS_MAC_ENTRY_EXT_STC  patternMacEtnry, maskMacEtnry;
    CPSS_MAC_ENTRY_EXT_STC  searchedMacEntry;
    GT_BOOL                 entryWasFound           = GT_FALSE;
    GT_STATUS               status                  = GT_OK;
    GT_CHAR_PTR             error_message           = NULL;
    int                     returned_param_count    = 0;

    status = prvCpssDxChBrgFdbNextEntryAndItsIndexGet(L, 3, devNum,
                                                      searchOnlyValid, &index,
                                                      &indexWasFound,
                                                      &entryWasFound,
                                                      &searchedValid,
                                                      &searchedSkip,
                                                      &searchedAged,
                                                      &patternMacEtnry,
                                                      &maskMacEtnry,
                                                      &searchedMacEntry,
                                                      &error_message
                                                      P_CALLING_ACTUAL_DATA);

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssNumberToLuaOnConditionPush(status, L, index, indexWasFound);
    returned_param_count    +=
        pvrCpssValidSkipAgedToLuaOnConditionPush(status, L, searchedValid,
                                                 searchedSkip, searchedAged,
                                                 entryWasFound);
    returned_param_count    +=
        pvrCpssMacEntryToLuaOnConditionPush(status, L, &searchedMacEntry,
                                            entryWasFound);
    return returned_param_count;
}


/*******************************************************************************
* prvCpssDxChBrgIsEntryExists
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Checking of entry existence.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                 - lua state (in stack should be device number,
*                           starting index, optional parrenrs and masks)
*       searchOnlyValid   - property of searching only valid entries
*       P_CALLING_FORMAL_DATA
*                         - general debug information (environment variables
*                           etc); could be empty
*
* OUTPUTS:
*       isExists            - existing property
*       errorMessagePtr     - error message
*
* RETURNS:
*       status and FDB enrty index, if exists, to lua stack
*
* COMMENTS:
*       in case the entry is multicast, it also returns VIDX
*
*******************************************************************************/
int prvCpssDxChBrgIsEntryExists
(
    IN  lua_State           *L,
    IN  GT_BOOL             searchOnlyValid,
    OUT GT_BOOL             *isExists,
    OUT GT_U32              *FDBindex,
    OUT GT_U32              *vidx,
    OUT GT_CHAR_PTR         *errorMessagePtr
    P_CALLING_FORMAL_DATA
)
{
    GT_U8                   devNum              = (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_U32                  index               = (GT_U32) lua_tonumber(L, 2);
                                                                    /* index */
    GT_BOOL                 searchedValid       = GT_FALSE;
    GT_BOOL                 searchedSkip        = GT_FALSE;
    GT_BOOL                 searchedAged        = GT_FALSE;
    GT_U32                  searchedDevNum, j;
    GT_BOOL                 isMacEntryTaken;
    CPSS_MAC_ENTRY_EXT_STC  searchedMacEntry, macEntry;
    GT_U32                  hashLenth;
    CPSS_MAC_VL_ENT         vlanLearningMode;

    GT_STATUS               status              = GT_OK;

    *errorMessagePtr = NULL;

    status = prvCpssDxChBrgFdbMacEntryFromLuaGet(L, 3, &isMacEntryTaken,
                                                 &macEntry, errorMessagePtr);
    if ((GT_OK != status) || (GT_FALSE == isMacEntryTaken))
    {
        status        = GT_BAD_PARAM;
        *errorMessagePtr = fdbMacEntryFromLuaGettingErrorMessage;
    }

    prvCpssPrintFdbKey("prvCpssDxChBrgIsEntryExists",&macEntry.key,GT_FALSE);

    status = cpssDxChBrgFdbMaxLookupLenGet(devNum, &hashLenth);
    if (GT_OK != status)
    {
        status        = GT_BAD_PARAM;
        *errorMessagePtr = fdbMacEntryFromLuaGettingErrorMessage;
    }
    status = cpssDxChBrgFdbHashCalc(devNum, &(macEntry.key), &index);
    if (GT_OK != status)
    {
        status        = GT_BAD_PARAM;
        *errorMessagePtr = fdbMacEntryFromLuaGettingErrorMessage;
    }
    *isExists = GT_FALSE;

/* for shared VLAN learning mode no need to compare VIDs of entries*/
    status = cpssDxChBrgFdbMacVlanLookupModeGet(devNum, &vlanLearningMode);
    if (GT_OK != status)
    {
        status        = GT_BAD_PARAM;
        *errorMessagePtr = fdbMacEntryFromLuaGettingErrorMessage;
    }

    for(j=0;j<hashLenth;j++)
    {
           status =  prvDxChBrgFdbMacEntryRead(devNum, index+j,
                                               &searchedValid,
                                               &searchedSkip,
                                               &searchedAged,
                                               &searchedDevNum,
                                               &searchedMacEntry,
                                               errorMessagePtr
                                               P_CALLING_ACTUAL_DATA);

           if (GT_OK == status)
           {
               if (  (GT_TRUE  == searchedValid) &&
                     (GT_FALSE == searchedSkip) /*&&
                     (GT_FALSE == searchedAged)*/ )/*age bit is not indication to anything*/
               {
                   if((osMemCmp(&(macEntry.key.key.macVlan.macAddr.arEther), &(searchedMacEntry.key.key.macVlan.macAddr.arEther), sizeof(GT_ETHERADDR)) == 0) &&
                      ((osMemCmp(&(macEntry.key.key.macVlan.vlanId), &(searchedMacEntry.key.key.macVlan.vlanId), sizeof(GT_U16)) == 0) || (vlanLearningMode == CPSS_SVL_E)))
                   {
                       *isExists = GT_TRUE;
                       *FDBindex = index+j;
                       if(searchedMacEntry.dstInterface.type == CPSS_INTERFACE_VIDX_E)
                           *vidx = searchedMacEntry.dstInterface.vidx;
                       break;
                   }

               }
           }
    }
    return status;
}


/*******************************************************************************
* prvCpssDxChBrgFdbTakenOrCreatedEntryAndItsIndexPushToLua
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of next fdb entry and its index with getting data from lua or
*       its creating, if not exists. Results is pushed to stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                 - lua state (in stack should be device number,
*                           starting index, optional parrenrs and masks)
*       searchOnlyValid   - property of searching only valid entries
*       P_CALLING_FORMAL_DATA
*                         - general debug information (environment variables
*                           etc); could be empty
*
* OUTPUTS:
*       status and FDB enrty index, if exists, to lua stack
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int prvCpssDxChBrgFdbTakenOrCreatedEntryAndItsIndexPushToLua
(
    IN  lua_State           *L,
    IN GT_BOOL              searchOnlyValid
    P_CALLING_FORMAL_DATA
)
{
    GT_U8                   devNum                  =
                                                    (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_U32                  index                   =
                                                    (GT_U32) lua_tonumber(L, 2);
                                                                    /* index */
    GT_BOOL                 indexWasFound           = GT_FALSE;
    GT_BOOL                 searchedValid           = GT_FALSE;
    GT_BOOL                 searchedSkip            = GT_FALSE;
    GT_BOOL                 isSearchedAgedPropertyTaken
                                                    = GT_FALSE;
    GT_BOOL                 maskAged                = GT_FALSE;
    GT_BOOL                 searchedAged            = GT_FALSE;
    GT_BOOL                 entryWasTaken           = GT_FALSE;
    GT_BOOL                 entryWasFound           = GT_FALSE;
    CPSS_MAC_ENTRY_EXT_STC  patternMacEtnry, maskMacEtnry;
    CPSS_MAC_ENTRY_EXT_STC  searchedMacEntry;
    GT_STATUS               status                  = GT_OK;
    GT_CHAR_PTR             error_message           = NULL;
    int                     returned_param_count    = 0;

    cpssOsMemSet(&maskMacEtnry, 0xFF, sizeof(CPSS_MAC_ENTRY_EXT_STC));

    status = prvCpssDxChBrgFdbNextEntryAndItsIndexGet(L, 3, devNum,
                                                      searchOnlyValid, &index,
                                                      &indexWasFound,
                                                      &entryWasFound,
                                                      &searchedValid,
                                                      &searchedSkip,
                                                      &searchedAged,
                                                      &patternMacEtnry,
                                                      &maskMacEtnry,
                                                      &searchedMacEntry,
                                                      &error_message
                                                      P_CALLING_ACTUAL_DATA);

    if ((GT_OK == status) && (GT_FALSE == entryWasFound))
    {
        if (GT_FALSE == indexWasFound)
        {
            index  = 0;

            status = prvCpssDxChBrgAgedAndMacEntryFromLuaGet
                         (L, 3, &isSearchedAgedPropertyTaken, &searchedAged,
                          &maskAged, &entryWasTaken, &patternMacEtnry,
                          &maskMacEtnry, &error_message);
        }

        if ((GT_OK == status) && (GT_FALSE == indexWasFound))
        {
            status = prvCpssDxChBrgFdbNextInvalidEntryIndexGet(
                         devNum, &index, &indexWasFound, &error_message
                         P_CALLING_ACTUAL_DATA);
        }

        if (GT_OK == status)
        {
            status = prv_lua_apply_mask(&patternMacEtnry, &searchedMacEntry,
                                        &maskMacEtnry,
                                        sizeof(searchedMacEntry));
        }

        if (GT_OK == status)
        {
            status =  prvDxChBrgFdbMacEntryWrite(devNum, index, searchedSkip,
                                                 &searchedMacEntry,
                                                 &error_message
                                                 P_CALLING_ACTUAL_DATA);
        }
    }

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssNumberToLuaOnConditionPush(status, L, index, indexWasFound);
    returned_param_count    +=
        pvrCpssBooleanToLuaPush(status, L, searchedSkip);
    returned_param_count    +=
        pvrCpssMacEntryToLuaPush(status, L, &searchedMacEntry);
    return returned_param_count;
}


/*******************************************************************************
* prvCpssDxChBrgMacAddressTableCountStatisticUpdate
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Updating of mac address table count statistic according to
*       valid/skip/aged property and mac-entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        valid                      - valid property
*        skip                       - skip property
*        aged                       - aged property
*        macEnrtyPtr                - mac entry
*        statisticPtr              - updated statistic
*
* OUTPUTS:
*       status and FDB enrty index, if exists, to lua stack
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgMacAddressTableCountStatisticUpdate
(
    IN    GT_BOOL                                       valid,
    IN    GT_BOOL                                       skip,
    IN    GT_BOOL                                       aged,
    IN    CPSS_MAC_ENTRY_EXT_STC                        *macEnrtyPtr,
    INOUT WRAPL_CPSS_MAC_ADDRESS_TABLE_COUT_STATICTIC   *statisticPtr
)
{
    GT_STATUS                   status          = GT_OK;

    statisticPtr->total_entries_count++;

    if ((GT_FALSE == valid) && (GT_FALSE == skip))
    {
        statisticPtr->free_entries_count++;
    }

    if ((GT_TRUE  == valid) && (GT_FALSE == skip) )
    {
        statisticPtr->used_entries_count++;
    }

    if ((GT_FALSE == skip) && (GT_TRUE == macEnrtyPtr->isStatic) && (GT_TRUE == valid))
    {
        statisticPtr->static_entries_count++;
    }

    if ((GT_FALSE == skip) && (GT_FALSE == macEnrtyPtr->isStatic) && (GT_TRUE == valid))
    {
        statisticPtr->dynamic_entries_count++;
    }

    if (GT_TRUE == skip)
    {
        statisticPtr->skipped_entries_count++;
    }

    aged = 0;

    return status;
}


/*******************************************************************************
* wrlCpssDxChBrgFdbTableSizeGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of FDB tables facility size.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                   - lua state
*
* OUTPUTS:
*       status and FDB tables facility size to lua stack
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChBrgFdbTableSizeGet
(
    IN lua_State            *L
)
{
    GT_U8                   devNum                  =
                                                    (GT_U8) lua_tonumber(L, 1);
                                                                    /* devId */
    GT_U32                  fdbTableSize            = 0;
    GT_STATUS               status                  = GT_OK;
    GT_CHAR_PTR             error_message           = NULL;
    int                     returned_param_count    = 0;

    status = prvCpssDxChBrgFdbTableSizeGet(devNum, &fdbTableSize,
                                           &error_message);

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssNumberToLuaPush(status, L, fdbTableSize);
    return returned_param_count;
}


/*******************************************************************************
* wrlCpssDxChBrgFdbValidEntryIndexGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Checking if fdb index valid.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                   - lua state
*
* OUTPUTS:
*       status and FDB index validity to lua stack
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChBrgFdbValidEntryIndexGet
(
    IN lua_State            *L
)
{
    GT_U8                   devNum                  =
                                                    (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_U32                  index                   =
                                                    (GT_U32) lua_tonumber(L, 2);
                                                                    /* index */
    GT_BOOL                 valid                   = GT_FALSE;
    GT_BOOL                 skip                    = GT_FALSE;
    GT_STATUS               status                  = GT_OK;
    GT_CHAR_PTR             error_message           = NULL;
    int                     returned_param_count    = 0;
    P_CALLING_CHECKING;

    CPSS_API_CALL_LOCK();
    status =  cpssDxChBrgFdbMacEntryStatusGet(devNum, index++, &valid, &skip);
    CPSS_API_CALL_UNLOCK();
    P_CALLING_API(
        cpssDxChBrgFdbMacEntryStatusGet,
        PC_NUMBER(devNum,                   devNum,                     GT_U8,
        PC_NUMBER(index,                    index,                      GT_U32,
        PC_BOOL  (valid,                    valid,
        PC_BOOL  (skip,                     skip,
        PC_LAST_PARAM)))),
        PC_STATUS);
    if (GT_OK != status)
    {
        error_message = fdbMacEntryReadErrorMessage;
    }

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssBooleanToLuaPush(status, L, valid);
    return returned_param_count;
}


/*******************************************************************************
* wrlCpssDxChBrgFdbNextEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of next fdb entry index (valid and invalid).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                   - lua state (in stack should be device number,
*                             starting index, optional parrenrs and masks)
*
* OUTPUTS:
*       number of returned to lua elements, status FDB enrty index,
*       valid/sjip/aged struct anf mac-entry to lua stack, if exists,
*       otherwise 0
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChBrgFdbNextEntryGet
(
    IN lua_State            *L
)
{
    P_CALLING_CHECKING;

    return prvCpssDxChBrgFdbNextEntryAndItsIndexPushToLua(
               L, GT_FALSE P_CALLING_ACTUAL_DATA);
}


/*******************************************************************************
* wrlCpssDxChBrgFdbNextValidEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of next fdb valid entry index.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                   - lua state (in stack should be device number,
*                             starting index, optional parrenrs and masks)
*
* OUTPUTS:
*       number of returned to lua elements, status FDB enrty index,
*       valid/sjip/aged struct anf mac-entry to lua stack, if exists,
*       otherwise 0
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChBrgFdbNextValidEntryGet
(
    IN lua_State            *L
)
{
    P_CALLING_CHECKING;

    return prvCpssDxChBrgFdbNextEntryAndItsIndexPushToLua(
               L, GT_TRUE P_CALLING_ACTUAL_DATA);
}


/*******************************************************************************
* wrlCpssDxChBrgFdbNextInvalidEntryIndexGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of next fdb invalid entry index.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                   - lua state
*
* OUTPUTS:
*       number of returned to lua elements, status and FDB enrty index to
*       lua stack, if exists, otherwise 0
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChBrgFdbNextInvalidEntryIndexGet
(
    IN lua_State            *L
)
{
    GT_U8                   devNum                  =
                                                    (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_U32                  index                   =
                                                    (GT_U32) lua_tonumber(L, 2);
                                                                    /* index */
    GT_BOOL                 indexWasFound           = GT_FALSE;
    GT_STATUS               status                  = GT_OK;
    GT_CHAR_PTR             error_message           = NULL;
    int                     returned_param_count    = 0;
    P_CALLING_CHECKING;

    status = prvCpssDxChBrgFdbNextInvalidEntryIndexGet(devNum, &index,
                                                       &indexWasFound,
                                                       &error_message
                                                       P_CALLING_ACTUAL_DATA);

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssNumberToLuaOnConditionPush(status, L, index, indexWasFound);
    return returned_param_count;
}


/*******************************************************************************
* wrlCpssDxChBrgFdbInvalidEntryIndexGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of first fdb invalid entry index.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                   - lua state
*
* OUTPUTS:
*       number of returned to lua elements, status and FDB enrty index to
*       lua stack, if exists, otherwise 0
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChBrgFdbInvalidEntryIndexGet
(
    IN lua_State            *L
)
{
    GT_U8                   devNum                  =
                                                    (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_U32                  index                   = 0;
    GT_BOOL                 indexWasFound           = GT_FALSE;
    GT_STATUS               status                  = GT_OK;
    GT_CHAR_PTR             error_message           = NULL;
    int                     returned_param_count    = 0;
    P_CALLING_CHECKING;

    status = prvCpssDxChBrgFdbNextInvalidEntryIndexGet(devNum, &index,
                                                       &indexWasFound,
                                                       &error_message
                                                       P_CALLING_ACTUAL_DATA);

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssNumberToLuaOnConditionPush(status, L, index, indexWasFound);
    return returned_param_count;
}


/*******************************************************************************
* wrlCpssDxChBrgFdbAllDevicesInvalidEntryIndexGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of next fdb invalid entry index in device range.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                   - lua state
*
* OUTPUTS:
*       number of returned to lua elements, status and FDB enrty index to
*       lua stack, if exists, otherwise 0
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChBrgFdbAllDevicesInvalidEntryIndexGet
(
    IN lua_State            *L
)
{
    const int               L_index                 = 1;
    GT_U32                  index                   = 0;
    GT_U8                   devNum                  = 0;
    GT_BOOL                 indexWasFound           = GT_FALSE;
    GT_STATUS               status                  = GT_OK;
    GT_CHAR_PTR             error_message           = NULL;
    int                     returned_param_count    = 0;
    P_CALLING_CHECKING;

    if (0 != lua_istable(L, L_index))
    {
        lua_pushnil(L);
        while ((GT_OK == status) && (0 != lua_next(L, L_index)) &&
               (GT_FALSE == indexWasFound))
        {
            if(0 != lua_isnumber(L, -1))
            {
                index = 0;
                devNum = (GT_U8) lua_tonumber(L, -1);
                status = prvCpssDxChBrgFdbNextInvalidEntryIndexGet(
                             devNum, &index, &indexWasFound, &error_message
                             P_CALLING_ACTUAL_DATA);
            }
            lua_pop( L, 1 );
        }
        lua_pop( L, 1);
    }

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssNumberToLuaOnConditionPush(status, L, index, indexWasFound);
    return returned_param_count;
}


/*******************************************************************************
* wrlCpssDxChBrgFdbMacAddressTableCountGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of fdb mac address-table count command statistic.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                   - lua state (in stack should be device number,
*                             starting index, optional parrenrs and masks)
*
* OUTPUTS:
*       2 and status with count statistic lua stack
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChBrgFdbMacAddressTableCountGet
(
    IN lua_State            *L
)
{
    GT_U8                   devNum                  =
                                                    (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_U32                  index                   =
                                                    (GT_U32) lua_tonumber(L, 2);
                                                                    /* index */
    GT_BOOL                 indexWasFound           = GT_FALSE;
    GT_BOOL                 searchedValid           = GT_FALSE;
    GT_BOOL                 searchedSkip            = GT_TRUE;
    GT_BOOL                 searchedAged            = GT_FALSE;
    GT_BOOL                 isSearchedAgedPropertyTaken
                                                    = GT_FALSE;
    GT_BOOL                 patternAged             = GT_FALSE;
    GT_BOOL                 maskAged                = (GT_BOOL) -1;
    GT_BOOL                 isSearchedMacEntryTaken
                                                    = GT_FALSE;
    CPSS_MAC_ENTRY_EXT_STC  patternMacEtnry, maskMacEtnry;
    CPSS_MAC_ENTRY_EXT_STC  searchedMacEntry;
    GT_BOOL                 entryWasFound           = GT_FALSE;
    WRAPL_CPSS_MAC_ADDRESS_TABLE_COUT_STATICTIC
                            statistic;
    GT_STATUS               status                  = GT_OK;
    GT_CHAR_PTR             error_message           = NULL;
    int                     returned_param_count    = 0;
    P_CALLING_CHECKING;

    cpssOsMemSet(&searchedMacEntry, 0xFF, sizeof(searchedMacEntry));
    cpssOsMemSet(&statistic, 0, sizeof(statistic));

    status = pvrCpssMacAddressTableCountStatisticFromLuaGet(L, 3, &statistic,
                                                                &error_message);

    if (GT_OK == status)
    {
        status = prvCpssDxChBrgNextEntryIndexAndAgedAndMacEntryFromLuaGet
                    (L, 4, devNum, GT_FALSE, &index, &indexWasFound,
                     &isSearchedAgedPropertyTaken, &patternAged, &maskAged,
                     &isSearchedMacEntryTaken, &patternMacEtnry, &maskMacEtnry,
                     &error_message P_CALLING_ACTUAL_DATA);
    }


    if (GT_OK == status)
    {
        while (GT_TRUE == indexWasFound)
        {
            status = prvCpssDxChBrgFdbNextEntryGet(devNum,
                                                   isSearchedAgedPropertyTaken ||
                                                       isSearchedMacEntryTaken,
                                                   GT_FALSE, patternAged,
                                                   maskAged, &patternMacEtnry,
                                                   &maskMacEtnry, &index,
                                                   &searchedValid,
                                                   &searchedSkip,
                                                   &searchedAged,
                                                   &searchedMacEntry,
                                                   &entryWasFound,
                                                   &error_message
                                                   P_CALLING_ACTUAL_DATA);

            if ((GT_OK == status) && (GT_TRUE == entryWasFound))
            {
                status = prvCpssDxChBrgMacAddressTableCountStatisticUpdate
                            (searchedValid, searchedSkip, searchedAged,
                             &searchedMacEntry, &statistic);

                index++;

                status = prvCpssDxChBrgFdbNextEntryIndexGet(
                             devNum, GT_FALSE, &index, &indexWasFound,
                             &error_message P_CALLING_ACTUAL_DATA);
            }
            else
            {
                indexWasFound = GT_FALSE;
            }
        }
    }

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssMacAddressTableCountStatisticToLuaPush(status, L, &statistic);
    return returned_param_count;
}


/*******************************************************************************
* wrlCpssDxChBrgIsEntryExists
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Checking of entry existence.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                  - lua state (in stack should be device number,
*                             starting index, optional parrenrs and masks)
*
* OUTPUTS:
*        2; status and existing property pushed to lua stack
*        2; error code and error message pushed to lua stack
*
* RETURNS:
*       status and FDB enrty index, if exists, to lua stack
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChBrgIsEntryExists
(
    IN  lua_State           *L
)
{
    GT_BOOL                 isExists                = GT_FALSE;
    GT_STATUS               status                  = GT_OK;
    GT_CHAR_PTR             error_message           = NULL;
    OUT GT_U32              FDBindex                = 0;
    OUT GT_U32              vidx                    = 0xFFFF;
    int                     returned_param_count    = 0;
    P_CALLING_CHECKING;

    status = prvCpssDxChBrgIsEntryExists(L, GT_TRUE, &isExists, &FDBindex, &vidx, &error_message
                                         P_CALLING_ACTUAL_DATA);


    DEBUG_PRINT(("prvCpssDxChBrgIsEntryExists: macEntry : isExists = %d , FDBindex = %d \n",
           isExists , FDBindex ));


    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssBooleanToLuaPush(status, L, isExists);
    returned_param_count    +=
        pvrCpssNumberToLuaPush(status, L, FDBindex);
    returned_param_count    +=
        pvrCpssNumberToLuaPush(status, L, vidx);
    return returned_param_count;
}


/*******************************************************************************
* wrlCpssDxChBrgFdbEntryGreateOrGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of next fdb entry and its index with getting data from lua or
*       its creating, if not exists. Results is pushed to stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                  - lua state (in stack should be device number,
*                             starting index, optional parrenrs and masks)
*
* OUTPUTS:
*       status and FDB enrty index, if exists, to lua stack
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChBrgFdbEntryGreateOrGet
(
    IN  lua_State           *L
)
{
    P_CALLING_CHECKING;

    return prvCpssDxChBrgFdbTakenOrCreatedEntryAndItsIndexPushToLua(
               L, GT_TRUE P_CALLING_ACTUAL_DATA);
}


/*******************************************************************************
* wrlCpssDxChBrgFdbMacEntryWrite
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Writing of mac entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                  - lua state (in stack should be device number,
*                             starting index, optional parrenrs and masks)
*
* OUTPUTS:
*       status and FDB enrty index, if exists, to lua stack
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChBrgFdbMacEntryWrite
(
    IN  lua_State           *L
)
{
    GT_U8                   devNum              = (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_U32                  index               = (GT_U32) lua_tonumber(L, 2);
                                                                    /* index */
    GT_BOOL                 skip                = (GT_BOOL) lua_tonumber(L, 3);
                                                                    /* skip */
    GT_BOOL                 isMacEntryTaken     = GT_FALSE;
    CPSS_MAC_ENTRY_EXT_STC  macEntry;
    GT_STATUS               status              = GT_OK;
    GT_CHAR_PTR             error_message       = NULL;
    P_CALLING_CHECKING;

    status = prvCpssDxChBrgFdbMacEntryFromLuaGet(L, 4, &isMacEntryTaken,
                                                 &macEntry, &error_message);
    if ((GT_OK != status) || (GT_FALSE == isMacEntryTaken))
    {
        status        = GT_BAD_PARAM;
        error_message = fdbMacEntryFromLuaGettingErrorMessage;
    }

    if (GT_OK == status)
    {
        status = prvDxChBrgFdbMacEntryWrite(devNum, index, skip, &macEntry,
                                            &error_message
                                            P_CALLING_ACTUAL_DATA);
    }

    return prvWrlCpssStatusToLuaPush(status, L, error_message);
}


/*******************************************************************************
* wrlCpssDxChBrgFdbMacEntrySet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Writing of mac entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                  - lua state (in stack should be device number,
*                             mac entry)
*
* OUTPUTS:
*       status and FDB enrty index, if exists, to lua stack
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChBrgFdbMacEntrySet
(
    IN  lua_State           *L
)
{
    GT_U8                   devNum              = (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_BOOL                 isMacEntryTaken     = GT_FALSE;
    CPSS_MAC_ENTRY_EXT_STC  macEntry;
    GT_STATUS               status              = GT_OK;
    GT_CHAR_PTR             error_message       = NULL;
    P_CALLING_CHECKING;

    status = prvCpssDxChBrgFdbMacEntryFromLuaGet(L, 2, &isMacEntryTaken,
                                                 &macEntry, &error_message);
    if ((GT_OK != status) || (GT_FALSE == isMacEntryTaken))
    {
        status        = GT_BAD_PARAM;
        error_message = fdbMacEntryFromLuaGettingErrorMessage;
    }

    if (GT_OK == status)
    {
        status = prvDxChBrgFdbMacEntrySet(devNum, &macEntry, &error_message
                                          P_CALLING_ACTUAL_DATA);
    }

    return prvWrlCpssStatusToLuaPush(status, L, error_message);
}

/*******************************************************************************
* wrlCpssDxChBrgFdbMacEntryDelete
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Delete of mac key (by message).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                  - lua state (in stack should be device number,
*                             mac entry)
*
* OUTPUTS:
*       status and FDB enrty index, if exists, to lua stack
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChBrgFdbMacEntryDelete
(
    IN  lua_State           *L
)
{
    GT_U8                   devNum              = (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_BOOL                 isMacEntryTaken     = GT_FALSE;
    CPSS_MAC_ENTRY_EXT_KEY_STC  macEntryKey;
    GT_STATUS               status              = GT_OK;
    GT_CHAR_PTR             error_message       = NULL;
    P_CALLING_CHECKING;

    status = prvCpssDxChBrgFdbMacEntryKeyFromLuaGet(L, 2, &isMacEntryTaken ,
                                                 &macEntryKey, &error_message);
    if ((GT_OK != status) || (GT_FALSE == isMacEntryTaken))
    {
        status        = GT_BAD_PARAM;
        error_message = fdbMacEntryFromLuaGettingErrorMessage;
    }

    if (GT_OK == status)
    {
        status = prvDxChBrgFdbMacEntryDelete(devNum, &macEntryKey, &error_message
                                          P_CALLING_ACTUAL_DATA);
    }

    return prvWrlCpssStatusToLuaPush(status, L, error_message);
}

/*******************************************************************************
* wrlDxChBrgFdbAgingTimeoutCheck
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Checking of fdb aging timeout validity.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                  - lua state (in stack should be device number and
*                             timeout value)
*
* OUTPUTS:
*       status and FDB enrty index, if exists, to lua stack
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlDxChBrgFdbAgingTimeoutCheck
(
    IN  lua_State           *L
)
{
    GT_U8                   devNum                  =
                                                    (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_U32                  timeout                 =
                                                    (GT_U32) lua_tonumber(L, 2);
                                                                    /* index */
    GT_BOOL                 isTimeoutCorrect        = GT_TRUE;
    GT_STATUS               status                  = GT_OK;
    GT_CHAR_PTR             error_message           = NULL;
    int                     returned_param_count    = 0;

    if (CPSS_PP_FAMILY_CHEETAH3_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        if (timeout < 16 / 2)
        {
            isTimeoutCorrect = GT_FALSE;
        }
    }
    else
    {
        if (timeout < 10 / 2)
        {
            isTimeoutCorrect = GT_FALSE;
        }
    }

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssBooleanToLuaPush(status, L, isTimeoutCorrect);
    return returned_param_count;
}


/*******************************************************************************
* wraplCpssDxChBrgFdbFlush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Flushes fdb table
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                  - lua state (in stack should be device number and
*                             includeStatic value)
*
* OUTPUTS:
*       status
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wraplCpssDxChBrgFdbFlush
(
                IN  lua_State           *L
)
{
    GT_U8                   devNum                  =
                                                    (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    CPSS_DXCH_BRG_FDB_DEL_MODE_ENT mode = lua_tonumber(L, 2);

    GT_U32                          origActDev;
    GT_U32                          origActDevMask;
    GT_U16                          origVlanId;
    GT_U16                          origVlanMask;
    GT_STATUS                       rc;
    GT_BOOL                         messagesToCpuEnabled;
    GT_BOOL                         isAuqFull;
    CPSS_DXCH_BRG_FDB_DEL_MODE_ENT  modeEnabled;
    GT_U32                          origActUerDefined;
    GT_U32                          origActUerDefinedMask;
#ifdef ASIC_SIMULATION
    GT_U32      maxRetry      = 100;
#endif
#ifdef GM_USED
    static GT_U32 isGmUsed = 1;
#else
    static GT_U32 isGmUsed = 0;
#endif

    GT_BOOL     isCompleted   = GT_FALSE;

    CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT origTrunkAgingMode;
    modeEnabled = mode;
    messagesToCpuEnabled = GT_FALSE;

    if(prvTgfFdbShadowUsed == GT_TRUE)
    {
        /* check the mode is applicable for the device */
        if ( (CPSS_DXCH_BRG_FDB_DEL_MODE_STATIC_ONLY_E == mode) &&
             !PRV_CPSS_SIP_5_CHECK_MAC(devNum))
        {
            rc = GT_NOT_SUPPORTED;
        }
        else
        {
            prvTgfBrgFdbFlushByShadowPart1(devNum, mode);
            prvTgfBrgFdbFlushByShadowPart2(devNum, mode);
            rc = GT_OK;
        }

        lua_pushinteger(L, rc);
        return 1;

    }
    else if (isGmUsed &&
             PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        /* A 'normal' FDB flush doesn't work in Bobcat2 GM.
         * So raise an error if shadow lists are not used */
        lua_pushinteger(L, GT_BAD_VALUE);
        return 1;
    }

    do
    {
        /* get original act device info */

        rc = cpssDxChBrgFdbActionActiveDevGet(devNum,&origActDev,&origActDevMask);
        if (GT_OK != rc)
        {
                break;
        }

        /* get FDB flush status */
        rc = cpssDxChBrgFdbStaticDelEnableGet(devNum, &modeEnabled);
        if (GT_OK != rc)
        {
                break;
        }

        if(mode != modeEnabled)
        {
            /* set FDB flush status */
            rc = cpssDxChBrgFdbStaticDelEnable(devNum, mode);
            if (GT_OK != rc)
            {
                    break;
            }
        }

        /* disable AA and TA messages to CPU */
        rc = cpssDxChBrgFdbAAandTAToCpuGet(devNum, &messagesToCpuEnabled);
        if ((rc == GT_OK) && messagesToCpuEnabled)
        {
            rc = cpssDxChBrgFdbAAandTAToCpuSet(devNum, GT_FALSE);
            if (GT_OK != rc)
            {
                    break;
            }
        }
        else
        {
            messagesToCpuEnabled = GT_FALSE;
            break;
        }

        /* set new act device info */
        rc = cpssDxChBrgFdbActionActiveDevSet(devNum,0,0);
        if (GT_OK != rc)
        {
                break;
        }

        /*get orig value vlans in flush */
        rc = cpssDxChBrgFdbActionActiveVlanGet(devNum, &origVlanId, &origVlanMask);
        if (GT_OK != rc)
        {
                break;
        }

        /*allow all vlans in flush */
        rc = cpssDxChBrgFdbActionActiveVlanSet(devNum, 0, 0);
        if (GT_OK != rc)
        {
                break;
        }

        if(GT_TRUE == PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum))
        {
            rc = cpssDxChBrgFdbActionActiveUserDefinedGet(devNum,&origActUerDefined,&origActUerDefinedMask);
            if (GT_OK != rc)
            {
                    break;
            }

            rc = cpssDxChBrgFdbActionActiveUserDefinedSet(devNum,0,0);
            if (GT_OK != rc)
            {
                    break;
            }
        }


        /* save Trunk Aging Mode*/
        rc = cpssDxChBrgFdbTrunkAgingModeGet(devNum, &origTrunkAgingMode);
        if (GT_OK != rc)
        {
                break;
        }

        /* Set Trunk Aging Mode*/
        rc = cpssDxChBrgFdbTrunkAgingModeSet(devNum, CPSS_FDB_AGE_TRUNK_ENTRIES_ALL_E);
        if (GT_OK != rc)
        {
                break;
        }


        /* trigger address deleting */
        rc = cpssDxChBrgFdbQueueFullGet(devNum, CPSS_DXCH_FDB_QUEUE_TYPE_AU_E, &isAuqFull);
        if(rc == GT_NOT_APPLICABLE_DEVICE)
        {
            isAuqFull = GT_FALSE;
            rc = GT_OK;
        }
        while(rc == GT_OK && isAuqFull == GT_TRUE)
        {
            /* allow the AppDemo to process AUQ messages --> so AUQ will get empty */
            cpssOsTimerWkAfter(50);
            rc = cpssDxChBrgFdbQueueFullGet(devNum, CPSS_DXCH_FDB_QUEUE_TYPE_AU_E, &isAuqFull);
        }

        if(rc == GT_OK)
        {
            rc = cpssDxChBrgFdbTrigActionStart(devNum, CPSS_FDB_ACTION_DELETING_E);
        }

        if (GT_OK != rc)
        {
            /* restore AA and TA messages to CPU */
            if (messagesToCpuEnabled)
            {
                rc = cpssDxChBrgFdbAAandTAToCpuSet(devNum, GT_TRUE);
            }
            break;
        }

        /* wait that triggered action is completed */
        while (GT_TRUE != isCompleted)
        {
#ifdef ASIC_SIMULATION
            if((--maxRetry) == 0)
            {
                break;
            }
            /* allow simulation to complete the operation*/
            cpssOsTimerWkAfter(50);
#endif /* ASIC_SIMULATION */

            /* get triggered action status */
            rc = cpssDxChBrgFdbTrigActionStatusGet(devNum, &isCompleted);
            if (GT_OK != rc)
            {
                break;
            }
        }


        /* restore original Trunk Aging Mode*/
        rc = cpssDxChBrgFdbTrunkAgingModeSet(devNum, origTrunkAgingMode);
        if (GT_OK != rc)
        {
            break;
        }

        /* restore original act device info */
        rc = cpssDxChBrgFdbActionActiveDevSet(devNum,origActDev,origActDevMask);
        if (GT_OK != rc)
        {
            break;
        }

        rc = cpssDxChBrgFdbActionActiveVlanSet(devNum, origVlanId,origVlanMask);
        if (GT_OK != rc)
        {
                break;
        }

        /* restore original act UserDefined info */
        if(GT_TRUE == PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum))
        {
            rc = cpssDxChBrgFdbActionActiveUserDefinedSet(devNum,origActUerDefined,origActUerDefinedMask);
            if (GT_OK != rc)
            {
                    break;
            }
        }

    }
    while (0);


    if (messagesToCpuEnabled)
    {
        rc = cpssDxChBrgFdbAAandTAToCpuSet(devNum, GT_TRUE);
    }

    if(mode != modeEnabled && rc == GT_OK)
    {
        /* set FDB flush status */
        rc = cpssDxChBrgFdbStaticDelEnable(devNum, modeEnabled);
    }


    lua_pushinteger(L, rc);
    return 1; /*return values*/
}

/* look for an fdb entry by it's 'key' to find the actual index that it uses
   in the HW */
static GT_STATUS getActualIndexOfFdbEntry
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr,
    OUT GT_U32                      *indexPtr
)
{
    GT_STATUS                    rc;
    GT_U32                       hash, ii;
    CPSS_MAC_HASH_FUNC_MODE_ENT  hashMode;
    GT_U32                       lookupLen;
    GT_U32                       indexArr[32];
    CPSS_MAC_ENTRY_EXT_STC       entry;
    GT_BOOL                      valid,skip,aged;
    GT_HW_DEV_NUM                associatedHwDevNum;

    rc = cpssDxChBrgFdbHashModeGet(
        devNum, &hashMode);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChBrgFdbMaxLookupLenGet(
        devNum, &lookupLen);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch (hashMode)
    {
        default:
            rc = cpssDxChBrgFdbHashCalc(
                devNum, macEntryKeyPtr, &hash);
            if (rc != GT_OK)
            {
                return rc;
            }

            for(ii = 0 ; ii < lookupLen ; ii ++)
            {
                indexArr[ii] = hash + ii;
            }

            break;
        case CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E:
            rc = cpssDxChBrgFdbHashCrcMultiResultsCalc(
                devNum, macEntryKeyPtr,
                0 /*multiHashStartBankIndex*/,
                16 /*numOfBanks*/,
                indexArr);
            if (rc != GT_OK)
            {
                return rc;
            }
            break;
    }

    for(ii = 0 ; ii < lookupLen ; ii ++)
    {
        /* check if the entry actual in in this index */
        rc = cpssDxChBrgFdbMacEntryRead(devNum,indexArr[ii],&valid,&skip,
                &aged,&associatedHwDevNum,&entry);

        if(valid == 0)
        {
            continue;
        }

        if(skip == 1 && hashMode != CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E)
        {
            continue;
        }

        if(cpssOsMemCmp(&entry.key,macEntryKeyPtr,sizeof(*macEntryKeyPtr)))
        {
            continue;
        }

        /* found match of the entry we look for */
        *indexPtr = indexArr[ii];

        return GT_OK;
    }


    return GT_NOT_FOUND;
}

static GT_STATUS local_fdbNextEntryGet_fromFdbShadow(
    IN GT_U8    devNum,
    IN CPSS_DXCH_BRG_FDB_DEL_MODE_ENT mode,
    INOUT GT_U32   *cookiePtr,
    OUT GT_U32  *indexPtr,
    OUT CPSS_MAC_ENTRY_EXT_STC  **foundMacEntryPtrPtr /*can be NULL*/
)
{
#define COOKIE_IN_DB_2 0x80000000
    GT_STATUS   rc = GT_NOT_IMPLEMENTED;
    CPSS_MAC_ENTRY_EXT_STC    *cpss_macEntryPtr = NULL;
    PRV_TGF_BRG_MAC_ENTRY_STC  *tgf_macEntryPtr = NULL;
    /* 'static' because we give reference to it , to caller function */
    static CPSS_MAC_ENTRY_EXT_STC  convertedFromTgf_MacEntry;
    GT_U32      fdbIndex = 0xFFFFFFFF;/* the index of the entry in the FDB. */
    GT_BOOL do_shadow1 = GT_FALSE;
    GT_BOOL do_shadow2 = GT_FALSE;
    GT_U32  shadow_cookie;

    if(prvTgfFdbShadowUsed == GT_FALSE)
    {
        return GT_NOT_INITIALIZED;
    }

retryNextEntry_lbl:

    *indexPtr = fdbIndex;

    if((*cookiePtr) & COOKIE_IN_DB_2)
    {
        do_shadow2 = GT_TRUE;
        (*cookiePtr)     -= COOKIE_IN_DB_2;
    }
    else
    {
        do_shadow1 = GT_TRUE;
    }

    if(do_shadow1 == GT_TRUE)
    {
        shadow_cookie = (*cookiePtr);

        rc = prvTgfBrgFdbMacEntryFromShadowGetNext_shadowPart1(
            devNum,mode,
            INOUT &shadow_cookie,
            OUT   &cpss_macEntryPtr);
        if (rc != GT_OK)
        {
            do_shadow2 = GT_TRUE;
            do_shadow1 = GT_FALSE;

            (*cookiePtr) = 0;/*start from start of shadow*/
        }
        else
        {
            (*cookiePtr) = shadow_cookie;
        }
    }

    if(do_shadow2 == GT_TRUE)
    {
        shadow_cookie = (*cookiePtr);

        rc = prvTgfBrgFdbMacEntryFromShadowGetNext_shadowPart2(
            devNum,mode,
            INOUT &shadow_cookie,
            OUT   &tgf_macEntryPtr);
        if (rc != GT_OK)
        {
            /* this error returned to caller */
            (*cookiePtr) = 0;
        }
        else
        {
            (*cookiePtr) = shadow_cookie;
            (*cookiePtr) |= COOKIE_IN_DB_2;/* this is INOUT parameter to the LUA caller
                            we need indication that we are in the shadow2 now */
        }
    }

    if(rc != GT_OK)
    {
        /* no more parameters needed */
        return rc;
    }

    if (foundMacEntryPtrPtr)
    {
        *foundMacEntryPtrPtr = NULL;
    }

    if(do_shadow1 == GT_TRUE && cpss_macEntryPtr)
    {
        rc = getActualIndexOfFdbEntry(devNum,
            &cpss_macEntryPtr->key,&fdbIndex);
        if (rc != GT_OK)
        {
            prvCpssPrintFdbKey("shadow1: entry not found in HW:",&cpss_macEntryPtr->key,GT_TRUE);
            goto retryNextEntry_lbl;
        }

        if (foundMacEntryPtrPtr)
        {
            *foundMacEntryPtrPtr = cpss_macEntryPtr;
        }
    }
    else
    if(do_shadow2 == GT_TRUE && tgf_macEntryPtr)
    {
        /* convert the TGF format to CPSS format */
        rc = prvTgfConvertGenericToDxChMacEntry(devNum,
            tgf_macEntryPtr,
            &convertedFromTgf_MacEntry);

        if (rc == GT_OK)
        {
            if (foundMacEntryPtrPtr)
            {
                *foundMacEntryPtrPtr = &convertedFromTgf_MacEntry;
            }

            rc = getActualIndexOfFdbEntry(devNum,
                &convertedFromTgf_MacEntry.key,&fdbIndex);
            if (rc != GT_OK)
            {
                prvCpssPrintFdbKey("shadow2: entry not found in HW:",&convertedFromTgf_MacEntry.key,GT_TRUE);
                goto retryNextEntry_lbl;
            }
        }
    }
    else
    {
        rc = GT_ERROR;/* ?! */
    }

    if (fdbIndex == 0)
    {
        zeroRealIndex__for_fdb_shadow = GT_TRUE;
    }

    *indexPtr = fdbIndex;



    return rc;
}

/*******************************************************************************
* wrlCpssDxChBrgFdbNextEntryGet_fromFdbShadow
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of next fdb entry index from FDB shadow.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                  - lua state (in stack should be device number and
*                             includeStatic value)
*       param 1 - devNum - the device number (GT_U8)
*       param 2 - mode - the 'show' fdb mode (CPSS_DXCH_BRG_FDB_DEL_MODE_ENT)
*       param 3 - cookie - the 'cookie' number.
*               to get first need to be 0.
*               after this the cookie is also returned as 'OUT'
*       param 4 - macAged
*       param 5 - macFilter - filters about which entries should be.
*
* OUTPUTS:
*       status - the 'error code'           (GT_STATUS)
*       cookie - the updated 'cookie' number. (GT_U32)
*       fdbIndex - the fdbIndex of the found entry.(GT_U32)
*       fdbEntry - the found fdb entry. (CPSS_MAC_ENTRY_EXT_STC)
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChBrgFdbNextEntryGet_fromFdbShadow
(
    IN  lua_State           *L
)
{
    GT_STATUS   rc = GT_NOT_IMPLEMENTED;
    GT_U8 devNum        = (GT_U8)lua_tonumber(L, 1);
    CPSS_DXCH_BRG_FDB_DEL_MODE_ENT mode = lua_tonumber(L, 2);
    GT_U32 cookie       = (GT_U32)lua_tonumber(L, 3);
    CPSS_MAC_ENTRY_EXT_STC  *foundMacEntryPtr;
    GT_U32      paramsOnStack;
    GT_U32      fdbIndex;

    rc = local_fdbNextEntryGet_fromFdbShadow(devNum,mode,&cookie,&fdbIndex,&foundMacEntryPtr);
    if(rc != GT_OK)
    {
        /* no more parameters needed */
        lua_pushnumber(L, rc);
        return 1;
    }

    paramsOnStack = 0;

    /* put the return code on the stack */
    lua_pushnumber(L, rc);
    paramsOnStack++;

    /* put the cookie on the stack */
    lua_pushnumber(L, cookie);
    paramsOnStack++;

    /* put the fdb-index on the stack */
    lua_pushnumber(L, fdbIndex);
    paramsOnStack++;

    if(foundMacEntryPtr)
    {
        /* put the full entry on the stack */
        pvrCpssMacEntryToLuaPush(GT_OK, L, foundMacEntryPtr);
        paramsOnStack++;
    }

    return paramsOnStack;

}
