/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChXLGMacCtrl.c
*
* DESCRIPTION:
*       bobcat2 XLG mac control reg 5 API
*
* FILE REVISION NUMBER:
*       $Revision: 7$
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/PortMapping/prvCpssDxChPortMappingShadowDB.h>
#include <cpss/dxCh/dxChxGen/port/macCtrl/prvCpssDxChXLGMacCtrl.h>
#include <cpss/dxCh/dxChxGen/port/macCtrl/prvCpssDxChMacCtrl.h>

/*---------------------------------------------------------------------------------------------
 * /Cider/EBU/Bobcat2/Bobcat2 {Current}/Switching Core/GOP/<Gige MAC IP> Gige MAC IP Units%g/Tri-Speed Port MAC Configuration/Port MAC Control Register1
 * 15 - 15  Short preable    0x0 -- 8 bytes
 *                           0x1 -- 4 bytes
 * /Cider/EBU/Bobcat2/Bobcat2 {Current}/Switching Core/GOP/<Gige MAC IP> Gige MAC IP Units%g/Tri-Speed Port MAC Configuration/Port MAC Control Register3
 * 6-14     IPG 
 *---------------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------------
 * /Cider/EBU/Bobcat2/Bobcat2 {Current}/Switching Core/GOP/<XLG MAC IP> XLG MAC IP Units%p/Port MAC Control Register5
 * 
 * 
 *  0 -  3  TxIPGLength          minimal vaue is 8 for 10G and 40G
 *  4 -  6  PreambleLengthTx     0 -- 8 bytes 
 *                               1..7 -- 1..7-bytes
 *                               for 10G 4,8 are only allowed 
 *  7 -  9  PreambleLengthRx     0 -- 8 bytes 
 *                               1..7 -- 1..7-bytes
 * 10 - 12  TxNumCrcBytes        legal value 1,2,3,4
 * 13 - 15  RxNumCrcBytes        legal value 1,2,3,4
 *---------------------------------------------------------------------------------------------
 */



/*******************************************************************************
* prvCpssDxChBobcat2PortMacXLGIsSupported
*
* DESCRIPTION:
*       check whether XLG mac is supported for specific mac
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       mac      - mac number
*
* OUTPUTS:
*       isSupportedPtr. is mac supported result
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - on bad ptr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortMacXLGIsSupported
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     mac,
    OUT GT_BOOL                *isSupportedPtr
)
{
    GT_U32 regAddr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);
    CPSS_NULL_PTR_CHECK_MAC(isSupportedPtr);

    PRV_CPSS_DXCH_PORT_MAC_CTRL5_REG_MAC(devNum, mac, PRV_CPSS_PORT_XLG_E, &regAddr);

    if(regAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        *isSupportedPtr = GT_FALSE;
    }
    else
    {
        *isSupportedPtr = GT_TRUE;
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChBobcat2PortXLGMacCtrl5FldSet
*
* DESCRIPTION:
*       XLG mac control register 5 set specific field
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum      - physical port number
*       fldOffs  - fld offset 
*       fldLen   - fld len
*       fldVal   - value to write
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortXLGMacCtrl5FldSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_U32                   fldOffs,
    IN GT_U32                   fldLen,
    IN GT_U32                   fldVal
)
{
    GT_STATUS rc;
    GT_U32 regAddr;
    GT_PHYSICAL_PORT_NUM    portMacNum;
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    rc = prvCpssDxChMacByPhysPortGet(devNum, portNum, &portMacNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL5_REG_MAC(devNum, portMacNum, PRV_CPSS_PORT_XLG_E, &regAddr);

    if(regAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    regDataArray[PRV_CPSS_PORT_GE_E].regAddr = regAddr;
    regDataArray[PRV_CPSS_PORT_GE_E].fieldData = fldVal;
    regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = fldLen;
    regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = fldOffs;
    
    rc = prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);

    return rc;
}

/*******************************************************************************
* prvCpssDxChBobcat2PortXLGMacCtrl5FldGet
*
* DESCRIPTION:
*       XLG mac control register 5 get specific field
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       mac      - physical port number
*       fldOffs  - fld offset 
*       fldLen   - fld len
*
* OUTPUTS:
*       fldValPtr   - value to write
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - on NULL ptr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortXLGMacCtrl5FldGet
(
    IN  GT_U8                    devNum,
    IN  GT_PHYSICAL_PORT_NUM     mac,
    IN  GT_U32                   fldOffs,
    IN  GT_U32                   fldLen,
    OUT GT_U32                  *fldValPtr
)
{
    GT_STATUS rc;
    GT_U32 regAddr;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);
    CPSS_NULL_PTR_CHECK_MAC(fldValPtr);

    PRV_CPSS_DXCH_PORT_MAC_CTRL5_REG_MAC(devNum, mac, PRV_CPSS_PORT_XLG_E, &regAddr);

    if(regAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    rc = prvCpssHwPpPortGroupGetRegField(devNum, 0, regAddr,
                                            fldOffs, 
                                            fldLen, 
                                            fldValPtr);
    return rc;
}

/*******************************************************************************
* prvCpssDxChBobcat2PortXLGMacIPGLengthSet
*
* DESCRIPTION:
*       XLG mac control register 5 set IPG length
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       mac      - physical port number
*       length   - ipg length
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortXLGMacIPGLengthSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_U32                   length
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    /* if (length < XLG_MAC_CTRL5_TX_IPG_MIN_VAL_D || */ /* currently minimal possible length 0 -- technically */
    if (length > XLG_MAC_CTRL5_TX_IPG_MAX_VAL_D) 
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChBobcat2PortXLGMacCtrl5FldSet(devNum,portNum
                                ,XLG_MAC_CTRL5_TX_IPG_LEN_FLD_OFFS_D
                                ,XLG_MAC_CTRL5_TX_IPG_LEN_FLD_LEN_D 
                                ,length);
    return rc;
}


/*******************************************************************************
* prvCpssDxChBobcat2PortXLGMacIPGLengthGet
*
* DESCRIPTION:
*       XLG mac control register 5 get IPG length
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       mac      - physical port number
*
* OUTPUTS:
*       lengthPtr - pointer to length
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, mac
*       GT_BAD_PTR      - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortXLGMacIPGLengthGet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     mac,
    IN GT_U32                  *lengthPtr
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);
    CPSS_NULL_PTR_CHECK_MAC(lengthPtr);
    *lengthPtr = 0;

    rc = prvCpssDxChBobcat2PortXLGMacCtrl5FldGet(devNum,mac
                                ,XLG_MAC_CTRL5_TX_IPG_LEN_FLD_OFFS_D
                                ,XLG_MAC_CTRL5_TX_IPG_LEN_FLD_LEN_D 
                                ,/*OUT*/lengthPtr);
    if (rc != GT_OK)
    {
        *lengthPtr = 0;
        return rc;
    }
    return GT_OK;
}



/*******************************************************************************
* prvCpssDxChBobcat2PortXLGMacPreambleLengthSet
*
* DESCRIPTION:
*       XLG mac control register 5 set Preamble length
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum      - max number
*       direction - RX/TX/both
*       length   - preamble length
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortXLGMacPreambleLengthSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_U32                   length
)
{
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    if (length < XLG_MAC_CTRL5_PREAMBLE_MIN_VAL_D || length > XLG_MAC_CTRL5_PREAMBLE_MAX_VAL_D)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if (length == XLG_MAC_CTRL5_PREAMBLE_MAX_VAL_D) /* see comment at cider  swicthing code/GOP/XLG mac IP/units/Port Mac Control register 5 */
    {
        length = 0;
    }

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
            rc = prvCpssDxChBobcat2PortXLGMacCtrl5FldSet(devNum,portNum
                                        ,XLG_MAC_CTRL5_PREAMBLE_LEN_RX_FLD_OFFS_D 
                                        ,XLG_MAC_CTRL5_PREAMBLE_LEN_RX_FLD_LEN_D  
                                        ,length);
            if (rc != GT_OK)
            {
                return rc;
            }
        break;
        case CPSS_PORT_DIRECTION_TX_E:
            rc = prvCpssDxChBobcat2PortXLGMacCtrl5FldSet(devNum,portNum
                                        ,XLG_MAC_CTRL5_PREAMBLE_LEN_TX_FLD_OFFS_D 
                                        ,XLG_MAC_CTRL5_PREAMBLE_LEN_TX_FLD_LEN_D  
                                        ,length);
            if (rc != GT_OK)
            {
                return rc;
            }
        break;
        case CPSS_PORT_DIRECTION_BOTH_E:
            rc = prvCpssDxChBobcat2PortXLGMacCtrl5FldSet(devNum,portNum
                                        ,XLG_MAC_CTRL5_PREAMBLE_LEN_RX_FLD_OFFS_D 
                                        ,XLG_MAC_CTRL5_PREAMBLE_LEN_RX_FLD_LEN_D  
                                        ,length);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDxChBobcat2PortXLGMacCtrl5FldSet(devNum,portNum
                                        ,XLG_MAC_CTRL5_PREAMBLE_LEN_TX_FLD_OFFS_D 
                                        ,XLG_MAC_CTRL5_PREAMBLE_LEN_TX_FLD_LEN_D  
                                        ,length);
            if (rc != GT_OK)
            {
                return rc;
            }
        break;
        default:
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChBobcat2PortXLGMacPreambleLengthGet
*
* DESCRIPTION:
*       XLG mac control register 5 get Preamble length
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       mac      - max number
*       direction - RX/TX/both
*
* OUTPUTS:
*       lengthPtrr   - pointer to preamble length
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - on NULL ptr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortXLGMacPreambleLengthGet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     mac,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_U32                  *lengthPtr
)
{
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(lengthPtr);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
            rc = prvCpssDxChBobcat2PortXLGMacCtrl5FldGet(devNum,mac
                                        ,XLG_MAC_CTRL5_PREAMBLE_LEN_RX_FLD_OFFS_D 
                                        ,XLG_MAC_CTRL5_PREAMBLE_LEN_RX_FLD_LEN_D  
                                        ,lengthPtr);
            if (rc != GT_OK)
            {
                return rc;
            }
        break;
        case CPSS_PORT_DIRECTION_TX_E:
            rc = prvCpssDxChBobcat2PortXLGMacCtrl5FldGet(devNum,mac
                                        ,XLG_MAC_CTRL5_PREAMBLE_LEN_TX_FLD_OFFS_D 
                                        ,XLG_MAC_CTRL5_PREAMBLE_LEN_TX_FLD_LEN_D  
                                        ,lengthPtr);
            if (rc != GT_OK)
            {
                return rc;
            }
        break;
        default:
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    if (*lengthPtr == 0) /* see comment at cider  swicthing code/GOP/XLG mac IP/units/Port Mac Control register 5 */
    {
        *lengthPtr = XLG_MAC_CTRL5_PREAMBLE_MAX_VAL_D;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChBobcat2PortXLGMacNumCRCByteshSet
*
* DESCRIPTION:
*       XLG mac control register 5 set number of CRC bytes
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       direction - RX/TX/both
*       crcBytesNum - number of CRC bytes
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortXLGMacNumCRCByteshSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_U32                   crcBytesNum
)
{
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    if (crcBytesNum < XLG_MAC_CTRL5_NUM_CRC_MIN_VAL_D  || crcBytesNum > XLG_MAC_CTRL5_NUM_CRC_MAX_VAL_D)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
            rc = prvCpssDxChBobcat2PortXLGMacCtrl5FldSet(devNum,portNum
                                        ,XLG_MAC_CTRL5_RX_NUM_CRC_BYTES_FLD_OFFS_D
                                        ,XLG_MAC_CTRL5_RX_NUM_CRC_BYTES_FLD_LEN_D 
                                        ,crcBytesNum);
            if (rc != GT_OK)
            {
                return rc;
            }
        break;
        case CPSS_PORT_DIRECTION_TX_E:
            rc = prvCpssDxChBobcat2PortXLGMacCtrl5FldSet(devNum,portNum
                                        ,XLG_MAC_CTRL5_TX_NUM_CRC_BYTES_FLD_OFFS_D 
                                        ,XLG_MAC_CTRL5_TX_NUM_CRC_BYTES_FLD_LEN_D  
                                        ,crcBytesNum);
            if (rc != GT_OK)
            {
                return rc;
            }
        break;
        case CPSS_PORT_DIRECTION_BOTH_E:
            rc = prvCpssDxChBobcat2PortXLGMacCtrl5FldSet(devNum,portNum
                                        ,XLG_MAC_CTRL5_RX_NUM_CRC_BYTES_FLD_OFFS_D
                                        ,XLG_MAC_CTRL5_RX_NUM_CRC_BYTES_FLD_LEN_D 
                                        ,crcBytesNum);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDxChBobcat2PortXLGMacCtrl5FldSet(devNum,portNum
                                        ,XLG_MAC_CTRL5_TX_NUM_CRC_BYTES_FLD_OFFS_D 
                                        ,XLG_MAC_CTRL5_TX_NUM_CRC_BYTES_FLD_LEN_D  
                                        ,crcBytesNum);
            if (rc != GT_OK)
            {
                return rc;
            }
        break;
        default:
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChBobcat2PortXLGMacNumCRCByteshGet
*
* DESCRIPTION:
*       XLG mac control register 5 get number of CRC bytes
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       mac      - max number
*       direction - RX/TX/both
*
* OUTPUTS:
*       crcBytesNumPtr - pointer to number of CRC bytes
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortXLGMacNumCRCByteshGet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     mac,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_U32                  *crcBytesNumPtr
)
{
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(crcBytesNumPtr);

    switch (direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
            rc = prvCpssDxChBobcat2PortXLGMacCtrl5FldGet(devNum,mac
                                        ,XLG_MAC_CTRL5_RX_NUM_CRC_BYTES_FLD_OFFS_D
                                        ,XLG_MAC_CTRL5_RX_NUM_CRC_BYTES_FLD_LEN_D 
                                        ,/*OUT*/crcBytesNumPtr);
            if (rc != GT_OK)
            {
                return rc;
            }
        break;
        case CPSS_PORT_DIRECTION_TX_E:
            rc = prvCpssDxChBobcat2PortXLGMacCtrl5FldGet(devNum,mac
                                        ,XLG_MAC_CTRL5_TX_NUM_CRC_BYTES_FLD_OFFS_D 
                                        ,XLG_MAC_CTRL5_TX_NUM_CRC_BYTES_FLD_LEN_D  
                                        ,/*OUT*/crcBytesNumPtr);
            if (rc != GT_OK)
            {
                return rc;
            }
        break;
        default:
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    return GT_OK;
}

