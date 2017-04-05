/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxCh3Policer.c
*
* DESCRIPTION:
*       wrappers for cpssDxCh3Policer.c
*
* FILE REVISION NUMBER:
*       $Revision: 35 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxCh3/policer/cpssDxCh3Policer.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfoEnhanced.h>

/* Policer stage number: Ingress #0, Ingress #1 or Egress */
static CPSS_DXCH_POLICER_STAGE_TYPE_ENT plrStage = CPSS_DXCH_POLICER_STAGE_INGRESS_0_E;

/* Save configured metering resolution to be used in eArch metering entry set */
/* while using legacy APIs - used for Ingress #0, Ingress #1 and Egress.      */
static CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT plrMeterResolution[3] =
                                  {CPSS_DXCH3_POLICER_METER_RESOLUTION_BYTES_E,
                                   CPSS_DXCH3_POLICER_METER_RESOLUTION_BYTES_E,
                                   CPSS_DXCH3_POLICER_METER_RESOLUTION_BYTES_E};

/* support for multi port groups */

/*******************************************************************************
* policerMultiPortGroupsBmpGet
*
* DESCRIPTION:
*       Get the portGroupsBmp for multi port groups device.
*       when 'enabled' --> wrappers will use the APIs
*       with portGroupsBmp parameter
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       enablePtr - (pointer to)enable / disable the use of APIs with portGroupsBmp parameter.
*       portGroupsBmpPtr - (pointer to)port groups bmp , relevant only when enable = GT_TRUE
*
* RETURNS:
*       NONE
*
* COMMENTS:
*
*******************************************************************************/
static void policerMultiPortGroupsBmpGet
(
    IN   GT_U8               devNum,
    OUT  GT_BOOL             *enablePtr,
    OUT  GT_PORT_GROUPS_BMP  *portGroupsBmpPtr
)
{
    /* default */
    *enablePtr  = GT_FALSE;
    *portGroupsBmpPtr = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

    if (0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        return;
    }

    utilMultiPortGroupsBmpGet(devNum, enablePtr, portGroupsBmpPtr);
}

/* Port Group aqnd Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxCh3PolicerMeteringEntryRefresh
(
    IN GT_U8                          devNum,
    IN GT_U32                         entryIndex
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    policerMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return  cpssDxCh3PolicerMeteringEntryRefresh(devNum, plrStage, entryIndex);
    }
    else
    {
        return  cpssDxChPolicerPortGroupMeteringEntryRefresh(devNum, pgBmp,
                                                              plrStage,
                                                              entryIndex);
    }
}

/* Port Group aqnd Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxCh3PolicerErrorGet
(
    IN  GT_U8                               devNum,
    OUT CPSS_DXCH3_POLICER_ENTRY_TYPE_ENT   *entryTypePtr,
    OUT GT_U32                              *entryAddrPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    policerMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return  cpssDxCh3PolicerErrorGet(devNum, plrStage, entryTypePtr,
                                         entryAddrPtr);
    }
    else
    {
        return  cpssDxChPolicerPortGroupErrorGet(devNum, pgBmp, plrStage,
                                                  entryTypePtr, entryAddrPtr);
    }
}


/* Port Group aqnd Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxCh3PolicerErrorCounterGet
(
    IN  GT_U8                               devNum,
    OUT GT_U32                              *cntrValuePtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    policerMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return  cpssDxCh3PolicerErrorCounterGet(devNum, plrStage, cntrValuePtr);
    }
    else
    {
        return  cpssDxChPolicerPortGroupErrorCounterGet(devNum, pgBmp, plrStage,
                                                         cntrValuePtr);
    }
}


/* Port Group aqnd Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxCh3PolicerManagementCountersSet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT      mngCntrSet,
    IN CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT     mngCntrType,
    IN CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC    *mngCntrPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    policerMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return  cpssDxCh3PolicerManagementCountersSet(devNum, plrStage,
                                                      mngCntrSet,
                                                      mngCntrType, mngCntrPtr);
    }
    else
    {
        return  cpssDxChPolicerPortGroupManagementCountersSet(devNum, pgBmp,
                                                               plrStage,
                                                               mngCntrSet,
                                                               mngCntrType,
                                                               mngCntrPtr);
    }
}


/* Port Group aqnd Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxCh3PolicerManagementCountersGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT     mngCntrSet,
    IN  CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT    mngCntrType,
    OUT CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC   *mngCntrPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    policerMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return  cpssDxCh3PolicerManagementCountersGet(devNum, plrStage,
                                                      mngCntrSet,
                                                      mngCntrType, mngCntrPtr);
    }
    else
    {
        return  cpssDxChPolicerPortGroupManagementCountersGet(devNum, pgBmp,
                                                               plrStage,
                                                               mngCntrSet,
                                                               mngCntrType,
                                                               mngCntrPtr);
    }
}

/* Port Group aqnd Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxCh3PolicerMeteringEntrySet
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  entryIndex,
    IN  CPSS_DXCH3_POLICER_METERING_ENTRY_STC   *entryPtr,
    OUT CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT  *tbParamsPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    policerMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return  cpssDxCh3PolicerMeteringEntrySet(devNum, plrStage, entryIndex,
                                                 entryPtr, tbParamsPtr);
    }
    else
    {
        return  cpssDxChPolicerPortGroupMeteringEntrySet(devNum, pgBmp,
                                                          plrStage, entryIndex,
                                                          entryPtr, tbParamsPtr);
    }
}

/* new eArch fields setting to avoid failures */
static GT_STATUS cpssDxCh3PolicerMeteringEntryLegacyUpdates
(
    IN  GT_U8                                   devNum,
    INOUT CPSS_DXCH3_POLICER_METERING_ENTRY_STC *entryPtr
)
{
    entryPtr->byteOrPacketCountingMode = plrMeterResolution[plrStage];

    if(plrStage )
    {
        if(entryPtr->modifyExp == CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E)
        {
            entryPtr->modifyExp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
        }

        if(entryPtr->modifyTc == CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E)
        {
            entryPtr->modifyTc = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
        }
    }

    return cpssDxCh3PolicerPacketSizeModeGet(devNum,
                                             plrStage,
                                             &entryPtr->packetSizeMode);
}


/* Port Group aqnd Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxCh3PolicerMeteringEntryGet
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  entryIndex,
    OUT CPSS_DXCH3_POLICER_METERING_ENTRY_STC   *entryPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    policerMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return  cpssDxCh3PolicerMeteringEntryGet(devNum, plrStage, entryIndex,
                                                 entryPtr);
    }
    else
    {
        return  cpssDxChPolicerPortGroupMeteringEntryGet(devNum, pgBmp,
                                                          plrStage, entryIndex,
                                                          entryPtr);
    }
}


/* Port Group aqnd Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxCh3PolicerBillingEntrySet
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  entryIndex,
    IN  CPSS_DXCH3_POLICER_BILLING_ENTRY_STC    *billingCntrPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    policerMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return  cpssDxCh3PolicerBillingEntrySet(devNum, plrStage, entryIndex,
                                                billingCntrPtr);
    }
    else
    {
        return  cpssDxChPolicerPortGroupBillingEntrySet(devNum, pgBmp,
                                                         plrStage, entryIndex,
                                                         billingCntrPtr);
    }
}


/* Port Group aqnd Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxCh3PolicerBillingEntryGet
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  entryIndex,
    IN  GT_BOOL                                 reset,
    OUT CPSS_DXCH3_POLICER_BILLING_ENTRY_STC    *billingCntrPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    policerMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return  cpssDxCh3PolicerBillingEntryGet(devNum, plrStage, entryIndex,
                                                reset, billingCntrPtr);
    }
    else
    {
        return  cpssDxChPolicerPortGroupBillingEntryGet(devNum, pgBmp,
                                                         plrStage, entryIndex,
                                                         reset, billingCntrPtr);
    }
}


/* Port Group aqnd Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChPolicerPolicyCntrSet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              index,
    IN  GT_U32                              cntrValue
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    policerMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return  cpssDxChPolicerPolicyCntrSet(devNum, plrStage, index,
                                             cntrValue);
    }
    else
    {
        return  cpssDxChPolicerPortGroupPolicyCntrSet(devNum, pgBmp, plrStage,
                                                      index, cntrValue);
    }
}


/* Port Group aqnd Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChPolicerPolicyCntrGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              index,
    OUT GT_U32                              *cntrValuePtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    policerMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return  cpssDxChPolicerPolicyCntrGet(devNum, plrStage, index,
                                             cntrValuePtr);
    }
    else
    {
        return  cpssDxChPolicerPortGroupPolicyCntrGet(devNum, pgBmp, plrStage,
                                                      index, cntrValuePtr);
    }
}

/* Port Group aqnd Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChPolicerBucketsCurrentStateGet
(
    IN   GT_U8                                       devNum,
    IN   CPSS_DXCH_POLICER_STAGE_TYPE_ENT            stage,
    IN   GT_U32                                      policerIndex,
    OUT  CPSS_DXCH_POLICER_BUCKETS_STATE_STC        *bucketsStatePtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    policerMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return  cpssDxChPolicerBucketsCurrentStateGet(
            devNum, stage, policerIndex, bucketsStatePtr);
    }
    else
    {
        return  cpssDxChPolicerPortGroupBucketsCurrentStateGet(
            devNum, pgBmp, stage, policerIndex, bucketsStatePtr);
    }
}


/*******************************************************************************
* cpssDxCh3PolicerMeteringEnableSet
*
* DESCRIPTION:
*       Enables or disables metering per device.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*       enable          - Enable/disable metering:
*                         GT_TRUE  - metering is enabled on the device.
*                         GT_FALSE - metering is disabled on the device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*        If metering is enabled, it can be triggered
*        either by Policy engine or per port.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerMeteringEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxCh3PolicerMeteringEnableSet(devNum, plrStage, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerMeteringEnableGet
*
* DESCRIPTION:
*       Gets device metering status (Enable/Disable).
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       enablePtr       - pointer to Enable/Disable metering:
*                         GT_TRUE  - metering is enabled on the device.
*                         GT_FALSE - metering is disabled on the device.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*        If metering is enabled, it can be triggered
*        either by Policy engine or per port.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerMeteringEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxCh3PolicerMeteringEnableGet(devNum, plrStage, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerBillingCountingEnableSet
*
* DESCRIPTION:
*       Enables or disables Billing Counting.
*       Billing is the process of counting the amount of traffic that
*       belongs to a flow and can be activated only by metering.
*       When metering is activated per source port, Billing counting
*       cannot be enabled.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*       enable          - enable/disable Billing Counting:
*                         GT_TRUE  - Billing Counting is enabled on the device.
*                         GT_FALSE - Billing Counting is disabled on the device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerBillingCountingEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxCh3PolicerBillingCountingEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerBillingCountingEnableGet
*
* DESCRIPTION:
*       Gets device Billing Counting status (Enable/Disable).
*       Billing is the process of counting the amount of traffic that
*       belongs to a flow and can be activated only by metering.
*       When metering is activated per source port, Billing counting
*       cannot be enabled.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       enablePtr       - pointer to Enable/Disable Billing Counting:
*                         GT_TRUE  - Billing Counting is enabled on the device.
*                         GT_FALSE - Billing Counting is disabled on the device.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerBillingCountingEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxCh3PolicerBillingCountingEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerBillingCountersModeSet
*
* DESCRIPTION:
*      Sets the Billing Counters resolution.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*       cntrMode        - Billing Counters resolution: 1 Byte,
*                         16 Byte or packet.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum or cntrMode.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerBillingCountersModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT cntrMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    cntrMode = (CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxCh3PolicerBillingCountersModeSet(devNum, cntrMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerBillingCountersModeGet
*
* DESCRIPTION:
*      Gets the Billing Counters resolution.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       cntrModePtr     - pointer to the Billing Counters resolution:
*                         1 Byte, 16 Byte or packet.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerBillingCountersModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT cntrMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxCh3PolicerBillingCountersModeGet(devNum, &cntrMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cntrMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerPacketSizeModeSet
*
* DESCRIPTION:
*       Sets metered Packet Size Mode that metering and billing is done
*       according to.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum             - physical device number.
*       packetSize         - Type of packet size:
*                               - L3 datagram size only (this does not include
*                                 the L2 header size and packets CRC).
*                               - L2 packet length including.
*                               - L1 packet length including
*                                 (preamble + IFG + FCS).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_BAD_PARAM        - on on wrong devNum or packetSize.
*       GT_HW_ERROR         - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerPacketSizeModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_POLICER_PACKET_SIZE_MODE_ENT packetSize;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    packetSize = (CPSS_POLICER_PACKET_SIZE_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxCh3PolicerPacketSizeModeSet(devNum, plrStage, packetSize);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerPacketSizeModeGet
*
* DESCRIPTION:
*       Gets metered Packet Size Mode that metering and billing is done
*       according to.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       packetSizePtr   - pointer to the Type of packet size:
*                           - L3 datagram size only (this does not include
*                             the L2 header size and packets CRC).
*                           - L2 packet length including.
*                           - L1 packet length including (preamble + IFG + FCS).
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerPacketSizeModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_POLICER_PACKET_SIZE_MODE_ENT packetSize;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxCh3PolicerPacketSizeModeGet(devNum, plrStage, &packetSize);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", packetSize);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerMeterResolutionSet
*
* DESCRIPTION:
*       Sets metering algorithm resolution: packets per
*       second or bytes per second.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*       resolution      - packet/Byte based Meter resolution.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum or resolution.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerMeterResolutionSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT resolution;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    resolution = (CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxCh3PolicerMeterResolutionSet(devNum, plrStage, resolution);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    plrMeterResolution[plrStage] = resolution;

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerMeterResolutionGet
*
* DESCRIPTION:
*       Gets metering algorithm resolution: packets per
*       second or bytes per second.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       resolutionPtr   - pointer to the Meter resolution: packet or Byte based.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerMeterResolutionGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT resolution;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxCh3PolicerMeterResolutionGet(devNum, plrStage, &resolution);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", resolution);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerDropTypeSet
*
* DESCRIPTION:
*       Sets the Policer out-of-profile drop command type.
*       This setting controls if non-conforming dropped packets
*       (Red or Yellow) will be SOFT DROP or HARD DROP.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*       dropType        - Policer Drop Red Type: Soft or Hard.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum or dropType.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerDropTypeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DROP_MODE_TYPE_ENT dropType;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    dropType = (CPSS_DROP_MODE_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxCh3PolicerDropTypeSet(devNum, plrStage, dropType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerDropTypeGet
*
* DESCRIPTION:
*       Gets the Policer out-of-profile drop command type.
*       This setting controls if non-conforming dropped packets
*       (Red or Yellow) will be SOFT DROP or HARD DROP.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       dropTypePtr     - pointer to the Policer Drop Red Type: Soft or Hard.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerDropTypeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DROP_MODE_TYPE_ENT dropType;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxCh3PolicerDropTypeGet(devNum, plrStage, &dropType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dropType);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerCountingColorModeSet
*
* DESCRIPTION:
*       Sets the Policer color counting mode.
*       The Color Counting can be done according to the packet's
*       Drop Precedence or Conformance Level.
*       This affects both the Billing and Management counters.
*       If the packet was subject to remarking, the drop precedence used
*       here is AFTER remarking.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*       mode            - Color counting mode: Drop Precedence or
*                         Conformance Level.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum or mode.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerCountingColorModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxCh3PolicerCountingColorModeSet(devNum, plrStage, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerCountingColorModeGet
*
* DESCRIPTION:
*       Gets the Policer color counting mode.
*       The Color Counting can be done according to the packet's
*       Drop Precedence or Conformance Level.
*       This affects both the Billing and Management counters.
*       If the packet was subject to remarking, the drop precedence used
*       here is AFTER remarking.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       modePtr         - pointer to the color counting mode:
*                         Drop Precedence or Conformance Level.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerCountingColorModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH3_POLICER_COLOR_COUNT_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxCh3PolicerCountingColorModeGet(devNum, plrStage, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerManagementCntrsResolutionSet
*
* DESCRIPTION:
*       Sets the Management counters resolution (either 1 Byte or 16 Bytes).
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*       cntrSet         - Management Counters Set [0..2].
*       cntrResolution  - Management Counters resolution: 1 or 16 Bytes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum, Management Counters Set or
*                         cntrResolution.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerManagementCntrsResolutionSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT cntrSet;
    CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT cntrResolution;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    cntrSet = (CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT)inArgs[1];
    cntrResolution = (CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxCh3PolicerManagementCntrsResolutionSet(devNum, plrStage,
                                                          cntrSet,
                                                          cntrResolution);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerManagementCntrsResolutionGet
*
* DESCRIPTION:
*       Gets the Management counters resolution (either 1 Byte or 16 Bytes).
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum              - physical device number.
*       cntrSet             - Management Counters Set [0..2].
*
* OUTPUTS:
*       cntrResolutionPtr   - pointer to the Management Counters
*                             resolution: 1 or 16 Bytes.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_BAD_PARAM        - on wrong devNum or Management Counters Set
*       GT_HW_ERROR         - on Hardware error.
*       GT_BAD_PTR          - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerManagementCntrsResolutionGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT cntrSet;
    CPSS_DXCH3_POLICER_MNG_CNTR_RESOLUTION_ENT cntrResolution;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    cntrSet = (CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxCh3PolicerManagementCntrsResolutionGet(devNum, plrStage,
                                                          cntrSet,
                                                          &cntrResolution);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cntrResolution);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerPacketSizeModeForTunnelTermSet
*
* DESCRIPTION:
*      Sets size mode for metering and counting of tunnel terminated packets.
*      The Policer provides the following modes to define packet size:
*       - Regular packet metering and counting. The packet size is defined
*         by the cpssDxCh3PolicerPacketSizeModeSet().
*       - Passenger packet metering and counting.
*         Metering and counting of TT packets is performed according
*         to L3 datagram size only. This mode does not include the tunnel
*         header size, the L2 header size, and the packet CRC in the metering
*         and counting.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum              - physical device number.
*       ttPacketSizeMode    - Tunnel Termination Packet Size Mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK               - on success.
*       GT_HW_ERROR         - on Hardware error.
*       GT_BAD_PARAM        - on wrong devNum or ttPacketSizeMode.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerPacketSizeModeForTunnelTermSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT ttPacketSizeMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    ttPacketSizeMode = (CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxCh3PolicerPacketSizeModeForTunnelTermSet(devNum, plrStage,
                                                            ttPacketSizeMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerPacketSizeModeForTunnelTermGet
*
* DESCRIPTION:
*      Gets size mode for metering and counting of tunnel terminated packets.
*      The Policer provides the following modes to define packet size:
*       - Regular packet metering and counting. The packet size is defined
*         by the cpssDxCh3PolicerPacketSizeModeSet().
*       - Passenger packet metering and counting.
*         Metering and counting of TT packets is performed according
*         to L3 datagram size only. This mode does not include the tunnel
*         header size, the L2 header size, and the packet CRC in the metering
*         and counting.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum                  - physical device number.
*
* OUTPUTS:
*       ttPacketSizeModePtr     - pointer to the Tunnel Termination
*                                 Packet Size Mode.
*
* RETURNS:
*       GT_OK                   - on success.
*       GT_HW_ERROR             - on hardware error
*       GT_BAD_PARAM            - on wrong devNum.
*       GT_BAD_PTR              - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerPacketSizeModeForTunnelTermGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT ttPacketSizeMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxCh3PolicerPacketSizeModeForTunnelTermGet(devNum, plrStage,
                                                            &ttPacketSizeMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ttPacketSizeMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerMeteringAutoRefreshScanEnableSet
*
* DESCRIPTION:
*       Enables or disables the metering Auto Refresh Scan mechanism.
*
*       If no traffic passes through a meter (long periods of "silence") the
*       meter state variables do not get refreshed. This can cause a problem of
*       state misinterpreted once the flow traffic is restarted, because the
*       timers can have already wrapped around.
*       To avoid this problem the software must initiate a refresh transaction
*       on the meter every ten minutes.
*
*       This CPU intensive metering refresh maintanance can be offloaded by
*       Auto Refresh Scan mechanism.
*       The range of Auto Refresh Scan is configurable by the
*       cpssDxCh3PolicerMeteringAutoRefreshRangeSet().
*       The Auto Refresh Scan Rate can be controlled by setting time interval
*       between two refresh access to metering table. The time interval is
*       configured by the cpssDxCh3PolicerMeteringAutoRefreshIntervalSet().
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*       enable          - Enable/disable Auto Refresh Scan mechanism:
*                         GT_TRUE  - enable Auto Refresh scan mechanism on
*                                    the device.
*                         GT_FALSE - disable Auto Refresh scan mechanism on
*                                    the device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       The CPU is still capable to perform any Refresh Transactions during
*       the Auto Refresh Scan operation.
*       Once the CPU triggers Refresh transaction, the policer accesses the
*       metering entry and performs metering refresh with the packet's Byte
*       Count set to zero.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerMeteringAutoRefreshScanEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxCh3PolicerMeteringAutoRefreshScanEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerMeteringAutoRefreshScanEnableGet
*
* DESCRIPTION:
*       Gets metering Auto Refresh Scan mechanism status (Enabled/Disabled).
*
*       If no traffic passes through a meter (long periods of "silence") the
*       meter state variables do not get refreshed. This can cause a problem of
*       state misinterpreted once the flow traffic is restarted, because the
*       timers can have already wrapped around.
*       To avoid this problem the software must initiate a refresh transaction
*       on the meter every ten minutes.
*
*       This CPU intensive metering refresh maintanance can be offloaded by
*       Auto Refresh Scan mechanism.
*       The range of Auto Refresh Scan is configurable by the
*       cpssDxCh3PolicerMeteringAutoRefreshRangeSet().
*       The Auto Refresh Scan Rate can be controlled by setting time interval
*       between two refresh access to metering table. The time interval is
*       configured by the cpssDxCh3PolicerMeteringAutoRefreshIntervalSet().
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       enablePtr       - pointer on Auto Refresh Scan mechanism status
*                         (Enable/Disable) :
*                         GT_TRUE  - Auto Refresh scan mechanism is enabled.
*                         GT_FALSE - Auto Refresh scan mechanism is disabled.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       The CPU is still capable to perform any Refresh Transactions during
*       the Auto Refresh Scan operation.
*       Once the CPU triggers Refresh transaction, the policer accesses the
*       metering entry and performs metering refresh with the packet's Byte
*       Count set to zero.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerMeteringAutoRefreshScanEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxCh3PolicerMeteringAutoRefreshScanEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerMeteringAutoRefreshRangeSet
*
* DESCRIPTION:
*       Sets Metering Refresh Scan address range (Start and Stop addresses).
*       The Auto Refresh Scan mechanism is enabled by the
*       cpssDxCh3PolicerMeteringAutoRefreshScanEnableSet().
*       The Auto Refresh Scan Rate can be controlled by setting time interval
*       between two refresh access to metering table. The time interval is
*       configured by the cpssDxCh3PolicerMeteringAutoRefreshIntervalSet().
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum              - physical device number.
*       startAddress        - beginning of the address range to be scanned
*                             by the Auto Refresh Scan mechanism [0..1023].
*       stopAddress         - end of the address range to be scanned by the
*                             Auto Refresh Scan mechanism [0..1023].
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_BAD_PARAM        - on wrong devNum.
*       GT_HW_ERROR         - on Hardware error.
*       GT_OUT_OF_RANGE     - on out of range of startAddress or stopAddress.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerMeteringAutoRefreshRangeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 startAddress;
    GT_U32 stopAddress;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    startAddress = (GT_U32)inArgs[1];
    stopAddress = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxCh3PolicerMeteringAutoRefreshRangeSet(devNum, startAddress, stopAddress);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerMeteringAutoRefreshRangeGet
*
* DESCRIPTION:
*       Gets Metering Refresh Scan address range (Start and Stop addresses).
*       The Auto Refresh Scan mechanism is enabled by the
*       cpssDxCh3PolicerMeteringAutoRefreshScanEnableSet().
*       The Auto Refresh Scan Rate can be controlled by setting time interval
*       between two refresh access to metering table. The time interval is
*       configured by the cpssDxCh3PolicerMeteringAutoRefreshIntervalSet().
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum              - physical device number.
*
* OUTPUTS:
*       startAddressPtr     - pointer to the beginning address of Refresh
*                             Scan address range.
*       stopAddressPtr      - pointer to the end address of Refresh Scan
*                             address range.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_BAD_PARAM        - on wrong devNum.
*       GT_HW_ERROR         - on Hardware error.
*       GT_BAD_PTR          - on NULL pointers.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerMeteringAutoRefreshRangeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 startAddress;
    GT_U32 stopAddress;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxCh3PolicerMeteringAutoRefreshRangeGet(devNum, &startAddress, &stopAddress);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", startAddress, stopAddress);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerMeteringAutoRefreshIntervalSet
*
* DESCRIPTION:
*       Sets the time interval between two refresh access to metering table.
*       The Auto Refresh Scan mechanism is enabled by the
*       cpssDxCh3PolicerMeteringAutoRefreshScanEnableSet().
*       The range of Auto Refresh Scan is configurable by the
*       cpssDxCh3PolicerMeteringAutoRefreshRangeSet().
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*       interval        - time interval between refresh of two entries
*                         in nano seconds, range [0..15907286277 nS]
*                         (upon 270 Mhz core clock value).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_HW_ERROR     - on Hardware error.
*       GT_OUT_OF_RANGE - on out of range of interval value.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerMeteringAutoRefreshIntervalSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 interval;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    interval = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxCh3PolicerMeteringAutoRefreshIntervalSet(devNum, interval);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerMeteringAutoRefreshIntervalGet
*
* DESCRIPTION:
*       Gets the time interval between two refresh access to metering table.
*       The Auto Refresh Scan mechanism is enabled by the
*       cpssDxCh3PolicerMeteringAutoRefreshScanEnableSet().
*       The range of Auto Refresh Scan is configurable by the
*       cpssDxCh3PolicerMeteringAutoRefreshRangeSet().
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       intervalPtr     - pointer to the time interval between refresh of two
*                         entries in nano seconds, range [0..15907286277 nS]
*                         (upon 270 Mhz core clock value).
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerMeteringAutoRefreshIntervalGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 interval;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxCh3PolicerMeteringAutoRefreshIntervalGet(devNum, &interval);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", interval);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerMeteringEntryRefresh
*
* DESCRIPTION:
*       Refresh the Policer Metering Entry.
*       Meter Bucket's state parameters refreshing are needed in order to
*       prevent mis-behavior due to wrap around of timers.
*
*       The wrap around problem can occur when there are long periods of
*       'silence' on the flow and the bucket's state parameters do not get
*       refreshed (meter is in the idle state). This causes a problem once the
*       flow traffic is re-started since the timers might have already wrapped
*       around which can cause a situation in which the bucket's state is
*       mis-interpreted and the incoming packet is marked as non-conforming
*       even though the bucket was actually supposed to be full.
*       To prevent this from happening the CPU needs to trigger a meter
*       refresh transaction at least once every 10 minutes per meter.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - device number.
*       entryIndex      - index of Policer Metering Entry.
*                         Range: see datasheet for specific device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or entryIndex.
*       GT_TIMEOUT      - on time out of Policer Tables non-direct access.
*
* COMMENTS:
*       Once the CPU triggers Refresh transaction the policer accesses the
*       metering entry and performs metering refresh with the packet's Byte
*       Count set to zero.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerMeteringEntryRefresh
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 entryIndex;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    entryIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxCh3PolicerMeteringEntryRefresh(devNum, entryIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerPortMeteringEnableSet
*
* DESCRIPTION:
*       Enables or disables a port metering trigger for packets
*       arriving on this port.
*       When feature is enabled the meter entry index is a port number.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number (including the CPU port).
*       enable          - Enable/Disable per-port metering for packets arriving
*                         on this port:
*                         GT_TRUE  - Metering is triggered on specified port.
*                         GT_FALSE - Metering isn't triggered on specified port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum or portNum.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerPortMeteringEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM portNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxCh3PolicerPortMeteringEnableSet(devNum, plrStage, portNum,
                                                   enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerPortMeteringEnableGet
*
* DESCRIPTION:
*       Gets port status (Enable/Disable) of metering for packets
*       arriving on this port.
*       When feature is enabled the meter entry index is a port number.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number (including the CPU port).
*
* OUTPUTS:
*       enablePtr       - pointer on per-port metering status (Enable/Disable)
*                         for packets arriving on specified port:
*                         GT_TRUE  - Metering is triggered on specified port.
*                         GT_FALSE - Metering isn't triggered on specified port.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum or portNum.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerPortMeteringEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM portNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxCh3PolicerPortMeteringEnableGet(devNum, plrStage, portNum,
                                                   &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerMruSet
*
* DESCRIPTION:
*       Sets the Policer Maximum Receive Unit size.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*       mruSize         - Policer MRU value in bytes, range of values [0..64K].
*                         This value is used in the metering algorithm.
*                         When the number of bytes in the bucket is lower than
*                         this value a packet is marked as non conforming.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_OUT_OF_RANGE - on mruSize out of range of [0..65535 Bytes].
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerMruSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 mruSize;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mruSize = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxCh3PolicerMruSet(devNum, plrStage, mruSize);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerMruGet
*
* DESCRIPTION:
*       Gets the Policer Maximum Receive Unit size.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       mruSizePtr      - pointer to the Policer MRU value in bytes,
*                         range of values [0..64K].
*                         This value is used in the metering algorithm.
*                         When the number of bytes in the bucket is lower than
*                         this value a packet is marked as non conforming.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerMruGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 mruSize;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxCh3PolicerMruGet(devNum, plrStage, &mruSize);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mruSize);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerErrorGet
*
* DESCRIPTION:
*       Gets address and type of Policer Entry that had an ECC error.
*       This information available if error is happened and not read till now.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       entryTypePtr    - pointer to the Type of Entry (Metering or Counting)
*                         that had an error.
*       entryAddrPtr    - pointer to the Policer Entry that had an error.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_EMPTY        - on missing error information.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerErrorGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH3_POLICER_ENTRY_TYPE_ENT entryType;
    GT_U32 entryAddr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = pg_wrap_cpssDxCh3PolicerErrorGet(devNum, &entryType, &entryAddr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", entryType, entryAddr);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerErrorCounterGet
*
* DESCRIPTION:
*       Gets the value of the Policer ECC Error Counter.
*       The Error Counter is a free-running non-sticky 8-bit read-only
*       counter.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       cntrValuePtr    - pointer to the Policer ECC Error counter value.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerErrorCounterGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 cntrValue;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = pg_wrap_cpssDxCh3PolicerErrorCounterGet(devNum, &cntrValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cntrValue);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerManagementCountersSet
*
* DESCRIPTION:
*       Sets the value of specified Management Counters.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*       mngCntrSet      - Management Counters Set[0..2].
*       mngCntrType     - Management Counters Type (GREEN, YELLOW, RED, DROP).
*       mngCntrPtr      - pointer to the Management Counters Entry must be set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum or mngCntrType or Management
*                         Counters Set number.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerManagementCountersSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT mngCntrSet;
    CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT mngCntrType;
    CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC mngCntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

   devNum = (GT_U8)inArgs[0];
   mngCntrSet=(CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT)inArgs[1];
   mngCntrType=(CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT)inArgs[2];
   mngCntr.duMngCntr.l[0]=(GT_U32)inArgs[3];
   mngCntr.packetMngCntr=(GT_U32)inArgs[4];

    /* call cpss api function */
    result = pg_wrap_cpssDxCh3PolicerManagementCountersSet(devNum, mngCntrSet,
                                                           mngCntrType,
                                                           &mngCntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerManagementCountersGet
*
* DESCRIPTION:
*       Gets the value of specified Management Counters.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*       mngCntrSet      - Management Counters Set[0..2].
*       mngCntrType     - Management Counters Type (GREEN, YELLOW, RED, DROP).
*
* OUTPUTS:
*       mngCntrPtr      - pointer to the requested Management Counters Entry.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum or mngCntrType or Management
*                         Counters Set number.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerManagementCountersGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT mngCntrSet;
    CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT mngCntrType;
    CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC mngCntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

     devNum = (GT_U8)inArgs[0];
     mngCntrSet=(CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT)inArgs[1];
     mngCntrType=(CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT)inArgs[2];

    /* call cpss api function */
    result = pg_wrap_cpssDxCh3PolicerManagementCountersGet(devNum, mngCntrSet,
                                                           mngCntrType, &mngCntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d",mngCntr.duMngCntr.l[0],mngCntr.packetMngCntr);

    return CMD_OK;
}
 /**************Table:cpssDxCh3PolicerMeteringEntry********************************/
static  GT_U32 minMeteringEntryIndex;
static  GT_U32 maxMeteringEntryIndex;
static  GT_U32 meteringEntryIndex;
/*******************************************************************************
* cpssDxCh3PolicerMeteringEntrySet
*
* DESCRIPTION:
*      Sets Metering Policer Entry.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - device number.
*       entryIndex      - index of Policer Metering Entry [0..1023].
*       entryPtr        - pointer to the metering policer entry going to be set.
*
* OUTPUTS:
*       tbParamsPtr     - pointer to actual policer token bucket parameters.
*                         The token bucket parameters are returned as output
*                         values. This is due to the hardware rate resolution,
*                         the exact rate or burst size requested may not be
*                         honored. The returned value gives the user the
*                         actual parameters configured in the hardware.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - wrong devNum, entryIndex or entry parameters.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_TIMEOUT      - on time out of Policer Tables non-direct access.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerMeteringEntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 entryIndex;
    CPSS_DXCH3_POLICER_METERING_ENTRY_STC entry;
    CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT tbParams;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(&entry, 0, sizeof(entry));

    /* map input arguments to locals */

    devNum =(GT_U8)inArgs[0];

    entryIndex=(GT_U32)inFields[0];
    entry.countingEntryIndex=(GT_U32)inFields[1];
    entry.mngCounterSet=(CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT)inFields[2];
    entry.meterColorMode=(CPSS_POLICER_COLOR_MODE_ENT)inFields[3];
    entry.meterMode=(CPSS_DXCH3_POLICER_METER_MODE_ENT)inFields[4];

    if(entry.meterMode==CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E)
        {
          entry.tokenBucketParams.srTcmParams.cir=(GT_U32)inFields[5];
          entry.tokenBucketParams.srTcmParams.cbs=(GT_U32)inFields[6];
          entry.tokenBucketParams.srTcmParams.ebs=(GT_U32)inFields[7];
        }
    else
        if(entry.meterMode==CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E)
        {
          entry.tokenBucketParams.trTcmParams.cir=(GT_U32)inFields[8];
          entry.tokenBucketParams.trTcmParams.cbs=(GT_U32)inFields[9];
          entry.tokenBucketParams.trTcmParams.pir=(GT_U32)inFields[10];
          entry.tokenBucketParams.trTcmParams.pbs=(GT_U32)inFields[11];
        }

    entry.modifyUp=(CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[12];
    entry.modifyDscp=(CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[13];
    entry.yellowPcktCmd=(CPSS_DXCH3_POLICER_NON_CONFORM_CMD_ENT)inFields[14];
    entry.redPcktCmd=(CPSS_DXCH3_POLICER_NON_CONFORM_CMD_ENT)inFields[15];

    result = cpssDxCh3PolicerMeteringEntryLegacyUpdates(devNum, &entry);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = pg_wrap_cpssDxCh3PolicerMeteringEntrySet(devNum, entryIndex,
                                                      &entry, &tbParams);

    /* pack output arguments to galtis string */


    if(entry.meterMode==CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E)
        {
          galtisOutput(outArgs, result, "%d%d%d",tbParams.srTcmParams.cir,tbParams.srTcmParams.cbs,tbParams.srTcmParams.ebs);
        }
    else
        if(entry.meterMode==CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E)
        {
          galtisOutput(outArgs, result, "%d%d%d%d",tbParams.trTcmParams.cir,tbParams.trTcmParams.cbs,
                                               tbParams.trTcmParams.pir,tbParams.trTcmParams.pbs);
        }

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerMeteringEntryGet
*
* DESCRIPTION:
*      Gets Metering Policer Entry configuration.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - device number.
*       entryIndex      - index of Policer Metering Entry [0..1023].
*
* OUTPUTS:
*       entryPtr        - pointer to the requested metering policer entry.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - wrong devNum or entryIndex.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerMeteringEntryGetEntry
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;

    CPSS_DXCH3_POLICER_METERING_ENTRY_STC entry;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

     if (meteringEntryIndex>maxMeteringEntryIndex||meteringEntryIndex>1023)
    {
        galtisOutput(outArgs, CMD_OK, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */

    devNum =(GT_U8)inArgs[0];


    /* call cpss api function */
    result = pg_wrap_cpssDxCh3PolicerMeteringEntryGet(devNum, meteringEntryIndex,
                                                      &entry);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    cmdOsMemSet(inFields,0,CMD_MAX_FIELDS);

    inFields[0]=meteringEntryIndex;
    inFields[1]=entry.countingEntryIndex;
    inFields[2]=entry.mngCounterSet;
    inFields[3]=entry.meterColorMode;
    inFields[4]=entry.meterMode;

    if(entry.meterMode==CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E)
        {
          inFields[5]=entry.tokenBucketParams.srTcmParams.cir;
          inFields[6]=entry.tokenBucketParams.srTcmParams.cbs;
          inFields[7]=entry.tokenBucketParams.srTcmParams.ebs;
        }
    else
        if(entry.meterMode==CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E)
        {
          inFields[8]=entry.tokenBucketParams.trTcmParams.cir;
          inFields[9]=entry.tokenBucketParams.trTcmParams.cbs;
          inFields[10]=entry.tokenBucketParams.trTcmParams.pir;
          inFields[11]=entry.tokenBucketParams.trTcmParams.pbs;
        }

    inFields[12]=entry.modifyUp;
    inFields[13]=entry.modifyDscp;
    inFields[14]=entry.yellowPcktCmd;
    inFields[15]=entry.redPcktCmd;


    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", inFields[0],inFields[1],inFields[2],inFields[3],
                                                   inFields[4],inFields[5],inFields[6],inFields[7],
                                                   inFields[8],inFields[9],inFields[10],inFields[11],
                                                  inFields[12],inFields[13],inFields[14],inFields[15]);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}
/*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerMeteringEntryGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    minMeteringEntryIndex=(GT_U32)inArgs[1];
    maxMeteringEntryIndex=(GT_U32)inArgs[2];

    meteringEntryIndex=minMeteringEntryIndex;

    return wrCpssDxCh3PolicerMeteringEntryGetEntry(inArgs,inFields,numFields,outArgs);

}
/*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerMeteringEntryGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    meteringEntryIndex++;

    return wrCpssDxCh3PolicerMeteringEntryGetEntry(inArgs,inFields,numFields,outArgs);
}
/*******************************************************************************
* cpssDxCh3PolicerEntryMeterParamsCalculate
*
* DESCRIPTION:
*      Calculates Token Bucket parameters in the Application format without
*      HW update.
*      The token bucket parameters are returned as output values. This is
*      due to the hardware rate resolution, the exact rate or burst size
*      requested may not be honored. The returned value gives the user the
*      actual parameters configured in the hardware.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - device number
*       tbInParamsPtr   - pointer to Token bucket input parameters.
*       meterMode       - Meter mode (SrTCM or TrTCM).
*
* OUTPUTS:
*       tbOutParamsPtr  - pointer to Token bucket output paramters.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong DevicedevNum.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerEntryMeterParamsCalculate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH3_POLICER_METER_MODE_ENT meterMode;
    CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT tbInParams;
    CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT tbOutParams;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

     devNum =(GT_U8)inArgs[0];
     meterMode=(CPSS_DXCH3_POLICER_METER_MODE_ENT)inArgs[1];

     if(meterMode==CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E)
     {
         tbInParams.srTcmParams.cir=(GT_U32)inArgs[2];
         tbInParams.srTcmParams.cbs=(GT_U32)inArgs[3];
         tbInParams.srTcmParams.ebs=(GT_U32)inArgs[4];
    }
    else if(meterMode==CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E)
    {
        tbInParams.trTcmParams.cir=(GT_U32)inArgs[5];
        tbInParams.trTcmParams.cbs=(GT_U32)inArgs[6];
        tbInParams.trTcmParams.pir=(GT_U32)inArgs[7];
        tbInParams.trTcmParams.pbs=(GT_U32)inArgs[8];
    }

    /* call cpss api function */
    result = cpssDxCh3PolicerEntryMeterParamsCalculate(devNum, plrStage, meterMode, &tbInParams, &tbOutParams);

    if(meterMode==CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d%d%d%d%d%d%d",
                     tbOutParams.srTcmParams.cir,tbOutParams.srTcmParams.cbs,
                     tbOutParams.srTcmParams.ebs,
                     0,0,0,0);
    }
    else if(meterMode==CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d%d%d%d%d%d%d",
                     0,0,0,
                     tbOutParams.trTcmParams.cir,tbOutParams.trTcmParams.cbs,
                     tbOutParams.trTcmParams.pir,tbOutParams.trTcmParams.pbs);
    }

    return CMD_OK;
}
/**************Table:cpssDxCh3PolicerBillingEntry********************************/
static  GT_U32 minBillingEntryIndex;
static  GT_U32 maxBillingEntryIndex;
static  GT_U32 billingEntryIndex;
/*******************************************************************************
* cpssDxCh3PolicerBillingEntrySet
*
* DESCRIPTION:
*       Sets Policer Billing Counters.
*       Billing is the process of counting the amount of traffic that belongs
*       to a flow. This process can be activated only by metering and is used
*       either for statistical analysis of a flow's traffic or for actual
*       billing.
*       The billing counters are free-running no-sticky counters.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - device number.
*       entryIndex      - index of Policer Billing Counters Entry [0..1023].
*       billingCntrPtr  - pointer to the Billing Counters Entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or entryIndex.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       The packet is counted per outgoing DP or Conformance Level. When
*       metering is disabled, billing is always done according to packet's
*       Drop Precedence.
*       When metering is activated per source port, Billing counting cannot
*       be enabled.
*       None-zeroed setting of Billing Counters while metring/counting is
*       enabled may cause data coherency problems.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerBillingEntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 entryIndex;
    CPSS_DXCH3_POLICER_BILLING_ENTRY_STC billingCntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    entryIndex=(GT_U32)inFields[0];
    billingCntr.greenCntr.l[0]=(GT_U32)inFields[1];
    billingCntr.yellowCntr.l[0]=(GT_U32)inFields[2];
    billingCntr.redCntr.l[0]=(GT_U32)inFields[3];
    /* default values */
    billingCntr.greenCntr.l[1] = 0;
    billingCntr.yellowCntr.l[1] = 0;
    billingCntr.redCntr.l[1] = 0;
    billingCntr.billingCntrMode = CPSS_DXCH3_POLICER_BILLING_CNTR_1_BYTE_E;
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        billingCntr.billingCntrAllEnable = GT_FALSE;
        billingCntr.lmCntrCaptureMode =
            CPSS_DXCH3_POLICER_LM_CNTR_CAPTURE_MODE_COPY_E;
        billingCntr.greenCntrSnapshot.l[0] = 0;
        billingCntr.greenCntrSnapshot.l[1] = 0;
        billingCntr.greenCntrSnapshotValid = GT_FALSE;
        billingCntr.packetSizeMode = CPSS_POLICER_PACKET_SIZE_L2_INCLUDE_E;
        billingCntr.tunnelTerminationPacketSizeMode =
            CPSS_DXCH3_POLICER_TT_PACKET_SIZE_REGULAR_E;
        billingCntr.dsaTagCountingMode =
            CPSS_DXCH3_POLICER_DSA_TAG_COUNTING_MODE_INCLUDED_E;
        billingCntr.timeStampCountingMode =
            CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_INCLUDE_E;
    }
    /* call cpss api function */
    result = pg_wrap_cpssDxCh3PolicerBillingEntrySet(devNum, entryIndex,
                                                     &billingCntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerBillingEntryGet
*
* DESCRIPTION:
*       Gets Policer Billing Counters.
*       Billing is the process of counting the amount of traffic that belongs
*       to a flow. This process can be activated only by metering and is used
*       either for statistical analysis of a flow's traffic or for actual
*       billing.
*       The billing counters are free-running no-sticky counters.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - device number.
*       entryIndex      - index of Policer Billing Counters Entry [0..1023].
*       reset           - reset flag:
*                         GT_TRUE  - performing read and reset atomic operation.
*                         GT_FALSE - performing read counters operation only.
*
* OUTPUTS:
*       billingCntrPtr  - pointer to the Billing Counters Entry.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or entryIndex.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_TIMEOUT      - on time out of Policer Tables non-direct access.
*
* COMMENTS:
*       The packet is counted per outgoing DP or Conformance Level. When
*       metering is disabled, billing is always done according to packet's
*       Drop Precedence.
*       When metering is activated per source port, Billing counting cannot
*       be enabled.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerBillingEntryGetEntry
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL reset;
    CPSS_DXCH3_POLICER_BILLING_ENTRY_STC billingCntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (billingEntryIndex>maxBillingEntryIndex||billingEntryIndex>1023)
    {
        galtisOutput(outArgs, CMD_OK, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    reset=(GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxCh3PolicerBillingEntryGet(devNum, billingEntryIndex,
                                                     reset, &billingCntr);

   if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0]=billingEntryIndex;
    inFields[1]=billingCntr.greenCntr.l[0];
    inFields[2]=billingCntr.yellowCntr.l[0];
    inFields[3]=billingCntr.redCntr.l[0];

    fieldOutput("%d%d%d%d", inFields[0],inFields[1],inFields[2],inFields[3]);
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}
/*********************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerBillingEntryGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    minBillingEntryIndex=(GT_U32)inArgs[2];
    maxBillingEntryIndex=(GT_U32)inArgs[3];
    billingEntryIndex=minBillingEntryIndex;

    return wrCpssDxCh3PolicerBillingEntryGetEntry(inArgs,inFields,numFields,outArgs);
}
/*********************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerBillingEntryGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
     billingEntryIndex++;

     return wrCpssDxCh3PolicerBillingEntryGetEntry(inArgs,inFields,numFields,outArgs);

}

/**************Table:cpssDxCh3PolicerQosRemarkingEntry********************************/
static  GT_U8 minQosRemarkingEntryIndex;
static  GT_U8 maxQosRemarkingEntryIndex;
static  GT_U8 qosRemarkingEntryIndex;
/*******************************************************************************
* cpssDxCh3PolicerQosRemarkingEntrySet
*
* DESCRIPTION:
*       Sets Policer Relative Qos Remarking Entry.
*       The Qos Remarking Entry contains two indexes [0..127] of Qos Profile
*       assigned to Out of profile packets with the Yellow and Red colors.
*       According to this indexes, new QoS parameters: UP,DP,TC and DSCP,
*       are extracted from the Qos Profile Table Entry.
*       Lion3 and Bobcat2 Qos Remarking Entry contains three indexes [0..1023] of Qos Profile
*       assigned to In-Profile Green colored and Out-Of-profile packets with the Yellow and Red colors.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum                      - device number.
*       stage                       - Policer Stage type: Ingress #0 or Ingress #1.
*                                     Stage type is significant for xCat and above devices
*                                     and ignored by DxCh3.
*       qosProfileIndex             - index of Qos Remarking Entry will be set.
*       greenQosTableRemarkIndex    - QoS profile (index in the Qos Table)
*                                     assigned to In-Profile packets with
*                                     the Green color. (APPLICABLE RANGES: 0..1023).
*                                     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.).
*       yellowQosTableRemarkIndex   - QoS profile (index in the Qos Table)
*                                     assigned to Out-Of-Profile packets with
*                                     the Yellow color. (APPLICABLE RANGES: 0..127).
*                                     Lion3 and Bobcat2 - (APPLICABLE RANGES: 0..1023).
*       redQosTableRemarkIndex      - QoS profile (index in the Qos Table)
*                                     assigned to Out-Of-Profile packets with
*                                     the Red color. (APPLICABLE RANGES: 0..127).
*                                     Lion3 and Bobcat2 - (APPLICABLE RANGES: 0..1023).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or qosProfileIndex.
*       GT_OUT_OF_RANGE             - on greenQosTableRemarkIndex,
*                                     yellowQosTableRemarkIndex and
*                                     redQosTableRemarkIndex out of range.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       Supported only for Ingress Policer.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerQosRemarkingEntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 qosProfileIndex;
    GT_U32 greenQosTableRemarkIndex;
    GT_U32 yellowQosTableRemarkIndex;
    GT_U32 redQosTableRemarkIndex;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    qosProfileIndex = (GT_U32)inFields[0];
    greenQosTableRemarkIndex = (GT_U32)inFields[1];
    yellowQosTableRemarkIndex = (GT_U32)inFields[2];
    redQosTableRemarkIndex = (GT_U32)inFields[3];

    /* call cpss api function */
    result = cpssDxCh3PolicerQosRemarkingEntrySet(
        devNum, plrStage, qosProfileIndex,
        greenQosTableRemarkIndex,
        yellowQosTableRemarkIndex,
        redQosTableRemarkIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerQosRemarkingEntryGet
*
* DESCRIPTION:
*       Gets Policer Relative Qos Remarking Entry.
*       The Qos Remarking Entry contains two indexes [0..127] of Qos Profile
*       assigned to Out of profile packets with the Yellow and Red colours.
*       According to this indexes, new QoS parameters: UP,DP,TC and DSCP,
*       are extracted from the Qos Profile Table Entry.
*       Lion3 and Bobcat2 Qos Remarking Entry contains three indexes [0..1023] of Qos Profile
*       assigned to In-Profile Green colored and Out-Of-profile packets with the Yellow and Red colors.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum              - device number.
*       stage               - Policer Stage type: Ingress #0 or Ingress #1.
*                             Stage type is significant for xCat and above devices
*                             and ignored by DxCh3.
*       qosProfileIndex     - index of requested Qos Remarking Entry.
*
* OUTPUTS:
*       greenQosTableRemarkIndexPtr     - pointer to the QoS profile (index in
*                                         the Qos Table) assigned to
*                                         In-Profile packets with the
*                                         Green color.
*                                         (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.).
*       yellowQosTableRemarkIndexPtr    - pointer to the QoS profile (index in
*                                         the Qos Table) assigned to
*                                         Out-Of-Profile packets with the
*                                         Yellow color.
*       redQosTableRemarkIndexPtr       - pointer to the QoS profile (index in
*                                         the Qos Table) assigned to
*                                         Out-Of-Profile packets with the Red
*                                         color.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum or qosProfileIndex.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       Supported only for Ingress Policer.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerQosRemarkingEntryGetEntry
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 greenQosTableRemarkIndex;
    GT_U32 yellowQosTableRemarkIndex;
    GT_U32 redQosTableRemarkIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (qosRemarkingEntryIndex > maxQosRemarkingEntryIndex)
    {
        galtisOutput(outArgs, CMD_OK, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxCh3PolicerQosRemarkingEntryGet(
        devNum, plrStage, qosRemarkingEntryIndex,
        &greenQosTableRemarkIndex,
        &yellowQosTableRemarkIndex,
        &redQosTableRemarkIndex);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    fieldOutput(
        "%d%d%d%d", qosRemarkingEntryIndex,
        greenQosTableRemarkIndex,
        yellowQosTableRemarkIndex,
        redQosTableRemarkIndex);
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/***************************************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerQosRemarkingEntryGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    minQosRemarkingEntryIndex=(GT_U8)inArgs[1];
    maxQosRemarkingEntryIndex=(GT_U8)inArgs[2];
    qosRemarkingEntryIndex=minQosRemarkingEntryIndex;

    return wrCpssDxCh3PolicerQosRemarkingEntryGetEntry(inArgs,inFields,numFields,outArgs);

}
/***************************************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerQosRemarkingEntryGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
   qosRemarkingEntryIndex++;
   return wrCpssDxCh3PolicerQosRemarkingEntryGetEntry(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssDxChPolicerCountingModeSet
*
* DESCRIPTION:
*       Configures counting mode.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant only for DxChXcat devices
*                         and ignored by DxCh3.
*       mode           -  counting mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or mode.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerCountingModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_POLICER_COUNTING_MODE_ENT mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_DXCH_POLICER_COUNTING_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPolicerCountingModeSet(devNum, plrStage, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPolicerCountingModeGet
*
* DESCRIPTION:
*       Gets the couning mode.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*                         Stage type is significant only for DxChXcat devices
*                         and ignored by DxCh3.
*
* OUTPUTS:
*       modePtr         - pointer to Counting mode.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerCountingModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    /* CPSS_DXCH_POLICER_STAGE_TYPE_ENT    stage; */
    CPSS_DXCH_POLICER_COUNTING_MODE_ENT mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPolicerCountingModeGet(devNum, plrStage, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);


    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerEgressQosRemarkingEntrySet
*
* DESCRIPTION:
*       Sets Egress Policer Re-Marking table Entry.
*       Re-marking is the process of assigning new QoS parameters to the
*       packet, at the end of the metering process, based on the
*       Conformance Level the packet is assigned by metering process.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum              - device number.
*       remarkTableType     - Remark table type: DSCP, EXP or TC/UP.
*       remarkParamValue    - QoS parameter value.
*       confLevel           - Conformance Level: Green, Yellow, Red.
*       qosParamPtr         - pointer to the Re-Marking Entry going to be set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_HW_ERROR         - on hardware error.
*       GT_BAD_PARAM        - on wrong devNum, remarkTableType,
*                                remarkParamValue or confLevel.
*                              -------------------
*                             |QoS param | Range  |
*                             |----------|--------|
*                             |  DSCP    | [0:63] |
*                             |  TC/UP   | [0:7]  |
*                             |  EXP     | [0:7]  |
*                              -------------------
*       GT_BAD_PTR                               - on NULL pointer.
*       GT_OUT_OF_RANGE     - on QoS parameter out of range
*       GT_NOT_APPLICABLE_DEVICE                 - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerEgressQosRemarkingEntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;
    GT_U8                                       devNum;
    CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_ENT     remarkTableType;
    GT_U32                                      remarkParamValue;
    CPSS_DP_LEVEL_ENT                           confLevel;
    CPSS_DXCH_POLICER_QOS_PARAM_STC             qosParam;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    remarkTableType = (CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_ENT)inArgs[1];
    remarkParamValue = (GT_U32)inArgs[2];
    confLevel = (CPSS_DP_LEVEL_ENT)inArgs[3];

    qosParam.up = (GT_U32)inFields[0];
    qosParam.dscp = (GT_U32)inFields[1];
    qosParam.exp = (GT_U32)inFields[2];
    qosParam.dp  = (CPSS_DP_LEVEL_ENT)inFields[3];

    /* call cpss api function */
    result = cpssDxChPolicerEgressQosRemarkingEntrySet(devNum,
                                                       remarkTableType,
                                                       remarkParamValue,
                                                       confLevel,
                                                       &qosParam);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerEgressQosRemarkingEntryGet
*
* DESCRIPTION:
*       Gets Egress Policer Re-Marking table Entry.
*       Re-marking is the process of assigning new QoS parameters to the
*       packet, at the end of the metering process, based on the
*       Conformance Level the packet is assigned by metering process.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum              - device number.
*       remarkTableType     - Remark table type: DSCP, EXP or TC/UP.
*       remarkParamValue    - QoS parameter value.
*       confLevel           - Conformance Level: Green, Yellow, Red.
*
* OUTPUTS:
*       qosParamPtr         - pointer to the requested Re-Marking Entry.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_HW_ERROR         - on hardware error.
*       GT_BAD_PARAM        - on wrong devNum, remarkTableType,
*                                remarkParamValue or confLevel.
*                              -------------------
*                             |QoS param | Range  |
*                             |----------|--------|
*                             |  DSCP    | [0:63] |
*                             |  TC/UP   | [0:7]  |
*                             |  EXP     | [0:7]  |
*                              -------------------
*       GT_BAD_PTR                               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE                 - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerEgressQosRemarkingEntryGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;
    GT_U8                                       devNum;
    CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_ENT     remarkTableType;
    GT_U32                                      remarkParamValue;
    CPSS_DP_LEVEL_ENT                           confLevel;
    CPSS_DXCH_POLICER_QOS_PARAM_STC             qosParam;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    remarkTableType = (CPSS_DXCH_POLICER_REMARK_TABLE_TYPE_ENT)inArgs[1];
    remarkParamValue = (GT_U32)inArgs[2];
    confLevel = (CPSS_DP_LEVEL_ENT)inArgs[3];


    /* call cpss api function */
    result = cpssDxChPolicerEgressQosRemarkingEntryGet(devNum,
                                                       remarkTableType,
                                                       remarkParamValue,
                                                       confLevel,
                                                       &qosParam);

    inFields[0] = qosParam.up;
    inFields[1] = qosParam.dscp;
    inFields[2] = qosParam.exp;
    inFields[3] = qosParam.dp;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1],
                              inFields[2], inFields[3]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPolicerStageMeterModeSet
*
* DESCRIPTION:
*       Sets Policer Global stage mode.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       mode            - Policer meter mode: FLOW or PORT.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or mode.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       In FLOW mode both the Policy and the port-trigger commands are
*       considered and in case of contention, the Policy command takes
*       precedence.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerStageMeterModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;
    GT_U8                                       devNum;
    CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT      mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPolicerStageMeterModeSet(devNum, plrStage, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerStageMeterModeGet
*
* DESCRIPTION:
*       Gets Policer Global stage mode.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       modePtr         - (pointer to) Policer meter mode: FLOW or PORT.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       In FLOW mode both the Policy and the port-trigger commands are
*       considered and in case of contention, the Policy command takes
*       precedence.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerStageMeterModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]

)
{
    GT_STATUS                                   result;
    GT_U8                                       devNum;
    CPSS_DXCH_POLICER_STAGE_METER_MODE_ENT      mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPolicerStageMeterModeGet(devNum, plrStage, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerMeteringOnTrappedPktsEnableSet
*
* DESCRIPTION:
*       Enable/Disable Ingress metering for Trapped packets.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0 or Ingress #1.
*       enable          - GT_TRUE - policing is executed on all packets (data
*                                   and control).
*                         GT_FALSE - policing is executed only on packets that
*                                    were asigned Forward (mirrored to analyzer
*                                    is inclusive) or Mirror to CPU commands by
*                                    previous processing stages.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or mode.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       The Ingress Policer considers data traffic as any packet that is not
*       trapped or dropped.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerMeteringOnTrappedPktsEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;
    GT_U8        devNum;
    GT_BOOL      enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPolicerMeteringOnTrappedPktsEnableSet(devNum, plrStage,
                                                           enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerMeteringOnTrappedPktsEnableGet
*
* DESCRIPTION:
*       Get Ingress metering status (Enabled/Disabled) for Trapped packets.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0 or Ingress #1.
*
* OUTPUTS:
*       enablePtr       - GT_TRUE - policing is executed on all packets (data
*                                   and control).
*                         GT_FALSE - policing is executed only on packets that
*                                    were asigned Forward (mirrored to analyzer
*                                    is inclusive) or Mirror to CPU commands by
*                                    previous processing stages.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       The Ingress Policer considers data traffic as any packet that is not
*       trapped or dropped.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerMeteringOnTrappedPktsEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;
    GT_U8        devNum;
    GT_BOOL      enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPolicerMeteringOnTrappedPktsEnableGet(devNum, plrStage,
                                                           &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerPortStormTypeIndexSet
*
* DESCRIPTION:
*       Associates policing profile with source/target port and storm rate type.
*       There are up to four policers per port and every storm type can be
*       flexibly associated with any of the four policers.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       portNum         - port number (including the CPU port).
*       stormType       - storm type
*       index           - policer index (0..3)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage, stormType ot port.
*       GT_OUT_OF_RANGE             - on out of range of index.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerPortStormTypeIndexSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    GT_PHYSICAL_PORT_NUM                               portNum;
    CPSS_DXCH_POLICER_STORM_TYPE_ENT    stormType;
    GT_U32                              index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    stormType = (CPSS_DXCH_POLICER_STORM_TYPE_ENT)inArgs[2];
    index = (GT_U32)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPolicerPortStormTypeIndexSet(devNum, plrStage, portNum,
                                                  stormType, index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPolicerPortStormTypeIndexGet
*
* DESCRIPTION:
*       Get policing profile for given source/target port and storm rate type.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       portNum         - port number (including the CPU port).
*       stormType       - storm type
*
* OUTPUTS:
*       indexPtr        - pointer to policer index.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage, stormType or portNum.
*       GT_OUT_OF_RANGE             - on out of range of port.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerPortStormTypeIndexGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    GT_PHYSICAL_PORT_NUM                               portNum;
    CPSS_DXCH_POLICER_STORM_TYPE_ENT    stormType;
    GT_U32                              index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    stormType = (CPSS_DXCH_POLICER_STORM_TYPE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPolicerPortStormTypeIndexGet(devNum, plrStage, portNum,
                                                  stormType, &index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", index);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerEgressL2RemarkModelSet
*
* DESCRIPTION:
*       Sets Egress Policer L2 packets remarking model.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - physical device number.
*       model           - L2 packets remarking model. It defines QoS parameter,
*                         which used as index in the Qos table:
*                         User Priority or Traffic Class.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerEgressL2RemarkModelSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                result;
    GT_U8                                    devNum;
    CPSS_DXCH_POLICER_L2_REMARK_MODEL_ENT    model;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    model = (CPSS_DXCH_POLICER_L2_REMARK_MODEL_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPolicerEgressL2RemarkModelSet(devNum, model);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerEgressL2RemarkModelGet
*
* DESCRIPTION:
*       Gets Egress Policer L2 packets remarking model.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       modelPtr        - L2 packets remarking model. It defines QoS parameter,
*                         which used as index in the Qos table:
*                         User Priority or Traffic Class.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerEgressL2RemarkModelGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                result;
    GT_U8                                    devNum;
    CPSS_DXCH_POLICER_L2_REMARK_MODEL_ENT    model;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPolicerEgressL2RemarkModelGet(devNum, &model);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", model);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerEgressQosUpdateEnableSet
*
* DESCRIPTION:
*       The function enables or disables QoS remarking of conforming packets.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum  - physical device number.
*       enable  - Enable/disable Qos update for conforming packets:
*                 GT_TRUE  - Remark Qos parameters of conforming packets.
*                 GT_FALSE - Keep incoming Qos parameters of conforming packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerEgressQosUpdateEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U8           devNum;
    GT_BOOL         enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPolicerEgressQosUpdateEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerEgressQosUpdateEnableGet
*
* DESCRIPTION:
*       The function get QoS remarking status of conforming packets
*       (enable/disable).
*
* APPLICABLE DEVICES: All DxChXcat Devices.
*
* INPUTS:
*       devNum  - physical device number.
*
* OUTPUTS:
*       enablePtr  - Enable/disable Qos update for conforming packets:
*                   GT_TRUE  - Remark Qos parameters of conforming packets.
*                   GT_FALSE - Keep incoming Qos parameters of conforming packets.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerEgressQosUpdateEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U8           devNum;
    GT_BOOL         enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPolicerEgressQosUpdateEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerVlanCntrSet
*
* DESCRIPTION:
*      Sets Policer VLAN Counters located in the Policer Counting Entry upon
*      enabled VLAN counting mode.
*      When working in VLAN Counting Mode each counter counts the number of
*      packets or bytes according to the VLAN counters mode set
*      by cpssDxChPolicerVlanCountingModeSet(). Index of counter is VLAN ID.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       vid             - VLAN ID. Bits [1:0] of VID are used to select the
*                         relevant 32 bit counter inside the Counting Entry.
*       cntrValue       - Packet/Byte counter indexed by the packet's VID.
*                         For counter reset this parameter should be nullified.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on disabled VLAN Counting.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or vid (VID range is limited by max
*                         number of Policer Counting Entries) or stage.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The VLAN counters are free-running no-sticky counters.
*
*       In order to set Vlan counting entry under traffic perform the following
*       algorithm:
*           - disconnect entryes from traffic
*           - call cpssDxChPolicerCountingWriteBackCacheFlush
*           - set new values
*           - connect entries back to traffic
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerVlanCntrSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U8           devNum;
    GT_U16          vid;
    GT_U32          cntrValue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vid = (GT_U16)inArgs[1];
    cntrValue = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPolicerVlanCntrSet(devNum, plrStage, vid, cntrValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerVlanCntrGet
*
* DESCRIPTION:
*      Gets Policer VLAN Counters located in the Policer Counting Entry upon
*      enabled VLAN counting mode.
*      When working in VLAN Counting Mode each counter counts the number of
*      packets or bytes according to the VLAN counters mode set
*      by cpssDxChPolicerVlanCountingModeSet(). Index of counter is VLAN ID.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       vid             - VLAN ID. Bits [1:0] of VID are used to select the
*                         relevant 32 bit counter inside the Counting Entry.
*
* OUTPUTS:
*       cntrValuePtr    - Packet/Byte counter indexed by the packet's VID.
*                         For counter reset this parameter should be nullified.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on disabled VLAN Counting.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or vid (VID range is limited by max
*                         number of Policer Counting Entries) or stage.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The VLAN counters are free-running no-sticky counters.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerVlanCntrGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U8           devNum;
    GT_U16          vid;
    GT_U32          cntrValue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vid = (GT_U16)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPolicerVlanCntrGet(devNum, plrStage, vid, &cntrValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cntrValue);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerVlanCountingPacketCmdTriggerSet
*
* DESCRIPTION:
*       Enables or disables VLAN counting triggering according to the
*       specified packet command.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0 or Ingress #1.
*       cmdTrigger      - The packet command trigger.
*                         The following packet command are supported:
*                           Forward (CPSS_PACKET_CMD_FORWARD_E),
*                           Hard Drop (CPSS_PACKET_CMD_DROP_HARD_E),
*                           Soft Drop (CPSS_PACKET_CMD_DROP_SOFT_E),
*                           Trap (CPSS_PACKET_CMD_TRAP_TO_CPU_E),
*                           Mirror to CPU (CPSS_PACKET_CMD_MIRROR_TO_CPU_E).
*       enable          - Enable/Disable VLAN Counting according to the packet
*                         command trigger:
*                         GT_TRUE  - Enable VLAN counting on packet with
*                                    packet command trigger.
*                         GT_FALSE - Disable VLAN counting on packet with
*                                    packet command trigger.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - on not supported egress direction
*
* COMMENTS:
*       All packet commands are enabled in the HW by default.
*       For egress policer the default trigger values shouldn't be changed.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerVlanCountingPacketCmdTriggerSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;
    GT_U8                   devNum;
    CPSS_PACKET_CMD_ENT     cmdTrigger;
    GT_BOOL                 enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    cmdTrigger = (CPSS_PACKET_CMD_ENT)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPolicerVlanCountingPacketCmdTriggerSet(devNum, plrStage,
                                                            cmdTrigger, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerVlanCountingPacketCmdTriggerGet
*
* DESCRIPTION:
*       Gets VLAN counting triggering status (Enable/Disable) according to the
*       specified packet command.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0 or Ingress #1.
*       cmdTrigger      - The packet command trigger.
*                         The following packetcommand are supported:
*                           Forward (CPSS_PACKET_CMD_FORWARD_E),
*                           Hard Drop (CPSS_PACKET_CMD_DROP_HARD_E),
*                           Soft Drop (CPSS_PACKET_CMD_DROP_SOFT_E),
*                           Trap (CPSS_PACKET_CMD_TRAP_TO_CPU_E),
*                           Mirror to CPU (CPSS_PACKET_CMD_MIRROR_TO_CPU_E).
*
* OUTPUTS:
*       enablePtr       - Pointer to Enable/Disable VLAN Counting according
*                         to the packet command trigger:
*                         GT_TRUE  - Enable VLAN counting on packet with
*                                    triggered packet command.
*                         GT_FALSE - Disable VLAN counting on packet with
*                                    triggered packet command.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       All packet commands are enabled in the HW by default.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerVlanCountingPacketCmdTriggerGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;
    GT_U8                   devNum;
    CPSS_PACKET_CMD_ENT     cmdTrigger;
    GT_BOOL                 enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    cmdTrigger = (CPSS_PACKET_CMD_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPolicerVlanCountingPacketCmdTriggerGet(devNum, plrStage,
                                                            cmdTrigger, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerVlanCountingModeSet
*
* DESCRIPTION:
*       Sets the Policer VLAN counters mode as byte or packet based.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       mode            - VLAN counters mode: Byte or Packet based.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerVlanCountingModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U8                                   devNum;
    CPSS_DXCH_POLICER_VLAN_CNTR_MODE_ENT    mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_DXCH_POLICER_VLAN_CNTR_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPolicerVlanCountingModeSet(devNum, plrStage, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerVlanCountingModeGet
*
* DESCRIPTION:
*       Gets the Policer VLAN counters mode as byte or packet based.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       modePtr         - (pointer to) VLAN counters mode: Byte or Packet based.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerVlanCountingModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U8                                   devNum;
    CPSS_DXCH_POLICER_VLAN_CNTR_MODE_ENT    mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPolicerVlanCountingModeGet(devNum, plrStage, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerPolicyCntrSet
*
* DESCRIPTION:
*      Sets Policer Policy Counters located in the Policer Counting Entry upon
*      enabled Policy Counting mode.
*      When working in Policy Counting Mode each counter counts the number of
*      packets matching on a Policy rules.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       index           - index set by Policy Action Entry or Metering entry
*                         and belongs to the range from 0 up to maximal number
*                         of Policer Policy counters.
*                         Maximal number is number of policer entries for
*                         counting is number of counting entries
*                         multiplied by 8.
*                         Some indexes in the range are not valid. Not valid
*                         indexes are:
*                           - (index % 8) == 6
*                           - (index % 8) == 7
*                         e.g. not valid indexes 6, 7, 14, 15, 22, 23 ...
*       cntrValue       - packets counter.
*                         For counter reset this parameter should be nullified.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on disabled Policy Counting.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or index (index range is limited by
*                         max number of Policer Policy counters).
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Policy counters are free-running no-sticky counters.
*
*       In order to set Policy counting entry under traffic perform the following
*       algorithm:
*           - disconnect entryes from traffic
*           - call cpssDxChPolicerCountingWriteBackCacheFlush
*           - set new values
*           - connect entries back to traffic
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerPolicyCntrSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      index;
    GT_U32      cntrValue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
    cntrValue = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = pg_wrap_cpssDxChPolicerPolicyCntrSet(devNum, index, cntrValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerPolicyCntrGet
*
* DESCRIPTION:
*      Gets Policer Policy Counters located in the Policer Counting Entry upon
*      enabled Policy Counting mode.
*      When working in Policy Counting Mode each counter counts the number of
*      packets matching on a a Policy rules.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       index           - index set by Policy Action Entry or Metering entry
*                         and belongs to the range from 0 up to maximal number
*                         of Policer Policy counters.
*                         Maximal number is number of policer entries for
*                         counting is number of counting entries
*                         multiplied by 8.
*                         Some indexes in the range are not valid. Not valid
*                         indexes are:
*                           - (index % 8) == 6
*                           - (index % 8) == 7
*                         e.g. not valid indexes 6, 7, 14, 15, 22, 23 ...
*
* OUTPUTS:
*       cntrValuePtr    - Pointer to the packet counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on disabled Policy Counting.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or index.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_TIMEOUT               - on time out.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Policy counters are free-running no-sticky counters.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerPolicyCntrGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      index;
    GT_U32      cntrValue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxChPolicerPolicyCntrGet(devNum, index, &cntrValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cntrValue);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerMemorySizeModeSet
*
* DESCRIPTION:
*      Sets internal table sizes and the way they are shared between the Ingress
*      policers.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - device number.
*       mode            - The mode in which internal tables are shared.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Mode change should not be done when meter or counter entries are in use.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerMemorySizeModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U8                                   devNum;
    CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT  mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPolicerMemorySizeModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerMemorySizeModeGet
*
* DESCRIPTION:
*      Gets internal table sizes and the way they are shared between the Ingress
*      policers.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - device number.
*
* OUTPUTS:
*       modePtr         - The mode in which internal tables are shared.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerMemorySizeModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U8                                   devNum;
    CPSS_DXCH_POLICER_MEMORY_CTRL_MODE_ENT  mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPolicerMemorySizeModeGet(devNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;}

/*******************************************************************************
* cpssDxChPolicerCountingWriteBackCacheFlush
*
* DESCRIPTION:
*       Flush internal Write Back Cache (WBC) of counting entries.
*       The Policer implements internal Write Back Cache for frequent and rapid
*       update of counting entries.
*       Since the WBC holds the policer entries' data, the CPU must clear its
*       content when performing direct write access of the counting entries by
*       one of following functions:
*        - cpssDxCh3PolicerBillingEntrySet
*        - cpssDxChPolicerVlanCntrSet
*        - cpssDxChPolicerPolicyCntrSet
*       The policer counting should be disabled before flush of WBC if direct
*       write access is performed under traffic. And policer counting should be
*       enabled again after finish of write access.
*       The cpssDxChPolicerCountingModeSet may be used to disable/enable
*       the policer counting.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or stage.
*       GT_HW_ERROR              - on Hardware error.
*       GT_TIMEOUT               - on time out.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To use the listed above APIs (cpssDxCh3PolicerBillingEntrySet,
*       cpssDxChPolicerVlanCntrSet and cpssDxChPolicerPolicyCntrSet) under
*       traffic, perform the following algorithm:
*           - disconnect entryes from traffic
*           - call cpssDxChPolicerCountingWriteBackCacheFlush
*           - set new values
*           - connect entries back to traffic

*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerCountingWriteBackCacheFlush
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U8           devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPolicerCountingWriteBackCacheFlush(devNum, plrStage);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerGlobalStageSet
*
* DESCRIPTION:
*      Sets global Policer stage parameter.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - device number.
*       stage           - stage: : Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*
* COMMENTS:
*       Mode change should not be done when meter or counter entries are in use.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerGlobalStageSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    plrStage = (CPSS_DXCH_POLICER_STAGE_TYPE_ENT)inArgs[0];

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxCh3PolicerManagementCountersSetV1
*
* DESCRIPTION:
*       Sets the value of specified Management Counters.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - physical device number.
*       mngCntrSet      - Management Counters Set[0..2].
*       mngCntrType     - Management Counters Type (GREEN, YELLOW, RED, DROP).
*       mngCntrPtr      - pointer to the Management Counters Entry must be set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum or mngCntrType or Management
*                         Counters Set number.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerManagementCountersSetV1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT mngCntrSet;
    CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT mngCntrType;
    CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC mngCntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

   devNum = (GT_U8)inArgs[0];
   mngCntrSet=(CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT)inArgs[1];
   mngCntrType=(CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT)inArgs[2];
   /* counter word 0 */
   mngCntr.duMngCntr.l[0]=(GT_U32)inArgs[3];
   /* counter word 1 */
   mngCntr.duMngCntr.l[1]=(GT_U32)inArgs[4];
   mngCntr.packetMngCntr=(GT_U32)inArgs[5];

    /* call cpss api function */
    result = pg_wrap_cpssDxCh3PolicerManagementCountersSet(devNum, mngCntrSet,
                                                           mngCntrType, &mngCntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerManagementCountersGetV1
*
* DESCRIPTION:
*       Gets the value of specified Management Counters.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum          - physical device number.
*       mngCntrSet      - Management Counters Set[0..2].
*       mngCntrType     - Management Counters Type (GREEN, YELLOW, RED, DROP).
*
* OUTPUTS:
*       mngCntrPtr      - pointer to the requested Management Counters Entry.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum or mngCntrType or Management
*                         Counters Set number.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerManagementCountersGetV1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT mngCntrSet;
    CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT mngCntrType;
    CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC mngCntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

     devNum = (GT_U8)inArgs[0];
     mngCntrSet=(CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT)inArgs[1];
     mngCntrType=(CPSS_DXCH3_POLICER_MNG_CNTR_TYPE_ENT)inArgs[2];

    /* call cpss api function */
    result = pg_wrap_cpssDxCh3PolicerManagementCountersGet(devNum, mngCntrSet,
                                                           mngCntrType, &mngCntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d",mngCntr.duMngCntr.l[0],
                 mngCntr.duMngCntr.l[1],mngCntr.packetMngCntr);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChPolicerMeteringCalcMethodSet
*
* DESCRIPTION:
*      Sets Metering Policer parameters calculation orientation method.
*      Due to the fact that selecting algorithm has effect on how HW metering
*      parameters are set, this API influence on
*      "cpssDxCh3PolicerMeteringEntrySet" and
*      "cpssDxCh3PolicerEntryMeterParamsCalculate" APIs.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum                  - device number.
*       meteringCalcMethod      - selecting calculation of HW metering
*                                 parameters between CIR\PIR or CBS\PBS
*                                 orientation.
*       cirPirAllowedDeviation  - the allowed deviation in percentage from the
*                                 requested CIR\PIR. Relevant only for
*                         CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_AND_CBS_E.
*       cbsPbsCalcOnFail        - GT_TRUE: If CBS constraints cannot be
*                                 matched return to CIR oriented calculation.
*                                 GT_FALSE: If CBS constraints cannot be
*                                 matched return error. Relevant only for
*                         CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_AND_CBS_E.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PARAM                - wrong devNum or mode.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_ONLY_E - The default
*       algorithm in which metering parameters are configured in such a way
*       that the selected CIR\PIR is the one which is the nearest possible
*       (under the granularity constraint) to the requested CIR\PIR.
*       CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_AND_CBS_E - In this
*       algorithm a deviation from requested CIR\PIR is defined. At first stage
*       the algorithm tries to find parameters that will complay with both the
*       CBS\PBS requested and the range allowed for the CIR\PIR. If no matching
*       solution is achieved there is an option to perform another stage. At
*       the second (optional) stage CIR\PIR is selected in the allowed range
*       and under this selection CBS\PBS is configured to be as near as
*       possible to the requested one. If second stage is needed and not
*       selected failure (error) indication is reported.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerMeteringCalcMethodSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_POLICER_METERING_CALC_METHOD_ENT   meteringCalcMethod;
    GT_U32                                       cirPirAllowedDeviation;
    GT_BOOL                                      cbsPbsCalcOnFail;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    meteringCalcMethod=(CPSS_DXCH_POLICER_METERING_CALC_METHOD_ENT)inArgs[1];
    cirPirAllowedDeviation=(GT_U32)inArgs[2];
    cbsPbsCalcOnFail=(GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPolicerMeteringCalcMethodSet(devNum,
                                                  meteringCalcMethod,
                                                  cirPirAllowedDeviation,
                                                  cbsPbsCalcOnFail);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerMeteringCalcMethodGet
*
* DESCRIPTION:
*      Gets Metering Policer parameters calculation orientation method.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum              - device number.
*
* OUTPUTS:
*       meteringCalcMethodPtr       - (pointer to) selected calculation of HW
*                                     metering parameters between CIR\PIR or
*                                     CBS\PBS orientation.
*       cirPirAllowedDeviationPtr   - (pointer to) the allowed deviation in
*                                     percentage from the requested CIR\PIR.
*                                     Relevant only for
*                         CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_AND_CBS_E.
*       cbsPbsCalcOnFailPtr         - (pointer to)
*                                     GT_TRUE: If CBS\PBS constraints cannot be
*                                     matched return to CIR\PIR oriented
*                                     calculation.
*                                     GT_FALSE: If CBS\PBS constraints cannot
*                                     be matched return error.
*                                     Relevant only for
*                         CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_AND_CBS_E.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_BAD_PARAM                - wrong devNum or mode.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_ONLY_E - The default
*       algorithm in which metering parameters are configured in such a way
*       that the selected CIR\PIR is the one which is the nearest possible
*       (under the granularity constraint) to the requested CIR\PIR.
*       CPSS_DXCH_POLICER_METERING_CALC_METHOD_CIR_AND_CBS_E - In this
*       algorithm a deviation from requested CIR\PIR is defined. At first stage
*       the algorithm tries to find parameters that will complay with both the
*       CBS\PBS requested and the range allowed for the CIR\PIR. If no matching
*       solution is achieved there is an option to perform another stage. At
*       the second (optional) stage CIR\PIR is selected in the allowed range
*       and under this selection CBS\PBS is configured to be as near as
*       possible to the requested one. If second stage is needed and not
*       selected failure (error) indication is reported.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerMeteringCalcMethodGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_POLICER_METERING_CALC_METHOD_ENT   meteringCalcMethod;
    GT_U32                                       cirPirAllowedDeviation;
    GT_BOOL                                      cbsPbsCalcOnFail;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    cirPirAllowedDeviation = 0xFFFFFFFF;
    cbsPbsCalcOnFail = GT_FALSE;

    /* call cpss api function */
    result = cpssDxChPolicerMeteringCalcMethodGet(devNum,
                                                  &meteringCalcMethod,
                                                  &cirPirAllowedDeviation,
                                                  &cbsPbsCalcOnFail);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d",meteringCalcMethod,
                 cirPirAllowedDeviation,cbsPbsCalcOnFail);

    return CMD_OK;
}

 /**************Table:cpssDxChXcatPolicerMeteringEntry********************************/
static  GT_U32 minMeteringEntryXcatIndex;
static  GT_U32 maxMeteringEntryXcatIndex;
static  GT_U32 meteringEntryXcatIndex;
/*******************************************************************************
* wrCpssDxCh3PolicerMeteringEntrySetV1
*
* DESCRIPTION:
*      Sets Metering Policer Entry.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - device number.
*       entryIndex      - index of Policer Metering Entry [0..1023].
*       entryPtr        - pointer to the metering policer entry going to be set.
*
* OUTPUTS:
*       tbParamsPtr     - pointer to actual policer token bucket parameters.
*                         The token bucket parameters are returned as output
*                         values. This is due to the hardware rate resolution,
*                         the exact rate or burst size requested may not be
*                         honored. The returned value gives the user the
*                         actual parameters configured in the hardware.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - wrong devNum, entryIndex or entry parameters.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_TIMEOUT      - on time out of Policer Tables non-direct access.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerMeteringEntrySetV1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 entryIndex;
    CPSS_DXCH3_POLICER_METERING_ENTRY_STC entry;
    CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT tbParams;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(&entry, 0, sizeof(entry));

    /* map input arguments to locals */

    devNum =(GT_U8)inArgs[0];

    entryIndex=(GT_U32)inFields[0];
    entry.countingEntryIndex=(GT_U32)inFields[1];
    entry.mngCounterSet=(CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT)inFields[2];
    entry.meterColorMode=(CPSS_POLICER_COLOR_MODE_ENT)inFields[3];
    entry.meterMode=(CPSS_DXCH3_POLICER_METER_MODE_ENT)inFields[4];

    if(entry.meterMode==CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E)
        {
          entry.tokenBucketParams.srTcmParams.cir=(GT_U32)inFields[5];
          entry.tokenBucketParams.srTcmParams.cbs=(GT_U32)inFields[6];
          entry.tokenBucketParams.srTcmParams.ebs=(GT_U32)inFields[7];
        }
    else
        if(entry.meterMode==CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E)
        {
          entry.tokenBucketParams.trTcmParams.cir=(GT_U32)inFields[8];
          entry.tokenBucketParams.trTcmParams.cbs=(GT_U32)inFields[9];
          entry.tokenBucketParams.trTcmParams.pir=(GT_U32)inFields[10];
          entry.tokenBucketParams.trTcmParams.pbs=(GT_U32)inFields[11];
        }

    entry.modifyUp = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[12];
    entry.modifyDscp = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[13];
    entry.modifyDp = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[14];
    entry.yellowPcktCmd = (CPSS_DXCH3_POLICER_NON_CONFORM_CMD_ENT)inFields[15];
    entry.redPcktCmd = (CPSS_DXCH3_POLICER_NON_CONFORM_CMD_ENT)inFields[16];
    entry.qosProfile = (GT_U32)inFields[17];
    entry.remarkMode = (CPSS_DXCH_POLICER_REMARK_MODE_ENT)inFields[18];

    result = cpssDxCh3PolicerMeteringEntryLegacyUpdates(devNum, &entry);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = pg_wrap_cpssDxCh3PolicerMeteringEntrySet(devNum, entryIndex,
                                                      &entry, &tbParams);

    /* pack output arguments to galtis string */


    if(entry.meterMode==CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E)
        {
          galtisOutput(outArgs, result, "%d%d%d",tbParams.srTcmParams.cir,tbParams.srTcmParams.cbs,tbParams.srTcmParams.ebs);
        }
    else
        if(entry.meterMode==CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E)
        {
          galtisOutput(outArgs, result, "%d%d%d%d",tbParams.trTcmParams.cir,tbParams.trTcmParams.cbs,
                                               tbParams.trTcmParams.pir,tbParams.trTcmParams.pbs);
        }

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChPolicerLastEntryGet
*
* DESCRIPTION:
*      Gets metering and counting last entry for refresh procedures.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - device number.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_U32           - memory size
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_U32 wrCpssDxChPolicerLastEntryGet
(
    IN  GT_U8 devNum
)
{
    if (PRV_CPSS_MAX_PP_DEVICES_CNS <= devNum)
    {
        return 8191;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        return 8191;
    }
    else
    {
        return 2047;
    }
}

/*******************************************************************************
* wrCpssDxCh3PolicerMeteringEntryGetEntryV1
*
* DESCRIPTION:
*      Gets Metering Policer Entry configuration.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - device number.
*       entryIndex      - index of Policer Metering Entry [0..1023].
*
* OUTPUTS:
*       entryPtr        - pointer to the requested metering policer entry.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - wrong devNum or entryIndex.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerMeteringEntryGetEntryV1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;

    CPSS_DXCH3_POLICER_METERING_ENTRY_STC entry;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =(GT_U8)inArgs[0];

     if (meteringEntryXcatIndex>maxMeteringEntryXcatIndex||meteringEntryXcatIndex>wrCpssDxChPolicerLastEntryGet(devNum))
    {
        galtisOutput(outArgs, CMD_OK, "%d", -1);
        return CMD_OK;
    }

    /* reset entry to avoid trash in output for not used fields */
    cmdOsMemSet(&entry, 0, sizeof(entry));

    /* call cpss api function */
    result = pg_wrap_cpssDxCh3PolicerMeteringEntryGet(devNum, meteringEntryXcatIndex,
                                                      &entry);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    cmdOsMemSet(inFields,0,CMD_MAX_FIELDS);

    inFields[0]=meteringEntryXcatIndex;
    inFields[1]=entry.countingEntryIndex;
    inFields[2]=entry.mngCounterSet;
    inFields[3]=entry.meterColorMode;
    inFields[4]=entry.meterMode;

    if(entry.meterMode==CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E)
        {
          inFields[5]=entry.tokenBucketParams.srTcmParams.cir;
          inFields[6]=entry.tokenBucketParams.srTcmParams.cbs;
          inFields[7]=entry.tokenBucketParams.srTcmParams.ebs;
        }
    else
        if(entry.meterMode==CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E)
        {
          inFields[8]=entry.tokenBucketParams.trTcmParams.cir;
          inFields[9]=entry.tokenBucketParams.trTcmParams.cbs;
          inFields[10]=entry.tokenBucketParams.trTcmParams.pir;
          inFields[11]=entry.tokenBucketParams.trTcmParams.pbs;
        }

    inFields[12]=entry.modifyUp;
    inFields[13]=entry.modifyDscp;
    inFields[14]=entry.modifyDp;
    inFields[15]=entry.yellowPcktCmd;
    inFields[16]=entry.redPcktCmd;
    inFields[17]=entry.qosProfile;
    inFields[18]=entry.remarkMode;


    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0],inFields[1],inFields[2],inFields[3],inFields[4],
                inFields[5],inFields[6],inFields[7],inFields[8],inFields[9],
                inFields[10],inFields[11],inFields[12],inFields[13],
                inFields[14],inFields[15],inFields[16],inFields[17],inFields[18]);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}
/*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerMeteringEntryGetFirstV1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    minMeteringEntryXcatIndex=(GT_U32)inArgs[1];
    maxMeteringEntryXcatIndex=(GT_U32)inArgs[2];

    meteringEntryXcatIndex=minMeteringEntryXcatIndex;

    return wrCpssDxCh3PolicerMeteringEntryGetEntryV1(inArgs,inFields,numFields,outArgs);

}
/*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerMeteringEntryGetNextV1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    meteringEntryXcatIndex++;

    return wrCpssDxCh3PolicerMeteringEntryGetEntryV1(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* wrCpssDxCh3PolicerMeteringEntrySetV2
*
* DESCRIPTION:
*      Sets Metering Policer Entry.
*
* APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number.
*       entryIndex      - index of Policer Metering Entry [0..1023].
*       entryPtr        - pointer to the metering policer entry going to be set.
*
* OUTPUTS:
*       tbParamsPtr     - pointer to actual policer token bucket parameters.
*                         The token bucket parameters are returned as output
*                         values. This is due to the hardware rate resolution,
*                         the exact rate or burst size requested may not be
*                         honored. The returned value gives the user the
*                         actual parameters configured in the hardware.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - wrong devNum, entryIndex or entry parameters.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_TIMEOUT      - on time out of Policer Tables non-direct access.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerMeteringEntrySetV2
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 entryIndex;
    CPSS_DXCH3_POLICER_METERING_ENTRY_STC entry;
    CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT tbParams;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(&entry, 0, sizeof(entry));

    /* map input arguments to locals */

    devNum =(GT_U8)inArgs[0];

    entryIndex=(GT_U32)inFields[0];
    entry.countingEntryIndex=(GT_U32)inFields[1];
    entry.mngCounterSet=(CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT)inFields[2];
    entry.meterColorMode=(CPSS_POLICER_COLOR_MODE_ENT)inFields[3];
    entry.meterMode=(CPSS_DXCH3_POLICER_METER_MODE_ENT)inFields[4];

    switch(entry.meterMode)
    {
        case CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E:
            entry.tokenBucketParams.srTcmParams.cir=(GT_U32)inFields[5];
            entry.tokenBucketParams.srTcmParams.cbs=(GT_U32)inFields[6];
            entry.tokenBucketParams.srTcmParams.ebs=(GT_U32)inFields[7];
            break;
        case CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E:
        case CPSS_DXCH3_POLICER_METER_MODE_MEF0_E:
        case CPSS_DXCH3_POLICER_METER_MODE_MEF1_E:
            entry.tokenBucketParams.trTcmParams.cir=(GT_U32)inFields[8];
            entry.tokenBucketParams.trTcmParams.cbs=(GT_U32)inFields[9];
            entry.tokenBucketParams.trTcmParams.pir=(GT_U32)inFields[10];
            entry.tokenBucketParams.trTcmParams.pbs=(GT_U32)inFields[11];
            break;
        default:
            galtisOutput(outArgs, GT_BAD_PARAM, "%d", -1);
            return CMD_OK;
    }

    entry.modifyUp = (CPSS_DXCH_POLICER_MODIFY_UP_ENT)inFields[12];
    entry.modifyDscp = (CPSS_DXCH_POLICER_MODIFY_DSCP_ENT)inFields[13];
    entry.modifyDp = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[14];
    entry.modifyExp = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[15];
    entry.modifyTc = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[16];
    entry.greenPcktCmd = (CPSS_DXCH3_POLICER_NON_CONFORM_CMD_ENT)inFields[17];
    entry.yellowPcktCmd = (CPSS_DXCH3_POLICER_NON_CONFORM_CMD_ENT)inFields[18];
    entry.redPcktCmd = (CPSS_DXCH3_POLICER_NON_CONFORM_CMD_ENT)inFields[19];
    entry.qosProfile = (GT_U32)inFields[20];
    entry.remarkMode = (CPSS_DXCH_POLICER_REMARK_MODE_ENT)inFields[21];

    entry.byteOrPacketCountingMode =
                    (CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT)inFields[22];
    entry.packetSizeMode = (CPSS_POLICER_PACKET_SIZE_MODE_ENT)inFields[23];
    entry.tunnelTerminationPacketSizeMode =
                    (CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT)inFields[24];
    entry.dsaTagCountingMode =
                    (CPSS_DXCH3_POLICER_DSA_TAG_COUNTING_MODE_ENT)inFields[25];
    entry.timeStampCountingMode =
                    (CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT)inFields[26];
    entry.yellowEcnMarkingEnable = (GT_BOOL)inFields[27];

    /* call cpss api function */
    result = pg_wrap_cpssDxCh3PolicerMeteringEntrySet(devNum, entryIndex,
                                                      &entry, &tbParams);

    /* pack output arguments to galtis string */
    switch(entry.meterMode)
    {
        case CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E:
            galtisOutput(outArgs, result, "%d%d%d",tbParams.srTcmParams.cir,tbParams.srTcmParams.cbs,tbParams.srTcmParams.ebs);
            break;
        case CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E:
        case CPSS_DXCH3_POLICER_METER_MODE_MEF0_E:
        case CPSS_DXCH3_POLICER_METER_MODE_MEF1_E:
            galtisOutput(outArgs, result, "%d%d%d%d",tbParams.trTcmParams.cir,tbParams.trTcmParams.cbs,
                                               tbParams.trTcmParams.pir,tbParams.trTcmParams.pbs);
            break;
        default:
            galtisOutput(outArgs, GT_BAD_PARAM, "%d", -1);
            return CMD_OK;
    }

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxCh3PolicerMeteringEntryGetEntryV2
*
* DESCRIPTION:
*      Gets Metering Policer Entry configuration.
*
* APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number.
*       entryIndex      - index of Policer Metering Entry [0..1023].
*
* OUTPUTS:
*       entryPtr        - pointer to the requested metering policer entry.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - wrong devNum or entryIndex.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerMeteringEntryGetEntryV2
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH3_POLICER_METERING_ENTRY_STC entry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =(GT_U8)inArgs[0];

    if (meteringEntryXcatIndex>maxMeteringEntryXcatIndex||meteringEntryXcatIndex>wrCpssDxChPolicerLastEntryGet(devNum))
    {
        galtisOutput(outArgs, CMD_OK, "%d", -1);
        return CMD_OK;
    }

    /* reset entry to avoid trash in output for not used fields */
    cmdOsMemSet(&entry, 0, sizeof(entry));

    /* call cpss api function */
    result = pg_wrap_cpssDxCh3PolicerMeteringEntryGet(devNum, meteringEntryXcatIndex,
                                                      &entry);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    cmdOsMemSet(inFields,0,CMD_MAX_FIELDS);

    inFields[0]=meteringEntryXcatIndex;
    inFields[1]=entry.countingEntryIndex;
    inFields[2]=entry.mngCounterSet;
    inFields[3]=entry.meterColorMode;
    inFields[4]=entry.meterMode;

    switch(entry.meterMode)
    {
        case CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E:
            inFields[5]=entry.tokenBucketParams.srTcmParams.cir;
            inFields[6]=entry.tokenBucketParams.srTcmParams.cbs;
            inFields[7]=entry.tokenBucketParams.srTcmParams.ebs;
            break;
        case CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E:
        case CPSS_DXCH3_POLICER_METER_MODE_MEF0_E:
        case CPSS_DXCH3_POLICER_METER_MODE_MEF1_E:
            inFields[8]=entry.tokenBucketParams.trTcmParams.cir;
            inFields[9]=entry.tokenBucketParams.trTcmParams.cbs;
            inFields[10]=entry.tokenBucketParams.trTcmParams.pir;
            inFields[11]=entry.tokenBucketParams.trTcmParams.pbs;
            break;
        default:
            galtisOutput(outArgs, GT_BAD_PARAM, "%d", -1);
            return CMD_OK;
    }

    inFields[12]=entry.modifyUp;
    inFields[13]=entry.modifyDscp;
    inFields[14]=entry.modifyDp;
    inFields[15]=entry.modifyExp;
    inFields[16]=entry.modifyTc;
    inFields[17]=entry.greenPcktCmd;
    inFields[18]=entry.yellowPcktCmd;
    inFields[19]=entry.redPcktCmd;
    inFields[20]=entry.qosProfile;
    inFields[21]=entry.remarkMode;
    inFields[22]=entry.byteOrPacketCountingMode;
    inFields[23]=entry.packetSizeMode;
    inFields[24]=entry.tunnelTerminationPacketSizeMode;
    inFields[25]=entry.dsaTagCountingMode;
    inFields[26]=entry.timeStampCountingMode;
    inFields[27]=entry.yellowEcnMarkingEnable;

    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0],inFields[1],inFields[2],inFields[3],inFields[4],
                inFields[5],inFields[6],inFields[7],inFields[8],inFields[9],
                inFields[10],inFields[11],inFields[12],inFields[13],
                inFields[14],inFields[15],inFields[16],inFields[17],
                inFields[18],inFields[19],inFields[20],inFields[21],
                inFields[22],inFields[23],inFields[24],inFields[25],
                inFields[26],inFields[27]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}
/*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerMeteringEntryGetFirstV2
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    minMeteringEntryXcatIndex=(GT_U32)inArgs[1];
    maxMeteringEntryXcatIndex=(GT_U32)inArgs[2];

    meteringEntryXcatIndex=minMeteringEntryXcatIndex;

    return wrCpssDxCh3PolicerMeteringEntryGetEntryV2(inArgs,inFields,numFields,outArgs);

}
/*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerMeteringEntryGetNextV2
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    meteringEntryXcatIndex++;

    return wrCpssDxCh3PolicerMeteringEntryGetEntryV2(inArgs,inFields,numFields,outArgs);
}

/**************Table:cpssDxChXcatPolicerBillingEntry********************************/
static  GT_U32 minBillingEntryXcatIndex;
static  GT_U32 maxBillingEntryXcatIndex;
static  GT_U32 billingEntryXcatIndex;
/*******************************************************************************
* cpssDxCh3PolicerBillingEntrySetV1
*
* DESCRIPTION:
*       Sets Policer Billing Counters.
*       Billing is the process of counting the amount of traffic that belongs
*       to a flow. This process can be activated only by metering and is used
*       either for statistical analysis of a flow's traffic or for actual
*       billing.
*       The billing counters are free-running no-sticky counters.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - device number.
*       entryIndex      - index of Policer Billing Counters Entry [0..1023].
*       billingCntrPtr  - pointer to the Billing Counters Entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or entryIndex.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       The packet is counted per outgoing DP or Conformance Level. When
*       metering is disabled, billing is always done according to packet's
*       Drop Precedence.
*       When metering is activated per source port, Billing counting cannot
*       be enabled.
*       None-zeroed setting of Billing Counters while metring/counting is
*       enabled may cause data coherency problems.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerBillingEntrySetV1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 entryIndex;
    CPSS_DXCH3_POLICER_BILLING_ENTRY_STC billingCntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    entryIndex=(GT_U32)inFields[0];
    billingCntr.greenCntr.l[0]=(GT_U32)inFields[1];
    billingCntr.greenCntr.l[1]=(GT_U32)inFields[2];
    billingCntr.yellowCntr.l[0]=(GT_U32)inFields[3];
    billingCntr.yellowCntr.l[1]=(GT_U32)inFields[4];
    billingCntr.redCntr.l[0]=(GT_U32)inFields[5];
    billingCntr.redCntr.l[1]=(GT_U32)inFields[6];
    billingCntr.billingCntrMode =
        (CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT)inFields[7];

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        billingCntr.billingCntrAllEnable = GT_FALSE;
        billingCntr.lmCntrCaptureMode =
            CPSS_DXCH3_POLICER_LM_CNTR_CAPTURE_MODE_COPY_E;
        billingCntr.greenCntrSnapshot.l[0] = 0;
        billingCntr.greenCntrSnapshot.l[1] = 0;
        billingCntr.greenCntrSnapshotValid = GT_FALSE;
        billingCntr.packetSizeMode = CPSS_POLICER_PACKET_SIZE_L2_INCLUDE_E;
        billingCntr.tunnelTerminationPacketSizeMode =
            CPSS_DXCH3_POLICER_TT_PACKET_SIZE_REGULAR_E;
        billingCntr.dsaTagCountingMode =
            CPSS_DXCH3_POLICER_DSA_TAG_COUNTING_MODE_INCLUDED_E;
        billingCntr.timeStampCountingMode =
            CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_INCLUDE_E;
    }

    /* call cpss api function */
    result = pg_wrap_cpssDxCh3PolicerBillingEntrySet(devNum, entryIndex,
                                                     &billingCntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerBillingEntryGetV1
*
* DESCRIPTION:
*       Gets Policer Billing Counters.
*       Billing is the process of counting the amount of traffic that belongs
*       to a flow. This process can be activated only by metering and is used
*       either for statistical analysis of a flow's traffic or for actual
*       billing.
*       The billing counters are free-running no-sticky counters.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum          - device number.
*       entryIndex      - index of Policer Billing Counters Entry [0..1023].
*       reset           - reset flag:
*                         GT_TRUE  - performing read and reset atomic operation.
*                         GT_FALSE - performing read counters operation only.
*
* OUTPUTS:
*       billingCntrPtr  - pointer to the Billing Counters Entry.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or entryIndex.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_TIMEOUT      - on time out of Policer Tables non-direct access.
*
* COMMENTS:
*       The packet is counted per outgoing DP or Conformance Level. When
*       metering is disabled, billing is always done according to packet's
*       Drop Precedence.
*       When metering is activated per source port, Billing counting cannot
*       be enabled.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerBillingEntryGetEntryV1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL reset;
    CPSS_DXCH3_POLICER_BILLING_ENTRY_STC billingCntr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    reset=(GT_BOOL)inArgs[1];

    if (billingEntryXcatIndex>maxBillingEntryXcatIndex||billingEntryXcatIndex>wrCpssDxChPolicerLastEntryGet(devNum))
    {
        galtisOutput(outArgs, CMD_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = pg_wrap_cpssDxCh3PolicerBillingEntryGet(devNum, billingEntryXcatIndex,
                                                     reset, &billingCntr);

   if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0]=billingEntryXcatIndex;
    inFields[1]=billingCntr.greenCntr.l[0];
    inFields[2]=billingCntr.greenCntr.l[1];
    inFields[3]=billingCntr.yellowCntr.l[0];
    inFields[4]=billingCntr.yellowCntr.l[1];
    inFields[5]=billingCntr.redCntr.l[0];
    inFields[6]=billingCntr.redCntr.l[1];
    inFields[7]=billingCntr.billingCntrMode;

    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0],inFields[1],inFields[2],
                inFields[3],inFields[4],inFields[5],inFields[6],inFields[7]);
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}
/*********************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerBillingEntryGetFirstV1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    minBillingEntryXcatIndex=(GT_U32)inArgs[2];
    maxBillingEntryXcatIndex=(GT_U32)inArgs[3];
    billingEntryXcatIndex=minBillingEntryXcatIndex;

    return wrCpssDxCh3PolicerBillingEntryGetEntryV1(inArgs,inFields,numFields,outArgs);
}
/*********************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerBillingEntryGetNextV1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
     billingEntryXcatIndex++;

     return wrCpssDxCh3PolicerBillingEntryGetEntryV1(inArgs,inFields,numFields,outArgs);

}

/*******************************************************************************
* cpssDxCh3PolicerBillingEntrySetV2
*
* DESCRIPTION:
*       Sets Policer Billing Counters.
*       Billing is the process of counting the amount of traffic that belongs
*       to a flow. This process can be activated only by metering and is used
*       either for statistical analysis of a flow's traffic or for actual
*       billing.
*       The billing counters are free-running no-sticky counters.
*
* APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number.
*       entryIndex      - index of Policer Billing Counters Entry [0..1023].
*       billingCntrPtr  - pointer to the Billing Counters Entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or entryIndex.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       The packet is counted per outgoing DP or Conformance Level. When
*       metering is disabled, billing is always done according to packet's
*       Drop Precedence.
*       When metering is activated per source port, Billing counting cannot
*       be enabled.
*       None-zeroed setting of Billing Counters while metring/counting is
*       enabled may cause data coherency problems.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerBillingEntrySetV2
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 entryIndex;
    CPSS_DXCH3_POLICER_BILLING_ENTRY_STC billingCntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    entryIndex=(GT_U32)inFields[0];
    billingCntr.greenCntr.l[0]=(GT_U32)inFields[1];
    billingCntr.greenCntr.l[1]=(GT_U32)inFields[2];
    billingCntr.yellowCntr.l[0]=(GT_U32)inFields[3];
    billingCntr.yellowCntr.l[1]=(GT_U32)inFields[4];
    billingCntr.redCntr.l[0]=(GT_U32)inFields[5];
    billingCntr.redCntr.l[1]=(GT_U32)inFields[6];
    billingCntr.billingCntrMode =
        (CPSS_DXCH3_POLICER_BILLING_CNTR_MODE_ENT)inFields[7];
    billingCntr.billingCntrAllEnable = (GT_BOOL)inFields[8];
    billingCntr.lmCntrCaptureMode =
        (CPSS_DXCH3_POLICER_LM_CNTR_CAPTURE_MODE_ENT)inFields[9];
    billingCntr.greenCntrSnapshot.l[0] = (GT_U32)inFields[10];
    billingCntr.greenCntrSnapshot.l[1] = (GT_U32)inFields[11];
    billingCntr.greenCntrSnapshotValid = (GT_BOOL)inFields[12];
    billingCntr.packetSizeMode =
        (CPSS_POLICER_PACKET_SIZE_MODE_ENT)inFields[13];
    billingCntr.tunnelTerminationPacketSizeMode =
        (CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT)inFields[14];
    billingCntr.dsaTagCountingMode =
        (CPSS_DXCH3_POLICER_DSA_TAG_COUNTING_MODE_ENT)inFields[15];
    billingCntr.timeStampCountingMode =
        (CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT)inFields[16];

    /* call cpss api function */
    result = pg_wrap_cpssDxCh3PolicerBillingEntrySet(devNum, entryIndex,
                                                     &billingCntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PolicerBillingEntryGetV2
*
* DESCRIPTION:
*       Gets Policer Billing Counters.
*       Billing is the process of counting the amount of traffic that belongs
*       to a flow. This process can be activated only by metering and is used
*       either for statistical analysis of a flow's traffic or for actual
*       billing.
*       The billing counters are free-running no-sticky counters.
*
* APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number.
*       entryIndex      - index of Policer Billing Counters Entry [0..1023].
*       reset           - reset flag:
*                         GT_TRUE  - performing read and reset atomic operation.
*                         GT_FALSE - performing read counters operation only.
*
* OUTPUTS:
*       billingCntrPtr  - pointer to the Billing Counters Entry.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or entryIndex.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_TIMEOUT      - on time out of Policer Tables non-direct access.
*
* COMMENTS:
*       The packet is counted per outgoing DP or Conformance Level. When
*       metering is disabled, billing is always done according to packet's
*       Drop Precedence.
*       When metering is activated per source port, Billing counting cannot
*       be enabled.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerBillingEntryGetEntryV2
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL reset;
    CPSS_DXCH3_POLICER_BILLING_ENTRY_STC billingCntr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    reset=(GT_BOOL)inArgs[1];

    if (billingEntryXcatIndex>maxBillingEntryXcatIndex||billingEntryXcatIndex>wrCpssDxChPolicerLastEntryGet(devNum))
    {
        galtisOutput(outArgs, CMD_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = pg_wrap_cpssDxCh3PolicerBillingEntryGet(devNum, billingEntryXcatIndex,
                                                     reset, &billingCntr);

   if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = billingEntryXcatIndex;
    inFields[1] = billingCntr.greenCntr.l[0];
    inFields[2] = billingCntr.greenCntr.l[1];
    inFields[3] = billingCntr.yellowCntr.l[0];
    inFields[4] = billingCntr.yellowCntr.l[1];
    inFields[5] = billingCntr.redCntr.l[0];
    inFields[6] = billingCntr.redCntr.l[1];
    inFields[7] = billingCntr.billingCntrMode;
    inFields[8] = billingCntr.billingCntrAllEnable;
    inFields[9] = billingCntr.lmCntrCaptureMode;
    inFields[10] = billingCntr.greenCntrSnapshot.l[0];
    inFields[11] = billingCntr.greenCntrSnapshot.l[1];
    inFields[12] = billingCntr.greenCntrSnapshotValid;
    inFields[13] = billingCntr.packetSizeMode;
    inFields[14] = billingCntr.tunnelTerminationPacketSizeMode;
    inFields[15] = billingCntr.dsaTagCountingMode;
    inFields[16] = billingCntr.timeStampCountingMode;

    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", inFields[0],inFields[1],inFields[2],
                inFields[3],inFields[4],inFields[5],inFields[6],inFields[7],
                inFields[8],inFields[9],inFields[10],inFields[11],inFields[12],
                inFields[13],inFields[14],inFields[15],inFields[16]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}
/*********************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerBillingEntryGetFirstV2
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    minBillingEntryXcatIndex=(GT_U32)inArgs[2];
    maxBillingEntryXcatIndex=(GT_U32)inArgs[3];
    billingEntryXcatIndex=minBillingEntryXcatIndex;

    return wrCpssDxCh3PolicerBillingEntryGetEntryV2(inArgs,inFields,numFields,outArgs);
}
/*********************************************************************************/
static CMD_STATUS wrCpssDxCh3PolicerBillingEntryGetNextV2
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
     billingEntryXcatIndex++;

     return wrCpssDxCh3PolicerBillingEntryGetEntryV2(inArgs,inFields,numFields,outArgs);

}

/*******************************************************************************
* cpssDxChPolicerCountingTriggerByPortEnableSet
*
* DESCRIPTION:
*       Enable/Disable Counting when port based metering is triggered for
*       the packet.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum      - device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       enable      - GT_TRUE - If counting mode is Billing, counting is performed
*                               for every packet subject to port based metering.
*                               If counting mode is not Billing, counting is
*                               performed for these packets only if the Policy
*                               engine enabled for Counting.
*                     GT_FALSE - Counting is performed for these packets only
*                                if the Policy engine enabled for Counting.
.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerCountingTriggerByPortEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPolicerCountingTriggerByPortEnableSet(devNum, plrStage, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerCountingTriggerByPortEnableGet
*
* DESCRIPTION:
*       Get Counting trigger by port status.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum          - device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       enablePtr   - GT_TRUE - If counting mode is Billing, counting is performed
*                               for every packet subject to port based metering.
*                               If counting mode is not Billing, counting is
*                               performed for these packets only if the Policy
*                               engine enabled for Counting.
*                     GT_FALSE - Counting is performed for these packets only
*                                if the Policy engine enabled for Counting.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_BAD_PTR                  - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerCountingTriggerByPortEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPolicerCountingTriggerByPortEnableGet(devNum, plrStage, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPolicerPortModeAddressSelectSet
*
* DESCRIPTION:
*       Configure Metering Address calculation type.
*       Relevant when stage mode is CPSS_DXCH_POLICER_STAGE_METER_MODE_PORT_E.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*       devNum      - device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       type        - Address select type: Full or Compressed.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or type.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerPortModeAddressSelectSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8                                                devNum;
    CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT     type;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    type = (CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPolicerPortModeAddressSelectSet(devNum, plrStage, type);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerPortModeAddressSelectGet
*
* DESCRIPTION:
*       Get Metering Address calculation type.
*       Relevant when stage mode is CPSS_DXCH_POLICER_STAGE_METER_MODE_PORT_E.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*       devNum      - device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       typePtr     - (pointer to) Address select type: Full or Compressed.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_BAD_PTR                  - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerPortModeAddressSelectGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                                               devNum;
    CPSS_DXCH_POLICER_PORT_MODE_ADDR_SELECT_TYPE_ENT    type;
    GT_STATUS                                           result;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPolicerPortModeAddressSelectGet(devNum, plrStage, &type);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", type);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChPolicerEAttributesMeteringModeSet
*
* DESCRIPTION:
*       Set ePort/eVLAN attributes metering mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       mode            - ePort/eVLAN Metering mode
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or mode.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerEAttributesMeteringModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;
    GT_U8                                       devNum;
    CPSS_DXCH_POLICER_E_ATTR_METER_MODE_ENT     mode;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT            stage;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    stage = (CPSS_DXCH_POLICER_STAGE_TYPE_ENT)inArgs[1];
    mode = (CPSS_DXCH_POLICER_E_ATTR_METER_MODE_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPolicerEAttributesMeteringModeSet(devNum, stage, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChPolicerEAttributesMeteringModeGet
*
* DESCRIPTION:
*       Get ePort/eVLAN attributes metering mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
*
* OUTPUTS:
*       modePtr         - (pointer to) ePort/eVLAN Metering mode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum or stage.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerEAttributesMeteringModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U8                                   devNum;
    CPSS_DXCH_POLICER_E_ATTR_METER_MODE_ENT mode;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT        stage;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    stage = (CPSS_DXCH_POLICER_STAGE_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPolicerEAttributesMeteringModeGet(devNum, stage, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************/
static void wrCpssDxChPolicerTriggerEntryBuild
(
    IN  GT_UINTPTR                                   inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR                                   inFields[CMD_MAX_FIELDS],
    OUT CPSS_DXCH_POLICER_TRIGGER_ENTRY_STC     *entryPtr
)
{
    entryPtr->meteringEnable = (GT_BOOL)inFields[0];
    entryPtr->countingEnable = (GT_BOOL)inFields[1];
    entryPtr->policerIndex = (GT_U32)inFields[2];
    entryPtr->ucKnownEnable = (GT_BOOL)inFields[3];
    entryPtr->ucKnownOffset = (GT_U32)inFields[4];
    entryPtr->ucUnknownEnable = (GT_BOOL)inFields[5];
    entryPtr->ucUnknownOffset = (GT_U32)inFields[6];
    entryPtr->mcRegisteredEnable = (GT_BOOL)inFields[7];
    entryPtr->mcRegisteredOffset = (GT_U32)inFields[8];
    entryPtr->mcUnregisteredEnable = (GT_BOOL)inFields[9];
    entryPtr->mcUnregisteredOffset = (GT_U32)inFields[10];
    entryPtr->bcEnable = (GT_BOOL)inFields[11];
    entryPtr->bcOffset = (GT_U32)inFields[12];
    entryPtr->tcpSynEnable = (GT_BOOL)inFields[13];
    entryPtr->tcpSynOffset = (GT_U32)inFields[14];
}

/*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerTriggerEntryParse
(
    IN  GT_UINTPTR                                   inFields[CMD_MAX_FIELDS],
    IN  GT_32                                   triggerEntryIndex,
    IN CPSS_DXCH_POLICER_TRIGGER_ENTRY_STC      entry,
    OUT GT_8                                    outArgs[CMD_MAX_BUFFER]
)
{
   inFields[0] = entry.meteringEnable;
   inFields[1] = entry.countingEnable;
   inFields[2] = entry.policerIndex;
   inFields[3] = entry.ucKnownEnable;
   inFields[4] = entry.ucKnownOffset;
   inFields[5] = entry.ucUnknownEnable;
   inFields[6] = entry.ucUnknownOffset;
   inFields[7] = entry.mcRegisteredEnable;
   inFields[8] = entry.mcRegisteredOffset;
   inFields[9] = entry.mcUnregisteredEnable;
   inFields[10] = entry.mcUnregisteredOffset;
   inFields[11] = entry.bcEnable;
   inFields[12] = entry.bcOffset;
   inFields[13] = entry.tcpSynEnable;
   inFields[14] = entry.tcpSynOffset;

   fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
               inFields[0],inFields[1],inFields[2],inFields[3],inFields[4],
               inFields[5],inFields[6],inFields[7],inFields[8],inFields[9],
               inFields[10],inFields[11],inFields[12],inFields[13],
               inFields[14]);

   /* pack output arguments to galtis string */
   galtisOutput(outArgs, GT_OK, "%f");

   return CMD_OK;
}

/**************Table:wrCpssDxChPolicerTriggerEntrySet********************************/
/*******************************************************************************
* wrCpssDxChPolicerTriggerEntrySet
*
* DESCRIPTION:
*       Set Policer trigger entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       entryIndex  - entry index
*       entryPtr    - (pointer to) Policer port trigger entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or portNum.
*       GT_BAD_PTR                  - on NULL pointer
*       GT_OUT_OF_RANGE             - on table index out of range.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerTriggerEntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                                   devNum;
    GT_STATUS                               result;
    GT_U32                                  entryIndex;
    CPSS_DXCH_POLICER_TRIGGER_ENTRY_STC     entry;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT        stage;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    stage  = (CPSS_DXCH_POLICER_STAGE_TYPE_ENT)inArgs[1];
    entryIndex = (GT_U32)inArgs[2];

    cmdOsMemSet(&entry, 0, sizeof(entry));

    /* build the entry */
    wrCpssDxChPolicerTriggerEntryBuild(inArgs,inFields,&entry);

    /* call cpss api function */
    result = cpssDxChPolicerTriggerEntrySet(devNum, stage,
                                            entryIndex, &entry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChPolicerTriggerEntryGet
*
* DESCRIPTION:
*       Get Policer trigger entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number.
*       stage       - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       entryIndex  - entry index
*
* OUTPUTS:
*       entryPtr    - (pointer to) Policer port trigger entry
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or portNum.
*       GT_BAD_PTR                  - on NULL pointer
*       GT_OUT_OF_RANGE             - on table index out of range.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerTriggerEntryGetEntry
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U8                                   devNum;
    CPSS_DXCH_POLICER_TRIGGER_ENTRY_STC     entry;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT        stage;
    GT_U32                                  triggerEntryIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

   /* map input arguments to locals */
   devNum = (GT_U8)inArgs[0];
   stage  = (CPSS_DXCH_POLICER_STAGE_TYPE_ENT)inArgs[1];
   triggerEntryIndex = (GT_U32)inArgs[2];

   /* reset entry to avoid trash in output for not used fields */
   cmdOsMemSet(&entry, 0, sizeof(entry));

   /* call cpss api function */
   result = cpssDxChPolicerTriggerEntryGet(devNum, stage,
                                           triggerEntryIndex,
                                           &entry);

   if (result != GT_OK)
   {
       galtisOutput(outArgs, result, "%d", -1);
       return CMD_OK;
   }
   cmdOsMemSet(inFields,0,CMD_MAX_FIELDS);

   return wrCpssDxChPolicerTriggerEntryParse(inFields,
                                             triggerEntryIndex,
                                             entry,
                                             outArgs);
}

/*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerTriggerEntryGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChPolicerTriggerEntryGetEntry(inArgs,inFields,numFields,outArgs);

}
/*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerTriggerEntryGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    galtisOutput(outArgs, CMD_OK, "%d", -1);
    return CMD_OK;
}


/**** database initialization **************************************/

/*******************************************************************************
* wrCpssDxChPolicerGetNext
*
* DESCRIPTION:
*       Default getNext Function to stop loop.

*
* APPLICABLE DEVICES: ALL DxCh Devices.
*
* INPUTS:
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or entryIndex.
*       GT_TIMEOUT      - on time out.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d", -1);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChPolicerFlowIdCountingCfgSet
*
* DESCRIPTION:
*        Set Flow Id based counting configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number.
*       stage               - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       billingIndexCfgPtr  - (pointer to) billing index configuration structure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum or stage
*       GT_BAD_PTR               - on NULL pointer.
*       GT_OUT_OF_RANGE          - on out of range data
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerFlowIdCountingCfgSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U8                                   devNum;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT        stage;
    CPSS_DXCH_POLICER_BILLING_INDEX_CFG_STC billingIndexCfg;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    stage  = (CPSS_DXCH_POLICER_STAGE_TYPE_ENT)inArgs[1];
    billingIndexCfg.billingIndexMode       =
        (CPSS_DXCH_POLICER_BILLING_INDEX_MODE_ENT)inArgs[2];
    billingIndexCfg.billingFlowIdIndexBase = (GT_U32)inArgs[3];
    billingIndexCfg.billingMinFlowId       = (GT_U32)inArgs[4];
    billingIndexCfg.billingMaxFlowId       = (GT_U32)inArgs[5];

    /* call cpss api function */
    result = cpssDxChPolicerFlowIdCountingCfgSet(
        devNum, stage, &billingIndexCfg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerFlowIdCountingCfgGet
*
* DESCRIPTION:
*        Get Flow Id based counting configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number.
*       stage               - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       billingIndexCfgPtr  - (pointer to) billing index configuration structure
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum or stage
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerFlowIdCountingCfgGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U8                                   devNum;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT        stage;
    CPSS_DXCH_POLICER_BILLING_INDEX_CFG_STC billingIndexCfg;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    stage  = (CPSS_DXCH_POLICER_STAGE_TYPE_ENT)inArgs[1];

    cpssOsMemSet(&billingIndexCfg, 0, sizeof(billingIndexCfg));

    /* call cpss api function */
    result = cpssDxChPolicerFlowIdCountingCfgGet(
        devNum, stage, &billingIndexCfg);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d %d %d %d",
        billingIndexCfg.billingIndexMode,
        billingIndexCfg.billingFlowIdIndexBase,
        billingIndexCfg.billingMinFlowId,
        billingIndexCfg.billingMaxFlowId);

    return CMD_OK;
}

/* cpssDxChPolicerHierarchicalTable */

/* current relative index for getNext - cleared by getFirst */
static GT_U32 prvCpssDxChPolicerHierarchicalTable_relativeIndexForGet = 0xFFFF;

/*******************************************************************************
* cpssDxChPolicerHierarchicalTableEntrySet
*
* DESCRIPTION:
*        Set Hierarchical Table Entry configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - device number.
*       index     - Hierarchical Table Entry index.
*       entryPtr  - (pointer to) Hierarchical Table Entry structure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum or stage or index
*       GT_BAD_PTR               - on NULL pointer.
*       GT_OUT_OF_RANGE          - on out of range data
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Applicable only to Ingress #0 Policer Stage.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerHierarchicalTableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       result;
    GT_U8                                           devNum;
    GT_U32                                          index;
    CPSS_DXCH_POLICER_HIERARCHICAL_TABLE_ENTRY_STC  entry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    cpssOsMemSet(&entry, 0, sizeof(entry));
    devNum               = (GT_U8)inArgs[0];
    index                = (GT_U32)inFields[0];
    entry.countingEnable = (GT_BOOL)inFields[1];
    entry.meteringEnable = (GT_BOOL)inFields[2];
    entry.policerPointer = (GT_U32)inFields[3];

    /* call cpss api function */
    result = cpssDxChPolicerHierarchicalTableEntrySet(
        devNum, index, &entry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerHierarchicalTableEntryGet
*
* DESCRIPTION:
*        Get Hierarchical Table Entry configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - device number.
*       index     - Hierarchical Table Entry index.
*
* OUTPUTS:
*       entryPtr  - (pointer to) Hierarchical Table Entry structure
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum or stage or index
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Applicable only to Ingress #0 Policer Stage.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerHierarchicalTableGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       result;
    GT_U8                                           devNum;
    GT_U32                                          baseIndex;
    GT_U32                                          amount;
    GT_U32                                          index;
    CPSS_DXCH_POLICER_HIERARCHICAL_TABLE_ENTRY_STC  entry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    cpssOsMemSet(&entry, 0, sizeof(entry));
    devNum     = (GT_U8)inArgs[0];
    baseIndex  = (GT_U32)inArgs[1];
    amount     = (GT_U32)inArgs[2];

    if (prvCpssDxChPolicerHierarchicalTable_relativeIndexForGet >= amount)
    {
        /* end of required entries range */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    index =
        baseIndex + prvCpssDxChPolicerHierarchicalTable_relativeIndexForGet;
    prvCpssDxChPolicerHierarchicalTable_relativeIndexForGet ++;

    /* call cpss api function */
    result = cpssDxChPolicerHierarchicalTableEntryGet(
        devNum, index, &entry);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    fieldOutput(
        "%d%d%d%d", index, entry.countingEnable,
        entry.meteringEnable, entry.policerPointer);
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

static CMD_STATUS wrCpssDxChPolicerHierarchicalTableGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    prvCpssDxChPolicerHierarchicalTable_relativeIndexForGet = 0;

    return wrCpssDxChPolicerHierarchicalTableGetNext(
        inArgs, inFields, numFields, outArgs);
}

/*******************************************************************************
* cpssDxChPolicerSecondStageIndexMaskSet
*
* DESCRIPTION:
*       Sets second stage policer index mask.
*
* APPLICABLE DEVICES:
*       Lion.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion2; Lion3.
*
* INPUTS:
*       devNum          - device number.
*       plrIndexMask    - mask for policer index (APPLICABLE RANGES: 0..0xFFF).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on illegal plrIndexMask
*
* COMMENTS:
*       Note: Lion device applicable starting from revision B2
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerSecondStageIndexMaskSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS           result;
    GT_U8               devNum;
    GT_U32              plrIndexMask;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    plrIndexMask = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPolicerSecondStageIndexMaskSet(devNum, plrIndexMask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPolicerSecondStageIndexMaskGet
*
* DESCRIPTION:
*       Gets second stage policer index mask.
*
* APPLICABLE DEVICES:
*       Lion.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion2; Lion3.
*
* INPUTS:
*       devNum          - device number.
*
* OUTPUTS:
*       plrIndexMaskPtr - (pointer to) mask for policer index.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       Note: Lion device applicable starting from revision B2
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerSecondStageIndexMaskGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS           result;
    GT_U8               devNum;
    GT_U32              plrIndexMask;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPolicerSecondStageIndexMaskGet(devNum, &plrIndexMask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", plrIndexMask);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerTrappedPacketsBillingEnableSet
*
* DESCRIPTION:
*       Enables the billing algorithm for Trapped packets.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       enable          - enable/disable billing for Trapped packets.
*                         GT_TRUE  - enable billing for Trapped packets.
*                         GT_FALSE - disable billing for Trapped packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerTrappedPacketsBillingEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPolicerTrappedPacketsBillingEnableSet(devNum, plrStage, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerTrappedPacketsBillingEnableGet
*
* DESCRIPTION:
*       Get state of the billing algorithm for Trapped packets.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       enablePtr       - (pointer to) enable/disable billing for Trapped packets.
*                         GT_TRUE  - enable billing for Trapped packets.
*                         GT_FALSE - disable billing for Trapped packets.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or stage.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerTrappedPacketsBillingEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPolicerTrappedPacketsBillingEnableGet(devNum, plrStage, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerLossMeasurementCounterCaptureEnableSet
*
* DESCRIPTION:
*       Enables or disables Loss Measurement capture.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       enable          - enable/disable Loss Measurement capture.
*                         GT_TRUE  - enable capture of Loss Measurement packets.
*                         GT_FALSE - disable capture of Loss Measurement packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum or stage
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerLossMeasurementCounterCaptureEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPolicerLossMeasurementCounterCaptureEnableSet(devNum, plrStage, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerLossMeasurementCounterCaptureEnableGet
*
* DESCRIPTION:
*       Get Loss Measurement capture state.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       enablePtr       - (pointer to)enable/disable Loss Measurement capture.
*                         GT_TRUE  - enable capture of Loss Measurement packets.
*                         GT_FALSE - disable capture of Loss Measurement packets.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - wrong devNum or stage.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerLossMeasurementCounterCaptureEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPolicerLossMeasurementCounterCaptureEnableGet(devNum, plrStage, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerPortStormTypeInFlowModeEnableSet
*
* DESCRIPTION:
*       Enable port-based metering triggered in Flow mode to work in
*       {storm-pkt-type, port} based, otherwise {port} based is used.
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       enable          - Enable\Disable Port metering according to storm type
*                         when working in flow mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum or stage
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerPortStormTypeInFlowModeEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT plrStage;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    plrStage = (CPSS_DXCH_POLICER_STAGE_TYPE_ENT)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPolicerPortStormTypeInFlowModeEnableSet(devNum, plrStage, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerPortStormTypeInFlowModeEnableGet
*
* DESCRIPTION:
*       Get if port-based metering triggered in Flow mode is enabled for
*       {storm-pkt-type, port} based (otherwise {port} based is used).
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - physical device number.
*       stage           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       enablePtr       - (pointer to) Enable status of Port metering according to storm type
*                         when working in flow mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum or stage
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerPortStormTypeInFlowModeEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT plrStage;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    plrStage = (CPSS_DXCH_POLICER_STAGE_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPolicerPortStormTypeInFlowModeEnableGet(devNum, plrStage, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerPacketSizeModeForTimestampTagSet
*
* DESCRIPTION:
*      Determines whether the timestamp tag should be included in the byte count
*      computation or not.
*      The configuration is used for IPFIX, VLAN and Management counters.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum              - physical device number.
*       stage               - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       timestampTagMode    - packet size counting mode for timestamp tag.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage or timestampTagMode.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerPacketSizeModeForTimestampTagSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;
    GT_U8                                       devNum;
    CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPolicerPacketSizeModeForTimestampTagSet(devNum, plrStage, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerPacketSizeModeForTimestampTagGet
*
* DESCRIPTION:
*      Retrieves whether the timestamp tag should be included in the byte count
*      computation or not.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum              - physical device number.
*       stage               - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*
* OUTPUTS:
*       timestampTagModePtr - (pointer to) packet size counting mode for timestamp tag.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, stage.
*       GT_BAD_PTR                  - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerPacketSizeModeForTimestampTagGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;
    GT_U8                                       devNum;
    CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPolicerPacketSizeModeForTimestampTagGet(devNum, plrStage, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerFlowBasedMeteringToFloodedOnlyEnableSet
*
* DESCRIPTION:
*       Enable/Disable Flow Based Metering to Flooded Traffic Only.
*
* APPLICABLE DEVICES:
*        xCat.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2.
*
* INPUTS:
*       devNum         - device number
*       enable         - GT_TRUE  - Enable: Flow Based Metering to Flooded Traffic Only.
*                        GT_FALSE - Disable: Flow Based Metering to All Traffic.
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerFlowBasedMeteringToFloodedOnlyEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPolicerFlowBasedMeteringToFloodedOnlyEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerFlowBasedMeteringToFloodedOnlyEnableGet
*
* DESCRIPTION:
*       Gets Enable/Disable Flow Based Metering to Flooded Traffic Only.
*
* APPLICABLE DEVICES:
*        xCat.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2.
*
* INPUTS:
*       devNum         - device number
* OUTPUTS:
*       enablePtr      - (pointer to)
*                        GT_TRUE  - Enable: Flow Based Metering to Flooded Traffic Only.
*                        GT_FALSE - Disable: Flow Based Metering to All Traffic.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerFlowBasedMeteringToFloodedOnlyEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPolicerFlowBasedMeteringToFloodedOnlyEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerFlowBasedBillingToFloodedOnlyEnableSet
*
* DESCRIPTION:
*       Enable/Disable Flow Based Billing to Flooded Traffic Only.
*
* APPLICABLE DEVICES:
*        xCat.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2.
*
* INPUTS:
*       devNum         - device number
*       enable         - GT_TRUE  - Enable: Flow Based Billing to Flooded Traffic Only.
*                        GT_FALSE - Disable: Flow Based Billing to All Traffic.
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerFlowBasedBillingToFloodedOnlyEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPolicerFlowBasedBillingToFloodedOnlyEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerFlowBasedBillingToFloodedOnlyEnableGet
*
* DESCRIPTION:
*       Gets Enable/Disable Flow Based Billing to Flooded Traffic Only.
*
* APPLICABLE DEVICES:
*        xCat.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2.
*
* INPUTS:
*       devNum         - device number
* OUTPUTS:
*       enablePtr      - (pointer to)
*                        GT_TRUE  - Enable: Flow Based Billing to Flooded Traffic Only.
*                        GT_FALSE - Disable: Flow Based Billing to All Traffic.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerFlowBasedBillingToFloodedOnlyEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPolicerFlowBasedBillingToFloodedOnlyEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/* MEF10.3 commands start */

/* static valiables for table state mashines */
static GT_U32                                 bucketEnvelop_entryIndex;
static CPSS_DXCH3_POLICER_METERING_ENTRY_STC  bucketEnvelop_entryArr[8];
static CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT bucketEnvelop_tbParamsArr[8];
static const GT_U32                           bucketEnvelop_entryIndexLimit = 8;
static GT_U32                                 bucketEnvelop_envelopeSize;
static GT_BOOL                                bucketEnvelop_couplingFlag0;
static GT_U32                                 bucketEnvelop_startQosProfileIndex;
static GT_U32                                 bucketEnvelop_numOfQosProfiles;
static GT_U32                                 bucketEnvelop_maxRateEntryIndex;
static GT_U32                                 bucketEnvelop_maxRateBucketIndex;
static GT_U32                                 bucketEnvelop_maxRateNumOfEntries;
static GT_U32                                 bucketEnvelop_maxRateBucketMask;

/* retrieve fields from command line */
static void prvWrCpssDxChPolicerMeteringEntryEnvelopeFieldsFromCmdLine
(
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS]
)
{
    CPSS_DXCH3_POLICER_METERING_ENTRY_STC *entryPtr;

    entryPtr = &(bucketEnvelop_entryArr[bucketEnvelop_entryIndex]);

    entryPtr->countingEntryIndex=(GT_U32)inFields[0];
    entryPtr->mngCounterSet=(CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT)inFields[1];
    entryPtr->meterColorMode=(CPSS_POLICER_COLOR_MODE_ENT)inFields[2];
    entryPtr->meterMode = (bucketEnvelop_entryIndex == 0)
        ? CPSS_DXCH3_POLICER_METER_MODE_START_OF_ENVELOPE_E
        : CPSS_DXCH3_POLICER_METER_MODE_NOT_START_OF_ENVELOPE_E;
    entryPtr->tokenBucketParams.envelope.cir    =(GT_U32)inFields[3];
    entryPtr->tokenBucketParams.envelope.cbs    =(GT_U32)inFields[4];
    entryPtr->tokenBucketParams.envelope.eir    =(GT_U32)inFields[5];
    entryPtr->tokenBucketParams.envelope.ebs    =(GT_U32)inFields[6];
    entryPtr->tokenBucketParams.envelope.maxCir =(GT_U32)inFields[7];
    entryPtr->tokenBucketParams.envelope.maxEir =(GT_U32)inFields[8];
    entryPtr->modifyUp = (CPSS_DXCH_POLICER_MODIFY_UP_ENT)inFields[9];
    entryPtr->modifyDscp = (CPSS_DXCH_POLICER_MODIFY_DSCP_ENT)inFields[10];
    entryPtr->modifyDp = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[11];
    entryPtr->modifyExp = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[12];
    entryPtr->modifyTc = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[13];
    entryPtr->greenPcktCmd = (CPSS_DXCH3_POLICER_NON_CONFORM_CMD_ENT)inFields[14];
    entryPtr->yellowPcktCmd = (CPSS_DXCH3_POLICER_NON_CONFORM_CMD_ENT)inFields[15];
    entryPtr->redPcktCmd = (CPSS_DXCH3_POLICER_NON_CONFORM_CMD_ENT)inFields[16];
    entryPtr->qosProfile = (GT_U32)inFields[17];
    entryPtr->remarkMode = (CPSS_DXCH_POLICER_REMARK_MODE_ENT)inFields[18];

    entryPtr->byteOrPacketCountingMode =
                        (CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT)inFields[19];
    entryPtr->packetSizeMode = (CPSS_POLICER_PACKET_SIZE_MODE_ENT)inFields[20];
    entryPtr->tunnelTerminationPacketSizeMode =
                        (CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT)inFields[21];
    entryPtr->dsaTagCountingMode =
                        (CPSS_DXCH3_POLICER_DSA_TAG_COUNTING_MODE_ENT)inFields[22];
    entryPtr->timeStampCountingMode =
                        (CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT)inFields[23];
    entryPtr->yellowEcnMarkingEnable = (GT_BOOL)inFields[24];
    entryPtr->couplingFlag = (GT_BOOL)inFields[25];
    entryPtr->maxRateIndex = (GT_U32)inFields[26];
}

/* put fields to reply line */
static void prvWrCpssDxChPolicerMeteringEntryEnvelopeFieldsToReplyLine()
{
    CPSS_DXCH3_POLICER_METERING_ENTRY_STC *entryPtr;

    entryPtr = &(bucketEnvelop_entryArr[bucketEnvelop_entryIndex]);

    fieldOutput(
        "%d%d%d",
        (GT_U32)entryPtr->countingEntryIndex,
        (GT_U32)entryPtr->mngCounterSet,
        (GT_U32)entryPtr->meterColorMode);

    fieldOutputSetAppendMode();
    fieldOutput(
        "%d%d%d%d%d%d",
        (GT_U32)entryPtr->tokenBucketParams.envelope.cir,
        (GT_U32)entryPtr->tokenBucketParams.envelope.cbs,
        (GT_U32)entryPtr->tokenBucketParams.envelope.eir,
        (GT_U32)entryPtr->tokenBucketParams.envelope.ebs,
        (GT_U32)entryPtr->tokenBucketParams.envelope.maxCir,
        (GT_U32)entryPtr->tokenBucketParams.envelope.maxEir);

    fieldOutputSetAppendMode();
    fieldOutput(
        "%d%d%d%d%d",
        (GT_U32)entryPtr->modifyUp,
        (GT_U32)entryPtr->modifyDscp,
        (GT_U32)entryPtr->modifyDp,
        (GT_U32)entryPtr->modifyExp,
        (GT_U32)entryPtr->modifyTc);

    fieldOutputSetAppendMode();
    fieldOutput(
        "%d%d%d%d%d",
        (GT_U32)entryPtr->greenPcktCmd,
        (GT_U32)entryPtr->yellowPcktCmd,
        (GT_U32)entryPtr->redPcktCmd,
        (GT_U32)entryPtr->qosProfile,
        (GT_U32)entryPtr->remarkMode);

    fieldOutputSetAppendMode();
    fieldOutput(
        "%d%d%d%d%d%d",
        (GT_U32)entryPtr->byteOrPacketCountingMode,
        (GT_U32)entryPtr->packetSizeMode,
        (GT_U32)entryPtr->tunnelTerminationPacketSizeMode,
        (GT_U32)entryPtr->dsaTagCountingMode,
        (GT_U32)entryPtr->timeStampCountingMode,
        (GT_U32)entryPtr->yellowEcnMarkingEnable);

    fieldOutputSetAppendMode();
    fieldOutput(
        "%d%d",
        (GT_U32)entryPtr->couplingFlag,
        (GT_U32)entryPtr->maxRateIndex);
}

/* save first command line data */
static CMD_STATUS wrCpssDxChPolicerMeteringEntryEnvelopeSetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    cpssOsMemSet(bucketEnvelop_entryArr, 0, sizeof(bucketEnvelop_entryArr));
    cpssOsMemSet(bucketEnvelop_tbParamsArr, 0, sizeof(bucketEnvelop_tbParamsArr));
    bucketEnvelop_entryIndex = 0;
    prvWrCpssDxChPolicerMeteringEntryEnvelopeFieldsFromCmdLine(inFields);
    bucketEnvelop_entryIndex ++;
    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/* save next command lines data */
static CMD_STATUS wrCpssDxChPolicerMeteringEntryEnvelopeSetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    if (bucketEnvelop_entryIndex >= bucketEnvelop_entryIndexLimit)
    {
        galtisOutput(outArgs, GT_BAD_PARAM, "%d", -1);
        return CMD_OK;
    }
    prvWrCpssDxChPolicerMeteringEntryEnvelopeFieldsFromCmdLine(inFields);
    bucketEnvelop_entryIndex ++;
    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/* save last command line data and store to HW */
static CMD_STATUS wrCpssDxChPolicerMeteringEntryEnvelopeEndSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                                       devNum;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT            stage;
    GT_U32                                      startEntryIndex;
    GT_BOOL                                     couplingFlag0;
    GT_STATUS                                   rc;

    devNum             = (GT_U8)                           inArgs[0];
    stage              = (CPSS_DXCH_POLICER_STAGE_TYPE_ENT)inArgs[1];
    startEntryIndex    = (GT_U32)                          inArgs[2];
    couplingFlag0      = (GT_BOOL)                         inArgs[3];

    rc = cpssDxChPolicerMeteringEntryEnvelopeSet(
        devNum, stage, startEntryIndex,
        bucketEnvelop_entryIndex /*envelopeSize*/, couplingFlag0,
        bucketEnvelop_entryArr, bucketEnvelop_tbParamsArr);

    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

/* Retrieve all envelope from HW, send first line to reply line */
static CMD_STATUS wrCpssDxChPolicerMeteringEntryEnvelopeGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                                       devNum;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT            stage;
    GT_U32                                      startEntryIndex;
    GT_STATUS                                   rc;

    devNum             = (GT_U8)                           inArgs[0];
    stage              = (CPSS_DXCH_POLICER_STAGE_TYPE_ENT)inArgs[1];
    startEntryIndex    = (GT_U32)                          inArgs[2];

    cpssOsMemSet(bucketEnvelop_entryArr, 0, sizeof(bucketEnvelop_entryArr));
    bucketEnvelop_entryIndex = 0;

    rc = cpssDxChPolicerMeteringEntryEnvelopeGet(
        devNum, stage, startEntryIndex,
        8 /*maxEnvelopeSize*/,
        &bucketEnvelop_envelopeSize /*envelopeSizePtr*/,
        &bucketEnvelop_couplingFlag0 /*couplingFlag0Ptr*/,
        bucketEnvelop_entryArr);

    if (rc != GT_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    prvWrCpssDxChPolicerMeteringEntryEnvelopeFieldsToReplyLine();
    bucketEnvelop_entryIndex ++;

    galtisOutput(outArgs, rc, "%f");
    return CMD_OK;
}

/* send first next to reply line */
static CMD_STATUS wrCpssDxChPolicerMeteringEntryEnvelopeGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    if (bucketEnvelop_entryIndex >= bucketEnvelop_envelopeSize)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);

        cmdOsPrintf(
            "\nenvelopeSize: %d, couplingFlag0: %d\n",
            bucketEnvelop_envelopeSize, bucketEnvelop_couplingFlag0);

        return CMD_OK;
    }

    prvWrCpssDxChPolicerMeteringEntryEnvelopeFieldsToReplyLine();
    bucketEnvelop_entryIndex ++;

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerMeterTableFlowBasedIndexConfigSet
*
* DESCRIPTION:
*       Set Policy Pointer threshold and maximal envelope size for using the
*       Flow based algorithm. For Policy Pointer values greater or equal
*       than threshold the Priority based algorithm used.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                          - device number
*       stage                           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       threshold                       - upper threshold of Policy Pointer for Flow Based algorithm.
*                                         (APPLICABLE RANGES: 0..0x1FFFF)
*       maxSize                         - maximal envelope size and alignment.
*                                         the index of start entry of each envelope
*                                         accessed by Flow Based Algorithm should be
*                                         aligned to maxSize.
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerMeterTableFlowBasedIndexConfigSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                result;
    GT_U8                                    devNum;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage;
    GT_U32                                   threshold;
    CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_ENT  maxSize;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum    = (GT_U8)                                  inArgs[0];
    stage     = (CPSS_DXCH_POLICER_STAGE_TYPE_ENT)       inArgs[1];
    threshold = (GT_U32)                                 inArgs[2];
    maxSize   = (CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPolicerMeterTableFlowBasedIndexConfigSet(
        devNum, stage, threshold, maxSize);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerMeterTableFlowBasedIndexConfigGet
*
* DESCRIPTION:
*       Get Policy Pointer threshold and maximal envelope size for using the
*       Flow based algorithm. For Policy Pointer values greater or equal
*       than threshold the Priority based algorithm used.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                          - device number
*       stage                           - Policer Stage type: Ingress #0, Ingress #1 or Egress.
* OUTPUTS:
*      thresholdPtr                     - (poiner to) upper threshold
*                                         of Policy Pointer for Flow Based algorithm.
*      maxSizePtr                       - (poiner to) maximal envelope size and alignment.
*                                         the index of start entry of each envelope
*                                         accessed by Flow Based Algorithm should be
*                                         aligned to maxSize.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_STATE             - on wrong HW data
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerMeterTableFlowBasedIndexConfigGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                result;
    GT_U8                                    devNum;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage;
    GT_U32                                   threshold;
    CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_ENT  maxSize;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum    = (GT_U8)                                  inArgs[0];
    stage     = (CPSS_DXCH_POLICER_STAGE_TYPE_ENT)       inArgs[1];
    threshold = (GT_U32)                                 inArgs[2];
    maxSize   = (CPSS_DXCH_POLICER_ENVELOPE_MAX_SIZE_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPolicerMeterTableFlowBasedIndexConfigGet(
        devNum, stage, &threshold, &maxSize);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", threshold, maxSize);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerQosProfileToPriorityMapSet
*
* DESCRIPTION:
*       Set entry of Qos Profile Mapping to Priority table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       stage             - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       qosProfileIndex   - Qos Profile index.
*                           (APPLICABLE RANGES: 0..1023)
*       priority          - priority.
*                           (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerQosProfileToPriorityMapSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;
    GT_U8                              devNum;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT   stage;
    GT_U32                             qosProfileIndex;
    GT_U32                             priority;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)                             inArgs[0];
    stage           = (CPSS_DXCH_POLICER_STAGE_TYPE_ENT)  inArgs[1];
    qosProfileIndex = (GT_U32)                            inFields[0];
    priority        = (GT_U32)                            inFields[1];

    /* call cpss api function */
    result = cpssDxChPolicerQosProfileToPriorityMapSet(
        devNum, stage, qosProfileIndex, priority);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerQosProfileToPriorityMapGet
*
* DESCRIPTION:
*       Get entry of Qos Profile Mapping to Priority table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       stage             - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       qosProfileIndex   - Qos Profile index.
*                           (APPLICABLE RANGES: 0..1023)
*
* OUTPUTS:
*       priorityPtr       - (pointer to) priority.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS prvWrCpssDxChPolicerQosProfileToPriorityMapGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                result;
    GT_U8                                    devNum;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage;
    GT_U32                                   priority;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)                             inArgs[0];
    stage           = (CPSS_DXCH_POLICER_STAGE_TYPE_ENT)  inArgs[1];

    /* call cpss api function */
    result = cpssDxChPolicerQosProfileToPriorityMapGet(
        devNum, stage, bucketEnvelop_startQosProfileIndex, &priority);

    /* pack output arguments to galtis string */
    fieldOutput("%d%d", bucketEnvelop_startQosProfileIndex, priority);
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

static CMD_STATUS wrCpssDxChPolicerQosProfileToPriorityMapGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    bucketEnvelop_startQosProfileIndex = (GT_U32)inArgs[2];
    bucketEnvelop_numOfQosProfiles     = (GT_U32)inArgs[3];

    if (bucketEnvelop_numOfQosProfiles == 0)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }
    return prvWrCpssDxChPolicerQosProfileToPriorityMapGet(
        inArgs, inFields, numFields, outArgs);
}

static CMD_STATUS wrCpssDxChPolicerQosProfileToPriorityMapGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    bucketEnvelop_startQosProfileIndex ++;
    bucketEnvelop_numOfQosProfiles     --;

    if (bucketEnvelop_numOfQosProfiles == 0)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }
    return prvWrCpssDxChPolicerQosProfileToPriorityMapGet(
        inArgs, inFields, numFields, outArgs);
}

/*******************************************************************************
* cpssDxChPolicerTokenBucketMaxRateSet
*
* DESCRIPTION:
*       Set the value of the Maximal Rate in the Maximal Rate table entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       stage             - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       entryIndex        - Bucket Max Rate table index.
*                           (APPLICABLE RANGES: 0..127)
*       bucketIndex       - Bucket index.
*                           (APPLICABLE RANGES: 0..1)
*       meterEntryRate    - CIR or EIR of meter entry that use this
*                           maximal rate in Kbps (1Kbps = 1000 bits per second)
*       meterEntryRate    - CBS or EBS of meter entry that use this maximal rate in bytes.
*       maxRate           - maximal Information Rate in Kbps (1Kbps = 1000 bits per second)
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       See MEF10.3 examples in FS to know relation between bucket0/1 and Commited/Exess
*******************************************************************************/
static CMD_STATUS wrCpssDxChPolicerTokenBucketMaxRateSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                result;
    GT_U8                                    devNum;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage;
    GT_U32                                   entryIndex;
    GT_U32                                   bucketIndex;
    GT_U32                                   meterEntryRate;
    GT_U32                                   meterEntryBurstSize;
    GT_U32                                   maxRate;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum                  = (GT_U8)                             inArgs[0];
    stage                   = (CPSS_DXCH_POLICER_STAGE_TYPE_ENT)  inArgs[1];
    meterEntryRate          = (GT_U32)                            inArgs[2];
    meterEntryBurstSize     = (GT_U32)                            inArgs[3];
    entryIndex              = (GT_U32)                            inFields[0];
    bucketIndex             = (GT_U32)                            inFields[1];
    maxRate                 = (GT_U32)                            inFields[2];

    /* call cpss api function */
    result = cpssDxChPolicerTokenBucketMaxRateSet(
        devNum, stage, entryIndex, bucketIndex,
        meterEntryRate, meterEntryBurstSize, maxRate);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPolicerTokenBucketMaxRateGet
*
* DESCRIPTION:
*       Get the value of the Maximal Rate in the Maximal Rate table entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       stage             - Policer Stage type: Ingress #0, Ingress #1 or Egress.
*       entryIndex        - Bucket Max Rate table index.
*                           (APPLICABLE RANGES: 0..127)
*       bucketIndex       - Bucket index.
*                           (APPLICABLE RANGES: 0..1)
*       meterEntryRate    - CIR or EIR of meter entry that use this
*                           maximal rate in Kbps (1Kbps = 1000 bits per second)
*       meterEntryRate    - CBS or EBS of meter entry that use this maximal rate in bytes.
*
* OUTPUTS:
*       maxRatePtr        - (pointer to) maximal Information Rate in Kbps (1Kbps = 1000 bits per second)
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS prvWrCpssDxChPolicerTokenBucketMaxRateGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                result;
    GT_U8                                    devNum;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT         stage;
    GT_U32                                   meterEntryRate;
    GT_U32                                   meterEntryBurstSize;
    GT_U32                                   maxRate;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum                  = (GT_U8)                             inArgs[0];
    stage                   = (CPSS_DXCH_POLICER_STAGE_TYPE_ENT)  inArgs[1];
    meterEntryRate          = (GT_U32)                            inArgs[2];
    meterEntryBurstSize     = (GT_U32)                            inArgs[3];

    /* call cpss api function */
    result = cpssDxChPolicerTokenBucketMaxRateGet(
        devNum, stage,
        bucketEnvelop_maxRateEntryIndex, bucketEnvelop_maxRateBucketIndex,
        meterEntryRate, meterEntryBurstSize, &maxRate);

    /* pack output arguments to galtis string */
    fieldOutput(
        "%d%d%d", bucketEnvelop_maxRateEntryIndex,
        bucketEnvelop_maxRateBucketIndex, maxRate);
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/* state mashine for next table line                                           */
/* bucketEnvelop_maxRateBucketIndex >= 32 means look for first bit in the mask */
/* bucketEnvelop_maxRateNumOfEntries == 0 means the end of table               */
static void prvWrCpssDxChPolicerTokenBucketMaxRateNextIndex()
{
    GT_U32 i;
    GT_U32 firstBit;

    if (bucketEnvelop_maxRateBucketMask == 0)
    {
        /* probably error, detected by caller */
        bucketEnvelop_maxRateNumOfEntries = 0;
        return;
    }
    if (bucketEnvelop_maxRateNumOfEntries == 0)
    {
        return;
    }
    firstBit = (bucketEnvelop_maxRateBucketIndex + 1);
    if (firstBit >= 32)
    {
        firstBit = 0;
    }
    for (i = firstBit; (i < 32); i++)
    {
        if ((1 << i) & bucketEnvelop_maxRateBucketMask)
        {
            /* next bucket in the same entry */
            bucketEnvelop_maxRateBucketIndex = i;
            return;
        }
    }
    /* (i >= 32) ==> pass to next line */
    bucketEnvelop_maxRateEntryIndex ++;
    bucketEnvelop_maxRateNumOfEntries --;
    if (bucketEnvelop_maxRateNumOfEntries == 0)
    {
        return;
    }
    for (i = 0; (i < 32); i++)
    {
        if ((1 << i) & bucketEnvelop_maxRateBucketMask)
        {
            /* next bucket in the same entry */
            bucketEnvelop_maxRateBucketIndex = i;
            return;
        }
    }
}

static CMD_STATUS wrCpssDxChPolicerTokenBucketMaxRateGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    prvWrCpssDxChPolicerTokenBucketMaxRateNextIndex();
    if (bucketEnvelop_maxRateNumOfEntries == 0)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }
    return prvWrCpssDxChPolicerTokenBucketMaxRateGet(
        inArgs, inFields, numFields, outArgs);
}

static CMD_STATUS wrCpssDxChPolicerTokenBucketMaxRateGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    bucketEnvelop_maxRateEntryIndex    = (GT_U32)inArgs[4];
    bucketEnvelop_maxRateNumOfEntries  = (GT_U32)inArgs[5];
    bucketEnvelop_maxRateBucketMask    = (GT_U32)inArgs[6];
    bucketEnvelop_maxRateBucketIndex   = 0xFF;

    return wrCpssDxChPolicerTokenBucketMaxRateGetNext(
        inArgs, inFields, numFields, outArgs);
}

/* MEF10.3 commands end */

/* current number of line*/
static GT_U32 currentBucketsState_relativeIndex = 0;

static CMD_STATUS wrCpssDxChPolicerBucketsCurrentStateGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                                       devNum;
    CPSS_DXCH_POLICER_STAGE_TYPE_ENT            stage;
    CPSS_DXCH_POLICER_BUCKETS_STATE_STC         bucketsState;
    GT_U32                                      baseIndex;
    GT_U32                                      numOfEntries;

    devNum       = (GT_U8)inArgs[0];
    stage        = (CPSS_DXCH_POLICER_STAGE_TYPE_ENT)inArgs[1];
    baseIndex    = (GT_U32)inArgs[2];
    numOfEntries = (GT_U32)inArgs[3];

    if (currentBucketsState_relativeIndex >= numOfEntries)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }
    rc = pg_wrap_cpssDxChPolicerBucketsCurrentStateGet(
        devNum, stage,
        (baseIndex + currentBucketsState_relativeIndex),
        &bucketsState);
    if (rc != GT_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    fieldOutput(
        "%d%d%d%d%d%d%d%d",
       bucketsState.lastTimeUpdate0,
       bucketsState.lastTimeUpdate1,
       bucketsState.wrapAround0,
       bucketsState.wrapAround1,
       bucketsState.bucketSize0,
       bucketsState.bucketSize1,
       bucketsState.bucketSignPositive0,
       bucketsState.bucketSignPositive1);
    galtisOutput(outArgs, GT_OK, "%f");

    currentBucketsState_relativeIndex ++;

    return CMD_OK;
}
static CMD_STATUS wrCpssDxChPolicerBucketsCurrentStateGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    currentBucketsState_relativeIndex = 0;

    return wrCpssDxChPolicerBucketsCurrentStateGetNext(
        inArgs, inFields, numFields, outArgs);
}

static CMD_COMMAND dbCommands[] =
{
        {"cpssDxCh3PolicerMeteringEnableSet",
         &wrCpssDxCh3PolicerMeteringEnableSet,
         2, 0},
        {"cpssDxCh3PolicerMeteringEnableGet",
         &wrCpssDxCh3PolicerMeteringEnableGet,
         1, 0},
        {"cpssDxCh3PolicerBillingCountingEnableSet",
         &wrCpssDxCh3PolicerBillingCountingEnableSet,
         2, 0},
        {"cpssDxCh3PolicerBillingCountingEnableGet",
         &wrCpssDxCh3PolicerBillingCountingEnableGet,
         1, 0},
        {"cpssDxCh3PolicerBillingCountersModeSet",
         &wrCpssDxCh3PolicerBillingCountersModeSet,
         2, 0},
        {"cpssDxCh3PolicerBillingCountersModeGet",
         &wrCpssDxCh3PolicerBillingCountersModeGet,
         1, 0},
        {"cpssDxCh3PolicerPacketSizeModeSet",
         &wrCpssDxCh3PolicerPacketSizeModeSet,
         2, 0},
        {"cpssDxCh3PolicerPacketSizeModeGet",
         &wrCpssDxCh3PolicerPacketSizeModeGet,
         1, 0},
        {"cpssDxCh3PolicerMeterResolutionSet",
         &wrCpssDxCh3PolicerMeterResolutionSet,
         2, 0},
        {"cpssDxCh3PolicerMeterResolutionGet",
         &wrCpssDxCh3PolicerMeterResolutionGet,
         1, 0},
        {"cpssDxCh3PolicerDropTypeSet",
         &wrCpssDxCh3PolicerDropTypeSet,
         2, 0},
        {"cpssDxCh3PolicerDropTypeGet",
         &wrCpssDxCh3PolicerDropTypeGet,
         1, 0},
        {"cpssDxCh3PolicerCountingColorModeSet",
         &wrCpssDxCh3PolicerCountingColorModeSet,
         2, 0},
        {"cpssDxCh3PolicerCountingColorModeGet",
         &wrCpssDxCh3PolicerCountingColorModeGet,
         1, 0},
        {"cpssDxCh3PolicerManagementCntrsResolutionSet",
         &wrCpssDxCh3PolicerManagementCntrsResolutionSet,
         3, 0},
        {"cpssDxCh3PolicerManagementCntrsResolutionGet",
         &wrCpssDxCh3PolicerManagementCntrsResolutionGet,
         2, 0},
        {"cpssDxCh3PolicerPacketSizeModeForTunnelTermSet",
         &wrCpssDxCh3PolicerPacketSizeModeForTunnelTermSet,
         2, 0},
        {"cpssDxCh3PolicerPacketSizeModeForTunnelTermGet",
         &wrCpssDxCh3PolicerPacketSizeModeForTunnelTermGet,
         1, 0},
        {"cpssDxCh3PolicerMeteringAutoRefreshScanEnableSet",
         &wrCpssDxCh3PolicerMeteringAutoRefreshScanEnableSet,
         2, 0},
        {"cpssDxCh3PolicerMeteringAutoRefreshScanEnableGet",
         &wrCpssDxCh3PolicerMeteringAutoRefreshScanEnableGet,
         1, 0},
        {"cpssDxCh3PolicerMeteringAutoRefreshRangeSet",
         &wrCpssDxCh3PolicerMeteringAutoRefreshRangeSet,
         3, 0},
        {"cpssDxCh3PolicerMeteringAutoRefreshRangeGet",
         &wrCpssDxCh3PolicerMeteringAutoRefreshRangeGet,
         1, 0},
        {"cpssDxCh3PolicerMeteringAutoRefreshIntervalSet",
         &wrCpssDxCh3PolicerMeteringAutoRefreshIntervalSet,
         2, 0},
        {"cpssDxCh3PolicerMeteringAutoRefreshIntervalGet",
         &wrCpssDxCh3PolicerMeteringAutoRefreshIntervalGet,
         1, 0},
        {"cpssDxCh3PolicerMeterEntryRefresh",
         &wrCpssDxCh3PolicerMeteringEntryRefresh,
         2, 0},
        {"cpssDxCh3PolicerPortMeteringEnableSet",
         &wrCpssDxCh3PolicerPortMeteringEnableSet,
         3, 0},
        {"cpssDxCh3PolicerPortMeteringEnableGet",
         &wrCpssDxCh3PolicerPortMeteringEnableGet,
         2, 0},
        {"cpssDxCh3PolicerMruSet",
         &wrCpssDxCh3PolicerMruSet,
         2, 0},
        {"cpssDxCh3PolicerMruGet",
         &wrCpssDxCh3PolicerMruGet,
         1, 0},
        {"cpssDxCh3PolicerErrorGet",
         &wrCpssDxCh3PolicerErrorGet,
         1, 0},
        {"cpssDxCh3PolicerErrorCounterGet",
         &wrCpssDxCh3PolicerErrorCounterGet,
         1, 0},
        {"cpssDxCh3PolicerManagementCountersSet",
         &wrCpssDxCh3PolicerManagementCountersSet,
         5, 0},
        {"cpssDxCh3PolicerManagementCountersGet",
         &wrCpssDxCh3PolicerManagementCountersGet,
         3, 0},
        {"cpssDxCh3PolicerMeteringEntrySet",
         &wrCpssDxCh3PolicerMeteringEntrySet,
         3, 16},
        {"cpssDxCh3PolicerMeteringEntryGetFirst",
         &wrCpssDxCh3PolicerMeteringEntryGetFirst,
         3, 0},
        {"cpssDxCh3PolicerMeteringEntryGetNext",
         &wrCpssDxCh3PolicerMeteringEntryGetNext,
         3, 0},
        {"cpssDxCh3PolicerEntryMeterParamsCalculate",
         &wrCpssDxCh3PolicerEntryMeterParamsCalculate,
         9, 0},
        {"cpssDxCh3PolicerBillingEntrySet",
         &wrCpssDxCh3PolicerBillingEntrySet,
         4, 4},
        {"cpssDxCh3PolicerBillingEntryGetFirst",
         &wrCpssDxCh3PolicerBillingEntryGetFirst,
         4, 0},
        {"cpssDxCh3PolicerBillingEntryGetNext",
         &wrCpssDxCh3PolicerBillingEntryGetNext,
         4, 0},
        {"cpssDxCh3PolicerQosRemarkingEntrySet",
         &wrCpssDxCh3PolicerQosRemarkingEntrySet,
         3, 4},
        {"cpssDxCh3PolicerQosRemarkingEntryGetFirst",
         &wrCpssDxCh3PolicerQosRemarkingEntryGetFirst,
         3, 0},
        {"cpssDxCh3PolicerQosRemarkingEntryGetNext",
         &wrCpssDxCh3PolicerQosRemarkingEntryGetNext,
         3, 0},
        /* DxChXcat and above */
        {"cpssDxChPolicerCountingModeSet",
         &wrCpssDxChPolicerCountingModeSet,
         2, 0},
        {"cpssDxChPolicerCountingModeGet",
         &wrCpssDxChPolicerCountingModeGet,
         1, 0},
        {"cpssDxChPolicerEgressQosRemarkingEntrySet",
         &wrCpssDxChPolicerEgressQosRemarkingEntrySet,
         4, 3},
        {"cpssDxChPolicerEgressQosRemarkingEntryGetFirst",
         &wrCpssDxChPolicerEgressQosRemarkingEntryGet,
         4, 0},
        {"cpssDxChPolicerEgressQosRemarkingEntryGetNext",
         &wrCpssDxChPolicerGetNext,
         4, 0},
        {"cpssDxChPolicerStageMeterModeSet",
         &wrCpssDxChPolicerStageMeterModeSet,
         2, 0},
        {"cpssDxChPolicerStageMeterModeGet",
         &wrCpssDxChPolicerStageMeterModeGet,
         1, 0},
        {"cpssDxChPolicerMeteringOnTrappedPktsEnableSet",
         &wrCpssDxChPolicerMeteringOnTrappedPktsEnableSet,
         2, 0},
        {"cpssDxChPolicerMeteringOnTrappedPktsEnableGet",
         &wrCpssDxChPolicerMeteringOnTrappedPktsEnableGet,
         1, 0},
        {"cpssDxChPolicerPortStormTypeIndexSet",
         &wrCpssDxChPolicerPortStormTypeIndexSet,
         4, 0},
        {"cpssDxChPolicerPortStormTypeIndexGet",
         &wrCpssDxChPolicerPortStormTypeIndexGet,
         3, 0},
        {"cpssDxChPolicerEgressL2RemarkModelSet",
         &wrCpssDxChPolicerEgressL2RemarkModelSet,
         2, 0},
        {"cpssDxChPolicerEgressL2RemarkModelGet",
         &wrCpssDxChPolicerEgressL2RemarkModelGet,
         1, 0},
        {"cpssDxChPolicerEgressQosUpdateEnableSet",
         &wrCpssDxChPolicerEgressQosUpdateEnableSet,
         2, 0},
        {"cpssDxChPolicerEgressQosUpdateEnableGet",
         &wrCpssDxChPolicerEgressQosUpdateEnableGet,
         1, 0},
        {"cpssDxChPolicerVlanCntrSet",
         &wrCpssDxChPolicerVlanCntrSet,
         3, 0},
        {"cpssDxChPolicerVlanCntrGet",
         &wrCpssDxChPolicerVlanCntrGet,
         2, 0},
        {"cpssDxChPolicerVlanCountingPacketCmdTriggerSet",
         &wrCpssDxChPolicerVlanCountingPacketCmdTriggerSet,
         3, 0},
        {"cpssDxChPolicerVlanCountingPacketCmdTriggerGet",
         &wrCpssDxChPolicerVlanCountingPacketCmdTriggerGet,
         2, 0},
        {"cpssDxChPolicerVlanCountingModeSet",
         &wrCpssDxChPolicerVlanCountingModeSet,
         2, 0},
        {"cpssDxChPolicerVlanCountingModeGet",
         &wrCpssDxChPolicerVlanCountingModeGet,
         1, 0},
        {"cpssDxChPolicerPolicyCntrSet",
         &wrCpssDxChPolicerPolicyCntrSet,
         3, 0},
        {"cpssDxChPolicerPolicyCntrGet",
         &wrCpssDxChPolicerPolicyCntrGet,
         2, 0},
        {"cpssDxChPolicerMemorySizeModeSet",
         &wrCpssDxChPolicerMemorySizeModeSet,
         2, 0},
        {"cpssDxChPolicerMemorySizeModeGet",
         &wrCpssDxChPolicerMemorySizeModeGet,
         1, 0},
        {"cpssDxChPolicerMemorySizeModeSet1",
         &wrCpssDxChPolicerMemorySizeModeSet,
         2, 0},
        {"cpssDxChPolicerMemorySizeModeGet1",
         &wrCpssDxChPolicerMemorySizeModeGet,
         1, 0},
        {"cpssDxChPolicerCountingWriteBackCacheFlush",
         &wrCpssDxChPolicerCountingWriteBackCacheFlush,
         1, 0},
        {"cpssDxChPolicerGlobalStageSet",
         &wrCpssDxChPolicerGlobalStageSet,
         1, 0},
        {"cpssDxCh3PolicerManagementCountersSetV1",
         &wrCpssDxCh3PolicerManagementCountersSetV1,
         6, 0},
        {"cpssDxCh3PolicerManagementCountersGetV1",
         &wrCpssDxCh3PolicerManagementCountersGetV1,
         3, 0},
        {"cpssDxChPolicerMeteringCalcMethodSet",
         &wrCpssDxChPolicerMeteringCalcMethodSet,
         4, 0},
        {"cpssDxChPolicerMeteringCalcMethodGet",
         &wrCpssDxChPolicerMeteringCalcMethodGet,
         1, 0},
        {"cpssDxCh3PolicerMeteringEntryV1Set",
         &wrCpssDxCh3PolicerMeteringEntrySetV1,
         3, 19},
        {"cpssDxCh3PolicerMeteringEntryV1GetFirst",
         &wrCpssDxCh3PolicerMeteringEntryGetFirstV1,
         3, 0},
        {"cpssDxCh3PolicerMeteringEntryV1GetNext",
         &wrCpssDxCh3PolicerMeteringEntryGetNextV1,
         3, 0},
        {"cpssDxCh3PolicerMeteringEntryV2Set",
         &wrCpssDxCh3PolicerMeteringEntrySetV2,
         3, 27},
        {"cpssDxCh3PolicerMeteringEntryV2GetFirst",
         &wrCpssDxCh3PolicerMeteringEntryGetFirstV2,
         3, 0},
        {"cpssDxCh3PolicerMeteringEntryV2GetNext",
         &wrCpssDxCh3PolicerMeteringEntryGetNextV2,
         3, 0},
        {"cpssDxCh3PolicerBillingEntryV1Set",
         &wrCpssDxCh3PolicerBillingEntrySetV1,
         4, 8},
        {"cpssDxCh3PolicerBillingEntryV1GetFirst",
         &wrCpssDxCh3PolicerBillingEntryGetFirstV1,
         4, 0},
        {"cpssDxCh3PolicerBillingEntryV1GetNext",
         &wrCpssDxCh3PolicerBillingEntryGetNextV1,
         4, 0},
        {"cpssDxCh3PolicerBillingEntryV2Set",
         &wrCpssDxCh3PolicerBillingEntrySetV2,
         4, 17},
        {"cpssDxCh3PolicerBillingEntryV2GetFirst",
         &wrCpssDxCh3PolicerBillingEntryGetFirstV2,
         4, 0},
        {"cpssDxCh3PolicerBillingEntryV2GetNext",
         &wrCpssDxCh3PolicerBillingEntryGetNextV2,
         4, 0},
        {"cpssDxChPolicerCountingTriggerByPortEnableSet",
         &wrCpssDxChPolicerCountingTriggerByPortEnableSet,
         2, 0},
        {"cpssDxChPolicerCountingTriggerByPortEnableGet",
         &wrCpssDxChPolicerCountingTriggerByPortEnableGet,
         1, 0},
        {"cpssDxChPolicerPortModeAddressSelectSet",
         &wrCpssDxChPolicerPortModeAddressSelectSet,
         2, 0},
        {"cpssDxChPolicerPortModeAddressSelectGet",
         &wrCpssDxChPolicerPortModeAddressSelectGet,
         1, 0},
        {"cpssDxChPolicerEAttributesMeteringModeSet",
         &wrCpssDxChPolicerEAttributesMeteringModeSet,
         3, 0},
        {"cpssDxChPolicerEAttributesMeteringModeGet",
         &wrCpssDxChPolicerEAttributesMeteringModeGet,
         2, 0},
        {"cpssDxChPolicerTriggerEntrySet",
         &wrCpssDxChPolicerTriggerEntrySet,
         3, 16},
        {"cpssDxChPolicerTriggerEntryGetFirst",
         &wrCpssDxChPolicerTriggerEntryGetFirst,
         3, 0},
        {"cpssDxChPolicerTriggerEntryGetNext",
         &wrCpssDxChPolicerTriggerEntryGetNext,
         3, 0},
        {"cpssDxChPolicerFlowIdCountingCfgSet",
         &wrCpssDxChPolicerFlowIdCountingCfgSet,
         6, 0},
        {"cpssDxChPolicerFlowIdCountingCfgGet",
         &wrCpssDxChPolicerFlowIdCountingCfgGet,
         2, 0},
        {"cpssDxChPolicerHierarchicalTableSet",
         &wrCpssDxChPolicerHierarchicalTableSet,
         3, 4},
        {"cpssDxChPolicerHierarchicalTableGetNext",
         &wrCpssDxChPolicerHierarchicalTableGetNext,
         3, 0},
        {"cpssDxChPolicerHierarchicalTableGetFirst",
         &wrCpssDxChPolicerHierarchicalTableGetFirst,
         3, 0},
        {"cpssDxChPolicerSecondStageIndexMaskSet",
         &wrCpssDxChPolicerSecondStageIndexMaskSet,
         2, 0},
        {"cpssDxChPolicerSecondStageIndexMaskGet",
         &wrCpssDxChPolicerSecondStageIndexMaskGet,
         1, 0},

        {"cpssDxChPolicerTrappedPacketsBillingEnableSet",
         &wrCpssDxChPolicerTrappedPacketsBillingEnableSet,
         2, 0},
        {"cpssDxChPolicerTrappedPacketsBillingEnableGet",
         &wrCpssDxChPolicerTrappedPacketsBillingEnableGet,
         1, 0},

        {"cpssDxChPolicerLossMeasurementCntrCaptureEnSet",
         &wrCpssDxChPolicerLossMeasurementCounterCaptureEnableSet,
         2, 0},

        {"cpssDxChPolicerLossMeasurementCntrCaptureEnGet",
         &wrCpssDxChPolicerLossMeasurementCounterCaptureEnableGet,
         1, 0},

        {"cpssDxChPolicerPortStormTypeInFlowModeEnableSet",
         &wrCpssDxChPolicerPortStormTypeInFlowModeEnableSet,
         3, 0},

        {"cpssDxChPolicerPortStormTypeInFlowModeEnableGet",
         &wrCpssDxChPolicerPortStormTypeInFlowModeEnableGet,
         2, 0},

        {"cpssDxChPolicerPacketSizeModeForTimestampTagSet",
         &wrCpssDxChPolicerPacketSizeModeForTimestampTagSet,
         2, 0},

        {"cpssDxChPolicerPacketSizeModeForTimestampTagGet",
         &wrCpssDxChPolicerPacketSizeModeForTimestampTagGet,
         1, 0},
        {"cpssDxChPolicerFlowBasedMeteringToFloodOnlyEnSet",
         &wrCpssDxChPolicerFlowBasedMeteringToFloodedOnlyEnableSet,
         2, 0},
        {"cpssDxChPolicerFlowBasedMeteringToFloodOnlyEnGet",
         &wrCpssDxChPolicerFlowBasedMeteringToFloodedOnlyEnableGet,
         1, 0},
        {"cpssDxChPolicerFlowBasedBillingToFloodOnlyEnSet",
         &wrCpssDxChPolicerFlowBasedBillingToFloodedOnlyEnableSet,
         2, 0},
        {"cpssDxChPolicerFlowBasedBillingToFloodOnlyEnGet",
         &wrCpssDxChPolicerFlowBasedBillingToFloodedOnlyEnableGet,
         1, 0},
        {"cpssDxChPolicerMeteringEntryEnvelopeSetFirst",
         &wrCpssDxChPolicerMeteringEntryEnvelopeSetFirst,
         4, 29},
        {"cpssDxChPolicerMeteringEntryEnvelopeSetNext",
         &wrCpssDxChPolicerMeteringEntryEnvelopeSetNext,
         4, 29},
        {"cpssDxChPolicerMeteringEntryEnvelopeEndSet",
         &wrCpssDxChPolicerMeteringEntryEnvelopeEndSet,
         4, 0},
        {"cpssDxChPolicerMeteringEntryEnvelopeGetFirst",
         &wrCpssDxChPolicerMeteringEntryEnvelopeGetFirst,
         4, 0},
        {"cpssDxChPolicerMeteringEntryEnvelopeGetNext",
         &wrCpssDxChPolicerMeteringEntryEnvelopeGetNext,
         4, 0},
        {"cpssDxChPolicerMeterTableFlowBasedIndexConfigSet",
         &wrCpssDxChPolicerMeterTableFlowBasedIndexConfigSet,
         4, 0},
        {"cpssDxChPolicerMeterTableFlowBasedIndexConfigGet",
         &wrCpssDxChPolicerMeterTableFlowBasedIndexConfigGet,
         2, 0},
        {"cpssDxChPolicerQosProfileToPriorityMapSet",
         &wrCpssDxChPolicerQosProfileToPriorityMapSet,
         2, 2},
        {"cpssDxChPolicerQosProfileToPriorityMapGetFirst",
         &wrCpssDxChPolicerQosProfileToPriorityMapGetFirst,
         4, 0},
        {"cpssDxChPolicerQosProfileToPriorityMapGetNext",
         &wrCpssDxChPolicerQosProfileToPriorityMapGetNext,
         4, 0},
        {"cpssDxChPolicerTokenBucketMaxRateSet",
         &wrCpssDxChPolicerTokenBucketMaxRateSet,
         7, 3},
        {"cpssDxChPolicerTokenBucketMaxRateGetFirst",
         &wrCpssDxChPolicerTokenBucketMaxRateGetFirst,
         7, 0},
        {"cpssDxChPolicerTokenBucketMaxRateGetNext",
         &wrCpssDxChPolicerTokenBucketMaxRateGetNext,
         7, 0},
        {"cpssDxChPolicerBucketsCurrentStateGetGetFirst",
         &wrCpssDxChPolicerBucketsCurrentStateGetFirst,
         4, 0},
        {"cpssDxChPolicerBucketsCurrentStateGetGetNext",
         &wrCpssDxChPolicerBucketsCurrentStateGetNext,
         4, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxCh3Policer
*
* DESCRIPTION:
*     Library database initialization function.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
GT_STATUS cmdLibInitCpssDxCh3Policer
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

