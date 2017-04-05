/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortTxQHighSpeedPortsDrv.h
*
* DESCRIPTION:
*       bobcat2 and higher TxQ high speed port support
*
* FILE REVISION NUMBER:
*       $Revision: 1$
*******************************************************************************/

#ifndef __CPSS_DXCH_PORT_TXQ_HIGH_SPEED_PORTS_DRV_H
#define __CPSS_DXCH_PORT_TXQ_HIGH_SPEED_PORTS_DRV_H

#include <cpss/generic/cpssTypes.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS        8


/*******************************************************************************
* cpssDxChPortDynamicPATxQHighSpeedPortGet
*
* DESCRIPTION:
*       get list of txQ ports that are declared as  high speed
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2;
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       numberOfHighSpeedPortsPtr - number of TxQ ports to configure as High Speed
*       portNumArr                - array of TxQ ports that are declared as high speed
*       highSpeedPortIdxArr       - array high speed port idx assigned to corresponded TxQ port 
*

*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - bad pointer
*       GT_FAIL         - on error
*
* COMMENTS:
*       if highSpeedPortIdxArr is NULL, port indexes are not filled
*
*******************************************************************************/
GT_STATUS cpssDxChPortDynamicPATxQHighSpeedPortGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *highSpeedPortNumberPtr,
    OUT GT_U32  portNumArr[CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS],
    OUT GT_U32  highSpeedPortIdxArr[CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS]
);

/*******************************************************************************
* cpssDxChPortDynamicPATxQHighSpeedPortDumpGet
*
* DESCRIPTION:
*       get dump of LL and DQ unit 
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2;
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       txqDqPortNumArr     - array of TxQ ports that are declared as high speed
*       txqLLPortNumArr     - array high speed port idx assigned to corresponded TxQ port 
*
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - bad pointer
*       GT_FAIL         - on error
*
* COMMENTS:
*       if high speed port is not assigned to any port , the corresponding entry is filled by ~0
*
*******************************************************************************/
GT_STATUS cpssDxChPortDynamicPATxQHighSpeedPortDumpGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  txqDqPortNumArr[CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS],
    OUT GT_U32  txqLLPortNumArr[CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS]
);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
