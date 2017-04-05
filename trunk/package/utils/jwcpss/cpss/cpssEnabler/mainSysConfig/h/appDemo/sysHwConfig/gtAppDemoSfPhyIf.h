/********************************************************************
This file contains functions prototypes and global defines/data for
higher-level functions using MDIO access to perform resets, perform
queries of the phy, etc. that are necessary to control and read
status of the Solarflare SFX7101 and SFT9001 10GBASE-T ethernet phy.

Copyright (c) 2010, Solarflare Communications
See copyright and confidentiality notice at the bottom of this file 
for details
********************************************************************/
#ifndef SFPHY_H
#define SFPHY_H
#if SF_EVK_TEST_ENVIRONMENT == 0
#if defined __cplusplus 
    extern "C" { 
#endif 
#endif

#define VERSION_MAJOR 1
#define VERSION_MINOR 5

/******************************************************************************
 unsigned short SFPhyGetAPIVersion(void)

 Inputs:
    None

 Outputs:
    None

 Returns:
    Upper 8 bits contains major version number
    Lower 8 bits contains minor version number

 Description:
    Gets the version of the API code represented as a 16-bit unsigned
    short. Upper 8 bits is major version, lower 8 bits is minor version number.

 Side effects:
    None

 Notes/Warnings:
    None

******************************************************************************/
unsigned short SFPhyGetAPIVersion(void);


/*******************************************************************
 Resets
 Download Mode
 Low Power Mode
 *******************************************************************/

/******************************************************************************
void SFPhySpecialSoftwareReset(CTX_PTR_TYPE contextPtr, unsigned long port)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    None

 Description:
    Performs a special software reset (1.49152.15 <= 1)
    The special software reset behaves the same as a hardware reset to the phy.
    Setting this bit is equivalent to toggling the XRESETn or
    power-on reset. Writing to this bit forces an internally generated reset 
    regardless of the state of the firmware. This bit is self clearing.

    If the pin FLASH_CFG[1] is held high when this function is called, the phy 
    will enter download mode expecting a download to flash or RAM. If the pin 
    is held low when this function is called the phy will upload the application 
    code from flash and start the application code.

 Side effects:

 Notes/Warnings:
    Hardware reset should complete within 1 ms. Phy application code should be 
    loaded and ready within 500ms (if phy is not in download mode).

    Use SFPhyIsPhyReadyAfterHWReset() to check if this reset has completed. 
    Use SFPhyIsPhyReadyAfterReset() to see if application code has loaded 
    from flash and is ready to accept MDIO commands.

******************************************************************************/
void SFPhySpecialSoftwareReset(CTX_PTR_TYPE contextPtr, unsigned long port);

/******************************************************************************
unsigned short SFPhyIsPhyInMdioDownloadMode(CTX_PTR_TYPE contextPtr, 
                                            unsigned long port)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    1   if it is waiting for download from the MDIO
    0   if it is not maiting for download.

 Description:
    Reads certain bits of register 3.53248 and returns '1' if waiting 
    for download and '0' 
    if not.

 Side effects:
    None.

 Notes/Warnings:
    None.

******************************************************************************/
unsigned short SFPhyIsPhyInMdioDownloadMode(CTX_PTR_TYPE contextPtr, 
                                            unsigned long port);

#define HARDWARE_RESET          0x00  // Power-on reset, hardware reset or 
                                      /* special software reset */
#define WATCHDOG_RESET          0x01  // Watchdog timer expired
#define SOFTWARE_RESET          0x02  // Phy reset itself
#define WDT_SW_RESET            0x03  // Both watchdog and a software reset
/* Returns value in 3.53248.9:8, one of the above */
/******************************************************************************
unsigned short SFPhyGetPhyResetReason(CTX_PTR_TYPE contextPtr, 
                                      unsigned long port)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    0x00    -   HARDWARE_RESET
    0x01    -   WATCHDOG_RESET    
    0x02    -   SOFTWARE_RESET
    0x03    -   WDT_SW_RESET
 Description:
    Reads certain bits of register 3.53248 and returns the above values.

 Side effects:
    None.

 Notes/Warnings:
    None.

******************************************************************************/
unsigned short SFPhyGetPhyResetReason(CTX_PTR_TYPE contextPtr, 
                                      unsigned long port);

/******************************************************************************
unsigned short SFPhyDidPhyAppCodeStart(CTX_PTR_TYPE contextPtr, 
                                       unsigned long port)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    1   if it did start
    0   if it did not start
 Description:
    Reads certain bits of register 3.53248 and returns the above values.

 Side effects:
    None.

 Notes/Warnings:
    None.

******************************************************************************/
unsigned short SFPhyDidPhyAppCodeStart(CTX_PTR_TYPE contextPtr, 
                                       unsigned long port);

/******************************************************************************
unsigned short SFPhyFlashUploadedChecksumGood(CTX_PTR_TYPE contextPtr, 
                                              unsigned long port)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    1   if code was uploaded to RAM from flash and checksum was good
    0   if checksum was not good
 Description:
    Reads certain bits of register 3.53248 and returns the above values.

 Side effects:
    None.

 Notes/Warnings:
    None.

******************************************************************************/
unsigned short SFPhyFlashUploadedChecksumGood(CTX_PTR_TYPE contextPtr, 
                                              unsigned long port);

/******************************************************************************
unsigned short SFPhyCheckForFatalBootError(CTX_PTR_TYPE contextPtr, 
                                              unsigned long port)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    1   if PHY had a fatal boot error 
        e.g. bad flash image and set to load from flash
    0   if good
 Description:
    Reads certain bits of register 3.53248 and returns the above values.

 Side effects:
    None.

 Notes/Warnings:
    None.

******************************************************************************/
unsigned short SFPhyCheckForFatalBootError(CTX_PTR_TYPE contextPtr, 
                                         unsigned long port);

/******************************************************************************
unsigned short SFPhyIsPhyReadyAfterReset(CTX_PTR_TYPE contextPtr, 
                                         unsigned long port)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    Returns 1 if reset has completed
    Returns 0 if reset is in progress

 Description:
    This function reads the register x.1.0.15 and checks if the phy application 
    code has loaded and started. It returns a value "1" meaning reset is 
    completed and code started and a "0" if reset is in progress or if the 
    phy is in download mode.

    MDIO commands (other than download-related and hardware reset commands) 
    should not be sent before this function returns a 1.

 Side effects:

 Notes/Warnings:
    To query if reset has completed and phy is ready to accept
    MDIO commands (app code started successfully and reset is complete).

    Should complete within 500ms following a special software reset, power-on
    reset or hardware reset.

    If Phy is in download mode (pin FLASH_CFG[1] is high), this function will
    always return 0.

******************************************************************************/
unsigned short SFPhyIsPhyReadyAfterReset(CTX_PTR_TYPE contextPtr, 
                                         unsigned long port);

/******************************************************************************
unsigned short SFPhyIsPhyReadyAfterHWReset(CTX_PTR_TYPE contextPtr, 
                                           unsigned long port)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    Returns 1 if reset has completed
    Returns 0 if reset is in progress

 Description:
    This function reads the register 1.49152.15 (the special software reset 
    bit) once and if hardware reset is completed it returns a value "1" 
    meaning reset is completed and a "0" if reset is in progress.

 Side effects:

 Notes/Warnings:
    Checks if special software reset is complete. To check if Phy application
    code is loaded and ready to accept MDIO commands use 
    SFPhyIsPhyReadyAfterReset().

******************************************************************************/
unsigned short SFPhyIsPhyReadyAfterHWReset(CTX_PTR_TYPE contextPtr, 
                                           unsigned long port);

/******************************************************************************
void SFPhyPutInFwLowPowerModeFirmware(CTX_PTR_TYPE contextPtr, 
                                      unsigned long port)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    None

 Description:
    This function uses a firmware register bit to put the PHY in low power 
    mode by writing a '1' to 1.49152.12.

 Side effects:

 Notes/Warnings:
    To remove the PHY from low power the PHY has to be reset.
    The user may use either the hardware reset or special software reset or 
    POR only. Alternatively the user may call the function 
    "SFPhyRemoveFwLowPowerMode".

    The user can also use a pin to put the PHY in low power mode (PWD_INn), 
    in which case the pin must be set low to assert low power mode. To exit 
    low power mode the pin is deasserted and a reset must be generated to 
    remove the phy from lower power mode.

******************************************************************************/
void SFPhyPutInFwLowPowerModeFirmware(CTX_PTR_TYPE contextPtr, 
                                      unsigned long port);

/******************************************************************************
void SFPhyRemoveFwLowPowerMode(CTX_PTR_TYPE contextPtr, unsigned long port)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    None

 Description:
    This function calls the function "SFPhySpecialSoftwareReset()".

 Side effects:
    Resets the phy, reloads the app code from flash and restarts the phy."

 Notes/Warnings:


******************************************************************************/
void SFPhyRemoveFwLowPowerMode(CTX_PTR_TYPE contextPtr, unsigned long port);

/****************************************************************************/
/*******************************************************************
  Firmware Version
 *******************************************************************/
/****************************************************************************/

/******************************************************************************
void SFPhyGetFirmwareVersion(CTX_PTR_TYPE contextPtr, unsigned long port, 
                             unsigned short *major,
                             unsigned short *minor, 
                             unsigned short *inc,
                             unsigned short *test)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    major - major version, X.Y.Z.W, the X
    minor - minor version, X.Y.Z.W, the Y
    inc   - incremental version, X.Y.Z.W, the Z
    test  - test version, X.Y.Z.W, the W, should be 0 for released code, 
            non-zero indicates this is a non-released code

 Returns:
    None

 Description:
    This function reads the firmware version number and stores it in the
    pointers passed in by the user.

 Side effects:
    None


 Notes/Warnings:
    This function returns 0 if the phy is in download mode. The phy 
    application code must have started and be ready before issuing this 
    command.

******************************************************************************/
void SFPhyGetFirmwareVersion(CTX_PTR_TYPE contextPtr, unsigned long port, 
                             unsigned short *major, 
                             unsigned short *minor, 
                             unsigned short *inc, 
                             unsigned short *test);

/******************************************************************************
unsigned short SFPhyIsSolarflarePhy(CTX_PTR_TYPE contextPtr, 
                                    unsigned long port, 
                                    unsigned short *modelNo, 
                                    unsigned short *revNo)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    modelNo - model number, see PRM for details in section on registers 1.2 
              and 1.3
    revNo - chip revision number, see PRM for details in section on registers 
            1.2 and 1.3

 Returns:
    1 - Is a Solarflare phy
    0 - Is not a Solarflare phy

 Description:
    This function reads register 1.2 and 1.3 and checks the OUI fields to make
    sure it matches the Solarflare OUI of 00-0A-68. It returns true  if the 
    OUI matches, and false if not.

    It returns the model number and chip revision from register 1.3.

 Side effects:
    None


 Notes/Warnings:
    In download mode, the chip revision must be read from 3.53249. In
    download mode the model number and chip revision are 0.

    On SFT 9001 chips and later works in all modes. For SFT 910X
    the phy being in download mode will return incorrect model number
    when using this function.
    Check if the phy is in download mode if working with these revisions
    before calling this function.

******************************************************************************/
unsigned short SFPhyIsSolarflarePhy(CTX_PTR_TYPE contextPtr, 
                                    unsigned long port, 
                                    unsigned short *modelNo, 
                                    unsigned short *revNo);

/*******************************************************************
 Enabling speeds
 Reading enabled speeds
 *******************************************************************/
#define SPEED_100M      0x0002 /*  This is full-duplex */
#define SPEED_1GIG      0x0004 /*  This is full-duplex */
#define SPEED_10GIG     0x0008
#define SPEED_100M_HD   0x0100 /*  This is half-duplex */
#define SPEED_OFF       0x00F0 /*  Speeds are not advertised. */
/* #define SPEED_1GIG_HD 0x0200  This is half-duplex  (Not supported for SGMII) */

/******************************************************************************
 void SFPhyEnableSpeeds(CTX_PTR_TYPE contextPtr, unsigned long port,
                        unsigned short speed_bits, 
                        unsigned short anRestart, 
                        )

 Inputs: 2
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31
    speed_bits - speeds to be advertised during auto-negotiation. One or more 
                 of the following:
                    SPEED_100M
                    SPEED_1GIG
                    SPEED_10GIG
                    SPEED_100M_HD
                    SPEED_OFF
    anRestart - this takes the value of '1' or '0' and indicates if 
                auto-negotiation should be restarted following the speed 
                enable change

 Outputs:
    None

 Returns:
    None

 Description:
    This function allows the user to select the speeds to be advertised to the 
    link partner during auto-negotiation.

    The function takes in a 16 bit value and writes to the appropriate bits to 
    enable one or more speeds by writing 1.49192.

    The function also checks if the input parameter is SPEED_OFF, in which case 
    all speeds are disabled effectively disabling the phy from training 
    (but not disabling auto-negotiation).

    If anRestart is 1, an auto-negotiation restart is issued making the change 
    immediate. If anRestart is 0, the change will not take effect until the 
    next time auto-negotiation restarts.

 Side effects:

 Notes/Warnings:
    Following speed enable, restart auto-negotiation to attempt to train
    the highest speed/highest duplex enabled (matching the far end)

    Example:
    To train the highest speed matching the far end among
    either 1000BASE-T Full-duplex or 10GBASE-T, (100BASE-T disabled):
    SFPhyEnableSpeeds(SPEED_1GIG | SPEED_10GIG);
    PhyRestartAutoneg();

    To allow only 10GBASE-T to train:
    SFPhyEnableSpeeds(SPEED_10GIG);
    PhyRestartAutoneg();

******************************************************************************/
void SFPhyEnableSpeeds(CTX_PTR_TYPE contextPtr, unsigned long port,
                       unsigned short speed_bits, unsigned short anRestart);

/******************************************************************************
 unsigned short SFPhyGetSpeedsEnabled(CTX_PTR_TYPE contextPtr, 
                                      unsigned long port)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    SPEED_100M
    SPEED_1GIG
    SPEED_10GIG
    SPEED_100M_HD

 Description:
    This function returns the speeds that have been enabled.

 Side effects:

 Notes/Warnings:
    Speeds enabled don't take effect until auto-negotiation is restarted.

******************************************************************************/
unsigned short SFPhyGetSpeedsEnabled(CTX_PTR_TYPE contextPtr, 
                                     unsigned long port);

/*******************************************************************
   802.3 Clause 28
   Clause 45 Auto-negotiation Related Control & Status
 *******************************************************************/

/******************************************************************************
 void SFPhyAutonegRestart(CTX_PTR_TYPE contextPtr, unsigned long port)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    None

 Description:
    Restarts auto-negotiation. The bit is self-clearing. If the link is up,
    the link will drop and auto-negotiation will start again.

 Side effects:
    None.

 Notes/Warnings:
    Restarting auto-negotiation will have no effect if auto-negotiation is 
    disabled.

    This function is important as it is necessary to restart auto-negotiation 
    after changing many auto-negotiation settings before the changes will take 
    effect.

******************************************************************************/
void SFPhyAutonegRestart(CTX_PTR_TYPE contextPtr, unsigned long port);

/******************************************************************************
 void SFPhyAutonegDisable(CTX_PTR_TYPE contextPtr, unsigned long port)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    None

 Description:
    Turns off the enable auto-negotiation bit disabling auto-negotiation. 
    The phy will not train with auto-negotiation disabled (except in 100M 
    forced mode).

 Side effects:

 Notes/Warnings:
    None

******************************************************************************/
void SFPhyAutonegDisable(CTX_PTR_TYPE contextPtr, unsigned long port);

/******************************************************************************
 void SFPhyAutonegEnable(CTX_PTR_TYPE contextPtr, unsigned long port)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    None

 Description:
    Re-enables auto-negotiation.

 Side effects:

 Notes/Warnings:
    None

******************************************************************************/
void SFPhyAutonegEnable(CTX_PTR_TYPE contextPtr, unsigned long port);

/******************************************************************************
 void SFPhyAutonegForceMaster(CTX_PTR_TYPE contextPtr, unsigned long port,
                                unsigned short anRestart)

 Inputs:
    contextPtr - pointer to host's private data
    anRestart - Set to 1 to restart auto-negotiation after the change, 
                0 will not do a restart
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    None

 Description:
    Sets auto-negotiation to manual and forced as a master.

    Restarts auto-negotiation by calling the function only if the input 
    parameter anRestart was selected. Also re-enables auto-negotiation before 
    restarting auto-negotiation.

 Side effects:
    auto-negotiation has to be restarted for it to take effect

 Notes/Warnings:
    auto-negotiation will not complete if far end is also forced as a master.

******************************************************************************/
void SFPhyAutonegForceMaster(CTX_PTR_TYPE contextPtr, unsigned long port,
                                unsigned short anRestart);

/******************************************************************************
 void SFPhyAutonegForceSlave(CTX_PTR_TYPE contextPtr, unsigned long port,
                        unsigned short anRestart) 
                          

 Inputs:
    contextPtr - pointer to host's private data
    anRestart - Set to 1 to restart auto-negotiation after the change, 0 will 
                not do a restart
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    None

 Description:
    Sets auto-negotiation to manual and forced as a slave.

    Restarts auto-negotiation by calling the function only if the input parameter 
    anRestart was selected. Also re-enables auto-negotiation before restarting 
    auto-negotiation.

 Side effects:
    auto-negotiation has to be restarted for it to take effect

 Notes/Warnings:
    auto-negotiation will not complete if far end is also forced as a slave.

******************************************************************************/
void SFPhyAutonegForceSlave(CTX_PTR_TYPE contextPtr, unsigned long port,
                        unsigned short anRestart);

/******************************************************************************
 void SFPhyAutonegSetAutoMasterSlave(CTX_PTR_TYPE contextPtr, 
                                            unsigned long port, 
                                     unsigned short anRestart) 
                                     

 Inputs:
    contextPtr - pointer to host's private data
    anRestart - Set to 1 to restart auto-negotiation after the change, 
                0 will not do a restart
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    None

 Description:
    Sets the master/slave determination to "auto" which means the 
    master/slave setting will be random from one auto-negotiation to the next 
    auto-negotiation.

    Restarts auto-negotiation by calling the function only if the input 
    parameter anRestart was selected. Also re-enables auto-negotiation before 
    restarting auto-negotiation.

 Side effects:
    auto-negotiation has to be restarted for it to take effect

******************************************************************************/
void SFPhyAutonegSetAutoMasterSlave(CTX_PTR_TYPE contextPtr, 
                                            unsigned long port, 
                                     unsigned short anRestart);

/******************************************************************************
 void SFPhyDisablePhy(CTX_PTR_TYPE contextPtr, unsigned long port,
                            unsigned short anRestart)
                      

 Inputs:
    contextPtr - pointer to host's private data
    anRestart - Set to 1 to restart auto-negotiation after the change, 
                0 will not do a restart
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    None

 Description:
    Disables advertising any speed, enables auto-negotiation and restarts 
    auto-negotiation to force a link drop and then disables auto-negotiation 
    from continuing. Phy will be quiet on the line after calling this function 
    (if anRestart was 1).

    If auto-negotiation was not restarted (anRestart was 0), the phy will go quiet 
    the next time auto-negotiation starts (following a link drop if the link was 
    up, or following a restart auto-negotiation if auto-negotiation was running 
    when this change was made).

 Side effects:
    None

 Notes/Warnings:
    The firmware will not proceed with the training unless auto-negotiation 
    completes. Disabling auto-negotiation will disable the PHY.

******************************************************************************/
void SFPhyDisablePhy(CTX_PTR_TYPE contextPtr, unsigned long port,
                            unsigned short anRestart);

/******************************************************************************
 void SFPhyAutonegSetMasterPreference(CTX_PTR_TYPE contextPtr,
                                        unsigned long port,
                                     unsigned short anRestart)

 Inputs:
    contextPtr - pointer to host's private data
    anRestart - Set to 1 to restart auto-negotiation after the change, 
                0 will not do a restart
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    None

 Description:
    Sets master/slave setting to "auto" and selects multi-port (preference is
    to be a master).

    Restarts auto-negotiation by calling the function only if the input parameter 
    anRestart was selected. Also re-enables auto-negotiation before restarting 
    auto-negotiation.

 Side effects:
    None

 Notes/Warnings:
     auto-negotiation has to be restarted for it to take effect


******************************************************************************/
void SFPhyAutonegSetMasterPreference(CTX_PTR_TYPE contextPtr,
                                        unsigned long port,
                                     unsigned short anRestart);

/******************************************************************************
 void SFPhyAutonegSetSlavePreference(CTX_PTR_TYPE contextPtr, 
                                     unsigned long port, 
                                     unsigned short anRestart)

 Inputs:
    contextPtr - pointer to host's private data
    anRestart - Set to 1 to restart auto-negotiation after the change, 
                0 will not do a restart
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    None

 Description:
    Sets master/slave setting to "auto" and selects single-port (preference is
    to be a slave).

    Restarts auto-negotiation by calling the function only if the input 
    parameter anRestart was selected. Also re-enables auto-negotiation before 
    restarting auto-negotiation.

 Side effects:
    None

 Notes/Warnings:
    auto-negotiation has to be restarted for it to take effect

******************************************************************************/
void SFPhyAutonegSetSlavePreference(CTX_PTR_TYPE contextPtr, 
                                     unsigned long port, 
                                     unsigned short anRestart);

/******************************************************************************
 void SFPhyGetANCompleteAndLASIAlarmStatus(CTX_PTR_TYPE contextPtr,
                                            unsigned long port,
                                           unsigned short *latchedValue, 
                                           unsigned short *currentValue) 
                                           

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    latchedValue - the value of the first read of the LASI status register
    currentValue - the value of the second read of the LASI status register

 Returns:
    None.

 Description:
    Reads the LASI status register (1.36869) twice and returns the results of 
    both reads. The value of the first read is returned in latchedValue and 
    the value of the second read is returned in currentValue. These values may 
    then be used with other functions to test for changes in status.

 Side effects:
    This register has LATCHED BITS and has to be used carefully. These bits 
    clear-on-read if the condition has cleared.

 Notes/Warnings:
    None

******************************************************************************/
void SFPhyGetANCompleteAndLASIAlarmStatus(CTX_PTR_TYPE contextPtr,
                                            unsigned long port,
                                           unsigned short *latchedValue, 
                                           unsigned short *currentValue);

/******************************************************************************
 unsigned short SFPhyIsAutonegComplete(CTX_PTR_TYPE contextPtr, 
                                       unsigned short latchedValue)

 Inputs:
    contextPtr - pointer to host's private data
    latchedValue - the result of reading the LASI status register once

 Outputs:
    None

 Returns:
    returns a '1' if an auto-negotiation complete event was flagged in the 
        LASI status
    returns a '0' if no auto-negotiation complete event has occurred

 Description:
    Tests the auto-negotiation complete bit in the LASI status register to see 
    if an auto-negotiation complete event has occurred. Use 
    SFPhyGetANCompleteAndLASIAlarmStatus() to get the latched value to be 
    passed in to this function to test for the event.

    The latched value may then be used with this function and others to test
    for events which have occurred.

 Side effects:
    None

 Notes/Warnings:

******************************************************************************/
unsigned short SFPhyIsAutonegComplete(CTX_PTR_TYPE contextPtr, 
                                      unsigned short latchedValue);

/******************************************************************************
 unsigned short SFPhyGetMasterSlaveConfigFault(CTX_PTR_TYPE contextPtr, 
                                               unsigned long port)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    returns 1 - if master/slave config fault was detected, 0 otherwise

 Description:
    Reads the master/slave config fault bit in the auto-negotiation register 
    7.33 and returns true (1) if a config fault has occurred since the
    last time this register was read.

 Side effects:
    This bit is a latched high bit and will get cleared when read.

 Notes/Warnings:
    This function is called in functions that find out if auto-negotiation has 
    resolved master/slave (unsigned short 
    SFPhyAutonegIsResolutionMasterOrSlave(void)) to see if a mis-configuration 
    of auto-negotiation has occurred.

    THE CONDITIONS FOR FAULT ARE :-
    both sides set to manual master
    both sides set to manual slave
    8 consecutive random seed attempts tied

******************************************************************************/
unsigned short SFPhyGetMasterSlaveConfigFault(CTX_PTR_TYPE contextPtr, 
                                              unsigned long port);

/******************************************************************************
 unsigned short SFPhyReadMasterOrSlaveBit(CTX_PTR_TYPE contextPtr, 
                                          unsigned long port)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    Returns '1' if local PHY is a Master
    Returns '0' if local PHY is a Slave

 Description:
    Reads the master/slave configuration resolution bit in 7.33 and returns
    1 if the phy is a master or 0 if the phy is a slave.

 Side effects:
    This function reads 7.33 and will clear the latched status of the
    master/slave configuration fault bit. Check for that before calling
    this function if that bit is of interest.

 Notes/Warnings:
    This function does not check if auto-negotiation is completed. The caller 
    should check if auto-negotiation has completed prior to calling this 
    function. There are several ways to check for this: check for an 
    auto-negotiation complete status in the LASI status register, check for 
    link up, or check the auto-negotiation complete bit in register 7.1.

******************************************************************************/
unsigned short SFPhyReadMasterOrSlaveBit(CTX_PTR_TYPE contextPtr, 
                                         unsigned long port);

/******************************************************************************
 unsigned short SFPhyAutonegIsResolutionMasterOrSlave(CTX_PTR_TYPE contextPtr, 
                                                      unsigned long port)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    Returns '1' if the PHY is a MASTER and autoneg was resolved
    Returns '0' if the PHY is a SLAVE and autoneg was resolved
    Returns '0xFF' if it is not resolved yet.

 Description:
    The function checks if auto-negotiation speed is resolved by calling the
    SFPhyAutonegIsSpeedResolutionDone(). If auto-negotiation is not done 
    (no master/slave has been selected yet), then 0xFF is returned. If 
    master/slave has been determined then SFPhyReadMasterOrSlaveBit() is 
    called and the master/slave resolution is returned.


 Side effects:
    Calling this function may alter the auto-negotiation config fault bit in 
    register 7.33 since it is a latched bit. Check for auto-negotiation config 
    fault before calling this function if that bit is of interest.

 Notes/Warnings:
    None

******************************************************************************/
unsigned short SFPhyAutonegIsResolutionMasterOrSlave(CTX_PTR_TYPE contextPtr, 
                                                     unsigned long port);

/******************************************************************************
 unsigned short SFPhyAutonegIsSpeedResolutionDone(CTX_PTR_TYPE contextPtr, 
                                                 unsigned long port)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    Returns '1' if speed is resolved
    Returns '0' if speed is not resolved

 Description:
    The bits 1.49192.7:4 gives the speed resolution. If these bits are 
    '0x0000' the function returns a '0' indicating auto-negotiation is in 
    progress (or possibly disabled). If the bits are non-zero (meaning a speed 
    has been selected by auto-negotiation) the function
    returns a '1'.

 Side effects:
    None

 Notes/Warnings:
    None

******************************************************************************/
unsigned short SFPhyAutonegIsSpeedResolutionDone(CTX_PTR_TYPE contextPtr, 
                                                 unsigned long port);

#define AN_IN_PROGRESS    0x0000 /*  Speed not resolved yet, auto-negotiation in 
                                     progress */
/*  #define SPEED_10BT     0x0001   Not supported */
#define SPEED_RES_100M_FD 0x0002 /*  100M Full Duplex */
#define SPEED_RES_1GIG_FD 0x0003 /*  1Gig Full Duplex */
#define SPEED_RES_10GIG   0x0004 /*  10Gig Full Duplex */
#define SPEED_RES_100M_HD 0x0005 /*  100M Half Duplex */
/* #define SPEED_RES_1GIG_HD 0x0006  Not supported for SGMII */

/******************************************************************************
unsigned short SFPhyGetAutonegSpeedResolution(CTX_PTR_TYPE contextPtr, 
                                              unsigned long port)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    None

  Returns: One of the following,
    AN_IN_PROGRESS
    SPEED_10BT
    SPEED_RES_100M_FD
    SPEED_RES_1GIG_FD
    SPEED_RES_10GIG
    SPEED_RES_100M_HD
    SPEED_RES_1GIG_HD


 Description:
    The bits 1.49192.7:4 gives the speed resolution as determined by 
    auto-negotiation. The function reads the bits and returns the 4 bit value. 
    A non-zero value indicates the speed has been resolved and the phy is 
    training for that speed or the link is up at that speed.

 Side effects:
    None

 Notes/Warnings:
    None

******************************************************************************/
unsigned short SFPhyGetAutonegSpeedResolution(CTX_PTR_TYPE contextPtr, 
                                              unsigned long port);

#define CLEAR_PAUSE     0 /*  clears both pause bits */
#define SYM_PAUSE       1 /*  for symmetric pause only */
#define ASYM_PAUSE      2 /*  for asymmetric pause only */
#define SYM_ASYM_PAUSE  3 /*  for both */
/******************************************************************************
void SFPhySetPauseAdvertisement(CTX_PTR_TYPE contextPtr, unsigned long port,
                                unsigned short anRestart,
                                unsigned short pauseType);
                               

 Inputs:
    contextPtr - pointer to host's private data
    anRestart - this takes the value of '1' or '0' and indicates if 
                auto-negotiation should be restarted following the speed enable 
                change
    pauseType - one of the following: SYM_PAUSE, ASYM_PAUSE, SYM_ASYM_PAUSE or 
                CLEAR_PAUSE.
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    None

 Description:
    This function sets the asymmetric and symmetric pause bits in the technology 
    ability field in the AN Advertisement register and optionally restarts 
    auto-negotiation to use the new values. This selects what type of pause 
    is to be advertised to the far end MAC during auto-negotiation. If 
    auto-negotiation is restarted, it is enabled first.

    Sets entire 2-bit field to the value passed in pauseType.

    To clear both bits, pass in CLEAR_PAUSE.

 Side effects:
    None

 Notes/Warnings:
    This function will not take effect unless the auto-negotiation is restarted.

******************************************************************************/
void SFPhySetPauseAdvertisement(CTX_PTR_TYPE contextPtr, unsigned long port,
                                unsigned short anRestart,
                                unsigned short pauseType);

/******************************************************************************
unsigned short SFPhyGetLPAdvertisedPause(CTX_PTR_TYPE contextPtr, 
                                         unsigned long port);

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    The function returns the pause setting that the link partner advertised
    during auto-negotiation. One of the following: SYM_PAUSE, ASYM_PAUSE, 
    SYM_ASYM_PAUSE

 Description:
    This function reads 7.19 (LP Base page ability) and returns the advertised
    pause setting that was received from the link partner.

 Side effects:
    None

 Notes/Warnings:
    The user must make sure auto-negotiation has completed prior to calling 
    this function to insure the information is current.

******************************************************************************/
unsigned short SFPhyGetLPAdvertisedPause(CTX_PTR_TYPE contextPtr, 
                                         unsigned long port);

/*******************************************************************
   Link Alarm Status Indicator (LASI)
  
   These functions are all that are needed to monitor the overall status
   of the data link. You may also connect the LASIn output (level
   generated) to an interrupt handler, but you need to unmask
   the LS_ALARM control to enable it.
  
   Keep in mind the training times of the various interfaces:
   10GBASE-T 2 seconds (from down back to up, down is immediate)
   1000BASE-T 350-750ms (from down back to up, down is immediate)
   100BASE-T down to up is almost immediate, down is immediate
   other interfaces up time is almost immediate, on the order
   of at most a couple hundred ms.
  
   Use SFPhyGetAutonegSpeedResolution() to know which speed's
   status to check.
  
   For 100BASE-T and 1000BASE-T, SGMII layer is also monitored.
  
   Alarms are indication of errors in different directions but
   not a good indication of link being up or down. (Alarms will
   be true if the link went down, but a true alarm indication does
   not mean the link went down, but only that there were some errors.
   Alarms can be used for further trouble shooting if errors are
   occurring with the link staying up. No alarm support is provided
   in this library because it's not needed under normal
   conditions.)
 *******************************************************************/

/*  For mac_interface */
#define XAUI_INTERFACE 0
#define XFI_INTERFACE  1

/******************************************************************************
 unsigned short SFPhyLasiQueryLinkChangedState(CTX_PTR_TYPE contextPtr, 
                                               unsigned short latchedValue)

 Inputs:
    contextPtr - pointer to host's private data
    latchedValue - The latched value read from the LASI status register.

 Outputs:
    None

 Returns:
    Returns '1' if there was a change in overall link status
    Returns '0' if no change in status (link remained up or remained down 
    since last call).

 Description:
    Checks the LS_ALARM bit in the LASI status register that indicates
    a change in link status (either the MAC interface or copper interface
    went up or down).

 Side effects:
    None

 Notes/Warnings:
    The user should call the function "SFPhyGetANCompleteAndLASIAlarmStatus"
    before calling this function to get the latched value of the LS_ALARM bit 
    in the LASI status register and then use the latched value to check the 
    individual bits using the functions which check those bits. Note that the 
    first call to this function will clear all latched bits if their inputs 
    have changed, so SFPhyGetANCompleteAndLASIAlarmStatus() should be called 
    once and then tested for each status bit of interest.

******************************************************************************/
unsigned short SFPhyLasiQueryLinkChangedState(CTX_PTR_TYPE contextPtr, 
                                              unsigned short latchedValue);

#define SPEED_MISMATCH     0xF0
#define SPEED_NOT_RESOLVED 0xFE
#define MISC_ERROR         0xFF
/******************************************************************************
 unsigned short SFPhyLasiIsLinkUp(CTX_PTR_TYPE contextPtr,
                                    unsigned long port,
                                  unsigned short mac_interface, 
                                  unsigned short speed) 
                                  

 Inputs:
    contextPtr - pointer to host's private data
    mac_interface - which interface is active, one of XAUI_INTERFACE, or 
                    XFI_INTERFACE
    speed - which speed was resolved or will be resolved by auto-negotiation, 
            one of following:
             SPEED_RES_100M_FD
             SPEED_RES_1GIG_FD
             SPEED_RES_10GIG
             SPEED_RES_100M_HD
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    Link up is '1' (both mac and copper side are up at the speed indicated)
    Link not up is '0' (auto-negotiation has resolved speed to requested speed, 
        but mac or copper side is not up)
    SPEED_NOT_RESOLVED, auto-negotiation is in progress,  (link is down)
    SPEED_MISMATCH, incorrect speed,  (i.e. resolution speed is different
        than requested speed to check)
    MISC_ERROR, invalid input, is any other error

 Description:
    Checks if auto-negotiation is resolved, and if auto-negotiation is in 
    progress returns SPEED_NOT_RESOLVED. Then if speed is resolved, makes 
    sure resolution speed matches requested speed passed in, if not, returns 
    SPEED_MISMATCH. Finally checks if the MAC and BASE-T are both up at the 
    speed requested and returns 1 if both are up at the correct speed, or
    0 if the either side is down.

 Side effects:
    None.

 Notes/Warnings:
    Gives real-time status of the link when the function is called. If several
    speeds are enabled, check for auto-negotiation complete first with 
    SFPhyGetAutonegSpeedResolution() and use resolved speed to pass into 
    this function. Or, alternatively, poll using this function going through 
    all speeds enabled until you see a link
    up or link down indication to indicate a change in the overall link status.

    This function calls SFPhyLasiIsMacUp() and SFPhyLasiIsBaseTUp().

******************************************************************************/
unsigned short SFPhyLasiIsLinkUp(CTX_PTR_TYPE contextPtr,
                                    unsigned long port,
                                  unsigned short mac_interface, 
                                  unsigned short speed);

/******************************************************************************
unsigned short SFPhyLasiIsMacUp(CTX_PTR_TYPE contextPtr,
                                unsigned long port, 
                                unsigned short mac_interface, 
                                unsigned short speed) 
                                

 Inputs:
    contextPtr - pointer to host's private data
    mac_interface - which interface is active, one of XAUI_INTERFACE, or 
                    XFI_INTERFACE
    speed - which speed was resolved or will be resolved by auto-negotiation, 
            one of following
             SPEED_RES_100M_FD
             SPEED_RES_1GIG_FD
             SPEED_RES_10GIG
             SPEED_RES_100M_HD
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    Link up is '1' (XAUI lanes are aligned for 10G, or SGMII is up for 100M/1Gig)
    Link not up is '0' (XAUI lanes are not aligned/XFI is locked for 10G,
        or SGMII is down for 100M/1Gig)
    SPEED_NOT_RESOLVED, auto-negotiation is in progress (auto-negotiation hasn't 
        completed, speed is unknown)
    SPEED_MISMATCH, incorrect speed is (i.e. resolution speed is different
        than requested speed to check)
    MISC_ERROR, is invalid input, is any other error

 Description:
    Checks if auto-negotiation is resolved. If auto-negotiation is in progress,
    it returns SPEED_NOT_RESOLVED. Then it checks if resolved speed equals speed
    passed in. The the speeds don't match, it returns SPEED_MISMATCH.

    If they match and the XAUI is chosen, it checks XAUI lane alignment if the 
    resolved speed is 10G and returns 1 if the XAUI lanes are aligned or 0 if 
    not. For 100M half and full duplex and 1Gig, it checks if the SGMII is up 
    and returns 1 if the SGMII is up, or 0 if the SGMII is down.

    If they match and XFI is chosen, it checks XFI block lock for 10Gig if the 
    resolved speed is 10G and returns 1 if XFI block lock is true, 0 otherwise. 
    For 100M half and full duplex and 1Gig, it checks if the SGMII is up and 
    returns 1 if the SGMII is up or 0 if the SGMII is down.
    
 Side effects:
    None

 Notes/Warnings:
    Gives real-time status of the mac/phy interface when the function is called. 
    If several speeds are enabled, check for auto-negotiation complete first 
    with SFPhyGetAutonegSpeedResolution() and use resolved speed to pass 
    into this function. Or, alternatively, poll using this function going 
    through all speeds enabled until you see a link up or link down indication 
    to indicate a change in the overall link status.

    This function is only needed if more information about why overall
    link status is not up (which intermediate interface is down). It checks
    only the interface between the mac and the phy.

******************************************************************************/
unsigned short SFPhyLasiIsMacUp(CTX_PTR_TYPE contextPtr,
                                unsigned long port, 
                                unsigned short mac_interface, 
                                unsigned short speed);

/******************************************************************************
unsigned short SFPhyLasiIsBaseTUp(CTX_PTR_TYPE contextPtr,
                                    unsigned long port,
                                  unsigned short speed)
                                  

 Inputs:
    contextPtr - pointer to host's private data
    speed - which speed was resolved or will be resolved by auto-negotiation, 
            one of following
             SPEED_RES_100M_FD
             SPEED_RES_1GIG_FD
             SPEED_RES_10GIG
             SPEED_RES_100M_HD
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    1 - Link Up (checks PCS block lock for 10G, or link status is for 
    100M/1Gig)
    0 - Link is not UP. (for 10G PCS is not locked, or link is down for 
    100M/1Gig)
    SPEED_NOT_RESOLVED - auto-negotiation is in progress (auto-negotiation  
        hasn't completed, speed is unknown)
    SPEED_MISMATCH, incorrect speed is (i.e. resolution speed is different
        than requested speed to check)
    MISC_ERROR, is invalid input, is any other error

 Description:
    First checks if auto-negotiation is resolved. If auto-negotiation is in progress,
    returns SPEED_NOT_RESOLVED.
    If speed is resolved, checks if speed matches resolved speed. If it doesn't
    match returns SPEED_MISMATCH. If it does, for 10G returns the status of the
    10G PCS block lock. For 100M/1Gig returns the link status.


 Side effects:
    None

 Notes/Warnings:
    Gives real-time status of the copper interface when the function is called. 
    If several speeds are enabled, check for auto-negotiation complete first with 
    SFPhyGetAutonegSpeedResolution() and use resolved speed to pass into this 
    function. Or, alternatively, poll using this function going through all speeds 
    enabled until you see a link up or link down indication to indicate a change 
    in the overall link status.

    This function is only needed if more information about why overall
    link status is not up (which intermediate interface is down). It checks
    only the coppper side interface between the near-end phy (the MDI interface)
    and far-end phy.

******************************************************************************/
unsigned short SFPhyLasiIsBaseTUp(CTX_PTR_TYPE contextPtr,
                                    unsigned long port,
                                  unsigned short speed);

/******************************************************************************
unsigned short SFPhyLasiIsSgmiiUp(CTX_PTR_TYPE contextPtr,
                                    unsigned long port, 
                                  unsigned short mac_interface) 
                                  

 Inputs:
    contextPtr - pointer to host's private data
    mac_interface:
    #define XAUI_INTERFACE 0
    #define XFI_INTERFACE  1

    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    Returns 1 - if sgmii interface is up
    Returns 0 - if sgmii interface is down
    Returns 0xFF - if input is invalid

 Description:
    Checks if the sgmii1 or sgmii2 is up.

 Side effects:
    None

 Notes/Warnings:
    None

******************************************************************************/
unsigned short SFPhyLasiIsSgmiiUp(CTX_PTR_TYPE contextPtr,
                                    unsigned long port, 
                                  unsigned short mac_interface);

/******************************************************************************
int SFPhyIsSFT9001RevBorLater(CTX_PTR_TYPE contextPtr, unsigned long port)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    Returns 1 if the PHY is a SFT9001 Rev B or later or an SFT910X Rev AX                         
        (192 Kbytes memory size)
    Returns 0 if the PHY is a SFT9001 Rev A2 or earlier, or if it's a SFX7101 
        (160 Kbytes memory size)

 Description:
    None

 Side effects:
    None

 Notes/Warnings:
    None

******************************************************************************/
int SFPhyIsSFT9001RevBorLater(CTX_PTR_TYPE contextPtr, unsigned long port);

/* Enum for selecting default MAC interface type and speeds while configuring 
    SFT910X PHY */
typedef enum
{
    XAUI_100M_1G_10G_FULLREACH                  = 0x00,
    XAUI_10GBASET_ONLY_FULLREACH                = 0x01,
    RXAUI_100M_1G_10G_FULLREACH                 = 0x02,
    RXAUI_10GBASET_ONLY_FULLREACH               = 0x03,
    XFI_100M_1G_10G_FULLREACH                   = 0x04,
    XFI_10GBASET_ONLY_FULLREACH                 = 0x05,
    XAUI_100M_1G_10G_30_METERS                  = 0x06,
    XAUI_10GBASET_ONLY_30_METERS                = 0x07,
    RXAUI_100M_1G_10G_REACH_30_METERS           = 0x08,
    RXAUI_10GBASET_ONLY_FULLREACH_30_METERS     = 0x09,
    XFI_100M_1G_10G_FULLREACH_30_METERS         = 0x0A,
    XFI_10GBASET_ONLY_FULLREACH_30_METERS       = 0x0B,
    XAUI_100M_1G_10G_8_METERS                   = 0x0C,
    XAUI_10GBASET_ONLY_8_METERS                 = 0x0D,
    RXAUI_100M_1G_10G_REACH_8_METERS            = 0x0E,
    RXAUI_10GBASET_ONLY_FULLREACH_8_METERS      = 0x0F,
    XFI_100M_1G_10G_FULLREACH_8_METERS          = 0x10,
    XFI_10GBASET_ONLY_FULLREACH_8_METERS        = 0x11,
    PHY_DISABLED                                = 0x12,
    RESERVED1                                   = 0x13,
    RESERVED2                                   = 0x14,
    RESERVED3                                   = 0x15,
    RESERVED4                                   = 0x16,
    RESERVED5                                   = 0x17,
    RESERVED6                                   = 0x18,
    RESERVED7                                   = 0x19,
    RESERVED8                                   = 0x1A,
    RESERVED9                                   = 0x1B,
    RESERVEDA                                   = 0x1C,
    XAUI_XFI_PASS_THRU_ENABLED_1G               = 0x1D,
    XAUI_XFI_PASS_THRU_ENABLED_10G              = 0x1E,
    XAUI_100M_1G_10G_FULLREACH_COPY             = 0x1F /* Same as 0x00 option */
}SFT910X_DEVCFG_TYPE;

/*****************************************************************************
unsigned short SFPhySetDevCfgSFT910X(CTX_PTR_TYPE contextPtr, 
                                        unsigned long port, 
                                        SFT910X_DEVCFG_TYPE dev_cfg)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31
    dev_cfg - could be one of the defines for selecting MAC interface type and 
    speeds as defined above
 
 Outputs:
    None

 Returns:
    Returns 1 if the PHY was successfully configured
    Returns 0 if PHY wasn't ready for configuration or dev_cfg was an invalid 
    configuration

 Description:
    Checks if SFT910X PHY is ready to accept device configuration (1.49154.14). 
    Then configures the PHY ((1.49154.4:0) for the default MAC inferface and 
    speeds according to the dev_cfg input.

 Side effects:
    None

 Notes/Warnings:
    None

******************************************************************************/
unsigned short SFPhySetDevCfgSFT910X(CTX_PTR_TYPE contextPtr, 
                                        unsigned long port, 
                                        SFT910X_DEVCFG_TYPE dev_cfg);    

/*****************************************************************************
SFT910X_DEVCFG_TYPE SFPhyGetDevCfgSFT910X(CTX_PTR_TYPE contextPtr, 
                                            unsigned long port)
                                        
Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31
 
 Outputs:
    None

 Returns:
    Returns a device configuration - could be one of the defines for 
    selecting MAC interface type and speeds as defined above

 Description:
    Returns the value of DEV_CFG bits (1.49154.4:0) reflecting the default MAC 
    inferface and speeds.

 Side effects:
    None

 Notes/Warnings:
    None

******************************************************************************/
SFT910X_DEVCFG_TYPE SFPhyGetDevCfgSFT910X(CTX_PTR_TYPE contextPtr, 
                                            unsigned long port);    

/* Enum for selecting default MAC interface type and speeds while configuring 
    SFT9001 PHY */
typedef enum
{    
    PHY_DISABLE                 = 0x0,
    XAUI_10GBT                  = 0x1,
    XAUI_10GBT_1GBT             = 0x2,
    XAUI_10GBT_1GBT_100BTX      = 0x3,
    XAUI_XFI_PASS_THRU_ENABLED  = 0x4,
    XFI_10GBT                   = 0x5,
    XFI_10GBT_1GBT              = 0x6,
    XFI_10GBT_1GBT_100BTX       = 0x7
}SFT9001_DEVCFG_TYPE;

/*****************************************************************************
unsigned short SFPhySetDevCfgSFT9001(CTX_PTR_TYPE contextPtr, 
                                        unsigned long port, 
                                        SFT9001_DEVCFG_TYPE dev_cfg)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31
    dev_cfg - could be one of the defines for selecting MAC interface type and 
    speeds as defined above
 
 Outputs:
    None

 Returns:
    Returns 1 if the PHY was successfully configured
    Returns 0 if PHY wasn't ready for configuration or dev_cfg was an invalid 
    configuration

 Description:
    Checks if SFT9001 PHY is ready to accept device configuration (1.49154.12). 
    Then configures the PHY ((1.49154.2:0) for the default MAC inferface and 
    speeds according to the dev_cfg input.

 Side effects:
    None

 Notes/Warnings:
    None

******************************************************************************/
unsigned short SFPhySetDevCfgSFT9001(CTX_PTR_TYPE contextPtr, 
                                        unsigned long port, 
                                        SFT9001_DEVCFG_TYPE dev_cfg);    

/*****************************************************************************
SFT9001_DEVCFG_TYPE SFPhyGetDevCfgSFT9001(CTX_PTR_TYPE contextPtr, 
                                        unsigned long port)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31
 
 Outputs:
    None

 Returns:
    Returns a device configuration - could be one of the defines for selecting 
    MAC interface type and speeds as defined above

 Description:
    Returns the value of DEV_CFG bits (1.49154.2:0) reflecting the default MAC 
    inferface and speeds.

 Side effects:
    None

 Notes/Warnings:
    None

******************************************************************************/
SFT9001_DEVCFG_TYPE SFPhyGetDevCfgSFT9001(CTX_PTR_TYPE contextPtr, 
                                        unsigned long port);

#define AUTO_MDI_MDIX   0x00
#define FORCE_MDI       0x01
#define FORCE_MDIX      0x02
/*****************************************************************************
void SFPhyMDIXControl(CTX_PTR_TYPE contextPtr, unsigned long port,
         unsigned short MDIXOptions, unsigned short anRestart)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31
    MDIXOptions - 0x00 - AUTO_MDI_MDIX
                  0x01 - FORCE_MDI
                  0x02 - FORCE_MDIX
    anRestart - 1 is restart auto negotiation.
                0 is not to restart auto negotiation.
 Outputs:
    None

 Returns:
    None

 Description:
    The function forces the MDI/MDI-X or sets it to auto MDI/MDI-X based on the
    input. 

 Side effects:
    None

 Notes/Warnings:
    To enforce the MDI/MDI-X option, auto negotiation should be started again.

******************************************************************************/
void SFPhyMDIXControl(CTX_PTR_TYPE contextPtr, unsigned long port,
         unsigned short MDIXOptions, unsigned short anRestart);

#define ENABLE_FORCING  1
#define DISABLE_FORCING 0
/*****************************************************************************
void SFPhyForce100BTMode(CTX_PTR_TYPE contextPtr, unsigned long port,
                      unsigned short mode, unsigned short anRestart)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31
    anRestart - 1 is restart auto negotiation.
                0 is not to restart auto negotiation.
    mode - Enables or disables forcing 100BASE-T Full Duplex
           1 - ENABLE_FORCING, Forces 100BASE-T Full Duplex and disables Auto-Negotiation
           0 - DISABLE_FORCING, Auto-Negotiation determines link speed
 
 Outputs:
    None

 Returns:
    
 Description:
    Enables/Disables forcing the PHY to train in 100BASE-T Full Duplex mode  

 Notes/Warnings:
    None.

******************************************************************************/
void SFPhyForce100BTMode(CTX_PTR_TYPE contextPtr, unsigned long port,
                      unsigned short mode, unsigned short anRestart);

/********************************************************************
Copyright (c) 2010, Solarflare Communications

The information is proprietary and confidential to Solarflare Communications, Inc.,
and for its customers’ internal use.

The terms of use for this software are as set forth in the Solarflare API 
Software License Agreement.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR OR HIS EMPLOYER BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/
#if SF_EVK_TEST_ENVIRONMENT == 0
#if defined __cplusplus 
}
#endif 
#endif

#endif /* defined SFPHY_H */

