 /*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplCpssDxChI2C.c
*
* DESCRIPTION:
*       A lua wrapper for i2c.
*
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 11 $
*******************************************************************************/
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/cpssTypes.h>
#include <generic/private/prvWraplGeneral.h>


#include <stdlib.h>
#include <generic/private/prvWraplGeneral.h>
#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>

#if (!defined ASIC_SIMULATION)
extern GT_STATUS hwIfTwsiReadByte
(               
    IN  GT_U32  devSlvId,
    IN  GT_U32  regAddr,
    OUT GT_U32 *dataPtr
);

extern GT_STATUS hwIfTwsiWriteByte
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr,
    IN GT_U32 value
);
#endif


/*******************************************************************************
* wrlCpssDxChHwIfTwsiWriteByte
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*
*
* INPUTS:
*        L                   - lua state
*
* OUTPUTS:
*
* RETURNS:
*       1; status code.
*
* COMMENTS:
*
*******************************************************************************/

GT_STATUS wrlCpssDxChHwIfTwsiWriteByte
(
    IN lua_State            *L
)
{
    GT_U32                                  t;
    #if (!defined ASIC_SIMULATION)
        GT_STATUS                               status                      = GT_OK;
        GT_U32                                  devSlvId                    =
                                                        (GT_U32)  lua_tonumber(L, 1);
                                                                        /* devSlvId */
        GT_U32                                  regAddr                     =
                                                        (GT_U32)  lua_tonumber(L, 2);
                                                                        /* regAddr */
        GT_U32                                  value                 	    =
                                                        (GT_U32)  lua_tonumber(L, 3);
                                                                        /* value */
                                                                        
        status = hwIfTwsiWriteByte(devSlvId, regAddr, value);   
        lua_newtable(L);
        t = lua_gettop(L);
        lua_pushnumber(L, status);
        lua_setfield(L, t, "status");
        lua_settop(L, t);
        return 1;
    #else
        lua_newtable(L);
        t = lua_gettop(L);
        lua_pushnumber(L, GT_NOT_SUPPORTED);
        lua_setfield(L, t, "status");
        lua_settop(L, t);
        return 1;
    #endif
}


/*******************************************************************************
* wrlCpssDxChHwIfTwsiReadByte
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*
*
* INPUTS:
*        L                   - lua state
*
* OUTPUTS:
*
* RETURNS:
*       1; status code          ,if error.
*        ; status code and data ,otherwise.
*
* COMMENTS:
*
*******************************************************************************/

GT_STATUS wrlCpssDxChHwIfTwsiReadByte
(
    IN lua_State            *L
)
{
    GT_U32                                  t;
    #if (!defined ASIC_SIMULATION)
        GT_U32                                  dataPtr;
        GT_STATUS                               status                      = GT_OK;
        GT_U32                                  devSlvId                    =
                                                        (GT_U32)  lua_tonumber(L, 1);
                                                                        /* devSlvId */
        GT_U32                                  regAddr                     =
                                                        (GT_U32)  lua_tonumber(L, 2);
                                                                        /* regAddr */
                                          
        status = hwIfTwsiReadByte(devSlvId, regAddr, &dataPtr);  
        if(status != GT_OK)
        {
            lua_newtable(L);
            t = lua_gettop(L);
            lua_pushnumber(L, status);
            lua_setfield(L, t, "status");
            lua_settop(L, t);
            return 1;
        }
        
        lua_newtable(L);
        t = lua_gettop(L);
        
        lua_pushnumber(L, status);
        lua_setfield(L, t, "status");

        lua_pushnumber(L, dataPtr);
        lua_setfield(L, t, "data");

        lua_settop(L, t);
        return 1;
    #else
        lua_newtable(L);
        t = lua_gettop(L);
        lua_pushnumber(L, GT_NOT_SUPPORTED);
        lua_setfield(L, t, "status");
        lua_settop(L, t);
        return 1;
    #endif 
}


