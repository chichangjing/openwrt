/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChMacCtrl.c
*
* DESCRIPTION:
*       bobcat2 mac control 
*
* FILE REVISION NUMBER:
*       $Revision: 5$
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/PortMapping/prvCpssDxChPortMappingShadowDB.h>
#include <cpss/dxCh/dxChxGen/port/macCtrl/prvCpssDxChMacCtrl.h>
#include <cpss/dxCh/dxChxGen/port/macCtrl/prvCpssDxChGEMacCtrl.h>
#include <cpss/dxCh/dxChxGen/port/macCtrl/prvCpssDxChXLGMacCtrl.h>
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
* prvCpssDxChMacByPhysPortGet
*
* DESCRIPTION:
*       get mac by port number 
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
*
* OUTPUTS:
*       macPtr   - pointer to is supported
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - on bad ptr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChMacByPhysPortGet
(
    IN  GT_U8                    devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_U32                  *macPtr
)
{
    GT_STATUS rc;
    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);
    PRV_CPSS_DXCH_ENHANCED_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(macPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_PORT_MAP_CHECK_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum,portNum,/*OUT*/&portMapShadowPtr);
        if (rc != GT_OK)
        {
            return rc;
        }

        if (portMapShadowPtr->valid == GT_FALSE)
        {
            rc = GT_BAD_PARAM;
            goto error;
        }
        if (portMapShadowPtr->portMap.macNum >= PRV_CPSS_DXCH_BOBCAT2_MAX_MAC_D) /* valid mac ? */  
        {
            rc = GT_BAD_PARAM;
            goto error;
        }
        *macPtr     = portMapShadowPtr->portMap.macNum;
    }
    else
    {
        *macPtr = portNum; 
    }

    return GT_OK;
error:
    *macPtr    = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
    return rc;
}

/*******************************************************************************
* prvCpssDxChBobcat2PortMacIsSupported
*
* DESCRIPTION:
*       check whether XLG mac is supported for specific port
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
*       portType - port type (mac)
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
GT_STATUS prvCpssDxChBobcat2PortMacIsSupported
(
    IN GT_U8                             devNum,
    IN GT_PHYSICAL_PORT_NUM              portNum,
    IN PRV_CPSS_PORT_TYPE_ENT            portType,
    OUT GT_BOOL                         *isSupportedPtr
)
{
    GT_STATUS rc;
    GT_U32    mac;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E 
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);
    CPSS_NULL_PTR_CHECK_MAC(isSupportedPtr);

    rc = prvCpssDxChMacByPhysPortGet(devNum,portNum,/*OUT*/&mac);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch (portType)
    {
        case PRV_CPSS_PORT_GE_E:
            rc = prvCpssDxChBobcat2PortMacGigaIsSupported(devNum,mac,/*OUT*/isSupportedPtr);
        break;
        case PRV_CPSS_PORT_XLG_E:
            rc = prvCpssDxChBobcat2PortMacXLGIsSupported(devNum,mac,/*OUT*/isSupportedPtr);
        break;
        default:
        {
            *isSupportedPtr = GT_FALSE;
            rc = GT_OK;
        }
    }
    return rc;
}

/*******************************************************************************
* prvCpssDxChBobcat2PortMacPreambleLengthSet
*
* DESCRIPTION:
*       set XLG mac Preable length
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
*       direction - direction (RX/TX/both)
*       length   = ipg length in bytes
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
GT_STATUS prvCpssDxChBobcat2PortMacPreambleLengthSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_U32                   length
)
{
    GT_STATUS rc;
    GT_STATUS rcGiga;
    GT_STATUS rcXLG;
    GT_U32    mac;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    rc = prvCpssDxChMacByPhysPortGet(devNum,portNum,/*OUT*/&mac);
    if (rc != GT_OK)
    {
        return rc;
    }

    rcXLG = prvCpssDxChBobcat2PortGigaMacPreambleLengthSet(devNum,portNum,direction,length);
    rcGiga = prvCpssDxChBobcat2PortXLGMacPreambleLengthSet(devNum,portNum,direction,length);

    if (rcXLG != GT_OK && rcGiga != GT_OK)
    {
        return rcXLG;
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChBobcat2PortMacPreambleLengthGet
*
* DESCRIPTION:
*       get XLG mac Preable length
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
*       portType - port type
*       direction - direction (RX/TX/both)
*
* OUTPUTS:
*       lengthPtr - preable ipg length in bytes
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortMacPreambleLengthGet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN PRV_CPSS_PORT_TYPE_ENT   portType,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_U32                  *lengthPtr
)
{
    GT_STATUS rc;
    GT_U32    mac;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);
    CPSS_NULL_PTR_CHECK_MAC(lengthPtr);
    *lengthPtr = 0;
    rc = prvCpssDxChMacByPhysPortGet(devNum,portNum,/*OUT*/&mac);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch (portType)
    {
        case  PRV_CPSS_PORT_GE_E:
            rc = prvCpssDxChBobcat2PortGigaMacPreambleLengthGet(devNum,mac,direction,/*OUT*/lengthPtr);
            if (rc != GT_OK)
            {
                return rc;
            }
        break;
        case  PRV_CPSS_PORT_XLG_E:
            rc = prvCpssDxChBobcat2PortXLGMacPreambleLengthGet(devNum,mac,direction,/*OUT*/lengthPtr);
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
* prvCpssDxChBobcat2PortMacIPGLengthSet
*
* DESCRIPTION:
*       set XLG mac IPG length
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
*       length   = ipg length in bytes
*
* OUTPUTS:
*       isSupportedPtr.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortMacIPGLengthSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_U32                   length
)
{
    GT_STATUS rc;
    GT_STATUS rcGiga;
    GT_STATUS rcXLG;
    GT_U32    mac;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    rc = prvCpssDxChMacByPhysPortGet(devNum,portNum,/*OUT*/&mac);
    if (rc != GT_OK)
    {
        return rc;
    }

    rcGiga = prvCpssDxChBobcat2PortGigaMacIPGLengthSet(devNum,portNum,length);
    rcXLG  = prvCpssDxChBobcat2PortXLGMacIPGLengthSet (devNum,portNum,length);
    if (rcXLG != GT_OK && rcGiga != GT_OK)
    {
        return rcXLG;
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChBobcat2PortMacIPGLengthGet
*
* DESCRIPTION:
*       get XLG mac IPG length
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
*       portType - port type
*
* OUTPUTS:
*       lengthPtr   = pointer to length in bytes
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - on bad ptr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortMacIPGLengthGet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN PRV_CPSS_PORT_TYPE_ENT   portType,
    IN GT_U32                  *lengthPtr
)
{
    GT_STATUS rc;
    GT_U32    mac;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);
    CPSS_NULL_PTR_CHECK_MAC(lengthPtr);
    *lengthPtr = 0;
    rc = prvCpssDxChMacByPhysPortGet(devNum,portNum,/*OUT*/&mac);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch (portType)
    {
        case  PRV_CPSS_PORT_GE_E:
            rc = prvCpssDxChBobcat2PortGigaMacIPGLengthGet(devNum,mac,/*OUT*/lengthPtr);
            if (rc != GT_OK)
            {
                return rc;
            }
        break;
        case  PRV_CPSS_PORT_XLG_E:
            rc = prvCpssDxChBobcat2PortXLGMacIPGLengthGet(devNum,mac,/*OUT*/lengthPtr);
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
* prvCpssDxChBobcat2PortMacCrcModeSet
*
* DESCRIPTION:
*       Set XLG mac CRC number of bytes
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
*       direction - direction (RX/TX/both)
*       numCrcBytes - number of bytes
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
GT_STATUS prvCpssDxChBobcat2PortMacCrcModeSet
(
    IN GT_U8                             devNum,
    IN GT_PHYSICAL_PORT_NUM              portNum,
    IN CPSS_PORT_DIRECTION_ENT           portDirection,
    IN GT_U32                            numCrcBytes
)
{
    GT_STATUS rc;
    /* GT_STATUS rcGigMac = GT_BAD_PARAM; */
    GT_STATUS rcXLGMac = GT_BAD_PARAM;
    GT_BOOL isXLGMacSupported;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    rc = prvCpssDxChBobcat2PortMacIsSupported(devNum,portNum,PRV_CPSS_PORT_XLG_E,/*OUT*/&isXLGMacSupported);
    if (rc != GT_OK)
    {
        return rc;
    }
    if (isXLGMacSupported == GT_TRUE)
    {
        rcXLGMac = prvCpssDxChBobcat2PortXLGMacNumCRCByteshSet(devNum,portNum,portDirection,numCrcBytes);
        if (rcXLGMac != GT_OK)
        {
            return rcXLGMac;
        }
    }
    else /* GE mac, there no option to set CRC, by definition 4 is used */
    {
        if (numCrcBytes != 4)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    return GT_OK;

}

/*******************************************************************************
* prvCpssDxChBobcat2PortMacCrcModeGet
*
* DESCRIPTION:
*       Get XLG mac CRC number of bytes
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
*       direction - direction (RX/TX/both)
*
* OUTPUTS:
*       numCrcBytesPtr - number of bytes
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - on NULL ptr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortMacCrcModeGet
(
    IN GT_U8                             devNum,
    IN GT_PHYSICAL_PORT_NUM              portNum,
    IN CPSS_PORT_DIRECTION_ENT           portDirection,
    IN GT_U32                           *numCrcBytesPtr
)
{
    GT_STATUS rc;
    GT_STATUS rcXLGMac = GT_BAD_PARAM;
    GT_U32    mac;
    GT_BOOL isXLGMacSupported;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    rc = prvCpssDxChMacByPhysPortGet(devNum,portNum,/*OUT*/&mac);
    if (rc != GT_OK)
    {
        return rc;
    }
    rc = prvCpssDxChBobcat2PortMacIsSupported(devNum,portNum,PRV_CPSS_PORT_XLG_E,/*OUT*/&isXLGMacSupported);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (isXLGMacSupported == GT_TRUE)
    {
        rcXLGMac = prvCpssDxChBobcat2PortXLGMacNumCRCByteshGet(devNum,mac,portDirection,/*OUT*/numCrcBytesPtr);
        if (rcXLGMac != GT_OK)
        {
            return rcXLGMac;
        }
    }
    else /* GE mac, there no option to get CRC, by definition 4 is used */
    {
        *numCrcBytesPtr = 4;
    }
    return GT_OK;
}
