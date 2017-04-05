/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChGEMacCtrl.c
*
* DESCRIPTION:
*       bobcat2 GE mac control 
*
* FILE REVISION NUMBER:
*       $Revision: 4$
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/PortMapping/prvCpssDxChPortMappingShadowDB.h>
#include <cpss/dxCh/dxChxGen/port/macCtrl/prvCpssDxChGEMacCtrl.h>
#include <cpss/dxCh/dxChxGen/port/macCtrl/prvCpssDxChMacCtrl.h>

/*---------------------------------------------------------------------------------------------
 * /Cider/EBU/Bobcat2/Bobcat2 {Current}/Switching Core/GOP/<Gige MAC IP> Gige MAC IP Units%g/Tri-Speed Port MAC Configuration/Port MAC Control Register1
 * 15 - 15  Short Tx preable    0x0 -- 8 bytes
 *                              0x1 -- 4 bytes
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
* prvCpssDxChBobcat2PortMacGigaIsSupported
*
* DESCRIPTION:
*       check whether GE mac is supported for specific mac
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*
* OUTPUTS:
*       isSupportedPtr - pointer to is supported
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - on bad ptr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortMacGigaIsSupported
(
    IN  GT_U8                    devNum,
    IN  GT_PHYSICAL_PORT_NUM     mac,
    OUT GT_BOOL                 *isSupportedPtr
)
{
    GT_U32    regAddr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PORT_MAC_CTRL3_REG_MAC(devNum, mac, PRV_CPSS_PORT_GE_E, &regAddr);

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
* prvCpssDxChBobcat2PortGigaMacIPGLengthSet
*
* DESCRIPTION:
*       GE mac set IPG length
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum      - physical port number
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
GT_STATUS prvCpssDxChBobcat2PortGigaMacIPGLengthSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_U32                   length
)
{
    GT_STATUS rc;
    GT_U32    regAddr;
    GT_PHYSICAL_PORT_NUM    portMacNum;
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    if (length > GIGA_MAC_CTRL3_IPG_MAX_LEN_D)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChMacByPhysPortGet(devNum, portNum, &portMacNum);
    if (rc != GT_OK)
    {
        return rc;
    }
    PRV_CPSS_DXCH_PORT_MAC_CTRL3_REG_MAC(devNum, portMacNum, PRV_CPSS_PORT_GE_E, &regAddr);

    if(regAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    regDataArray[PRV_CPSS_PORT_GE_E].regAddr = regAddr;
    regDataArray[PRV_CPSS_PORT_GE_E].fieldData = length;
    regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = GIGA_MAC_CTRL3_IPG_LEN_FLD_LEN_D;
    regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = GIGA_MAC_CTRL3_IPG_LEN_FLD_OFFS_D;
    
    rc = prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);

    return rc;
}

/*******************************************************************************
* prvCpssDxChBobcat2PortGigaMacIPGLengthGet
*
* DESCRIPTION:
*       GE mac set IPG length
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       mac      - physical port number
*
* OUTPUTS:
*       lengthPtr   - pointer to ipg length 
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortGigaMacIPGLengthGet
(
    IN  GT_U8                    devNum,
    IN  GT_PHYSICAL_PORT_NUM     mac,
    OUT GT_U32                  *lengthPtr
)
{
    GT_STATUS rc;
    GT_U32    regAddr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);
    CPSS_NULL_PTR_CHECK_MAC(lengthPtr);

    *lengthPtr = 0;

    PRV_CPSS_DXCH_PORT_MAC_CTRL3_REG_MAC(devNum, mac, PRV_CPSS_PORT_GE_E, &regAddr);

    if(regAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    rc = prvCpssHwPpPortGroupGetRegField(devNum, 0, regAddr
                                            ,GIGA_MAC_CTRL3_IPG_LEN_FLD_OFFS_D  
                                            ,GIGA_MAC_CTRL3_IPG_LEN_FLD_LEN_D  
                                            ,lengthPtr);
    if (rc != GT_OK)
    {
        *lengthPtr = 0;
        return rc;
    }
    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChBobcat2PortGigaMacPreambleLengthSet
*
* DESCRIPTION:
*       GE mac set preamble length
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum      - physical port number
*       direction - RX/TX/BOTH
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
GT_STATUS prvCpssDxChBobcat2PortGigaMacPreambleLengthSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_U32                   length
)
{
    GT_STATUS rc;
    GT_U32    regAddr;
    GT_U32    fldVal;
    GT_PHYSICAL_PORT_NUM    portMacNum;
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    if (length != GIGA_MAC_CTRL1_PREAMBLE_LEN_4_D && length != GIGA_MAC_CTRL1_PREAMBLE_LEN_8_D)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if (direction < CPSS_PORT_DIRECTION_RX_E || direction > CPSS_PORT_DIRECTION_BOTH_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if (direction != CPSS_PORT_DIRECTION_TX_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChMacByPhysPortGet(devNum, portNum, &portMacNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum, portMacNum, PRV_CPSS_PORT_GE_E, &regAddr);

    if(regAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if (length == GIGA_MAC_CTRL1_PREAMBLE_LEN_4_D)
    {
        fldVal = 1;
    }
    else
    {
        fldVal = 0;
    }

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    regDataArray[PRV_CPSS_PORT_GE_E].regAddr = regAddr;
    regDataArray[PRV_CPSS_PORT_GE_E].fieldData = fldVal;
    regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = GIGA_MAC_CTRL1_PREAMBLE_LEN_FLD_OFFS_D;
    regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = GIGA_MAC_CTRL1_PREAMBLE_LEN_FLD_OFFS_D;
    
    rc = prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);

    return rc;
}

/*******************************************************************************
* prvCpssDxChBobcat2PortGigaMacPreambleLengthGet
*
* DESCRIPTION:
*       GE mac set preamble length
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       mac      - physical port number
*       direction - RX/TX/BOTH
*
* OUTPUTS:
*       lengthPtr  - pointer to preamble length
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong devNum, portNum
*       GT_BAD_PTR      - on NULL ptr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortGigaMacPreambleLengthGet
(
    IN  GT_U8                    devNum,
    IN  GT_PHYSICAL_PORT_NUM     mac,
    IN  CPSS_PORT_DIRECTION_ENT  direction,
    OUT GT_U32                   *lengthPtr
)
{
    GT_STATUS rc;
    GT_U32    regAddr;
    GT_U32    fldVal;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);
    CPSS_NULL_PTR_CHECK_MAC(lengthPtr);

    if (direction != CPSS_PORT_DIRECTION_TX_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum, mac, PRV_CPSS_PORT_GE_E, &regAddr);

    if(regAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    rc = prvCpssHwPpPortGroupGetRegField(devNum, 0, regAddr
                                            ,GIGA_MAC_CTRL1_PREAMBLE_LEN_FLD_OFFS_D  
                                            ,GIGA_MAC_CTRL1_PREAMBLE_LEN_FLD_LEN_D  
                                            ,/*OUT*/&fldVal);
    if (rc != GT_OK)
    {
        return rc;
    }
    if (fldVal == 1)
    {
        *lengthPtr = GIGA_MAC_CTRL1_PREAMBLE_LEN_4_D;
    }
    else
    {
        *lengthPtr = GIGA_MAC_CTRL1_PREAMBLE_LEN_8_D;
    }
    return GT_OK;
}



GT_STATUS prvCpssDxChBobcat2PortGigaMacTypeSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN PRV_CPSS_DXCH_MAC_GE_PORT_TYPE_ENT portType
)
{
    GT_STATUS rc;
    GT_U32    regAddr;
    GT_U32    fldVal;
    GT_PHYSICAL_PORT_NUM    portMacNum;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    rc = prvCpssDxChMacByPhysPortGet(devNum, portNum, &portMacNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum, portMacNum, PRV_CPSS_PORT_GE_E, &regAddr);

    if(regAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (portType)
    {
        case  PRV_CPSS_DXCH_MAC_GE_PORT_TYPE_SGMII_E:         fldVal = 0; break;
        case  PRV_CPSS_DXCH_MAC_GE_PORT_TYPE_1000BaseX_E:     fldVal = 1; break;
        default:
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }


    rc = prvCpssHwPpPortGroupSetRegField(devNum, 0, regAddr
                                            ,GIGA_MAC_CTRL0_PORT_TYPE_FLD_OFFS_D  
                                            ,GIGA_MAC_CTRL0_PORT_TYPE_FLD_LEN_D  
                                            ,fldVal);
    if (rc != GT_OK)
    {
        return rc;
    }

    return rc;

}


static GT_STATUS prvCpssDxChBobcat2PortGigaMacTypeByMacGet
(
    IN  GT_U8                    devNum,
    IN  GT_U32                   portMacNum,
    OUT PRV_CPSS_DXCH_MAC_GE_PORT_TYPE_ENT *portTypePtr
)
{
    GT_STATUS rc;
    GT_U32    regAddr;
    GT_U32    fldVal;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);
    CPSS_NULL_PTR_CHECK_MAC(portTypePtr);


    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum, portMacNum, PRV_CPSS_PORT_GE_E, &regAddr);

    if(regAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    rc = prvCpssHwPpPortGroupGetRegField(devNum, 0, regAddr
                                            ,GIGA_MAC_CTRL0_PORT_TYPE_FLD_OFFS_D  
                                            ,GIGA_MAC_CTRL0_PORT_TYPE_FLD_LEN_D  
                                            ,/*OUT*/&fldVal);
    if (rc != GT_OK)
    {
        return rc;
    }
    if (fldVal == 0)
    {
        *portTypePtr = PRV_CPSS_DXCH_MAC_GE_PORT_TYPE_SGMII_E;
    }
    else
    {
        *portTypePtr = PRV_CPSS_DXCH_MAC_GE_PORT_TYPE_1000BaseX_E;
    }
    return GT_OK;
}


GT_STATUS prvCpssDxChBobcat2PortGigaMacTypeGet
(
    IN  GT_U8                    devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT PRV_CPSS_DXCH_MAC_GE_PORT_TYPE_ENT *portTypePtr
)
{
    GT_STATUS rc;
    GT_PHYSICAL_PORT_NUM    portMacNum;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);
    CPSS_NULL_PTR_CHECK_MAC(portTypePtr);


    rc = prvCpssDxChMacByPhysPortGet(devNum, portNum, &portMacNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChBobcat2PortGigaMacTypeByMacGet(devNum,portMacNum,/*OUT*/portTypePtr);

    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------
 *   /Cider/EBU/BobK/BobK {Current}/Switching Core/GOP/Units/GOP/<Gige MAC IP> Gige MAC IP Units%g/SGMII Configuration/Port SERDES Configuration Register0
 * 11 - 11   Auto-Negotiation Master Mode Enable     Enable setting Auto-Negotiation code word bit 15 according to the Port SERDES Configuration Register2.
                0x0 = Disabled; Disabled 
                0x1 = Enabled; Enabled 
 *------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#define PRV_CPSS_DXCH_AN_MASTER_MODE_EN_OFFS_CNS           11
#define PRV_CPSS_DXCH_AN_MASTER_MODE_EN_LEN_CNS            1
/*------------------------------------------------------------------------------------------------------------------------------------------------------------
 *   /Cider/EBU/BobK/BobK {Current}/Switching Core/GOP/Units/GOP/<Gige MAC IP> Gige MAC IP Units%g/SGMII Configuration/Port SERDES Configuration Register3
 *------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#define PRV_CPSS_DXCH_AN_LINK_PARTNER_CODEWORD_OFFS_CNS     0
#define PRV_CPSS_DXCH_AN_LINK_PARTNER_CODEWORD_LEN_CNS     16


GT_STATUS prvCpssDxChBobcat2PortGigaMacAnMasterModeSet
(
    IN  GT_U8  devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL  mode
)
{
    GT_STATUS rc;
    GT_U32    regAddr;
    GT_U32    fldVal;
    GT_PHYSICAL_PORT_NUM    portMacNum;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    rc = prvCpssDxChMacByPhysPortGet(devNum, portNum, &portMacNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portMacNum].serdesCnfg;

    if(regAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    fldVal = mode;
    rc = prvCpssHwPpPortGroupSetRegField(devNum, 0, regAddr
                                            ,PRV_CPSS_DXCH_AN_MASTER_MODE_EN_OFFS_CNS 
                                            ,PRV_CPSS_DXCH_AN_MASTER_MODE_EN_LEN_CNS 
                                            ,fldVal);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}


GT_STATUS prvCpssDxChBobcat2PortGigaMacAnMasterModeGet
(
    IN  GT_U8  devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL *modePtr
)
{
    GT_STATUS rc;
    GT_U32    regAddr;
    GT_U32    fldVal;
    GT_PHYSICAL_PORT_NUM    portMacNum;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    rc = prvCpssDxChMacByPhysPortGet(devNum, portNum, &portMacNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portMacNum].serdesCnfg;

    if(regAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    rc = prvCpssHwPpPortGroupGetRegField(devNum, 0, regAddr
                                            ,PRV_CPSS_DXCH_AN_MASTER_MODE_EN_OFFS_CNS 
                                            ,PRV_CPSS_DXCH_AN_MASTER_MODE_EN_LEN_CNS 
                                            ,/*OUT*/&fldVal);
    if (rc != GT_OK)
    {
        return rc;
    }

    *modePtr = (GT_BOOL)fldVal;

    return GT_OK;
}



GT_STATUS prvCpssDxChPortAutoNegLinkPartnerCodeWordNyMacGet
(
    IN  GT_U8  devNum,
    IN  GT_U32 portMacNum,
    OUT GT_U32 *codewordPtr
)
{
    GT_STATUS rc;
    GT_U32    regAddr;
    GT_U32    fldVal;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);
    CPSS_NULL_PTR_CHECK_MAC(codewordPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portMacNum].serdesCnfg3;

    if(regAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    rc = prvCpssHwPpPortGroupGetRegField(devNum, 0, regAddr
                                            ,PRV_CPSS_DXCH_AN_LINK_PARTNER_CODEWORD_OFFS_CNS  
                                            ,PRV_CPSS_DXCH_AN_LINK_PARTNER_CODEWORD_LEN_CNS  
                                            ,/*OUT*/&fldVal);
    if (rc != GT_OK)
    {
        return rc;
    }

    *codewordPtr = ~fldVal;
    *codewordPtr &= 0xFFFF;

    return GT_OK;
}


GT_STATUS prvCpssDxChPortAutoNegLinkPartnerCodeWordGet
(
    IN  GT_U8  devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_U32 *codewordPtr
)
{
    GT_STATUS rc;
    GT_U32    portMacNum;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);
    CPSS_NULL_PTR_CHECK_MAC(codewordPtr);


    rc = prvCpssDxChMacByPhysPortGet(devNum, portNum, &portMacNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChPortAutoNegLinkPartnerCodeWordNyMacGet(devNum,portMacNum,/*OUT*/codewordPtr);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}


/* 
    /Cider/EBU/BobK/BobK {Current}/Switching Core/GOP/Units/GOP/<Gige MAC IP> Gige MAC IP Units%g/Port Auto-Negotiation Configuration/Port Auto-Negotiation Configuration
    15  - 15    TxConfigRegToEncodeTable 
    14  - 14    AutoMedia SelectEn   
    13  - 13    AnDuplexEn    
    12  - 12    SetFullDuplex  0
    11  - 11    AnFcEn         0 - AnFc Disable
                               1 - AnFc Enable
    10  - 10    PauseAsmAdv    0 - Symmetric Flow Control  
                               1 - Assimetric
     9  -  9    PauseAdv       0 - No Flow Control
                               1 - Flow Control
     7  -  7    AnSpeedEn      0 - Disable Update
                               1 - Enable Update
     6  -  6    SetGMIISpeed    1000_Mbps.
     5  -  5    SetMIISpeed    100 Mbps Speed
     4  -  4    InBand ReStartAn    0
     3  -  3    InBandAn  Bypass
     2  -  2    InBandAn  Enabled
     1  -  1    ForceLinkUp    Force Link UP
     0  -  0    ForceLinkDown    Dont_Force Link
*/

static GT_STATUS prvCpssDxChPortAutoNegConfigByMacGet
(
    IN GT_U8  devNum,
    IN GT_U32 portMacNum,
    OUT PRV_CPSS_DXCH_PORT_AN_CONFIG_RES_STC *anConfigPtr
)
{
    GT_STATUS rc;
    GT_U32    regAddr;
    GT_U32    fldVal;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(anConfigPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portMacNum].autoNegCtrl;
    if(regAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssHwPpPortGroupGetRegField(devNum, 0, regAddr
                                            ,0
                                            ,16
                                            ,/*OUT*/&fldVal);
    if (rc != GT_OK)
    {
        return rc;
    }
    anConfigPtr->ForceLinkDown     = (GT_BOOL) ((fldVal >> PRV_CPSS_DXCH_AN_CONF_FORCE_LINK_DOWN_OFFS_CNS     ) & 0x1);
    anConfigPtr->ForceLinkUp       = (GT_BOOL) ((fldVal >> PRV_CPSS_DXCH_AN_CONF_FORCE_LINK_UP_OFFS_CNS       ) & 0x1);
    anConfigPtr->InBandAnEn        = (GT_BOOL) ((fldVal >> PRV_CPSS_DXCH_AN_CONF_INBAND_AN_EN_OFFS_NS         ) & 0x1);
    anConfigPtr->InBandAnByPassEn  = (GT_BOOL) ((fldVal >> PRV_CPSS_DXCH_AN_CONF_INBAND_AN_BYPASS_EN_OFFS_CNS ) & 0x1);
    anConfigPtr->SetMIISpeed       = (GT_BOOL) ((fldVal >> PRV_CPSS_DXCH_AN_CONF_GMII_SPEED_OFFS_CNS          ) & 0x1);
    anConfigPtr->SetGMIISpeed      = (GT_BOOL) ((fldVal >> PRV_CPSS_DXCH_AN_CONF_MII_SPEED_OFFS_CNS           ) & 0x1);
    anConfigPtr->AnSpeedEn         = (GT_BOOL) ((fldVal >> PRV_CPSS_DXCH_AN_CONF_AN_SPEED_EN_OFFS_CNS         ) & 0x1);
    anConfigPtr->PauseAdv          = (GT_BOOL) ((fldVal >> PRV_CPSS_DXCH_AN_CONF_PAUSE_ADV_OFFS_CNS           ) & 0x1);
    anConfigPtr->PauseAsmAdv       = (GT_BOOL) ((fldVal >> PRV_CPSS_DXCH_AN_CONF_PAUSE_ASM_ADV_OFFS_CNS       ) & 0x1);
    anConfigPtr->AnFcEn            = (GT_BOOL) ((fldVal >> PRV_CPSS_DXCH_AN_CONF_AN_FLOW_CTRL_EN_OFFS_CNS     ) & 0x1);
    anConfigPtr->SetFullDuplex     = (GT_BOOL) ((fldVal >> PRV_CPSS_DXCH_AN_CONF_FULL_DUPLEX_MODE_OFFS_CNS    ) & 0x1);
    anConfigPtr->AnDuplexEn        = (GT_BOOL) ((fldVal >> PRV_CPSS_DXCH_AN_CONF_AN_FULL_DUPLEX_EN_OFFS_CNS   ) & 0x1);

    return GT_OK;
}



GT_STATUS prvCpssDxChPortAutoNegConfigGet
(
    IN GT_U8  devNum,
    IN GT_PHYSICAL_PORT_NUM physPortNum,
    OUT PRV_CPSS_DXCH_PORT_AN_CONFIG_RES_STC *anConfigPtr
)
{
    GT_STATUS rc;
    GT_PHYSICAL_PORT_NUM    portMacNum;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(anConfigPtr);

    rc = prvCpssDxChMacByPhysPortGet(devNum, physPortNum, &portMacNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChPortAutoNegConfigByMacGet(devNum,portMacNum,/*OUT*/anConfigPtr);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}



GT_STATUS prvCpssDxChPortAutoNegPauseAdvPauseAsmAdvGet
(
    IN GT_U8  devNum,
    IN GT_PHYSICAL_PORT_NUM physPortNum,
    OUT GT_BOOL *pauseAdvPtr,
    OUT GT_BOOL *pauseAsmAdvPtr
)
{
    GT_STATUS rc;
    GT_U32    regAddr;
    GT_U32    fldVal;
    GT_PHYSICAL_PORT_NUM    portMacNum;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(pauseAdvPtr);
    CPSS_NULL_PTR_CHECK_MAC(pauseAsmAdvPtr);

    rc = prvCpssDxChMacByPhysPortGet(devNum, physPortNum, &portMacNum);
    if (rc != GT_OK)
    {
        return rc;
    }
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portMacNum].autoNegCtrl;
    if(regAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssHwPpPortGroupGetRegField(devNum, 0, regAddr
                                            ,PRV_CPSS_DXCH_AN_CONF_PAUSE_ADV_OFFS_CNS  
                                            ,1
                                            ,/*OUT*/&fldVal);
    if (rc != GT_OK)
    {
        return rc;
    }
    *pauseAdvPtr = (GT_BOOL) fldVal;
    rc = prvCpssHwPpPortGroupGetRegField(devNum, 0, regAddr
                                            ,PRV_CPSS_DXCH_AN_CONF_PAUSE_ASM_ADV_OFFS_CNS  
                                            ,1
                                            ,/*OUT*/&fldVal);
    if (rc != GT_OK)
    {
        return rc;
    }
    *pauseAsmAdvPtr = (GT_BOOL) fldVal;
    return GT_OK;
}


#define PRV_CPSS_DXCH_LINK_SYNC_STATUS_SYNC_OK_OFFS_CNS               14
#define PRV_CPSS_DXCH_LINK_SYNC_STATUS_INBAND_AN_BYPASS_ACT_OFFS_CNS  12
#define PRV_CPSS_DXCH_LINK_SYNC_STATUS_SYNC_FAIL_10ms_OFFS_CNS        10

static GT_STATUS prvCpssDxChPortAutoNegBypassActByMacGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   portMacNum,
    OUT GT_BOOL *inBandAnPypassActPtr
)
{
    GT_STATUS rc;
    GT_U32    regAddr;
    GT_U32    fldVal;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(inBandAnPypassActPtr);


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portMacNum].macStatus;
    if(regAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, 0, regAddr,/*OUT*/&fldVal);
    if (rc != GT_OK)
    {
        return rc;
    }
    *inBandAnPypassActPtr = (GT_BOOL) ((fldVal >> PRV_CPSS_DXCH_LINK_SYNC_STATUS_INBAND_AN_BYPASS_ACT_OFFS_CNS)        & 0x1);

    return GT_OK;
}


GT_STATUS prvCpssDxChPortAutoNegBypassActGet
(
    IN  GT_U8                 devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_BOOL              *inBandAnPypassActPtr
)
{
    GT_STATUS rc;
    GT_PHYSICAL_PORT_NUM    portMacNum;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(inBandAnPypassActPtr);


    rc = prvCpssDxChMacByPhysPortGet(devNum, portNum, /*OUT*/&portMacNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChPortAutoNegBypassActByMacGet(devNum,portMacNum,inBandAnPypassActPtr);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}


GT_STATUS prvCpssDxChPortAutoNegSyncChangeStateByMacGet
(
    IN  GT_U8                  devNum,
    IN  GT_PHYSICAL_PORT_NUM   portMacNum,
    OUT GT_BOOL               *portSyncOKPtr,
    OUT GT_BOOL               *SyncFail10MsPtr
)
{
    GT_STATUS rc;
    GT_U32    regAddr;
    GT_U32    fldVal;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(portSyncOKPtr);
    CPSS_NULL_PTR_CHECK_MAC(portSyncOKPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portMacNum].macStatus;
    if(regAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, 0, regAddr,/*OUT*/&fldVal);
    if (rc != GT_OK)
    {
        return rc;
    }
    *portSyncOKPtr   = (GT_BOOL) ((fldVal >> PRV_CPSS_DXCH_LINK_SYNC_STATUS_SYNC_OK_OFFS_CNS)        & 0x1);
    *SyncFail10MsPtr = (GT_BOOL) ((fldVal >> PRV_CPSS_DXCH_LINK_SYNC_STATUS_SYNC_FAIL_10ms_OFFS_CNS) & 0x1);

    return GT_OK;
}


GT_STATUS prvCpssDxChPortAutoNegSyncChangeStateGet
(
    IN  GT_U8 devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL * portSyncOKPtr,
    OUT GT_BOOL * syncFail10MsPtr
)
{
    GT_STATUS rc;
    GT_PHYSICAL_PORT_NUM    portMacNum;

    rc = prvCpssDxChMacByPhysPortGet(devNum, portNum, /*OUT*/&portMacNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChPortAutoNegSyncChangeStateByMacGet(devNum,portMacNum,/*OUT*/portSyncOKPtr,syncFail10MsPtr);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}

