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
*       A lua wrapper for device dependant data and functions.
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*******************************************************************************/

#include <cpss/generic/cpssTypes.h>
#include <lua.h>


/***** declarations ********/

/***** declarations ********/


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
);

/*******************************************************************************
* prvWrlCpssDeviceFamilyGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of divice family.
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
);


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
);


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
);


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
);


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
    OUT   GT_CHAR_PTR           *errorMessage
);


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
);

