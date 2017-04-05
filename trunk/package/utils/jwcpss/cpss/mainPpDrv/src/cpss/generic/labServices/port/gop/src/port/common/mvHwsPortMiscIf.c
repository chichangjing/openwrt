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
* mvHwsPortMiscIf.c
*
* DESCRIPTION:
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 84 $
******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>

#include <mvHwsPortInitIf.h>
#include <private/mvHwsPortPrvIf.h>
#include <private/mvHwsPortMiscIf.h>
#include <private/mvPortModeElements.h>
#include <serdes/mvHwsSerdesPrvIf.h>
#include <pcs/mvHwsPcsIf.h>
#include <mac/mvHwsMacIf.h>


const GT_CHAR* HWS_VERSION = (const GT_CHAR*) "SW_INFRA_HWSERVICES_4.1_127";

/*******************************************************************************
* mvHwsVersionGet
*
* DESCRIPTION:
*       Returns version of HWS APIs.
*
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*     Pointer to version string
*
*******************************************************************************/
const GT_CHAR* mvHwsVersionGet(void)
{
  return HWS_VERSION;
}

