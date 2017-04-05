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
* SerdesRxOptDeviceIf.c
*
* DESCRIPTION:
*       Serdes device interface implemenation
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
******************************************************************************/
#include <serdesOpt/SerdesRxOptDeviceIf.h>
#include <serdesOpt/private/SerdesRxOptAppIf.h>

void lionSerdesFuncRegistration(MV_ACCESS_FUNCS *accFuncs);
void xcatSerdesFuncRegistration(MV_ACCESS_FUNCS *accFuncs);

#ifndef CHX_FAMILY
void milos4SerdesFuncRegistration(MV_ACCESS_FUNCS *accFuncs);
#endif 

int mvSerdesLaneRegistration(unsigned int devNum, MV_ACCESS_FUNCS *accFuncs)
{
    MV_SERDES_TEST_DEV_TYPE devType;

    devType = mvDeviceInfoGetDevType(devNum);
    switch(devType)
    {
        case LionB:
        case BullsEye:
            lionSerdesFuncRegistration(accFuncs);
            break;
        case Xcat:
            xcatSerdesFuncRegistration(accFuncs);
            break;
#ifndef CHX_FAMILY
        case Milos4:
            milos4SerdesFuncRegistration(accFuncs);
            break;
#endif 
        default:
            return 1;
    }

    return 0;
}


