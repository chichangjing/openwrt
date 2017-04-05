/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgStp.c
*
* DESCRIPTION:
*       Spanning Tree protocol facility DxCh cpss implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 29 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/bridge/private/prvCpssDxChBrgLog.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgStp.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>

/* number of bits in the Bobcat2; Caelum; Bobcat3 - L2I ingress STP entry */
#define LION3_L2I_INGRESS_STP_BITS_SIZE_CNS  512
/* maximal size of STG entry in hemisphere */
#define PRV_DXCH_STG_HW_ENTRY_SIZE_CNS BITS_TO_WORDS_MAC(LION3_L2I_INGRESS_STP_BITS_SIZE_CNS)

static GT_STATUS stgEntrySplitTableWrite
(
    IN  GT_U8       dev,
    IN  GT_U16      stpId,
    IN  GT_U32     stpEntryWordArr[CPSS_DXCH_STG_ENTRY_SIZE_CNS]
);

static GT_STATUS stgEntrySplitTableRead
(
    IN  GT_U8       dev,
    IN  GT_U16      stpId,
    OUT  GT_U32     stpEntryWordArr[CPSS_DXCH_STG_ENTRY_SIZE_CNS]
);

/*Bobcat2 and above : compare ingress to egress*/
#define LION3_COMPARE_INGRESS_2_BITS_TO_EGRESS_1_BIT_MAC(ingressState,egressState) \
    if(((egressState == 1) && (ingressState != 0 && ingressState != 3)) ||         \
       ((egressState == 0) && (ingressState != 1 && ingressState != 2)))           \
    {                                                                              \
        /* the ingress and egress are not synchronized */                          \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);                                                       \
    }

/*Bobcat2 and above : convert ingress to egress*/
#define LION3_CONVERT_INGRESS_2_BITS_TO_EGRESS_1_BIT_MAC(ingressState)  \
        ((ingressState == 0 || ingressState == 3) ? 1 : 0)



/*******************************************************************************
* internal_cpssDxChBrgStpInit
*
* DESCRIPTION:
*       Initialize STP library.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev    - device number
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgStpInit
(
    IN GT_U8    dev
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgStpInit
*
* DESCRIPTION:
*       Initialize STP library.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev    - device number
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgStpInit
(
    IN GT_U8    dev
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgStpInit);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev));

    rc = internal_cpssDxChBrgStpInit(dev);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgStpStateSet
*
* DESCRIPTION:
*       Sets STP state of port belonging within an STP group.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev    - device number
*       portNum- port number
*       stpId  - STG (Spanning Tree Group) index
*       state  - STP port state.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device or port or stpId or state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgStpStateSet
(
    IN GT_U8                    dev,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_U16                   stpId,
    IN CPSS_STP_STATE_ENT       state
)
{
    GT_STATUS   rc;
    GT_U32      convState;          /* STP converted state */
    GT_U32      bitOffset;          /* bit offset in current word          */
    GT_U32      wordOffset;         /* current word offset                 */
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32      localPort;/* local port - support multi-port-groups device */
    GT_U32      ingressState;         /* 2 bits : ingress STP state */
    GT_U32      egressState;          /* 1 bit  : egress  STP state */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    PRV_CPSS_DXCH_STG_RANGE_CHECK_MAC(dev,stpId);

    /* convert STP port state to number */
    switch(state)
    {
        case CPSS_STP_DISABLED_E:
            convState = 0;
            break;
        case CPSS_STP_BLCK_LSTN_E:
            convState = 1;
            break;
        case CPSS_STP_LRN_E:
            convState = 2;
            break;
        case CPSS_STP_FRWRD_E:
            convState = 3;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(dev) == GT_TRUE)
    {
        PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(dev,portNum);

        ingressState = convState;

        rc = prvCpssDxChWriteTableEntryField(dev,
                                             PRV_CPSS_DXCH_LION_TABLE_STG_INGRESS_E,
                                             stpId,
                                             PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                             (2*portNum),
                                             2,
                                             ingressState);
        if(rc != GT_OK)
        {
            return rc;
        }

        /*convert ingress value to egress value*/
        egressState = LION3_CONVERT_INGRESS_2_BITS_TO_EGRESS_1_BIT_MAC(ingressState);

        /* call the generic function that handle port in tables of BMP of ports in the EGF_SHT */
        rc = prvCpssDxChHwEgfShtPortsBmpTableEntrySetPort(dev,
                    PRV_CPSS_DXCH_LION_TABLE_STG_EGRESS_E,
                    stpId,
                    portNum,
                    BIT2BOOL_MAC(egressState));
        return rc;
    }
    else
    {

        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(dev,portNum);

        if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
        {
            /* convert the 'Physical port' to local port -- supporting multi-port-groups device */
            localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(dev,portNum);
        }
        else
        {
            /* For Dual TXQ devices the  port is local to hemesphire; Others Global */
            localPort = PRV_CPSS_DXCH_GLOBAL_TO_HEM_LOCAL_PORT(dev,portNum);
        }

        /* calculate port stp state field offset in bits */
        /* each word in the STP entry hold 16 STP ports */
        wordOffset = (2 * localPort) >> 5;   /* / 32 */
        bitOffset = (2 * localPort) & 0x1f;  /* % 32 */

        /* convert the 'Physical port' to portGroupId -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, portNum);

        if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
        {
            return prvCpssDxChPortGroupWriteTableEntryField(dev,portGroupId,
                                                 PRV_CPSS_DXCH_TABLE_STG_E,
                                                 stpId,
                                                 wordOffset,
                                                 bitOffset,
                                                 2,
                                                 convState);
        }
        else
        {
            /*write to ingress*/
            rc = prvCpssDxChPortGroupWriteTableEntryField(dev,portGroupId,
                                                 PRV_CPSS_DXCH_LION_TABLE_STG_INGRESS_E,
                                                 stpId,
                                                 wordOffset,
                                                 bitOffset,
                                                 2,
                                                 convState);
            if(rc != GT_OK)
            {
                return rc;
            }

            /*write to egress*/
            return prvCpssDxChPortGroupWriteTableEntryField(dev,portGroupId,
                                                 PRV_CPSS_DXCH_LION_TABLE_STG_EGRESS_E,
                                                 stpId,
                                                 wordOffset,
                                                 bitOffset,
                                                 2,
                                                 convState);
        }
    }
}

/*******************************************************************************
* cpssDxChBrgStpStateSet
*
* DESCRIPTION:
*       Sets STP state of port belonging within an STP group.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev    - device number
*       portNum- port number
*       stpId  - STG (Spanning Tree Group) index
*       state  - STP port state.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device or port or stpId or state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgStpStateSet
(
    IN GT_U8                    dev,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_U16                   stpId,
    IN CPSS_STP_STATE_ENT       state
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgStpStateSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, portNum, stpId, state));

    rc = internal_cpssDxChBrgStpStateSet(dev, portNum, stpId, state);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, portNum, stpId, state));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgStpStateGet
*
* DESCRIPTION:
*       Gets STP state of port belonging within an STP group.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev         - device number
*       portNum     - port number
*       stpId       - STG (Spanning Tree Group)
*
* OUTPUTS:
*       statePtr    - (pointer to) STP port state.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device or port or stpId or state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgStpStateGet
(
    IN GT_U8                    dev,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_U16                   stpId,
    OUT CPSS_STP_STATE_ENT      *statePtr
)
{
    GT_STATUS   rc;                 /* return code  */
    GT_U32      convState;          /* STP converted state */
    GT_U32      bitOffset;          /* bit offset in current word          */
    GT_U32      wordOffset;         /* current word offset                 */
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32      localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    PRV_CPSS_DXCH_STG_RANGE_CHECK_MAC(dev,stpId);
    CPSS_NULL_PTR_CHECK_MAC(statePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(dev) == GT_TRUE)
    {
        PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(dev,portNum);

        /* read field only from ingress table */
        rc = prvCpssDxChReadTableEntryField(dev,
                                            PRV_CPSS_DXCH_LION_TABLE_STG_INGRESS_E,
                                            stpId,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            2*portNum,
                                            2,
                                            &convState);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(dev,portNum);

        if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
        {
            /* convert the 'Physical port' to local port -- supporting multi-port-groups device */
            localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(dev,portNum);
        }
        else
        {
            /* For Dual TXQ devices the  port is local to hemesphire; Others Global */
            localPort = PRV_CPSS_DXCH_GLOBAL_TO_HEM_LOCAL_PORT(dev,portNum);
        }

        /* calculate port stp state field offset in bits */
        /* each word in the STP entry hold 16 STP ports */
        wordOffset = (2 * localPort) >> 5;   /* / 32 */
        bitOffset = (2 * localPort) & 0x1f;  /* % 32 */

        /* convert the 'Physical port' to portGroupId -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, portNum);

        if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
        {
            rc = prvCpssDxChPortGroupReadTableEntryField(dev,portGroupId,
                                                         PRV_CPSS_DXCH_TABLE_STG_E,
                                                         stpId,
                                                         wordOffset,
                                                         bitOffset,
                                                         2,
                                                         &convState);
        }
        else
        {
            /* read field only from ingress table */
            rc = prvCpssDxChPortGroupReadTableEntryField(dev,portGroupId,
                                                PRV_CPSS_DXCH_LION_TABLE_STG_INGRESS_E,
                                                stpId,
                                                wordOffset,
                                                bitOffset,
                                                2,
                                                &convState);
        }
    }

    if( GT_OK != rc )
    {
        return rc;
    }

    /* convert number to STP port state */
    switch(convState)
    {
        case 0: *statePtr = CPSS_STP_DISABLED_E;
                break;
        case 1: *statePtr = CPSS_STP_BLCK_LSTN_E;
                break;
        case 2: *statePtr = CPSS_STP_LRN_E;
                break;
        case 3: *statePtr = CPSS_STP_FRWRD_E;
                break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgStpStateGet
*
* DESCRIPTION:
*       Gets STP state of port belonging within an STP group.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev         - device number
*       portNum     - port number
*       stpId       - STG (Spanning Tree Group)
*
* OUTPUTS:
*       statePtr    - (pointer to) STP port state.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device or port or stpId or state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgStpStateGet
(
    IN GT_U8                    dev,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_U16                   stpId,
    OUT CPSS_STP_STATE_ENT      *statePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgStpStateGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, portNum, stpId, statePtr));

    rc = internal_cpssDxChBrgStpStateGet(dev, portNum, stpId, statePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, portNum, stpId, statePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgStpEntryGet
*
* DESCRIPTION:
*       Gets the RAW STP entry from HW.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev    - device number
*       stpId  - STG (Spanning Tree Group) index (APPLICABLE RANGES: 0..255)
*
* OUTPUTS:
*       stpEntryWordArr     - (Pointer to) The raw stp entry to get.
*                         memory allocated by the caller.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device or stpId
*       GT_BAD_PTR               - on NULL pointer parameter
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgStpEntryGet
(
    IN  GT_U8       dev,
    IN  GT_U16      stpId,
    OUT GT_U32      stpEntryWordArr[CPSS_DXCH_STG_ENTRY_SIZE_CNS]
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    PRV_CPSS_DXCH_STG_RANGE_CHECK_MAC(dev,stpId);
    CPSS_NULL_PTR_CHECK_MAC(stpEntryWordArr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        return stgEntrySplitTableRead(dev,stpId,stpEntryWordArr);
    }
    else
    {
        return prvCpssDxChReadTableEntry(dev,
                                         PRV_CPSS_DXCH_TABLE_STG_E,
                                         (GT_U32)stpId,
                                         stpEntryWordArr);
    }
}

/*******************************************************************************
* cpssDxChBrgStpEntryGet
*
* DESCRIPTION:
*       Gets the RAW STP entry from HW.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev    - device number
*       stpId  - STG (Spanning Tree Group) index (APPLICABLE RANGES: 0..255)
*
* OUTPUTS:
*       stpEntryWordArr     - (Pointer to) The raw stp entry to get.
*                         memory allocated by the caller.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device or stpId
*       GT_BAD_PTR               - on NULL pointer parameter
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgStpEntryGet
(
    IN  GT_U8       dev,
    IN  GT_U16      stpId,
    OUT GT_U32      stpEntryWordArr[CPSS_DXCH_STG_ENTRY_SIZE_CNS]
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgStpEntryGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, stpId, stpEntryWordArr));

    rc = internal_cpssDxChBrgStpEntryGet(dev, stpId, stpEntryWordArr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, stpId, stpEntryWordArr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgStpEntryWrite
*
* DESCRIPTION:
*       Writes a raw STP entry to HW.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev             - device number
*       stpId           - STG (Spanning Tree Group) index (APPLICABLE RANGES: 0..255)
*       stpEntryWordArr     - (Pointer to) The raw stp entry to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device or stpId
*       GT_BAD_PTR               - on NULL pointer parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgStpEntryWrite
(
    IN  GT_U8       dev,
    IN  GT_U16      stpId,
    IN  GT_U32      stpEntryWordArr[CPSS_DXCH_STG_ENTRY_SIZE_CNS]
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    PRV_CPSS_DXCH_STG_RANGE_CHECK_MAC(dev,stpId);
    CPSS_NULL_PTR_CHECK_MAC(stpEntryWordArr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        return stgEntrySplitTableWrite(dev,stpId,stpEntryWordArr);
    }
    else
    {
        return prvCpssDxChWriteTableEntry(dev,
                                          PRV_CPSS_DXCH_TABLE_STG_E,
                                          (GT_U32)stpId,
                                          stpEntryWordArr);
    }
}

/*******************************************************************************
* cpssDxChBrgStpEntryWrite
*
* DESCRIPTION:
*       Writes a raw STP entry to HW.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev             - device number
*       stpId           - STG (Spanning Tree Group) index (APPLICABLE RANGES: 0..255)
*       stpEntryWordArr     - (Pointer to) The raw stp entry to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device or stpId
*       GT_BAD_PTR               - on NULL pointer parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgStpEntryWrite
(
    IN  GT_U8       dev,
    IN  GT_U16      stpId,
    IN  GT_U32      stpEntryWordArr[CPSS_DXCH_STG_ENTRY_SIZE_CNS]
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgStpEntryWrite);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, stpId, stpEntryWordArr));

    rc = internal_cpssDxChBrgStpEntryWrite(dev, stpId, stpEntryWordArr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, stpId, stpEntryWordArr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* stgEntrySplitTableRead
*
* DESCRIPTION:
*       Gets the RAW STP entry from HW. - for split table (ingress,egress)
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       dev    - device number
*       stpId  - STG (Spanning Tree Group) index (APPLICABLE RANGES: 0..255)
*
* OUTPUTS:
*       stpEntryWordArr     - (Pointer to) The raw stp entry to get.
*                         memory allocated by the caller.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device or stpId
*       GT_BAD_PTR               - on NULL pointer parameter
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_BAD_STATE    - the ingress table and egress table not match
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS stgEntrySplitTableRead
(
    IN  GT_U8       dev,
    IN  GT_U16      stpId,
    OUT  GT_U32     stpEntryWordArr[CPSS_DXCH_STG_ENTRY_SIZE_CNS]
)
{
    GT_STATUS rc;
    GT_U32  tmpHwStpEntry[PRV_DXCH_STG_HW_ENTRY_SIZE_CNS]; /* Lion2 has 8 words in entry */
    GT_U32  egressStpEntry[CPSS_DXCH_STG_ENTRY_SIZE_CNS];
    CPSS_PORTS_BMP_STC egressPortsMembers;/* Bobcat2; Caelum; Bobcat3 : bmp of ports for egress */
    GT_U32  i;
    GT_U32      ingressState;         /* 2 bits : ingress STP converted state */
    GT_U32      egressState;          /* 1 bit  : egress  STP converted state */
    GT_U32    port;/*port iterator*/
    GT_U32    numOfPhyPorts;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(dev) == GT_TRUE)
    {
        /* read from ingress table */
        rc =  prvCpssDxChReadTableEntry(dev,
                            PRV_CPSS_DXCH_LION_TABLE_STG_INGRESS_E,
                            stpId,
                            stpEntryWordArr);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* read from egress table */
        /* call the generic function that handle BMP of ports in the EGF_SHT */
        rc = prvCpssDxChHwEgfShtPortsBmpTableEntryRead(dev,
                PRV_CPSS_DXCH_LION_TABLE_STG_EGRESS_E,
                stpId,
                &egressPortsMembers,
                GT_FALSE,
                NULL);/* don't care */
        if(rc != GT_OK)
        {
            return rc;
        }

        /* check if ingress and egress are synchronized */
        numOfPhyPorts = PRV_CPSS_DXCH_MAX_PHY_PORT_NUMBER_MAC(dev);
        if (numOfPhyPorts > CPSS_MAX_PORTS_NUM_CNS)
        {
            /* should not occur, it is a bug */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
        for(port = 0 ; port < numOfPhyPorts ; port++)
        {
            /* get 2 bits of the ingress port */
            U32_GET_FIELD_IN_ENTRY_MAC(stpEntryWordArr ,(port*2),2,ingressState);
            /* get 1 bits of the egress port */
            U32_GET_FIELD_IN_ENTRY_MAC(egressPortsMembers.ports, port   ,1,egressState );

            /* compare for 'synchronized' */
            LION3_COMPARE_INGRESS_2_BITS_TO_EGRESS_1_BIT_MAC(ingressState,egressState);
        }

        for(/* continue port */ ; port < PRV_CPSS_MAX_PP_PORTS_NUM_CNS ; port++)
        {
            /* set 2 bits of the 'OUT' to be 0 ... don't care ! */
            U32_SET_FIELD_IN_ENTRY_MAC(stpEntryWordArr ,(port*2),2,0);
        }


        return GT_OK;
    }

    cpssOsMemSet(&egressStpEntry[0],0,sizeof(egressStpEntry));

    /* loop on all TXQ units */
    for (i = 0; i < PRV_CPSS_DXCH_PP_HW_INFO_TXQ_UNITS_NUM_MAC(dev); i++)
    {
        /* read from ingress table */
        rc =  prvCpssDxChPortGroupReadTableEntry(dev,
                            i*PRV_CPSS_DXCH_PORT_GROUPS_NUM_IN_HEMISPHERE_CNS,
                            PRV_CPSS_DXCH_LION_TABLE_STG_INGRESS_E,
                            (GT_U32)stpId,
                            tmpHwStpEntry);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* copy data from hemisphere */
        cpssOsMemCpy(&(stpEntryWordArr[i*4]),tmpHwStpEntry, sizeof(GT_U32) * 4);

        /* read from egress table */
        rc =  prvCpssDxChPortGroupReadTableEntry(dev,
                            i*PRV_CPSS_DXCH_PORT_GROUPS_NUM_IN_HEMISPHERE_CNS,
                            PRV_CPSS_DXCH_LION_TABLE_STG_EGRESS_E,
                            (GT_U32)stpId,
                            tmpHwStpEntry);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* copy data from hemisphere */
        cpssOsMemCpy(&(egressStpEntry[i*4]),tmpHwStpEntry, sizeof(GT_U32) * 4);
    }


    if(0 != cpssOsMemCmp(egressStpEntry,stpEntryWordArr,sizeof(egressStpEntry)))
    {
        /* the ingress STG and the egress STG not synchronized ?! */
        /* we have not API to allow this situation */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* stgEntrySplitTableWrite
*
* DESCRIPTION:
*       Sets the RAW STP entry from HW. - for split table (ingress,egress)
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       dev    - device number
*       stpId  - STG (Spanning Tree Group) index (APPLICABLE RANGES: 0..255)
*       stpEntryWordArr     - (Pointer to) The raw stp entry to get.
*                         memory allocated by the caller.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device or stpId
*       GT_BAD_PTR               - on NULL pointer parameter
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS stgEntrySplitTableWrite
(
    IN  GT_U8      dev,
    IN  GT_U16     stpId,
    IN  GT_U32     stpEntryWordArr[CPSS_DXCH_STG_ENTRY_SIZE_CNS]
)
{
    GT_STATUS rc;
    GT_U32    i;
    GT_U32    portGroupId;/*the port group Id */
    CPSS_PORTS_BMP_STC egressPortsMembers;/* Bobcat2; Caelum; Bobcat3: bmp of ports for egress */
    GT_U32    port;/*port iterator*/
    GT_U32      ingressState;         /* 2 bits : ingress STP state */
    GT_U32      egressState;          /* 1 bit  : egress  STP state */

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(dev) == GT_TRUE)
    {
        /* all the port groups see the same configurations */

        /* write to ingress table */
        rc = prvCpssDxChWriteTableEntry(dev,
                            PRV_CPSS_DXCH_LION_TABLE_STG_INGRESS_E,
                            stpId,
                            stpEntryWordArr);
        if(rc != GT_OK)
        {
            return rc;
        }

        /*convert ingress values to egress values*/
        for(port = 0 ; port < PRV_CPSS_DXCH_MAX_PHY_PORT_NUMBER_MAC(dev) ; port++)
        {
            /* get 2 bits of the ingress port */
            U32_GET_FIELD_IN_ENTRY_MAC(stpEntryWordArr ,(port*2),2,ingressState);

            /*convert ingress value to egress value*/
            egressState = LION3_CONVERT_INGRESS_2_BITS_TO_EGRESS_1_BIT_MAC(ingressState);
            /* set 1 bits for the egress port */
            U32_SET_FIELD_IN_ENTRY_MAC(egressPortsMembers.ports, port ,1,egressState );
        }

        for(/* continue port */ ; port < PRV_CPSS_MAX_PP_PORTS_NUM_CNS ; port++)
        {
            /* get 2 bits of the ingress port */
            U32_GET_FIELD_IN_ENTRY_MAC(stpEntryWordArr ,(port*2),2,ingressState);

            if(ingressState != 0)
            {
                /* port not exists in range ... make sure set as '0' */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }


        /* call the generic function that handle BMP of ports in the EGF_SHT */
        rc = prvCpssDxChHwEgfShtPortsBmpTableEntryWrite(dev,
                PRV_CPSS_DXCH_LION_TABLE_STG_EGRESS_E,
                stpId,
                &egressPortsMembers,
                GT_FALSE,
                GT_FALSE);/* don't care */
        return rc;
    }

    /* loop on all TXQ units */
    for (i = 0; i < PRV_CPSS_DXCH_PP_HW_INFO_TXQ_UNITS_NUM_MAC(dev); i++)
    {
        portGroupId = i*PRV_CPSS_DXCH_PORT_GROUPS_NUM_IN_HEMISPHERE_CNS;

        /* write to ingress table */
        rc = prvCpssDxChPortGroupWriteTableEntry(dev,
                            portGroupId,
                            PRV_CPSS_DXCH_LION_TABLE_STG_INGRESS_E,
                            (GT_U32)stpId,
                            &(stpEntryWordArr[i*4]));
        if(rc != GT_OK)
        {
            return rc;
        }

        /* write to egress table */
        rc =  prvCpssDxChPortGroupWriteTableEntry(dev,
                            portGroupId,
                            PRV_CPSS_DXCH_LION_TABLE_STG_EGRESS_E,
                            (GT_U32)stpId,
                            &(stpEntryWordArr[i*4]));
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}


/*******************************************************************************
* internal_cpssDxChBrgStpPortSpanningTreeStateSet
*
* DESCRIPTION:
*       Set Per-Port STP state.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*       state  - STP port state.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable when <Port STP state Mode> = ePort
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgStpPortSpanningTreeStateSet
(
    IN GT_U8                devNum,
    IN GT_PORT_NUM          portNum,
    IN CPSS_STP_STATE_ENT   state
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32      ingressState;         /* 2 bits : ingress STP converted state */
    GT_U32      egressState;          /* 1 bit  : egress  STP converted state */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

     /* convert STP port state to number */
    switch(state)
    {
        case CPSS_STP_DISABLED_E:
            ingressState = 0;
            break;
        case CPSS_STP_BLCK_LSTN_E:
            ingressState = 1;
            break;
        case CPSS_STP_LRN_E:
            ingressState = 2;
            break;
        case CPSS_STP_FRWRD_E:
            ingressState = 3;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /*convert ingress value to egress value*/
    egressState = LION3_CONVERT_INGRESS_2_BITS_TO_EGRESS_1_BIT_MAC(ingressState);

    /* set the <INGRESS_EPORT_SPANNING_TREE_STATE>*/
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                   PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                    portNum,
                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                    LION3_L2I_EPORT_TABLE_FIELDS_INGRESS_EPORT_SPANNING_TREE_STATE_E, /* field name */
                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                    ingressState);

    if(rc != GT_OK)
        return rc;

    /* set EGF_SHT unit */
    rc = prvCpssDxChHwEgfShtFieldSet(devNum,portNum,
        PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_STP_STATE_E,
            GT_TRUE, /*accessPhysicalPort*/
            GT_TRUE, /*accessEPort*/
            egressState );

    return rc;
}

/*******************************************************************************
* cpssDxChBrgStpPortSpanningTreeStateSet
*
* DESCRIPTION:
*       Set Per-Port STP state.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*       state  - STP port state.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable when <Port STP state Mode> = ePort
*
*******************************************************************************/
GT_STATUS cpssDxChBrgStpPortSpanningTreeStateSet
(
    IN GT_U8                devNum,
    IN GT_PORT_NUM          portNum,
    IN CPSS_STP_STATE_ENT   state
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgStpPortSpanningTreeStateSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, state));

    rc = internal_cpssDxChBrgStpPortSpanningTreeStateSet(devNum, portNum, state);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, state));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgStpPortSpanningTreeStateGet
*
* DESCRIPTION:
*       Get Per-Port STP state.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*
* OUTPUTS:
*       statePtr    - (pointer to) STP port state.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgStpPortSpanningTreeStateGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_NUM             portNum,
    OUT CPSS_STP_STATE_ENT      *statePtr
)
{
    GT_STATUS   rc = GT_OK;             /* function return code */
    GT_U32      ingressState, egressState; /* STP converted state */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(statePtr);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* get the <INGRESS_EPORT_SPANNING_TREE_STATE>*/
    rc = prvCpssDxChReadTableEntryField(devNum,
                                   PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                    portNum,
                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                    LION3_L2I_EPORT_TABLE_FIELDS_INGRESS_EPORT_SPANNING_TREE_STATE_E, /* field name */
                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                    &ingressState);
    if(rc!=GT_OK)
    {
        return rc;
    }

    /* set EGF_SHT unit */
    rc = prvCpssDxChHwEgfShtFieldGet(devNum,portNum,
        PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_STP_STATE_E,
            GT_FALSE, /*accessPhysicalPort*/
            &egressState );
    if(rc != GT_OK)
    {
        return rc;
    }

    LION3_COMPARE_INGRESS_2_BITS_TO_EGRESS_1_BIT_MAC(ingressState,egressState);

    /* convert ingressState to STP port state */
    switch(ingressState)
    {
        case 0: *statePtr = CPSS_STP_DISABLED_E;
                break;
        case 1: *statePtr = CPSS_STP_BLCK_LSTN_E;
                break;
        case 2: *statePtr = CPSS_STP_LRN_E;
                break;
        case 3: *statePtr = CPSS_STP_FRWRD_E;
                break;
        default:
            /* should not happen , since getting 2 bits value */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgStpPortSpanningTreeStateGet
*
* DESCRIPTION:
*       Get Per-Port STP state.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*
* OUTPUTS:
*       statePtr    - (pointer to) STP port state.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgStpPortSpanningTreeStateGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_NUM             portNum,
    OUT CPSS_STP_STATE_ENT      *statePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgStpPortSpanningTreeStateGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, statePtr));

    rc = internal_cpssDxChBrgStpPortSpanningTreeStateGet(devNum, portNum, statePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, statePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgStpPortSpanningTreeStateModeSet
*
* DESCRIPTION:
*       Determines whether the STP state for this port is
*       taken from the L2I ePort table or from the STP group table via the STG
*       index from eVLAN table
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       stateMode   - Port STP state Mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgStpPortSpanningTreeStateModeSet
(
    IN GT_U8                        devNum,
    IN GT_PORT_NUM                  portNum,
    IN CPSS_BRG_STP_STATE_MODE_ENT  stateMode
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32      convState;          /* STP converted state mode */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

     /* convert STP port state to number */
    switch(stateMode)
    {
        case CPSS_BRG_STP_E_VLAN_MODE_E:
            convState = 0;
            break;
        case CPSS_BRG_STP_E_PORT_MODE_E:
            convState = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* set the <INGRESS_EPORT_STP_STATE_MODE>*/
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                   PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                    portNum,
                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                    LION3_L2I_EPORT_TABLE_FIELDS_INGRESS_EPORT_STP_STATE_MODE_E, /* field name */
                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                    convState);

    if(rc != GT_OK)
        return rc;


    /* set EGF_SHT unit */
    rc = prvCpssDxChHwEgfShtFieldSet(devNum,portNum,
        PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_STP_STATE_MODE_E,
            GT_TRUE, /*accessPhysicalPort*/
            GT_TRUE, /*accessEPort*/
            convState );

    return rc;
}

/*******************************************************************************
* cpssDxChBrgStpPortSpanningTreeStateModeSet
*
* DESCRIPTION:
*       Determines whether the STP state for this port is
*       taken from the L2I ePort table or from the STP group table via the STG
*       index from eVLAN table
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       stateMode   - Port STP state Mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgStpPortSpanningTreeStateModeSet
(
    IN GT_U8                        devNum,
    IN GT_PORT_NUM                  portNum,
    IN CPSS_BRG_STP_STATE_MODE_ENT  stateMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgStpPortSpanningTreeStateModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, stateMode));

    rc = internal_cpssDxChBrgStpPortSpanningTreeStateModeSet(devNum, portNum, stateMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, stateMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgStpPortSpanningTreeStateModeGet
*
* DESCRIPTION:
*       Get whether the STP state for this port is taken from the
*       L2I ePort table or from the STP group table via the STG index from eVLAN table.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*
* OUTPUTS:
*       stateModePtr  - (pointer to) Port STP state Mode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgStpPortSpanningTreeStateModeGet
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_NUM                     portNum,
    OUT CPSS_BRG_STP_STATE_MODE_ENT     *stateModePtr
)
{
    GT_STATUS   rc = GT_OK;     /* function return code */
    GT_U32      convState, convState1;  /* STP converted state */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(stateModePtr);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* get the <INGRESS_EPORT_STP_STATE_MODE>*/
    rc = prvCpssDxChReadTableEntryField(devNum,
                                   PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                    portNum,
                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                    LION3_L2I_EPORT_TABLE_FIELDS_INGRESS_EPORT_STP_STATE_MODE_E, /* field name */
                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                    &convState);
    if(rc!=GT_OK)
    {
        return rc;
    }

    /* get EGF_SHT unit */
    rc = prvCpssDxChHwEgfShtFieldGet(devNum,portNum,
        PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_STP_STATE_MODE_E,
            GT_FALSE, /*accessPhysicalPort*/
            &convState1 );

    if(rc!=GT_OK)
    {
        return rc;
    }

    if(convState != convState1)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

     /* convert number to STP port state */
    switch(convState)
    {
        case 0:
            *stateModePtr = CPSS_BRG_STP_E_VLAN_MODE_E;
            break;
        case 1:
            *stateModePtr = CPSS_BRG_STP_E_PORT_MODE_E;
            break;
        default:
            /* should not happen - since getting 1 bit value */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgStpPortSpanningTreeStateModeGet
*
* DESCRIPTION:
*       Get whether the STP state for this port is taken from the
*       L2I ePort table or from the STP group table via the STG index from eVLAN table.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*
* OUTPUTS:
*       stateModePtr  - (pointer to) Port STP state Mode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgStpPortSpanningTreeStateModeGet
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_NUM                     portNum,
    OUT CPSS_BRG_STP_STATE_MODE_ENT     *stateModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgStpPortSpanningTreeStateModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, stateModePtr));

    rc = internal_cpssDxChBrgStpPortSpanningTreeStateModeGet(devNum, portNum, stateModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, stateModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


