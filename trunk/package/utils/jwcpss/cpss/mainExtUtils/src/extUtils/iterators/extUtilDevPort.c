/*******************************************************************************
*              (c), Copyright 2014, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* extUtilDevPort.c
*
* DESCRIPTION:
*       Device/Port common functions
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <extUtils/iterators/extUtilDevPort.h>

#ifdef CHX_FAMILY
    #include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
    #include <cpss/dxCh/dxChxGen/config/cpssDxChCfgInit.h>
    #include <cpss/dxCh/dxChxGen/port/cpssDxChPortMapping.h>
    #include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
    #include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfoEnhanced.h>
#endif /*CHX_FAMILY*/
#ifdef EXMXPM_FAMILY
#include <cpss/exMxPm/exMxPmGen/cpssHwInit/cpssExMxPmHwInit.h>
#include <cpss/exMxPm/exMxPmGen/config/cpssExMxPmCfg.h>
#include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmInfo.h>
#endif /*EXMXPM_FAMILY*/

/*******************************************************************************
* extUtilHwDeviceNumberGet
*
* DESCRIPTION:
*       Return Hardware device Id
*
* INPUTS:
*       swDevNum    - software number
*
* OUTPUTS:
*       hwDevPtr    - HW device id
*
* RETURNS:
*       GT_OK on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extUtilHwDeviceNumberGet
(
  IN  GT_U32              swDevNum,
  OUT GT_HW_DEV_NUM       *hwDevPtr
)
{
#if (defined CHX_FAMILY) || (defined EXMXPM_FAMILY)
    GT_STATUS rc;   /* return code */
    GT_U8     tmpDev=0;

    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(hwDevPtr);

    if(!PRV_CPSS_IS_DEV_EXISTS_MAC(swDevNum))
    {
        /* all cases that use this function need to convert SW to HW value
           and since we not know the 'remote device' we will do no convert
           so HW devNum == SW devNum
        */
        *hwDevPtr = swDevNum;
        return GT_OK;
    }

#if (defined CHX_FAMILY)
    tmpDev = tmpDev;
    rc = cpssDxChCfgHwDevNumGet((GT_U8)swDevNum, hwDevPtr);
#else
    rc = cpssExMxPmPpHwDevNumGet((GT_U8)swDevNum, &tmpDev);
    *hwDevPtr = tmpDev;
#endif

    return rc;

#else
    swDevNum = swDevNum;
    hwDevPtr = hwDevPtr;

    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* extUtilSwDeviceNumberGet
*
* DESCRIPTION:
*       Return software device numver by hardware device id
*
* INPUTS:
*       hwDev       - HW number
*
* OUTPUTS:
*       swDevNumPtr - software number
*
* RETURNS:
*       GT_OK on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extUtilSwDeviceNumberGet
(
  IN  GT_HW_DEV_NUM       hwDev,
  OUT GT_U32              *swDevNumPtr
)
{
#if (defined CHX_FAMILY) || (defined EXMXPM_FAMILY)
    GT_STATUS rc;   /* return code */
    GT_U8   i;      /* loop index */
    GT_HW_DEV_NUM   localHwDev; /* in loop variable for getting HW IDs */
    GT_U8           tmpDev = 0;

    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(swDevNumPtr);

    for(i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        if(PRV_CPSS_PP_MAC(i) == NULL)
        {
            continue;
        }

#if (defined CHX_FAMILY)
        tmpDev = tmpDev;
        rc = cpssDxChCfgHwDevNumGet(i, &localHwDev);
#else
        rc = cpssExMxPmPpHwDevNumGet(i, &tmpDev);
        localHwDev = tmpDev;
#endif

        if(rc != GT_OK)
        {
            return rc;
        }

        if( localHwDev == hwDev )
        {
            *swDevNumPtr = i;
            return GT_OK;
        }
    }

    *swDevNumPtr = hwDev; /* no conversion done */
    return GT_OK;

#else
    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(swDevNumPtr);

    hwDev = hwDev;
    *swDevNumPtr = devPtr;

    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* extUtilDoesDeviceExists
*
* DESCRIPTION:
*       Return true if device exists (configured)
*
* INPUTS:
*       devNum          - device ID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE if device exist, GT_FALSE otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_BOOL extUtilDoesDeviceExists
(
  IN  GT_U8         devNum
)
{
    return PRV_CPSS_IS_DEV_EXISTS_MAC(devNum) ? GT_TRUE : GT_FALSE;
}

/*******************************************************************************
* extUtilDoesPortExists
*
* DESCRIPTION:
*       Return true if port exists
*
* INPUTS:
*       devNum          - device ID
*       portNum         - port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE if port exist, GT_FALSE otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_BOOL extUtilDoesPortExists
(
  IN  GT_U8         devNum,
  IN  GT_PORT_NUM   portNum
)
{
    if (!PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
        return GT_FALSE;

#if (defined CHX_FAMILY)
    if (CPSS_IS_DXCH_FAMILY_MAC(PRV_CPSS_PP_MAC(devNum)->devFamily))
    {
        if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum))
        {
            if (portNum < PRV_CPSS_DXCH_MAX_PHY_PORT_NUMBER_MAC(devNum))
            {
                GT_BOOL isValid;
                if (cpssDxChPortPhysicalPortMapIsValidGet(devNum, portNum, &isValid) != GT_OK)
                    return GT_FALSE;
                return isValid;
            }
            /* portNum >= 256 */
            if (portNum <= PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_PORT_MAC(devNum))
                return GT_TRUE;
        }
        else
        {
            /* !eArch enabled */
            if (CPSS_PORTS_BMP_IS_PORT_SET_MAC(&PRV_CPSS_PP_MAC(devNum)->existingPorts , portNum))
                return GT_TRUE;

        }
    }
#endif
#if defined(EXMXPM_FAMILY)
    if (CPSS_IS_EXMXPM_FAMILY_MAC(PRV_CPSS_PP_MAC(devNum)->devFamily))
    {
        if (CPSS_PORTS_BMP_IS_PORT_SET_MAC(&PRV_CPSS_PP_MAC(devNum)->existingPorts , portNum))
            return GT_TRUE;
    }
#endif
    return GT_FALSE;
}

/*******************************************************************************
* extUtilDoesHwPortExists
*
* DESCRIPTION:
*       Return true if physical port exists
*
* INPUTS:
*       hwDevNum        - HW number
*       portNum         - port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE if port exist, GT_FALSE otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_BOOL extUtilDoesHwPortExists
(
  IN  GT_HW_DEV_NUM hwDevNum,
  IN  GT_PORT_NUM   portNum
)
{
    GT_U32  swDevNum;
    if (extUtilSwDeviceNumberGet(hwDevNum, &swDevNum) != GT_OK)
        return GT_FALSE;
    if (!PRV_CPSS_IS_DEV_EXISTS_MAC(swDevNum))
        return GT_FALSE;

    if (CPSS_PORTS_BMP_IS_PORT_SET_MAC(&PRV_CPSS_PP_MAC(swDevNum)->existingPorts , portNum))
        return GT_TRUE;

    return GT_FALSE;
}

