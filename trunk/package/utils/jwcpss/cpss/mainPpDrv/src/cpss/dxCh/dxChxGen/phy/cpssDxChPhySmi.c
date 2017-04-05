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
* cpssDxChPhySmi.c
*
* DESCRIPTION:
*       API implementation for port Core Serial Management Interface facility.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 64 $
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/phy/cpssDxChPhySmi.h>
#include <cpss/generic/phy/private/prvCpssGenPhySmi.h>
#include <cpss/dxCh/dxChxGen/phy/private/prvCpssDxChPhySmi.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>
#include <cpss/dxCh/dxChxGen/phy/prvCpssDxChSmiUnitDrv.h>
#include <cpss/dxCh/dxChxGen/phy/private/prvCpssDxChPhyLog.h>



/***************Private functions ********************************************/
/*
 *  Genaral architechute :
 *
 *   cpssDxChPhySmi          -  logical level Driver, it controls 4 SMI interfaces (High Level) on logical level.
 *         |                       interface : port level / mac (0:47)
 *         |                       cpssDxChPhyPortSmiRegisterRead(port)
 *         |                       cpssDxChPhyPortAddrSet(dev,port,phyAddr);
 *         |                       cpssDxChPhyPortSmiInterfaceSet(dev, port, smiIf);
 *         |                       cpssDxChPhyAutoPollNumOfPortsSet(dev, SMI0,SMI1,SMI2,SMI3)
 *         |
 *   prvCpssDxChSmiUnitDrv   -  Low level Driver ( it controls spicific SMI) and translate either to LMS or SMI unit
 *       |                      interface : SMI instance (0:3) , local (SMI) port. Convert Logical values to HW values
 *       |                   - prvCpssDxChSMIDrvInit()
 *       |                   - prvCpssDxChSMIInvertMDCSet()
 *       |
 *       |
 *       +--------- LMS (unit)------------------------------------------------SMI (unit) - SMI(0-3)
 *                   |   |                                                     |
 *                   |   +- LMS0 - SMI 0/1                                    SMI 0 -- 0-15
 *                   |   |    +LNS0Group -- SMI 0                             SMI 1 -- 0-15
 *                   |   |    +LNS1Group -- SMI 1                             SMI 2 -- 0-15
 *                   |   |                                                    SMI 3 -- 0 15
 *                   |   +--LMS1 - SMI 2/3                                        |
 *                   |        +LNS0Group -- SMI 2                                 +- Number of Ports-for-autoPolling
 *                   |        +LNS1Group -- SMI 3                                 +- Fast MDC division
 *                   |                                                            +- Inverd MDC
 *                   + MAC-Phy-Addr-DB (0-47)                                     +- port dev number[16]
 *                   | (SMI-IF hardwired to specific port                         +- media selector[16]
 *                   |     |
 *                   |     +--- LMS0 IP 0-23 (mac 0-23)
 *                   |     |        LM0
 *                   |     |           LMS0 Group 0 (0-5)    mac 0-5
 *                   |     |           LMS0 Group 1 (6-11)   mac 6-11
 *                   |     |        LM1
 *                   |     |           LMS0 Group 0 (12-17)
 *                   |     |           LMS0 Group 1 (18-23)
 *                   |     +--- LMS1 0-23 (mac 24-47)
 *                   |              LM0
 *                   |                 LMS0 Group 0 (0-5)    mac 24-29
 *                   |                 LMS0 Group 1 (6-11)   mac 30-35
 *                   |              LM1
 *                   |                 LMS0 Group 0 (12-17)
 *                   |                 LMS0 Group 1 (18-23)
 *                   |
 *                   + POrt Media Select (mac 0-47)
 *                         |
 *                         +--- LMS0 IP 0-23 (mac 0-23)
 *                         |        LM0
 *                         |           LMS0 Group 0 (0-5)    mac 0-5
 *                         |           LMS0 Group 1 (6-11)   mac 6-11
 *                         |        LM1
 *                         |           LMS0 Group 0 (12-17)
 *                         |           LMS0 Group 1 (18-23)
 *                         +--- LMS1 0-23 (mac 24-47)
 *                                  LM0
 *                                     LMS0 Group 0 (0-5)    mac 24-29
 *                                     LMS0 Group 1 (6-11)   mac 30-35
 *                                  LM1
 *                                     LMS0 Group 0 (12-17)
 *                                     LMS0 Group 1 (18-23)
 *
 *
 *
 */

#define CPSS_DX_SMI_RETRY_COUNTER_CNS       1000


static GT_VOID vctObjInit
(
    IN  GT_U8     devNum
);

static GT_STATUS prvCpssDxChPhyPortSmiRegisterWrite
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM  portMacNum,
    IN  GT_U8     phyReg,
    IN  GT_U16    data
);

static GT_STATUS prvCpssDxChPhySmi0NumOfPortsGet
(
  IN  GT_U8     devNum,
  OUT GT_U32    *smi0PortsNumPtr
);

extern GT_STATUS prvCpssDxChEArchPhyAutoPollNumOfPortsGet
(
    IN  GT_U8                                           devNum,
    OUT CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    *autoPollNumOfPortsSmi0Ptr,
    OUT CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    *autoPollNumOfPortsSmi1Ptr,
    OUT CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    *autoPollNumOfPortsSmi2Ptr,
    OUT CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    *autoPollNumOfPortsSmi3Ptr
);


GT_STATUS prvCpssDxChEArchPhyPortSMIGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  portMacNum,
    OUT GT_U32 *smiInstancePtr,
    OUT GT_U32 *smiLocalPortPtr
)
{
    GT_STATUS rc;
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    autoPollNumOfPortsSmi[CPSS_PHY_SMI_INTERFACE_MAX_E];
    GT_U32 smiPortBndArr[CPSS_PHY_SMI_INTERFACE_MAX_E];
    GT_U32 smiInstance;
    GT_U32 smiLocalPort;

    if (portMacNum < PRV_CPSS_DXCH_E_ARCH_SMI_PPU_PORTS_NUM_CNS)
    {
        rc = prvCpssDxChEArchPhyAutoPollNumOfPortsGet(devNum,
                                                        &autoPollNumOfPortsSmi[0],
                                                        &autoPollNumOfPortsSmi[1],
                                                        &autoPollNumOfPortsSmi[2],
                                                        &autoPollNumOfPortsSmi[3]);
        if(rc != GT_OK)
        {
            return rc;
        }

        smiPortBndArr[CPSS_PHY_SMI_INTERFACE_0_E] =                                             autoPollNumOfPortsSmi[0];
        smiPortBndArr[CPSS_PHY_SMI_INTERFACE_1_E] = smiPortBndArr[CPSS_PHY_SMI_INTERFACE_0_E] + autoPollNumOfPortsSmi[1];
        smiPortBndArr[CPSS_PHY_SMI_INTERFACE_2_E] = smiPortBndArr[CPSS_PHY_SMI_INTERFACE_1_E] + autoPollNumOfPortsSmi[2];
        smiPortBndArr[CPSS_PHY_SMI_INTERFACE_3_E] = smiPortBndArr[CPSS_PHY_SMI_INTERFACE_2_E] + autoPollNumOfPortsSmi[3];;

        for (smiInstance = CPSS_PHY_SMI_INTERFACE_0_E; smiInstance < CPSS_PHY_SMI_INTERFACE_MAX_E; smiInstance++)
        {
            if (portMacNum < smiPortBndArr[smiInstance])
            {
                if (smiInstance == 0)
                {
                    smiLocalPort = portMacNum;
                }
                else
                {
                    smiLocalPort = portMacNum - smiPortBndArr[smiInstance - 1];
                }

                *smiInstancePtr  = smiInstance;
                *smiLocalPortPtr = smiLocalPort;
                return GT_OK;
            }
        }
        *smiInstancePtr  = CPSS_PHY_SMI_INTERFACE_MAX_E;
        *smiLocalPortPtr = (GT_U32)~0;

        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    else
    {
        *smiInstancePtr  = PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacNum].smiIfInfo.smiInterface;
        *smiLocalPortPtr = ~((GT_U32)(-1));
        if (*smiInstancePtr == PRV_CPSS_PHY_INVALIDE_SMI_INSTANCE_CNS)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, "prvCpssDxChEArchPhyPortSMIGet: error in SMI_INSTANCE_NUM for port_mac = %d\n", portMacNum);
        }
        return GT_OK;
    }
}

/*******************************************************************************
* prvCpssDxChEArchPhyPortSmiInit
*
* DESCRIPTION:
*       Initialiaze the SMI control register port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChEArchPhyPortSmiInit
(
    IN GT_U8 devNum
)
{
    GT_PHYSICAL_PORT_NUM portMacNum; /* MAC number of port */
    GT_U32       localPort;          /* local port - support multi-port-groups device */
    GT_U32      portGroupId;        /* port group */
    GT_STATUS   rc;                 /* return status */
    /* array of numbers of ports par SMI interface */
    GT_U32  smiMngmtRegAddr;
    PRV_CPSS_GEN_PP_CONFIG_STC * pDev;

    pDev = PRV_CPSS_PP_MAC(devNum);
    /* loop over all ports */
    for(portMacNum = 0; portMacNum < pDev->numOfPorts; portMacNum++)
    {
        /* skip not existed ports */
        if (! PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, portMacNum))
            continue;

        /* skip XG ports and stack ports of XCAT2 */
        if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacNum].portType >= PRV_CPSS_PORT_XG_E)
        {
            continue;
        }

        /* Bind SMI controller to port only if was not already done */
        if( PRV_CPSS_PHY_SMI_NO_CTRL_ADDR_CNS == PRV_CPSS_PHY_SMI_CTRL_REG_ADDR_MAC(devNum, portMacNum) )
        {
            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum) == 0)
            {
                GT_U32 smiInstance;
                GT_U32 smiLocalPort;

                rc = prvCpssDxChEArchPhyPortSMIGet(devNum,portMacNum,/*OUT*/&smiInstance,&smiLocalPort);
                if (rc != GT_OK)
                {
                    return rc;
                }

                rc = prvCpssDxChSMIDrvManagementRegAddrGet(devNum,smiInstance,&smiMngmtRegAddr);
                if (rc != GT_OK)
                {
                    return rc;
                }
                PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacNum].smiIfInfo.smiCtrlRegAddr = smiMngmtRegAddr;
                PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacNum].smiIfInfo.smiInterface   = (CPSS_PHY_SMI_INTERFACE_ENT)smiInstance;
            }
            else
            {
                /* use SMI#0 controller to access port's PHY*/
                rc = prvCpssDxChSMIDrvManagementRegAddrGet(devNum,CPSS_PHY_SMI_INTERFACE_0_E,&smiMngmtRegAddr);
                if (rc != GT_OK)
                {
                    return rc;
                }
                PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacNum].smiIfInfo.smiCtrlRegAddr = smiMngmtRegAddr;
                PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacNum].smiIfInfo.smiInterface   = CPSS_PHY_SMI_INTERFACE_0_E;
            }
        }

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portMacNum);

        /* read all PHY Address Registers */
        if(localPort < PRV_CPSS_DXCH_E_ARCH_SMI_PPU_PORTS_NUM_CNS)
        {
            GT_U32 smiInstance;
            GT_U32 smiLocalPort;
            GT_U32 phyAddrReg;
            rc = prvCpssDxChEArchPhyPortSMIGet(devNum,portMacNum,&smiInstance,&smiLocalPort);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDxChSMIPortPhyAddGet(devNum,portGroupId,smiInstance,smiLocalPort,/*OUT*/&phyAddrReg);
            if (rc != GT_OK)
            {
                return rc;
            }
            PRV_CPSS_PHY_SMI_PORT_ADDR_MAC(devNum, portMacNum) = (GT_U16)phyAddrReg;
        }

        /* implement workarounds of FE/GE PHYs */
        if (prvCpssGenPhyErrataInitFix(devNum,
                                 portMacNum,
                                 &cpssDxChPhyPortSmiRegisterRead,
                                 &prvCpssDxChPhyPortSmiRegisterWrite ) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPhyPortSmiInit
*
* DESCRIPTION:
*       Initialiaze the SMI control register port, Check all GE ports and
*       activate the errata initialization fix.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPhyPortSmiInit
(
    IN GT_U8 devNum
)
{
    GT_PHYSICAL_PORT_NUM i;
    GT_U32      fieldOffset;        /* Phy address reg. field offset.   */
    GT_U32      regAddrOffset;      /* Register address offset.         */
    GT_U32      value = 0;          /* value to read from  the register */
    GT_U32      regAddr;            /* register address                 */
    GT_U8       numOfPorts;         /* number of ports                  */
    GT_U32      smi0portsNum;       /* number of ports which their phys are controled by SMI0  */
    GT_STATUS   rc;                 /* return code                      */
    GT_U32       localPort;          /* local port - support multi-port-groups device */
    GT_U32      portGroupId;        /* port group */
    GT_U32      smiCtrlRegUnitsStep; /* Step between units containing the SMI control register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* Initialize PHY virtual functions */
    vctObjInit(devNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        return prvCpssDxChEArchPhyPortSmiInit(devNum);
    }

    numOfPorts = PRV_CPSS_PP_MAC(devNum)->numOfPorts;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiPhyAddr;

    rc = prvCpssDxChPhySmi0NumOfPortsGet(devNum, &smi0portsNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* loop over all GE and FE ports */
    for(i = 0; i < numOfPorts; i++)
    {
        /* skip not existed ports */
        if (! PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, i))
            continue;

        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, i);
        if(localPort >= PRV_CPSS_DXCH_SMI_PPU_PORTS_NUM_CNS)
        {
            continue;
        }

        /* skip ports that has no GE PHY */
        if((PRV_CPSS_XG_PORT_XG_ONLY_E == PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,i))
           || (PRV_CPSS_XG_PORT_HX_QX_ONLY_E == PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,i)))
        {
            continue;
        }

        /* Bind SMI controller to port only if was not already done */
        if( PRV_CPSS_PHY_SMI_NO_CTRL_ADDR_CNS == PRV_CPSS_PHY_SMI_CTRL_REG_ADDR_MAC(devNum, i) )
        {
            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum) == 0)
            {
                if(i < smi0portsNum)
                {
                    /* use SMI#0 controller to access port's PHY*/
                    PRV_CPSS_PHY_SMI_CTRL_REG_ADDR_MAC(devNum,i) =
                        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiControl;
                }
                else if(i < PRV_CPSS_DXCH_SMI_PPU_PORTS_NUM_CNS)
                {
                    /* use SMI#1 controller to access port's PHY*/
                    if(CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
                    {
                        smiCtrlRegUnitsStep = 2 * CPSS_DX_SMI_MNG_CNTRL_OFFSET_CNS;
                    }
                    else
                    {
                        smiCtrlRegUnitsStep = CPSS_DX_SMI_MNG_CNTRL_OFFSET_CNS;
                    }
                    PRV_CPSS_PHY_SMI_CTRL_REG_ADDR_MAC(devNum,i) =
                        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiControl +
                                                            smiCtrlRegUnitsStep;
                }
            }
            else
            {
                /* use SMI#0 controller to access port's PHY*/
                PRV_CPSS_PHY_SMI_CTRL_REG_ADDR_MAC(devNum,i) =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiControl;
            }
        }

        /* read all PHY Address Registers */
        /* calc the reg and bit offset for the port */
        regAddrOffset = (localPort / 6) * CPSS_DX_PHY_ADDR_REG_OFFSET_CNS;
        if(PRV_CPSS_SIP_5_CHECK_MAC(devNum) ||
           (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
        {
            /* the units steps are 0x01000000 (not CPSS_DX_PHY_ADDR_REG_OFFSET_CNS)*/
            regAddrOffset *= 2;
            if(localPort >= 12 &&
               PRV_CPSS_SIP_5_CHECK_MAC(devNum))
            {
                /*currently .. support bobcat2 like lion3 */
                continue;
            }
        }
        fieldOffset   = (localPort % 6) * 5;

        if ((localPort % 6) == 0)
        {
        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, i);
                /* read all PHY Address Registers */
                if (prvCpssDrvHwPpPortGroupReadRegister(
                        devNum, portGroupId, regAddr + regAddrOffset, &value) != GT_OK)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
                }
            }

            /* init PHY Addresses Array */
            PRV_CPSS_PHY_SMI_PORT_ADDR_MAC(devNum, i) =
                (GT_U16)((value >> (fieldOffset)) & 0x1f);


        /* implement workarounds of FE/GE PHYs */
        if (prvCpssGenPhyErrataInitFix(devNum,
                                 i,
                                 &cpssDxChPhyPortSmiRegisterRead,
                                 &prvCpssDxChPhyPortSmiRegisterWrite ) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
    }

   return GT_OK;
}

/*******************************************************************************
* cpssDxChPhyPortSmiInit
*
* DESCRIPTION:
*       Initialiaze the SMI control register port, Check all GE ports and
*       activate the errata initialization fix.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPhyPortSmiInit
(
    IN GT_U8 devNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPhyPortSmiInit);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum));

    rc = internal_cpssDxChPhyPortSmiInit(devNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPhyPortSmiRegisterRead
*
* DESCRIPTION:
*       Read specified SMI Register on a specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       phyReg    - SMI register (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       dataPtr   - (pointer to) the read data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_OUT_OF_RANGE          - phyAddr bigger then 31
*       GT_NOT_SUPPORTED         - for XG ports
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - Flex port SMI or PHY address were not set
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPhyPortSmiRegisterRead
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_U8     phyReg,
    OUT GT_U16    *dataPtr
)
{
    GT_U32 value = 0;           /* value to write into the register */
    GT_U32 regAddr;             /* register address                 */
    GT_U16 portAddr;            /* port address                     */
    GT_U32 copm;
    volatile GT_U32 retryCnt;   /* retry counter for read success   */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    CPSS_SYSTEM_RECOVERY_INFO_STC   tempSystemRecoveryInfo;
    GT_U32 portMacNum;          /* MAC number */


    /* save current recovery state */
    tempSystemRecoveryInfo.systemRecoveryState = systemRecoveryInfo.systemRecoveryState;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(dataPtr);

    if(phyReg >= BIT_5)
    {
        /* only 5 bits for each of those parameters */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    /* skip ports that has no GE PHY */
    if((PRV_CPSS_XG_PORT_XG_ONLY_E == PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,portMacNum))
       || (PRV_CPSS_XG_PORT_HX_QX_ONLY_E == PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,portMacNum)))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }


    /* if port SMI or PHY address were not set access to phy registers through SMI */
    /* is prohibited. */
    if (PRV_CPSS_PHY_SMI_PORT_ADDR_MAC(devNum,portMacNum) ==  PRV_CPSS_PHY_SMI_NO_PORT_ADDR_CNS ||
        PRV_CPSS_PHY_SMI_CTRL_REG_ADDR_MAC(devNum,portMacNum) == PRV_CPSS_PHY_SMI_NO_CTRL_ADDR_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHY_SMI_GROUP_PORT_MAC(devNum,portMacNum);

    portAddr = PRV_CPSS_PHY_SMI_PORT_ADDR_MAC(devNum,portMacNum);
    value = ((portAddr & 0x1F) << 16) | ((phyReg & 0x1F) << 21) | (1 << 26);

    regAddr = PRV_CPSS_PHY_SMI_CTRL_REG_ADDR_MAC(devNum,portMacNum);

    /* that is exclusive case when prvCpssDrvHwPpWriteRegister should be done
       even during recovery process */
    tempSystemRecoveryInfo.systemRecoveryState = systemRecoveryInfo.systemRecoveryState;
    systemRecoveryInfo.systemRecoveryState = CPSS_SYSTEM_RECOVERY_COMPLETION_STATE_E;
    if (prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId, regAddr, value) != GT_OK)
    {
        systemRecoveryInfo.systemRecoveryState = tempSystemRecoveryInfo.systemRecoveryState;
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }
    systemRecoveryInfo.systemRecoveryState = tempSystemRecoveryInfo.systemRecoveryState;

    /* check if read operation has finished */
    retryCnt = CPSS_DX_SMI_RETRY_COUNTER_CNS;
    do
    {
        if (prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,regAddr, &value) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

        copm = (value >> 27) & 0x1;
        retryCnt--;
    }while (!copm && retryCnt != 0);

    if (0 == retryCnt)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);

    *dataPtr = (GT_U16) (value & 0xFFFF);
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPhyPortSmiRegisterRead
*
* DESCRIPTION:
*       Read specified SMI Register on a specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       phyReg    - SMI register (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       dataPtr   - (pointer to) the read data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_OUT_OF_RANGE          - phyAddr bigger then 31
*       GT_NOT_SUPPORTED         - for XG ports
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - Flex port SMI or PHY address were not set
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPhyPortSmiRegisterRead
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_U8     phyReg,
    OUT GT_U16    *dataPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPhyPortSmiRegisterRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, phyReg, dataPtr));

    rc = internal_cpssDxChPhyPortSmiRegisterRead(devNum, portNum, phyReg, dataPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, phyReg, dataPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
/*******************************************************************************
* internal_cpssDxChPhyPortSmiRegisterWrite
*
* DESCRIPTION:
*       Write value to specified SMI Register on a specified port on
*       specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - physical port number.
*       phyReg      - The new phy address, (APPLICABLE RANGES: 0...31).
*       data        - Data to write.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_SUPPORTED         - for XG ports
*       GT_OUT_OF_RANGE          - phyAddr bigger then 31
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - Flex port SMI or PHY address were not set
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPhyPortSmiRegisterWrite
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_U8     phyReg,
    IN  GT_U16    data
)
{
    GT_STATUS rc;                /* return code */
    GT_U32 portMacNum;          /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    if(phyReg >= BIT_5)
    {
        /* only 5 bits for each of those parameters */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    /* skip ports that has no GE PHY */
    if((PRV_CPSS_XG_PORT_XG_ONLY_E == PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,portMacNum))
       || (PRV_CPSS_XG_PORT_HX_QX_ONLY_E == PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,portMacNum)))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    /* if port SMIor PHY address were not set access to phy registers through SMI */
    /* is prohibited. */
    if (PRV_CPSS_PHY_SMI_PORT_ADDR_MAC(devNum,portMacNum) ==  PRV_CPSS_PHY_SMI_NO_PORT_ADDR_CNS ||
        PRV_CPSS_PHY_SMI_CTRL_REG_ADDR_MAC(devNum,portMacNum) == PRV_CPSS_PHY_SMI_NO_CTRL_ADDR_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    /* check for PHY WA erratum */

    /* Important: To generic function call with MAC port number */

    rc = prvCpssPhyRegWriteErrataFix(devNum, portMacNum, phyReg, data,
                                     &prvCpssDxChPhyPortSmiRegisterWrite);
    if(rc != GT_OK)
        return rc;

    rc = prvCpssDxChPhyPortSmiRegisterWrite(devNum, portMacNum, phyReg, data);

    return rc;

}

/*******************************************************************************
* cpssDxChPhyPortSmiRegisterWrite
*
* DESCRIPTION:
*       Write value to specified SMI Register on a specified port on
*       specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - physical port number.
*       phyReg      - The new phy address, (APPLICABLE RANGES: 0...31).
*       data        - Data to write.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_SUPPORTED         - for XG ports
*       GT_OUT_OF_RANGE          - phyAddr bigger then 31
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - Flex port SMI or PHY address were not set
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPhyPortSmiRegisterWrite
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_U8     phyReg,
    IN  GT_U16    data
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPhyPortSmiRegisterWrite);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, phyReg, data));

    rc = internal_cpssDxChPhyPortSmiRegisterWrite(devNum, portNum, phyReg, data);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, phyReg, data));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
/*******************************************************************************
* prvCpssDxChPhyPortSmiRegisterWrite
*
* DESCRIPTION:
*       Write value to specified SMI Register on a specified port on
*       specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       portMacNum  - MAC port number.
*       phyReg      - The new phy address, (APPLICABLE RANGES: 0...31).
*       data        - Data to write.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong devNum or portNum.
*       GT_NOT_SUPPORTED - for XG ports
*       GT_OUT_OF_RANGE  - phyAddr bigger then 31
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPhyPortSmiRegisterWrite
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM portMacNum,
    IN  GT_U8     phyReg,
    IN  GT_U16    data
)
{
    GT_U32 value = 0;           /* value to write into the register */
    GT_U32 regAddr;             /* register address                 */
    GT_U16 portAddr;            /* port address                     */
    GT_U32 busy;
    volatile GT_U32 retryCnt;   /* retry counter for busy SMI reg   */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portMacNum);

    if(phyReg >= BIT_5)
    {
        /* only 5 bits for each of those parameters */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    /* skip ports that has no GE PHY */
    if((PRV_CPSS_XG_PORT_XG_ONLY_E == PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,portMacNum))
       || (PRV_CPSS_XG_PORT_HX_QX_ONLY_E == PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,portMacNum)))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHY_SMI_GROUP_PORT_MAC(devNum,portMacNum);

    portAddr = PRV_CPSS_PHY_SMI_PORT_ADDR_MAC(devNum,portMacNum);
    value = data | ((portAddr & 0x1F) << 16) | ((phyReg & 0x1F) << 21);
    regAddr = PRV_CPSS_PHY_SMI_CTRL_REG_ADDR_MAC(devNum,portMacNum);

    if (prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId, regAddr, value) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }
    /* check if smi register is not busy */
    retryCnt = CPSS_DX_SMI_RETRY_COUNTER_CNS;
    do
    {
        if (prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId, regAddr, 28, 1, &busy) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

        retryCnt--;
    }while (busy && retryCnt != 0);

    if (0 == retryCnt)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPhyPort10GSmiRegisterRead
*
* DESCRIPTION:
*       Read specified SMI Register and PHY device of specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       phyId   - ID of external PHY (APPLICABLE RANGES: 0..31).
*       useExternalPhy - boolean variable, defines if to use external PHY
*                       NOTE: for DxCh3 and above only GT_TRUE supported !
*       phyReg  - SMI register, the register of PHY to read from
*       phyDev  - the PHY device to read from (APPLICABLE RANGES: 0..31).
*
* OUTPUTS:
*       dataPtr - (Pointer to) the read data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_OUT_OF_RANGE          - phyAddr bigger then 31
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If useExternalPhy flag is GT_FALSE, the phyId is being ignored
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPhyPort10GSmiRegisterRead
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    IN  GT_U8                               phyId,
    IN  GT_BOOL                             useExternalPhy,
    IN  GT_U16                              phyReg,
    IN  GT_U8                               phyDev,
    OUT GT_U16                              *dataPtr
)
{
    GT_U32 value = 0;           /* value to write into the register */
    GT_U32 regAddr;             /* register address                 */
    GT_U32 copm;                /* variable to compare              */
    GT_U32 busy;                /* XSMI Busy bit                    */
    volatile GT_U32 retryCnt;   /* retry counter for read success   */
    GT_U32 phyConfigRegVal = 0;
    GT_U32 phyConfigRegAddr = 0;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    CPSS_PHY_XSMI_INTERFACE_ENT xsmiInterface;  /* XSMI interface */
    GT_U32 interfaceNum;        /* XSMI interface number */
    GT_U32 portMacNum;          /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(dataPtr);

    if(GT_FALSE == useExternalPhy)
    {
        PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
             CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);
    }

    if(phyDev >= BIT_5 || phyId >= BIT_5)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);

    xsmiInterface = PRV_CPSS_PHY_XSMI_PORT_INTERFACE_MAC(devNum, portMacNum);
    interfaceNum = (xsmiInterface == CPSS_PHY_XSMI_INTERFACE_0_E) ? 0 : 1;

    /* check if XSMI is not busy */
    retryCnt = CPSS_DX_SMI_RETRY_COUNTER_CNS;
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr =
            PRV_DXCH_REG1_UNIT_MG_MAC(devNum).XSMI[interfaceNum].XSMIManagement;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smi10GeControl;
    }
    do
    {
        if (prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId, regAddr, 30, 1, &busy) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
        retryCnt--;
    }while (busy && retryCnt != 0);

    if (0 == retryCnt)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);

    /* Switching between internal and external PHY*/
    if(GT_FALSE == useExternalPhy)
    {
        /* write to XAUI configuration register1. */
        /* this register must be initialized before writing to the PHY */
        phyConfigRegAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smi10GePhyConfig1[portNum];

        /* store the register content */
        if (prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId, phyConfigRegAddr,
                                      0, 6, &phyConfigRegVal) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

        value = phyDev & 0x1F ;
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, phyConfigRegAddr,
                                      0, 6, value) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
    }


    /* write the address to be used in the read operation */
    value = phyReg;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr =
            PRV_DXCH_REG1_UNIT_MG_MAC(devNum).XSMI[interfaceNum].XSMIAddress;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smi10GeAddr;
    }

    if (prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId, regAddr, 0, 16, value) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    /* write address and read opCode to Control-Register */
    /* set port address */
    if(PRV_CPSS_PP_MAC(devNum)->numOfPorts == 1)
    {
        /* One Port devices */
        value |= (((useExternalPhy) ?
                   phyId : PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS) << 16);
    }
    else
    {
        /* Use external PHY if useExternalPhy == TRUE */
        value |= (((useExternalPhy) ?
                    phyId :
                    PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum))
                        << 16);
    }

    /* set device address */
    value |= (phyDev << 21);

    /* set read opcode */
    value |= (((GT_U32)7) << 26);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr =
            PRV_DXCH_REG1_UNIT_MG_MAC(devNum).XSMI[interfaceNum].XSMIManagement;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smi10GeControl;
    }
    if (prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId, regAddr, value) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    /* check if read operation has finished */
    retryCnt = CPSS_DX_SMI_RETRY_COUNTER_CNS;
    do
    {
        if (prvCpssDrvHwPpPortGroupReadRegister(devNum,portGroupId, regAddr, &value) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
        copm = (value >> 29) & 0x1;
        retryCnt--;
    }while (!copm && retryCnt != 0);

    if (0 == retryCnt)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);

    *dataPtr = (GT_U16) (value & 0xFFFF);

    /* write the original value of XAUI configuration register1. */
    if( GT_FALSE == useExternalPhy)
    {
       if (prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId, phyConfigRegAddr,
                                     0, 6, phyConfigRegVal) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPhyPort10GSmiRegisterRead
*
* DESCRIPTION:
*       Read specified SMI Register and PHY device of specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       phyId   - ID of external PHY (APPLICABLE RANGES: 0..31).
*       useExternalPhy - boolean variable, defines if to use external PHY
*                       NOTE: for DxCh3 and above only GT_TRUE supported !
*       phyReg  - SMI register, the register of PHY to read from
*       phyDev  - the PHY device to read from (APPLICABLE RANGES: 0..31).
*
* OUTPUTS:
*       dataPtr - (Pointer to) the read data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_OUT_OF_RANGE          - phyAddr bigger then 31
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If useExternalPhy flag is GT_FALSE, the phyId is being ignored
*
*******************************************************************************/
GT_STATUS cpssDxChPhyPort10GSmiRegisterRead
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    IN  GT_U8                               phyId,
    IN  GT_BOOL                             useExternalPhy,
    IN  GT_U16                              phyReg,
    IN  GT_U8                               phyDev,
    OUT GT_U16                              *dataPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPhyPort10GSmiRegisterRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr));

    rc = internal_cpssDxChPhyPort10GSmiRegisterRead(devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPhyPort10GSmiRegisterWrite
*
* DESCRIPTION:
*       Write value to a specified SMI Register and PHY device of
*       specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - physical port number.
*       phyId       - ID of external PHY (APPLICABLE RANGES: 0...31).
*       useExternalPhy - Boolean variable, defines if to use external PHY
*                       NOTE: for DxCh3 and above only GT_TRUE supported !
*       phyReg      - SMI register, the register of PHY to read from
*       phyDev      - the PHY device to read from (APPLICABLE RANGES: 0..31).
*       data        - Data to write.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_OUT_OF_RANGE          - phyAddr bigger then 31
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If useExternalPhy flag is GT_FALSE, the phyId is being ignored
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPhyPort10GSmiRegisterWrite
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    IN  GT_U8                               phyId,
    IN  GT_BOOL                             useExternalPhy,
    IN  GT_U16                              phyReg,
    IN  GT_U8                               phyDev,
    IN  GT_U16                              data
)
{
    GT_U32 value = 0;           /* value to write into the register */
    GT_U32 regAddr;             /* register address                 */
    GT_U32 busy;
    volatile GT_U32 retryCnt;   /* retry counter for busy SMI reg   */
    GT_U32 phyConfigRegVal = 0;
    GT_U32 phyConfigRegAddr = 0;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    CPSS_PHY_XSMI_INTERFACE_ENT xsmiInterface;  /* XSMI interface */
    GT_U32 interfaceNum;        /* XSMI interface number */
    GT_U32 portMacNum;          /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    if(GT_FALSE == useExternalPhy)
    {
        PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
             CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);
    }

    if(phyDev >= BIT_5 || phyId >= BIT_5)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    xsmiInterface = PRV_CPSS_PHY_XSMI_PORT_INTERFACE_MAC(devNum, portNum);
    interfaceNum = (xsmiInterface == CPSS_PHY_XSMI_INTERFACE_0_E) ? 0 : 1;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr =
            PRV_DXCH_REG1_UNIT_MG_MAC(devNum).XSMI[interfaceNum].XSMIManagement;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smi10GeControl;
    }

    /* check if smi register is not busy */
    retryCnt = CPSS_DX_SMI_RETRY_COUNTER_CNS;
    do
    {
        if (prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId, regAddr, 30, 1, &busy) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
        retryCnt--;
    }while (busy && retryCnt != 0);

    if (0 == retryCnt)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);

    if(GT_FALSE == useExternalPhy)
    {
        /* write to XAUI configuration register1. */
        /* this register must be initialized before writing to the PHY */
        phyConfigRegAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smi10GePhyConfig1[portNum];

        /* store the register content */
        if (prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId, phyConfigRegAddr,
                                      0, 6, &phyConfigRegVal) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

        value = phyDev & 0x1F ;
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId, phyConfigRegAddr, 0, 6, value) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
    }

    /* write address */
    value = phyReg;
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_MG_MAC(devNum).XSMI[interfaceNum].XSMIAddress;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smi10GeAddr;
    }

    if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, 0, 16, value) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    /* set data */
    value = data;

    /* set port address */
    if(PRV_CPSS_PP_MAC(devNum)->numOfPorts == 1)
    {
        /* One Port devices */
        value |= (((useExternalPhy) ?
                   phyId : PRV_CPSS_PHY_XENPAK_PORT_ADDRESS_CNS) << 16);
    }
    else
        value |= (((useExternalPhy) ?
                    phyId :
                    PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum))
                         << 16);


    /* set device address */
    value |= (phyDev << 21);

    /* set write opcode */
    value |= (5 << 26);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr =
            PRV_DXCH_REG1_UNIT_MG_MAC(devNum).XSMI[interfaceNum].XSMIManagement;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smi10GeControl;
    }

    /* Write the value */
    if (prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId, regAddr, value) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    /* write the original value of XAUI configuration register1. */
    if( GT_FALSE == useExternalPhy)
    {
       if (prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId, phyConfigRegAddr,
                                     0, 6, phyConfigRegVal) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPhyPort10GSmiRegisterWrite
*
* DESCRIPTION:
*       Write value to a specified SMI Register and PHY device of
*       specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - physical port number.
*       phyId       - ID of external PHY (APPLICABLE RANGES: 0...31).
*       useExternalPhy - Boolean variable, defines if to use external PHY
*                       NOTE: for DxCh3 and above only GT_TRUE supported !
*       phyReg      - SMI register, the register of PHY to read from
*       phyDev      - the PHY device to read from (APPLICABLE RANGES: 0..31).
*       data        - Data to write.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_OUT_OF_RANGE          - phyAddr bigger then 31
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If useExternalPhy flag is GT_FALSE, the phyId is being ignored
*
*******************************************************************************/
GT_STATUS cpssDxChPhyPort10GSmiRegisterWrite
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    IN  GT_U8                               phyId,
    IN  GT_BOOL                             useExternalPhy,
    IN  GT_U16                              phyReg,
    IN  GT_U8                               phyDev,
    IN  GT_U16                              data
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPhyPort10GSmiRegisterWrite);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, data));

    rc = internal_cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, data);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, data));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPhyAutoNegotiationSmiRegAddr
*
* DESCRIPTION:
*       This function calculates auto negotiation configuration SMI register.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - port number
*
* OUTPUTS:
*       smiIdxPtr   - pointer to SMI index
*       regAddrPtr  - pointer to SMI register address
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPhyAutoNegotiationSmiRegAddr
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_U32                 *smiIdxPtr,
    OUT GT_U32                 *regAddrPtr
)
{
    GT_U32       localPort;/* local port - support multi-port-groups device */
    GT_U32      smi0portsNum;
                        /* number of ports which their phys are controled by SMI0  */
    GT_STATUS   rc;                 /* return code                  */
    GT_U32 portMacNum;          /* MAC number */

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    rc = prvCpssDxChPhySmi0NumOfPortsGet(devNum, &smi0portsNum);
    if (rc != GT_OK)
    {
        return rc;
    }
    *smiIdxPtr = 0;
    /* for multi-port-group device : since this config is not really per port ,
        we will read the info from first active port group , and will set it to all port groups */
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    if (localPort < smi0portsNum)
    {
        *regAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.lms0PhyAutoNeg[0];
    }
    else if (localPort < PRV_CPSS_DXCH_SMI_PPU_PORTS_NUM_CNS)
    {
        *regAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.lms1PhyAutoNeg[0];
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

CPSS_TBD_BOOKMARK_XCAT3
/* should be used for the new API (enable/disable Master SMI Polling) */
#if 0
/*******************************************************************************
* prvCpssDxChEArchPhySmiMiscRegAddr
*
* DESCRIPTION:
*       This function gets SMI miscellaneous register
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2;
*
* INPUTS:
*       devNum          - physical device number.
*       portMacNum      - port MAC number
*
* OUTPUTS:
*       regAddrPtr      - pointer to register address
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChEArchPhySmiMiscRegAddr
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portMacNum,
    OUT GT_U32               *regAddrPtr

)
{
    GT_STATUS   rc;     /* return status */
    GT_U32  smiInstance;
    GT_U32  smiLocalPort;

    rc = prvCpssDxChEArchPhyPortSMIGet(devNum,portMacNum,/*OUT*/&smiInstance,&smiLocalPort);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChSMIDrvMiscConfigRegAddrGet(devNum,smiInstance,/*PUT*/regAddrPtr);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}



/*******************************************************************************
* prvCpssDxChPhyMiscSmiRegAddr
*
* DESCRIPTION:
*       This function calculates miscellaneous configuration SMI register.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
*
* NOT APPLICABLE DEVICES:
*        None
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - port number
*
* OUTPUTS:
*       regAddrPtr  - pointer to SMI register address
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPhyMiscSmiRegAddr
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_U32                  *regAddrPtr
)
{
    GT_U8       localPort;/* local port - support multi-port-groups device */
    GT_U32      smi0portsNum;
                        /* number of ports which their phys are controled by SMI0  */
    GT_STATUS   rc;                 /* return code                  */
    GT_U32 portMacNum;          /* MAC number */

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        return prvCpssDxChEArchPhySmiMiscRegAddr(devNum,portMacNum,regAddrPtr);
    }
    else
    {
        rc = prvCpssDxChPhySmi0NumOfPortsGet(devNum, &smi0portsNum);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* for multi-port-group device : since this config is not really per port ,
            we will read the info from first active port group , and will set it to all port groups */
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        if (localPort < smi0portsNum)
        {
            *regAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.lms0MiscConfig;
        }
        else if (localPort < PRV_CPSS_DXCH_SMI_PPU_PORTS_NUM_CNS)
        {
            *regAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.lms1MiscConfig;
        }
        else
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    return GT_OK;
}
#endif


/*******************************************************************************
* internal_cpssDxChPhyAutonegSmiGet
*
* DESCRIPTION:
*       This function gets the auto negotiation status between the PP and PHY.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - physical device number.
*       portNum      - port number: applicable ports from 0 till 23
*
* OUTPUTS:
*       enabledPtr   - (Pointer to) the auto negotiation process state between
*                      PP and Phy:
*                      0 = Auto-Negotiation process is preformed.
*                      1 = Auto-Negotiation process is not preforme
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Although the device ignores the information read from the PHY
*       registers, it keeps polling those registers.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPhyAutonegSmiGet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_BOOL *enabledPtr
)
{
    GT_U32      regAddr;            /* register address     */
    GT_U32      autonegEn;          /* autoneg enabled flag */
                        /* number of ports which their phys are controled by SMI0  */
    GT_STATUS   rc;                 /* return code          */
    GT_U32 portMacNum;          /* MAC number */
    GT_U32 smiInstance;
    GT_U32 smiLocalPort;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(enabledPtr);

   /* skip ports that has no GE PHY */
    if((PRV_CPSS_XG_PORT_XG_ONLY_E == PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,portMacNum))
       || (PRV_CPSS_XG_PORT_HX_QX_ONLY_E == PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,portMacNum)))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        GT_BOOL val;

        if(portMacNum > PRV_CPSS_DXCH_E_ARCH_SMI_PPU_PORTS_NUM_CNS)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        rc = prvCpssDxChEArchPhyPortSMIGet(devNum,portMacNum,&smiInstance,&smiLocalPort);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDxChSMIAutoNegStateGet(devNum,0,smiInstance,&val);
        if (rc != GT_OK)
        {
            return rc;
        }
        autonegEn = val;
    }
    else
    {
        rc = prvCpssDxChPhyAutoNegotiationSmiRegAddr(devNum, portNum, /*OUT*/&smiInstance, /*OUT*/&regAddr);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* Read the SMI AutoNeg enabled bit */
        if (prvCpssDrvHwPpGetRegField(devNum, regAddr, 7, 1, &autonegEn) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
    }
    *enabledPtr = (autonegEn == 0) ? GT_TRUE : GT_FALSE;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPhyAutonegSmiGet
*
* DESCRIPTION:
*       This function gets the auto negotiation status between the PP and PHY.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - physical device number.
*       portNum      - port number: applicable ports from 0 till 23
*
* OUTPUTS:
*       enabledPtr   - (Pointer to) the auto negotiation process state between
*                      PP and Phy:
*                      0 = Auto-Negotiation process is preformed.
*                      1 = Auto-Negotiation process is not preforme
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Although the device ignores the information read from the PHY
*       registers, it keeps polling those registers.
*
*******************************************************************************/
GT_STATUS cpssDxChPhyAutonegSmiGet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_BOOL *enabledPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPhyAutonegSmiGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enabledPtr));

    rc = internal_cpssDxChPhyAutonegSmiGet(devNum, portNum, enabledPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enabledPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPhyAutonegSmiSet
*
* DESCRIPTION:
*       This function sets the auto negotiation process, between
*       the PP and PHY, to enable/disable.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - port number: applicable ports from 0 till 23
*       enable      - 0 = Auto-Negotiation process is preformed.
*                     1 = Auto-Negotiation process is not preforme
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Although the device ignores the information read from the PHY
*       registers, it keeps polling those registers.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPhyAutonegSmiSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_BOOL  enable
)
{
    GT_U32      regAddr;            /* register address             */
    GT_U32      autonegEn;          /* autoneg enabled flag         */
    GT_STATUS   rc;                 /* return code                  */
    GT_U32      portMacNum;         /* MAC number */
    GT_U32      smiInstance;
    GT_U32      smiLocalPort;
    PRV_CPSS_PORT_TYPE_OPTIONS_ENT      portTypeOptions;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    portTypeOptions = PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,portMacNum);
    /* skip ports that has no GE PHY */
    if(   (PRV_CPSS_XG_PORT_XG_ONLY_E    == portTypeOptions)
       || (PRV_CPSS_XG_PORT_HX_QX_ONLY_E == portTypeOptions))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);;
    }


    autonegEn = (enable == GT_TRUE) ? 0 : 1;
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        if(portMacNum > PRV_CPSS_DXCH_E_ARCH_SMI_PPU_PORTS_NUM_CNS)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        rc = prvCpssDxChEArchPhyPortSMIGet(devNum,portMacNum,&smiInstance,&smiLocalPort);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDxChSMIAutoNegStateSet(devNum,0,smiInstance,(GT_BOOL)autonegEn);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        rc = prvCpssDxChPhyAutoNegotiationSmiRegAddr(devNum, portNum, &smiInstance,&regAddr);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* Write the SMI AutoNeg enabled bit */
        if (prvCpssDrvHwPpSetRegField(devNum, regAddr, 7, 1, autonegEn) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
    }


CPSS_TBD_BOOKMARK_XCAT3
    /* new API for enable/disable Master SMI Polling should be added */
#if 0
    if(CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        rc = prvCpssDxChPhyMiscSmiRegAddr(devNum, portNum, &regAddr);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* Write the Enable_Master_SMI_Polling bit */
        if (prvCpssDrvHwPpSetRegField(devNum, regAddr, 21, 1, (1-autonegEn)) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
    }
#endif

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPhyAutonegSmiSet
*
* DESCRIPTION:
*       This function sets the auto negotiation process, between
*       the PP and PHY, to enable/disable.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - port number: applicable ports from 0 till 23
*       enable      - 0 = Auto-Negotiation process is preformed.
*                     1 = Auto-Negotiation process is not preforme
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Although the device ignores the information read from the PHY
*       registers, it keeps polling those registers.
*
*******************************************************************************/
GT_STATUS cpssDxChPhyAutonegSmiSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPhyAutonegSmiSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChPhyAutonegSmiSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortSMIAutoPollingMappingGet
*
* DESCRIPTION:
*        this function returns SMI instance , SMI local port and autopolling
*        automaton at SMI controller
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - physical device number.
*       portNum         - port number.
*
* OUTPUTS:
*       smiInterfacePtr     - smi IF
*       smiLocalPortPtr     - smi local port (logical)
*       autoPollingPlacePtr - hw place at SMI controller (physical)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. applicable to BC2-B0 and higher, just for SMI controller (i.e. SMI unit and not LMS unit)
*       2. BC2 B0/CAELUM : at SMI IF 0,2 autopolling place equal to smi logical port
*                    at SMI IF 1,3 autopolling place is (16 - number of autopolling ports + logical) (adjusted to 16
*                       ex : autopolling = 8 , than HW places are 8-15 corrsponts to logical 0-7
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortSMIAutoPollingMappingGet
(
    IN  GT_U8                        devNum,
    IN  GT_PHYSICAL_PORT_NUM         portNum,
    OUT CPSS_PHY_SMI_INTERFACE_ENT  *smiInterfacePtr,
    OUT GT_U32                      *smiLocalPortPtr,
    OUT GT_U32                      *autoPollingPlacePtr
)
{
    GT_STATUS rc;
    GT_U32      portMacNum;         /* MAC number */

    GT_U32      smiInstance;
        GT_U32      smiLocalPort;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(smiInterfacePtr);
    CPSS_NULL_PTR_CHECK_MAC(smiLocalPortPtr);
    CPSS_NULL_PTR_CHECK_MAC(autoPollingPlacePtr);

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, /*OUT*/portMacNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
            if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.gop_lms.notSupported == GT_TRUE)
            {
                rc = prvCpssDxChEArchPhyPortSMIGet(devNum,portMacNum,/*OUT*/&smiInstance,&smiLocalPort);
                if (rc != GT_OK)
                {
                    return rc;
                }
                rc = prvCpssDxChSMIPortAutopollingPlaceGet(devNum,0,smiInstance,smiLocalPort,/*OUT*/autoPollingPlacePtr);
                if (rc != GT_OK)
                {
                    return rc;
                }
            *smiInterfacePtr = (CPSS_PHY_SMI_INTERFACE_ENT)smiInstance;
                    *smiLocalPortPtr = smiLocalPort;
            return GT_OK;

            }

    }
    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
}



/*******************************************************************************
* prvCpssDxChPhySmiAutoMediaRegAddr
*
* DESCRIPTION:
*       This function gets a auto negotioation SMI configuration register
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number
*
* OUTPUTS:
*       regAddrPtr      - pointer to register address
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPhySmiAutoMediaRegAddr
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_U32               *regAddrPtr,
    OUT GT_U32               *localPortPtr

)
{
    GT_U32   localPort;          /* local port - support multi-port-groups device */
    GT_U32  portMacNum;         /* MAC number */

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portMacNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }
    else
    {
        if (localPort <= 5)
        {
            *regAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.lms0PhyAutoNeg[0];
            *localPortPtr = localPort;
        }
        else if (localPort <= 11)
        {
            *regAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.lms0PhyAutoNeg[1];
            *localPortPtr = localPort-6;
        }
        else if (localPort <= 17)
        {
            *regAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.lms1PhyAutoNeg[0];
            *localPortPtr = localPort-12;
        }
        else if (localPort <= 23)
        {
            *regAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.lms1PhyAutoNeg[1];
            *localPortPtr = localPort-18;
        }
        else
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPhySmiAutoMediaSelectSet
*
* DESCRIPTION:
*       This function gets a bit for port
*       indicating whether this port is connected to a dual-media PHY,
*       such as Marvell 88E1112, that is able to perform Auto-media select
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number: applicable ports from 0-23 (single LMS) 0-47 (2 LMS or 4 SMI units)
*       autoMediaSelect - 0 = Port is not connected to dual-media PHY
*                         1 = Port is connected to dual-media PHY
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_OUT_OF_RANGE          - phyAddr bigger then 31
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPhySmiAutoMediaSelectSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN GT_U32   autoMediaSelect
)
{
    GT_U32      regAddr;            /* register address     */
    GT_U32      regOffset;
    GT_STATUS   rc;                 /* return status */
    GT_U32      portMacNum;         /* MAC number */
    GT_U32      localPortNum;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

   /* skip ports that has no GE PHY */
    if(   (PRV_CPSS_XG_PORT_XG_ONLY_E    == PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,portMacNum))
       || (PRV_CPSS_XG_PORT_HX_QX_ONLY_E == PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,portMacNum)))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(autoMediaSelect >= 2)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        GT_U32      smiInstance;
        GT_U32      smiLocalPort;

        if(portMacNum > PRV_CPSS_DXCH_E_ARCH_SMI_PPU_PORTS_NUM_CNS)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        rc = prvCpssDxChEArchPhyPortSMIGet(devNum,portMacNum,&smiInstance,&smiLocalPort);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = prvCpssDxChSMIPortAutoMediaDetectStatusSet(devNum,0,smiInstance,smiLocalPort,autoMediaSelect);
        if (rc != GT_OK)
        {
            return rc;
        }
        return GT_OK;
    }
    else
    {
        rc = prvCpssDxChPhySmiAutoMediaRegAddr(devNum, portMacNum, &regAddr,&localPortNum);
        if(rc != GT_OK)
        {
            return rc;
        }

        regOffset = 1<<localPortNum;
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, regOffset, 1, autoMediaSelect);
        if (rc != GT_OK)
        {
            return rc;
        }
        return GT_OK;
    }
}

/*******************************************************************************
* cpssDxChPhySmiAutoMediaSelectSet
*
* DESCRIPTION:
*       This function gets a bit for port
*       indicating whether this port is connected to a dual-media PHY,
*       such as Marvell 88E1112, that is able to perform Auto-media select
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number: applicable ports from 0-23 (single LMS) 0-47 (2 LMS or 4 SMI units)
*       autoMediaSelect - 0 = Port is not connected to dual-media PHY
*                         1 = Port is connected to dual-media PHY
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_OUT_OF_RANGE          - phyAddr bigger then 31
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPhySmiAutoMediaSelectSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN GT_U32   autoMediaSelect
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPhySmiAutoMediaSelectSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, autoMediaSelect));

    rc = internal_cpssDxChPhySmiAutoMediaSelectSet(devNum, portNum, autoMediaSelect);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, autoMediaSelect));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPhySmiAutoMediaSelectGet
*
* DESCRIPTION:
*       This function gets a bit for given port
*       indicating whether this port is connected to a dual-media PHY,
*       such as Marvell 88E1112, that is able to perform Auto-media select
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number: applicable ports from 0-23 (single LMS) 0-47 (2 LMS or 4 SMI units)
*
* OUTPUTS:
*       autoMediaSelectPtr - (Pointer to) port state
*                            0 = Port is not connected to dual-media PHY
*                            1 = Port is connected to dual-media PHY
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPhySmiAutoMediaSelectGet
(

    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_U32   *autoMediaSelectPtr
)
{
    GT_U32      regAddr;    /* register address     */
    GT_U32      regOffset;
    GT_STATUS   rc;         /* retun status */
    GT_U32      portMacNum;         /* MAC number */
    GT_U32      localPortNum;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(autoMediaSelectPtr);

   /* skip ports that has no GE PHY */
    if((PRV_CPSS_XG_PORT_XG_ONLY_E == PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,portMacNum))
       || (PRV_CPSS_XG_PORT_HX_QX_ONLY_E == PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum,portMacNum)))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        GT_U32      smiInstance;
        GT_U32      smiLocalPort;

        if(portMacNum > PRV_CPSS_DXCH_E_ARCH_SMI_PPU_PORTS_NUM_CNS)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        rc = prvCpssDxChEArchPhyPortSMIGet(devNum,portMacNum,&smiInstance,&smiLocalPort);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = prvCpssDxChSMIPortAutoMediaDetectStatusGet(devNum,0,smiInstance,smiLocalPort,/*OUT*/autoMediaSelectPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
        return GT_OK;
    }
    else
    {
        rc = prvCpssDxChPhySmiAutoMediaRegAddr(devNum, portMacNum, &regAddr,&localPortNum);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        regOffset = 1<<localPortNum;
        rc=  prvCpssDrvHwPpGetRegField(devNum, regAddr, regOffset, 1, autoMediaSelectPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
        return GT_OK;
    }
}

/*******************************************************************************
* cpssDxChPhySmiAutoMediaSelectGet
*
* DESCRIPTION:
*       This function gets a bit for given port
*       indicating whether this port is connected to a dual-media PHY,
*       such as Marvell 88E1112, that is able to perform Auto-media select
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number: applicable ports from 0-23 (single LMS) 0-47 (2 LMS or 4 SMI units)
*
* OUTPUTS:
*       autoMediaSelectPtr - (Pointer to) port state
*                            0 = Port is not connected to dual-media PHY
*                            1 = Port is connected to dual-media PHY
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPhySmiAutoMediaSelectGet
(

    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_U32   *autoMediaSelectPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPhySmiAutoMediaSelectGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, autoMediaSelectPtr));

    rc = internal_cpssDxChPhySmiAutoMediaSelectGet(devNum, portNum, autoMediaSelectPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, autoMediaSelectPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPhySmiCtrlRegAddr
*
* DESCRIPTION:
*       Calculate SMI control register address
*
* INPUTS:
*       devNum      - The PP to write to.
*       smiInterface - SMI interface.
*
* OUTPUTS:
*       regAddrPtr  - pointer to register address.
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPhySmiCtrlRegAddr
(
    IN  GT_U8    devNum,
    IN  CPSS_PHY_SMI_INTERFACE_ENT  smiInterface,
    OUT GT_U32   *regAddrPtr
)
{
    GT_STATUS rc;
    GT_U32  smiCtrlRegUnitsStep; /* Step between units containing the SMI control register */

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        rc = prvCpssDxChSMIDrvManagementRegAddrGet(devNum,(GT_U32)smiInterface,/*OUT*/regAddrPtr);
        if (rc != GT_OK)
        {
            return rc;
        }

    }
    else
    {
        switch(smiInterface)
        {
            case CPSS_PHY_SMI_INTERFACE_0_E:
                /* use SMI#0 controller to access port's PHY*/
                *regAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiControl;
                break;
            case CPSS_PHY_SMI_INTERFACE_1_E:
                /* use SMI#1 controller to access port's PHY*/
                if(CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
                {
                    smiCtrlRegUnitsStep = 2 * CPSS_DX_SMI_MNG_CNTRL_OFFSET_CNS;
                }
                else
                {
                    smiCtrlRegUnitsStep = CPSS_DX_SMI_MNG_CNTRL_OFFSET_CNS;
                }
                *regAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiControl +
                                                            smiCtrlRegUnitsStep;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPhySmiCtrlRegWrite
*
* DESCRIPTION:
*       Write to SMI control register
*
* INPUTS:
*       devNum      - The PP to write to.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       smiInterface - SMI interface.
*       value        - The value to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_BAD_PARAM - wrong smiIf
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPhySmiCtrlRegWrite
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN  CPSS_PHY_SMI_INTERFACE_ENT  smiInterface,
    IN GT_U32   value
)
{
    GT_U32 regAddr;         /* register address */
    GT_STATUS rc;           /* return status */

    rc = prvCpssDxChPhySmiCtrlRegAddr(devNum, smiInterface, &regAddr);
    if(rc != GT_OK)
    {
        return rc;
    }

    return prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId, regAddr, value);
}

/*******************************************************************************
* prvCpssDxChPhySmiCtrlRegRead
*
* DESCRIPTION:
*       Read to SMI control register
*
* INPUTS:
*       devNum      - The PP to read from.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       smiInterface    - SMI interface.
*
* OUTPUTS:
*       dataPtr - Includes the register value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_BAD_PARAM - wrong smiIf
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPhySmiCtrlRegRead
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN  CPSS_PHY_SMI_INTERFACE_ENT  smiInterface,
    OUT GT_U32  *dataPtr
)
{
    GT_U32 regAddr;         /* register address */
    GT_STATUS rc;           /* return status */

    rc = prvCpssDxChPhySmiCtrlRegAddr(devNum, smiInterface, &regAddr);
    if(rc != GT_OK)
    {
        return rc;
    }

    return prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, dataPtr);
}

/*******************************************************************************
* prvCpssDxChPhyXSmiMngRegsAddrGet
*
* DESCRIPTION:
*       Get addresses of control and address XSMI registers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The PP to read from.
*       xsmiInterface - XSMI instance
*
* OUTPUTS:
*       ctrlRegAddrPtr - address of XSMI control register
*       addrRegAddrPtr - address of XSMI address register
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM - bad parameter
*       GT_BAD_PTR  - NULL pointer in ctrlRegAddr or addrRegAddr
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPhyXSmiMngRegsAddrGet
(
    IN GT_U8    devNum,
    IN CPSS_PHY_XSMI_INTERFACE_ENT xsmiInterface,
    OUT GT_U32  *ctrlRegAddrPtr,
    OUT GT_U32  *addrRegAddrPtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(ctrlRegAddrPtr);
    CPSS_NULL_PTR_CHECK_MAC(addrRegAddrPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum))
    {
        if((xsmiInterface != CPSS_PHY_XSMI_INTERFACE_0_E) &&
           (xsmiInterface != CPSS_PHY_XSMI_INTERFACE_1_E))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        *ctrlRegAddrPtr = PRV_DXCH_REG1_UNIT_MG_MAC(devNum).XSMI[xsmiInterface].
                                                                XSMIManagement;
        *addrRegAddrPtr = PRV_DXCH_REG1_UNIT_MG_MAC(devNum).XSMI[xsmiInterface].
                                                                    XSMIAddress;
    }
    else
    {
        *ctrlRegAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smi10GeControl;
        *addrRegAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smi10GeAddr;
    }

    return GT_OK;
}

/******************************************************************************
* prvCpssDxChPhySmiObjInit
*
* DESCRIPTION:
*       Initialise SMI service function pointers : SMI Ctrl Reg. Read/Write.
*       The generic SMI functions cpssSmiRegisterReadShort,
*       cpssSmiRegisterWriteShort use these pointers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - the device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       none.
*
* COMMENTS:
*******************************************************************************/
GT_VOID prvCpssDxChPhySmiObjInit
(
    IN  GT_U8     devNum
)
{
    /* assign relevant virtual functions */
    PRV_CPSS_PP_MAC(devNum)->phyInfo.genSmiBindFunc.
                          cpssPhySmiCtrlRegRead = prvCpssDxChPhySmiCtrlRegRead;

    PRV_CPSS_PP_MAC(devNum)->phyInfo.genSmiBindFunc.
                          cpssPhySmiCtrlRegWrite = prvCpssDxChPhySmiCtrlRegWrite;

    PRV_CPSS_PP_MAC(devNum)->phyInfo.genXSmiBindFunc.
                          cpssPhyXSmiRegsAddrGet = prvCpssDxChPhyXSmiMngRegsAddrGet;

    return;
}

/******************************************************************************
* vctObjInit
* DESCRIPTION:
*       Initialise all function pointers : Read/Write SMI, AutoNegotiation
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - the device number
* OUTPUTS:
*       None.
*
* RETURNS:
*       none.
* COMMENTS:
*******************************************************************************/

static GT_VOID vctObjInit
(
    IN  GT_U8     devNum
)
{
    /* assign relevant virtual functions */
    PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.
                           cpssPhyAutoNegotiationGet = cpssDxChPhyAutonegSmiGet;

    PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.
                           cpssPhyAutoNegotiationSet = cpssDxChPhyAutonegSmiSet;

    PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.
                          cpssPhyRegisterRead = cpssDxChPhyPortSmiRegisterRead;

    PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.
                          cpssPhyRegisterWrite =cpssDxChPhyPortSmiRegisterWrite;


    return ;

}


/*******************************************************************************
* prvCpssDxChPhySmi0NumOfPortsGet
*
* DESCRIPTION:
*       Get number of Auto Poll ports for PHY SMI 0.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*
* OUTPUTS:
*       smi0PortsNumPtr - number of auto poll ports for SMI 0.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on fail
*       GT_BAD_PARAM     - on wrong devNum or portNum.
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPhySmi0NumOfPortsGet
(
  IN  GT_U8     devNum,
  OUT GT_U32   *smi0PortsNumPtr
)
{
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT       smi0ports;
                        /* PHY SMI 0 AUTO_POLL mode */
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT       smi1ports;
                        /* PHY SMI 1 AUTO_POLL mode */
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT       smi2ports;
                        /* PHY SMI 2 AUTO_POLL mode */
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT       smi3ports;
                        /* PHY SMI 3 AUTO_POLL mode */
    GT_STATUS       rc;     /* return code  */


    /* 2 SMI Master controllers exists - SMI0 and SMI1.                                       */
    /* For xCat FE devices due to the use of 3 octal phys, SMI0 is responsible for ports 0-15 */
    /* and SMI1 for ports 16-23. For other devices the is equal SMI0 and SMI1 for ports 0-11  */
    /* and 12-23 respectively.                                                                */

    /* Check device type */
    if ((PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum) &&
            (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)) ||
            (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E) ||
            (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        /* xCat A1 devices */

        /* Get Auto Poll configuration */
        rc = cpssDxChPhyAutoPollNumOfPortsGet(devNum, &smi0ports, &smi1ports,
                                                      &smi2ports, &smi3ports);
        if (rc != GT_OK)
        {
            return rc;
        }
        *smi0PortsNumPtr = (GT_U32)smi0ports;
    }
    else
    {
        switch(PRV_CPSS_PP_MAC(devNum)->devType)
        {
            case PRV_CPSS_DXCH_XCAT_FE_DEVICES_CASES_MAC:
                *smi0PortsNumPtr = 16;
                break;
            default:
                *smi0PortsNumPtr = 12;
                break;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPhyXsmiMdcDivisionFactorSet
*
* DESCRIPTION:
*       This function sets MDC frequency for Master XSMI Interface
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - physical device number.
*       xsmiInterface   - XSMI master interface number
*                           (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*       divisionFactor  - Division factor of the core clock to get the MDC
*                                           (Serial Management Interface Clock).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, xsmiInterface or divisionFactor
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPhyXsmiMdcDivisionFactorSet
(
    IN GT_U8 devNum,
    IN  CPSS_PHY_XSMI_INTERFACE_ENT    xsmiInterface,
    IN CPSS_PHY_SMI_MDC_DIVISION_FACTOR_ENT divisionFactor
)
{
    GT_U32      regAddr;  /* register address        */
    GT_U32      divSel;   /* division selector value */
    GT_U32      fastMdcEn = 0;
    GT_U32      interfaceIdx;/* XSMI interface index */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        switch(divisionFactor)
        {
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_16_E:
                fastMdcEn = 1;
                break;
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_128_E:
                fastMdcEn = 0;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.hgXsmiiCountersCtr;
        if (prvCpssDrvHwPpSetRegField(devNum, regAddr, 12, 1, fastMdcEn) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

    }
    else if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) ||
            (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
            (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.xsmiConfig;
        switch(divisionFactor)
        {
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_256_E:
                divSel = 0;
                break;
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_64_E:
                divSel = 1;
                break;
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_32_E:
                divSel = 2;
                break;
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_8_E:
                divSel = 3;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        if (prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 2, divSel) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
    }
    else if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        switch(xsmiInterface)
        {
            case CPSS_PHY_XSMI_INTERFACE_0_E:
                interfaceIdx = 0;
            break;
            case CPSS_PHY_XSMI_INTERFACE_1_E:
                interfaceIdx = 1;
            break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        regAddr = PRV_DXCH_REG1_UNIT_MG_MAC(devNum).XSMI[interfaceIdx].XSMIConfiguration;
        switch(divisionFactor)
        {
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_256_E:
                divSel = 0;
                break;
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_64_E:
                divSel = 1;
                break;
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_32_E:
                divSel = 2;
                break;
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_8_E:
                divSel = 3;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if (prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 2, divSel) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPhyXsmiMdcDivisionFactorSet
*
* DESCRIPTION:
*       This function sets MDC frequency for Master XSMI Interface
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - physical device number.
*       xsmiInterface   - XSMI master interface number
*                           (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*       divisionFactor  - Division factor of the core clock to get the MDC
*                                           (Serial Management Interface Clock).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, xsmiInterface or divisionFactor
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPhyXsmiMdcDivisionFactorSet
(
    IN GT_U8 devNum,
    IN  CPSS_PHY_XSMI_INTERFACE_ENT    xsmiInterface,
    IN CPSS_PHY_SMI_MDC_DIVISION_FACTOR_ENT divisionFactor
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPhyXsmiMdcDivisionFactorSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, xsmiInterface, divisionFactor));

    rc = internal_cpssDxChPhyXsmiMdcDivisionFactorSet(devNum, xsmiInterface, divisionFactor);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, xsmiInterface, divisionFactor));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPhyXsmiMdcDivisionFactorGet
*
* DESCRIPTION:
*       This function gets MDC frequency for Master XSMI Interface
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - physical device number.
*       xsmiInterface   - XSMI master interface number
*                           (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*
* OUTPUTS:
*       divisionFactorPtr  - (Pointer to) Division factor of the core clock
*                           to get the MDC (Serial Management Interface Clock).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, xsmiInterface or divisionFactorPtr
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPhyXsmiMdcDivisionFactorGet
(
    IN  GT_U8 devNum,
    IN  CPSS_PHY_XSMI_INTERFACE_ENT    xsmiInterface,
    OUT CPSS_PHY_SMI_MDC_DIVISION_FACTOR_ENT *divisionFactorPtr
)
{
    GT_U32      regAddr;  /* register address        */
    GT_U32      divSel;   /* division selector value */
    GT_U32      fastMdcEn;
    GT_U32      interfaceIdx;   /* XSMI master interface number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(divisionFactorPtr);

    if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.hgXsmiiCountersCtr;
        if (prvCpssDrvHwPpGetRegField(devNum, regAddr, 12, 1, &fastMdcEn) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

        if(fastMdcEn == 0)
            *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_128_E;
        else
            *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_16_E;
    }
    else if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) ||
            (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
            (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.xsmiConfig;
        if (prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 2, &divSel) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
        switch(divSel)
        {
            case 0:
                *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_256_E;
                break;
            case 1:
                *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_64_E;
                break;
            case 2:
                *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_32_E;
                break;
            case 3:
                *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_8_E;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
    }
    else if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        switch(xsmiInterface)
        {
            case CPSS_PHY_XSMI_INTERFACE_0_E:
                interfaceIdx = 0;
            break;
            case CPSS_PHY_XSMI_INTERFACE_1_E:
                interfaceIdx = 1;
            break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        regAddr = PRV_DXCH_REG1_UNIT_MG_MAC(devNum).XSMI[interfaceIdx].XSMIConfiguration;
        if (prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 2, &divSel) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
        switch(divSel)
        {
            case 0:
                *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_256_E;
                break;
            case 1:
                *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_64_E;
                break;
            case 2:
                *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_32_E;
                break;
            case 3:
                *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_8_E;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPhyXsmiMdcDivisionFactorGet
*
* DESCRIPTION:
*       This function gets MDC frequency for Master XSMI Interface
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - physical device number.
*       xsmiInterface   - XSMI master interface number
*                           (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*
* OUTPUTS:
*       divisionFactorPtr  - (Pointer to) Division factor of the core clock
*                           to get the MDC (Serial Management Interface Clock).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, xsmiInterface or divisionFactorPtr
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPhyXsmiMdcDivisionFactorGet
(
    IN  GT_U8 devNum,
    IN  CPSS_PHY_XSMI_INTERFACE_ENT    xsmiInterface,
    OUT CPSS_PHY_SMI_MDC_DIVISION_FACTOR_ENT *divisionFactorPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPhyXsmiMdcDivisionFactorGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, xsmiInterface, divisionFactorPtr));

    rc = internal_cpssDxChPhyXsmiMdcDivisionFactorGet(devNum, xsmiInterface, divisionFactorPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, xsmiInterface, divisionFactorPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPhySmiMdcDivisionFactorSet
*
* DESCRIPTION:
*       This function sets Fast MDC Division Selector
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number.
*       divisionFactor  - Division factor of the core clock to get the MDC
*                                           (Serial Management Interface Clock).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or divisionFactor
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPhySmiMdcDivisionFactorSet
(
    IN GT_U8 devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN CPSS_PHY_SMI_MDC_DIVISION_FACTOR_ENT divisionFactor
)
{
    GT_U32      regAddr;  /* register address        */
    GT_U32      divSel;   /* division selector value */
    GT_U32      fastMdcEn;/* fast MDC enable */
    GT_U32      smi0portsNum;
                        /* number of ports which their phys are controled by SMI0  */
    GT_STATUS rc;       /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

        rc = prvCpssDxChPhySmi0NumOfPortsGet(devNum, &smi0portsNum);
        if (rc != GT_OK)
        {
            return rc;
        }

        fastMdcEn = 1;
        switch(divisionFactor)
        {
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_16_E:
                divSel = 0;
                break;
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_32_E:
                divSel = 1;
                break;
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_64_E:
                divSel = 2;
                break;
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_128_E:
                fastMdcEn = 0;
                divSel = 0; /* to avoid VC warning */
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if(portNum < smi0portsNum)
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiPhyAddr;
        else
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiPhyAddr + 0x1000000;

        if (prvCpssDrvHwPpSetRegField(devNum, regAddr, 30, 1, fastMdcEn) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

        if(fastMdcEn != 0)
        {
            if(portNum < smi0portsNum)
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.lms0MiscConfig;
            else
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.lms1MiscConfig;
            if (prvCpssDrvHwPpSetRegField(devNum, regAddr, 15, 2, divSel) != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }
        }

    }
    else if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
            (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E))
    {
        switch(divisionFactor)
        {
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_32_E:
                divSel = 0;
                break;
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_64_E:
                divSel = 1;
                break;
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_128_E:
                divSel = 2;
                break;
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_256_E:
                divSel = 3;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.lms0MiscConfig;
        if (prvCpssDrvHwPpSetRegField(devNum, regAddr, 15, 2, divSel) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
    }
    else if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        GT_U32      smiInstance;
        GT_U32      smiLocalPort;
        GT_U32      portMacNum;         /* MAC number */

        PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);


        rc = prvCpssDxChEArchPhyPortSMIGet(devNum,portMacNum,&smiInstance,&smiLocalPort);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDxChSMIMdcDivisionFactorSet(devNum,0,smiInstance,divisionFactor);
        if (rc != GT_OK)
        {
            return rc;
        }
        return GT_OK;
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPhySmiMdcDivisionFactorSet
*
* DESCRIPTION:
*       This function sets Fast MDC Division Selector
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number.
*       divisionFactor  - Division factor of the core clock to get the MDC
*                                           (Serial Management Interface Clock).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or divisionFactor
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPhySmiMdcDivisionFactorSet
(
    IN GT_U8 devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN CPSS_PHY_SMI_MDC_DIVISION_FACTOR_ENT divisionFactor
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPhySmiMdcDivisionFactorSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, divisionFactor));

    rc = internal_cpssDxChPhySmiMdcDivisionFactorSet(devNum, portNum, divisionFactor);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, divisionFactor));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPhySmiMdcDivisionFactorGet
*
* DESCRIPTION:
*       This function gets Fast MDC Division Selector
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number.
*
* OUTPUTS:
*       divisionFactorPtr  - (Pointer to) Division factor of the core clock
*                           to get the MDC (Serial Management Interface Clock).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or divisionFactor
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPhySmiMdcDivisionFactorGet
(
    IN  GT_U8 devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT CPSS_PHY_SMI_MDC_DIVISION_FACTOR_ENT *divisionFactorPtr
)
{
    GT_U32      regAddr;  /* register address        */
    GT_U32      divSel;   /* division selector value */
    GT_U32      fastMdcEn;/* fast MDC enable */
    GT_U32      smi0portsNum;
                        /* number of ports which their phys are controled by SMI0 */
    GT_STATUS rc;       /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(divisionFactorPtr);

    if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

        rc = prvCpssDxChPhySmi0NumOfPortsGet(devNum, &smi0portsNum);
        if (rc != GT_OK)
        {
            return rc;
        }

        if(portNum < smi0portsNum)
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiPhyAddr;
        else
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiPhyAddr + 0x1000000;

        if (prvCpssDrvHwPpGetRegField(devNum, regAddr, 30, 1, &fastMdcEn) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

        if(fastMdcEn == 0)
        {
            *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_128_E;
            return GT_OK;
        }

        if(portNum < smi0portsNum)
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.lms0MiscConfig;
        else
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.lms1MiscConfig;
        if (prvCpssDrvHwPpGetRegField(devNum, regAddr, 15, 2, &divSel) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

        switch(divSel)
        {
            case 0:
                *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_16_E;
                break;
            case 1:
                *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_32_E;
                break;
            case 2:
                *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_64_E;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

    }
    else if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
            (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.lms0MiscConfig;
        if (prvCpssDrvHwPpGetRegField(devNum, regAddr, 15, 2, &divSel) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

        switch(divSel)
        {
            case 0:
                *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_32_E;
                break;
            case 1:
                *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_64_E;
                break;
            case 2:
                *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_128_E;
                break;
            case 3:
                *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_256_E;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        GT_U32      smiInstance;
        GT_U32      smiLocalPort;
        GT_U32      portMacNum;         /* MAC number */

        PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

        rc = prvCpssDxChEArchPhyPortSMIGet(devNum,portMacNum,&smiInstance,&smiLocalPort);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDxChSMIMdcDivisionFactorGet(devNum,0,smiInstance,/*OUT*/divisionFactorPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
        return GT_OK;
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPhySmiMdcDivisionFactorGet
*
* DESCRIPTION:
*       This function gets Fast MDC Division Selector
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number.
*
* OUTPUTS:
*       divisionFactorPtr  - (Pointer to) Division factor of the core clock
*                           to get the MDC (Serial Management Interface Clock).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or divisionFactor
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPhySmiMdcDivisionFactorGet
(
    IN  GT_U8 devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT CPSS_PHY_SMI_MDC_DIVISION_FACTOR_ENT *divisionFactorPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPhySmiMdcDivisionFactorGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, divisionFactorPtr));

    rc = internal_cpssDxChPhySmiMdcDivisionFactorGet(devNum, portNum, divisionFactorPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, divisionFactorPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPhyBobcat2SmiXsmiMuxSet
*
* DESCRIPTION:
*       Bobcat2 has XSMI#1 and SMI#3 but mutual exclusive. Both of them use same
*       pins. This means either XSMI#1 or SMI#3 may be used on board.
*
* APPLICABLE DEVICES:
*        Bobcat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - physical device number.
*       enableXsmi - XSMI master interface number:
*                           GT_TRUE - use XSMI#1
*                           GT_FALSE - use SMI#3
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPhyBobcat2SmiXsmiMuxSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enableXsmi
)
{
    GT_U32      regAddr;  /* register address        */
    GT_STATUS   rc;       /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
            CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
            CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_XCAT3_E);

    regAddr =
        PRV_CPSS_DXCH_DEV_RESET_AND_INIT_CONTROLLER_REGS_MAC(devNum)->
                          DFXServerUnitsBC2SpecificRegs.deviceCtrl4;

    rc = prvCpssDrvHwPpResetAndInitControllerSetRegField(devNum, regAddr, 18,
                                                1, BOOL2BIT_MAC(enableXsmi));

    return rc;
}

/*******************************************************************************
* cpssDxChPhyBobcat2SmiXsmiMuxSet
*
* DESCRIPTION:
*       Bobcat2 has XSMI#1 and SMI#3 but mutual exclusive. Both of them use same
*       pins. This means either XSMI#1 or SMI#3 may be used on board.
*
* APPLICABLE DEVICES:
*        Bobcat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - physical device number.
*       enableXsmi - XSMI master interface number:
*                           GT_TRUE - use XSMI#1
*                           GT_FALSE - use SMI#3
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPhyBobcat2SmiXsmiMuxSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enableXsmi
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPhyBobcat2SmiXsmiMuxSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enableXsmi));

    rc = internal_cpssDxChPhyBobcat2SmiXsmiMuxSet(devNum, enableXsmi);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enableXsmi));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPhyBobcat2SmiXsmiMuxGet
*
* DESCRIPTION:
*       Get what interface is active XSMI#1 or SMI#3.
*
* APPLICABLE DEVICES:
*        Bobcat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - physical device number.
*
* OUTPUTS:
*       enableXsmiPtr - XSMI master interface number:
*                           GT_TRUE - use XSMI#1
*                           GT_FALSE - use SMI#3
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - enableXsmiPtr == NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPhyBobcat2SmiXsmiMuxGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enableXsmiPtr
)
{
    GT_U32      regAddr;  /* register address        */
    GT_STATUS   rc;       /* return code */
    GT_U32      value;    /* field value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
            CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
            CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E |
            CPSS_XCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(enableXsmiPtr);

    regAddr =
        PRV_CPSS_DXCH_DEV_RESET_AND_INIT_CONTROLLER_REGS_MAC(devNum)->
                          DFXServerUnitsBC2SpecificRegs.deviceCtrl4;

    rc = prvCpssDrvHwPpResetAndInitControllerGetRegField(devNum, regAddr, 18,
                                                1, &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    *enableXsmiPtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPhyBobcat2SmiXsmiMuxGet
*
* DESCRIPTION:
*       Get what interface is active XSMI#1 or SMI#3.
*
* APPLICABLE DEVICES:
*        Bobcat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - physical device number.
*
* OUTPUTS:
*       enableXsmiPtr - XSMI master interface number:
*                           GT_TRUE - use XSMI#1
*                           GT_FALSE - use SMI#3
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - enableXsmiPtr == NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPhyBobcat2SmiXsmiMuxGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enableXsmiPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPhyBobcat2SmiXsmiMuxGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enableXsmiPtr));

    rc = internal_cpssDxChPhyBobcat2SmiXsmiMuxGet(devNum, enableXsmiPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enableXsmiPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPhyXSmiMDCInvertSet
*
* DESCRIPTION:
*       Set Invert or Do Not Invert XSmi MDC.
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - physical device number.
        invertMDC  - flag - invert or do not invert XSMI MDC
*
* OUTPUTS:
        None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error

*
* COMMENTS:
*       This function changed bit in the register.
*
*******************************************************************************/
GT_STATUS internal_cpssDxChPhyXSmiMDCInvertSet
(
    IN  GT_U8    devNum,
    IN  GT_BOOL  invertMDC
)
{
    GT_STATUS rc;
    GT_U32 regAddr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
            CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
            CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E |
            CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.xgControl;
    if(invertMDC)
    {
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 14, 1, 0);
    }
    else
    {
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 14, 1, 1);
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPhyXSmiMDCInvertSet
*
* DESCRIPTION:
*       Set Invert or Do Not Invert XSmi MDC.
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - physical device number.
        invertMDC  - flag - invert or do not invert XSMI MDC
*
* OUTPUTS:
        None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error

*
* COMMENTS:
*       This is function - wrapper for internal_cpssDxChPhyXSmiMDCInvertSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPhyXSmiMDCInvertSet
(
    IN  GT_U8    devNum,
    IN  GT_BOOL  invertMDC
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPhyXSmiMDCInvertSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, invertMDC));

    rc = internal_cpssDxChPhyXSmiMDCInvertSet(devNum, invertMDC);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, invertMDC));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPhyXSmiMDCInvertGet
*
* DESCRIPTION:
*       Get XSmi MDC setting (Invert or Do Not Invert)
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - physical device number.
*
* OUTPUTS:
        *invertMDCPtr - Flag - Invert or Do Not Invert XSMI MDC
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error

*
* COMMENTS:
*       This function read bit in the register.
*
*******************************************************************************/
GT_STATUS internal_cpssDxChPhyXSmiMDCInvertGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL  *invertMDCPtr
)
{
    GT_STATUS rc = GT_TRUE;
    GT_U32 regAddr;
    GT_U32 value;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
            CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
            CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E |
            CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(invertMDCPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.xgControl;
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 14, 1, &value);
    if(rc == GT_OK)
    {
        if(value == 0)
        {
            *invertMDCPtr = GT_TRUE;
        }
        else
        {
            *invertMDCPtr = GT_FALSE;
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPhyXSmiMDCInvertGet
*
* DESCRIPTION:
*       Get XSmi MDC setting (Invert or Do Not Invert)
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - physical device number.
*
* OUTPUTS:
        *invertMDCPtr - Flag - Invert or Do Not Invert XSMI MDC
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error

*
* COMMENTS:
*       This is function - wrapper for internal_cpssDxChPhyXSmiMDCInvertGet.
*
*******************************************************************************/
GT_STATUS cpssDxChPhyXSmiMDCInvertGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *invertMDCPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPhyXSmiMDCInvertGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, invertMDCPtr));

    rc = internal_cpssDxChPhyXSmiMDCInvertGet(devNum, invertMDCPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, invertMDCPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

