/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortPizzaArbiter.c
*
* DESCRIPTION:
*       CPSS implementation for SerDes configuration and control facility.
*
* FILE REVISION NUMBER:
*       $Revision: 65 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/cpssDxChPortPizzaArbiter.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/TablePizzaArbiter/prvCpssDxChPortTablePizzaArbiter.h>
/* pizza arbiter */
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/TablePizzaArbiter/prvCpssDxChPortPizzaArbiter.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/TablePizzaArbiter/prvCpssDxChPortPizzaArbiterDevInfo.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/TablePizzaArbiter/prvCpssDxChPortPizzaArbiterTables.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/TablePizzaArbiter/prvCpssDxChPortPizzaArbiterProfileStorage.h>
/* slices management for specific units */
#include <cpss/dxCh/dxChxGen/port/SliceManipulation/prvCpssDxChPortSlices.h>
#include <cpss/dxCh/dxChxGen/port/SliceManipulation/private/prvCpssDxChPortSlicesRxDMA.h>
#include <cpss/dxCh/dxChxGen/port/SliceManipulation/private/prvCpssDxChPortSlicesRxDMACTU.h>
#include <cpss/dxCh/dxChxGen/port/SliceManipulation/private/prvCpssDxChPortSlicesTxDMA.h>
#include <cpss/dxCh/dxChxGen/port/SliceManipulation/private/prvCpssDxChPortSlicesTxDMACTU.h>
#include <cpss/dxCh/dxChxGen/port/SliceManipulation/private/prvCpssDxChPortSlicesBM.h>
#include <cpss/dxCh/dxChxGen/port/SliceManipulation/private/prvCpssDxChPortSlicesTxQ.h>
#include <cpss/dxCh/dxChxGen/port/SliceManipulation/private/prvCpssDxChPortSlicesTxQHWDef.h>

#undef PIZZA_DEBUG_PRINT_DUMP

#ifdef PIZZA_DEBUG_PRINT_DUMP

    extern GT_STATUS gtLion2PortPizzaArbiterIfStateDump(/*IN*/GT_U8  devNum,
                                                         /*IN*/GT_U32 portGroupId);
    int g_printPizzaDump = 1;
#endif


/*******************************************************************************
* prvCpssDxChPortTablePizzaArbiterIfInit
*
* DESCRIPTION:
*       Pizza arbiter initialization in all Units where it's present
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*       I forced to place this function here, because it needs number of port
*       group where CPU port is connected and there is just no more suitable
*       place.
*
*******************************************************************************/

GT_STATUS prvCpssDxChPortTablePizzaArbiterIfInit
(
    IN  GT_U8                   devNum
)
{
    GT_STATUS   rc;                 /* return code */
    PRV_CPSS_DXCH_PORT_PIZZA_DEV_INFO_STC  pizzaDevInfo;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_XCAT2_E);

    rc = prvCpssDxChPortPizzaArbiterIfDevInfoBuild(/*IN*/devNum,/*OUT*/&pizzaDevInfo);
    if (rc != GT_OK)
    {
        return rc;
    }
    rc = prvCpssDxChPizzaArbiterDeviceInit(/*IN*/devNum,
                                           /*IN*/&pizzaDevInfo);
    if (rc != GT_OK)
    {
        return rc;
    }
    #ifdef PIZZA_DEBUG_PRINT_DUMP
        if (g_printPizzaDump == 1)
        {
            cpssOsPrintf("\nInit dev %2d core %2d",
                                devNum,0);
            rc = gtLion2PortPizzaArbiterIfStateDump(devNum,0);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

    #endif

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortTablePizzaArbiterIfCheckSupport
*
* DESCRIPTION:
*       Check whether Pizza Arbiter can be configured to new port's i/f mode and speed
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       portSpeed - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_NOT_SUPPORTED - not supported
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortTablePizzaArbiterIfCheckSupport
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_SPEED_ENT     portSpeed
)
{
    GT_STATUS rc;   /* return code */
    GT_BOOL                 isExtendedModeEnable;
    PRV_CPSS_DXCH_PORT_PIZZA_DEV_INFO_STC    pizzaDevInfo;

    /* slice not used by any port, but takes time to ensure equal arbitration */
    /* PRV_CPSS_GEN_PP_CONFIG_STC* devPtr; *//* pointer to device to be processed*/

    GT_U32 portGroupId;            /* port group number */
    GT_U32 localPort;              /* number of port in port group */
    /* CPSS_PORT_SPEED_ENT portSpeed; *//* new port speed to be configured */
    GT_U32              sliceNum2Use;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_XCAT2_E);

    /* portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);*/
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,portNum);
    localPort   = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    /*pDev = PRV_CPSS_PP_MAC(devNum);
      portSpeed = pDev->phyPortInfoArray[portNum].portSpeed;*/

    rc  = prvCpssDxChPizzaArbiterSelectSlicesBySpeed(/*IN*/devNum,
                                                     /*IN*/portGroupId,
                                                     /*IN*/localPort,
                                                     /*IN*/portSpeed,
                                                     /*OUT*/&sliceNum2Use);
    if (GT_OK != rc)
    {
        return rc;
    }
    rc = cpssDxChPortExtendedModeEnableGet(devNum,portNum, &isExtendedModeEnable);
    if (GT_OK != rc)
    {
        return rc;
    }


    rc = prvCpssDxChPortPizzaArbiterIfDevInfoBuild(/*IN*/devNum,/*OUT*/&pizzaDevInfo);

    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChPizzaArbiterPortSetState(devNum,
                                                 portGroupId,
                                                 localPort,
                                                 isExtendedModeEnable,
                                                 sliceNum2Use,
                                                 &pizzaDevInfo,
                                                 PA_PORT_ACTION_VERIFY);
    if (GT_OK != rc)
    {
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortTablePizzaArbiterIfConfigure
*
* DESCRIPTION:
*       Configure Pizza Arbiter accoringly to new port's i/f mode and speed
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       portSpeed - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_NOT_SUPPORTED - not supported
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortTablePizzaArbiterIfConfigure
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_SPEED_ENT     portSpeed
)
{
    GT_STATUS rc;   /* return code */
    GT_BOOL                 isExtendedModeEnable;
    PRV_CPSS_DXCH_PORT_PIZZA_DEV_INFO_STC    pizzaDevInfo;

    /* slice not used by any port, but takes time to ensure equal arbitration */
    /* PRV_CPSS_GEN_PP_CONFIG_STC* devPtr; *//* pointer to device to be processed*/

    GT_U32 portGroupId;            /* port group number */
    GT_U32 localPort;              /* number of port in port group */
    /* CPSS_PORT_SPEED_ENT portSpeed; *//* new port speed to be configured */
    GT_U32              sliceNum2Use;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_XCAT2_E);

    /* portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);*/
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,portNum);
    localPort   = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    /*pDev = PRV_CPSS_PP_MAC(devNum);
      portSpeed = pDev->phyPortInfoArray[portNum].portSpeed;*/

    rc  = prvCpssDxChPizzaArbiterSelectSlicesBySpeed(/*IN*/devNum,
                                                     /*IN*/portGroupId,
                                                     /*IN*/localPort,
                                                     /*IN*/portSpeed,
                                                     /*OUT*/&sliceNum2Use);
    if (GT_OK != rc)
    {
        return rc;
    }
    rc = cpssDxChPortExtendedModeEnableGet(devNum,portNum, &isExtendedModeEnable);
    if (GT_OK != rc)
    {
        return rc;
    }


    rc = prvCpssDxChPortPizzaArbiterIfDevInfoBuild(/*IN*/devNum,/*OUT*/&pizzaDevInfo);

    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChPizzaArbiterPortSetState(devNum,
                                             portGroupId,
                                             localPort,
                                             isExtendedModeEnable,
                                             sliceNum2Use,
                                             &pizzaDevInfo,
                                             PA_PORT_ACTION_PERFORM);
    if (GT_OK != rc)
    {
        return rc;
    }

    #ifdef PIZZA_DEBUG_PRINT_DUMP
        if (g_printPizzaDump == 1)
        {
            cpssOsPrintf("\nConfigure dev %2d core %2d port %2d slices %2d",
                                devNum,portGroupId,localPort, sliceNum2Use);
            rc = gtLion2PortPizzaArbiterIfStateDump(devNum,portGroupId);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    #endif
    return rc;
}

/*******************************************************************************
* prvCpssDxChPortTablePizzaArbiterIfDelete
*
* DESCRIPTION:
*       Delete port from Pizza Arbiter
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_NOT_SUPPORTED - not supported
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortTablePizzaArbiterIfDelete
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum
)
{
    GT_STATUS rc;   /* return code */
    GT_BOOL                 isExtendedModeEnable;
    PRV_CPSS_DXCH_PORT_PIZZA_DEV_INFO_STC    pizzaDevInfo;

    /* slice not used by any port, but takes time to ensure equal arbitration */
    /* PRV_CPSS_GEN_PP_CONFIG_STC* devPtr; *//* pointer to device to be processed*/

    GT_U32 portGroupId;            /* port group number */
    GT_U32 localPort;              /* number of port in port group */
    GT_U32 sliceNum2Use;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_XCAT2_E);

    /* portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);*/
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,portNum);
    localPort   = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        sliceNum2Use = 0;  /* we like to delete port */

    rc = cpssDxChPortExtendedModeEnableGet(devNum,portNum, &isExtendedModeEnable);
    if (GT_OK != rc)
    {
        return rc;
    }


    rc = prvCpssDxChPortPizzaArbiterIfDevInfoBuild(/*IN*/devNum,/*OUT*/&pizzaDevInfo);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChPizzaArbiterPortSetState(devNum,
                                             portGroupId,
                                             localPort,
                                             isExtendedModeEnable,
                                             sliceNum2Use,
                                             &pizzaDevInfo,
                                             PA_PORT_ACTION_PERFORM);
    if (GT_OK != rc)
    {
        return rc;
    }

    #ifdef PIZZA_DEBUG_PRINT_DUMP
        if (g_printPizzaDump == 1)
        {
            cpssOsPrintf("\Delete dev %2d core %2d port %2d",
                                devNum,portGroupId,localPort);
            rc = gtLion2PortPizzaArbiterIfStateDump(devNum,portGroupId);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    #endif
    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPizzaArbiterIfPortStateGet
*
* DESCRIPTION:
*       Get port state(how many slice it is configured)
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum        - device number
*       portGroupId   - port group
*       localPortNum  - local port number
*
* OUTPUTS:
*       sliceNumUsedPtr -- number of slice occupied by port.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPizzaArbiterIfPortStateGet
(
    IN  GT_U8 devNum,
    IN  GT_U32 portGroupId,
    IN  GT_PHYSICAL_PORT_NUM localPortNum,
    OUT GT_U32              *sliceNumUsedPtr
)
{
    GT_STATUS rc;   /* return code */
    PRV_CPSS_DXCH_PORT_PIZZA_DEV_INFO_STC                            pizzaDevInfo;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_XCAT2_E);

    rc = prvCpssDxChPortPizzaArbiterIfDevInfoBuild(/*IN*/devNum,/*OUT*/&pizzaDevInfo);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChPizzaArbiterPortGetState (/*IN*/devNum,
                                               /*IN*/portGroupId,
                                               /*IN*/localPortNum,
                                               /*IN*/&pizzaDevInfo,
                                               /*OUT*/sliceNumUsedPtr);


    return rc;
}

/*******************************************************************************
* cpssDxChPortPizzaArbiterIfPortStateGet
*
* DESCRIPTION:
*       Get port state(how many slice it is configured)
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum        - device number
*       portGroupId   - port group
*       localPortNum  - local port number
*
* OUTPUTS:
*       sliceNumUsedPtr -- number of slice occupied by port.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortPizzaArbiterIfPortStateGet
(
    IN  GT_U8 devNum,
    IN  GT_U32 portGroupId,
    IN  GT_PHYSICAL_PORT_NUM localPortNum,
    OUT GT_U32              *sliceNumUsedPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPizzaArbiterIfPortStateGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupId, localPortNum, sliceNumUsedPtr));

    rc = internal_cpssDxChPortPizzaArbiterIfPortStateGet(devNum, portGroupId, localPortNum, sliceNumUsedPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupId, localPortNum, sliceNumUsedPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPizzaArbiterIfSliceStateGet
*
* DESCRIPTION:
*       Get port slice state(is slice occupied by any port)
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - device number
*       portGroupIdx  - port group
*       sliceID       - sliceID
*
* OUTPUTS:
*       isOccupiedPtr - whether slice is occupied by any port
*       portNumPtr    - number of port occupied the slice .
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPizzaArbiterIfSliceStateGet
(
    IN  GT_U8 devNum,
    IN  GT_U32 portGroupIdx,
    IN  GT_U32 sliceID,
    OUT GT_BOOL              *isOccupiedPtr,
    OUT GT_PHYSICAL_PORT_NUM *portNumPtr
)
{
    GT_STATUS rc;   /* return code */

    PRV_CPSS_DXCH_PORT_PIZZA_DEV_INFO_STC                            pizzaDevInfo;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_XCAT2_E
                                            | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    rc = prvCpssDxChPortPizzaArbiterIfDevInfoBuild(/*IN*/devNum,/*OUT*/&pizzaDevInfo);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc= prvCpssDxChPizzaArbiterSliceGetState(/*IN*/devNum,
                                               /*IN*/portGroupIdx,
                                               /*IN*/sliceID,
                                               /*IN*/&pizzaDevInfo,
                                               /*OUT*/isOccupiedPtr,
                                               /*OUT*/portNumPtr);
    return rc;
}

/*******************************************************************************
* cpssDxChPortPizzaArbiterIfSliceStateGet
*
* DESCRIPTION:
*       Get port slice state(is slice occupied by any port)
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - device number
*       portGroupIdx  - port group
*       sliceID       - sliceID
*
* OUTPUTS:
*       isOccupiedPtr - whether slice is occupied by any port
*       portNumPtr    - number of port occupied the slice .
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortPizzaArbiterIfSliceStateGet
(
    IN  GT_U8 devNum,
    IN  GT_U32 portGroupIdx,
    IN  GT_U32 sliceID,
    OUT GT_BOOL              *isOccupiedPtr,
    OUT GT_PHYSICAL_PORT_NUM *portNumPtr
)
{
   GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPizzaArbiterIfSliceStateGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupIdx, sliceID, isOccupiedPtr, portNumPtr));

    rc = internal_cpssDxChPortPizzaArbiterIfSliceStateGet(devNum, portGroupIdx, sliceID, isOccupiedPtr, portNumPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupIdx, sliceID, isOccupiedPtr, portNumPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfProfileSet
*
* DESCRIPTION:
*       Set profile for specific device : port group
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
*
* INPUTS:
*       devNum           - device number
*       portGroupListPtr - port group
*       pPizzaProfilePtr - pointer to profile
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PTR      - wrong ptr
*       GT_NO_RESOURCE  - no memory at memory pools (increase constants !!!)
*       GT_FAIL         - on error
*       GT_ALREADY_EXIST - profile or speed conversion table already exists
*
* COMMENTS:
*       1. Suppose to be called before cpssInitSystem()
*       2. Traffic on all ports of PORT GROUP at the DEVICE shall be stopped
*       3. cpssDxChPortPizzaArbiterIfUserTableSet has two pointers. Only non-NULL pointers
*          are processed.
*******************************************************************************/
static GT_STATUS prvCpssDxChPortPizzaArbiterIfProfileSet
(
    IN GT_U8 devNum,
    IN PRV_CPSS_DXCH_ACTIVE_PORT_GROUP_LIST_STC *portGroupListPtr,
    IN CPSS_DXCH_PIZZA_PROFILE_STC              *pPizzaProfilePtr
)
{
    GT_STATUS rc;   /* return code */
    GT_U32 portGroupId;
    PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_STC     * pPizzaStoragePtr;
    PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_STC * pizzaProfileStoragePoolPtr;
    ActivePortGroupListIter_STC   portGroupListIter;


    /*
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_XCAT2_E);
    */

    if (NULL == pPizzaProfilePtr || NULL == portGroupListPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }


    pizzaProfileStoragePoolPtr = pizzaProfileStoragePoolGet();

    rc = ActivePortGroupListIterInit(&portGroupListIter,portGroupListPtr);
    if (rc != GT_OK)  /* found !!!!!*/
    {
        return rc;
    }

    /* find if any from port group already exists */
    for (;;)
    {
        rc = ActivePortGroupListIterGetCur(&portGroupListIter,&portGroupId);
        if (rc != GT_OK)  /* found !!!!!*/
        {
            return rc;
        }
        if (portGroupId == PRV_CPSS_BAD_PORTGROUP_CNS)
        {
            break;
        }
        rc = pizzaProfileStoragePoolFind1(pizzaProfileStoragePoolPtr, devNum,portGroupId,/*OUT*/&pPizzaStoragePtr);
        if (rc == GT_OK)  /* found !!!!!*/
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);
        }
        rc = ActivePortGroupListIterGotNext(&portGroupListIter);
        if (rc != GT_OK)  /* found !!!!!*/
        {
            return rc;
        }
    }

    rc = pizzaProfileStoragePoolAlloc(pizzaProfileStoragePoolPtr, /*OUT*/&pPizzaStoragePtr);
    if (rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    }

    rc = pizzaProfileStorageCopy(/*INOUT*/pPizzaStoragePtr, devNum, portGroupListPtr,/*IN*/pPizzaProfilePtr);
    return rc;
}


/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfProfileGet
*
* DESCRIPTION:
*       Set profile for specific device : port group
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
*
* INPUTS:
*       devNum   - device number
*       portGroupIdx  - port group
*
* OUTPUTS:
*       ppPizzaProfilePtrPtr - pointer to profile
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PTR      - wrong ptr
*
* COMMENTS:
*******************************************************************************/
static GT_STATUS prvCpssDxChPortPizzaArbiterIfProfileGet
(
    IN  GT_U8 devNum,
    IN  GT_U32 portGroupIdx,
    OUT CPSS_DXCH_PIZZA_PROFILE_STC **ppPizzaProfilePtrPtr
)
{
    GT_STATUS rc;   /* return code */
    PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_STC     * pPizzaStoragePtr;
    PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_STC * pizzaProfileStoragePoolPtr;

    PRV_CPSS_GEN_PP_CONFIG_STC* pDev;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_XCAT2_E);

    pDev = PRV_CPSS_PP_MAC(devNum);


    if (NULL == ppPizzaProfilePtrPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    *ppPizzaProfilePtrPtr = NULL;

    pizzaProfileStoragePoolPtr = pizzaProfileStoragePoolGet();
    rc = pizzaProfileStoragePoolFind1(pizzaProfileStoragePoolPtr, devNum,portGroupIdx,/*OUT*/&pPizzaStoragePtr);
    if (rc == GT_OK)  /* found !!!!!*/
    {
        *ppPizzaProfilePtrPtr = &pPizzaStoragePtr->pizzaProfile;
        return GT_OK;
    }
    /*---------------------------------------------*/
    /* user profile not found, get system profile  */
    /*---------------------------------------------*/
    rc = prvCpssDxChPizzaArbiterSystemProfileGet(/*IN*/pDev->devFamily,
                                                 /*IN*/pDev->revision,
                                                 /*IN*/pDev->coreClock,
                                                 /*OUT*/ppPizzaProfilePtrPtr);

    return rc;
}



/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfSpeed2SliceConvTableSet
*
* DESCRIPTION:
*       Set profile for specific device : port group
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
*
* INPUTS:
*       devNum   - device number
*       portGroupListPtr - list of port groups the table is applicable
*       portGroupPortSpeed2SliceNumPtr - list of port speed 2 slice conversion
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PTR      - wrong ptr
*       GT_NO_RESOURCE  - no memory at memory pools (increase constants !!!)
*       GT_FAIL         - on error
*       GT_ALREADY_EXIST - profile or speed conversion table already exists
*
* COMMENTS:
*       1. Suppose to be called before cpssInitSystem()
*       2. Traffic on all ports of PORT GROUP at the DEVICE shall be stopped
*       3. cpssDxChPortPizzaArbiterIfUserTableSet has two pointers. Only non-NULL pointers
*          are processed.
*******************************************************************************/
static GT_STATUS prvCpssDxChPortPizzaArbiterIfSpeed2SliceConvTableSet
(
    IN  GT_U8 devNum,
    IN  PRV_CPSS_DXCH_ACTIVE_PORT_GROUP_LIST_STC             *portGroupListPtr,
    IN  CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC *portGroupPortSpeed2SliceNumPtr
)
{
    GT_STATUS rc;   /* return code */
    GT_U32                          portGroupIdx;
    PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_STC     * speedConvTblStoragePtr;
    PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_POOL_STC * speedConvTableStoragePoolPtr;
    ActivePortGroupListIter_STC     portGroupListIter;

    /*
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_XCAT2_E);
    */

    if (NULL == portGroupPortSpeed2SliceNumPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    speedConvTableStoragePoolPtr = speedConvTableStoragePoolGet();

    rc = ActivePortGroupListIterInit(&portGroupListIter,portGroupListPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* find if any from port group already exists */
    for (;;)
    {
        rc = ActivePortGroupListIterGetCur(&portGroupListIter,&portGroupIdx);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (portGroupIdx == PRV_CPSS_BAD_PORTGROUP_CNS)
        {
            break;
        }
        rc = speedConvTableStoragePoolFind(speedConvTableStoragePoolPtr, devNum,portGroupIdx,/*OUT*/&speedConvTblStoragePtr);
        if (rc == GT_OK)  /* found !!!!!*/
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);
        }
        rc = ActivePortGroupListIterGotNext(&portGroupListIter);
        if (rc != GT_OK)
        {
            return rc;
        }
    }



    rc = speedConvTableStoragePoolAlloc(speedConvTableStoragePoolPtr, /*OUT*/&speedConvTblStoragePtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = speedConvTableStorageCopy(/*INOUT*/speedConvTblStoragePtr,
                                      /*IN*/devNum, portGroupListPtr,/*IN*/portGroupPortSpeed2SliceNumPtr);
    return rc;
}

/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfSpeed2SliceConvTableGet
*
* DESCRIPTION:
*       get speed 2 slice conversion list for specific port group
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
*
* INPUTS:
*       devNum   - device number
*       portGroupIdx  - port group
*       portGroupPortSpeed2SliceNumPtrPtr - pointer to profile
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PTR      - wrong ptr
*
* COMMENTS:
*******************************************************************************/
static GT_STATUS prvCpssDxChPortPizzaArbiterIfSpeed2SliceConvTableGet
(
    IN GT_U8 devNum,
    IN GT_U32 portGroupIdx,
    IN CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC **portGroupPortSpeed2SliceNumPtrPtr
)
{
    GT_STATUS rc;   /* return code */
    PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_STC     * speedConvTblStoragePtr;
    PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_POOL_STC * speedConvTableStoragePoolPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_XCAT2_E);

    if (NULL == portGroupPortSpeed2SliceNumPtrPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    *portGroupPortSpeed2SliceNumPtrPtr = NULL;

    speedConvTableStoragePoolPtr = speedConvTableStoragePoolGet();

    rc = speedConvTableStoragePoolFind(speedConvTableStoragePoolPtr, devNum,portGroupIdx,/*OUT*/&speedConvTblStoragePtr);
    if (rc == GT_OK)  /* found !!!!!*/
    {
        *portGroupPortSpeed2SliceNumPtrPtr = &speedConvTblStoragePtr->portSpeed2SlicesConvTbl;
        return GT_OK;
    }
    /*---------------------------------------------*/
    /* user profile not found, get system profile  */
    /*---------------------------------------------*/
    *portGroupPortSpeed2SliceNumPtrPtr = (CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC *)&prvPortGroupPortSpeed2SliceNumDefault;
    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortPizzaArbiterIfUserTableSet
*
* DESCRIPTION:
*       Set profile and/or speed conversion table
*       for specific device : port group
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum   - device number
*       portGroupBmp  - port group bitmap
*       pizzaProfilePtr - pointer to profile
*       portGroupPortSpeed2SliceNumPtr - pointer to speed 2 slice converion list
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_NO_RESOURCE  - no memory at memory pools (increase constants !!!)
*       GT_FAIL         - on error
*       GT_ALREADY_EXIST - profile or speed conversion table already exists
*
* COMMENTS:
*       1. Suppose to be called before cpssInitSystem()
*       2. Traffic on all ports of PORT GROUP at the DEVICE shall be stopped
*       3. cpssDxChPortPizzaArbiterIfUserTableSet has two pointers. Only non-NULL pointers
*          are processed.
*       4. In function cpssDxChPortPizzaArbiterIfUserTableSet() non-zero user profile is processed
*          first. If it already exists processing is stopped that means speed conversion
*          table is not processed.
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPizzaArbiterIfUserTableSet
(
    IN  GT_U8 devNum,
    IN  GT_U32 portGroupBmp,
    IN  CPSS_DXCH_PIZZA_PROFILE_STC                          *pizzaProfilePtr,
    IN  CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC *portGroupPortSpeed2SliceNumPtr
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_ACTIVE_PORT_GROUP_LIST_STC activePortGroupList;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_BOBCAT3_E |CPSS_CAELUM_E);

    if (pizzaProfilePtr == NULL && portGroupPortSpeed2SliceNumPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    if (0 == portGroupBmp)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = ActivePortGroupListBuildFromBmp(&activePortGroupList,portGroupBmp,0,31);
    if (rc != GT_OK)
    {
        return rc;
    }


    if (NULL != pizzaProfilePtr )
    {
        rc = prvCpssDxChPortPizzaArbiterIfProfileSet(/*IN*/devNum,
                                                    /*IN*/&activePortGroupList,
                                                    /*IN*/pizzaProfilePtr);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    if (NULL != portGroupPortSpeed2SliceNumPtr)
    {
        rc = prvCpssDxChPortPizzaArbiterIfSpeed2SliceConvTableSet(/*IN*/devNum,
                                                                  /*IN*/&activePortGroupList,
                                                                  /*IN*/portGroupPortSpeed2SliceNumPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPizzaArbiterIfUserTableSet
*
* DESCRIPTION:
*       Set profile and/or speed conversion table
*       for specific device : port group
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum   - device number
*       portGroupBmp  - port group bitmap
*       pizzaProfilePtr - pointer to profile
*       portGroupPortSpeed2SliceNumPtr - pointer to speed 2 slice converion list
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_NO_RESOURCE  - no memory at memory pools (increase constants !!!)
*       GT_FAIL         - on error
*       GT_ALREADY_EXIST - profile or speed conversion table already exists
*
* COMMENTS:
*       1. Suppose to be called before cpssInitSystem()
*       2. Traffic on all ports of PORT GROUP at the DEVICE shall be stopped
*       3. cpssDxChPortPizzaArbiterIfUserTableSet has two pointers. Only non-NULL pointers
*          are processed.
*       4. In function cpssDxChPortPizzaArbiterIfUserTableSet() non-zero user profile is processed
*          first. If it already exists processing is stopped that means speed conversion
*          table is not processed.
*******************************************************************************/
GT_STATUS cpssDxChPortPizzaArbiterIfUserTableSet
(
    IN  GT_U8 devNum,
    IN  GT_U32 portGroupBmp,
    IN  CPSS_DXCH_PIZZA_PROFILE_STC                          *pizzaProfilePtr,
    IN  CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC *portGroupPortSpeed2SliceNumPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPizzaArbiterIfUserTableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupBmp, pizzaProfilePtr, portGroupPortSpeed2SliceNumPtr));

    rc = internal_cpssDxChPortPizzaArbiterIfUserTableSet(devNum, portGroupBmp, pizzaProfilePtr, portGroupPortSpeed2SliceNumPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupBmp, pizzaProfilePtr, portGroupPortSpeed2SliceNumPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPizzaArbiterIfUserTableGet
*
* DESCRIPTION:
*       Get profile and/or speed conversion table
*       for specific device : port group
*          initially searches among user defined tables and
*            if it is not found there , searches in system tables
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum   - device number
*       portGroupBmp  - port group bitmap
*
* OUTPUTS:
*       pizzaProfilePtrPtrPtr - pointer to profile
*       portGroupPortSpeed2SliceNumPtrPtr - pointer to speed 2 slice converion list
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PTR      - any zero pointer
*       GT_NOT_FOUND    - if any table is found
*
* COMMENTS:
*       Only first port group is used.
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPizzaArbiterIfUserTableGet
(
    IN   GT_U8 devNum,
    IN   GT_U32 portGroupBmp,
    OUT  CPSS_DXCH_PIZZA_PROFILE_STC                          **pizzaProfilePtrPtrPtr,
    OUT  CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC **portGroupPortSpeed2SliceNumPtrPtr
)
{
    GT_STATUS rc1,rc2;
    GT_U32 portGroupId;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_XCAT2_E
                                            | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);
    if (portGroupBmp == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (NULL == pizzaProfilePtrPtrPtr && NULL == portGroupPortSpeed2SliceNumPtrPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    rc1 = GT_OK;
    rc2 = GT_OK;

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupBmp,/*OUT*/portGroupId)
    {
        break;
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    if (NULL != pizzaProfilePtrPtrPtr)
    {
        *pizzaProfilePtrPtrPtr = (CPSS_DXCH_PIZZA_PROFILE_STC *)NULL;
        rc1 = prvCpssDxChPortPizzaArbiterIfProfileGet(/*IN*/devNum,
                                                      /*IN*/portGroupId,
                                                      /*IN*/pizzaProfilePtrPtrPtr);
    }

    if (NULL != portGroupPortSpeed2SliceNumPtrPtr)
    {
        *portGroupPortSpeed2SliceNumPtrPtr = (CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC *)NULL;
        rc2 = prvCpssDxChPortPizzaArbiterIfSpeed2SliceConvTableGet(/*IN*/devNum,
                                                                /*IN*/portGroupId,
                                                                /*IN*/portGroupPortSpeed2SliceNumPtrPtr);
    }
    if (rc1 != GT_OK || rc2 != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPizzaArbiterIfUserTableGet
*
* DESCRIPTION:
*       Get profile and/or speed conversion table
*       for specific device : port group
*          initially searches among user defined tables and
*            if it is not found there , searches in system tables
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum   - device number
*       portGroupBmp  - port group bitmap
*
* OUTPUTS:
*       pizzaProfilePtrPtrPtr - pointer to profile
*       portGroupPortSpeed2SliceNumPtrPtr - pointer to speed 2 slice converion list
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PTR      - any zero pointer
*       GT_NOT_FOUND    - if any table is found
*
* COMMENTS:
*       Only first port group is used.
*******************************************************************************/
GT_STATUS cpssDxChPortPizzaArbiterIfUserTableGet
(
    IN   GT_U8 devNum,
    IN   GT_U32 portGroupBmp,
    OUT  CPSS_DXCH_PIZZA_PROFILE_STC                          **pizzaProfilePtrPtrPtr,
    OUT  CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC **portGroupPortSpeed2SliceNumPtrPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPizzaArbiterIfUserTableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupBmp, pizzaProfilePtrPtrPtr, portGroupPortSpeed2SliceNumPtrPtr));

    rc = internal_cpssDxChPortPizzaArbiterIfUserTableGet(devNum, portGroupBmp, pizzaProfilePtrPtrPtr, portGroupPortSpeed2SliceNumPtrPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupBmp, pizzaProfilePtrPtrPtr, portGroupPortSpeed2SliceNumPtrPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}



/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfProfileIterInit
*
* DESCRIPTION:
*       Set user profile iterator
*       traverse over list of profiles/speed conv tables
*       returns dev, portGroup,  profile or speed conversion table
*       This function inits iterator
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
*
* INPUTS:
*       iterPtr    pointer to iterator
*
* OUTPUTS:
*       iterPtr    pointer to iterator
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  - invalide ptr
*
* COMMENTS:
*       Usage :
*           PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_ITER_STC iter;
*               IterInit(&iter);
*               IterReset(&iter);
*               while (GT_OK == (rc = IterGetCur(&iter, &dev,&portGroup))
*               {
*                    // user action for each received data e.g. print
*                    IterGotoNext(&iter)
*               }
*******************************************************************************/
GT_STATUS prvCpssDxChPortPizzaArbiterIfProfileIterInit
(
    INOUT PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_ITER_STC *iterPtr
)
{
    if (NULL == iterPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    iterPtr->curIdx = 0;
    iterPtr->storagePoolPtr = pizzaProfileStoragePoolGet();
    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfProfileIterReset
*
* DESCRIPTION:
*       Set user profile iterator
*       traverse over list of profiles/speed conv tables
*       returns dev, portGroup,  profile or speed conversion table
*       this function resets iterator to beginning of the list
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       iterPtr    - pointer to iterator
*
* OUTPUTS:
*       iterPtr    - pointer to iterator
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  - invalide ptr
*
* COMMENTS:
*       Usage :
*           PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_ITER_STC iter;
*           IterInit(&iter);
*           IterReset(&iter);
*           while (GT_OK == (rc = IterGetCur(&iter, &dev,&portGroup))
*           {
*                // user action for each received data e.g. print
*                IterGotoNext(&iter)
*           }
*******************************************************************************/
GT_STATUS prvCpssDxChPortPizzaArbiterIfProfileIterReset
(
    INOUT  PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_ITER_STC *iterPtr
)
{
    if (NULL == iterPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    iterPtr->curIdx = 0;
    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfProfileIterGetCur
*
* DESCRIPTION:
*       Set user profile iterator
*       traverse over list of profiles/speed conv tables
*       returns dev, portGroup,  profile or speed conversion table
*       this function returns current item
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
*
* INPUTS:
*       iterPtr    pointer to iterator
*
* OUTPUTS:
*       iterPtr    pointer to iterator
*       devNumPtr   - device number
*       portGroupIdxPtr  - port group
*       pizzaProfilePtrPtr - pointer to profile
*
* RETURNS:
*       GT_OK       - on success
*       GT_NO_MORE  - all elements were passed already
*       GT_BAD_PTR  - invalide ptr
*
* COMMENTS:
*       Usage :
*           PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_ITER_STC iter;
*           IterInit(&iter);
*           IterReset(&iter);
*           while (GT_OK == (rc = IterGetCur(&iter, &dev,&portGroup))
*           {
*               // user action for each received data e.g. print
*               IterGotoNext(&iter)
*           }
*******************************************************************************/
GT_STATUS prvCpssDxChPortPizzaArbiterIfProfileIterGetCur
(
    INOUT PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_ITER_STC *iterPtr,
    OUT   GT_U8  *devNumPtr,
    OUT   GT_U32 *portGroupIdxPtr,
    OUT   CPSS_DXCH_PIZZA_PROFILE_STC **pizzaProfilePtrPtr
)
{
    if (NULL == iterPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (NULL == pizzaProfilePtrPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (NULL == devNumPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (NULL == portGroupIdxPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    *pizzaProfilePtrPtr = NULL;
    *devNumPtr           = (GT_U8)-1;
    *portGroupIdxPtr     = (GT_U32)-1;
    if (iterPtr->curIdx >= iterPtr->storagePoolPtr->profileStorageCur)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_MORE, LOG_ERROR_NO_MSG);
    }
    *devNumPtr           = iterPtr->storagePoolPtr->pizzaProfileStorageArr[iterPtr->curIdx].devNum;
    *portGroupIdxPtr     = iterPtr->storagePoolPtr->pizzaProfileStorageArr[iterPtr->curIdx].portGroupList.list[0];
    *pizzaProfilePtrPtr  = &iterPtr->storagePoolPtr->pizzaProfileStorageArr[iterPtr->curIdx].pizzaProfile;
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfProfileIterGotoNext
*
* DESCRIPTION:
*       Set user profile iterator
*       traverse over list of profiles/speed conv tables
*       returns dev, portGroup,  profile or speed conversion table
*       this function goes to next item in the list
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
*
* INPUTS:
*       iterPtr    pointer to iterator
* OUTPUTS:
*       iterPtr    pointer to iterator
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  - invalide ptr
*
* COMMENTS:
*       Usage :
*           PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_ITER_STC iter;
*           IterInit(&iter);
*           IterReset(&iter);
*           while (GT_OK == (rc = IterGetCur(&iter, &dev,&portGroup))
*           {
*               // user action for each received data e.g. print
*               IterGotoNext(&iter)
*           }
*******************************************************************************/
GT_STATUS prvCpssDxChPortPizzaArbiterIfProfileIterGotoNext
(
    INOUT PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_ITER_STC *iterPtr
)
{
    if (NULL == iterPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (iterPtr->curIdx < iterPtr->storagePoolPtr->profileStorageCur)
    {
        iterPtr->curIdx++;
    }
    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfSpeed2SliceConvTblIterInit
*
* DESCRIPTION:
*       Set user speed conversion table list iterator
*       traverse over list of profiles/speed conv tables
*       returns dev, portGroup,  profile or speed conversion table
*       This function inits iterator
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       iterPtr    pointer to iterator
*
* OUTPUTS:
*       iterPtr    pointer to iterator
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  - invalide ptr
*
* COMMENTS:
*       Usage :
*           PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_ITER_STC iter;
*           IterInit(&iter);
*           IterReset(&iter);
*           while (GT_OK == (rc = IterGetCur(&iter, &dev,&portGroup))
*           {
*               // user action for each received data e.g. print
*               IterGotoNext(&iter)
*           }
*******************************************************************************/
GT_STATUS prvCpssDxChPortPizzaArbiterIfSpeed2SliceConvTblIterInit
(
    INOUT PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_POOL_ITER_STC *iterPtr
)
{
    if (NULL == iterPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    iterPtr->curIdx = 0;
    iterPtr->storagePoolPtr = speedConvTableStoragePoolGet();
    return GT_OK;

}

/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfSpeed2SliceConvTblIterReset
*
* DESCRIPTION:
*       Set user profile iterator
*       traverse over list of profiles/speed conv tables
*       returns dev, portGroup,  profile or speed conversion table
*       this function resets iterator to beginning of the list
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
*
* INPUTS:
*       iterPtr    pointer to iterator
*
* OUTPUTS:
*       iterPtr    pointer to iterator
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  - invalide ptr
*
* COMMENTS:
*       Usage :
*           PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_ITER_STC iter;
*           IterInit(&iter);
*           IterReset(&iter);
*           while (GT_OK == (rc = IterGetCur(&iter, &dev,&portGroup))
*           {
*               // user action for each received data e.g. print
*               IterGotoNext(&iter)
*           }
*******************************************************************************/
GT_STATUS prvCpssDxChPortPizzaArbiterIfSpeed2SliceConvTblIterReset
(
    INOUT PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_POOL_ITER_STC *iterPtr
)
{
    if (NULL == iterPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    iterPtr->curIdx = 0;
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfSpeed2SliceConvTblIterGetCur
*
* DESCRIPTION:
*       Set user profile iterator
*       traverse over list of profiles/speed conv tables
*       returns dev, portGroup,  profile or speed conversion table
*       this function returns current item
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
*
* INPUTS:
*       iterPtr    - pointer to iterator
*
* OUTPUTS:
*       iterPtr    - pointer to iterator
*       devNumPtr        - device number
*       portGroupIdxPtr  - port group
*       portGroupPortSpeed2SliceNumPtrPtr - pointer to slice consversion table
*
* RETURNS:
*       GT_OK       - on success
*       GT_NO_MORE  - all elements were passed already
*       GT_BAD_PTR  - invalide ptr
*
* COMMENTS:
*       Usage :
*           PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_ITER_STC iter;
*           IterInit(&iter);
*           IterReset(&iter);
*           while (GT_OK == (rc = IterGetCur(&iter, &dev,&portGroup))
*           {
*               // user action for each received data e.g. print
*               IterGotoNext(&iter)
*           }
*******************************************************************************/
GT_STATUS prvCpssDxChPortPizzaArbiterIfSpeed2SliceConvTblIterGetCur
(
    INOUT  PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_POOL_ITER_STC *iterPtr,
    OUT    GT_U8  *devNumPtr,
    OUT    GT_U32 *portGroupIdxPtr,
    OUT    CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC **portGroupPortSpeed2SliceNumPtrPtr
)
{
    if (NULL == iterPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (NULL == portGroupPortSpeed2SliceNumPtrPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (NULL == devNumPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (NULL == portGroupIdxPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    *portGroupPortSpeed2SliceNumPtrPtr = NULL;
    *devNumPtr           = (GT_U8)-1;
    *portGroupIdxPtr     = (GT_U32)-1;

    if (iterPtr->curIdx >= iterPtr->storagePoolPtr->storageCur)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_MORE, LOG_ERROR_NO_MSG);
    }
    *devNumPtr           = iterPtr->storagePoolPtr->speedConvTableStorageArr[iterPtr->curIdx].devNum;
    *portGroupIdxPtr     = iterPtr->storagePoolPtr->speedConvTableStorageArr[iterPtr->curIdx].portGroupList.list[0];
    *portGroupPortSpeed2SliceNumPtrPtr = &iterPtr->storagePoolPtr->speedConvTableStorageArr[iterPtr->curIdx].portSpeed2SlicesConvTbl;
    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfSpeed2SliceConvTblIterGotoNext
*
* DESCRIPTION:
*       Set user profile iterator
*       traverse over list of profiles/speed conv tables
*       returns dev, portGroup,  profile or speed conversion table
*       this function goes to next item in the list
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
*
* INPUTS:
*       iterPtr    pointer to iterator
* OUTPUTS:
*       iterPtr    pointer to iterator
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  - invalide ptr
*
* COMMENTS:
*       Usage :
*           PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_ITER_STC iter;
*           IterInit(&iter);
*           IterReset(&iter);
*           while (GT_OK == (rc = IterGetCur(&iter, &dev,&portGroup))
*           {
*               // user action for each received data e.g. print
*               IterGotoNext(&iter)
*           }
*******************************************************************************/
GT_STATUS prvCpssDxChPortPizzaArbiterIfSpeed2SliceConvTblIterGotoNext
(
    INOUT PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_POOL_ITER_STC *iterPtr
)
{
    if (NULL == iterPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (iterPtr->curIdx < iterPtr->storagePoolPtr->storageCur)
    {
        iterPtr->curIdx++;
    }
    return GT_OK;
}


/*------------------------------------------------------------------------------
**  Pizza Arbiter state get
**    Get state and configurations of all units
**        - RxDMA/RxDMA CTU
**        - TxDMA/TxDMA CTU
**        - BM
**        - TxQ
**------------------------------------------------------------------------------*/

typedef GT_STATUS (*PRV_CPSS_DXCH_PIZZA_ARBITER_UNIT_SLICE_NUM_GET_FUNC)
(
    /*IN*/GT_U8     devNum,
    /*IN*/ GT_U32   portGroupId,
    /*OUT*/GT_U32  *slicesNumPtr,
    /*OUT*/GT_BOOL *enableStrictPriority4CPUPtr
);

typedef GT_STATUS (*PRV_CPSS_DXCH_PIZZA_ARBITER_UNIT_RXDMA_SLICE_STATE_GET_FUNC)
(
    IN   GT_U8  devNum,
    IN   GT_U32 portGroupId,
    IN   GT_U32 sliceId,
    OUT  GT_BOOL *isOccupiedPtr,
    OUT  GT_PHYSICAL_PORT_NUM *portNumPtr
);

typedef struct PRV_CPSS_PORT_PIZZA_ARBITER_UNIT16_SLICE_DESCR_STC
{
    CPSS_DXCH_UNIT32_SLICES_PIZZA_ARBITER_STATE_STC *unitPtr;
    PRV_CPSS_DXCH_PIZZA_ARBITER_UNIT_SLICE_NUM_GET_FUNC   pSlicesNumGetFunPtr;
    PRV_CPSS_DXCH_PIZZA_ARBITER_UNIT_RXDMA_SLICE_STATE_GET_FUNC pSliceStateGetFunPtr;
}PRV_CPSS_PORT_PIZZA_ARBITER_UNIT16_SLICE_DESCR_STC;



/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfDumpTxQUnitStateGet
*
* DESCRIPTION:
*       Lion 2 Pizza Arbiter TxQ unit state get
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - physical device number
*       portGroupId  - number of port group
*       pSlicesNumGetFunPtr - pointer to function slice num get
*       pSliceStateGetFunPtr - pointer to function slice state get
*
* OUTPUTS:
*       unitPtr  - ponter to unit
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortPizzaArbiterIfDumpUnitStateGet
(
    IN  GT_U8  devNum,
    IN  GT_U32 portGroupId,
    IN  PRV_CPSS_DXCH_PIZZA_ARBITER_UNIT_SLICE_NUM_GET_FUNC   pSlicesNumGetFunPtr,
    IN  PRV_CPSS_DXCH_PIZZA_ARBITER_UNIT_RXDMA_SLICE_STATE_GET_FUNC pSliceStateGetFunPtr,
    OUT CPSS_DXCH_UNIT32_SLICES_PIZZA_ARBITER_STATE_STC *unitPtr
)
{
    GT_STATUS   rc;         /* return code */

    GT_U32      slicesNumberPerGop;
    GT_U32      sliceId;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_BOBCAT3_E | CPSS_CAELUM_E);

    slicesNumberPerGop = sizeof(unitPtr->slice_enable)/sizeof(unitPtr->slice_enable[0]);
    rc = (*pSlicesNumGetFunPtr)(devNum,portGroupId,/*OUT*/&unitPtr->totalConfiguredSlices,
                                                   /*OUT*/&unitPtr->strictPriorityBit);
    if(rc != GT_OK)
    {
        return rc;
    }

    for (sliceId = 0 ; sliceId < slicesNumberPerGop ; sliceId++)
    {
        rc = (*pSliceStateGetFunPtr)(/*IN*/devNum,
                                     /*IN*/portGroupId,
                                     /*IN*/sliceId,
                                     /*OUT*/&unitPtr->slice_enable[sliceId],
                                     /*OUT*/&unitPtr->slice_occupied_by[sliceId]);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfDumpTxQUnitStateGet
*
* DESCRIPTION:
*       Lion 2 Pizza Arbiter TxQ unit state get
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum   - physical device number
*       portGroupId  - number of port group
*
* OUTPUTS:
*       unitPtr  - ponter to unit
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortPizzaArbiterIfDumpTxQUnitStateGet
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    OUT CPSS_DXCH_UNIT_TXQ_SLICES_PIZZA_ARBITER_STATE_STC *unitPtr
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      sliceId;
    GT_U32      sliceId2req;
    GT_U32      i;
    CPSS_DXCH_PIZZA_PROFILE_STC * pPizzaProfilePtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_XCAT2_E);

    rc = prvCpssDxChPortPizzaArbiterIfProfileGet(/*IN*/devNum,
                                                 /*IN*/portGroupId,
                                                 /*IN*/&pPizzaProfilePtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = prvLion2PortPizzaArbiterTxQSlicesNumGet(devNum,portGroupId,
                                                 /*IN*/&pPizzaProfilePtr->txQDef,
                                                /*OUT*/&unitPtr->totalConfiguredSlices,
                                                /*OUT*/&unitPtr->slicesNumInGop);
    if(rc != GT_OK)
    {
        return rc;
    }

    for (sliceId = 0 ; sliceId < TxQ_TOTAL_AVAIBLE_SLICES_CNS ; sliceId++)
    {
        unitPtr->slice_enable     [sliceId] = GT_FALSE;
        unitPtr->slice_occupied_by[sliceId] = 0;
        rc = prvLion2PortPizzaArbiterTxQDrvSliceStateGet(/*IN*/devNum,
                                                      /*IN*/portGroupId,
                                                      /*IN*/sliceId,
                                                      /*OUT*/&unitPtr->slice_enable[sliceId],
                                                      /*OUT*/&unitPtr->slice_occupied_by[sliceId]);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    unitPtr->cpu_port_type = CPSS_DXCH_CPUPORT_TYPE_UNDEFINED_E;
    if (unitPtr->totalConfiguredSlices != unitPtr->slicesNumInGop)  /* check whether CPU port is in use */
    {
        unitPtr->cpu_port_type = pPizzaProfilePtr->txQDef.cpuPortDef.type;
        switch (pPizzaProfilePtr->txQDef.cpuPortDef.type)
        {
            case CPSS_DXCH_CPUPORT_TYPE_UNDEFINED_E:
                unitPtr->cpu_port_sliceId[0] = CPSS_PA_INVALID_SLICE;
            break;
            case CPSS_DXCH_CPUPORT_TYPE_FIXED_E:
                for (i = 0 ; pPizzaProfilePtr->txQDef.cpuPortDef.cpuPortSlices[i] != CPSS_PA_INVALID_SLICE; i++)
                {
                    unitPtr->cpu_port_sliceId[i] = pPizzaProfilePtr->txQDef.cpuPortDef.cpuPortSlices[i];
                }
                unitPtr->cpu_port_sliceId[i] = CPSS_PA_INVALID_SLICE;
            break;
            case CPSS_DXCH_CPUPORT_TYPE_EXTERNAL_E:
                unitPtr->cpu_port_sliceId[0] = unitPtr->totalConfiguredSlices-2;
                unitPtr->cpu_port_sliceId[1] = CPSS_PA_INVALID_SLICE;
            break;
            default:
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_IMPLEMENTED, LOG_ERROR_NO_MSG);
            }
        }
        /* get state of all cpu port */
        for (i = 0 ; unitPtr->cpu_port_sliceId[i] != CPSS_PA_INVALID_SLICE; i++)
        {
            sliceId2req =  unitPtr->cpu_port_sliceId[i];
            rc = prvLion2PortPizzaArbiterTxQDrvSliceStateGet(/*IN*/devNum,
                                                             /*IN*/portGroupId,
                                                             /*IN*/sliceId2req,
                                                             /*OUT*/&unitPtr->cpu_port_slice_is_enable[i],
                                                             /*OUT*/&unitPtr->cpu_port_slice_occupied_by[i]);
        }
        unitPtr->cpu_port_slice_is_enable[i]   = GT_FALSE;
        unitPtr->cpu_port_slice_occupied_by[i] = CPSS_PA_INVALID_PORT;
    }
    return GT_OK;
}


typedef enum
{
    PRV_CPSS_DXCH_PIZZA_ARBITER_UNITID_RXDMA_E       = 0,
    PRV_CPSS_DXCH_PIZZA_ARBITER_UNITID_RXDMA_CTU_E,
    PRV_CPSS_DXCH_PIZZA_ARBITER_UNITID_TXDMA_E,
    PRV_CPSS_DXCH_PIZZA_ARBITER_UNITID_TXDMA_CTU_E,
    PRV_CPSS_DXCH_PIZZA_ARBITER_UNITID_BM_E,
    PRV_CPSS_DXCH_PIZZA_ARBITER_UNITID_MAX_E
}PRV_CPSS_DXCH_PIZZA_ARBITER_UNITID_ENT;


/*******************************************************************************
* cpssDxChPortPizzaArbiterDevStateInit
*
* DESCRIPTION:
*       Init the structure CPSS_DXCH_DEV_PIZZA_ARBITER_STATE_STC
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
*
* INPUTS:
*       pizzaDeviceStatePtr - pointer to structure describing the state of each units.
*
* OUTPUTS:
*       pizzaDeviceStatePtr - pointer to structure describing the state of each units.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on pizzaDeviceStatePtr is NULL
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortTablePizzaArbiterDevStateInit
(
    INOUT CPSS_DXCH_DEV_PIZZA_ARBITER_STATE_STC *pizzaDeviceStatePtr
)
{
    if (NULL == pizzaDeviceStatePtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    cpssOsMemSet(pizzaDeviceStatePtr,0,sizeof(*pizzaDeviceStatePtr));
    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChPortTablePizzaArbiterIfDevStateGet
*
* DESCRIPTION:
*       Lion 2 Pizza Arbiter State which includes state of all
*         -
*         - state of slices
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum   - physical device number
*       portGroupId  - number of port group
*
* OUTPUTS:
*       pizzaDeviceStatePtr - pointer to structure describing the state of each units.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortTablePizzaArbiterIfDevStateGet
(
    IN  GT_U8  devNum,
    IN  GT_U32 portGroupId,
    OUT CPSS_DXCH_DEV_PIZZA_ARBITER_STATE_STC *pizzaDeviceStatePtr
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      i;

    PRV_CPSS_PORT_PIZZA_ARBITER_UNIT16_SLICE_DESCR_STC pizzaArbiterUnit16SlicesDescrArr[] =
    {
         /* NULL depicts the unit which will be recieved after */
         { NULL, prvLion2PortPizzaArbiterRxDMASlicesNumGet,    prvLion2PortPizzaArbiterRxDMASliceGetState    }
        ,{ NULL, prvLion2PortPizzaArbiterRxDMACTUSlicesNumGet, prvLion2PortPizzaArbiterRxDMACTUSliceGetState }
        ,{ NULL, prvLion2PortPizzaArbiterTxDMASlicesNumGet,    prvLion2PortPizzaArbiterTxDMASliceGetState    }
        ,{ NULL, prvLion2PortPizzaArbiterTxDMACTUSlicesNumGet, prvLion2PortPizzaArbiterTxDMACTUSliceGetState }
        ,{ NULL, prvLion2PortPizzaArbiterBMSlicesNumGet,       prvLion2PortPizzaArbiterBMSliceGetState       }
    };

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    rc = prvCpssDxChPortTablePizzaArbiterDevStateInit(pizzaDeviceStatePtr);
    if (GT_OK != rc)
    {
        return rc;
    }

    pizzaDeviceStatePtr->devType = CPSS_PP_FAMILY_DXCH_LION2_E;

    pizzaArbiterUnit16SlicesDescrArr[PRV_CPSS_DXCH_PIZZA_ARBITER_UNITID_RXDMA_E    ].unitPtr = &pizzaDeviceStatePtr->devState.lion2.rxDMA;
    pizzaArbiterUnit16SlicesDescrArr[PRV_CPSS_DXCH_PIZZA_ARBITER_UNITID_RXDMA_CTU_E].unitPtr = &pizzaDeviceStatePtr->devState.lion2.rxDMA_CTU;
    pizzaArbiterUnit16SlicesDescrArr[PRV_CPSS_DXCH_PIZZA_ARBITER_UNITID_TXDMA_E    ].unitPtr = &pizzaDeviceStatePtr->devState.lion2.txDMA;
    pizzaArbiterUnit16SlicesDescrArr[PRV_CPSS_DXCH_PIZZA_ARBITER_UNITID_TXDMA_CTU_E].unitPtr = &pizzaDeviceStatePtr->devState.lion2.txDMA_CTU;
    pizzaArbiterUnit16SlicesDescrArr[PRV_CPSS_DXCH_PIZZA_ARBITER_UNITID_BM_E       ].unitPtr = &pizzaDeviceStatePtr->devState.lion2.BM;

    /*-----------------------------------------*/
    /*  RxDMA RxDMA CTU TxDMA TxDMA CTU  BM    */
    /*-----------------------------------------*/
    for (i = 0 ; i < sizeof (pizzaArbiterUnit16SlicesDescrArr)/sizeof(pizzaArbiterUnit16SlicesDescrArr[0]); i++)
    {
        rc = prvCpssDxChPortPizzaArbiterIfDumpUnitStateGet(/*IN*/devNum,
                                                           /*IN*/portGroupId,
                                                           /*IN*/pizzaArbiterUnit16SlicesDescrArr[i].pSlicesNumGetFunPtr,
                                                           /*IN*/pizzaArbiterUnit16SlicesDescrArr[i].pSliceStateGetFunPtr,
                                                           /*OUT*/pizzaArbiterUnit16SlicesDescrArr[i].unitPtr);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /*------------*/
    /*  TxQ       */
    /*------------*/
    rc = prvCpssDxChPortPizzaArbiterIfDumpTxQUnitStateGet(/*IN*/devNum,
                                                          /*IN*/portGroupId,
                                                          /*OUT*/&pizzaDeviceStatePtr->devState.lion2.TxQ);

    return rc;
}




#ifdef WIN32_DEBUG_TEST_FUNCTION

    GT_STATUS PizzaSim_UserProfileListPrint(void)
    {
        GT_U32    i;
        GT_STATUS rc;
        GT_U8 devNum;
        GT_U32 portGroupIdx;
        CPSS_DXCH_PIZZA_PROFILE_STC  * pProfilePtr;
        PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_ITER_STC iter;

        prvCpssDxChcpssDxChPortPizzaArbiterIfProfileIterInit(&iter);
        prvCpssDxChcpssDxChPortPizzaArbiterIfProfileIterReset(&iter);
        i = 0;
        while (GT_OK == (rc = prvCpssDxChcpssDxChPortPizzaArbiterIfProfileIterGetCur(&iter,
                                                                                   /*OUT*/&devNum,
                                                                                   /*OUT*/&portGroupIdx,
                                                                                   /*OUT*/&pProfilePtr)))
        {
            printf("\nProfile %d dev = %2d portGroup = %2d",i,devNum, portGroupIdx);
            PizzaSim_ProfilePrint(pProfilePtr);

            i++;
            prvCpssDxChcpssDxChPortPizzaArbiterIfProfileIterGotoNext(&iter);
        }
        return GT_OK;
    }


    GT_STATUS PizzaSim_Speed2SlaceConvTblListPrint(void)
    {
        GT_U32    i;
        GT_STATUS rc;
        GT_U8 devNum;
        GT_U32 portGroupIdx;
        CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC * table2UsePtr;
        PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_POOL_ITER_STC iter;

        prvCpssDxChcpssDxChPortPizzaArbiterIfSpeed2SliceConvTblIterInit(&iter);
        prvCcpssDxChcpssDxChPortPizzaArbiterIfSpeed2SliceConvTblIterReset(&iter);
        i = 0;
        while (GT_OK == (rc = prvCpssDxChcpssDxChPortPizzaArbiterIfSpeed2SliceConvTblIterGetCur(&iter,
                                                                                   /*OUT*/&devNum,
                                                                                   /*OUT*/&portGroupIdx,
                                                                                   /*OUT*/&table2UsePtr)))
        {
            printf("\nSpeed Conv Table %d dev = %2d portGroup = %2d",i,devNum, portGroupIdx);
            PizzaSim_Speed2SlaceConvTblPrint(table2UsePtr);

            i++;
            prvCpssDxChcpssDxChPortPizzaArbiterIfSpeed2SliceConvTblIterGotoNext(&iter);
        }
        return GT_OK;
    }

    extern CPSS_DXCH_PIZZA_PROFILE_STC  pizzaProfileLion2B0Clock360MHz12Ports24Sllices;
    extern CPSS_DXCH_PIZZA_PROFILE_STC  pizzaProfileLion2B0Clock480MHz12Ports32Sllices;
    extern const CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC prvPortGroupPortSpeed2SliceNumDefault;

    GT_STATUS PizzaSim_ProfileAdd(void)
    {
        GT_STATUS rc;
        CPSS_DXCH_PIZZA_PROFILE_STC * profilePtr;
        CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC * speedConvTblPtr;

        prvCpssDxChPortPizzaArbiterIfProfileSet             (0,0,(CPSS_DXCH_PIZZA_PROFILE_STC *)&pizzaProfileLion2B0Clock480MHz12Ports32Sllices);
        prvCpssDxChPortPizzaArbiterIfSpeed2SliceConvTableSet(0,0,(CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC *)&prvPortGroupPortSpeed2SliceNumDefault);
        //cpssDxChPortPizzaArbiterIfProfileSet(0,1,&pizzaProfileLion2B0Clock480MHz12Ports32Sllices);

        prvCpssDxChPortPizzaArbiterIfProfileGet(0,0,&profilePtr);
        printf("\nProfile dev = %2d portGroup = %2d",0,0);
        PizzaSim_ProfilePrint(profilePtr);
        prvCpssDxChPortPizzaArbiterIfSpeed2SliceConvTableGet(0,0,&speedConvTblPtr);
        printf("\nSpeed Conv Table dev = %2d portGroup = %2d",0,0);
        PizzaSim_Speed2SlaceConvTblPrint(speedConvTblPtr);

        cpssDxChPortPizzaArbiterIfUserTableSet(0,1,(CPSS_DXCH_PIZZA_PROFILE_STC *)&pizzaProfileLion2B0Clock480MHz12Ports32Sllices,
                                                   (CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC *)&prvPortGroupPortSpeed2SliceNumDefault);
        cpssDxChPortPizzaArbiterIfUserTableGet(0,1,&profilePtr,
                                                   &speedConvTblPtr);
        printf("\nProfile dev = %2d portGroup = %2d",0,1);
        PizzaSim_ProfilePrint(profilePtr);
        printf("\nSpeed Conv Table dev = %2d portGroup = %2d",0,1);
        PizzaSim_Speed2SlaceConvTblPrint(speedConvTblPtr);

        /*-------------------------------------------*/
        rc = cpssDxChPortPizzaArbiterIfUserTableSet(0,2,(CPSS_DXCH_PIZZA_PROFILE_STC *)&pizzaProfileLion2B0Clock480MHz12Ports32Sllices,
                                                   NULL);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChPortPizzaArbiterIfUserTableGet(0,2,&profilePtr,NULL);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortPizzaArbiterIfUserTableSet(0,2,NULL,
                                                   (CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC *)&prvPortGroupPortSpeed2SliceNumDefault);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortPizzaArbiterIfUserTableGet(0,2,NULL,
                                                   &speedConvTblPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChPortPizzaArbiterIfUserTableSet(0,3,NULL, NULL);
        if (rc != GT_BAD_PTR)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
        rc = cpssDxChPortPizzaArbiterIfUserTableGet(0,3,NULL, NULL);
        if (rc != GT_BAD_PTR)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }

        rc = cpssDxChPortPizzaArbiterIfUserTableGet(0,10,NULL, &speedConvTblPtr);
        if (rc != GT_NOT_FOUND)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }

        rc = cpssDxChPortPizzaArbiterIfUserTableGet(0,10,&profilePtr, NULL);
        if (rc != GT_NOT_FOUND)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }

        rc = cpssDxChPortPizzaArbiterIfUserTableGet(0,10,&profilePtr, &speedConvTblPtr);
        if (rc != GT_NOT_FOUND)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }

        PizzaSim_UserProfileListPrint();
        PizzaSim_Speed2SlaceConvTblListPrint();
        return GT_OK;
    }

#endif

