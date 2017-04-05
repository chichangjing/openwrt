/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplCpssDxChPlr.c
*
* DESCRIPTION:
*       A lua wrapper for Policer functions
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <generic/private/prvWraplGeneral.h>
#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>
#include <cpssCommon/wraplCpssTypes.h>
#include <cpssCommon/wrapCpssDebugInfo.h>

#include <lua.h>

/* declarations of manual wrappers */
void prv_lua_to_c_CPSS_DXCH3_POLICER_METERING_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH3_POLICER_METERING_ENTRY_STC *val
);
void prv_c_to_lua_CPSS_DXCH3_POLICER_METERING_ENTRY_STC(
    lua_State *L,
    CPSS_DXCH3_POLICER_METERING_ENTRY_STC *val
);
void prv_c_to_lua_CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT_envelope(
    lua_State *L,
    CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT *val
);
extern enumDescr enumDescr_CPSS_DXCH_POLICER_STAGE_TYPE_ENT[];

/* static variables to save stack space */
/* used both in set and in get          */
static CPSS_DXCH3_POLICER_METERING_ENTRY_STC       prvMeterEntryArr[8];
static CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT      prvMeterTbParamsArr[8];

/*******************************************************************************
* wrlCpssDxChPolicerMeteringEntryEnvelopeSet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Builds and writes Metering Entries Envelope to HW.
*
* APPLICABLE DEVICES:
*        BobK, BobCat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                   - lua state
*
* OUTPUTS:
*
* RETURNS:
*       1 and result pushed to lua stack
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChPolicerMeteringEntryEnvelopeSet
(
    IN lua_State                            *L
)
{
    GT_STATUS                                   rc;
    GT_U32                                      i,t;
    GT_U8                                       devNum;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT            stage;
    GT_U32                                      startEntryIndex;
    GT_U32                                      envelopeSize;
    GT_BOOL                                     couplingFlag0;
    P_CALLING_CHECKING;

    devNum           = (GT_U8)                           lua_tonumber(L, 1);
    rc = GT_OK;
    PARAM_ENUM(rc, stage, 2, CPSS_DXCH_POLICER_STAGE_TYPE_ENT);
    if (rc != GT_OK)
    {
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
    }
    startEntryIndex  = (GT_U32)                          lua_tonumber(L, 3);
    envelopeSize     = (GT_U32)                          lua_tonumber(L, 4);
    couplingFlag0    = (GT_BOOL)                         lua_toboolean(L, 5);

    if (envelopeSize > 8)
    {
        /* wrong parameters */
        lua_pushinteger(L, GT_BAD_PARAM);
        return 1;
    }

    for (i = 0; (i < envelopeSize); i++)
    {
        lua_pushinteger(L, i);
        lua_gettable(L, 6); /*meterEntryArr[i]*/
        prv_lua_to_c_CPSS_DXCH3_POLICER_METERING_ENTRY_STC(L, &(prvMeterEntryArr[i]));
    }

    rc = cpssDxChPolicerMeteringEntryEnvelopeSet(
        devNum, stage, startEntryIndex, envelopeSize, couplingFlag0,
        prvMeterEntryArr, prvMeterTbParamsArr);
    P_CALLING_API(
        cpssDxChPolicerMeteringEntryEnvelopeSet,
        PC_NUMBER(devNum,                       devNum,                  GT_U8,
        PC_ENUM(stage,                          stage,                   CPSS_DXCH_POLICER_STAGE_TYPE_ENT,
        PC_NUMBER(startEntryIndex,              startEntryIndex,         GT_U32,
        PC_NUMBER(envelopeSize,                 envelopeSize,            GT_U32,
        PC_BOOL(couplingFlag0,                  couplingFlag0,
        PC_LAST_PARAM))))),
        rc);
    /* tracing of parameters below temporary not supported      */
    /* CPSS_DXCH3_POLICER_METERING_ENTRY_STC       entryArr[]   */
    /* CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT      tbParamsArr[]*/

    lua_pushinteger(L, rc);
    if (rc != GT_OK)
    {
        return 1;
    }

    lua_newtable(L);
    t = lua_gettop(L);
    for (i = 0; (i < envelopeSize); i++)
    {
        prv_c_to_lua_CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT_envelope(
            L, &(prvMeterTbParamsArr[i]));
        lua_pushinteger(L, i);
        lua_settable(L, t);
    }
    return 2;
}


/*******************************************************************************
* wrlCpssDxChPolicerMeteringEntryEnvelopeGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Reads and converts to Lua Metering Entries Envelope from HW.
*
* APPLICABLE DEVICES:
*        BobK, BobCat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                   - lua state
*
* OUTPUTS:
*
* RETURNS:
*       1 and result pushed to lua stack
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChPolicerMeteringEntryEnvelopeGet
(
    IN lua_State                            *L
)
{
    GT_STATUS                                   rc;
    GT_U32                                      i,t;
    GT_U8                                       devNum;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT            stage;
    GT_U32                                      startEntryIndex;
    GT_U32                                      envelopeSize;
    GT_U32                                      maxEnvelopeSize;
    GT_BOOL                                     couplingFlag0;
    P_CALLING_CHECKING;

    devNum           = (GT_U8)                           lua_tonumber(L, 1);
    rc = GT_OK;
    PARAM_ENUM(rc, stage, 2, CPSS_DXCH_POLICER_STAGE_TYPE_ENT);
    if (rc != GT_OK)
    {
        lua_pushinteger(L, (lua_Integer)rc);
        return 1;
    }
    startEntryIndex  = (GT_U32)                          lua_tonumber(L, 3);
    maxEnvelopeSize  = (GT_U32)                          lua_tonumber(L, 4);

    if (maxEnvelopeSize > 8)
    {
        /* wrong parameters */
        lua_pushinteger(L, GT_BAD_PARAM);
        return 1;
    }

    rc = cpssDxChPolicerMeteringEntryEnvelopeGet(
        devNum, stage, startEntryIndex, maxEnvelopeSize,
        &envelopeSize, &couplingFlag0, prvMeterEntryArr);
    P_CALLING_API(
        cpssDxChPolicerMeteringEntryEnvelopeGet,
        PC_NUMBER(devNum,                       devNum,                  GT_U8,
        PC_ENUM(stage,                          stage,                   CPSS_DXCH_POLICER_STAGE_TYPE_ENT,
        PC_NUMBER(startEntryIndex,              startEntryIndex,         GT_U32,
        PC_NUMBER(maxEnvelopeSize,              maxEnvelopeSize,         GT_U32,
        PC_NUMBER(envelopeSize,                 envelopeSize,            GT_U32,
        PC_BOOL(couplingFlag0,                  couplingFlag0,
        PC_LAST_PARAM)))))),
        rc);
    /* tracing of parameters below temporary not supported      */
    /* CPSS_DXCH3_POLICER_METERING_ENTRY_STC       entryArr[]   */

    lua_pushinteger(L, rc);
    if (rc != GT_OK)
    {
        return 1;
    }

    lua_pushinteger(L, envelopeSize);
    lua_pushboolean(L, couplingFlag0);
    lua_newtable(L);
    t = lua_gettop(L);
    for (i = 0; (i < envelopeSize); i++)
    {
        lua_pushinteger(L, i); /* key for table */
        prv_c_to_lua_CPSS_DXCH3_POLICER_METERING_ENTRY_STC(
            L, &(prvMeterEntryArr[i])); /* value for table */
        lua_settable(L, t); /* add to table pair key, value */
    }
    return 4;
}
