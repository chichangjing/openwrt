/*******************************************************************************
*              Copyright 2002, MARVELL TECHNOLOGY, LTD.
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
* brd0011aGppIsr.c
*
* DESCRIPTION:
*       Includes driver interrupts control functions.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/
#include <gtOs/gtGenTypes.h>

typedef GT_U8 (*GT_ISR_FUNCP)
(
    IN void *cookie
);

/*
 * Typedef: enum GT_GPP_ID
 *
 * Description: Holds the different GPP ids in a given Pp.
 *
 * Fields:
 *  GT_GPP_INT_1    - Gpp interrupt pin No.1
 *  GT_GPP_INT_3    - Gpp interrupt pin No.2
 *  GT_GPP_INT_2    - Gpp interrupt pin No.3
 *  GT_UPLINK_GPP   - Uplink Gpp.
 *  GT_GPP_INTERNAL - Gpp used for logical internal interrupt connections.
 *  GT_GPP_MAC_NUM  - Indicates the number of Gpps defined in system.
 *
 * Note:
 *  Don't modify the order or values of this enum.
 */
typedef enum
{
    GT_GPP_INT_1 = 0,
    GT_GPP_INT_2,
    GT_GPP_INT_3,
    GT_UPLINK_GPP,
    GT_GPP_INTERNAL_1,
    GT_GPP_INTERNAL_2,
    GT_GPP_INTERNAL_3,
    GT_GPP_INTERNAL_4,
    GT_GPP_INTERNAL_5,
    GT_GPP_INTERNAL_6,
    GT_GPP_INTERNAL_7,
    GT_GPP_INTERNAL_8,
    GT_GPP_INTERNAL_9,
    GT_GPP_INTERNAL_10,
    GT_GPP_INTERNAL_11,
    GT_GPP_INTERNAL_12,
    GT_GPP_INTERNAL_13,
    GT_GPP_INTERNAL_14,
    GT_GPP_INTERNAL_15,
    GT_GPP_INTERNAL_16,
    GT_GPP_INTERNAL_17,
    GT_GPP_MAX_NUM
}GT_GPP_ID;

/*******************************************************************************
* extDrvConnectGppIsr
*
* DESCRIPTION:
*       This function connects an Isr for a given Gpp interrupt of
*       the specified Pp device number.
*
* INPUTS:
*       devNum      - The Pp device number at which the Gpp device is conncted.
*       gppId       - The Gpp Id to be connected.
*       isrFuncPtr  - A pointer to the function to be called on Gpp interrupt
*                     receiption.
*       cookie      - A cookie to be passed to the isrFuncPtr when its called.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       1.  To disconnect a Gpp Isr, call this function with a NULL parameter in
*           the isrFuncPtr param.
*
*******************************************************************************/
GT_STATUS extDrvConnectGppIsr
(
    IN  GT_U8           devNum,
    IN  GT_GPP_ID       gppId,
    IN  GT_ISR_FUNCP    isrFuncPtr,
    IN  void            *cookie
)
{
   return GT_FAIL;
}


