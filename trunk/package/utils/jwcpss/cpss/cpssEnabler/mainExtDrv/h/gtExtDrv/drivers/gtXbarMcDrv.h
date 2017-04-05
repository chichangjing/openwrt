/*******************************************************************************
*                   Copyright 2002, GALILEO TECHNOLOGY, LTD.                   *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL.                      *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
*                                                                              *
* MARVELL COMPRISES MARVELL TECHNOLOGY GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, *
* MARVELL INTERNATIONAL LTD. (MIL), MARVELL TECHNOLOGY, INC. (MTI), MARVELL    *
* SEMICONDUCTOR, INC. (MSI), MARVELL ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K.  *
* (MJKK), GALILEO TECHNOLOGY LTD. (GTL) AND GALILEO TECHNOLOGY, INC. (GTI).    *
********************************************************************************
* gtXbarMcDrv.h
*
* DESCRIPTION: APIs used by the TAPI to configure crossbar devices with
*              multicast goup device members.
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/


#ifndef __gtXbarMcDrvh
#define __gtXbarMcDrvh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <gtOs/gtGenTypes.h>


/*******************************************************************************
* extDrvAddDev2XbarMcGroup
*
* DESCRIPTION:  Add a device to a crossbar multicast group
*
*
* INPUTS:
*       ppVidx     - packet processor multicast group index
*       devNum   - Prestera device number joining the multicast group
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error.
*
* COMMENTS:
*       If the packet processor vidx range is greater than the crossbar
*       fabric vidx range, the ppVidx may may need to be folded over into
*       the crossbar 'vidx' range.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvAddDev2XbarMcGroup
(
    IN GT_U16               vidx,
    IN GT_U8                devId
);


/*******************************************************************************
* extDrvRemDevFromXbarMcGroup
*
* DESCRIPTION:  Remove a device from a crossbar multicast group
*
*
* INPUTS:
*       vidx     - TAPI multicast group index
*       devNum   - Prestera device number joining the multicast group
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error.
*
* COMMENTS:
*       If the packet processor vidx range is greater than the crossbar
*       fabric vidx range, the ppVidx may may need to be folded over into
*       the crossbar 'vidx' range.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvRemDevFromXbarMcGroup
(
    IN GT_U16               vidx,
    IN GT_U8                devId
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __gtXbarMcDrvh */

