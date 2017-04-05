/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenPhySmi.h
*
* DESCRIPTION:
*        Private API implementation for port Core Serial Management Interface facility.
*
* FILE REVISION NUMBER:
*       $Revision: 19 $
*
*******************************************************************************/
#ifndef __prvCpssGenPhySmih
#define __prvCpssGenPhySmih

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

#define PRV_CPSS_MARVELL_OUI_MSB     0x0141
#define PRV_CPSS_MARVELL_OUI_LSB     0x0C00
#define PRV_CPSS_OUI_LSB_MASK        0xFC00
#define PRV_CPSS_PHY_MODEL_MASK      0x03F0
#define PRV_CPSS_PHY_REV_MASK        0x000F

#define PRV_CPSS_DEV_E3082          (0x8 << 4)
#define PRV_CPSS_DEV_E104X          (0x2 << 4)
#define PRV_CPSS_DEV_E1111          (0xC << 4)
#define PRV_CPSS_DEV_E114X          (0xD << 4)
#define PRV_CPSS_DEV_E1180          (0xE << 4)
#define PRV_CPSS_DEV_E1112          (0x9 << 4)
#define PRV_CPSS_DEV_E1149          (0xA << 4)
#define PRV_CPSS_DEV_E1149R         (0x25 << 4)
#define PRV_CPSS_DEV_E1240          (0x23 << 4)
/* pre-production version of 88E1340*/
#define PRV_CPSS_DEV_E1340_X0       (0x27 << 4)
/* production and ES versions of 88E1340*/
#define PRV_CPSS_DEV_E1340          (0x1C << 4)
#define PRV_CPSS_DEV_E1540          (0x2B << 4)
/* PHYs 88E1543 & 88E1545 have the same device version 0x2A.         */
/* To distinguish between the two read page 253, register 28, bit 5: */
/* 0 for 88E1543, 1 for 88E1545.                                     */
#define PRV_CPSS_DEV_E1543          (0x2A << 4)
#define PRV_CPSS_DEV_E1680          (0x2D << 4)
/* PHYs 88E1680M & 88E1680L have the same device version 0x2E.       */
#define PRV_CPSS_DEV_E1680M         (0x2E << 4)
#define PRV_CPSS_DEV_E1548P         (0x2C << 4)

/*  macro to get a function pointer for VCT feature
    devNum  - the device id of the device
    portNum - port number
    NOTE : the macro do NO validly checks !!!!
           (caller responsible for checking with other means/macro)
*/
#define PRV_CPSS_PP_VCT_BIND_FUNC_MAC(devNum, portNum) \
    (&(PRV_CPSS_PP_MAC(devNum)->phyInfo))

/* Macro to perform WRITE operation to PHY register */
#define PRV_PHY_REG_WRITE_MAC(dev, port, phyRegOffset, data) \
    (PRV_CPSS_PP_VCT_BIND_FUNC_MAC(dev, portNum)->genVctBindFunc.cpssPhyRegisterWrite\
     (dev, port, phyRegOffset, data))

/* Macro to perform READ operation from PHY register */
#define PRV_PHY_REG_READ_MAC(dev, port, phyRegOffset, dataPtr) \
    (PRV_CPSS_PP_VCT_BIND_FUNC_MAC(dev, portNum)->genVctBindFunc.cpssPhyRegisterRead\
     (dev, port, phyRegOffset,dataPtr))

typedef enum
{
    PRV_CPSS_PHY_CNTRL = 0,          /* Control Register */
    PRV_CPSS_PHY_STATUS,             /* Status Register */
    PRV_CPSS_PHY_ID0,                /* PHY Identifier  */
    PRV_CPSS_PHY_ID1,                /* PHY Identifier  */
    PRV_CPSS_PHY_SPEC_STATUS = 17,   /* PHY Specific Status Register */
    PRV_CPSS_GE_PHY_EXT_ADDR = 22,   /* Extended Address for Cable Diagnostic Reg */
    PRV_CPSS_PHY_REG23 = 23,         /* Reg. 23 */
    PRV_CPSS_PHY_REG24 = 24,         /* Reg. 24 */
    PRV_CPSS_FE_PHY_CABLE_DIAG1 = 26,/* FE if Cable Diagnostic Registers for first pair*/
    PRV_CPSS_FE_PHY_CABLE_DIAG2 = 27,/* FE if Cable Diagnostic Registers for 2-nd pair */
    PRV_CPSS_GE_PHY_CABLE_DIAG = 28, /* GE interface Cable Diagnostic Registers */
    PRV_CPSS_PHY_REG29,              /* Reg. 29 */
    PRV_CPSS_PHY_REG30,              /* Reg. 30 */
    PRV_CPSS_PHY_REG31               /* Reg. 31 */

}PRV_CPSS_PHY_REG_ADDR;


/*******************************************************************************
* CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC
*
* DESCRIPTION:
*       Generic function prototype for read phy smi register.
*       Read specified SMI Register on a specified port on specified device.
*
*  APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       phyReg    - SMI register (value 0..31)
*
* OUTPUTS:
*       data      - Pointer to the read data.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_OUT_OF_RANGE  - phyAddr bigger then 31
*       GT_NOT_SUPPORTED - for XG ports
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (* CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC)
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    IN  GT_U8     phyReg,
    OUT GT_U16    *dataPtr
);

/*******************************************************************************
* CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC
*
* DESCRIPTION:
*       Generic function prototype for read phy smi register.
*       Write value to specified SMI Register on a specified port on
*       specified device.
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*       phyReg - SMI register (value 0..31)
*       data   - value to write 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_OUT_OF_RANGE          - phyAddr bigger then 31
*       GT_NOT_SUPPORTED         - for XG ports
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (* CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC)
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    IN  GT_U8     phyReg,
    IN  GT_U16    data
);


/*******************************************************************************
* prvCpssGenPhyErrataInitFix
*
* DESCRIPTION:
*   This function implements the WA for phys during the initialization.
*
* APPLICABLE DEVICES:  
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - Device Number.
*       portNum             - Port Number.
*       smiReadRegister     - Read function
*       smiWriteRegister    - Write function
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssGenPhyErrataInitFix
(
    IN GT_U8 devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
);
/*******************************************************************************
* prvCpssPhyRegWriteErrataFix
*
* DESCRIPTION:
*   Implement PHY register write workarounds.
*
* APPLICABLE DEVICES:  
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*       regPhy    - phy's register address
*       data      - value to be set
*       smiWriteRegister - pointer to relevant SMI write implementation 
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssPhyRegWriteErrataFix
(
    IN GT_U8  devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN GT_U8  regPhy,
    IN GT_U16 data,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssGenPhySmih */

