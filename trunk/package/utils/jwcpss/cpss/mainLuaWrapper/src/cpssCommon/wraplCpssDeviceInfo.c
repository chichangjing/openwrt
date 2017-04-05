/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplCpssDeviceInfo.c
*
* DESCRIPTION:
*       A lua wrapper for
*           enum    CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT
*           struct  CPSS_PORTS_BMP_STC
*
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 31 $
*******************************************************************************/

/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/config/cpssGenCfg.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/generic/version/cpssGenStream.h>
#ifdef CHX_FAMILY
#include <cpss/dxCh/dxChxGen/diag/cpssDxChDiagDescriptor.h>
#include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#endif
#include <generic/private/prvWraplGeneral.h>
#include <cpssCommon/wraplCpssDeviceInfo.h>
#include <cpssCommon/wraplCpssExtras.h>

#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <cpssCommon/wraplCpssDeviceInfo.h>
#include <cpssCommon/wrapCpssDebugInfo.h>
#include <extUtils/trafficEngine/tgfTrafficTable.h>
#include <extUtils/iterators/extUtilDevPort.h>

#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>
#include <cmdShell/os/cmdOs.h>

#if (!defined(FREEBSD) && !defined(UCLINUX) && !defined(LINUX) && \
     !defined(_WIN32) && !defined(ASIC_SIMULATION))
/* This is standard VxWorks function which returns the BSP version */
extern char *sysBspRev(void);
#endif

extern const GT_CHAR* mvHwsVersionGet(void);

/*******************************************************************************
*   Device info processing errors                                              *
*******************************************************************************/

GT_CHAR deviceInfoBadDeviceNumberErrorMessage[] = "Bad device Id.";
GT_CHAR badDeviceCountErrorMessage[] = "Device count is more that allowed";

/*******************************************************************************
 * External usage variables                                                    *
 ******************************************************************************/
typedef enum{
    PORT_ITERATOR_TYPE_MAC_E,   /* ports with MAC   */
    PORT_ITERATOR_TYPE_RXDMA_E, /* ports with RxDMA */
    PORT_ITERATOR_TYPE_TXQ_E
}PORT_ITERATOR_TYPE_ENT;

static PORT_ITERATOR_TYPE_ENT portIteratorType = PORT_ITERATOR_TYPE_MAC_E;

/* flag for debug to remove the hwDevNum convert*/
GT_BOOL prvWrapHwDevNumConvert = GT_TRUE;


/***** declarations ********/

use_prv_enum(CPSS_PP_FAMILY_TYPE_ENT);
use_prv_enum(CPSS_PP_SUB_FAMILY_TYPE_ENT);
use_prv_enum(PRV_CPSS_SIP_ENT);
use_prv_enum(CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT);

/***** declarations ********/

int check_wraplCpssDeviceConfig()
{
    return 1;
}

/* Device filtering utilities and data */

/* initial state - devices 0-255 enabled */
static int deviceFilterBitmap[] =
{
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
};

void prvWrlDevFilterRangeSet(GT_U8 minDevId, GT_U8 maxDevId, int valid)
{
    GT_U32 devId, word_num, bit_num;

    for (devId = minDevId; (devId <= (GT_U32)maxDevId); devId++)
    {
        word_num = (devId / 32);
        bit_num  = (devId % 32);
        if (valid)
        {
            deviceFilterBitmap[word_num] |= (1 << bit_num);
        }
        else
        {
            deviceFilterBitmap[word_num] &= (~ (1 << bit_num));
        }
    }
}

int prvWrlDevFilterValidGet(GT_U8 devId)
{
    GT_U32 word_num = (devId / 32);
    GT_U32 bit_num  = (devId % 32);
    return ((deviceFilterBitmap[word_num] >> bit_num) & 1);
}

/* parameteres in L: minDevId, maxDevId, valid */
int wrlDevFilterRangeSet(
    lua_State *L
)
{
    GT_U8 minDevId = (GT_U8)lua_tointeger(L, 1);
    GT_U8 maxDevId = (GT_U8)lua_tointeger(L, 2);
    int   valid    = (int)lua_toboolean(L, 3);
    prvWrlDevFilterRangeSet(minDevId, maxDevId, valid);
    return 1;
}

/*******************************************************************************
* prvWrlDevFilterNextDevGet
*
* DESCRIPTION:
*
*       Return the number of the next existing device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number to start from. For the first one devNum should be 0xFF.
*
* OUTPUTS:
*       nextDevNumPtr - number of next device after devNum.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_NO_MORE               - devNum is the last device. nextDevNumPtr will be set to 0xFF.
*       GT_BAD_PARAM             - devNum > max device number
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvWrlDevFilterNextDevGet
(
    IN  GT_U8 devNum,
    OUT GT_U8 *nextDevNumPtr
)
{
    GT_STATUS rc = GT_OK;
    GT_U8     currDev = devNum;
    while (1)
    {
        rc = cpssPpCfgNextDevGet(currDev, &currDev);
        if (rc != GT_OK)
        {
            *nextDevNumPtr = currDev;
            return rc;
        }
        if (prvWrlDevFilterValidGet(currDev))
        {
            *nextDevNumPtr = currDev;
            return GT_OK;
        }
    }
}

/*******************************************************************************
* prvWrlCpssDeviceFamilyGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of device family.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                - device number
*
* OUTPUTS:
*
* RETURNS:
*       device family
*
* COMMENTS:
*
*******************************************************************************/
CPSS_PP_FAMILY_TYPE_ENT prvWrlCpssDeviceFamilyGet
(
    IN GT_U8                    devNum
)
{
    return WRL_PRV_CPSS_PP_MAC(devNum)->devFamily;
}


/*******************************************************************************
* pvrCpssDeviceArrayFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Pop dev array from lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                   - lua state
*       L_index             - entry lua stack index
*
* OUTPUTS:
*       numDevices          - nuber of taking devices
*       deviceArray         - taking devices
*       error_message       - error message (relevant, if GT_OK != status)
*
* RETURNS:
*       1 if array was pushed to stack otherwise 0
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssDeviceArrayFromLuaGet
(
    IN  lua_State               *L,
    IN  GT_32                   L_index,
    OUT GT_U8_PTR               numDevices,
    OUT GT_U8                   deviceArray[],
    OUT GT_CHAR_PTR             *error_message
)
{
    GT_STATUS                   status          = GT_OK;

    *numDevices = 0;

    if (0 != lua_istable(L, L_index))
    {
        lua_pushnil(L);
        while ((GT_OK == status) && (0 != lua_next(L, L_index)))
        {
            if(0 != lua_isnumber(L, -1))
            {
                deviceArray[(*numDevices)++] = (GT_U8) lua_tonumber(L, -1);

                if (GT_FALSE == prvCpssIsDevExists(*numDevices))
                {
                    status          = GT_BAD_PARAM;
                    *error_message  = badDeviceCountErrorMessage;
                }
            }
            lua_pop( L, 1 );
        }
        lua_pop( L, 1);
    }

    return status;
}

/*******************************************************************************
* prvCpssIsDevExists
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Checks of device existance.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum                - device number
*
* OUTPUTS:
*
* RETURNS:
*       GT_TRUE, if exists, otherwise GT_FALSE
*
* COMMENTS:
*
*******************************************************************************/
GT_BOOL prvCpssIsDevExists
(
    IN  GT_U8                   devNum
)
{
    GT_U8                       tempDevNum      = (devNum == 0) ? 0xFF : devNum - 1;

    while (GT_OK == cpssPpCfgNextDevGet(tempDevNum, &tempDevNum))
    {
        if (devNum == tempDevNum)
        {
            return GT_TRUE;
        }
    }

    return GT_FALSE;
}

/*******************************************************************************
* prvCpssMaxDeviceNumberGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of max device number.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*       max device number
*
* COMMENTS:
*
*******************************************************************************/
GT_U8 prvCpssMaxDeviceNumberGet
(
)
{
    GT_U8                       tempDevNum      = 0xFF;
    GT_U8                       maxDevNum       = 0;

    while (GT_OK == cpssPpCfgNextDevGet(tempDevNum, &tempDevNum))
    {
        if (maxDevNum < tempDevNum)
        {
            maxDevNum = tempDevNum;
        }
    }

    return maxDevNum;
}


/*******************************************************************************
* prvCpssPortsDevConvertDataBack
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Provides specific device and port data conversion.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       hwDevNumPtr            - HW device number
*       portNumPtr             - port number
*       errorMessage           - error message
*
* OUTPUTS:
*
* RETURNS:
*       1; pushed to lua stack converted device and port
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPortsDevConvertDataBack
(
    INOUT GT_HW_DEV_NUM         *hwDevNumPtr,
    INOUT GT_PORT_NUM           *portNumPtr,
    OUT GT_CHAR_PTR             *errorMessage
)
{
    GT_HW_DEV_NUM               hwDevNum        = *hwDevNumPtr;
    GT_U8                       index           = 0xFF;
    GT_STATUS                   status          = GT_OK;

    (*portNumPtr)--; (*portNumPtr)++;
    *errorMessage   = NULL;

    if(GT_TRUE == prvWrapHwDevNumConvert)
    {
        /* first we need to find the SW device that hold the hwDevNum */

        /* need to fix value of Max number of PPs in system */
        CPSS_TBD_BOOKMARK_EARCH

        while (GT_OK == cpssPpCfgNextDevGet(index, &index))
        {
            if(prvCpssPpConfig[index] == NULL)
            {
                continue;
            }

            if(PRV_CPSS_HW_DEV_NUM_MAC(index) == hwDevNum)
            {
                /* this is the 'sw_devNum' we look for */
                *hwDevNumPtr  = index;
                break;
            }
        }

        /* if we got here and index that is equal to Max number of PPs in
           system, meaning that the HW-devNum not belongs to any of the cpss
           devices , so we assume 'remote device' and we can't convert it to
           'sw-devNum' so we will use it 'as is' */
    }

    /*for(index = 0 ; index < 4 ; index++)
    {
        if((portDevMapArray[index][2] == (devPortStruct.devNum)) &&
           (portDevMapArray[index][3] == (devPortStruct.portNum)))
        {
            *devNumPtr  = portDevMapArray[index][0];
            *portNumPtr = portDevMapArray[index][1];
            break;
        }
    }*/

    return status;
}


/*******************************************************************************
* prvCpssHwDevicePortConvert
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Provides specific device and port data conversion.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       hwDevNum               - hardware device number
*       hwPortNum              - hardware port number
*
* OUTPUTS:
*       devNumPtr              - device number
*       portNumPtr             - port number
*       errorMessagePtrPtr     - error message
*
* RETURNS:
*       1; pushed to lua stack converted device and port
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssHwDevicePortConvert
(
    IN  GT_HW_DEV_NUM           hwDevNum,
    IN  GT_PHYSICAL_PORT_NUM    hwPortNum,
    OUT GT_U8_PTR               devNumPtr,
    OUT GT_PORT_NUM             *portNumPtr,
    OUT GT_CHAR_PTR             *errorMessagePtrPtr
)
{
    GT_HW_DEV_NUM               currentHwDevNum = hwDevNum;
    GT_STATUS                   status          = GT_OK;

    *portNumPtr  = (GT_PORT_NUM) hwPortNum;
    *errorMessagePtrPtr = NULL;

    status = prvCpssPortsDevConvertDataBack(&currentHwDevNum, portNumPtr,
                                            errorMessagePtrPtr);

    *devNumPtr   = (GT_U8) currentHwDevNum;

    return  status;
}


/*******************************************************************************
* prvCpssIsHwDevicePortExist
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Checks of hw dev/port pair existance.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       hwDevNum              - hardware device number
*       hwPortNum             - hardware port number
*
* OUTPUTS:
*       isExistsPtr           - dev/port existence property
*       errorMessagePtrPtr    - error message
*
* RETURNS:
*       GT_TRUE, if exists, otherwise GT_FALSE
*
* COMMENTS:
*
*******************************************************************************/
GT_BOOL prvCpssIsHwDevicePortExist
(
    IN  GT_HW_DEV_NUM           hwDevNum,
    IN  GT_PHYSICAL_PORT_NUM    hwPortNum,
    OUT GT_BOOL                 *isExistsPtr,
    OUT GT_CHAR_PTR             *errorMessagePtrPtr
)
{
    GT_U8                       devNum;
    GT_PORT_NUM                 portNum;
    GT_STATUS                   status          = GT_OK;

    *isExistsPtr        = GT_TRUE;
    *errorMessagePtrPtr = NULL;

    status = prvCpssHwDevicePortConvert(hwDevNum, hwPortNum, &devNum, &portNum,
                                        errorMessagePtrPtr);


    if (GT_OK == status)
    {
        *isExistsPtr    = extUtilDoesPortExists(devNum,portNum);
    }

    return status;
}


/*******************************************************************************
* wrlCpssDeviceFamilyGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of device family.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                     - lua state
*
* OUTPUTS:
*       2 parameters pushed to lua stack
*       index 0 - device family
*       index 1 - device sub-family
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDeviceFamilyGet
(
    IN lua_State                *L
)
{
    GT_STATUS  rc      = GT_OK;
    GT_U8      devNum;

    PARAM_NUMBER(rc, devNum, 1, GT_U8);

    if (rc != GT_OK)
    {
        /* illegal input parameter */
        return 0; /* return nil */
    }

    if (GT_FALSE == prvCpssIsDevExists(devNum))
    {
        /* device not found */
        return 0; /* return nil */
    }

    /* push : family string */
    prv_c_to_lua_enum(L, enumDescr_CPSS_PP_FAMILY_TYPE_ENT,
                          prvWrlCpssDeviceFamilyGet(devNum));


    /* push : sub-family string */
    prv_c_to_lua_enum(L, enumDescr_CPSS_PP_SUB_FAMILY_TYPE_ENT,
                        WRL_PRV_CPSS_PP_MAC(devNum)->devSubFamily);
    return 2;
}

/*******************************************************************************
* wrlCpssSipVersionCheck
*
* DESCRIPTION:
*       Getting of device SIP version.
*
* APPLICABLE DEVICES:
*        All devices.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                     - lua state
*       param 0     - devNum
*       param 1     - sipId : one of PRV_CPSS_SIP_ENT
*
* OUTPUTS:
*       index 0 - boolean - is the device support this sip
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssSipVersionCheck
(
    IN lua_State                *L
)
{
    GT_STATUS  rc      = GT_OK;
    GT_U8      devNum;
    PRV_CPSS_SIP_ENT    sipId;
    GT_U32      isSip;

    PARAM_NUMBER(rc, devNum, 1, GT_U8);
    if (rc != GT_OK)
    {
        /* illegal input parameter */
        return 0; /* return nil */
    }

    if (GT_FALSE == prvCpssIsDevExists(devNum))
    {
        /* device not found */
        return 0; /* return nil */
    }

    PARAM_ENUM  (rc,  sipId,  2, PRV_CPSS_SIP_ENT);
    if (rc != GT_OK)
    {
        /* illegal input parameter */
        return 0; /* return nil */
    }


    isSip = PRV_CPSS_SIP_CHECK_MAC(devNum,sipId);

    lua_pushboolean(L, isSip);

    return 1;
}

/*******************************************************************************
* wrlCpssDeviceVidxEntriesNumberGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get's device vidx entries number.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                   - lua state (devNum at the Top)
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK, returned value pushed to lua stack.
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDeviceVidxEntriesNumberGet
(
    IN  lua_State               *L
)
{
    /* To implement */
    lua_pushnumber(L, 0);

    return 1;
}


/*******************************************************************************
* wrlCpssDeviceCountGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get's device count.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                   - lua state (devNum at the Top)
*
* OUTPUTS:
*
* RETURNS:
*       1, device count pushed to lua stack.
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDeviceCountGet
(
    IN  lua_State               *L
)
{
    GT_STATUS                   status = GT_OK;

    return pvrCpssNumberToLuaPush(status, L, prvCpssMaxDeviceNumberGet() + 1);
}


/*******************************************************************************
* wrlCpssDevicePortGroupNumberGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get's device's portGoups number (regardless active or non-active)
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                    - lua state (devNum at the Top)
*
* OUTPUTS:
*
* RETURNS:
*       1 if failed. A GT_BAD_PARAM is placed in the Lua stack
*       2 if succes: A GT_OK and port groups number are placed in the stack
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDevicePortGroupNumberGet
(
    IN lua_State *L
)
{
    GT_U8       devNum = (GT_U8) lua_tonumber(L, 1);

    if (GT_FALSE == prvCpssIsDevExists(devNum))
    {
        lua_pushinteger(L, GT_BAD_PARAM);
        return 1;
    }
    else
    {
        lua_pushinteger(L, GT_OK);
        lua_pushinteger(L, PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.numOfPortGroups);
        return 2;
    }
}






/*******************************************************************************
* wrlCpssDevicePortNumberGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get's device port number.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                    - lua state (devNum at the Top)
*
* OUTPUTS:
*
* RETURNS:
*       2, GT_OK and returned value pushed to lua stack
*       2, error code and error message
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDevicePortNumberGet
(
    IN  lua_State               *L
)
{
    GT_U8                       devNum =  (GT_U8) lua_tonumber(L, 1);
    GT_U32                      returnedDeviceRevision = 0;
    GT_STATUS                   status = GT_OK;
    GT_CHAR_PTR                 error_message = NULL;
    int                         returned_param_count    = 0;

    if (GT_FALSE == prvCpssIsDevExists(devNum))
    {
        status        = GT_BAD_PARAM;
        error_message = deviceInfoBadDeviceNumberErrorMessage;
    }

    if (GT_OK == status)
    {
#ifdef CHX_FAMILY
        if(PRV_CPSS_DXCH_PP_HW_INFO_PORT_MAP_CHECK_SUPPORTED_MAC(devNum) == GT_TRUE)
        {

            returnedDeviceRevision = PRV_CPSS_DXCH_MAX_PHY_PORT_NUMBER_MAC(devNum) - 1;
        }
        else
#endif
        {
            returnedDeviceRevision = PRV_CPSS_PP_MAC(devNum)->numOfPorts;
        }
    }

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssNumberToLuaPush(status, L, returnedDeviceRevision);
    return returned_param_count;
}


/*******************************************************************************
* wrlCpssDeviceTypeGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get's device type number.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                   - lua state (devNum at the Top)
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDeviceTypeGet
(
    IN  lua_State               *L
)
{
    GT_STATUS  rc      = GT_OK;
    GT_U8      devNum;

    PARAM_NUMBER(rc, devNum, 1, GT_U8);
    if (rc != GT_OK)
    {
        /* illegal input parameters */
        return 0; /* return nil */
    }

    if (GT_FALSE == prvCpssIsDevExists(devNum))
    {
        /* device not found */
        return 0; /* return nil */
    }

    lua_pushnumber(L, PRV_CPSS_PP_MAC(devNum)->devType);
    return 1;
}


/*******************************************************************************
* wrlCpssDeviceRevisionGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get's device revision number.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                   - lua state
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDeviceRevisionGet
(
    IN  lua_State               *L
)
{
    GT_STATUS  rc      = GT_OK;
    GT_U8      devNum;

    PARAM_NUMBER(rc, devNum, 1, GT_U8);
    if (rc != GT_OK)
    {
        /* illegal input parameters */
        return 0; /* return nil */
    }

    if (GT_FALSE == prvCpssIsDevExists(devNum))
    {
        /* device not found */
        return 0; /* return nil */
    }

    lua_pushnumber (L, PRV_CPSS_PP_MAC(devNum)->revision);
    return 1;
}


/*******************************************************************************
* wrlCpssRouterTtiTcamUtilizationGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get's router tti tcam utilization.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                   - lua state (devNum at the Top)
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK, returned value pushed to lua stack.
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssRouterTtiTcamUtilizationGet
(
    IN  lua_State               *L
)
{
    /* To implement */
    lua_pushnumber(L, 0);

    return 1;
}


/*******************************************************************************
* wrlCpssPolicyTcamUtilizationGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get's policy tcam utilization.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                   - lua state (devNum at the Top)
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK, returned value pushed to lua stack.
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssPolicyTcamUtilizationGet
(
    IN  lua_State               *L
)
{
    /* To implement */
    lua_pushnumber(L, 0);

    return 1;
}


/*******************************************************************************
* wrlCpssPclRuleIdtoPclIdGet
*
* DESCRIPTION:
*       Gets a rule's PCL-id.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                   - lua state (devNum at the Top)
*       first param         - dev id
*       second param        - rule id
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK, returned value pushed to lua stack.
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssPclRuleIdtoPclIdGet
(
    IN  lua_State               *L
)
{
#ifdef CHX_FAMILY
    int rc;
    GT_U32 maskArr[48],patternArr[48],actionArr[48];
#endif
    if (!(lua_isnumber(L,1)) || !(lua_isnumber(L,2)) || !(lua_isnumber(L,3)))
    {
        lua_pushnil(L);
    }
    else
    {
#ifdef CHX_FAMILY
        CPSS_API_CALL_LOCK();
        rc=cpssDxChPclRuleGet((GT_U8) lua_tonumber(L, 1),lua_tointeger(L, 3),lua_tointeger(L, 2),maskArr,patternArr,actionArr);
        CPSS_API_CALL_UNLOCK();
        if (rc==0)
        {
            lua_pushnumber(L, 0x3ff & patternArr[0]);
        }
        else
        {
            lua_pushnil(L);
        }
#else
        /*TODO for puma*/
        lua_pushnil(L);
#endif
    }
    return 1;
}


/*******************************************************************************
* wrlCpssIsPortSetInPortsBmp
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Check's port setting in port bmp mask.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                   - lua state (devNum at the Top)
*
* OUTPUTS:
*
* RETURNS:
*       1; true or false pushed to lua stack.
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssIsPortSetInPortsBmp
(
    IN  lua_State               *L
)
{
    CPSS_PORTS_BMP_STC          portsBmp;
    GT_PORT_NUM                 portNum         = 0;
    GT_BOOL                     isPortSetted    = GT_FALSE;
    GT_STATUS                   status          = GT_OK;
    GT_CHAR_PTR                 error_message   = NULL;

    portNum = (GT_PORT_NUM) lua_tonumber(L, 2);
    status = pvrCpssPortBitmatFromLuaGet(L, 1, &portsBmp, &error_message);

    if (GT_OK == status)
    {
        isPortSetted = CPSS_PORTS_BMP_IS_PORT_SET_MAC(&portsBmp, portNum);
    }
    else
    {
        isPortSetted = GT_FALSE;
    }

    lua_pushboolean(L, isPortSetted);

    return 1;
}


/*******************************************************************************
* wrlCpssClearBmpPort
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Clears port in port bitmap.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                   - lua state (devNum at the Top)
*
* OUTPUTS:
*
* RETURNS:
*       1; pushed to lua stack cleared port bitmap
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssClearBmpPort
(
    IN  lua_State               *L
)
{
    CPSS_PORTS_BMP_STC          portsBmp;
    GT_STATUS                   status          = GT_OK;

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsBmp);

    return pvrCpssPortBitmatToLuaPush(status, L, &portsBmp);
}


/*******************************************************************************
* wrlCpssSetBmpPort
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Sets port in port bitmap.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                   - lua state (devNum at the Top)
*
* OUTPUTS:
*
* RETURNS:
*       1; pushed to lua stack port bitmap
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssSetBmpPort
(
    IN  lua_State               *L
)
{
    CPSS_PORTS_BMP_STC          portsBmp;
    GT_PORT_NUM                 portNum         = 0;
    GT_STATUS                   status          = GT_OK;
    GT_CHAR_PTR                 error_message    = NULL;

    portNum = (GT_PORT_NUM) lua_tonumber(L, 2);
    status = pvrCpssPortBitmatFromLuaGet(L, 1, &portsBmp, &error_message);

    if (GT_OK == status)
    {
        CPSS_PORTS_BMP_PORT_SET_MAC(&portsBmp, portNum);
    }
    else
    {
        cpssOsMemSet(&portsBmp, 0, sizeof(portsBmp));
    }

    return pvrCpssPortBitmatToLuaPush(status, L, &portsBmp);
}


/*******************************************************************************
* wrlCpssPortsDevConvertDataBack
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Provides specific device and port data conversion.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                   - lua state (devNum at the Top)
*
* OUTPUTS:
*
* RETURNS:
*       1; pushed to lua stack converted device and port
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssPortsDevConvertDataBack
(
    IN  lua_State               *L
)
{
    WRAP_DEV_PORT_STC       devPortStruct;
    GT_STATUS               status                  = GT_OK;
    GT_CHAR_PTR             error_message           = NULL;
    int                     returned_param_count    = 0;

    devPortStruct.hwDevNum    = (GT_HW_DEV_NUM) lua_tonumber(L, 1);   /* devId */
    devPortStruct.portNum   = (GT_PORT_NUM) lua_tonumber(L, 2); /* portNum */

    status = prvCpssPortsDevConvertDataBack(&(devPortStruct.hwDevNum),
                                            &(devPortStruct.portNum),
                                            &error_message);

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssNumberToLuaPush(status, L, devPortStruct.hwDevNum);
    returned_param_count    +=
        pvrCpssNumberToLuaPush(status, L, devPortStruct.portNum);
    /*     pvrCpssDevicePortStructToLuaPush(status, L, &devPortStruct)      */
    return returned_param_count;
}


static GT_STATUS portToMac
(
    IN    GT_U8        devNum,
    IN    GT_PORT_NUM  portNum,
    OUT   GT_U32       *portMacNumPtr
)
{
    *portMacNumPtr = portNum;
#ifdef CHX_FAMILY
    if (portIteratorType == PORT_ITERATOR_TYPE_MAC_E)
    {
        /* Macro contains return on error */
        PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(
            devNum, portNum, (*portMacNumPtr));
    }
    else
    if (portIteratorType == PORT_ITERATOR_TYPE_RXDMA_E)
    {
        PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_RXDMA_NUM_GET_MAC(
            devNum, portNum, (*portMacNumPtr));
    }
    else
    if (portIteratorType == PORT_ITERATOR_TYPE_TXQ_E)
    {
        PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_TXQ_PORT_GET_MAC(
            devNum, portNum, (*portMacNumPtr));
    }
    else
    {
        return GT_NOT_IMPLEMENTED;
    }
#endif
    return GT_OK;
}

/*******************************************************************************
* prvCpssCommonAllPortsOfGivenDeviceToLuaPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Pushing of all avaible port of given device to lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                  - lua state
*        devNum             - device number
*        linkUpOnly         - only ports which are Up
*
* OUTPUTS:
*
*
* RETURNS:
*       number of items, that was pushed to lua stack
*
* COMMENTS:
*
******************************************************************************/
int prvCpssCommonAllPortsOfGivenDeviceToLuaPush
(
    IN  lua_State               *L,
    IN  GT_U8                   devNum,
    IN  GT_BOOL                 linkUpOnly
)
{
    GT_STATUS rc;
    GT_PORT_NUM                 port;
    int                         portIndex           = 1;
    GT_PORT_NUM                 numOfPorts;
    GT_U32 portMacNum; /* MAC number */
#ifdef CHX_FAMILY
    numOfPorts = PRV_CPSS_DXCH_MAX_PHY_PORT_NUMBER_MAC(devNum);
#endif
#ifdef EXMXPM_FAMILY
    numOfPorts = PRV_CPSS_PP_MAC(devNum)->numOfPorts;
#endif

    /* create a device table */
    lua_pushnumber(L, devNum);
    lua_newtable(L);
    for (port = 0; port < numOfPorts; port++)
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_PORT_MAP_CHECK_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            if(PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[port].valid == GT_FALSE ||
               PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[port].portMap.mappingType == CPSS_DXCH_PORT_MAPPING_TYPE_CPU_SDMA_E )
            {
                /* cause errors into the CPSS LOG */
                continue;
            }
        }

        rc = portToMac(devNum, port, &portMacNum);
        if (rc != GT_OK)
        {
            continue;
        }

        if (portMacNum < PRV_CPSS_PP_MAC(devNum)->numOfPorts)
        {
            /* array phyPortInfoArray[] allocated according to 'numOfPorts' */
            /* indexed by MAC, not by port */
            if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacNum].portType ==
                                                        PRV_CPSS_PORT_NOT_EXISTS_E)
            {
                continue;
            }
        }

        if (extUtilDoesPortExists(devNum,port) != GT_TRUE)
        {
            continue;
        }

        if (linkUpOnly == GT_TRUE)
        {
            GT_STATUS rc = GT_FAIL;
            GT_BOOL   linkUp = GT_FALSE;
#ifdef CHX_FAMILY
            rc = cpssDxChPortLinkStatusGet(devNum, port, &linkUp);
#endif
#ifdef EXMXPM_FAMILY
            rc = cpssExMxPmPortLinkStatusGet(devNum, port, &linkUp);
#endif
            if ((rc == GT_OK) && (linkUp != GT_TRUE))
                continue;
        }
        lua_pushnumber(L, portIndex);
        lua_pushnumber(L, port);
        lua_settable(L, -3);
        portIndex++;
    }
    lua_settable(L, -3);

    return 1;
}

/*******************************************************************************
* luaCLI_DevInfo_portType_Define
*
* DESCRIPTION:
*       define the port types to be returned by luaCLI_getDevInfo(...)
*       the default behavior is "MAC" ports
*       options are : "RXDMA"
*
* INPUTS:
*       L       - lua_State
*           [0] - type : "MAC" / "RXDMA" /...
*                 when nil --> considered "MAC"
*
* OUTPUTS:
*       only GT_STATUS
*
* RETURNS:
*       none
*
* COMMENTS:
*       None
*
*******************************************************************************/
int luaCLI_DevInfo_portType_Define(lua_State *L)
{
    GT_STATUS rc = GT_OK;

    if (lua_isstring(L, 1))
    {
        const char *str;
        size_t      len;

        str = lua_tolstring(L, 1, &len);

        if (0 == osStrCmp(str,"MAC"))
        {
            portIteratorType = PORT_ITERATOR_TYPE_MAC_E;
        }
        else
        if (0 == osStrCmp(str,"RXDMA"))
        {
            portIteratorType = PORT_ITERATOR_TYPE_RXDMA_E;
        }
        else
        {
            rc = GT_BAD_PARAM;
        }
    }
    else
    {
        /* default */
        portIteratorType = PORT_ITERATOR_TYPE_MAC_E;
    }

    lua_pushinteger(L, (lua_Integer)rc);

    return 1;
}

/*******************************************************************************
* luaCLI_getDevInfo
*
* DESCRIPTION:
*       Get dev info for devices in the system
*
* INPUTS:
*       L       - lua_State (checked device number could be pushed in stack
*                 otherwise all devices will be checked)
*
* OUTPUTS:
*       A lua table with all devices and ports available (for the ports validator)
*
* RETURNS:
*       none
*
* COMMENTS:
*       None
*
*******************************************************************************/
int luaCLI_getDevInfo(lua_State *L)
{
    GT_U8   devNum;
    GT_BOOL linkUpOnly = GT_FALSE;
    GT_BOOL firstDev;
    P_CALLING_CHECKING;

    if (lua_isboolean(L, 2))
    {
        if (lua_toboolean(L, 2))
            linkUpOnly = GT_TRUE;
    }
    if (lua_isnumber(L, 1))
    {
        devNum = (GT_U8) lua_tointeger(L, 1);
        if(GT_FALSE == prvCpssIsDevExists(devNum))
        {
            return 0;
        }
        lua_newtable(L);
        prvCpssCommonAllPortsOfGivenDeviceToLuaPush(L, devNum, linkUpOnly);
    }
    else
    {
        devNum  = 0xFF;
        firstDev = GT_TRUE;

        while (GT_OK == prvWrlDevFilterNextDevGet(devNum, &devNum))
        {
            if (firstDev == GT_TRUE)
            {
                firstDev = GT_FALSE;
                lua_newtable(L);
            }
            P_CALLING_API(
                prvWrlDevFilterNextDevGet,
                PC_NUMBER(devNum,               devNum,                  GT_U8,
                PC_LAST_PARAM),
                GT_OK);
            prvCpssCommonAllPortsOfGivenDeviceToLuaPush(L, devNum, linkUpOnly);
        }
    }

    return 1;
}

/*******************************************************************************
* wrlCpssCommonCpssVersionGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of cpss version name.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                     - lua state
*
* OUTPUTS:
*       2; pushed to lua stack successful execution status and cpss version
*       string
*       2; pushed to lua stack error code and error message
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssCommonCpssVersionGet
(
    IN lua_State                *L
)
{
    GT_CHAR_PTR                 cpssVersion             = CPSS_STREAM_NAME_CNS;
    GT_STATUS                   status                  = GT_OK;
    GT_CHAR_PTR                 error_message           = NULL;
    int                         returned_param_count    = 0;

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssStringToLuaPush(status, L, cpssVersion);
    return returned_param_count;
}

/*******************************************************************************
* wrlCpssCommonCpssVersionGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of cpss version name.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                     - lua state
*
* OUTPUTS:
*     HWS version
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/

int wrlMvHwsVersionGet
(
    IN lua_State                *L
)
{
    lua_pushstring(L, mvHwsVersionGet());
    return 1;
}


/*******************************************************************************
* wrlCpssCommonBspVersionGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of bsp version name.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                     - lua state
*
* OUTPUTS:
*       2; pushed to lua stack successful execution status and bsp version
*       string
*       1; pushed to lua stack only successful execution status is bst version
*       is not accessible
*       2; pushed to lua stack error code and error message
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssCommonBspVersionGet
(
    IN lua_State                *L
)
{
    GT_CHAR_PTR                 bstVersion              = NULL;
    GT_BOOL                     bstVersionExistend;
    GT_STATUS                   status                  = GT_OK;
    GT_CHAR_PTR                 error_message           = NULL;
    int                         returned_param_count    = 0;

    #if (!defined(FREEBSD) && !defined(UCLINUX) && \
         !defined(LINUX)   && !defined(_WIN32) && !defined(ASIC_SIMULATION))
        bstVersion = sysBspRev();
        bstVersionExistend = GT_TRUE;
    #else
        bstVersionExistend = GT_FALSE;
    #endif

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssStringToLuaOnConditionPush(status, L, bstVersion,
                                          bstVersionExistend);
    return returned_param_count;
}


/*******************************************************************************
* wrlCpssCommonBoardNameGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of board name.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                     - lua state
*
* OUTPUTS:
*       2; pushed to lua stack successful execution status and board name string
*       2; pushed to lua stack error code and error message
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssCommonBoardNameGet
(
    IN lua_State                *L
)
{
    /*GT_U8                       devNum                 =
                                                    (GT_U8) lua_tonumber(L, 1);

    GT_CHAR_PTR                 boardName               = NULL;*/

    GT_CHAR_PTR                 boardName               = "n_board";

    GT_STATUS                   status                  = GT_OK;
    GT_CHAR_PTR                 error_message           = NULL;
    int                         returned_param_count    = 0;

/*
    if (prvCpssIsDevExists(devNum))
    {
        boardName = boardsList[PRV_CPSS_PP_MAC(devNum)->boardIdx - 1].boardName;
    }
    else
    {
        status = GT_BAD_PARAM;
        error_message = deviceProcessingGetErrorMessage;
    }
 */
    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssStringToLuaPush(status, L, boardName);
        return returned_param_count;
}


/*******************************************************************************
* luaCLI_getPortGroups
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Retruns a device's active port-group list.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                     - lua state
*       First param           - Device number
*
* OUTPUTS:
*       on success pushes portgroups as an array to lua stack
*       else nil
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/

int luaCLI_getPortGroups(lua_State *L)
{
    GT_U8   devNum;
    GT_U32 portGroupIndex,arrayIndex=1;


    if (lua_isnumber(L, 1))
    {
        devNum = (GT_U8) lua_tointeger(L, 1);
        if(GT_FALSE == prvCpssIsDevExists(devNum))
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }

    lua_newtable(L);

    for((portGroupIndex) = PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.firstActivePortGroup;
           (portGroupIndex) <= PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.lastActivePortGroup; portGroupIndex++)
    {
        if(0 != (PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.activePortGroupsBmp & (1<<(portGroupIndex))))
        {
            lua_pushnumber(L, arrayIndex);
            lua_pushnumber(L, portGroupIndex);
            lua_settable(L, -3);
            arrayIndex++;
        }
    }

    return 1;

}

/*******************************************************************************
* wrlDoesDeviceExists
*
* DESCRIPTION:
*        Check if device exists
*
* APPLICABLE DEVICES:
*        All
*
* NOT APPLICABLE DEVICES:
*        None
*
* INPUTS:
*       L                         - lua state
*       L[1]                      - device number
*
* OUTPUTS:
*
* RETURNS:
*       true if device exists, false - otherwise
*
* COMMENTS:
*
*******************************************************************************/
int wrlDoesDeviceExists
(
    IN lua_State *L
)
{

    GT_U8         devNum;
    GT_STATUS     rc = GT_OK;

    PARAM_NUMBER(rc, devNum, 1, GT_U8);
    if (rc != GT_OK)
    {
        lua_pushboolean(L, 0);
        return 1; /* return false */
    }

    lua_pushboolean(L, (extUtilDoesDeviceExists(devNum) == GT_TRUE) ? 1: 0);
    return 1; /* return bool */
}

/*******************************************************************************
* wrlDoesPortExists
*
* DESCRIPTION:
*        Check if port exists
*
* APPLICABLE DEVICES:
*        All
*
* NOT APPLICABLE DEVICES:
*        None
*
* INPUTS:
*       L                         - lua state
*       L[1]                      - device number
*       L[2]                      - port number
*
* OUTPUTS:
*
* RETURNS:
*       true if devNum/portNum exists, false - otherwise
*
* COMMENTS:
*
*******************************************************************************/
int wrlDoesPortExists
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
        lua_pushboolean(L, 0);
        return 1; /* return false */
    }

    lua_pushboolean(L, (extUtilDoesPortExists(devNum,portNum) == GT_TRUE) ? 1: 0);
    return 1; /* return bool */
}

/*******************************************************************************
* wrlCpssIsAsicSimulation
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       check if running Simulation
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; bobCat2;
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                   - lua state
*
* OUTPUTS:
*
* RETURNS:
*       true if simulation, false otherwise
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssIsAsicSimulation
(
    IN  lua_State               *L
)
{
    #ifdef ASIC_SIMULATION
        lua_pushboolean(L, GT_TRUE);
    #else
        lua_pushboolean(L, GT_FALSE);
    #endif

    return 1;
}

/*******************************************************************************
* wrlCpssIsGmUsed
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       check if running Golden Model(GM) Simulation
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; bobCat2;
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                   - lua state
*
* OUTPUTS:
*
* RETURNS:
*       true if GM, false otherwise
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssIsGmUsed
(
    IN  lua_State               *L
)
{
    #ifdef GM_USED
        lua_pushboolean(L, GT_TRUE);
    #else
        lua_pushboolean(L, GT_FALSE);
    #endif

    return 1;
}



/*******************************************************************************
* wrlCpssGetStartupDirectory
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of appDemo startup directory.
*
* APPLICABLE DEVICES:
*        All.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                     - lua state
*
* OUTPUTS:
*     appDemo startup directory
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/

int wrlCpssGetStartupDirectory
(
    IN lua_State                *L
)
{
#ifndef CLI_STANDALONE
    if (cmdOsConfigFilePath == NULL)
    {
        return 0; /* nil */
    }
    lua_pushstring(L, cmdOsConfigFilePath);
    return 1;
#else
    /* for luaCLI application it is not relevant */
    return 0;
#endif
}

/*******************************************************************************
* luaCLI_convertPhysicalPortsToTxq
*
* DESCRIPTION:
*       function to convert physical ports to TXQ ports
*
* APPLICABLE DEVICES:
*        All.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                     - lua state
*       [1] - devNum
*
* OUTPUTS:
*     table of:
*       each entry [x] = {physical_portNum,txq_PortNum}
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int luaCLI_convertPhysicalPortsToTxq
(
    IN lua_State                *L
)
{
    GT_U8         devNum;
    GT_STATUS     rc = GT_OK;
    GT_U32 portNum;
    GT_U32 txqPortNum; /* MAC number */

    PARAM_NUMBER(rc, devNum, 1, GT_U8);
    if (rc != GT_OK)
    {
        lua_pushboolean(L, 0);
        return 1; /* return false */
    }

    /* create a table */
    lua_newtable(L);

    /* set convert type */
    portIteratorType = PORT_ITERATOR_TYPE_TXQ_E;

    for(portNum = 0 ; portNum < PRV_CPSS_DXCH_MAX_PHY_PORT_NUMBER_MAC(devNum); portNum++)
    {
        rc = portToMac(devNum, portNum, &txqPortNum);
        if (rc != GT_OK)
        {
            continue;
        }

        lua_pushnumber(L, portNum);
        lua_pushnumber(L, txqPortNum);
        lua_settable(L, -3);
    }

    /* restore defaults of convert type */
    portIteratorType = PORT_ITERATOR_TYPE_MAC_E;

    return 1;
}



