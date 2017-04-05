/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortDynamicPAGCDAlgo.h
*
* DESCRIPTION:
*       GCD (greatest common divider) algorithm
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*******************************************************************************/
#ifndef __PRV_CPSS_DXCH_DYN_PA_TYPEDEF_H
#define __PRV_CPSS_DXCH_DYN_PA_TYPEDEF_H

#ifndef STAND_ALONE
    #include <cpss/generic/cpssTypes.h>
    #include <cpss/extServices/os/gtOs/gtEnvDep.h>
#else
    #include "CpssInclude/cpssTypes.h"
#endif

#define PRV_CPSS_DXCH_PORT_NUM_CNS              512
#define PRV_CPSS_DXCH_DYNAMIC_PA_SLICE_NUM_CNS 2000

#define PRV_CPSS_DXCH_PA_BW_COEFF  1000   /* IN MBit */

#endif


