 /*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplCpssDxChPortMapping.c
*
* DESCRIPTION:
*       A lua wrapper for port mapping.
*
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/cpssTypes.h>
#include <generic/private/prvWraplGeneral.h>

#include <cpss/dxCh/dxChxGen/port/cpssDxChPortMapping.h>

#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>
#include <cpssCommon/wrapCpssDebugInfo.h>

use_prv_struct(CPSS_DXCH_PORT_MAP_STC)


/*******************************************************************************
* wrlCpssDxChPortPhysicalPortMapSet
*
* DESCRIPTION:
*       configure ports mapping
*
* APPLICABLE DEVICES:
*        Bobcat2
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
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
int wrlCpssDxChPortPhysicalPortMapSet
(
    IN lua_State                            *L
)
{
    GT_STATUS                                   rc;
    GT_U32                                      i;
    GT_U8                                       devNum;
    GT_U32                                      portMapArraySize;
    static CPSS_DXCH_PORT_MAP_STC               *portMapArrayPtr = NULL;
    P_CALLING_CHECKING;


    devNum            = (GT_U8)                          lua_tonumber(L, 1);
    portMapArraySize  = (GT_U32)                         lua_tonumber(L, 2);

    portMapArrayPtr = cpssOsMalloc(portMapArraySize*sizeof(CPSS_DXCH_PORT_MAP_STC));
    if (portMapArrayPtr == NULL)
    {
        /* Could not allocate memory */
        lua_pushinteger(L, GT_OUT_OF_CPU_MEM);
        return 1;
    }

    for (i = 0; (i < portMapArraySize); i++)
    {
        lua_pushinteger(L, i+1);
        lua_gettable(L, 3);                   /* portMapArrayPtr[i] */
        prv_lua_to_c_CPSS_DXCH_PORT_MAP_STC(L, &(portMapArrayPtr[i]));
    }

    CPSS_API_CALL_LOCK();
    rc = cpssDxChPortPhysicalPortMapSet(devNum, portMapArraySize, portMapArrayPtr);
    CPSS_API_CALL_UNLOCK();

    P_CALLING_API(
        cpssDxChPortPhysicalPortMapSet,
        PC_NUMBER(devNum,                       devNum,                  GT_U8,
        PC_NUMBER(portMapArraySize,             portMapArraySize,        GT_U32,
        PC_LAST_PARAM)),
        rc);

    cpssOsFree(portMapArrayPtr);

    lua_pushinteger(L, rc);
    return 1;
}

