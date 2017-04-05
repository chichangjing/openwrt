/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* SersesRxOptVipsIf.c
*
* DESCRIPTION:
*       VIPS IF implementation needed by Serdes Optimizer system.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
******************************************************************************/

#include "SerdesRxOptAppIf.h"
#include <api/sysConf/gtSysConf.h>
#include <api/hwIf/gtHwIf.h>
#include <common/os/gtOs.h>

/*******************************************************************************
* mvDeviceInfoGetDevType
*
* DESCRIPTION:
*       Return device type for device number.
*	    Supported device types are: LionB, Xcat.
*
* INPUTS:
*       devNum - device number in the system.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Current device type or Unknown.
*
*******************************************************************************/
MV_SERDES_TEST_DEV_TYPE mvDeviceInfoGetDevType
(
    GT_U32    devNum
)
{
    return Unknown;
}

/*******************************************************************************
* genRegisterSet
*
* DESCRIPTION:
*       Implement write access to device registers.
*
* INPUTS:
*       devNum    - Device Number
*       portGroup - port group (core) number
*		address   - address to access
* 		data      - data to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
int genRegisterSet(GT_U8 devNum, GT_U32 portGroup, GT_U32 address, GT_U32 data, GT_U32 mask)
{
    return 0;
}

/*******************************************************************************
* genRegisterGet
*
* DESCRIPTION:
*       Read access to device registers.
*
* INPUTS:
*       devNum    - Device Number
*       portGroup - port group (core) number
*		address   - address to access
*
* OUTPUTS:
* 		data      - read data
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
int genRegisterGet (GT_U8 devNum, GT_U32 portGroup, GT_U32 address, GT_U32 *data, GT_U32 mask)
{
        
    return 0;
}


