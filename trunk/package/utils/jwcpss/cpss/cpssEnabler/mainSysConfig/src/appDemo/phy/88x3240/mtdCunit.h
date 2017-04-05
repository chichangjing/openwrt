#include "mtdCopyright.h" 
/* Copyright 2013 Marvell International Ltd, See included file for licensing information */

/********************************************************************
This file contains functions prototypes and global defines/data for
higher-level functions for controlling and getting status from 
the Control Unit (C Unit) of the 88X32X0 PHY.
********************************************************************/
#ifndef CUNIT_H
#define CUNIT_H
#if C_LINKAGE
#if defined __cplusplus 
    extern "C" { 
#endif 
#endif


/*******************************************************************
  Resets
  C Unit SW Reset
  T Unit SW Reset
  T Unit HW Reset
  Chip HW Reset
 *******************************************************************/

/******************************************************************************
 MTD_STATUS mtdCunitSwReset
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port
 )

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31

 Outputs:
    None

 Returns:
    MTD_OK or MTD_FAIL

 Description:
    Performs a software reset on one particular port's C Unit. 

    Retain bits will keep the value that has been written to them. Non-
    retain bits will be reset to the hardware reset state.

    This bit is self clearing once the reset has been complete it will
    return to 0.

 Side effects:
    None

 Notes/Warnings:
    Software resets only the C Unit. The T Unit has its own software reset,
    use mtdSoftwareReset(), or hardware reset the T Unit.

******************************************************************************/
MTD_STATUS mtdCunitSwReset
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port
);

/******************************************************************************
  NOTE: For T Unit Software Reset, call mtdSoftwareReset() in mtdAPI.c/h
******************************************************************************/

/******************************************************************************
 MTD_STATUS mtdTunitHwReset
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port
 )

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31

 Outputs:
    None

 Returns:
    MTD_OK or MTD_FAIL

 Description:
    Performs a hardware reset on the T Unit. All register values will return
    to their hardware reset state.  

    If the firmware was loaded from flash, it will be reloaded from flash.

    If the chip was strapped to load firmware from the host, the firmware
    must be reloaded.

    This bit self clears.

 Side effects:
    None

 Notes/Warnings:
    Any strap values overriden by register 31.F008 will be latched in and take
    effect when this reset is performed.

    Note that this is the major difference between this hardware reset and
    the special software reset, 1.49152.15 in the T Unit.

    The T Unit hardware reset in the C Unit must be used if any new values
    in 31.F008 need to be re-latched to pick up the new strap values.

******************************************************************************/
MTD_STATUS mtdTunitHwReset
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port
);


/******************************************************************************
 MTD_STATUS mtdChipHardwareReset
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port
 )

 Inputs:
    contextPtr - pointer to host's private data
    port - this can be any port on the chip, entire chip is reset though

 Outputs:
    None

 Returns:
    MTD_OK or MTD_FAIL

 Description:
    Performs a chip-level hardware reset, 31.F001.14 <= 1. This should reset
    all state machines and restore all registers to their power-on state.

    All ports are reset. This is equivalent to a chip level hardware reset.

 Side effects:
    See below.

 Notes/Warnings:
    Any established data links will drop. The chip will return to the same
    state as the power-on state and must be reinitialized.

******************************************************************************/
MTD_STATUS mtdChipHardwareReset
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port
);


/*******************************************************************
  Macsec Configuration
  Macsec Set Control
  Macsec Get Control
 *******************************************************************/

#define MTD_MACSEC_POWERON (0x3)
#define MTD_MACSEC_SLEEP   (0x2)
#define MTD_MACSEC_RESET   (0x1)
#define MTD_FAULT_FORWARDING_OFF   (0x0) /* faults are terminated by mac inside phy */
#define MTD_FORWARD_LINK_FAULT     (0x1) /* faults are terminated by mac inside phy except link fault, which is forwarded */
#define MTD_FORWARD_NON_LINK_FAULT (0x2) /* link fault is terminated by mac inside phy, others are forwarded */
#define MTD_FORWARD_ALL_FAULT      (0x3) /* all faults are forwarded, no faults are terminated by mac inside phy */
/******************************************************************************
 MTD_STATUS mtdSetMacsecControl
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     IN MTD_U16 powerControl,
     IN MTD_U16 faultForwardControl
 )

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31
    powerControl - one of the following:
        MTD_MACSEC_POWERON to power up the Macsec block
        MTD_MACSEC_SLEEP to put the Macsec block in sleep mode
        MTD_MACSET_RESET to put the Macsec block into the reset state
    faultForwardControl - one of the following:
        MTD_FAULT_FORWARDING_OFF
        MTD_FORWARD_LINK_FAULT
        MTD_FORWARD_NON_LINK_FAULT
        MTD_FORWARD_ALL_FAULT

 Outputs:
    None

 Returns:
    MTD_OK or MTD_FAIL if setting the control failed

 Description:
    This function is used to power-up the Macsec block, power it down,
    or put it into the reset state. Writes to the C Unit bits
    31.F000.13:12.

    The Macsec block must be powered up prior to configuring it, if the
    block was not powered up during the power-on initialization.

    The power state of the Macsec is configured according to "powerControl"
    and the fault forwarding is configured according to "faultForwardControl".

 Side effects:
    None

 Notes/Warnings:
    If the Macsec is in reset or sleep, all faults are forwarded through.
    Fault forwarding configuration is only functional if the Macsec is powered
    up, otherwise all faults are passed through to the host.

******************************************************************************/
MTD_STATUS mtdSetMacsecControl
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 powerControl,
    IN MTD_U16 faultForwardControl
);

/******************************************************************************
 MTD_STATUS mtdGetMacsecControl
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     OUT MTD_U16 *powerStatus,
     OUT MTD_U16 *faultForwardStatus
 )

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31

 Outputs:
    powerStatus - one of the following:
        MTD_MACSEC_POWERON Macsec block is powered up
        MTD_MACSEC_SLEEP Macsec block is in sleep mode
        MTD_MACSET_RESET Macsec block is in the reset state
    faultForwardStatus - one of the following:    
        MTD_FAULT_FORWARDING_OFF
        MTD_FORWARD_LINK_FAULT
        MTD_FORWARD_NON_LINK_FAULT
        MTD_FORWARD_ALL_FAULT

 Returns:
    MTD_OK or MTD_FAIL

 Description:
    This function reads the value of the Macsec power/reset bits in
    31.F000.13:12 and returns it in "powerStatus" and reads the 
    value of the fault forwarding configuration bits in
    31.F000.14 and 11 and returns the configuration in 
    "faultForwardStatus".

 Side effects:
    None

 Notes/Warnings:
    None

******************************************************************************/
MTD_STATUS mtdGetMacsecControl
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_U16 *powerStatus,
    OUT MTD_U16 *faultForwardStatus
);


/*******************************************************************
  C Unit Port Power Down
 *******************************************************************/

/******************************************************************************
 MTD_STATUS mtdSetPortPowerDown
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port.
     IN MTD_BOOL powerDown
 )

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31
    powerDown - MTD_TRUE powers the port down, MTD_FALSE powers the port up

 Outputs:
    None

 Returns:
    MTD_OK or MTD_FAIL

 Description:
    Powers the port down if powerDown is MTD_TRUE by setting 31.F001.11 <= 1,
    or powers the port back up by setting the bit to 0
    if powerDown is MTD_FALSE.

 Side effects:
    None

 Notes/Warnings:
    Once the port is powered down, the F2R cannot be used to power the port
    back up! Only MDIO write or a hardware reset is capable of setting
    the power back on.
******************************************************************************/
MTD_STATUS mtdSetPortPowerDown
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_BOOL powerDown
);

/******************************************************************************
 MTD_STATUS mtdGetPortPowerDown
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port.
     OUT MTD_BOOL *powerDown
 )

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31

 Outputs:
    powerDown - MTD_TRUE if powered down, MTD_FALSE if powered up

 Returns:
    MTD_OK or MTD_FAIL

 Description:
    Returns the value of the bit 31.F001.11, C Unit Port Power Down.

 Side effects:
    None

 Notes/Warnings:
    Once the port is powered down, the F2R cannot be used to power the port
    back up! Only MDIO write or a hardware reset is capable of setting
    the power back on.

******************************************************************************/
MTD_STATUS mtdGetPortPowerDown
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_BOOL *powerDown
);

/*******************************************************************
  C Unit Top Configuration
 *******************************************************************/


#define MTD_F2R_OFF   (0) 
#define MTD_F2R_0BHDR (1)
#define MTD_F2R_4BHDR (2)
#define MTD_F2R_8BHDR (3)
#define MTD_MS_CU_ONLY         (0)
#define MTD_MS_FBR_ONLY        (1)
#define MTD_MS_AUTO_PREFER_CU  (2)
#define MTD_MS_AUTO_PREFER_FBR (3)
#define MTD_MS_USE_REDNDT_HOST (4)
#define MTD_MS_AUTO_FIRST_LINK (7)
#define MTD_FT_1000BASEX       (0)
#define MTD_FT_SGMII_SYSTEM    (1)
#define MTD_FT_SGMII_PHY       (2) /* unlikely to be used, but hardware supports it */
#define MTD_FT_10GBASER        (3)
/******************************************************************************
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
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31
    frameToRegister - overwrites the Frame-To-Register strapped at power-on.
        One of the following:
            MTD_F2R_OFF 
            MTD_F2R_0BHDR
            MTD_F2R_4BHDR
            MTD_F2R_8BHDR    
        Changing this parameter requires a software reset for the change
        to take effect.
        
    mediaSelect - overwrites the media select strapped at power-on.
        One of the following:
            MTD_MS_CU_ONLY
            MTD_MS_FBR_ONLY
            MTD_MS_AUTO_PREFER_CU
            MTD_MS_AUTO_PREFER_FBR
            MTD_MS_USE_REDNDT_HOST
            MTD_MS_AUTO_FIRST_LINK    
        Changing this parameter requires a software reset for the change
        to take effect.
    
    fiberType - overwrites the fiber type selected by strap option. 
        One of the following:
            MTD_FT_1000BASEX
            MTD_FT_SGMII_SYSTEM
            MTD_FT_SGMII_PHY
            MTD_FT_10GBASER    
        Changing this parameter requires a software reset for the change
        to take effect.
    
    npMediaEnergyDetect - MTD_TRUE or MTD_FALSE to set the non-preferred
        media energy detect enable.
    maxPowerTunitAMDetect - MTD_TRUE or MTD_FALSE to set the max power
        consumption for T Unit auto-media detect
    softwareResetCunit - MTD_TRUE or MTD_FALSE to perfom a software reset
        after changing the top configuration.

 Outputs:
    None

 Returns:
    MTD_OK or MTD_FAIL

 Description:
    Writes C Unit configuration in 31.F000 and 31.F001 according to parameters
    passed in above. Several of these parameters require a software reset
    of the C Unit before they take effect. Pass in softwareResetCunit
    to MTD_TRUE to have this API function do the software reset after 
    changing all the values.

 Side effects:
    None

 Notes/Warnings:
    If the link is up when this function is called, the link will drop
    if softwareResetCunit is set to MTD_TRUE.

    Only use MTD_FALSE for softwareResetCunit if planning to do the C Unit
    software reset at a later time. Changes will not take effect until
    the software reset has been done. A few of these parameters do not 
    require a software reset. See the datasheet for details.

******************************************************************************/
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
);

/******************************************************************************
 MTD_STATUS mtdGetCunitTopConfig
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     OUT MTD_U16 *frameToRegister,
     OUT MTD_U16 *mediaSelect,
     OUT MTD_U16 *fiberType,
     OUT MTD_BOOL *npMediaEnergyDetect,
     OUT MTD_BOOL *maxPowerTunitAMDetect
 );


 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31

 Outputs:
    frameToRegister - current register value of this setting.
        One of the following:
        MTD_F2R_OFF 
        MTD_F2R_0BHDR
        MTD_F2R_4BHDR
        MTD_F2R_8BHDR    
    mediaSelect - current register value of this setting.
        One of the following:
        MTD_MS_CU_ONLY
        MTD_MS_FBR_ONLY
        MTD_MS_AUTO_PREFER_CU
        MTD_MS_AUTO_PREFER_FBR
        MTD_MS_USE_REDNDT_HOST
        MTD_MS_AUTO_FIRST_LINK    
    fiberType - current register value of this setting. 
        One of the following:
        MTD_FT_1000BASEX
        MTD_FT_SGMII_SYSTEM
        MTD_FT_SGMII_PHY
        MTD_FT_10GBASER    
    npMediaEnergyDetect - current register value of this bit
        MTD_TRUE or MTD_FALSE
    maxPowerTunitAMDetect - current register value of this bit
        MTD_TRUE or MTD_FALSE
        
 Returns:
    MTD_OK or MTD_FAIL

 Description:
    Reads the C Unit configuration in 31.F000 and 31.F001 and returns
    the values in the corresponding variables above.

 Side effects:
    None

 Notes/Warnings:
    The current value from the register is returned. If a software reset
    was not performed after the value was written, the values here may
    not be the currently used value.

******************************************************************************/
MTD_STATUS mtdGetCunitTopConfig
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_U16 *frameToRegister,
    OUT MTD_U16 *mediaSelect,
    OUT MTD_U16 *fiberType,
    OUT MTD_BOOL *npMediaEnergyDetect,
    OUT MTD_BOOL *maxPowerTunitAMDetect
);

#if C_LINKAGE
#if defined __cplusplus
}
#endif
#endif

#endif

