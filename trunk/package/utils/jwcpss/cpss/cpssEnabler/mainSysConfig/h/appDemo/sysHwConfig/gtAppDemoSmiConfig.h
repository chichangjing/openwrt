/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* gtAppDemoPciConfig.h
*
* DESCRIPTION:
*       Prestera Devices smi initialization & detection module.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.2.1 $
*
*******************************************************************************/
#ifndef __gtAppDemoSmiConfigh
#define __gtAppDemoSmiConfigh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <gtExtDrv/drivers/gtSmiDrv.h>
#include <appDemo/os/appOs.h>


#define GT_SMI_VENDOR_ID    0x11AB

/*
 * typedef: struct GT_SMI_DEV_VENDOR_ID
 *
 * Description: SMI device and vendor ID struct
 *
 * Fields:
 *   vendorId - The Prestera SMI vendor Id.
 *   devId    - The different Prestera SMI device Id.
 */
typedef struct
{
    GT_U16  vendorId;
    GT_U16  devId;
} GT_SMI_DEV_VENDOR_ID;


/*
 * typedef: struct GT_SMI_INFO
 *
 * Description: SMI device information
 *
 * Fields:
 *      smiDevVendorId - The Prestera SMI vendor Id.
 *
 */
typedef struct
{
    GT_SMI_DEV_VENDOR_ID    smiDevVendorId;
    GT_U32                  smiIdSel;
} GT_SMI_INFO;



/*******************************************************************************
* gtPresteraGetSmiDev
*
* DESCRIPTION:
*       This routine search for Prestera Devices Over the SMI.
*
* INPUTS:
*       first - whether to bring the first device, if GT_FALSE return the next
*               device.
*
* OUTPUTS:
*       smiInfo - the next device SMI info.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*       GT_NO_MORE - no more prestera devices.
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
* Toolkit:
*
*******************************************************************************/
GT_STATUS gtPresteraGetSmiDev
(
    IN  GT_BOOL     first,
    OUT GT_SMI_INFO *smiInfo
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* __gtAppDemoSmiConfigh */
