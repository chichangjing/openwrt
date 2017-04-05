/*******************************************************************************
*              (c), Copyright 2014, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* extUtilDevPort.h
*
* DESCRIPTION:
*       Device/Port common functions
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/
#ifndef __extUtilDevPorth
#define __extUtilDevPorth

#include <cpssCommon/cpssPresteraDefs.h>

/*
 * Example:
 *
 * GT_U8 devNum;
 *
 * EXT_UTIL_ITERATE_DEVICES_BEGIN(devNum)
 *     do_smth_for_device(devNum);
 * EXT_UTIL_ITERATE_DEVICES_END(devNum)\
 */
#define EXT_UTIL_ITERATE_DEVICES_BEGIN(_devNum)
#define EXT_UTIL_ITERATE_DEVICES_END(_devNum)

/* _type:
 * PHYSICAL
 * PHYSICAL_OR_CPU
 * VIRTUAL
 * MAC
 * TXQ
 * RXDMA
 * TXDMA
 */
#define EXT_UTIL_GET_MAX_PORTS_NUM(_devNum, _type)

/*
 * Example:
 *
 * GT_U8 devNum = 0;
 * GT_PORT_NUM portNum;
 *
 * EXT_UTIL_ITERATE_PORTS_BEGIN(PHYSICAL, devNum, portNum)
 *     do_smth_for_devport(devNum, portNum);
 * EXT_UTIL_ITERATE_PORTS_END(PHYSICAL, devNum, portNum)
 */
#define EXT_UTIL_ITERATE_PORTS_BEGIN(_type, _devNum,_portNum)
#define EXT_UTIL_ITERATE_PORTS_END(_type, _devNum,_portNum)


/*******************************************************************************
* extUtilHwDeviceNumberGet
*
* DESCRIPTION:
*       Return Hardware device Id
*
* INPUTS:
*       swDevNum    - software number
*
* OUTPUTS:
*       hwDevPtr    - HW device id
*
* RETURNS:
*       GT_OK on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extUtilHwDeviceNumberGet
(
  IN  GT_U32              swDevNum,
  OUT GT_HW_DEV_NUM       *hwDevPtr
);

/*******************************************************************************
* extUtilSwDeviceNumberGet
*
* DESCRIPTION:
*       Return software device numver by hardware device id
*
* INPUTS:
*       hwDev       - HW number
*
* OUTPUTS:
*       swDevNumPtr - software number
*
* RETURNS:
*       GT_OK on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extUtilSwDeviceNumberGet
(
  IN  GT_HW_DEV_NUM       hwDev,
  OUT GT_U32              *swDevNumPtr
);

/*******************************************************************************
* extUtilDoesDeviceExists
*
* DESCRIPTION:
*       Return true if device exists (configured)
*
* INPUTS:
*       devNum          - device ID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE if device exist, GT_FALSE otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_BOOL extUtilDoesDeviceExists
(
  IN  GT_U8         devNum
);

/*******************************************************************************
* extUtilDoesPortExists
*
* DESCRIPTION:
*       Return true if port exists
*
* INPUTS:
*       devNum          - device ID
*       portNum         - port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE if port exist, GT_FALSE otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_BOOL extUtilDoesPortExists
(
  IN  GT_U8         devNum,
  IN  GT_PORT_NUM   portNum
);

/*******************************************************************************
* extUtilDoesHwPortExists
*
* DESCRIPTION:
*       Return true if physical port exists
*
* INPUTS:
*       hwDevNum        - HW number
*       portNum         - port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE if port exist, GT_FALSE otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_BOOL extUtilDoesHwPortExists
(
  IN  GT_HW_DEV_NUM hwDevNum,
  IN  GT_PORT_NUM   portNum
);

#endif /* __extUtilDevPorth */
