#include "mtdCopyright.h" 
/* Copyright 2014 Marvell International Ltd, See included file for licensing information */

/********************************************************************
This file contains functions and global data for
higher-level functions for controlling and getting status from 
the Control Unit (C Unit) of the 88X32X0 PHY.
********************************************************************/
#include "mtdApiRegs.h"
#include "mtdApiTypes.h"
#include "mtdHwCntl.h"
#include "mtdAPI.h"
#include "mtdCunit.h"

/*******************************************************************/
/*******************************************************************
  Resets
  Port SW Reset
  Chip HW Reset
 *******************************************************************/
/****************************************************************************/

MTD_STATUS mtdCunitSwReset
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port
)
{
    return (mtdHwSetPhyRegField(contextPtr,port,MTD_CUNIT_PORT_CTRL,15,1,1));
}

MTD_STATUS mtdTunitHwReset
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port
)
{
    return (mtdHwSetPhyRegField(contextPtr,port,MTD_CUNIT_PORT_CTRL,12,1,1));    
}


MTD_STATUS mtdChipHardwareReset
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port
)
{
    return (mtdHwSetPhyRegField(contextPtr,port,MTD_CUNIT_PORT_CTRL,14,1,1));
}

/*******************************************************************
  Macsec Configuration
  Macsec Set Control
  Macsec Get Control
 *******************************************************************/


MTD_STATUS mtdSetMacsecControl
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 powerControl,
    IN MTD_U16 faultForwardControl
)
{
    if (powerControl == MTD_MACSEC_POWERON ||
        powerControl == MTD_MACSEC_RESET ||
        powerControl == MTD_MACSEC_SLEEP)        
    {
        ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,MTD_CUNIT_MODE_CONFIG,12,2,powerControl));
    }
    else
    {
        return MTD_FAIL;
    }

    switch (faultForwardControl)
    {
        case MTD_FAULT_FORWARDING_OFF:
            ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,MTD_CUNIT_MODE_CONFIG,14,1,0));
            ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,MTD_CUNIT_MODE_CONFIG,11,1,0));
            break;
            
        case MTD_FORWARD_LINK_FAULT:
            ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,MTD_CUNIT_MODE_CONFIG,14,1,1));
            ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,MTD_CUNIT_MODE_CONFIG,11,1,0));            
            break;
            
        case MTD_FORWARD_NON_LINK_FAULT:
            ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,MTD_CUNIT_MODE_CONFIG,14,1,0));
            ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,MTD_CUNIT_MODE_CONFIG,11,1,1));            
            break;
            
        case MTD_FORWARD_ALL_FAULT:
            ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,MTD_CUNIT_MODE_CONFIG,14,1,1));
            ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,MTD_CUNIT_MODE_CONFIG,11,1,1));            
            break;
            
        default:
            return MTD_FAIL; /* invalid value passed in */
            break;
    }

    return MTD_OK;
    
}

MTD_STATUS mtdGetMacsecControl
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_U16 *powerStatus,
    OUT MTD_U16 *faultForwardStatus
)
{
    MTD_U16 bit14, bit11;
    MTD_U16 regValue;

    ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_CUNIT_MODE_CONFIG,&regValue));

    ATTEMPT(mtdHwGetRegFieldFromWord(regValue,12,2,powerStatus));
    ATTEMPT(mtdHwGetRegFieldFromWord(regValue,14,1,&bit14));
    ATTEMPT(mtdHwGetRegFieldFromWord(regValue,11,1,&bit11));

    if (bit11 && bit14)
    {
        *faultForwardStatus = MTD_FORWARD_ALL_FAULT;
    }
    else if (bit11 && !bit14)
    {
        *faultForwardStatus = MTD_FORWARD_NON_LINK_FAULT;
    }
    else if (!bit11 && bit14)
    {
        *faultForwardStatus = MTD_FORWARD_LINK_FAULT;
    }
    else
    {
        /* must be !bit11 && !bit14 */
        *faultForwardStatus = MTD_FAULT_FORWARDING_OFF;
    }

    return MTD_OK;
}

/*******************************************************************
  C Unit Port Power Down
 *******************************************************************/

MTD_STATUS mtdSetPortPowerDown
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_BOOL powerDown
)
{
    return(mtdHwSetPhyRegField(contextPtr,port,MTD_CUNIT_PORT_CTRL,11,1,(powerDown == MTD_TRUE ? 1 : 0)));
}

MTD_STATUS mtdGetPortPowerDown
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_BOOL *powerDown
)
{
    MTD_U16 bit11;

    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,MTD_CUNIT_PORT_CTRL,11,1,&bit11));

    if (bit11)
    {
        *powerDown = MTD_TRUE;
    }
    else
    {
        *powerDown = MTD_FALSE;
    }

    return MTD_OK;
}

/*******************************************************************
  C Unit Top Configuration
 *******************************************************************/

MTD_STATUS mtdSetCunitTopConfig
(
 IN CTX_PTR_TYPE contextPtr,
 IN MTD_U16 port,
 IN MTD_U16 frameToRegister,
 IN MTD_U16 mediaSelect,
 IN MTD_U16 fiberType,
 IN MTD_BOOL npMediaEnergyDetect,
 IN MTD_BOOL maxPowerTunitAMDetect,
 IN MTD_BOOL softwareResetCunit
)
{
    MTD_U16 regValue1;
    
    /* check error check of ranges/values of the different parameters */
    if (frameToRegister > MTD_F2R_8BHDR)
    {
        return MTD_FAIL;
    }

    if (mediaSelect == 5 || mediaSelect == 6 || mediaSelect > MTD_MS_AUTO_FIRST_LINK)
    {
        return MTD_FAIL;
    }

    if (fiberType > MTD_FT_10GBASER)
    {
        return MTD_FAIL;
    }

    ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_CUNIT_MODE_CONFIG,&regValue1));
    ATTEMPT(mtdHwSetRegFieldToWord(regValue1,frameToRegister,4,2,&regValue1));
    ATTEMPT(mtdHwSetRegFieldToWord(regValue1,mediaSelect,0,3,&regValue1));
    ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_CUNIT_MODE_CONFIG,regValue1));

    ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_CUNIT_PORT_CTRL,&regValue1));
    ATTEMPT(mtdHwSetRegFieldToWord(regValue1,(npMediaEnergyDetect?1:0),10,1,&regValue1));
    ATTEMPT(mtdHwSetRegFieldToWord(regValue1,(maxPowerTunitAMDetect?1:0),9,1,&regValue1));
    ATTEMPT(mtdHwSetRegFieldToWord(regValue1,fiberType,3,2,&regValue1));
    ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_CUNIT_PORT_CTRL,regValue1));

    /* do a software reset so the new values take effect, several of the above require it */
    if (softwareResetCunit)
    {
        ATTEMPT(mtdCunitSwReset(contextPtr,port));        
    }

    return MTD_OK;
}

MTD_STATUS mtdGetCunitTopConfig
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_U16 *frameToRegister,
    OUT MTD_U16 *mediaSelect,
    OUT MTD_U16 *fiberType,
    OUT MTD_BOOL *npMediaEnergyDetect,
    OUT MTD_BOOL *maxPowerTunitAMDetect
)
{
    MTD_U16 regValue1,bitValue;
    
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_CUNIT_MODE_CONFIG,&regValue1));
    ATTEMPT(mtdHwGetRegFieldFromWord(regValue1,4,2,frameToRegister));
    ATTEMPT(mtdHwGetRegFieldFromWord(regValue1,0,3,mediaSelect));

    ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_CUNIT_PORT_CTRL,&regValue1));
    ATTEMPT(mtdHwGetRegFieldFromWord(regValue1,10,1,&bitValue));
    if (bitValue)
    {
        *npMediaEnergyDetect = MTD_TRUE;
    }
    else
    {
        *npMediaEnergyDetect = MTD_FALSE;
    }
    ATTEMPT(mtdHwGetRegFieldFromWord(regValue1,9,1,&bitValue));
    if (bitValue)
    {
        *maxPowerTunitAMDetect= MTD_TRUE;
    }
    else
    {
        *maxPowerTunitAMDetect = MTD_FALSE;
    }
    ATTEMPT(mtdHwGetRegFieldFromWord(regValue1,3,2,fiberType));
    
    return MTD_OK;
}




