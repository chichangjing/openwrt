/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpIntInit.c
*
* DESCRIPTION:
*       Includes driver level interrupts initialization code.
*
* FILE REVISION NUMBER:
*       $Revision: 35 $
*
*******************************************************************************/

#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterruptsInit.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvDxExMxInterrupts.h>
#include <cpssDriver/pp/hardware/prvCpssDrvObj.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>

/*******************************************************************************
 * Local variables
 ******************************************************************************/
PRV_CPSS_DRV_DEVICE_INTERRUPTS_INFO_STC prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_LAST_E]={
/*Twist-C*/
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_TWISTC_12_E,   */
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_TWISTC_52_E,   */

/*Twist-D*/
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_TWISTD_XG_E,   */
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_TWISTD_12_E,   */
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_TWISTD_52_E,   */

/*Samba*/
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_SAMBA_XG_E,    */
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_SAMBA_12_E,    */
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_SAMBA_52_E,    */

/*Salsa*/
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_SALSA_12_E,    */
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_SALSA_16_E,    */
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_SALSA_24_E,    */

/*Cheetah,cheetah2, cheetah3*/
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_CHEETAH_E,                                        */
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_CHEETAH_SMI_E,  SMI                               */
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_CHEETAH_SMI_106_E,  SMI - Dx106                   */
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_CHEETAH_HX_SMI_E,  SMI - DX249/269                */
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_CHEETAH_HX_PCI_E,  PCI - DX249/269                */
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_CHEETAH2_E,                                       */
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_CHEETAH2_SMI_E, SMI                               */
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_CHEETAH3_E,        PEX - ports [0:23] + XG [24:27]*/
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_CHEETAH3_SMI_E,    SMI - ports [0:23] + XG [24:27]*/

/*DXCH - xCat */
   {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_DXCH_XCAT_REV0_E,    xCat rev0, PEX - ports [0:23] + XG [24:27]  */
   {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_DXCH_XCAT_E,         xCat, PEX - ports [0:23] + XG [24:27]       */
   {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_DXCH_XCAT3_E,        xCat3 - ports [0:23] + XG [24:27]           */
   {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_DXCH_XCAT2_E,        xCat2, PEX/MBUS - ports [0:23] + Gig [24:27]*/

/*Tiger*/
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_TIGER_XG_E,*/
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_TIGER_12_E,*/
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_TIGER_52_E,*/

/*Puma*/
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_PUMA_E,    */

/*Lion */
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_DXCH_LION_PORT_GROUP_ID_0_E, Lion, portGroupId 0 - PEX 12 GE/XG ports */
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_DXCH_LION_PORT_GROUP_ID_1_E, Lion, portGroupId 1 - PEX 12 GE/XG ports */
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_DXCH_LION_PORT_GROUP_ID_2_E, Lion, portGroupId 2 - PEX 12 GE/XG ports */
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_DXCH_LION_PORT_GROUP_ID_3_E, Lion, portGroupId 3 - PEX 12 GE/XG ports */

/*Lion 2/3 */
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_DXCH_LION_PORT_GROUP_ID_0_E, Lion, portGroupId 0 - PEX 12 GE/XG ports */
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_DXCH_LION_PORT_GROUP_ID_1_E, Lion, portGroupId 1 - PEX 12 GE/XG ports */
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_DXCH_LION_PORT_GROUP_ID_2_E, Lion, portGroupId 2 - PEX 12 GE/XG ports */
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_DXCH_LION_PORT_GROUP_ID_3_E, Lion, portGroupId 3 - PEX 12 GE/XG ports */
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_DXCH_LION_PORT_GROUP_ID_4_E, Lion, portGroupId 4 - PEX 12 GE/XG ports */
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_DXCH_LION_PORT_GROUP_ID_5_E, Lion, portGroupId 5 - PEX 12 GE/XG ports */
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_DXCH_LION_PORT_GROUP_ID_6_E, Lion, portGroupId 6 - PEX 12 GE/XG ports */
    {0,NULL,NULL,0,NULL,0,NULL,GT_FALSE,0,NULL,0,NULL,NULL},  /*PRV_CPSS_DRV_FAMILY_DXCH_LION_PORT_GROUP_ID_7_E, Lion, portGroupId 7 - PEX 12 GE/XG ports */
};

/*******************************************************************************
 * Forward functions declarations
 ******************************************************************************/
#define MAX_LEVELS_CNS  16
/*******************************************************************************
* gtDrvIntEnable
*
* DESCRIPTION:
*       Enable interrupts for the given device.
*
* INPUTS:
*       devNum      - The device number to initialize the interrupts mechanism
*                     for.
*       portGroupId      - the portGroupId
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - on failure.
*       GT_NOT_INITIALIZED - if the driver was not initialized
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS drvInterruptsEnable
(
    IN  GT_U8   devNum,
    IN  GT_U32  portGroupId
)
{
    GT_STATUS   retVal;     /* Function's return value. */

    PRV_CPSS_DRV_CHECK_GEN_FUNC_OBJ_MAC(devNum);

    retVal = prvCpssDrvPpObjConfig[devNum]->genPtr->drvHwPpInitInMasks(devNum,portGroupId,GT_TRUE);

    return retVal;
}


/*******************************************************************************
* drvInterruptsEnableFakeMaskSet
*
* DESCRIPTION:
*       Fake interrupts do not need unmasking, just return TRUE.
*
* INPUTS:
*       evReqNode   - The interrupt node representing the interrupt to be
*                     unmasked.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success,
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS drvInterruptsEnableFakeMaskSet
(
    IN PRV_CPSS_DRV_EV_REQ_NODE_STC *evReqNode,
    IN CPSS_EVENT_MASK_SET_ENT      operation
)
{
    /* fix warnings */
    evReqNode = evReqNode;
    operation = operation;

    return GT_OK;
}

/*******************************************************************************
* drvInterruptsInfoGet
*
* DESCRIPTION:
*       get pointer to the interrupt tree info according to the device type
*
* INPUTS:
*       devNum          - The device number to get the interrupts tree info.
*       portGroupId          - The port group Id. relevant only to multi port group devices
*
* OUTPUTS:
*       devIntPtrPtr    - (pointer to)pointer to the interrupts tree of the
*                       device
*
* RETURNS:
*       GT_OK   - on success,
*       GT_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS drvInterruptsInfoGet
(
    IN  GT_U8           devNum,
    IN  GT_U32          portGroupId,
    OUT PRV_CPSS_DRV_DEVICE_INTERRUPTS_INFO_STC **devIntPtrPtr
)
{
    PRV_CPSS_DRV_DEVICE_INTERRUPTS_INFO_STC *devIntPtr = NULL;

    switch (prvCpssDrvPpConfig[devNum]->devFamily)
    {
        case CPSS_PP_FAMILY_TWISTC_E:
            switch(prvCpssDrvPpConfig[devNum]->numOfPorts)
            {
                case 10:
                case 12:
                    devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_TWISTC_12_E];
                    break;
                case 52:
                    devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_TWISTC_52_E];
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }
            break;
        case CPSS_PP_FAMILY_TWISTD_E:
            switch(prvCpssDrvPpConfig[devNum]->numOfPorts)
            {
                case 1:
                    devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_TWISTD_XG_E];
                    break;
                case 12:
                    devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_TWISTD_12_E];
                    break;
                case 52:
                    devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_TWISTD_52_E];
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }
            break;

        case CPSS_PP_FAMILY_SAMBA_E:
            switch(prvCpssDrvPpConfig[devNum]->numOfPorts)
            {
                case 1:
                    devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_SAMBA_XG_E];
                    break;
                case 12:
                    devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_SAMBA_12_E];
                    break;
                case 52:
                    devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_SAMBA_52_E];
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }
            break;

        case CPSS_PP_FAMILY_SALSA_E:
            switch(prvCpssDrvPpConfig[devNum]->numOfPorts)
            {
                case 12:
                    devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_SALSA_12_E];
                    break;
                case 16:
                    devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_SALSA_16_E];
                    break;
                case 24:
                    devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_SALSA_24_E];
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }
            break;

        case CPSS_PP_FAMILY_CHEETAH_E:
            /* special device treatment */
            if(prvCpssDrvPpConfig[devNum]->devType == CPSS_98DX106_CNS)
            {
                devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_CHEETAH_SMI_106_E];
            }
            else if ((prvCpssDrvPpConfig[devNum]->devType == CPSS_98DX249_CNS) ||
                     (prvCpssDrvPpConfig[devNum]->devType == CPSS_98DX269_CNS) ||
                     (prvCpssDrvPpConfig[devNum]->devType == CPSS_98DX169_CNS))
            {
                if(PRV_CPSS_DRV_HW_IF_PCI_COMPATIBLE_MAC(devNum))
                {
                    devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_CHEETAH_HX_PCI_E];
                }
                else
                {
                    devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_CHEETAH_HX_SMI_E];
                }
            }
            else if(PRV_CPSS_DRV_HW_IF_PCI_COMPATIBLE_MAC(devNum))
            {
                devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_CHEETAH_E];
            }
            else
            {
                devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_CHEETAH_SMI_E];
            }

            break;

        case CPSS_PP_FAMILY_CHEETAH2_E:
            if(PRV_CPSS_DRV_HW_IF_PCI_COMPATIBLE_MAC(devNum))
            {
                devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_CHEETAH2_E];
            }
            else
            {
                devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_CHEETAH2_SMI_E];
            }
            break;

        case CPSS_PP_FAMILY_CHEETAH3_E:
            if(PRV_CPSS_DRV_HW_IF_PEX_COMPATIBLE_MAC(devNum))
            {
                devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[
                                            PRV_CPSS_DRV_FAMILY_CHEETAH3_E];
            }
            else
            {
                devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[
                                       PRV_CPSS_DRV_FAMILY_CHEETAH3_SMI_E];

            }
            break;

        case CPSS_PP_FAMILY_DXCH_XCAT_E:
            /* the xCat devices */
            if(PRV_CPSS_PP_MAC(devNum)->revision != 0)
            {
                devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[
                                            PRV_CPSS_DRV_FAMILY_DXCH_XCAT_E];
            }
            else
            {
                devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[
                                            PRV_CPSS_DRV_FAMILY_DXCH_XCAT_REV0_E];
            }
            break;

        case CPSS_PP_FAMILY_DXCH_XCAT3_E:
            /* the xCat3 devices */
            devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[
                                            PRV_CPSS_DRV_FAMILY_DXCH_XCAT3_E];
            break;

        case CPSS_PP_FAMILY_DXCH_XCAT2_E:
            devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[
                                        PRV_CPSS_DRV_FAMILY_DXCH_XCAT2_E];
            break;
        case CPSS_PP_FAMILY_TIGER_E:
            switch(prvCpssDrvPpConfig[devNum]->numOfPorts)
            {
                case 1:
                    devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_TIGER_XG_E];
                    break;
                case 12:
                    devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_TIGER_12_E];
                    break;
                case 52:
                    devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_TIGER_52_E];
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }
            break;
        case CPSS_PP_FAMILY_PUMA_E:
            devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_PUMA_E];
            break;
        case CPSS_PP_FAMILY_PUMA3_E:
            if(portGroupId >= 2)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_PUMA3_PORT_GROUP_ID_0_E + portGroupId];
            break;

        case CPSS_PP_FAMILY_DXCH_LION_E:
            if(portGroupId >= 4)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_DXCH_LION_PORT_GROUP_ID_0_E + portGroupId];
            break;
        case CPSS_PP_FAMILY_DXCH_LION2_E:
            if(portGroupId >= 8)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_DXCH_LION2_PORT_GROUP_ID_0_E + portGroupId];

            break;
        case CPSS_PP_FAMILY_DXCH_BOBCAT2_E:
#ifdef GM_USED  /* currently keep as was initially in cpss --> with lion2 tree */
            devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_DXCH_LION2_PORT_GROUP_ID_0_E];
#else
            if(prvCpssDrvPpConfig[devNum]->devSubFamily == CPSS_PP_SUB_FAMILY_BOBCAT2_BOBK_E)
            {
                /* bobcat2-bobk info */
                switch(prvCpssDrvPpConfig[devNum]->devType)
                {
                    case CPSS_BOBK_CETUS_DEVICES_CASES_MAC:
                        devIntPtr =
                            &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_DXCH_BOBCAT2_BOBK_CETUS_E];
                        break;
                    default:
                        devIntPtr =
                            &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_DXCH_BOBCAT2_BOBK_CAELUM_E];
                        break;
                }
            }
            else
            {
                /* bobcat2 info */
                devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_DXCH_BOBCAT2_E];
            }
#endif
            break;
        case CPSS_PP_FAMILY_DXCH_BOBCAT3_E:
#ifdef GM_USED  /* currently keep as was initially in cpss --> with lion2 tree */
            devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_DXCH_LION2_PORT_GROUP_ID_0_E];
#else /*!GM_USED*/
            /* bobcat3 info */
            devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_DXCH_BOBCAT3_E];
#endif /*!GM_USED*/
            break;
        case CPSS_PP_FAMILY_DXCH_ALDRIN_E:
#ifdef GM_USED  /* currently keep as was initially in cpss --> with lion2 tree */
            devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_DXCH_LION2_PORT_GROUP_ID_0_E];
#else
            devIntPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_DXCH_ALDRIN_E];
#endif
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    /* sanity check on the info */
    if(devIntPtr                                  == NULL ||
       devIntPtr->drvIntCauseToUniEvConvertFunc   == NULL ||
       devIntPtr->maskRegistersAddressesArray     == NULL ||
       devIntPtr->maskRegistersDefaultValuesArray == NULL ||
       devIntPtr->interruptsScanArray             == NULL ||
       devIntPtr->numOfInterrupts                 == 0
       )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    *devIntPtrPtr = devIntPtr;

    return GT_OK;
}


/*******************************************************************************
* drvInterruptsInit
*
* DESCRIPTION:
*       Initialize the interrupts mechanism for a given device.
*
* INPUTS:
*       devNum          - The device number to initialize the interrupts
*                         mechanism for.
*       portGroupId          - The port group Id.
*       intVecNum       - The interrupt vector number this device is connected
*                         to.
*       intMask         - The interrupt mask to enable/disable interrupts on
*                         this device.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS drvInterruptsInit
(
    IN  GT_U8           devNum,
    IN  GT_U32          portGroupId,
    IN  GT_U32          intVecNum,
    IN  GT_U32          intMask
)
{
    PRV_CPSS_DRV_EV_REQ_NODE_STC *intReqNodeListPtr;/* event node array      */
    PRV_CPSS_DRV_EV_REQ_NODE_STC *intReqNodePtr = NULL;/* event node pointer */
    GT_U32                  scanArrIdx;
    GT_STATUS               retVal;             /* Function's return values.*/
    GT_U32                  i,jj;               /* Loops indexes.           */
    PRV_CPSS_DRV_DEVICE_INTERRUPTS_INFO_STC *devIntPtr = NULL;

    if (prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].intNodesPool != NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);
    }

    /* Get the devices interrupts initialization parameters,    */
    /* according to the device's type.                          */
    retVal = drvInterruptsInfoGet(devNum,portGroupId,&devIntPtr);
    if(retVal != GT_OK)
    {
        return retVal;
    }

    /* Allocate place for the interrupts nodes pool array.  */
    prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].intNodesPool =
        cpssOsMalloc(devIntPtr->numOfInterrupts * sizeof(PRV_CPSS_DRV_EV_REQ_NODE_STC));
    if(prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].intNodesPool == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    intReqNodeListPtr = prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].intNodesPool;

    /* bind in the PP Interrupt Service Routine */
    prvCpssDrvPpConfig[devNum]->intCtrl.isrFuncPtr = prvCpssDrvInterruptPpSR;

    prvCpssDrvPpConfig[devNum]->intCtrl.numOfIntBits  = devIntPtr->numOfInterrupts;

    /* Initialize the Interrupts hierarchy tree.    */
    scanArrIdx = 0;
    retVal = prvCpssDrvInterruptScanInit(
                devIntPtr->numOfScanElements,
                devIntPtr->interruptsScanArray,
                &scanArrIdx,
                &(prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].intScanRoot));
    if(retVal != GT_OK)
    {
        return retVal;
    }

    /* check numOfInterrupts%32 != 0 */
    if((devIntPtr->numOfInterrupts & 0x1f) &&
       (prvCpssDrvPpConfig[devNum]->devFamily != CPSS_PP_FAMILY_SALSA_E))
    {
        /* the enumeration of interrupts for this device has error */
        /* the numOfInterrupts number must be divided by 32 with no left over */
        /* the only exception is Salsa devices that has extra fake interrupts for
           RxTx packets to/from CPU */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);
    }

    /* in polling mode, only summary interrupts are unmasked by default */
    /* according to the interrupt scan tree. */
    if (prvCpssDrvPpConfig[devNum]->intCtrl.needInterruptMaskDefaultValueInit == GT_TRUE)
    {
        retVal = prvCpssDrvInterruptsMaskDefSummaryInit(
                            devIntPtr->numOfInterrupts >> 5,
                            devIntPtr->maskRegistersAddressesArray,
                            devIntPtr->maskRegistersDefaultValuesArray,
                            prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].intScanRoot);

        if (GT_OK != retVal)
        {
            return retVal;
        }
    }

    prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].intMaskShadow = cpssOsMalloc(sizeof(GT_U32)* (devIntPtr->numOfInterrupts>> 5));
    prvCpssDrvPpConfig[devNum]->intCtrl.accessB4StartInit = cpssOsMalloc(sizeof(GT_BOOL)* (devIntPtr->numOfInterrupts>> 5));
    if(prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].intMaskShadow == NULL ||
       prvCpssDrvPpConfig[devNum]->intCtrl.accessB4StartInit == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    for(i = 0; i < devIntPtr->numOfInterrupts ; i++)
    {
        intReqNodePtr = &intReqNodeListPtr[i];

        intReqNodePtr->devNum           = devNum;
        intReqNodePtr->portGroupId           = portGroupId;
        intReqNodePtr->nextPtr          = NULL;
        intReqNodePtr->prevPtr          = NULL;
        intReqNodePtr->intStatus        = PRV_CPSS_DRV_EV_DRVN_INT_ACTIVE_E;
        intReqNodePtr->intRecStatus     = PRV_CPSS_DRV_EV_DRVN_INT_IDLE_E;
        if(devIntPtr->intMaskSetFptr == NULL)
        {
            intReqNodePtr->intMaskSetFptr   = prvCpssDrvInterruptMaskSet;
        }
        else
        {
            intReqNodePtr->intMaskSetFptr   = devIntPtr->intMaskSetFptr;
        }
        intReqNodePtr->intCbFuncPtr     = NULL;
        intReqNodePtr->intCause         = i;
        intReqNodePtr->intMaskReg       = devIntPtr->maskRegistersAddressesArray[i >> 5];
        intReqNodePtr->intBitMask       = 1 << (i & 0x1F);

        if(intReqNodePtr->intMaskReg == CPSS_EVENT_SKIP_MASK_REG_ADDR_CNS)
        {
            /* the entry should be fully ignored !!! */
            intReqNodePtr->uniEvCause = CPSS_UNI_RSRVD_EVENT_E;
            intReqNodePtr->uniEvExt   = 0;
        }
        else
        {
            devIntPtr->drvIntCauseToUniEvConvertFunc(devNum, portGroupId, i,
                                       &intReqNodePtr->uniEvCause,
                                       &intReqNodePtr->uniEvExt);

            /* init fake interrupts */
            if(devIntPtr->hasFakeInterrupts == GT_TRUE &&
               i >= devIntPtr->firstFakeInterruptId)
            {
                if(i >= devIntPtr->firstFakeInterruptId)
                {
                    intReqNodePtr->intMaskSetFptr = drvInterruptsEnableFakeMaskSet;
                }
            }
        }

        /* Set the interrupt mask registers to their default values */
        if((i % 32) == 0)
        {
            prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].intMaskShadow[i / 32] =
                devIntPtr->maskRegistersDefaultValuesArray[i / 32];
            /* Set the device's registers that may not be accessed  */
            /* before start init.                                   */

            prvCpssDrvPpConfig[devNum]->intCtrl.accessB4StartInit[i / 32] = GT_TRUE;
            /* get the not accessible before 'Start Init' */
            for(jj = 0 ; jj < devIntPtr->numOfInterruptRegistersNotAccessibleBeforeStartInit; jj++)
            {
                if(devIntPtr->maskRegistersAddressesArray[i / 32] == devIntPtr->notAccessibleBeforeStartInitPtr[jj])
                {
                    prvCpssDrvPpConfig[devNum]->intCtrl.accessB4StartInit[i / 32] = GT_FALSE;
                    break;
                }
            }
        }
    }

    /* bind the ISR callback routine done before signalling app. */
    if (prvCpssDrvPpConfig[devNum]->intCtrl.bindTheIsrCbRoutineToFdbTrigEnded == GT_TRUE)
    {
        /* set this specific call back at the end of the events loop */
        intReqNodeListPtr[devIntPtr->fdbTrigEndedId].intCbFuncPtr = devIntPtr->fdbTrigEndedCbPtr;
    }

    prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].intVecNum = intVecNum;

    /* Initialize the event queue */
    if (prvCpssDrvEvReqQInit(CPSS_UNI_EVENT_COUNT_E) != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    /* connect the interrupts    */
    retVal =
        prvCpssDrvInterruptConnect(intVecNum,intMask,
                           prvCpssDrvPpConfig[devNum]->intCtrl.isrFuncPtr,
                           &(prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].isrCookieInfo),
                           &(prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].isrConnectionId));
    if (GT_OK != retVal)
    {
        return retVal;
    }
    if (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_HA_E)
    {
        /* enable interrupts in accordance with interrupt mask */
        retVal =  prvCpssDrvInterruptEnable(intMask);
        if (GT_OK != retVal)
        {
            return retVal;
        }
    }
    /* Enable the interrupts in packet processor */
    retVal =  drvInterruptsEnable(devNum,portGroupId);

    return retVal;
}


/*******************************************************************************
* prvCpssDrvDxExMxIntInitObject
*
* DESCRIPTION:
*      This function creates and initializes DxExMx device driver object
*
* INPUTS:
*       none
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - if the driver object have been created before
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvDxExMxIntInitObject
(
     void
)
{
    /* driver object initialization */
    prvCpssDrvGenExMxDxObjPtr->drvHwPpInterruptInit = drvInterruptsInit;

    return GT_OK;
}


/*******************************************************************************
* prvCpssDrvExMxDxHwPpInterruptsMaskToDefault
*
* DESCRIPTION:
*       Initialize interrupts mask to default for a given device.
*       (all interrupts bits are masked except of summary)
*
* INPUTS:
*       devNum          - The device number to set interrupts masks for.
*       portGroupId     - The port group Id.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvExMxDxHwPpInterruptsMaskToDefault
(
    IN  GT_U8           devNum,
    IN  GT_U32          portGroupId
)
{
    GT_U32 i;
    GT_STATUS rc = GT_OK;
    GT_U32  basRegAddr = 0x88c005c;
    GT_U32  regAddr = 0;
    GT_U32  causeBitIdx = 3;
    GT_U32  intMask = 0;
    PRV_CPSS_DRV_DEVICE_INTERRUPTS_INFO_STC *devIntPtr = NULL;
    /* Get the devices interrupts initialization parameters,    */
    /* according to the device's type.                          */
    rc = drvInterruptsInfoGet(devNum,portGroupId,&devIntPtr);
    if(rc != GT_OK)
    {
        return rc;
    }
    for(i = 0; i < (devIntPtr->numOfInterrupts >> 5); i++)
    {
        if(devIntPtr->maskRegistersAddressesArray[i] == CPSS_EVENT_SKIP_MASK_REG_ADDR_CNS)
        {
            /* the entry should be fully ignored !!! */
            continue;
        }

        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,
                                                  portGroupId,
                                                  devIntPtr->maskRegistersAddressesArray[i],
                                                  devIntPtr->maskRegistersDefaultValuesArray[i]);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    if (prvCpssDrvPpConfig[devNum]->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)
    {
        for (i = 0; i < 15; i++)
        {
            if(i == 13)
            {
                continue;
            }
            regAddr = basRegAddr + i*0x1000;
            /* read mask from register */
            rc = prvCpssDrvHwPpPortGroupReadRegister(devNum,portGroupId,regAddr,&intMask);
            if (rc != GT_OK)
            {
                return rc;
            }
            /* mask bit3 HGL port interrupts*/
            intMask &= (~(1 << causeBitIdx));
            /* write updated mask */
            rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,
                                                      portGroupId,
                                                      regAddr,
                                                      intMask);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
    return rc;
}


/*******************************************************************************
* prvCpssDrvExMxDxHwPpInterruptsTreeGet
*
* DESCRIPTION:
*       function return :
*       1. the root to the interrupts tree info of the specific device
*       2. the interrupt registers that can't be accesses before 'Start Init'
*
* APPLICABLE DEVICES: All devices --> Packet Processors (not Fa/Xbar)
*
* INPUTS:
*       devNum - the device number
*       portGroupId          - The port group Id. relevant only to multi port group devices
*
* OUTPUTS:
*       numOfElementsPtr - (pointer to) number of elements in the tree.
*       treeRootPtrPtr - (pointer to) pointer to root of the interrupts tree info.
*       numOfInterruptRegistersNotAccessibleBeforeStartInitPtr - (pointer to)
*                           number of interrupt registers that can't be accessed
*                           before 'Start init'
*       notAccessibleBeforeStartInitPtrPtr (pointer to)pointer to the interrupt
*                           registers that can't be accessed before 'Start init'
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_INITIALIZED       - the driver was not initialized for the device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS   prvCpssDrvExMxDxHwPpInterruptsTreeGet
(
    IN GT_U8    devNum,
    IN  GT_U32          portGroupId,
    OUT GT_U32  *numOfElementsPtr,
    OUT const CPSS_INTERRUPT_SCAN_STC        **treeRootPtrPtr,
    OUT GT_U32  *numOfInterruptRegistersNotAccessibleBeforeStartInitPtr,
    OUT GT_U32  **notAccessibleBeforeStartInitPtrPtr
)
{
    GT_STATUS rc;
    PRV_CPSS_DRV_DEVICE_INTERRUPTS_INFO_STC *devIntPtr;/*pointer to interrupts info*/

    rc = drvInterruptsInfoGet(devNum,portGroupId,&devIntPtr);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }

    *numOfElementsPtr = devIntPtr->numOfScanElements;/* number of elements in interruptsScanArray */
    *treeRootPtrPtr = devIntPtr->interruptsScanArray;/*attach to pointer of interrupts tree*/
    *numOfInterruptRegistersNotAccessibleBeforeStartInitPtr =devIntPtr->numOfInterruptRegistersNotAccessibleBeforeStartInit;
    *notAccessibleBeforeStartInitPtrPtr = devIntPtr->notAccessibleBeforeStartInitPtr;

    return GT_OK;
}
/* macro to skip non valid entries that starts from specific index */
#define INDEX_AFTER_SKIPPED_ENTRIES_MAC(/*IN*/_startIndex,/*IN*/_rangeSize,/*OUT*/_indexAfterSkippedEntries)    \
{                                                                                                               \
    _indexAfterSkippedEntries = _startIndex;                                                                    \
    while(intrScanArr[_indexAfterSkippedEntries].maskRegAddr == CPSS_EVENT_SKIP_MASK_REG_ADDR_CNS)              \
    {                                                                                                           \
        /* the entry should be fully ignored !!! */                                                             \
        _indexAfterSkippedEntries++;                                                                            \
                                                                                                                \
        if((_indexAfterSkippedEntries+1) >= _rangeSize)                                                         \
        {                                                                                                       \
            /* the tree is over */                                                                              \
            break;                                                                                              \
        }                                                                                                       \
    }                                                                                                           \
}

/*******************************************************************************
* prvCpssDrvExMxDxHwPpMaskRegInfoGet
*
* DESCRIPTION:
*      function to build from the interrupts tree and the mask registers
*      addresses , the content of the summary bits in those mask registers.
*      1. fill the array of mask registers addresses
*      2. fill the array of default values for the mask registers
*      3. update the bits of nonSumBitMask in the scan tree
*
* INPUTS:
*       devFamily - device family
*       numScanElements - number of elements in intrScanArr[]
*       intrScanArr[] - interrupts scan info array
*       numMaskRegisters - number of registers in maskRegMapArr,maskRegDefaultSummaryArr
*       maskRegMapArr[]            - (pointer to) place holder for the mask registers addresses
*       maskRegDefaultSummaryArr[] - (pointer to) place holder for the registers values
*
* OUTPUTS:
*       intrScanArr[ii].nonSumBitMask - non summary bits updated
*       maskRegMapArr[]     - (pointer to) the mask registers addresses
*       maskRegDefaultSummaryArr[] - (pointer to) the registers values
*
* RETURNS:
*       GT_OK   - on success,
*       GT_OUT_OF_CPU_MEM - fail to allocate cpu memory (osMalloc)
*       GT_BAD_PARAM - the scan tree information has error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvExMxDxHwPpMaskRegInfoGet
(
    IN  CPSS_PP_FAMILY_TYPE_ENT devFamily,
    IN  GT_U32  numScanElements,
    INOUT PRV_CPSS_DRV_INTERRUPT_SCAN_STC intrScanArr[],
    IN  GT_U32   numMaskRegisters,
    OUT GT_U32   maskRegDefaultSummaryArr[],
    OUT GT_U32   maskRegMapArr[]
)
{
    GT_STATUS rc = GT_OK;
    GT_U32  ii;
    GT_U32  jj;
    GT_U32  *tmpMaskRegMapArr;
    GT_U32  *tmpMaskRegDefaultSummaryArr;
    GT_U32  enumerationInterruptIndex;
    GT_U32  lastIndexChecked;
    GT_BOOL wasSubTree;
    GT_U32  currentLevel;
    GT_U32  treeNumLeafs[MAX_LEVELS_CNS]={0};
    GT_U32  dummyMaskRegAddr = CPSS_EVENT_SKIP_MASK_REG_ADDR_CNS;/* register address to be used to fill all the
                                    'empty' indexes in the array of maskRegMapArr */
    GT_U32  pexCauseAddr ,pexMaskAddr;
    GT_U32  indexAfterSkippedEntries;

    rc = prvCpssDrvExMxDxHwPpPexAddrGet(devFamily,&pexCauseAddr,&pexMaskAddr);
    if(rc != GT_OK)
    {
        return rc;
    }

    tmpMaskRegMapArr = cpssOsMalloc(numScanElements * sizeof(GT_U32));
    tmpMaskRegDefaultSummaryArr = cpssOsMalloc(numScanElements * sizeof(GT_U32));

    if(tmpMaskRegMapArr == NULL || tmpMaskRegDefaultSummaryArr == NULL)
    {
        /* Error */
        rc = GT_OUT_OF_CPU_MEM;
        goto handle_error_lbl;
    }

    /* fill array tmpMaskRegMapArr[] */
    /* fill array tmpMaskRegDefaultSummaryArr[] */
    /* calculate the intrScanArr[].nonSumBitMask -->
       nonSumBitMask are the non-summary bits --> calculate it between bits of:
       startIdx , endIdx and by the subIntrListLen that hold sub tree of summary
       bits */
    for(ii = 0 ; ii < numScanElements; ii++)
    {
        GT_U32  nonSumBitMask = 0;
        GT_U32  numBitsStartToEnd;
        GT_U32  startBit;

        if(ii < numMaskRegisters)
        {
            maskRegDefaultSummaryArr[ii] = 0;/* init */
            maskRegMapArr[ii] = dummyMaskRegAddr;/* init */
        }

        if(intrScanArr[ii].maskRegAddr == CPSS_EVENT_SKIP_MASK_REG_ADDR_CNS)
        {
            /* the entry should be fully ignored !!! */
            continue;
        }

        /* replace the place holder with real PEX interrupt mask address */
        if(intrScanArr[ii].maskRegAddr == PRV_CPSS_DRV_SCAN_PEX_MASK_REG_PLACE_HOLDER_CNS)
        {
            intrScanArr[ii].maskRegAddr = pexMaskAddr;
        }

        /* replace the place holder with real PEX interrupt cause address */
        if(intrScanArr[ii].causeRegAddr == PRV_CPSS_DRV_SCAN_PEX_CAUSE_REG_PLACE_HOLDER_CNS)
        {
            intrScanArr[ii].causeRegAddr = pexCauseAddr;
        }

        /*fill array tmpMaskRegMapArr[]*/
        tmpMaskRegMapArr[ii] = intrScanArr[ii].maskRegAddr;

        tmpMaskRegDefaultSummaryArr[ii] = 0;

        /* validation check */
        if((intrScanArr[ii].subIntrListLen + ii) > numScanElements)
        {
            /* Error */
            rc = GT_BAD_PARAM;
            goto handle_error_lbl;
        }

        if(intrScanArr[ii].endIdx < intrScanArr[ii].startIdx)
        {
            /* Error */
            rc = GT_BAD_PARAM;
            goto handle_error_lbl;
        }

        numBitsStartToEnd = intrScanArr[ii].endIdx - intrScanArr[ii].startIdx + 1;
        startBit = intrScanArr[ii].startIdx & 0x1F;/* % 32 */

        if(numBitsStartToEnd > 32)
        {
            /* Error */
            rc = GT_BAD_PARAM;
            goto handle_error_lbl;
        }
        else if(numBitsStartToEnd == 32)
        {
            nonSumBitMask = 0xFFFFFFFF;
        }
        else
        {
            /* the MACRO of U32_SET_FIELD_MASKED_MAC will cut the value of
               0xFFFFFFFF to set bits only in needed length */
            U32_SET_FIELD_MASKED_MAC(nonSumBitMask ,startBit,numBitsStartToEnd,0xFFFFFFFF);
        }

        lastIndexChecked = ii;
        wasSubTree = GT_FALSE;

        /* loop on the summary bits */
        for(jj = 0 ; jj < intrScanArr[ii].subIntrListLen; jj++)
        {
            GT_U32  bitmapOfBit;

            currentLevel = 0;

            INDEX_AFTER_SKIPPED_ENTRIES_MAC((lastIndexChecked+1) , numScanElements , indexAfterSkippedEntries);

            if((indexAfterSkippedEntries < numScanElements) &&
                wasSubTree == GT_FALSE &&
                intrScanArr[indexAfterSkippedEntries].subIntrListLen != 0)
            {
                /* the next index is going to have sub tree */
                /* so we first need to treat the root of this sub tree */
                lastIndexChecked = indexAfterSkippedEntries;
                wasSubTree = GT_TRUE;

            }
            else
            {
                wasSubTree = GT_FALSE;

                INDEX_AFTER_SKIPPED_ENTRIES_MAC(lastIndexChecked , numScanElements , indexAfterSkippedEntries);
                if(indexAfterSkippedEntries != lastIndexChecked)
                {
                    /* indication of skipped entries ... do the '-1'
                       because the 'do-while' will add '++' */
                    lastIndexChecked = indexAfterSkippedEntries - 1;
                }
                else
                if(lastIndexChecked != ii &&
                   intrScanArr[lastIndexChecked].subIntrListLen != 0)
                {
                    /* in the previous loop we treated the root of a sub tree */
                    /* so now we need to skip the sub tree , for that we must
                       start with the root of the sub tree again */
                    lastIndexChecked--;
                }

                do
                {
                    lastIndexChecked++;

                    if(lastIndexChecked >= numScanElements)
                    {
                        /* the tree is over */
                        break;
                    }

                    INDEX_AFTER_SKIPPED_ENTRIES_MAC(lastIndexChecked , numScanElements , indexAfterSkippedEntries);
                    lastIndexChecked = indexAfterSkippedEntries;

                    if(intrScanArr[lastIndexChecked].subIntrListLen != 0)
                    {
                        /* this one has sub tree */
                        if(treeNumLeafs[currentLevel] == 0)
                        {
                            treeNumLeafs[currentLevel] = intrScanArr[lastIndexChecked].subIntrListLen;
                            currentLevel++;
                            /* set this level sub tree num leafs */
                            if(currentLevel >= MAX_LEVELS_CNS)
                            {
                                /* error in tree or really this device needs more levels ?! */
                                rc = GT_BAD_PARAM;
                                goto handle_error_lbl;
                            }
                        }
                        else
                        {


                            /* set next level sub tree num leafs */
                            currentLevel++;
                            /* set this level sub tree num leafs */
                            if(currentLevel >= MAX_LEVELS_CNS)
                            {
                                /* error in tree or really this device needs more levels ?! */
                                rc = GT_BAD_PARAM;
                                goto handle_error_lbl;
                            }
                            treeNumLeafs[currentLevel] = intrScanArr[lastIndexChecked].subIntrListLen;
                        }
                        wasSubTree = GT_TRUE;
                    }
                    else if(currentLevel == 0)
                    {
                        if(treeNumLeafs[currentLevel])
                        {
                            treeNumLeafs[currentLevel]--;
                        }
                    }
                    else if(treeNumLeafs[currentLevel] == 0)
                    {
                        currentLevel--;
                        lastIndexChecked--;/* make sure to check again this leaf to
                                              see if it ends more deep branch */
                    }
                    else
                    {
                        while(1)
                        {
                            treeNumLeafs[currentLevel]--;
                            if(treeNumLeafs[currentLevel] == 0)
                            {
                                /* this level cleared -->remove the sub tree from the 'father' */
                                if(currentLevel)
                                {
                                    currentLevel--;
                                    continue;
                                }
                            }

                            break;
                        }
                    }

                }while(currentLevel || treeNumLeafs[0]);

                if(wasSubTree == GT_TRUE)
                {
                    wasSubTree = GT_FALSE;
                    /* last checked index belongs to the sub tree */

                    /* so we need to start from the end of it */
                    jj--;/* we handled the sub-tree at this loop so we need to
                            be able to , makeup for it with extra loop */
                    continue;
                }

            }

            if(lastIndexChecked >= numScanElements)
            {
                /* not error */
                break;
            }

            if(intrScanArr[lastIndexChecked].bitNum > 31)
            {
                /* Error */
                rc = GT_BAD_PARAM;
                goto handle_error_lbl;
            }

            if(intrScanArr[lastIndexChecked].maskRegAddr == CPSS_EVENT_SKIP_MASK_REG_ADDR_CNS)
            {
                /* the entry should be fully ignored !!! */
                bitmapOfBit = 0;
            }
            else
            {
                bitmapOfBit = 1 << intrScanArr[lastIndexChecked].bitNum;
            }

            /*fill array tmpMaskRegDefaultSummaryArr[]*/
            tmpMaskRegDefaultSummaryArr[ii] |= bitmapOfBit;
        }

        /* remove the summary bits from nonSumBitMask */
        intrScanArr[ii].nonSumBitMask =
                        nonSumBitMask &~ tmpMaskRegDefaultSummaryArr[ii];
    }

    for(/*continue ii*/ ; ii < numMaskRegisters; ii++)
    {
        maskRegMapArr[ii] = dummyMaskRegAddr;/* init */
    }


    /* convert array tmpMaskRegMapArr[] --> maskRegMapArr[] */
    /* convert array tmpMaskRegDefaultSummaryArr[] --> maskRegDefaultSummaryArr[] */
    for(ii = 0 ; ii < numScanElements; ii++)
    {
        if(intrScanArr[ii].maskRegAddr == CPSS_EVENT_SKIP_MASK_REG_ADDR_CNS)
        {
            /* the entry should be fully ignored !!! */
            continue;
        }

        /* the start index is in the range of interrupts for this register */
        enumerationInterruptIndex = intrScanArr[ii].startIdx >> 5;/* /32 */

        if(enumerationInterruptIndex >= numMaskRegisters)
        {
            /* Error */
            rc = GT_BAD_PARAM;
            goto handle_error_lbl;
        }

        if(maskRegMapArr[enumerationInterruptIndex] != dummyMaskRegAddr &&
           maskRegMapArr[enumerationInterruptIndex] != pexMaskAddr )/* there are 2 leaf point to the same register of PEX */
        {
            /* already set !? */

            /* Error */
            rc = GT_BAD_PARAM;
            goto handle_error_lbl;
        }

        maskRegMapArr[enumerationInterruptIndex] = tmpMaskRegMapArr[ii];

        /* NOTE : must be "|=" and not "=" to support the fact that there are
           2 summary bits to the same register --> see PexErrSum , PexIntSum */
        maskRegDefaultSummaryArr[enumerationInterruptIndex] |= tmpMaskRegDefaultSummaryArr[ii];

        /* PATCH for GM simulation */
        #ifdef ASIC_SIMULATION
        if(CPSS_IS_EXMXPM_FAMILY_MAC(devFamily) == GT_TRUE)
        {
            intrScanArr[ii].rwBitMask = 0;/* when rwBitMask!=0 makes the GM
                                                 update also ROC bits */
        }
        #endif /*ASIC_SIMULATION*/

    }

handle_error_lbl:
    if(tmpMaskRegMapArr)
    {
        cpssOsFree(tmpMaskRegMapArr);
    }

    if(tmpMaskRegDefaultSummaryArr)
    {
        cpssOsFree(tmpMaskRegDefaultSummaryArr);
    }

    return rc;
}

/*******************************************************************************
* prvCpssDrvExMxDxHwPpPexAddrGet
*
* DESCRIPTION:
*      function to get addresses of cause and mask interrupt registers of PEX/PCI.
*       (in address space of 'Config cycle')
*
* INPUTS:
*       devFamily - device family
*
* OUTPUTS:
*       pexCauseRegAddrPtr - (pointer to) PEX/PCI interrupt cause register address
*       pexMaskRegAddrPtr  - (pointer to) PEX/PCI interrupt mask register address
*
* RETURNS:
*       GT_OK   - on success,
*       GT_BAD_PTR  - on NULL pointer
*       GT_BAD_PARAM - on bad param
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvExMxDxHwPpPexAddrGet
(
    IN  CPSS_PP_FAMILY_TYPE_ENT devFamily,
    OUT GT_U32      *pexCauseRegAddrPtr,
    OUT GT_U32      *pexMaskRegAddrPtr
)
{
    enum{
         USE_PEX_E       /* pex */
        ,USE_PCI_E       /* pci */
        ,USE_NONE_E     /* none used */

    }usePexType;
    GT_U32  extraUnitOffset = 0;/* offset added to 'fixed' address */

    CPSS_NULL_PTR_CHECK_MAC(pexCauseRegAddrPtr);
    CPSS_NULL_PTR_CHECK_MAC(pexMaskRegAddrPtr);

    usePexType = USE_PEX_E;

    switch(devFamily)
    {
        case CPSS_PP_FAMILY_PUMA3_E :
            extraUnitOffset = 0x00000000;   /*0x00001900*/
            break;

        case CPSS_PP_FAMILY_TWISTC_E:
        case CPSS_PP_FAMILY_TWISTD_E:
        case CPSS_PP_FAMILY_SAMBA_E:
        case CPSS_PP_FAMILY_TIGER_E:
        case  CPSS_PP_FAMILY_CHEETAH_E:
        case  CPSS_PP_FAMILY_CHEETAH2_E:
            usePexType = USE_PCI_E;
            break;

        case CPSS_PP_FAMILY_PUMA_E:
        case CPSS_PP_FAMILY_CHEETAH3_E :
        case CPSS_PP_FAMILY_DXCH_LION_E:
        case CPSS_PP_FAMILY_DXCH_LION2_E:
            extraUnitOffset = 0x00070000;/*0x00071900*/
            break;

        case CPSS_PP_FAMILY_DXCH_BOBCAT2_E:
        case CPSS_PP_FAMILY_DXCH_BOBCAT3_E:
        case CPSS_PP_FAMILY_DXCH_ALDRIN_E:
#ifdef GM_USED  /* currently keep as was initially in cpss --> with lion2 tree */
            extraUnitOffset = 0x00070000;/*0x00071900*/
#else
            extraUnitOffset = 0x00040000;/*0x00041900*/
#endif
            break;

        case CPSS_PP_FAMILY_DXCH_XCAT_E:
            extraUnitOffset = 0x000F0000; /*0x000F1900*/
            break;

        case CPSS_PP_FAMILY_DXCH_XCAT3_E:
            extraUnitOffset = 0x00040000;
            break;

        case CPSS_PP_FAMILY_SALSA_E:
        case CPSS_PP_FAMILY_DXCH_XCAT2_E:
            usePexType = USE_NONE_E;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch(usePexType)
    {
        case USE_PEX_E:
            *pexCauseRegAddrPtr = extraUnitOffset + 0x00001900;
            *pexMaskRegAddrPtr  = extraUnitOffset + 0x00001910;
            break;
        case USE_PCI_E:
            *pexCauseRegAddrPtr = 0x00000114;/*PCI*/
            *pexMaskRegAddrPtr  = 0x00000118;/*PCI*/
            break;
        default:
            *pexCauseRegAddrPtr = 0xFFFFFFFF; /* unused register address */
            *pexMaskRegAddrPtr  = 0xFFFFFFFF; /* unused register address */
            break;
    }


    return GT_OK;
}

static GT_U32   currentIndex = 0;

#ifdef DUMP_DEFAULT_INFO
#define INDENT_STRING   "%s"
static void printElement(
    CPSS_INTERRUPT_SCAN_STC *currIntInfoPtr,
    GT_CHAR *indentStringOffsetPtr
)
{
    static    GT_CHAR readRegFuncAddr[20];
    static    GT_CHAR writeRegFuncAddr[20];

    cpssOsPrintf("currentIndex = %d (0x%x) \n" ,
        currentIndex,currentIndex);
    currentIndex++;

    if(currIntInfoPtr->maskRegAddr == CPSS_EVENT_SKIP_MASK_REG_ADDR_CNS)
    {
        /* the entry should be fully ignored !!! */
        cpssOsPrintf("the entry should be fully ignored !!! \n");

        /* generate the same amount of 'lines' as the 'valid' entry ...
           to allow simple 'compare' of trees */
        cpssOsPrintf("\n");
        cpssOsPrintf("\n");
        cpssOsPrintf("\n\n\n");
        return;
    }

    cpssOsSprintf(readRegFuncAddr ,"0x%8.8x",currIntInfoPtr->pRegReadFunc );
    cpssOsSprintf(writeRegFuncAddr,"0x%8.8x",currIntInfoPtr->pRegWriteFunc);

    cpssOsPrintf(INDENT_STRING "%d %d %d 0x%8.8x \n"/*bitNum ..gppFuncPtr*/
                INDENT_STRING "0x%8.8x 0x%8.8x %s %s \n"/*causeRegAddr..pRegWriteFunc*/
                INDENT_STRING "0x%8.8x 0x%8.8x 0x%8.8x 0x%8.8x \n"/*startIdx..rwBitMask*/
                INDENT_STRING "%d %d \n\n\n"/*maskRcvIntrEn..rwBitMask*/
    ,indentStringOffsetPtr
            ,currIntInfoPtr->bitNum
            ,currIntInfoPtr->isGpp
            ,currIntInfoPtr->gppId
            ,currIntInfoPtr->gppFuncPtr
    ,indentStringOffsetPtr
            ,currIntInfoPtr->causeRegAddr
            ,currIntInfoPtr->maskRegAddr
            ,(currIntInfoPtr->pRegReadFunc  == prvCpssDrvHwPpPortGroupIsrRead  ? "prvCpssDrvHwPpPortGroupIsrRead"  : readRegFuncAddr )
            ,(currIntInfoPtr->pRegWriteFunc == prvCpssDrvHwPpPortGroupIsrWrite ? "prvCpssDrvHwPpPortGroupIsrWrite" : writeRegFuncAddr)
    ,indentStringOffsetPtr
            ,currIntInfoPtr->startIdx
            ,currIntInfoPtr->endIdx
            ,currIntInfoPtr->nonSumBitMask
            ,currIntInfoPtr->rwBitMask
    ,indentStringOffsetPtr
            ,currIntInfoPtr->maskRcvIntrEn
            ,currIntInfoPtr->subIntrListLen
        );
}
/*******************************************************************************
* printSubTree
*
* DESCRIPTION:
*       print sub tree according to proper indent.
*
* INPUTS:
*       fullTreePtr - pointer to the 'Full tree' of interrupts
*       numElementsInFullTree - number of elements in the 'Full tree'
*       subTreeIndex - index in the 'Full tree' that represent the 'Sub tree'
*                   which need print.
*       indentId    - the indent id.
* OUTPUTS:
*       lastIndexSubTreePtr - (pointer to) Index in the 'Full tree' which belong
*                           to the last Index in the 'Sub Tree'.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_BAD_STATE - algorithm / DB error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS printSubTree
(
    IN PRV_CPSS_DRV_INTERRUPT_SCAN_STC    *fullTreePtr ,
    IN GT_U32    numElementsInFullTree,
    IN GT_U32    subTreeIndex,
    OUT GT_U32   *lastIndexSubTreePtr,
    IN GT_U32    indentId
)
{
    GT_STATUS   rc;
    GT_U32  ii, iiNew;
    CPSS_INTERRUPT_SCAN_STC *currentElementPtr;
    GT_U32  count;
    GT_CHAR indentStringOffset[128];
    GT_U32 kk;
    GT_CHAR levelIdStr[20];
    GT_U32  stringLen;

    if(subTreeIndex >= numElementsInFullTree)
    {
        /* algorithm / DB error */
        cpssOsPrintf(" Invalid Tree ... got index[%d] >= range[%d] \n" ,
            subTreeIndex , numElementsInFullTree);
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    currentElementPtr = &fullTreePtr[subTreeIndex];
    count = currentElementPtr->subIntrListLen;

    if(indentId == 0)
    {
        indentStringOffset[0] = ' ';
        kk = 1;
    }
    else
    {
        kk = 0;
        for(ii = 0 ; ii < indentId ; ii++)
        {
            kk = ii*4;
            indentStringOffset[kk]   = ' ';
            indentStringOffset[kk+1] = ' ';
            indentStringOffset[kk+2] = ' ';
            indentStringOffset[kk+3] = ' ';
        }
        kk += 4;
    }
    indentStringOffset[kk] = 0;/* end of string */

    /* set the level into the string */
    cpssOsSprintf(levelIdStr,"L[%d] ",indentId);
    stringLen = cpssOsStrlen(levelIdStr);
    if (kk >= stringLen)
    {
        cpssOsMemCpy(indentStringOffset,levelIdStr,stringLen);
    }


    /* print the base of the sub tree */
    printElement(currentElementPtr,indentStringOffset);

    /* update indentStringOffset for the 'first level leafs' of this sub tree */
    {
        indentStringOffset[kk]   = ' ';
        indentStringOffset[kk+1] = ' ';
        indentStringOffset[kk+2] = ' ';
        indentStringOffset[kk+3] = ' ';
        kk += 4;
    }
    indentStringOffset[kk] = 0;/* end of string */

    indentId++;

    cpssOsSprintf(levelIdStr,"L[%d] ",indentId);
    stringLen = cpssOsStrlen(levelIdStr);
    if (kk >= stringLen)
    {
        cpssOsMemCpy(indentStringOffset,levelIdStr,stringLen);
    }


    for(ii = (subTreeIndex + 1);/* skip start of sub tree */
       (ii < numElementsInFullTree) && count ;
       ii++)
    {
        if(0 == (ii & 0x0f))
        {
            /* allow the terminal to observe all the 'printings' */
            cpssOsTimerWkAfter(50);
        }

        currentElementPtr = &fullTreePtr[ii];

        if(currentElementPtr->subIntrListLen) /* sub tree */
        {
            /* let recursive on the sub tree to print it */
            rc = printSubTree(fullTreePtr,numElementsInFullTree,ii,&iiNew,indentId);
            if(rc != GT_OK)
            {
                return rc;
            }

            ii = iiNew;/* last index that is in the sub tree ...
                          with the '++' of the loop it will become 'first index out the tree' */
        }
        else /* leaf . not sub tree */
        {
            /* print the entry that is leaf in the sub tree */
            printElement(currentElementPtr,indentStringOffset);
        }

        if(currentElementPtr->maskRegAddr == CPSS_EVENT_SKIP_MASK_REG_ADDR_CNS)
        {
            /* not counting this invalid entry */
            continue;
        }


        count--;/* indicate that another child (leaf/sub tree) was done */
    }

    *lastIndexSubTreePtr = ii-1;/* we return value of 'last Index in the Sub Tree'*/

    return GT_OK;
}


/*******************************************************************************
* prvCpssDrvPpIntDefPrint
*
* DESCRIPTION:
*      print the interrupts arrays info, that build by
*      prvCpssDrvExMxDxHwPpMaskRegInfoGet
*
* INPUTS:
*       numScanElements - number of elements in intrScanArr[]
*       intrScanArr[] - interrupts scan info array
*       numMaskRegisters - number of registers in maskRegMapArr,maskRegDefaultSummaryArr
*       maskRegMapArr[]            - (pointer to) place holder for the mask registers addresses
*       maskRegDefaultSummaryArr[] - (pointer to) place holder for the registers values
*
* OUTPUTS:
*
* RETURNS:
*       void
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void  prvCpssDrvPpIntDefPrint(
    IN GT_U32  numScanElements,
    IN CPSS_INTERRUPT_SCAN_STC intrScanArr[],
    IN GT_U32  numMaskRegisters,
    IN GT_U32  maskRegMapArr[] ,
    IN GT_U32  maskRegDefaultSummaryArr[]
)
{
    GT_U32  ii;

    currentIndex = 0;/* global index for the lines */

    cpssOsPrintf("maskRegMapArr , maskRegDefaultSummaryArr \n");
    cpssOsPrintf("================================================ \n");
    for(ii = 0; ii < numMaskRegisters ; ii++)
    {
        if(0 == (ii & 0x1f))
        {
            /* allow the terminal to observe all the 'printings' */
            cpssOsTimerWkAfter(50);
        }
        cpssOsPrintf("0x%8.8x       , 0x%8.8x \n",maskRegMapArr[ii] , maskRegDefaultSummaryArr[ii]);
    }

    cpssOsPrintf("\n");

    cpssOsPrintf("intrScanArr \n");
    cpssOsPrintf("=============== \n");

    cpssOsPrintf("bitNum,isGpp,gppId,gppFuncPtr \n");
    cpssOsPrintf("causeRegAddr,maskRegAddr,pRegReadFunc,pRegWriteFunc \n");
    cpssOsPrintf("startIdx,endIdx,nonSumBitMask,rwBitMask \n");
    cpssOsPrintf("maskRcvIntrEn,subIntrListLen \n");
    cpssOsPrintf("=============== \n");

    ii = 0;
    printSubTree(intrScanArr,numScanElements,ii,&ii,1);

    if((ii + 1) < numScanElements)
    {
        ii++;/* first index that was not printed yet */
        /* we got leftovers */
        cpssOsPrintf("NOTE: next entries are 'out of the tree' !!! \n");
        /* continue to dump 'left over' elements !!! */
        for(/*ii continue */;ii < numScanElements; ii++)
        {
            printElement(&intrScanArr[ii]," ----  ");
        }
    }

    cpssOsPrintf("interrupt tree summary: numScanElements[%d] \n",
        numScanElements);
}
#endif/*DUMP_DEFAULT_INFO*/

/*******************************************************************************
* prvCpssDrvPpIntDefPrint_regInfoByInterruptIndex
*
* DESCRIPTION:
*      print the register info according to value in their 'huge' interrupts enum
*       for example : for bobcat2 : PRV_CPSS_BOBCAT2_INT_CAUSE_ENT
*       (assuming that this register is part of the interrupts tree)
*
* INPUTS:
*       numScanElements - number of elements in intrScanArr[]
*       intrScanArr[] - interrupts scan info array
*       interruptId - the Id to look it the corresponding register.
*
* OUTPUTS:
*
* RETURNS:
*       void
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void  prvCpssDrvPpIntDefPrint_regInfoByInterruptIndex(
    IN GT_U32  numScanElements,
    IN CPSS_INTERRUPT_SCAN_STC intrScanArr[],
    IN GT_U32   interruptId
)
{
    GT_U32  ii;
    CPSS_INTERRUPT_SCAN_STC *currIntInfoPtr;
    GT_U32 registerId = (interruptId >> 5);/*ignore 5 first LSBits*/

    currIntInfoPtr = &intrScanArr[0];

    for(ii = 0; ii < numScanElements ; ii++, currIntInfoPtr++)
    {
        if(currIntInfoPtr->maskRegAddr == CPSS_EVENT_SKIP_MASK_REG_ADDR_CNS)
        {
            /* the entry should be fully ignored !!! */
            continue;
        }

        if((currIntInfoPtr->startIdx >> 5) == registerId)/*ignore 5 first LSBits*/
        {
            /* found match */
            break;
        }
    }

    if(ii == numScanElements)
    {
        cpssOsPrintf("no match found for interruptId[%d] \n",
            interruptId);
        return;
    }

    cpssOsPrintf("\n");

    cpssOsPrintf("interruptId[%d] is bit[%d] in register address [0x%8.8x]  (in the huge enum in H file in registerId[%d]) \n" ,
        interruptId,
        (interruptId & 0x1F),
        currIntInfoPtr->causeRegAddr,
        registerId);

    cpssOsPrintf("\n");
    cpssOsPrintf("more info ... \n");
    cpssOsPrintf("position in intrScanArr[%d] \n" , ii);
    cpssOsPrintf("=============== \n");

    cpssOsPrintf("bitNum,isGpp,gppId,gppFuncPtr \n");
    cpssOsPrintf("causeRegAddr,maskRegAddr,pRegReadFunc,pRegWriteFunc \n");
    cpssOsPrintf("startIdx,endIdx,nonSumBitMask,rwBitMask \n");
    cpssOsPrintf("maskRcvIntrEn,subIntrListLen \n");
    cpssOsPrintf("=============== \n");

    cpssOsPrintf("%d %d %d 0x%8.8x \n"/*bitNum ..gppFuncPtr*/
                "0x%8.8x 0x%8.8x 0x%8.8x 0x%8.8x \n"/*causeRegAddr..pRegWriteFunc*/
                "0x%8.8x 0x%8.8x 0x%8.8x 0x%8.8x \n"/*startIdx..rwBitMask*/
                "%d %d \n\n\n"/*maskRcvIntrEn..rwBitMask*/
            ,currIntInfoPtr->bitNum
            ,currIntInfoPtr->isGpp
            ,currIntInfoPtr->gppId
            ,currIntInfoPtr->gppFuncPtr
            ,currIntInfoPtr->causeRegAddr
            ,currIntInfoPtr->maskRegAddr
            ,currIntInfoPtr->pRegReadFunc
            ,currIntInfoPtr->pRegWriteFunc
            ,currIntInfoPtr->startIdx
            ,currIntInfoPtr->endIdx
            ,currIntInfoPtr->nonSumBitMask
            ,currIntInfoPtr->rwBitMask
            ,currIntInfoPtr->maskRcvIntrEn
            ,currIntInfoPtr->subIntrListLen
        );
}


