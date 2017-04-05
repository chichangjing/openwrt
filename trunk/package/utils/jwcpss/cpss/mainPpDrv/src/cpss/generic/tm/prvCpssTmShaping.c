/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssTmShaping.c
*
* DESCRIPTION:
*       Private utilities for  TM Shaping Configuration verification
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/extServices/os/gtOs/gtGenTypes.h>
/*
#include <cpss/generic/tm/cpssTmServices.h>
#include <cpss/generic/tm/cpssTmCtl.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/tm/cpssTmPublicDefs.h>
#include <tm_ctl.h>
#include <tm_nodes_read.h>
#include <tm_drop.h>
#include <platform/tm_regs.h>
#include <platform/cpss_tm_rw_registers_proc.h>
*/
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>
#include <cpss/generic/cpssHwInit/private/prvCpssHwRegisters.h>
#include <cpss/generic/tm/cpssTmShaping.h>
#include <cpssCommon/cpssPresteraDefs.h>


/* private functions , for usage in command string */
/*******************************************************************************
* prvCpssTmShapingProfileValidate
*
* DESCRIPTION:
**       Validates shaping profile can be configured and prints  updated bursts values if possible
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUT:
*       devNum          - Device number.
*       level           - Level to configure the profile for.
*       uint32_t          cirBw.
*       uint32_t          eirBw.
*       uint32_t          cbs.
*       uint32_t          ebs.
*
* OUTPUT:
*       result of cpssTmShapingProfileValidate call
*       if error is  GT_BAD_SIZE  the right  values of cbs & ebs are printed.
* COMMENTS:
*       None.
*******************************************************************************/

GT_STATUS prvCpssTmShapingProfileValidate 
(
    IN GT_U8                    devNum,
    IN GT_U32                   level,
    IN GT_U32                   cirBw,
    IN GT_U32                   eirBw,
    IN GT_U32                   cbs,
    IN GT_U32                   ebs
)
{
    GT_STATUS rc;
	CPSS_TM_SHAPING_PROFILE_PARAMS_STC      profileStr;
	profileStr.cirBw = cirBw;
	profileStr.cbs = cbs;
	profileStr.eirBw = eirBw;
	profileStr.ebs = ebs;
    rc = cpssTmShapingProfileValidate(devNum,level,&profileStr);
	cpssOsPrintf("result : %d\n", rc);
	if (GT_BAD_SIZE == rc)
	{
		cpssOsPrintf("cbs/ebs  are too small for  desired B/W\n");
		if (profileStr.cirBw) cpssOsPrintf("   cbs should be : %d\n",profileStr.cbs);
		if (profileStr.eirBw) cpssOsPrintf("   ebs should be : %d\n",profileStr.ebs);
	}
    return rc;
}

/*******************************************************************************
* prvCpssTmPortShapingValidate
*
* DESCRIPTION:
**       Validates shaping profile can be configured and prints  updated bursts values if possible
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUT:
*       devNum          - Device number.
*       port_speed        1/2/10/40/50/100  for 1G/2.5G/10G/40g/50G/100G ports appropriately.
*       uint32_t          cirBw.
*       uint32_t          eirBw.
*       uint32_t          cbs.
*       uint32_t          ebs.
*
* OUTPUT:
*       result of cpssTmPortShapingValidate call
*       if error is  GT_BAD_SIZE  the right  values of cbs & ebs are printed.
* COMMENTS:
*       None.
*******************************************************************************/

GT_STATUS prvCpssTmPortShapingValidate
(
    IN GT_U8                    devNum,
    IN GT_U32                   portSpeed,
    IN GT_U32                   cirBw,
    IN GT_U32                   eirBw,
    IN GT_U32                   cbs,
    IN GT_U32                   ebs
)
{
    GT_STATUS rc;
	CPSS_TM_PORT_BW_ENT portType;
	switch (portSpeed)
	{
		case 1  : portType = CPSS_TM_PORT_BW_1G_E; break;
		case 2  : portType = CPSS_TM_PORT_BW_2HG_E; break;
		case 10 : portType = CPSS_TM_PORT_BW_10G_E; break;
		case 40 : portType = CPSS_TM_PORT_BW_40G_E; break;
		case 50 : portType = CPSS_TM_PORT_BW_50G_E; break;
		case 100: portType = CPSS_TM_PORT_BW_100G_E; break;
		default : 
			cpssOsPrintf("wrong portSpeed index . Should be one of  :1/2/10/40/50/100  for 1G/2.5G/10G/40g/50G/100G ports appropriately\n");
			CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
	}
    rc = cpssTmPortShapingValidate(devNum,portType,cirBw, eirBw, &cbs , &ebs);
	cpssOsPrintf("result : %d\n", rc);
	if (GT_BAD_SIZE == rc)
	{
		cpssOsPrintf("cbs/ebs  are too small for  desired B/W\n");
		if (cirBw) cpssOsPrintf("   cbs should be : %d\n",cbs);
		if (eirBw) cpssOsPrintf("   ebs should be : %d\n",ebs);
	}
     return rc;
}

