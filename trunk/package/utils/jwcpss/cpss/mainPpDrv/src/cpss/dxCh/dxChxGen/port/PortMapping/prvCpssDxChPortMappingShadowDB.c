/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortMappingShadowDB.c
*
* DESCRIPTION:
*       interface to port mapping shadow DB
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*******************************************************************************/
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortMapping.h>
#include <cpss/dxCh/dxChxGen/port/PortMapping/prvCpssDxChPortMappingShadowDB.h>


GT_STATUS prvCpssDxChPortPhysicalPortMapShadowDBClear
(
    IN GT_U8 devNum,
    IN GT_PHYSICAL_PORT_NUM physPortNum
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    if (physPortNum >= PRV_CPSS_MAX_PP_PORTS_NUM_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physPortNum].valid = GT_FALSE;
    PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physPortNum].portMap.trafficManagerEn = GT_FALSE;
    PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physPortNum].portMap.mappingType      = CPSS_DXCH_PORT_MAPPING_TYPE_INVALID_E;
    PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physPortNum].portMap.portGroup        = 0;
    PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physPortNum].portMap.macNum           = CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS;
    PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physPortNum].portMap.rxDmaNum         = CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS;
    PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physPortNum].portMap.txDmaNum         = CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS;
    PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physPortNum].portMap.txqNum           = CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS;
    PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physPortNum].portMap.ilknChannel      = CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS;
    PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physPortNum].portMap.tmPortIdx        = CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS;
    PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physPortNum].portMap.txFifoPortNum    = CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS;
    return GT_OK;
}


GT_STATUS prvCpssDxChPortPhysicalPortMapShadowDBSet
(
    IN GT_U8                   devNum,
    IN GT_PHYSICAL_PORT_NUM    physPortNum,
    IN CPSS_DXCH_PORT_MAP_STC *portMapPtr,
    IN GT_U32                  portArgArr[PRV_CPSS_DXCH_PORT_TYPE_MAX_E]

)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    if (physPortNum >= PRV_CPSS_MAX_PP_PORTS_NUM_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    prvCpssDxChPortPhysicalPortMapShadowDBClear(devNum,physPortNum);
    PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physPortNum].valid = GT_TRUE;
    PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physPortNum].portMap.trafficManagerEn = portMapPtr->tmEnable;
    PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physPortNum].portMap.mappingType      = portMapPtr->mappingType;
    PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physPortNum].portMap.portGroup        = portMapPtr->portGroup;
    PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physPortNum].portMap.macNum           = portArgArr[PRV_CPSS_DXCH_PORT_TYPE_MAC_E];
    PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physPortNum].portMap.rxDmaNum         = portArgArr[PRV_CPSS_DXCH_PORT_TYPE_RxDMA_E];
    PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physPortNum].portMap.txDmaNum         = portArgArr[PRV_CPSS_DXCH_PORT_TYPE_TxDMA_E];
    PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physPortNum].portMap.txFifoPortNum    = portArgArr[PRV_CPSS_DXCH_PORT_TYPE_TxDMA_E];
    PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physPortNum].portMap.txqNum           = portArgArr[PRV_CPSS_DXCH_PORT_TYPE_TXQ_E];
    PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physPortNum].portMap.ilknChannel      = portArgArr[PRV_CPSS_DXCH_PORT_TYPE_ILKN_CHANNEL_E];
    if (GT_TRUE == portMapPtr->tmEnable)
    {
        PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physPortNum].portMap.tmPortIdx        = portArgArr[PRV_CPSS_DXCH_PORT_TYPE_TM_PORT_IDX_E];
    }

    PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMac2PhyscalConvArr[portArgArr[PRV_CPSS_DXCH_PORT_TYPE_MAC_E]] = physPortNum;
    return GT_OK;
}

GT_STATUS prvCpssDxChPortPhysicalPortMapShadowDBGet
(
    IN   GT_U8                   devNum,
    IN   GT_PHYSICAL_PORT_NUM    physPortNum,
    OUT  CPSS_DXCH_DETAILED_PORT_MAP_STC **portMapShadowPtrPtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    if (physPortNum >= PRV_CPSS_MAX_PP_PORTS_NUM_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    CPSS_NULL_PTR_CHECK_MAC(portMapShadowPtrPtr);
    *portMapShadowPtrPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physPortNum];
    return GT_OK;

}

GT_STATUS prvCpssDxChPortPhysicalPortMapShadowDBInit
(
    IN GT_U8 devNum
)
{
    GT_PHYSICAL_PORT_NUM portNum;
    GT_U32               macNum;
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    for (portNum = 0; portNum < PRV_CPSS_MAX_PP_PORTS_NUM_CNS; portNum++)
    {
        prvCpssDxChPortPhysicalPortMapShadowDBClear(devNum,portNum);
    }

    /*-----------------------------------*
     * inverse MAC--> Physical port init *
     *-----------------------------------*/
    for (macNum = 0; macNum < PRV_CPSS_MAX_MAC_PORTS_NUM_CNS; macNum++)
    {
        PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMac2PhyscalConvArr[macNum] = CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS;
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortPhysicalPortMapCheckAndConvert
*
* DESCRIPTION:
*       Function checks and gets valid mapping from shadow DB.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum            - device number
*       physicalPortNum   - physical port to convert
*       convertedPortType - converted port type
*
* OUTPUTS:
*       convertedPortNumPtr  - (pointer to) converted port, according to port type
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, physicalPortNum
*       GT_BAD_PTR      - on bad pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortPhysicalPortMapCheckAndConvert
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            physicalPortNum,
    IN  PRV_CPSS_DXCH_PORT_TYPE_ENT     convertedPortType,
    OUT GT_U32                          *convertedPortNumPtr
)
{
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, physicalPortNum);

    /* check that valid mapping exists (except PHYS to PHYS check and convert) */
    if((convertedPortType != PRV_CPSS_DXCH_PORT_TYPE_PHYS_E) &&
       (PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physicalPortNum].valid != GT_TRUE))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, "physicalPortNum[%d] : convertedPortType[%d] not valid in shadow",
            physicalPortNum,convertedPortType);
    }

    switch(convertedPortType)
    {
        case PRV_CPSS_DXCH_PORT_TYPE_PHYS_E:
            *convertedPortNumPtr = physicalPortNum;
            break;

        case PRV_CPSS_DXCH_PORT_TYPE_RxDMA_E:
            *convertedPortNumPtr = PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physicalPortNum].portMap.rxDmaNum;
            break;

        case PRV_CPSS_DXCH_PORT_TYPE_TxDMA_E:
            *convertedPortNumPtr = PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physicalPortNum].portMap.txDmaNum;
            break;

        case PRV_CPSS_DXCH_PORT_TYPE_MAC_E:

            if(PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physicalPortNum].portMap.mappingType == CPSS_DXCH_PORT_MAPPING_TYPE_CPU_SDMA_E)
            {
                /* CPU port has no MAC */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, "physicalPortNum[%d] : is 'CPU SDMA' so hold no 'MAC'",
                    physicalPortNum);
            }

            *convertedPortNumPtr = PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physicalPortNum].portMap.macNum;
            break;

        case PRV_CPSS_DXCH_PORT_TYPE_TXQ_E:
            *convertedPortNumPtr = PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physicalPortNum].portMap.txqNum;
            break;

        case PRV_CPSS_DXCH_PORT_TYPE_ILKN_CHANNEL_E:
            *convertedPortNumPtr = PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[physicalPortNum].portMap.ilknChannel;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortPhysicalPortMapReverseMappingGet
*
* DESCRIPTION:
*       Function checks and returns physical port number that mapped to given
*       MAC/TXQ/DMA port number.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum            - device number
*       origPortType      - original port type
*       origPortNum       - MAC/TXQ/DMA port number
*
* OUTPUTS:
*       physicalPortNumPtr  - (pointer to) physical port number
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, origPortNum
*       GT_BAD_PTR      - on bad pointer
*       GT_NOT_FOUND    - not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortPhysicalPortMapReverseMappingGet
(
    IN  GT_U8                           devNum,
    IN  PRV_CPSS_DXCH_PORT_TYPE_ENT     origPortType,
    IN  GT_U32                          origPortNum,
    OUT GT_PHYSICAL_PORT_NUM            *physicalPortNumPtr
)
{
    GT_PHYSICAL_PORT_NUM portNum;
    GT_U32               currentPortValue;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(physicalPortNumPtr);

    if(origPortType == PRV_CPSS_DXCH_PORT_TYPE_PHYS_E)
    {
        *physicalPortNumPtr = origPortNum;
        return GT_OK;
    }
    /*----------------------------------------------------------------------------------------------*
     * for MAC --> physical port conversion there exists separate table, that allows direct access. *
     * so the search is excessive                                                                   *
     *----------------------------------------------------------------------------------------------*/
    if (origPortType == PRV_CPSS_DXCH_PORT_TYPE_MAC_E)
    {
        GT_U32 macNum = origPortNum;
        if (macNum >= PRV_CPSS_MAX_MAC_PORTS_NUM_CNS)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        *physicalPortNumPtr = PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMac2PhyscalConvArr[macNum];
        if (*physicalPortNumPtr == CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
        }
        return GT_OK;
    }

    for(portNum = 0; portNum < PRV_CPSS_MAX_PP_PORTS_NUM_CNS; portNum++)
    {
        if(PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[portNum].valid != GT_TRUE)
        {
            continue;
        }

        switch(origPortType)
        {
            case PRV_CPSS_DXCH_PORT_TYPE_RxDMA_E:
                currentPortValue = PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[portNum].portMap.rxDmaNum;
                break;

            case PRV_CPSS_DXCH_PORT_TYPE_TxDMA_E:
                currentPortValue = PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[portNum].portMap.txDmaNum;
                break;

         /*
          *  case PRV_CPSS_DXCH_PORT_TYPE_MAC_E:
          *      currentPortValue = PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[portNum].portMap.macNum;
          *      break;
          */

            case PRV_CPSS_DXCH_PORT_TYPE_TXQ_E:
                currentPortValue = PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[portNum].portMap.txqNum;
                break;

            case PRV_CPSS_DXCH_PORT_TYPE_ILKN_CHANNEL_E:
                currentPortValue = PRV_CPSS_DXCH_PP_MAC(devNum)->port.portsMapInfoShadowArr[portNum].portMap.ilknChannel;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if(currentPortValue == origPortNum)
        {
            *physicalPortNumPtr = portNum;
            return GT_OK;
        }
    }

    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
}


