#include "mtdCopyright.h" 
/* Copyright 2014 Marvell International Ltd, See included file for licensing information */

/********************************************************************
This file contains functions and global data for
higher-level functions using MDIO access to enable test modes,
loopbacks, and other diagnostic functions of the Marvell 88X3240/X3220 
ethernet PHY.
********************************************************************/
#ifndef MTDDIAG_H
#define MTDDIAG_H

#if C_LINKAGE
#if defined __cplusplus
    extern "C" {
#endif
#endif

#define MTD_REV_UNK (0)    /* unknown phy type */
#define MTD_REV_QZ2 (1)    /* quad-port chip Z2 */
#define MTD_REV_QA0 (2)    /* quad-port A0 */
#define MTD_REV_QA1 (3)    /* unassigned */
#define MTD_REV_DZ2 (4)    /* dual-port Z2 */
#define MTD_REV_DA0 (5)    /* dual-port A0 */
                           /* NOTE: releases prior to 0.1.3.0 firmware */
                           /* use following definitions: */
                           /* MTD_REV_DZ1 (4) */
                           /* MTD_REV_DZ2 (5) */
/******************************************************************************
MTD_STATUS mtdGetPhyRevision
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_U8 *phyRev,
    OUT MTD_U8 *numPorts,
    OUT MTD_U8 *thisPort
);

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    phyRev - revision of this chip
    numPorts - number of ports on this chip (see note below)
    thisPort - this port number 0-1, or 0-4

 Returns:
    MTD_OK if query was successful, MTD_FAIL if not

 Description:
    Determines the PHY revision and returns the value in phyRev.
    See list above for valid values.

 Side effects:
    None.

 Notes/Warnings:
    The phyRev can be used to determine number PHY revision,
    number of ports, which port this is from PHY perspective
    (0-based indexing 0...3 or 0..2).

    If phyRev is MTD_REV_UNK, numPorts and thisPort will be returned
    as 0 and the function will return MTD_FAIL. Early versions
    of firmware (pre-A0 support) will return MTD_REV_UNK if
    PHY is strapped to fiber-only mode.

    If T-unit is in download mode, thisPort will be returned as 0.

    If phyRev is MTD_REV_QZ2, on early pre-A0 supported firmware,
    thisPort will be returned as 0 (not valid).

******************************************************************************/
MTD_STATUS mtdGetPhyRevision
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_U8 *phyRev,
    OUT MTD_U8 *numPorts,
    OUT MTD_U8 *thisPort
);

#define MTD_BIST_ABNORMAL_RESTART  ((MTD_U16)(1 << 0)) /* Non-fatal BIST status, last reset was caused by a processor error */
#define MTD_BIST_CKSUMS_EXCEEDED   ((MTD_U16)(1 << 1)) /* Non-fatal BIST status, indicates 2 or more tries to load from SPI flash was needed */
#define MTD_BIST_PMA_FAIL          ((MTD_U16)(1 << 2)) /* Fatal BIST status, XG PMA test failed */
#define MTD_BIST_ENX_FAIL          ((MTD_U16)(1 << 3)) /* Fatal BIST status, XG ENX test failed */
#define MTD_BIST_RTOS_FAIL         ((MTD_U16)(1 << 4)) /* Fatal BIST Couldn't allocate OS resources */
#define MTD_BIST_SW_FAIL           ((MTD_U16)(1 << 5)) /* Fatal BIST Software Error, only valid on engineering only builds */
#define MTD_BIST_ECC_CORRECT       ((MTD_U16)(1 << 6)) /* Non-fatal BIST status, sw corrected an ECC RAM error */
#define MTD_BIST_WAIT_POWERDOWN    ((MTD_U16)(1 << 7)) /* Non-fata BIST status, BIST is waiting for low power condition to clear before running */
#define MTD_BIST_ECHO_TEST_DONE    ((MTD_U16)(1 << 14))/* Non-fatal BIST status, echo test is done */
#define MTD_BIST_IN_PROCESS        ((MTD_U16)(1 << 15))/* BIST is in process, should be true only during reset time or echo test time */
/******************************************************************************
 MTD_STATUS mtdGetTunitBISTStatus
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     OUT MTD_U16 *bistStatus
 );
 
 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    bistStatus - value of T-unit's current BIST status, read bit
                 definitions above

            
 Returns:
    MTD_OK if query was successful, MTD_FAIL if not

 Description:
    This function returns the T-unit's Built-In Self Test status from 1.C00C.

    The BIST is run following a T-unit hard reset or software reset before the
    reset completes.

    See defines above for checking for fatal errors or other status.

 Side effects:
    None.

 Notes/Warnings:
    BIST status bits are divided into fatal, non-fatal, and info-only
    bits.
 
    Fatal BIST errors will cause the T-unit to hang in reset until it passes.
    Check this register if the T-unit does not exit reset for the reason.

    On engineering-only builds (non-released firmware), non-fatal BIST errors
    will also cause the T-unit firmware to hang.

******************************************************************************/
MTD_STATUS mtdGetTunitBISTStatus
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_U16 *bistStatus
);


#define MTD_POWERON_INIT_AN (0) /* during reset/power-on init, state is 0 for
                                   power on, non-zero for AN */
#define MTD_XG_SPEED_LINK   (1) /* during 10GBASE-T training/data */
#define MTD_LOW_POWER_MODE  (2) /* T-unit powered-down/low power mode */
#define MTD_TEST_MODE       (3) /* during a test mode */
#define MTD_IG_SPEED_LINK   (4) /* during 1000BASE-T training/data */
#define MTD_CM_SPEED_LINK   (5) /* during 100BASE-T training/data */
#define MTD_XM_SPEED_LINK   (6) /* during 10BASE-T training/data */
/******************************************************************************
 MTD_STATUS mtdGetTunitFirmwareMode
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     OUT MTD_U16 *tunitMode,
     OUT MTD_U16 *tunitState
 );

 
 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    tunitMode - current mode of the T-unit firmware state machine from
                1.C001.11:8, one of the following    
        MTD_POWERON_INIT
        MTD_AN          
        MTD_XM_SPEED_LINK 
        MTD_CM_SPEED_LINK 
        MTD_IG_SPEED_LINK 
        MTD_XG_SPEED_LINK 
        MTD_LOW_POWER_MODE
        MTD_TEST_MODE     
    
    tunitState - current substate of the current mode of the T-unit firmware
                 state machine from 1.C001.7:0                 
 Returns:
    MTD_OK if query was successful, MTD_FAIL if not

 Description:
    This reads 1.C001 and returns the T-unit mode and state. This 
    information describes the current state of the firmware state machine
    at the time it was read. This is a real-time operating mode/
    state of the firmware state machine.

 Side effects:
    None.

 Notes/Warnings:
    Calling this function will result in a read of register 1.C001 which
    contains the short reach fault bit. Reading this register will clear
    that bit. Check the short reach fault bit before calling this function 
    if that bit value is of interest.

******************************************************************************/
MTD_STATUS mtdGetTunitFirmwareMode
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_U16 *tunitMode,
    OUT MTD_U16 *tunitState
);



/*********************PMA TEST MODES********************************/
#define MTD_PMA_TESTMODE_OFF 0x0000  /* return to normal operation */
#define MTD_PMA_TESTMODE_1  0x2000  /* 0010 0000 0000 0000*/
#define MTD_PMA_TESTMODE_2  0x4000  /* 0100 0000 0000 0000*/
#define MTD_PMA_TESTMODE_3  0x6000  /* 0110 0000 0000 0000*/
#define MTD_PMA_TESTMODE_4  0x8000  /* 1000 0000 0000 0000*/
#define MTD_PMA_TESTMODE_5  0xA000  /* 1010 0000 0000 0000*/
#define MTD_PMA_TESTMODE_6  0xC000  /* 1100 0000 0000 0000*/
#define MTD_PMA_TESTMODE_7  0xE000  /* 1110 0000 0000 0000*/

/*********************PMA TEST MODES - Frequencies*******************/
#define MTD_DUAL_TONE_1     0x0400  /* 0000 0100 0000 0000*/
#define MTD_DUAL_TONE_2     0x0800  /* 0000 1000 0000 0000*/
#define MTD_DUAL_TONE_3     0x1000  /* 0001 0000 0000 0000*/
#define MTD_DUAL_TONE_4     0x1400  /* 0001 0100 0000 0000*/
#define MTD_DUAL_TONE_5     0x1800  /* 0001 1000 0000 0000*/
#define MTD_NO_FREQ         0x0000  /* 0000 0000 0000 0000*/

/******************************************************************************
 MTD_STATUS mtdSetPMATestModes
 (
     IN CTX_PTR_TYPE contextPtr, 
     IN MTD_U16 port,
     IN MTD_U16 testMode, 
     IN MTD_U16 testFrequency
 );
 
 Inputs:
    contextPtr - pointer to host's private data.

    port - MDIO port address, 0-31

    testMode - The test mode needed for testing. One of the following...
        MTD_PMA_TESTMODE_OFF
        MTD_PMA_TESTMODE_1
        MTD_PMA_TESTMODE_2
        MTD_PMA_TESTMODE_3
        MTD_PMA_TESTMODE_4
        MTD_PMA_TESTMODE_5
        MTD_PMA_TESTMODE_6
        MTD_PMA_TESTMODE_7

    testFrequency - These are the test frequency options for test mode 4
        MTD_DUAL_TONE_1
        MTD_DUAL_TONE_2
        MTD_DUAL_TONE_3
        MTD_DUAL_TONE_4
        MTD_DUAL_TONE_5
        MTD_NO_FREQ (for other test modes besides test mode 4, pass this value)

 Outputs:
    None

 Returns:
    MTD_OK if successful, or MTD_FAIL if not

 Description:
    Either puts the PHY into the 10GBASE-T PMA Test mode or removes the
    PHY from the PMA test mode based on testMode and testFrequency.
    
    The bits 1.132.(15:13) are the PMA test mode bits. The function reads
    the register 132 and clears the 3MSB's and writes the (testMode),
    input parameter to the apropriate bits.

    The bits 1.132.(12:10) are the transmitter test frequencies used
    for test mode 4. If test mode 4 is the input option, then the code
    reads register 132 and clears bits 12, 11 & 10. Then it writes the
    input frequency to the apropriate bits.

    To move from one test mode to another, the PMA test mode must first be
    turned off to select a new PMA test mode.

 Side effects:
    None.

 Notes/Warnings:
    1. For all test modes except test mode 4, the input parameter
    testFrequency should be MTD_NO_FREQ.
    2. For all test modes except test mode 4, the transmitter frequency bits are
    not written.
    3. The user can enter any test mode at anytime during training but the way
    to exit from any of the test modes is to do a reset or call this function
    with testMode set to MTD_PMA_TESTMODE_OFF
    4. Test mode must be turned off (pass MTD_PMA_TESTMODE_OFF) in prior to
    switching to a new PMA test mode.
    5. The tranceiver should be terminated into a 100 Ohm load or unpowered port
    during these tests and prior to starting a test.

******************************************************************************/
MTD_STATUS mtdSetPMATestModes
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    IN MTD_U16 testMode, 
    IN MTD_U16 testFrequency
);

/******************************************************************************
 MTD_STATUS mtdGetPMATestMode
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     OUT MTD_U16 *testMode,
     OUT MTD_U16 *testFrequency
 );
 
 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    testMode - The value of 1.132 bits 15:13 with bits 12:0 masked off. 
        One of the following
         MTD_PMA_TESTMODE_OFF
         MTD_PMA_TESTMODE_1
         MTD_PMA_TESTMODE_2
         MTD_PMA_TESTMODE_3
         MTD_PMA_TESTMODE_4
         MTD_PMA_TESTMODE_5
         MTD_PMA_TESTMODE_6
         MTD_PMA_TESTMODE_7
    testFrequency - The value of 1.132 bits 12:10 with other bits masked off.
        One of the following
         MTD_DUAL_TONE_1
         MTD_DUAL_TONE_2
         MTD_DUAL_TONE_3
         MTD_DUAL_TONE_4
         MTD_DUAL_TONE_5
         MTD_NO_FREQ (for other test modes besides test mode 4, pass this value)
            
 Returns:
    MTD_OK if query was successful, MTD_FAIL if not

 Description:
    The bits 1.132.(15:13) are the PMA test mode bits. The function reads
    the register 132 and returns the 3MSB's in testMode and bits 12:10 in
    testFrequency with other bits masked off.

 Side effects:
    None.

 Notes/Warnings:
    None

******************************************************************************/
MTD_STATUS mtdGetPMATestMode
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_U16 *testMode,
    OUT MTD_U16 *testFrequency
);

/* for loopbacks see the H-unit API */

/******************************************************************************
 MTD_STATUS mtdGet10GBTSNROperatingMargin
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     OUT MTD_S16 snr_4chan[]
 )

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    snr_4chan[] - an array to fetch the SNR operating margin. SNR's are 16 bit
                    signed values in unit of 0.1dB

 Returns:
    MTD_OK if the query was successful, MTD_FAIL if not

 Description:
    Reads the 10GBASE-T SNR operating margin from 1.133-136 (channels A-D)
    and returns it in snr_4chan[] converted from an offset binary number
    to a two's complement number in snr_4chan. The units in
    snr_4chan is 1-bit equals 0.1 dB of margin.

 Side effects:
    None

 Notes/Warnings:
    The 10GBASE-T copper link should be up when calling this function or
    results will not be valid.

******************************************************************************/
MTD_STATUS mtdGet10GBTSNROperatingMargin
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_S16 snr_4chan[]
);

/******************************************************************************
 MTD_STATUS mtdGet10GBTSNRMinimumMargin
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     OUT MTD_S16 snr_4chan[]
 )

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    snr_4chan[] - an array to fetch the SNR minimum margin. SNR's are 16 bit
                    signed values in unit of 0.1dB

 Returns:
    MTD_OK if the query was successful, MTD_FAIL if not

 Description:
    Reads the 10GBASE-T SNR minimum margin from 1.137-140 (channels A-D)
    and returns it in snr_4chan[] converted from an offset binary number
    to a two's complement number in snr_4chan. The units in
    snr_4chan is 1-bit equals 0.1 dB of margin.

 Side effects:
    Sets the SNR minimum margin to the maximum value when this register is
    read. The read returns the minimum seen since the last read or since
    the link came up.

 Notes/Warnings:
    The 10GBASE-T copper link should be up when calling this function or
    results will not be valid.

******************************************************************************/
MTD_STATUS mtdGet10GBTSNRMinimumMargin
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_S16 snr_4chan[]
);

/********** Cable Diagnostics *********/

/******************************************************************************
 MTD_STATUS mtdRun_CableDiagnostics
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port, 
     IN MTD_BOOL breakLink,
     IN MTD_BOOL disableInterPairShort
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31
    
    breakLink - MTD_TRUE will break the link in data mode and run the test
                    (data will be interrupted if this option is chosen).
                MTD_FALSE will not run the test, but instead will return
                    results of the link training information available
                    leaving the link up if it's up. 

    disableInterPairShort -  MTD_TRUE disables checking for inter pair shorts
                             MTD_FALSE enables checking for inter pair shorts
                             (MTD_FALSE is only available if breakLink is MTD_TRUE)

 Outputs:
    None.

 Returns:
    MTD_OK if test was started successfully, MTD_FAIL if any error occurred

 Description:
    If breakLink is MTD_TRUE, the link is dropped (if it was up) and the cable
    diagnostics are executed to determine length and termination. If this option
    is chosen and disableInterPairShort is MTD_FALSE, the inter-pair short
    test is also executed.

    Call mtdGet_CableDiagnostics_Status() to determine when the test has completed
    and results may be read using mtdGet_CableDiagnostics_Results(), which
    reads the results and returns the port to normal operating condition.

    If breakLink is MTD_FALSE, the link is not dropped (data will not be interrupted),
    the test is not run. If the link is up in 10GBASE-T mode, length and
    termination status as determined from the training are returned. If
    the link is up at some other speed, or the link is down, 0 is returned
    for termination and 0xFF is returned for the length.

    disableInterPairShort is ignored if breakLink is MTD_FALSE.
 

 Side effects:
     None.

 Notes/Warnings:
    The length is reported in meters only.

    If the test is currently running, this function will return MTD_FAIL and
    no registers will be written.

******************************************************************************/
MTD_STATUS mtdRun_CableDiagnostics
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port, 
    IN MTD_BOOL breakLink,
    IN MTD_BOOL disableInterPairShort
);

/******************************************************************************
 MTD_STATUS mtdGet_CableDiagnostics_Status
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     OUT MTD_BOOL *testDone
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    testDone - MTD_TRUE - test is complete, and results may be read
               MTD_FALSE - test is still running

 Returns:
    MTD_OK if query succeeded, MTD_FAIL if not (MTD_FAIL if test was not started)

 Description:
    This function gives the status of the test if the test is complete or still
    running.

 Side effects:
    None

 Notes/Warnings:
    If the function returns MTD_FAIL, testDone should be ignored (it's not valid).

******************************************************************************/
MTD_STATUS mtdGet_CableDiagnostics_Status
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_BOOL *testDone
);

/******************************************************************************
 MTD_STATUS mtdGet_CableDiagnostics_Results
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port, 
     MTD_U8 terminationCode[],
     MTD_U8 length[]
 );
 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    terminationCode[] - for all 4 channels, A-D, one of the following:
        0x0 - invalid or incomplete test
        0x1 - pair okay, no fault
        0x2 - pair open
        0x3 - intra pair short
        ox4 - inter pair short
        0x5 - 0xF - reserved

    length[] - for all 4 channels, A-D, reported in meters

 Returns:
    MTD_OK if query successful, MTD_FAIL if query is unsuccessful or test
        is still running or was not started

 Description:
    This function checks if the test was started and is complete, and if it was
    not started or is not complete, it returns MTD_FAIL.

    If test is complete, it writes the results to terminationCode[] and length[]
    and clears the request to return the PHY to normal operating mode and returns 
    MTD_OK.

 Side effects:
    None

 Notes/Warnings:
    None.

******************************************************************************/
MTD_STATUS mtdGet_CableDiagnostics_Results
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port, 
    MTD_U8 terminationCode[],
    MTD_U8 length[]
);
    

/******************** Echo Test **********************/
/******************************************************************************
 MTD_STATUS mtdStart10GEchoTest
 (
     IN CTX_PTR_TYPE contextPtr, 
     IN MTD_U16 port
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    None.

 Returns:
    MTD_OK if the test was requested successfully, MTD_FAIL otherwise
    MTD_FAIL if a previously requested test is still in progress.

 Description:
    Puts the PHY port into a special test mode, and performs various
    echo tests. Use mtdIs10GEchoTestDone() to find out if the test is
    finished and results available. Use mtdGet10GEchoTestResults()
    to read the results and exit the test mode and return to normal
    operation.

 Side effects:
    None

 Notes/Warnings:
    The tranceiver copper port must be terminated into a 100 Ohm load or
    unpowered port during these tests.

    These tests should take about 5 seconds.

******************************************************************************/
MTD_STATUS mtdStart10GEchoTest
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port
);

/******************************************************************************
 MTD_STATUS mtdIs10GEchoTestDone
 (
     IN CTX_PTR_TYPE contextPtr, 
     IN MTD_U16 port,
     OUT MTD_BOOL *testDone
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    testDone - MTD_TRUE if the test has completed, MTD_FALSE if the test is in
               progress.

 Returns:
    MTD_OK if the status was returned successfully, MTD_FAIL otherwise. MTD_FAIL
    if the test was not requested.

 Description:
    This function first checks if the PHY was requested to run the echo test.
    If not, it returns MTD_FAIL. If it was, it returns in testDone the result
    of the echo test status bit, 1.C00C.14.

 Side effects:
    None

 Notes/Warnings:
    The tranceiver must be terminated terminated into a 100 Ohm load or
    unpowered port during these tests.


    These tests should take about 5 seconds.

******************************************************************************/
MTD_STATUS mtdIs10GEchoTestDone
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    OUT MTD_BOOL *testDone
);

/******************************************************************************
 MTD_STATUS mtdGet10GEchoTestResults
 (
     IN CTX_PTR_TYPE contextPtr, 
     IN MTD_U16 port,
     OUT MTD_U16 results[]
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    results[] - 20 element array to store the echo/NEXT values.

 Returns:
    MTD_OK if the results were fetched ok, MTD_FAIL if not. MTD_FAIL if
    the test was not requested to be run or isn't done.

 Description:
    This function first makes sure the test was requested to be run and the
    results are ready. If either isn't true, it returns MTD_FAIL.
 
    Then the function fetches the ECHO/NEXT values for the 10G echo test then
    puts the PHY back into normal operating mode.

 Side effects:
    None

 Notes/Warnings:
    The tranceiver must be terminated terminated into a 100 Ohm load or
    unpowered port during these tests.


******************************************************************************/
MTD_STATUS mtdGet10GEchoTestResults
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    OUT MTD_U16 results[]
);

/******************************************************************************
 MTD_STATUS mtdGetAmbientNoise
 (
     IN CTX_PTR_TYPE contextPtr, 
     IN MTD_U16 port,
     OUT MTD_U16 noiseResults[]
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
     noiseResults[] - 4 element array to store the channel noise values for A, B, C, and D

 Returns:
    MTD_OK if the test/query was successful, MTD_FAIL if there was any error

 Description:
    The function disables transmit power and turns on the echo test for 10GBASE-T.
    Once the test is completed it fetches the test results in noiseResults[].

 Side effects:
    None

 Notes/Warnings:
    Calls the following functions.
    1. mtdStart10GEchoTest()
    2. mtdIs10GEchoTestDone()
    3. mtdGet10GEchoTestResults()

******************************************************************************/
MTD_STATUS mtdGetAmbientNoise
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    OUT MTD_U16 noiseResults[]
);

/***** BER COUNTER *****/
/******************************************************************************
 MTD_STATUS mtdReadBERCount
 (
     IN CTX_PTR_TYPE contextPtr, 
     IN MTD_U16 port,
     OUT MTD_U16 *count
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    count - BER count (max value is 0x3F)

 Returns:
    MTD_OK if query was successful, MTD_FAIL if not

 Description:
    This function reads the 10GBASE-T PCS Status 2 register bits 13:8 and
    returns the BER count.

 Side effects:
    This function clears the BER counter after reading its value.

 Notes/Warnings:
    1. Call this function only after the link is up.
    2. Call this function at the beginning of the time period for which you want
    to monitor the BER to clear the BER count. Then call this function again at
    the end of the time period to get the correct BER count.
    3. After a link drop, the BER count is most likely to be 0x3F

******************************************************************************/
MTD_STATUS mtdReadBERCount
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    OUT MTD_U16 *count
);

/******************************************************************************
 MTD_STATUS mtdIsLPInfoValid
 (
     IN CTX_PTR_TYPE contextPtr, 
     IN MTD_U16 port,
     OUT MTD_BOOL *validFlag
 );
 
 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    validFlag - MTD_TRUE if link partner info is valid and can be read, MTD_FALSE
                if not

 Returns:
    MTD_OK if query was successful, MTD_FAIL if not

 Description:
    This function reads the 10GBASE-T status register 1.129 and returns MTD_TRUE
    in validFlag if the 10GBASE-T startup protocol has been completed and the 
    10GBASE-T link partner bits set during the startup protocol are valid for the 
    link partner.

    A MTD_FALSE result indicates the bits set during the startup are invalid for the
    link partner.

 Side effects:
    None

 Notes/Warnings:
    This function should be called and checked before reading any of the link partner
    information exchanged during 10GBASE-T training.

******************************************************************************/
MTD_STATUS mtdIsLPInfoValid
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    OUT MTD_BOOL *validFlag
);

/***** PBO - NEAR END and FAR END *****/
/******************************************************************************
 MTD_STATUS mtdReadNearEndPBO
 (
     IN CTX_PTR_TYPE contextPtr, 
     IN MTD_U16 port,
     OUT *pboValue
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    pboValue -    PBO value
        0x000 - 0 dB
        0x001 - 2 dB
        0x010 - 4 dB
        0x011 - 6 dB
        0x100 - 8 dB
        0x101 - 10 dB
        0x110 - 12 dB
        0x111 - 14 dB

 Returns:
    MTD_OK if query was successful, MTD_FAIL if not

 Description:
    The function reads the register 1.131.12:10 and returns the value.

 Side effects:
    None

 Notes/Warnings:
    This function should be called only after link up. If called during the
    startup process the value is invalid.
******************************************************************************/
MTD_STATUS mtdReadNearEndPBO
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    OUT MTD_U16 *pboValue
);

/******************************************************************************
 MTD_STATUS mtdReadFarEndPBO
 (
     IN CTX_PTR_TYPE contextPtr, 
     IN MTD_U16 port,
     OUT *pboValue
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    pboValue -    PBO value
        0x000 - 0 dB
        0x001 - 2 dB
        0x010 - 4 dB
        0x011 - 6 dB
        0x100 - 8 dB
        0x101 - 10 dB
        0x110 - 12 dB
        0x111 - 14 dB

 Returns:
    MTD_OK if query was successful, MTD_FAIL if not

 Description:
    The function reads the register 1.131.15:13 and returns the value.

 Side effects:
    None

 Notes/Warnings:
    This function should be called only after link up. If called during the
    startup process, the value is invalid. Poll the function
    mtdIsLPInfoValid() until the function returns a MTD_TRUE for validFlag.

******************************************************************************/
MTD_STATUS mtdReadFarEndPBO
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    OUT MTD_U16 *pboValue
);


#define MTD_EYEDIAGRAM_NROWS 51
#define MTD_EYEDIAGRAM_NCOLS 128
#define MTD_EYEDIAGRAM_NQ (MTD_EYEDIAGRAM_NROWS*MTD_EYEDIAGRAM_NCOLS)
#define MTD_XFI_LANEOFFSET 0x200

/******************************************************************************
MTD_STATUS mtdGetSerdesEyeStatistics
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port, 
    IN MTD_U16 curSelLane,
    OUT MTD_U32 **eyeDataUp, 
    OUT MTD_U32 **eyeDataDn,  
    OUT MTD_U16 *eyeArea, 
    OUT MTD_U16 *eyeWidth, 
    OUT MTD_U16 *eyeHeight
)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31
    curSelLane - XFI lane number, 0 or 1

 Outputs:
    eyeDataUp - 26 x 128 array of 32-bit eye data (readout from register 30.8012 & 30.8013)
    eyeDataDn - 26 x 128 array of 32-bit eye data (readout from register 30.8012 & 30.8013)
    eyeArea - percentage of open eye area in the eye diagram plot
    eyeWidth - width of the eye opening (in ps)
    eyeHeight - height of the eye opening (in mV)

 Returns:
    MTD_OK if query was successful, MTD_FAIL if not

 Description:
    The function is translated from the source code of Marvell's SMI Eye Diagram GUI. 
    It reads Serdes eye diagram data and computes relevant eye statistics.
    Two arrays of size 26x128x32-bit should be allocated before calling this function and 
    the 2D pointers should be passed down for eye data collection (see example below).

 Usage example:
        :
        MTD_U32 eyeDataUp[(MTD_EYEDIAGRAM_NROWS + 1) / 2][MTD_EYEDIAGRAM_NCOLS] = {0};
        MTD_U32 eyeDataDn[(MTD_EYEDIAGRAM_NROWS + 1) / 2][MTD_EYEDIAGRAM_NCOLS] = {0};

        ATTEMPT(mtdGetSerdesEyeStatistics(contextPtr,port,curSelLane,eyeDataUp,eyeDataDn,&eyeArea,&eyeWidth,&eyeHeight));
        :

 Side effects:
    None

 Notes/Warnings:
    1. Eye data can only be collected from active Serdes lanes. 
    2. Eye data collection takes at least 20 seconds.
******************************************************************************/
MTD_STATUS mtdGetSerdesEyeStatistics
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port, 
    IN MTD_U16 curSelLane,
    OUT MTD_U32 eyeDataUp[][MTD_EYEDIAGRAM_NCOLS],
    OUT MTD_U32 eyeDataDn[][MTD_EYEDIAGRAM_NCOLS],
    OUT MTD_U16 *eyeArea,
    OUT MTD_U16 *eyeWidth, 
    OUT MTD_U16 *eyeHeight
);

#if C_LINKAGE
#if defined __cplusplus
}
#endif
#endif

#endif /* defined MTDDIAG_H */
