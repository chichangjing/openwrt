/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortFastInit.c
*
* DESCRIPTION:
*       CPSS implementation for Port initialization.
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*******************************************************************************/

/* macro needed to support the call to PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC */
/* this define must come before include files */
#define PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_SUPPORTED_FLAG_CNS
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortLog.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>

extern CPSS_DXCH_PORT_SERDES_SPEED_ENT serdesFrequency[CPSS_PORT_INTERFACE_MODE_NA_E][CPSS_PORT_SPEED_NA_E];

extern GT_STATUS prvCpssDxChPortTraceDelay
(
    IN GT_U8       devNum,
    IN GT_U32      portGroupId,
    IN GT_U32      millisec
);

static GT_STATUS macSpeedSet
(
    IN  GT_U8                        devNum,
    IN  GT_PHYSICAL_PORT_NUM         portNum,
    IN  CPSS_PORT_SPEED_ENT          speed
);

static GT_STATUS serdesPowerUpConfigure
(
    IN  GT_U8                        devNum,
    IN  CPSS_PORTS_BMP_STC           portsBmp,
    IN  CPSS_PORT_INTERFACE_MODE_ENT ifMode,
    IN  CPSS_PORT_SPEED_ENT          speed
);

/*******************************************************************************
* prvCpssDxChPortFastXgPcsResetStateSet
*
* DESCRIPTION:
*       Just set XG and above PCS Reset state.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; Lion2.
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
*       state    - Reset state
*               GT_TRUE   - Port pcs is under Reset
*               GT_FALSE - Port pcs is Not under Reset, normal operation
*
* OUTPUTS:
*       None.
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
GT_STATUS prvCpssDxChPortFastXgPcsResetStateSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 state
)
{
    GT_STATUS   rc;             /* return code */
    GT_U32      regAddr;        /* register address */
    GT_U32      value;          /* value to write into the register */
    GT_U32      portGroupId;    /* local core number */

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) < PRV_CPSS_PORT_XG_E)
    {
        return GT_OK;
    }

    portGroupId =
        PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
                                                                    portNum);

    if((GT_TRUE == state) || (PRV_CPSS_PORT_XG_E == 
                              PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum)))
    {
        value = BOOL2BIT_MAC(!state);
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                      macRegs.perPortRegs[portNum].xgGlobalConfReg0;       
        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                        0, 1, value);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    if((GT_TRUE == state) || (PRV_CPSS_PORT_XLG_E == 
                              PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum)))
    {
        value = BOOL2BIT_MAC(state);
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->xlgRegs.pcs40GRegs.commonCtrl;
        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, 
                                                    9, 1, value);
            if (rc != GT_OK)
                return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortFastSerdesResetStateSet
*
* DESCRIPTION:
*       Set SERDES Reset state on specified port on specified device.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number (not CPU port)
*       startSerdes - first SERDES number
*       serdesesNum - number of SERDESes
*       state  - Reset state
*                GT_TRUE  - Port SERDES is under Reset
*                GT_FALSE - Port SERDES is Not under Reset, normal operation
*
* OUTPUTS:
*       None.
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
GT_STATUS prvCpssDxChPortFastSerdesResetStateSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_U32    startSerdes,
    IN  GT_U32    serdesesNum,
    IN  GT_BOOL   state
)
{
    GT_U32    regValue;     /* register field value */
    GT_U32    regAddr;      /* register address */
    GT_U32    fieldOffset;  /* register field offset */
    GT_U32    i;            /* iterator */
    GT_U32    portGroupId;  /* local core number */
    GT_U32    localPort;    /* number of port in local core */
    GT_STATUS rc;           /* return code */

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum);

    regValue = BOOL2BIT_MAC(!state);
    fieldOffset = 3; /* sd_reset_in */
    for (i = startSerdes; i < startSerdes + serdesesNum; i++)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].serdesExternalReg2;
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId , regAddr, 
                                                fieldOffset, 1, regValue);
        if(rc !=GT_OK)
        {
            return rc;
        }
    }

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        if((CPSS_PORT_INTERFACE_MODE_XGMII_E == PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,portNum))
            && (CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E == PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,portNum+1))
            && (state == GT_TRUE))
        {   /* each bit in Serdes miscellaneous reg. resets all lanes of port accordingly to current
             * interface mode 4 for XAUI, 2 for RXAUI etc., so lets fake here 2 serdes mode
             */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.mediaInterface;
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, 
                                                   startSerdes, serdesesNum,
                                                   BIT_MASK_MAC(serdesesNum)) != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }
        }

        /* Set Serdes reset latency */
        for (i = startSerdes; i < startSerdes + serdesesNum; i++)
        {
            regAddr = 0x9800010 + 0x400*i;
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, 
                                                    8, 1, 1);
            if(rc !=GT_OK)
            {
                return rc;
            }
        }

        /* sd_reset using Serdes miscellaneous register */
        regAddr = (localPort < 6) ? 0x9800010 : 0x9803010;
        /* if number of lanes is 8 we are in XLG mode - use bit 6 */
        fieldOffset = (serdesesNum < 8) ? (localPort % 6) : 6;
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, 
                                                fieldOffset, 1, regValue);
        if(rc !=GT_OK)
        {
            return rc;
        }

        if((CPSS_PORT_INTERFACE_MODE_XGMII_E == PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,portNum))
            && (CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E == PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,portNum+1))
            && (state == GT_TRUE))
        {/* restore original XAUI media interface mode */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.mediaInterface;
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, 
                                                   startSerdes, serdesesNum, 0) 
                != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortFastMacResetStateSet
*
* DESCRIPTION:
*       Set MAC Reset state on specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number (or CPU port)
*       state    - Reset state
*               GT_TRUE   - Port MAC is under Reset
*               GT_FALSE - Port MAC is Not under Reset, normal operation
*
* OUTPUTS:
*       None.
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
GT_STATUS prvCpssDxChPortFastMacResetStateSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 state
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      regAddr;    /* register address */
    GT_U32      value;      /* value to write into the register */
    GT_U32      offset;     /* bit number inside register       */
    GT_U32      portGroupId;/* local core number */
    GT_U32      localPort;  /* port number in local core */

    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(
                                                                devNum, portNum);
    offset = 1;
    value = BOOL2BIT_MAC(!state);
    if((GT_TRUE == state) || (PRV_CPSS_PORT_XG_E == 
                              PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum, portNum)))
    {/* set MACResetn bit */
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,PRV_CPSS_PORT_XG_E,&regAddr);
        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            if((rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                        offset, 1, value)) != GT_OK)
            {
                return rc;
            }
        }
    }

    if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
       && ((localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, 
                                                                       portNum)) 
           == 10))
    {
        if((GT_TRUE == state) || (PRV_CPSS_PORT_XLG_E == 
                                  PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum, portNum)))
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,PRV_CPSS_PORT_XLG_E,&regAddr);
            if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
            {
                if((rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                            offset, 1, value)) != GT_OK)
                {
                    return rc;
                }
            }
        }
    }

    if((GT_TRUE == state) || (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) < 
                              PRV_CPSS_PORT_XG_E))
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,PRV_CPSS_PORT_GE_E,&regAddr);
        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            offset = 6;
            value = BOOL2BIT_MAC(state);
            if((rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                        offset, 1, value)) != GT_OK)
            {
                return rc;
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* fastPortGeConfig
*
* DESCRIPTION:
*       Make GE specific configurations
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       ifMode    - Interface mode
*       speed     - port speed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*
* COMMENTS:
*        None
*
*******************************************************************************/
static GT_STATUS fastPortGeConfig
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode,
    IN  CPSS_PORT_SPEED_ENT            speed
)
{
    GT_U32      regAddr;        /* register address */
    GT_STATUS   rc;             /* return code     */
    GT_U32      portGroupId;    /* number of local core */

    portGroupId = 
        PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, 
                                                                   portNum);

    /* PcsEn field in Port MAC Control Register2*/
    PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum, portNum, PRV_CPSS_PORT_GE_E,
                                         &regAddr);
    if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                           3, 1, 1) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChPortGePortTypeSet(devNum, portNum, ifMode);
    if(rc != GT_OK)
    {
        return rc;
    }

    return geMacUnitSpeedSet(devNum, portNum, speed);
}

/*******************************************************************************
* fastPortXpcsActiveLanesSet
*
* DESCRIPTION:
*       Configure number of active XPCS lanes accordingly to interface mode
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       ifMode    - Interface mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*
* COMMENTS:
*        None
*
*******************************************************************************/
static GT_STATUS fastPortXpcsActiveLanesSet
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      actLanesNum;    /* value to write into the register */
    GT_U32      portGroupId;    /* number of local core */

    switch (ifMode)
    {
        case CPSS_PORT_INTERFACE_MODE_HX_E:
            actLanesNum = 1;
            break;
        case CPSS_PORT_INTERFACE_MODE_XGMII_E:
        case CPSS_PORT_INTERFACE_MODE_RXAUI_E:
        case CPSS_PORT_INTERFACE_MODE_XLG_E:
        case CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E:
            /* RXAUI and XAUI both use 4 lanes, but RXAUI uses 2 SerDes
               while XAUI uses 4 */
            actLanesNum = 2;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    portGroupId = 
        PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, 
                                                                   portNum);
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum].
                                                                xgGlobalConfReg0;
    /* set LaneActive */
    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, 
                                           5, 2, actLanesNum);
}

/*******************************************************************************
* fastPortInitPhase1
*
* DESCRIPTION:
*       First phase of fast port init
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       ifMode    - Interface mode
*       speed     - port speed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*
* COMMENTS:
*        None
*
*******************************************************************************/
static GT_STATUS fastPortInitPhase1
(
    IN  GT_U8                          devNum,
    IN  CPSS_PORTS_BMP_STC             portsBmp,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode,
    IN  CPSS_PORT_SPEED_ENT            speed
)
{
    GT_U32      regAddr;        /* register address */
    GT_STATUS   rc;             /* return code     */
    GT_U32      portGroupId;    /*the port group Id - support multi-port-groups device */
    GT_U32      startSerdes;    /* number of first serdes used by port */
    GT_U32      numOfSerdesLanes; /* number of serdes lanes occupied by port */
    GT_PHYSICAL_PORT_NUM portNum; /* physical port number */
    GT_U32      macSelect;

    for(portNum = 0; portNum < PRV_CPSS_PP_MAC(devNum)->numOfPorts; portNum++)
    {
        if(GT_FALSE == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&portsBmp, portNum))
        {
            continue;
        }

        /* update current port type - used to define which mac unit currently in use by port */
        prvCpssDxChPortTypeSet(devNum, portNum, ifMode, speed);

        /* update addresses of mac registers accordingly to used MAC 1G/XG/XLG */
        rc = prvCpssDxChHwRegAddrPortMacUpdate(devNum, portNum, ifMode);
        if (rc != GT_OK)
        {
            return rc;
        }

        if((rc = prvCpssDxChPortIfModeCheckAndSerdesNumGet(devNum, portNum, ifMode,
                                                           &startSerdes,
                                                           &numOfSerdesLanes)) 
           != GT_OK)
        {
            return rc;
        }

        PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portNum) = ifMode;
        PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum, portNum) = speed;

        rc = prvCpssDxChPortFastSerdesResetStateSet(devNum, portNum, startSerdes,
                                                    numOfSerdesLanes, GT_TRUE);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDxChPortFastXgPcsResetStateSet(devNum,portNum,GT_TRUE);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDxChPortFastMacResetStateSet(devNum,portNum,GT_TRUE);
        if (rc != GT_OK)
        {
            return rc;
        }

        portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, 
                                                                                 portNum);

        /* RXAUI -> other
            1.  Change MAC Mode to 1G (this makes sure that MAC uses 1 SERDES only and
                                                        0x38C will not kill adjacent port);
            2.  Change Media interface to "non-RXAUI" (clear the relevant bit in 0x38C);

           other -> RXAUI
            1.  Change Media interface to RXAUI(set relevant bits in 0x38C)
                This makes sure that adjacent port is NOT getting affected since we are
                 still in 1G MAC mode and port uses 1 serdes.
            2.  Change MAC Mode to 10G. Is safe now since 0x38C is configured to RXAUI already.
        */
        if(CPSS_PORT_INTERFACE_MODE_RXAUI_E == ifMode)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.mediaInterface;
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, 
                                                   startSerdes, 2, 0x3) != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }
        }

        /* MAC Mode is selected in XG MAC.
            This field is not applicable for 40G ports and for 1G
            it has some other purpose */
        PRV_CPSS_DXCH_PORT_MAC_CTRL3_REG_MAC(devNum, portNum, PRV_CPSS_PORT_XG_E,
                                             &regAddr);
        switch (ifMode)
        {
            case CPSS_PORT_INTERFACE_MODE_SGMII_E:
            case CPSS_PORT_INTERFACE_MODE_1000BASE_X_E:
                macSelect = 0;
                break;
            case CPSS_PORT_INTERFACE_MODE_HX_E:
            case CPSS_PORT_INTERFACE_MODE_XGMII_E: /* both 10G and 20G */
            case CPSS_PORT_INTERFACE_MODE_RXAUI_E:
            case CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E:
                macSelect = 1;
                break;
            case CPSS_PORT_INTERFACE_MODE_XLG_E:
                macSelect = 3;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, 13, 3, 
                                               macSelect) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

        if(ifMode != CPSS_PORT_INTERFACE_MODE_RXAUI_E)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.mediaInterface;
            if(portNum%2 != 0)
            {/* if we configure odd port while his less significant even neighbour 
                port not configured we must change media of even port to RXAUI 
                otherwise won't be link on odd port, although odd port requires XAUI 
                media on his serdeses */
                if(CPSS_PORT_INTERFACE_MODE_NA_E == PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portNum-1))
                {
                    if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, 
                                                           startSerdes-2, 2, 0x3) != GT_OK)
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
                    }
                }
            }

            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, 
                                                   startSerdes, numOfSerdesLanes, 
                                                   0) != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }
        }

        rc = macSpeedSet(devNum, portNum, speed);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum, portNum) == PRV_CPSS_PORT_GE_E)
        {
            rc = fastPortGeConfig(devNum, portNum, ifMode, speed);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
        else
        {
            rc = fastPortXpcsActiveLanesSet(devNum, portNum, ifMode);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

    } /* for(portNum = 0 */

    return GT_OK;
}

/*******************************************************************************
* fastPortInitPhase2
*
* DESCRIPTION:
*       Second phase of fast port init
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       ifMode    - Interface mode
*       speed     - port speed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*
* COMMENTS:
*        None
*
*******************************************************************************/
static GT_STATUS fastPortInitPhase2
(
    IN  GT_U8                          devNum,
    IN  CPSS_PORTS_BMP_STC             portsBmp,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode,
    IN  CPSS_PORT_SPEED_ENT            speed
)
{
    GT_STATUS               rc;                 /* return code */
    GT_U32                  startSerdes;    /* first serdes */
    GT_U32                  serdesesNum;    /* serdes quantity */
    GT_U32                  portGroupId;     /* the port group Id - support multi-port-groups device */
    GT_PHYSICAL_PORT_NUM    localPort;  /* local port - support multi-port-groups device */
    CPSS_DXCH_PORT_SERDES_SPEED_ENT serdesSpeed,/* serdes frequency to configure */
                                    serdesSpeedPrev;    /* old serdes frequency */
    GT_U32                  regAddr;    /* register address */
    GT_U32                  fieldOffset; /* offset in register */
    GT_PHYSICAL_PORT_NUM    portNum;    /* physical port number */
    CPSS_PORTS_BMP_STC      fixedPortsBmp; /* ports bitmap without not needed 
                                                    ports */
    GT_U32                  i;          /* iterator */

    serdesSpeed = serdesFrequency[ifMode][speed];
    for(portNum = 0, fixedPortsBmp = portsBmp; portNum < 
                                PRV_CPSS_PP_MAC(devNum)->numOfPorts; portNum++)
    {
        if(GT_FALSE == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&portsBmp, portNum))
        {
            continue;
        }

        if((rc = prvCpssDxChPortIfModeCheckAndSerdesNumGet(devNum, portNum, ifMode,
                                                           &startSerdes,
                                                           &serdesesNum)) != GT_OK)
        {
            return rc;
        }
    
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);
        /* if special mode required and port is odd, check that even port
           configuration compatible to required */
        if((CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E == ifMode) && ((localPort%2) != 0))
        {
            if(CPSS_PORT_INTERFACE_MODE_XGMII_E == 
               PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portNum-1))
            {
                portGroupId = 
                    PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, 
                                                                               portNum);
                rc = prvGetLpSerdesSpeed(devNum, portGroupId, startSerdes-2,
                                         &serdesSpeedPrev);
                if (rc != GT_OK)
                {
                    return rc;
                }

                if(serdesSpeedPrev == serdesSpeed)
                {/* needed speed already configured */
                    CPSS_PORTS_BMP_PORT_CLEAR_MAC(&fixedPortsBmp, portNum);
                    continue;
                }
            }
        }

    }/* for(portNum = 0;  */

    rc = serdesPowerUpConfigure(devNum, fixedPortsBmp, ifMode, speed);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* RxInit set and unset */
    for(portNum = 0; portNum < PRV_CPSS_PP_MAC(devNum)->numOfPorts; portNum++)
    {
        if(GT_FALSE == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&fixedPortsBmp, portNum))
        {
            continue;
        }

        portGroupId = 
            PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, 
                                                                       portNum);

        if((rc = prvCpssDxChPortIfModeCheckAndSerdesNumGet(devNum, portNum, ifMode,
                                                           &startSerdes,
                                                           &serdesesNum)) != GT_OK)
        {
            return rc;
        }

        PRV_PER_SERDES_LOOP_MAC
        {            
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].serdesExternalReg2;
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                    4, 1, 1);
            if (rc != GT_OK)
                return rc;
        }
    }

    rc = prvCpssDxChPortTraceDelay(devNum, 0, 10);
    if(rc != GT_OK)
    {
        return rc;
    }

    for(portNum = 0; portNum < PRV_CPSS_PP_MAC(devNum)->numOfPorts; portNum++)
    {
        if(GT_FALSE == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&fixedPortsBmp, portNum))
        {
            continue;
        }

        portGroupId = 
            PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, 
                                                                       portNum);

        if((rc = prvCpssDxChPortIfModeCheckAndSerdesNumGet(devNum, portNum, ifMode,
                                                           &startSerdes,
                                                           &serdesesNum)) != GT_OK)
        {
            return rc;
        }

        PRV_PER_SERDES_LOOP_MAC
        {            
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].serdesExternalReg2;
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                    4, 1, 0);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    for(portNum = 0; portNum < PRV_CPSS_PP_MAC(devNum)->numOfPorts; portNum++)
    {
        if(GT_FALSE == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&fixedPortsBmp, portNum))
        {
            continue;
        }

        rc = prvCpssDxChPortIfModeCheckAndSerdesNumGet(devNum, portNum, ifMode,
                                                        &startSerdes, &serdesesNum);
        if(rc != GT_OK)
        {
            return rc;
        }

        portGroupId = 
            PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, 
                                                                       portNum);
        /* Set Serdes reset latency */
        PRV_PER_SERDES_LOOP_MAC
        {
            regAddr = 0x9800010 + 0x400*i;
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, 
                                                    8, 1, 1);
            if(rc !=GT_OK)
            {
                return rc;
            }
        }

        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, 
                                                                   portNum);
        /* sd_reset using Serdes miscellaneous register */
        regAddr = (localPort < 6) ? 0x9800010 : 0x9803010;
        /* if number of lanes is 8 we are in XLG mode - use bit 6 */
        fieldOffset = (serdesesNum < 8) ? (localPort % 6) : 6;
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, 
                                                fieldOffset, 1, 1);
        if(rc !=GT_OK)
        {
            return rc;
        }
    }

    rc = prvCpssDxChPortTraceDelay(devNum, 0, 10);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* De assert PCS Reset */
    for(portNum = 0; portNum < PRV_CPSS_PP_MAC(devNum)->numOfPorts; portNum++)
    {
        if(GT_FALSE == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&fixedPortsBmp, portNum))
        {
            continue;
        }

        rc = prvCpssDxChPortFastXgPcsResetStateSet(devNum,portNum,GT_FALSE);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    rc = prvCpssDxChPortTraceDelay(devNum, 0, 5);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* De assert MAC Reset */
    for(portNum = 0; portNum < PRV_CPSS_PP_MAC(devNum)->numOfPorts; portNum++)
    {
        if(GT_FALSE == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&fixedPortsBmp, portNum))
        {
            continue;
        }

        rc = prvCpssDxChPortFastMacResetStateSet(devNum,portNum,GT_FALSE);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    rc = prvCpssDxChPortTraceDelay(devNum, 0, 5);
    if(rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* macSpeedSet
*
* DESCRIPTION:
*       Configure mac and if needed WA
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion2.
*
* INPUTS:
*       devNum - physical device number
*       portNum - physical port number
*       speed  - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - the speed is not supported on the port
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong speed
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS macSpeedSet
(
    IN  GT_U8                        devNum,
    IN  GT_PHYSICAL_PORT_NUM         portNum,
    IN  CPSS_PORT_SPEED_ENT          speed
)
{
    GT_STATUS rc;   /* return code */
    GT_U32      portGroupId;     /*the port group Id - support multi-port-groups device */
    GT_PHYSICAL_PORT_NUM localPort;  /* local port - support multi-port-groups device */
    GT_U32 regAddr; /* register address */
    GT_U32 regValue;    /* register value */
    GT_U32 fieldOffset; /* offset of configured field in register */
    GT_U32 index;   /* iterator */

    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    switch(speed)
    {
        case     CPSS_PORT_SPEED_10_E:
        case     CPSS_PORT_SPEED_100_E:
        case     CPSS_PORT_SPEED_1000_E:
        case     CPSS_PORT_SPEED_2500_E:
        case     CPSS_PORT_SPEED_5000_E:
        case     CPSS_PORT_SPEED_10000_E:
            regValue = 0;
            break;

        case     CPSS_PORT_SPEED_16000_E:
        case     CPSS_PORT_SPEED_20000_E:
            regValue = 0x15;
            break;

        case     CPSS_PORT_SPEED_40000_E:
            regValue = 0x17;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }
    index = (localPort < 6) ? 0 : 1;
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.txdmaThresholdOverrideEn[index];
    fieldOffset = (localPort%6)*5;
    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,fieldOffset,5,regValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(GT_FALSE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                                    PRV_CPSS_DXCH_LION_XLG_WS_SUPPORT_WA_E))
    {
        return GT_OK;
    }

    /* Metal_fix[31] - Enable Strict Priority to port10 (40Gbps)
        Metal_fix[30] - Port0 is 20 Gpbs
        Metal_fix[29] - Port2 is 20 Gpbs
        Metal_fix[28] - Port4 is 20 Gpbs
        Metal_fix[27] - Port6 is 20 Gpbs
        Metal_fix[26] - Port8 is 20 Gpbs
        Metal_fix[25] - Port10 is 20 Gpbs
        Metal_fix[24] - Enable fix
        NOTE: - When one of the bits Metal_fix[31:25] is SET, these 2 other configurations must be set as well:
        1. Metal_fix[24]
        2. Scheduler/Scheduler Configuration/Scheduler Configuration/DeficitEnable
    */
    if(localPort%2 != 0)
    {
        return GT_OK;
    }
    
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.global.debug.metalFix;
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId,regAddr,24,8,&regValue);
    if (rc != GT_OK)
    {
        return rc;
    }
    if((speed == CPSS_PORT_SPEED_20000_E) || (speed == CPSS_PORT_SPEED_16000_E))
    {
        U32_SET_FIELD_MAC(regValue,6-(localPort/2),1,1);
        /* disable metal fix for 40G */
        if(localPort == 10)
            U32_SET_FIELD_MAC(regValue,7,1,0);
    
        /* enable metal fix */
        U32_SET_FIELD_MAC(regValue,0,1,1);
    }
    else if(speed == CPSS_PORT_SPEED_40000_E)
    {
        /* disable metal fix for 20G on ports 8 and 10 */
        U32_SET_FIELD_MAC(regValue,1,2,0);
    
        /* enable metal fix for 40G */
        U32_SET_FIELD_MAC(regValue,7,1,1);
    
        /* enable metal fix */
        U32_SET_FIELD_MAC(regValue,0,1,1);
    }
    else
    {
        U32_SET_FIELD_MAC(regValue,6-(localPort/2),1,0);
        if(localPort == 10)
            U32_SET_FIELD_MAC(regValue,7,1,0);
    
        /* if just enable metal fix bit left - disable metal fix */
        if(1 == regValue)
            U32_SET_FIELD_MAC(regValue,0,1,0);
    }
    
    /* write full configuration at once to avoid state where metal fix enabled
        and no port requiring it set */
    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,24,8,regValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* serdesFastConfig
*
* DESCRIPTION:
*       Write to HW serdes power sequence
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat3; Lion2.
*
* INPUTS:
*       devNum      - physical device number
*       portGroupId - port group number
*       startSerdes - first serdes to configure
*       serdesesNum - number of serdeses to configure
*       serdesSpeed - required serdes frequency
*       serdesPowerUpSequencePtr - (ptr to) serdes power up sequnce array
*       cfgStep     - current step in serdes power up sequence
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS serdesFastConfig
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   startSerdes,
    IN GT_U32   serdesesNum,
    IN CPSS_DXCH_PORT_SERDES_SPEED_ENT serdesSpeed,
    IN PRV_CPSS_DXCH_PORT_SERDES_POWER_UP_ARRAY    *serdesPowerUpSequencePtr,
    IN   GT_U32  cfgStep
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      i,          /* iterator */
                regAddr,    /* register address */
                regValue,   /* register value */
                regAddr2,    /* register address */
                regValue2,   /* register value */
                *serdesRegsAddrStructPtr; /* (ptr to) registers addresses DB */
    CPSS_DXCH_PORT_SERDES_TUNE_STC *tuneValuesPtr; /* serdes board specific fine 
                                                        tuning values structure */
    GT_U32      lanesNumInDev;
    GT_U32      globalLaneNum;

    lanesNumInDev = prvCpssDxChHwInitNumOfSerdesGet(devNum);

    globalLaneNum = (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum) == 0) ? 0 :
                                                    (portGroupId*lanesNumInDev);
    PRV_PER_SERDES_LOOP_MAC
    {
        serdesRegsAddrStructPtr = (GT_U32*)&PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i];
        regValue = serdesPowerUpSequencePtr[cfgStep][serdesSpeed];
        regAddr = serdesRegsAddrStructPtr[serdesPowerUpSequencePtr[cfgStep]
                                            [CPSS_DXCH_PORT_SERDES_SPEED_NA_E]];

        if(NULL != PRV_CPSS_DXCH_PP_MAC(devNum)->serdesCfgDbArrPtr)
        {/* if serdes fine tuning values DB initialized */
            if(PRV_CPSS_DXCH_PP_MAC(devNum)->serdesCfgDbArrPtr[globalLaneNum+i] != NULL)
            {
                tuneValuesPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->serdesCfgDbArrPtr[globalLaneNum+i][serdesSpeed];

                switch(serdesPowerUpSequencePtr[cfgStep][CPSS_DXCH_PORT_SERDES_SPEED_NA_E])
                {
                    case PRV_CPSS_DXCH_PORT_SERDES_FFE_REG0_OFFSET_E:
                        U32_SET_FIELD_MAC(regValue,0,4,tuneValuesPtr->ffeC);
                        U32_SET_FIELD_MAC(regValue,4,3,tuneValuesPtr->ffeR);
                        U32_SET_FIELD_MAC(regValue,8,2,tuneValuesPtr->ffeS);
                    break;

                    case PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG0_OFFSET_E:
                        U32_SET_FIELD_MAC(regValue,15,1,BOOL2BIT_MAC(tuneValuesPtr->txEmphEn));
                    break;

                    case PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG1_OFFSET_E:
                        U32_SET_FIELD_MAC(regValue,0,4,tuneValuesPtr->txEmphAmp);
                        U32_SET_FIELD_MAC(regValue,8,5,tuneValuesPtr->txAmp);
                        U32_SET_FIELD_MAC(regValue,13,1,tuneValuesPtr->txAmpAdj);
                    break;

                    case PRV_CPSS_DXCH_PORT_SERDES_DFE_F0F1_COEFFICIENT_CTRL_OFFSET_E:
                        U32_SET_FIELD_MAC(regValue,0,5,tuneValuesPtr->dfe);
                    break;

                    case PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG7_OFFSET_E:
                        U32_SET_FIELD_MAC(regValue,8,7,tuneValuesPtr->sampler);
                    break;

                    case PRV_CPSS_DXCH_PORT_SERDES_RECEIVER_REG1_OFFSET_E:
                        /* configure squelch in receiverReg0 before RECEIVER_REG1 configuration */
                        regAddr2 = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                        serdesConfig[i].receiverReg0;
                        if((rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr2,
                                                                        0, 4, &regValue2)) != GT_OK)
                            return rc;
                        if((rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr2,
                                                                        0, 4, tuneValuesPtr->sqlch)) != GT_OK)
                            return rc;
                    break;

                    default:
                        break;
                }
            }
        }
        if(PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG0_OFFSET_E !=
            serdesPowerUpSequencePtr[cfgStep][CPSS_DXCH_PORT_SERDES_SPEED_NA_E])
        {
            rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regValue);
        }
        else
        {
            /* don't touch bit 0 of transmit reg 0 it's set by
                cpssDxChPortSerdesTxEnableSet */
            rc = prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum,portGroupId,
                                                regAddr,0xFFFFFFFE,regValue);
        }
        if (rc != GT_OK)
            return rc;

    }/* PRV_PER_SERDES_LOOP_MAC*/

    return GT_OK;
}

/*******************************************************************************
* serdesPowerUpConfigure
*
* DESCRIPTION:
*       Configure serdes power up sequence
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion2.
*
* INPUTS:
*       devNum - physical device number
*       portNum - physical port number
*       startSerdes - first serdes to configure
*       serdesesNum - number of serdeses to configure
*       serdesSpeed - required serdes frequency
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - the speed is not supported on the port
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong speed
*
* COMMENTS:
*       None.
*
*******************************************************************************/
/*
extern GT_STATUS appDemoTraceHwAccessOutputModeSet
(
    IN GT_U32   mode
);

extern GT_STATUS appDemoTraceHwAccessDbDump
(
    IN GT_U32 accessType
);
*/
static GT_STATUS serdesPowerUpConfigure
(
    IN  GT_U8                        devNum,
    IN  CPSS_PORTS_BMP_STC           portsBmp,
    IN  CPSS_PORT_INTERFACE_MODE_ENT ifMode,
    IN  CPSS_PORT_SPEED_ENT          speed
)
{
    GT_STATUS   rc;         /* return status    */
    GT_U32      portGroupId;     /*the port group Id - support multi-port-groups device */
    GT_U32      startSerdes, /* first serdes to configure */
                serdesesNum; /* number of serdes used by port */
    CPSS_DXCH_PORT_SERDES_SPEED_ENT serdesSpeed;    /* required serdes frequency */
    PRV_CPSS_DXCH_PORT_SERDES_POWER_UP_ARRAY    *serdesPowerUpSequencePtr;
        /* ptr to array of serdes registers power up sequence configuration */
    GT_U32                  configStep; /* reached step in power up sequence */
    GT_U32                  delay;      /* speep time after configuration */
    GT_PHYSICAL_PORT_NUM    portNum;    /* physical port number */
    GT_U32                  regAddr;    /* address of register */
    GT_U32                  i;          /* iterator */

    for(portNum = 0; portNum < PRV_CPSS_PP_MAC(devNum)->numOfPorts; portNum++)
    {
        if(GT_FALSE == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&portsBmp, portNum))
        {
            continue;
        }

        rc = prvCpssDxChPortIfModeCheckAndSerdesNumGet(devNum, portNum, ifMode,
                                                        &startSerdes, &serdesesNum);
        if(rc != GT_OK)
        {
            return rc;
        }

        portGroupId = 
            PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, 
                                                                       portNum);
        PRV_PER_SERDES_LOOP_MAC
        {/* deassert sd_reset_in */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].
                                                            serdesExternalReg2;
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, 
                                                    3, 1, 1);
            if(rc !=GT_OK)
            {
                return rc;
            }
        }
    }

    rc = prvCpssDxChPortTraceDelay(devNum, 0, 10);
    if(rc != GT_OK)
    {
        return rc;
    }
/*
    rc = appDemoTraceHwAccessOutputModeSet(2);
    if(rc != GT_OK)
    {
        cpssOsPrintf("appDemoTraceHwAccessOutputModeSet failed\n");
        return rc;
    }

    rc = cpssDrvPpHwTraceEnable(devNum,CPSS_DRV_HW_TRACE_TYPE_WRITE_DELAY_E,GT_TRUE);
    if(rc != GT_OK)
    {
        cpssOsPrintf("cpssDrvPpHwTraceEnable failed\n");
        return rc;
    }
*/
    serdesPowerUpSequencePtr = lionSerdesPowerUpSequence;
    serdesSpeed = serdesFrequency[ifMode][speed];
    for(configStep = 0; serdesPowerUpSequencePtr[configStep]
            [CPSS_DXCH_PORT_SERDES_SPEED_NA_E] != PRV_CPSS_SW_PTR_ENTRY_UNUSED; 
         configStep++)
    {
        for(portNum = 0; portNum < PRV_CPSS_PP_MAC(devNum)->numOfPorts; portNum++)
        {
            if(GT_FALSE == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&portsBmp, portNum))
            {
                continue;
            }
    
            rc = prvCpssDxChPortIfModeCheckAndSerdesNumGet(devNum, portNum, ifMode,
                                                            &startSerdes, &serdesesNum);
            if(rc != GT_OK)
            {
                return rc;
            }
    
            portGroupId = 
                PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, 
                                                                           portNum);
            rc = serdesFastConfig(devNum, portGroupId, startSerdes, serdesesNum,
                                  serdesSpeed, serdesPowerUpSequencePtr, configStep);
            if(rc != GT_OK)
            {
                return rc;
            }
        }/* for(portNum = 0; */
    
        if((delay = serdesPowerUpSequencePtr[configStep]
                                    [CPSS_DXCH_PORT_SERDES_SPEED_NA_E+1]) > 0)
        {
            rc = prvCpssDxChPortTraceDelay(devNum, 0, delay);
            if(rc != GT_OK)
            {
                return rc;
            }
        }    
    }

/*
    rc = appDemoTraceHwAccessDbDump(1);
    if(rc != GT_OK)
    {
        cpssOsPrintf("appDemoTraceHwAccessDbDump failed\n");
        return rc;
    }
*/
    return GT_OK;
}

/*******************************************************************************
* validityCheck
*
* DESCRIPTION:
*       Check if required configuration alowed for port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - physical device number
*       portsBmp  - physical port number (not CPU port)
*       ifMode    - interface mode
*       speed     - port data speed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Same like cpssDxChPortModeSpeedSet(powerUp=true) but faster.
*       Purposed for use on system init, so assumes that ports are in HW default
*       i.e. all units in reset and serdes in power down.
* 
*******************************************************************************/
static GT_STATUS validityCheck
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORTS_BMP_STC              portsBmp,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed
)
{
    GT_STATUS               rc;             /* return code     */
    GT_U32                  portGroupId;    /*the port group Id - support multi-port-groups device */
    GT_PHYSICAL_PORT_NUM    localPort;      /* number of port in port group */
    GT_U32                  startSerdes;    /* number of first serdes used by port */
    GT_U32                  numOfSerdesLanes; /* number of serdes lanes occupied by port */
    GT_PHYSICAL_PORT_NUM    globalPort;     /* calculated port number in device */
    GT_PHYSICAL_PORT_NUM    portNum;        /* physical port number */
    CPSS_PORT_INTERFACE_MODE_ENT    tmpIfMode;/* interface mode */
    CPSS_DXCH_PORT_SERDES_SPEED_ENT serdesSpeed, /* required serdes frequency */
                                    serdesSpeedPrev; /* current serdes frequency */
    GT_BOOL                 isSupported;

    if(NULL == PRV_CPSS_DXCH_DEV_OBJ_MAC(devNum)->portPtr->
                                                    setPortInterfaceMode[ifMode])
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    if(NULL == PRV_CPSS_DXCH_PP_MAC(devNum)->devObj.portPtr->setSerdesPowerStatus)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->port.serdesRefClock !=
                    CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_156_25_DIFF_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    if((serdesSpeed = serdesFrequency[ifMode][speed]) == 
                                                CPSS_DXCH_PORT_SERDES_SPEED_NA_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }
    
    for(portNum = 0; portNum < PRV_CPSS_PP_MAC(devNum)->numOfPorts; portNum++)
    {
        if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(&portsBmp, portNum))
        {
            PRV_CPSS_PHY_PORT_CHECK_MAC(devNum,portNum);
        }
        else
        {
            continue;
        }

        /* check if interface mode supported by given port */
        rc = prvCpssDxChPortSupportedModeCheck(devNum,portNum,ifMode,/*OUT*/&isSupported);
        if (GT_OK != rc)
        {
            return rc;
        }
        if (isSupported == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }

        portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,portNum);

        if((rc = prvCpssDxChPortIfModeCheckAndSerdesNumGet(devNum, portNum, 
                                                           ifMode, &startSerdes, 
                                                           &numOfSerdesLanes)) 
           != GT_OK)
        {
            return rc;
        }

        /* can't define XLG mode if one of ports which serdeses will be occupied
           by XLG port is special, except for configured port itself */
        if(CPSS_PORT_INTERFACE_MODE_XLG_E == ifMode)
        {
            for(localPort = 8; localPort < 12; localPort++)
            {
                if(10 == localPort)
                {
                    continue;
                }

                globalPort = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum, 
                                                                            portGroupId, 
                                                                            localPort);
                tmpIfMode = PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, globalPort);
                if(CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E == tmpIfMode)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }
            }
        }
        else if((CPSS_PORT_INTERFACE_MODE_XGMII_E == ifMode) && (numOfSerdesLanes == 2))
        {/* if neighbor port defined as special port need check if serdes frequency
            defined on his serdeses equal to required for even port.
            number of serdeses less then needed for this mode i.e. neighbor is 
            special port */
            rc = prvGetLpSerdesSpeed(devNum, portGroupId, startSerdes+2,
                                     &serdesSpeedPrev);
            if (rc != GT_OK)
            {
                return rc;
            }
            if(serdesSpeedPrev != serdesSpeed)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
        else if(CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E == ifMode)
        {
            localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum);
            if((localPort == 8) || (localPort == 9) || (localPort == 11))
            {/* can't define special mode on ports which serdeses occupied by XLG port */
                globalPort = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum, 
                                                                    portGroupId, 10);
                tmpIfMode = PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, globalPort);
                if(CPSS_PORT_INTERFACE_MODE_XLG_E == tmpIfMode)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }
            }

            if(localPort%2 != 0)
            {/* if neighbor even port occupies more then 2 serdes then special mode 
                could be defined only if neighbor is XGMII */
                tmpIfMode = PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portNum-1);
                if((rc = prvCpssDxChPortIfModeSerdesNumGet(devNum, portNum-1,
                            tmpIfMode, &startSerdes, &numOfSerdesLanes)) != GT_OK)
                {
                    return rc;
                }
                if((numOfSerdesLanes > 2) && (tmpIfMode != 
                                              CPSS_PORT_INTERFACE_MODE_XGMII_E))
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }

                if(CPSS_PORT_INTERFACE_MODE_XGMII_E == tmpIfMode)
                {/* if special mode required and port is odd, check that even port 
                    configuration compatible to required */
                    rc = prvGetLpSerdesSpeed(devNum, portGroupId, startSerdes,
                                             &serdesSpeedPrev);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }
                    if(serdesSpeedPrev != serdesSpeed)
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }

            }/* if(localPort%2 != 0) */

        }/* else if(CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E */

    }/* for(portNum = 0; */

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortsFastInit
*
* DESCRIPTION:
*       Fast ports configuration (interface, speed, serdes power up) during
*       system initialization. Function configures all ports in bitmap with same
*       parameters.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - physical device number
*       portsBmp  - bitmap of physical ports (not CPU port) to configure
*       ifMode    - interface mode
*       speed     - port data speed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Same like cpssDxChPortModeSpeedSet(powerUp=true) but faster.
*       Purposed for use on system init, so assumes that ports are in HW default
*       i.e. all port's units in reset and serdes in power down.
* 
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortsFastInit
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORTS_BMP_STC              portsBmp,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed
)
{
    GT_STATUS   rc; /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                          | CPSS_CH2_E | CPSS_CH3_E);

    if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_IMPLEMENTED, LOG_ERROR_NO_MSG);
    }

    if((ifMode >= CPSS_PORT_INTERFACE_MODE_NA_E) ||
            (ifMode < CPSS_PORT_INTERFACE_MODE_REDUCED_10BIT_E))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if((speed < CPSS_PORT_SPEED_10_E) || (speed >= CPSS_PORT_SPEED_NA_E))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    
    rc = validityCheck(devNum, portsBmp, ifMode, speed);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = fastPortInitPhase1(devNum, portsBmp, ifMode, speed);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = fastPortInitPhase2(devNum, portsBmp, ifMode, speed);
    if(rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortsFastInit
*
* DESCRIPTION:
*       Fast ports configuration (interface, speed, serdes power up) during
*       system initialization. Function configures all ports in bitmap with same
*       parameters.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - physical device number
*       portsBmp  - bitmap of physical ports (not CPU port) to configure
*       ifMode    - interface mode
*       speed     - port data speed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Same like cpssDxChPortModeSpeedSet(powerUp=true) but faster.
*       Purposed for use on system init, so assumes that ports are in HW default
*       i.e. all port's units in reset and serdes in power down.
* 
*******************************************************************************/
GT_STATUS cpssDxChPortsFastInit
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORTS_BMP_STC              portsBmp,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortsFastInit);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portsBmp, ifMode, speed));

    rc = internal_cpssDxChPortsFastInit(devNum, portsBmp, ifMode, speed);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portsBmp, ifMode, speed));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


