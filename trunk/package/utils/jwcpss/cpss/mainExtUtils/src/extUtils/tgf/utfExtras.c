/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvUtfExtras.c
*
* DESCRIPTION:
*       defines API for helpers functions
*       which are specific for cpss unit testing.
*
* FILE REVISION NUMBER:
*       $Revision: 1
*
*******************************************************************************/

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#if (defined EX_FAMILY) || (defined MX_FAMILY)
#define CALL_EX_MX_CODE_MAC
#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>
#endif /* (defined EX_FAMILY) || (defined MX_FAMILY) */

#ifdef CHX_FAMILY
    #ifndef CALL_EX_MX_CODE_MAC
        /*the include to H file of exmx clash with the h file of the dxch
            (about definition of _txPortRegs)*/
        #include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
        #define CALL_DXCH_CODE_MAC
    #endif /*CALL_EX_MX_CODE_MAC*/
    #include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
    #include <cpss/dxCh/dxChxGen/config/cpssDxChCfgInit.h>
#endif /*CHX_FAMILY*/

#ifdef EXMXPM_FAMILY
#include <cpss/exMxPm/exMxPmGen/cpssHwInit/cpssExMxPmHwInit.h>
#include <cpss/exMxPm/exMxPmGen/config/cpssExMxPmCfg.h>
#include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmInfo.h>
#endif /*EXMXPM_FAMILY*/

#include <extUtils/tgf/prvUtfExtras.h>

/*******************************************************************************
* prvUtfHwDeviceNumberSet
*
* DESCRIPTION:
*       Set HW device number from the SW device number
*
* INPUTS:
*       dev      - SW device number
*       hwDev    - new HW device number
*
* OUTPUTS:

*
* RETURNS:
*       GT_OK           -   Set revision of device OK
*       GT_BAD_PARAM    -   Invalid device id
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfHwDeviceNumberSet
(
    IN GT_U8               dev,
    IN GT_HW_DEV_NUM       hwDev
)
{
#if (defined CHX_FAMILY)
    return  cpssDxChCfgHwDevNumSet(dev, hwDev);

#elif (defined EXMXPM_FAMILY)
    return cpssExMxPmPpHwDevNumSet(dev, (GT_U8)hwDev);

#else
    TGF_PARAM_NOT_USED(dev);
    TGF_PARAM_NOT_USED(hwDev);
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvUtfHwDeviceNumberGet
*
* DESCRIPTION:
*       Get HW device number from the SW device number
*
* INPUTS:
*       swDevNum    - SW device number
*
* OUTPUTS:
*       hwDevPtr    - (pointer to)HW device number
*
* RETURNS:
*       GT_OK           -   Get revision of device OK
*       GT_BAD_PARAM    -   Invalid device id
*       GT_BAD_PTR      -   Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfHwDeviceNumberGet
(
    IN  GT_U32              swDevNum,
    OUT GT_HW_DEV_NUM       *hwDevPtr
)
{
#if (defined CHX_FAMILY) || (defined EXMXPM_FAMILY)
    GT_STATUS rc;   /* return code */
    GT_U8     tmpDev=0;

    if(!PRV_CPSS_IS_DEV_EXISTS_MAC(swDevNum))
    {
        /* all cases that use this function need to convert SW to HW value
           and since we not know the 'remote device' we will do no convert
           so HW devNum == SW devNum
        */
        *hwDevPtr = swDevNum;
        return GT_OK;
    }

    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(hwDevPtr);

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
* prvUtfSwFromHwDeviceNumberGet
*
* DESCRIPTION:
*       Get SW device number from the HW device number
*
* INPUTS:
*       hwDev   - HW device number
*
* OUTPUTS:
*       devPtr  - (pointer to)SW device number
*
* RETURNS:
*       GT_OK           -   Get revision of device OK
*       GT_BAD_PARAM    -   Invalid device id
*       GT_BAD_PTR      -   Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfSwFromHwDeviceNumberGet
(
    IN GT_HW_DEV_NUM    hwDev,
    OUT GT_U32          *devPtr
)
{
#if (defined CHX_FAMILY) || (defined EXMXPM_FAMILY)
    GT_STATUS rc;   /* return code */
    GT_U8   i;      /* loop index */
    GT_HW_DEV_NUM   localHwDev; /* in loop variable for getting HW IDs */
    GT_U8           tmpDev = 0;

    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(devPtr);

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
            *devPtr = i;
            return GT_OK;
        }
    }

    *devPtr = hwDev; /* no conversion done */
    return GT_OK;

#else
    hwDev = hwDev;
    devPtr = devPtr;

    return GT_BAD_STATE;
#endif
}
/*******************************************************************************
* prvUtfIsGmCompilation
*
* DESCRIPTION:
*    Check if the image was compiled with 'GM_USED' (Golden model - simulation).
*    the 'GM' is 'heavy' and runtime with low performance.
*    so long iterations should be shorten...
* INPUTS:
*       none
*
* RETURNS:
*       GT_TRUE - the image was compiled with 'GM_USED'
*       GT_FALSE - the image was NOT compiled with 'GM_USED'
*
* COMMENTS:
*
*
*******************************************************************************/
GT_BOOL prvUtfIsGmCompilation
(
    void
)
{
#ifdef GM_USED
    return GT_TRUE;
#else
    return GT_FALSE;
#endif
}

