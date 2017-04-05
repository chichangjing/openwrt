/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenUtils.c
*
* DESCRIPTION:
*       CPSS generic wrappers for serdes optimizer and port configuration
*           black box
*
* FILE REVISION NUMBER:
*       $Revision: 12 $
*******************************************************************************/

#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>
#include <cpss/generic/cpssHwInit/private/prvCpssHwRegisters.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>

#include <private/mvHwsPortPrvIf.h>

GT_STATUS gtStatus;

/*******************************************************************************
* genRegisterSet
*
* DESCRIPTION:
*       Writes the unmasked bits of a register.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - PP device number to write to.
*       portGroup   - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       address     - Register address to write to.
*       data        - Data to be written to register.
*       mask        - Mask for selecting the written bits.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       The bits in value to be written are the masked bit of 'mask'.
*
*******************************************************************************/
GT_STATUS genRegisterSet
(
    IN GT_U8 devNum,
    IN GT_U32 portGroup,
    IN GT_U32 address,
    IN GT_U32 data,
    IN GT_U32 mask
)
{
    #ifdef EXMXPM_FAMILY
        return prvCpssHwPpPortGroupWriteRegBitMask(devNum,portGroup,address,
                                        (mask != 0) ? mask : 0xffffffff,data);
    #else
        return cpssDrvPpHwRegBitMaskWrite(devNum,portGroup,address,
                                        (mask != 0) ? mask : 0xffffffff,data);
    #endif
}

/*******************************************************************************
* genRegisterGet
*
* DESCRIPTION:
*       Reads the unmasked bits of a register.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - PP device number to read from.
*       portGroup - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       address     - Register address to read from.
*       mask        - Mask for selecting the read bits.
*
* OUTPUTS:
*       data    - Data read from register.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       The bits in value to be read are the masked bit of 'mask'.
*
*******************************************************************************/
GT_STATUS genRegisterGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  portGroup,
    IN  GT_U32  address,
    OUT GT_U32  *data,
    IN  GT_U32  mask
)
{
    CPSS_NULL_PTR_CHECK_MAC(data);

    #ifdef EXMXPM_FAMILY
            return prvCpssHwPpPortGroupReadRegBitMask(devNum,portGroup,address,
                                    (mask != 0) ? mask : 0xffffffff,data);
    #else
        return cpssDrvPpHwRegBitMaskRead(devNum,portGroup,address,
                                     (mask != 0) ? mask : 0xffffffff,data);
    #endif
}

/*******************************************************************************
* gtBreakOnFail
*
* DESCRIPTION:
*       Treat failure
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
*******************************************************************************/
GT_VOID gtBreakOnFail
(
        GT_VOID
)
{
}

/*******************************************************************************
* prvCpssGenCpssDevType2HwsTranslate
*
* DESCRIPTION:
*       Translate device family and revision to HWS type
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       device type in HWS format (GT_U32 used from internal path definition
*                                   considerations)
*
*******************************************************************************/
GT_U32 prvCpssGenCpssDevType2HwsTranslate
(
    GT_U8   devNum
)
{
    GT_BOOL isLion2;/* is Lion2 indication */

    PRV_CPSS_GEN_PP_CONFIG_STC *devPtr = PRV_CPSS_PP_MAC(devNum);/* pointer to common device info */
    CPSS_PP_FAMILY_TYPE_ENT devFamily = devPtr->devFamily;/* device family */
    CPSS_PP_DEVICE_TYPE     devType   = devPtr->devType;  /* device type*/
    GT_U32                  revision  = devPtr->revision; /* device revision */
    CPSS_PP_SUB_FAMILY_TYPE_ENT devSubFamily = devPtr->devSubFamily; /* device sub family */

    if(CPSS_PP_FAMILY_DXCH_LION2_E == devFamily)
    {
        isLion2 = GT_TRUE;
    }
    else
    {
        isLion2 = GT_FALSE;
    }

    if(isLion2 == GT_TRUE)
    {
        if(0 == revision)
        {
            return Lion2A0;
        }
        else
        {
            switch(devType)
            {
                case CPSS_LION2_HOOPER_PORT_GROUPS_0123_DEVICES_CASES_MAC:
                    return HooperA0;
                default:
                    /* Lion2B0, Lion2B1 and above */
                    return Lion2B0;
            }
        }
    }
    else if(CPSS_PP_FAMILY_PUMA3_E == devFamily)
    {
        /* Puma3A0 and above */
        if(0 == revision)
        {
            return Puma3A0;
        }
        else
        {
            return Puma3B0;
        }
    }
    else if(CPSS_PP_FAMILY_DXCH_BOBCAT2_E == devFamily ||
            CPSS_PP_FAMILY_DXCH_BOBCAT3_E == devFamily ||
            CPSS_PP_FAMILY_DXCH_ALDRIN_E == devFamily)
    {
        if((devFamily    == CPSS_PP_FAMILY_DXCH_BOBCAT2_E) &&
           (devSubFamily == CPSS_PP_SUB_FAMILY_BOBCAT2_BOBK_E))
        {
            /*Cetus and Caelum*/
            return BobK;
        }
        else /*bc2,bc3,aldrin*/
        {
            /*BC2 A0 and B0*/
            return BobcatA0;
        }
    }
    else if(CPSS_PP_FAMILY_DXCH_XCAT3_E == devFamily)
    {
        return Alleycat3A0;
    }

    return CPSS_MAX_FAMILY;
}

/*******************************************************************************
* prvCpssGenPortGroupSerdesWriteRegBitMask
*
* DESCRIPTION:
*       Implement write access to SERDES external/internal registers.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group number
*       regType     - internal/external
*       serdesNum   - seredes number to access
*       regAddr     - serdes register address (offset) to access
*       data        - data to write
*       mask        - mask write
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_BAD_PARAM - on wrong parameter
*
*******************************************************************************/
GT_STATUS prvCpssGenPortGroupSerdesWriteRegBitMask
(
    IN  GT_U8  devNum,
    IN  GT_U8  portGroup,
    IN  GT_U8  regType,
    IN  GT_U8  serdesNum,
    IN  GT_U32 regAddr,
    IN  GT_U32 data,
    IN  GT_U32 mask
)
{
    GT_U32  realRegAddr;
    GT_U32  baseAddr;

    if(regType > 1)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    baseAddr = (0 == regType) ? 0x13000000 : 0x13000800;

    realRegAddr = baseAddr + regAddr + 0x1000*serdesNum;

#ifdef CHX_FAMILY
    if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT2_E) &&
       (PRV_CPSS_PP_MAC(devNum)->devSubFamily == CPSS_PP_SUB_FAMILY_BOBCAT2_BOBK_E))
    {
        if(serdesNum > 11)
        {
            realRegAddr = 0x13000000 + regAddr + 0x1000*serdesNum + 0x40000;
        }
        else
        {
            realRegAddr = 0x13000000 + regAddr + 0x1000*serdesNum;
        }
    }
#endif
    return genRegisterSet(devNum,portGroup,realRegAddr,data,mask);
}
/* bobcat3 pipe1 address indication */
#define BOBCAT3_PIPE_1_INDICATION_CNS 0x80000000

/*******************************************************************************
* prvCpssGenPortGroupSerdesReadRegBitMask
*
* DESCRIPTION:
*       Implement read access from SERDES external/internal registers.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group number
*       regType     - internal/external
*       serdesNum   - seredes number to access
*       regAddr     - serdes register address (offset) to access
*       mask        - read mask
*
* OUTPUTS:
*       dataPtr     - read data
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS prvCpssGenPortGroupSerdesReadRegBitMask
(
    IN  GT_U8  devNum,
    IN  GT_U8  portGroup,
    IN  GT_U8  regType,
    IN  GT_U8  serdesNum,
    IN  GT_U32 regAddr,
    IN  GT_U32 *dataPtr,
    IN  GT_U32 mask
)
{
    GT_U32  realRegAddr;
    GT_U32  baseAddr;
    GT_U32  offset;

    if(regType > 1)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    baseAddr = (0 == regType) ? 0x13000000 : 0x13000800;

    realRegAddr = baseAddr + regAddr + 0x1000*serdesNum;

    if(PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))/* use AVAGO SERDES */
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT3_E ||
           PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_ALDRIN_E)
        {
            offset = (serdesNum < 72) ? serdesNum % 36 : 36/*72,73*/;
            realRegAddr = 0x13000000 + regAddr + 0x1000*offset;
            if(serdesNum >= 36 && serdesNum != 72)
            {
                /* pipe 1 */
                realRegAddr |= BOBCAT3_PIPE_1_INDICATION_CNS;
            }
        }
        else /* Bobk */
        {
            if(serdesNum > 11)
            {
                realRegAddr = 0x13000000 + regAddr + 0x1000*serdesNum + 0x40000;
            }
            else
            {
                realRegAddr = 0x13000000 + regAddr + 0x1000*serdesNum;
            }
        }
    }

    return genRegisterGet(devNum,portGroup,realRegAddr,dataPtr,mask);
}



