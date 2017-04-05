 /*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplCpssDxChSerdesSequence.c
*
* DESCRIPTION:
*       A lua wrapper for serdes sequence functions.
*
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 11 $
*******************************************************************************/

/* max number of MRU indexes */

#ifndef __wraplCpssDxChSerdesSequence__
#define __wraplCpssDxChSerdesSequence__

#include <cpssCommon/wrapCpssDebugInfo.h>


/*******************************************************************************
* prvCpssDxChSerdesConfigurationSet
*
* DESCRIPTION:
*       Serdes Configuration Set
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        devNum                   - device number
*        portGroup                - port-group number
*        seqType                  - serdes sequence type  
*        lineNum                  - line number
*        operationType            - operation type
*        unitIndex                - unit index
*        regAddress               - reg address
*        dataToWrite              - data to write
*        mask                     - mask
*        delay                    - delay
*        numOfLoops               - number of loops
*
* OUTPUTS:
*
* RETURNS:
*       2; GT_OK and next port vlan id is pused to lua stack if no errors
*       occurs and such port exists
*       1; GT_OK is pused to lua stack if no errors occurs and such vlan is not
*       exists
*       2; error code and error message, if error occures
*
* COMMENTS:
*
*******************************************************************************/

static GT_STATUS prvCpssDxChSerdesConfigurationSet(
  /*!     INPUTS:             */
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  seqType,
    GT_U32  lineNum,
    GT_U32  operationType,
    GT_U32  baseAddress,
    GT_U32  regAddress,
    GT_U32  dataToWrite,
    GT_U32  mask,
    GT_U32  delay,
    GT_U32  numOfLoops
);

/*******************************************************************************
* prvCpssDxChSerdesConfigurationGet
*
* DESCRIPTION:
*       Serdes Configuration Set
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        devNum                   - device number
*        portGroup                - port-group number
*        seqType                  - serdes sequence type  
*
* OUTPUTS:
*
* RETURNS:
*       2; GT_OK and next port vlan id is pused to lua stack if no errors
*       occurs and such port exists
*       1; GT_OK is pused to lua stack if no errors occurs and such vlan is not
*       exists
*       2; error code and error message, if error occures
*
* COMMENTS:
*
*******************************************************************************/

GT_STATUS prvCpssDxChSerdesConfigurationGet(
  /*!     INPUTS:             */
      IN  GT_U8       devNum,
      IN  GT_U32      portGroup,
      IN  GT_U32      seqType
      
);
#endif /* __wraplCpssDxChSerdesSequence__ */
