/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssTmDrop.h
*
* DESCRIPTION:
*       TM Drop Unit configuration privat file.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#ifndef __prvCpssTmDroph
#define __prvCpssTmDroph

#include <cpss/generic/tm/cpssTmPublicDefs.h>

/*******************************************************************************
* prvCpssTmDropProfileGetNext
*
* DESCRIPTION:
*       Get Next Drop profile.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum              - Device number.
*       level               - A Nodes level for the next Drop profile (Port/C/B/A/Q).
*       cos                 - CoS of Drop profile (APPLICABLE RANGES: 0..7).
*       profileIndPtr       - (pointer to) Drop profile Index (APPLICABLE RANGES: 0..MAX_Drop_Profile_Per_Level).
*       
* OUTPUTS:
*       profileStrPtr       - (pointer to) Next Drop profile index.
*
* RETURNS:
*       GT_OK                    - next drop profile does exist.
*       GT_NOT_FOUND             - next drop profile index does not exist.
*
* COMMENTS:
* 
*******************************************************************************/
GT_STATUS prvCpssTmDropProfileGetNext
(
    IN GT_U8                            devNum,
    IN CPSS_TM_LEVEL_ENT                level,
    IN GT_U32                           cos,
    INOUT GT_U32                        *profileIndPtr
);

#endif 	    /* __prvCpssTmDroph */


