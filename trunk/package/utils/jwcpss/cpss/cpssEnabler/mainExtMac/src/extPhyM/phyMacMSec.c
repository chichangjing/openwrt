/*******************************************************************************
*              (c), Copyright 2010, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* phyMacMSec.c
*
* DESCRIPTION:
*       
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*
*******************************************************************************/
#include <cpss/generic/extMac/cpssExtMacDrv.h> 
#include <cpss/generic/phy/private/prvCpssGenPhySmi.h> 
#include <gtOs/gtGenTypes.h>
#include <cpss/extServices/private/prvCpssBindFunc.h> 
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h> 
#include <cpss/generic/smi/cpssGenSmi.h> 

#include <extPhyM/phyMacInit.h> 
#include <extPhyM/phyMacRegs1540M.h> 
#include <extPhyM/phyMacMSec.h> 

#include <gtOs/gtOsIo.h>

int osPrintf(const char* format, ...);

extern CPSS_SMI_REGISTER_WRITE_FUNC  macPhy1540MSmiWriteRegister;
extern CPSS_SMI_REGISTER_READ_FUNC  macPhy1540MSmiReadRegister;

extern CPSS_SMI_START_TRANSACTION_FUNC  macPhy1540MSmiStartTransaction;
extern CPSS_SMI_FINISH_TRANSACTION_FUNC  macPhy1540MSmiFinishTransaction;


extern phy1540MDataBase *phy1540DataBasePtr;

/*******************************************************************************
* macDrvGetSmiAddress
*
* DESCRIPTION:
*       - returns the slice number for cpecified devNum and portNum
*
* INPUTS:
*       - devNum - device number
*       - portNum - port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS macDrvGetSmiAddress
(
    IN GT_U8 devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_U32 *smiAddress
)
{
    GT_U32 i;
    GT_U32 dbSize = phy1540DataBasePtr->phyInfoDBSize;

    /* copy data base by items */
    for (i = 0; i < dbSize; i++) {
        if (phy1540DataBasePtr->phyInfoPtr[i].devNum == devNum &&
            phy1540DataBasePtr->phyInfoPtr[i].portNum == portNum) 
        {
            *smiAddress = phy1540DataBasePtr->phyInfoPtr[i].smiAddr;
            return GT_OK;
        }
    }
        /* Wrong parameters - devNum or portNum  */
    osPrintf("  >> MACSEC ERROR macDrvGetSmiAddress - wrong devNum: %d or devPort : %d\n",
                                                devNum,portNum);
    return GT_FAIL;
}
/*******************************************************************************
* macDrvGetSlice
*
* DESCRIPTION:
*       - returns the slice number for cpecified devNum and portNum
*
* INPUTS:
*       - devNum - device number
*       - portNum - port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS macDrvGetSlice
(
    IN GT_U8 devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_U32 *sliceOffset
)
{
    GT_U32 i;
    GT_U32 dbSize = phy1540DataBasePtr->phyInfoDBSize;
    GT_U32 sliceNum;

    

    /* copy data base by items */
    for (i = 0; i < dbSize; i++) {
        if (phy1540DataBasePtr->phyInfoPtr[i].devNum == devNum &&
            phy1540DataBasePtr->phyInfoPtr[i].portNum == portNum) 
        {
            sliceNum = phy1540DataBasePtr->phyInfoPtr[i].phySlice;

            switch (sliceNum)
            {
                case 0 : 
                    *sliceOffset = PHYMAC_1540_SLICE0; 
                    return GT_OK;
                case 1 : 
                    *sliceOffset = PHYMAC_1540_SLICE1; 
                    return GT_OK;
                case 2 : 
                    *sliceOffset = PHYMAC_1540_SLICE2; 
                    return GT_OK;
                case 3 : 
                    *sliceOffset = PHYMAC_1540_SLICE3; 
                    return GT_OK;
                default:
                    osPrintf("Do not supported sliceNum %d \n", sliceNum);
                    return GT_FAIL;

            }
        }
    }
    /* Wrong parameters - devNum or portNum  */
    osPrintf("  >> MACSEC ERROR macDrvGetSlice - wrong devNum: %d or devPort : %d\n",
                                            devNum,portNum);
    return GT_FAIL;
}

/*******************************************************************************
* phyMacMSecWriteRegField
*
* DESCRIPTION:
*       This function writes data Value to MacSec register address.
*
* page number 16 and register 0~3 are defined for indirect access to MACsec address space:
*     reg 0 - ReadAddr
*     reg 1 - WriteAddr
*     reg 2 - Data low 16 bits
*     reg 3 - Data high 16 bits
*
* INPUTS:
*    IN GT_U8   devNum   -  device number  
*    IN GT_U32  regAddr  -  register address 
*    IN GT_U32  data     -  value to write
*    IN GT_U32  bitmap   -  bitmap of the feld specified
*   
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS phyMacMSecWriteRegField
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    IN GT_U16  regAddr,
    IN GT_U32  data,
    IN GT_U32  bitmap
)
{
    GT_STATUS rc;

  /*  GT_U8   devNum */
    GT_PORT_GROUPS_BMP  portGroupsBmp = 0x0;
    CPSS_PHY_SMI_INTERFACE_ENT   smiInterface = CPSS_PHY_SMI_INTERFACE_0_E;
    GT_U32  smiAddr,regData;
    GT_U16  lowData, highData;

    rc = macDrvGetSmiAddress(devNum,portNum,&smiAddr);
    if (rc != GT_OK)
    {
        osPrintf("  >> MACSEC ERROR Failed to get Smi Address - rc - %d\n",rc);
        return rc;
    }

    /* Set page address 16 in page register ( reg 22 ) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,22,16);
    if (rc != GT_OK) 
    {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* Set Read Address in register 0 of current page (16) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,0,regAddr);
    if (rc != GT_OK) 
    {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* Read Low data   */
    rc = macPhy1540MSmiReadRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,(GT_U32)2,&lowData);
    if (rc != GT_OK) 
    {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* Read High data   */
    rc = macPhy1540MSmiReadRegister(devNum,portGroupsBmp,smiInterface,
                                    smiAddr,3,&highData);
    if (rc != GT_OK) 
    {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }
    /* set read data  */
    regData = (GT_U32)(highData);
    regData = ((regData & 0xFFFF) << 16 ) + (lowData & 0xFFFF);

    /* set value to write */
    data = data & bitmap;
    regData = regData & ~bitmap;
    regData = regData | data;

    /* Write Address in register 1 of current page (16) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,1,regAddr);
    if (rc != GT_OK) 
    {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* Write to DataL register  */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,2,(GT_U16)(regData & 0xFFFF));
    if (rc != GT_OK) 
    {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* Write to DataH register  */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,3,(GT_U16)(regData >> 16));
    if (rc != GT_OK) 
    {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }
        /* go to page 0 */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,22,0);

    if (rc != GT_OK)
    {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* phyMacMSecReadRegField
*
* DESCRIPTION:
*       This function reads data Value from MacSec register address.
*
* page number 16 and register 0~3 are defined for indirect access to MACsec address space:
*     reg 0 - ReadAddr
*     reg 1 - WriteAddr
*     reg 2 - Data low 16 bits
*     reg 3 - Data high 16 bits
*
* INPUTS:
*    IN GT_U8   devNum   -  device number  
*    IN GT_U16  regAddr  -  register address 
*    IN GT_U32  data     -  value to write
*    IN GT_U32  bitmap   -  bitmap of the feld specified
*   
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS phyMacMSecReadRegField
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    IN GT_U16  regAddr,
    IN GT_U32  *readData,
    IN GT_U32  bitmap
)
{
    GT_STATUS rc;

  /*  GT_U8   devNum */
    GT_PORT_GROUPS_BMP  portGroupsBmp = 0x0;
    CPSS_PHY_SMI_INTERFACE_ENT   smiInterface = CPSS_PHY_SMI_INTERFACE_0_E;
    GT_U32  smiAddr,regData;
    GT_U16  lowData, highData;

    rc = macDrvGetSmiAddress(devNum,portNum,&smiAddr);
    if (rc != GT_OK)
    {
        osPrintf("  >> MACSEC ERROR Failed to get Smi Address - rc - %d\n",rc);
        return rc;
    }

    /* Set page address 16 in page register ( reg 22 ) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,22,16);
    if (rc != GT_OK) 
    {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* Set Read Address in register 0 of current page (16) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,0,regAddr);
    if (rc != GT_OK) 
    {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* Read Low data   */
    rc = macPhy1540MSmiReadRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,2,&lowData);
    if (rc != GT_OK) 
    {
        osPrintf("  >> MACSEC ERROR Failed to read register - rc - %d\n",rc);
        return rc;
    }

    /* Read High data   */
    rc = macPhy1540MSmiReadRegister(devNum,portGroupsBmp,smiInterface,
                                    smiAddr,3,&highData);
    if (rc != GT_OK) 
    {
        osPrintf("  >> MACSEC ERROR Failed to read register - rc - %d\n",rc);
        return rc;
    }

    /* set read data  */
    regData = (GT_U32)(highData);
    regData = ((regData & 0xFFFF) << 16 ) + (lowData & 0xFFFF);

    /* bitmap selection  */
    regData = regData & bitmap;

    *readData = regData;

    /* go to page 0 */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,22,0);

    if (rc != GT_OK)
    {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }
    return GT_OK;

}


/*******************************************************************************
* phyMacMSecStartCnfgTransaction
*
* DESCRIPTION:
*       This function prepare the MAC to configuration operation.
*           The steps:
*           - stop polling in ASIC
*           - link down in PHY
*       
*
* INPUTS:
*
*
*   
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/

static GT_STATUS phyMacMSecStartCnfgTransaction
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_BOOL *pollStat
)
{
    GT_STATUS rc;

    /**************************************** 
    stop polling in ASIC:
        - get current status
        - if the polling in ASIC runs stop it  
        - store the currect polling status
    ******************************************/

    /* start transaction & get polling status */
    rc = macPhy1540MSmiStartTransaction(devNum,portNum,pollStat);
    if (rc != GT_OK) 
    {
        osPrintf("  >> MACSEC ERROR macPhy1540MSmiStartTransaction return %d \n",rc);
        return rc;
    }
    /**************************************** 
        Set Link Down in SS MAC and PS MAC:
            - 
            - if the polling in ASIC runs stop it  
            - store the currect polling status
    ******************************************/

  /* Set link down for SS MAC  
    rc = phyMacMSecWriteRegField(devNum,portNum,PHYMAC_1540_SYS_HIDDEN0,
                                 0x0,PHYMAC_1540_LINK_ENABLE_BITMAP);
   */ 

    return GT_OK;
}

static GT_STATUS phyMacMSecFinishCnfgTransaction
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_BOOL pollStat
)
{
    GT_STATUS rc;

    /**************************************** 
    stop polling in ASIC:
        - get current status
        - if the polling in ASIC runs stop it  
        - store the currect polling status
    ******************************************/

    rc = macPhy1540MSmiFinishTransaction(devNum,portNum,pollStat);
    if (rc != GT_OK) 
    {
        osPrintf("  >> MACSEC ERROR cpssDxChPhyAutonegSmiSet return %d \n",rc);
        return rc;
    }
    /**************************************** 
        Set Link Up in SS MAC and PS MAC:
            - 
            - if the polling in ASIC runs stop it  
            - store the currect polling status
    ******************************************/

    /* Set link down for SS MAC  
    rc = phyMacMSecWriteRegField(devNum,portNum,PHYMAC_1540_SYS_HIDDEN0,
                                 0x1,PHYMAC_1540_LINK_ENABLE_BITMAP);
    */

    return GT_OK;
}

/*******************************************************************************
* phyMacMSecCleanAfterFail
*
* DESCRIPTION:
*       This function: 
*           - sets page 0 in PHY
*           - starts polling in ASIC
*       
*
* INPUTS:
*    GT_U8    devNum 
*    GT_U8    portNum
*    GT_BOOL  pollStat  - polling status 
*   
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/

static GT_STATUS phyMacMSecCleanAfterFail
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum
)
{
    GT_STATUS rc;
    GT_BOOL pollStatus = GT_TRUE;

    rc = phyMacMSecFinishCnfgTransaction(devNum,portNum,pollStatus);
    return rc;
}

/*******************************************************************************
* phyMacMSecSetSpeed (1)
*
* DESCRIPTION:
*       This function sets speed to PHY SIDE MAC and SWITCH SIDE MAC of 
*       1540M PHY.
*
*
* INPUTS:
*
*    devNum           - device number    
*    portNum          - port number
*    ssSpeed          - speed value to set in Switch Side MAC
*    psStage          - speed value to set in Phy Side MAC   
*    sideAction       - side actions (SS or PS or BOTH) 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*
*******************************************************************************/
GT_STATUS phyMacMSecSetSpeed
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM        portNum,
    IN CPSS_PORT_SPEED_ENT  ssSpeed,
    IN CPSS_PORT_SPEED_ENT  psSpeed,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
)
{
    GT_STATUS rc;
    GT_U16    ssRegAddr,psRegAddr;
    GT_U32    sliceOffset;
    GT_BOOL   pollStatus = GT_TRUE;

    /*********************************
        - get slice
        - set parameters
        - start transaction
        - set SS MAC
        - set PS MAC
        - finish transaction
    **********************************/
    
    /* get slice offset */
    rc = macDrvGetSlice(devNum,portNum,&sliceOffset);
    if (rc != GT_OK) 
    {
        osPrintf("  >> MACSEC ERROR phyMacMSecSetSpeed - macDrvGetSlice return %d \n",rc);
        return rc;
    }
    /* Phy Side MAC speed set */
    if (sideAction == PHYMAC_PHY_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {
        /* set reg address for PS MAC */
        psRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_WIR_AUTO_NEG_CONTROL);

        /* start transaction  */
        rc = phyMacMSecStartCnfgTransaction(devNum,portNum,&pollStatus);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }

        /*  set speed for Phy Side MAC */
        rc = phyMacMSecWriteRegField(devNum,portNum,psRegAddr,
                             psSpeed,PHYMAC_1540_SPEED_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
    }
    /* Switch Side MAC speed set */
    if (sideAction == PHYMAC_SWITCH_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {
        /* set reg address for SS MAC */
        ssRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_SYS_AUTO_NEG_CONTROL);

        /*  set speed for Switch Side MAC */
        rc = phyMacMSecWriteRegField(devNum,portNum,ssRegAddr,
                             ssSpeed,PHYMAC_1540_SPEED_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
    }
    /* finish  transaction  */
    rc = phyMacMSecFinishCnfgTransaction(devNum,portNum,pollStatus);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* phyMacMSecGetSpeed (2)
*
*       This function returns speed values from PHY SIDE MAC and SWITCH SIDE MAC of 
*       1540M PHY.
*
* INPUTS:
*
*    devNum           - device number    
*    portNum          - port number
*    sideAction       - side actions (SS or PS or BOTH) 
*
* OUTPUTS:
*    *ssSpeed          - speed ptr to get the value from Switch Side MAC
*    *psStage          - speed ptr to get the value from Phy Side MAC   
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS phyMacMSecGetSpeed
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    OUT CPSS_PORT_SPEED_ENT  *ssSpeed,
    OUT CPSS_PORT_SPEED_ENT  *psSpeed,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction

)
{
    GT_STATUS rc;
    GT_U16    ssRegAddr,psRegAddr;
    GT_U32    sliceOffset;
    GT_BOOL   pollStatus = GT_TRUE;
    GT_U32    ssReadValue = 0; 
    GT_U32    psReadValue = 0; 

    /*********************************
        - get slice
        - set parameters
        - start transaction
        - set SS MAC
        - set PS MAC
        - finish transaction
    **********************************/

    /* get slice offset */
    rc = macDrvGetSlice(devNum,portNum,&sliceOffset);

    /* Phy Side MAC speed set */
    if (sideAction == PHYMAC_PHY_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {
        /* set reg address for PS MAC */
        psRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_WIR_AUTO_NEG_CONTROL);

        /* start transaction  */
        rc = phyMacMSecStartCnfgTransaction(devNum,portNum,&pollStatus);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }

        /*  set speed for Phy Side MAC */
        rc = phyMacMSecReadRegField(devNum,portNum,psRegAddr,
                             &psReadValue,PHYMAC_1540_SPEED_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
        switch (psReadValue)
        {
            case 0 : 
                *psSpeed = CPSS_PORT_SPEED_10_E;
                break;
            case 1 : 
                *psSpeed = CPSS_PORT_SPEED_100_E;
                break;
            case 2 : 
                *psSpeed = CPSS_PORT_SPEED_1000_E;
                break;
            default:
                rc = phyMacMSecCleanAfterFail(devNum,portNum);
                return rc;
        }
    }
    /* Switch Side MAC speed set */
    if (sideAction == PHYMAC_SWITCH_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {

        /* set reg address for SS MAC */
        ssRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_SYS_AUTO_NEG_CONTROL);

        /*  set speed for Switch Side MAC */
        rc = phyMacMSecReadRegField(devNum,portNum,ssRegAddr,
                             &ssReadValue,PHYMAC_1540_SPEED_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
        switch (ssReadValue)
        {
            case 0 : 
                *ssSpeed = CPSS_PORT_SPEED_10_E;
                break;
            case 1 : 
                *ssSpeed = CPSS_PORT_SPEED_100_E;
                break;
            case 2 : 
                *ssSpeed = CPSS_PORT_SPEED_1000_E;
                break;
            default:
                rc = phyMacMSecCleanAfterFail(devNum,portNum);
                return rc;
        }
    }

    /* finish  transaction  */
    rc = phyMacMSecFinishCnfgTransaction(devNum,portNum,pollStatus);
    if (rc != GT_OK) 
    {
        osPrintf("  >> MACSEC ERROR phyMacMSecGetSpeed - phyMacMSecFinishCnfgTransaction return %d \n",rc);
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* phyMacMSecSetANDuplex (3)
*
* DESCRIPTION:
*       This function sets duplex Auto Negotiation state to MacSec 
*       Switch Side & Phy Side MACs
*
* INPUTS:
*
*    devNum           - device number    
*    portNum          - port number
*    ssState          - duplex state to set the value from Switch Side MAC
*    psState          - duplex state to set the value from Phy Side MAC   
*    sideAction       - side actions (SS or PS or BOTH) 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS phyMacMSecSetANDuplex
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    IN GT_BOOL ssState,
    IN GT_BOOL psState,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
)
{
    GT_STATUS rc;
    GT_U16  ssRegAddr,psRegAddr;
    GT_U32  sliceOffset;
    GT_BOOL pollStatus = GT_TRUE;

    /*********************************
        - get slice
        - set parameters
        - start transaction
        - set SS MAC
        - set PS MAC
        - finish transaction
    **********************************/
    
    /* get slice offset */
    rc = macDrvGetSlice(devNum,portNum,&sliceOffset);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }

    /* Phy Side MAC speed set */
    if (sideAction == PHYMAC_PHY_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {
        /* set reg address for PS MAC */
        psRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_WIR_AUTO_NEG_CONTROL);

        /* start transaction  */
        rc = phyMacMSecStartCnfgTransaction(devNum,portNum,&pollStatus);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }

        /*  set AN duplex enable for Phy Side MAC */
        rc = phyMacMSecWriteRegField(devNum,portNum,psRegAddr,
                             (psState==GT_TRUE)?PHYMAC_1540_AN_SPEED_ENABLE_BITMAP:0,PHYMAC_1540_AN_SPEED_ENABLE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
    }
    /* Switch Side MAC AN duplex enable set */
    if (sideAction == PHYMAC_SWITCH_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {

        /* set reg address for SS MAC */
        ssRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_SYS_AUTO_NEG_CONTROL);

        /*  set AN duplex for Switch Side MAC */
         rc = phyMacMSecWriteRegField(devNum,portNum,ssRegAddr,
                             (ssState==GT_TRUE)?PHYMAC_1540_AN_SPEED_ENABLE_BITMAP:0,PHYMAC_1540_AN_SPEED_ENABLE_BITMAP);
      if (rc != GT_OK) 
        {
          rc = phyMacMSecCleanAfterFail(devNum,portNum);
          return rc;
        }
    }
    /* finish  transaction  */
    rc = phyMacMSecFinishCnfgTransaction(devNum,portNum,pollStatus);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* phyMacMSecGetANDuplex (4)
*
* DESCRIPTION:
*       This function returns AN duplex values from MacSec SS & PS MACs.
*
* INPUTS:
*
*    devNum     - device number
*    portNum    - port number
*    sideAction - side actions (SS or PS or BOTH) 
*   
* OUTPUTS:
*    *ssState   - SS MAC duplex state ptr
*    *psState   - PS MAC duplex state ptr
*
* RETURNS:
*       GT_OK          - on success
*       GT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS phyMacMSecGetANDuplex
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_BOOL *ssState,
    OUT GT_BOOL *psState,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
)
{
    GT_STATUS rc;
    GT_U16  ssRegAddr,psRegAddr;
    GT_U32  sliceOffset;
    GT_BOOL pollStatus = GT_TRUE;
    GT_U32  ssReadValue = 0;
    GT_U32  psReadValue = 0;

    /*********************************
        - get slice
        - set parameters
        - start transaction
        - set SS MAC
        - set PS MAC
        - finish transaction
    **********************************/

    /* get slice offset */
    rc = macDrvGetSlice(devNum,portNum,&sliceOffset);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }

    /* Phy Side MAC AN duplex enable get */
    if (sideAction == PHYMAC_PHY_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {
        /* set reg address for PS MAC */
        psRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_WIR_AUTO_NEG_CONTROL);

        /* start transaction  */
        rc = phyMacMSecStartCnfgTransaction(devNum,portNum,&pollStatus);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }

        /*  read duplex AN enable value from Phy Side MAC */
        rc = phyMacMSecReadRegField(devNum,portNum,psRegAddr,
                             &psReadValue,PHYMAC_1540_AN_SPEED_ENABLE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
        *psState = psReadValue?GT_TRUE:GT_FALSE;

    }
    /* Switch Side MAC AN duplex enable get */
    if (sideAction == PHYMAC_SWITCH_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {

        /* set reg address for SS MAC */
        ssRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_SYS_AUTO_NEG_CONTROL);

        /*  get AN duplex enable for Switch Side MAC */
        rc = phyMacMSecReadRegField(devNum,portNum,ssRegAddr,
                             &ssReadValue,PHYMAC_1540_AN_SPEED_ENABLE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
        *ssState = ssReadValue?GT_TRUE:GT_FALSE;

    }
    /* finish  transaction  */
    rc = phyMacMSecFinishCnfgTransaction(devNum,portNum,pollStatus);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* phyMacMSecSetANFlowControl (5)
*
* DESCRIPTION:
*       This function sets flow control parameters to Switch Side & 
*       Phy Side MACSEC MACs
*
* INPUTS:
*
*    devNum           - device number    
*    portNum          - port number
*    ssState          - flow control state to set the value to Switch Side MAC
*    ssPauseAdvertise - pause advertise (symmetric/assimetric) to set 
*                       the value to Switch Side MAC
*    psState          - flow control state to set the value to Phy Side MAC   
*    psPauseAdvertise - pause advertise (symmetric/assimetric) to set 
*                       the value to Switch Side MAC
*    sideAction       - side actions (SS or PS or BOTH) 
*   
* OUTPUTS:
*       None
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS phyMacMSecSetANFlowControl
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    IN GT_BOOL ssState,
    IN GT_BOOL ssPauseAdvertise,
    IN GT_BOOL psState,
    IN GT_BOOL psPauseAdvertise,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
)
{
    GT_STATUS rc;
    GT_U16    ssRegAddr,psRegAddr;
    GT_U32    sliceOffset;
    GT_BOOL   pollStatus = GT_TRUE;

    /*********************************
        - get slice
        - set parameters
        - start transaction
        - set SS MAC
        - set PS MAC
        - finish transaction
    **********************************/
    
    /* get slice offset */
    rc = macDrvGetSlice(devNum,portNum,&sliceOffset);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }

    /* Phy Side MAC speed set */
    if (sideAction == PHYMAC_PHY_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {
        /* set reg address for PS MAC */
        psRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_WIR_AUTO_NEG_CONTROL);

        /* start transaction  */
        rc = phyMacMSecStartCnfgTransaction(devNum,portNum,&pollStatus);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }

        /*  set AN duplex for Phy Side MAC */
        rc = phyMacMSecWriteRegField(devNum,portNum,psRegAddr,
                                    (psState==GT_TRUE)?PHYMAC_1540_AN_FC_PAUSE_ENABLE_BITMAP: 0,
                                    PHYMAC_1540_AN_FC_PAUSE_ENABLE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
    }
    /* Switch Side MAC speed set */
    if (sideAction == PHYMAC_SWITCH_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {

        /* set reg address for SS MAC */
        ssRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_SYS_AUTO_NEG_CONTROL);

        /*  set AN FC enable/disable for Switch Side MAC */
        rc = phyMacMSecWriteRegField(devNum,portNum,ssRegAddr,
                             (ssState==GT_TRUE)?PHYMAC_1540_AN_FC_PAUSE_ENABLE_BITMAP:0,
                              PHYMAC_1540_AN_FC_PAUSE_ENABLE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
    }
    /* finish  transaction  */
    rc = phyMacMSecFinishCnfgTransaction(devNum,portNum,pollStatus);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* phyMacMSecGetANFlowControl (6)
*
* DESCRIPTION:
*       This function returns flow control parameters from Switch Side & 
*       Phy Side MACSEC MACs
*
* INPUTS:
*
*    devNum           - device number    
*    portNum          - port number
*    sideAction       - side actions (SS or PS or BOTH) 
*   
* OUTPUTS:
*    ssState          - flow control state to set the value to Switch Side MAC
*    ssPauseAdvertise - pause advertise (symmetric/assimetric) to set 
*                       the value to Switch Side MAC
*    psState          - flow control state to set the value to Phy Side MAC   
*    psPauseAdvertise - pause advertise (symmetric/assimetric) to set 
*
* RETURNS:
*       MAD_OK          - on success
*       MAD_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS phyMacMSecGetANFlowControl
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_BOOL *ssState,
    OUT GT_BOOL *ssPauseAdvertise,
    OUT GT_BOOL *psState,
    OUT GT_BOOL *psPauseAdvertise,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
)
{
    GT_STATUS rc;
    GT_U16  ssRegAddr,psRegAddr;
    GT_U32  sliceOffset;
    GT_BOOL pollStatus = GT_TRUE;
    GT_U32  ssReadValue = 0;
    GT_U32  psReadValue = 0;


    /*********************************
        - get slice
        - set parameters
        - start transaction
        - set SS MAC
        - set PS MAC 
        - finish transaction
    **********************************/
    
    /* get slice offset */
    rc = macDrvGetSlice(devNum,portNum,&sliceOffset);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }

    /* Phy Side MAC Get AN Flow Control */
    if (sideAction == PHYMAC_PHY_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {
        /* set reg address for PS MAC */
        psRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_WIR_AUTO_NEG_CONTROL);

        /* start transaction  */
        rc = phyMacMSecStartCnfgTransaction(devNum,portNum,&pollStatus);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }

        /*  Get AN Flow Control from Phy Side MAC */
        rc = phyMacMSecReadRegField(devNum,portNum,psRegAddr,
                             &psReadValue,PHYMAC_1540_AN_FC_PAUSE_ENABLE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }

        *psPauseAdvertise = *psState = psReadValue ? GT_TRUE : GT_FALSE;

    }
    /* Switch Side MAC speed set */
    if (sideAction == PHYMAC_SWITCH_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {

        /* set reg address for SS MAC */
        ssRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_SYS_AUTO_NEG_CONTROL);

        /*  set AN duplex for Switch Side MAC */
        rc = phyMacMSecReadRegField(devNum,portNum,ssRegAddr,
                             &ssReadValue,PHYMAC_1540_AN_FC_PAUSE_ENABLE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }

        *ssPauseAdvertise = *ssState = ssReadValue ? GT_TRUE : GT_FALSE;

    }
    /* finish  transaction  */
    rc = phyMacMSecFinishCnfgTransaction(devNum,portNum,pollStatus);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* phyMacMSecSetSpeedANEnable (7)
*
* DESCRIPTION:
*       This function enables/disables speed AN on Switch Side & 
*       Phy Side MACSEC MACs
*
* INPUTS:
*
*    devNum           - device number    
*    portNum          - port number
*    ssState          - flow control state to set the value to Switch Side MAC
*    psState          - flow control state to set the value to Phy Side MAC   
*    sideAction       - side actions (SS or PS or BOTH) 
*   
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS phyMacMSecSetSpeedANEnable
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    IN GT_BOOL ssState,
    IN GT_BOOL psState,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
)
{
    GT_STATUS rc;
    GT_U16  ssRegAddr,psRegAddr;
    GT_U32  sliceOffset;
    GT_BOOL pollStatus = GT_TRUE;

    /*********************************
        - get slice
        - set parameters
        - start transaction
        - set SS MAC
        - set PS MAC
        - finish transaction
    **********************************/
    
    /* get slice offset */
    rc = macDrvGetSlice(devNum,portNum,&sliceOffset);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }

    /* Phy Side MAC speed set */
    if (sideAction == PHYMAC_PHY_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {
        /* set reg address for PS MAC */
        psRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_WIR_AUTO_NEG_CONTROL);

        /* start transaction  */
        rc = phyMacMSecStartCnfgTransaction(devNum,portNum,&pollStatus);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }

        /*  set AN duplex for Phy Side MAC */
        rc = phyMacMSecWriteRegField(devNum,portNum,psRegAddr,
                             (psState==GT_TRUE)?PHYMAC_1540_AN_SPEED_ENABLE_BITMAP:0,PHYMAC_1540_AN_SPEED_ENABLE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
    }
    /* Switch Side MAC speed set */
    if (sideAction == PHYMAC_SWITCH_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {

        /* set reg address for SS MAC */
        ssRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_SYS_AUTO_NEG_CONTROL);

        /*  set AN duplex for Switch Side MAC */
         rc = phyMacMSecWriteRegField(devNum,portNum,ssRegAddr,
                             (ssState==GT_TRUE)?PHYMAC_1540_AN_SPEED_ENABLE_BITMAP:0,PHYMAC_1540_AN_SPEED_ENABLE_BITMAP);
      if (rc != GT_OK) 
        {
          rc = phyMacMSecCleanAfterFail(devNum,portNum);
          return rc;
        }
    }
    /* finish  transaction  */
    rc = phyMacMSecFinishCnfgTransaction(devNum,portNum,pollStatus);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* phyMacMSecGetSpeedANEnable (8)
*
* DESCRIPTION:
*       This function returns speed AN values from Switch Side & 
*       Phy Side MACSEC MACs
*
* INPUTS:
*
*    devNum           - device number    
*    portNum          - port number
*    sideAction       - side actions (SS or PS or BOTH) 
*   
* OUTPUTS:
*    ssState          - speed enable value from Switch Side MAC
*    psState          - speed enable value from Phy Side MAC   
*
* RETURNS:
*       GT_OK          - on success
*       GT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS phyMacMSecGetSpeedANEnable
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_BOOL *ssState,
    OUT GT_BOOL *psState,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
)
{
    GT_STATUS rc;
    GT_U16  ssRegAddr,psRegAddr;
    GT_U32  sliceOffset;
    GT_BOOL pollStatus = GT_TRUE;
    GT_U32  ssReadValue = 0;
    GT_U32  psReadValue = 0;

    /*********************************
        - get slice
        - set parameters
        - start transaction
        - set SS MAC
        - set PS MAC
        - finish transaction
    **********************************/
    
    /* get slice offset */
    rc = macDrvGetSlice(devNum,portNum,&sliceOffset);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }

    /* Phy Side MAC speed set */
    if (sideAction == PHYMAC_PHY_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {
        /* set reg address for PS MAC */
        psRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_WIR_AUTO_NEG_CONTROL);

        /* start transaction  */
        rc = phyMacMSecStartCnfgTransaction(devNum,portNum,&pollStatus);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }

        /*  read speed AN enable value from Phy Side MAC */
        rc = phyMacMSecReadRegField(devNum,portNum,psRegAddr,
                             &psReadValue,PHYMAC_1540_AN_SPEED_ENABLE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
        *psState = psReadValue?GT_TRUE:GT_FALSE;

    }
    /* Switch Side MAC speed set */
    if (sideAction == PHYMAC_SWITCH_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {

        /* set reg address for SS MAC */
        ssRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_SYS_AUTO_NEG_CONTROL);

        /*  set AN duplex for Switch Side MAC */
        rc = phyMacMSecReadRegField(devNum,portNum,ssRegAddr,
                             &ssReadValue,PHYMAC_1540_AN_SPEED_ENABLE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
        *ssState = ssReadValue?GT_TRUE:GT_FALSE;

    }
    /* finish  transaction  */
    rc = phyMacMSecFinishCnfgTransaction(devNum,portNum,pollStatus);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* phyMacMSecSetFlowControlEnable (9)
*
* DESCRIPTION:
*       This function sets flow control mode for Switch Side & 
*       Phy Side MACSEC MACs
*
* INPUTS:
*
*    devNum           - device number    
*    portNum          - port number
*    ssState          - flow control mode for Switch Side MAC
*    psState          - flow control mode for Phy Side MAC   
*    sideAction       - side actions (SS or PS or BOTH) 
*   
* OUTPUTS:
*
* RETURNS:
*       GT_OK          - on success
*       GT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS phyMacMSecSetFlowControlEnable
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    IN CPSS_PORT_FLOW_CONTROL_ENT ssState,
    IN CPSS_PORT_FLOW_CONTROL_ENT psState,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
)
{
    GT_STATUS rc;
    GT_U16  ssRegAddr,psRegAddr;
    GT_U32  sliceOffset;
    GT_BOOL  pollStatus = GT_TRUE;
    GT_U32  psRegValue;
    GT_U32  ssRegValue;

    /*********************************
        - get slice
        - set parameters
        - start transaction
        - set SS MAC
        - set PS MAC
        - finish transaction
    **********************************/
    
    /* get slice offset */
    rc = macDrvGetSlice(devNum,portNum,&sliceOffset);
    if (rc != GT_OK) 
    {
        osPrintf("  >>> MACSEC ERROR phyMacMSecGetSpeedANEnable - macDrvGetSlice return %d \n",rc);
        return rc;
    }

    /****************************************************************
        Flow control "enabled" configuration of PHYMAC defined as:
            - switch MAC - force Rx/Tx enabled
            - switch syde PHY MAC - forced Rx/Tx enabled
            - phy syde PHY MAC - forced for Rx/Tx enabled 

        Flow control "disabled" configuration of PHYMAC defined as:
            - switch MAC - force Rx/Tx enabled
            - switch syde PHY MAC - forced Rx/Tx enabled
            - phy syde PHY MAC - for Rx/Tx disabled 

    *****************************************************************/

    /* Phy Side MAC speed set */
    if (sideAction == PHYMAC_PHY_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {
        /* set reg address for PS MAC */
        psRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_WIR_AUTO_NEG_CONTROL);

        /* start transaction  */
        rc = phyMacMSecStartCnfgTransaction(devNum,portNum,&pollStatus);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }

        /* set FC AN disabled  */
        if (psState == CPSS_PORT_FLOW_CONTROL_DISABLE_E) {
            psRegValue = PHYMAC_1540_FLOW_CONTROL_RX_TX_DISABLED_BITMAP; 
        }
        else
        {
            psRegValue = PHYMAC_1540_FLOW_CONTROL_RX_TX_BITMAP; 
        }

        /*  set flow control mode for Phy Side MAC */
        rc = phyMacMSecWriteRegField(devNum,portNum,psRegAddr,
                             psRegValue,PHYMAC_1540_FLOW_CONTROL_MODE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
    }
    /* Switch Side MAC speed set */
    if (sideAction == PHYMAC_SWITCH_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {

        /* set reg address for SS MAC */
        ssRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_SYS_AUTO_NEG_CONTROL);

        /* set FC AN disabled  */
        if (ssState == CPSS_PORT_FLOW_CONTROL_DISABLE_E) {
            ssRegValue = PHYMAC_1540_FLOW_CONTROL_RX_TX_DISABLED_BITMAP; 
        }
        else
        {
            ssRegValue = PHYMAC_1540_FLOW_CONTROL_RX_TX_BITMAP; 
        }

        /*  set AN duplex for Switch Side MAC */
        rc = phyMacMSecWriteRegField(devNum,portNum,ssRegAddr,
                        ssRegValue,PHYMAC_1540_FLOW_CONTROL_MODE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
    }
    /* finish  transaction  */
    rc = phyMacMSecFinishCnfgTransaction(devNum,portNum,pollStatus);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* phyMacMSecGetFlowControlEnable (10)
*
* DESCRIPTION:
*       This function returns flow control mode for Switch Side & 
*       Phy Side MACSEC MACs
*
* INPUTS:
*
*    devNum           - device number    
*    portNum          - port number
*    sideAction       - side actions (SS or PS or BOTH) 
*   
* OUTPUTS:
*    ssState          - flow control mode for Switch Side MAC
*    psState          - flow control mode for Phy Side MAC   
*
* RETURNS:
*       GT_OK          - on success
*       GT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS phyMacMSecGetFlowControlEnable
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    OUT CPSS_PORT_FLOW_CONTROL_ENT *ssState,
    OUT CPSS_PORT_FLOW_CONTROL_ENT *psState,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
)
{
    GT_STATUS rc;
    GT_U16  ssRegAddr,psRegAddr;
    GT_U32  sliceOffset;
    GT_BOOL pollStatus = GT_TRUE;
    GT_U32  psRegValue = 0;
    GT_U32  ssRegValue = 0;

    /*********************************
        - get slice
        - set parameters
        - start transaction
        - set SS MAC
        - set PS MAC
        - finish transaction
    **********************************/
    
    /* get slice offset */
    rc = macDrvGetSlice(devNum,portNum,&sliceOffset);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }

    /* Phy Side MAC speed set */
    if (sideAction == PHYMAC_PHY_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {
        /* set reg address for PS MAC */
        psRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_WIR_MAC_STATUS0);

        /* start transaction  */
        rc = phyMacMSecStartCnfgTransaction(devNum,portNum,&pollStatus);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
        
        /*  set flow control mode for Phy Side MAC */
        rc = phyMacMSecReadRegField(devNum,portNum,psRegAddr,
                             &psRegValue,PHYMAC_1540_FLOW_CONTROL_GET_RX_TX_DISABLED_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }

        *psState = psRegValue;
    }

    /* Switch Side MAC speed set */
    if (sideAction == PHYMAC_SWITCH_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {

        /* set reg address for SS MAC */
        ssRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_SYS_MAC_STATUS0);

        /*  set AN duplex for Switch Side MAC */
        rc = phyMacMSecReadRegField(devNum,portNum,ssRegAddr,
                        &ssRegValue,PHYMAC_1540_FLOW_CONTROL_GET_RX_TX_DISABLED_BITMAP);

        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
        *ssState = ssRegValue; 

    }
    /* finish  transaction  */
    rc = phyMacMSecFinishCnfgTransaction(devNum,portNum,pollStatus);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* macPhyPeriodFlowCntlSet (11)
*
* DESCRIPTION:
*       This function Enable/Disable transmits of periodic 802.3x flow control
*       for Switch Side & Phy Side MACSEC MACs
*
* INPUTS:
*
*    devNum           - device number    
*    portNum          - port number
*    ssState          - flow control mode for Switch Side MAC
*    psState          - flow control mode for Phy Side MAC   
*    sideAction       - side actions (SS or PS or BOTH) 
*   
* OUTPUTS:
*
* RETURNS:
*       GT_OK          - on success
*       GT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS phyMacMSecSetPeriodicFlowControlEnable
(
    IN GT_U8     devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    IN GT_BOOL   ssState,
    IN GT_BOOL   psState,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
)
{
    GT_STATUS rc;
    GT_U16  ssRegAddr = 0,psRegAddr = 0;
    GT_U32  sliceOffset;
    GT_BOOL  pollStatus = GT_TRUE;
    GT_U32  psRegValue = 0;
    GT_U32  ssRegValue = 0;

    /*********************************
        - get slice
        - set parameters
        - start transaction
        - set SS MAC
        - set PS MAC
        - finish transaction
    **********************************/
    
    /* get slice offset */
    rc = macDrvGetSlice(devNum,portNum,&sliceOffset);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }

    /* Phy Side MAC speed set */
    if (sideAction == PHYMAC_PHY_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {
        /* set reg address for PS MAC */
        psRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_WIR_MAC_CONTROL1);

        /* start transaction  */
        rc = phyMacMSecStartCnfgTransaction(devNum,portNum,&pollStatus);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
        /*  set flow control mode for Phy Side MAC */
        rc = phyMacMSecWriteRegField(devNum,portNum,psRegAddr,
                 psRegValue,PHYMAC_1540_PERIODIC_FLOW_CONTROL_ENABLE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
    }
    /* Switch Side MAC speed set */
    if (sideAction == PHYMAC_SWITCH_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {

        /* set reg address for SS MAC */
        ssRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_WIR_MAC_CONTROL1);

        /*  set AN duplex for Switch Side MAC */
        rc = phyMacMSecWriteRegField(devNum,portNum,ssRegAddr,
                  ssRegValue,PHYMAC_1540_PERIODIC_FLOW_CONTROL_ENABLE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
    }
    /* finish  transaction  */
    rc = phyMacMSecFinishCnfgTransaction(devNum,portNum,pollStatus);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* phyMacMSecGetPeriodicFlowControlEnable (12)
*
* DESCRIPTION:
*       This function gets status of periodic 802.3x flow control
*       for Switch Side & Phy Side MACSEC MACs
*
* INPUTS:
*
*    devNum           - device number    
*    portNum          - port number
*    sideAction       - side actions (SS or PS or BOTH) 
*   
* OUTPUTS:
*    ssState          - periodic flow control state for Switch Side MAC
*    psState          - periodic flow control state for Phy Side MAC   
*
* RETURNS:
*       GT_OK          - on success
*       GT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS phyMacMSecGetPeriodicFlowControlEnable
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_BOOL *ssState,
    OUT GT_BOOL *psState,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
)
{
    GT_STATUS rc;
    GT_U16  ssRegAddr,psRegAddr;
    GT_U32  sliceOffset;
    GT_BOOL pollStatus = GT_TRUE;
    GT_U32  ssReadValue = 0;
    GT_U32  psReadValue = 0;

    /*********************************
        - get slice
        - set parameters
        - start transaction
        - set SS MAC
        - set PS MAC
        - finish transaction
    **********************************/
    
    /* get slice offset */
    rc = macDrvGetSlice(devNum,portNum,&sliceOffset);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }

    /* Phy Side MAC speed set */
    if (sideAction == PHYMAC_PHY_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {
        /* set reg address for PS MAC */
        psRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_WIR_MAC_CONTROL1);

        /* start transaction  */
        rc = phyMacMSecStartCnfgTransaction(devNum,portNum,&pollStatus);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
        
        /*  set flow control mode for Phy Side MAC */
        rc = phyMacMSecReadRegField(devNum,portNum,psRegAddr,
                       &psReadValue,PHYMAC_1540_PERIODIC_FLOW_CONTROL_ENABLE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
        if (psReadValue == 0) 
            *psState = GT_FALSE;
        else
            *psState = GT_TRUE;
    }
    /* Switch Side MAC speed set */
    if (sideAction == PHYMAC_SWITCH_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {

        /* set reg address for SS MAC */
        ssRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_WIR_MAC_CONTROL1);

        /*  set AN duplex for Switch Side MAC */
        rc = phyMacMSecReadRegField(devNum,portNum,ssRegAddr,
                &ssReadValue,PHYMAC_1540_PERIODIC_FLOW_CONTROL_ENABLE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
    }
    /* finish  transaction  */
    if (ssReadValue == 0) 
        *ssState = GT_FALSE;
    else
        *ssState = GT_TRUE;

    rc = phyMacMSecFinishCnfgTransaction(devNum,portNum,pollStatus);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }
    return GT_OK;
}
/*******************************************************************************
* phyMacMSecSetBackPressureEnable (13)
*
* DESCRIPTION:
*       This function Enable/Disable back pressure on port in half duplex 
*       condition for Switch Side & Phy Side MACSEC MACs
*
* INPUTS:
*
*    devNum           - device number    
*    portNum          - port number
*    ssState          - back pressure mode for Switch Side MAC
*    psState          - back pressure mode for Phy Side MAC   
*    sideAction       - side actions (SS or PS or BOTH) 
*   
* OUTPUTS:
*
* RETURNS:
*       GT_OK          - on success
*       GT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS phyMacMSecSetBackPressureEnable
(
    IN GT_U8     devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_BOOL   ssState,
    IN GT_BOOL   psState,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
)
{
    GT_STATUS rc;
    GT_U16  ssRegAddr = 0,psRegAddr = 0;
    GT_U32  sliceOffset;
    GT_BOOL  pollStatus = GT_TRUE;
    GT_U32  psRegValue = 0;
    GT_U32  ssRegValue = 0;

    /*********************************
        - get slice
        - set parameters
        - start transaction
        - set SS MAC
        - set PS MAC
        - finish transaction
    **********************************/
    
    /* get slice offset */
    rc = macDrvGetSlice(devNum,portNum,&sliceOffset);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }

    /* Phy Side MAC speed set */
    if (sideAction == PHYMAC_PHY_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {
        /* set reg address for PS MAC */
        psRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_SYS_MAC_STATUS0);

        /* start transaction  */
        rc = phyMacMSecStartCnfgTransaction(devNum,portNum,&pollStatus);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
        /*  set flow control mode for Phy Side MAC */
        rc = phyMacMSecWriteRegField(devNum,portNum,psRegAddr,
                 psRegValue,PHYMAC_1540_PORT_BACKPRESSURE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
    }
    /* Switch Side MAC speed set */
    if (sideAction == PHYMAC_SWITCH_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {

        /* set reg address for SS MAC */
        ssRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_SYS_MAC_STATUS0);

        /*  set AN duplex for Switch Side MAC */
        rc = phyMacMSecWriteRegField(devNum,portNum,ssRegAddr,
                  ssRegValue,PHYMAC_1540_PORT_BACKPRESSURE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
    }
    /* finish  transaction  */
    rc = phyMacMSecFinishCnfgTransaction(devNum,portNum,pollStatus);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* phyMacMSecGetBackPressureEnable (14)
*
* DESCRIPTION:
*       This function gets status of backpressure on port
*       for Switch Side & Phy Side MACSEC MACs
*
* INPUTS:
*
*    devNum           - device number    
*    portNum          - port number
*    sideAction       - side actions (SS or PS or BOTH) 
*   
* OUTPUTS:
*    ssState          - back pressure state for Switch Side MAC
*    psState          - back pressure state for Phy Side MAC   
*
* RETURNS:
*       GT_OK          - on success
*       GT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS phyMacMSecGetBackPressureEnable
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_BOOL *ssState,
    OUT GT_BOOL *psState,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
)
{
    GT_STATUS rc;
    GT_U16  ssRegAddr,psRegAddr;
    GT_U32  sliceOffset;
    GT_BOOL pollStatus = GT_TRUE;
    GT_U32  ssReadValue = 0;
    GT_U32  psReadValue = 0;

    /*********************************
        - get slice
        - set parameters
        - start transaction
        - set SS MAC
        - set PS MAC
        - finish transaction
    **********************************/
    
    /* get slice offset */
    rc = macDrvGetSlice(devNum,portNum,&sliceOffset);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }

    /* Phy Side MAC speed set */
    if (sideAction == PHYMAC_PHY_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {
        /* set reg address for PS MAC */
        psRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_SYS_MAC_STATUS0);

        /* start transaction  */
        rc = phyMacMSecStartCnfgTransaction(devNum,portNum,&pollStatus);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
        
        /*  set flow control mode for Phy Side MAC */
        rc = phyMacMSecReadRegField(devNum,portNum,psRegAddr,
                       &psReadValue,PHYMAC_1540_PORT_BACKPRESSURE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
        if (psReadValue == 0) 
            *psState = GT_FALSE;
        else
            *psState = GT_TRUE;
    }
    /* Switch Side MAC speed set */
    if (sideAction == PHYMAC_SWITCH_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {

        /* set reg address for SS MAC */
        ssRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_SYS_MAC_STATUS0);

        /*  set AN duplex for Switch Side MAC */
        rc = phyMacMSecReadRegField(devNum,portNum,ssRegAddr,
                &ssReadValue,PHYMAC_1540_PORT_BACKPRESSURE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
    }
    /* finish  transaction  */
    if (ssReadValue == 0) 
        *ssState = GT_FALSE;
    else
        *ssState = GT_TRUE;

    rc = phyMacMSecFinishCnfgTransaction(devNum,portNum,pollStatus);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* phyMacMSecGetLinkStatus (15)
*
*       This function returns link status values from PHY SIDE MAC and SWITCH SIDE MAC of 
*       1540M PHY.
*
* INPUTS:
*
*    devNum           - device number    
*    portNum          - port number
*    sideAction       - side actions (SS or PS or BOTH) 
*
* OUTPUTS:
*    *ssSpeed          - speed ptr to get the value from Switch Side MAC
*    *psStage          - speed ptr to get the value from Phy Side MAC   
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS phyMacMSecGetLinkStatus
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_BOOL *ssLinkStatus,
    OUT GT_BOOL *psLinkStatus,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction

)
{
    GT_STATUS rc;
    GT_U16  ssRegAddr,psRegAddr;
    GT_U32  sliceOffset;
    GT_BOOL pollStatus = GT_TRUE;
    GT_U32  ssReadValue = 0;
    GT_U32  psReadValue = 0;

    /*********************************
        - get slice
        - set parameters
        - start transaction
        - set SS MAC
        - set PS MAC
        - finish transaction
    **********************************/

    /* get slice offset */
    rc = macDrvGetSlice(devNum,portNum,&sliceOffset);

    /* Phy Side MAC speed set */
    if (sideAction == PHYMAC_PHY_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {
        /* set reg address for PS MAC */
        psRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_WIR_HIDDEN0);

        /* start transaction  */
        rc = phyMacMSecStartCnfgTransaction(devNum,portNum,&pollStatus);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }

        /*  set speed for Phy Side MAC */
        rc = phyMacMSecReadRegField(devNum,portNum,psRegAddr,
                             &psReadValue,PHYMAC_1540_LINK_STATUS_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
        if (psReadValue == 0)
            *psLinkStatus = GT_FALSE;
        else
            *psLinkStatus = GT_TRUE;

    }
    /* Switch Side MAC speed set */
    if (sideAction == PHYMAC_SWITCH_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {

        /* set reg address for SS MAC */
        ssRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_SYS_HIDDEN0);

        /*  set speed for Switch Side MAC */
        rc = phyMacMSecReadRegField(devNum,portNum,ssRegAddr,
                             &ssReadValue,PHYMAC_1540_SPEED_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
        if (psReadValue == 0)
            *ssLinkStatus = GT_FALSE;
        else
            *ssLinkStatus = GT_TRUE;
    }

    /* finish  transaction  */
    rc = phyMacMSecFinishCnfgTransaction(devNum,portNum,pollStatus);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }
    return GT_OK;
}


/*******************************************************************************
* phyMacMSecSetDuplexMode  (16)
*
* DESCRIPTION:
*       This function sets duplex mode for PHY SIDE MAC 
*       and SWITCH SIDE MAC of 1540M PHY.
*
*
* INPUTS:
*
*    devNum             - device number    
*    portNum            - port number
*    ssDuplexMode       - duplex mode value to set in Switch Side MAC
*    psDuplexMode       - duplex mode value to set in Phy Side MAC   
*    sideAction         - side actions (SS or PS or BOTH) 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*
*******************************************************************************/
GT_STATUS phyMacMSecSetDuplexMode
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    IN GT_BOOL ssDuplexMode,
    IN GT_BOOL psDuplexMode,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
)
{
    GT_STATUS rc;
    GT_U16    ssRegAddr,psRegAddr;
    GT_U32    sliceOffset;
    GT_BOOL   pollStatus = GT_TRUE;

    /*********************************
        - get slice
        - set parameters
        - start transaction
        - set SS MAC
        - set PS MAC
        - finish transaction
    **********************************/

    /* get slice offset */
    rc = macDrvGetSlice(devNum,portNum,&sliceOffset);
    if (rc != GT_OK) 
    {
        osPrintf("  >> MACSEC ERROR phyMacMSecSetSpeed - macDrvGetSlice return %d \n",rc);
        return rc;
    }
    /* Phy Side MAC speed set */
    if (sideAction == PHYMAC_PHY_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {
        /* set reg address for PS MAC */
        psRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_WIR_AUTO_NEG_CONTROL);

        /* start transaction  */
        rc = phyMacMSecStartCnfgTransaction(devNum,portNum,&pollStatus);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }

        /*  set speed for Phy Side MAC */
        rc = phyMacMSecWriteRegField(devNum,portNum,psRegAddr,
                             psDuplexMode,PHYMAC_1540_PORT_DUPLEX_MODE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
    }
    /* Switch Side MAC speed set */
    if (sideAction == PHYMAC_SWITCH_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {
        /* set reg address for SS MAC */
        ssRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_SYS_AUTO_NEG_CONTROL);

        /*  set speed for Switch Side MAC */
        rc = phyMacMSecWriteRegField(devNum,portNum,ssRegAddr,
                             ssDuplexMode,PHYMAC_1540_PORT_DUPLEX_MODE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
    }
    /* finish  transaction  */
    rc = phyMacMSecFinishCnfgTransaction(devNum,portNum,pollStatus);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* phyMacMSecGetDuplexMode  (17)
*
* DESCRIPTION:
*       This function returns duplex mode enable/disable 
*       status for PHY SIDE MAC and SWITCH SIDE MAC of 1540M PHY.
*
*
* INPUTS:
*
*    devNum             - device number    
*    portNum            - port number
*    sideAction         - side actions (SS or PS or BOTH) 
*
* OUTPUTS:
*    ssPrdFlowControl   - periodic flow control value to set in Switch Side MAC
*    psPrdFlowControl   - periodic flow control value to set in Phy Side MAC   
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*
*******************************************************************************/
GT_STATUS phyMacMSecGetDuplexMode 
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    OUT CPSS_PORT_DUPLEX_ENT *ssDuplexModePtr,
    OUT CPSS_PORT_DUPLEX_ENT  *psDuplexModePtr,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
)
{
    GT_STATUS  rc;
    GT_U16     ssRegAddr,psRegAddr;
    GT_U32     sliceOffset;
    GT_BOOL    pollStatus = GT_TRUE;
    GT_U32     ssReadValue = 0;
    GT_U32     psReadValue = 0;

    /*********************************
        - get slice
        - set parameters
        - start transaction
        - set SS MAC
        - set PS MAC
        - finish transaction
    **********************************/

    /* get slice offset */
    rc = macDrvGetSlice(devNum,portNum,&sliceOffset);

    /* Phy Side MAC speed set */
    if (sideAction == PHYMAC_PHY_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {
        /* set reg address for PS MAC */
        psRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_WIR_MAC_STATUS0);

        /* start transaction  */
        rc = phyMacMSecStartCnfgTransaction(devNum,portNum,&pollStatus);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }

        /*  set speed for Phy Side MAC */
        rc = phyMacMSecReadRegField(devNum,portNum,psRegAddr,
                             &psReadValue,PHYMAC_1540_PORT_DUPLEX_STATUS_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }

        if (psReadValue == 0)
            *psDuplexModePtr = CPSS_PORT_FULL_DUPLEX_E;
        else
            *psDuplexModePtr = CPSS_PORT_HALF_DUPLEX_E;
    }
    /* Switch Side MAC speed set */
    if (sideAction == PHYMAC_SWITCH_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {

        /* set reg address for SS MAC */
        ssRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_WIR_MAC_STATUS0);

        /*  set speed for Switch Side MAC */
        rc = phyMacMSecReadRegField(devNum,portNum,ssRegAddr,
                             &ssReadValue,PHYMAC_1540_PORT_DUPLEX_STATUS_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
        if (ssReadValue == 0)
            *ssDuplexModePtr = CPSS_PORT_FULL_DUPLEX_E;
        else
            *ssDuplexModePtr = CPSS_PORT_HALF_DUPLEX_E;

    }

    /* finish  transaction  */
    rc = phyMacMSecFinishCnfgTransaction(devNum,portNum,pollStatus);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }
    return GT_OK; 
}

/*******************************************************************************
* phyMacMSecSetPortEnable  (18)
*
* DESCRIPTION:
*       This function sets port enable/disable mode for PHY SIDE MAC 
*       and SWITCH SIDE MAC of 1540M PHY.
*
*
* INPUTS:
*
*    devNum             - device number    
*    portNum            - port number
*    ssPortEnable       - port enable/disable value to set in Switch Side MAC
*    psPortEnable       - port enable/disable value to set in Phy Side MAC   
*    sideAction         - side actions (SS or PS or BOTH) 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*
*******************************************************************************/
GT_STATUS phyMacMSecSetPortEnable
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    IN GT_BOOL ssPortEnable,
    IN GT_BOOL psPortEnable,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
)
{
    GT_STATUS rc;
    GT_U16    ssRegAddr,psRegAddr;
    GT_U32    sliceOffset;
    GT_BOOL   pollStatus = GT_TRUE;

    /*********************************
        - get slice
        - set parameters
        - start transaction
        - set SS MAC
        - set PS MAC
        - finish transaction
    **********************************/

    /* get slice offset */
    rc = macDrvGetSlice(devNum,portNum,&sliceOffset);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }

    /* Phy Side MAC speed set */
    if (sideAction == PHYMAC_PHY_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {
        /* set reg address for PS MAC */
        psRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_WIR_MAC_CONTROL0);

        /* start transaction  */
        rc = phyMacMSecStartCnfgTransaction(devNum,portNum,&pollStatus);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }

        /*  set port enable/disable for Phy Side MAC */
        rc = phyMacMSecWriteRegField(devNum,portNum,psRegAddr,
                             (psPortEnable==GT_TRUE)?PHYMAC_1540_PORT_ENABLE_BITMAP:0,PHYMAC_1540_PORT_ENABLE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
    }
    /* Switch Side MAC speed set */
    if (sideAction == PHYMAC_SWITCH_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {

        /* set reg address for SS MAC */
        ssRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_SYS_MAC_CONTROL0);

        /*  set port enable/disable for Switch Side MAC */
        rc = phyMacMSecWriteRegField(devNum,portNum,ssRegAddr,
                             (ssPortEnable==GT_TRUE)?PHYMAC_1540_PORT_ENABLE_BITMAP:0,PHYMAC_1540_PORT_ENABLE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
    }
    /* finish  transaction  */
    rc = phyMacMSecFinishCnfgTransaction(devNum,portNum,pollStatus);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* phyMacMSecGetPortEnable  (19)
*
* DESCRIPTION:
*       This function returns port enable/disable  
*       status for PHY SIDE MAC of 1540M PHY.
*
*
* INPUTS:
*
*    devNum             - device number    
*    portNum            - port number
*    sideAction         - side actions (SS or PS or BOTH) 
*
* OUTPUTS:
*    ssPortEnablePtr   - ptr to port enable value from Switch Side MAC
*    psPortEnablePtr   - ptr to port enable value from Phy Side MAC   
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*
*******************************************************************************/
GT_STATUS phyMacMSecGetPortEnable 
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_BOOL *ssPortEnablePtr,
    OUT GT_BOOL  *psPortEnablePtr,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
)
{
    GT_STATUS  rc;
    GT_U16     ssRegAddr,psRegAddr;
    GT_U32     sliceOffset;
    GT_BOOL    pollStatus = GT_TRUE;
    GT_U32     ssReadValue = 0;
    GT_U32     psReadValue = 0;


    /*********************************
        - get slice
        - set parameters
        - start transaction
        - set SS MAC
        - set PS MAC
        - finish transaction
    **********************************/


    /* get slice offset */
    rc = macDrvGetSlice(devNum,portNum,&sliceOffset);


    /* Phy Side MAC speed set */
    if (sideAction == PHYMAC_PHY_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {
        /* set reg address for PS MAC */
        psRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_WIR_MAC_CONTROL0);


        /* start transaction  */
        rc = phyMacMSecStartCnfgTransaction(devNum,portNum,&pollStatus);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }

        /*  set speed for Phy Side MAC */
        rc = phyMacMSecReadRegField(devNum,portNum,psRegAddr,
                             &psReadValue,PHYMAC_1540_PORT_ENABLE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
        *psPortEnablePtr = psReadValue;
    }
    /* Switch Side MAC speed set */
    if (sideAction == PHYMAC_SWITCH_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {

        /* set reg address for SS MAC */
        ssRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_SYS_MAC_CONTROL0);

        /*  set speed for Switch Side MAC */
        rc = phyMacMSecReadRegField(devNum,portNum,ssRegAddr,
                             &ssReadValue,PHYMAC_1540_PORT_ENABLE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }

        *ssPortEnablePtr = ssReadValue;
    }

    /* finish  transaction  */
    rc = phyMacMSecFinishCnfgTransaction(devNum,portNum,pollStatus);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* phyMacMSecSetPaddingEnable  (22)
*
* DESCRIPTION:
*       This function sets port padding enable mode 
*       for PHY SIDE MAC and SWITCH SIDE MAC of 1540M PHY.
*
*
* INPUTS:
*
*    devNum                - device number    
*    portNum               - port number
*    ssPaddingEnable       - Padding enable/disable value to set in Switch Side MAC
*    psPaddingEnable       - Padding enable/disable value to set in Phy Side MAC   
*    sideAction            - side actions (SS or PS or BOTH) 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*
*******************************************************************************/
GT_STATUS phyMacMSecSetPaddingEnable
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    IN GT_BOOL ssPaddingEnable,
    IN GT_BOOL psPaddingEnable,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
)
{
    GT_STATUS rc;
    GT_U16    ssRegAddr,psRegAddr;
    GT_U32    sliceOffset;
    GT_BOOL   pollStatus = GT_TRUE;

    /*********************************
        - get slice
        - set parameters
        - start transaction
        - set SS MAC
        - set PS MAC
        - finish transaction
    **********************************/
    
    /* get slice offset */
    rc = macDrvGetSlice(devNum,portNum,&sliceOffset);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }
    /* Phy Side MAC speed set */
    if (sideAction == PHYMAC_PHY_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {
        /* set reg address for PS MAC */
        psRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_WIR_MAC_CONTROL2);

        /* start transaction  */
        rc = phyMacMSecStartCnfgTransaction(devNum,portNum,&pollStatus);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
        /*  set speed for Phy Side MAC */
        rc = phyMacMSecWriteRegField(devNum,portNum,psRegAddr,
                             psPaddingEnable,PHYMAC_1540_PADDING_ENABLE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
    }
    /* Switch Side MAC speed set */
    if (sideAction == PHYMAC_SWITCH_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {

        /* set reg address for SS MAC */
        ssRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_SYS_MAC_CONTROL2);

        /*  set speed for Switch Side MAC */
        rc = phyMacMSecWriteRegField(devNum,portNum,ssRegAddr,
                             ssPaddingEnable,PHYMAC_1540_PADDING_ENABLE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
    }
    /* finish  transaction  */
    rc = phyMacMSecFinishCnfgTransaction(devNum,portNum,pollStatus);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* phyMacMSecGetPaddingEnable  (23)
*
* DESCRIPTION:
*       This function returns port padding enable mode 
*       for PHY SIDE MAC of 1540M PHY.
*
*
* INPUTS:
*
*    devNum                - device number    
*    portNum               - port number
*    sideAction            - side actions (SS or PS or BOTH) 
*
* OUTPUTS:
*    ssPaddingEnable       - Padding enable/disable value to get from Switch Side MAC
*    psPaddingEnable       - Padding enable/disable value to get from Phy Side MAC   
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*
*******************************************************************************/
GT_STATUS phyMacMSecGetPaddingEnable
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    IN GT_BOOL *ssPaddingEnable,
    IN GT_BOOL *psPaddingEnable,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
)
{
    GT_STATUS rc;
    GT_U16    ssRegAddr,psRegAddr;
    GT_U32    sliceOffset;
    GT_BOOL   pollStatus = GT_TRUE;
    GT_U32    ssReadValue = 0;
    GT_U32    psReadValue = 0;

    /*********************************
        - get slice
        - set parameters
        - start transaction
        - set SS MAC
        - set PS MAC
        - finish transaction
    **********************************/
    
    /* get slice offset */
    rc = macDrvGetSlice(devNum,portNum,&sliceOffset);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }
    /* Phy Side MAC speed set */
    if (sideAction == PHYMAC_PHY_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {
        /* set reg address for PS MAC */
        psRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_WIR_MAC_CONTROL2);

        /* start transaction  */
        rc = phyMacMSecStartCnfgTransaction(devNum,portNum,&pollStatus);
        if (rc != GT_OK) 
        {
            osPrintf("  >> MACSEC ERROR phyMacMSecExcessiveCollisionDropEnable - phyMacMSecStartCnfgTransaction return %d \n",rc);
            return rc;
        }
        /*  set speed for Phy Side MAC */
        rc = phyMacMSecReadRegField(devNum,portNum,psRegAddr,
                             &psReadValue,PHYMAC_1540_PADDING_ENABLE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
        if (psReadValue == 0)
            *psPaddingEnable = GT_FALSE;
        else
            *psPaddingEnable = GT_TRUE;

    }
    /* Switch Side MAC speed set */
    if (sideAction == PHYMAC_SWITCH_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {

        /* set reg address for SS MAC */
        ssRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_SYS_MAC_CONTROL2);

        /*  set speed for Switch Side MAC */
        rc = phyMacMSecReadRegField(devNum,portNum,ssRegAddr,
                             &ssReadValue,PHYMAC_1540_PADDING_ENABLE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
        if (ssReadValue == 0)
            *ssPaddingEnable = GT_FALSE;
        else
            *ssPaddingEnable = GT_TRUE;
    }
    /* finish  transaction  */
    rc = phyMacMSecFinishCnfgTransaction(devNum,portNum,pollStatus);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* phyMacMSecSetCRCCheckEnable  (26)
*
* DESCRIPTION:
*       This function enable/disable CRC check mode 
*       for PHY SIDE MAC and SWITCH SIDE MAC of 1540M PHY.
*
*
* INPUTS:
*
*    devNum                - device number    
*    portNum               - port number
*    ssCRCCheckEnable       - CRC check enable/disable value to set in Switch Side MAC
*    psCRCCheckEnable       - CRC check enable/disable value to set in Phy Side MAC   
*    sideAction            - side actions (SS or PS or BOTH) 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*
*******************************************************************************/
GT_STATUS phyMacMSecSetCRCCheckEnable
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    IN GT_BOOL ssCRCCheckEnable,
    IN GT_BOOL psCRCCheckEnable,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
)
{
    GT_STATUS rc;
    GT_U16    ssRegAddr,psRegAddr;
    GT_U32    sliceOffset;
    GT_BOOL   pollStatus = GT_TRUE;

    /*********************************
        - get slice
        - set parameters
        - start transaction
        - set SS MAC
        - set PS MAC
        - finish transaction
    **********************************/
    
    /* get slice offset */
    rc = macDrvGetSlice(devNum,portNum,&sliceOffset);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }
    /* Phy Side MAC speed set */
    if (sideAction == PHYMAC_PHY_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {
        /* set reg address for PS MAC */
        psRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_WIR_MAC_CONTROL1);

        /* start transaction  */
        rc = phyMacMSecStartCnfgTransaction(devNum,portNum,&pollStatus);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
        /*  set speed for Phy Side MAC */
        rc = phyMacMSecWriteRegField(devNum,portNum,psRegAddr,
                             psCRCCheckEnable,PHYMAC_1540_CRC_CHECK_ENABLE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
    }
    /* Switch Side MAC speed set */
    if (sideAction == PHYMAC_SWITCH_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {

        /* set reg address for SS MAC */
        ssRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_SYS_MAC_CONTROL1);

        /*  set speed for Switch Side MAC */
        rc = phyMacMSecWriteRegField(devNum,portNum,ssRegAddr,
                             ssCRCCheckEnable,PHYMAC_1540_CRC_CHECK_ENABLE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
    }
    /* finish  transaction  */
    rc = phyMacMSecFinishCnfgTransaction(devNum,portNum,pollStatus);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* phyMacMSecGetCRCCheckEnable  (27)
*
* DESCRIPTION:
*       This function returns port padding enable mode 
*       for PHY SIDE MAC of 1540M PHY.
*
*
* INPUTS:
*
*    devNum                - device number    
*    portNum               - port number
*    sideAction            - side actions (SS or PS or BOTH) 
*
* OUTPUTS:
*    ssCRCCheckEnable       - CRC Check enable/disable value to get from Switch Side MAC
*    psCRCCheckEnable       - CRC Check enable/disable value to get from Phy Side MAC   
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*
*******************************************************************************/
GT_STATUS phyMacMSecGetCRCCheckEnable
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    IN GT_BOOL *ssCRCCheckEnable,
    IN GT_BOOL *psCRCCheckEnable,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
)
{
    GT_STATUS rc;
    GT_U16    ssRegAddr,psRegAddr;
    GT_U32    sliceOffset;
    GT_U32    ssReadValue = 0;
    GT_U32    psReadValue = 0;    
    GT_BOOL   pollStatus = GT_TRUE;

    /*********************************
        - get slice
        - set parameters
        - start transaction
        - set SS MAC
        - set PS MAC
        - finish transaction
    **********************************/
    
    /* get slice offset */
    rc = macDrvGetSlice(devNum,portNum,&sliceOffset);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }
    /* Phy Side MAC speed set */
    if (sideAction == PHYMAC_PHY_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {
        /* set reg address for PS MAC */
        psRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_WIR_MAC_CONTROL1);

        /* start transaction  */
        rc = phyMacMSecStartCnfgTransaction(devNum,portNum,&pollStatus);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
        /*  get CRC check enable/disable from Phy Side MAC */
        rc = phyMacMSecReadRegField(devNum,portNum,psRegAddr,
                             &psReadValue,PHYMAC_1540_CRC_CHECK_ENABLE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }

        if (psReadValue == 0)
            *psCRCCheckEnable = GT_FALSE;
        else
            *psCRCCheckEnable = GT_TRUE;
    }
    /* Switch Side MAC speed set */
    if (sideAction == PHYMAC_SWITCH_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {

        /* set reg address for SS MAC */
        ssRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_SYS_MAC_CONTROL1);

        /*  set speed for Switch Side MAC */
        rc = phyMacMSecReadRegField(devNum,portNum,ssRegAddr,
                             &ssReadValue,PHYMAC_1540_CRC_CHECK_ENABLE_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }

        if (ssReadValue == 0)
            *ssCRCCheckEnable = GT_FALSE;
        else
            *ssCRCCheckEnable = GT_TRUE;
    }
    /* finish  transaction  */
    rc = phyMacMSecFinishCnfgTransaction(devNum,portNum,pollStatus);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }

    return GT_OK;
}


/*******************************************************************************
* phyMacMSecSetMRU  (28)
*
* DESCRIPTION:
*       This function sets MRU size  
*       for PHY SIDE MAC and SWITCH SIDE MAC of 1540M PHY.
*
*
* INPUTS:
*
*    devNum                - device number    
*    portNum               - port number
*    ssMruSize             - MruSize value to set in Switch Side MAC
*    psMruSize             - MruSize value to set in Phy Side MAC   
*    sideAction            - side actions (SS or PS or BOTH) 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*
*******************************************************************************/
GT_STATUS phyMacMSecSetMRU
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    IN GT_U32  ssMruSize,
    IN GT_U32  psMruSize,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
)
{
    GT_STATUS rc;
    GT_U16    ssRegAddr,psRegAddr;
    GT_U32    sliceOffset;
    GT_BOOL   pollStatus = GT_TRUE;

    /*********************************
        - get slice
        - set parameters
        - start transaction
        - set SS MAC
        - set PS MAC
        - finish transaction
    **********************************/
    
    /* get slice offset */
    rc = macDrvGetSlice(devNum,portNum,&sliceOffset);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }
    /* Phy Side MAC speed set */
    if (sideAction == PHYMAC_PHY_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {
        /* set reg address for PS MAC */
        psRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_WIR_MAC_CONTROL0);

        /* start transaction  */
        rc = phyMacMSecStartCnfgTransaction(devNum,portNum,&pollStatus);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }

        /*  set speed for Phy Side MAC */
        rc = phyMacMSecWriteRegField(devNum,portNum,psRegAddr,
                             (psMruSize<<2),PHYMAC_1540_MRU_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
    }
    /* Switch Side MAC speed set */
    if (sideAction == PHYMAC_SWITCH_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {

        /* set reg address for SS MAC */
        ssRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_SYS_MAC_CONTROL0);

        /*  set speed for Switch Side MAC */
        rc = phyMacMSecWriteRegField(devNum,portNum,ssRegAddr,
                             (ssMruSize<<2),PHYMAC_1540_MRU_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
    }
    /* finish  transaction  */
    rc = phyMacMSecFinishCnfgTransaction(devNum,portNum,pollStatus);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* phyMacMSecGetMRU  (29)
*
* DESCRIPTION:
*       This function returns port padding enable mode 
*       for PHY SIDE MAC of 1540M PHY.
*
*
* INPUTS:
*
*    devNum                - device number    
*    portNum               - port number
*    sideAction            - side actions (SS or PS or BOTH) 
*
* OUTPUTS:
*    ssMruSize             - Mru Size value to get from Switch Side MAC
*    psMruSize             - Mru Size value to get from Phy Side MAC   
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*
*******************************************************************************/
GT_STATUS phyMacMSecGetMRU
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_U32 *ssMruSize,
    OUT GT_U32 *psMruSize,
    IN PHYMAC_SIDE_SET_ACTION_ENT sideAction
)
{
    GT_STATUS rc;
    GT_U16    ssRegAddr,psRegAddr;
    GT_U32    sliceOffset;
    GT_BOOL   pollStatus = GT_TRUE;

    /*********************************
        - get slice
        - set parameters
        - start transaction
        - set SS MAC
        - set PS MAC
        - finish transaction
    **********************************/
    
    /* get slice offset */
    rc = macDrvGetSlice(devNum,portNum,&sliceOffset);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }
    /* Phy Side MAC speed set */
    if (sideAction == PHYMAC_PHY_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {
        /* set reg address for PS MAC */
        psRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_WIR_MAC_CONTROL0);

        /* start transaction  */
        rc = phyMacMSecStartCnfgTransaction(devNum,portNum,&pollStatus);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }
        /*  set speed for Phy Side MAC */
        rc = phyMacMSecReadRegField(devNum,portNum,psRegAddr,
                             psMruSize,PHYMAC_1540_MRU_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }

        *psMruSize = (*psMruSize >> 2);

    }
    /* Switch Side MAC speed set */
    if (sideAction == PHYMAC_SWITCH_SIDE_SET_E ||
        sideAction == PHYMAC_BOTH_SIDE_SET_E) 
    {

        /* set reg address for SS MAC */
        ssRegAddr = (GT_U16)(sliceOffset + PHYMAC_1540_SYS_MAC_CONTROL0);

        /*  set speed for Switch Side MAC */
        rc = phyMacMSecReadRegField(devNum,portNum,ssRegAddr,
                             ssMruSize,PHYMAC_1540_MRU_BITMAP);
        if (rc != GT_OK) 
        {
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return rc;
        }

        *ssMruSize = (*ssMruSize >> 2);

    }
    /* finish  transaction  */
    rc = phyMacMSecFinishCnfgTransaction(devNum,portNum,pollStatus);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }

    return GT_OK;
}



/**************************************************************************
*        EEE PHYMAC functions
***************************************************************************/

/*******************************************************************************
* phyMacEEEModeSet
*
* DESCRIPTION:
*       This function sets EEE mode   
*
*
* INPUTS:
*
*    devNum                - device number    
*    portNum               - port number
*    mode                  - defines one of: 
*                                    master mode 
*                                    slave mode 
*                                    force mode 
*                                    EEE disable mode 
*                                   
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*
*******************************************************************************/
GT_STATUS phyMacEEEModeSet
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    IN MACPHY_EEE_LPI_MODE_ENT mode 
)
{
    GT_STATUS rc;
    GT_U16    regAddr;
    GT_U32    sliceOffset;
    GT_BOOL   pollStatus = GT_TRUE;

    if (phy1540DataBasePtr==NULL) 
    {
        return GT_NOT_SUPPORTED;
    }
    /* get slice offset */
    rc = macDrvGetSlice(devNum,portNum,&sliceOffset);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }

    /* set LPI config register address  */
    regAddr = (GT_U16)(sliceOffset + PHYMAC_1540_EEE_LPI_CONFIG);

    /* start transaction  */
    rc = phyMacMSecStartCnfgTransaction(devNum,portNum,&pollStatus);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }
        /* set mode */
    switch ( mode )
      {
        case MACPHY_EEE_LPI_DISABLE_MODE_E:
            /*  set enable/disable LPI Master mode */
            rc = phyMacMSecWriteRegField(devNum,portNum,regAddr,
                                 MACPHY_EEE_LPI_DISABLE_MODE_E,PHYMAC_1540_EEE_DISABLE_MODE_BITMAP);
            if (rc != GT_OK) 
            {
                rc = phyMacMSecCleanAfterFail(devNum,portNum);
                return rc;
            }
            break;
        case MACPHY_EEE_LPI_MASTER_MODE_E:
            /*  set enable/disable LPI Master mode */
            rc = phyMacMSecWriteRegField(devNum,portNum,regAddr,
                                 MACPHY_EEE_LPI_MASTER_MODE_E,PHYMAC_1540_EEE_MASTER_MODE_BITMAP);
            if (rc != GT_OK) 
            {
                rc = phyMacMSecCleanAfterFail(devNum,portNum);
                return rc;
            }
            break;
        case MACPHY_EEE_LPI_SLAVE_MODE_E:
            /*  set enable/disable LPI Master mode */
            rc = phyMacMSecWriteRegField(devNum,portNum,regAddr,
                                 MACPHY_EEE_LPI_SLAVE_MODE_E,PHYMAC_1540_EEE_SLAVE_MODE_BITMAP);
            if (rc != GT_OK) 
            {
                rc = phyMacMSecCleanAfterFail(devNum,portNum);
                return rc;
            }
             break;
        case MACPHY_EEE_LPI_FORCE_MODE_E:
            /*  set enable/disable LPI Master mode */
            rc = phyMacMSecWriteRegField(devNum,portNum,regAddr,
                                 MACPHY_EEE_LPI_FORCE_MODE_E,PHYMAC_1540_EEE_FORCE_MODE_BITMAP);
            if (rc != GT_OK) 
            {
                rc = phyMacMSecCleanAfterFail(devNum,portNum);
                return rc;
            }
            break;
        default:
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return GT_BAD_PARAM;
      }

    /* finish  transaction  */
    rc = phyMacMSecFinishCnfgTransaction(devNum,portNum,pollStatus);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* phyMacEEEModeGet
*
* DESCRIPTION:
*       This function returns EEE mode   
*
*
* INPUTS:
*
*    devNum                - device number    
*    portNum               - port number
*                                   
* OUTPUTS:
*    modePtr                  - defines one of: 
*                                    master mode 
*                                    slave mode 
*                                    force mode 
*                                    EEE disable mode 
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on wrong parameters and set failed
*       GT_BAD_PARAM             - wrong parameter
*
* COMMENTS:
*       
*
*******************************************************************************/
GT_STATUS phyMacEEEModeGet
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    OUT MACPHY_EEE_LPI_MODE_ENT *modePtr    
)
{
    GT_STATUS rc;
    GT_U16    regAddr;
    GT_U32    sliceOffset;
    GT_BOOL   pollStatus = GT_TRUE;
    GT_U32    regValue = 0; 

    if (phy1540DataBasePtr==NULL) 
    {
        return GT_NOT_SUPPORTED;
    }
    /* get slice offset */
    rc = macDrvGetSlice(devNum,portNum,&sliceOffset);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }

    /* set LPI config register address  */
    regAddr = (GT_U16)(sliceOffset + PHYMAC_1540_EEE_LPI_CONFIG);

    /* start transaction  */
    rc = phyMacMSecStartCnfgTransaction(devNum,portNum,&pollStatus);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }
            /*  set speed for Phy Side MAC */
    rc = phyMacMSecReadRegField(devNum,portNum,regAddr,
                             &regValue,PHYMAC_1540_EEE_DISABLE_MODE_BITMAP);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }
    /* get mode */
    switch ( regValue )
      {
        case 0x0:
            *modePtr = MACPHY_EEE_LPI_DISABLE_MODE_E;
            break;
        case 0x1:
            *modePtr = MACPHY_EEE_LPI_MASTER_MODE_E;
            break;
        case 0x2:
            *modePtr = MACPHY_EEE_LPI_SLAVE_MODE_E;
            break;
        case 0x4:
            *modePtr = MACPHY_EEE_LPI_FORCE_MODE_E;
            break;
        default:
            rc = phyMacMSecCleanAfterFail(devNum,portNum);
            return GT_BAD_PARAM;
      }

    /* finish  transaction  */
    rc = phyMacMSecFinishCnfgTransaction(devNum,portNum,pollStatus);
    if (rc != GT_OK) 
    {
        rc = phyMacMSecCleanAfterFail(devNum,portNum);
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* phyMacEEEInit
*
* DESCRIPTION:
*       This function:
*           - enable EEE auto-neg advertisment
*           - soft reset
*           - run EEE workaround for 1540
*
* INPUTS:
*    IN GT_U8   devNum   -  device number  
*    IN GT_PHYSICAL_PORT_NUM  portNum,  -  port number 
*   
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS phyMacEEEInit
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum
)
{
    GT_STATUS rc;
    GT_PORT_GROUPS_BMP  portGroupsBmp = 0x0;
    CPSS_PHY_SMI_INTERFACE_ENT   smiInterface = CPSS_PHY_SMI_INTERFACE_0_E;
    GT_U32  smiAddr;

    rc = macDrvGetSmiAddress(devNum,portNum,&smiAddr);
    if (rc != GT_OK)
    {
        osPrintf("  >> MACSEC ERROR Failed to get Smi Address - rc - %d\n",rc);
        return rc;
    }

    /************************************
    *  enable EEE Auto-neg advertisment 
    ************************************/

    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,smiAddr,
                                     13,0x0007);
    if (rc != GT_OK){
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,smiAddr,
                                     14,0x003C);
    if (rc != GT_OK){
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,smiAddr,
                                     13,0x4007);
    if (rc != GT_OK){
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,smiAddr,
                                     14,0x0006);
    if (rc != GT_OK){
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /*****************************************************
    *                    soft reset for port
    ******************************************************/

    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,smiAddr,
                                     22,0x0);
    if (rc != GT_OK){
        osPrintf("  >> MACSEC ERROR Failed to read register - rc - %d\n",rc);
        return rc;
    }

    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,smiAddr,
                                     0,0x9140);
    if (rc != GT_OK){
        osPrintf("  >> MACSEC ERROR Failed to read register - rc - %d\n",rc);
        return rc;
    }

    /* wait after port reset */
    (void)cpssOsTimerWkAfter(1000);


    /*****************************************************
    *                    EEE workaround
    ******************************************************/

    /* 1  Set page address 0x00FB in page register ( reg 22 ) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,smiAddr,
                                     22,0x00FB);
    if (rc != GT_OK){
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* 2  Set 0x3666 in register 8 of current page (0x00FB) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,8,0x3666);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }


    /* 3  Set page address 0x00FF in page register ( reg 22 ) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,smiAddr,
                                     22,0x00FF);
    if (rc != GT_OK){
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* 4  Set 0x3666 in register 17 of current page (0x0F0C) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,17, 0x0F0C);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* 5  Set 0x3666 in register 16 of current page (0x2146) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,16, 0x2146);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* 6  Set 0x3666 in register 17 of current page (0xC090) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,17, 0xC090);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* 7  Set 0x3666 in register 16 of current page (0x2147) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,16, 0x2147);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* 8  Set 0x00FF in register 22  */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,22, 0x00FF);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* 9  Set 0x0000 in register 17 of current page (0x00FF) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,17, 0x0);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* 10  Set 0x2000 in register 16 of current page (0x00FF) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,16, 0x2000);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* 11  Set 0x00FC in register 22 */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,22, 0x00FC);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* 12  Set 0x2000 in register 23 of current page (0x011A) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,23, 0x011A);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* 13  Set 0x00FF in register 22 */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,22, 0x00FF);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* 14  Set 0x6000 in register 17 of current page (0x00FF) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,17, 0x00FF);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* 15  Set 0x2143 in register 16 of current page (0x00FF) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,16, 0x2143);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* 16  Set 0xC004 in register 17 of current page (0x00FF) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,17, 0xC004);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* 17  Set 0xC004 in register 17 of current page (0x00FF) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,17, 0xC004);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* 18  Set 0xC006 in register 17 of current page (0x00FF) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,17, 0xC006);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* 19  Set 0x2100 in register 16 of current page (0x00FF) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,16, 0x2100);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }
    /* 20  Set 0x0104 in register 17 of current page (0x00FF) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,17, 0x0104);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* 21  Set 0x2102 in register 16 of current page (0x00FF) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,16, 0x2102);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }
    /* 22  Set 0x49E8 in register 17 of current page (0x00FF) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,17, 0x49E8);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* 23  Set 0x2144 in register 16 of current page (0x00FF) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,16, 0x2144);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }
    /* 24  Set 0x3180 in register 17 of current page (0x00FF) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,17, 0x3180);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* 25  Set 0x2148 in register 16 of current page (0x00FF) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,16, 0x2148);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }
    /* 26  Set 0xFC44 in register 17 of current page (0x00FF) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,17, 0xFC44);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* 27  Set 0x214B in register 16 of current page (0x00FF) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,16, 0x214B);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }
    /* 28  Set 0x7FD2 in register 17 of current page (0x00FF) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,17, 0x7FD2);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* 29  Set 0x214C in register 16 of current page (0x00FF) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,16, 0x214C);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }
    /* 30  Set 0x2240 in register 17 of current page (0x00FF) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,17, 0x2240);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* 31  Set 0x2140 in register 16 of current page (0x00FF) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,16, 0x2140);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }
    /* 32  Set 0x3008 in register 17 of current page (0x00FF) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,17, 0x3008);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* 33  Set 0x214E in register 16 of current page (0x00FF) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,16, 0x214E);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }
    /* 34  Set 0x3DF0 in register 17 of current page (0x00FF) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,17, 0x3DF0);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    /* 35  Set 0x214F in register 16 of current page (0x00FF) */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,16, 0x214F);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }
    /* 36  Set page 0 in register 22  */
    rc = macPhy1540MSmiWriteRegister(devNum,portGroupsBmp,smiInterface,
                                     smiAddr,22, 0x0000);
    if (rc != GT_OK) {
        osPrintf("  >> MACSEC ERROR Failed to write register - rc - %d\n",rc);
        return rc;
    }

    return GT_OK;
}


