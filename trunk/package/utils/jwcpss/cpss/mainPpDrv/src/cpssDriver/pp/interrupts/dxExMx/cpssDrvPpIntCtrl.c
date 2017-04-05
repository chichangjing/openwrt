/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpIntCtrl.c
*
* DESCRIPTION:
*       Includes driver interrupts control functions.
*
* FILE REVISION NUMBER:
*       $Revision: 10 $
*
*******************************************************************************/
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/hardware/prvCpssDrvObj.h>

/*******************************************************************************
* internalDriverDxExMxEventsMask
*
* DESCRIPTION:
*       enables / disables a given event from reaching the CPU.
*
* INPUTS:
*       devNum      - The PP's device number to mask / unmask the interrupt for.
*       portGroupId - the port group Id.
*       intIndex    - The interrupt cause to enable/disable.
*       enable      - GT_TRUE enable, GT_FALSE disable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internalDriverDxExMxEventsMask
(
    IN  GT_U8   devNum,
    IN  GT_U32  portGroupId,
    IN  GT_U32  intIndex,
    IN  GT_BOOL enable
)
{
    GT_U32 maskIdx;             /* Index in the mask shadows array,     */
                                /* representing the register to which   */
                                /* this cause belongs.                  */
    GT_U32 *intMaskShadow;      /* Holds the mask shadow array pointer. */
    GT_U32 causeBitIdx;         /* Index in the cause register, of the  */
                                /* bit representing the given cause.    */
    GT_U32 maskRegAddr;         /* Address of the interrupt mask        */
                                /* register.                            */
    GT_STATUS retVal;
    PRV_CPSS_DRV_EV_REQ_NODE_STC * intReqNodeList;

    intMaskShadow   = prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].intMaskShadow;
    maskIdx         = intIndex / 32;
    causeBitIdx     = intIndex % 32;

    intReqNodeList = prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].intNodesPool;

    if(intReqNodeList == NULL)
    {
        /* the interrupt engine was not initialized. */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    if(intReqNodeList[intIndex].intMaskReg == CPSS_EVENT_SKIP_MASK_REG_ADDR_CNS)
    {
        /* the entry should be fully ignored !!! */
        return GT_OK;
    }

    if (intReqNodeList[intIndex].uniEvCause != CPSS_UNI_RSRVD_EVENT_E)
    {
        if (prvCpssDrvEvReqQUserHndlGet(intReqNodeList[intIndex].uniEvCause) == NULL)
        {
            /* event not binded yet by application */
            if (GT_TRUE == enable)
            {
                /* by default the event is disabled, nothing to do here... */
                return GT_OK;
            }
            else
            {
                /* only binded events can be enabled */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }
        }
    }

    maskRegAddr = intReqNodeList[intIndex].intMaskReg;

    if(enable == GT_FALSE)
        intMaskShadow[maskIdx] |= (1 << causeBitIdx);
    else
        intMaskShadow[maskIdx] &= (~(1 << causeBitIdx));

    if((maskRegAddr != prvCpssDrvPpConfig[devNum]->intCtrl.intSumMaskRegAddr) ||
       (0 == PRV_CPSS_DRV_HW_IF_PCI_COMPATIBLE_MAC(devNum)))
    {
        retVal = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,maskRegAddr,intMaskShadow[maskIdx]);
    }
    else
    {
        retVal = prvCpssDrvHwPpPortGroupWriteInternalPciReg(devNum,portGroupId,maskRegAddr,
                                         intMaskShadow[maskIdx]);
    }

    return retVal;
}
/*******************************************************************************
* driverDxExMxEventsMask
*
* DESCRIPTION:
*       enables / disables a given event from reaching the CPU.
*
* INPUTS:
*       devNum      - The PP's device number to mask / unmask the interrupt for.
*       intIndex    - The interrupt cause to enable/disable.
*       enable      - GT_TRUE enable, GT_FALSE disable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS driverDxExMxEventsMask
(
    IN  GT_U8   devNum,
    IN  GT_U32  intIndex,
    IN  GT_BOOL enable
)
{
    GT_STATUS rc;
    GT_U32  portGroupId;/*port group Id*/

    PRV_CPSS_DRV_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        rc = internalDriverDxExMxEventsMask(devNum,portGroupId,intIndex,enable);
        if(rc != GT_OK)
        {
            return rc;
        }

    }
    PRV_CPSS_DRV_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    return GT_OK;
}



/*******************************************************************************
* driverDxExMxEventsMaskGet
*
* DESCRIPTION:
*       Gets enable/disable status of a given event reaching the CPU.
*
* INPUTS:
*       devNum      - The PP's device number to mask / unmask the interrupt for.
*       intIndex    - The interrupt cause to get enable/disable status.
*
* OUTPUTS:
*       enablePtr   - (pointer to)GT_TRUE enable, GT_FALSE disable.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_VOID driverDxExMxEventsMaskGet
(
    IN   GT_U8   devNum,
    IN   GT_U32  intIndex,
    OUT  GT_BOOL *enablePtr
)
{
    GT_U32 maskIdx;             /* Index in the mask shadows array,     */
                                /* representing the register to which   */
                                /* this cause belongs.                  */
    GT_U32 *intMaskShadow;      /* Holds the mask shadow array pointer. */
    GT_U32 causeBitIdx;         /* Index in the cause register, of the  */
                                /* bit representing the given cause.    */
    GT_U32  portGroupId;        /*port group Id*/


    portGroupId = prvCpssDrvPpConfig[devNum]->portGroupsInfo.firstActivePortGroup;

    /* defaults */
    *enablePtr = GT_FALSE;

    intMaskShadow   = prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].intMaskShadow;
    /* Interrupts mechanism isn't initialized -> return GT_FALSE*/
    if (intMaskShadow == NULL)
    {
        return;
    }
    maskIdx         = intIndex >> 5;
    causeBitIdx     = intIndex & 0x1F;

    if ((intMaskShadow[maskIdx] & (1 << causeBitIdx)) != 0)
    {
        /* found unmasked */
        *enablePtr = GT_TRUE;
    }

}

/*******************************************************************************
* driverInitIntMasks
*
* DESCRIPTION:
*      Set the interrupt mask for a given device.
*
* INPUTS:
*       devNum      - The Pp's device number.
*       portGroupId      - The port group Id
*       b4StartInit - Is the call to this function is done before / after start
*                     Init.
*
* OUTPUTS:
*      None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*      None.
*
*******************************************************************************/
static GT_STATUS driverDxExMxInitIntMasks
(
    IN  GT_U8   devNum,
    IN  GT_U32  portGroupId,
    IN  GT_BOOL b4StartInit
)
{
    PRV_CPSS_DRV_EV_REQ_NODE_STC     *intReqNodeList;
    GT_U32          *intMaskShadow;
    GT_U32          numOfIntBits;       /* Number of interrupt bits.    */
    GT_U32          i;
    GT_STATUS       rc, ret = GT_OK;

    if(prvCpssDrvPpConfig[devNum] == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_SUCH, LOG_ERROR_NO_MSG);
    }

    intMaskShadow   = prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].intMaskShadow;
    numOfIntBits    = prvCpssDrvPpConfig[devNum]->intCtrl.numOfIntBits;

    intReqNodeList = prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].intNodesPool;

    for(i = 0; i < (numOfIntBits >> 5); i++)
    {
        if(intReqNodeList[i << 5].intMaskReg == CPSS_EVENT_SKIP_MASK_REG_ADDR_CNS)
        {
            /* the entry should be fully ignored !!! */
            continue;
        }

        if(prvCpssDrvPpConfig[devNum]->intCtrl.accessB4StartInit[i] == b4StartInit)
        {
            if((intReqNodeList[i << 5].intMaskReg !=
                prvCpssDrvPpConfig[devNum]->intCtrl.intSumMaskRegAddr) ||
                (0 == PRV_CPSS_DRV_HW_IF_PCI_COMPATIBLE_MAC(devNum)))
            {
                rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,
                                  intReqNodeList[i << 5].intMaskReg,
                                  intMaskShadow[i]);
            }
            else
            {
                rc = prvCpssDrvHwPpPortGroupWriteInternalPciReg(devNum,portGroupId,intReqNodeList[i << 5].intMaskReg,
                                        intMaskShadow[i]);
            }

            if (rc != GT_OK)
            {
                ret = GT_FAIL;
            }
        }
    }
    return ret;
}


/*******************************************************************************
* prvCpssDrvDxExMxGppIsrConnect
*
* DESCRIPTION:
*       This function connects an Isr for a given Gpp interrupt.
*
* INPUTS:
*       devNum      - The Pp device number at which the Gpp device is connected.
*       gppId       - The Gpp Id to be connected.
*       isrFuncPtr  - A pointer to the function to be called on Gpp interrupt
*                     reception.
*       cookie      - A cookie to be passed to the isrFuncPtr when its called.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       1.  To disconnect a Gpp Isr, call this function with a NULL parameter in
*           the isrFuncPtr param.
*
*******************************************************************************/
GT_STATUS prvCpssDrvDxExMxGppIsrConnect
(
    IN  GT_U8           devNum,
    IN  CPSS_EVENT_GPP_ID_ENT  gppId,
    IN  CPSS_EVENT_ISR_FUNC      isrFuncPtr,
    IN  void            *cookie
)
{
    if((prvCpssDrvPpConfig[devNum] == NULL) ||
       (gppId >= CPSS_EVENT_GPP_MAX_NUM_E))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    prvCpssDrvPpConfig[devNum]->intCtrl.gppIsrFuncs[gppId].gppIsrPtr  = isrFuncPtr;
    prvCpssDrvPpConfig[devNum]->intCtrl.gppIsrFuncs[gppId].cookie     = cookie;

    return GT_OK;
}



/*******************************************************************************
* prvCpssDrvDxExMxGppIsrCall
*
* DESCRIPTION:
*       Call the isr connected to a specified gpp.
*
* INPUTS:
*       devNum  - The Pp device number.
*       gppId   - The GPP Id to call the Isr for.
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
GT_STATUS prvCpssDrvDxExMxGppIsrCall
(
    IN  GT_U8       devNum,
    IN  CPSS_EVENT_GPP_ID_ENT   gppId
)
{
    CPSS_EVENT_ISR_FUNC    gppIsrFunc;

    if((prvCpssDrvPpConfig[devNum] == NULL) ||
       (gppId >= CPSS_EVENT_GPP_MAX_NUM_E))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }


    gppIsrFunc = prvCpssDrvPpConfig[devNum]->intCtrl.gppIsrFuncs[gppId].gppIsrPtr;
    if(gppIsrFunc)
    {
        gppIsrFunc(prvCpssDrvPpConfig[devNum]->intCtrl.gppIsrFuncs[gppId].cookie);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortGroupGopIsrRegRead
*
* DESCRIPTION:
*       Read ports GOP0-3 MIBs Interrupt Cause register value
*       for the specific port group
*
* INPUTS:
*       devNum  - The PP to read from.
*       portGroupId  - port group Id.
*       regAddr - The register's address to read from.
*
* OUTPUTS:
*       dataPtr - Includes the register value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortGroupGopIsrRegRead
(
    IN GT_U8 devNum,
    IN GT_U32  portGroupId,
    IN GT_U32 regAddr,
    IN GT_U32 *dataPtr
)
{
    GT_STATUS rc;

    rc = prvCpssDrvHwPpPortGroupIsrRead(devNum, portGroupId,regAddr, dataPtr);
    if(rc == GT_OK)
    {
        /* The GOP interrupt cause bits starts from bit 24.
           The correspondent GOP interrupt mask bits and interrupt enum
           starts as usual from bit 0.
           To synch value of cause reg with mask one and enum move value
           of register from bit 24 to bit 0.*/
        *dataPtr >>= 24;
    }

    return rc;
}

/*******************************************************************************
* prvCpssDxChGopIsrRegRead
*
* DESCRIPTION:
*       Read ports GOP0-3 MIBs Interrupt Cause register value
*
* INPUTS:
*       devNum  - The PP to read from.
*       regAddr - The register's address to read from.
*
* OUTPUTS:
*       dataPtr - Includes the register value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChGopIsrRegRead
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr,
    IN GT_U32 *dataPtr
)
{
    return prvCpssDxChPortGroupGopIsrRegRead(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr,dataPtr);
}


/*******************************************************************************
* driverDxExMxEventsInitObject
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
GT_STATUS prvCpssDrvDxExMxEventsInitObject
(
     void
)
{
    /* driver object initialization */
    prvCpssDrvGenExMxDxObjPtr->drvHwPpEventsMask     = driverDxExMxEventsMask;
    prvCpssDrvGenExMxDxObjPtr->drvHwPpEventsMaskGet  = driverDxExMxEventsMaskGet;
    prvCpssDrvGenExMxDxObjPtr->drvHwPpInitInMasks    = driverDxExMxInitIntMasks;

    return GT_OK;
}

