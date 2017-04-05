/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortCtrl.c
*
* DESCRIPTION:
*       CPSS implementation for Port configuration and control facility.
*
*       The following APIs can run now "preliminary stage" and "post stage" callbacks
*       if the bind with port MAC object pointer will be run:
*
*        - cpssDxChPortDuplexAutoNegEnableSet;
*        - cpssDxChPortDuplexAutoNegEnableGet;
*        - cpssDxChPortFlowCntrlAutoNegEnableSet;
*        - cpssDxChPortFlowCntrlAutoNegEnableGet;
*        - cpssDxChPortSpeedAutoNegEnableSet;
*        - cpssDxChPortSpeedAutoNegEnableGet;
*        - cpssDxChPortFlowControlEnableSet;
*        - cpssDxChPortFlowControlEnableGet;
*        - cpssDxChPortPeriodicFcEnableSet;
*        - cpssDxChPortPeriodicFcEnableGet;
*        - cpssDxChPortBackPressureEnableSet;
*        - cpssDxChPortBackPressureEnableGet;
*        - cpssDxChPortLinkStatusGet;
*        - cpssDxChPortDuplexModeSet;
*        - cpssDxChPortDuplexModeGet;
*        - cpssDxChPortEnableSet;
*        - cpssDxChPortEnableGet;
*        - cpssDxChPortExcessiveCollisionDropEnableSet;
*        - cpssDxChPortExcessiveCollisionDropEnableGet;
*        - cpssDxChPortPaddingEnableSet;
*        - cpssDxChPortPaddingEnableGet;
*        - cpssDxChPortPreambleLengthSet;
*        - cpssDxChPortPreambleLengthGet;
*        - cpssDxChPortCrcCheckEnableSet;
*        - cpssDxChPortCrcCheckEnableGet;
*        - cpssDxChPortMruSet;
*        - cpssDxChPortMruGet;
*
* FILE REVISION NUMBER:
*       $Revision: 225 $
*******************************************************************************/

/* macro needed to support the call to PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC */
/* this define must come before include files */
#define PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_SUPPORTED_FLAG_CNS
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortLog.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/macCtrl/prvCpssDxChMacCtrl.h>
#include <cpss/dxCh/dxChxGen/port/PortMapping/prvCpssDxChPortMappingShadowDB.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortIfModeCfgBobKResource.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/dxCh/dxChxGen/phy/cpssDxChPhySmi.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/cscd/cpssDxChCscd.h>
#include <cpss/dxCh/dxChxGen/vnt/cpssDxChVnt.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortTx.h>
#include <cpss/generic/private/prvCpssCommonDefs.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCombo.h>

#include <cpss/generic/extMac/cpssExtMacDrv.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>

#include <common/siliconIf/mvSiliconIf.h>
#include <private/mvHwsPortMiscIf.h>
#include <private/mvPortModeElements.h>
#include <pcs/mvHwsPcsIf.h>

#ifdef OS_MALLOC_MEMORY_LEAKAGE_DBG
static GT_VOID *myCpssOsMalloc
(
    IN GT_U32 size
)
{
    return cpssOsMalloc_MemoryLeakageDbg(size,__FILE__,__LINE__);
}
static GT_VOID myCpssOsFree
(
    IN GT_VOID* const memblock
)
{
    cpssOsFree_MemoryLeakageDbg(memblock,__FILE__,__LINE__);
}
#endif /*OS_MALLOC_MEMORY_LEAKAGE_DBG*/

/* PATCH : indicate the HWS that we use bobcat3 */
extern void hwsBobcat3ForceModeSet(void);

extern GT_STATUS prvCpssDxChPortIfFunctionsObjInit
(
    INOUT PRV_CPSS_DXCH_PORT_OBJ_STC *objPtr,
    IN    CPSS_PP_FAMILY_TYPE_ENT     devFamily
);


extern PRV_CPSS_DXCH_PORT_INTERFACE_MODE_SET_FUN
        portIfModeFuncPtrArray[CPSS_PP_FAMILY_END_DXCH_E-CPSS_PP_FAMILY_START_DXCH_E-1]
                                                                        [CPSS_PORT_INTERFACE_MODE_NA_E];
extern PRV_CPSS_DXCH_PORT_INTERFACE_MODE_GET_FUN
        portIfModeGetFuncPtrArray[CPSS_PP_FAMILY_END_DXCH_E-CPSS_PP_FAMILY_START_DXCH_E-1];

extern PRV_CPSS_DXCH_PORT_SPEED_SET_FUN
        portSpeedSetFuncPtrArray[CPSS_PP_FAMILY_END_DXCH_E-CPSS_PP_FAMILY_START_DXCH_E-1]
                                                                        [PRV_CPSS_XG_PORT_OPTIONS_MAX_E];
extern PRV_CPSS_DXCH_PORT_SPEED_GET_FUN
        portSpeedGetFuncPtrArray[CPSS_PP_FAMILY_END_DXCH_E-CPSS_PP_FAMILY_START_DXCH_E-1];

extern PRV_CPSS_DXCH_PORT_SERDES_POWER_STATUS_SET_FUN
        portSerdesPowerStatusSetFuncPtrArray[CPSS_PP_FAMILY_END_DXCH_E-CPSS_PP_FAMILY_START_DXCH_E-1];

/* length of system tick in millisecond */
static GT_U32 tickTimeMsec;

extern GT_BOOL multiLaneConfigSupport(GT_BOOL enable);

#if ((defined GT_PCI) || (defined GT_PEX))
    #define SUPPORT_SIM_CONVERT
#endif/*((defined GT_PCI) || (defined GT_PEX))*/

#ifdef ASIC_SIMULATION
extern GT_STATUS snetGmPortInfoGet
(
    IN  GT_U8    deviceNumber,
    IN  GT_U8    portNumber,
    OUT GT_BOOL  *isLinkUpPtr,
    OUT GT_BOOL  *isLinkChangedPtr
);

/* convert CPSS devNum, coreId to simulation deviceId */
extern GT_STATUS prvCpssDrvDevNumToSimDevIdConvert
(
    IN  GT_U8       devNum,
    IN  GT_U32      portGroupId,
    OUT GT_U32      *simDeviceIdPtr
);
extern GT_STATUS skernelFatherDeviceIdFromSonDeviceIdGet
(
    IN  GT_U32                sonDeviceId,
    OUT GT_U32               *fatherDeviceIdPtr
);

/*******************************************************************************
* getSimDevIdFromSwDevNum
*
* DESCRIPTION:
*        simulation function that converts the cpss SW number to simulation deviceId
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       swDevNum - the (cpss) SW device number
*       portGroupId - the port groupId
* OUTPUTS:
*        simDeviceIdPtr - (pointer to) the simulation device number
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  - on NULL ptr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS   getSimDevIdFromSwDevNum
(
    IN GT_U8    swDevNum,
    IN  GT_U32  portGroupId,
    OUT GT_U32  *simDeviceIdPtr
)
{
    GT_STATUS rc;
    /* convert the SW device number to 'Simulation device ID' */
#ifdef SUPPORT_SIM_CONVERT
    rc = prvCpssDrvDevNumToSimDevIdConvert(swDevNum,portGroupId,simDeviceIdPtr);
    if(rc != GT_OK)
    {
        return rc;
    }
#else /*! SUPPORT_SIM_CONVERT*/
    /* do no convert allow SMI/TWSI */
    *simDeviceIdPtr = swDevNum;
#endif /*! SUPPORT_SIM_CONVERT*/

    if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(swDevNum))
    {
        /* in case of multi-core device we still need to get the 'father' that
           represent this device in terms of 'Global ports' */
        rc = skernelFatherDeviceIdFromSonDeviceIdGet((*simDeviceIdPtr),simDeviceIdPtr);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}
#endif/*ASIC_SIMULATION*/

/*******************************************************************************
* prvCpssDxChPortMacConfigurationClear
*
* DESCRIPTION:
*        Clear array of registers data
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       regDataArray - "clean" array of register's data
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  - on NULL ptr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortMacConfigurationClear
(
    INOUT PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   *regDataArray
)
{
    PRV_CPSS_PORT_TYPE_ENT  portMacType;

    CPSS_NULL_PTR_CHECK_MAC(regDataArray);

    for(portMacType = PRV_CPSS_PORT_GE_E; portMacType <
                                            PRV_CPSS_PORT_NOT_APPLICABLE_E;
                                                                portMacType++)
    {
        regDataArray[portMacType].regAddr = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortMacConfiguration
*
* DESCRIPTION:
*        Write value to register field and duplicate it to other members of SW
*           combo if needed
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       regDataArray - array of register's address/offset/field lenght/value to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  - on NULL ptr
*       GT_HW_ERROR - if write failed
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortMacConfiguration
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  const PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   *regDataArray
)
{
    GT_STATUS               rc;          /* return code */
    GT_U32                  portGroupId; /* port group id */
    PRV_CPSS_PORT_TYPE_ENT  portMacType; /* MAC unit of port */
    GT_PHYSICAL_PORT_NUM    portMacNum;  /* MAC number of given physical port */
    GT_U32                  comboRegAddr;/* address of not active combo port */
    GT_U32                  i;          /* iterator */
    GT_U32                  localPort;

    CPSS_NULL_PTR_CHECK_MAC(regDataArray);

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);

    for(portMacType = PRV_CPSS_PORT_GE_E; portMacType < PRV_CPSS_PORT_NOT_APPLICABLE_E; portMacType++)
    {
        if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_FALSE)
        {
            if((portNum == CPSS_CPU_PORT_NUM_CNS) && (portMacType >= PRV_CPSS_PORT_XG_E))
                continue;
        }

        if(regDataArray[portMacType].regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                                                    regDataArray[portMacType].regAddr,
                                                    regDataArray[portMacType].fieldOffset,
                                                    regDataArray[portMacType].fieldLength,
                                                    regDataArray[portMacType].fieldData) != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }
        }
    }

    if((CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
        && (portNum != CPSS_CPU_PORT_NUM_CNS))
    {
        GT_BOOL                 enable;
        CPSS_DXCH_PORT_COMBO_PARAMS_STC comboParams;

        rc = cpssDxChPortComboModeEnableGet(devNum, portNum, &enable, &comboParams);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(enable)
        {
            localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portMacNum);
            for(i = 0; i < CPSS_DXCH_PORT_MAX_MACS_IN_COMBO_CNS; i++)
            {
                if(CPSS_DXCH_PORT_COMBO_NA_MAC_CNS == comboParams.macArray[i].macNum)
                    break;

                if((localPort == comboParams.macArray[i].macNum) &&
                    (portGroupId == comboParams.macArray[i].macPortGroupNum))
                {
                    continue;
                }

                for(portMacType = PRV_CPSS_PORT_GE_E; portMacType < PRV_CPSS_PORT_NOT_APPLICABLE_E; portMacType++)
                {
                    if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_FALSE)
                    {
                        if((portNum == CPSS_CPU_PORT_NUM_CNS) && (portMacType >= PRV_CPSS_PORT_XG_E))
                            continue;
                    }

                    if(regDataArray[portMacType].regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
                    {
                        comboRegAddr = (regDataArray[portMacType].regAddr & 0xFFFF0FFF)
                                        + comboParams.macArray[i].macNum * 0x1000;
                        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                                                                comboRegAddr,
                                                                regDataArray[portMacType].fieldOffset,
                                                                regDataArray[portMacType].fieldLength,
                                                                regDataArray[portMacType].fieldData) != GT_OK)
                        {
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
                        }
                    }

                }/* for(portMacType */

            }/* for(i = 0; */

        }/* if combo port */

    }/* if lion2 */

    return GT_OK;
}

#define PRV_CPSS_DXCH_PORT_MAC_SA_LSB_FLD_LEN_CNS   8
#define PRV_CPSS_DXCH_PORT_MAC_SA_LSB_FLD_OFFSET_GE_CNS   7
#define PRV_CPSS_DXCH_PORT_MAC_SA_LSB_FLD_OFFSET_XG_CNS   0
/*******************************************************************************
* internal_cpssDxChPortMacSaLsbSet
*
* DESCRIPTION:
*       Set the least significant byte of the MAC SA of specified port on
*       specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       macSaLsb    - The ls byte of the MAC SA
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
*       The upper 40 bits of the MAC Address are the same for all ports in the
*       device and the lower eight bits are unique per port. The upper 40 bits
*       are configured by cpssDxChPortMacSaBaseSet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortMacSaLsbSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM    portNum,
    IN GT_U8    macSaLsb
)
{
    GT_STATUS       rc;              /* return code */
    GT_U32          regAddr;         /* register address */
    GT_U32          value;           /* value to write into the register */
    GT_U32          portMacNum;      /* MAC number */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    value = macSaLsb;

    if((PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
    {
        PRV_CPSS_DXCH_PORT_FCA_REGS_ADDR_STC *fcaStcPtr;

        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
        {
            fcaStcPtr = (portMacNum == CPSS_CPU_PORT_NUM_CNS) ?
                 &PRV_DXCH_REG_UNIT_GOP_CPU_FCA_MAC(devNum) :
                 &PRV_DXCH_REG_UNIT_GOP_FCA_MAC(devNum, portMacNum);

            regAddr = fcaStcPtr->MACSA0To15;
        }
        else
        {
            regAddr = PRV_DXCH_REG1_UNIT_GOP_FCA_MAC(devNum, portMacNum).MACSA0To15;
        }

        rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 8, value);
        if(GT_OK != rc)
        {
            return rc;
        }
    }

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_GE_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_GE_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = PRV_CPSS_DXCH_PORT_MAC_SA_LSB_FLD_LEN_CNS;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = PRV_CPSS_DXCH_PORT_MAC_SA_LSB_FLD_OFFSET_GE_CNS;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldLength = PRV_CPSS_DXCH_PORT_MAC_SA_LSB_FLD_LEN_CNS;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldOffset = PRV_CPSS_DXCH_PORT_MAC_SA_LSB_FLD_OFFSET_XG_CNS;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XLG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XLG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldLength = PRV_CPSS_DXCH_PORT_MAC_SA_LSB_FLD_LEN_CNS;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldOffset = PRV_CPSS_DXCH_PORT_MAC_SA_LSB_FLD_OFFSET_XG_CNS;
    }

    rc = prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);

    return rc;
}

/*******************************************************************************
* cpssDxChPortMacSaLsbSet
*
* DESCRIPTION:
*       Set the least significant byte of the MAC SA of specified port on
*       specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       macSaLsb    - The ls byte of the MAC SA
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
*       The upper 40 bits of the MAC Address are the same for all ports in the
*       device and the lower eight bits are unique per port. The upper 40 bits
*       are configured by cpssDxChPortMacSaBaseSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacSaLsbSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM    portNum,
    IN GT_U8    macSaLsb
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMacSaLsbSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, macSaLsb));

    rc = internal_cpssDxChPortMacSaLsbSet(devNum, portNum, macSaLsb);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, macSaLsb));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortMacSaLsbGet
*
* DESCRIPTION:
*       Get the least significant byte of the MAC SA of specified port on
*       specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*
* OUTPUTS:
*       macSaLsbPtr - (pointer to) The ls byte of the MAC SA
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The upper 40 bits of the MAC Address are the same for all ports in the
*       device and the lower eight bits are unique per port.The upper 40 bits
*       are configured by cpssDxChPortMacSaBaseSet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortMacSaLsbGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_U8    *macSaLsbPtr
)
{
    GT_U32          regAddr;         /* register address */
    GT_U32          value;
    GT_U32          fieldOffset;     /* start to write register at this bit */
    GT_STATUS       rc = GT_OK;
    PRV_CPSS_PORT_TYPE_ENT portMacType;
    GT_U32          portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(macSaLsbPtr);

   /* cpssDxChPortMacSaLsbSet updates all available MAC's - here enough to read */
    /* one of them */
    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum);

    if (portMacType >= PRV_CPSS_PORT_XG_E)
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portMacNum,portMacType,&regAddr);
        fieldOffset = PRV_CPSS_DXCH_PORT_MAC_SA_LSB_FLD_OFFSET_XG_CNS;
    }
    else
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portMacNum,portMacType,&regAddr);
        fieldOffset = PRV_CPSS_DXCH_PORT_MAC_SA_LSB_FLD_OFFSET_GE_CNS;
    }
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,
            PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
                                                                        portMacNum),
            regAddr, fieldOffset, PRV_CPSS_DXCH_PORT_MAC_SA_LSB_FLD_LEN_CNS, &value);
    if(rc != GT_OK)
        return rc;

    *macSaLsbPtr = (GT_U8)value;

    return rc;
}

/*******************************************************************************
* cpssDxChPortMacSaLsbGet
*
* DESCRIPTION:
*       Get the least significant byte of the MAC SA of specified port on
*       specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*
* OUTPUTS:
*       macSaLsbPtr - (pointer to) The ls byte of the MAC SA
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The upper 40 bits of the MAC Address are the same for all ports in the
*       device and the lower eight bits are unique per port.The upper 40 bits
*       are configured by cpssDxChPortMacSaBaseSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacSaLsbGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_U8    *macSaLsbPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMacSaLsbGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, macSaLsbPtr));

    rc = internal_cpssDxChPortMacSaLsbGet(devNum, portNum, macSaLsbPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, macSaLsbPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortEnableSet
*
* DESCRIPTION:
*       Enable/disable a specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number, CPU port number
*       enable   - GT_TRUE:  enable port,
*                  GT_FALSE: disable port.
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
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    IN  GT_BOOL   enable
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      value;      /* value to write */
    GT_STATUS   rc;         /* return code */
    GT_U32      portMacNum; /* MAC number */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];
    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum))
    {
        if(enable)
        {/* Unreset PTP before port state is enabled */
            rc = prvCpssDxChPortPtpReset(devNum, portNum, GT_FALSE, GT_FALSE);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    value = BOOL2BIT_MAC(enable);

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_GE_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_GE_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = 0;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = 1;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldOffset = 0;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldLength = 1;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XLG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XLG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldOffset = 0;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldLength = 1;
    }

    rc = prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
    if(rc != GT_OK)
    {
        return rc;
    }

    if((!enable) && (PRV_CPSS_SIP_5_CHECK_MAC(devNum)))
    {/* ensure last packets passed pipe */
        GT_U16      portTxqDescNum; /* number of not treated TXQ descriptors */
        GT_U32      timeout;        /* resources free timeout counter */
        GT_U32      portTxdmaNum;   /* TXDMA allocated for port */
        GT_U32      scdmaTxFifoCounters;    /* value of both Header(bits 10-19)
                                    and Payload(bits 0-9) scdma TxFifo Counters */
        GT_U8       tcQueue;
        GT_BOOL     queueEn;
        GT_U32      portGroupId;/* group number of port (in multi-port-group device)*/
#ifndef ASIC_SIMULATION
        GT_U32      fifoThresholdCounters = 0;
        GT_U32      txdmaPortCreditsCounters = 0;
#endif


        rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum, portNum, &portMapShadowPtr);
        if(rc != GT_OK)
        {
            return rc;
        }

        portGroupId =
            PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
                                                                        portMacNum);
        for (tcQueue = 0; tcQueue < CPSS_TC_RANGE_CNS; tcQueue++)
        {
            rc = cpssDxChPortTxQueueTxEnableGet(devNum, portNum, tcQueue,
                                                &queueEn);
            if(rc != GT_OK)
            {
                return rc;
            }
            if (!queueEn)
            {/* if at least one of queues disabled, exit without wait,
                we suppose that user debugging system and descriptors left in
                pipe no matter */
                return GT_OK;
            }
        }

        if (GT_FALSE == portMapShadowPtr->portMap.trafficManagerEn) /* if port passed through TM, dont't check */
        {
            for(timeout = 100; timeout > 0; timeout--)
            {
                rc = cpssDxChPortTxDescNumberGet(devNum, portNum, &portTxqDescNum);
                if(rc != GT_OK)
                {
                    return rc;
                }

                if(0 == portTxqDescNum)
                    break;
                else
                    cpssOsTimerWkAfter(10);
            }

            if(0 == timeout)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_TIMEOUT, LOG_ERROR_NO_MSG);
            }
        }

        rc = prvCpssDxChPortPhysicalPortMapCheckAndConvert(devNum, portNum,
                                                           PRV_CPSS_DXCH_PORT_TYPE_TxDMA_E,
                                                           &portTxdmaNum);
        if(GT_OK != rc)
        {
            return rc;
        }
#ifndef ASIC_SIMULATION
        if(PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
        {
            if(      (PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portMacNum) != CPSS_PORT_INTERFACE_MODE_NA_E)
                  && (portMapShadowPtr->portMap.trafficManagerEn == GT_FALSE)
              )
            {
                rc = prvCpssDxChCaelumPortRegFieldGet(devNum, portNum,
                                                      PRV_CAELUM_REG_FIELDS_TXDMA_DESC_CREDITS_SCDMA_E,
                                                      &fifoThresholdCounters);
                if(rc != GT_OK)
                {
                    return rc;
                }

                for(timeout = 100; timeout > 0; timeout--)
                {
                    rc = prvCpssDxChCaelumPortRegFieldGet(devNum, portNum,
                                                          PRV_CAELUM_REG_FIELDS_TXQ_DQ_TXDMA_PORT_CREDIT_COUNTER_E,
                                                          &txdmaPortCreditsCounters);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }

                    if(fifoThresholdCounters == txdmaPortCreditsCounters)
                    {
                        break;
                    }
                    else
                    {
                        cpssOsTimerWkAfter(10);
                    }
                }

                if(0 == timeout)
                {
                    cpssOsPrintf("prvCpssDxChPortEnableSet[port=%d]: TIMEOUT error -> FIFOsThresholdsConfigsSCDMAReg1(%d) != txdmaPortCreditCounter(%d)\r\n", portNum, fifoThresholdCounters, txdmaPortCreditsCounters);
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_TIMEOUT, LOG_ERROR_NO_MSG);
                }
            }
        }
#endif
        if (GT_FALSE == portMapShadowPtr->portMap.trafficManagerEn) /* if port passed through TM, dont't check scdmaTxFifoCounters */
        {
            regAddr = PRV_DXCH_REG1_UNIT_TXDMA_MAC(devNum).txDMADebug.
                                    informativeDebug.SCDMAStatusReg1[portTxdmaNum];
            for(timeout = 100; timeout > 0; timeout--)
            {
                rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, 0,
                                                     20, &scdmaTxFifoCounters);
                if(rc != GT_OK)
                {
                    return rc;
                }
    #ifdef GM_USED
                /* the registers not exists in GM ... and return '0xbadad' */
                scdmaTxFifoCounters = 0;
    #endif /*GM_USED*/
                if(0 == scdmaTxFifoCounters)
                    break;
                else
                    cpssOsTimerWkAfter(10);
            }

            if(0 == timeout)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_TIMEOUT, LOG_ERROR_NO_MSG);
            }
        }
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum))
    {
        if(!enable)
        {/* Reset PTP unit after port state is disabled */
            rc = prvCpssDxChPortPtpReset(devNum, portNum, GT_TRUE, GT_TRUE);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortEnableSet
*
* DESCRIPTION:
*       Enable/disable a specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number, CPU port number
*       enable   - GT_TRUE:  enable port,
*                  GT_FALSE: disable port.
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
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    IN  GT_BOOL   enable
)
{
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;
    GT_U32 portMacNum; /* MAC number */

    GT_BOOL targetEnable = enable;

    GT_U8       tcQueue;
    GT_BOOL     tcQueueEnable;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portMacNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacPortEnableSetFunc(devNum,portMacNum,
                                  targetEnable, CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,&targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* switch MAC port enable configuration */
    if (doPpMacConfig == GT_TRUE)
    {
        /* Don't apply WA on CPU port in SDMA mode */
        if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,PRV_CPSS_DXCH_XCAT_GE_PORT_DISABLE_WA_E) &&
           ((portNum != CPSS_CPU_PORT_NUM_CNS) || ((portNum == CPSS_CPU_PORT_NUM_CNS) &&
            (PRV_CPSS_PP_MAC(devNum)->cpuPortMode != CPSS_NET_CPU_PORT_MODE_SDMA_E)) ||
            ((portNum == CPSS_CPU_PORT_NUM_CNS) && PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))))
        {
            for(tcQueue = 0; tcQueue < CPSS_TC_RANGE_CNS; tcQueue++)
            {
                if(GT_FALSE == targetEnable)
                {
                    tcQueueEnable = GT_FALSE;
                }
                else
                {
                    /* restore queueing of packets on all TC queues */
                    tcQueueEnable = CPSS_PORTS_BMP_IS_PORT_SET_MAC((&(PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                            info_PRV_CPSS_DXCH_XCAT_GE_PORT_DISABLE_WA_E.portTxQueuesBmpPtr[tcQueue])),portNum);
                    tcQueueEnable = BIT2BOOL_MAC(tcQueueEnable);
                }

                if((rc = prvCpssDxChPortTxQueueingEnableSet(devNum,portNum,tcQueue,tcQueueEnable)) != GT_OK)
                    return rc;
            }
        }

        rc = prvCpssDxChPortEnableSet(devNum,portNum,targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }

        if (PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                        info_PRV_CPSS_DXCH_XCAT_GE_PORT_UNIDIRECT_WA_E.enabled == GT_TRUE)
        {
            CPSS_PORTS_BMP_PORT_ENABLE_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
               info_PRV_CPSS_DXCH_XCAT_GE_PORT_UNIDIRECT_WA_E.portEnableBmpPtr,portMacNum,targetEnable);
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacPortEnableSetFunc(devNum,portMacNum,
                                  targetEnable,
                                  CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,&targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortEnableSet
*
* DESCRIPTION:
*       Enable/disable a specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number, CPU port number
*       enable   - GT_TRUE:  enable port,
*                  GT_FALSE: disable port.
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
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*******************************************************************************/
GT_STATUS cpssDxChPortEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    IN  GT_BOOL   enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChPortEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortEnableGet
*
* DESCRIPTION:
*       Get the Enable/Disable status of a specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*
* OUTPUTS:
*       statePtr - Pointer to the Get Enable/disable state of the port.
*                  GT_TRUE for enabled port, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortEnableGet
(
    IN   GT_U8     devNum,
    IN   GT_PHYSICAL_PORT_NUM     portNum,
    OUT  GT_BOOL   *statePtr
)
{
    GT_U32 regAddr;
    GT_U32 value;
    GT_U32 portMacNum; /* MAC number */

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                    info_PRV_CPSS_DXCH_XCAT_GE_PORT_UNIDIRECT_WA_E.enabled == GT_TRUE)
    {
        *statePtr = CPSS_PORTS_BMP_IS_PORT_SET_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                info_PRV_CPSS_DXCH_XCAT_GE_PORT_UNIDIRECT_WA_E.portEnableBmpPtr,portMacNum);
        return GT_OK;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum, portMacNum,
                        PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum), &regAddr);
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    /* store value of port state */
    if (prvCpssDrvHwPpPortGroupGetRegField(devNum,
            PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
                                                                        portMacNum),
                regAddr, 0, 1, &value) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    *statePtr = (value == 1) ? GT_TRUE : GT_FALSE;
    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortEnableGet
*
* DESCRIPTION:
*       Get the Enable/Disable status of a specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*
* OUTPUTS:
*       statePtr - Pointer to the Get Enable/disable state of the port.
*                  GT_TRUE for enabled port, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortEnableGet
(
    IN   GT_U8     devNum,
    IN   GT_PHYSICAL_PORT_NUM portNum,
    OUT  GT_BOOL   *statePtr
)
{
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;
    GT_U32 portMacNum; /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(statePtr);

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portMacNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacPortEnableGetFunc(devNum,portMacNum,
                                  statePtr, CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,statePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* get port enable status from switch MAC */
    if (doPpMacConfig == GT_TRUE)
    {
        rc = prvCpssDxChPortEnableGet(devNum,portNum,statePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacPortEnableGetFunc(devNum,portMacNum,
                                  statePtr,
                                  CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,statePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortEnableGet
*
* DESCRIPTION:
*       Get the Enable/Disable status of a specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*
* OUTPUTS:
*       statePtr - Pointer to the Get Enable/disable state of the port.
*                  GT_TRUE for enabled port, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortEnableGet
(
    IN   GT_U8     devNum,
    IN   GT_PHYSICAL_PORT_NUM portNum,
    OUT  GT_BOOL   *statePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, statePtr));

    rc = internal_cpssDxChPortEnableGet(devNum, portNum, statePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, statePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortDuplexModeSet
*
* DESCRIPTION:
*       Set the port to enable and the port mode to half- or full-duplex mode.
*       When port is disabled it does not transmit pending packets and
*       drops packets received from the line.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number or CPU port
*       dMode    - 0 = The port operates in half-duplex mode.
*                  1 = The port operates in full-duplex mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or dMode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported duplex mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The <SetFullDx> bit is meaningless when <ANDuplex> is set.
*       For CPU port, this field must be set to reflect this port Duplex mode.
*
*******************************************************************************/
#define PRV_CPSS_DXCH_PORT_MAC_DUPLEX_FLD_LEN_CNS   1
#define PRV_CPSS_DXCH_PORT_MAC_DUPLEX_FLD_OFFSET_GE_CNS   12
static GT_STATUS prvCpssDxChPortDuplexModeSet
(
    IN  GT_U8                 devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    IN  CPSS_PORT_DUPLEX_ENT  dMode
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32 portState = 1;       /* current port state (enable/disable) */
    GT_U32 duplexModeRegAddr; /* config. reg. address */
    PRV_CPSS_PORT_TYPE_ENT portMacType; /* mac type to use */
    GT_U32 portMacNum; /* MAC number */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    if (dMode != CPSS_PORT_FULL_DUPLEX_E &&
        dMode != CPSS_PORT_HALF_DUPLEX_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* in devices of Lion family half-duplex not supported in any port mode/speed */
    if (dMode == CPSS_PORT_HALF_DUPLEX_E)
    {
        if ( (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)  ||
             (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
             (PRV_CPSS_DXCH_BOBCAT2_A0_CHECK_MAC(devNum)) /* BC2 A0 */
           )
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }
    }

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum);
    if(portMacType >= PRV_CPSS_PORT_XG_E)
    {
        if((GT_TRUE == PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported)
            || (portNum != CPSS_CPU_PORT_NUM_CNS))
        {/* check if auto-neg. supported for current mode/port*/
            if (dMode == CPSS_PORT_HALF_DUPLEX_E)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }
        }
    }

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portMacNum,&duplexModeRegAddr);
    if(duplexModeRegAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) <= PRV_CPSS_PORT_GE_E)
    {/* disable port if we need */
        rc = prvCpssDxChPortStateDisableSet(devNum,portNum,&portState);
        if(rc != GT_OK)
            return rc;
    }

    regDataArray[PRV_CPSS_PORT_GE_E].regAddr = duplexModeRegAddr;
    regDataArray[PRV_CPSS_PORT_GE_E].fieldData = (dMode == CPSS_PORT_FULL_DUPLEX_E) ? 1 : 0;
    regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = PRV_CPSS_DXCH_PORT_MAC_DUPLEX_FLD_OFFSET_GE_CNS;
    regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = PRV_CPSS_DXCH_PORT_MAC_DUPLEX_FLD_LEN_CNS;

    rc = prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) <= PRV_CPSS_PORT_GE_E)
    {/* restore port enable state */
        rc = prvCpssDxChPortStateEnableSet(devNum,portNum, portState);
        if(rc != GT_OK)
            return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortDuplexModeSet
*
* DESCRIPTION:
*       Set the port to enable and the port mode to half- or full-duplex mode.
*       When port is disabled it does not transmit pending packets and
*       drops packets received from the line.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number or CPU port
*       dMode    - 0 = The port operates in half-duplex mode.
*                  1 = The port operates in full-duplex mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or dMode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported duplex mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The <SetFullDx> bit is meaningless when <ANDuplex> is set.
*       For CPU port, this field must be set to reflect this port Duplex mode.
*
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortDuplexModeSet
(
    IN  GT_U8                 devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    IN  CPSS_PORT_DUPLEX_ENT  dMode
)
{
    CPSS_PORT_DUPLEX_ENT  targetdMode = dMode;   /* mode value - set by callback */
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */
    GT_U32 portMacNum; /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    if (dMode != CPSS_PORT_FULL_DUPLEX_E &&
        dMode != CPSS_PORT_HALF_DUPLEX_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.hitlessWriteMethodEnable)
    {
        CPSS_PORT_DUPLEX_ENT   currentState;

        rc = cpssDxChPortDuplexModeGet(devNum, portNum, &currentState);
        if(rc != GT_OK)
        {
            return rc;
        }
        if(dMode == currentState)
        {
            return GT_OK;
        }
    }

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portMacNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacDuplexSetFunc(devNum,portMacNum,
                                  targetdMode, CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,&targetdMode);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* switch MAC configuration */
    if (doPpMacConfig == GT_TRUE)
    {
        rc = prvCpssDxChPortDuplexModeSet(devNum,portNum,targetdMode);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run */
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacDuplexSetFunc(devNum,portNum,
                                  targetdMode,
                                  CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,&targetdMode);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortDuplexModeSet
*
* DESCRIPTION:
*       Set the port to enable and the port mode to half- or full-duplex mode.
*       When port is disabled it does not transmit pending packets and
*       drops packets received from the line.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number or CPU port
*       dMode    - 0 = The port operates in half-duplex mode.
*                  1 = The port operates in full-duplex mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or dMode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported duplex mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The <SetFullDx> bit is meaningless when <ANDuplex> is set.
*       For CPU port, this field must be set to reflect this port Duplex mode.
*
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortDuplexModeSet
(
    IN  GT_U8                 devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    IN  CPSS_PORT_DUPLEX_ENT  dMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortDuplexModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, dMode));

    rc = internal_cpssDxChPortDuplexModeSet(devNum, portNum, dMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, dMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortDuplexModeGet
*
* DESCRIPTION:
*       Gets duplex mode for specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       dModePtr - duplex mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortDuplexModeGet
(
    IN  GT_U8                 devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT CPSS_PORT_DUPLEX_ENT  *dModePtr
)
{
    GT_U32 value;
    GT_U32 offset;          /* bit number inside register       */
    GT_U32 duplexModeRegAddr;
    GT_STATUS rc;
    CPSS_PORT_SPEED_ENT speed;
    GT_U32 portMacNum; /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(dModePtr);

     /* can not be changed for 1G and above */
    if((portNum != CPSS_CPU_PORT_NUM_CNS) || ((PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_TRUE)))
    {
        if((rc = cpssDxChPortSpeedGet(devNum, portNum, &speed)) != GT_OK)
            return rc;

        if(speed > CPSS_PORT_SPEED_1000_E)
        {
            *dModePtr = CPSS_PORT_FULL_DUPLEX_E;
            return GT_OK;
        }
    }

    PRV_CPSS_DXCH_PORT_STATUS_CTRL_REG_MAC(devNum,portMacNum,&duplexModeRegAddr);
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == duplexModeRegAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    offset = 3;

    if (prvCpssDrvHwPpPortGroupGetRegField(devNum,PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum),
                                  duplexModeRegAddr,
                                  offset,
                                  1,
                                  &value) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);

    *dModePtr = (value == 1) ? CPSS_PORT_FULL_DUPLEX_E : CPSS_PORT_HALF_DUPLEX_E;

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortDuplexModeGet
*
* DESCRIPTION:
*       Gets duplex mode for specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       dModePtr - duplex mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortDuplexModeGet
(
    IN  GT_U8                 devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT CPSS_PORT_DUPLEX_ENT  *dModePtr
)
{
    GT_STATUS rc = GT_OK;
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */
    GT_U32 portMacNum; /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(dModePtr);

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portMacNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacDuplexGetFunc(devNum,portMacNum,
                                  dModePtr, CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,dModePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* get value from switch MAC */
    if (doPpMacConfig == GT_TRUE)
    {
        rc = prvCpssDxChPortDuplexModeGet(devNum,portNum,dModePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacDuplexGetFunc(devNum,portMacNum,
                                  dModePtr,
                                  CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,dModePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortDuplexModeGet
*
* DESCRIPTION:
*       Gets duplex mode for specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       dModePtr - duplex mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*******************************************************************************/
GT_STATUS cpssDxChPortDuplexModeGet
(
    IN  GT_U8                 devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT CPSS_PORT_DUPLEX_ENT  *dModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortDuplexModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, dModePtr));

    rc = internal_cpssDxChPortDuplexModeGet(devNum, portNum, dModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, dModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortDuplexAutoNegEnableSet
*
*
* DESCRIPTION:
*       Enable/disable an Auto-Negotiation for duplex mode on specified port on
*       specified device.
*       When duplex Auto-Negotiation is enabled, the port's duplex mode is
*       resolved via Auto-Negotiation. The Auto-Negotiation advertised duplex
*       mode is full-duplex.
*       When duplex Auto-Negotiation is disabled, the port's duplex mode is set
*       via cpssDxChPortDuplexModeSet
*       The function does not configure the PHY connected to the port. It
*       configures only the MAC layer of the port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       state    - GT_TRUE for enable Auto-Negotiation for duplex mode,
*                  GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CPU port not supports the duplex auto negotiation
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortDuplexAutoNegEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL   state
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32 portState = 1;       /* current port state (enable/disable) */
    GT_U32 dupledAutoNegRegAddr; /* address of reg. for duplex auto-neg. config. */
    PRV_CPSS_PORT_TYPE_ENT portMacType;
    GT_U32 portMacNum; /* MAC number */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    /* in devices of Lion family half-duplex not supported in any port mode/speed,
        so disable duplex auto-neg. to prevent unpredictable behaviour */
    if (((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
         (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)) &&
        (state == GT_TRUE))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum);
    if((portMacType >= PRV_CPSS_PORT_XG_E) ||
        ((portNum == CPSS_CPU_PORT_NUM_CNS) &&
            (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_FALSE)))
    {/* check if auto-neg. supported for current mode/port*/
        if (state == GT_TRUE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }
    }

    PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portMacNum,&dupledAutoNegRegAddr);
    if(dupledAutoNegRegAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    if(portMacType <= PRV_CPSS_PORT_GE_E)
    {/* disable port if we need */
        rc = prvCpssDxChPortStateDisableSet(devNum,portNum,&portState);
        if(rc != GT_OK)
            return rc;
    }

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    regDataArray[PRV_CPSS_PORT_GE_E].regAddr = dupledAutoNegRegAddr;
    regDataArray[PRV_CPSS_PORT_GE_E].fieldData = BOOL2BIT_MAC(state);
    regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = 13;
    regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = 1;

    rc = prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(portMacType <= PRV_CPSS_PORT_GE_E)
    {/* restore port enable state */
        rc = prvCpssDxChPortStateEnableSet(devNum,portNum, portState);
        if(rc != GT_OK)
            return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortDuplexAutoNegEnableSet
*
* DESCRIPTION:
*       Enable/disable an Auto-Negotiation for duplex mode on specified port on
*       specified device.
*       When duplex Auto-Negotiation is enabled, the port's duplex mode is
*       resolved via Auto-Negotiation. The Auto-Negotiation advertised duplex
*       mode is full-duplex.
*       When duplex Auto-Negotiation is disabled, the port's duplex mode is set
*       via cpssDxChPortDuplexModeSet
*       The function does not configure the PHY connected to the port. It
*       configures only the MAC layer of the port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       state    - GT_TRUE for enable Auto-Negotiation for duplex mode,
*                  GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.CPU port not supports the duplex auto negotiation
*
*       2.Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortDuplexAutoNegEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL   state
)
{
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;
    GT_BOOL targetPortState = state;
    GT_U32 portMacNum; /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.hitlessWriteMethodEnable)
    {
        GT_BOOL   currentState;

        rc = cpssDxChPortDuplexAutoNegEnableGet(devNum, portNum, &currentState);
        if(rc != GT_OK)
        {
            return rc;
        }
        if(state == currentState)
        {
            return GT_OK;
        }
    }

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portMacNum);

    /* run MACPHY callback  */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacDuplexANSetFunc(devNum,portMacNum,
                                  targetPortState,CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,&targetPortState);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* if required run prvCpssDxChPortDuplexAutoNegEnableSet -
       switch MAC configuration code */
    if (doPpMacConfig == GT_TRUE)
    {
        rc = prvCpssDxChPortDuplexAutoNegEnableSet(devNum,portNum,targetPortState);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

       /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacDuplexANSetFunc(devNum,portMacNum,
                                  targetPortState,CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,&targetPortState);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortDuplexAutoNegEnableSet
*
* DESCRIPTION:
*       Enable/disable an Auto-Negotiation for duplex mode on specified port on
*       specified device.
*       When duplex Auto-Negotiation is enabled, the port's duplex mode is
*       resolved via Auto-Negotiation. The Auto-Negotiation advertised duplex
*       mode is full-duplex.
*       When duplex Auto-Negotiation is disabled, the port's duplex mode is set
*       via cpssDxChPortDuplexModeSet
*       The function does not configure the PHY connected to the port. It
*       configures only the MAC layer of the port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       state    - GT_TRUE for enable Auto-Negotiation for duplex mode,
*                  GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.CPU port not supports the duplex auto negotiation
*
*       2.Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*******************************************************************************/
GT_STATUS cpssDxChPortDuplexAutoNegEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL   state
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortDuplexAutoNegEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, state));

    rc = internal_cpssDxChPortDuplexAutoNegEnableSet(devNum, portNum, state);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, state));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortDuplexAutoNegEnableGet
*
* DESCRIPTION:
*       Get enable/disable status of an Auto-Negotiation for MAC duplex mode
*       per port.
*       When duplex Auto-Negotiation is enabled, the port's duplex mode is
*       resolved via Auto-Negotiation. The Auto-Negotiation advertised duplex
*       mode is full-duplex.
*       When duplex Auto-Negotiation is disabled, the port's duplex mode is set
*       via cpssDxChPortDuplexModeSet
*       The function does not configure the PHY connected to the port. It
*       configures only the MAC layer of the port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
*
* OUTPUTS:
*       statePtr - GT_TRUE for enable Auto-Negotiation for duplex mode,
*                  GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CPU port not supports the duplex auto negotiation
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortDuplexAutoNegEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL    *statePtr
)
{
    GT_U32 value;       /* value of duplex auto-neg. status */
    GT_U32 portGroupId; /* number of port group for mlti-port group dev's */
    GT_U32 dupledAutoNegRegAddr; /* address of duplex auto-neg. config. reg. */
    GT_U32 portMacNum; /* MAC number */

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    /* can not be changed for 10 Gb interface or CPU port */
    if((PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) >= PRV_CPSS_PORT_XG_E)
        || ((portNum == CPSS_CPU_PORT_NUM_CNS)&&(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_FALSE))
    /* in devices of Lion family half-duplex not supported in any port mode/speed */
        || (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E))
    {
        *statePtr = GT_FALSE;
        return GT_OK;
    }

    PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portMacNum,&dupledAutoNegRegAddr);

    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
                                                                             portMacNum);
    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, dupledAutoNegRegAddr,
                                            13, 1, &value) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    *statePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortDuplexAutoNegEnableGet
*
* DESCRIPTION:
*       Get enable/disable status of an Auto-Negotiation for MAC duplex mode
*       per port.
*       When duplex Auto-Negotiation is enabled, the port's duplex mode is
*       resolved via Auto-Negotiation. The Auto-Negotiation advertised duplex
*       mode is full-duplex.
*       When duplex Auto-Negotiation is disabled, the port's duplex mode is set
*       via cpssDxChPortDuplexModeSet
*       The function does not configure the PHY connected to the port. It
*       configures only the MAC layer of the port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
*
* OUTPUTS:
*       statePtr - GT_TRUE for enable Auto-Negotiation for duplex mode,
*                  GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. CPU port not supports the duplex auto negotiation
*
*       2. Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortDuplexAutoNegEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL    *statePtr
)
{
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port callback pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;                       /* do switch mac code indicator */
    GT_U32 portMacNum; /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(statePtr);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    /* Get PHY MAC object ptr */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portMacNum);

    /* run MACPHY callback */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacDuplexANGetFunc(devNum,portMacNum,
                                  statePtr,CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,statePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
           /*
     *  the function returns the switch MAC value if callback not bound or callback set
     *  doPpMacConfig = GT_TRUE; in this case prvCpssDxChPortDuplexAutoNegEnableGet
     *  will run
     */
    if (doPpMacConfig == GT_TRUE)
    {
        rc = prvCpssDxChPortDuplexAutoNegEnableGet(devNum,portNum,statePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* post callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacDuplexANGetFunc(devNum,portMacNum,
                                  statePtr,CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,statePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortDuplexAutoNegEnableGet
*
* DESCRIPTION:
*       Get enable/disable status of an Auto-Negotiation for MAC duplex mode
*       per port.
*       When duplex Auto-Negotiation is enabled, the port's duplex mode is
*       resolved via Auto-Negotiation. The Auto-Negotiation advertised duplex
*       mode is full-duplex.
*       When duplex Auto-Negotiation is disabled, the port's duplex mode is set
*       via cpssDxChPortDuplexModeSet
*       The function does not configure the PHY connected to the port. It
*       configures only the MAC layer of the port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
*
* OUTPUTS:
*       statePtr - GT_TRUE for enable Auto-Negotiation for duplex mode,
*                  GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. CPU port not supports the duplex auto negotiation
*
*       2. Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortDuplexAutoNegEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL    *statePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortDuplexAutoNegEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, statePtr));

    rc = internal_cpssDxChPortDuplexAutoNegEnableGet(devNum, portNum, statePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, statePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortFlowCntrlAutoNegEnableSet
*
* DESCRIPTION:
*       Enable/disable an Auto-Negotiation for Flow Control on
*       specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number
*       state          - GT_TRUE for enable Auto-Negotiation for Flow Control
*                        GT_FALSE otherwise
*       pauseAdvertise - Advertise symmetric flow control support in
*                        Auto-Negotiation. 0 = Disable, 1 = Enable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CPU port not supports the flow control auto negotiation
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortFlowCntrlAutoNegEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL   state,
    IN  GT_BOOL   pauseAdvertise
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32 stateValue;      /* value to write into the register */
    GT_U32 pauseAdvValue;   /* value to write into the register */
    GT_U32 portState = 1;       /* current port state (enable/disable) */
    GT_U32 fcAutoNegRegAddr;/* config. reg. address */
    PRV_CPSS_PORT_TYPE_ENT portMacType;
    GT_U32          portMacNum;      /* MAC number */
    GT_U32 fcaRegAddr = PRV_CPSS_SW_PTR_ENTRY_UNUSED;         /* register address */
    GT_U32 fcaValue = 0;    /* value to write into the FCA register */

    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum);
    if((portMacType >= PRV_CPSS_PORT_XG_E) ||
        ((portNum == CPSS_CPU_PORT_NUM_CNS) &&
        (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_FALSE)))
    {/* check if auto-neg. supported for current mode/port*/
        if ((state == GT_TRUE) || (pauseAdvertise == GT_TRUE))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }
    }

    PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portMacNum,&fcAutoNegRegAddr);
    if(fcAutoNegRegAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    if(portMacType <= PRV_CPSS_PORT_GE_E)
    {/* disable port if we need */
        rc = prvCpssDxChPortStateDisableSet(devNum,portNum,&portState);
        if(rc != GT_OK)
            return rc;
    }

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    stateValue = BOOL2BIT_MAC(state);
    pauseAdvValue = BOOL2BIT_MAC(pauseAdvertise);
    if(fcAutoNegRegAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_GE_E].regAddr = fcAutoNegRegAddr;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldData = stateValue;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = 11;              /* AnFCEn Enables Auto-Negotiation for Flow */
        regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = 1;
    }

    rc = prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(fcAutoNegRegAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_GE_E].regAddr = fcAutoNegRegAddr;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldData = pauseAdvValue;           /* PauseAdv This is the Flow Control advertise  */
        regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = 9;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = 1;
    }

    rc = prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(portMacType <= PRV_CPSS_PORT_GE_E)
    {
        /* restore port enable state */
        rc = prvCpssDxChPortStateEnableSet(devNum,portNum, portState);
        if(rc != GT_OK)
            return rc;
    }

    if((PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
    {
        PRV_CPSS_DXCH_PORT_FCA_REGS_ADDR_STC *fcaStcPtr;

        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
        {
            fcaStcPtr = (portMacNum == CPSS_CPU_PORT_NUM_CNS) ?
                 &PRV_DXCH_REG_UNIT_GOP_CPU_FCA_MAC(devNum) :
                 &PRV_DXCH_REG_UNIT_GOP_FCA_MAC(devNum, portMacNum);

            fcaRegAddr = fcaStcPtr->FCACtrl;
        }
        else
        {
            fcaRegAddr = PRV_DXCH_REG1_UNIT_GOP_FCA_MAC(devNum, portMacNum).FCACtrl;
        }
        /* <FCA <enable/disable> bit configuration */
        if((GT_TRUE == state) && (GT_TRUE == pauseAdvertise))
        {
            fcaValue = 0;
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum),
                    fcaRegAddr, 1, 1, fcaValue) != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortFlowCntrlAutoNegEnableSet
*
* DESCRIPTION:
*       Enable/disable an Auto-Negotiation for Flow Control on
*       specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number
*       state          - GT_TRUE for enable Auto-Negotiation for Flow Control
*                        GT_FALSE otherwise
*       pauseAdvertise - Advertise symmetric flow control support in
*                        Auto-Negotiation. 0 = Disable, 1 = Enable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.CPU port not supports the flow control auto negotiation
*
*       2. Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortFlowCntrlAutoNegEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL   state,
    IN  GT_BOOL   pauseAdvertise
)
{
    GT_BOOL   targetState = state;
    GT_BOOL   targetPauseAdvertise = pauseAdvertise;

    CPSS_MACDRV_OBJ_STC * portMacObjPtr;      /* port PHYMAC object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;          /* do switch mac code indicator */
    GT_U32          portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);


    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portMacNum);

    /* run MACPHY callback */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacFlowCntlANSetFunc(devNum,portMacNum,
                                     targetState,targetPauseAdvertise,
                                     CPSS_MACDRV_STAGE_PRE_E,
                                     &doPpMacConfig,&targetState,
                                     &targetPauseAdvertise);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

         /* run prvCpssDxChPortDuplexAutoNegEnableGet - switch MAC configuration code */
    if (doPpMacConfig == GT_TRUE)
    {
        rc = prvCpssDxChPortFlowCntrlAutoNegEnableSet(devNum,portNum,targetState,
                                     targetPauseAdvertise);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacFlowCntlANSetFunc(devNum,portMacNum,
                                  state,pauseAdvertise,
                                  CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,&targetState,
                                  &targetPauseAdvertise);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortFlowCntrlAutoNegEnableSet
*
* DESCRIPTION:
*       Enable/disable an Auto-Negotiation for Flow Control on
*       specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number
*       state          - GT_TRUE for enable Auto-Negotiation for Flow Control
*                        GT_FALSE otherwise
*       pauseAdvertise - Advertise symmetric flow control support in
*                        Auto-Negotiation. 0 = Disable, 1 = Enable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.CPU port not supports the flow control auto negotiation
*
*       2. Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*******************************************************************************/
GT_STATUS cpssDxChPortFlowCntrlAutoNegEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL   state,
    IN  GT_BOOL   pauseAdvertise
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortFlowCntrlAutoNegEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, state, pauseAdvertise));

    rc = internal_cpssDxChPortFlowCntrlAutoNegEnableSet(devNum, portNum, state, pauseAdvertise);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, state, pauseAdvertise));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortFlowCntrlAutoNegEnableGet
*
* DESCRIPTION:
*       Get Auto-Negotiation enable/disable state for Flow Control per port
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number
*
* OUTPUTS:
*       statePtr       -   GT_TRUE for enable Auto-Negotiation for Flow Control
*                          GT_FALSE otherwise
*       pauseAdvertisePtr - Advertise symmetric flow control support in
*                           Auto-Negotiation. 0 = Disable, 1 = Enable.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CPU port not supports the flow control auto negotiation
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortFlowCntrlAutoNegEnableGet
(
    IN   GT_U8       devNum,
    IN   GT_PHYSICAL_PORT_NUM   portNum,
    OUT  GT_BOOL     *statePtr,
    OUT  GT_BOOL     *pauseAdvertisePtr
)
{
    GT_U32 value;   /* current status */
    GT_U32 fcAutoNegRegAddr;    /* config. reg. address */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32          portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
                                                                            portMacNum);

    if((PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) >= PRV_CPSS_PORT_XG_E) ||
        (portNum == CPSS_CPU_PORT_NUM_CNS))
    {
        *statePtr = GT_FALSE;
        *pauseAdvertisePtr = GT_FALSE;
        return GT_OK;
    }

    PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portMacNum,&fcAutoNegRegAddr);
    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, fcAutoNegRegAddr,
                                            11, 1, &value) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    *statePtr = BIT2BOOL_MAC(value);

    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, fcAutoNegRegAddr,
                                            9, 1, &value) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    *pauseAdvertisePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortFlowCntrlAutoNegEnableGet
*
* DESCRIPTION:
*       Get Auto-Negotiation enable/disable state for Flow Control per port
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number
*
* OUTPUTS:
*       statePtr       -   GT_TRUE for enable Auto-Negotiation for Flow Control
*                          GT_FALSE otherwise
*       pauseAdvertisePtr - Advertise symmetric flow control support in
*                           Auto-Negotiation. 0 = Disable, 1 = Enable.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CPU port not supports the flow control auto negotiation
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortFlowCntrlAutoNegEnableGet
(
    IN   GT_U8       devNum,
    IN   GT_PHYSICAL_PORT_NUM   portNum,
    OUT  GT_BOOL     *statePtr,
    OUT  GT_BOOL     *pauseAdvertisePtr
)
{
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;       /* port PHYMAC object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;           /* do switch mac code indicator */
    GT_U32          portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(statePtr);
    CPSS_NULL_PTR_CHECK_MAC(pauseAdvertisePtr);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    /* Get PHY MAC object ptr */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portMacNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacFlowCntlANGetFunc(devNum,portMacNum,
                                  statePtr,pauseAdvertisePtr,
                                  CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,statePtr,
                                  pauseAdvertisePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /*   if switch MAC value should be returned prvCpssDxChPortFlowCntrlAutoNegEnableGet
     *   will run
     */
    if (doPpMacConfig == GT_TRUE)
    {
        rc = prvCpssDxChPortFlowCntrlAutoNegEnableGet(devNum,portNum,statePtr,
                                                   pauseAdvertisePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post sage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacFlowCntlANGetFunc(devNum,portMacNum,
                                                    statePtr,pauseAdvertisePtr,
                                                    CPSS_MACDRV_STAGE_POST_E,
                                                    &doPpMacConfig,statePtr,
                                                    pauseAdvertisePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortFlowCntrlAutoNegEnableGet
*
* DESCRIPTION:
*       Get Auto-Negotiation enable/disable state for Flow Control per port
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number
*
* OUTPUTS:
*       statePtr       -   GT_TRUE for enable Auto-Negotiation for Flow Control
*                          GT_FALSE otherwise
*       pauseAdvertisePtr - Advertise symmetric flow control support in
*                           Auto-Negotiation. 0 = Disable, 1 = Enable.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CPU port not supports the flow control auto negotiation
*
*******************************************************************************/
GT_STATUS cpssDxChPortFlowCntrlAutoNegEnableGet
(
    IN   GT_U8       devNum,
    IN   GT_PHYSICAL_PORT_NUM   portNum,
    OUT  GT_BOOL     *statePtr,
    OUT  GT_BOOL     *pauseAdvertisePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortFlowCntrlAutoNegEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, statePtr, pauseAdvertisePtr));

    rc = internal_cpssDxChPortFlowCntrlAutoNegEnableGet(devNum, portNum, statePtr, pauseAdvertisePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, statePtr, pauseAdvertisePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
/*******************************************************************************
* prvCpssDxChPortSpeedAutoNegEnableSet
*
* DESCRIPTION:
*       Enable/disable an Auto-Negotiation of interface speed on specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       state    - GT_TRUE for enable Auto-Negotiation of interface speed,
*                  GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CPU port not supports the speed auto negotiation
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortSpeedAutoNegEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL   state
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32 portState = 1;       /* current port state (enable/disable) */
    GT_U32 speedAutoNegRegAddr; /* address of speed auto-neg. config. reg. */
    PRV_CPSS_PORT_TYPE_ENT portMacType; /* port mac unit to use */
    GT_U32          portMacNum;      /* MAC number */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum);
    if((portMacType >= PRV_CPSS_PORT_XG_E) ||
        ((portNum == CPSS_CPU_PORT_NUM_CNS) &&
        (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_FALSE)))
    {/* check if auto-neg. supported for current mode/port*/
        if (state == GT_TRUE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }
    }

    PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portMacNum,&speedAutoNegRegAddr);
    if(speedAutoNegRegAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    if(portMacType <= PRV_CPSS_PORT_GE_E)
    {/* disable port if we need */
        rc = prvCpssDxChPortStateDisableSet(devNum,portNum,&portState);
        if(rc != GT_OK)
            return rc;
    }

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    if(speedAutoNegRegAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_GE_E].regAddr = speedAutoNegRegAddr;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldData = BOOL2BIT_MAC(state);
        regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = 7;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = 1;
    }

    rc = prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(portMacType <= PRV_CPSS_PORT_GE_E)
    {/* restore port enable state */
        rc = prvCpssDxChPortStateEnableSet(devNum,portNum, portState);
        if(rc != GT_OK)
            return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortSpeedAutoNegEnableSet
*
* DESCRIPTION:
*       Enable/disable an Auto-Negotiation of interface speed on specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       state    - GT_TRUE for enable Auto-Negotiation of interface speed,
*                  GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*       2.CPU port not supports the speed auto negotiation
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortSpeedAutoNegEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL   state
)
{
    GT_BOOL targetState = state;
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;            /* do switch mac code indicator */
    GT_U32          portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.hitlessWriteMethodEnable)
    {
        GT_BOOL   currentState;

        rc = cpssDxChPortSpeedAutoNegEnableGet(devNum, portNum, &currentState);
        if(rc != GT_OK)
        {
            return rc;
        }
        if(state == currentState)
        {
            return GT_OK;
        }
    }

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portMacNum);

    /* run MACPHY callback */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacSpeedANSetFunc(devNum,portMacNum,
                                  targetState,CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,&targetState);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* set switch MAC configuration */
    if (doPpMacConfig == GT_TRUE)
    {
        rc = prvCpssDxChPortSpeedAutoNegEnableSet(devNum,portNum,state);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacSpeedANSetFunc(devNum,portMacNum,
                                  targetState,CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,&targetState);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortSpeedAutoNegEnableSet
*
* DESCRIPTION:
*       Enable/disable an Auto-Negotiation of interface speed on specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       state    - GT_TRUE for enable Auto-Negotiation of interface speed,
*                  GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*       2.CPU port not supports the speed auto negotiation
*
*******************************************************************************/
GT_STATUS cpssDxChPortSpeedAutoNegEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL   state
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortSpeedAutoNegEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, state));

    rc = internal_cpssDxChPortSpeedAutoNegEnableSet(devNum, portNum, state);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, state));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortSpeedAutoNegEnableGet
*
*
* DESCRIPTION:
*
*   Get status  of  Auto-Negotiation enable on specified port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
*
* OUTPUTS:
*       statePtr - GT_TRUE for enabled Auto-Negotiation of interface speed,
*                  GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CPU port not supports the speed auto negotiation
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortSpeedAutoNegEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT  GT_BOOL  *statePtr
)
{
    GT_U32 value;               /* current speed auto-neg. status */
    GT_U32 speedAutoNegRegAddr; /* speed auto-neg. reg. address */
    GT_U32 portGroupId;         /* port group num. for multi-port group dev's */
    GT_U32 portMacNum;          /* MAC number */

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    if((PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) >= PRV_CPSS_PORT_XG_E)
        || ((portNum == CPSS_CPU_PORT_NUM_CNS)&&(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_FALSE)))
    {
        *statePtr = GT_FALSE;
        return GT_OK;
    }

    PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portMacNum,&speedAutoNegRegAddr);
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == speedAutoNegRegAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
                                                                            portMacNum);
    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, speedAutoNegRegAddr,
                                            7, 1, &value) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    *statePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortSpeedAutoNegEnableGet
*
* DESCRIPTION:
*
*   Get status  of  Auto-Negotiation enable on specified port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
*
* OUTPUTS:
*       statePtr - GT_TRUE for enabled Auto-Negotiation of interface speed,
*                  GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*       2. CPU port not supports the speed auto negotiation
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortSpeedAutoNegEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT  GT_BOOL  *statePtr
)
{
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port callback pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;                       /* do switch mac code indicator */
    GT_U32          portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(statePtr);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portMacNum);

    /* MACPHY callback run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacSpeedANGetFunc(devNum,portMacNum,
                                  statePtr,CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,statePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* get the value from switch MAC */
    if (doPpMacConfig == GT_TRUE)
    {
        rc = prvCpssDxChPortSpeedAutoNegEnableGet(devNum,portNum,statePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacSpeedANGetFunc(devNum,portMacNum,
                                  statePtr,CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,statePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortSpeedAutoNegEnableGet
*
* DESCRIPTION:
*
*   Get status  of  Auto-Negotiation enable on specified port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
*
* OUTPUTS:
*       statePtr - GT_TRUE for enabled Auto-Negotiation of interface speed,
*                  GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*       2. CPU port not supports the speed auto negotiation
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortSpeedAutoNegEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT  GT_BOOL  *statePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortSpeedAutoNegEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, statePtr));

    rc = internal_cpssDxChPortSpeedAutoNegEnableGet(devNum, portNum, statePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, statePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
/*******************************************************************************
* prvCpssDxChPortFlowControlEnableSet
*
* DESCRIPTION:
*       Enable/disable receiving and transmission of 802.3x Flow Control frames
*       in full duplex on specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       state    - Flow Control state: Both disabled,
*                  Both enabled, Only Rx or Only Tx enabled.
*                  Note: only XG ports can be configured in all 4 options,
*                        Tri-Speed and FE ports may use only first two.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, state or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortFlowControlEnableSet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  CPSS_PORT_FLOW_CONTROL_ENT  state
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32 regAddr;         /* register address */
    GT_U32 macRegAddr;      /* register address */
    GT_U32 fcaRegAddr = PRV_CPSS_SW_PTR_ENTRY_UNUSED;         /* register address */
    GT_U32 value;           /* value to write into the register */
    GT_U32 fcaValue = 0;    /* value to write into the FCA register */
    GT_U32 portState = 1;   /* current port state (enable/disable) */
    GT_U32 offset;          /* bit number inside register       */
    GT_U32 length;          /* number of bits to be written to the register */
    GT_U32  portMacNum;      /* MAC number */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) <= PRV_CPSS_PORT_GE_E)
    {
        /* disable port if current interface is FE or GE */
        if (prvCpssDxChPortStateDisableSet(devNum,portNum, &portState) != GT_OK)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    if((PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
    {
        PRV_CPSS_DXCH_PORT_FCA_REGS_ADDR_STC *fcaStcPtr;

        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
        {
            fcaStcPtr = (portMacNum == CPSS_CPU_PORT_NUM_CNS) ?
                 &PRV_DXCH_REG_UNIT_GOP_CPU_FCA_MAC(devNum) :
                 &PRV_DXCH_REG_UNIT_GOP_FCA_MAC(devNum, portMacNum);

            fcaRegAddr = fcaStcPtr->FCACtrl;
        }
        else
        {
            fcaRegAddr = PRV_DXCH_REG1_UNIT_GOP_FCA_MAC(devNum, portMacNum).FCACtrl;
        }

        switch (state)
        {
            case CPSS_PORT_FLOW_CONTROL_DISABLE_E:
                fcaValue = 1;
                break;
            case CPSS_PORT_FLOW_CONTROL_RX_TX_E:
                fcaValue = 0;
                break;
            case CPSS_PORT_FLOW_CONTROL_RX_ONLY_E:
                fcaValue = 1;
                break;
            case CPSS_PORT_FLOW_CONTROL_TX_ONLY_E:
                fcaValue = 0;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* <FCA TX disable> bit configuration */
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum),
                fcaRegAddr, 1, 1, fcaValue) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

        PRV_CPSS_DXCH_PORT_MAC_CTRL4_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_GE_E,&regAddr);
        offset = 3;
        length = 2;

        switch (state)
        {
            case CPSS_PORT_FLOW_CONTROL_DISABLE_E:
                value = 0;
                break;
            case CPSS_PORT_FLOW_CONTROL_RX_TX_E:
                value = 3;
                break;
            case CPSS_PORT_FLOW_CONTROL_RX_ONLY_E:
                value = 1;
                break;
            case CPSS_PORT_FLOW_CONTROL_TX_ONLY_E:
                value = 2;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if(PRV_CPSS_DXCH_BOBCAT2_A0_CHECK_MAC(devNum))
        {
            value &= 0x1; /* For BC2-A0 only RX is configured via MAC - TX is controlled by FCA unit */
                          /* TX_FN_EN is disabled as part of CPSS_DXCH_IMPLEMENT_WA_TRI_SPEED_PORT_AN_FC_E */
        }
    }
    else
    {
        PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portMacNum,&regAddr);

        offset = 8;
        length = 1;

        switch (state)
        {
            case CPSS_PORT_FLOW_CONTROL_DISABLE_E:
                value = 0;
                break;
            case CPSS_PORT_FLOW_CONTROL_RX_TX_E:
                value = 1;
                break;
            default:
                if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum)<PRV_CPSS_PORT_XG_E)
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                goto xg_configuration;
        }

        /* in addition to AN-register configuration, MAC4 register should be configured - bit_3 txq_FC_update_en */
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL4_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_GE_E,&macRegAddr);
            if(macRegAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
            {
                regDataArray[PRV_CPSS_PORT_GE_E].regAddr = macRegAddr;
                regDataArray[PRV_CPSS_PORT_GE_E].fieldData = value;
                regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = 3;
                regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = 1;
            }

            rc = prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_GE_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = offset;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = length;
    }

    rc = prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
    if(rc != GT_OK)
    {
        return rc;
    }

xg_configuration:
    switch (state)
    {
        case CPSS_PORT_FLOW_CONTROL_DISABLE_E:
            value = 0;
            break;
        case CPSS_PORT_FLOW_CONTROL_RX_TX_E:
            value = 3;
            break;
        case CPSS_PORT_FLOW_CONTROL_RX_ONLY_E:
            value = 1;
            break;
        case CPSS_PORT_FLOW_CONTROL_TX_ONLY_E:
            value = 2;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    offset = ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)||
              (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) ||
              (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
              (PRV_CPSS_SIP_5_CHECK_MAC(devNum))) ? 7 : 8;

    if((PRV_CPSS_SIP_5_CHECK_MAC(devNum)) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
    {
        value &= 0x1; /* For BC2 and XCAT3 only for XLG ports RX is configured via MAC - TX is controlled by FCA unit */
    }

    length = 2;

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldOffset = offset;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldLength = length;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XLG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XLG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldOffset = offset;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldLength = length;
    }

    rc = prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) <= PRV_CPSS_PORT_GE_E)
    {
        /* restore port enable state */
        if (prvCpssDxChPortStateEnableSet(devNum,portNum, portState) != GT_OK)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortFlowControlEnableSet
*
* DESCRIPTION:
*       Enable/disable receiving and transmission of 802.3x Flow Control frames
*       in full duplex on specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       state    - Flow Control state: Both disabled,
*                  Both enabled, Only Rx or Only Tx enabled.
*                  Note: only XG ports can be configured in all 4 options,
*                        Tri-Speed and FE ports may use only first two.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, state or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortFlowControlEnableSet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  CPSS_PORT_FLOW_CONTROL_ENT  state
)
{
    CPSS_PORT_FLOW_CONTROL_ENT  targetState = state;
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port PHYMAC object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portMacNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacFlowCntlSetFunc(devNum,portMacNum,
                                  state,CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,&targetState);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* switch MAC configuration*/
    if (doPpMacConfig == GT_TRUE)
    {
        rc = prvCpssDxChPortFlowControlEnableSet(devNum,portNum,targetState);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

       /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacFlowCntlSetFunc(devNum,portMacNum,
                                  targetState,CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,&targetState);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortFlowControlEnableSet
*
* DESCRIPTION:
*       Enable/disable receiving and transmission of 802.3x Flow Control frames
*       in full duplex on specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       state    - Flow Control state: Both disabled,
*                  Both enabled, Only Rx or Only Tx enabled.
*                  Note: only XG ports can be configured in all 4 options,
*                        Tri-Speed and FE ports may use only first two.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, state or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*       Before calling cpssDxChPortFlowControlEnableSet,
*       cpssDxChPortPeriodicFlowControlCounterSet should be called
*       to set the interval between the transmission of two consecutive
*       Flow Control packets according to port speed
*
*******************************************************************************/
GT_STATUS cpssDxChPortFlowControlEnableSet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  CPSS_PORT_FLOW_CONTROL_ENT  state
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortFlowControlEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, state));

    rc = internal_cpssDxChPortFlowControlEnableSet(devNum, portNum, state);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, state));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortFlowControlEnableGet
*
* DESCRIPTION:
*       Get status of 802.3x Flow Control on specific logical port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       statePtr - Pointer to Flow Control state: Both disabled,
*                  Both enabled, Only Rx or Only Tx enabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortFlowControlEnableGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT CPSS_PORT_FLOW_CONTROL_ENT      *statePtr
)
{
    GT_U32  value;
    GT_U32  fcaValue = 1;
    GT_U32  regAddr;
    GT_U32  fcaRegAddr;
    GT_U32  fieldOffset;
    GT_U32  fieldLength;
    PRV_CPSS_PORT_TYPE_ENT portMacType;
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(statePtr);
    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum);

    if((PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
    {
        PRV_CPSS_DXCH_PORT_FCA_REGS_ADDR_STC *fcaStcPtr;

        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
        {
            fcaStcPtr = (portMacNum == CPSS_CPU_PORT_NUM_CNS) ?
                 &PRV_DXCH_REG_UNIT_GOP_CPU_FCA_MAC(devNum) :
                 &PRV_DXCH_REG_UNIT_GOP_FCA_MAC(devNum, portMacNum);

            fcaRegAddr = fcaStcPtr->FCACtrl;
        }
        else
        {
            fcaRegAddr = PRV_DXCH_REG1_UNIT_GOP_FCA_MAC(devNum, portMacNum).FCACtrl;
        }

        if (prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum),
                fcaRegAddr, 1, 1, &fcaValue) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
    }

    fieldLength = 2;

    if(portMacType >= PRV_CPSS_PORT_XG_E)
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,portMacType,&regAddr);
        fieldOffset = ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
                       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
                       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E) ||
                       (PRV_CPSS_SIP_5_CHECK_MAC(devNum))) ? 7 : 8;
    }
    else
    {
        if(PRV_CPSS_DXCH_BOBCAT2_A0_CHECK_MAC(devNum))
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL4_REG_MAC(devNum,portMacNum,portMacType,&regAddr);
            fieldOffset = 3;
            fieldLength = 1;
        }
        else
        {
            PRV_CPSS_DXCH_PORT_STATUS_CTRL_REG_MAC(devNum,portMacNum,&regAddr);
            fieldOffset = 4;
        }
    }

    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum),
        regAddr, fieldOffset, fieldLength, &value) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);

    if((PRV_CPSS_SIP_5_CHECK_MAC(devNum)) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
    {
        /* combine Rx and Tx values */
        value |= ((~fcaValue & 0x1) << 1);
    }

    switch (value)
    {
        case 0:
            *statePtr = CPSS_PORT_FLOW_CONTROL_DISABLE_E;
            break;
        case 1:
            *statePtr = CPSS_PORT_FLOW_CONTROL_RX_ONLY_E;
            break;
        case 2:
            *statePtr = CPSS_PORT_FLOW_CONTROL_TX_ONLY_E;
            break;
        case 3:
            *statePtr = CPSS_PORT_FLOW_CONTROL_RX_TX_E;
            break;
        default:
            /* no chance getting here */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortFlowControlEnableGet
*
* DESCRIPTION:
*       Get status of 802.3x Flow Control on specific logical port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       statePtr - Pointer to Flow Control state: Both disabled,
*                  Both enabled, Only Rx or Only Tx enabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortFlowControlEnableGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT CPSS_PORT_FLOW_CONTROL_ENT      *statePtr
)
{
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port PHYMAC object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */
    GT_U32  portMacNum;      /* MAC number */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(statePtr);

    /* Get PHY MAC object ptr */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portMacNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacFlowCntlGetFunc(devNum,portMacNum,
                                  statePtr,CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,statePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* get vavue from switch MAC */
    if (doPpMacConfig == GT_TRUE)
    {
        rc = prvCpssDxChPortFlowControlEnableGet(devNum,portNum,statePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacFlowCntlGetFunc(devNum,portMacNum,
                                  statePtr,CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,statePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortFlowControlEnableGet
*
* DESCRIPTION:
*       Get status of 802.3x Flow Control on specific logical port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       statePtr - Pointer to Flow Control state: Both disabled,
*                  Both enabled, Only Rx or Only Tx enabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortFlowControlEnableGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT CPSS_PORT_FLOW_CONTROL_ENT      *statePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortFlowControlEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, statePtr));

    rc = internal_cpssDxChPortFlowControlEnableGet(devNum, portNum, statePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, statePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortPeriodicFcEnableSet
*
* DESCRIPTION:
*       Enable/Disable transmits of periodic 802.3x flow control.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       enable   - periodic 802.3x flow control tramsition state
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
*       The gig port cannot disable the xoff, therefore the configuration for the gig port:
*       CPSS_PORT_PERIODIC_FLOW_CONTROL_DISABLE_E = xoff only
*       CPSS_PORT_PERIODIC_FLOW_CONTROL_XON_ONLY_E = both xoff and xon
*
*       Note: In Bobcat2, Caelum, Bobcat3 CPSS_PORT_PERIODIC_FLOW_CONTROL_XON_ONLY_E
*       option is not supported.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortPeriodicFcEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_PERIODIC_FLOW_CONTROL_TYPE_ENT   enable
)
{
    GT_STATUS   rc; /* return code */
    GT_U32 regAddr; /* register address               */
    GT_U32 regValue=0, valueOn=0,valueOff=0;           /* value to write into the register    */
    GT_U32     portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32  portMacNum;      /* MAC number */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);

    switch (enable) {
    case CPSS_PORT_PERIODIC_FLOW_CONTROL_DISABLE_E:
        break;
    case CPSS_PORT_PERIODIC_FLOW_CONTROL_XON_XOFF_E:
        valueOn=1;
        valueOff=1;
        break;
    case CPSS_PORT_PERIODIC_FLOW_CONTROL_XON_ONLY_E:
        valueOn=1;
        if((PRV_CPSS_SIP_5_CHECK_MAC(devNum)))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }
        break;
    case CPSS_PORT_PERIODIC_FLOW_CONTROL_XOFF_ONLY_E:
        valueOff=1;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if((PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
    {
        PRV_CPSS_DXCH_PORT_FCA_REGS_ADDR_STC *fcaStcPtr;

        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
        {
            fcaStcPtr = (portMacNum == CPSS_CPU_PORT_NUM_CNS) ?
                 &PRV_DXCH_REG_UNIT_GOP_CPU_FCA_MAC(devNum) :
                 &PRV_DXCH_REG_UNIT_GOP_FCA_MAC(devNum, portMacNum);

            regAddr = fcaStcPtr->FCACtrl;
        }
        else
        {
            regAddr = PRV_DXCH_REG1_UNIT_GOP_FCA_MAC(devNum, portMacNum).FCACtrl;
        }

        regValue = (valueOff << 11) | (valueOn << 7);

        return prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum, portGroupId, regAddr, (BIT_11 | BIT_7), regValue);
    }

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    /* configure XON */
    PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_GE_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_GE_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldData = valueOn;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = 1;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = 1;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldData = valueOn;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldOffset =
            ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) ||
              (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
              (PRV_CPSS_SIP_5_CHECK_MAC(devNum)))? 10 : 11;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldLength = 1;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XLG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XLG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldData = valueOn;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldOffset =
            ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) ||
              (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
              (PRV_CPSS_SIP_5_CHECK_MAC(devNum)))? 10 : 11;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldLength = 1;
    }

    rc = prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* clear reg's data for new configuration */
    rc = prvCpssDxChPortMacConfigurationClear(regDataArray);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* configure XOFF only on XG/XLG */
    PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldData = valueOff;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldOffset =
            ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) ||
              (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
              (PRV_CPSS_SIP_5_CHECK_MAC(devNum)))? 10 : 11;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldLength = 1;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XLG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XLG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldData = valueOff;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldOffset =
            ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) ||
              (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
              (PRV_CPSS_SIP_5_CHECK_MAC(devNum)))? 10 : 11;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldLength = 1;
    }

    return prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
}

/*******************************************************************************
* internal_cpssDxChPortPeriodicFcEnableSet
*
* DESCRIPTION:
*       Enable/Disable transmits of periodic 802.3x flow control.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       enable   - periodic 802.3x flow control tramsition state
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - on unsupported request
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*       Note: The gig port cannot disable the xoff, therefore the configuration for the gig port:
*       CPSS_PORT_PERIODIC_FLOW_CONTROL_DISABLE_E = xoff only
*       CPSS_PORT_PERIODIC_FLOW_CONTROL_XON_ONLY_E = both xoff and xon
*
*       Note: In Bobcat2, Caelum, Bobcat3 CPSS_PORT_PERIODIC_FLOW_CONTROL_XON_ONLY_E
*       option is not supported.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPeriodicFcEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_PERIODIC_FLOW_CONTROL_TYPE_ENT   enable
)
{
    GT_BOOL targetEnable = enable;
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;            /* do switch mac code indicator */
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portMacNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        if (enable==CPSS_PORT_PERIODIC_FLOW_CONTROL_XON_ONLY_E  ||
            enable==CPSS_PORT_PERIODIC_FLOW_CONTROL_XOFF_ONLY_E)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }

        rc = portMacObjPtr->macDrvMacPeriodFlowCntlSetFunc(devNum,portMacNum,
                                  targetEnable,CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,&targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* set switch MAC configuration */
    if (doPpMacConfig == GT_TRUE)
    {
        rc = prvCpssDxChPortPeriodicFcEnableSet(devNum,portNum,targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacPeriodFlowCntlSetFunc(devNum,portMacNum,
                                    targetEnable,CPSS_MACDRV_STAGE_POST_E,
                                    &doPpMacConfig,&targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortPeriodicFcEnableSet
*
* DESCRIPTION:
*       Enable/Disable transmits of periodic 802.3x flow control.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       enable   - periodic 802.3x flow control tramsition state
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - on unsupported request
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*       Note: The gig port cannot disable the xoff, therefore the configuration for the gig port:
*       CPSS_PORT_PERIODIC_FLOW_CONTROL_DISABLE_E = xoff only
*       CPSS_PORT_PERIODIC_FLOW_CONTROL_XON_ONLY_E = both xoff and xon
*
*       Note: In Bobcat2, Caelum, Bobcat3 CPSS_PORT_PERIODIC_FLOW_CONTROL_XON_ONLY_E
*       option is not supported.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPeriodicFcEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_PERIODIC_FLOW_CONTROL_TYPE_ENT   enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPeriodicFcEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChPortPeriodicFcEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortPeriodicFcEnableGet
*
* DESCRIPTION:
*       Get status of periodic 802.3x flow control transmition.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr  - Pointer to the periodic 802.3x flow control
*                    tramsition state.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The gig port cannot disable the xoff, therefore the configuration for the gig port:
*       CPSS_PORT_PERIODIC_FLOW_CONTROL_DISABLE_E = xoff only
*       CPSS_PORT_PERIODIC_FLOW_CONTROL_XON_ONLY_E = both xoff and xon
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortPeriodicFcEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT CPSS_PORT_PERIODIC_FLOW_CONTROL_TYPE_ENT   *enablePtr
)
{
    GT_U32 regAddr, regAddrXoff; /* register address       */
    GT_U32 value, valueXoff;     /* value read from register    */
    GT_U32 offset;          /* bit number inside register  */
    GT_STATUS rc;
    PRV_CPSS_PORT_TYPE_ENT portMacType;
    GT_U32     portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);
    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum);

    if((PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
    {
        PRV_CPSS_DXCH_PORT_FCA_REGS_ADDR_STC *fcaStcPtr;

        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
        {
            fcaStcPtr = (portMacNum == CPSS_CPU_PORT_NUM_CNS) ?
                 &PRV_DXCH_REG_UNIT_GOP_CPU_FCA_MAC(devNum) :
                 &PRV_DXCH_REG_UNIT_GOP_FCA_MAC(devNum, portMacNum);

            regAddr = fcaStcPtr->FCACtrl;
        }
        else
        {
            regAddr = PRV_DXCH_REG1_UNIT_GOP_FCA_MAC(devNum, portMacNum).FCACtrl;
        }

        rc = prvCpssDrvHwPpPortGroupReadRegBitMask(devNum, portGroupId, regAddr, (BIT_11 | BIT_7), &value);
        if(GT_OK != rc)
        {
            return rc;
        }

        switch(value)
        {
            case (0):
                *enablePtr = CPSS_PORT_PERIODIC_FLOW_CONTROL_DISABLE_E;
                break;
            case (BIT_7):
                *enablePtr = CPSS_PORT_PERIODIC_FLOW_CONTROL_XON_ONLY_E;
                break;
            case (BIT_11):
                *enablePtr = CPSS_PORT_PERIODIC_FLOW_CONTROL_XOFF_ONLY_E;
                break;
            case (BIT_7 | BIT_11):
                *enablePtr = CPSS_PORT_PERIODIC_FLOW_CONTROL_XON_XOFF_E;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

        return GT_OK;
    }


   /* Gig has only Xon (Xoff always enabled)*/
    if(portMacType < PRV_CPSS_PORT_XG_E)
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum, portMacNum, portMacType, &regAddr);
        if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

        offset = 1;
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, offset, 1, &value);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

        *enablePtr=(value==1)?CPSS_PORT_PERIODIC_FLOW_CONTROL_XON_XOFF_E:
            CPSS_PORT_PERIODIC_FLOW_CONTROL_XOFF_ONLY_E;
    }
    /* XG has both xoff and xon*/
    else
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,portMacType,&regAddr);
        if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum, portMacNum, portMacType, &regAddrXoff);
        if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddrXoff)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

        if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
           (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
           (PRV_CPSS_SIP_5_CHECK_MAC(devNum)))
        {
            offset = 10;
        }
        else
        {
            offset = 11;
        }

        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, offset, 1, &value);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddrXoff, offset, 1, &valueXoff);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

        *enablePtr=(value==0)? (valueXoff==0?CPSS_PORT_PERIODIC_FLOW_CONTROL_DISABLE_E:
                                CPSS_PORT_PERIODIC_FLOW_CONTROL_XOFF_ONLY_E  ):
                               (valueXoff==0?CPSS_PORT_PERIODIC_FLOW_CONTROL_XON_ONLY_E:
                                CPSS_PORT_PERIODIC_FLOW_CONTROL_XON_XOFF_E  );
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortPeriodicFcEnableGet
*
* DESCRIPTION:
*       Get status of periodic 802.3x flow control transmition.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr  - Pointer to the periodic 802.3x flow control
*                    tramsition state.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*       Note: The gig port cannot disable the xoff, therefore the configuration for the gig port:
*       CPSS_PORT_PERIODIC_FLOW_CONTROL_DISABLE_E = xoff only
*       CPSS_PORT_PERIODIC_FLOW_CONTROL_XON_ONLY_E = both xoff and xon
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPeriodicFcEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT CPSS_PORT_PERIODIC_FLOW_CONTROL_TYPE_ENT   *enablePtr
)
{
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* No need to check external mac, checking switch side only*/

    /* get value from switch MAC */
    if (doPpMacConfig == GT_TRUE)
    {
        rc = prvCpssDxChPortPeriodicFcEnableGet(devNum,portNum,enablePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    return rc;
}

/*******************************************************************************
* cpssDxChPortPeriodicFcEnableGet
*
* DESCRIPTION:
*       Get status of periodic 802.3x flow control transmition.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr  - Pointer to the periodic 802.3x flow control
*                    tramsition state.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*       Note: The gig port cannot disable the xoff, therefore the configuration for the gig port:
*       CPSS_PORT_PERIODIC_FLOW_CONTROL_DISABLE_E = xoff only
*       CPSS_PORT_PERIODIC_FLOW_CONTROL_XON_ONLY_E = both xoff and xon
*
*******************************************************************************/
GT_STATUS cpssDxChPortPeriodicFcEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT CPSS_PORT_PERIODIC_FLOW_CONTROL_TYPE_ENT   *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPeriodicFcEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChPortPeriodicFcEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortBackPressureEnableSet
*
* DESCRIPTION:
*       Enable/disable of Back Pressure in half-duplex on specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       state    - GT_TRUE for enable Back Pressure, GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - if feature not supported by port
*
* COMMENTS:
*       Although for XGMII (10 Gbps) ports feature is not supported we let
*       application to configure it, in case GE mac unit exists for this port
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortBackPressureEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    IN  GT_BOOL   state
)
{
    GT_U32 backPressureRegAddr; /* register address */
    GT_U32  portMacNum;      /* MAC number */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    /* set BackPressure Enable bit */
    /* Serial Parameters Register is one for 6 ports set */
    PRV_CPSS_DXCH_PORT_SERIAL_PARAM_CTRL_REG_MAC(devNum, portMacNum,
                                                    &backPressureRegAddr);
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == backPressureRegAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    regDataArray[PRV_CPSS_PORT_GE_E].regAddr = backPressureRegAddr;
    regDataArray[PRV_CPSS_PORT_GE_E].fieldData = BOOL2BIT_MAC(state);
    regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = 1;
    regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = 4;

    return prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
}

/*******************************************************************************
* internal_cpssDxChPortBackPressureEnableSet
*
* DESCRIPTION:
*       Enable/disable of Back Pressure in half-duplex on specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       state    - GT_TRUE for enable Back Pressure, GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - if feature not supported by port
*
* COMMENTS:
*       1. Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*       2. Although for XGMII (10 Gbps) ports feature is not supported the
*       function let application to configure it, in case GE mac unit exists
*       for this port
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortBackPressureEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    IN  GT_BOOL   state
)
{
    GT_BOOL targetState = state;

    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    /* Get PHY MAC object ptr */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portMacNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacBackPrSetFunc(devNum,portMacNum,
                                  targetState,CPSS_MACDRV_STAGE_PRE_E,
                                       &doPpMacConfig,&targetState);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* set switch MAC configuration */
    if (doPpMacConfig == GT_TRUE)
    {
        rc = prvCpssDxChPortBackPressureEnableSet(devNum,portNum,targetState);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacBackPrSetFunc(devNum,portMacNum,
                                  targetState,CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,&targetState);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortBackPressureEnableSet
*
* DESCRIPTION:
*       Enable/disable of Back Pressure in half-duplex on specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       state    - GT_TRUE for enable Back Pressure, GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - if feature not supported by port
*
* COMMENTS:
*       1. Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*       2. Although for XGMII (10 Gbps) ports feature is not supported the
*       function let application to configure it, in case GE mac unit exists
*       for this port
*
*******************************************************************************/
GT_STATUS cpssDxChPortBackPressureEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    IN  GT_BOOL   state
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortBackPressureEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, state));

    rc = internal_cpssDxChPortBackPressureEnableSet(devNum, portNum, state);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, state));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortBackPressureEnableGet
*
* DESCRIPTION:
*       Gets the state of Back Pressure in half-duplex on specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr  - pointer to back pressure enable/disable state:
*                       - GT_TRUE to enable Back Pressure
*                       - GT_FALSE to disable Back Pressure.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Although for XGMII (10 Gbps) ports feature is not supported the
*       function let application to configure it, in case GE mac unit exists
*       for this port
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortBackPressureEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL   *enablePtr
)
{
    GT_U32 value;           /* value to write into the register */
    GT_U32 regAddr;         /* register address            */
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    /* get BackPressure Enable bit */
    /* Serial Parameters Register is one for 6 ports set */
    PRV_CPSS_DXCH_PORT_SERIAL_PARAM_CTRL_REG_MAC(devNum,portMacNum,&regAddr);
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    if (prvCpssDrvHwPpPortGroupGetRegField(devNum,
        PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
                                                                    portMacNum),
            regAddr, 4, 1, &value) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortBackPressureEnableGet
*
* DESCRIPTION:
*       Gets the state of Back Pressure in half-duplex on specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr  - pointer to back pressure enable/disable state:
*                       - GT_TRUE to enable Back Pressure
*                       - GT_FALSE to disable Back Pressure.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*       2. Although for XGMII (10 Gbps) ports feature is not supported the
*       function let application to configure it, in case GE mac unit exists
*       for this port
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortBackPressureEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL   *enablePtr
)
{
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portMacNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacBackPrGetFunc(devNum,portMacNum,
                                  enablePtr,CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,enablePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* get value from switch MAC */
    if (doPpMacConfig == GT_TRUE)
    {
        rc = prvCpssDxChPortBackPressureEnableGet(devNum,portNum,enablePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacBackPrGetFunc(devNum,portMacNum,
                                  enablePtr,CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,enablePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortBackPressureEnableGet
*
* DESCRIPTION:
*       Gets the state of Back Pressure in half-duplex on specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr  - pointer to back pressure enable/disable state:
*                       - GT_TRUE to enable Back Pressure
*                       - GT_FALSE to disable Back Pressure.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*       2. Although for XGMII (10 Gbps) ports feature is not supported the
*       function let application to configure it, in case GE mac unit exists
*       for this port
*
*******************************************************************************/
GT_STATUS cpssDxChPortBackPressureEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL   *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortBackPressureEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChPortBackPressureEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* prvCpssDxChPortLinkStatusGet
*
* DESCRIPTION:
*       Gets Link Status of specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       isLinkUpPtr - GT_TRUE for link up, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortLinkStatusGet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL   *isLinkUpPtr
)
{
    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* value to write into the register */
    GT_U32 portGroupId;
    GT_U32 portMacNum; /* MAC number */

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(
                                                                devNum, portMacNum);
    if ((PRV_CPSS_SIP_5_CHECK_MAC(devNum)) ||
        (((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
          (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)) &&
                                            (portNum != CPSS_CPU_PORT_NUM_CNS)))
    {
        GT_STATUS   rc; /* return code */
        GT_U32      phyPortNum; /* port number in local core */
        MV_HWS_PORT_STANDARD    portMode;
        CPSS_DXCH_PORT_COMBO_PARAMS_STC *comboParamsPtr;
        CPSS_PORT_INTERFACE_MODE_ENT    cpssIfMode;
        CPSS_PORT_SPEED_ENT             cpssSpeed;

#ifdef GM_USED
        {
            GT_U32  sim_deviceId;
            /* convert cpss devNum to simulation deviceId */
            rc = getSimDevIdFromSwDevNum(devNum,0,&sim_deviceId);
            if(rc != GT_OK)
            {
                return rc;
            }
            /* get info from the simulation about the GM */
            snetGmPortInfoGet((GT_U8)sim_deviceId,(GT_U8)portMacNum,isLinkUpPtr,NULL);

            /* to avoid :  warning C4702: unreachable code. after the :
                return GT_OK;
            */
            if(rc != GT_ABORTED)
            {
                /* in GM we always get in here */
                return GT_OK;
            }
        }
#endif /*GM_USED*/

        cpssIfMode = PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portMacNum);
        cpssSpeed = PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum, portMacNum);

        if((CPSS_PORT_INTERFACE_MODE_NA_E == cpssIfMode) ||
           (CPSS_PORT_SPEED_NA_E == cpssSpeed))
        {
            *isLinkUpPtr = GT_FALSE;
            return GT_OK;
        }

        portMode = NON_SUP_MODE;
        comboParamsPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->port.comboPortsInfoArray[portMacNum];
        if (!PRV_CPSS_SIP_5_CHECK_MAC(devNum))
        {
            phyPortNum = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,
                                                                        portMacNum);
        }
        else
        {
            phyPortNum = portMacNum;
            portGroupId = 0;
        }

        if(comboParamsPtr->macArray[0].macNum != CPSS_DXCH_PORT_COMBO_NA_MAC_CNS)
        { /* if it's combo port */
            CPSS_DXCH_PORT_MAC_PARAMS_STC   mac;
            rc = cpssDxChPortComboPortActiveMacGet(devNum, portNum, &mac);
            if(rc != GT_OK)
                return rc;
            if (mac.macNum != phyPortNum)
            {
                switch(cpssIfMode)
                {
                    case CPSS_PORT_INTERFACE_MODE_SR_LR_E:
                    case CPSS_PORT_INTERFACE_MODE_KR_E:
                        portMode = _100GBase_KR10;
                        break;
                    case CPSS_PORT_INTERFACE_MODE_SGMII_E:
                        portMode = QSGMII;
                        break;
                    case CPSS_PORT_INTERFACE_MODE_1000BASE_X_E:
                        portMode = _100Base_FX;
                        break;
                    default:
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                }
            }
        }

        if (NON_SUP_MODE == portMode)
        {
            rc = prvCpssLion2CpssIfModeToHwsTranslate(
                                        PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portMacNum),
                                        PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum, portMacNum),
                                        &portMode);
            if(rc != GT_OK)
                return rc;
        }
        CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPortLinkStatusGet(devNum[%d], portGroupId[%d], phyPortNum[%d], portMode[%d], *isLinkUpPtr)",devNum, portGroupId, phyPortNum, portMode);
        rc = mvHwsPortLinkStatusGet(devNum, portGroupId, phyPortNum, portMode,
                                    isLinkUpPtr);
        if(rc != GT_OK)
        {
            CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
            return rc;
        }
    }
    else
    {
        PRV_CPSS_DXCH_PORT_STATUS_CTRL_REG_MAC(devNum,portMacNum,&regAddr);
        if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

        if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, 0, 1,
                                               &value) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

        *isLinkUpPtr = BIT2BOOL_MAC(value);
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortLinkStatusGet
*
* DESCRIPTION:
*       Gets Link Status of specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       isLinkUpPtr - GT_TRUE for link up, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortLinkStatusGet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL   *isLinkUpPtr
)
{
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */
    GT_U32 portMacNum; /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(isLinkUpPtr);

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portMacNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacPortlinkGetFunc(devNum,portMacNum,
                                  isLinkUpPtr,CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,isLinkUpPtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* get value from switch MAC */
    if (doPpMacConfig == GT_TRUE)
    {
        rc = prvCpssDxChPortLinkStatusGet(devNum,portNum,isLinkUpPtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacPortlinkGetFunc(devNum,portMacNum,
                                  isLinkUpPtr,CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,isLinkUpPtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortLinkStatusGet
*
* DESCRIPTION:
*       Gets Link Status of specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       isLinkUpPtr - GT_TRUE for link up, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortLinkStatusGet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL   *isLinkUpPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortLinkStatusGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, isLinkUpPtr));

    rc = internal_cpssDxChPortLinkStatusGet(devNum, portNum, isLinkUpPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, isLinkUpPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortForceLinkPassEnableSet
*
* DESCRIPTION:
*       Enable/disable Force Link Pass on specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       state    - GT_TRUE for force link pass, GT_FALSE otherwise
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
static GT_STATUS internal_cpssDxChPortForceLinkPassEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    IN  GT_BOOL   state
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  portMacNum; /* MAC number */
    GT_U32  value;      /* data to write to register */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.hitlessWriteMethodEnable)
    {
        GT_STATUS   rc;
        GT_BOOL     currentState;

        rc = cpssDxChPortForceLinkPassEnableGet(devNum,portNum,&currentState);
        if(rc != GT_OK)
        {
            return rc;
        }
        if(state == currentState)
        {
            return GT_OK;
        }
    }

    value = BOOL2BIT_MAC(state);

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portMacNum,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_GE_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = 1;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldOffset = 3;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XLG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XLG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldOffset = 3;
    }
        
    return prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
}

/*******************************************************************************
* cpssDxChPortForceLinkPassEnableSet
*
* DESCRIPTION:
*       Enable/disable Force Link Pass on specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       state    - GT_TRUE for force link pass, GT_FALSE otherwise
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
GT_STATUS cpssDxChPortForceLinkPassEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    IN  GT_BOOL   state
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortForceLinkPassEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, state));

    rc = internal_cpssDxChPortForceLinkPassEnableSet(devNum, portNum, state);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, state));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortForceLinkPassEnableGet
*
* DESCRIPTION:
*       Get Force Link Pass on specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       statePtr - (ptr to) current force link pass state:
*                           GT_TRUE for force link pass, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - NULL pointer in statePtr
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortForceLinkPassEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL   *statePtr
)
{
    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* value to write into the register */
    GT_U32 offset;          /* bit number inside register       */
    GT_U32 portGroupId;     /*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT portMacType; /* mac type of port */
    GT_U32 portMacNum; /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(statePtr);

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum);
    if (portMacType >= PRV_CPSS_PORT_XG_E)
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,portMacType,&regAddr);
        offset = 3;
    }
    else
    {
        PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portMacNum,&regAddr);
        offset = 1;
    }
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    /* get force link pass bit */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);
    if(prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, offset, 1, &value) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);

    *statePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortForceLinkPassEnableGet
*
* DESCRIPTION:
*       Get Force Link Pass on specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       statePtr - (ptr to) current force link pass state:
*                           GT_TRUE for force link pass, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - NULL pointer in statePtr
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortForceLinkPassEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL   *statePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortForceLinkPassEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, statePtr));

    rc = internal_cpssDxChPortForceLinkPassEnableGet(devNum, portNum, statePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, statePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortForceLinkDownEnableSet
*
* DESCRIPTION:
*       Enable/disable Force Link Down on specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       state    - GT_TRUE for force link down, GT_FALSE otherwise
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
GT_STATUS prvCpssDxChPortForceLinkDownEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    IN  GT_BOOL   state
)
{
    GT_U32  forceLinkDownRegAddr; /* register address */
    GT_U32  value;                /* value to write into the register */
    GT_U32  portMacNum; /* MAC number */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.hitlessWriteMethodEnable)
    {
        GT_STATUS   rc;
        GT_BOOL     currentState;

        rc = cpssDxChPortForceLinkDownEnableGet(devNum,portNum,&currentState);
        if(rc != GT_OK)
        {
            return rc;
        }
        if(state == currentState)
        {
            return GT_OK;
        }
    }

    value = BOOL2BIT_MAC(state);

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum, portMacNum, &forceLinkDownRegAddr);
    if(forceLinkDownRegAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_GE_E].regAddr = forceLinkDownRegAddr;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = 0;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum, portMacNum, PRV_CPSS_PORT_XG_E,
                                                        &forceLinkDownRegAddr);
    if(forceLinkDownRegAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XG_E].regAddr = forceLinkDownRegAddr;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldOffset = 2;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum, portMacNum, PRV_CPSS_PORT_XLG_E,
                                                        &forceLinkDownRegAddr);
    if(forceLinkDownRegAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XLG_E].regAddr = forceLinkDownRegAddr;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldOffset = 2;
    }

    return prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
}

/*******************************************************************************
* internal_cpssDxChPortForceLinkDownEnableSet
*
* DESCRIPTION:
*       Enable/disable Force Link Down on specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       state    - GT_TRUE for force link down, GT_FALSE otherwise
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
static GT_STATUS internal_cpssDxChPortForceLinkDownEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    IN  GT_BOOL   state
)
{
    GT_STATUS   rc;
    GT_U32 portMacNum; /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    rc = prvCpssDxChPortForceLinkDownEnableSet(devNum,portNum,state);
    if(rc != GT_OK)
    {
        return rc;
    }

    if ((PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                             PRV_CPSS_DXCH_LION2_DISMATCH_PORTS_LINK_WA_E))
        || (PRV_CPSS_SIP_5_CHECK_MAC(devNum))
        || (CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily))
    {
        CPSS_PORTS_BMP_PORT_ENABLE_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
           info_PRV_CPSS_DXCH_LION2_DISMATCH_PORT_LINK_WA_E.
                        portForceLinkDownBmpPtr,portNum,state);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortForceLinkDownEnableSet
*
* DESCRIPTION:
*       Enable/disable Force Link Down on specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       state    - GT_TRUE for force link down, GT_FALSE otherwise
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
GT_STATUS cpssDxChPortForceLinkDownEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    IN  GT_BOOL   state
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortForceLinkDownEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, state));

    rc = internal_cpssDxChPortForceLinkDownEnableSet(devNum, portNum, state);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, state));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortForceLinkDownEnableGet
*
* DESCRIPTION:
*       Get Force Link Down status of specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       statePtr - (ponter to) current force link down status:
*                              GT_TRUE - force link down, GT_FALSE - otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - NULL pointer in statePtr
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortForceLinkDownEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL   *statePtr
)
{
    GT_U32        forceLinkDownRegAddr; /* register address */
    GT_U32        value;                /* value to write into the register */
    GT_U32        forceLinkDownOffset;  /* bit number inside register       */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT portMacType;
    GT_U32 portMacNum; /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(statePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);
    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum);

    if(CPSS_SYSTEM_RECOVERY_PROCESS_NOT_ACTIVE_E == systemRecoveryInfo.systemRecoveryProcess)
    {/* read from SW DB only if not during some recovery */
        if ((PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                                         PRV_CPSS_DXCH_LION2_DISMATCH_PORTS_LINK_WA_E))
            || (PRV_CPSS_SIP_5_CHECK_MAC(devNum))
            || (CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily))

        {
            *statePtr = CPSS_PORTS_BMP_IS_PORT_SET_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                               info_PRV_CPSS_DXCH_LION2_DISMATCH_PORT_LINK_WA_E.
                                 portForceLinkDownBmpPtr,portNum);

            return GT_OK;
        }
    }

    /* for 10 Gb interface and up */
    if(portMacType >= PRV_CPSS_PORT_XG_E)
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,portMacType,&forceLinkDownRegAddr);
        forceLinkDownOffset = 2;
    }
    else
    {
        PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portMacNum,&forceLinkDownRegAddr);
        forceLinkDownOffset = 0;
    }
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == forceLinkDownRegAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, forceLinkDownRegAddr,
                                           forceLinkDownOffset, 1, &value) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    *statePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortForceLinkDownEnableGet
*
* DESCRIPTION:
*       Get Force Link Down status of specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       statePtr - (ponter to) current force link down status:
*                              GT_TRUE - force link down, GT_FALSE - otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - NULL pointer in statePtr
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortForceLinkDownEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL   *statePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortForceLinkDownEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, statePtr));

    rc = internal_cpssDxChPortForceLinkDownEnableGet(devNum, portNum, statePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, statePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortMruSet
*
* DESCRIPTION:
*       Sets the Maximal Receive Packet size for specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (or CPU port)
*       mruSize    - max receive packet size in bytes. (0..10304) - jumbo frame + 64  (including 4 bytes CRC)
*                    value must be even
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or
*                                   odd value of mruSize
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - mruSize > 10304
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
#define PRV_CPSS_DXCH_PORT_MRU_FIELD_LENGTH_CNS 13
#define PRV_CPSS_DXCH_PORT_MRU_FIELD_OFFSET_GE_CNS 2
#define PRV_CPSS_DXCH_PORT_MRU_FIELD_OFFSET_XG_CNS 0
static GT_STATUS prvCpssDxChPortMruSet
(
    IN  GT_U8  devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    IN  GT_U32 mruSize
)
{
    GT_STATUS   rc;
    GT_U32  value;
    GT_U32  regAddr;
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    /* The resolution of this field is 2 bytes and
     * the default value is 1522 bytes => 0x2f9
     */
    if(mruSize & 1)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(mruSize > CPSS_DXCH_PORT_MAX_MRU_CNS)
    {
        /* 10KB + 64 max frame size supported */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    value = mruSize >> 1;

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum, portMacNum, PRV_CPSS_PORT_GE_E,
                                                                        &regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_GE_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = PRV_CPSS_DXCH_PORT_MRU_FIELD_OFFSET_GE_CNS;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = PRV_CPSS_DXCH_PORT_MRU_FIELD_LENGTH_CNS;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldData = value;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum, portMacNum, PRV_CPSS_PORT_XG_E,
                                                                        &regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldOffset = PRV_CPSS_DXCH_PORT_MRU_FIELD_OFFSET_XG_CNS;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldLength = PRV_CPSS_DXCH_PORT_MRU_FIELD_LENGTH_CNS;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldData = value;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum, portMacNum, PRV_CPSS_PORT_XLG_E,
                                                                        &regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XLG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldOffset = PRV_CPSS_DXCH_PORT_MRU_FIELD_OFFSET_XG_CNS;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldLength = PRV_CPSS_DXCH_PORT_MRU_FIELD_LENGTH_CNS;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldData = value;
    }

    rc = prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortMruSet
*
* DESCRIPTION:
*       Sets the Maximal Receive Packet size for specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (or CPU port)
*       mruSize    - max receive packet size in bytes.
*                    (APPLICABLE RANGES: 0..10304) - jumbo frame + 64 (including
*                    4 bytes CRC). Value must be even.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or
*                          odd value of mruSize
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - mruSize > 10304
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortMruSet
(
    IN  GT_U8  devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    IN  GT_U32 mruSize
)
{
    /***********************************************
    * to solve jumbo packet problem:
    *  - set MRU value on PHY MACs as maximum - 0x3FFF
    *  - switch MAC MRU value set as require in API
    ************************************************/
    GT_U32 targetMruSize = 0x3FFF;

    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    /* Get PHY MAC object ptr */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portMacNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacMRUSetFunc(devNum,portMacNum,
                                  targetMruSize,
                                  CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,
                                  &targetMruSize);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* set MRU size value for switch MAC */
    if (doPpMacConfig == GT_TRUE)
    {
        rc = prvCpssDxChPortMruSet(devNum,portNum,mruSize);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacMRUSetFunc(devNum,portMacNum,
                                  targetMruSize,
                                  CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,
                                  &targetMruSize);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortMruSet
*
* DESCRIPTION:
*       Sets the Maximal Receive Packet size for specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (or CPU port)
*       mruSize    - max receive packet size in bytes.
*                    (APPLICABLE RANGES: 0..10304) - jumbo frame + 64 (including
*                    4 bytes CRC). Value must be even.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or
*                          odd value of mruSize
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - mruSize > 10304
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortMruSet
(
    IN  GT_U8  devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    IN  GT_U32 mruSize
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMruSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, mruSize));

    rc = internal_cpssDxChPortMruSet(devNum, portNum, mruSize);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, mruSize));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortMruGet
*
* DESCRIPTION:
*       Gets the Maximal Receive Packet size for specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (or CPU port)
*
* OUTPUTS:
*       mruSizePtr - (pointer to) max receive packet size in bytes. (0..10304)
*                    jumbo frame + 64  (including 4 bytes CRC)
*                    value must be even
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or
*                                   odd value of mruSize
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - mruSize > 10304
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortMruGet
(
    IN   GT_U8  devNum,
    IN   GT_PHYSICAL_PORT_NUM  portNum,
    OUT  GT_U32 *mruSizePtr
)
{
    GT_U32 value;           /* value to read from the register  */
    GT_U32 offset;          /* bit number inside register       */
    GT_U32 regAddr;         /* register address                 */
    PRV_CPSS_PORT_TYPE_ENT portMacType;
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    /* cpssDxChPortMruSet updates all available MAC's - here enough to read */
    /* one of them */
    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum);

    if(portMacType >= PRV_CPSS_PORT_XG_E)
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portMacNum,portMacType,&regAddr);
        offset = PRV_CPSS_DXCH_PORT_MRU_FIELD_OFFSET_XG_CNS;
    }
    else
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,portMacType,&regAddr);
        offset = PRV_CPSS_DXCH_PORT_MRU_FIELD_OFFSET_GE_CNS;
    }
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    if (prvCpssDrvHwPpPortGroupGetRegField(devNum,
        PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
                                                                    portMacNum),
            regAddr, offset, PRV_CPSS_DXCH_PORT_MRU_FIELD_LENGTH_CNS, &value) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    *mruSizePtr = value<<1;

    return GT_OK;

}

/*******************************************************************************
* internal_cpssDxChPortMruGet
*
* DESCRIPTION:
*       Gets the Maximal Receive Packet size for specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (or CPU port)
*
* OUTPUTS:
*       mruSizePtr - (pointer to) max receive packet size in bytes. 10K+64 (including 4 bytes CRC)
*                    value must be even
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or
*                          odd value of mruSize
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - mruSize > 10304
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortMruGet
(
    IN   GT_U8  devNum,
    IN   GT_PHYSICAL_PORT_NUM  portNum,
    OUT  GT_U32 *mruSizePtr
)
{
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */
    GT_U32  targetMruSize;
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(mruSizePtr);

    /* Get PHY MAC object ptr */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portMacNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacMRUGetFunc(devNum,portMacNum,
                                  mruSizePtr,
                                  CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,
                                  &targetMruSize);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* set MRU size value for switch MAC */
    if (doPpMacConfig == GT_TRUE)
    {
        rc = prvCpssDxChPortMruGet(devNum,portNum,mruSizePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacMRUGetFunc(devNum,portMacNum,
                                  mruSizePtr,
                                  CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,
                                  &targetMruSize);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;

}

/*******************************************************************************
* cpssDxChPortMruGet
*
* DESCRIPTION:
*       Gets the Maximal Receive Packet size for specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (or CPU port)
*
* OUTPUTS:
*       mruSizePtr - (pointer to) max receive packet size in bytes. 10K+64 (including 4 bytes CRC)
*                    value must be even
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or
*                          odd value of mruSize
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - mruSize > 10304
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortMruGet
(
    IN   GT_U8  devNum,
    IN   GT_PHYSICAL_PORT_NUM  portNum,
    OUT  GT_U32 *mruSizePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMruGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, mruSizePtr));

    rc = internal_cpssDxChPortMruGet(devNum, portNum, mruSizePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, mruSizePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortMruProfileSet
*
* DESCRIPTION:
*       Set an MRU profile for port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number, CPU port
*                         In eArch devices portNum is default ePort.
*       profileId       - the profile index (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortMruProfileSet
(
    IN GT_U8            devNum,
    IN GT_PORT_NUM      portNum,
    IN GT_U32           profileId
)
{
    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);
    if (profileId > 7)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* write to pre-tti-lookup-ingress-eport table */
    return prvCpssDxChWriteTableEntryField(devNum,
                                           PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                           portNum,
                                           PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                           LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_MRU_INDEX_E, /* field name */
                                           PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                           profileId);
}

/*******************************************************************************
* cpssDxChPortMruProfileSet
*
* DESCRIPTION:
*       Set an MRU profile for port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number, CPU port
*                         In eArch devices portNum is default ePort.
*       profileId       - the profile index (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMruProfileSet
(
    IN GT_U8            devNum,
    IN GT_PORT_NUM      portNum,
    IN GT_U32           profileId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMruProfileSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, profileId));

    rc = internal_cpssDxChPortMruProfileSet(devNum, portNum, profileId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, profileId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortMruProfileGet
*
* DESCRIPTION:
*       Get an MRU profile for port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number, CPU port
*                         In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       profileIdPtr    - (pointer to) the profile index (APPLICABLE RANGES: 0..7)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortMruProfileGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *profileIdPtr
)
{
    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(profileIdPtr);

    /* write to pre-tti-lookup-ingress-eport table */
    return prvCpssDxChReadTableEntryField(devNum,
                                          PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                          portNum,
                                          PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                          LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_MRU_INDEX_E, /* field name */
                                          PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                          profileIdPtr);
}

/*******************************************************************************
* cpssDxChPortMruProfileGet
*
* DESCRIPTION:
*       Get an MRU profile for port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number, CPU port
*                         In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       profileIdPtr    - (pointer to) the profile index (APPLICABLE RANGES: 0..7)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMruProfileGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *profileIdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMruProfileGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, profileIdPtr));

    rc = internal_cpssDxChPortMruProfileGet(devNum, portNum, profileIdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, profileIdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortProfileMruSizeSet
*
* DESCRIPTION:
*       Set an MRU size for profile
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum     - device number
*       profile    - the profile (APPLICABLE RANGES: 0..7)
*       mruSize    - max receive packet size in bytes
*                    (APPLICABLE RANGES: 0..16383)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, profile or MRU size
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortProfileMruSizeSet
(
    IN GT_U8    devNum,
    IN GT_U32   profile,
    IN GT_U32   mruSize
)
{
    GT_U32 regAddr;
    GT_U32 offset;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if (profile > 7)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    else
    {
        offset = ((profile % 2) ? 14 : 0);
    }

    if (mruSize > 16383)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).MRUException.MRUSize[profile / 2];

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, offset, 14, mruSize);
}

/*******************************************************************************
* cpssDxChPortProfileMruSizeSet
*
* DESCRIPTION:
*       Set an MRU size for profile
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum     - device number
*       profile    - the profile (APPLICABLE RANGES: 0..7)
*       mruSize    - max receive packet size in bytes
*                    (APPLICABLE RANGES: 0..16383)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, profile or MRU size
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortProfileMruSizeSet
(
    IN GT_U8    devNum,
    IN GT_U32   profile,
    IN GT_U32   mruSize
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortProfileMruSizeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, profile, mruSize));

    rc = internal_cpssDxChPortProfileMruSizeSet(devNum, profile, mruSize);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, profile, mruSize));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortProfileMruSizeGet
*
* DESCRIPTION:
*       Get an MRU size for profile
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum     - device number
*       profile    - the profile (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       mruSizePtr - (pointer to) max receive packet size in bytes
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device or profile
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortProfileMruSizeGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  profile,
    OUT GT_U32  *mruSizePtr
)
{
    GT_U32 regAddr;
    GT_U32 offset;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(mruSizePtr);

    if (profile > 7)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    else
    {
        offset = ((profile % 2) ? 14 : 0);
    }

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).MRUException.MRUSize[profile / 2];

    return prvCpssDrvHwPpGetRegField(devNum, regAddr, offset, 14, mruSizePtr);
}

/*******************************************************************************
* cpssDxChPortProfileMruSizeGet
*
* DESCRIPTION:
*       Get an MRU size for profile
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum     - device number
*       profile    - the profile (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       mruSizePtr - (pointer to) max receive packet size in bytes
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device or profile
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortProfileMruSizeGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  profile,
    OUT GT_U32  *mruSizePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortProfileMruSizeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, profile, mruSizePtr));

    rc = internal_cpssDxChPortProfileMruSizeGet(devNum, profile, mruSizePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, profile, mruSizePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortMruExceptionCommandSet
*
* DESCRIPTION:
*       Set the command assigned to frames that exceed the default ePort MRU
*       size
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum     - device number
*       command    - the command. valid values:
*                       CPSS_PACKET_CMD_FORWARD_E
*                       CPSS_PACKET_CMD_MIRROR_TO_CPU_E
*                       CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                       CPSS_PACKET_CMD_DROP_HARD_E
*                       CPSS_PACKET_CMD_DROP_SOFT_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device or command
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortMruExceptionCommandSet
(
    IN GT_U8                    devNum,
    IN CPSS_PACKET_CMD_ENT      command
)
{
    GT_U32  regAddr;
    GT_U32  value;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,command);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).MRUException.MRUCommandAndCode;
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 8, 3, value);
}

/*******************************************************************************
* cpssDxChPortMruExceptionCommandSet
*
* DESCRIPTION:
*       Set the command assigned to frames that exceed the default ePort MRU
*       size
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum     - device number
*       command    - the command. valid values:
*                       CPSS_PACKET_CMD_FORWARD_E
*                       CPSS_PACKET_CMD_MIRROR_TO_CPU_E
*                       CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                       CPSS_PACKET_CMD_DROP_HARD_E
*                       CPSS_PACKET_CMD_DROP_SOFT_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device or command
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMruExceptionCommandSet
(
    IN GT_U8                    devNum,
    IN CPSS_PACKET_CMD_ENT      command
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMruExceptionCommandSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, command));

    rc = internal_cpssDxChPortMruExceptionCommandSet(devNum, command);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, command));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortMruExceptionCommandGet
*
* DESCRIPTION:
*       Get the command assigned to frames that exceed the default ePort MRU
*       size
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       commandPtr - (pointer to) the command
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortMruExceptionCommandGet
(
    IN  GT_U8                   devNum,
    OUT CPSS_PACKET_CMD_ENT     *commandPtr
)
{
    GT_U32      regAddr;
    GT_U32      value;
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(commandPtr);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).MRUException.MRUCommandAndCode;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 8, 3, &value);
    if (rc == GT_OK)
    {
        PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(*commandPtr,value);
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortMruExceptionCommandGet
*
* DESCRIPTION:
*       Get the command assigned to frames that exceed the default ePort MRU
*       size
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       commandPtr - (pointer to) the command
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMruExceptionCommandGet
(
    IN  GT_U8                   devNum,
    OUT CPSS_PACKET_CMD_ENT     *commandPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMruExceptionCommandGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, commandPtr));

    rc = internal_cpssDxChPortMruExceptionCommandGet(devNum, commandPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, commandPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortMruExceptionCpuCodeSet
*
* DESCRIPTION:
*       Set the CPU/drop code assigned to a frame which fails the MRU check
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum     - device number
*       cpuCode    - the CPU/drop code
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device or cpu/drop code
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortMruExceptionCpuCodeSet
(
    IN GT_U8                        devNum,
    IN CPSS_NET_RX_CPU_CODE_ENT     cpuCode
)
{
    GT_U32                                  regAddr;
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT  dsaCpuCode;
    GT_STATUS                               rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).MRUException.MRUCommandAndCode;

    rc = prvCpssDxChNetIfCpuToDsaCode(cpuCode, &dsaCpuCode);
    if (rc != GT_OK)
    {
        return rc;
    }

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 8, (GT_U32)dsaCpuCode);
}

/*******************************************************************************
* cpssDxChPortMruExceptionCpuCodeSet
*
* DESCRIPTION:
*       Set the CPU/drop code assigned to a frame which fails the MRU check
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum     - device number
*       cpuCode    - the CPU/drop code
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device or cpu/drop code
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMruExceptionCpuCodeSet
(
    IN GT_U8                        devNum,
    IN CPSS_NET_RX_CPU_CODE_ENT     cpuCode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMruExceptionCpuCodeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cpuCode));

    rc = internal_cpssDxChPortMruExceptionCpuCodeSet(devNum, cpuCode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cpuCode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortMruExceptionCpuCodeGet
*
* DESCRIPTION:
*       Get the CPU/drop code assigned to a frame which fails the MRU check
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       cpuCodePtr - the CPU/drop code
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortMruExceptionCpuCodeGet
(
    IN  GT_U8                       devNum,
    OUT CPSS_NET_RX_CPU_CODE_ENT    *cpuCodePtr
)
{
    GT_U32      regAddr;
    GT_U32      dsaCpuCode;
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(cpuCodePtr);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).MRUException.MRUCommandAndCode;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 8, &dsaCpuCode);
    if (rc != GT_OK)
    {
        return rc;
    }

    return prvCpssDxChNetIfDsaToCpuCode((PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT)dsaCpuCode,
                                        cpuCodePtr);
}

/*******************************************************************************
* cpssDxChPortMruExceptionCpuCodeGet
*
* DESCRIPTION:
*       Get the CPU/drop code assigned to a frame which fails the MRU check
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       cpuCodePtr - the CPU/drop code
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMruExceptionCpuCodeGet
(
    IN  GT_U8                       devNum,
    OUT CPSS_NET_RX_CPU_CODE_ENT    *cpuCodePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMruExceptionCpuCodeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cpuCodePtr));

    rc = internal_cpssDxChPortMruExceptionCpuCodeGet(devNum, cpuCodePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cpuCodePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortCrcCheckEnableSet
*
* DESCRIPTION:
*       Enable/Disable 32-bit the CRC checking.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (or CPU port)
*       enable     - If GT_TRUE, enable CRC checking
*                    If GT_FALSE, disable CRC checking
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
static GT_STATUS prvCpssDxChPortCrcCheckEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL     enable
)
{
    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* value to write into the register */
    GT_U32 offset;          /* bit number inside register       */
    GT_U32  portMacNum;      /* MAC number */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    value = (enable == GT_TRUE) ? 1 : 0;

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_GE_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_GE_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = 0;
    }

    offset = ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
              (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
              (PRV_CPSS_SIP_5_CHECK_MAC(devNum))) ? 9 : 10;

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldOffset = offset;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XLG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XLG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldOffset = offset;
    }

    return prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
}

/*******************************************************************************
* internal_cpssDxChPortCrcCheckEnableSet
*
* DESCRIPTION:
*       Enable/Disable 32-bit the CRC checking.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (or CPU port)
*       enable     - If GT_TRUE, enable CRC checking
*                    If GT_FALSE, disable CRC checking
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
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCrcCheckEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL     enable
)
{
    GT_BOOL     targetEnable = enable;
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portMacNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacCRCCheckSetFunc(devNum,portMacNum,
                                  targetEnable,
                                  CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,
                                  &targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* set Port Crc Check Enable value for switch MAC */
    if (doPpMacConfig == GT_TRUE)
    {
        rc = prvCpssDxChPortCrcCheckEnableSet(devNum,portNum,targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacCRCCheckSetFunc(devNum,portMacNum,
                                  targetEnable,
                                  CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,
                                  &targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortCrcCheckEnableSet
*
* DESCRIPTION:
*       Enable/Disable 32-bit the CRC checking.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (or CPU port)
*       enable     - If GT_TRUE, enable CRC checking
*                    If GT_FALSE, disable CRC checking
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
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*******************************************************************************/
GT_STATUS cpssDxChPortCrcCheckEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL     enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCrcCheckEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChPortCrcCheckEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortCrcCheckEnableGet
*
* DESCRIPTION:
*       Get CRC checking (Enable/Disable) state for received packets.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr - Pointer to the CRS checking state :
*                       GT_TRUE  - CRC checking is enable,
*                       GT_FALSE - CRC checking is disable.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortCrcCheckEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* value to write into the register */
    GT_U32 offset;          /* bit number inside register       */
    GT_STATUS rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT portMacType;
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);
    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum);

    if(portMacType >= PRV_CPSS_PORT_XG_E)
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,portMacType,&regAddr);
        offset = ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
                  (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
                  (PRV_CPSS_SIP_5_CHECK_MAC(devNum))) ? 9 : 10;
    }
    else
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portMacNum,portMacType,&regAddr);
        offset = 0;
    }

    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    /* get state */
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, offset, 1, &value);
    if (rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }
    *enablePtr = (value == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortCrcCheckEnableGet
*
* DESCRIPTION:
*       Get CRC checking (Enable/Disable) state for received packets.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr - Pointer to the CRS checking state :
*                       GT_TRUE  - CRC checking is enable,
*                       GT_FALSE - CRC checking is disable.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCrcCheckEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL     *enablePtr
)
{
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portMacNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacCRCCheckGetFunc(devNum,portMacNum,
                                  enablePtr,
                                  CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,
                                  enablePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* get Port CRC Check Enable value from switch MAC  */
    if (doPpMacConfig == GT_TRUE)
    {
        rc = prvCpssDxChPortCrcCheckEnableGet(devNum,portNum,enablePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacCRCCheckGetFunc(devNum,portMacNum,
                                          enablePtr,
                                          CPSS_MACDRV_STAGE_POST_E,
                                          &doPpMacConfig,
                                          enablePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortCrcCheckEnableGet
*
* DESCRIPTION:
*       Get CRC checking (Enable/Disable) state for received packets.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr - Pointer to the CRS checking state :
*                       GT_TRUE  - CRC checking is enable,
*                       GT_FALSE - CRC checking is disable.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*******************************************************************************/
GT_STATUS cpssDxChPortCrcCheckEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCrcCheckEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChPortCrcCheckEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortXGmiiModeSet
*
* DESCRIPTION:
*       Sets XGMII mode on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       mode  - XGMII mode
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortXGmiiModeSet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  CPSS_PORT_XGMII_MODE_ENT    mode
)
{
    GT_U32  value;          /* value to write into the register */
    GT_U32  offset;         /* field offset */
    GT_U32  regAddr;        /* register address */
    GT_U32  portMacNum;     /* MAC number */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    switch(mode)
    {
        case CPSS_PORT_XGMII_LAN_E:
            value = 0;
            break;
        case CPSS_PORT_XGMII_WAN_E:
            value = 1;
            break;
        case CPSS_PORT_XGMII_FIXED_E:
            value = 2;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
       (PRV_CPSS_SIP_5_CHECK_MAC(devNum)))
    {
        offset = 5;
    }
    else
    {
        offset = 6;
    }

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldLength = 2;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldOffset = offset;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XLG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XLG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldLength = 2;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldOffset = offset;
    }

    return prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
}

/*******************************************************************************
* cpssDxChPortXGmiiModeSet
*
* DESCRIPTION:
*       Sets XGMII mode on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       mode  - XGMII mode
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortXGmiiModeSet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  CPSS_PORT_XGMII_MODE_ENT    mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortXGmiiModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, mode));

    rc = internal_cpssDxChPortXGmiiModeSet(devNum, portNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortXGmiiModeGet
*
* DESCRIPTION:
*       Gets XGMII mode on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
* OUTPUTS:
*       modePtr  - Pointer to XGMII mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortXGmiiModeGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    OUT CPSS_PORT_XGMII_MODE_ENT    *modePtr
)
{
    GT_U32 value;           /* value read from the register */
    GT_U32 offset; /* field offset */
    PRV_CPSS_PORT_TYPE_ENT portMacType;
    GT_U32          regAddr;         /* register address */
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum);

    if(portMacType < PRV_CPSS_PORT_XG_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
       (PRV_CPSS_SIP_5_CHECK_MAC(devNum)))
    {
        offset = 5;
    }
    else
    {
        offset = 6;
    }

    /* get xgmii mode */
    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,portMacType,&regAddr);
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    if (prvCpssDrvHwPpPortGroupGetRegField(devNum,
            PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum),
            regAddr, offset, 2, &value) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);

    switch(value)
    {
        case 0:
            *modePtr = CPSS_PORT_XGMII_LAN_E;
            break;
        case 1:
            *modePtr = CPSS_PORT_XGMII_WAN_E;
            break;
        case 2:
            *modePtr = CPSS_PORT_XGMII_FIXED_E;
            break;
        default:
            break;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortXGmiiModeGet
*
* DESCRIPTION:
*       Gets XGMII mode on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
* OUTPUTS:
*       modePtr  - Pointer to XGMII mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortXGmiiModeGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    OUT CPSS_PORT_XGMII_MODE_ENT    *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortXGmiiModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, modePtr));

    rc = internal_cpssDxChPortXGmiiModeGet(devNum, portNum, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortIpgBaseSet
*
* DESCRIPTION:
*       Sets IPG base for fixed IPG mode on XG ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       ipgBase  - IPG base
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, device or ipgBase
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant only when XG Port is in CPSS_PORT_XGMII_FIXED_E mode.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortIpgBaseSet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  CPSS_PORT_XG_FIXED_IPG_ENT  ipgBase
)
{
    GT_U32 value;   /* value to write into the register */
    GT_U32 offset;  /* bit number inside register       */
    GT_U32          regAddr;         /* register address */
    GT_U32  portMacNum;      /* MAC number */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    offset = ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E) ||
              (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)  ||
              (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
              (PRV_CPSS_SIP_5_CHECK_MAC(devNum)))? 9 : 10;

    switch(ipgBase)
    {
        case CPSS_PORT_XG_FIXED_IPG_12_BYTES_E: value = 0;
                                                break;
        case CPSS_PORT_XG_FIXED_IPG_8_BYTES_E:  value = 1;
                                                break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldOffset = offset;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XLG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XLG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldOffset = offset;
    }

    return prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
}

/*******************************************************************************
* cpssDxChPortIpgBaseSet
*
* DESCRIPTION:
*       Sets IPG base for fixed IPG mode on XG ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       ipgBase  - IPG base
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, device or ipgBase
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant only when XG Port is in CPSS_PORT_XGMII_FIXED_E mode.
*
*******************************************************************************/
GT_STATUS cpssDxChPortIpgBaseSet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  CPSS_PORT_XG_FIXED_IPG_ENT  ipgBase
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortIpgBaseSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, ipgBase));

    rc = internal_cpssDxChPortIpgBaseSet(devNum, portNum, ipgBase);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, ipgBase));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortIpgBaseGet
*
* DESCRIPTION:
*       Gets IPG base for fixed IPG mode on XG ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
* OUTPUTS:
*       ipgBasePtr  - pointer to IPG base
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       Relevant only when XG Port is in CPSS_PORT_XGMII_FIXED_E mode.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortIpgBaseGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    OUT CPSS_PORT_XG_FIXED_IPG_ENT  *ipgBasePtr
)
{
    GT_U32 value;   /* value read from the register */
    GT_U32 offset;  /* bit number inside register   */
    PRV_CPSS_PORT_TYPE_ENT portMacType;
    GT_U32          regAddr;         /* register address */
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(ipgBasePtr);

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum);

    if(portMacType < PRV_CPSS_PORT_XG_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    offset = ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E) ||
              (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)  ||
              (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
              (PRV_CPSS_SIP_5_CHECK_MAC(devNum))) ? 9 : 10;
    PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portMacNum,portMacType,&regAddr);
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    /* set xgmii mode */
    if (prvCpssDrvHwPpPortGroupGetRegField(devNum,
                PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum),
                regAddr, offset, 1, &value) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);

    *ipgBasePtr = (value == 0) ? CPSS_PORT_XG_FIXED_IPG_12_BYTES_E :
                                 CPSS_PORT_XG_FIXED_IPG_8_BYTES_E;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortIpgBaseGet
*
* DESCRIPTION:
*       Gets IPG base for fixed IPG mode on XG ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
* OUTPUTS:
*       ipgBasePtr  - pointer to IPG base
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       Relevant only when XG Port is in CPSS_PORT_XGMII_FIXED_E mode.
*
*******************************************************************************/
GT_STATUS cpssDxChPortIpgBaseGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    OUT CPSS_PORT_XG_FIXED_IPG_ENT  *ipgBasePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortIpgBaseGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, ipgBasePtr));

    rc = internal_cpssDxChPortIpgBaseGet(devNum, portNum, ipgBasePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, ipgBasePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/****************************************************************************
* internal_cpssDxChPortIpgSet
*
* DESCRIPTION:
*       Sets the Inter-Packet Gap (IPG) interval of a tri-speed physical port.
*       Using this API may be required to enable wire-speed in traffic paths
*       that include cascading ports, where it may not be feasible to reduce
*       the preamble length.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical or CPU port number
*       ipg         - IPG in bytes, acceptable range:
*                     (APPLICABLE RANGES: 0..511) (APPLICABLE DEVICES: DxCh3 and above)
*                     (APPLICABLE RANGES: 0..15) (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2.)
*                     Default HW value is 12 bytes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on wrong port type
*       GT_OUT_OF_RANGE  - ipg value out of range
*       GT_BAD_PARAM     - on bad parameter
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortIpgSet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    IN  GT_U32  ipg
)
{
    GT_STATUS rc;
    GT_U32 regAddr; /* register address */
    GT_U32  portMacNum;      /* MAC number */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    if (PRV_CPSS_SIP_5_CHECK_MAC(devNum) ||
        PRV_CPSS_PP_MAC(devNum)->devFamily ==  CPSS_PP_FAMILY_DXCH_XCAT3_E)
    {
        rc = prvCpssDxChBobcat2PortMacIPGLengthSet(devNum,portNum,ipg);

        return rc;
    }

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    /* set */
    if(PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))
    {
        if(ipg >= BIT_9)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

        PRV_CPSS_DXCH_PORT_MAC_CTRL3_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_GE_E,&regAddr);
        if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

        regDataArray[PRV_CPSS_PORT_GE_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldData = ipg;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = 9;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = 6;
    }
    else
    {
        if(ipg >= BIT_4)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

        if(portNum != CPSS_CPU_PORT_NUM_CNS)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                                        perPortRegs[portNum].serialParameters;
        }
        else /* CPU port */
        {
             regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                                                    cpuPortRegs.serialParameters;
        }

        if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

        regDataArray[PRV_CPSS_PORT_GE_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldData = ipg;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = 4;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = 0;
    }

    return prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
}

/****************************************************************************
* cpssDxChPortIpgSet
*
* DESCRIPTION:
*       Sets the Inter-Packet Gap (IPG) interval of a tri-speed physical port.
*       Using this API may be required to enable wire-speed in traffic paths
*       that include cascading ports, where it may not be feasible to reduce
*       the preamble length.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical or CPU port number
*       ipg         - IPG in bytes, acceptable range:
*                     (APPLICABLE RANGES: 0..511) (APPLICABLE DEVICES: DxCh3 and above)
*                     (APPLICABLE RANGES: 0..15) (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2.)
*                     Default HW value is 12 bytes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on wrong port type
*       GT_OUT_OF_RANGE  - ipg value out of range
*       GT_BAD_PARAM     - on bad parameter
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortIpgSet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    IN  GT_U32  ipg
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortIpgSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, ipg));

    rc = internal_cpssDxChPortIpgSet(devNum, portNum, ipg);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, ipg));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/****************************************************************************
* internal_cpssDxChPortIpgGet
*
* DESCRIPTION:
*       Gets the Inter-Packet Gap (IPG) interval of a tri-speed physical port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical or CPU port number
*
* OUTPUTS:
*       ipgPtr      - (pointer to) IPG value in bytes
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on wrong port type
*       GT_BAD_PARAM     - on bad parameter
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortIpgGet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT  GT_U32  *ipgPtr
)
{
    GT_U32 regAddr; /* register address */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(ipgPtr);


    if (PRV_CPSS_SIP_5_CHECK_MAC(devNum) ||
        PRV_CPSS_PP_MAC(devNum)->devFamily ==  CPSS_PP_FAMILY_DXCH_XCAT3_E)
    {
        GT_STATUS rcXLG;
        GT_STATUS rcGE;
        GT_U32 lengthXLG;
        GT_U32 lengthGE;

        *ipgPtr = 0;
        rcXLG = prvCpssDxChBobcat2PortMacIPGLengthGet(devNum,portNum,PRV_CPSS_PORT_XLG_E,&lengthXLG);
        rcGE  = prvCpssDxChBobcat2PortMacIPGLengthGet(devNum,portNum,PRV_CPSS_PORT_GE_E, &lengthGE);
        if (rcXLG != GT_OK && rcGE != GT_OK)
        {
            return rcXLG;
        }
        if (rcXLG == GT_OK)  /* it is not proper, current MAC shall be used, or interface shall be changed */
        {
            *ipgPtr = lengthXLG;
        }
        else
        {
            *ipgPtr = lengthGE;
        }
        return GT_OK;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);

    /* get */
    if(PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL3_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_GE_E,&regAddr);
        if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

        return prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 6, 9, ipgPtr);
    }
    else
    {
        PRV_CPSS_DXCH_PORT_SERIAL_PARAM_CTRL_REG_MAC(devNum,portMacNum,&regAddr);
        if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

        return prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 0, 4, ipgPtr);
    }
}

/****************************************************************************
* cpssDxChPortIpgGet
*
* DESCRIPTION:
*       Gets the Inter-Packet Gap (IPG) interval of a tri-speed physical port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical or CPU port number
*
* OUTPUTS:
*       ipgPtr      - (pointer to) IPG value in bytes
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on wrong port type
*       GT_BAD_PARAM     - on bad parameter
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortIpgGet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT  GT_U32  *ipgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortIpgGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, ipgPtr));

    rc = internal_cpssDxChPortIpgGet(devNum, portNum, ipgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, ipgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortExtraIpgSet
*
* DESCRIPTION:
*       Sets the number of 32-bit words to add to the 12-byte IPG.
*       Hence, 12+4*ExtraIPG is the basis for the entire IPG calculation.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       number   -  number of words
*                (APPLICABLE RANGES: 0..127) (APPLICABLE DEVICES: DxCh3 and above)
*                (APPLICABLE RANGES: 0..3) (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - the number is out of range
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortExtraIpgSet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_U8       number
)
{
    GT_U32 value;           /* value to write into the register */
    GT_U32  regAddr;    /* register address */
    GT_U32 offset;      /* bit number inside register       */
    GT_U32 sizeOfValue; /* number of bits to write to the register       */
    GT_U32  portMacNum;      /* MAC number */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    value = number;

    /* set */
    if(PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))
    {
        if(value >= BIT_7)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
        sizeOfValue=7;
        offset=6;
    }
    else
    {
        if(value >= BIT_2)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
        sizeOfValue=2;
        offset=4;
    }

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    if(PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))
    {
       PRV_CPSS_DXCH_PORT_MAC_CTRL3_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XG_E,&regAddr);
    }
    else
    {
       PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,PRV_CPSS_PORT_XG_E,&regAddr);
    }
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldLength = sizeOfValue;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldOffset = offset;
    }

    if(PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))
    {
       PRV_CPSS_DXCH_PORT_MAC_CTRL3_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XLG_E,&regAddr);
    }
    else
    {
       PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,PRV_CPSS_PORT_XLG_E,&regAddr);
    }
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XLG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldLength = sizeOfValue;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldOffset = offset;
    }

    return prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
}

/*******************************************************************************
* cpssDxChPortExtraIpgSet
*
* DESCRIPTION:
*       Sets the number of 32-bit words to add to the 12-byte IPG.
*       Hence, 12+4*ExtraIPG is the basis for the entire IPG calculation.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       number   -  number of words
*                (APPLICABLE RANGES: 0..127) (APPLICABLE DEVICES: DxCh3 and above)
*                (APPLICABLE RANGES: 0..3) (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2.)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - the number is out of range
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortExtraIpgSet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_U8       number
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortExtraIpgSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, number));

    rc = internal_cpssDxChPortExtraIpgSet(devNum, portNum, number);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, number));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortExtraIpgGet
*
* DESCRIPTION:
*       Gets the number of 32-bit words to add to the 12-byte IPG.
*       Hence, 12+4*ExtraIPG is the basis for the entire IPG calculation.
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
*
* OUTPUTS:
*       numberPtr -  pointer to number of words
*                (APPLICABLE RANGES: 0..127) (APPLICABLE DEVICES: DxCh3 and above)
*                (APPLICABLE RANGES: 0..3) (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2.)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - the number is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortExtraIpgGet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_U8       *numberPtr
)
{
    GT_U32 value;           /* value to write into the register */
    GT_U32  regAddr;
    GT_STATUS rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT portMacType;
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(numberPtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);
    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum);

    if(portMacType < PRV_CPSS_PORT_XG_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))
    {

        PRV_CPSS_DXCH_PORT_MAC_CTRL3_REG_MAC(devNum,portMacNum,portMacType,&regAddr);
        if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId,regAddr,6, 7, &value);
    }
    else
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);
        if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId,regAddr,4, 2, &value);
    }
    if (rc != GT_OK)
        return rc;

    *numberPtr = (GT_U8)value;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortExtraIpgGet
*
* DESCRIPTION:
*       Gets the number of 32-bit words to add to the 12-byte IPG.
*       Hence, 12+4*ExtraIPG is the basis for the entire IPG calculation.
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
*
* OUTPUTS:
*       numberPtr -  pointer to number of words
*                (APPLICABLE RANGES: 0..127) (APPLICABLE DEVICES: DxCh3 and above)
*                (APPLICABLE RANGES: 0..3) (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2.)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - the number is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortExtraIpgGet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_U8       *numberPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortExtraIpgGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, numberPtr));

    rc = internal_cpssDxChPortExtraIpgGet(devNum, portNum, numberPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, numberPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortXgmiiLocalFaultGet
*
* DESCRIPTION:
*       Reads bit then indicate if the XGMII RS has detected local
*       fault messages.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number
*
* OUTPUTS:
*       isLocalFaultPtr - GT_TRUE, if bit set or
*                         GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortXgmiiLocalFaultGet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL     *isLocalFaultPtr
)
{
    GT_U32 value;
    GT_U32  regAddr;
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(isLocalFaultPtr);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) < PRV_CPSS_PORT_XG_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    PRV_CPSS_DXCH_PORT_STATUS_CTRL_REG_MAC(devNum,portMacNum,&regAddr);
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    if (prvCpssDrvHwPpPortGroupGetRegField(devNum,
        PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
                                                                    portMacNum),
                regAddr, 2, 1, &value) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    *isLocalFaultPtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortXgmiiLocalFaultGet
*
* DESCRIPTION:
*       Reads bit then indicate if the XGMII RS has detected local
*       fault messages.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number
*
* OUTPUTS:
*       isLocalFaultPtr - GT_TRUE, if bit set or
*                         GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortXgmiiLocalFaultGet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL     *isLocalFaultPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortXgmiiLocalFaultGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, isLocalFaultPtr));

    rc = internal_cpssDxChPortXgmiiLocalFaultGet(devNum, portNum, isLocalFaultPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, isLocalFaultPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortXgmiiRemoteFaultGet
*
* DESCRIPTION:
*       Reads bit then indicate if the XGMII RS has detected remote
*       fault messages.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number
*       portNum         - physical port number
*
* OUTPUTS:
*       isRemoteFaultPtr  - GT_TRUE, if bit set or
*                           GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortXgmiiRemoteFaultGet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL     *isRemoteFaultPtr
)
{
    GT_U32 value;
    GT_U32  regAddr;
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(isRemoteFaultPtr);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) < PRV_CPSS_PORT_XG_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    PRV_CPSS_DXCH_PORT_STATUS_CTRL_REG_MAC(devNum,portMacNum,&regAddr);
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    if (prvCpssDrvHwPpPortGroupGetRegField(devNum,
            PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
                                                                        portMacNum),
                regAddr, 1, 1, &value) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    *isRemoteFaultPtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortXgmiiRemoteFaultGet
*
* DESCRIPTION:
*       Reads bit then indicate if the XGMII RS has detected remote
*       fault messages.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number
*       portNum         - physical port number
*
* OUTPUTS:
*       isRemoteFaultPtr  - GT_TRUE, if bit set or
*                           GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortXgmiiRemoteFaultGet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL     *isRemoteFaultPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortXgmiiRemoteFaultGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, isRemoteFaultPtr));

    rc = internal_cpssDxChPortXgmiiRemoteFaultGet(devNum, portNum, isRemoteFaultPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, isRemoteFaultPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortMacStatusGet
*
* DESCRIPTION:
*       Reads bits that indicate different problems on specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
* OUTPUTS:
*       portMacStatusPtr  - info about port MAC
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortMacStatusGet
(
    IN  GT_U8                      devNum,
    IN  GT_PHYSICAL_PORT_NUM       portNum,
    OUT CPSS_PORT_MAC_STATUS_STC   *portMacStatusPtr
)
{
    GT_U32 regAddr;         /* register address */
    GT_U32 value;
    GT_U32 portMacNum;      /* MAC number */
    GT_U32 xoff_status = 1; /* The status is XOFF when xoff_status = 0 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(portMacStatusPtr);

    PRV_CPSS_DXCH_PORT_STATUS_CTRL_REG_MAC(devNum,portMacNum,&regAddr);
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    if (prvCpssDrvHwPpPortGroupReadRegister(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum),
            regAddr, &value) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);

    /* Read XOFF status from FCA DB register0 */
    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        if(prvCpssHwPpGetRegField(devNum, PRV_DXCH_REG1_UNIT_GOP_FCA_MAC(devNum, portMacNum).DBReg0, 0, 1, &xoff_status) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
    }

    /*  10 Gb interface */
    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) >= PRV_CPSS_PORT_XG_E)
    {
        portMacStatusPtr->isPortRxPause  =    (value & 0x40) ? GT_TRUE : GT_FALSE;
        portMacStatusPtr->isPortTxPause  =    (value & 0x80) ? GT_TRUE : GT_FALSE;

        /* Return XOFF status from FCA DB register0 */
        if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
        {
            if(xoff_status == 0)
            {
                portMacStatusPtr->isPortTxPause = GT_TRUE;
            }
            else
            {
                portMacStatusPtr->isPortTxPause = GT_FALSE;
            }
        }

        portMacStatusPtr->isPortBackPres =    GT_FALSE;
        portMacStatusPtr->isPortBufFull  =    (value & 0x100) ? GT_TRUE : GT_FALSE;/* Bit#8 */
        portMacStatusPtr->isPortSyncFail =    (value & 0x1) ? GT_FALSE : GT_TRUE; /* value 0 -> SyncFail */
        portMacStatusPtr->isPortHiErrorRate = GT_FALSE;
        portMacStatusPtr->isPortAnDone =      GT_FALSE;
        portMacStatusPtr->isPortFatalError =  (value & 0x6) ? GT_TRUE : GT_FALSE; /* Bit#1 Or Bit#2 */
    }
    else
    {
        portMacStatusPtr->isPortRxPause = (value & 0x40) ? GT_TRUE : GT_FALSE;
        portMacStatusPtr->isPortTxPause = (value & 0x80) ? GT_TRUE : GT_FALSE;
        portMacStatusPtr->isPortBackPres = (value & 0x100) ? GT_TRUE : GT_FALSE;
        portMacStatusPtr->isPortBufFull = (value & 0x200) ? GT_TRUE : GT_FALSE;
        portMacStatusPtr->isPortSyncFail = (value & 0x400) ? GT_TRUE : GT_FALSE;
        portMacStatusPtr->isPortAnDone = (value & 0x800) ? GT_TRUE : GT_FALSE;

        portMacStatusPtr->isPortHiErrorRate = GT_FALSE;
        portMacStatusPtr->isPortFatalError = GT_FALSE;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortMacStatusGet
*
* DESCRIPTION:
*       Reads bits that indicate different problems on specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
* OUTPUTS:
*       portMacStatusPtr  - info about port MAC
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacStatusGet
(
    IN  GT_U8                      devNum,
    IN  GT_PHYSICAL_PORT_NUM       portNum,
    OUT CPSS_PORT_MAC_STATUS_STC   *portMacStatusPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMacStatusGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, portMacStatusPtr));

    rc = internal_cpssDxChPortMacStatusGet(devNum, portNum, portMacStatusPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, portMacStatusPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortLion2GeLinkStatusWaEnableSet
*
* DESCRIPTION:
*       Enable/disable WA for FE-4933007 (In Lion2 port in tri-speed mode link
*       status doesn't change in some cases when cable is disconnected/connected.)
*       Must disable WA if any type of loopback defined on GE port to see link up.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number (not CPU port)
*       enable    - If GT_TRUE, enable WA
*                   If GT_FALSE, disable WA
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
GT_STATUS prvCpssDxChPortLion2GeLinkStatusWaEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      portMacNum;      /* MAC number */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    PRV_CPSS_DXCH_PORT_MAC_CTRL4_REG_MAC(devNum, portMacNum, PRV_CPSS_PORT_GE_E,
                                                                    &regAddr);
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    regDataArray[PRV_CPSS_PORT_GE_E].regAddr = regAddr;
    regDataArray[PRV_CPSS_PORT_GE_E].fieldData = BOOL2BIT_MAC(enable);
    regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = 1;
    regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = 5;

    return prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
}

/*******************************************************************************
* prvCpssDxChPortLion2InternalLoopbackEnableSet
*
* DESCRIPTION:
*       Configure MAC and PCS TX2RX loopbacks on port.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number (not CPU port)
*       enable    - If GT_TRUE, enable loopback
*                   If GT_FALSE, disable loopback
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
GT_STATUS prvCpssDxChPortLion2InternalLoopbackEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable
)
{
    GT_STATUS   rc; /* return code */
    GT_U32      portGroupId;    /* core number of port */
    GT_U32      localPort;      /* port number in local core */
    MV_HWS_PORT_STANDARD    portMode;   /* port interface in HWS format */
    MV_HWS_PORT_STANDARD    portModeCombo;   /* port interface in HWS format for
                                                    non-active combo MAC member*/
    MV_HWS_PORT_LB_TYPE     lbType;     /* loopback type in HWS format */
    GT_U32      portState = 1;  /* port enable state */
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode; /* current interface of port */
    CPSS_PORT_SPEED_ENT             speed;  /* current speed of port */
    GT_U32      portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    ifMode = PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portMacNum);
    speed = PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum, portMacNum);
    if((CPSS_PORT_INTERFACE_MODE_NA_E == ifMode) || (CPSS_PORT_SPEED_NA_E == speed))
    {/* loopback on port which interface not defined yet forbidden */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portMacNum);

    if (PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                                     PRV_CPSS_DXCH_LION2_GIGE_MAC_LINK_STATUS_WA_E))
    {/* disable WA to let link up, when loopback enabled */
        rc = prvCpssDxChPortLion2GeLinkStatusWaEnableSet(devNum,portNum,!enable);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                               PRV_CPSS_DXCH_LION2_DISMATCH_PORTS_LINK_WA_E))
    {
        if(enable)
        {/* restore application configuration for force link down if loopback enabled */
            rc = prvCpssDxChPortForceLinkDownEnableSet(devNum,portNum,
           CPSS_PORTS_BMP_IS_PORT_SET_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                              info_PRV_CPSS_DXCH_LION2_DISMATCH_PORT_LINK_WA_E.
                                portForceLinkDownBmpPtr,portNum));
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    lbType = (enable) ? TX_2_RX_LB : DISABLE_LB;

    rc = prvCpssLion2CpssIfModeToHwsTranslate(ifMode, speed, &portMode);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* disable port always because loopback procedure resets and unreset
       port's MAC. Traffic should be avoided during reset/unreset of MAC to
       avoid corrupted packets */
    if (prvCpssDxChPortStateDisableSet(devNum, portNum, &portState) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                               PRV_CPSS_DXCH_LION2_PORT_MAC_MIB_COUNTERS_CORRUPTION_WA_E))
    {
        /* read MAC counters and store values in shadow */
        rc = prvCpssDxChPortMacCountersOnPortGet(devNum,portNum, GT_FALSE, NULL,
                                                 PRV_DXCH_PORT_MAC_CNTR_READ_MODE_UPDATE_SHADOW_E);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(portMode != HGL) /* HGL doesn't support MAC loopback */
    {
        CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPortLoopbackSet(devNum[%d], portGroupId[%d], phyPortNum[%d], portMode[%d], lpPlace[%d], lbType[%d])",devNum, portGroupId, localPort, portMode, HWS_MAC, lbType);
        rc = mvHwsPortLoopbackSet(devNum, portGroupId, localPort, portMode,
                                  HWS_MAC, lbType);
        if(rc != GT_OK)
        {
            CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
            return rc;
        }
    }
    CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPortLoopbackSet(devNum[%d], portGroupId[%d], phyPortNum[%d], portMode[%d], lpPlace[%d], lbType[%d])",devNum, portGroupId, localPort, portMode, HWS_PCS, lbType);
    rc = mvHwsPortLoopbackSet(devNum, portGroupId, localPort, portMode,
                              HWS_PCS, lbType);
    if(rc != GT_OK)
    {
        CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
        return rc;
    }

    if((_10GBase_KR == portMode) || (SGMII == portMode) || (_1000Base_X == portMode))
    {
        CPSS_DXCH_PORT_COMBO_PARAMS_STC *comboParamsPtr;

        comboParamsPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->port.comboPortsInfoArray[portNum];
        if(comboParamsPtr->macArray[0].macNum != CPSS_DXCH_PORT_COMBO_NA_MAC_CNS)
        { /* if it's combo port */
            if (_10GBase_KR == portMode)
            {
                portModeCombo = _100GBase_KR10;
            }
            else if ((SGMII == portMode) || (_1000Base_X == portMode))
            {
                portModeCombo = (SGMII == portMode) ? QSGMII : _100Base_FX;
            }
            else
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }

            CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPortLoopbackSet(devNum[%d], portGroupId[%d], phyPortNum[%d], portMode[%d], lpPlace[%d], lbType[%d])",devNum, portGroupId, localPort, portModeCombo, HWS_MAC, lbType);
            rc = mvHwsPortLoopbackSet(devNum, portGroupId, localPort, portModeCombo,
                                      HWS_MAC, lbType);
            if(rc != GT_OK)
            {
                CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
                return rc;
            }

            CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPortLoopbackSet(devNum[%d], portGroupId[%d], phyPortNum[%d], portMode[%d], lpPlace[%d], lbType[%d])",devNum, portGroupId, localPort, portModeCombo, HWS_PCS, lbType);
            rc = mvHwsPortLoopbackSet(devNum, portGroupId, localPort, portModeCombo,
                                      HWS_PCS, lbType);
            if(rc != GT_OK)
            {
                CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
                return rc;
            }
        }
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                               PRV_CPSS_DXCH_LION2_PORT_MAC_MIB_COUNTERS_CORRUPTION_WA_E))
    {
        /* reset MAC counters and do not update shadow */
        rc = prvCpssDxChPortMacCountersOnPortGet(devNum,portNum, GT_FALSE, NULL,
                                                 PRV_DXCH_PORT_MAC_CNTR_READ_MODE_RESET_HW_E);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* enable port if we need */
    if (prvCpssDxChPortStateEnableSet(devNum, portNum, portState) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                               PRV_CPSS_DXCH_LION2_DISMATCH_PORTS_LINK_WA_E))
    {
        if((CPSS_PORT_SPEED_10000_E == speed) &&
           ((CPSS_PORT_INTERFACE_MODE_KR_E == ifMode) ||
               (CPSS_PORT_INTERFACE_MODE_SR_LR_E == ifMode) ||
            (CPSS_PORT_INTERFACE_MODE_XHGS_E == ifMode) ||
            (CPSS_PORT_INTERFACE_MODE_XHGS_SR_E == ifMode)))
        {
            if(!enable)
            {/* run WA again to configure force link down state appropriate for
                current state of port */
                return prvCpssDxChPortLion2LinkFixWa(devNum, portNum);
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortPreLion2InternalLoopbackEnableSet
*
* DESCRIPTION:
*       Configure MAC and PCS TX2RX loopbacks on port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number (not CPU port)
*       enable    - If GT_TRUE, enable loopback
*                   If GT_FALSE, disable loopback
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
static GT_STATUS prvCpssDxChPortPreLion2InternalLoopbackEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable
)
{
    GT_U32 regAddr;               /* register address                    */
    GT_U32 value;                 /* value to write into the register    */
    GT_U32 portState = 1;         /* current port state (enable/disable) */
    GT_U32 fieldOffset;           /* bit field offset */
    GT_U32 fieldLength;           /* number of bits to be written to register */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT portMacType; /* MAC unit used by port */
    GT_STATUS rc;       /* return code */
    GT_U32          portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) <= PRV_CPSS_PORT_GE_E)
    {
        /* disable port if we need */
        if (prvCpssDxChPortStateDisableSet(devNum,portNum, &portState) != GT_OK)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    if((rc = prvCpssDxChPortPcsLoopbackEnableSet(devNum,portNum,enable)) != GT_OK)
        return rc;

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);

    for(portMacType = PRV_CPSS_PORT_GE_E; portMacType < PRV_CPSS_PORT_NOT_APPLICABLE_E; portMacType++)
    {
        if((portMacNum == CPSS_CPU_PORT_NUM_CNS) && (portMacType >= PRV_CPSS_PORT_XG_E))
                continue;

        if(portMacType < PRV_CPSS_PORT_XG_E)
        /* Set GMII loopback mode */
        {
            fieldOffset = 5;
            fieldLength = 1;
            value = (enable == GT_TRUE) ? 1 : 0;
        }
        else
            continue;

        PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portMacNum,portMacType,&regAddr);

        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr,
                                                   fieldOffset, fieldLength,
                                                   value) != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }
        }
    }

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) <= PRV_CPSS_PORT_GE_E)
    {
        /* enable port if we need */
        if (prvCpssDxChPortStateEnableSet(devNum,portNum, portState) != GT_OK)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortInternalLoopbackEnableSet
*
* DESCRIPTION:
*       Set the internal Loopback state in the packet processor MAC port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       enable    - If GT_TRUE, enable loopback
*                   If GT_FALSE, disable loopback
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
*       For port 25 in DX269 (XG/HX port) it's the application responsibility to
*       reconfigure the loopback mode after switching XG/HX mode.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortInternalLoopbackEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable
)
{
    GT_U32 portMacNum;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                    info_PRV_CPSS_DXCH_XCAT_GE_PORT_UNIDIRECT_WA_E.enabled == GT_TRUE)
    {/* could be this WA needed not only for XCAT */
        CPSS_PORTS_BMP_PORT_ENABLE_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                                       info_PRV_CPSS_DXCH_XCAT_GE_PORT_UNIDIRECT_WA_E.
                                       pcsLoopbackBmpPtr,portNum,enable);
    }

    if((CPSS_PP_FAMILY_DXCH_LION2_E <= PRV_CPSS_PP_MAC(devNum)->devFamily) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
    {
        if(portNum != CPSS_CPU_PORT_NUM_CNS)
        {
            return prvCpssDxChPortLion2InternalLoopbackEnableSet(devNum, portNum,
                                                                 enable);
        }
    }

    /* for CPU port and older devices */
    return prvCpssDxChPortPreLion2InternalLoopbackEnableSet(devNum, portNum,
                                                                 enable);
}

/*******************************************************************************
* cpssDxChPortInternalLoopbackEnableSet
*
* DESCRIPTION:
*       Set the internal Loopback state in the packet processor MAC port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       enable    - If GT_TRUE, enable loopback
*                   If GT_FALSE, disable loopback
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
*       For port 25 in DX269 (XG/HX port) it's the application responsibility to
*       reconfigure the loopback mode after switching XG/HX mode.
*
*******************************************************************************/
GT_STATUS cpssDxChPortInternalLoopbackEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortInternalLoopbackEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChPortInternalLoopbackEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortInternalLoopbackEnableGet
*
* DESCRIPTION:
*       Get Internal Loopback
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
* OUTPUTS:
*       enablePtr  - Pointer to the Loopback state.
*                    If GT_TRUE, loopback is enabled
*                    If GT_FALSE, loopback is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not relevant for the CPU port.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortInternalLoopbackEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_STATUS status;       /* execution status */
    PRV_CPSS_PORT_TYPE_ENT  portMacType; /* MAC unit used by port */
    GT_U32      portGroupId;    /* core number of port */
    GT_U32          portMacNum;      /* MAC number */
    GT_U32 regAddr;         /* register address */
    GT_U32 portState;       /* current port state (enable/disable) */
    GT_U32 fieldOffset;     /* bit field offset */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
                                                                             portMacNum);

    if((CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily) ||
       (PRV_CPSS_SIP_5_CHECK_MAC(devNum)) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
    {
        GT_U32      localPort;      /* port number in local core */
        MV_HWS_PORT_STANDARD    portMode;   /* port interface in HWS format */
        MV_HWS_PORT_LB_TYPE     lbType;     /* loopback type in HWS format */
        MV_HWS_UNIT             lpPlace;    /* which unit to read for lb state */
        CPSS_PORT_INTERFACE_MODE_ENT    ifMode; /* current interface of port */
        CPSS_PORT_SPEED_ENT             speed;  /* current speed of port */

        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portMacNum);

        lpPlace = HWS_MAC;
        ifMode = PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portMacNum);
        speed = PRV_CPSS_DXCH_PORT_SPEED_MAC(devNum, portMacNum);
        if((CPSS_PORT_INTERFACE_MODE_NA_E == ifMode) || (CPSS_PORT_SPEED_NA_E == speed))
        {/* loopback on port which interface not defined yet - forbidden */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
        }

        status = prvCpssLion2CpssIfModeToHwsTranslate(ifMode, speed, &portMode);
        if(status != GT_OK)
        {
            return status;
        }

        CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPortLoopbackStatusGet(devNum[%d], portGroupId[%d], phyPortNum[%d], portMode[%d], lpPlace[%d], *lbType)",devNum, portGroupId, localPort, portMode, lpPlace);
        status = mvHwsPortLoopbackStatusGet(devNum, portGroupId, localPort, portMode,
                                            lpPlace, &lbType);
        if (status != GT_OK)
        {
            CPSS_LOG_INFORMATION_MAC("Hws return code is %d", status);
            return status;
        }

        *enablePtr = (TX_2_RX_LB == lbType);
    }
    else/* not Lion2/3 devices */
    {
        portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum);
        PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portMacNum,portMacType,&regAddr);
        if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

        if(portMacType >= PRV_CPSS_PORT_XG_E)
        {
            if (PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                            info_PRV_CPSS_DXCH_XCAT_GE_PORT_UNIDIRECT_WA_E.enabled == GT_TRUE)
            {/* not needed for FE and GE mac's, because there GMII loopback state returned */
                *enablePtr = CPSS_PORTS_BMP_IS_PORT_SET_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                                info_PRV_CPSS_DXCH_XCAT_GE_PORT_UNIDIRECT_WA_E.pcsLoopbackBmpPtr,portMacNum);
                *enablePtr = BIT2BOOL_MAC(*enablePtr);
                return GT_OK;
            }

            fieldOffset = 13;
        }
        else
        {
            fieldOffset = 5;
        }

        /* store value of port state */
        status = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                                    fieldOffset, 1, &portState);
        if (status != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
        *enablePtr = BIT2BOOL_MAC(portState);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortInternalLoopbackEnableGet
*
* DESCRIPTION:
*       Get Internal Loopback
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
* OUTPUTS:
*       enablePtr  - Pointer to the Loopback state.
*                    If GT_TRUE, loopback is enabled
*                    If GT_FALSE, loopback is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not relevant for the CPU port.
*
*******************************************************************************/
GT_STATUS cpssDxChPortInternalLoopbackEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortInternalLoopbackEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChPortInternalLoopbackEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortInbandAutoNegEnableSet
*
* DESCRIPTION:
*       Configure Auto-Negotiation mode of MAC for a port.
*       The Tri-Speed port MAC may operate in one of the following two modes:
*       - SGMII Mode - In this mode, Auto-Negotiation may be performed
*         out-of-band via the device's Master SMI interface or in-band.
*         The function sets the mode of Auto-Negotiation to in-band or
*         out-of-band.
*       - 1000BASE-X mode - In this mode, the port operates at 1000 Mbps,
*         full-duplex only and supports in-band Auto-Negotiation for link and
*         for Flow Control.
*         The function set in-band Auto-Negotiation mode only.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - GT_TRUE  - Auto-Negotiation works in in-band mode.
*                 GT_FALSE - Auto-Negotiation works in out-of-band via
*                            the device's Master SMI interface mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - The feature is not supported the port/device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not supported for CPU port.
*       SGMII port In-band Auto-Negotiation is performed by the PCS layer to
*       establish link, speed, and duplex mode.
*       1000BASE-X port In-band Auto-Negotiation is performed by the PCS layer
*       to establish link and flow control support.
*       The change of the Auto-Negotiation causes temporary change of the link
*       to down and up for ports with link up.
*
*       Although for XGMII (10 Gbps) ports feature is not supported the function
*       lets application to configure it
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortInbandAutoNegEnableSet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL  enable
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32 regAddr;         /* register address */
    GT_U32 portState;       /* current port state (enabled/disabled) */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32          portMacNum;      /* MAC number */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);

    /* related to GE interfaces only */
    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_GE_E,&regAddr);
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    /* store value of port state */
    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 0, 1, &portState) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);

    /* Disable port if it's enabled */
    if (portState == 1)
    {
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 0, 1, 0) != GT_OK)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portMacNum,&regAddr);
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    if(GT_TRUE == enable)
    {
        CPSS_PORT_INTERFACE_MODE_ENT ifMode = PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,portMacNum);
        /* the only modes that the function supports */
        if(ifMode == CPSS_PORT_INTERFACE_MODE_SGMII_E ||
           ifMode == CPSS_PORT_INTERFACE_MODE_QSGMII_E  ||
           ifMode == CPSS_PORT_INTERFACE_MODE_1000BASE_X_E)
        {
            /* no need to check result, application may call this function prior
               to GE ifMode configuration, then this function will be engaged
               during ifModeSet */
            (GT_VOID)prvCpssDxChPortInbandAutonegMode(devNum, portNum,
                                      ifMode);
        }
    }

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    /* Set inband auto-negotiation */
    regDataArray[PRV_CPSS_PORT_GE_E].regAddr = regAddr;
    regDataArray[PRV_CPSS_PORT_GE_E].fieldData = BOOL2BIT_MAC(enable);
    regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = 1;
    regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = 2;

    rc = prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* Restore original port state */
    if (portState == 1)
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_GE_E,&regAddr);

        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 0, 1, 1) != GT_OK)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortInbandAutoNegEnableSet
*
* DESCRIPTION:
*       Configure Auto-Negotiation mode of MAC for a port.
*       The Tri-Speed port MAC may operate in one of the following two modes:
*       - SGMII Mode - In this mode, Auto-Negotiation may be performed
*         out-of-band via the device's Master SMI interface or in-band.
*         The function sets the mode of Auto-Negotiation to in-band or
*         out-of-band.
*       - 1000BASE-X mode - In this mode, the port operates at 1000 Mbps,
*         full-duplex only and supports in-band Auto-Negotiation for link and
*         for Flow Control.
*         The function set in-band Auto-Negotiation mode only.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - GT_TRUE  - Auto-Negotiation works in in-band mode.
*                 GT_FALSE - Auto-Negotiation works in out-of-band via
*                            the device's Master SMI interface mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - The feature is not supported the port/device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not supported for CPU port.
*       SGMII port In-band Auto-Negotiation is performed by the PCS layer to
*       establish link, speed, and duplex mode.
*       1000BASE-X port In-band Auto-Negotiation is performed by the PCS layer
*       to establish link and flow control support.
*       The change of the Auto-Negotiation causes temporary change of the link
*       to down and up for ports with link up.
*
*       Although for XGMII (10 Gbps) ports feature is not supported the function
*       lets application to configure it
*
*******************************************************************************/
GT_STATUS cpssDxChPortInbandAutoNegEnableSet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortInbandAutoNegEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChPortInbandAutoNegEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortInbandAutoNegEnableGet
*
* DESCRIPTION:
*       Gets Auto-Negotiation mode of MAC for a port.
*       The Tri-Speed port MAC may operate in one of the following two modes:
*       - SGMII Mode - In this mode, Auto-Negotiation may be performed
*         out-of-band via the device's Master SMI interface or in-band.
*         The function sets the mode of Auto-Negotiation to in-band or
*         out-of-band.
*       - 1000BASE-X mode - In this mode, the port operates at 1000 Mbps,
*         full-duplex only and supports in-band Auto-Negotiation for link and
*         for Flow Control.
*         The function set in-band Auto-Negotiation mode only.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE  - Auto-Negotiation works in in-band mode.
*                    GT_FALSE - Auto-Negotiation works in out-of-band via
*                            the device's Master SMI interface mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - The feature is not supported the port/device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not supported for CPU port.
*       SGMII port In-band Auto-Negotiation is performed by the PCS layer to
*       establish link, speed, and duplex mode.
*       1000BASE-X port In-band Auto-Negotiation is performed by the PCS layer
*       to establish link and flow control support.
*
*       Although for XGMII (10 Gbps) ports feature is not supported the function
*       lets application to configure it
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortInbandAutoNegEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* value to write into the register */
    GT_STATUS rc;
    GT_U32      portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portMacNum,&regAddr);
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    /* store value of port state */
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum),
            regAddr, 2, 1, &value);
    if (rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    *enablePtr = (value == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortInbandAutoNegEnableGet
*
* DESCRIPTION:
*       Gets Auto-Negotiation mode of MAC for a port.
*       The Tri-Speed port MAC may operate in one of the following two modes:
*       - SGMII Mode - In this mode, Auto-Negotiation may be performed
*         out-of-band via the device's Master SMI interface or in-band.
*         The function sets the mode of Auto-Negotiation to in-band or
*         out-of-band.
*       - 1000BASE-X mode - In this mode, the port operates at 1000 Mbps,
*         full-duplex only and supports in-band Auto-Negotiation for link and
*         for Flow Control.
*         The function set in-band Auto-Negotiation mode only.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE  - Auto-Negotiation works in in-band mode.
*                    GT_FALSE - Auto-Negotiation works in out-of-band via
*                            the device's Master SMI interface mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - The feature is not supported the port/device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not supported for CPU port.
*       SGMII port In-band Auto-Negotiation is performed by the PCS layer to
*       establish link, speed, and duplex mode.
*       1000BASE-X port In-band Auto-Negotiation is performed by the PCS layer
*       to establish link and flow control support.
*
*       Although for XGMII (10 Gbps) ports feature is not supported the function
*       lets application to configure it
*
*******************************************************************************/
GT_STATUS cpssDxChPortInbandAutoNegEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortInbandAutoNegEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChPortInbandAutoNegEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortAttributesOnPortGet
*
* DESCRIPTION:
*       Gets port attributes for particular logical port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number (or CPU port)
*
* OUTPUTS:
*       portAttributSetArrayPtr - Pointer to attributes values array.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortAttributesOnPortGet
(
    IN    GT_U8                     devNum,
    IN    GT_PHYSICAL_PORT_NUM      portNum,
    OUT   CPSS_PORT_ATTRIBUTES_STC  *portAttributSetArrayPtr
)
{
    GT_BOOL              portLinkUp;      /* port attributes */
    CPSS_PORT_SPEED_ENT  portSpeed;       /* port attributes */
    CPSS_PORT_DUPLEX_ENT portDuplexity;   /* port attributes */
    GT_STATUS            rc;              /* return code     */
    GT_U32               portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(portAttributSetArrayPtr);

    /* get port attributes */

    rc = cpssDxChPortLinkStatusGet(devNum, portNum, &portLinkUp);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChPortSpeedGet(devNum, portNum, &portSpeed);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChPortDuplexModeGet(devNum ,portNum ,&portDuplexity);
    if (rc != GT_OK)
    {
        return rc;
    }

    portAttributSetArrayPtr->portLinkUp     = portLinkUp;
    portAttributSetArrayPtr->portSpeed      = portSpeed;
    portAttributSetArrayPtr->portDuplexity  = portDuplexity;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortAttributesOnPortGet
*
* DESCRIPTION:
*       Gets port attributes for particular logical port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number (or CPU port)
*
* OUTPUTS:
*       portAttributSetArrayPtr - Pointer to attributes values array.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortAttributesOnPortGet
(
    IN    GT_U8                     devNum,
    IN    GT_PHYSICAL_PORT_NUM      portNum,
    OUT   CPSS_PORT_ATTRIBUTES_STC  *portAttributSetArrayPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortAttributesOnPortGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, portAttributSetArrayPtr));

    rc = internal_cpssDxChPortAttributesOnPortGet(devNum, portNum, portAttributSetArrayPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, portAttributSetArrayPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* prvCpssDxChPortPreambleLengthSet
*
* DESCRIPTION:
*       Set the port with preamble length for Rx or Tx or both directions.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - physical port number (or CPU port)
*       direction    - Rx or Tx or both directions
*                      only XG ports support Rx direction and "both directions"
*                      options (GE ports support only Tx direction)
*       length       - length of preamble in bytes
*                      support only values of 4,8
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number or
*                      wrong direction or wrong length
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortPreambleLengthSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_U32                   length
)
{
    GT_U32  regAddr;/* register address to set */
    GT_U32  offset;/* start offset in register to set */
    GT_U32  fieldLen;/* number of bits to set */
    GT_U32  value;/* value to set to bits */
    GT_U32  portMacNum;      /* MAC number */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    if (PRV_CPSS_SIP_5_CHECK_MAC(devNum) ||
        PRV_CPSS_PP_MAC(devNum)->devFamily ==  CPSS_PP_FAMILY_DXCH_XCAT3_E)
    {
        return prvCpssDxChBobcat2PortMacPreambleLengthSet(devNum,portNum,direction,length);
    }

    if((length != 4) && (length != 8))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if ((direction < CPSS_PORT_DIRECTION_RX_E) || (direction > CPSS_PORT_DIRECTION_BOTH_E))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* XG and up accept all directions , GE and down only accepts TX*/
    if ((direction != CPSS_PORT_DIRECTION_TX_E) &&
            (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum, portMacNum) < PRV_CPSS_PORT_XG_E))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    fieldLen = 1;/* 1 bit to set*/
    if (direction != CPSS_PORT_DIRECTION_RX_E)
    {/* if current used MAC unit is XG/XLG then for DIRECTION_BOTH configure in
        GE DIRECTION_TX, but if user wants just RX skip GE configuration */
        value = (length == 8) ? 0 : 1;
        offset = 15;

        /* MAC Control 1 */
        PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_GE_E,&regAddr);
        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            regDataArray[PRV_CPSS_PORT_GE_E].regAddr = regAddr;
            regDataArray[PRV_CPSS_PORT_GE_E].fieldData = value;
            regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = fieldLen;
            regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = offset;
        }
    }

    value = (length == 8) ? 1 : 0;
    switch(direction)
    {
        case CPSS_PORT_DIRECTION_RX_E:
            offset = 0;/* rx bit */
            break;
        case CPSS_PORT_DIRECTION_TX_E:
            offset = 1;/* tx bit */
            break;
        case CPSS_PORT_DIRECTION_BOTH_E:
            fieldLen = 2;/* 2 bits to set*/
            offset = 0;/* start from rx bit */
            value |= (value << 1);/* set 2 bits instead of 1 ,  1 => 11 , 0 =>00 */
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    /* MAC Control 0 */
    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XG_E,&regAddr);
    offset += ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
               (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
               (PRV_CPSS_SIP_5_CHECK_MAC(devNum))) ? 11 : 12;

    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldLength = fieldLen;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldOffset = offset;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XLG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XLG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldLength = fieldLen;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldOffset = offset;
    }

    return prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
}

/*******************************************************************************
* internal_cpssDxChPortPreambleLengthSet
*
* DESCRIPTION:
*       Set the port with preamble length for Rx or Tx or both directions.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - physical port number (or CPU port)
*       direction    - Rx or Tx or both directions
*                      only XG ports support Rx direction and "both directions"
*                      options (GE ports support only Tx direction)
*       length       - length of preamble in bytes
*                      support only values of 4,8
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number or
*                      wrong direction or wrong length
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPreambleLengthSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_U32                   length
)
{
    CPSS_PORT_DIRECTION_ENT  targetDirection = direction;
     GT_U32                   targetLength = length;

    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    /* Get PHY MAC object ptr */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portMacNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL)
    {
        rc = portMacObjPtr->macDrvMacPreambleLengthSetFunc(devNum,portMacNum,
                                  targetDirection,targetLength,
                                  CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,&targetDirection,
                                  &targetLength);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
      /* set switch MAC Port Preamble Length */
    if (doPpMacConfig == GT_TRUE)
    {
        rc = prvCpssDxChPortPreambleLengthSet(devNum,portNum,
                                              targetDirection,targetLength);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)
    {
        rc = portMacObjPtr->macDrvMacPreambleLengthSetFunc(devNum,portMacNum,
                                  targetDirection,targetLength,
                                  CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,&targetDirection,
                                  &targetLength);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortPreambleLengthSet
*
* DESCRIPTION:
*       Set the port with preamble length for Rx or Tx or both directions.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - physical port number (or CPU port)
*       direction    - Rx or Tx or both directions
*                      only XG ports support Rx direction and "both directions"
*                      options (GE ports support only Tx direction)
*       length       - length of preamble in bytes
*                      support only values of 4,8
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number or
*                      wrong direction or wrong length
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPreambleLengthSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_U32                   length
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPreambleLengthSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, direction, length));

    rc = internal_cpssDxChPortPreambleLengthSet(devNum, portNum, direction, length);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, direction, length));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortPreambleLengthGet
*
* DESCRIPTION:
*       Get the port with preamble length for Rx or Tx or both directions.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - physical port number (or CPU port)
*       direction    - Rx or Tx or both directions
*                      only XG ports support Rx direction
*                      GE ports support only Tx direction.
*
*
* OUTPUTS:
*       lengthPtr    - pointer to preamble length in bytes :
*                      supported length values are : 4,8.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM - on wrong device number or wrong physical port number or
*                      wrong direction or wrong length
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortPreambleLengthGet
(
    IN  GT_U8                    devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_DIRECTION_ENT  direction,
    OUT GT_U32                   *lengthPtr
)
{
    GT_U32  regAddr;/* register address to set */
    GT_U32  offset;/* start offset in register to set */
    GT_U32  value;/* value to set to bits */
    GT_BOOL XGport = GT_FALSE;
    PRV_CPSS_PORT_TYPE_ENT portMacType;
    GT_U32  portMacNum;      /* MAC number */

    CPSS_NULL_PTR_CHECK_MAC(lengthPtr);

    if (PRV_CPSS_SIP_5_CHECK_MAC(devNum) ||
        PRV_CPSS_PP_MAC(devNum)->devFamily ==  CPSS_PP_FAMILY_DXCH_XCAT3_E)
    {
        GT_STATUS rcXLG;
        GT_STATUS rcGE;
        GT_U32 lengthXLG;
        GT_U32 lengthGE;

        *lengthPtr = 0;
        rcXLG = prvCpssDxChBobcat2PortMacPreambleLengthGet(devNum,portNum,PRV_CPSS_PORT_XLG_E,direction,&lengthXLG);
        rcGE  = prvCpssDxChBobcat2PortMacPreambleLengthGet(devNum,portNum,PRV_CPSS_PORT_GE_E, direction,&lengthGE);
        if (rcXLG != GT_OK && rcGE != GT_OK)
        {
            return rcXLG;
        }
        if (rcXLG == GT_OK)  /* it is not proper, current MAC shall be used, or interface shall be changed */
        {
            *lengthPtr = lengthXLG;
        }
        else
        {
            *lengthPtr = lengthGE;
        }
        return GT_OK;
    }

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum);

    if(((portNum != CPSS_CPU_PORT_NUM_CNS) || ((PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_TRUE))) &&
        (portMacType >= PRV_CPSS_PORT_XG_E))
    {
        XGport = GT_TRUE;
        /* MAC Control 0 */
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,portMacType,&regAddr);

        switch(direction)
        {
        case CPSS_PORT_DIRECTION_RX_E:
            offset = ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
                      (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
                      (PRV_CPSS_SIP_5_CHECK_MAC(devNum))) ? 11 : 12;/* rx bit */
            break;
        case CPSS_PORT_DIRECTION_TX_E:
            offset = ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
                      (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
                      (PRV_CPSS_SIP_5_CHECK_MAC(devNum))) ? 12 : 13;/* tx bit */
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        switch(direction)
        {
            case CPSS_PORT_DIRECTION_TX_E:
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        /* MAC Control 1 */
        PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portMacNum,portMacType,&regAddr);
        offset = 15;
    }
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    if(prvCpssDrvHwPpPortGroupGetRegField(devNum,
        PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
                                                                    portMacNum),
            regAddr, offset, 1, &value) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    if (value == 0)
    {
        *lengthPtr = (XGport == GT_TRUE) ? 4 : 8;
    }
    else /* value == 1*/
    {
        *lengthPtr = (XGport == GT_TRUE) ? 8 : 4;
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortPreambleLengthGet
*
* DESCRIPTION:
*       Get the port with preamble length for Rx or Tx or both directions.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - physical port number (or CPU port)
*       direction    - Rx or Tx or both directions
*                      only XG ports support Rx direction
*                      GE ports support only Tx direction.
*
*
* OUTPUTS:
*       lengthPtr    - pointer to preamble length in bytes :
*                      supported length values are : 4,8.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM - on wrong device number or wrong physical port number or
*                      wrong direction or wrong length
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPreambleLengthGet
(
    IN  GT_U8                    devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_DIRECTION_ENT  direction,
      OUT GT_U32                   *lengthPtr
)
{
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */
    CPSS_PORT_DIRECTION_ENT  targetDirection = direction;
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(lengthPtr);

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portMacNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacPreambleLengthGetFunc(devNum,portMacNum,
                                  direction,lengthPtr,
                                  CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,&targetDirection,
                                  lengthPtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* get switch MAC Port Preamble Length value  */
    if (doPpMacConfig == GT_TRUE)
    {
        rc = prvCpssDxChPortPreambleLengthGet(devNum,portNum,targetDirection,lengthPtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacPreambleLengthGetFunc(devNum,portMacNum,
                                  targetDirection,lengthPtr,
                                  CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,&targetDirection,
                                  lengthPtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortPreambleLengthGet
*
* DESCRIPTION:
*       Get the port with preamble length for Rx or Tx or both directions.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - physical port number (or CPU port)
*       direction    - Rx or Tx or both directions
*                      only XG ports support Rx direction
*                      GE ports support only Tx direction.
*
*
* OUTPUTS:
*       lengthPtr    - pointer to preamble length in bytes :
*                      supported length values are : 4,8.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM - on wrong device number or wrong physical port number or
*                      wrong direction or wrong length
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPreambleLengthGet(
    IN  GT_U8                    devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_DIRECTION_ENT  direction,
      OUT GT_U32                   *lengthPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPreambleLengthGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, direction, lengthPtr));

    rc = internal_cpssDxChPortPreambleLengthGet(devNum, portNum, direction, lengthPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, direction, lengthPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortMacSaAddr
*
* DESCRIPTION:
*       Get port source middle MAC address
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       lmsInstance     - LMS instance number
*
* OUTPUTS:
*       regAddrMiddlePtr - pointer to middle SA MAC address register
*       regAddrHighPtr - pointer to high SA MAC address register
*
* RETURNS:
*       GT_OK                   - on success
*       GT_NOT_SUPPORTED        - on not supported register address
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortMacSaAddr
(
    IN  GT_U8           devNum,
    IN  GT_U32          lmsInstance,
    OUT GT_U32          *regAddrMiddlePtr,
    OUT GT_U32          *regAddrHighPtr
)
{
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum))
    {
        *regAddrMiddlePtr = PRV_DXCH_REG1_UNIT_LMS_MAC(devNum, lmsInstance).
            LMS0.LMS0Group0.portsMACSourceAddr.sourceAddrMiddle;
        *regAddrHighPtr = PRV_DXCH_REG1_UNIT_LMS_MAC(devNum, lmsInstance).
            LMS0.LMS0Group0.portsMACSourceAddr.sourceAddrHigh;
    }
    else
    {
        *regAddrMiddlePtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.srcAddrMiddle;
        *regAddrHighPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.srcAddrHigh;
    }

    if((PRV_CPSS_SW_PTR_ENTRY_UNUSED == *regAddrMiddlePtr) ||
       (PRV_CPSS_SW_PTR_ENTRY_UNUSED == *regAddrHighPtr))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortMacSaBaseSet
*
* DESCRIPTION:
*       Sets the base part(40 upper bits) of all device's ports MAC addresses.
*       Port MAC addresses are used as the MAC SA for Flow Control Packets
*       transmitted by the device. In addition these addresses can be used as
*       MAC DA for Flow Control packets received by these ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       macPtr - (pointer to)The system Mac address to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The upper 40 bits of the MAC Address are the same for all ports in the
*       device and the lower eight bits are unique per port.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortMacSaBaseSet
(
    IN  GT_U8           devNum,
    IN  GT_ETHERADDR    *macPtr
)
{
    GT_U32  macMiddle;          /* Holds bits 8 - 15 of mac     */
    GT_U32  macHigh;            /* Holds bits 16 - 47 of mac    */
    GT_STATUS rc;               /* Return code */
    GT_U32  regAddrMiddle;      /* pointer to middle SA MAC register address */
    GT_U32  regAddrHigh;        /* pointer to high SA MAC register address */
    GT_U32  regAddr;            /* register address */
    GT_U32  lmsInstance;        /* LMS instance */
    GT_U32  lmsInstanceNum;     /* LMS instance number */
    GT_U32  portMacNum;         /* MAC number */
    GT_U32  maxMacNum;          /* max MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(macPtr);

    if((PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
    {
        maxMacNum = (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE) ? 28 : 72;

        for(portMacNum = 0; portMacNum < maxMacNum; portMacNum++)
        {
            /* support not continues MACs of bobk */
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, portMacNum);

            /* MAC SA - bits [15:8] */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE)
            {
                regAddr = PRV_DXCH_REG_UNIT_GOP_FCA_MAC(devNum, portMacNum).MACSA0To15;
            }
            else
            {
                regAddr = PRV_DXCH_REG1_UNIT_GOP_FCA_MAC(devNum, portMacNum).MACSA0To15;
            }
            rc = prvCpssHwPpSetRegField(devNum, regAddr, 8, 8, macPtr->arEther[4]);
            if(GT_OK != rc)
            {
                return rc;
            }

            /* MAC SA - bits [31:16] */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE)
            {
                regAddr = PRV_DXCH_REG_UNIT_GOP_FCA_MAC(devNum, portMacNum).MACSA16To31;
            }
            else
            {
                regAddr = PRV_DXCH_REG1_UNIT_GOP_FCA_MAC(devNum, portMacNum).MACSA16To31;
            }
            rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 16, ((macPtr->arEther[3]) | (macPtr->arEther[2] << 8)));
            if(GT_OK != rc)
            {
                return rc;
            }

            /* MAC SA - bits [48:32] */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE)
            {
                regAddr = PRV_DXCH_REG_UNIT_GOP_FCA_MAC(devNum, portMacNum).MACSA32To47;
            }
            else
            {
                regAddr = PRV_DXCH_REG1_UNIT_GOP_FCA_MAC(devNum, portMacNum).MACSA32To47;
            }
            rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 16, ((macPtr->arEther[1]) | (macPtr->arEther[0] << 8)));
            if(GT_OK != rc)
            {
                return rc;
            }
        }
    }

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.gop_lms.notSupported == GT_TRUE)
    {
        /* no LMS to configure */
        return GT_OK;
    }

    macMiddle   =  macPtr->arEther[4];
    macHigh     = (macPtr->arEther[3]        |
                  (macPtr->arEther[2] << 8)  |
                  (macPtr->arEther[1] << 16) |
                  (macPtr->arEther[0] << 24));

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum))
    {
        lmsInstanceNum = 3;
    }
    else
    {
        lmsInstanceNum = 1;
    }

    for(lmsInstance = 0; lmsInstance < lmsInstanceNum; lmsInstance++)
    {
        rc = prvCpssDxChPortMacSaAddr(devNum, lmsInstance,
                                      &regAddrMiddle, &regAddrHigh);
        if(rc != GT_OK)
        {
            return rc;
        }
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddrMiddle, 0, 8, macMiddle);
        if( rc != GT_OK )
        {
            return rc;
        }
        rc = prvCpssDrvHwPpWriteRegister(devNum, regAddrHigh, macHigh);
        if( rc != GT_OK )
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortMacSaBaseSet
*
* DESCRIPTION:
*       Sets the base part(40 upper bits) of all device's ports MAC addresses.
*       Port MAC addresses are used as the MAC SA for Flow Control Packets
*       transmitted by the device. In addition these addresses can be used as
*       MAC DA for Flow Control packets received by these ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       macPtr - (pointer to)The system Mac address to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The upper 40 bits of the MAC Address are the same for all ports in the
*       device and the lower eight bits are unique per port.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacSaBaseSet
(
    IN  GT_U8           devNum,
    IN  GT_ETHERADDR    *macPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMacSaBaseSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, macPtr));

    rc = internal_cpssDxChPortMacSaBaseSet(devNum, macPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, macPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* portMacSaBaseFromFirstPortGet
*
* DESCRIPTION:
*       Gets the base part (40 upper bits) of all device's ports MAC addresses.
*       take from first valid port.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       macPtr - (pointer to)The system Mac address.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS portMacSaBaseFromFirstPortGet
(
    IN  GT_U8           devNum,
    OUT GT_ETHERADDR    *macPtr
)
{
    GT_STATUS rc;               /* Return code */
    GT_U32  regAddr;            /* register address */
    GT_U32  portMacNum;         /* MAC number */
    GT_U32  maxMacNum;          /* max MAC number */
    GT_U32  value;              /* value from register */

    if((PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
    {
        maxMacNum = (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE) ? 28 : 72;

        for(portMacNum = 0; portMacNum < maxMacNum; portMacNum++)
        {
            /* support not continues MACs of bobk */
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, portMacNum);

            /* MAC SA - bits [15:8] */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE)
            {
                regAddr = PRV_DXCH_REG_UNIT_GOP_FCA_MAC(devNum, portMacNum).MACSA0To15;
            }
            else
            {
                regAddr = PRV_DXCH_REG1_UNIT_GOP_FCA_MAC(devNum, portMacNum).MACSA0To15;
            }
            rc = prvCpssHwPpGetRegField(devNum, regAddr, 8, 8, &value);
            if(GT_OK != rc)
            {
                return rc;
            }

            macPtr->arEther[4] = (GT_U8)value;

            /* MAC SA - bits [31:16] */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE)
            {
                regAddr = PRV_DXCH_REG_UNIT_GOP_FCA_MAC(devNum, portMacNum).MACSA16To31;
            }
            else
            {
                regAddr = PRV_DXCH_REG1_UNIT_GOP_FCA_MAC(devNum, portMacNum).MACSA16To31;
            }
            rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 16, &value);
            if(GT_OK != rc)
            {
                return rc;
            }
            macPtr->arEther[2] = (GT_U8)(value>>8);
            macPtr->arEther[3] = (GT_U8)value;

            /* MAC SA - bits [48:32] */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE)
            {
                regAddr = PRV_DXCH_REG_UNIT_GOP_FCA_MAC(devNum, portMacNum).MACSA32To47;
            }
            else
            {
                regAddr = PRV_DXCH_REG1_UNIT_GOP_FCA_MAC(devNum, portMacNum).MACSA32To47;
            }
            rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 16, &value);
            if(GT_OK != rc)
            {
                return rc;
            }

            macPtr->arEther[0] = (GT_U8)(value>>8);
            macPtr->arEther[1] = (GT_U8)value;

            /* stop after first valid port */
            return GT_OK;
        }
    }

    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);/* error ... no valid ports */
}


/*******************************************************************************
* internal_cpssDxChPortMacSaBaseGet
*
* DESCRIPTION:
*       Gets the base part (40 upper bits) of all device's ports MAC addresses.
*       Port MAC addresses are used as the MAC SA for Flow Control Packets
*       transmitted by the device.In addition these addresses can be used as
*       MAC DA for Flow Control packets received by these ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       macPtr - (pointer to)The system Mac address.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The upper 40 bits of the MAC Address are the same for all ports in the
*       device and the lower eight bits are unique per port.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortMacSaBaseGet
(
    IN  GT_U8           devNum,
    OUT GT_ETHERADDR    *macPtr
)
{
    GT_STATUS status = GT_OK;
    GT_U32  macMiddle;          /* Holds bits 8 - 15 of mac     */
    GT_U32  macHigh;            /* Holds bits 16 - 47 of mac    */
    GT_U32  regAddrMiddle;      /* middle SA MAC register address */
    GT_U32  regAddrHigh;        /* high SA MAC register address */
    GT_STATUS rc;               /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(macPtr);

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.gop_lms.notSupported == GT_TRUE)
    {
        /* no LMS , get info from first port */
        return portMacSaBaseFromFirstPortGet(devNum,macPtr);
    }

    rc = prvCpssDxChPortMacSaAddr(devNum, 0, &regAddrMiddle, &regAddrHigh);
    if(rc != GT_OK)
    {
        return rc;
    }

    status = prvCpssDrvHwPpGetRegField(devNum, regAddrMiddle, 0, 8, &macMiddle);
    if(status != GT_OK)
        return status;


    status = prvCpssDrvHwPpReadRegister(devNum, regAddrHigh, &macHigh);
    if(status != GT_OK)
        return status;

    macPtr->arEther[3] = (GT_U8)(macHigh & 0xFF);
    macPtr->arEther[2] = (GT_U8)((macHigh & 0xFF00) >> 8);
    macPtr->arEther[1] = (GT_U8)((macHigh & 0xFF0000) >> 16);
    macPtr->arEther[0] = (GT_U8)((macHigh & 0xFF000000) >> 24);
    macPtr->arEther[4] = (GT_U8)macMiddle;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortMacSaBaseGet
*
* DESCRIPTION:
*       Gets the base part (40 upper bits) of all device's ports MAC addresses.
*       Port MAC addresses are used as the MAC SA for Flow Control Packets
*       transmitted by the device.In addition these addresses can be used as
*       MAC DA for Flow Control packets received by these ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       macPtr - (pointer to)The system Mac address.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The upper 40 bits of the MAC Address are the same for all ports in the
*       device and the lower eight bits are unique per port.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacSaBaseGet
(
    IN  GT_U8           devNum,
    OUT GT_ETHERADDR    *macPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMacSaBaseGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, macPtr));

    rc = internal_cpssDxChPortMacSaBaseGet(devNum, macPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, macPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortStateDisableSet
*
* DESCRIPTION:
*       Disable a specified port on specified device.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
* OUTPUTS:
*       portStatePtr - (pointer to) the state (en/dis) of port before calling this function
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortStateDisableSet
(
    IN GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_U32  *portStatePtr
)
{
    GT_STATUS   rc; /* return code */
    GT_BOOL     portState; /* current port state(enabled/disabled) */

    rc = prvCpssDxChPortEnableGet(devNum, portNum, &portState);
    if(rc != GT_OK)
    {
        return rc;
    }
    *portStatePtr = (GT_U32)portState;

    /* disable port if we need */
    if (1 == *portStatePtr)
    {
        rc = cpssDxChPortEnableSet(devNum, portNum, GT_FALSE);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortStateEnableSet
*
* DESCRIPTION:
*       Enable a specified port on specified device.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       portState - the state (en/dis) of port before calling the port disable
*                   function
*
* OUTPUTS:
*
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortStateEnableSet
(
    IN GT_U8 devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN GT_U32 portState
)
{
    /* enable port if we need */
    if (portState == 1)
    {
        return cpssDxChPortEnableSet(devNum, portNum, GT_TRUE);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortPaddingEnableSet
*
* DESCRIPTION:
*       Enable/Disable padding of transmitted packets shorter than 64B.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port).
*       enable   - GT_TRUE  - Pad short packet in Tx.
*                - GT_FALSE - No padding in short packets.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortPaddingEnableSet
(
    IN  GT_U8      devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL    enable
)
{
    GT_U32      regAddr; /* register address */
    GT_U32      value;   /* register field value */
    GT_U32      offset;  /* field offset */
    GT_U32  portMacNum;      /* MAC number */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    value = (enable == GT_TRUE) ? 0 : 1;
    PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_GE_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_GE_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = 5;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XG_E,&regAddr);
    offset = ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
              (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
              (PRV_CPSS_SIP_5_CHECK_MAC(devNum))) ? 13 : 14;
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldOffset = offset;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XLG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XLG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldOffset = offset;
    }

    return prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
}

/*******************************************************************************
* internal_cpssDxChPortPaddingEnableSet
*
* DESCRIPTION:
*       Enable/Disable padding of transmitted packets shorter than 64B.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port).
*       enable   - GT_TRUE  - Pad short packet in Tx.
*                - GT_FALSE - No padding in short packets.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPaddingEnableSet
(
    IN  GT_U8      devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL    enable
)
{
    GT_BOOL    targetEnable = enable;
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;            /* do switch mac code indicator */
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portMacNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacPaddingEnableSetFunc(devNum,portMacNum,
                                  targetEnable, CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,&targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* set switch MAC Padding Enable configuration */
    if (doPpMacConfig == GT_TRUE)
    {
        rc = prvCpssDxChPortPaddingEnableSet(devNum,portNum,targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacPaddingEnableSetFunc(devNum,portMacNum,
                                  targetEnable,
                                  CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,&targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortPaddingEnableSet
*
* DESCRIPTION:
*       Enable/Disable padding of transmitted packets shorter than 64B.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port).
*       enable   - GT_TRUE  - Pad short packet in Tx.
*                - GT_FALSE - No padding in short packets.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPaddingEnableSet
(
    IN  GT_U8      devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL    enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPaddingEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChPortPaddingEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortPaddingEnableGet
*
* DESCRIPTION:
*       Gets padding status of transmitted packets shorter than 64B.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port).
*
* OUTPUTS:
*       enablePtr   - pointer to packet padding status.
*                     - GT_TRUE  - Pad short packet in Tx.
*                     - GT_FALSE - No padding in short packets.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortPaddingEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL    *enablePtr
)
{
    GT_U32      regAddr; /* register address */
    GT_U32      value;   /* register field value */
    GT_U32      offset;  /* field offset */
    GT_STATUS   rc;      /* return status */
    PRV_CPSS_PORT_TYPE_ENT portMacType;
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum);

    if(portMacType < PRV_CPSS_PORT_XG_E)
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portMacNum,portMacType,&regAddr);
        offset = 5;
    }
    else
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,portMacType,&regAddr);
        if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
           (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E) ||
           (PRV_CPSS_SIP_5_CHECK_MAC(devNum)))
        {
            offset = 13;
        }
        else
        {
            offset = 14;
        }
    }
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,
            PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
                                                                        portMacNum),
            regAddr, offset, 1, &value);

    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (value == 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortPaddingEnableGet
*
* DESCRIPTION:
*       Gets padding status of transmitted packets shorter than 64B.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port).
*
* OUTPUTS:
*       enablePtr   - pointer to packet padding status.
*                     - GT_TRUE  - Pad short packet in Tx.
*                     - GT_FALSE - No padding in short packets.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPaddingEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL    *enablePtr
)
{
    GT_STATUS   rc = GT_OK;                      /* return status */
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    /* Get PHY MAC object ptr */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portMacNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacPaddingEnableGetFunc(devNum,portMacNum,
                                  enablePtr, CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,enablePtr);
          if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* get Port Padding Enable value from switch MAC*/
    if (doPpMacConfig == GT_TRUE)
    {
        rc = prvCpssDxChPortPaddingEnableGet(devNum,portNum,enablePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacPaddingEnableGetFunc(devNum,portMacNum,
                                  enablePtr,
                                  CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,enablePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortPaddingEnableGet
*
* DESCRIPTION:
*       Gets padding status of transmitted packets shorter than 64B.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port).
*
* OUTPUTS:
*       enablePtr   - pointer to packet padding status.
*                     - GT_TRUE  - Pad short packet in Tx.
*                     - GT_FALSE - No padding in short packets.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPaddingEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL    *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPaddingEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChPortPaddingEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortExcessiveCollisionDropEnableSet
*
* DESCRIPTION:
*       Enable/Disable excessive collision packets drop.
*       In half duplex mode if a collision occurs the device tries to transmit
*       the packet again. If the number of collisions on the same packet is 16
*       and  excessive collision packets drop is enabled,
*       the packet is dropped.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port).
*       enable   - GT_TRUE  - if the number of collisions on the same packet
*                             is 16 the packet is dropped.
*                - GT_FALSE - A collided packet will be retransmitted by device
*                             until it is transmitted without collisions,
*                             regardless of the number of collisions on the packet.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*       The setting is not relevant in full duplex mode
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortExcessiveCollisionDropEnableSet
(
    IN  GT_U8      devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL    enable
)
{
    GT_U32      regAddr; /* register address */
    GT_U32      portMacNum;      /* MAC number */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_GE_E,&regAddr);
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_GE_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldData = (enable == GT_TRUE) ? 0 : 1;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = 4;
    }

    return prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
}

/*******************************************************************************
* internal_cpssDxChPortExcessiveCollisionDropEnableSet
*
* DESCRIPTION:
*       Enable/Disable excessive collision packets drop.
*       In half duplex mode if a collision occurs the device tries to transmit
*       the packet again. If the number of collisions on the same packet is 16
*       and  excessive collision packets drop is enabled,
*       the packet is dropped.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port).
*       enable   - GT_TRUE  - if the number of collisions on the same packet
*                             is 16 the packet is dropped.
*                - GT_FALSE - A collided packet will be retransmitted by device
*                             until it is transmitted without collisions,
*                             regardless of the number of collisions on the packet.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*       The setting is not relevant in full duplex mode
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortExcessiveCollisionDropEnableSet
(
    IN  GT_U8      devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL    enable
)
{
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */
    GT_BOOL    targetEnable = enable;
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portMacNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacExcessiveCollisionDropSetFunc(devNum,portMacNum,
                                  targetEnable, CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,&targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* set switch MAC Collision Drop configuration */
    if (doPpMacConfig == GT_TRUE)
    {
        rc = prvCpssDxChPortExcessiveCollisionDropEnableSet(devNum,portNum,targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacExcessiveCollisionDropSetFunc(devNum,portMacNum,
                                  targetEnable,
                                  CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,&targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortExcessiveCollisionDropEnableSet
*
* DESCRIPTION:
*       Enable/Disable excessive collision packets drop.
*       In half duplex mode if a collision occurs the device tries to transmit
*       the packet again. If the number of collisions on the same packet is 16
*       and  excessive collision packets drop is enabled,
*       the packet is dropped.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port).
*       enable   - GT_TRUE  - if the number of collisions on the same packet
*                             is 16 the packet is dropped.
*                - GT_FALSE - A collided packet will be retransmitted by device
*                             until it is transmitted without collisions,
*                             regardless of the number of collisions on the packet.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*       The setting is not relevant in full duplex mode
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortExcessiveCollisionDropEnableSet
(
    IN  GT_U8      devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL    enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortExcessiveCollisionDropEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChPortExcessiveCollisionDropEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* prvCpssDxChPortExcessiveCollisionDropEnableGet
*
* DESCRIPTION:
*       Gets status of excessive collision packets drop.
*       In half duplex mode if a collision occurs the device tries to transmit
*       the packet again. If the number of collisions on the same packet is 16
*       and  excessive collision packets drop is enabled,
*       the packet is dropped.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number (or CPU port).
*
* OUTPUTS:
*       enablePtr - pointer to status of excessive collision packets drop.
*                   - GT_TRUE  - if the number of collisions on the same packet
*                               is 16 the packet is dropped.
*                   - GT_FALSE - A collided packet will be retransmitted by
*                                device until it is transmitted
*                                without collisions, regardless of the number
*                                of collisions on the packet.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       Not relevant in full duplex mode
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortExcessiveCollisionDropEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL    *enablePtr
)
{
    GT_U32      regAddr; /* register address */
    GT_U32      value;   /* register field value */
    GT_STATUS   rc;      /* return status */
    GT_U32      portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_GE_E,&regAddr);
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,
            PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
                                                                        portMacNum),
            regAddr, 4, 1, &value);

    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (value == 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;

}

/*******************************************************************************
* internal_cpssDxChPortExcessiveCollisionDropEnableGet
*
* DESCRIPTION:
*       Gets status of excessive collision packets drop.
*       In half duplex mode if a collision occurs the device tries to transmit
*       the packet again. If the number of collisions on the same packet is 16
*       and  excessive collision packets drop is enabled,
*       the packet is dropped.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number (or CPU port).
*
* OUTPUTS:
*       enablePtr - pointer to status of excessive collision packets drop.
*                   - GT_TRUE  - if the number of collisions on the same packet
*                               is 16 the packet is dropped.
*                   - GT_FALSE - A collided packet will be retransmitted by
*                                device until it is transmitted
*                                without collisions, regardless of the number
*                                of collisions on the packet.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       1. Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*       2. Not relevant in full duplex mode
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortExcessiveCollisionDropEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL    *enablePtr
)
{
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */
    GT_U32      portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portMacNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacExcessiveCollisionDropGetFunc(devNum,portMacNum,
                                  enablePtr, CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,enablePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* Get Port Excessive Collision Drop value from switch MAC*/
    if (doPpMacConfig == GT_TRUE)
    {
        rc = prvCpssDxChPortExcessiveCollisionDropEnableGet(devNum,portNum,enablePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacExcessiveCollisionDropGetFunc(devNum,portMacNum,
                                  enablePtr,
                                  CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,enablePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortExcessiveCollisionDropEnableGet
*
* DESCRIPTION:
*       Gets status of excessive collision packets drop.
*       In half duplex mode if a collision occurs the device tries to transmit
*       the packet again. If the number of collisions on the same packet is 16
*       and  excessive collision packets drop is enabled,
*       the packet is dropped.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number (or CPU port).
*
* OUTPUTS:
*       enablePtr - pointer to status of excessive collision packets drop.
*                   - GT_TRUE  - if the number of collisions on the same packet
*                               is 16 the packet is dropped.
*                   - GT_FALSE - A collided packet will be retransmitted by
*                                device until it is transmitted
*                                without collisions, regardless of the number
*                                of collisions on the packet.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       1. Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value
*       is TRUE the switch MAC will be configured
*
*       2. Not relevant in full duplex mode
*
*******************************************************************************/
GT_STATUS cpssDxChPortExcessiveCollisionDropEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL    *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortExcessiveCollisionDropEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChPortExcessiveCollisionDropEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortInBandAutoNegBypassEnableSet
*
* DESCRIPTION:
*       Enable/Disable Auto-Negotiation by pass.
*       If the link partner doesn't respond to Auto-Negotiation process,
*       the link is established by bypassing the Auto-Negotiation procedure.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number.
*       enable   - GT_TRUE  - Auto-Negotiation can't be bypassed.
*                - GT_FALSE - Auto-Negotiation is bypassed.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - wrong devNum, portNum
*       GT_NOT_SUPPORTED - The feature is not supported the port/device
*       GT_FAIL          - on error
*
* COMMENTS:
*       Relevant when Inband Auto-Negotiation is enabled.
*       (See cpssDxChPortInbandAutoNegEnableSet.)
*
*       Although for XGMII (10 Gbps) ports feature is not supported the function
*       lets application to configure it
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortInBandAutoNegBypassEnableSet
(
    IN  GT_U8      devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL    enable
)
{
    GT_U32      regAddr; /* register address */
    GT_U32      portMacNum; /* MAC number */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portMacNum,&regAddr);
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    regDataArray[PRV_CPSS_PORT_GE_E].regAddr = regAddr;
    regDataArray[PRV_CPSS_PORT_GE_E].fieldData = BOOL2BIT_MAC(enable);
    regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = 1;
    regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = 3;

    return prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
}

/*******************************************************************************
* cpssDxChPortInBandAutoNegBypassEnableSet
*
* DESCRIPTION:
*       Enable/Disable Auto-Negotiation by pass.
*       If the link partner doesn't respond to Auto-Negotiation process,
*       the link is established by bypassing the Auto-Negotiation procedure.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number.
*       enable   - GT_TRUE  - Auto-Negotiation can't be bypassed.
*                - GT_FALSE - Auto-Negotiation is bypassed.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - wrong devNum, portNum
*       GT_NOT_SUPPORTED - The feature is not supported the port/device
*       GT_FAIL          - on error
*
* COMMENTS:
*       Relevant when Inband Auto-Negotiation is enabled.
*       (See cpssDxChPortInbandAutoNegEnableSet.)
*
*       Although for XGMII (10 Gbps) ports feature is not supported the function
*       lets application to configure it
*
*******************************************************************************/
GT_STATUS cpssDxChPortInBandAutoNegBypassEnableSet
(
    IN  GT_U8      devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL    enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortInBandAutoNegBypassEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChPortInBandAutoNegBypassEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortInBandAutoNegBypassEnableGet
*
* DESCRIPTION:
*       Gets Auto-Negotiation by pass status.
*       If the link partner doesn't respond to Auto-Negotiation process,
*       the link is established by bypassing the Auto-Negotiation procedure.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number.
*
* OUTPUTS:
*       enablePtr - pointer to Auto-Negotiation by pass status.
*                   - GT_TRUE  - Auto-Negotiation can't be bypassed.
*                   - GT_FALSE - Auto-Negotiation is bypassed.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - wrong devNum, portNum
*       GT_NOT_SUPPORTED - The feature is not supported the port/device
*       GT_FAIL          - on error
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*
* COMMENTS:
*       Relevant when Inband Auto-Negotiation is enabled.
*       (See cpssDxChPortInbandAutoNegEnableSet.)
*
*       Although for XGMII (10 Gbps) ports feature is not supported the function
*       lets application to configure it
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortInBandAutoNegBypassEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL    *enablePtr
)
{
    GT_U32      regAddr; /* register address */
    GT_U32      value;   /* register field value */
    GT_STATUS   rc;      /* return status */
    GT_U32      portMacNum; /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portMacNum,&regAddr);
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,
            PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
                                                                        portMacNum),
            regAddr, 3, 1, &value);

    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;

}

/*******************************************************************************
* cpssDxChPortInBandAutoNegBypassEnableGet
*
* DESCRIPTION:
*       Gets Auto-Negotiation by pass status.
*       If the link partner doesn't respond to Auto-Negotiation process,
*       the link is established by bypassing the Auto-Negotiation procedure.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number.
*
* OUTPUTS:
*       enablePtr - pointer to Auto-Negotiation by pass status.
*                   - GT_TRUE  - Auto-Negotiation can't be bypassed.
*                   - GT_FALSE - Auto-Negotiation is bypassed.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - wrong devNum, portNum
*       GT_NOT_SUPPORTED - The feature is not supported the port/device
*       GT_FAIL          - on error
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*
* COMMENTS:
*       Relevant when Inband Auto-Negotiation is enabled.
*       (See cpssDxChPortInbandAutoNegEnableSet.)
*
*       Although for XGMII (10 Gbps) ports feature is not supported the function
*       lets application to configure it
*
*******************************************************************************/
GT_STATUS cpssDxChPortInBandAutoNegBypassEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL    *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortInBandAutoNegBypassEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChPortInBandAutoNegBypassEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortMacErrorIndicationPortSet
*
* DESCRIPTION:
*       Sets port monitored for MAC errors.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port monitored for MAC errors.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*       MAC source addresses and the error type can be read
*       for the port. See cpssDxChPortMacErrorIndicationGet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortMacErrorIndicationPortSet
(
    IN  GT_U8      devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum
)
{
    GT_U32      regAddr; /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bridgeRegs.macErrorIndicationPortConfReg;
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    return prvCpssDrvHwPpPortGroupSetRegField(devNum,
        PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
                                                                    portNum),
            regAddr, 0, 6, portNum);
}

/*******************************************************************************
* cpssDxChPortMacErrorIndicationPortSet
*
* DESCRIPTION:
*       Sets port monitored for MAC errors.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port monitored for MAC errors.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*       MAC source addresses and the error type can be read
*       for the port. See cpssDxChPortMacErrorIndicationGet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacErrorIndicationPortSet
(
    IN  GT_U8      devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMacErrorIndicationPortSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum));

    rc = internal_cpssDxChPortMacErrorIndicationPortSet(devNum, portNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortMacErrorIndicationPortGet
*
* DESCRIPTION:
*       Gets port monitored for MAC errors.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       portNumPtr  - pointer to port monitored for MAC errors.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       MAC source addresses and the error type can be read
*       for the port. See cpssDxChPortMacErrorIndicationGet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortMacErrorIndicationPortGet
(
    IN  GT_U8      devNum,
    OUT GT_PHYSICAL_PORT_NUM      *portNumPtr
)
{
    GT_U32      regAddr; /* register address */
    GT_U32      value;   /* register value */
    GT_STATUS   rc;      /* return status */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(portNumPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bridgeRegs.macErrorIndicationPortConfReg;
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    rc = prvCpssDrvHwPpGetRegField(devNum,
            regAddr, 0, 6, &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    *portNumPtr = value;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortMacErrorIndicationPortGet
*
* DESCRIPTION:
*       Gets port monitored for MAC errors.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       portNumPtr  - pointer to port monitored for MAC errors.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       MAC source addresses and the error type can be read
*       for the port. See cpssDxChPortMacErrorIndicationGet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacErrorIndicationPortGet
(
    IN  GT_U8      devNum,
    OUT GT_PHYSICAL_PORT_NUM      *portNumPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMacErrorIndicationPortGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNumPtr));

    rc = internal_cpssDxChPortMacErrorIndicationPortGet(devNum, portNumPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNumPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortMacErrorIndicationGet
*
* DESCRIPTION:
*       Gets MAC source addresses and the error type for the monitored port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       typePtr     - pointer to MAC error type.
*       macPtr      - pointer to MAC SA of the last packet received
*                     with  a MAC error on MAC error indication port.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*      Port monitored for MAC errors should be set.
*      See cpssDxChPortMacErrorIndicationPortSet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortMacErrorIndicationGet
(
    IN  GT_U8                               devNum,
    OUT CPSS_DXCH_PORT_MAC_ERROR_TYPE_ENT   *typePtr,
    OUT GT_ETHERADDR                        *macPtr
)
{
    GT_U32      regAddr; /* register address */
    GT_U32      value;   /* register value */
    GT_STATUS   rc;      /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);


    CPSS_NULL_PTR_CHECK_MAC(macPtr);
    CPSS_NULL_PTR_CHECK_MAC(typePtr);


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bridgeRegs.macErrorIndicationStatusReg[0];

    rc = prvCpssDrvHwPpReadRegister(devNum,regAddr, &value);
    if(rc != GT_OK)
    {
        return rc;
    }
    /* MAC SA of the last packet received [31:0] */
    macPtr->arEther[5] = (GT_U8)(value & 0xFF);
    macPtr->arEther[4] = (GT_U8)((value >> 8) & 0xFF);
    macPtr->arEther[3] = (GT_U8)((value >> 16) & 0xFF);
    macPtr->arEther[2] = (GT_U8)((value >> 24) & 0xFF);

    /* MAC SA of the last packet received [47:32] */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bridgeRegs.macErrorIndicationStatusReg[1];

    rc = prvCpssDrvHwPpGetRegField(devNum,regAddr, 0, 19, &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    macPtr->arEther[1] = (GT_U8)(value & 0xFF);
    macPtr->arEther[0] = (GT_U8)((value >> 8) & 0xFF);

    switch((value >> 16) & 7)
    {
        case 0:
            *typePtr = CPSS_DXCH_PORT_MAC_ERROR_JABBER_E;
            break;
        case 1:
            *typePtr = CPSS_DXCH_PORT_MAC_ERROR_FRAGMENTS_E;
            break;
        case 2:
            *typePtr = CPSS_DXCH_PORT_MAC_ERROR_UNDERSIZE_E;
            break;
        case 3:
            *typePtr = CPSS_DXCH_PORT_MAC_ERROR_OVERSIZE_E;
            break;
        case 4:
            *typePtr = CPSS_DXCH_PORT_MAC_ERROR_RX_E;
            break;
        case 5:
            *typePtr = CPSS_DXCH_PORT_MAC_ERROR_CRC_E;
            break;
        case 6:
            *typePtr = CPSS_DXCH_PORT_MAC_ERROR_OVERRUN_E;
            break;
        case 7:
            *typePtr = CPSS_DXCH_PORT_MAC_ERROR_NO_E;
            break;

    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortMacErrorIndicationGet
*
* DESCRIPTION:
*       Gets MAC source addresses and the error type for the monitored port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       typePtr     - pointer to MAC error type.
*       macPtr      - pointer to MAC SA of the last packet received
*                     with  a MAC error on MAC error indication port.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*      Port monitored for MAC errors should be set.
*      See cpssDxChPortMacErrorIndicationPortSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacErrorIndicationGet
(
    IN  GT_U8                               devNum,
    OUT CPSS_DXCH_PORT_MAC_ERROR_TYPE_ENT   *typePtr,
    OUT GT_ETHERADDR                        *macPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMacErrorIndicationGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, typePtr, macPtr));

    rc = internal_cpssDxChPortMacErrorIndicationGet(devNum, typePtr, macPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, typePtr, macPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/******************************************************************************
* internal_cpssDxChPortSerdesGroupGet
*
* DESCRIPTION:
*       Get SERDES port group that may be used in per SERDES group APIs.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
* OUTPUTS:
*       portSerdesGroupPtr  - Pointer to port group number
*                 DxCh3 Giga/2.5 G, xCat GE devices:
*                        Ports       |    SERDES Group
*                        0..3        |      0
*                        4..7        |      1
*                        8..11       |      2
*                        12..15      |      3
*                        16..19      |      4
*                        20..23      |      5
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*                DxCh3 XG devices:
*                        0           |      0
*                        4           |      1
*                        10          |      2
*                        12          |      3
*                        16          |      4
*                        22          |      5
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*               xCat FE devices
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*               Lion devices: Port == SERDES Group
*
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on wrong devNum, portNum
*     GT_FAIL          - on error
*     GT_BAD_PTR       - on NULL pointer
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     None.
*
*
******************************************************************************/
static GT_STATUS internal_cpssDxChPortSerdesGroupGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_U32   *portSerdesGroupPtr
)
{
    GT_U32      portMacNum; /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                          | CPSS_CH2_E | CPSS_LION2_E
                                          | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(portSerdesGroupPtr);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        *portSerdesGroupPtr = portMacNum;
    }
    else /* DxCh3, xCat */
    {
        if(portNum < 24)
        {
            *portSerdesGroupPtr = portNum / 4;
        }
        else
        {
            *portSerdesGroupPtr = portNum - 18;
        }
    }
    return GT_OK;
}

/******************************************************************************
* cpssDxChPortSerdesGroupGet
*
* DESCRIPTION:
*       Get SERDES port group that may be used in per SERDES group APIs.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
* OUTPUTS:
*       portSerdesGroupPtr  - Pointer to port group number
*                 DxCh3 Giga/2.5 G, xCat GE devices:
*                        Ports       |    SERDES Group
*                        0..3        |      0
*                        4..7        |      1
*                        8..11       |      2
*                        12..15      |      3
*                        16..19      |      4
*                        20..23      |      5
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*                DxCh3 XG devices:
*                        0           |      0
*                        4           |      1
*                        10          |      2
*                        12          |      3
*                        16          |      4
*                        22          |      5
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*               xCat FE devices
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*               Lion devices: Port == SERDES Group
*
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on wrong devNum, portNum
*     GT_FAIL          - on error
*     GT_BAD_PTR       - on NULL pointer
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     None.
*
*
******************************************************************************/
GT_STATUS cpssDxChPortSerdesGroupGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_U32   *portSerdesGroupPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortSerdesGroupGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, portSerdesGroupPtr));

    rc = internal_cpssDxChPortSerdesGroupGet(devNum, portNum, portSerdesGroupPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, portSerdesGroupPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortSerdesCalibrationStartSet
*
* DESCRIPTION:
*       Start calibration on all SERDES of given port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       portNum         - physical port number.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - on not supported port type
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortSerdesCalibrationStartSet
(
    IN GT_U8  devNum,
    IN GT_PHYSICAL_PORT_NUM portNum
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      i;          /* loop index       */
    GT_U32      portGroupId;     /*the port group Id - support multi-port-groups device */
    GT_STATUS   rc = GT_OK; /* return status    */
    GT_U32      startSerdes = 0, serdesesNum = 0;
    CPSS_PORT_INTERFACE_MODE_ENT   ifMode;

    /* Check if device is active and it is a SERDES port */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_FE_E)
    {
        /* no serdes support for FE ports */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    if((rc = cpssDxChPortInterfaceModeGet(devNum,portNum,&ifMode)) != GT_OK)
        return rc;

    if (ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    if((rc = prvCpssDxChPortIfModeCheckAndSerdesNumGet(devNum,portNum,ifMode,&startSerdes,&serdesesNum)) != GT_OK)
        return rc;

    for (i = startSerdes; i < startSerdes + serdesesNum;i++)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[i].calibrationReg0;
        if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

        /* activate calibration start */
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId , regAddr, 15, 1, 1);
        if (rc != GT_OK)
            return rc;
        /* de - activate calibration start */
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId , regAddr, 15, 1, 0);
        if (rc != GT_OK)
            return rc;

    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortSerdesCalibrationStartSet
*
* DESCRIPTION:
*       Start calibration on all SERDES of given port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       portNum         - physical port number.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - on not supported port type
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortSerdesCalibrationStartSet
(
    IN GT_U8  devNum,
    IN GT_PHYSICAL_PORT_NUM portNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortSerdesCalibrationStartSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum));

    rc = internal_cpssDxChPortSerdesCalibrationStartSet(devNum, portNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPortFlowControlModeSet
*
* DESCRIPTION:
*       Sets Flow Control mode on given port.
*
* APPLICABLE DEVICES:
*         Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number, CPU port number.
*       fcMode     - flow control mode.
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
*       For Lion and above:
*       This function also configures insertion of DSA tag for PFC frames.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortFlowControlModeSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_DXCH_PORT_FC_MODE_ENT fcMode
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32 fieldOffset;/* the start bit number in the register */
    GT_U32 value, pfcValue, oppcodeValue = 0;      /* register value */
    GT_STATUS rc;      /* function return value */
    CPSS_CSCD_PORT_TYPE_ENT portCscdType;
    GT_BOOL enablePfcCascade;
    GT_U32 portMacNum; /* MAC number */
    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr;
    GT_U32  dipBtsEn;   /* for Lion2 value of DIP_BTS_xxx_en fields dependent on
                            FC mode */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    switch (fcMode)
    {
        case CPSS_DXCH_PORT_FC_MODE_802_3X_E:

            if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_BOBCAT2_FC_802_3X_NOT_SUPPORTED_TM_MAPPED_PORTS_WA_E))
            {
                rc = prvCpssDxChPortPhysicalPortMapShadowDBGet(devNum, portNum, &portMapShadowPtr);
                if(rc != GT_OK)
                {
                    return rc;
                }

                /* if port mapped to TM - FC not supported */
                if(portMapShadowPtr->portMap.trafficManagerEn)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                }
            }

            value = 0;
            oppcodeValue = 0x1;
            break;

        case CPSS_DXCH_PORT_FC_MODE_PFC_E:

            if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_BOBCAT2_PFC_NOT_SUPPORTED_PORTS_48_71_WA_E))
            {
                /* For ports 48..71 - PFC not supported */
                if((portMacNum >= 48) && (portMacNum <= 71))
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                }
            }

            value = 1;
            oppcodeValue = 0x101;
            break;

        case CPSS_DXCH_PORT_FC_MODE_LL_FC_E:

            if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }

            value = 2;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* use FCA unit for TX FC mode configuration (only if supported)
       Note: RX FC mode is configured to the same value via MAC registers */
    if((PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
    {
        PRV_CPSS_DXCH_PORT_FCA_REGS_ADDR_STC *fcaStcPtr;

        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
        {
            fcaStcPtr = (portMacNum == CPSS_CPU_PORT_NUM_CNS) ?
                 &PRV_DXCH_REG_UNIT_GOP_CPU_FCA_MAC(devNum) :
                 &PRV_DXCH_REG_UNIT_GOP_FCA_MAC(devNum, portMacNum);

            regAddr = fcaStcPtr->FCACtrl;
        }
        else
        {
            regAddr = PRV_DXCH_REG1_UNIT_GOP_FCA_MAC(devNum, portMacNum).FCACtrl;
        }
        fieldOffset = 2;

        /* set FC_MODE */
        rc =  prvCpssHwPpSetRegField(devNum, regAddr, fieldOffset, 2, value);
        if (rc != GT_OK)
        {
            return rc;
        }

        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
        {
            fcaStcPtr = (portMacNum == CPSS_CPU_PORT_NUM_CNS) ?
                 &PRV_DXCH_REG_UNIT_GOP_CPU_FCA_MAC(devNum) :
                 &PRV_DXCH_REG_UNIT_GOP_FCA_MAC(devNum, portMacNum);

            regAddr = fcaStcPtr->L2CtrlReg1;
        }
        else
        {
            regAddr = PRV_DXCH_REG1_UNIT_GOP_FCA_MAC(devNum, portMacNum).L2CtrlReg1;
        }

        fieldOffset = 0;

        /* set OPPCODE */
        rc =  prvCpssHwPpSetRegField(devNum, regAddr, fieldOffset, 16, oppcodeValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if ((fcMode != CPSS_DXCH_PORT_FC_MODE_802_3X_E) &&
        (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E))
    {
        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.
            pfcRegs.pfcSrcPortFcMode;
        regAddr += (portNum/32)*4;
        pfcValue = (fcMode == CPSS_DXCH_PORT_FC_MODE_PFC_E) ? 0 : 1;
        rc = prvCpssHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, portNum%32, 1, pfcValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_GE_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_GE_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = 2;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = 1;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldLength = 2;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldOffset = 14;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XLG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XLG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldLength = 2;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldOffset = 14;
    }

    rc = prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

        /* The rule is when the field “fc_mode” is configured to
             802_3x_MODE” in the following register:
            Port MAC Control Register2, then the fields
                 DIP_BTS_690_fix_en“ and “DIP_BTS_693_en” for GE ports
                                must to be configured to disabled (0)
            for XLG ports the fields “DIP_BTS_550_en“ and “DIP_BTS_677_en”
            in the corresponding XLG register must be configured to disabled (0).
            Fortunately in both GE and XLG registers appropriate bits are 8:9,
            so just register address must be specific.
        */
        PRV_CPSS_DXCH_PORT_MAC_VAR_REG_MAC(devNum,portNum,PRV_CPSS_PORT_GE_E,ppfcControl,(&regAddr));
        dipBtsEn = (CPSS_DXCH_PORT_FC_MODE_802_3X_E == fcMode) ? 0 : 0x3;
        regDataArray[PRV_CPSS_PORT_GE_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldData = dipBtsEn;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = 2;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = 8;

        regDataArray[PRV_CPSS_PORT_XG_E].regAddr = PRV_CPSS_SW_PTR_ENTRY_UNUSED;

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                    perPortRegs[portNum].macRegsPerType[PRV_CPSS_PORT_XLG_E].ppfcControl;
        regDataArray[PRV_CPSS_PORT_XLG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldData = dipBtsEn;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldLength = 2;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldOffset = 8;

        rc = prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* for cascade port and pfc mode, enable insertion of DSA tag to PFC packets */
    if (fcMode == CPSS_DXCH_PORT_FC_MODE_PFC_E)
    {
        /* get cascade port configuration */
        rc = cpssDxChCscdPortTypeGet(devNum,portNum,CPSS_PORT_DIRECTION_TX_E,&portCscdType);
        if (rc != GT_OK)
        {
            return rc;
        }
        enablePfcCascade = (portCscdType == CPSS_CSCD_PORT_NETWORK_E) ?
                            GT_FALSE : GT_TRUE;
    }
    else
    {
        enablePfcCascade = GT_FALSE;
    }

   return prvCpssDxChPortPfcCascadeEnableSet(devNum,portNum,enablePfcCascade);
}

/*******************************************************************************
* cpssDxChPortFlowControlModeSet
*
* DESCRIPTION:
*       Sets Flow Control mode on given port.
*
* APPLICABLE DEVICES:
*         Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number, CPU port number.
*       fcMode     - flow control mode.
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
*       For Lion and above:
*       This function also configures insertion of DSA tag for PFC frames.
*
*******************************************************************************/
GT_STATUS cpssDxChPortFlowControlModeSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_DXCH_PORT_FC_MODE_ENT fcMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortFlowControlModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, fcMode));

    rc = internal_cpssDxChPortFlowControlModeSet(devNum, portNum, fcMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, fcMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPortFlowControlModeGet
*
* DESCRIPTION:
*       Gets Flow Control mode on given port.
*
* APPLICABLE DEVICES:
*         Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number, CPU port number.
*
* OUTPUTS:
*       fcModePtr  - flow control mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortFlowControlModeGet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT CPSS_DXCH_PORT_FC_MODE_ENT   *fcModePtr
)
{

    GT_U32 regAddr;    /* register address */
    GT_U32 portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32 fieldOffset;/* the start bit number in the register */
    GT_U32 value;      /* register value */
    GT_STATUS rc;
    PRV_CPSS_PORT_TYPE_ENT portMacType;
    GT_U32 portMacNum; /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(fcModePtr);

    /* use FCA unit (instead of MAC) for FC mode configuration (only if supported) */
    if((PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
    {
        PRV_CPSS_DXCH_PORT_FCA_REGS_ADDR_STC *fcaStcPtr;

        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
        {
            fcaStcPtr = (portMacNum == CPSS_CPU_PORT_NUM_CNS) ?
                 &PRV_DXCH_REG_UNIT_GOP_CPU_FCA_MAC(devNum) :
                 &PRV_DXCH_REG_UNIT_GOP_FCA_MAC(devNum, portMacNum);

            regAddr = fcaStcPtr->FCACtrl;
        }
        else
        {
            regAddr = PRV_DXCH_REG1_UNIT_GOP_FCA_MAC(devNum, portMacNum).FCACtrl;
        }
        fieldOffset = 2;

        /* set FC_MODE */
        rc =  prvCpssHwPpGetRegField(devNum, regAddr, fieldOffset, 2, &value);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum);
        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portMacNum,portMacType,&regAddr);
        if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

        if(portMacType >= PRV_CPSS_PORT_XG_E)
        {
            fieldOffset = 14;
        }
        else
        {
            fieldOffset = 1;
        }

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);

        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, fieldOffset, 2, &value);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    *fcModePtr = (CPSS_DXCH_PORT_FC_MODE_ENT)value;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortFlowControlModeGet
*
* DESCRIPTION:
*       Gets Flow Control mode on given port.
*
* APPLICABLE DEVICES:
*         Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number, CPU port number.
*
* OUTPUTS:
*       fcModePtr  - flow control mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortFlowControlModeGet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT CPSS_DXCH_PORT_FC_MODE_ENT   *fcModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortFlowControlModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, fcModePtr));

    rc = internal_cpssDxChPortFlowControlModeGet(devNum, portNum, fcModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, fcModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortMacResetStateSet
*
* DESCRIPTION:
*       Set MAC Reset state on specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number (or CPU port)
*       state    - Reset state
*                  GT_TRUE   - Port MAC is under Reset
*                  GT_FALSE - Port MAC is Not under Reset, normal operation
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
static GT_STATUS internal_cpssDxChPortMacResetStateSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL   state
)
{
    return prvCpssDxChPortMacResetStateSet(devNum, portNum, state);
}

/*******************************************************************************
* cpssDxChPortMacResetStateSet
*
* DESCRIPTION:
*       Set MAC Reset state on specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number (or CPU port)
*       state    - Reset state
*                  GT_TRUE   - Port MAC is under Reset
*                  GT_FALSE - Port MAC is Not under Reset, normal operation
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
GT_STATUS cpssDxChPortMacResetStateSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL   state
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMacResetStateSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, state));

    rc = internal_cpssDxChPortMacResetStateSet(devNum, portNum, state);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, state));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortMacResetStateSet
*
* DESCRIPTION:
*       Set MAC and XPCS Reset state on specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
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
GT_STATUS prvCpssDxChPortMacResetStateSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL   state
)
{
    GT_STATUS rc;           /* return code */
    GT_U32 regAddr;         /* register address */
    PRV_CPSS_PORT_TYPE_ENT portMacType; /* MAC unit used by port */
    GT_U32  portMacNum;      /* MAC number */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum);
    if(portMacType >= PRV_CPSS_PORT_XG_E)
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            rc = prvCpssDxChPortXgPcsResetStateSet(devNum,portNum,state);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,portMacType,&regAddr);
        regDataArray[portMacType].regAddr = regAddr;
        regDataArray[portMacType].fieldData = BOOL2BIT_MAC(!state);
        regDataArray[portMacType].fieldLength = 1;
        regDataArray[portMacType].fieldOffset = 1;
    }
    else
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portMacNum,portMacType,&regAddr);
        regDataArray[portMacType].regAddr = regAddr;
        regDataArray[portMacType].fieldData = BOOL2BIT_MAC(state);
        regDataArray[portMacType].fieldLength = 1;
        regDataArray[portMacType].fieldOffset = 6;
    }
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    /* set MACResetn bit */
    rc = prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
    if(rc != GT_OK)
    {
        return rc;
    }
    /* let system stabilise */
    HW_WAIT_MILLISECONDS_MAC(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,10);

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortMacResetStateGet
*
* DESCRIPTION:
*       Get MAC Reset state on specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number (including CPU)
*
* OUTPUTS:
*       statePtr - Reset state
*               GT_TRUE   - Port MAC is under Reset
*               GT_FALSE - Port MAC is Not under Reset, normal operation
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - statePtr == NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortMacResetStateGet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL   *statePtr
)
{
    GT_STATUS rc;
    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* value to write into the register */
    GT_U32 offset;          /* bit number inside register       */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT portMacType; /* MAC unit used by port */
    GT_U32  portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum);
    if(PRV_CPSS_PORT_NOT_EXISTS_E == portMacType)
    {
        *statePtr = GT_TRUE;
        return GT_OK;
    }

    if(portMacType >= PRV_CPSS_PORT_XG_E)
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum, portMacNum, portMacType,
                                            &regAddr);
        offset = 1;
    }
    else
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum, portMacNum, portMacType,
                                             &regAddr);
        offset = 6;
    }
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    /* get MACResetn bit */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(
                                                                devNum, portMacNum);
    if((rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                                offset, 1, &value)) != GT_OK)
    {
        return rc;
    }

    if(portMacType >= PRV_CPSS_PORT_XG_E)
    {
        *statePtr = (1 == value) ? GT_FALSE : GT_TRUE;
    }
    else
    {
        *statePtr = BIT2BOOL_MAC(value);
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortForward802_3xEnableSet
*
* DESCRIPTION:
*       Enable/disable forwarding of 802.3x Flow Control frames to the ingress
*       pipeline of a specified port. Processing of  802.3x Flow Control frames
*       is done like regular data frames if forwarding enabled.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (CPU port not supported)
*       enable   - GT_TRUE:  forward 802.3x frames to the ingress pipe,
*                  GT_FALSE: do not forward 802.3x frames to the ingress pipe.
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
*       A packet is considered a valid Flow Control packet (i.e., it may be used
*       to halt the port’s packet transmission if it is an XOFF packet, or to
*       resume the port’s packets transmission, if it is an XON packet) if all of
*       the following are true:
*       - Packet’s Length/EtherType field is 88-08
*       - Packet’s OpCode field is 00-01
*       - Packet’s MAC DA is 01-80-C2-00-00-01 or the port’s configured MAC Address
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortForward802_3xEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL enable
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 value;      /* register value */
    GT_U32 offset;     /* bit number inside register       */
    GT_U32 portMacNum; /* MAC number */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    if(PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portMacNum].portType >= PRV_CPSS_PORT_XG_E)
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    if((GT_TRUE == enable) &&
       (GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,PRV_CPSS_DXCH_XCAT_FC_FORWARD_NOT_FUNCTIONAL_WA_E)))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    value = BOOL2BIT_MAC(enable);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL4_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_GE_E,&regAddr);
        offset = 0;
    }
    else
    {
        PRV_CPSS_DXCH_PORT_SERIAL_PARAM_CTRL_REG_MAC(devNum,portMacNum,&regAddr);
        offset = 3;
    }

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_GE_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = offset;
    }

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E ||
        PRV_CPSS_SIP_5_CHECK_MAC(devNum) ||
        PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL4_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XG_E,&regAddr);
        offset = 5;
    }
    else
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XG_E,&regAddr);
        offset = 4;
    }

    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldOffset = offset;
    }

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E ||
        PRV_CPSS_SIP_5_CHECK_MAC(devNum) ||
        PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL4_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XLG_E,&regAddr);
    }
    else
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XLG_E,&regAddr);
    }

    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XLG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldOffset = offset;
    }

    return prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
}

/*******************************************************************************
* cpssDxChPortForward802_3xEnableSet
*
* DESCRIPTION:
*       Enable/disable forwarding of 802.3x Flow Control frames to the ingress
*       pipeline of a specified port. Processing of  802.3x Flow Control frames
*       is done like regular data frames if forwarding enabled.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (CPU port not supported)
*       enable   - GT_TRUE:  forward 802.3x frames to the ingress pipe,
*                  GT_FALSE: do not forward 802.3x frames to the ingress pipe.
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
*       A packet is considered a valid Flow Control packet (i.e., it may be used
*       to halt the port’s packet transmission if it is an XOFF packet, or to
*       resume the port’s packets transmission, if it is an XON packet) if all of
*       the following are true:
*       - Packet’s Length/EtherType field is 88-08
*       - Packet’s OpCode field is 00-01
*       - Packet’s MAC DA is 01-80-C2-00-00-01 or the port’s configured MAC Address
*
*******************************************************************************/
GT_STATUS cpssDxChPortForward802_3xEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortForward802_3xEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChPortForward802_3xEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPortForward802_3xEnableGet
*
* DESCRIPTION:
*       Get status of 802.3x frames forwarding on a specified port
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (CPU port not supported)
*
* OUTPUTS:
*       enablePtr - status of 802.3x frames forwarding
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       A packet is considered a valid Flow Control packet (i.e., it may be used
*       to halt the port’s packet transmission if it is an XOFF packet, or to
*       resume the port’s packets transmission, if it is an XON packet) if all of
*       the following are true:
*       - Packet’s Length/EtherType field is 88-08
*       - Packet’s OpCode field is 00-01
*       - Packet’s MAC DA is 01-80-C2-00-00-01 or the port’s configured MAC Address
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortForward802_3xEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32 value;      /* register value */
    GT_STATUS rc;      /* return code */
    GT_U32 offset;     /* bit number inside register       */
    PRV_CPSS_PORT_TYPE_ENT portMacType; /* type of mac unit of port */
    GT_U32 portMacNum; /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);


    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum);
    if(portMacType >= PRV_CPSS_PORT_XG_E)
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E ||
            PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E ||
            PRV_CPSS_SIP_5_CHECK_MAC(devNum))
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL4_REG_MAC(devNum,portMacNum,portMacType,&regAddr);
            offset = 5;
        }
        else
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,portMacType,&regAddr);
            offset = 4;
        }
    }
    else
    {
        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL4_REG_MAC(devNum,portMacNum,portMacType,&regAddr);
            offset = 0;
        }
        else
        {
            PRV_CPSS_DXCH_PORT_SERIAL_PARAM_CTRL_REG_MAC(devNum,portMacNum,&regAddr);
            offset = 3;
        }
    }
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);
    if((rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, offset, 1, &value)) != GT_OK)
        return rc;

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortForward802_3xEnableGet
*
* DESCRIPTION:
*       Get status of 802.3x frames forwarding on a specified port
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (CPU port not supported)
*
* OUTPUTS:
*       enablePtr - status of 802.3x frames forwarding
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       A packet is considered a valid Flow Control packet (i.e., it may be used
*       to halt the port’s packet transmission if it is an XOFF packet, or to
*       resume the port’s packets transmission, if it is an XON packet) if all of
*       the following are true:
*       - Packet’s Length/EtherType field is 88-08
*       - Packet’s OpCode field is 00-01
*       - Packet’s MAC DA is 01-80-C2-00-00-01 or the port’s configured MAC Address
*
*******************************************************************************/
GT_STATUS cpssDxChPortForward802_3xEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortForward802_3xEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChPortForward802_3xEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortForwardUnknownMacControlFramesEnableSet
*
* DESCRIPTION:
*       Enable/disable forwarding of unknown MAC control frames to the ingress
*       pipeline of a specified port. Processing of unknown MAC control frames
*       is done like regular data frames if forwarding enabled.
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (CPU port not supported)
*       enable   - GT_TRUE:  forward unknown MAC control frames to the ingress pipe,
*                  GT_FALSE: do not forward unknown MAC control frames to
*                               the ingress pipe.
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
*       A packet is considered as an unknown MAC control frame if all of
*       the following are true:
*       - Packet’s Length/EtherType field is 88-08
*       - Packet’s OpCode field is not 00-01 and not 01-01
*           OR
*         Packet’s MAC DA is not 01-80-C2-00-00-01 and not the port’s configured
*           MAC Address
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortForwardUnknownMacControlFramesEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL enable
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32 value;      /* register value */
    GT_U32 portMacNum; /* MAC number */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    value = BOOL2BIT_MAC(enable);

    if (CPSS_PP_FAMILY_DXCH_XCAT2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum].serialParameters1;
        if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);

        return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, 2, 1, value);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portMacNum].serialParameters1;
    }
    else
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL4_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_GE_E,&regAddr);
    }

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_GE_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = 2;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL4_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldOffset = 7;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL4_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XLG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XLG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldOffset = 7;
    }

    return prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
}

/*******************************************************************************
* cpssDxChPortForwardUnknownMacControlFramesEnableSet
*
* DESCRIPTION:
*       Enable/disable forwarding of unknown MAC control frames to the ingress
*       pipeline of a specified port. Processing of unknown MAC control frames
*       is done like regular data frames if forwarding enabled.
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (CPU port not supported)
*       enable   - GT_TRUE:  forward unknown MAC control frames to the ingress pipe,
*                  GT_FALSE: do not forward unknown MAC control frames to
*                               the ingress pipe.
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
*       A packet is considered as an unknown MAC control frame if all of
*       the following are true:
*       - Packet’s Length/EtherType field is 88-08
*       - Packet’s OpCode field is not 00-01 and not 01-01
*           OR
*         Packet’s MAC DA is not 01-80-C2-00-00-01 and not the port’s configured
*           MAC Address
*
*******************************************************************************/
GT_STATUS cpssDxChPortForwardUnknownMacControlFramesEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortForwardUnknownMacControlFramesEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChPortForwardUnknownMacControlFramesEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPortForwardUnknownMacControlFramesEnableGet
*
* DESCRIPTION:
*       Get current status of unknown MAC control frames
*           forwarding on a specified port
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number (CPU port not supported)
*
* OUTPUTS:
*       enablePtr   - status of unknown MAC control frames forwarding
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       A packet is considered as an unknown MAC control frame if all of
*       the following are true:
*       - Packet’s Length/EtherType field is 88-08
*       - Packet’s OpCode field is not 00-01 and not 01-01
*           OR
*         Packet’s MAC DA is not 01-80-C2-00-00-01 and not the port’s configured
*           MAC Address
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortForwardUnknownMacControlFramesEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32 value;      /* register value */
    GT_STATUS rc;      /* return code */
    GT_U32 offset;     /* bit number inside register       */
    PRV_CPSS_PORT_TYPE_ENT portMacType; /* type of mac unit of port */
    GT_U32 portMacNum; /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum].serialParameters1;
        if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);

        if((rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, 2, 1, &value)) != GT_OK)
            return rc;
    }
    else
    {
        portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum);

        if((PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE) &&
           (portMacType < PRV_CPSS_PORT_XG_E))
        {
            offset = 2;
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portMacNum].serialParameters1;
        }
        else
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL4_REG_MAC(devNum,portMacNum,portMacType,&regAddr);
            if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

            if(portMacType >= PRV_CPSS_PORT_XG_E)
            {
                offset = 7;
            }
            else
            {
                offset = 2;
            }
        }

        if((rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, offset, 1, &value)) != GT_OK)
            return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortForwardUnknownMacControlFramesEnableGet
*
* DESCRIPTION:
*       Get current status of unknown MAC control frames
*           forwarding on a specified port
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number (CPU port not supported)
*
* OUTPUTS:
*       enablePtr   - status of unknown MAC control frames forwarding
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       A packet is considered as an unknown MAC control frame if all of
*       the following are true:
*       - Packet’s Length/EtherType field is 88-08
*       - Packet’s OpCode field is not 00-01 and not 01-01
*           OR
*         Packet’s MAC DA is not 01-80-C2-00-00-01 and not the port’s configured
*           MAC Address
*
*******************************************************************************/
GT_STATUS cpssDxChPortForwardUnknownMacControlFramesEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortForwardUnknownMacControlFramesEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChPortForwardUnknownMacControlFramesEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* prvCpssDxChPortInbandAutonegMode
*
* DESCRIPTION:
*       Set inband autoneg mode accordingly to required ifMode
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number (CPU port not supported)
*       ifMode      - port interface mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant when <InBandAnEn> is set to 1.
*       Not relevant for the CPU port.
*       This field may only be changed when the port link is down.
*       In existing devices inband auto-neg. disabled by default.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortInbandAutonegMode
(
    IN GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN CPSS_PORT_INTERFACE_MODE_ENT ifMode
)
{
    GT_U32      regAddr;/* register address */
    GT_U32      value;  /* register value */
    GT_U32      portMacNum;      /* MAC number */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    switch(ifMode)
    {
        case CPSS_PORT_INTERFACE_MODE_SGMII_E:
        case CPSS_PORT_INTERFACE_MODE_QSGMII_E:
            value = 1;
            break;

        case CPSS_PORT_INTERFACE_MODE_1000BASE_X_E:
            value = 0;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_GE_E,&regAddr);
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    regDataArray[PRV_CPSS_PORT_GE_E].regAddr = regAddr;
    regDataArray[PRV_CPSS_PORT_GE_E].fieldData = value;
    regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = 1;
    regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = 0;

    return prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
}

/******************************************************************************
* prvCpssDxChPortTraceDelay
*
* DESCRIPTION:
*       Make delay and trace it
*
* INPUTS:
*       devNum      - PP device number
*       portGroupId - ports group number
*       millisec   -  the delay in millisec
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on fail
*
* COMMENTS:
*       None.
*
******************************************************************************/
GT_STATUS prvCpssDxChPortTraceDelay
(
    IN GT_U8       devNum,
    IN GT_U32      portGroupId,
    IN GT_U32      millisec
)
{
    if(millisec > tickTimeMsec/2)
    {/* if required delay more then half of tick time do OS delay */
        HW_WAIT_MILLISECONDS_MAC(devNum,portGroupId,millisec);
    }
    else
    {/* if delay is short do busy wait */
        if (prvCpssDrvTraceHwDelay[devNum] == GT_TRUE)
        {
            cpssTraceHwAccessDelay(devNum,portGroupId,millisec);
        }
        {
            GT_STATUS   rc; /* return code */
            GT_U32  startTimeStamp, /* value of time stamp register on delay start */
                    currentTimeStamp;/* current value of time stamp register */
            GT_U32  diff; /* emount of core clocks passed */
            GT_U32  passed; /* delay time passed in millisec */
            GT_U32  value;  /* dummy - user defined register value */
            GT_U32  coreClockTime; /* in millisec */
            GT_U32  regAddr;    /* address of register to read */

            coreClockTime = PRV_CPSS_PP_MAC(devNum)->coreClock * 1000;
            /* register must be not in MG unit */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum))
            {
                regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).bridgeEngineConfig.
                                                            bridgeCommandConfig0;
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    bridgeRegs.bridgeGlobalConfigRegArray[0];
            }
            if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
            }
            /* read register to cause time stamp register update */
            if (prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,
                    regAddr, &value) != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }

            rc = cpssDxChVntPortGroupLastReadTimeStampGet(devNum, (1<<portGroupId),
                                                          &startTimeStamp);
            if(rc != GT_OK)
            {
                return rc;
            }

            do
            {
                if (prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,
                        regAddr, &value) != GT_OK)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
                }

                rc = cpssDxChVntPortGroupLastReadTimeStampGet(devNum, (1<<portGroupId),
                                                              &currentTimeStamp);
                if(rc != GT_OK)
                {
                    return rc;
                }

                /* calculate difference between current and start */
                if (currentTimeStamp > startTimeStamp)
                {
                    diff = currentTimeStamp - startTimeStamp;
                }
                else
                {/* take care of wraparound */
                   diff = (0xFFFFFFFFU - startTimeStamp) + currentTimeStamp;
                }

                passed = diff/coreClockTime;

                #ifdef ASIC_SIMULATION
                    /* single iteration is enough ... as we 'checked' that above
                       logic access existing registers */
                    passed = millisec;
                #endif /*ASIC_SIMULATION*/
            }while(passed < millisec);
        }
    }

    return GT_OK;
}

static GT_STATUS prvCpssDxChPortDevInfo
(
    IN  GT_U8  devNum,
    OUT GT_U32 *devId,
    OUT GT_U32 *revNum
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    *devId = PRV_CPSS_PP_MAC(devNum)->devType;
    *revNum = PRV_CPSS_PP_MAC(devNum)->revision;
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortIfCfgInit
*
* DESCRIPTION:
*       Initialize port interface mode configuration method in device object
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_OUT_OF_CPU_MEM   - port object allocation failed
*       GT_FAIL             - wrong devFamily
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortIfCfgInit
(
    IN GT_U8    devNum
)
{
    GT_STATUS rc;
    CPSS_PP_FAMILY_TYPE_ENT devFamily;      /* device family */
    PRV_CPSS_PORT_TYPE_OPTIONS_ENT portTypeOptions;/* options available for port */
    GT_U32  ticks;  /* system clock rate - ticks per second */

    /* allocate Init object */
    PRV_CPSS_DXCH_DEV_OBJ_MAC(devNum)->portPtr = (PRV_CPSS_DXCH_PORT_OBJ_PTR)cpssOsMalloc(sizeof(PRV_CPSS_DXCH_PORT_OBJ_STC));
    if (PRV_CPSS_DXCH_DEV_OBJ_MAC(devNum)->portPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    if((PRV_CPSS_PP_MAC(devNum)->devFamily <= CPSS_PP_FAMILY_START_DXCH_E) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_END_DXCH_E))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    devFamily = PRV_CPSS_PP_MAC(devNum)->devFamily - CPSS_PP_FAMILY_START_DXCH_E - 1;

    rc = prvCpssDxChPortIfFunctionsObjInit(PRV_CPSS_DXCH_DEV_OBJ_MAC(devNum)->portPtr, PRV_CPSS_PP_MAC(devNum)->devFamily);
    if (rc != GT_OK)
    {
        return rc;
    }
#if 0
    for(ifMode = CPSS_PORT_INTERFACE_MODE_REDUCED_10BIT_E; ifMode < CPSS_PORT_INTERFACE_MODE_NA_E; ifMode++)
    {
        PRV_CPSS_DXCH_DEV_OBJ_MAC(devNum)->portPtr->setPortInterfaceMode[ifMode] = portIfModeFuncPtrArray[devFamily][ifMode];
    }

    PRV_CPSS_DXCH_DEV_OBJ_MAC(devNum)->portPtr->getPortInterfaceMode = portIfModeGetFuncPtrArray[devFamily];
#endif
    for(portTypeOptions = PRV_CPSS_XG_PORT_XG_ONLY_E; portTypeOptions < PRV_CPSS_XG_PORT_OPTIONS_MAX_E; portTypeOptions++)
    {
        PRV_CPSS_DXCH_DEV_OBJ_MAC(devNum)->portPtr->setPortSpeed[portTypeOptions] =  portSpeedSetFuncPtrArray[devFamily][portTypeOptions];
    }

    PRV_CPSS_DXCH_DEV_OBJ_MAC(devNum)->portPtr->getPortSpeed = portSpeedGetFuncPtrArray[devFamily];

    PRV_CPSS_DXCH_DEV_OBJ_MAC(devNum)->portPtr->setSerdesPowerStatus = portSerdesPowerStatusSetFuncPtrArray[devFamily];

    /* calculate length of tick in mSec for exact delay function */
    cpssOsGetSysClockRate(&ticks);
    tickTimeMsec = 1000/ticks;

    if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)
        || (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum))
        || (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
    {
        GT_STATUS       rc;          /* return code */
        HWS_OS_FUNC_PTR funcPtrsStc; /* pointers to OS/CPU dependent system calls */
        CPSS_DXCH_PORT_COMBO_PARAMS_STC *comboPortsInfoArray; /* ptr to combo
                                ports array for shorten code and quicker access */
        GT_U32  i, j;   /* iterators */

        cpssOsMemSet(&funcPtrsStc, 0, sizeof(funcPtrsStc));

        funcPtrsStc.osExactDelayPtr = prvCpssDxChPortTraceDelay;
        funcPtrsStc.osTimerWkPtr    = cpssOsTimerWkAfter;
        funcPtrsStc.osMemSetPtr     = cpssOsMemSet;
#ifdef OS_MALLOC_MEMORY_LEAKAGE_DBG
        funcPtrsStc.osFreePtr       = myCpssOsFree;
        funcPtrsStc.osMallocPtr     = myCpssOsMalloc;
#else  /*! OS_MALLOC_MEMORY_LEAKAGE_DBG */
        funcPtrsStc.osFreePtr       = cpssOsFree;
        funcPtrsStc.osMallocPtr     = cpssOsMalloc;
#endif /*! OS_MALLOC_MEMORY_LEAKAGE_DBG */
        funcPtrsStc.osMemCopyPtr    = (MV_OS_MEM_COPY_FUNC)cpssOsMemCpy;
        funcPtrsStc.sysDeviceInfo   = prvCpssDxChPortDevInfo;
        funcPtrsStc.serverRegSetAccess = cpssDrvHwPpResetAndInitControllerWriteReg;
        funcPtrsStc.serverRegGetAccess = cpssDrvHwPpResetAndInitControllerReadReg;

        if (PRV_CPSS_SIP_5_CHECK_MAC(devNum))
        {
            funcPtrsStc.serdesRegGetAccess =
                (MV_SERDES_REG_ACCESS_GET)prvCpssGenPortGroupSerdesReadRegBitMask;
            funcPtrsStc.serdesRegSetAccess =
                (MV_SERDES_REG_ACCESS_SET)prvCpssGenPortGroupSerdesWriteRegBitMask;
            (GT_VOID)multiLaneConfigSupport(GT_FALSE);

            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT3_E)
            {
                CPSS_LOG_INFORMATION_MAC("Calling: hwsBobKIfInit(devNum[%d], *funcPtr)", devNum);
                CPSS_TBD_BOOKMARK_BOBCAT3
                /* PATCH : indicate the HWS that we use bobcat3 */
                hwsBobcat3ForceModeSet();
                rc = hwsBobKIfInit(devNum, &funcPtrsStc);
            }
            else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_ALDRIN_E)
            {
                CPSS_LOG_INFORMATION_MAC("Calling: hwsBobKIfInit(devNum[%d], *funcPtr)", devNum);
                CPSS_TBD_BOOKMARK_ALDRIN
                /* PATCH : indicate the HWS that we use bobcat3 */
                hwsBobcat3ForceModeSet();
                rc = hwsBobKIfInit(devNum, &funcPtrsStc);
            }
            else
            if(PRV_CPSS_PP_MAC(devNum)->devSubFamily == CPSS_PP_SUB_FAMILY_BOBCAT2_BOBK_E)
            {
                CPSS_LOG_INFORMATION_MAC("Calling: hwsBobKIfInit(devNum[%d], *funcPtr)", devNum);
                rc = hwsBobKIfInit(devNum, &funcPtrsStc);
            }
            else
            {
                CPSS_LOG_INFORMATION_MAC("Calling: hwsBobcat2IfInit(devNum[%d], *funcPtr)", devNum);
                rc = hwsBobcat2IfInit(devNum, &funcPtrsStc);
            }
        }
        else
        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
        {
            funcPtrsStc.serdesRegGetAccess =
                (MV_SERDES_REG_ACCESS_GET)prvCpssGenPortGroupSerdesReadRegBitMask;
            funcPtrsStc.serdesRegSetAccess =
                (MV_SERDES_REG_ACCESS_SET)prvCpssGenPortGroupSerdesWriteRegBitMask;
            (GT_VOID)multiLaneConfigSupport(GT_FALSE);
            CPSS_LOG_INFORMATION_MAC("Calling: mvHwsAlleycat3IfInit(devNum[%d], *funcPtr)", devNum);
            rc = mvHwsAlleycat3IfInit(devNum, &funcPtrsStc);
        }
        else /* lion2 / hooper */
        {
            switch(PRV_CPSS_PP_MAC(devNum)->devType)
            {
                case CPSS_LION2_HOOPER_PORT_GROUPS_0123_DEVICES_CASES_MAC:
                    CPSS_LOG_INFORMATION_MAC("Calling: hwsHooperIfInit(devNum[%d], *funcPtr)", devNum);
                    rc = hwsHooperIfInit(devNum, &funcPtrsStc);
                    break;
                default:
                    CPSS_LOG_INFORMATION_MAC("Calling: hwsLion2IfInit(devNum[%d], *funcPtr)", devNum);
                    rc = hwsLion2IfInit(devNum, &funcPtrsStc);
                    break;
            }
        }

        if((rc != GT_OK) && (rc != GT_ALREADY_EXIST))
        {
            CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
            return rc;
        }

        /* initialize only cpssDriver for HW access, skip the rest */
        if (dxChInitRegDefaults == GT_TRUE)
            return GT_OK;

        if(CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
        {
            GT_U32  portGroup;
            GT_U32  localPort;
            CPSS_PORT_INTERFACE_MODE_ENT    ifModeHw;

            for(i = 0; i < CPSS_MAX_PORTS_NUM_CNS; i++)
            {
                PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, i);

                if(CPSS_CPU_PORT_NUM_CNS == i)
                {
                    continue;
                }

                rc = prvCpssDxChPortLion2InterfaceModeHwGet(devNum, i, &ifModeHw);
                if((GT_OK == rc) && (ifModeHw != CPSS_PORT_INTERFACE_MODE_NA_E))
                {/* if found one configured port we are not after HW reset -
                    skip following resets */
                    break;
                }

                portGroup = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, i);
                localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, i);
                CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPcsReset(devNum[%d], portGroup[%d], pcsNum[%d], pcsType[%d], action[%d])", devNum, portGroup, localPort, MMPCS, RESET);
                rc = mvHwsPcsReset(devNum, portGroup, localPort, MMPCS, RESET);
                if(rc != GT_OK)
                {
                    CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
                    return rc;
                }

                if(9 == localPort)
                {
                    CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPcsReset(devNum[%d], portGroup[%d], pcsNum[%d], pcsType[%d], action[%d])", devNum, portGroup, 6/* xpcsNum of port 9 in RXAUI*/, XPCS, RESET);
                    rc = mvHwsPcsReset(devNum, portGroup,
                                        6/* xpcsNum of port 9 in RXAUI*/, XPCS,
                                        RESET);
                    if(rc != GT_OK)
                    {
                        CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
                        return rc;
                    }
                }

                if(11 == localPort)
                {
                    CPSS_LOG_INFORMATION_MAC("Calling: mvHwsPcsReset(devNum[%d], portGroup[%d], pcsNum[%d], pcsType[%d], action[%d])", devNum, portGroup, 7/* xpcsNum of port 11 in RXAUI*/, XPCS, RESET);
                    rc = mvHwsPcsReset(devNum, portGroup,
                                        7/* xpcsNum of port 11 in RXAUI*/,
                                        XPCS, RESET);
                    if(rc != GT_OK)
                    {
                        CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
                        return rc;
                    }
                }
            }
        }

        /* init combo ports array */
        comboPortsInfoArray = PRV_CPSS_DXCH_PP_MAC(devNum)->port.comboPortsInfoArray;
        for(i = 0; i < PRV_CPSS_MAX_PP_PORTS_NUM_CNS; i++)
        {
            comboPortsInfoArray[i].preferredMacIdx = CPSS_DXCH_PORT_COMBO_NA_MAC_CNS;
            for(j = 0; j < CPSS_DXCH_PORT_MAX_MACS_IN_COMBO_CNS; j++)
            {
                comboPortsInfoArray[i].macArray[j].macNum = CPSS_DXCH_PORT_COMBO_NA_MAC_CNS;
            }
        }

        /* init PRBS modes ports array */
        for(i = 0; i < PRV_CPSS_MAX_PP_PORTS_NUM_CNS; i++)
        {
            PRV_CPSS_DXCH_PP_MAC(devNum)->port.prbsMode[i] = CPSS_DXCH_DIAG_TRANSMIT_MODE_REGULAR_E;
        }

        /* init PRBS Serdes Counters Clear On Read Enable */
        for(i = 0; i < PRV_CPSS_MAX_MAC_PORTS_NUM_CNS; i++)
        {
            for(j = 0; j < PRV_CPSS_MAX_PORT_LANES_CNS; j++)
            {
                PRV_CPSS_DXCH_PP_MAC(devNum)->port.prbsSerdesCountersClearOnReadEnable[i][j] = GT_TRUE;
            }
        }
    }

    return GT_OK;
}


/******************************************************************************
* internal_cpssDxChPortPeriodicFlowControlIntervalSet
*
* DESCRIPTION:
*       Set Periodic Flow Control interval.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - physical device number
*       portType  - port type
*       value     - The interval in microseconds between two successive
*                   Flow Control frames that are sent periodically by the port.
*                   (APPLICABLE RANGES: 0..171798691)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad devNum, portType
*     GT_OUT_OF_RANGE  - on bad value
*     GT_FAIL          - on error
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     None.
*
******************************************************************************/
static GT_STATUS internal_cpssDxChPortPeriodicFlowControlIntervalSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_PORT_PERIODIC_FC_TYPE_ENT portType,
    IN  GT_U32                              value
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      regValue;   /* register value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
                                          CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    switch (portType)
    {
        case CPSS_DXCH_PORT_PERIODIC_FC_TYPE_GIG_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                miscellaneousRegs.lms0PeriodicFlowControlInterval0;
                break;
        case CPSS_DXCH_PORT_PERIODIC_FC_TYPE_XG_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                miscellaneousRegs.lms1PeriodicFlowControlInterval1;
                break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    if(value > 0xFFFFFFFF / 25)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    regValue = value * 25; /* the register value is in 40 ns. */

    return prvCpssDrvHwPpWriteRegister(devNum, regAddr, regValue);
}

/******************************************************************************
* cpssDxChPortPeriodicFlowControlIntervalSet
*
* DESCRIPTION:
*       Set Periodic Flow Control interval.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - physical device number
*       portType  - port type
*       value     - The interval in microseconds between two successive
*                   Flow Control frames that are sent periodically by the port.
*                   (APPLICABLE RANGES: 0..171798691)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad devNum, portType
*     GT_OUT_OF_RANGE  - on bad value
*     GT_FAIL          - on error
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     None.
*
******************************************************************************/
GT_STATUS cpssDxChPortPeriodicFlowControlIntervalSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_PORT_PERIODIC_FC_TYPE_ENT portType,
    IN  GT_U32                              value
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPeriodicFlowControlIntervalSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portType, value));

    rc = internal_cpssDxChPortPeriodicFlowControlIntervalSet(devNum, portType, value);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portType, value));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/******************************************************************************
* internal_cpssDxChPortPeriodicFlowControlIntervalGet
*
* DESCRIPTION:
*       Get Periodic Flow Control interval.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - physical device number
*       portType  - port type
*
* OUTPUTS:
*       valuePtr  - pointer to interval value in microseconds between two
*                   successive Flow Control frames that are sent periodically
*                   by the port.
*                   (APPLICABLE RANGES: 0..171798691)
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad devNum, portType
*     GT_FAIL          - on error
*     GT_BAD_PTR       - on NULL pointer
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     None.
*
******************************************************************************/
static GT_STATUS internal_cpssDxChPortPeriodicFlowControlIntervalGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_PORT_PERIODIC_FC_TYPE_ENT portType,
    OUT GT_U32                              *valuePtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      regValue;   /* register value */
    GT_STATUS   rc;         /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
                                          CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(valuePtr);

    switch (portType)
    {
        case CPSS_DXCH_PORT_PERIODIC_FC_TYPE_GIG_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                miscellaneousRegs.lms0PeriodicFlowControlInterval0;
                break;
        case CPSS_DXCH_PORT_PERIODIC_FC_TYPE_XG_E:
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                miscellaneousRegs.lms1PeriodicFlowControlInterval1;
                break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == regAddr)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    rc = prvCpssDrvHwPpReadRegister(devNum, regAddr, &regValue);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* the register value is in 40 ns. */
    *valuePtr = regValue / 25;

    return rc;

}

/******************************************************************************
* cpssDxChPortPeriodicFlowControlIntervalGet
*
* DESCRIPTION:
*       Get Periodic Flow Control interval.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - physical device number
*       portType  - port type
*
* OUTPUTS:
*       valuePtr  - pointer to interval value in microseconds between two
*                   successive Flow Control frames that are sent periodically
*                   by the port.
*                   (APPLICABLE RANGES: 0..171798691)
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad devNum, portType
*     GT_FAIL          - on error
*     GT_BAD_PTR       - on NULL pointer
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     None.
*
******************************************************************************/
GT_STATUS cpssDxChPortPeriodicFlowControlIntervalGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_PORT_PERIODIC_FC_TYPE_ENT portType,
    OUT GT_U32                              *valuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPeriodicFlowControlIntervalGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portType, valuePtr));

    rc = internal_cpssDxChPortPeriodicFlowControlIntervalGet(devNum, portType, valuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portType, valuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/******************************************************************************
* prvCpssDxChPortPeriodicFlowControlIntervalSelectionSet
*
* DESCRIPTION:
*       Set Periodic Flow Control interval selection.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       portType  - interval selection: use interval 0 tuned by default for GE
*                                       or interval 1 tuned by default for XG
*
* OUTPUTS:
*       None.
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad devNum, portNum, portType
*     GT_FAIL          - on error
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     None.
*
******************************************************************************/
GT_STATUS prvCpssDxChPortPeriodicFlowControlIntervalSelectionSet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    IN  CPSS_DXCH_PORT_PERIODIC_FC_TYPE_ENT portType
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      regValue;   /* register value */
    GT_U32      portGroupId;/* core number */
    GT_U32      localPort;  /* port number in local core */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
      CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);

    if((portType != CPSS_DXCH_PORT_PERIODIC_FC_TYPE_GIG_E)
        && (portType != CPSS_DXCH_PORT_PERIODIC_FC_TYPE_XG_E))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regValue = (CPSS_DXCH_PORT_PERIODIC_FC_TYPE_GIG_E == portType) ? 0 : 1;
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum);
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->miscellaneousRegs.
                                            lms1PeriodicFlowControlIntervalSel;
    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                localPort /* fieldOffset */,
                                                1, regValue);
}

/******************************************************************************
* internal_cpssDxChPortPeriodicFlowControlIntervalSelectionSet
*
* DESCRIPTION:
*       Set Periodic Flow Control interval selection.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       portType  - interval selection: use interval 0 tuned by default for GE
*                                       or interval 1 tuned by default for XG
*
* OUTPUTS:
*       None.
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad devNum, portNum, portType
*     GT_FAIL          - on error
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     None.
*
******************************************************************************/
static GT_STATUS internal_cpssDxChPortPeriodicFlowControlIntervalSelectionSet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    IN  CPSS_DXCH_PORT_PERIODIC_FC_TYPE_ENT portType
)
{
    return prvCpssDxChPortPeriodicFlowControlIntervalSelectionSet(devNum,
                                                                    portNum,
                                                                    portType);
}

/******************************************************************************
* cpssDxChPortPeriodicFlowControlIntervalSelectionSet
*
* DESCRIPTION:
*       Set Periodic Flow Control interval selection.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       portType  - interval selection: use interval 0 tuned by default for GE
*                                       or interval 1 tuned by default for XG
*
* OUTPUTS:
*       None.
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad devNum, portNum, portType
*     GT_FAIL          - on error
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     None.
*
******************************************************************************/
GT_STATUS cpssDxChPortPeriodicFlowControlIntervalSelectionSet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    IN  CPSS_DXCH_PORT_PERIODIC_FC_TYPE_ENT portType
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPeriodicFlowControlIntervalSelectionSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, portType));

    rc = internal_cpssDxChPortPeriodicFlowControlIntervalSelectionSet(devNum, portNum, portType);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, portType));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/******************************************************************************
* internal_cpssDxChPortPeriodicFlowControlIntervalSelectionGet
*
* DESCRIPTION:
*       Get Periodic Flow Control interval selection.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       portTypePtr  - interval selection: use interval 0 tuned by default for GE
*                                           or interval 1 tuned by default for XG
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad devNum, portNum, portType
*     GT_FAIL          - on error
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     None.
*
******************************************************************************/
static GT_STATUS internal_cpssDxChPortPeriodicFlowControlIntervalSelectionGet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    IN  CPSS_DXCH_PORT_PERIODIC_FC_TYPE_ENT *portTypePtr
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      regAddr;    /* register address */
    GT_U32      regValue;   /* register value */
    GT_U32      portGroupId;/* core number */
    GT_U32      localPort;  /* port number in local core */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
      CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(portTypePtr);

    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum);
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->miscellaneousRegs.
                                            lms1PeriodicFlowControlIntervalSel;
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                                localPort /* fieldOffset */,
                                                1, &regValue);
    if(rc != GT_OK)
    {
        return rc;
    }

    *portTypePtr = (0 == regValue) ? CPSS_DXCH_PORT_PERIODIC_FC_TYPE_GIG_E :
                                        CPSS_DXCH_PORT_PERIODIC_FC_TYPE_XG_E;
    return GT_OK;
}

/******************************************************************************
* cpssDxChPortPeriodicFlowControlIntervalSelectionGet
*
* DESCRIPTION:
*       Get Periodic Flow Control interval selection.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       portTypePtr  - interval selection: use interval 0 tuned by default for GE
*                                           or interval 1 tuned by default for XG
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad devNum, portNum, portType
*     GT_FAIL          - on error
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     None.
*
******************************************************************************/
GT_STATUS cpssDxChPortPeriodicFlowControlIntervalSelectionGet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    IN  CPSS_DXCH_PORT_PERIODIC_FC_TYPE_ENT *portTypePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPeriodicFlowControlIntervalSelectionGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, portTypePtr));

    rc = internal_cpssDxChPortPeriodicFlowControlIntervalSelectionGet(devNum, portNum, portTypePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, portTypePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortMacTypeGet
*
* DESCRIPTION:
*       Get port MAC type.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*
* OUTPUTS:
*       portMacTypePtr - (pointer to) port MAC type
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortMacTypeGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT CPSS_PORT_MAC_TYPE_ENT  *portMacTypePtr
)
{
    GT_U32                  portMacNum;      /* MAC number */
    PRV_CPSS_PORT_TYPE_ENT  prvPortMacType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(portMacTypePtr);

    prvPortMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum);

    switch(prvPortMacType)
    {
        case PRV_CPSS_PORT_NOT_EXISTS_E:
            *portMacTypePtr = CPSS_PORT_MAC_TYPE_NOT_EXISTS_E;
            break;

        case PRV_CPSS_PORT_FE_E:
            *portMacTypePtr = CPSS_PORT_MAC_TYPE_FE_E;
            break;

        case PRV_CPSS_PORT_GE_E:
            *portMacTypePtr = CPSS_PORT_MAC_TYPE_GE_E;
            break;

        case PRV_CPSS_PORT_XG_E:
            *portMacTypePtr = CPSS_PORT_MAC_TYPE_XG_E;
            break;

        case PRV_CPSS_PORT_XLG_E:
            *portMacTypePtr = CPSS_PORT_MAC_TYPE_XLG_E;
            break;

        case PRV_CPSS_PORT_HGL_E:
            *portMacTypePtr = CPSS_PORT_MAC_TYPE_HGL_E;
            break;

        case PRV_CPSS_PORT_CG_E:
            *portMacTypePtr = CPSS_PORT_MAC_TYPE_CG_E;
            break;

        case PRV_CPSS_PORT_ILKN_E:
            *portMacTypePtr = CPSS_PORT_MAC_TYPE_ILKN_E;
            break;

        case PRV_CPSS_PORT_NOT_APPLICABLE_E:
            *portMacTypePtr = CPSS_PORT_MAC_TYPE_NOT_APPLICABLE_E;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortMacTypeGet
*
* DESCRIPTION:
*       Get port MAC type.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*
* OUTPUTS:
*       portMacTypePtr - (pointer to) port MAC type
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacTypeGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT CPSS_PORT_MAC_TYPE_ENT  *portMacTypePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortMacTypeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, portMacTypePtr));

    rc = internal_cpssDxChPortMacTypeGet(devNum, portNum, portMacTypePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, portMacTypePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/******************************************************************************
* internal_cpssDxChPortPeriodicFlowControlCounterSet
*
* DESCRIPTION:
*       Set Periodic Flow Control interval. The interval in microseconds
*       between two successive Flow Control frames.
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - port number
*       value     - The interval in microseconds between two successive
*                   Flow Control frames that are sent periodically by the port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success.
*       GT_BAD_PARAM     - on bad devNum, portNum
*       GT_OUT_OF_RANGE  - on bad value
*       GT_FAIL          - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
******************************************************************************/
static GT_STATUS internal_cpssDxChPortPeriodicFlowControlCounterSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  value
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      regAddr;    /* register address */
    GT_U32      regValue;   /* register value */
    GT_U32      portMacNum; /* MAC number */
    GT_U32      periodicEnable; /* surrent status of periodic FC per port */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
                                          CPSS_XCAT_E | CPSS_XCAT2_E | CPSS_LION_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);


    /* store <periodic_enable> field */
    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
    {
        regAddr = PRV_DXCH_REG_UNIT_GOP_FCA_MAC(devNum, portMacNum).FCACtrl;
    }
    else
    {
        regAddr = PRV_DXCH_REG1_UNIT_GOP_FCA_MAC(devNum, portMacNum).FCACtrl;
    }
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 11, 1, &periodicEnable);
    if(GT_OK != rc)
    {
        return rc;
    }

    /* disable <periodic_enable> field */
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 11, 1, 0);
    if(GT_OK != rc)
    {
        return rc;
    }

    if(value > (0xFFFFFFFF / PRV_CPSS_PP_MAC(devNum)->coreClock))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    regValue = value * PRV_CPSS_PP_MAC(devNum)->coreClock;

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
    {
        regAddr = PRV_DXCH_REG_UNIT_GOP_FCA_MAC(devNum, portMacNum).periodicCntrLSB;
    }
    else
    {
        regAddr = PRV_DXCH_REG1_UNIT_GOP_FCA_MAC(devNum, portMacNum).periodicCntrLSB;
    }
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 16, U32_GET_FIELD_MAC(regValue, 0, 16));
    if(GT_OK != rc)
    {
        return rc;
    }


    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
    {
        regAddr = PRV_DXCH_REG_UNIT_GOP_FCA_MAC(devNum, portMacNum).periodicCntrMSB;
    }
    else
    {
        regAddr = PRV_DXCH_REG1_UNIT_GOP_FCA_MAC(devNum, portMacNum).periodicCntrMSB;
    }
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 16, U32_GET_FIELD_MAC(regValue, 16, 16));
    if(GT_OK != rc)
    {
        return rc;
    }

    /* restore <periodic_enable> field */
    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
    {
        regAddr = PRV_DXCH_REG_UNIT_GOP_FCA_MAC(devNum, portMacNum).FCACtrl;
    }
    else
    {
        regAddr = PRV_DXCH_REG1_UNIT_GOP_FCA_MAC(devNum, portMacNum).FCACtrl;
    }
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 11, 1, periodicEnable);
    return rc;
}

/******************************************************************************
* cpssDxChPortPeriodicFlowControlCounterSet
*
* DESCRIPTION:
*       Set Periodic Flow Control interval. The interval in microseconds
*       between two successive Flow Control frames.
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - port number
*       value     - The interval in microseconds between two successive
*                   Flow Control frames that are sent periodically by the port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success.
*       GT_BAD_PARAM     - on bad devNum, portNum
*       GT_OUT_OF_RANGE  - on bad value
*       GT_FAIL          - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The interval in micro seconds between transmission of two consecutive
*       Flow Control packets recommended interval is calculated by the following formula:
*                   period (micro seconds) = 33553920 / speed(M)
*       Exception: for 10M, 100M and 10000M Flow Control packets recommended interval is 33500
*       Following are recommended intervals in micro seconds for common port speeds:
*                        33500   for speed 10M
*                        33500   for speed 100M
*                        33500   for speed 1G
*                        13421   for speed 2.5G
*                        6710    for speed 5G
*                        3355    for speed 10G
*                        2843    for speed 11.8G
*                        2796    for speed 12G
*                        2467    for speed 13.6G
*                        2236    for speed 15G
*                        2097    for speed 16G
*                        1677    for speed 20G
*                        838     for speed 40G
*                        710     for speed 47.2G
*                        671     for speed 50G
*                        447     for speed 75G
*                        335     for speed 100G
*                        239     for speed 140G
*
******************************************************************************/
GT_STATUS cpssDxChPortPeriodicFlowControlCounterSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  value
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPeriodicFlowControlCounterSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, value));

    rc = internal_cpssDxChPortPeriodicFlowControlCounterSet(devNum, portNum, value);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, value));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/******************************************************************************
* internal_cpssDxChPortPeriodicFlowControlCounterGet
*
* DESCRIPTION:
*       Get Periodic Flow Control interval. The interval in microseconds
*       between two successive Flow Control frames.
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - port number
*
* OUTPUTS:
*       valuePtr  - (pointer to) The interval in microseconds between two successive
*                   Flow Control frames that are sent periodically by the port.
*
* RETURNS:
*       GT_OK            - on success.
*       GT_BAD_PARAM     - on bad devNum, portNum
*       GT_OUT_OF_RANGE  - on bad value
*       GT_BAD_PTR       - on NULL pointer
*       GT_FAIL          - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
******************************************************************************/
static GT_STATUS internal_cpssDxChPortPeriodicFlowControlCounterGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_U32                  *valuePtr
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      regAddr;    /* register address */
    GT_U32      regValue;   /* register value */
    GT_U32      portMacNum; /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
                                          CPSS_XCAT_E | CPSS_XCAT2_E | CPSS_LION_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(valuePtr);

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
    {
        regAddr = PRV_DXCH_REG_UNIT_GOP_FCA_MAC(devNum, portMacNum).periodicCntrLSB;
    }
    else
    {
        regAddr = PRV_DXCH_REG1_UNIT_GOP_FCA_MAC(devNum, portMacNum).periodicCntrLSB;
    }
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 16, &regValue);
    if(GT_OK != rc)
    {
        return rc;
    }

    *valuePtr = regValue;

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
    {
        regAddr = PRV_DXCH_REG_UNIT_GOP_FCA_MAC(devNum, portMacNum).periodicCntrMSB;
    }
    else
    {
        regAddr = PRV_DXCH_REG1_UNIT_GOP_FCA_MAC(devNum, portMacNum).periodicCntrMSB;
    }
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 16, &regValue);
    if(GT_OK != rc)
    {
        return rc;
    }

    *valuePtr |= (regValue << 16);

    *valuePtr = *valuePtr / PRV_CPSS_PP_MAC(devNum)->coreClock;

    return GT_OK;
}

/******************************************************************************
* cpssDxChPortPeriodicFlowControlCounterGet
*
* DESCRIPTION:
*       Get Periodic Flow Control interval. The interval in microseconds
*       between two successive Flow Control frames.
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - port number
*
* OUTPUTS:
*       valuePtr  - (pointer to) The interval in microseconds between two successive
*                   Flow Control frames that are sent periodically by the port.
*
* RETURNS:
*       GT_OK            - on success.
*       GT_BAD_PARAM     - on bad devNum, portNum
*       GT_OUT_OF_RANGE  - on bad value
*       GT_BAD_PTR       - on NULL pointer
*       GT_FAIL          - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
******************************************************************************/
GT_STATUS cpssDxChPortPeriodicFlowControlCounterGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_U32                  *valuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPeriodicFlowControlCounterGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, valuePtr));

    rc = internal_cpssDxChPortPeriodicFlowControlCounterGet(devNum, portNum, valuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, valuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/******************************************************************************
* prvHwsUnitsIdToCpssUnitsId
*
* DESCRIPTION:
*       Conver from Hws Units Id Enum to Cpss Units Id Enum.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        Lion; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Caelum; Bobcat3; Bobcat2.
*
* INPUTS:
*       hwUnitId        - Hws Units Id
*
* OUTPUTS:
*       unitId          - Cpss Units Id
*
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad hwUnitId
*
* COMMENTS:
*     Debug function.
*
******************************************************************************/
static GT_STATUS prvHwsUnitsIdToCpssUnitsId
(
    IN  MV_HWS_UNITS_ID             hwUnitId,
    OUT CPSS_DXCH_PORT_UNITS_ID_ENT *unitId
)
{
    switch (hwUnitId)
    {
        case GEMAC_UNIT:
            (*unitId) = CPSS_DXCH_PORT_UNITS_ID_GEMAC_UNIT_E;
            break;
        case XLGMAC_UNIT:
            (*unitId) = CPSS_DXCH_PORT_UNITS_ID_XLGMAC_UNIT_E;
            break;
        case HGLMAC_UNIT:
            (*unitId) = CPSS_DXCH_PORT_UNITS_ID_HGLMAC_UNIT_E;
            break;
        case XPCS_UNIT:
            (*unitId) = CPSS_DXCH_PORT_UNITS_ID_XPCS_UNIT_E;
            break;
        case MMPCS_UNIT:
            (*unitId) = CPSS_DXCH_PORT_UNITS_ID_MMPCS_UNIT_E;
            break;
        case CG_UNIT:
            (*unitId) = CPSS_DXCH_PORT_UNITS_ID_CG_UNIT_E;
            break;
        case INTLKN_UNIT:
            (*unitId) = CPSS_DXCH_PORT_UNITS_ID_INTLKN_UNIT_E;
            break;
        case INTLKN_RF_UNIT:
            (*unitId) = CPSS_DXCH_PORT_UNITS_ID_INTLKN_RF_UNIT_E;
            break;
        case SERDES_UNIT:
            (*unitId) = CPSS_DXCH_PORT_UNITS_ID_SERDES_UNIT_E;
            break;
        case SERDES_PHY_UNIT:
            (*unitId) = CPSS_DXCH_PORT_UNITS_ID_SERDES_PHY_UNIT_E;
            break;
        case ETI_UNIT:
            (*unitId) = CPSS_DXCH_PORT_UNITS_ID_ETI_UNIT_E;
            break;
        case ETI_ILKN_RF_UNIT:
            (*unitId) = CPSS_DXCH_PORT_UNITS_ID_ETI_ILKN_RF_UNIT_E;
            break;
        case D_UNIT:
            (*unitId) = CPSS_DXCH_PORT_UNITS_ID_D_UNIT_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    return GT_OK;
}


/******************************************************************************
* prvCpssUnitsIdToHwsUnitsId
*
* DESCRIPTION:
*       Conver from Cpss Units Id Enum to Hws Units Id Enum.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        Lion; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Caelum; Bobcat3; Bobcat2.
*
* INPUTS:
*       unitId          - Cpss Units Id
*
* OUTPUTS:
*       hwUnitId        - Hws Units Id
*
*
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad unitId
*
* COMMENTS:
*     Debug function.
*
******************************************************************************/
static GT_STATUS prvCpssUnitsIdToHwsUnitsId
(
    IN   CPSS_DXCH_PORT_UNITS_ID_ENT unitId,
    OUT  MV_HWS_UNITS_ID             *hwUnitId
)
{
    switch (unitId)
    {
        case CPSS_DXCH_PORT_UNITS_ID_GEMAC_UNIT_E:
            (*hwUnitId) = GEMAC_UNIT;
            break;
        case CPSS_DXCH_PORT_UNITS_ID_XLGMAC_UNIT_E:
            (*hwUnitId) = XLGMAC_UNIT;
            break;
        case CPSS_DXCH_PORT_UNITS_ID_HGLMAC_UNIT_E:
            (*hwUnitId) = HGLMAC_UNIT;
            break;
        case CPSS_DXCH_PORT_UNITS_ID_XPCS_UNIT_E:
            (*hwUnitId) = XPCS_UNIT;
            break;
        case CPSS_DXCH_PORT_UNITS_ID_MMPCS_UNIT_E:
            (*hwUnitId) = MMPCS_UNIT;
            break;
        case CPSS_DXCH_PORT_UNITS_ID_CG_UNIT_E:
            (*hwUnitId) = CG_UNIT;
            break;
        case CPSS_DXCH_PORT_UNITS_ID_INTLKN_UNIT_E:
            (*hwUnitId) = INTLKN_UNIT;
            break;
        case CPSS_DXCH_PORT_UNITS_ID_INTLKN_RF_UNIT_E:
            (*hwUnitId) = INTLKN_RF_UNIT;
            break;
        case CPSS_DXCH_PORT_UNITS_ID_SERDES_UNIT_E:
            (*hwUnitId) = SERDES_UNIT;
            break;
        case CPSS_DXCH_PORT_UNITS_ID_SERDES_PHY_UNIT_E:
            (*hwUnitId) = SERDES_PHY_UNIT;
            break;
        case CPSS_DXCH_PORT_UNITS_ID_ETI_UNIT_E:
            (*hwUnitId) = ETI_UNIT;
            break;
        case CPSS_DXCH_PORT_UNITS_ID_ETI_ILKN_RF_UNIT_E:
            (*hwUnitId) = ETI_ILKN_RF_UNIT;
            break;
        case CPSS_DXCH_PORT_UNITS_ID_D_UNIT_E:
            (*hwUnitId) = D_UNIT;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    return GT_OK;
}

/******************************************************************************
* internal_cpssDxChPortUnitInfoGetByAddr
*
* DESCRIPTION:
*       Get unit ID by unit address in device
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        Lion; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Caelum; Bobcat3; Bobcat2.
*
* INPUTS:
*       devNum     - physical device number
*       baseAddr   - unit base address in device
*
* OUTPUTS:
*       unitId     - unit ID (MAC, PCS, SERDES)
*
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad devNum, portNum, portType
*     GT_FAIL          - on error
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     Debug function.
*
******************************************************************************/
static GT_STATUS internal_cpssDxChPortUnitInfoGetByAddr
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      baseAddr,
    OUT CPSS_DXCH_PORT_UNITS_ID_ENT *unitId
)
{
    MV_HWS_UNITS_ID     hwUnitId;
    GT_STATUS           rc = GT_OK;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
                                            CPSS_CH1_E      | CPSS_CH1_DIAMOND_E    | CPSS_CH2_E    |  CPSS_CH3_E      |
                                            CPSS_XCAT2_E    | CPSS_BOBCAT2_E        | CPSS_XCAT3_E  |  CPSS_XCAT_E     | CPSS_LION_E  );
    CPSS_NULL_PTR_CHECK_MAC(unitId);

    CPSS_LOG_INFORMATION_MAC("Calling: mvUnitInfoGetByAddr(devNum[%d], baseAddr[%d], *hwUnitId)", devNum, baseAddr);
    mvUnitInfoGetByAddr(devNum, baseAddr, &hwUnitId);

    rc = prvHwsUnitsIdToCpssUnitsId(hwUnitId, unitId);
    if(rc != GT_OK)
    {
        CPSS_LOG_INFORMATION_MAC("Hws return code is %d", rc);
    }
    return rc;

}

/******************************************************************************
* cpssDxChPortUnitInfoGetByAddr
*
* DESCRIPTION:
*       Get unit ID by unit address in device
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        Lion; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Caelum; Bobcat3; Bobcat2.
*
* INPUTS:
*       devNum         - physical device number
*       baseAddr       - unit base address in device
*
* OUTPUTS:
*       unitIdPtr      - unit ID (MAC, PCS, SERDES)
*
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad devNum, portNum, portType
*     GT_FAIL          - on error
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     Debug function.
*
******************************************************************************/
GT_STATUS cpssDxChPortUnitInfoGetByAddr
(
    IN  GT_U8                        devNum,
    IN  GT_U32                       baseAddr,
    OUT CPSS_DXCH_PORT_UNITS_ID_ENT  *unitIdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortUnitInfoGetByAddr);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, baseAddr, unitIdPtr));

    rc = internal_cpssDxChPortUnitInfoGetByAddr(devNum, baseAddr, unitIdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, baseAddr, unitIdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/******************************************************************************
* internal_cpssDxChPortUnitInfoGet
*
* DESCRIPTION:
*       Return silicon specific base address and index for specified unit
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        Lion; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Caelum; Bobcat3; Bobcat2.
*
* INPUTS:
*       devNum    - Device Number
*       unitId    - unit ID (MAC, PCS, SERDES)
*
* OUTPUTS:
*       baseAddr  - unit base address in device
*       unitIndex - unit index in device
*
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad devNum, portNum, portType
*     GT_FAIL          - on error
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     Debug function.
*
******************************************************************************/
static GT_STATUS internal_cpssDxChPortUnitInfoGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_DXCH_PORT_UNITS_ID_ENT unitId,
    OUT GT_U32                      *baseAddr,
    OUT GT_U32                      *unitIndex
)
{
    MV_HWS_UNITS_ID     hwUnitId;
    GT_STATUS           rc = GT_OK;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
                                            CPSS_CH1_E      | CPSS_CH1_DIAMOND_E    | CPSS_CH2_E    |  CPSS_CH3_E      |
                                            CPSS_XCAT2_E    | CPSS_BOBCAT2_E        | CPSS_XCAT3_E  |  CPSS_XCAT_E     | CPSS_LION_E  );
    CPSS_NULL_PTR_CHECK_MAC(baseAddr);
    CPSS_NULL_PTR_CHECK_MAC(unitIndex);

    rc = prvCpssUnitsIdToHwsUnitsId(unitId, &hwUnitId);
    if(rc != GT_OK)
    {
        return rc;
    }
    CPSS_LOG_INFORMATION_MAC("Calling: mvUnitInfoGet(devNum[%d], unitId[%d], *baseAddr, *unitIndex)", devNum, hwUnitId);
    mvUnitInfoGet(devNum, hwUnitId, baseAddr, unitIndex);

    return rc;
}

/******************************************************************************
* cpssDxChPortUnitInfoGet
*
* DESCRIPTION:
*       Return silicon specific base address and index for specified unit
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        Lion; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Caelum; Bobcat3; Bobcat2.
*
* INPUTS:
*       devNum         - Device Number
*       unitId         - unit ID (MAC, PCS, SERDES)
*
* OUTPUTS:
*       baseAddrPtr    - unit base address in device
*       unitIndexPtr   - unit index in device
*
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad devNum, portNum, portType
*     GT_FAIL          - on error
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     Debug function.
*
******************************************************************************/
GT_STATUS cpssDxChPortUnitInfoGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_DXCH_PORT_UNITS_ID_ENT unitId,
    OUT GT_U32                      *baseAddrPtr,
    OUT GT_U32                      *unitIndexPtr
)
{
    GT_STATUS rc = GT_OK;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortUnitInfoGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, unitId, baseAddrPtr, unitIndexPtr));

    rc = internal_cpssDxChPortUnitInfoGet(devNum, unitId, baseAddrPtr, unitIndexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, unitId, baseAddrPtr, unitIndexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


