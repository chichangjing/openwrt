/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
*
* cpssDxChBrgSrcId.c
*
* DESCRIPTION:
*       CPSS DxCh Source ID facility implementation
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 28 $
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/bridge/private/prvCpssDxChBrgLog.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/private/prvCpssCommonDefs.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgSrcId.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>


/*******************************************************************************
* prvCpssDxChBrgSrcIdGroupTableSet
*
* DESCRIPTION:
*        Set a bit per port indicating if the packet with this Source-ID may be
*        forwarded to this port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       sourceId - Source ID number that the port is added to
*       portNum  - Physical port number, CPU port
*       action   - GT_FALSE  - the port is not a member of this Source ID group
*                              and packets assigned with this Source ID are not
*                              forwarded to this port.
*                - GT_TRUE  - the portis a member of this Source ID group and
*                             packets assigned with this Source ID may be
*                             forwarded to this port.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - wrong devNum, portNum, sourceId
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChBrgSrcIdGroupTableSet
(
    IN GT_U8                    devNum,
    IN GT_U32                   sourceId,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_BOOL                  action
)
{
    GT_U32 data;        /* Data to be written into the register */
    GT_U32 regAddr;     /* register address */
    GT_U32 fieldOffset; /* The start bit number in the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(sourceId > PRV_CPSS_DXCH_PP_HW_INFO_LAST_INDEX_IN_TABLE_SRC_ID_MAC(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM,
            "sourceId[%d] is out of range[0..%d]",
            sourceId,
            PRV_CPSS_DXCH_PP_HW_INFO_LAST_INDEX_IN_TABLE_SRC_ID_MAC(devNum));
    }

    data = (action == GT_TRUE) ? 1 : 0;

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        fieldOffset = portNum;

        if(portNum == CPSS_CPU_PORT_NUM_CNS)
        {
            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
            {
                fieldOffset = 27; /* CPU port is bit 27 in Cheetah */
            }
            else
            {
                fieldOffset = 31; /* CPU port is bit 31 in Cheetah2 */
            }
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                              bufferMng.srcIdEggFltrTbl[sourceId];
        return prvCpssHwPpSetRegField(devNum, regAddr, fieldOffset, 1, data);
    }
    else
    {
        /* call the generic function that handle port in tables of BMP of ports in the EGF_SHT */
        return prvCpssDxChHwEgfShtPortsBmpTableEntrySetPort(devNum,
                    PRV_CPSS_DXCH_LION_TABLE_TXQ_SOURCE_ID_MEMBERS_E,
                    sourceId,portNum,action);
    }


}

/*******************************************************************************
* internal_cpssDxChBrgSrcIdGroupPortAdd
*
* DESCRIPTION:
*         Add a port to Source ID group. Packets assigned with this Source ID
*         may be forwarded to this port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       sourceId - Source ID  number that the port is added to
*       portNum  - Physical port number, CPU port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum, sourceId
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSrcIdGroupPortAdd
(
    IN GT_U8                    devNum,
    IN GT_U32                   sourceId,
    IN GT_PHYSICAL_PORT_NUM     portNum
)
{
    return prvCpssDxChBrgSrcIdGroupTableSet(devNum,sourceId,portNum,GT_TRUE);
}

/*******************************************************************************
* cpssDxChBrgSrcIdGroupPortAdd
*
* DESCRIPTION:
*         Add a port to Source ID group. Packets assigned with this Source ID
*         may be forwarded to this port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       sourceId - Source ID  number that the port is added to
*       portNum  - Physical port number, CPU port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum, sourceId
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdGroupPortAdd
(
    IN GT_U8                    devNum,
    IN GT_U32                   sourceId,
    IN GT_PHYSICAL_PORT_NUM     portNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSrcIdGroupPortAdd);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, sourceId, portNum));

    rc = internal_cpssDxChBrgSrcIdGroupPortAdd(devNum, sourceId, portNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, sourceId, portNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgSrcIdGroupPortDelete
*
* DESCRIPTION:
*         Delete a port from Source ID group. Packets assigned with this
*         Source ID will not be forwarded to this port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       sourceId - Source ID  number that the port is deleted from
*       portNum  - Physical port number, CPU port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum, sourceId
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSrcIdGroupPortDelete
(
    IN GT_U8                    devNum,
    IN GT_U32                   sourceId,
    IN GT_PHYSICAL_PORT_NUM     portNum
)
{
    return prvCpssDxChBrgSrcIdGroupTableSet(devNum,sourceId,portNum,GT_FALSE);
}

/*******************************************************************************
* cpssDxChBrgSrcIdGroupPortDelete
*
* DESCRIPTION:
*         Delete a port from Source ID group. Packets assigned with this
*         Source ID will not be forwarded to this port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       sourceId - Source ID  number that the port is deleted from
*       portNum  - Physical port number, CPU port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum, sourceId
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdGroupPortDelete
(
    IN GT_U8                    devNum,
    IN GT_U32                   sourceId,
    IN GT_PHYSICAL_PORT_NUM     portNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSrcIdGroupPortDelete);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, sourceId, portNum));

    rc = internal_cpssDxChBrgSrcIdGroupPortDelete(devNum, sourceId, portNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, sourceId, portNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* portGroupSrcIdGroupEntrySet
*
* DESCRIPTION:
*        Set entry in Source ID Egress Filtering table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupId          - the port group Id . to support multi-port-groups device
*       sourceId        - Source ID  number
*       maxPortNum      - max number of bits for the ports
*       cpuPortBit      - the bit for the CPU port
*       cpuSrcIdMember  - GT_TRUE - CPU is member of of Src ID group.
*                         GT_FALSE - CPU isn't member of of Src ID group.
*       portsMembersPtr - pointer to bitmap of ports that are
*                         Source ID Members of specified PP device.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, sourceId, ports bitmap value
*       GT_BAD_PTR               - portsMembersPtr is NULL pointer
*       GT_OUT_OF_RANGE          - length of portsMembersPtr is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS portGroupSrcIdGroupEntrySet
(
    IN GT_U8               devNum,
    IN GT_U32              portGroupId,
    IN GT_U32              sourceId,
    IN GT_U32              maxPortNum,
    IN GT_U32              cpuPortBit,
    IN GT_BOOL             cpuSrcIdMember,
    IN CPSS_PORTS_BMP_STC  *portsMembersPtr
)
{
    GT_U32 regAddr;           /* register address */
    GT_U32 data;              /* data to be written into the register */
    GT_U32 cpuPortBitValue;   /* CPU port bit value*/
    GT_STATUS rc;             /* return value */

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                              bufferMng.srcIdEggFltrTbl[sourceId];


        rc = prvCpssHwPpPortGroupReadRegister(devNum,portGroupId, regAddr, &data);
        if(rc != GT_OK)
            return rc;

        /* set port bitmap */
        U32_SET_FIELD_MAC(data,0,maxPortNum,portsMembersPtr->ports[0]);

        cpuPortBitValue = (cpuSrcIdMember == GT_TRUE) ? 1 : 0;

        /* set CPU bit */
        U32_SET_FIELD_MAC(data,cpuPortBit,1,cpuPortBitValue);

        /* Write bitmap of ports are belonged to the Source ID */
        return prvCpssHwPpPortGroupWriteRegister(devNum,portGroupId, regAddr, data);
    }
    else
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            /* check that the device supports all the physical ports that defined in the BMP */
            rc = prvCpssDxChTablePortsBmpCheck(devNum , portsMembersPtr);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        /* call the generic function that handle BMP of ports in the EGF_SHT */
        rc = prvCpssDxChHwEgfShtPortsBmpTableEntryWrite(devNum,
                PRV_CPSS_DXCH_LION_TABLE_TXQ_SOURCE_ID_MEMBERS_E,
                sourceId,
                portsMembersPtr,
                GT_TRUE,
                cpuSrcIdMember);

        return rc;

    }
}

/*******************************************************************************
* internal_cpssDxChBrgSrcIdGroupEntrySet
*
* DESCRIPTION:
*        Set entry in Source ID Egress Filtering table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       sourceId       - Source ID  number
*       cpuSrcIdMember - GT_TRUE - CPU is member of of Src ID group.
*                        GT_FALSE - CPU isn't member of of Src ID group.
*       portsMembersPtr - pointer to the bitmap of ports that are
*                        Source ID Members of specified PP device.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, sourceId, ports bitmap value
*       GT_BAD_PTR               - portsMembersPtr is NULL pointer
*       GT_OUT_OF_RANGE          - length of portsMembersPtr is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSrcIdGroupEntrySet
(
    IN GT_U8               devNum,
    IN GT_U32              sourceId,
    IN GT_BOOL             cpuSrcIdMember,
    IN CPSS_PORTS_BMP_STC  *portsMembersPtr
)
{
    GT_U32 cpuPortBit;        /* CPU port bit */
    GT_U32 maxPortNum;        /* max port number in the Source ID group */
                              /* DxCh2 - 28; DxCh - 27 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(portsMembersPtr);

    if(sourceId > PRV_CPSS_DXCH_PP_HW_INFO_LAST_INDEX_IN_TABLE_SRC_ID_MAC(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM,
            "sourceId[%d] is out of range[0..%d]",
            sourceId,
            PRV_CPSS_DXCH_PP_HW_INFO_LAST_INDEX_IN_TABLE_SRC_ID_MAC(devNum));
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
        {
            maxPortNum = 27;
            cpuPortBit = 27;

        }
        else
        {
            maxPortNum = 28;
            cpuPortBit = 31;

        }

        if (portsMembersPtr->ports[0] >= ((GT_U32)(1 << maxPortNum)))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        maxPortNum = 0;/* don't care */
        cpuPortBit = 0;/* don't care */
    }

    return portGroupSrcIdGroupEntrySet(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,sourceId,
            maxPortNum,cpuPortBit,cpuSrcIdMember,portsMembersPtr);
}

/*******************************************************************************
* cpssDxChBrgSrcIdGroupEntrySet
*
* DESCRIPTION:
*        Set entry in Source ID Egress Filtering table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       sourceId       - Source ID  number
*       cpuSrcIdMember - GT_TRUE - CPU is member of of Src ID group.
*                        GT_FALSE - CPU isn't member of of Src ID group.
*       portsMembersPtr - pointer to the bitmap of ports that are
*                        Source ID Members of specified PP device.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, sourceId, ports bitmap value
*       GT_BAD_PTR               - portsMembersPtr is NULL pointer
*       GT_OUT_OF_RANGE          - length of portsMembersPtr is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdGroupEntrySet
(
    IN GT_U8               devNum,
    IN GT_U32              sourceId,
    IN GT_BOOL             cpuSrcIdMember,
    IN CPSS_PORTS_BMP_STC  *portsMembersPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSrcIdGroupEntrySet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, sourceId, cpuSrcIdMember, portsMembersPtr));

    rc = internal_cpssDxChBrgSrcIdGroupEntrySet(devNum, sourceId, cpuSrcIdMember, portsMembersPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, sourceId, cpuSrcIdMember, portsMembersPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgSrcIdGroupEntryGet
*
* DESCRIPTION:
*        Get entry in Source ID Egress Filtering table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       sourceId          - Source ID number
*
* OUTPUTS:
*       cpuSrcIdMemberPtr - GT_TRUE - CPU is member of of Src ID group.
*                           GT_FALSE - CPU isn't member of of Src ID group.
*       portsMembersPtr   - pointer to the bitmap of ports are Source ID Members
*                           of specified PP device.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, sourceId
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSrcIdGroupEntryGet
(
    IN  GT_U8               devNum,
    IN  GT_U32              sourceId,
    OUT GT_BOOL             *cpuSrcIdMemberPtr,
    OUT CPSS_PORTS_BMP_STC  *portsMembersPtr
)
{
    GT_U32    regAddr;      /* register address */
    GT_U32    data;         /* data to be read from the register */
    GT_U32    cpuPortBit;   /* CPU port bit */
    GT_U32    maxPortNum;   /* max port number in the Source ID group */
                            /* DxCh2 - 28; DxCh - 27 */
    GT_STATUS rc;           /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(cpuSrcIdMemberPtr);
    CPSS_NULL_PTR_CHECK_MAC(portsMembersPtr);

    if(sourceId > PRV_CPSS_DXCH_PP_HW_INFO_LAST_INDEX_IN_TABLE_SRC_ID_MAC(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM,
            "sourceId[%d] is out of range[0..%d]",
            sourceId,
            PRV_CPSS_DXCH_PP_HW_INFO_LAST_INDEX_IN_TABLE_SRC_ID_MAC(devNum));
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        cpssOsMemSet(portsMembersPtr, 0, sizeof(CPSS_PORTS_BMP_STC));

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                              bufferMng.srcIdEggFltrTbl[sourceId];

        rc =  prvCpssHwPpReadRegister(devNum,regAddr,&data);
        if(rc != GT_OK)
            return rc;

        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
        {
            maxPortNum = 27;
            cpuPortBit = 27;

        }
        else
        {
            maxPortNum = 28;
            cpuPortBit = 31;
        }

        /* set port bitmap excluding CPU bit */
        U32_SET_FIELD_MASKED_MAC(portsMembersPtr->ports[0],0,maxPortNum,data);

        if(((data >> cpuPortBit) & 0x1) == 1)
        {
            *cpuSrcIdMemberPtr = GT_TRUE;
        }
        else
        {
            *cpuSrcIdMemberPtr = GT_FALSE;
        }

        return GT_OK;
    }
    else
    {
        /* call the generic function that handle BMP of ports in the EGF_SHT */
        rc = prvCpssDxChHwEgfShtPortsBmpTableEntryRead(devNum,
                PRV_CPSS_DXCH_LION_TABLE_TXQ_SOURCE_ID_MEMBERS_E,
                sourceId,
                portsMembersPtr,
                GT_TRUE,
                cpuSrcIdMemberPtr);

        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            if(rc != GT_OK)
            {
                return rc;
            }
            /* adjusted physical ports of BMP to hold only bmp of existing ports*/
            rc = prvCpssDxChTablePortsBmpAdjustToDevice(devNum , portsMembersPtr , portsMembersPtr);
        }

        return rc;
    }
}

/*******************************************************************************
* cpssDxChBrgSrcIdGroupEntryGet
*
* DESCRIPTION:
*        Get entry in Source ID Egress Filtering table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       sourceId          - Source ID number
*
* OUTPUTS:
*       cpuSrcIdMemberPtr - GT_TRUE - CPU is member of of Src ID group.
*                           GT_FALSE - CPU isn't member of of Src ID group.
*       portsMembersPtr   - pointer to the bitmap of ports are Source ID Members
*                           of specified PP device.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, sourceId
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdGroupEntryGet
(
    IN  GT_U8               devNum,
    IN  GT_U32              sourceId,
    OUT GT_BOOL             *cpuSrcIdMemberPtr,
    OUT CPSS_PORTS_BMP_STC  *portsMembersPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSrcIdGroupEntryGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, sourceId, cpuSrcIdMemberPtr, portsMembersPtr));

    rc = internal_cpssDxChBrgSrcIdGroupEntryGet(devNum, sourceId, cpuSrcIdMemberPtr, portsMembersPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, sourceId, cpuSrcIdMemberPtr, portsMembersPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgSrcIdPortDefaultSrcIdSet
*
* DESCRIPTION:
*       Configure Port's Default Source ID.
*       The Source ID is used for source based egress filtering.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number, CPU port
*       defaultSrcId    - Port's Default Source ID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum, defaultSrcId
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSrcIdPortDefaultSrcIdSet
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    IN GT_U32       defaultSrcId
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32      regAddr;  /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    if(defaultSrcId > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_SRC_ID_MAC(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM,
            "defaultSrcId[%d] is out of range[0..%d]",
            defaultSrcId,
            PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_SRC_ID_MAC(devNum));
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* set the <SRC_ID>*/
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_EPORT_TABLE_FIELDS_SRC_ID_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        defaultSrcId);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        regAddr = PRV_CPSS_DEV_BRG_PORT_CONFIG_0_REGADDR_PTR(devNum,portNum);

        rc = prvCpssHwPpPortGroupSetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                regAddr, 6, 5, defaultSrcId);
    }
    return rc;
}

/*******************************************************************************
* cpssDxChBrgSrcIdPortDefaultSrcIdSet
*
* DESCRIPTION:
*       Configure Port's Default Source ID.
*       The Source ID is used for source based egress filtering.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number, CPU port
*       defaultSrcId    - Port's Default Source ID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum, defaultSrcId
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdPortDefaultSrcIdSet
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    IN GT_U32       defaultSrcId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSrcIdPortDefaultSrcIdSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, defaultSrcId));

    rc = internal_cpssDxChBrgSrcIdPortDefaultSrcIdSet(devNum, portNum, defaultSrcId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, defaultSrcId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgSrcIdPortDefaultSrcIdGet
*
* DESCRIPTION:
*       Get configuration of Port's Default Source ID.
*       The Source ID is used for Source based egress filtering.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       portNum           - port number, CPU port
*
* OUTPUTS:
*       defaultSrcIdPtr   - Port's Default Source ID
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSrcIdPortDefaultSrcIdGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *defaultSrcIdPtr
)
{

    GT_STATUS   rc = GT_OK;
    GT_U32      regAddr;  /* register address */
    GT_U32      value;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(defaultSrcIdPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* get the <SRC_ID>*/
        rc = prvCpssDxChReadTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_EPORT_TABLE_FIELDS_SRC_ID_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &value);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        regAddr = PRV_CPSS_DEV_BRG_PORT_CONFIG_0_REGADDR_PTR(devNum,portNum);

        rc = prvCpssHwPpPortGroupGetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                                                regAddr, 6, 5, &value);
    }

    if(rc != GT_OK)
        return rc;

    *defaultSrcIdPtr = value;

    return rc;
}

/*******************************************************************************
* cpssDxChBrgSrcIdPortDefaultSrcIdGet
*
* DESCRIPTION:
*       Get configuration of Port's Default Source ID.
*       The Source ID is used for Source based egress filtering.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       portNum           - port number, CPU port
*
* OUTPUTS:
*       defaultSrcIdPtr   - Port's Default Source ID
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdPortDefaultSrcIdGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *defaultSrcIdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSrcIdPortDefaultSrcIdGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, defaultSrcIdPtr));

    rc = internal_cpssDxChBrgSrcIdPortDefaultSrcIdGet(devNum, portNum, defaultSrcIdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, defaultSrcIdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChBrgSrcIdGlobalUcastEgressFilterSet
*
* DESCRIPTION:
*     Enable or disable Source ID egress filter for unicast
*     packets. The Source ID filter is configured by
*     cpssDxChBrgSrcIdGroupPortAdd and cpssDxChBrgSrcIdGroupPortDelete.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - device number
*       enable   - GT_TRUE -  enable Source ID filtering on unicast packets.
*                             Unicast packet is dropped if egress port is not
*                             member in the Source ID group.
*                - GT_FALSE - disable Source ID filtering on unicast packets.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSrcIdGlobalUcastEgressFilterSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32  regAddr;      /* register address */
    GT_U32  data;         /* reg subfield data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_LION_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    data = (enable == GT_TRUE) ? 1 : 0;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfigExt;

    return prvCpssHwPpSetRegField(devNum, regAddr, 29, 1, data);
}

/*******************************************************************************
* cpssDxChBrgSrcIdGlobalUcastEgressFilterSet
*
* DESCRIPTION:
*     Enable or disable Source ID egress filter for unicast
*     packets. The Source ID filter is configured by
*     cpssDxChBrgSrcIdGroupPortAdd and cpssDxChBrgSrcIdGroupPortDelete.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - device number
*       enable   - GT_TRUE -  enable Source ID filtering on unicast packets.
*                             Unicast packet is dropped if egress port is not
*                             member in the Source ID group.
*                - GT_FALSE - disable Source ID filtering on unicast packets.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdGlobalUcastEgressFilterSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSrcIdGlobalUcastEgressFilterSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChBrgSrcIdGlobalUcastEgressFilterSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgSrcIdGlobalUcastEgressFilterGet
*
* DESCRIPTION:
*     Get Source ID egress filter configuration for unicast
*     packets. The Source ID filter is configured by
*     cpssDxChBrgSrcIdGroupPortAdd and cpssDxChBrgSrcIdGroupPortDelete.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       enablePtr   - GT_TRUE -  enable Source ID filtering on unicast packets.
*                             Unicast packet is dropped if egress port is not
*                             member in the Source ID group.
*                   - GT_FALSE - disable Source ID filtering on unicast packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSrcIdGlobalUcastEgressFilterGet
(
    IN GT_U8     devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32    regAddr;      /* register address */
    GT_U32    data;         /* reg subfield data */
    GT_STATUS rc;           /* return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_LION_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfigExt;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 29, 1, &data);
    if(rc != GT_OK)
        return rc;

    *enablePtr = (data == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgSrcIdGlobalUcastEgressFilterGet
*
* DESCRIPTION:
*     Get Source ID egress filter configuration for unicast
*     packets. The Source ID filter is configured by
*     cpssDxChBrgSrcIdGroupPortAdd and cpssDxChBrgSrcIdGroupPortDelete.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       enablePtr   - GT_TRUE -  enable Source ID filtering on unicast packets.
*                             Unicast packet is dropped if egress port is not
*                             member in the Source ID group.
*                   - GT_FALSE - disable Source ID filtering on unicast packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdGlobalUcastEgressFilterGet
(
    IN GT_U8     devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSrcIdGlobalUcastEgressFilterGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChBrgSrcIdGlobalUcastEgressFilterGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChBrgSrcIdGlobalSrcIdAssignModeSet
*
* DESCRIPTION:
*         Set Source ID Assignment mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       mode      - the assignment mode of the packet Source ID.
*                   CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_DA_PORT_DEFAULT_E mode is
*                   supported for xCat and above.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSrcIdGlobalSrcIdAssignModeSet
(
    IN GT_U8                            devNum,
    IN CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT  mode
)
{
    GT_STATUS   rc;
    GT_U32 regAddr;     /* register address */
    GT_U32 data;        /* reg subfield data */
    GT_U32 fieldOffset; /* field offset */
    GT_U32 fieldLength; /* field length */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH2_E | CPSS_CH3_E);

    switch(mode)
    {
        /* XCAT A1 and above:
            0 = Disable: FDB source ID assignment is disabled
            1 = DA based: FDB source ID assignment is enabled for DA-based assignment
            2 = SA based: FDB source ID assignment is enabled for SA-based assignment
        */
        /* DxCh1
            0 = Assign non-DSA tagged packets with the Source-ID configured in the
                source address FDB entry.
            1 = Assign non-DSA tagged packets with the Source-ID according to the
                port default configuration (ignoring the source FDB entry Source-ID
                assignment)
        */
        case CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E:
            data = (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E) ? 2 : 0;
            break;
        case CPSS_BRG_SRC_ID_ASSIGN_MODE_PORT_DEFAULT_E:
            data = (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E) ? 0 : 1;
            break;
        case CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_DA_PORT_DEFAULT_E:
            if (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E)
            {
                data = 1;
            }
            else
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* need to set the same value in FDB and in L2I units ! */
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).
                    FDBCore.FDBGlobalConfig.FDBGlobalConfig1;
        fieldLength = 2;
        fieldOffset = 17;

        rc = prvCpssHwPpSetRegField(devNum, regAddr, fieldOffset, fieldLength, data);
        if(rc != GT_OK)
        {
            return rc;
        }

        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    bridgeEngineConfig.bridgeGlobalConfig1;
        fieldLength = 2;
        fieldOffset = 4;
    }
    else if (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        /* FDBbasedSrcID Assign Mode [14:13] */
        fieldLength = 2;
        fieldOffset = 13;
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                         bridgeRegs.bridgeGlobalConfigRegArray[2];
    }
    else
    {
        /* UsePortDefault-SrcId [4] */
        fieldLength = 1;
        fieldOffset = 4;
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                         bridgeRegs.bridgeGlobalConfigRegArray[0];
    }

    return prvCpssHwPpSetRegField(devNum, regAddr, fieldOffset, fieldLength, data);
}

/*******************************************************************************
* cpssDxChBrgSrcIdGlobalSrcIdAssignModeSet
*
* DESCRIPTION:
*         Set Source ID Assignment mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       mode      - the assignment mode of the packet Source ID.
*                   CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_DA_PORT_DEFAULT_E mode is
*                   supported for xCat and above.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdGlobalSrcIdAssignModeSet
(
    IN GT_U8                            devNum,
    IN CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT  mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSrcIdGlobalSrcIdAssignModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mode));

    rc = internal_cpssDxChBrgSrcIdGlobalSrcIdAssignModeSet(devNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgSrcIdGlobalSrcIdAssignModeGet
*
* DESCRIPTION:
*         Get Source ID Assignment mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*
* OUTPUTS:
*       modePtr      - the assignment mode of the packet Source ID
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSrcIdGlobalSrcIdAssignModeGet
(
    IN  GT_U8                            devNum,
    OUT CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT  *modePtr
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 data;        /* reg subfield data */
    GT_STATUS rc;       /* return value */
    GT_U32 fieldOffset; /* field offset */
    GT_U32 fieldLength; /* field length */
    GT_U32 data1=0;     /* reg subfield data1 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* need to set the same value in FDB and in L2I units ! */
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).
                    FDBCore.FDBGlobalConfig.FDBGlobalConfig1;
        fieldLength = 2;
        fieldOffset = 17;
        rc =  prvCpssHwPpGetRegField(devNum, regAddr, fieldOffset, fieldLength, &data1);
        if(rc != GT_OK)
            return rc;

        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    bridgeEngineConfig.bridgeGlobalConfig1;
        fieldLength = 2;
        fieldOffset = 4;
    }
    else if (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        /* FDBbasedSrcID Assign Mode [14:13] */
        fieldLength = 2;
        fieldOffset = 13;
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                         bridgeRegs.bridgeGlobalConfigRegArray[2];
    }
    else
    {
        /* UsePortDefault-SrcId [4] */
        fieldLength = 1;
        fieldOffset = 4;
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                         bridgeRegs.bridgeGlobalConfigRegArray[0];
    }

    rc =  prvCpssHwPpGetRegField(devNum, regAddr, fieldOffset, fieldLength, &data);
    if(rc != GT_OK)
        return rc;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* need to set the same value in FDB and in L2I units ! */
        if(data != data1)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
    }

    switch(data)
    {
        case 0:
            *modePtr = (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E)
                        ? CPSS_BRG_SRC_ID_ASSIGN_MODE_PORT_DEFAULT_E
                        : CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E;
            break;
        case 1:
            *modePtr = (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E)
                        ? CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_DA_PORT_DEFAULT_E
                        : CPSS_BRG_SRC_ID_ASSIGN_MODE_PORT_DEFAULT_E;
            break;
        default:
            *modePtr = CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E;
            break;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgSrcIdGlobalSrcIdAssignModeGet
*
* DESCRIPTION:
*         Get Source ID Assignment mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*
* OUTPUTS:
*       modePtr      - the assignment mode of the packet Source ID
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdGlobalSrcIdAssignModeGet
(
    IN  GT_U8                            devNum,
    OUT CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT  *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSrcIdGlobalSrcIdAssignModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, modePtr));

    rc = internal_cpssDxChBrgSrcIdGlobalSrcIdAssignModeGet(devNum, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChBrgSrcIdPortSrcIdAssignModeSet
*
* DESCRIPTION:
*         Set Source ID Assignment mode for non-DSA tagged packets
*         per Ingress Port.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - device number
*       portNum   - Physical port number, CPU port
*       mode      - the assignment mode of the packet Source ID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum, mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSrcIdPortSrcIdAssignModeSet
(
    IN GT_U8                            devNum,
    IN GT_PORT_NUM                      portNum,
    IN CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT  mode
)
{

    GT_U32 regAddr;     /* register address */
    GT_U32 regData;     /* register data */
    GT_U32 data;        /* reg subfield data */
    GT_U32 fieldOffset; /* The start bit number in the register */
    GT_STATUS rc;       /* return value */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32   localPort;/* local port - support multi-port-groups device */

    /* DxCh2, DxCh3 are supported */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                   bridgeRegs.srcIdReg.srcIdAssignedModeConfReg;

    if(localPort == CPSS_CPU_PORT_NUM_CNS)
        fieldOffset = 31; /* CPU port is bit 31 */
    else
        fieldOffset = localPort;

    switch(mode)
    {
       case CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E:
            data = 0;
            break;
       case CPSS_BRG_SRC_ID_ASSIGN_MODE_PORT_DEFAULT_E:
            data = 1;
            break;
       default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    /* FEr#1084 -  Wrong data when reading Source-ID Assignment Mode Conf. reg. */
    if(PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
          PRV_CPSS_DXCH2_READ_SRC_ID_ASSIGMENT_MODE_CONF_REG_WA_E) == GT_TRUE)
    {
        /* NOTE: this WA not relevant to the Lion - multi-port-groups device ,
            otherwise DB of srcIdAssignedModeConfRegData should have been per port group */
        regData =  (PRV_CPSS_DXCH_PP_MAC(devNum))->
                                            errata.srcIdAssignedModeConfRegData;
        U32_SET_FIELD_MAC(regData,fieldOffset,1,data);

        rc = prvCpssHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regData);
        if(rc != GT_OK)
            return rc;

        (PRV_CPSS_DXCH_PP_MAC(devNum))->errata.srcIdAssignedModeConfRegData =
                                                                        regData;
        return GT_OK;

    }
    else
    {
        return prvCpssHwPpPortGroupSetRegField(devNum,portGroupId, regAddr, fieldOffset, 1, data);

    }

}

/*******************************************************************************
* cpssDxChBrgSrcIdPortSrcIdAssignModeSet
*
* DESCRIPTION:
*         Set Source ID Assignment mode for non-DSA tagged packets
*         per Ingress Port.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - device number
*       portNum   - Physical port number, CPU port
*       mode      - the assignment mode of the packet Source ID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum, mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdPortSrcIdAssignModeSet
(
    IN GT_U8                            devNum,
    IN GT_PORT_NUM                      portNum,
    IN CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT  mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSrcIdPortSrcIdAssignModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, mode));

    rc = internal_cpssDxChBrgSrcIdPortSrcIdAssignModeSet(devNum, portNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgSrcIdPortSrcIdAssignModeGet
*
* DESCRIPTION:
*         Get Source ID Assignment mode for non-DSA tagged packets
*         per Ingress Port.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - device number
*       portNum   - Physical port number, CPU port
*
* OUTPUTS:
*       modePtr   - the assignment mode of the packet Source ID
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSrcIdPortSrcIdAssignModeGet
(
    IN GT_U8                            devNum,
    IN GT_PORT_NUM                      portNum,
    OUT CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT  *modePtr
)
{

    GT_U32 regAddr;     /* register address */
    GT_U32 data;        /* reg subfield data */
    GT_U32 fieldOffset; /* The start bit number in the register */
    GT_STATUS rc;       /* return value */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32   localPort;/* local port - support multi-port-groups device */

    /* DxCh2, DxCh3 are supported */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                   bridgeRegs.srcIdReg.srcIdAssignedModeConfReg;

    if(localPort == CPSS_CPU_PORT_NUM_CNS)
        fieldOffset = 31; /* CPU port is bit 31 */
    else
        fieldOffset = localPort;

    /* FEr#1084 -  Wrong data when reading Source-ID Assignment Mode Conf. reg. */
    if(PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
          PRV_CPSS_DXCH2_READ_SRC_ID_ASSIGMENT_MODE_CONF_REG_WA_E) == GT_TRUE)
    {
        /* NOTE: this WA not relevant to the Lion - multi-port-groups device ,
            otherwise DB of srcIdAssignedModeConfRegData should have been per port group */

        data =  (PRV_CPSS_DXCH_PP_MAC(devNum))->
                                            errata.srcIdAssignedModeConfRegData;

        data = (data >> fieldOffset) & 1;
    }
    else
    {
        rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, fieldOffset, 1, &data);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    switch(data)
    {
       case 0:
            *modePtr = CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E;
            break;
       case 1:
            *modePtr = CPSS_BRG_SRC_ID_ASSIGN_MODE_PORT_DEFAULT_E;
            break;
       default:
            break;
    }

    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgSrcIdPortSrcIdAssignModeGet
*
* DESCRIPTION:
*         Get Source ID Assignment mode for non-DSA tagged packets
*         per Ingress Port.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - device number
*       portNum   - Physical port number, CPU port
*
* OUTPUTS:
*       modePtr   - the assignment mode of the packet Source ID
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdPortSrcIdAssignModeGet
(
    IN GT_U8                            devNum,
    IN GT_PORT_NUM                      portNum,
    OUT CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT  *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSrcIdPortSrcIdAssignModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, modePtr));

    rc = internal_cpssDxChBrgSrcIdPortSrcIdAssignModeGet(devNum, portNum, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgSrcIdPortUcastEgressFilterSet
*
* DESCRIPTION:
*     Per Egress Port enable or disable Source ID egress filter for unicast
*     packets. The Source ID filter is configured by
*     cpssDxChBrgSrcIdGroupPortAdd and cpssDxChBrgSrcIdGroupPortDelete.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*       portNum  - Physical port number/all ports wild card, CPU port
*       enable   - GT_TRUE - enable Source ID filtering on unicast packets
*                            forwarded to this port. Unicast packet is dropped
*                            if egress port is not member in the Source ID group.
*                - GT_FALSE - disable Source ID filtering on unicast packets
*                             forwarded to this port.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSrcIdPortUcastEgressFilterSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_BOOL                  enable
)
{
    GT_U32  regAddr;      /* register address */
    GT_U32  data;         /* reg subfield data */
    GT_U32  fieldOffset;  /* The start bit number in the register */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    data = (enable == GT_TRUE) ? 1 : 0;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        return prvCpssDxChHwEgfShtFieldSet(devNum,portNum,
            PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_UC_SRC_ID_FILTER_E,
                GT_TRUE, /*accessPhysicalPort*/
                GT_FALSE, /*accessEPort*/
                data);
    }

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* CPU port is bit 31 */
        fieldOffset = (localPort == CPSS_CPU_PORT_NUM_CNS) ? 31 : localPort;

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                     bridgeRegs.srcIdReg.srcIdUcEgressFilterConfReg;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.egr.filterConfig.ucSrcIdFilterEn[OFFSET_TO_WORD_MAC(portNum)];
        fieldOffset = OFFSET_TO_BIT_MAC(portNum);
    }

    return prvCpssHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, fieldOffset, 1, data);
}

/*******************************************************************************
* cpssDxChBrgSrcIdPortUcastEgressFilterSet
*
* DESCRIPTION:
*     Per Egress Port enable or disable Source ID egress filter for unicast
*     packets. The Source ID filter is configured by
*     cpssDxChBrgSrcIdGroupPortAdd and cpssDxChBrgSrcIdGroupPortDelete.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*       portNum  - Physical port number/all ports wild card, CPU port
*       enable   - GT_TRUE - enable Source ID filtering on unicast packets
*                            forwarded to this port. Unicast packet is dropped
*                            if egress port is not member in the Source ID group.
*                - GT_FALSE - disable Source ID filtering on unicast packets
*                             forwarded to this port.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdPortUcastEgressFilterSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_BOOL                  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSrcIdPortUcastEgressFilterSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChBrgSrcIdPortUcastEgressFilterSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgSrcIdPortUcastEgressFilterGet
*
* DESCRIPTION:
*     Get Per Egress Port Source ID egress filter configuration for unicast
*     packets. The Source ID filter is configured by
*     cpssDxChBrgSrcIdGroupPortAdd and cpssDxChBrgSrcIdGroupPortDelete.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*       portNum  - Physical port number/all ports wild card, CPU port
*
* OUTPUTS:
*       enablePtr   - GT_TRUE - enable Source ID filtering on unicast packets
*                            forwarded to this port. Unicast packet is dropped
*                            if egress port is not member in the Source ID group.
*                   - GT_FALSE - disable Source ID filtering on unicast packets
*                             forwarded to this port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSrcIdPortUcastEgressFilterGet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_U32  regAddr;      /* register address */
    GT_U32  data;         /* reg subfield data */
    GT_U32  fieldOffset;  /* The start bit number in the register */
    GT_STATUS rc;         /* return value */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        rc = prvCpssDxChHwEgfShtFieldGet(devNum,portNum,
            PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_UC_SRC_ID_FILTER_E,
                GT_TRUE, /*accessPhysicalPort*/
                &data);

        *enablePtr = (data == 1) ? GT_TRUE : GT_FALSE;

        return rc;
    }

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* CPU port is bit 31 */
        fieldOffset = (localPort == CPSS_CPU_PORT_NUM_CNS) ? 31 : localPort;

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                     bridgeRegs.srcIdReg.srcIdUcEgressFilterConfReg;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.egr.filterConfig.ucSrcIdFilterEn[OFFSET_TO_WORD_MAC(portNum)];
        fieldOffset = OFFSET_TO_BIT_MAC(portNum);
    }


    rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, fieldOffset, 1, &data);
    if(rc != GT_OK)
        return rc;

    *enablePtr = (data == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgSrcIdPortUcastEgressFilterGet
*
* DESCRIPTION:
*     Get Per Egress Port Source ID egress filter configuration for unicast
*     packets. The Source ID filter is configured by
*     cpssDxChBrgSrcIdGroupPortAdd and cpssDxChBrgSrcIdGroupPortDelete.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*       portNum  - Physical port number/all ports wild card, CPU port
*
* OUTPUTS:
*       enablePtr   - GT_TRUE - enable Source ID filtering on unicast packets
*                            forwarded to this port. Unicast packet is dropped
*                            if egress port is not member in the Source ID group.
*                   - GT_FALSE - disable Source ID filtering on unicast packets
*                             forwarded to this port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdPortUcastEgressFilterGet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSrcIdPortUcastEgressFilterGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChBrgSrcIdPortUcastEgressFilterGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgSrcIdPortSrcIdForceEnableSet
*
* DESCRIPTION:
*         Set Source ID Assignment force mode per Ingress Port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number, CPU port
*       enable    - enable/disable SourceID force mode
*                     GT_TRUE - enable Source ID force mode
*                     GT_FALSE - disable Source ID force mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSrcIdPortSrcIdForceEnableSet
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    IN GT_BOOL      enable
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32      regAddr;     /* register address */
    GT_U32      data;        /* data to write into the register */
    GT_U32      fieldOffset; /* field offset */
    GT_U32      portGroupId; /* the port group Id - support multi-port-groups device */
    GT_U32       localPort;   /* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    data = BOOL2BIT_MAC(enable);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* set the <PORT_SOURCE_ID_FORCE_MODE>*/
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_EPORT_TABLE_FIELDS_PORT_SOURCE_ID_FORCE_MODE_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        data);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);


        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                       bridgeRegs.srcIdReg.srcIdAssignedModeConfReg;

        if(localPort == CPSS_CPU_PORT_NUM_CNS)
            fieldOffset = 31; /* CPU port is bit 31 */
        else
            fieldOffset = localPort;

        rc = prvCpssHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, fieldOffset, 1, data);
    }
    return rc;
}

/*******************************************************************************
* cpssDxChBrgSrcIdPortSrcIdForceEnableSet
*
* DESCRIPTION:
*         Set Source ID Assignment force mode per Ingress Port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number, CPU port
*       enable    - enable/disable SourceID force mode
*                     GT_TRUE - enable Source ID force mode
*                     GT_FALSE - disable Source ID force mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdPortSrcIdForceEnableSet
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    IN GT_BOOL      enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSrcIdPortSrcIdForceEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChBrgSrcIdPortSrcIdForceEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChBrgSrcIdPortSrcIdForceEnableGet
*
* DESCRIPTION:
*         Get Source ID Assignment force mode per Ingress Port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number, CPU port
*
* OUTPUTS:
*       enablePtr - (pointer to) Source ID Assignment force mode state
*                     GT_TRUE - Source ID force mode is enabled
*                     GT_FALSE - Source ID force mode is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSrcIdPortSrcIdForceEnableGet
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      data;       /* data to write into the register */
    GT_U32      fieldOffset;/* field offset */
    GT_STATUS   rc;         /* returned status */
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32       localPort;  /* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* get the <PORT_SOURCE_ID_FORCE_MODE>*/
        rc = prvCpssDxChReadTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_EPORT_TABLE_FIELDS_PORT_SOURCE_ID_FORCE_MODE_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &data);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                       bridgeRegs.srcIdReg.srcIdAssignedModeConfReg;

        if(localPort == CPSS_CPU_PORT_NUM_CNS)
            fieldOffset = 31; /* CPU port is bit 31 */
        else
            fieldOffset = localPort;

        rc = prvCpssHwPpPortGroupGetRegField(devNum,portGroupId, regAddr, fieldOffset, 1, &data);
    }
    if (GT_OK != rc)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(data);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgSrcIdPortSrcIdForceEnableGet
*
* DESCRIPTION:
*         Get Source ID Assignment force mode per Ingress Port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number, CPU port
*
* OUTPUTS:
*       enablePtr - (pointer to) Source ID Assignment force mode state
*                     GT_TRUE - Source ID force mode is enabled
*                     GT_FALSE - Source ID force mode is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSrcIdPortSrcIdForceEnableGet
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSrcIdPortSrcIdForceEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChBrgSrcIdPortSrcIdForceEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


