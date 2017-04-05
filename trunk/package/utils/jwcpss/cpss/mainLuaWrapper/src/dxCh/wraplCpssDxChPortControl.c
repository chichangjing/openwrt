/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplCpssDxChPortControl.c
*
* DESCRIPTION:
*       port control functions wrapper
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 16 $
*******************************************************************************/
#include <string.h>
#include <generic/private/prvWraplGeneral.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpssCommon/wraplCpssExtras.h>
#include <cpssCommon/wrapCpssDebugInfo.h>

#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>


/*******************************************************************************
*   Trunk wrapper warnings                                                     *
*******************************************************************************/
GT_CHAR wrlFunctionArgumentsFromLuaGettingErrorMessage[] =
                                    "Error at reading of function arguments";

GT_CHAR wrlPortModeSpeedErrorMwssage[] =
                                "API error at setting of port mode and speed";

GT_CHAR portTypeUnsupportingInRxOrTxFlowControlConfiguringWarningMessage[]    =
    "Only Rx or only Tx flow control could be not configuted on such port type";


/***** declarations ********/

use_prv_enum(CPSS_PORT_INTERFACE_MODE_ENT);
use_prv_enum(CPSS_PORT_SPEED_ENT);
use_prv_print_struct(CPSS_PORTS_BMP_STC);

/***** declarations ********/

/*******************************************************************************
* wrlDxChPortModeSpeedSet
*
* DESCRIPTION:
*        Configure Interface mode and speed on a specified port and execute on
*        port's serdeses power up sequence; or configure power down on port's
*        serdeses.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       L                         - lua state
*       L[1]                      - physical device number
*       L[2]                      - physical port number (or CPU port)
*       L[3]                      - serdes power
*       L[4]                      - interface mode
*       L[5]                      - port data speed
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK is pused to lua stack if no errors occurs
*       error status and error message it error occures
*
* COMMENTS:
*
*******************************************************************************/
 int wrlDxChPortModeSpeedSet
(
    IN lua_State *L
)
{
    GT_U8                           devNum          = (GT_U8)
                                                            lua_tonumber(L, 1);
                                                                    /* devId */
    GT_PORT_NUM                     portNum         = (GT_PORT_NUM)
                                                            lua_tonumber(L, 2);
                                                                /* portNum */
    CPSS_PORTS_BMP_STC              portsBmp;
    GT_BOOL                         powerUp         = (GT_BOOL)
                                                            lua_toboolean(L, 3);
                                                                /* powerUp */
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode          =
                                                CPSS_PORT_INTERFACE_MODE_NA_E;
    CPSS_PORT_SPEED_ENT             speed           = CPSS_PORT_SPEED_NA_E;
    GT_STATUS                       status          = GT_OK;
    GT_CHAR_PTR                     error_message   = NULL;
    P_CALLING_CHECKING;

    cpssOsMemSet(&portsBmp, 0, sizeof(portsBmp));
    CPSS_PORTS_BMP_PORT_SET_MAC(&portsBmp, portNum);

    status = pvrCpssPortInterfaceModeFromLuaGet(L, 4, &ifMode, &error_message);
    if (GT_OK != status)
    {
        error_message   = wrlFunctionArgumentsFromLuaGettingErrorMessage;
    }

    if (GT_OK == status)
    {
        status = pvrCpssPortSpeedFromLuaGet(L, 5, &speed, &error_message);
        if (GT_OK != status)
        {
            error_message   = wrlFunctionArgumentsFromLuaGettingErrorMessage;
        }
    }

    if (GT_OK == status)
    {
        CPSS_API_CALL_LOCK();
        status = cpssDxChPortModeSpeedSet(devNum, portsBmp, powerUp, ifMode,
                                          speed);
        CPSS_API_CALL_UNLOCK();
        P_CALLING_API(
            cpssDxChPortModeSpeedSet,
            PC_NUMBER(devNum,                   devNum,                  GT_U8,
            PC_STRUCT(&portsBmp,                portsBmp,
                                                            CPSS_PORTS_BMP_STC,
            PC_BOOL  (powerUp,                  powerUp,
            PC_ENUM  (ifMode,                   ifMode,
                                                  CPSS_PORT_INTERFACE_MODE_ENT,
            PC_ENUM  (speed,                    speed,     CPSS_PORT_SPEED_ENT,
            PC_LAST_PARAM))))),
            PC_STATUS);
        if (GT_OK != status)
        {
            error_message   = wrlPortModeSpeedErrorMwssage;
        }
    }

    return prvWrlCpssStatusToLuaPush(status, L, error_message);
}


/*******************************************************************************
* wrlDxChPortsModeSpeedSet
*
* DESCRIPTION:
*        Configure Interface mode and speed on a specified port bitmap and
*        execute on port's serdeses power up sequence; or configure power down
*        on port's serdeses.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       L                         - lua state
*       L[1]                      - physical device number
*       L[2]                      - port bitmap
*       L[3]                      - serdes power
*       L[4]                      - interface mode
*       L[5]                      - port data speed
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK is pused to lua stack if no errors occurs
*       error status and error message it error occures
*
* COMMENTS:
*
*******************************************************************************/
 int wrlDxChPortsModeSpeedSet
(
    IN lua_State *L
)
{
    GT_U8                           devNum          = (GT_U8)
                                                            lua_tonumber(L, 1);
                                                                    /* devId */
    CPSS_PORTS_BMP_STC              portsBmp;
    GT_BOOL                         powerUp         = (GT_BOOL)
                                                            lua_toboolean(L, 3);
                                                                /* powerUp */
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode          =
                                                CPSS_PORT_INTERFACE_MODE_NA_E;
    CPSS_PORT_SPEED_ENT             speed           = CPSS_PORT_SPEED_NA_E;
    GT_STATUS                       status          = GT_OK;
    GT_CHAR_PTR                     error_message   = NULL;
    P_CALLING_CHECKING;

    cpssOsMemSet(&portsBmp, 0, sizeof(portsBmp));

    status = pvrCpssPortInterfaceModeFromLuaGet(L, 4, &ifMode, &error_message);
    if (GT_OK != status)
    {
        error_message   = wrlFunctionArgumentsFromLuaGettingErrorMessage;
    }

    if (GT_OK == status)
    {
        status = pvrCpssPortSpeedFromLuaGet(L, 5, &speed, &error_message);
        if (GT_OK != status)
        {
            error_message   = wrlFunctionArgumentsFromLuaGettingErrorMessage;
        }
    }

    if (GT_OK == status)
    {
        status = pvrCpssPortsMembersFromLuaGet(L, 2, &portsBmp, &error_message);
        if (GT_OK != status)
        {
            error_message   = wrlFunctionArgumentsFromLuaGettingErrorMessage;
        }
    }

    if (GT_OK == status)
    {
        CPSS_API_CALL_LOCK();
        status = cpssDxChPortModeSpeedSet(devNum, portsBmp, powerUp, ifMode,
                                          speed);
        CPSS_API_CALL_UNLOCK();
        P_CALLING_API(
            cpssDxChPortModeSpeedSet,
            PC_NUMBER(devNum,                   devNum,                  GT_U8,
            PC_STRUCT(&portsBmp,                portsBmp,
                                                            CPSS_PORTS_BMP_STC,
            PC_BOOL  (powerUp,                  powerUp,
            PC_ENUM  (ifMode,                   ifMode,
                                                  CPSS_PORT_INTERFACE_MODE_ENT,
            PC_ENUM  (speed,                    speed,     CPSS_PORT_SPEED_ENT,
            PC_LAST_PARAM))))),
            PC_STATUS);
        if (GT_OK != status)
        {
            error_message   = wrlPortModeSpeedErrorMwssage;
        }
    }

    return prvWrlCpssStatusToLuaPush(status, L, error_message);
}


/*******************************************************************************
* wrlDxChPortFlowControlConfiguringConditionChecking
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Checking of condition for port flow-control configuring.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                     - lua state
*       L[1]                  - device number
*       L[2]                  - trunk id
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
 int wrlDxChPortFlowControlConfiguringConditionChecking
(
    IN lua_State *L
)
{
    GT_U8                       devNum                  =
                                    (GT_U8)                lua_tonumber(L, 1);
                                                                    /* devId */
    GT_PHYSICAL_PORT_NUM        portNum                 =
                                    (GT_PHYSICAL_PORT_NUM) lua_tonumber(L, 2);
                                                                /* portNum */
    CPSS_PORT_FLOW_CONTROL_ENT  portFlowControl         =
                                            CPSS_PORT_FLOW_CONTROL_DISABLE_E;
    GT_BOOL                     chekingResult           = GT_TRUE;
    GT_CHAR_PTR                 conditionWarningString  = NULL;
    GT_STATUS                   status                  = GT_OK;
    GT_CHAR_PTR                 error_message           = NULL;
    int                         returned_param_count    = 0;

    status = pvrCpssPortFlowControlFromLuaGet(L, 3, &portFlowControl,
                                              &error_message);

    if ((CPSS_PORT_FLOW_CONTROL_DISABLE_E != portFlowControl)   &&
        (CPSS_PORT_FLOW_CONTROL_RX_TX_E != portFlowControl)     &&
        (WRL_PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum, portNum) <
                                                        WRL_PRV_CPSS_PORT_XG_E))
    {
        chekingResult           = GT_FALSE;
        conditionWarningString  =
            portTypeUnsupportingInRxOrTxFlowControlConfiguringWarningMessage;
    }

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssBooleanToLuaPush(status, L, chekingResult);
    returned_param_count    +=
        pvrCpssStringToLuaPush(status, L, conditionWarningString);
    return returned_param_count;
}

int wrlDxChPortEomMatrixGet
(
    IN lua_State *L
)
{
    GT_U32 rc,voltage,phase;
    GT_U32 rowSize;
    static CPSS_DXCH_PORT_EOM_MATRIX_STC *matrix = NULL;



    if (!(lua_isnumber(L,1)) || !(lua_isnumber(L,2)) || !(lua_isnumber(L,3)) || !(lua_isnumber(L,4)))
    {
		lua_pushnil(L);
        lua_pushstring(L,"All parameters must be integers");
	}
	else
	{
        if (matrix == NULL)
        {
            matrix = cpssOsMalloc(sizeof(CPSS_DXCH_PORT_EOM_MATRIX_STC));
            if (matrix == NULL)
            {
                lua_pushnil(L);
                lua_pushstring(L,"Could not allocate memory");
                return 2;
            }
        }
        CPSS_API_CALL_LOCK();
		rc=cpssDxChPortEomMatrixGet((GT_U8) lua_tonumber(L, 1),(GT_PHYSICAL_PORT_NUM)lua_tointeger(L, 2),
                                    (GT_U32) lua_tonumber(L, 3),(GT_U32) lua_tonumber(L, 4),&rowSize,matrix);
        CPSS_API_CALL_UNLOCK();

        if (rc==GT_OK)
        {
            lua_createtable(L,CPSS_DXCH_PORT_EOM_VOLT_RANGE_CNS,0);

            for (voltage=0;voltage<CPSS_DXCH_PORT_EOM_VOLT_RANGE_CNS;voltage++)
            {
                lua_createtable(L,rowSize,0);

                for (phase=0;phase<rowSize;phase++)
                {
                    lua_pushnumber(L, (matrix->lowerMatrix)[voltage][phase]);
                    lua_rawseti(L, -2, phase+1);
                }
                lua_rawseti(L, -2, voltage+1);
            }

            /* Upper matrix's first row is omitted since it is also in the lower matrix*/
            lua_createtable(L,CPSS_DXCH_PORT_EOM_VOLT_RANGE_CNS-1,0);

            for (voltage=1;voltage<CPSS_DXCH_PORT_EOM_VOLT_RANGE_CNS;voltage++)
            {
                lua_createtable(L,rowSize,0);

                for (phase=0;phase<rowSize;phase++)
                {
                    lua_pushnumber(L, (matrix->upperMatrix)[voltage][phase]);
                    lua_rawseti(L, -2, phase+1);
                }
                lua_rawseti(L, -2, voltage);
            }
        }
        else
        {
            lua_pushnil(L);
            lua_pushstring(L,"API returned status: ");
            lua_pushinteger(L,rc);
            lua_concat(L,2);
        }
    }

    return 2;
}

/*******************************************************************************
* wrlDxChPortIsValidEport
*
* DESCRIPTION:
*        Configure Interface mode and speed on a specified port and execute on
*        port's serdeses power up sequence; or configure power down on port's
*        serdeses.
*
* APPLICABLE DEVICES:
*        Bobcat2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       L                         - lua state
*       L[1]                      - device number
*       L[2]                      - port number
*
* OUTPUTS:
*
* RETURNS:
*       GT_TRUE if devNum/portNum is valid ePort, GT_FALSE - otherwise
*
* COMMENTS:
*
*******************************************************************************/
int wrlDxChPortIsValidEport
(
    IN lua_State *L
)
{

    GT_U8         devNum;
    GT_PORT_NUM   portNum;
    GT_STATUS     rc = GT_OK;

    PARAM_NUMBER(rc, devNum, 1, GT_U8);
    PARAM_NUMBER(rc, portNum, 2, GT_PORT_NUM);
    if (rc != GT_OK)
    {
        /* return false */
        lua_pushboolean(L, 0);
        return 1;
    }

    /* ePort relevant only for eArch devices */
    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum))
    {
        /* check port number */
        if (portNum <= PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_PORT_MAC(devNum))
        {
             /* return true*/
            lua_pushboolean(L, 1);
            return 1;
        }
    }
    /* return false*/
    lua_pushboolean(L, 0);
    return 1;
}

/*******************************************************************************
* wrlDxChPortModeSpeedAutoDetectAndConfig
*
* DESCRIPTION:
*        Autodetect and configure interface mode and speed of a given port
*       If the process succeeded the port gets configuration ready for link,
*       otherwise (none of provided by application options valid) the port
*       is left in reset mode.
*
* APPLICABLE DEVICES:
*        Lion; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat3; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       L                         - lua state
*
* OUTPUTS:
*
* RETURNS:
*       1, with parameters (status, ifMode, speed)
*
* COMMENTS:
*
*******************************************************************************/
int wrlDxChPortModeSpeedAutoDetectAndConfig
(
    IN lua_State *L
)
{
    GT_U8                                   index;
    GT_U8                                   arrLength;
    GT_U8                                   autoDetectArr[8];
    CPSS_PORT_MODE_SPEED_STC                modesAdvertiseArr[8];
    CPSS_PORT_MODE_SPEED_STC                currentModePtr;
    GT_U32									t;
    GT_STATUS                               rc                      = GT_OK;
    GT_U8                                   devNum             		=
                                                    (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devNum */
    GT_PHYSICAL_PORT_NUM                    portNum                 =
                                                    (GT_PHYSICAL_PORT_NUM) lua_tonumber(L, 2);
                                                                    /* portNum */
    autoDetectArr[0]                                                =
                                                    (GT_U8)  lua_tonumber(L, 3);
                                                                    /* autoDetectArr[0] */
    autoDetectArr[1]                                                =
                                                    (GT_U8)  lua_tonumber(L, 4);
                                                                    /* autoDetectArr[1] */
    autoDetectArr[2]                                                =
                                                    (GT_U8)  lua_tonumber(L, 5);
                                                                    /* autoDetectArr[2] */
    autoDetectArr[3]                                                =
                                                    (GT_U8)  lua_tonumber(L, 6);
                                                                    /* autoDetectArr[3] */
    autoDetectArr[4]                                                =
                                                    (GT_U8)  lua_tonumber(L, 7);
                                                                    /* autoDetectArr[4] */
    autoDetectArr[5]                                                =
                                                    (GT_U8)  lua_tonumber(L, 8);
                                                                    /* autoDetectArr[5] */
    autoDetectArr[6]                                              	=
                                                    (GT_U8)  lua_tonumber(L, 9);
                                                                    /* autoDetectArr[6] */
    autoDetectArr[7]                                                =
                                                    (GT_U8)  lua_tonumber(L, 10);
                                                                    /* autoDetectArr[7] */
    arrLength                                                =
                                                    (GT_U8)  lua_tonumber(L, 11);
                                                                    /* arrLength */

    for (index = 0; index < arrLength; index++) 
    {
        switch (autoDetectArr[index]) 
        {
        case 1:                             /* XLG_40000 */
            modesAdvertiseArr[index].ifMode =       CPSS_PORT_INTERFACE_MODE_XLG_E;
            modesAdvertiseArr[index].speed  =       CPSS_PORT_SPEED_40000_E;
            break;
        case 2:                             /* RXAUI_10000 */
            modesAdvertiseArr[index].ifMode =       CPSS_PORT_INTERFACE_MODE_RXAUI_E;
            modesAdvertiseArr[index].speed  =       CPSS_PORT_SPEED_10000_E;
            break;
        case 3:                             /* XAUI_10000 */
            modesAdvertiseArr[index].ifMode =       CPSS_PORT_INTERFACE_MODE_XGMII_E;
            modesAdvertiseArr[index].speed  =       CPSS_PORT_SPEED_10000_E;
            break;
        case 4:                             /* XAUI_20000 */
            modesAdvertiseArr[index].ifMode =       CPSS_PORT_INTERFACE_MODE_XGMII_E;
            modesAdvertiseArr[index].speed  =       CPSS_PORT_SPEED_20000_E;
            break;
        case 5:                             /* 1000BaseX_1000 */
            modesAdvertiseArr[index].ifMode =       CPSS_PORT_INTERFACE_MODE_1000BASE_X_E;
            modesAdvertiseArr[index].speed  =       CPSS_PORT_SPEED_1000_E;
            break;
        case 6:                             /* SGMII_1000 */
            modesAdvertiseArr[index].ifMode =       CPSS_PORT_INTERFACE_MODE_SGMII_E;
            modesAdvertiseArr[index].speed  =       CPSS_PORT_SPEED_1000_E;
            break;
        case 7:                             /* SGMII_2500 */
            modesAdvertiseArr[index].ifMode =       CPSS_PORT_INTERFACE_MODE_SGMII_E;
            modesAdvertiseArr[index].speed  =       CPSS_PORT_SPEED_2500_E;
            break;
        case 8:                             /* QSGMII_1000 */
            modesAdvertiseArr[index].ifMode =       CPSS_PORT_INTERFACE_MODE_QSGMII_E;
            modesAdvertiseArr[index].speed  =       CPSS_PORT_SPEED_1000_E;
            break;
        default:
            rc = GT_BAD_PARAM;
            break;
        }
    }
    if(rc == GT_OK)
    {
        rc = cpssDxChPortModeSpeedAutoDetectAndConfig(devNum, portNum, modesAdvertiseArr, arrLength, &currentModePtr);
    }

    if(rc != GT_OK)
	{
		lua_newtable(L);
		t = lua_gettop(L);
		lua_pushinteger(L, rc);
		lua_setfield(L, t, "status");
		lua_settop(L, t);
		return 1;
	}

    lua_newtable(L);
    t = lua_gettop(L);
	lua_pushinteger(L, rc);
	lua_setfield(L, t, "status");
    lua_pushinteger(L, currentModePtr.ifMode);
	lua_setfield(L, t, "ifMode");
    lua_pushinteger(L, currentModePtr.speed);
	lua_setfield(L, t, "speed");
	lua_settop(L, t);
    return 1;
}


/*******************************************************************************
* wrlDxChPortLoopbackModeEnableGet
*
* DESCRIPTION:
*       get port loopback mode
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       enable        - enable/disable loopback
*
* RETURNS:
*       return code
*
* COMMENTS:
*
*******************************************************************************/
int wrlDxChPortLoopbackModeEnableGet
(
    IN lua_State *L
)
{
    GT_U8                devNum;
    GT_PORT_NUM          portNum;
    GT_BOOL              enablePtr;
    GT_STATUS            rc = GT_OK;
    PARAM_NUMBER(rc, devNum,  1, GT_U8);
    PARAM_NUMBER(rc, portNum, 2, GT_PORT_NUM);


    rc = cpssDxChPortInternalLoopbackEnableGet(devNum, portNum, &enablePtr);
    
    lua_pushinteger(L, rc);
    lua_pushinteger(L, enablePtr);

    return 2;
}




