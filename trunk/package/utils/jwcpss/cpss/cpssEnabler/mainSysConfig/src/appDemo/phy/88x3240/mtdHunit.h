#include "mtdCopyright.h" 
/* Copyright 2013 Marvell International Ltd, See included file for licensing information */

/********************************************************************
This file contains functions prototypes and global defines/data for
higher-level functions of the Host Interface Unit (H Unit) of 
the 88X32X0 PHY.
********************************************************************/
#ifndef MTDHUNIT_H
#define MTDHUNIT_H
#if C_LINKAGE
#if defined __cplusplus 
    extern "C" { 
#endif 
#endif

/******************************************************************************
 Mac Interface functions
******************************************************************************/

/* for macType */
#define MTD_MAC_TYPE_RXAUI_SGMII_AN_EN  (0x0)
#define MTD_MAC_TYPE_RXAUI_SGMII_AN_DIS (0x1)
#define MTD_MAC_TYPE_RXAUI_RATE_ADAPT   (0x2)
#define MTD_MAC_TYPE_XFI_SGMII_AN_EN    (0x4)
#define MTD_MAC_TYPE_XFI_SGMII_AN_DIS   (0x5)
#define MTD_MAC_TYPE_XFI_RATE_ADAPT     (0x6)
#define MTD_MAC_TYPE_SXGMII             (0x7)
#define MTD_MAC_LEAVE_UNCHANGED         (0x8) /* use this option to not touch these bits */
/* for macIfSnoopSel */
#define MTD_MAC_SNOOP_FROM_NETWORK      (0x2)
#define MTD_MAC_SNOOP_FROM_HOST         (0x3)
#define MTD_MAC_SNOOP_OFF               (0x0)
#define MTD_MAC_SNOOP_LEAVE_UNCHANGED   (0x4) /* use this option to not touch these bits */
/* for macLinkDownSpeed */
#define MTD_MAC_SPEED_10_MBPS            MTD_CU_SPEED_10_MBPS 
#define MTD_MAC_SPEED_100_MBPS           MTD_CU_SPEED_100_MBPS
#define MTD_MAC_SPEED_1000_MBPS          MTD_CU_SPEED_1000_MBPS
#define MTD_MAC_SPEED_10_GBPS            MTD_CU_SPEED_10_GBPS
#define MTD_MAC_SPEED_LEAVE_UNCHANGED    (0x4)
/******************************************************************************
 MTD_STATUS mtdSetMacInterfaceControl
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     IN MTD_U16 macType,
     IN MTD_BOOL macIfPowerDown,
     IN MTD_U16 macIfSnoopSel,
     IN MTD_U16 macIfActiveLaneSelect,
     IN MTD_U16 macLinkDownSpeed,
     IN MTD_BOOL doSwReset
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31
    macType - the type of MAC interface being used (the hardware interface). One of the following:
        MTD_MAC_TYPE_RXAUI_SGMII_AN_EN - selects RXAUI with SGMII AN enabled
        MTD_MAC_TYPE_RXAUI_SGMII_AN_DIS - selects RXAUI with SGMII AN disabled
        MTD_MAC_TYPE_RXAUI_RATE_ADAPT  - selects RXAUI with rate matching
        MTD_MAC_TYPE_XFI_SGMII_AN_EN - selects XFI with SGMII AN enabled
        MTD_MAC_TYPE_XFI_SGMII_AN_DIS - selects XFI with SGMII AN disabled
        MTD_MAC_TYPE_XFI_RATE_ADAPT  - selects XFI with rate matching
        MTD_MAC_TYPE_SXGMII - selects XFI with byte replication
        MTD_MAC_LEAVE_UNCHANGED - option to leave this parameter unchanged/as it is           
    macIfPowerDown - MTD_TRUE if the host interface is always to be powered up
                     MTD_FALSE if the host interface can be powered down under 
                         certain circumstances (see datasheet)
    macIfSnoopSel - If snooping is requested on the other lane, selects the source
        MTD_MAC_SNOOP_FROM_NETWORK - source of snooped data is to come from the network        
        MTD_MAC_SNOOP_FROM_HOST - source of snooped data is to come from the host
        MTD_MAC_SNOOP_OFF - snooping is to be turned off
        MTD_MAC_SNOOP_LEAVE_UNCHANGED - option to leave this parameter unchanged/as it is
    macIfActiveLaneSelect - For redundant host mode, this selects the active lane. 0 or 1
        only. 0 selects 0 as the active lane and 1 as the standby. 1 selects the other way.
    macLinkDownSpeed - The speed the mac interface should run when the media side is
        link down. One of the following:
            MTD_MAC_SPEED_10_MBPS
            MTD_MAC_SPEED_100_MBPS
            MTD_MAC_SPEED_1000_MBPS
            MTD_MAC_SPEED_10_GBPS  
            MTD_MAC_SPEED_LEAVE_UNCHANGED        
    doSwReset - MTD_TRUE if a software reset (31.F001.15) should be done after these changes
        have been made, or MTD_FALSE otherwise. See note below.
    

 Outputs:
    None

 Returns:
    MTD_OK or MTD_FAIL if a bad parameter was passed, or an IO error occurs.

 Description:
    Changes the above parameters as indicated in 31.F000 and 31.F001 and
    optionally does a software reset afterwards for those bits which require a 
    software reset to take effect.

 Side effects:
    None

 Notes/Warnings:
    These bits are actually in the C unit, but pertain to the host interface
    control so the API called was placed here.

    Changes to the MAC type (31.F001.2:0) do not take effect until a software
    reset is performed on the port.

    Changes to macLinkDownSpeed (31.F001.7:6) require 2 software resets to
    take effect. This function will do 2 resets if doSwReset is MTD_TRUE
    and macLinkDownSpeed is being changed.

    IMPORTANT: the readback reads back the last written value following
    a software reset. Writes followed by reads without an intervening
    software reset will read back the old bit value for all those bits
    requiring a software.

    Because of this, read-modify-writes to different bitfields must have an
    intervening software reset to pick up the latest value before doing
    another read-modify-write to the register, otherwise the bitfield
    may lose the value.

    Suggest always setting doSwReset to MTD_TRUE to avoid problems of
    possibly losing changes.

******************************************************************************/
MTD_STATUS mtdSetMacInterfaceControl
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 macType,
    IN MTD_BOOL macIfPowerDown,
    IN MTD_U16 macIfSnoopSel,
    IN MTD_U16 macIfActiveLaneSelect,
    IN MTD_U16 macLinkDownSpeed,
    IN MTD_BOOL doSwReset
);

/******************************************************************************
 MTD_STATUS mtdGetMacInterfaceControl
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     OUT MTD_U16 *macType,
     OUT MTD_BOOL *macIfPowerDown,
     OUT MTD_U16 *macIfSnoopSel,
     OUT MTD_U16 *macIfActiveLaneSelect,
     OUT MTD_U16 *macLinkDownSpeed
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31
    
 Outputs:
    macType - the type of MAC interface being selected. One of the following:
        MTD_MAC_TYPE_RXAUI_SGMII_AN_EN -  RXAUI with SGMII AN enabled
        MTD_MAC_TYPE_RXAUI_SGMII_AN_DIS -  RXAUI with SGMII AN disabled
        MTD_MAC_TYPE_RXAUI_RATE_ADAPT  -  RXAUI with rate matching
        MTD_MAC_TYPE_XFI_SGMII_AN_EN -  XFI with SGMII AN enabled
        MTD_MAC_TYPE_XFI_SGMII_AN_DIS -  XFI with SGMII AN disabled
        MTD_MAC_TYPE_XFI_RATE_ADAPT  -  XFI with rate matching
        MTD_MAC_TYPE_SXGMII -  XFI with byte replication
    macIfPowerDown - MTD_TRUE if the host interface is always to be powered up
                     MTD_FALSE if the host interface can be powered down under 
                         certain circumstances (see datasheet)
    macIfSnoopSel - Indicates if snooping is turned on and source
        MTD_MAC_SNOOP_FROM_NETWORK - source of snooped data is to come from the network        
        MTD_MAC_SNOOP_FROM_HOST - source of snooped data is to come from the host
        MTD_MAC_SNOOP_OFF - snooping is to be turned off        
    macIfActiveLaneSelect - For redundant host mode, this indicates the active lane. 0 or 1
        only. 0 indicates 0 as the active lane and 1 as the standby. 
        1 indicates the other way.
    macLinkDownSpeed - The speed the mac interface should run when the media side is
        link down. One of the following:
            MTD_MAC_SPEED_10_MBPS
            MTD_MAC_SPEED_100_MBPS
            MTD_MAC_SPEED_1000_MBPS
            MTD_MAC_SPEED_10_GBPS           

 Returns:
    MTD_OK or MTD_FAIL if query failed (for exampe an IO error occured).

 Description:
    Reads parameters as currently selected in 31.F000 and 31.F001 and returns them.

 Side effects:
    None

 Notes/Warnings:
    These bits are actually in the C unit, but pertain to the host interface
    control so the API called was placed here.

    Changes to the MAC type (31.F001.2:0) and link down speed (31.F000.7:6) 
    do not take effect until a software reset is performed on the port. 

    This function returns what's been written to the register, but may not 
    indicate what is currently being used if a software reset was not 
    performed after the MAC type was changed).

******************************************************************************/
MTD_STATUS mtdGetMacInterfaceControl
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_U16 *macType,
    OUT MTD_BOOL *macIfPowerDown,
    OUT MTD_U16 *macIfSnoopSel,
    OUT MTD_U16 *macIfActiveLaneSelect,
    OUT MTD_U16 *macLinkDownSpeed
);


/******************************************************************************
 SGMII functions
******************************************************************************/

/******************************************************************************
 MTD_STATUS mtdGetSGMIIAutoneg_Advertisement
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     OUT MTD_BOOL *link_status,
     OUT MTD_U16 *speed,
     OUT MTD_U16 *full_duplex,
     OUT MTD_BOOL *tx_pause,
     OUT MTD_BOOL *rx_pause,
     OUT MTD_BOOL *fiber_media,
     OUT MTD_BOOL *eee_enabled,
     OUT MTD_BOOL *clock_stop_lpi
 );


 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31
    link_status - MTD_TRUE if PHY link is up, MTD_FALSE if link is down
    full_duplex - MTD_TRUE if PHY resolved to full duplex, MTD_FALSE if
                  PHY resolved to half duplex
    speed - one of the following:
       MTD_SGMII_SPEED_10M
       MTD_SGMII_SPEED_100M  
       MTD_SGMII_SPEED_1G    
    tx_pause - MTD_TRUE if transmit pause is enabled, MTD_FALSE if not
    rx_pause - MTD_TRUE if receive pause is enabled, MTD_FALSE if not
    fiber_media - MTD_TRUE if resolved to fiber media, MTD_FALSE if copper
    eee_enabled - MTD_TRUE if PHY resolved to use EEE, MTD_FALSE if not
    clock_stop_lpi - MTD_TRUE if the clock stops during LPI for EEE,
                     MTD_FALSE if not

 Outputs:
    None

 Returns:
    MTD_OK or MTD_FAIL

 Description:
    This function queries the SGMII autoneg result in 4.2004 (H unit
    auto-negotiation advertisement register) and returns the results.

    This function should be called after autonegotiation is complete to
    read the result.

 Side effects:
    None

 Notes/Warnings:
    None

******************************************************************************/
MTD_STATUS mtdGetSGMIIAutoneg_Advertisement
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_BOOL *link_status,
    OUT MTD_BOOL *full_duplex,
    OUT MTD_U16 *speed,
    OUT MTD_BOOL *tx_pause,
    OUT MTD_BOOL *rx_pause,
    OUT MTD_BOOL *fiber_media,
    OUT MTD_BOOL *eee_enabled,
    OUT MTD_BOOL *clock_stop_lpi
);


/******************************************************************************
 X2 (RXAUI) functions
******************************************************************************/

/******************************************************************************
 MTD_STATUS mtdSetX2SerdesLanePolarity
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     IN MTD_BOOL invert_input_pol_l0,
     IN MTD_BOOL invert_output_pol_l0,
     IN MTD_BOOL invert_input_pol_l1,
     IN MTD_BOOL invert_output_pol_l1
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31
    invert_input_pol_l0 - MTD_TRUE to invert the serdes input polarity, 
                          MTD_FALSE to leave it as-is (lane 0)
    invert_output_pol_l0 - MTD_TRUE to invert the serdes output polarity, 
                           MTD_FALSE to leave it as-is (lane 0)
    invert_input_pol_l1 - MTD_TRUE to invert the serdes input polarity, 
                          MTD_FALSE to leave it as-is (lane 1)
    invert_output_pol_l1 - MTD_TRUE to invert the serdes output polarity, 
                           MTD_FALSE to leave it as-is (lane 1)

 Outputs:
    None

 Returns:
    MTD_OK or MTD_FAIL

 Description:
    This function can be used to invert the input or output polarity of
    lane 0 or lane 1 in 10GBASE-X2 (RXAUI) mode.

 Side effects:
    None

 Notes/Warnings:
    In 10GBASE-X2 (RXAUI) mode, both lanes are controlled by 4.F004.15:12.    

    Call mtdSetSerdesLanePolarity() to control the polarity of the
    serdes lanes in 10GBASE-R, 1000BASE-X/SGMII modes.

******************************************************************************/
MTD_STATUS mtdSetX2SerdesLanePolarity
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_BOOL invert_input_pol_l0,
    IN MTD_BOOL invert_output_pol_l0,
    IN MTD_BOOL invert_input_pol_l1,
    IN MTD_BOOL invert_output_pol_l1
);


/******************************************************************************
 MTD_STATUS mtdGetX2SerdesLanePolarity
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     IN MTD_BOOL *invert_input_pol_l0,
     IN MTD_BOOL *invert_output_pol_l0,
     IN MTD_BOOL *invert_input_pol_l1,
     IN MTD_BOOL *invert_output_pol_l1    
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31
    HorXunit - MTD_H_UNIT or MTD_X_UNIT, depending on which lane is
                being gotten (see description below)

 Outputs:
    invert_input_pol - MTD_TRUE the serdes input polarity is inverted, 
                       MTD_FALSE it's not inverted
    invert_output_pol - MTD_TRUE the serdes output polarity is inverted, 
                       MTD_FALSE it's not inverted

 Returns:
    MTD_OK or MTD_FAIL

 Description:
    This function can be called to read the current polarity setting
    in 10GBASE-X2 (RXAUI) mode.

 Side effects:
    None

 Notes/Warnings:
    In 10GBASE-X2 (RXAUI) mode, both lanes are controlled by 4.F004.15:12.    

    Call mtdGetSerdesLanePolarity() to get the polarity of the
    serdes lanes in 10GBASE-R, 1000BASE-X/SGMII modes.

******************************************************************************/
MTD_STATUS mtdGetX2SerdesLanePolarity
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_BOOL *invert_input_pol_l0,
    IN MTD_BOOL *invert_output_pol_l0,
    IN MTD_BOOL *invert_input_pol_l1,
    IN MTD_BOOL *invert_output_pol_l1    
);


#if C_LINKAGE
#if defined __cplusplus
}
#endif
#endif


#endif

