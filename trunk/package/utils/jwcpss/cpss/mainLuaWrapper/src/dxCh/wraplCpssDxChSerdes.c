/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplCpssSerdes.c
*
* DESCRIPTION:
*       A lua wrapper for serdes functions.
*
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 11 $
*******************************************************************************/

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <generic/private/prvWraplGeneral.h>
#include <cpssCommon/wrapCpssDebugInfo.h>


#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>

/*******************************************************************************
*   Serdes functions errors                                                    *
*******************************************************************************/
GT_CHAR serdesPoweringErrorMessage[]                   = 
                                                "Could not powerind serdes";
GT_CHAR interfaceModeGettingErrorMessage[]             = 
                                            "Could not get interface mode";
GT_CHAR interfaceSpeedGettingErrorMessage[]            = 
                                            "Could not get interface speed";
                                                
GT_CHAR interfaceModeAndSpeedSupportingErrorMessage[]  = 
                            "Such interface mode and speed is not supported";

                            
/***** declarations ********/
                            
use_prv_enum(CPSS_PORT_DIRECTION_ENT);
use_prv_enum(CPSS_PORT_INTERFACE_MODE_ENT);
use_prv_enum(CPSS_PORT_SPEED_ENT);

/***** declarations ********/                            
                            
/*******************************************************************************
* pvrCpssDxChGetLionLanesBmpGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of Lion lanes bmp (see gtAppDemoLionPortModeSpeedSet).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       ifMode              - port interface mode 
*       speed               - port speed
* 
* OUTPUTS:
*       lanesBmpPtr         - bitmap of SERDES lanes (bit 0-> lane 0, etc.)
*       error_message       - possible error message, relevant 
*                                   if status != GT_OK
*
* RETURNS:
*       GT_OK if no errors occurs otherwise error code and error message
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssDxChGetLionLanesBmpGet
(
    IN  CPSS_PORT_INTERFACE_MODE_ENT        ifMode,
    IN  CPSS_PORT_SPEED_ENT                 speed,
    OUT GT_U32_PTR                          lanesBmpPtr,
    OUT GT_CHAR_PTR                         *error_message
)
{
    GT_STATUS               status          = GT_OK;
    
    *error_message = NULL;
    
    if((ifMode == CPSS_PORT_INTERFACE_MODE_RXAUI_E) && 
       (speed == CPSS_PORT_SPEED_10000_E))
    {
        *lanesBmpPtr = 0x3;
    }
    else if((ifMode == CPSS_PORT_INTERFACE_MODE_SGMII_E) || 
            (ifMode == CPSS_PORT_INTERFACE_MODE_1000BASE_X_E))
    {
        if(speed <= CPSS_PORT_SPEED_1000_E)
        {
            if((ifMode == CPSS_PORT_INTERFACE_MODE_1000BASE_X_E) && 
               (speed < CPSS_PORT_SPEED_1000_E))
            {
                status = GT_BAD_PARAM;
            }
        }
        else if(speed == CPSS_PORT_SPEED_2500_E)
        {
            if(ifMode == CPSS_PORT_INTERFACE_MODE_1000BASE_X_E)
            {
                status = GT_BAD_PARAM;
            }
       }
        else
            status = GT_NOT_SUPPORTED;

        *lanesBmpPtr = 0x1;
    }
    else if(ifMode == CPSS_PORT_INTERFACE_MODE_XGMII_E)
    {
        /* XAUI 10G and 20G supported - lionCub not needed */
        *lanesBmpPtr = 0xf;
    }
    else if(ifMode == CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E)
    {
        /* although it's XAUI just 2 local serdeses of port used */
        *lanesBmpPtr = 0x3;
    }
    else if((ifMode == CPSS_PORT_INTERFACE_MODE_HX_E) && 
            (speed == CPSS_PORT_SPEED_5000_E))
    {
        *lanesBmpPtr = 0x3;
    }
    else if(ifMode == CPSS_PORT_INTERFACE_MODE_XLG_E)
    {
        /* XLG (40G) supported - lionCub not needed */
        *lanesBmpPtr = 0xff; /* 8 serdes (4 ports) used by XLG */
    }
    else
    {
        status = GT_NOT_SUPPORTED;
    }    
    
    if (GT_OK != status)
    {
        *error_message = interfaceModeAndSpeedSupportingErrorMessage;
    }
    
    return status;
}

/*******************************************************************************
* wrlCpssDxChSerdesDefaultPowerUp
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Standart powering up of port serdes (for instance, alter speed and port 
*       mode setting).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                   - lua state (in stack should be device and port 
*                             number)
* 
* OUTPUTS:
*       1 and succed operation code pushed to lua stack
*       2 and succed error code and error message pushed to lua stack
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChSerdesDefaultPowerUp
(
    IN lua_State                            *L
)
{
    GT_U8                   devNum          =  (GT_U8)      lua_tonumber(L, 1); 
                                                                    /* devId */
    GT_PHYSICAL_PORT_NUM    portNum         = 
                                    (GT_PHYSICAL_PORT_NUM)  lua_tonumber(L, 2); 
                                                                    /* portNum */
    const CPSS_PORT_DIRECTION_ENT 
                            direction       = CPSS_PORT_DIRECTION_BOTH_E;
    GT_U32                  lanesBmp        = 0xF;
    const GT_BOOL           powerUp         = GT_TRUE;
    CPSS_PORT_INTERFACE_MODE_ENT   
                            ifMode          = CPSS_PORT_INTERFACE_MODE_NA_E;
    CPSS_PORT_SPEED_ENT     speed           = CPSS_PORT_SPEED_NA_E;
    GT_STATUS               status          = GT_OK;
    GT_CHAR_PTR             error_message   = NULL;                                                                      
    P_CALLING_CHECKING;      

    CPSS_API_CALL_LOCK();
    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH_E:
    case CPSS_PP_FAMILY_CHEETAH2_E:    
        if (PRV_CPSS_PORT_XG_E <= PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum, portNum))
        {
            status = cpssDxChPortSerdesPowerStatusSet(devNum, portNum, 
                                                      direction, lanesBmp, 
                                                      powerUp);                                               
            P_CALLING_API(
                cpssDxChPortSerdesPowerStatusSet,
                PC_NUMBER(devNum,               devNum,                  GT_U8, 
                PC_NUMBER(portNum,              portNum,  GT_PHYSICAL_PORT_NUM, 
                PC_ENUM  (direction,            direction, 
                                                       CPSS_PORT_DIRECTION_ENT,
                PC_NUMBER(lanesBmp,             lanesBmp,               GT_U32, 
                PC_BOOL  (powerUp,              powerUp,
                PC_LAST_PARAM))))),
                PC_STATUS);
            if (GT_OK != status)
            {
                error_message = serdesPoweringErrorMessage;
            }
        } 
        
        break;
        
    case CPSS_PP_FAMILY_CHEETAH3_E:
        if (PRV_CPSS_PORT_XG_E == PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum, portNum))
        {
            status = cpssDxChPortSerdesPowerStatusSet(devNum, portNum, 
                                                      direction, lanesBmp, 
                                                      powerUp);                                                       
            P_CALLING_API(
                cpssDxChPortSerdesPowerStatusSet,
                PC_NUMBER(devNum,               devNum,                  GT_U8, 
                PC_NUMBER(portNum,              portNum,  GT_PHYSICAL_PORT_NUM, 
                PC_ENUM  (direction,            direction, 
                                                       CPSS_PORT_DIRECTION_ENT,
                PC_NUMBER(lanesBmp,             lanesBmp,               GT_U32, 
                PC_BOOL  (powerUp,              powerUp,
                PC_LAST_PARAM))))),
                PC_STATUS);                                                      
            if (GT_OK != status)
            {
                error_message = serdesPoweringErrorMessage;
            }                                                      
        }
        break;
        
    case CPSS_PP_FAMILY_DXCH_XCAT_E:
    case CPSS_PP_FAMILY_DXCH_XCAT3_E:
    case CPSS_PP_FAMILY_DXCH_XCAT2_E:  
            status = cpssDxChPortInterfaceModeGet(devNum, portNum, &ifMode);
            P_CALLING_API(
                cpssDxChPortInterfaceModeGet,
                PC_NUMBER(devNum,               devNum,                  GT_U8, 
                PC_NUMBER(portNum,              portNum,  GT_PHYSICAL_PORT_NUM, 
                PC_ENUM  (ifMode,            ifMode, 
                                                  CPSS_PORT_INTERFACE_MODE_ENT,
                PC_LAST_PARAM))),
                PC_STATUS);               
            if (GT_OK != status)
            {
                error_message = interfaceModeGettingErrorMessage;
            }
        
        if ((GT_OK == status) && 
            (CPSS_PORT_INTERFACE_MODE_MII_PHY_E < ifMode) &&
            (CPSS_PORT_INTERFACE_MODE_NA_E != ifMode))
        {
            status = cpssDxChPortSerdesPowerStatusSet(devNum, portNum, 
                                                      direction, lanesBmp, 
                                                      powerUp);  
            P_CALLING_API(
                cpssDxChPortSerdesPowerStatusSet,
                PC_NUMBER(devNum,               devNum,                  GT_U8, 
                PC_NUMBER(portNum,              portNum,  GT_PHYSICAL_PORT_NUM, 
                PC_ENUM  (direction,            direction, 
                                                       CPSS_PORT_DIRECTION_ENT,
                PC_NUMBER(lanesBmp,             lanesBmp,               GT_U32, 
                PC_BOOL  (powerUp,              powerUp,
                PC_LAST_PARAM))))),
                PC_STATUS);                                                       
            if (GT_OK != status)
            {
                error_message = serdesPoweringErrorMessage;
            }
        }
        break;
        
    case CPSS_PP_FAMILY_DXCH_LION_E:
            status = cpssDxChPortInterfaceModeGet(devNum, portNum, &ifMode);
            P_CALLING_API(
                cpssDxChPortInterfaceModeGet,
                PC_NUMBER(devNum,               devNum,                  GT_U8, 
                PC_NUMBER(portNum,              portNum,  GT_PHYSICAL_PORT_NUM, 
                PC_ENUM  (ifMode,            ifMode, 
                                                  CPSS_PORT_INTERFACE_MODE_ENT,
                PC_LAST_PARAM))),
                PC_STATUS);             
            if (GT_OK != status)
            {
                error_message = interfaceModeGettingErrorMessage;
            }
        
        if (GT_OK == status)
        {
            status = cpssDxChPortSpeedGet(devNum, portNum, &speed);
            P_CALLING_API(
                cpssDxChPortSpeedGet,
                PC_NUMBER(devNum,               devNum,                  GT_U8, 
                PC_NUMBER(portNum,              portNum,  GT_PHYSICAL_PORT_NUM, 
                PC_ENUM  (speed,                speed,     CPSS_PORT_SPEED_ENT,                
                PC_LAST_PARAM))),
                PC_STATUS);              
            if (GT_OK != status)
            {
                error_message = interfaceSpeedGettingErrorMessage;
            }
        }        
        
        if (GT_OK == status)
        {
            status = pvrCpssDxChGetLionLanesBmpGet(ifMode, speed, &lanesBmp, 
                                                   &error_message);
        }
        
        if (GT_OK == status) 
        {
            status = cpssDxChPortSerdesPowerStatusSet(devNum, portNum, 
                                                      direction, lanesBmp, 
                                                      powerUp);  
            P_CALLING_API(
                cpssDxChPortSerdesPowerStatusSet,
                PC_NUMBER(devNum,               devNum,                  GT_U8, 
                PC_NUMBER(portNum,              portNum,  GT_PHYSICAL_PORT_NUM, 
                PC_ENUM  (direction,            direction, 
                                                       CPSS_PORT_DIRECTION_ENT,
                PC_NUMBER(lanesBmp,             lanesBmp,               GT_U32, 
                PC_BOOL  (powerUp,              powerUp,
                PC_LAST_PARAM))))),
                PC_STATUS);                                                       
            if (GT_OK != status)
            {
                error_message = serdesPoweringErrorMessage;
            }
        }
        break;
        
    case CPSS_PP_FAMILY_DXCH_LION2_E:
            status = cpssDxChPortInterfaceModeGet(devNum, portNum, &ifMode);
            P_CALLING_API(
                cpssDxChPortInterfaceModeGet,
                PC_NUMBER(devNum,               devNum,                  GT_U8, 
                PC_NUMBER(portNum,              portNum,  GT_PHYSICAL_PORT_NUM, 
                PC_ENUM  (ifMode,            ifMode, 
                                                  CPSS_PORT_INTERFACE_MODE_ENT,
                PC_LAST_PARAM))),
                PC_STATUS);             
            if (GT_OK != status)
            {
                error_message = interfaceModeGettingErrorMessage;
            }
    
        if ((GT_OK == status) &&
            (CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_156_25_DIFF_E == 
                PRV_CPSS_DXCH_PP_MAC(devNum)->port.serdesRefClock) &&
            (CPSS_PORT_INTERFACE_MODE_NA_E != ifMode))
                    
        {
            status = cpssDxChPortSerdesPowerStatusSet(devNum, portNum, 
                                                      direction, lanesBmp, 
                                                      powerUp);                                                       
            P_CALLING_API(
                cpssDxChPortSerdesPowerStatusSet,
                PC_NUMBER(devNum,               devNum,                  GT_U8, 
                PC_NUMBER(portNum,              portNum,  GT_PHYSICAL_PORT_NUM, 
                PC_ENUM  (direction,            direction, 
                                                       CPSS_PORT_DIRECTION_ENT,
                PC_NUMBER(lanesBmp,             lanesBmp,               GT_U32, 
                PC_BOOL  (powerUp,              powerUp,
                PC_LAST_PARAM))))),
                PC_STATUS);                                                       
            if (GT_OK != status)
            {
                error_message = serdesPoweringErrorMessage;
            }                                                      
        }    
        break;
        
    default:
        break;
    }
    CPSS_API_CALL_UNLOCK();
      
    return prvWrlCpssStatusToLuaPush(status, L, error_message);
}


/*******************************************************************************
* wrlCpssDxChPortNumberOfSerdesLanesGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get number of quantity of SERDESes occupied by given
*       port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       L                   - lua state (in stack should be device and port 
*                             number)
* 
* OUTPUTS:
*       number of returned to lua elements, quantity of SERDESes occupied to lua stack, 
*		if exists, otherwise 0  
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChPortNumberOfSerdesLanesGet(lua_State *L)
{
    GT_U32                          firstSerdes;
    GT_U32                          numOfSerdesLanes;
    GT_U8                   devNum; 
    GT_PHYSICAL_PORT_NUM    portNum;
    GT_STATUS				rc;
    
    if ((lua_gettop(L)==2) &&  (lua_isnumber(L, 1)) && (lua_isnumber(L, 2))  )
    { 
        devNum          =  (GT_U8)      lua_tonumber(L, 1); 
                                                                    /* devId */
        portNum         =  (GT_PHYSICAL_PORT_NUM)  lua_tonumber(L, 2); 
                                                                    /* portNum */
    }
    else
    {
        lua_pushnil(L);
        return 1;
    }
    

    rc = prvCpssDxChPortNumberOfSerdesLanesGet(devNum, portNum, &firstSerdes, &numOfSerdesLanes); 
    if (rc !=GT_OK)
    {
        lua_pushinteger(L,rc);   
        lua_pushnil(L);
         
        return 2;
    }
    lua_pushinteger(L,rc);  
    lua_pushinteger(L,numOfSerdesLanes);
    return 2;
}
