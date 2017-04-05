#include "mtdCopyright.h" 
/* Copyright 2013 Marvell International Ltd, See included file for licensing information */

/********************************************************************
This file contains functions prototypes and global defines/data for
higher-level functions using MDIO access to control and get
status of the 10GBASE-T fast retrain functionality for the Marvell 
88X3240 ethernet PHY.
********************************************************************/
#ifndef MTDFR_H
#define MTDFR_H

#if C_LINKAGE
#if defined __cplusplus
    extern "C" {
#endif
#endif

/* The Marvell Phy 88X3240/88X3140 supports two types of fast retrain:
   the standard-defined one as specified in the 10GBASE-T standard, herein
   referred to 10GBTFR and a pre-standard version in the
   Cisco specification, "Negotiated Fast Retrain Revision 2.0", herein
   referred to as NFR. Both, neither, or either may be advertised.

   Both ends must support the same type of fast retrain for the fast
   retrain to be used once the link goes into data mode. If both ends
   support both fast retrains, the 10GBTFR will be used. 

   The combined fast retrain resolution is reported in 7.32781.12:10 */
/*******************************************************************
 Fast Retrain Control/Status - Top Level Functions

 Use these functions to configure/enable the NFR/10GBTFR and to 
 check the resolution to see which fast retrain type will be used
 following auto-negotiation.

 It is suggested to use only the top-level function for configuration.

 After the link is up, the function mtdGetFastRetrainResolution()
 should be called  to determine the type of fast retrain which is being
 used. 

 While the link is up, mtdGetFastRetrainStatus() can be used to read
 the status (which will clear the count bits in 1.147) then the corresponding
 set of status functions can be called to check the status (see the list of 
 functions in the description for mtdGetFastRetrainStatus()).

 If only the 10GBTFR is desired, the functions in the 10GBTFR section
 plus mtdGetFastRetrainStatus() can be used instead of the top-level
 functions.

 If only the NFR is desired, the functions in the NFR section plus
 mtdGetFastRetrainResolution() and mtdGetFastRetrainStatus() can 
 be used instead of the top-level configuration function.

 *******************************************************************/

/* Different combinations for enabling both fast retrains */
#define MTD_DISABLE_FR          (0) /* Disable all fast retrains*/
#define MTD_ENABLE_10GBTFR_ONLY (1) /* Enable only the standard-defined fast retrain*/
#define MTD_ENABLE_NFR_ONLY     (2) /* Enable only the negotiated fast retrain*/
#define MTD_ENABLE_BOTH_FR      (3) /* Enable both*/

/* Type of signal to send MAC during fast retrain, note that both types of */
/* fast retrains share this setting. It's not possible to set different signalling */
/* for different fast retrains. */
#define MTD_FR_IDLE                       0x00
#define MTD_FR_LOCAL_FAULT                0x01
#define MTD_FR_LINK_INTRRUPTION           0x02
/*****************************************************************************
MTD_STATUS mtdConfigureFastRetrain
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    IN MTD_U16 fr_mode, 
    IN MTD_U16 frSignalType,
    IN MTD_BOOL anRestart
);

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

    fr_mode - Which fast retrains to enable and advertise. One of the following
    MTD_DISABLE_FR
    MTD_ENABLE_10GBTFR_ONLY 
    MTD_ENABLE_NFR_ONLY     
    MTD_ENABLE_BOTH_FR      

    frSignalType - type of signal to send during fast retrain. The options are ...
    MTD_FR_IDLE                       0x00
    MTD_FR_LOCAL_FAULT                0x01
    MTD_FR_LINK_INTRRUPTION           0x02

    anRestart - this takes the value of MTD_TRUE or MTD_FALSE and indicates 
                if auto-negotiation should be restarted following the speed 
                enable change. If this is MTD_FALSE, the change will not
                take effect until AN is restarted in some other way (link
                drop, toggle low power, toggle AN enable).

                If this is MTD_TRUE and AN has been disabled, it will be
                enabled before being restarted.
 
 Outputs:
    None

 Returns:
    MTD_OK or MTD_FAIL, if action was successful or failed
    
 Description: Configures the PHY to enable and advertise one, both, or none
 of the two types of fast retrains, 10GBTFR and NFR.
    
 Notes/Warnings:
    None.

******************************************************************************/
MTD_STATUS mtdConfigureFastRetrain
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    IN MTD_U16 fr_mode, 
    IN MTD_U16 frSignalType,
    IN MTD_BOOL anRestart
);

#define MTD_FR_DISABLED       (0) /* mtl  No common fast retrain found, or it's disabled*/
#define MTD_10GBTFR_RESOLVED  (1) /* mtl  Both ends using 10GBTFR*/
#define MTD_NFR_RESOLVED      (2) /* mtl  Both ends using NFR*/
/*****************************************************************************
MTD_STATUS mtdGetFastRetrainResolution
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    OUT MTD_U16 *frResolution
);

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 
 Outputs:
    frResolution - the type of fast retrain which will be used among the
        following:
             MTD_FR_DISABLED
             MTD_10GBTFR_RESOLVED
             MTD_NFR_RESOLVED
    

 Returns:
    MTD_OK or MTD_FAIL, if query was successful or not
     
 Description: This function must be called only after auto-negotiation is complete
 and the link is up at 10BASE-T speed in order
 to discover which type of fast retrain the PHY is using (if any). It uses
 the Marvell proprietary fast retrain resolution bits in 7.32781.12:10 to
 report the fast retrain resolution.
    
 Notes/Warnings:
    Check if autoneg is complete/done or if 10GBT link is up before calling this
    function.

******************************************************************************/
MTD_STATUS mtdGetFastRetrainResolution
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    OUT MTD_U16 *frResolution
);


/******************************************************************************
MTD_STATUS mtdGetFastRetrainStatus
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    IN MTD_U16 *currentValue
);

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31
    
 Outputs:
    currentValue - The value of the fast retrain status and control register
                   (1.147) 

 Returns:
    MTD_OK or MTD_FAIL, if query was successful or not

 Description:
    Reads the fast retrain status and control register (1.147) and 
    stores the value. This value is to be used later to check the status of 
    the various bits in this register by calling the functions listed below
    in the "Notes" section.

 Side effects:
    This register has self clearing bits. After this function has been
    called, those bits will be cleared.

 Notes/Warnings:
    The pointer value should be passed in to the following functions. Call
    mtdGetFastRetrainResolution() to determine which set of functions
    to use during data mode.

    If resolution was NFR, the following function is available for status:
    mtdGetLDNFRCount()

    If resolution was 10GBTFR, the following functions are available for status:
    mtdGetLP10GBTFRCount()
    mtdGetLD10GBTFRCount()
******************************************************************************/
MTD_STATUS mtdGetFastRetrainStatus
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    IN MTD_U16 *currentValue
);

    
/*******************************************************************
 Fast Retrain Control/Status - NFR-specific Functions

 Use these functions to configure/enable and check the status of
 only the NFR (negotiated fast retrain).
 *******************************************************************/

/******************************************************************************
MTD_STATUS mtdAdvertNFR
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 frSignalType,
    IN MTD_BOOL anRestart
);

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

    frSignalType - type of signal to send during fast retrain. The options are ...
    MTD_FR_IDLE                       0x00
    MTD_FR_LOCAL_FAULT                0x01
    MTD_FR_LINK_INTRRUPTION           0x02
    
    anRestart - this takes the value of MTD_TRUE or MTD_FALSE and indicates 
                if auto-negotiation should be restarted following the speed 
                enable change. If this is MTD_FALSE, the change will not
                take effect until AN is restarted in some other way (link
                drop, toggle low power, toggle AN enable).

                If this is MTD_TRUE and AN has been disabled, it will be
                enabled before being restarted.
    
 Outputs:
    None.

 Returns:
    MTD_OK or MTD_FAIL, if action was successful or not

 Description:
    Reads the 10GBASE-T Auto negotiation control register 7.49158 and sets
    bit 0 to advertise NFR capability. Also sets the signal type bits in 1.147
    (which are shared with the 10GBTFR).
    
 Side effects:
    None. 
    
 Notes/Warnings:
    Autonegotiation must be restarted before the new setting will be used. The
    link will drop if it is up anRestart is set to MTD_TRUE.

    As a side effect, this function also disables 10GBTFR. Use top-level
    configuration function to enable both.

******************************************************************************/
MTD_STATUS mtdAdvertNFR
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 frSignalType,
    IN MTD_BOOL anRestart
);

/******************************************************************************
MTD_STATUS mtdDoNotAdvertNFR
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_BOOL anRestart
)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31
    anRestart - this takes the value of MTD_TRUE or MTD_FALSE and indicates 
                if auto-negotiation should be restarted following the speed 
                enable change. If this is MTD_FALSE, the change will not
                take effect until AN is restarted in some other way (link
                drop, toggle low power, toggle AN enable).

                If this is MTD_TRUE and AN has been disabled, it will be
                enabled before being restarted.
    
 Outputs:
    None.

 Returns:
    MTD_OK or MTD_FAIL, if action was successful or not

 Description:
    Sets 7.800C.0 to 0, and optionally restarts AN, to stop advertisement
    of NFR.
    
 Side effects:
    None. 
    
 Notes/Warnings:
    Autonegotiation must be restarted before the new setting will be used. The
    link will drop if it is up and anRestart is passed as MTD_TRUE.

******************************************************************************/
MTD_STATUS mtdDoNotAdvertNFR
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_BOOL anRestart
);

/******************************************************************************
MTD_STATUS mtdLPNFR
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_BOOL *lpNFRAbility    
);

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31
    
 Outputs:
    lpNFRAbility - MTD_TRUE - Link partner is capable of NFR
                   MTD_FALSE - Link partner is not capable of NFR

 Returns:
    MTD_OK or MTD_FAIL, if query was successful or not

 Description:
    Reads the Link Partner NFR Ability register 7.800D bit 0 and
    returns the bit value.
    
 Side effects:
    None. 
    
 Notes/Warnings:
    This must be called after the auto-negotiation process has finished.

******************************************************************************/
MTD_STATUS mtdLPNFR
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_BOOL *lpNFRAbility
);

/******************************************************************************
MTD_STATUS mtdGetLDNFRCount
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 currentValue,
    OUT MTD_U16 *ldCount
);

 Inputs:
    contextPtr - pointer to host's private data
    currentValue - register value of 1.147 (fast retrain status and control reg)
        
 Outputs:
    ldCount - LD fast retrain count since last time it was "read"

 Returns:
    MTD_OK or MTD_FAIL depending on result of function

 Description:
    Gets the number of NFR fast retrains performed by the local device.
    It parses the information from the parameter passed in (currentValue).
    It returns the count.

 Side effects:
    None 
    
 Notes/Warnings:
    The function "mtdGetFastRetrainStatus" should be called first and the 
    register value should be passed in to this function.
******************************************************************************/
MTD_STATUS mtdGetLDNFRCount
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 currentValue,
    OUT MTD_U16 *ldCount
);


/*******************************************************************
 Fast Retrain Control/Status - 10GBTFR-specific Functions

 Use these functions to configure/enable and check the status of
 only the 10GBTFR (standard-defined fast retrain).
 *******************************************************************/


/******************************************************************************
MTD_STATUS mtdGetLP10GBTFRCount
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 currentValue,
    OUT MTD_U16 *lpCount
);

 Inputs:
    contextPtr - pointer to host's private data
    currentValue - register value of 1.147 (fast retrain status and control reg)
        
 Outputs:
    lpCount - LP fast retrain count since last time it was "read"

 Returns:
    MTD_OK or MTD_FAIL depending on result of function

 Description:
    Gets the number of 10GBTFR fast retrains rqeuested by the link partner.
    It parses the information from the parameter passed in (currentValue)
    and returns the count.

 Side effects:
    None. 
    
 Notes/Warnings:
    The function "mtdGetFastRetrainStatus" should be called first and the 
    register value should be passed in to this function.
******************************************************************************/
MTD_STATUS mtdGetLP10GBTFRCount
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 currentValue,
    OUT MTD_U16 *lpCount
);

/******************************************************************************
MTD_STATUS mtdGetLD10GBTFRCount
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 currentValue,
    OUT MTD_U16 *ldCount
);

 Inputs:
    contextPtr - pointer to host's private data
    currentValue - register value of 1.147 (fast retrain status and control reg)
        
 Outputs:
    ldCount - LD fast retrain count since last time it was "read"

 Returns:
    MTD_OK or MTD_FAIL depending on function success

 Description:
    Gets the number of 10GBTFR fast retrains requested by the local device.
    It parses the information from the parameter passed in (currentValue)
    and returns the count.

 Side effects:
    None 
    
 Notes/Warnings:
    The function "mtdGetFastRetrainStatus" should be called first and the 
    register value should be passed in to this function.
******************************************************************************/
MTD_STATUS mtdGetLD10GBTFRCount
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 currentValue,
    OUT MTD_U16 *ldCount
);


/******************************************************************************
MTD_STATUS mtdIs10GBTFRNegotiated
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 currentValue,
    OUT MTD_BOOL *isNegotiated
);

 Inputs:
    contextPtr - pointer to host's private data
    currentValue - register value of 1.147 (fast retrain status and control reg)
        
 Outputs:
    isNegotiated - MTD_TRUE -> 10GBTFR was negotiated
                   MTD_FALSE -> 10GBTFR was not negotiated

 Returns:
   MTD_OK or MTD_FAIL depending on function success

 Description:
    Reads the Fast retrain negotiated bit and returns the bit value.
    MTD_TRUE if 10GBTFR was negotiated and MTD_FALSE if not negotiated.
    
 Side effects:
    None 
    
 Notes/Warnings:
    The function "mtdGetFastRetrainStatus" should be called first and the 
    register value should be passed in to this function.
******************************************************************************/
MTD_STATUS mtdIs10GBTFRNegotiated
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 currentValue,
    OUT MTD_BOOL *isNegotiated
);

/******************************************************************************
MTD_STATUS mtdEnable10GBTFR
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    IN MTD_U16 frSignalType
);

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31
    frSignalType - type of signal to send for fast retrain. The options are ...
    MTD_FR_IDLE                       0x00
    MTD_FR_LOCAL_FAULT                0x01
    MTD_FR_LINK_INTRRUPTION           0x02
        
 Outputs:
    None

 Returns:
    MTD_OK or MTD_FAIL, if command was successful or not

 Description:
    Sets the signal type and enables 10GBTFR by setting the appropriate bits.
    
 Side effects:
    None. 
    
 Notes/Warnings:
    None
******************************************************************************/
MTD_STATUS mtdEnable10GBTFR
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    IN MTD_U16 frSignalType
);

/******************************************************************************
MTD_STATUS mtdDisable10GBTFR
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port
);

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31
            
 Outputs:
    None

 Returns:
    MTD_OK or MTD_FAIL, if command was successful or not

 Description:
    This function disables 10GBTFR by writing '0' to the register (which
    also clears the signal type at the same time).
    
 Side effects:
    None. 
    
 Notes/Warnings:
    This function writes '0' to the whole register, 1.147. 
    The RO bits do not get affected by this. Counts are left intact since
    the register isn't read.

    Disabling 10GBTFR after the link is up with a resolution of 10GBTFR will
    cause a complete link drop the next time a fast retrain is needed at
    either end of the link.

    Do not call this function when the link is up with a resolution of NFR,
    otherwise the signal type requested may not be provided during NFR.
******************************************************************************/
MTD_STATUS mtdDisable10GBTFR
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port
);

/******************************************************************************
MTD_STATUS mtdAdvert10GBTFR
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_BOOL anRestart
);

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31
    anRestart - this takes the value of MTD_TRUE or MTD_FALSE and indicates 
                if auto-negotiation should be restarted following the speed 
                enable change. If this is MTD_FALSE, the change will not
                take effect until AN is restarted in some other way (link
                drop, toggle low power, toggle AN enable).

                If this is MTD_TRUE and AN has been disabled, it will be
                enabled before being restarted.
    
 Outputs:
    None.

 Returns:
    MTD_OK or MTD_FAIL, if command was successful or not

 Description:
    Reads the 10GBASE-T Auto negotiation control register 7.32 and sets
    bit 1 to advertise 10GBASE-T fast retrain capability.
    
 Side effects:
    None. 
    
 Notes/Warnings:
    Autonegotiation must be restarted before the new setting will be used. The
    link will drop if it is up and anRestart is passed as MTD_TRUE.

******************************************************************************/
MTD_STATUS mtdAdvert10GBTFR
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_BOOL anRestart
);

/******************************************************************************
MTD_STATUS mtdDoNotAdvert10GBTFR
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 anRestart
);

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31
    anRestart - this takes the value of MTD_TRUE or MTD_FALSE and indicates 
                if auto-negotiation should be restarted following the speed 
                enable change. If this is MTD_FALSE, the change will not
                take effect until AN is restarted in some other way (link
                drop, toggle low power, toggle AN enable).

                If this is MTD_TRUE and AN has been disabled, it will be
                enabled before being restarted.
    
 Outputs:
    None.

 Returns:
    MTD_OK or MTD_FAIL, if command was successful or not

 Description:
    Reads the 10GBASE-T Auto negotiation control register 7.32 and sets
    bit 1 to not advertise 10GBTFR capability.
    
 Side effects:
    None. 
    
 Notes/Warnings:
    Autonegotiation must be restarted before the new setting will be used. The
    link will drop if it is up and restart_an is passed as 1.

******************************************************************************/
MTD_STATUS mtdDoNotAdvert10GBTFR
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 anRestart
);

/******************************************************************************
MTD_STATUS mtdLP10GBTFR
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_BOOL *lp10BTFRAble
);

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31
    
 Outputs:
    lp10BTFRAble - MTD_TRUE if the link partner advertised 10GBASE-T Fast
                   Retrain ability, MTD_FALSE otherwise

 Returns:
    MTD_OK or MTD_FAIL, if query was successful or not

 Description:
    Reads the Link Partner Fast Retrain ability  register 7.33 bit 1 and
    returns the bit value in lp10BTFRAble.
    
 Side effects:
    None. 
    
 Notes/Warnings:
    This must be called after the auto-negotiation process has finished.

******************************************************************************/
MTD_STATUS mtdLP10GBTFR
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_BOOL *lp10BTFRAble
);

#if C_LINKAGE
#if defined __cplusplus
}
#endif
#endif

#endif /* defined MTDFR_H */
