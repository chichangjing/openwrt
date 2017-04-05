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
* cpssGenPpSmi.c
*
* DESCRIPTION:
*       API for read/write register of device, which connected to SMI master
*           controller of packet processor
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 13 $
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/generic/smi/private/prvCpssGenSmiLog.h>

#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/phy/private/prvCpssPhy.h>
#include <cpss/generic/phy/cpssGenPhySmi.h>

/*************************** Private definitions ******************************/

#define  PRV_CPSS_SMI_WRITE_ADDRESS_MSB_REGISTER_CNS   (0x00)
#define  PRV_CPSS_SMI_WRITE_ADDRESS_LSB_REGISTER_CNS   (0x01)
#define  PRV_CPSS_SMI_WRITE_DATA_MSB_REGISTER_CNS      (0x02)
#define  PRV_CPSS_SMI_WRITE_DATA_LSB_REGISTER_CNS      (0x03)
#define  PRV_CPSS_SMI_READ_ADDRESS_MSB_REGISTER_CNS    (0x04)
#define  PRV_CPSS_SMI_READ_ADDRESS_LSB_REGISTER_CNS    (0x05)
#define  PRV_CPSS_SMI_READ_DATA_MSB_REGISTER_CNS       (0x06)
#define  PRV_CPSS_SMI_READ_DATA_LSB_REGISTER_CNS       (0x07)
#define  PRV_CPSS_SMI_STATUS_REGISTER_CNS              (0x1f)
#define  PRV_CPSS_SMI_STATUS_WRITE_DONE_CNS            (0x02)
#define  PRV_CPSS_SMI_STATUS_READ_READY_CNS            (0x01)

#define PRV_CPSS_SMI_MNG_CNTRL_OFFSET_CNS    0x1000000
#define PRV_CPSS_SMI_RETRY_COUNTER_CNS       1000

/*************************** Private functions ********************************/

/*******************************************************************************
* smiStatusCheck
*
* DESCRIPTION:
*      Check if SMI is busy or during read operation
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev - device number
*       portGroup - port group number
*       smiInterface - SMI master interface Id
*       read - set to TRUE if it read operation
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_HW_ERROR - HW failure
*       GT_NOT_READY - xsmi busy
*
* COMMENTS:
*       None.
*
*******************************************************************************/
#ifndef ASIC_SIMULATION
static GT_STATUS smiStatusCheck
(
    IN GT_U8    dev,
    IN GT_U32   portGroup,
    IN CPSS_PHY_SMI_INTERFACE_ENT   smiInterface,
    IN GT_BOOL  read
)
{
    GT_U32 status;
    GT_U32 desired_status = 0;
    GT_U32 bit = 28;
    volatile GT_U32 retryCnt = PRV_CPSS_SMI_RETRY_COUNTER_CNS;   /* retry counter for busy SMI reg   */

    /* check if for given device there is registered callback */
    if(PRV_CPSS_PP_MAC(dev)->phyInfo.genSmiBindFunc.cpssPhySmiCtrlRegRead == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);

    if (GT_TRUE == read)
    {
        desired_status = 1;
        bit = 27;
    }

    /* check if smi register is not busy or read operation done */
    do
    {
        if (PRV_CPSS_PP_MAC(dev)->phyInfo.genSmiBindFunc.cpssPhySmiCtrlRegRead(dev,
                                            portGroup, smiInterface, &status) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
        status = U32_GET_FIELD_MAC(status, bit, 1);
        retryCnt--;
    }while ((status != desired_status) && (retryCnt != 0));

    if (0 == retryCnt)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_READY, LOG_ERROR_NO_MSG);

    return GT_OK;
}
#endif


/*******************************************************************************
* xsmiStatusCheck
*
* DESCRIPTION:
*      Check if XSMI is busy or during read operation
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev - device number
*       portGroupId - port group number
*       read - set to TRUE if it read operation
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_HW_ERROR - HW failure
*       GT_NOT_READY - xsmi busy
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS xsmiStatusCheck
(
    IN GT_U8                        dev,
    IN GT_U32                       portGroupId,
    IN  CPSS_PHY_XSMI_INTERFACE_ENT xsmiInterface,
    IN GT_BOOL                      read
)
{
    GT_U32 busy;
    GT_U32 desired_status = 0;
    GT_U32 bit = 30;
    volatile GT_U32 retryCnt = PRV_CPSS_SMI_RETRY_COUNTER_CNS; /* retry counter 
                                                                for busy SMI reg */
    GT_U32  ctrlRegAddr;    /* address of XSMI ctrl reg. */
    GT_U32  addrRegAddr;    /* address of XSMI addr reg. */
    GT_STATUS rc;

    if((rc = PRV_CPSS_PP_MAC(dev)->phyInfo.genXSmiBindFunc.
                cpssPhyXSmiRegsAddrGet(dev, xsmiInterface, &ctrlRegAddr, 
                                        &addrRegAddr)) != GT_OK)
        return rc;

    if (GT_TRUE == read)
    {
        desired_status = 1;
        bit = 29;
    }

    do
    {
        if (prvCpssHwPpPortGroupGetRegField(dev, portGroupId, ctrlRegAddr, bit, 1, &busy) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
        retryCnt--;
    }while ((busy != desired_status) && (retryCnt != 0));

    if (0 == retryCnt)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_READY, LOG_ERROR_NO_MSG);

    return GT_OK;
}

/*************************** Global functions ********************************/

/*******************************************************************************
* internal_cpssSmiRegisterReadShort
*
* DESCRIPTION:
*      The function reads register of a device, which connected to SMI master
*           controller of packet processor
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       smiInterface - SMI master interface Id
*       smiAddr     - address of configurated device on SMI (APPLICABLE RANGES: 0..31)
*       regAddr     - register address
*
* OUTPUTS:
*       dataPtr - pointer to place data from read operation
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, smiInterface
*       GT_BAD_PTR      - pointer to place data is NULL
*       GT_NOT_READY    - smi is busy
*       GT_HW_ERROR     - hw error
*       GT_NOT_INITIALIZED - smi ctrl register callback not registered
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssSmiRegisterReadShort
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  CPSS_PHY_SMI_INTERFACE_ENT   smiInterface,
    IN  GT_U32  smiAddr,
    IN  GT_U32  regAddr,
    OUT GT_U16  *dataPtr
)
{
#ifndef ASIC_SIMULATION
    GT_STATUS rc;
    GT_U32  regVal;
    GT_U32  readData = 0;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(0 == PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {   /* for non multi-port groups device the MACRO assign value */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    }

    CPSS_NULL_PTR_CHECK_MAC(dataPtr);

    if(smiInterface >= CPSS_PHY_SMI_INTERFACE_MAX_E)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    /* check if smi register is not busy */
    if ((rc = smiStatusCheck(devNum, portGroupId, smiInterface, GT_FALSE)) != GT_OK)
    {
        return rc;
    }

    regVal = ((smiAddr & 0x1F) << 16) | ((regAddr & 0x1F) << 21) | (1 << 26) ;
    if (PRV_CPSS_PP_MAC(devNum)->phyInfo.genSmiBindFunc.cpssPhySmiCtrlRegWrite(devNum,
                                                portGroupId, smiInterface, regVal) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    /* check if smi read operation done */
    if ((rc = smiStatusCheck(devNum, portGroupId, smiInterface, GT_TRUE)) != GT_OK)
    {
        return rc;
    }

    /* Read from Lion SMI management register */
    if (PRV_CPSS_PP_MAC(devNum)->phyInfo.genSmiBindFunc.cpssPhySmiCtrlRegRead(devNum,
                                        portGroupId, smiInterface, &readData) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    *dataPtr = (GT_U16)(readData & 0xFFFF);
#else
    devNum          = devNum;
    portGroupsBmp   = portGroupsBmp;
    smiInterface    = smiInterface;
    smiAddr         = smiAddr;
    regAddr         = regAddr;
    dataPtr         = dataPtr;
#endif
    return GT_OK;
}

/*******************************************************************************
* cpssSmiRegisterReadShort
*
* DESCRIPTION:
*      The function reads register of a device, which connected to SMI master
*           controller of packet processor
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       smiInterface - SMI master interface Id
*       smiAddr     - address of configurated device on SMI (APPLICABLE RANGES: 0..31)
*       regAddr     - register address
*
* OUTPUTS:
*       dataPtr - pointer to place data from read operation
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, smiInterface
*       GT_BAD_PTR      - pointer to place data is NULL
*       GT_NOT_READY    - smi is busy
*       GT_HW_ERROR     - hw error
*       GT_NOT_INITIALIZED - smi ctrl register callback not registered
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssSmiRegisterReadShort
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  CPSS_PHY_SMI_INTERFACE_ENT   smiInterface,
    IN  GT_U32  smiAddr,
    IN  GT_U32  regAddr,
    OUT GT_U16  *dataPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssSmiRegisterReadShort);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, smiInterface, smiAddr, regAddr, dataPtr));

    rc = internal_cpssSmiRegisterReadShort(devNum, portGroupsBmp, smiInterface, smiAddr, regAddr, dataPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, smiInterface, smiAddr, regAddr, dataPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssSmiRegisterWriteShort
*
* DESCRIPTION:
*      The function writes register of a device, which connected to SMI master
*           controller of packet processor
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       smiInterface - SMI master interface Id
*       smiAddr     - address of configurated device on SMI (APPLICABLE RANGES: 0..31)
*       regAddr     - address of register of configurated device
*       data        - data to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, smiInterface
*       GT_NOT_READY    - smi is busy
*       GT_HW_ERROR     - hw error
*       GT_NOT_INITIALIZED - smi ctrl register callback not registered
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssSmiRegisterWriteShort
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  CPSS_PHY_SMI_INTERFACE_ENT   smiInterface,
    IN  GT_U32  smiAddr,
    IN  GT_U32  regAddr,
    IN  GT_U16  data
)
{
#ifndef ASIC_SIMULATION
    GT_STATUS rc;
    GT_U32  regVal;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(0 == PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {   /* for non multi-port groups device the MACRO assign value */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    }

    if(smiInterface >= CPSS_PHY_SMI_INTERFACE_MAX_E)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    regVal = data & 0xFFFF;
    regVal |= ((smiAddr & 0x1F) << 16) | ((regAddr & 0x1F) << 21) | (0 << 26) ;

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        /* check if smi register is not busy */
        if ((rc = smiStatusCheck(devNum, portGroupId, smiInterface, GT_FALSE)) != GT_OK)
        {
            return rc;
        }

        if (PRV_CPSS_PP_MAC(devNum)->phyInfo.genSmiBindFunc.cpssPhySmiCtrlRegWrite(devNum,
                                                portGroupId, smiInterface, regVal) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
#else
    devNum          = devNum;
    portGroupsBmp   = portGroupsBmp;
    smiInterface    = smiInterface;
    smiAddr         = smiAddr;
    regAddr         = regAddr;
    data            = data;
#endif
    return GT_OK;
}

/*******************************************************************************
* cpssSmiRegisterWriteShort
*
* DESCRIPTION:
*      The function writes register of a device, which connected to SMI master
*           controller of packet processor
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       smiInterface - SMI master interface Id
*       smiAddr     - address of configurated device on SMI (APPLICABLE RANGES: 0..31)
*       regAddr     - address of register of configurated device
*       data        - data to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, smiInterface
*       GT_NOT_READY    - smi is busy
*       GT_HW_ERROR     - hw error
*       GT_NOT_INITIALIZED - smi ctrl register callback not registered
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssSmiRegisterWriteShort
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  CPSS_PHY_SMI_INTERFACE_ENT   smiInterface,
    IN  GT_U32  smiAddr,
    IN  GT_U32  regAddr,
    IN  GT_U16  data
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssSmiRegisterWriteShort);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, smiInterface, smiAddr, regAddr, data));

    rc = internal_cpssSmiRegisterWriteShort(devNum, portGroupsBmp, smiInterface, smiAddr, regAddr, data);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, smiInterface, smiAddr, regAddr, data));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssSmiRegisterRead
*
* DESCRIPTION:
*      The function reads register of a Marvell device, which connected to
*           SMI master controller of packet processor
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       smiInterface - SMI master interface Id
*       smiAddr     - address of configurated device on SMI (APPLICABLE RANGES: 0..31)
*       regAddr     - register address
*
* OUTPUTS:
*       dataPtr - pointer to place data from read operation
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, smiInterface
*       GT_BAD_PTR      - pointer to place data is NULL
*       GT_NOT_READY    - smi is busy
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       Function specific for Marvell devices with 32-bit registers
*
*******************************************************************************/
static GT_STATUS internal_cpssSmiRegisterRead
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  CPSS_PHY_SMI_INTERFACE_ENT   smiInterface,
    IN  GT_U32  smiAddr,
    IN  GT_U32  regAddr,
    OUT GT_U32  *dataPtr
)
{
#ifndef ASIC_SIMULATION
    GT_U16              msb;
    GT_U16              lsb;
    GT_U32              value;
    GT_STATUS           rc;

    CPSS_NULL_PTR_CHECK_MAC(dataPtr);

    /* write addr to read */
    msb = (GT_U16)(regAddr >> 16);
    lsb = (GT_U16)(regAddr & 0xFFFF);

    if ((rc = cpssSmiRegisterWriteShort(devNum, portGroupsBmp, smiInterface, smiAddr,
                                        PRV_CPSS_SMI_READ_ADDRESS_MSB_REGISTER_CNS, msb)) != GT_OK)
    {
        return rc;
    }

    if ((rc = cpssSmiRegisterWriteShort(devNum, portGroupsBmp, smiInterface, smiAddr,
                                        PRV_CPSS_SMI_READ_ADDRESS_LSB_REGISTER_CNS, lsb)) != GT_OK)
    {
        return rc;
    }

    /* read data */
    if ((rc = cpssSmiRegisterReadShort(devNum, portGroupsBmp, smiInterface, smiAddr,
                                        PRV_CPSS_SMI_READ_DATA_MSB_REGISTER_CNS, &msb)) != GT_OK)
    {
        return rc;
    }

    if ((rc = cpssSmiRegisterReadShort(devNum, portGroupsBmp, smiInterface, smiAddr,
                                        PRV_CPSS_SMI_READ_DATA_LSB_REGISTER_CNS, &lsb)) != GT_OK)
    {
        return rc;
    }

    value = (GT_U32)msb;
    *dataPtr = (value << 16) | lsb;
#else

    devNum  =       devNum;
    portGroupsBmp = portGroupsBmp;
    smiInterface  = smiInterface;
    smiAddr =       smiAddr;
    regAddr =       regAddr;
    dataPtr =       dataPtr;

#endif

    return GT_OK;
}

/*******************************************************************************
* cpssSmiRegisterRead
*
* DESCRIPTION:
*      The function reads register of a Marvell device, which connected to
*           SMI master controller of packet processor
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       smiInterface - SMI master interface Id
*       smiAddr     - address of configurated device on SMI (APPLICABLE RANGES: 0..31)
*       regAddr     - register address
*
* OUTPUTS:
*       dataPtr - pointer to place data from read operation
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, smiInterface
*       GT_BAD_PTR      - pointer to place data is NULL
*       GT_NOT_READY    - smi is busy
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       Function specific for Marvell devices with 32-bit registers
*
*******************************************************************************/
GT_STATUS cpssSmiRegisterRead
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  CPSS_PHY_SMI_INTERFACE_ENT   smiInterface,
    IN  GT_U32  smiAddr,
    IN  GT_U32  regAddr,
    OUT GT_U32  *dataPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssSmiRegisterRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, smiInterface, smiAddr, regAddr, dataPtr));

    rc = internal_cpssSmiRegisterRead(devNum, portGroupsBmp, smiInterface, smiAddr, regAddr, dataPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, smiInterface, smiAddr, regAddr, dataPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssSmiRegisterWrite
*
* DESCRIPTION:
*      The function writes register of a Marvell device, which connected to SMI master
*           controller of packet processor
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       smiInterface - SMI master interface Id
*       smiAddr     - address of configurated device on SMI (APPLICABLE RANGES: 0..31)
*       regAddr     - address of register of configurated device
*       data        - data to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, smiInterface
*       GT_NOT_READY    - smi is busy
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       Function specific for Marvell devices with 32-bit registers
*
*******************************************************************************/
static GT_STATUS internal_cpssSmiRegisterWrite
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  CPSS_PHY_SMI_INTERFACE_ENT   smiInterface,
    IN  GT_U32  smiAddr,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
)
{
#ifndef ASIC_SIMULATION
    GT_U16              msb;
    GT_U16              lsb;

    /* write addr to write */
    msb = (GT_U16)(regAddr >> 16);
    lsb = (GT_U16)(regAddr & 0xFFFF);

    if (cpssSmiRegisterWriteShort(devNum, portGroupsBmp, smiInterface, smiAddr,
                                PRV_CPSS_SMI_WRITE_ADDRESS_MSB_REGISTER_CNS, msb) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    if (cpssSmiRegisterWriteShort(devNum, portGroupsBmp, smiInterface, smiAddr,
                                PRV_CPSS_SMI_WRITE_ADDRESS_LSB_REGISTER_CNS, lsb) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    /* write data to write */
    msb = (GT_U16)(data >> 16);
    lsb = (GT_U16)(data & 0xFFFF);

    if (cpssSmiRegisterWriteShort(devNum, portGroupsBmp, smiInterface, smiAddr,
                                PRV_CPSS_SMI_WRITE_DATA_MSB_REGISTER_CNS, msb) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    if (cpssSmiRegisterWriteShort(devNum, portGroupsBmp, smiInterface, smiAddr,
                                PRV_CPSS_SMI_WRITE_DATA_LSB_REGISTER_CNS, lsb) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }
#else

    devNum  =       devNum;
    portGroupsBmp = portGroupsBmp;
    smiInterface  = smiInterface;
    smiAddr =       smiAddr;
    regAddr =       regAddr;
    data    =       data;

#endif

    return GT_OK;
}

/*******************************************************************************
* cpssSmiRegisterWrite
*
* DESCRIPTION:
*      The function writes register of a Marvell device, which connected to SMI master
*           controller of packet processor
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       smiInterface - SMI master interface Id
*       smiAddr     - address of configurated device on SMI (APPLICABLE RANGES: 0..31)
*       regAddr     - address of register of configurated device
*       data        - data to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, smiInterface
*       GT_NOT_READY    - smi is busy
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       Function specific for Marvell devices with 32-bit registers
*
*******************************************************************************/
GT_STATUS cpssSmiRegisterWrite
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  CPSS_PHY_SMI_INTERFACE_ENT   smiInterface,
    IN  GT_U32  smiAddr,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssSmiRegisterWrite);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, smiInterface, smiAddr, regAddr, data));

    rc = internal_cpssSmiRegisterWrite(devNum, portGroupsBmp, smiInterface, smiAddr, regAddr, data);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, smiInterface, smiAddr, regAddr, data));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssXsmiPortGroupRegisterWrite
*
* DESCRIPTION:
*       Write value to a specified XSMI Register.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       xsmiInterface - XSMI instance
*       xsmiAddr    - address of configurated device on XSMI (APPLICABLE RANGES: 0..31)
*       regAddr     - address of register of configurated device
*       phyDev      - the PHY device to write to (APPLICABLE RANGES: 0..31).
*       data        - data to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, xsmiAddr, phyDev.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_INITIALIZED       - callback not set
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssXsmiPortGroupRegisterWrite
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  CPSS_PHY_XSMI_INTERFACE_ENT xsmiInterface,
    IN  GT_U32  xsmiAddr,
    IN  GT_U32  regAddr,
    IN  GT_U32  phyDev,
    IN  GT_U16  data
)
{
    GT_U32 value = 0;   /* value to write into the register */
    GT_U32 portGroupId; /*the port group Id - support multi-port-groups device */
    GT_U32  ctrlRegAddr; /* address of XSMI ctrl reg. */
    GT_U32  addrRegAddr; /* address of XSMI addr reg. */
    GT_STATUS rc;

    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if((phyDev >= BIT_5) || (xsmiAddr >= BIT_5))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(NULL == PRV_CPSS_PP_MAC(devNum)->phyInfo.genXSmiBindFunc.cpssPhyXSmiRegsAddrGet)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);

    if((rc = PRV_CPSS_PP_MAC(devNum)->phyInfo.genXSmiBindFunc.
                cpssPhyXSmiRegsAddrGet(devNum, xsmiInterface, &ctrlRegAddr, 
                                       &addrRegAddr)) != GT_OK)
        return rc;

    if(0 == PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {   /* for non multi-port groups device the MACRO assign value */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    }

    /* set data */
    value = data;

    /* set port address */
    value |= (xsmiAddr << 16);

    /* set device address */
    value |= (phyDev << 21);

    /* set write opcode */
    value |= (5 << 26);

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        if((rc = xsmiStatusCheck(devNum, portGroupId, xsmiInterface, GT_FALSE)) != GT_OK)
            return rc;

        /* write address */
        if (prvCpssHwPpPortGroupSetRegField(devNum, portGroupId, addrRegAddr,
                0, 16, regAddr) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

        /* Write the value */
        if (prvCpssHwPpPortGroupWriteRegister(devNum, portGroupId, ctrlRegAddr,
                value) != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssXsmiPortGroupRegisterWrite
*
* DESCRIPTION:
*       Write value to a specified XSMI Register.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       xsmiInterface - XSMI instance
*       xsmiAddr    - address of configurated device on XSMI (APPLICABLE RANGES: 0..31)
*       regAddr     - address of register of configurated device
*       phyDev      - the PHY device to write to (APPLICABLE RANGES: 0..31).
*       data        - data to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, xsmiAddr, phyDev.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_INITIALIZED       - callback not set
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssXsmiPortGroupRegisterWrite
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  CPSS_PHY_XSMI_INTERFACE_ENT xsmiInterface,
    IN  GT_U32  xsmiAddr,
    IN  GT_U32  regAddr,
    IN  GT_U32  phyDev,
    IN  GT_U16  data
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssXsmiPortGroupRegisterWrite);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, xsmiInterface, xsmiAddr, regAddr, phyDev, data));

    rc = internal_cpssXsmiPortGroupRegisterWrite(devNum, portGroupsBmp, xsmiInterface, xsmiAddr, regAddr, phyDev, data);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, xsmiInterface, xsmiAddr, regAddr, phyDev, data));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssXsmiPortGroupRegisterRead
*
* DESCRIPTION:
*       Read value of a specified XSMI Register.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       xsmiInterface - XSMI instance
*       xsmiAddr    - address of configurated device on XSMI (APPLICABLE RANGES: 0..31)
*       regAddr     - address of register of configurated device
*       phyDev      - the PHY device to read from (APPLICABLE RANGES: 0..31).
*
* OUTPUTS:
*       dataPtr - (Pointer to) the read data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameter.
*       GT_BAD_PTR               - pointer to place data is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_INITIALIZED       - callback not set
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssXsmiPortGroupRegisterRead
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  CPSS_PHY_XSMI_INTERFACE_ENT xsmiInterface,
    IN  GT_U32  xsmiAddr,
    IN  GT_U32  regAddr,
    IN  GT_U32  phyDev,
    OUT GT_U16  *dataPtr
)
{
    GT_U32  value = 0;      /* value to write into the register */
    GT_U32  portGroupId;    /*the port group Id - support multi-port-groups device */
    GT_U32  ctrlRegAddr;    /* address of XSMI ctrl reg. */
    GT_U32  addrRegAddr;    /* address of XSMI addr reg. */
    GT_STATUS rc;

    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    CPSS_NULL_PTR_CHECK_MAC(dataPtr);

    if(phyDev >= BIT_5 || xsmiAddr >= BIT_5)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(NULL == PRV_CPSS_PP_MAC(devNum)->phyInfo.genXSmiBindFunc.cpssPhyXSmiRegsAddrGet)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);

    if((rc = PRV_CPSS_PP_MAC(devNum)->phyInfo.genXSmiBindFunc.
                cpssPhyXSmiRegsAddrGet(devNum, xsmiInterface, &ctrlRegAddr, 
                                       &addrRegAddr)) != GT_OK)
    {
        return rc;
    }

    if(0 == PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {   /* for non multi-port groups device the MACRO assign value */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    }

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    if((rc = xsmiStatusCheck(devNum, portGroupId, xsmiInterface, GT_FALSE)) != GT_OK)
        return rc;

    /* write the address to be used in the read operation */
    if (prvCpssHwPpPortGroupSetRegField(devNum, portGroupId, addrRegAddr,
            0, 16, regAddr) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    /* write address and read opCode to Control-Register */
    /* set port address */
    value |= (xsmiAddr << 16);

    /* set device address */
    value |= (phyDev << 21);

    /* set read opcode */
    value |= (((GT_U32)7) << 26);

    if (prvCpssHwPpPortGroupWriteRegister(devNum, portGroupId, ctrlRegAddr, value) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    /* check if read operation has finished */
    if((rc = xsmiStatusCheck(devNum, portGroupId, xsmiInterface, GT_TRUE)) != GT_OK)
        return rc;

    if (prvCpssHwPpPortGroupReadRegister(devNum, portGroupId, ctrlRegAddr, &value) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    *dataPtr = (GT_U16) (value & 0xFFFF);

    return GT_OK;
}

/*******************************************************************************
* cpssXsmiPortGroupRegisterRead
*
* DESCRIPTION:
*       Read value of a specified XSMI Register.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       xsmiInterface - XSMI instance
*       xsmiAddr    - address of configurated device on XSMI (APPLICABLE RANGES: 0..31)
*       regAddr     - address of register of configurated device
*       phyDev      - the PHY device to read from (APPLICABLE RANGES: 0..31).
*
* OUTPUTS:
*       dataPtr - (Pointer to) the read data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameter.
*       GT_BAD_PTR               - pointer to place data is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_INITIALIZED       - callback not set
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssXsmiPortGroupRegisterRead
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  CPSS_PHY_XSMI_INTERFACE_ENT xsmiInterface,
    IN  GT_U32  xsmiAddr,
    IN  GT_U32  regAddr,
    IN  GT_U32  phyDev,
    OUT GT_U16  *dataPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssXsmiPortGroupRegisterRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, xsmiInterface, xsmiAddr, regAddr, phyDev, dataPtr));

    rc = internal_cpssXsmiPortGroupRegisterRead(devNum, portGroupsBmp, xsmiInterface, xsmiAddr, regAddr, phyDev, dataPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, xsmiInterface, xsmiAddr, regAddr, phyDev, dataPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssXsmiRegisterWrite
*
* DESCRIPTION:
*       Write value to a specified XSMI Register.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       xsmiInterface - XSMI instance
*       xsmiAddr    - address of configurated device on XSMI (APPLICABLE RANGES: 0..31)
*       regAddr     - address of register of configurated device
*       phyDev      - the PHY device to write to (APPLICABLE RANGES: 0..31).
*       data        - data to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameter.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_INITIALIZED       - callback not set
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssXsmiRegisterWrite
(
    IN  GT_U8   devNum,
    IN  CPSS_PHY_XSMI_INTERFACE_ENT xsmiInterface,
    IN  GT_U32  xsmiAddr,
    IN  GT_U32  regAddr,
    IN  GT_U32  phyDev,
    IN  GT_U16  data
)
{
    return cpssXsmiPortGroupRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                          xsmiInterface, xsmiAddr, regAddr, 
                                          phyDev, data);
}

/*******************************************************************************
* cpssXsmiRegisterWrite
*
* DESCRIPTION:
*       Write value to a specified XSMI Register.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       xsmiInterface - XSMI instance
*       xsmiAddr    - address of configurated device on XSMI (APPLICABLE RANGES: 0..31)
*       regAddr     - address of register of configurated device
*       phyDev      - the PHY device to write to (APPLICABLE RANGES: 0..31).
*       data        - data to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameter.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_INITIALIZED       - callback not set
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssXsmiRegisterWrite
(
    IN  GT_U8   devNum,
    IN  CPSS_PHY_XSMI_INTERFACE_ENT xsmiInterface,
    IN  GT_U32  xsmiAddr,
    IN  GT_U32  regAddr,
    IN  GT_U32  phyDev,
    IN  GT_U16  data
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssXsmiRegisterWrite);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, xsmiInterface, xsmiAddr, regAddr, phyDev, data));

    rc = internal_cpssXsmiRegisterWrite(devNum, xsmiInterface, xsmiAddr, regAddr, phyDev, data);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, xsmiInterface, xsmiAddr, regAddr, phyDev, data));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssXsmiRegisterRead
*
* DESCRIPTION:
*       Read value of a specified XSMI Register.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       xsmiInterface - XSMI instance
*       xsmiAddr    - address of configurated device on XSMI (APPLICABLE RANGES: 0..31)
*       regAddr     - address of register of configurated device
*       phyDev      - the PHY device to read from (APPLICABLE RANGES: 0..31).
*
* OUTPUTS:
*       dataPtr - (Pointer to) the read data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameter.
*       GT_BAD_PTR               - pointer to place data is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_INITIALIZED       - callback not set
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssXsmiRegisterRead
(
    IN  GT_U8   devNum,
    IN  CPSS_PHY_XSMI_INTERFACE_ENT xsmiInterface,
    IN  GT_U32  xsmiAddr,
    IN  GT_U32  regAddr,
    IN  GT_U32  phyDev,
    OUT GT_U16  *dataPtr
)
{
    return cpssXsmiPortGroupRegisterRead(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                         xsmiInterface, xsmiAddr, regAddr, phyDev, 
                                         dataPtr);
}

/*******************************************************************************
* cpssXsmiRegisterRead
*
* DESCRIPTION:
*       Read value of a specified XSMI Register.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       xsmiInterface - XSMI instance
*       xsmiAddr    - address of configurated device on XSMI (APPLICABLE RANGES: 0..31)
*       regAddr     - address of register of configurated device
*       phyDev      - the PHY device to read from (APPLICABLE RANGES: 0..31).
*
* OUTPUTS:
*       dataPtr - (Pointer to) the read data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameter.
*       GT_BAD_PTR               - pointer to place data is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_INITIALIZED       - callback not set
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssXsmiRegisterRead
(
    IN  GT_U8   devNum,
    IN  CPSS_PHY_XSMI_INTERFACE_ENT xsmiInterface,
    IN  GT_U32  xsmiAddr,
    IN  GT_U32  regAddr,
    IN  GT_U32  phyDev,
    OUT GT_U16  *dataPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssXsmiRegisterRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, xsmiInterface, xsmiAddr, regAddr, phyDev, dataPtr));

    rc = internal_cpssXsmiRegisterRead(devNum, xsmiInterface, xsmiAddr, regAddr, phyDev, dataPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, xsmiInterface, xsmiAddr, regAddr, phyDev, dataPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/* ----------------- debug functions --------------------------- */
#define PRV_CPSS_GEN_SMI_DBG_PHY_PAGE_REG_ADDR_CNS 22 /* PHY page number register */
#define PRV_CPSS_GEN_SMI_DBG_PHY_ID_1_REG_ADDR_CNS 2 /* PHY Identifier 1 register */
#define PRV_CPSS_GEN_SMI_DBG_PHY_ID_2_REG_ADDR_CNS 3 /* PHY Identifier 2 register */
GT_VOID prvCpssGenSmiDebugSmiScan
(
    IN  GT_U8   devNum
)
{
    GT_STATUS   rc; /* return code */
    GT_U32      portGroupId; /* local core number */
    GT_PORT_GROUPS_BMP  portGroupsBmp; /* port groups bitmap */
    CPSS_PHY_SMI_INTERFACE_ENT   smiInterface;  /* SMI i/f iterator */
    GT_U32      smiAddr;    /* SMI Address iterator */
    GT_U32      regAddr;    /* PHY register address */
    GT_U16      data=0;       /* register data */

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        portGroupsBmp = 1<<portGroupId;
        for(smiInterface = CPSS_PHY_SMI_INTERFACE_0_E; 
             smiInterface < CPSS_PHY_SMI_INTERFACE_MAX_E; smiInterface++)
        {
            for(smiAddr = 0; smiAddr < 16; smiAddr++)
            {
                /* set PHY page 0 */
                regAddr = PRV_CPSS_GEN_SMI_DBG_PHY_PAGE_REG_ADDR_CNS;
                rc = cpssSmiRegisterWriteShort(devNum, portGroupsBmp, 
                                               smiInterface, smiAddr, regAddr, 0);
                if(rc != GT_OK)
                {
                    cpssOsPrintf("cpssSmiRegisterWriteShort FAIL:devNum=%d,\
portGroupId=%d,smiInterface=%d,smiAddr=%d,\
regAddr=%d,data=%d\n", 
                                 devNum, portGroupId, smiInterface, smiAddr, regAddr, 0);
                    continue;
                }
                /* read PHY ID 1 */
                regAddr = PRV_CPSS_GEN_SMI_DBG_PHY_ID_1_REG_ADDR_CNS;
                rc = cpssSmiRegisterReadShort(devNum, portGroupsBmp, smiInterface,
                                              smiAddr, regAddr, &data);
                if(rc != GT_OK)
                {
                    cpssOsPrintf("cpssSmiRegisterReadShort FAIL:devNum=%d,\
portGroupId=%d,smiInterface=%d,smiAddr=%d,\
regAddr=%d,data=%d\n", 
                                 devNum, portGroupId, smiInterface, smiAddr, regAddr);
                    continue;
                }
                cpssOsPrintf("cpssSmiRegisterReadShort:devNum=%d,\
portGroupId=%d,smiInterface=%d,smiAddr=%d,\
PHY_ID_1=0x%x\n", 
                             devNum, portGroupId, smiInterface, smiAddr, data);
                /* read PHY ID 2 */
                regAddr = PRV_CPSS_GEN_SMI_DBG_PHY_ID_2_REG_ADDR_CNS;
                rc = cpssSmiRegisterReadShort(devNum, portGroupsBmp, smiInterface,
                                              smiAddr, regAddr, &data);
                if(rc != GT_OK)
                {
                    cpssOsPrintf("cpssSmiRegisterReadShort FAIL:devNum=%d,\
portGroupId=%d,smiInterface=%d,smiAddr=%d,\
regAddr=%d,data=%d\n", 
                                 devNum, portGroupId, smiInterface, smiAddr, regAddr);
                    continue;
                }
                cpssOsPrintf("cpssSmiRegisterReadShort:devNum=%d,\
portGroupId=%d,smiInterface=%d,smiAddr=%d,\
PHY_ID_2=0x%x\n", 
                             devNum, portGroupId, smiInterface, smiAddr, data);
            }
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    return;
}



