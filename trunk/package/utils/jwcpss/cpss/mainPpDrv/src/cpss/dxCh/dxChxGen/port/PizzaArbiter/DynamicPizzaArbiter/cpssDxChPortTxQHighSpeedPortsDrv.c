/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortTxQHighSpeedPortsDrv.c
*
* DESCRIPTION:
*       bobcat2 and higher TxQ high speed port support
*
* FILE REVISION NUMBER:
*       $Revision: 6$
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortLog.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortMapping.h>
#include <cpss/dxCh/dxChxGen/port/PortMapping/prvCpssDxChPortMappingShadowDB.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortTxQHighSpeedPortsDrv.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/cpssDxChPortTxQHighSpeedPortsDrv.h>


/*-------------------------------------------------------------------------------
 * Genaral design
 *
 *
 * 1.  DQ Unit
 *     a.  Scheduler profile:
 *         1. /Cider/EBU-SIP/TXQ_IP/SIP5.0 (Bobcat2)/TXQ_IP_SIP5 {Current}/TXQ_IP_dq/Scheduler/Priority Arbiter Weights/Port<%n> Scheduler Profile
 *         2. Total 72 register (per TxQ ports ?)
 *     3.  Total 16 profiles. Each port is assigned a single profile. Several ports may share same profile.
 *         b.  Enable Fast Scheduler
 *             1.  /Cider/EBU-SIP/TXQ_IP/SIP5.0/TXQ_IP_SIP5 {Current}/TXQ_IP_dq/Global/Global DQ Config/Profile <%p> Byte count modification register   field Enable High Speed Scheduler Profile <%p>
 *             2.       Total 16 profiles
 *
 * When particular port is declared as <High Speed>, Its <Enable Fast Scheduler> shall be set to 1. You propose set <Enable Fast Scheduler> = 1 to all  profiles . Is that right ?
 *
 *         c.  Port Shaper
 *             1.  Port Request Mask
 *                 1. /Cider/EBU-SIP/TXQ_IP/SIP5.0/TXQ_IP_SIP5 {Current}/TXQ_IP_dq/Scheduler/Port Shaper/Port Request Maskf
 *                 2. total 4 different masks
 *                 3. default value – 3 (Register misconfiguration changed to 4) , for High Speed Port mask shall be 0
 *
 *             2.  Port Request Mask Selector
 *                 1.  /Cider/EBU-SIP/TXQ_IP/SIP5.0/TXQ_IP_SIP5 {Current}/TXQ_IP_dq/Scheduler/Port Shaper/Port <%n> Request Mask Selector
 *                 2.  total 72 (per TxQ Port )
 *                 3.  Default value 0 (? )
 *
 * Lets assign the mask #3 for High Speed Ports, than
 *     when port declared as <High Speed>, its  Port Request Mask Selector  shall be changed to #3  from default value
 *
 * Bobcat2 B0
 *-----------
 *   In order to enable fix the following configurations must be applied:
 *   1. 0x40000A00[16] = 0x1 (DQ Metal Fix Register)
 *
 *   /Cider/EBU-SIP/TXQ_IP/SIP5.0 (Bobcat2)/TXQ_IP_SIP5 {Current}/TXQ_IP_dq/Global/Debug Bus/DQ Metal Fix Register
 *   'Dq_Metal_Fix'  => 0x1ffff,   # enable MF
 *
 *   This should be done only if TM is enabled
 *-------------------------------------------------------------------------------*/


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
GT_STATUS internal_cpssDxChPortDynamicPATxQHighSpeedPortGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *highSpeedPortNumberPtr,
    OUT GT_U32  portNumArr[CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS],
    OUT GT_U32  highSpeedPortIdxArr[CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS]
)
{
    GT_STATUS rc;
    GT_U32    portNum;
    GT_BOOL   isEnabled;
    GT_U32    highSpeedPortIdx;
    GT_U32    highSpeedPortNumber;
    GT_U32    var_PRV_CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(highSpeedPortNumberPtr);
    CPSS_NULL_PTR_CHECK_MAC(portNumArr);

    if (!PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        var_PRV_CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM = 1;
    }
    else
    {
        var_PRV_CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM = PRV_CPSS_DXCH_BOBCAT2_B0_PORT_TXQ_DQ_HIGH_SPEED_PORT_NUM_CNS;
    }


    for (highSpeedPortIdx = 0 ; highSpeedPortIdx < var_PRV_CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM; highSpeedPortIdx++)
    {
        portNumArr[highSpeedPortIdx] = PRV_CPSS_DXCH_HIGH_SPEED_PORT_INVALID_CNS;
        if (highSpeedPortIdxArr != NULL)
        {
            highSpeedPortIdxArr[highSpeedPortIdx] = PRV_CPSS_DXCH_HIGH_SPEED_PORT_INVALID_CNS;
        }
    }
    /*----------------------------------------------------------------------------*/
    /* build map of alredy configured port                                        */
    /* when port is not mapped use PRV_CPSS_DXCH_HIGH_SPEED_PORT_INVALID_CNS      */
    /*----------------------------------------------------------------------------*/
    highSpeedPortNumber = 0;
    for (highSpeedPortIdx = 0 ; highSpeedPortIdx < var_PRV_CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM; highSpeedPortIdx++)
    {
        rc = prvCpssDxChPortTxQUnitDQHighSpeedPortGet(devNum,highSpeedPortIdx,/*OUT*/&isEnabled,&portNum);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (isEnabled == GT_TRUE)
        {
            portNumArr[highSpeedPortNumber] = portNum;
            if (highSpeedPortIdxArr != NULL)
            {
                highSpeedPortIdxArr[highSpeedPortNumber] = highSpeedPortIdx;
            }
            highSpeedPortNumber++;
        }
    }
    *highSpeedPortNumberPtr = highSpeedPortNumber;
    return GT_OK;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortDynamicPATxQHighSpeedPortGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, highSpeedPortNumberPtr, portNumArr,highSpeedPortIdxArr));

    rc = internal_cpssDxChPortDynamicPATxQHighSpeedPortGet(devNum, highSpeedPortNumberPtr, portNumArr,highSpeedPortIdxArr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, highSpeedPortNumberPtr, portNumArr,highSpeedPortIdxArr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* cpssDxChPortDynamicPATxQHighSpeedPortDumpGet
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
*       BOBCAT2 B0: only txq port 64 can be declared as High Speed Port
*
*******************************************************************************/
GT_STATUS internal_cpssDxChPortDynamicPATxQHighSpeedPortDumpGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  txqDqPortNumArr[CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS],
    OUT GT_U32  txqLLPortNumArr[CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS]
)
{
    GT_STATUS rc;
    GT_U32    portNum;
    GT_BOOL   isEnabled;
    GT_U32    highSpeedPortIdx;
    GT_U32    var_PRV_CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(txqDqPortNumArr);
    CPSS_NULL_PTR_CHECK_MAC(txqLLPortNumArr);

    if (!PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        var_PRV_CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM = 1;
    }
    else
    {
        var_PRV_CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM = PRV_CPSS_DXCH_BOBCAT2_B0_PORT_TXQ_DQ_HIGH_SPEED_PORT_NUM_CNS;
    }


    for (highSpeedPortIdx = 0 ; highSpeedPortIdx < var_PRV_CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM; highSpeedPortIdx++)
    {
        txqDqPortNumArr[highSpeedPortIdx] = PRV_CPSS_DXCH_HIGH_SPEED_PORT_INVALID_CNS;
        txqLLPortNumArr[highSpeedPortIdx] = PRV_CPSS_DXCH_HIGH_SPEED_PORT_INVALID_CNS;
    }
    /*----------------------------------------------------------------------------*/
    /* build map of alredy configured port                                        */
    /* when port is not mapped use PRV_CPSS_DXCH_HIGH_SPEED_PORT_INVALID_CNS      */
    /*----------------------------------------------------------------------------*/
    for (highSpeedPortIdx = 0 ; highSpeedPortIdx < var_PRV_CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM; highSpeedPortIdx++)
    {
        rc = prvCpssDxChPortTxQUnitDQHighSpeedPortGet(devNum,highSpeedPortIdx,/*OUT*/&isEnabled,&portNum);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (isEnabled == GT_TRUE)
        {
            txqDqPortNumArr[highSpeedPortIdx] = portNum;
        }
        rc = prvCpssDxChPortTxQUnitLLHighSpeedPortGet(devNum,highSpeedPortIdx,/*OUT*/&isEnabled,&portNum);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (isEnabled == GT_TRUE)
        {
            txqLLPortNumArr[highSpeedPortIdx] = portNum;
        }

    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortDynamicPATxQHighSpeedPortDumpGet
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
*       BOBCAT2 B0: only txq port 64 can be declared as High Speed Port
*
*******************************************************************************/
GT_STATUS cpssDxChPortDynamicPATxQHighSpeedPortDumpGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  txqDqPortNumArr[CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS],
    OUT GT_U32  txqLLPortNumArr[CPSS_DXCH_PORT_HIGH_SPEED_PORT_NUM_CNS]
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortDynamicPATxQHighSpeedPortDumpGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum,  txqDqPortNumArr, txqLLPortNumArr));

    rc = internal_cpssDxChPortDynamicPATxQHighSpeedPortDumpGet(devNum, txqDqPortNumArr, txqLLPortNumArr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum,  txqDqPortNumArr, txqLLPortNumArr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}




