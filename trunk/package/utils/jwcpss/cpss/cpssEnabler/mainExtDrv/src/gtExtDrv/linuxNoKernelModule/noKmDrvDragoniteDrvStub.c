/*******************************************************************************
*                Copyright 2015, MARVELL SEMICONDUCTOR, LTD.                   *
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
* (MJKK), MARVELL ISRAEL LTD. (MSIL).                                          *
********************************************************************************
* noKmDrvDragoniteDrvStub.c
*
* DESCRIPTION:
*       Includes DRAGONITE functions wrappers, for DRAGONITE manage
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#include <cpssCommon/cpssPresteraDefs.h>
#include <gtExtDrv/drivers/gtDragoniteDrv.h>


#ifdef XCAT_DRV
/*******************************************************************************
* extDrvDragoniteShMemBaseAddrGet
*
* DESCRIPTION:
*       Get start address of communication structure in DTCM
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       dtcmPtr      - Pointer to beginning of DTCM, where
*                      communication structures must be placed
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS extDrvDragoniteShMemBaseAddrGet
(
    OUT  GT_U32  *dtcmPtr
)
{
    (void)dtcmPtr;
    return GT_FAIL;
}

/*******************************************************************************
* extDrvDragoniteSWDownload
*
* DESCRIPTION:
*       Download FW to instruction shared memory
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       For debug purposes only
*
*******************************************************************************/
GT_STATUS extDrvDragoniteSWDownload
(
    GT_VOID
)
{
    return GT_FAIL;
}


/*******************************************************************************
* extDrvDragoniteGetIntVec
*
* DESCRIPTION:
*       Get interrupt line number of Dragonite
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       intVec - interrupt line number
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       For debug purposes only
*
*******************************************************************************/
GT_STATUS extDrvDragoniteGetIntVec
(
    OUT GT_U32 *intVec
)
{
    (void)intVec;
    return GT_FAIL;
}

/*******************************************************************************
* extDrvDragoniteInit
*
* DESCRIPTION:
*       Get interrupt line number of Dragonite
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       For debug purposes only
*
*******************************************************************************/
GT_STATUS extDrvDragoniteInit
(
    GT_VOID
)
{   
    return GT_FAIL;
}

/*******************************************************************************
* extDrvDragoniteEnableSet
*
* DESCRIPTION:
*       Get interrupt line number of Dragonite
*
* INPUTS:
*       enable - GT_TRUE - release Dragonite from reset
*                GT_FALSE - reset Dragonite
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       For debug purposes only
*
*******************************************************************************/
GT_STATUS extDrvDragoniteEnableSet
(
    IN  GT_BOOL enable
)
{
    (void)enable;
    return GT_FAIL;
}

/*******************************************************************************
* extDrvDragoniteFwCrcCheck
*
* DESCRIPTION:
*       This routine executes Dragonite firmware checksum test
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       Mostly for debug purposes, when FW download executed by CPSS CRC check 
*       engaged automatically
*
*******************************************************************************/
GT_STATUS extDrvDragoniteFwCrcCheck
(
    GT_VOID
)
{   
    return GT_FAIL;
}
#endif /* XCAT_DRV */
