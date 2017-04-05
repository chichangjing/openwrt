/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* prvCpssDxChNetIfMii.c
*
* DESCRIPTION:
*       Include DxCh MII/CPU Ethernet port network interface management functions.
*
*
* FILE REVISION NUMBER:
*       $Revision: 25 $
*
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
#include <cpss/dxCh/dxChxGen/networkIf/private/prvCpssDxChNetIf.h>
#include <cpss/generic/networkIf/cpssGenNetIfMii.h>
#include <cpss/generic/networkIf/private/prvCpssGenNetworkIfMii.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/cos/private/prvCpssDxChCoS.h> /* for queue number validity check */

#if 0
/*******************************************************************************
* prvCpssDxChNetIfXcat3MiiConfig
*
* DESCRIPTION:
*       Configure the CPU port in SGMII mode.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChNetIfXcat3MiiConfig
(
    IN GT_U8 devNum
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      regAddr;    /* register address */

    /* 1. GUNIT[0] and MAC[31] reset*/
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl2;
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 6, 1, 1);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDrvPpHwInternalPciRegWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, 0x72c08, 0xc048);
    if (rc != GT_OK)
    {
        return rc;
    }


    /* 2. Set DFX to select CPU connection to MSYS[0] */

    /* DFX configuration is relevant only for internal CPU */
    /* If external CPU is used it must remain as default value -> '0',
       + SERDES[10] configuration is needed */
    CPSS_TBD_BOOKMARK_XCAT3

    regAddr = PRV_CPSS_DXCH_DEV_RESET_AND_INIT_CONTROLLER_REGS_MAC(devNum)->DFXServerUnitsBC2SpecificRegs.deviceCtrl14;
    rc = prvCpssDrvHwPpResetAndInitControllerSetRegField(devNum, regAddr, 25, 1, 1);
    if (rc != GT_OK)
    {
        return rc;
    }


    /* 3. Set MSM2 select MAC[31] vs SDMA */

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.cpuPortCtrlReg;
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 1, 1, 0);
    if (rc != GT_OK)
    {
        return rc;
    }


    /* 4. Set XLG_MAC[31] to SGMII mode */
    rc = prvCpssHwPpSetRegField(devNum, 0x120DF01C, 13, 3, 0);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* MG GLobal configuration_1 - bit{11} - disable MG_TO_TXQ_PRIO_MASK */
    rc = prvCpssHwPpSetRegField(devNum, 0x110, 11, 1, 1);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Disable Sampling on Negedge */
    rc = cpssDrvPpHwInternalPciRegWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, 0x72c14, 0x8d8); /* Serial COnfiguration bit{5} */
    if (rc != GT_OK)
    {
        return rc;
    }

    /* 5. Set GUNIT[0] to 100Base-x, Full Duplex, No Auto Negotiation */
    rc = cpssDrvPpHwInternalPciRegWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, 0x72c00, 0x8be7); /* port enable, 1000-x */
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDrvPpHwInternalPciRegWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, 0x72c0c, 0x9068); /* no AN, FD, AN bypass */
    if (rc != GT_OK)
    {
        return rc;
    }

    /* 6. Set MAC[31] to 100Base-x, Full Duplex, No Auto Negotiation */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl;
    rc = prvCpssHwPpWriteRegister(devNum, regAddr, 0x8be7); /* port enable, 1000-x */
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.autoNegCtrl;
    rc = prvCpssHwPpWriteRegister(devNum, regAddr, 0x9068); /* no AN, FD, AN bypass, GMII speed 1G */
    if (rc != GT_OK)
    {
        return rc;
    }


    rc = cpssDrvPpHwInternalPciRegWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, 0x72c08, 0xc008); /* MAC unreset */
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl2;
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 6, 1, 0);  /* MAC unreset */
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}
#endif


/*******************************************************************************
* prvCpssDxChNetIfXcat3MiiConfig
*
* DESCRIPTION:
*       Configure the CPU port in SGMII mode.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChNetIfXcat3MiiConfig
(
    IN GT_U8 devNum
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      regAddr;    /* register address */

    /* 1. GUNIT[0] and MAC[31] reset*/
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl2;
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 6, 1, 1);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* 2. Set DFX to select CPU connection to MSYS[0] */

    /* DFX configuration is relevant only for internal CPU */
    /* If external CPU is used it must remain as default value -> '0',
       + SERDES[10] configuration is needed */
    CPSS_TBD_BOOKMARK_XCAT3

    regAddr = PRV_CPSS_DXCH_DEV_RESET_AND_INIT_CONTROLLER_REGS_MAC(devNum)->DFXServerUnitsBC2SpecificRegs.deviceCtrl14;
    rc = prvCpssDrvHwPpResetAndInitControllerSetRegField(devNum, regAddr, 25, 1, 1);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* 3. Set MSM2 select MAC[31] vs SDMA */

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.cpuPortCtrlReg;
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 1, 1, 0);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* 4. Set XLG_MAC[31] to SGMII mode */
    rc = prvCpssHwPpSetRegField(devNum, 0x120DF01C, 13, 3, 0);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* MG GLobal configuration_1 - bit{11} - disable MG_TO_TXQ_PRIO_MASK */
    rc = prvCpssHwPpSetRegField(devNum, 0x110, 11, 1, 1);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* 6. Set MAC[31] to 100Base-x, Full Duplex, No Auto Negotiation */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl;
    rc = prvCpssHwPpWriteRegister(devNum, regAddr, 0x8be7); /* port enable, 1000-x */
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.autoNegCtrl;
    rc = prvCpssHwPpWriteRegister(devNum, regAddr, 0x9068); /* no AN, FD, AN bypass, GMII speed 1G */
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl2;
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 6, 1, 0);  /* MAC unreset */
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChNetIfXcat3MsysMiiConfig
*
* DESCRIPTION:
*       Configure the MSYS CPU port.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChNetIfXcat3MsysMiiConfig
(
    IN GT_U8 devNum
)
{
    GT_STATUS   rc;         /* return code */

    /* 1. GUNIT[0] and MAC[31] reset*/
    rc = cpssDrvPpHwInternalPciRegWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, 0x72c08, 0xc048);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Disable Sampling on Negedge */
    rc = cpssDrvPpHwInternalPciRegWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, 0x72c14, 0x8d8); /* Serial COnfiguration bit{5} */
    if (rc != GT_OK)
    {
        return rc;
    }

    /* 5. Set GUNIT[0] to 100Base-x, Full Duplex, No Auto Negotiation */
    rc = cpssDrvPpHwInternalPciRegWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, 0x72c00, 0x8be7); /* port enable, 1000-x */
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDrvPpHwInternalPciRegWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, 0x72c0c, 0x9068); /* no AN, FD, AN bypass */
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDrvPpHwInternalPciRegWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, 0x72c08, 0xc008); /* MAC unreset */
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChCpuPortMiiConfig
*
* DESCRIPTION:
*       Configure the CPU port in MII/RGMII mode.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChCpuPortMiiConfig
(
    IN GT_U8 devNum
)
{
    GT_STATUS   retVal;     /* return code */
    GT_U32      regAddr;    /* register address */
    GT_U32      regOffset;  /* offset in register */
    GT_U32      regData;    /* register data */

    /* use CPU Ethernet port/MII for CPU packets interface and PowerSave = 0*/
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.globalControl;
    retVal = prvCpssHwPpSetRegField(devNum,regAddr,19,2,0);
    if (retVal != GT_OK)
        return retVal;



    if(PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E)
    {
        /* Set CPUPortActive = 1 */
        regOffset = 0;
        regData = 1;
    }
    else /* CPSS_PP_FAMILY_DXCH_XCAT3_E */
    {
        /* Set to work in CPU Port mode */
        regOffset = 1;
        regData = 0;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.cpuPortCtrlReg;
    retVal = prvCpssHwPpSetRegField(devNum,regAddr,regOffset,1,regData);
    if (retVal != GT_OK)
        return retVal;

    /* additional configuration for XCAT */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E ||
        PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E ||
        PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {

        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
        {
            /* Set CPUPortIFType = 1*/
            retVal = prvCpssHwPpSetRegField(devNum,regAddr,1,2,1);
            if (retVal != GT_OK)
                return retVal;
        }
        else
        {
            /* Set CPUPortIFType = 2*/
            retVal = prvCpssHwPpSetRegField(devNum,regAddr,1,2,2);
            if (retVal != GT_OK)
                return retVal;
        }


        /* Set R0_Active = 0, RGPP_TEST = 0, GPP_Active = 1 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.sampledAtResetReg;
        retVal = prvCpssHwPpSetRegField(devNum,regAddr,18,3,1);
        if (retVal != GT_OK)
            return retVal;

        /* Set ref_clk_125_sel  to PLL */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl;
        retVal = prvCpssHwPpSetRegField(devNum,regAddr,10,1,1);
        if (retVal != GT_OK)
            return retVal;

        /* UseIntClkforEn = 1, CollisionOnBackPressureCntEn=1 */
        /* I'm prevented here from using macro, because it causes ARM5281_WB_DIAB
            "warning (dcc:1522): statement not reached" */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.
                    macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl2;
        retVal = prvCpssHwPpWriteRegBitMask(devNum,regAddr, 0x4010,0x4010);
        if (retVal != GT_OK)
            return retVal;

        /* BufferSize = 0 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.
                    macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl3;
        retVal = prvCpssHwPpSetRegField(devNum,regAddr,0,6,0);
        if (retVal != GT_OK)
            return retVal;

    }

    /* unset PcsEn */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.
                macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl2;
    retVal = prvCpssHwPpSetRegField(devNum, regAddr, 3, 1, 0);
    if (retVal != GT_OK)
        return retVal;

    /* unset PortMACReset bit ---
     without this Rx/Tx will not work to/From CPU */
    retVal = prvCpssHwPpSetRegField(devNum, regAddr, 6, 1, 0);
    if (retVal != GT_OK)
        return retVal;

    /* Port<n> MAC Control Register1 (0<=n<24, CPUPort = 63)
        unset RxCRCCheckEn
        if CPU want to send bad CRC -- let it */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.
                macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl1;
    retVal = prvCpssHwPpSetRegField(devNum, regAddr, 0, 1, 0);
    if (retVal != GT_OK)
        return retVal;
    if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT_E &&
        PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E &&
        PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        /* Port<n> Auto-Negotiation Configuration Register (0<=n<24,
         CPUPort = 63) */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.autoNegCtrl;
        /* set ForceLinkDown */
        /* unset ForceLinkUp */
        retVal = prvCpssHwPpSetRegField(devNum,regAddr,0,2,1);
        if (retVal != GT_OK)
            return retVal;

        /* unset SetGMIISpeed --- need to be done when port is down */
        retVal = prvCpssHwPpSetRegField(devNum,regAddr,6,1,0);
        if (retVal != GT_OK)
            return retVal;

        /* unset AnDuplexEn --- need to be done when port is down */
        retVal = prvCpssHwPpSetRegField(devNum,regAddr,13,1,0);
        if (retVal != GT_OK)
            return retVal;

        /* unset ForceLinkDown */
        /* set ForceLinkUp */
        retVal = prvCpssHwPpSetRegField(devNum,regAddr,0,2,2);
        if (retVal != GT_OK)
            return retVal;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChNetIfLion2MiiConfig
*
* DESCRIPTION:
*       Configure CPU port for RGMII support on Lion2
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChNetIfLion2MiiConfig
(
    IN GT_U8   devNum
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      regAddr;    /* register address */
    GT_U32      cpuPortGroupId;  /* core number where CPU port resides */
    struct  dxch_perPortRegs *cpuPortRegsPtr;
    GT_U32      tmpValue;/*value to write to register*/

    /* use CPU Ethernet port/MII for CPU packets interface, configure on all cores */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.globalControl;
    rc = prvCpssHwPpSetRegField(devNum,regAddr,20,1,0);
    if (rc != GT_OK)
        return rc;

    /* Set CPUPortActive = 1, configure on all cores */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.cpuPortCtrlReg;
    rc = prvCpssHwPpSetRegField(devNum,regAddr,0,1,1);
    if (rc != GT_OK)
        return rc;

    cpuPortGroupId = PRV_CPSS_CPU_PORT_PORT_GROUP_ID_MAC(devNum);

    cpuPortRegsPtr = &(PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs);

    /* EarlyRejectOn BufFullEn disabled, Pcs disabled, Mac reset set */
    regAddr = cpuPortRegsPtr->macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl2;
    rc = prvCpssHwPpPortGroupWriteRegister(devNum, cpuPortGroupId, regAddr, 0x4040);
    if(rc != GT_OK)
        return rc;

    /* ensure HW default in register: MIBCntEn=1,FrameSizeLimit=1522,
       PortType=SGMII,port enabled */
    regAddr = cpuPortRegsPtr->macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl;
    rc = prvCpssHwPpPortGroupWriteRegister(devNum, cpuPortGroupId, regAddr, 0x8be5);
    if(rc != GT_OK)
        return rc;

    /* Mac un-reset set */
    regAddr = cpuPortRegsPtr->macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl2;
    rc = prvCpssHwPpPortGroupSetRegField(devNum, cpuPortGroupId, regAddr, 6, 1, 0);
    if(rc != GT_OK)
        return rc;

    /* unset RxCRCCheckEn if CPU want to send bad CRC -- let it */
    rc = prvCpssHwPpPortGroupSetRegField(devNum, cpuPortGroupId, regAddr, 0, 1, 0);
    if(rc != GT_OK)
        return rc;

    /* set ForceLinkDown, unset ForceLinkUp */
    regAddr = cpuPortRegsPtr->autoNegCtrl;
    rc = prvCpssHwPpPortGroupSetRegField(devNum, cpuPortGroupId, regAddr, 0, 2, 1);
    if(rc != GT_OK)
        return rc;

    /* unset AnDuplexEn --- need to be done when port is down */
    rc = prvCpssHwPpPortGroupSetRegField(devNum, cpuPortGroupId, regAddr, 13, 1, 0);
    if(rc != GT_OK)
        return rc;

    /* unset ForceLinkDown, set ForceLinkUp */
    rc = prvCpssHwPpPortGroupSetRegField(devNum, cpuPortGroupId, regAddr, 0, 2, 2);
    if(rc != GT_OK)
        return rc;

    tmpValue = cpuPortGroupId << 1;

    /* configure TXQ_EGR */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            /* Not used any more */
            return GT_OK;
        }
        else
        {
            /* bit 0 - <CpuPortMode> set to 0 'global mode' */
            /* bits 1..4 -  <CpuTargetCore> set to the 'SDMA_PORT_GROUP_ID'  */
            rc = prvCpssHwPpSetRegField(devNum,
                    PRV_DXCH_REG1_UNIT_EGF_EFT_MAC(devNum).global.cpuPortDist,
                    0,5,
                    tmpValue);
        }
    }
    else
    {
        /* bit 0 - <CpuPortMode> set to 0 'global mode' */
        /* bits 1..2 -  <CpuTargetCore> set to the 'SDMA_PORT_GROUP_ID'  */
        rc = prvCpssHwPpSetRegField(devNum,
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.egr.global.cpuPortDistribution,0,3,
                tmpValue);
        /* not needed ... the  prvCpssHwPpSetRegField already duplicated the write !!!
        regAddr += 0x800000;
        rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 3, (1<<cpuPortGroupId));
        */
    }

    return rc;
}

/*******************************************************************************
* prvCpssDxChNetIfMiiInitNetIfDev
*
* DESCRIPTION:
*       Initialize the network interface structures, Rx buffers and Tx header
*       buffers (For a single device).
*
* INPUTS:
*       devNum      - device number
*       miiInitPtr  - MII Init parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChNetIfMiiInitNetIfDev
(
    IN GT_U8 devNum,
    IN CPSS_DXCH_NETIF_MII_INIT_STC *miiInitPtr
)
{
    GT_STATUS rc;

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_TRUE)
    {
        /* the device hold NO dedicated MAC as 'CPU port' so no awareness by the device */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }


    if ((CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily) ||
        (PRV_CPSS_SIP_5_CHECK_MAC(devNum)))
    {/* Configure the CPU port in MII/RGMII mode */
        rc = prvCpssDxChNetIfLion2MiiConfig(devNum);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    rc = prvCpssGenNetIfMiiInitNetIfDev(devNum,miiInitPtr->numOfTxDesc,miiInitPtr->txInternalBufBlockPtr,
                                        miiInitPtr->txInternalBufBlockSize,miiInitPtr->bufferPercentage,
                                        miiInitPtr->rxBufSize,miiInitPtr->headerOffset,
                                        miiInitPtr->rxBufBlockPtr,miiInitPtr->rxBufBlockSize);
    if (rc != GT_OK)
    {
        return rc;
    }

    if ((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION2_E) &&
        (!PRV_CPSS_SIP_5_CHECK_MAC(devNum)))
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
        {
            rc = prvCpssDxChNetIfXcat3MsysMiiConfig(devNum);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* Configure the CPU port in SGMII mode */
            rc = prvCpssDxChNetIfXcat3MiiConfig(devNum);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        else
        {
            rc = prvCpssDxChCpuPortMiiConfig(devNum);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChNetIfMiiPacketTx
*
* DESCRIPTION:
*       This function receives packet buffers & parameters from Application .
*       Prepares them for the transmit operation, and transmits a packet
*       through the ethernet port.
*
* INPUTS:
*       devNum                  - dev num
*       pcktParamsPtr           - (pointer to)The internal packet params to be set into the packet
*                                   descriptors.
*       packetBuffsArrPtr       - (pointer to)The packet data buffers list.
*       buffLenArr              - A list of the buffers len in packetBuffsArrPtr.
*       numOfBufs               - Length of packetBuffsArrPtr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_NO_RESOURCE if there is not enough desc. for enqueuing the packet, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChNetIfMiiPacketTx
(
    IN GT_U8                            devNum,
    IN CPSS_DXCH_NET_TX_PARAMS_STC      *pcktParamsPtr,
    IN GT_U8                            *packetBuffsArrPtr[],
    IN GT_U32                           buffLenArr[],
    IN GT_U32                           numOfBufs
)
{
    GT_STATUS       rc;
    GT_U8         * hdr_PTR;                /* TX header buff from pool     */
    GT_U16          buff;                   /* buffer index                 */
    GT_U8           segment;                /* the segment index            */
    GT_U8         * segmentList[PRV_CPSS_GEN_NETIF_MAX_GT_BUF_SEG_NUM_CNS]; /* segment ptr list    */
    GT_U32          segmentLen[PRV_CPSS_GEN_NETIF_MAX_GT_BUF_SEG_NUM_CNS]; /* segment length list   */
    GT_U8           bufferPcktOffset;       /* second segment offset        */
    GT_U32          ii;
    GT_U32          dsaSize;
    PRV_CPSS_NETIF_MII_TX_CTRL_STC *txCtrlPtr;
#if __WORDSIZE == 64
    typedef struct {
        GT_UINTPTR  cookie;
        GT_UINTPTR  evReqHndl;
    } COOKIE_HANDLER_STC;
    COOKIE_HANDLER_STC *cookie_handler;
#else /* !(__WORDSIZE == 64) */
    GT_U32          cookie;                 /* the user cookie              */
    GT_U32          hndl;                   /* handle got from gtEventBind  */
#endif

    CPSS_NULL_PTR_CHECK_MAC(pcktParamsPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetBuffsArrPtr);
    CPSS_NULL_PTR_CHECK_MAC(buffLenArr);

    /* validate minimum packet length */
    if (0 == numOfBufs || PRV_CPSS_GEN_NETIF_MII_BUFF_MIN_SIZE_CNS > buffLenArr[0])
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    txCtrlPtr = prvCpssGenNetIfMiiTxCtrlGet();

    if (NULL == txCtrlPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    /* check Event Handler in the Asynch Tx mode */
    if ((pcktParamsPtr->sdmaInfo.evReqHndl == 0) &&
        (txCtrlPtr->txMode == PRV_CPSS_GEN_NETIF_MII_TX_MODE_ASYNCH_E))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    hdr_PTR = (GT_U8 *)prvCpssGenNetIfMiiPoolGetBuf(PRV_CPSS_GEN_NETIF_MII_TX_POOL_E);
    if (NULL == hdr_PTR)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    }

    /* copy the packet MAC addresses */
    cpssOsMemCpy(&hdr_PTR[PRV_CPSS_GEN_NETIF_MII_HDR_MAC_OFFSET_CNS],
             packetBuffsArrPtr[0],
             PRV_CPSS_GEN_NETIF_MII_HDR_MAC_SIZE_CNS);

    if (pcktParamsPtr->packetIsTagged == GT_TRUE)
    {
        bufferPcktOffset    = PRV_CPSS_GEN_NETIF_BUFF_PCKT_TAGGED_OFFSET_CNS;
    }
    else
    {
        bufferPcktOffset    = PRV_CPSS_GEN_NETIF_BUFF_PCKT_UNTAGGED_OFFSET_CNS;
    }

    switch(pcktParamsPtr->dsaParam.commonParams.dsaTagType)
    {
        case CPSS_DXCH_NET_DSA_1_WORD_TYPE_ENT:
            dsaSize = 4;
            break;
        case CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT:
            dsaSize = 8;
            break;
        case CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT:
            dsaSize = 16;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* set first segment to point to buffer header from pool */
    segmentList[0]  = (GT_U8*)&hdr_PTR[PRV_CPSS_GEN_NETIF_MII_HDR_MAC_OFFSET_CNS];
    segmentLen[0]   = PRV_CPSS_GEN_NETIF_MII_HDR_MAC_SIZE_CNS + dsaSize/* bytes of regular/extended DSA*/;

    /* set the second segment to point to the rest of the buffer */
    segmentList[1]  = &packetBuffsArrPtr[0][bufferPcktOffset];
    segmentLen[1]   = buffLenArr[0] - bufferPcktOffset;

    /* copy the rest of the pointers and length to segment list */
    for (segment = 2, buff = 1; buff < numOfBufs; buff++, segment++)
    {
        segmentList[segment] = &packetBuffsArrPtr[buff][0];
        segmentLen[segment]  = buffLenArr[buff];
    }

    /* reduce the packet length by 4 because CPU MII/RGMII interface
       adds the CRC to the packet        */
    segmentLen[numOfBufs] -= 4;

    /* build DSA Tag buffer */
    rc = cpssDxChNetIfDsaTagBuild(devNum,
            &pcktParamsPtr->dsaParam,
            &hdr_PTR[PRV_CPSS_GEN_NETIF_MII_HDR_MRVLTAG_OFFSET_CNS]);

    if(rc != GT_OK)
    {
        (void)prvCpssGenNetIfMiiPoolFreeBuf(PRV_CPSS_GEN_NETIF_MII_TX_POOL_E, hdr_PTR);
        return rc;
    }

    /* set the device number in the header buffer */
    ii = PRV_CPSS_GEN_NETIF_MII_HDR_DEVICE_OFFSET_CNS;
    hdr_PTR[ii] = devNum; /* one byte */

    /* set the queue index in the header buffer */
    ii = PRV_CPSS_GEN_NETIF_MII_HDR_QUEUE_OFFSET_CNS; /* one byte */
    if (pcktParamsPtr->sdmaInfo.txQueue >= BIT_3)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    hdr_PTR[ii] = pcktParamsPtr->sdmaInfo.txQueue;

#if __WORDSIZE == 64
    CPSS_TBD_BOOKMARK
    /* Redesign: pointer to structure is stored instead of cookie + evReqHndl */
    /* Allocate memory for cookie and handler (16 bytes),
     * store pointer (8bytes) in
     * PRV_CPSS_GEN_NETIF_MII_HDR_COOKIE, PRV_CPSS_GEN_NETIF_MII_HDR_HNDL
     */
    cookie_handler = (COOKIE_HANDLER_STC*)cpssOsMalloc(sizeof(*cookie_handler));
    if (!cookie_handler)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }
    cookie_handler->cookie = (GT_UINTPTR)pcktParamsPtr->cookie;
    cookie_handler->evReqHndl = (GT_UINTPTR)pcktParamsPtr->sdmaInfo.evReqHndl;
    cpssOsMemCpy(hdr_PTR+PRV_CPSS_GEN_NETIF_MII_HDR_COOKIE_OFFSET_CNS,
        &cookie_handler, sizeof(cookie_handler));
#else /* !(__WORDSIZE == 64) */
    ii = PRV_CPSS_GEN_NETIF_MII_HDR_COOKIE_OFFSET_CNS;
    /* set the control cookie in the header buffer */
    cookie = (GT_U32)pcktParamsPtr->cookie;
    hdr_PTR[ii++] = (GT_U8)(cookie >> 24);
    hdr_PTR[ii++] = (GT_U8)(cookie >> 16);
    hdr_PTR[ii++] = (GT_U8)(cookie >> 8);
    hdr_PTR[ii++] = (GT_U8)(cookie);

    ii = PRV_CPSS_GEN_NETIF_MII_HDR_HNDL_OFFSET_CNS;
    hndl = pcktParamsPtr->sdmaInfo.evReqHndl;
    hdr_PTR[ii++] = (GT_U8)(hndl >> 24);
    hdr_PTR[ii++] = (GT_U8)(hndl >> 16);
    hdr_PTR[ii++] = (GT_U8)(hndl >> 8);
    hdr_PTR[ii++] = (GT_U8)(hndl);
#endif /* !(__WORDSIZE == 64) */

    if (cpssExtDrvEthPortTx(segmentList, segmentLen, segment,pcktParamsPtr->sdmaInfo.txQueue) != GT_OK)
    {
        rc = prvCpssGenNetIfMiiPoolFreeBuf(PRV_CPSS_GEN_NETIF_MII_TX_POOL_E, hdr_PTR);
#if __WORDSIZE == 64
        cpssOsFree(cookie_handler);
#endif
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChNetIfMiiRxGet
*
* DESCRIPTION:
*       This function returns packets from PP destined to the CPU port.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - Device number.
*       queueIdx    - The queue from which this packet was received.
*       numOfBuffPtr- Num of buffs in packetBuffsArrPtr.
*
* OUTPUTS:
*       numOfBuffPtr        - Num of used buffs in packetBuffsArrPtr.
*       packetBuffsArrPtr   - The received packet buffers list.
*       buffLenArr          - List of buffer lengths for packetBuffs.
*       rxParamsPtr         - (Pointer to)information parameters of received packets
*
* RETURNS:
*       GT_NO_MORE               - no more packets on the device/queue
*       GT_OK                    - packet got with no error.
*       GT_FAIL                  - failed to get the packet
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - wrong devNum,portNum,queueIdx.
*       GT_NOT_INITIALIZED       - the library was not initialized
*       GT_DSA_PARSING_ERROR     - DSA tag parsing error.
*
* COMMENTS:
*       It is recommended to call cpssDxChNetIfRxBufFree for this queue
*       i.e. return the buffer to their original queue.
*
*******************************************************************************/
GT_STATUS prvCpssDxChNetIfMiiRxGet
(
    IN GT_U8                                devNum,
    IN GT_U8                                queueIdx,
    INOUT GT_U32                            *numOfBuffPtr,
    OUT GT_U8                               *packetBuffsArrPtr[],
    OUT GT_U32                              buffLenArr[],
    OUT CPSS_DXCH_NET_RX_PARAMS_STC         *rxParamsPtr
)
{
    GT_STATUS   rc = GT_BAD_STATE; /* Return Code                                  */
    GT_U32      i;          /* Iterator                                     */
    GT_U32      byte2Remove;/* Bytes to remove in order to remove DSA tag   */
    GT_U32      pcktOffset; /* Offset from the start of the buffer          */
    GT_U8       *buffPtr;   /* temporary buffer ptr                         */
    GT_U32      dsaSize;
    GT_BOOL     isTagged = GT_FALSE;
    GT_U32      cfiBit = 0;
    GT_U32      dsaOffset = 0;/* offset of the DSA tag from start of packet */
    PRV_CPSS_NETIF_MII_RX_Q_CTRL_STC *qCtrl_PTR;

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(queueIdx);
    CPSS_NULL_PTR_CHECK_MAC(numOfBuffPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetBuffsArrPtr);
    CPSS_NULL_PTR_CHECK_MAC(buffLenArr);
    CPSS_NULL_PTR_CHECK_MAC(rxParamsPtr);

    /* get the Rx control block */
    qCtrl_PTR = prvCpssGenNetIfMiiRxQCtrlGet();

    if (NULL == qCtrl_PTR)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    /* get the raw packet */
    rc = prvCpssGenNetIfMiiRxPacketGet(queueIdx,numOfBuffPtr,packetBuffsArrPtr,buffLenArr);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->netIf.prePendTwoBytesHeader == GT_TRUE)
    {
        /* the PP will put the start of the Ethernet header of the packet , in 2
           bytes offset , to allow the IP header of the packet to be 4 bytes
           aligned */

        dsaOffset += PRV_CPSS_DSA_TAG_PCKT_OFFSET_CNS + /* 12 */
                    PRV_CPSS_DXCH_IP_ALIGN_OFFSET_CNS; /* 2 */
    }
    else
    {
        dsaOffset += PRV_CPSS_DSA_TAG_PCKT_OFFSET_CNS; /* 12 */
    }

    rc = cpssDxChNetIfDsaTagParse(devNum, &packetBuffsArrPtr[0][dsaOffset],
                                    &rxParamsPtr->dsaParam);
    if(rc != GT_OK)
    {
        /* we "remove" 8 bytes to free the packet with prvCpssGenNetIfMiiRxBufFree*/
        packetBuffsArrPtr[0] = packetBuffsArrPtr[0] + 8;
        buffLenArr[0]  = buffLenArr[0] - 8;

        /* save info on how much bytes the pointer of packet moved */
        PRV_CPSS_NETIF_MII_RX_ORIG_PCKT_TAG_BYTES_SET(packetBuffsArrPtr[0], 8, qCtrl_PTR->headerOffset);
        /* don't check rc of RxBufFree - problem with ethPrtRxGet, what happens with
           RxBufFree not important */
        prvCpssGenNetIfMiiRxBufFree(devNum,queueIdx,packetBuffsArrPtr,*numOfBuffPtr);
        return rc;
    }

    switch(rxParamsPtr->dsaParam.commonParams.dsaTagType)
    {
        case CPSS_DXCH_NET_DSA_1_WORD_TYPE_ENT:
            dsaSize = 4;
            break;
        case CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT:
            dsaSize = 8;
            break;
        case CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT:
            dsaSize = 16;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch(rxParamsPtr->dsaParam.dsaType)
    {
        case CPSS_DXCH_NET_DSA_CMD_TO_CPU_E:
            isTagged = rxParamsPtr->dsaParam.dsaInfo.toCpu.isTagged ;
            break;
        case CPSS_DXCH_NET_DSA_CMD_FORWARD_E:
            isTagged = rxParamsPtr->dsaParam.dsaInfo.forward.srcIsTagged;
            break;
        case CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E:
            isTagged = rxParamsPtr->dsaParam.dsaInfo.toAnalyzer.isTagged;
            break;
        default:
            isTagged = GT_TRUE;/* from CPU */
            break;
    }

    cfiBit = rxParamsPtr->dsaParam.commonParams.cfiBit;

    /**************************************************/
    /* remove the extended marvell tag from buffer[0] */
    /**************************************************/
    if(GT_TRUE == isTagged)
    {
        /* packet was tagged originally */

        byte2Remove = dsaSize - 4 ;/* 12 if was extended 4 words dsa tag
                                      4 if was extended 2 words dsa tag ,
                                      0 if was regular 1 word dsa tag */
        /* after the vlan tag */
        pcktOffset = dsaOffset + 4;/*16 or 18 */

        /* restore the "vlan tag style" to the needed bytes */
        buffPtr = (GT_U8 *)&packetBuffsArrPtr[0][dsaOffset];

        /* set 0x8100 */
        buffPtr[0] = (GT_U8)PRV_CPSS_GEN_NETIF_VLAN_ETHR_TYPE_MSB_CNS;
        buffPtr[1] = (GT_U8)PRV_CPSS_GEN_NETIF_VLAN_ETHR_TYPE_LSB_CNS;

        /* reset the cfiBit value */
        buffPtr[2] &= ~(1<<4);

        /* set the cfiBit -- according to info */
        buffPtr[2] |= (cfiBit << 4);
    }
    else
    {
        byte2Remove = dsaSize;/* 16 if was CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT dsa tag,
                                 8 if was CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT dsa tag ,
                                 4 if was CPSS_DXCH_NET_DSA_1_WORD_TYPE_ENT dsa tag */

        /* after the mac addresses  */
        pcktOffset = dsaOffset;/*12 or 14 */
    }

    /* move the start of the data on the buffer */
    buffPtr = (GT_U8 *)&packetBuffsArrPtr[0][pcktOffset-1];

    for(i = pcktOffset ; i; i-- , buffPtr--)
    {
        buffPtr[byte2Remove] = buffPtr[0];
    }

    packetBuffsArrPtr[0] = (buffPtr + 1) + byte2Remove;
    buffLenArr[0]  = buffLenArr[0] - byte2Remove;

    /* Next code is for internal use :
       to know how to retrieve actual pointer to the buffer when free the buffer

       Note : this action of emulation tagged/untagged must be done after the
       packetBuffsArrPtr[0] is stable (the pointer is not changed any more)
    */

    /* save info on how much bytes the pointer of packet moved */
    PRV_CPSS_NETIF_MII_RX_ORIG_PCKT_TAG_BYTES_SET(packetBuffsArrPtr[0], byte2Remove, qCtrl_PTR->headerOffset);

    return GT_OK;
}

