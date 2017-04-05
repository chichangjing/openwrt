/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssPhy.h
*
* DESCRIPTION:
*        Definitions and enumerators for port Core Serial Management
*        Interface facility.
*
* FILE REVISION NUMBER:
*       $Revision: 18 $
*
*******************************************************************************/
#ifndef __prvCpssPhyh
#define __prvCpssPhyh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/phy/cpssGenPhySmi.h>

/* use in accessing Twist-D-XG's SMI control Reg */
#define PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS   0x05 /* DTE device address */
#define PRV_CPSS_PHY_XENPAK_PORT_ADDRESS_CNS  0x01 /* DTE port address   */

/* default values to indicate no SMI configuration */
#define PRV_CPSS_PHY_SMI_NO_PORT_ADDR_CNS   0xFFFF
#define PRV_CPSS_PHY_SMI_NO_CTRL_ADDR_CNS   0xFFFFFFFF
#define PRV_CPSS_PHY_INVALIDE_SMI_INSTANCE_CNS  0xFFFF

/* macro to get the port's PHY SMI address -- not relevant to XG ports */
#define PRV_CPSS_PHY_SMI_PORT_ADDR_MAC(devNum,portNum)  \
    PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].smiIfInfo.smiPortAddr

/* macro to get the port's PHY SMI port group -- not relevant to XG ports */
#define PRV_CPSS_PHY_SMI_GROUP_PORT_MAC(devNum,portNum)  \
    PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].smiIfInfo.portGroupHostingSmiInterface

    /* macro to get the port's SMI control register address --
   not relevant to XG ports */
#define PRV_CPSS_PHY_SMI_CTRL_REG_ADDR_MAC(devNum,portNum)  \
    PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].smiIfInfo.smiCtrlRegAddr

#define PRV_CPSS_PHY_SMI_INSTANCE_MAC(devNum,portNum)  \
    PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].smiIfInfo.smiInterface


/* macro to get the port's PHY XSMI interface */
#define PRV_CPSS_PHY_XSMI_PORT_INTERFACE_MAC(devNum,portNum)  \
    PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].xsmiInterface

/*
 * Typedef: GT_PORT_SMI_IF_INFO
 *
 * Description: Holds information about the Smi interface to be used for a given
 *              port.
 *
 * Fields:
 *      smiInterface                 - relation of port to SMI
 *      smiPortAddr                  - SMI port address.
 *      smiCtrlRegAddr               - Address of the Smi control register to be used for the
 *                                     Port
 *      portGroupHostingSmiInterface - the port group that hosts the SMI to access the port (PHY)
 *
 */
typedef struct
{
    GT_U16  smiInterface;
    GT_U16  smiPortAddr;
    GT_U32  smiCtrlRegAddr;
    GT_U32  portGroupHostingSmiInterface;
}PRV_CPSS_PHY_PORT_SMI_IF_INFO_STC;

/*******************************************************************************
* PRV_CPSS_VCT_PHY_READ_PHY_REGISTER_FUN
*
* DESCRIPTION:
*       Read specified SMI Register on a specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*       phyReg - SMI register
*
* OUTPUTS:
*       data  - data read.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_VCT_PHY_READ_PHY_REGISTER_FUN)
(
     IN  GT_U8     dev,
     IN  GT_PHYSICAL_PORT_NUM  port,
     IN  GT_U8     phyReg,
     OUT GT_U16    *data
);
/*******************************************************************************
* PRV_CPSS_VCT_PHY_WRITE_PHY_REGISTER_FUN
*
* DESCRIPTION:
*       Write value to specified SMI Register on a specified port on
*       specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*       phyReg - SMI register
*       data   - value to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_VCT_PHY_WRITE_PHY_REGISTER_FUN)
(
     IN  GT_U8     dev,
     IN  GT_PHYSICAL_PORT_NUM  port,
     IN  GT_U8     phyReg,
     IN  GT_U16    data
);

/*******************************************************************************
* PRV_CPSS_VCT_PHY_AUTONEG_SET_FUN
*
* DESCRIPTION:
*       This function sets the auto negotiation process between the PP and
*       Phy status.
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - the device number
*       port            - port number
*       enable          - enable/disable Auto Negotiation status
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*
* COMMENTS:
*

*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_VCT_PHY_AUTONEG_SET_FUN)
(
     IN  GT_U8     devNum,
     IN  GT_PHYSICAL_PORT_NUM   port,
     IN  GT_BOOL   enable
);

/*******************************************************************************
* PRV_CPSS_VCT_PHY_AUTONEG_GET_FUN
*
* DESCRIPTION:
*       This function sets the auto negotiation process between the PP and
*       Phy status.
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - the device number
*       port            - port number
*       enablePtr       - read Auto Negotiation status
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*
* COMMENTS:
*

*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_VCT_PHY_AUTONEG_GET_FUN)
(
     IN  GT_U8     devNum,
     IN  GT_PHYSICAL_PORT_NUM     port,
     OUT GT_BOOL   *enablePtr
);

/*
 * Typedef: struct PRV_CPSS_VCT_GEN_BIND_FUNC_STC
 *
 * Description: A structure to hold common VCT functions for PP Family needed
 *              in CPSS
 *      INFO "PER DEVICE FAMILY"
 *
 * Fields:
 *
 *  cpssPhyRegisterRead   - pointer to read phy register function;
 *  cpssPhyRegisterWrite  - pointer to write phy register function;
 *  cpssPhyAutoNegotiationSet - pointer to set phy auto-neg. poll state function;
 *  cpssPhyAutoNegotiationGet - pointer to get phy auto-neg. poll state function;
 *
 */
typedef struct{
    PRV_CPSS_VCT_PHY_READ_PHY_REGISTER_FUN     cpssPhyRegisterRead;
    PRV_CPSS_VCT_PHY_WRITE_PHY_REGISTER_FUN    cpssPhyRegisterWrite;
    PRV_CPSS_VCT_PHY_AUTONEG_SET_FUN           cpssPhyAutoNegotiationSet;
    PRV_CPSS_VCT_PHY_AUTONEG_GET_FUN           cpssPhyAutoNegotiationGet;

}PRV_CPSS_VCT_GEN_BIND_FUNC_STC;


/*******************************************************************************
* PRV_CPSS_SMI_PHY_READ_CTRL_REGISTER_FUN
*
* DESCRIPTION:
*       Read to SMI control register
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The PP to read from.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       smiInterface    - SMI interface.
*
* OUTPUTS:
*       data - Includes the register value.
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
typedef GT_STATUS (*PRV_CPSS_SMI_PHY_READ_CTRL_REGISTER_FUN)
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN  CPSS_PHY_SMI_INTERFACE_ENT  smiInterface,
    OUT GT_U32  *data
);

/*******************************************************************************
* PRV_CPSS_SMI_PHY_WRITE_CTRL_REGISTER_FUN
*
* DESCRIPTION:
*       Write to SMI control register
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The PP to write to.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       smiInterface - SMI interface.
*       data        - The value to be written.
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
typedef GT_STATUS (*PRV_CPSS_SMI_PHY_WRITE_CTRL_REGISTER_FUN)
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN  CPSS_PHY_SMI_INTERFACE_ENT  smiInterface,
    IN GT_U32   value
);

/*
 * Typedef: struct PRV_CPSS_SMI_GEN_BIND_FUNC_STC
 *
 * Description: A structure to hold common SMI functions for PP Family needed
 *              in CPSS
 *      INFO "PER DEVICE FAMILY"
 *
 * Fields:
 *
 *  cpssPhySmiCtrlRegRead   - read smi control register;
 *  cpssPhySmiCtrlRegWrite  - write smi control register;
 *
 */
typedef struct{
    PRV_CPSS_SMI_PHY_READ_CTRL_REGISTER_FUN     cpssPhySmiCtrlRegRead;
    PRV_CPSS_SMI_PHY_WRITE_CTRL_REGISTER_FUN    cpssPhySmiCtrlRegWrite;

}PRV_CPSS_SMI_GEN_BIND_FUNC_STC;


/*******************************************************************************
* PRV_CPSS_XSMI_PHY_MNG_REGISTERS_ADDR_GET_FUN
*
* DESCRIPTION:
*       Get addresses of control and address XSMI registers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The PP to read from.
*       xsmiInterface - XSMI instance
*
* OUTPUTS:
*       ctrlRegAddrPtr - (ptr to) address of XSMI control register
*       addrRegAddrPtr - (ptr to) address of XSMI address register
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM - bad devNum
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_XSMI_PHY_MNG_REGISTERS_ADDR_GET_FUN)
(
    IN GT_U8    devNum,
    IN  CPSS_PHY_XSMI_INTERFACE_ENT xsmiInterface,
    OUT GT_U32  *ctrlRegAddrPtr,
    OUT GT_U32  *addrRegAddrPtr
);

/*
 * Typedef: struct PRV_CPSS_XSMI_GEN_BIND_FUNC_STC
 *
 * Description: A structure to hold common XSMI functions for PP Family needed
 *              in CPSS
 *      INFO "PER DEVICE FAMILY"
 *
 * Fields:
 *
 *  cpssPhyXSmiRegsAddrGet - get addresses of control and address XSMI registers;
 *
 */
typedef struct{
    PRV_CPSS_XSMI_PHY_MNG_REGISTERS_ADDR_GET_FUN cpssPhyXSmiRegsAddrGet;

}PRV_CPSS_XSMI_GEN_BIND_FUNC_STC;

/*
 * Typedef: PRV_CPSS_PHY_INFO_STC
 *
 * Description: Holds info regarding virtual functions
 *              port.
 *
 * Fields:
 *      genVctBindFunc - structure of pointers to VCT service routines
 *      genSmiBindFunc - structure of pointers to SMI service routines
 *      genXSmiBindFunc - structure of pointers to XSMI service routines
 *
 */
typedef struct
{
    PRV_CPSS_VCT_GEN_BIND_FUNC_STC genVctBindFunc;
    PRV_CPSS_SMI_GEN_BIND_FUNC_STC genSmiBindFunc;
    PRV_CPSS_XSMI_GEN_BIND_FUNC_STC genXSmiBindFunc;
}PRV_CPSS_PHY_INFO_STC;

/*******************************************************************************
* prvCpssVctDbRelease
*
* DESCRIPTION:
*       private (internal) function to release the DB of the VCT.
*       NOTE: function 'free' the allocated memory.
*             and restore DB to 'pre-init' state
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       None
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssVctDbRelease
(
    void
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssPhyh */

