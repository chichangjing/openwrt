/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChTmGlueQueueMap.c
*
* DESCRIPTION:
*       Traffic Manager Glue - Packet Queue Id mapping.
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/tmGlue/private/prvCpssDxChTmGlueLog.h>

#include <cpss/dxCh/dxChxGen/tmGlue/cpssDxChTmGlueQueueMap.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

/*******************************************************************************
* internal_cpssDxChTmGlueQueueMapBypassEnableSet
*
* DESCRIPTION:
*   The function sets enable/disable state of Bypass TM Queue Mapping.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum             - device number
*       bypass             - GT_TRUE - bypass, GT_FALSE - mapping.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTmGlueQueueMapBypassEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   bypass
)
{
    GT_STATUS rc;      /* return code */
    GT_U32    regAddr; /* register address */
    GT_U32    data;    /* HW data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT3_E  );

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    data = BOOL2BIT_MAC(bypass);

    regAddr = PRV_DXCH_REG1_UNIT_TM_QMAP_MAC(devNum).TMQMAPGlobalConfig;

    rc = prvCpssHwPpSetRegField(
        devNum, regAddr, 25/*fieldOffset*/, 1/*fieldLength*/, data);

    return rc;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapBypassEnableSet
*
* DESCRIPTION:
*   The function sets enable/disable state of Bypass TM Queue Mapping.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum             - device number
*       bypass             - GT_TRUE - bypass, GT_FALSE - mapping.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
* 
* Bypassing TM Queue Mapping should not be activated on TM enabled systems.
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapBypassEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   bypass
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueQueueMapBypassEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, bypass));

    rc = internal_cpssDxChTmGlueQueueMapBypassEnableSet(devNum, bypass);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, bypass));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTmGlueQueueMapBypassEnableGet
*
* DESCRIPTION:
*   The function gets enable/disable state of Bypass TM Queue Mapping.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum             - device number
*
* OUTPUTS:
*       bypassPtr          - (pointer to)GT_TRUE - bypass, GT_FALSE - mapping.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTmGlueQueueMapBypassEnableGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *bypassPtr
)
{
    GT_STATUS rc;      /* return code */
    GT_U32    regAddr; /* register address */
    GT_U32    data;    /* HW data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT3_E  );

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    CPSS_NULL_PTR_CHECK_MAC(bypassPtr);

    regAddr = PRV_DXCH_REG1_UNIT_TM_QMAP_MAC(devNum).TMQMAPGlobalConfig;

    rc = prvCpssHwPpGetRegField(
        devNum, regAddr, 25/*fieldOffset*/, 1/*fieldLength*/, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    *bypassPtr  = BIT2BOOL_MAC(data);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapBypassEnableGet
*
* DESCRIPTION:
*   The function gets enable/disable state of Bypass TM Queue Mapping.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum             - device number
*
* OUTPUTS:
*       bypassPtr          - (pointer to)GT_TRUE - bypass, GT_FALSE - mapping.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapBypassEnableGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *bypassPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueQueueMapBypassEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, bypassPtr));

    rc = internal_cpssDxChTmGlueQueueMapBypassEnableGet(devNum, bypassPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, bypassPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTmGlueQueueMapBitSelectTableEntrySet
*
* DESCRIPTION:
*   The function sets the Queue Id Map Bit Select Table Entry.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       entryIndex     - entry index
*                        (APPLICABLE RANGES: 0..255).
*       entryPtr       - (pointer to) entry contents
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on out-of-range parameters
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTmGlueQueueMapBitSelectTableEntrySet
(
    IN  GT_U8                                              devNum,
    IN  GT_U32                                             entryIndex,
    IN  CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_ENTRY_STC   *entryPtr
)
{
    GT_STATUS rc;         /* return code     */
    GT_U32    dataArr[8]; /* HW data         */
    GT_U32    i;          /* loop index      */
    GT_U32    offset;     /* field offset    */
    GT_U32    hwFldType;  /* HW field type   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT3_E  );

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* entryIndex will be checked by table access function */

    CPSS_NULL_PTR_CHECK_MAC(entryPtr);

    cpssOsMemSet(dataArr, 0, sizeof(dataArr));

    if (entryPtr->queueIdBase >= BIT_14)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    dataArr[0] |= entryPtr->queueIdBase;

    for (i = 0; (i < 14); i++)
    {
        switch (entryPtr->bitSelectArr[i].selectType)
        {
            case CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_ZERO_E:
                hwFldType = 0;
                break;
            case CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_EPCL_QUEUE_ID_E:
                hwFldType = 1;
                break;
            case CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_FLOW_ID_E:
                hwFldType = 2;
                break;
            case CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_LOCAL_TARGET_PHY_PORT_E:
                hwFldType = 3;
                break;
            case CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_TM_TC_E:
                hwFldType = 4;
                break;
            case CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_PACKET_HASH_E:
                hwFldType = 5;
                break;
            case CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_TARGET_DEV_E:
                hwFldType = 6;
                break;
            case CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_TARGET_PHY_PORT_E:
                hwFldType = 7;
                break;
            case CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_TARGET_EPORT_E:
                hwFldType = 8;
                break;
            case CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_ONE_E:
                hwFldType = 9;
                break;
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if (entryPtr->bitSelectArr[i].bitSelector >= BIT_4)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }

        /* set field type */
        offset  = 14 + (i * 4);
        U32_SET_FIELD_IN_ENTRY_MAC(dataArr, offset, 4/*length*/, hwFldType);

        /* set offset of bit in the field */
        offset = 70 + (i * 4);
        U32_SET_FIELD_IN_ENTRY_MAC(
            dataArr, offset, 4/*length*/, entryPtr->bitSelectArr[i].bitSelector);
    }

    rc = prvCpssDxChWriteTableEntry(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_TM_QMAP_QUEUE_ID_SELECTOR_E,
        entryIndex, dataArr);

    return rc;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapBitSelectTableEntrySet
*
* DESCRIPTION:
*   The function sets the Queue Id Map Bit Select Table Entry.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       entryIndex     - entry index
*                        (APPLICABLE RANGES: 0..255).
*       entryPtr       - (pointer to) entry contents
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on out-of-range parameters
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapBitSelectTableEntrySet
(
    IN  GT_U8                                              devNum,
    IN  GT_U32                                             entryIndex,
    IN  CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_ENTRY_STC   *entryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueQueueMapBitSelectTableEntrySet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, entryIndex, entryPtr));

    rc = internal_cpssDxChTmGlueQueueMapBitSelectTableEntrySet(devNum, entryIndex, entryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, entryIndex, entryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTmGlueQueueMapBitSelectTableEntryGet
*
* DESCRIPTION:
*   The function gets the Queue Id Map Bit Select Table Entry.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       entryIndex     - entry index
*                        (APPLICABLE RANGES: 0..255).
*
* OUTPUTS:
*       entryPtr       - (pointer to) entry contents
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - on unexpected HW value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTmGlueQueueMapBitSelectTableEntryGet
(
    IN  GT_U8                                              devNum,
    IN  GT_U32                                             entryIndex,
    OUT CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_ENTRY_STC   *entryPtr
)
{
    GT_STATUS rc;         /* return code     */
    GT_U32    dataArr[8]; /* HW data         */
    GT_U32    i;          /* loop index      */
    GT_U32    offset;     /* field offset    */
    GT_U32    hwFldType;  /* HW field type   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT3_E  );

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* entryIndex will be checked by table access function */

    CPSS_NULL_PTR_CHECK_MAC(entryPtr);

    rc = prvCpssDxChReadTableEntry(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_TM_QMAP_QUEUE_ID_SELECTOR_E,
        entryIndex, dataArr);
    if (rc != GT_OK)
    {
        return rc;
    }

    entryPtr->queueIdBase = (dataArr[0] & 0x3FFF);

    for (i = 0; (i < 14); i++)
    {
        /* get field type */
        offset  = 14 + (i * 4);
        U32_GET_FIELD_IN_ENTRY_MAC(dataArr, offset, 4/*length*/, hwFldType);

        switch (hwFldType)
        {
            case 0:
                entryPtr->bitSelectArr[i].selectType =
                    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_ZERO_E;
                break;
            case 1:
                entryPtr->bitSelectArr[i].selectType =
                    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_EPCL_QUEUE_ID_E;
                break;
            case 2:
                entryPtr->bitSelectArr[i].selectType =
                    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_FLOW_ID_E;
                break;
            case 3:
                entryPtr->bitSelectArr[i].selectType =
                    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_LOCAL_TARGET_PHY_PORT_E;
                break;
            case 4:
                entryPtr->bitSelectArr[i].selectType =
                    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_TM_TC_E;
                break;
            case 5:
                entryPtr->bitSelectArr[i].selectType =
                    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_PACKET_HASH_E;
                break;
            case 6:
                entryPtr->bitSelectArr[i].selectType =
                    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_TARGET_DEV_E;
                break;
            case 7:
                entryPtr->bitSelectArr[i].selectType =
                    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_TARGET_PHY_PORT_E;
                break;
            case 8:
                entryPtr->bitSelectArr[i].selectType =
                    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_TARGET_EPORT_E;
                break;
            case 9:
                entryPtr->bitSelectArr[i].selectType =
                    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_ONE_E;
                break;
            default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        /* get offset of bit in the field */
        offset = 70 + (i * 4);
        U32_GET_FIELD_IN_ENTRY_MAC(
            dataArr, offset, 4/*length*/, entryPtr->bitSelectArr[i].bitSelector);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapBitSelectTableEntryGet
*
* DESCRIPTION:
*   The function gets the Queue Id Map Bit Select Table Entry.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       entryIndex     - entry index
*                        (APPLICABLE RANGES: 0..255).
*
* OUTPUTS:
*       entryPtr       - (pointer to) entry contents
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - on unexpected HW value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapBitSelectTableEntryGet
(
    IN  GT_U8                                              devNum,
    IN  GT_U32                                             entryIndex,
    OUT CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_ENTRY_STC   *entryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueQueueMapBitSelectTableEntryGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, entryIndex, entryPtr));

    rc = internal_cpssDxChTmGlueQueueMapBitSelectTableEntryGet(devNum, entryIndex, entryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, entryIndex, entryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTmGlueQueueMapTargetDeviceToIndexMapSet
*
* DESCRIPTION:
*   The function sets the Queue Id Map Bit Select Table Entry index
*   mapped to the given target device.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       targetHwDevId  - target HW device Id
*                        (APPLICABLE RANGES: 0..1023).
*       entryIndex     - index of Queue Id Map Bit Select Table Entry
*                        being mapped to the given target device.
*                        (APPLICABLE RANGES: 0..255).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on out-of-range parameters
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTmGlueQueueMapTargetDeviceToIndexMapSet
(
    IN  GT_U8           devNum,
    IN  GT_HW_DEV_NUM   targetHwDevId,
    IN  GT_U32          entryIndex
)
{
    GT_STATUS rc;         /* return code     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT3_E  );

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* targetHwDevId used as table index and checked */
    /* by table access function                    */

    if (entryIndex >= BIT_8)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChWriteTableEntry(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_TM_QMAP_TARGET_DEV_TO_INDEX_E,
        targetHwDevId/*entryIndex*/, &entryIndex/*entryData*/);

    return rc;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapTargetDeviceToIndexMapSet
*
* DESCRIPTION:
*   The function sets the Queue Id Map Bit Select Table Entry index
*   mapped to the given target device.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       targetHwDevId  - target HW device Id
*                        (APPLICABLE RANGES: 0..1023).
*       entryIndex     - index of Queue Id Map Bit Select Table Entry
*                        being mapped to the given target device.
*                        (APPLICABLE RANGES: 0..255).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on out-of-range parameters
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapTargetDeviceToIndexMapSet
(
    IN  GT_U8           devNum,
    IN  GT_HW_DEV_NUM   targetHwDevId,
    IN  GT_U32          entryIndex
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueQueueMapTargetDeviceToIndexMapSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, targetHwDevId, entryIndex));

    rc = internal_cpssDxChTmGlueQueueMapTargetDeviceToIndexMapSet(devNum, targetHwDevId, entryIndex);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, targetHwDevId, entryIndex));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTmGlueQueueMapTargetDeviceToIndexMapGet
*
* DESCRIPTION:
*   The function gets the Queue Id Map Bit Select Table Entry index
*   mapped to the given target device.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       targetHwDevId  - target HW device Id
*                        (APPLICABLE RANGES: 0..1023).
*
* OUTPUTS:
*       entryIndexPtr  - (pointer to)index of Queue Id Map Bit Select Table Entry
*                        being mapped to the given target device.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTmGlueQueueMapTargetDeviceToIndexMapGet
(
    IN  GT_U8           devNum,
    IN  GT_HW_DEV_NUM   targetHwDevId,
    OUT GT_U32          *entryIndexPtr
)
{
    GT_STATUS rc;         /* return code     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT3_E  );

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* targetHwDevId used as table index and checked */
    /* by table access function                    */

    CPSS_NULL_PTR_CHECK_MAC(entryIndexPtr);

    rc = prvCpssDxChReadTableEntry(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_TM_QMAP_TARGET_DEV_TO_INDEX_E,
        targetHwDevId/*entryIndex*/, entryIndexPtr/*entryData*/);

    return rc;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapTargetDeviceToIndexMapGet
*
* DESCRIPTION:
*   The function gets the Queue Id Map Bit Select Table Entry index
*   mapped to the given target device.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       targetHwDevId  - target HW device Id
*                        (APPLICABLE RANGES: 0..1023).
*
* OUTPUTS:
*       entryIndexPtr  - (pointer to)index of Queue Id Map Bit Select Table Entry
*                        being mapped to the given target device.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapTargetDeviceToIndexMapGet
(
    IN  GT_U8           devNum,
    IN  GT_HW_DEV_NUM   targetHwDevId,
    OUT GT_U32          *entryIndexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueQueueMapTargetDeviceToIndexMapGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, targetHwDevId, entryIndexPtr));

    rc = internal_cpssDxChTmGlueQueueMapTargetDeviceToIndexMapGet(devNum, targetHwDevId, entryIndexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, targetHwDevId, entryIndexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTmGlueQueueMapCpuCodeToTcMapSet
*
* DESCRIPTION:
*   The function sets the mapped TM Traffic Class for CPU Code.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       cpuCode        - CPU code
*                        CPSS_NET_ALL_CPU_OPCODES_E - to configure all table entries.
*       tmTc           - mapped TM Traffic Class.
*                        (APPLICABLE RANGES: 0..15).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on out-of-range parameters
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTmGlueQueueMapCpuCodeToTcMapSet
(
    IN  GT_U8                      devNum,
    IN  CPSS_NET_RX_CPU_CODE_ENT   cpuCode,
    IN  GT_U32                     tmTc
)
{
    GT_STATUS rc;             /* return code                      */
    GT_U32    hwCpuCodeLow;   /* low bound of HW CPU codes range  */
    GT_U32    hwCpuCodeHigh;  /* high bound of HW CPU codes range */
    GT_U32    i;              /* loop index                       */
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT dsaCpuCode; /*to use for conversion*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT3_E  );

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if (tmTc > 15)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (cpuCode == CPSS_NET_ALL_CPU_OPCODES_E)
    {
        hwCpuCodeLow  = 0;
        hwCpuCodeHigh = 255;
    }
    else
    {
        rc = prvCpssDxChNetIfCpuToDsaCode(cpuCode, &dsaCpuCode);
        if (rc != GT_OK)
        {
            return rc;
        }
        hwCpuCodeLow  = dsaCpuCode;
        hwCpuCodeHigh = dsaCpuCode;
    }

    for (i = hwCpuCodeLow; (i <= hwCpuCodeHigh); i++)
    {
        rc = prvCpssDxChWriteTableEntry(
            devNum, PRV_CPSS_DXCH_LION3_TABLE_TM_QMAP_CPU_CODE_TO_TC_E,
            i/*entryIndex*/, &tmTc/*entryData*/);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapCpuCodeToTcMapSet
*
* DESCRIPTION:
*   The function sets the mapped TM Traffic Class for CPU Code.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       cpuCode        - CPU code
*                        CPSS_NET_ALL_CPU_OPCODES_E - to configure all table entries.
*       tmTc           - mapped TM Traffic Class.
*                        (APPLICABLE RANGES: 0..15).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on out-of-range parameters
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapCpuCodeToTcMapSet
(
    IN  GT_U8                      devNum,
    IN  CPSS_NET_RX_CPU_CODE_ENT   cpuCode,
    IN  GT_U32                     tmTc
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueQueueMapCpuCodeToTcMapSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cpuCode, tmTc));

    rc = internal_cpssDxChTmGlueQueueMapCpuCodeToTcMapSet(devNum, cpuCode, tmTc);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cpuCode, tmTc));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTmGlueQueueMapCpuCodeToTcMapGet
*
* DESCRIPTION:
*   The function gets the mapped TM Traffic Class for CPU Code.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       cpuCode        - CPU code
*
* OUTPUTS:
*       tmTcPtr        - (pointer to) mapped TM Traffic Class.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTmGlueQueueMapCpuCodeToTcMapGet
(
    IN  GT_U8                      devNum,
    IN  CPSS_NET_RX_CPU_CODE_ENT   cpuCode,
    OUT GT_U32                     *tmTcPtr
)
{
    GT_STATUS rc;             /* return code                      */
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT dsaCpuCode; /*to use for conversion*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT3_E  );

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    CPSS_NULL_PTR_CHECK_MAC(tmTcPtr);

    rc = prvCpssDxChNetIfCpuToDsaCode(cpuCode, &dsaCpuCode);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChReadTableEntry(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_TM_QMAP_CPU_CODE_TO_TC_E,
        dsaCpuCode/*entryIndex*/, tmTcPtr/*entryData*/);

    return rc;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapCpuCodeToTcMapGet
*
* DESCRIPTION:
*   The function gets the mapped TM Traffic Class for CPU Code.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       cpuCode        - CPU code
*
* OUTPUTS:
*       tmTcPtr        - (pointer to) mapped TM Traffic Class.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapCpuCodeToTcMapGet
(
    IN  GT_U8                      devNum,
    IN  CPSS_NET_RX_CPU_CODE_ENT   cpuCode,
    OUT GT_U32                     *tmTcPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueQueueMapCpuCodeToTcMapGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cpuCode, tmTcPtr));

    rc = internal_cpssDxChTmGlueQueueMapCpuCodeToTcMapGet(devNum, cpuCode, tmTcPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cpuCode, tmTcPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChTmGlueQueueMapTcToTcMapRegisterIndexGet
*
* DESCRIPTION:
*   The function gets register index for mapping of Traffic Class To TM Traffic Class.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       packetDsaCommand  - packet command.
*                           Forward, From CPU and Mirror only valid
*       isUnicast         - GT_TRUE - unicast, GT_FALSE - multicast
*
* OUTPUTS:
*       indexPtr          - (pointer to)register index.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChTmGlueQueueMapTcToTcMapRegisterIndexGet
(
    IN  CPSS_DXCH_NET_DSA_CMD_ENT   packetDsaCommand,
    IN  GT_BOOL                     isUnicast,
    OUT GT_U32                      *indexPtr
)
{
    GT_U32 cmdIndex; /* command depended index multiplier */

    switch (packetDsaCommand)
    {
        case CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E:
            cmdIndex = 0;
            break;
        case CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E:
            cmdIndex = 1;
            break;
        case CPSS_DXCH_NET_DSA_CMD_FORWARD_E:
            cmdIndex = 2;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    *indexPtr = (cmdIndex << 1) | BOOL2BIT_MAC(isUnicast);

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChTmGlueQueueMapTcToTcMapSet
*
* DESCRIPTION:
*   The function sets mapping of Traffic Class To TM Traffic Class.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum            - device number
*       packetDsaCommand  - packet Outgoing DSA Tag command.
*                           Forward, From CPU and Mirror only valid
*       isUnicast         - GT_TRUE - unicast, GT_FALSE - multicast
*       tc                - Traffic Class
*                           (APPLICABLE RANGES: 0..7).
*       tmTc              - Traffic Manager Traffic Class
*                           (APPLICABLE RANGES: 0..15).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on out-of-range parameters
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTmGlueQueueMapTcToTcMapSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_DXCH_NET_DSA_CMD_ENT   packetDsaCommand,
    IN  GT_BOOL                     isUnicast,
    IN  GT_U32                      tc,
    IN  GT_U32                      tmTc
)
{
    GT_STATUS rc;             /* return code      */
    GT_U32    regAddr;        /* register address */
    GT_U32    regIdx;         /* register index   */
    GT_U32    offset;         /* field offset     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT3_E  );

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = prvCpssDxChTmGlueQueueMapTcToTcMapRegisterIndexGet(
        packetDsaCommand, isUnicast, &regIdx);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (tc > 7)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (tmTc > 15)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    offset = tc * 4;

    regAddr = PRV_DXCH_REG1_UNIT_TM_QMAP_MAC(devNum).TC2TCMap8To16[regIdx];

    rc = prvCpssHwPpSetRegField(
        devNum, regAddr, offset, 4/*fieldLength*/, tmTc);

    return rc;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapTcToTcMapSet
*
* DESCRIPTION:
*   The function sets mapping of Traffic Class To TM Traffic Class.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum            - device number
*       packetDsaCommand  - packet Outgoing DSA Tag command.
*                           Forward, From CPU and Mirror only valid
*       isUnicast         - GT_TRUE - unicast, GT_FALSE - multicast
*       tc                - Traffic Class
*                           (APPLICABLE RANGES: 0..7).
*       tmTc              - Traffic Manager Traffic Class
*                           (APPLICABLE RANGES: 0..15).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on out-of-range parameters
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapTcToTcMapSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_DXCH_NET_DSA_CMD_ENT   packetDsaCommand,
    IN  GT_BOOL                     isUnicast,
    IN  GT_U32                      tc,
    IN  GT_U32                      tmTc
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueQueueMapTcToTcMapSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, packetDsaCommand, isUnicast, tc, tmTc));

    rc = internal_cpssDxChTmGlueQueueMapTcToTcMapSet(devNum, packetDsaCommand, isUnicast, tc, tmTc);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, packetDsaCommand, isUnicast, tc, tmTc));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTmGlueQueueMapTcToTcMapGet
*
* DESCRIPTION:
*   The function gets mapping of Traffic Class To TM Traffic Class.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum            - device number
*       packetDsaCommand  - packet Outgoing DSA Tag command.
*                           Forward, From CPU and Mirror only valid
*       isUnicast         - GT_TRUE - unicast, GT_FALSE - multicast
*       tc                - Traffic Class
*                           (APPLICABLE RANGES: 0..7).
*
* OUTPUTS:
*       tmTcPtr        - (pointer to)Traffic Manager Traffic Class
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTmGlueQueueMapTcToTcMapGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_DXCH_NET_DSA_CMD_ENT   packetDsaCommand,
    IN  GT_BOOL                     isUnicast,
    IN  GT_U32                      tc,
    OUT GT_U32                      *tmTcPtr
)
{
    GT_STATUS rc;             /* return code      */
    GT_U32    regAddr;        /* register address */
    GT_U32    regIdx;         /* register index   */
    GT_U32    offset;         /* field offset     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT3_E  );

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = prvCpssDxChTmGlueQueueMapTcToTcMapRegisterIndexGet(
        packetDsaCommand, isUnicast, &regIdx);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (tc > 7)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    CPSS_NULL_PTR_CHECK_MAC(tmTcPtr);

    offset = tc * 4;

    regAddr = PRV_DXCH_REG1_UNIT_TM_QMAP_MAC(devNum).TC2TCMap8To16[regIdx];

    rc = prvCpssHwPpGetRegField(
        devNum, regAddr, offset, 4/*fieldLength*/, tmTcPtr);

    return rc;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapTcToTcMapGet
*
* DESCRIPTION:
*   The function gets mapping of Traffic Class To TM Traffic Class.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum            - device number
*       packetDsaCommand  - packet Outgoing DSA Tag command.
*                           Forward, From CPU and Mirror only valid
*       isUnicast         - GT_TRUE - unicast, GT_FALSE - multicast
*       tc                - Traffic Class
*                           (APPLICABLE RANGES: 0..7).
*
* OUTPUTS:
*       tmTcPtr        - (pointer to)Traffic Manager Traffic Class
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapTcToTcMapGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_DXCH_NET_DSA_CMD_ENT   packetDsaCommand,
    IN  GT_BOOL                     isUnicast,
    IN  GT_U32                      tc,
    OUT GT_U32                      *tmTcPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueQueueMapTcToTcMapGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, packetDsaCommand, isUnicast, tc, tmTcPtr));

    rc = internal_cpssDxChTmGlueQueueMapTcToTcMapGet(devNum, packetDsaCommand, isUnicast, tc, tmTcPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, packetDsaCommand, isUnicast, tc, tmTcPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTmGlueQueueMapEgressPolicerForceSet
*
* DESCRIPTION:
*   The function sets force state for TM queue Id based policing.
*   If force the previous policer settings of the packet overridden.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       queueId        - Traffic Manager Queue Id
*                        (APPLICABLE RANGES: 0..0x3FFF).
*       forceMeter     - force meter,
*                        GT_TRUE - force, GT_FALSE - don't force.
*       forceCounting  - force counting
*                        GT_TRUE - force, GT_FALSE - don't force.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTmGlueQueueMapEgressPolicerForceSet
(
    IN  GT_U8    devNum,
    IN  GT_U32   queueId,
    IN  GT_BOOL  forceMeter,
    IN  GT_BOOL  forceCounting
)
{
    GT_STATUS rc;             /* return code    */
    GT_U32    tabIdx;         /* table index    */
    GT_U32    offset;         /* field offset   */
    GT_U32    hwData;         /* HW data        */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT3_E  );

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if (queueId >= 0x4000)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    tabIdx = queueId / 16;
    offset = 2 * (queueId % 16);
    hwData = (BOOL2BIT_MAC(forceMeter) << 1) | BOOL2BIT_MAC(forceCounting);

    rc = prvCpssDxChWriteTableEntryField(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_TM_QMAP_POLICING_ENABLE_E,
        tabIdx, 0/*fieldWordNum*/, offset, 2/*fieldLength*/, hwData);

    return rc;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapEgressPolicerForceSet
*
* DESCRIPTION:
*   The function sets force state for TM queue Id based policing.
*   If force the previous policer settings of the packet overridden.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       queueId        - Traffic Manager Queue Id
*                        (APPLICABLE RANGES: 0..0x3FFF).
*       forceMeter     - force meter,
*                        GT_TRUE - force, GT_FALSE - don't force.
*       forceCounting  - force counting
*                        GT_TRUE - force, GT_FALSE - don't force.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapEgressPolicerForceSet
(
    IN  GT_U8    devNum,
    IN  GT_U32   queueId,
    IN  GT_BOOL  forceMeter,
    IN  GT_BOOL  forceCounting
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueQueueMapEgressPolicerForceSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, queueId, forceMeter, forceCounting));

    rc = internal_cpssDxChTmGlueQueueMapEgressPolicerForceSet(devNum, queueId, forceMeter, forceCounting);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, queueId, forceMeter, forceCounting));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTmGlueQueueMapEgressPolicerForceGet
*
* DESCRIPTION:
*   The function gets force state for TM queue Id based policing.
*   If force the previous policer settings of the packet overridden.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       queueId        - Traffic Manager Queue Id
*                        (APPLICABLE RANGES: 0..0x3FFF).
*
* OUTPUTS:
*       forceMeterPtr     - (pointer to)force meter
*                        GT_TRUE - force, GT_FALSE - don't force.
*       forceCountingPtr  - (pointer to)force counting
*                        GT_TRUE - force, GT_FALSE - don't force.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTmGlueQueueMapEgressPolicerForceGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   queueId,
    OUT GT_BOOL  *forceMeterPtr,
    OUT GT_BOOL  *forceCountingPtr
)
{
    GT_STATUS rc;             /* return code    */
    GT_U32    tabIdx;         /* table index    */
    GT_U32    offset;         /* field offset   */
    GT_U32    hwData;         /* HW data        */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT3_E  );

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if (queueId >= 0x4000)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    CPSS_NULL_PTR_CHECK_MAC(forceMeterPtr);
    CPSS_NULL_PTR_CHECK_MAC(forceCountingPtr);

    tabIdx = queueId / 16;
    offset = 2 * (queueId % 16);

    rc = prvCpssDxChReadTableEntryField(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_TM_QMAP_POLICING_ENABLE_E,
        tabIdx, 0/*fieldWordNum*/, offset, 2/*fieldLength*/, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *forceCountingPtr   = BIT2BOOL_MAC((hwData & 1));
    *forceMeterPtr      = BIT2BOOL_MAC(((hwData >> 1) & 1));

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapEgressPolicerForceGet
*
* DESCRIPTION:
*   The function gets force state for TM queue Id based policing.
*   If force the previous policer settings of the packet overridden.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       queueId        - Traffic Manager Queue Id
*                        (APPLICABLE RANGES: 0..0x3FFF).
*
* OUTPUTS:
*       forceMeterPtr     - (pointer to)force meter
*                        GT_TRUE - force, GT_FALSE - don't force.
*       forceCountingPtr  - (pointer to)force counting
*                        GT_TRUE - force, GT_FALSE - don't force.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapEgressPolicerForceGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   queueId,
    OUT GT_BOOL  *forceMeterPtr,
    OUT GT_BOOL  *forceCountingPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueQueueMapEgressPolicerForceGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, queueId, forceMeterPtr, forceCountingPtr));

    rc = internal_cpssDxChTmGlueQueueMapEgressPolicerForceGet(devNum, queueId, forceMeterPtr, forceCountingPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, queueId, forceMeterPtr, forceCountingPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTmGlueQueueMapToCpuModeSet
*
* DESCRIPTION:
*   The function sets mode of index’s calculation in Queue Id Bit Select Table
*   for TO CPU packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum             - device number
*       toCpuMode          - mode of index’s calculation in
*                            Queue Id Bit Select Table for TO CPU packets.
*       toCpuSelectorIndex - index in Queue Id Bit Select Table
*                            for TO CPU packets.
*                            Always stored to HW, but used only when
*                            toCpuMode is "Use TO_CPU Queue Bit Selector configuration".
*                            (APPLICABLE RANGES: 0..0xFF).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on out-of-range parameters
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTmGlueQueueMapToCpuModeSet
(
    IN  GT_U8                                         devNum,
    IN  CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_ENT   toCpuMode,
    IN  GT_U32                                        toCpuSelectorIndex
)
{
    GT_STATUS rc;      /* return code */
    GT_U32    regAddr; /* register address */
    GT_U32    mode;    /* HW mode */
    GT_U32    data;    /* HW data */
    GT_U32    mask;    /* HW mask */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT3_E  );

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if (toCpuSelectorIndex >= BIT_8)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    switch (toCpuMode)
    {
        case CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_REGULAR_E:
            mode = 0;
            break;
        case CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_USE_TO_CPU_CONFIG_E:
            mode = 1;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    data = mode | (toCpuSelectorIndex << 17);
    mask = 1 | (0xFF << 17);

    regAddr = PRV_DXCH_REG1_UNIT_TM_QMAP_MAC(devNum).TMQMAPGlobalConfig;

    rc = prvCpssHwPpWriteRegBitMask(
        devNum, regAddr, mask, data);

    return rc;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapToCpuModeSet
*
* DESCRIPTION:
*   The function sets mode of index’s calculation in Queue Id Bit Select Table
*   for TO CPU packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum             - device number
*       toCpuMode          - mode of index’s calculation in
*                            Queue Id Bit Select Table for TO CPU packets.
*       toCpuSelectorIndex - index in Queue Id Bit Select Table
*                            for TO CPU packets.
*                            Always stored to HW, but used only when
*                            toCpuMode is "Use TO_CPU Queue Bit Selector configuration".
*                            (APPLICABLE RANGES: 0..0xFF).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on out-of-range parameters
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapToCpuModeSet
(
    IN  GT_U8                                         devNum,
    IN  CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_ENT   toCpuMode,
    IN  GT_U32                                        toCpuSelectorIndex
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueQueueMapToCpuModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, toCpuMode, toCpuSelectorIndex));

    rc = internal_cpssDxChTmGlueQueueMapToCpuModeSet(devNum, toCpuMode, toCpuSelectorIndex);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, toCpuMode, toCpuSelectorIndex));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTmGlueQueueMapToCpuModeGet
*
* DESCRIPTION:
*   The function gets mode of index’s calculation in Queue Id Bit Select Table
*   for TO CPU packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum             - device number
*
* OUTPUTS:
*       toCpuModePtr          - (pointer to)mode of index’s calculation in
*                               Queue Id Bit Select Table for TO CPU packets.
*       toCpuSelectorIndexPtr - (pointer to)index in Queue Id Bit Select Table
*                               for TO CPU packets.
*                               Always retrieved from HW, but used only when
*                               toCpuMode is "Use TO_CPU Queue Bit Selector configuration".
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTmGlueQueueMapToCpuModeGet
(
    IN  GT_U8                                         devNum,
    OUT CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_ENT   *toCpuModePtr,
    OUT GT_U32                                        *toCpuSelectorIndexPtr
)
{
    GT_STATUS rc;      /* return code */
    GT_U32    regAddr; /* register address */
    GT_U32    data;    /* HW data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT3_E  );

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    CPSS_NULL_PTR_CHECK_MAC(toCpuModePtr);
    CPSS_NULL_PTR_CHECK_MAC(toCpuSelectorIndexPtr);

    regAddr = PRV_DXCH_REG1_UNIT_TM_QMAP_MAC(devNum).TMQMAPGlobalConfig;

    rc = prvCpssHwPpReadRegister(
        devNum, regAddr, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    *toCpuModePtr =  ((data & 1) == 0)
        ? CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_REGULAR_E
        : CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_USE_TO_CPU_CONFIG_E;

    *toCpuSelectorIndexPtr = ((data >> 17) & 0xFF);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapToCpuModeGet
*
* DESCRIPTION:
*   The function gets mode of index’s calculation in Queue Id Bit Select Table
*   for TO CPU packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum             - device number
*
* OUTPUTS:
*       toCpuModePtr          - (pointer to)mode of index’s calculation in
*                               Queue Id Bit Select Table for TO CPU packets.
*       toCpuSelectorIndexPtr - (pointer to)index in Queue Id Bit Select Table
*                               for TO CPU packets.
*                               Always retrieved from HW, but used only when
*                               toCpuMode is "Use TO_CPU Queue Bit Selector configuration".
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapToCpuModeGet
(
    IN  GT_U8                                         devNum,
    OUT CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_ENT   *toCpuModePtr,
    OUT GT_U32                                        *toCpuSelectorIndexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueQueueMapToCpuModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, toCpuModePtr, toCpuSelectorIndexPtr));

    rc = internal_cpssDxChTmGlueQueueMapToCpuModeGet(devNum, toCpuModePtr, toCpuSelectorIndexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, toCpuModePtr, toCpuSelectorIndexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTmGlueQueueMapMcModeSet
*
* DESCRIPTION:
*   The function sets mode of index’s calculation in Queue Id Bit Select Table
*   for Multicast packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       mcMode          - mode of index’s calculation in
*                         Queue Id Bit Select Table for Multicast packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTmGlueQueueMapMcModeSet
(
    IN  GT_U8                                     devNum,
    IN  CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_ENT   mcMode
)
{
    GT_STATUS rc;      /* return code */
    GT_U32    regAddr; /* register address */
    GT_U32    data;    /* HW data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT3_E  );

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    switch (mcMode)
    {
        case CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_REGULAR_E:
            data = 0;
            break;
        case CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_FORCE_EPCL_CONFIG_E:
            data = 1;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_TM_QMAP_MAC(devNum).TMQMAPGlobalConfig;

    rc = prvCpssHwPpSetRegField(
        devNum, regAddr, 1/*fieldOffset*/, 1/*fieldLength*/, data);

    return rc;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapMcModeSet
*
* DESCRIPTION:
*   The function sets mode of index’s calculation in Queue Id Bit Select Table
*   for Multicast packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       mcMode          - mode of index’s calculation in
*                         Queue Id Bit Select Table for Multicast packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapMcModeSet
(
    IN  GT_U8                                     devNum,
    IN  CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_ENT   mcMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueQueueMapMcModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mcMode));

    rc = internal_cpssDxChTmGlueQueueMapMcModeSet(devNum, mcMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mcMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTmGlueQueueMapMcModeGet
*
* DESCRIPTION:
*   The function gets mode of index’s calculation in Queue Id Bit Select Table
*   for Multicast packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       mcModePtr       - (pointer to)mode of index’s calculation in
*                         Queue Id Bit Select Table for Multicast packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTmGlueQueueMapMcModeGet
(
    IN  GT_U8                                     devNum,
    OUT CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_ENT   *mcModePtr
)
{
    GT_STATUS rc;      /* return code */
    GT_U32    regAddr; /* register address */
    GT_U32    data;    /* HW data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT3_E  );

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    CPSS_NULL_PTR_CHECK_MAC(mcModePtr);

    regAddr = PRV_DXCH_REG1_UNIT_TM_QMAP_MAC(devNum).TMQMAPGlobalConfig;

    rc = prvCpssHwPpGetRegField(
        devNum, regAddr, 1/*fieldOffset*/, 1/*fieldLength*/, &data);
    if (rc != 0)
    {
        return rc;
    }

    switch (data)
    {
        case 0:
            *mcModePtr = CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_REGULAR_E;
            break;
        case 1:
            *mcModePtr = CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_FORCE_EPCL_CONFIG_E;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG); /*never occurs*/
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapMcModeGet
*
* DESCRIPTION:
*   The function gets mode of index’s calculation in Queue Id Bit Select Table
*   for Multicast packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       mcModePtr       - (pointer to)mode of index’s calculation in
*                         Queue Id Bit Select Table for Multicast packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapMcModeGet
(
    IN  GT_U8                                     devNum,
    OUT CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_ENT   *mcModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueQueueMapMcModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mcModePtr));

    rc = internal_cpssDxChTmGlueQueueMapMcModeGet(devNum, mcModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mcModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTmGlueQueueMapSelectorTableAccessModeSet
*
* DESCRIPTION:
*   The function sets Bit Selector Table Access Mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum  - device number
*       mode    - Bit Selector Table Access Mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTmGlueQueueMapSelectorTableAccessModeSet
(
    IN  GT_U8                                                 devNum,
    IN  CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_ENT  mode
)
{
    GT_STATUS rc;      /* return code */
    GT_U32    regAddr; /* register address */
    GT_U32    data;    /* HW data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT3_E  );

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    switch (mode)
    {
        case CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_LOCAL_TARGET_PORT_E:
            data = 0;
            break;
        case CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_TARGET_DEV_MAP_E:
            data = 1;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_TM_QMAP_MAC(devNum).TMQMAPGlobalConfig;

    rc = prvCpssHwPpSetRegField(
        devNum, regAddr, 2/*fieldOffset*/, 1/*fieldLength*/, data);

    return rc;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapSelectorTableAccessModeSet
*
* DESCRIPTION:
*   The function sets Bit Selector Table Access Mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum  - device number
*       mode    - Bit Selector Table Access Mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapSelectorTableAccessModeSet
(
    IN  GT_U8                                                 devNum,
    IN  CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_ENT  mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueQueueMapSelectorTableAccessModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mode));

    rc = internal_cpssDxChTmGlueQueueMapSelectorTableAccessModeSet(devNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTmGlueQueueMapSelectorTableAccessModeGet
*
* DESCRIPTION:
*   The function gets Bit Selector Table Access Mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - (pointer to)Bit Selector Table Access Mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTmGlueQueueMapSelectorTableAccessModeGet
(
    IN  GT_U8                                                 devNum,
    OUT CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_ENT  *modePtr
)
{
    GT_STATUS rc;      /* return code */
    GT_U32    regAddr; /* register address */
    GT_U32    data;    /* HW data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT3_E  );

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    regAddr = PRV_DXCH_REG1_UNIT_TM_QMAP_MAC(devNum).TMQMAPGlobalConfig;

    rc = prvCpssHwPpGetRegField(
        devNum, regAddr, 2/*fieldOffset*/, 1/*fieldLength*/, &data);
    if (rc != 0)
    {
        return rc;
    }

    switch (data)
    {
        case 0:
            *modePtr =
                CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_LOCAL_TARGET_PORT_E;
            break;
        case 1:
            *modePtr =
                CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_TARGET_DEV_MAP_E;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG); /*never occurs*/
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapSelectorTableAccessModeGet
*
* DESCRIPTION:
*   The function gets Bit Selector Table Access Mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - (pointer to)Bit Selector Table Access Mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapSelectorTableAccessModeGet
(
    IN  GT_U8                                                 devNum,
    OUT CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_ENT  *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueQueueMapSelectorTableAccessModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, modePtr));

    rc = internal_cpssDxChTmGlueQueueMapSelectorTableAccessModeGet(devNum, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTmGlueQueueMapL1PacketLengthOffsetSet
*
* DESCRIPTION:
*   The function sets Egress L1 Packet Length Offset Configuration.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       physicalPort    - physical port number.
*       lengthOffsetPtr - (pointer to)Egress L1 Packet Length Offset Configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on out-of-range parameters
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTmGlueQueueMapL1PacketLengthOffsetSet
(
    IN  GT_U8                                                    devNum,
    IN  GT_PHYSICAL_PORT_NUM                                     physicalPort,
    IN  CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_STC  *lengthOffsetPtr
)
{
    GT_STATUS rc;             /* return code    */
    GT_U32    tabIdx;         /* table index    */
    GT_U32    offset;         /* field offset   */
    GT_U32    hwData;         /* HW data        */
    GT_U32    hwCmd;          /* HW command     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT3_E  );

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    PRV_CPSS_DXCH_ENHANCED_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, physicalPort);
    CPSS_NULL_PTR_CHECK_MAC(lengthOffsetPtr);

    if (lengthOffsetPtr->offsetValue >= BIT_7)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    switch (lengthOffsetPtr->offsetCmd)
    {
        case CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_CMD_ADD_E:
            hwCmd = 0;
            break;
        case CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_CMD_SUBTRACT_E:
            hwCmd = 1;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    tabIdx = physicalPort / 4;
    offset = (physicalPort % 4) * 8;
    hwData = (hwCmd << 7) | lengthOffsetPtr->offsetValue;

    rc = prvCpssDxChWriteTableEntryField(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_TM_INGR_GLUE_L1_PKT_LEN_OFFSET_E,
        tabIdx, 0/*fieldWordNum*/, offset, 8/*fieldLength*/, hwData);

    return rc;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapL1PacketLengthOffsetSet
*
* DESCRIPTION:
*   The function sets Egress L1 Packet Length Offset Configuration.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       physicalPort    - physical port number.
*       lengthOffsetPtr - (pointer to)Egress L1 Packet Length Offset Configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on out-of-range parameters
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapL1PacketLengthOffsetSet
(
    IN  GT_U8                                                    devNum,
    IN  GT_PHYSICAL_PORT_NUM                                     physicalPort,
    IN  CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_STC  *lengthOffsetPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueQueueMapL1PacketLengthOffsetSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, physicalPort, lengthOffsetPtr));

    rc = internal_cpssDxChTmGlueQueueMapL1PacketLengthOffsetSet(devNum, physicalPort, lengthOffsetPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, physicalPort, lengthOffsetPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTmGlueQueueMapL1PacketLengthOffsetGet
*
* DESCRIPTION:
*   The function gets Egress L1 Packet Length Offset Configuration.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       physicalPort    - physical port number.
*
* OUTPUTS:
*       lengthOffsetPtr - (pointer to)Egress L1 Packet Length Offset Configuration.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTmGlueQueueMapL1PacketLengthOffsetGet
(
    IN  GT_U8                                                    devNum,
    IN  GT_PHYSICAL_PORT_NUM                                     physicalPort,
    OUT CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_STC  *lengthOffsetPtr
)
{
    GT_STATUS rc;             /* return code    */
    GT_U32    tabIdx;         /* table index    */
    GT_U32    offset;         /* field offset   */
    GT_U32    hwData;         /* HW data        */
    GT_U32    hwCmd;          /* HW command     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT3_E  );

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    PRV_CPSS_DXCH_ENHANCED_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, physicalPort);
    CPSS_NULL_PTR_CHECK_MAC(lengthOffsetPtr);

    tabIdx = physicalPort / 4;
    offset = (physicalPort % 4) * 8;

    rc = prvCpssDxChReadTableEntryField(
        devNum, PRV_CPSS_DXCH_LION3_TABLE_TM_INGR_GLUE_L1_PKT_LEN_OFFSET_E,
        tabIdx, 0/*fieldWordNum*/, offset, 8/*fieldLength*/, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    hwCmd                        = ((hwData >> 7) & 1);
    lengthOffsetPtr->offsetValue = (hwData & 0x7F);

    switch (hwCmd)
    {
        case 0:
            lengthOffsetPtr->offsetCmd =
                CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_CMD_ADD_E;
            break;
        case 1:
            lengthOffsetPtr->offsetCmd =
                CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_CMD_SUBTRACT_E;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG); /* never occurs */
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapL1PacketLengthOffsetGet
*
* DESCRIPTION:
*   The function gets Egress L1 Packet Length Offset Configuration.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       physicalPort    - physical port number.
*
* OUTPUTS:
*       lengthOffsetPtr - (pointer to)Egress L1 Packet Length Offset Configuration.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueQueueMapL1PacketLengthOffsetGet
(
    IN  GT_U8                                                    devNum,
    IN  GT_PHYSICAL_PORT_NUM                                     physicalPort,
    OUT CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_STC  *lengthOffsetPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueQueueMapL1PacketLengthOffsetGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, physicalPort, lengthOffsetPtr));

    rc = internal_cpssDxChTmGlueQueueMapL1PacketLengthOffsetGet(devNum, physicalPort, lengthOffsetPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, physicalPort, lengthOffsetPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}







