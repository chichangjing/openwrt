#include "mtdCopyright.h" 
/* Copyright 2013 Marvell International Ltd, See included file for licensing information */

/********************************************************************
This file contains functions prototypes and global defines/data for
higher-level functions that are shared by the H Unit (host/interface
to the MAC) and the X Unit (media/fiber interface).
********************************************************************/
#ifndef MTDHXUNIT_H
#define MTDHXUNIT_H
#if C_LINKAGE
#if defined __cplusplus 
    extern "C" { 
#endif 
#endif

/******************************************************************************
 SERDES control (common)
******************************************************************************/

/******************************************************************************
 MTD_STATUS mtdSetSerdesControl1
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     IN MTD_U16 HorXunit,
     IN MTD_BOOL loopback,
     IN MTD_BOOL rx_powerdown,
     IN MTD_BOOL block_tx_on_loopback
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31
    HorXunit - MTD_H_UNIT or MTD_X_UNIT, depending on which interface is
                being set
    loopback - MTD_TRUE to set loopback, MTD_FALSE otherwise
    tx_powerdown - MTD_TRUE to power down receiver, MTD_FALSE to power up
    block_tx_on_loopback - MTD_TRUE if when loopback is true, will not pass
                           loopbacked data through to tx path, MTD_FALSE
                           loopbacked data will be transmitted through

 Outputs:
    None

 Returns:
    MTD_OK or MTD_FAIL

 Description:
    Controls the Serdes for the H unit or X unit.

    On loopback = MTD_TRUE, on H unit does a host loopback, on the X unit does
    a link loopback.

    block_tx_on_loopback = MTD_TRUE, on H unit blocks ingress path during loopback,
    on X unit blocks egress path during loopback.

 Side effects:
    None

 Notes/Warnings:
    Use mtdGetSerdesControl1() to get current settings.

******************************************************************************/
MTD_STATUS mtdSetSerdesControl1
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    IN MTD_BOOL loopback,
    IN MTD_BOOL rx_powerdown,
    IN MTD_BOOL block_tx_on_loopback
);

/******************************************************************************
 MTD_STATUS mtdGetSerdesControl1
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     IN MTD_U16 HorXunit,
     OUT MTD_BOOL *loopback,
     OUT MTD_BOOL *rx_powerdown,
     OUT MTD_BOOL *block_tx_on_loopback
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31
    HorXunit - MTD_H_UNIT or MTD_X_UNIT, depending on which interface is
                being read

 Outputs:
    loopback - MTD_TRUE indicates loopback set, MTD_FALSE otherwise
    tx_powerdown - MTD_TRUE receiver powered down, MTD_FALSE otherwise
    block_tx_on_loopback - MTD_TRUE if when loopback is true, data blocked
                           on transmit path, MTD_FALSE
                           data not blocked on transmit path 

 Returns:
    MTD_OK or MTD_FAIL

 Description:
    Reads the the Serdes control register 1 for the H unit or X unit.

    On loopback = MTD_TRUE, on H unit indicates a host loopback, on the X unit 
    indicates a link loopback.

    block_tx_on_loopback = MTD_TRUE, on H unit indicates ingress path blocked
    during loopback, on X unit indicates egress path blocked during loopback.

 Side effects:
    None

 Notes/Warnings:
    Use mtdSetSerdesControl1() to change current settings.

******************************************************************************/
MTD_STATUS mtdGetSerdesControl1
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    OUT MTD_BOOL *loopback,
    OUT MTD_BOOL *rx_powerdown,
    OUT MTD_BOOL *block_tx_on_loopback
);


/******************************************************************************
 MTD_STATUS mtdSetSerdesLanePolarity
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     IN MTD_U16 HorXunit,
     IN MTD_BOOL invert_input_pol,
     IN MTD_BOOL invert_output_pol
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31
    HorXunit - MTD_H_UNIT or MTD_X_UNIT, depending on which lane is
                being set (see description below)
    invert_input_pol - MTD_TRUE to invert the serdes input polarity, 
                       MTD_FALSE to leave it as-is
    invert_output_pol - MTD_TRUE to invert the serdes output polarity, 
                        MTD_FALSE to leave it as-is

 Outputs:
    None

 Returns:
    MTD_OK or MTD_FAIL

 Description:
    This function can be used to invert the input or output polarity of
    the serdes pins for either the H unit (lane 0) or X unit (lane 1)
    in either 10GBASE-R mode (lane0/1), or 1000BASE-X/SGMII fiber mode.

    Passing in MTD_H_UNIT adjusts lane 0, MTD_X_UNIT adjusts lane 1,
    or in 1000BASE-X/SGMII mode MTD_H_UNIT adjusts the MAC interface
    and MTD_X_UNIT adjusts the fiber interface.

 Side effects:
    None

 Notes/Warnings:
    This is only for 10GBASE-R, 1000BASE-X, or SGMII mode. In these modes,
    lane 0 polarity is controlled by 4.F004.[14,12] (H unit), and lane 1
    polarity is controlled by 3.F004.[15,13] (X unit).

    In 10GBASE-X2 (RXAUI) mode, both lanes are controlled by 4.F004.15:12. A
    different API function call is provided for this mode of operation,
    mtdSetX2SerdesLanePolarity().   

    Call mtdGetSerdesLanePolarity() to find out the current setting.

******************************************************************************/
MTD_STATUS mtdSetSerdesLanePolarity
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    IN MTD_BOOL invert_input_pol,
    IN MTD_BOOL invert_output_pol
);


/******************************************************************************
 MTD_STATUS mtdGetSerdesLanePolarity
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     IN MTD_U16 HorXunit,
     IN MTD_BOOL *invert_input_pol,
     IN MTD_BOOL *invert_output_pol
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
    in 10GBASE-R, 1000BASE-X/SGMII mode. Pass in MTD_H_UNIT to 
    read the lane 0 setting (or MAC interface setting) and MTD_X_UNIT
    to read the lane 1 setting (or fiber interface setting).

    A different function is provided for setting/getting the polarity
    in 10GBASE-X2 (RXAUI) mode, mtdGetX2SerdesLanePolarity().

 Side effects:
    None

 Notes/Warnings:
    This is only for 10GBASE-R, 1000BASE-X, or SGMII mode. In these modes,
    lane 0 polarity is controlled by 4.F004.[14,12] (H unit), and lane 1
    polarity is controlled by 3.F004.[15,13] (X unit).

    In 10GBASE-X2 (RXAUI) mode, both lanes are controlled by 4.F004.15:12. A
    different API function call is provided for this mode of operation,
    mtdGetX2SerdesLanePolarity().   

    Call mtdSetSerdesLanePolarity() to change the current setting.

******************************************************************************/
MTD_STATUS mtdGetSerdesLanePolarity
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    IN MTD_BOOL *invert_input_pol,
    IN MTD_BOOL *invert_output_pol
);



/******************************************************************************
 MTD_STATUS mtdSetSerdesInterruptEnable
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     IN MTD_U16 HorXunit,
     IN MTD_BOOL fifo_overflow,
     IN MTD_BOOL fifo_underflow,
     IN MTD_BOOL pkt_check_crc
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31
    HorXunit - MTD_H_UNIT or MTD_X_UNIT, depending on which interface is
               being set
    fifo_overflow - MTD_TRUE to enable the fifo overflow interrupt, MTD_FALSE
                    to disable                    
    fifo_underflow - MTD_TRUE to enable the fifo underflow interrupt, MTD_FALSE
                     to disable
    pkt_check_crc - MTD_TRUE to enable the packet checker CRC interrupt, MTD_FALSE
                    to disable    

 Outputs:
    None

 Returns:
    MTD_OK or MTD_FAIL

 Description:
    This function can be used to enable one of the above interrupts on the H
    or X unit serdes interface.

    Call mtdGetSerdesInterruptEnable() to check if an interrupt is enabled.

    Call mtdGetSerdesInterruptStatus() to check the interrupt status.
    
 Side effects:
    None

 Notes/Warnings:
    None

******************************************************************************/
MTD_STATUS mtdSetSerdesInterruptEnable
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    IN MTD_BOOL fifo_overflow,
    IN MTD_BOOL fifo_underflow,
    IN MTD_BOOL pkt_check_crc
);


/******************************************************************************
 MTD_STATUS mtdGetSerdesInterruptEnable
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     IN MTD_U16 HorXunit,
     OUT MTD_BOOL *fifo_overflow,
     OUT MTD_BOOL *fifo_underflow,
     OUT MTD_BOOL *pkt_check_crc
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31
    HorXunit - MTD_H_UNIT or MTD_X_UNIT, depending on which interface is
               being set

 Outputs:
    fifo_overflow - MTD_TRUE if the fifo overflow interrupt enabled, MTD_FALSE
                    if disabled
    fifo_underflow - MTD_TRUE if the fifo underflow interrupt enabled, MTD_FALSE
                     if disabled
    pkt_check_crc - MTD_TRUE if the packet checker CRC interrupt enabled, MTD_FALSE
                    if disabled    

 Returns:
    MTD_OK or MTD_FAIL

 Description:
    This function can be called to check if an interrupt is enabled on one
    of the serdes interfaces for the H unit or X unit.

    Call mtdSetSerdesInterruptEnable() to enable/disable an interrupt.

    Call mtdGetSerdesInterruptStatus() to check the interrupt status.
    
 Side effects:
    None

 Notes/Warnings:
    None

******************************************************************************/
MTD_STATUS mtdGetSerdesInterruptEnable
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    OUT MTD_BOOL *fifo_overflow,
    OUT MTD_BOOL *fifo_underflow,
    OUT MTD_BOOL *pkt_check_crc
);


/******************************************************************************
 MTD_STATUS mtdGetSerdesInterruptStatus
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     IN MTD_U16 HorXunit,
     OUT MTD_BOOL *fifo_overflow,
     OUT MTD_BOOL *fifo_underflow,
     OUT MTD_BOOL *pkt_check_crc
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31
    HorXunit - MTD_H_UNIT or MTD_X_UNIT, depending on which interface is
               being set

 Outputs:
    fifo_overflow - MTD_TRUE if a fifo overflow was detected, MTD_FALSE
                    if not
    fifo_underflow - MTD_TRUE if a fifo underflow was detect, MTD_FALSE
                     if not
    pkt_check_crc - MTD_TRUE if the packet checker detected a CRC error
                    and it is enabled, MTD_FALSE if not  

 Returns:
    MTD_OK or MTD_FAIL

 Description:
    This function can be called to check the status of an interrupt on the
    serdes of the H unit or X unit. Calling this function clears the
    latched high bits for these bits in 3.F00B and 4.F00B.

    Call mtdSetSerdesInterruptEnable() to enable/disable an interrupt.
    
 Side effects:
    None

 Notes/Warnings:
    Clears latch high (LH) interrupt status bits in the status register
    when this function is called.

******************************************************************************/
MTD_STATUS mtdGetSerdesInterruptStatus
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    OUT MTD_BOOL *fifo_overflow,
    OUT MTD_BOOL *fifo_underflow,
    OUT MTD_BOOL *pkt_check_crc
);


#define MTD_SERDES_PPM_FIFO_2K         0 /* set fifo depth to 2K bytes */
#define MTD_SERDES_PPM_FIFO_10K        1 /* set fifo depth to 10K bytes */
#define MTD_SERDES_PPM_FIFO_20K        2 /* set fifo depth to 20K bytes */
#define MTD_SERDES_PPM_FIF0_40K        3 /* set fifo depth to 40K bytes */
#define MTD_SERDES_PPM_FIFO_GETONLY    4 /* This value will cause function to return
                                            current setting without updating */
/******************************************************************************
 MTD_STATUS mtdSetGetSerdesPPMFifo
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     IN MTD_U16 HorXunit,    
     INOUT MTD_U16 *fifo_offset
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31
    HorXunit - MTD_H_UNIT or MTD_X_UNIT, depending on which interface is
               being set
    fifo_offset - if input parameter is set to one of the following: 
                    MTD_SERDES_PPM_FIFO_2K
                    MTD_SERDES_PPM_FIFO_10K
                    MTD_SERDES_PPM_FIFO_20K
                    MTD_SERDES_PPM_FIF0_40K
                  updates the serdes PPM fifo depth accordingly and
                  returns the parameter unchanged.
                  if input parameter is:
                    MTD_SERDES_PPM_FIFO_GETONLY
                  returns the current PPM fifo offset without changing
                  the register.
    

 Outputs:
    fifo_offset - if input parameter is set to MTD_SERDES_PPM_FIFO_GETONLY,
                  returns the current setting in fifo_offset without
                  modifying the register bits. The returned value will be
                  one of the following:
                    MTD_SERDES_PPM_FIFO_2K
                    MTD_SERDES_PPM_FIFO_10K
                    MTD_SERDES_PPM_FIFO_20K
                    MTD_SERDES_PPM_FIF0_40K
                                    
 Returns:
    MTD_OK or MTD_FAIL if there's an io error, or input parameter is out of range.

 Description:
    This function is used to either set the PPM fifo depth of the H unit
    or X unit fifo by writing 4.F00C.15:14 or 3.F00c.15:14, or returns
    the current setting by reading the same bit field without modifying
    the register bits if the input parameter fifo_offset is set to
    MTD_SERDES_PPM_FIFO_GETONLY.

 Side effects:
    None

 Notes/Warnings:
    None

******************************************************************************/
MTD_STATUS mtdSetGetSerdesPPMFifo
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,    
    INOUT MTD_U16 *fifo_offset
);

/* possible values for pktPatternControl */
#define MTD_PKT_NO_MASK            0  /* payload sent without change */
#define MTD_PKT_INV_SKIP_WRD       2  /* inverts every other word  */
#define MTD_PKT_2INV_2NOT          3  /* two inverted two not inverted */
#define MTD_PKT_LFT_SHFT_BYTE      4  /* left shift by byte */
#define MTD_PKT_RHT_SHFT_BYTE      5  /* right shift by byte */
#define MTD_PKT_LFT_SHFT_WRD       6  /* left shift by word */
#define MTD_PKT_RHT_SHFT_WRD       7  /* right shift by word */
#define MTD_PKT_INC_BYTE           8  /* increment by byte */
#define MTD_PKT_DEC_BYTE           9  /* decrement by byte */
#define MTD_PKT_RANDOM_BYTE       10  /* pseudo-random byte */
#define MTD_PKT_RANDOM_WORD       11  /* pseudo-random word */

/* possible values for frameLengthControl */
#define MTD_PKT_RAND_LEN0          0 /* rand len 64 bytes - 1518 bytes */
#define MTD_PKT_RAND_LEN1          1 /* rand len 64 bytes - 0xFFF bytes */
#define MTD_PKT_RAND_LEN2          2 /* rand len 64 bytes - 0x1FFF bytes */
#define MTD_PKT_RAND_LEN3          3 /* rand len 64 bytes - 0x3FFF bytes */
#define MTD_PKT_RAND_LEN4          4 /* rand len 64 bytes - 0x7FFF bytes */
#define MTD_PKT_RAND_LEN5          5 /* rand len 64 bytes - 0xFFFF bytes */
/* 6 and 7 are reserved/undefined */
/* values 8 and greater are the exact length of the frame in bytes up to 0xFFFF */

/******************************************************************************
 MTD_STATUS mtdConfigurePktGeneratorChecker
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     IN MTD_U16 HorXunit,
     IN MTD_BOOL readToClear,
     IN MTD_BOOL dontuseSFDinChecker,
     IN MTD_U16 pktPatternControl,
     IN MTD_BOOL generateCRCoff,
     IN MTD_U32 initialPayload,
     IN MTD_U16 frameLengthControl,
     IN MTD_U16 numPktsToSend,
     IN MTD_BOOL randomIPG,
     IN MTD_U16 ipgDuration
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31
    HorXunit - MTD_H_UNIT or MTD_X_UNIT, depending on which interface is
               being configured
    readToClear - MTD_TRUE if the desired behavior is that when the
                  counters are read, they are reset. MTD_FALSE if the
                  desired behavior is that they must be explicitly cleared
                  by mtdPktGeneratorCounterReset() and reading
                  them just returns the current value without clearing them.
    dontuseSFDinChecker - MTD_TRUE indicates to start CRC checking after the
                      first 8 bytes in the packet, MTD_FALSE indicates
                      to look for SFD before starting CRC checking
    pktPatternControl - controls the generation of the payload. One of the
                        following:
                            MTD_PKT_NO_MASK
                            MTD_PKT_INV_SKIP_WRD
                            MTD_PKT_2INV_2NOT
                            MTD_PKT_LFT_SHFT_BYTE
                            MTD_PKT_RHT_SHFT_BYTE
                            MTD_PKT_LFT_SHFT_WRD
                            MTD_PKT_RHT_SHFT_WRD
                            MTD_PKT_INC_BYTE
                            MTD_PKT_DEC_BYTE
                            MTD_PKT_RANDOM_BYTE
                            MTD_PKT_RANDOM_WORD
    generateCRCoff - MTD_TRUE turns CRC generation off, MTD_FALSE turns
                     CRC generation on
    initialPayload - start of payload value. Format is from MS bit to
                     LS bit: BYTE3:BYTE2:BYTE1:BYTE0, bit 31 to bit 0,
                     left to right
    frameLengthControl - controls the length of the frame in bytes.
                         values 0...5 (see above) pick various random lengths
                         from 64 to a different stop value. 6/7 is undefined
                         and values 8 and greater are the exact frame length
                         in bytes. See definitions above MTD_PKT_RAND_LEN0-5.
    numPktsToSend - 0 stops generation, 0x001-0xFFFE sends exactly that number
                    of packets then stops. 0xFFFF sends continuously.
    randomIPG - MTD_TRUE uses a random IPG from 5 bytes to value specified
                in ipgDuration. MTD_FALSE uses fixed IPG as specified in
                ipgDuration.
    ipgDuration - Meaning depends on randomIPG (see above). Each bit
                  equals 4 bytes of idle. Valid range is 0...0x7FFF.

 Outputs:
    None

 Returns:
    MTD_OK or MTD_FAIL if io error or bad parameter passed in (out of range)

 Description:
    This function is used to configure the packet generator/checker for the
    H unit or X unit.

 Side effects:
    None

 Notes/Warnings:
    Call mtdEnablePktGeneratorChecker() to enable/start the generator/checker.

******************************************************************************/
MTD_STATUS mtdConfigurePktGeneratorChecker
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    IN MTD_BOOL readToClear,
    IN MTD_BOOL dontuseSFDinChecker,
    IN MTD_U16 pktPatternControl,
    IN MTD_BOOL generateCRCoff,
    IN MTD_U32 initialPayload,
    IN MTD_U16 frameLengthControl,
    IN MTD_U16 numPktsToSend,
    IN MTD_BOOL randomIPG,
    IN MTD_U16 ipgDuration
);

/******************************************************************************
 MTD_STATUS mtdEnablePktGeneratorChecker
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     IN MTD_U16 HorXunit,
     IN MTD_BOOL enableGenerator,
     IN MTD_BOOL enableChecker
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31
    HorXunit - MTD_H_UNIT or MTD_X_UNIT, depending on which interface is
               being enabled
    enableGenerator - MTD_TRUE starts the packet generator, MTD_FALSE
                      stops it
    enableChecker - MTD_TRUE starts the packet checker, MTD_FALSE
                    stops it
        
 Outputs:
    None

 Returns:
    MTD_OK or MTD_FAIL

 Description:
    This function enables/disables either the packet generator or packet checker 
    (or both) in either the H unit or the X unit.

 Side effects:
    None

 Notes/Warnings:
    None

******************************************************************************/
MTD_STATUS mtdEnablePktGeneratorChecker
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    IN MTD_BOOL enableGenerator,
    IN MTD_BOOL enableChecker
);

/******************************************************************************
 MTD_STATUS mtdPktGeneratorCounterReset
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     IN MTD_U16 HorXunit    
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31
    HorXunit - MTD_H_UNIT or MTD_X_UNIT, depending on which interface is
               being reset
    
 Outputs:
    None

 Returns:
    MTD_OK or MTD_FAIL

 Description:
    This function explicitly clears all the counters when the packet
    generator/checker has been setup to be cleared by writing a 
    bit to the control register instead of being cleared when
    the counter(s) are read.

    This function operates on the counters in the H unit or X unit.

    When this function is called (and 3.F010.15/4.F010.15 is 0)
    the transmit, receive, error and link drop counters will be cleared.

 Side effects:
    None

 Notes/Warnings:
    This function assumes the generator/checker has been configured to
    be cleared by bit 3.F010.6/4.F010.6 by previously passing 
    MTD_FALSE for parameter readToClear in function 
    mtdConfigurePktGeneratorChecker() (thus setting bit F010.15 <= 0).

******************************************************************************/
MTD_STATUS mtdPktGeneratorCounterReset
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit    
);

#define MTD_PKT_GET_TX  0
#define MTD_PKT_GET_RX  1 
#define MTD_PKT_GET_ERR 2
/******************************************************************************
 MTD_STATUS mtdPktGeneratorGetCounter
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     IN MTD_U16 HorXunit,
     IN MTD_U16 whichCounter,
     OUT MTD_U64 *packetCount,
     OUT MTD_U64 *byteCount    
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31
    HorXunit - MTD_H_UNIT or MTD_X_UNIT, depending on which interface is
               being reset
    whichCounter - One of the following, depending on which set of counters
                   is being read:
                       MTD_PKT_GET_TX to read the generator/transmit counters
                       MTD_PKT_GET_RX to read the checker/receive counters
                       MTD_PKT_GET_ERR to read the packet error counter
                           

 Outputs:
    packetCount - tx/rx/err packet count in the following format
                  from MS bit to LS bit (note top 16-bits is always
                  0, since this is a 48-bit result):
                  0:WORD2:WORD1:WORD0 where each word is a 16-bit
                  unsigned value, and words are combined into 
                  a single, 48-bit result.                  
    byteCount - tx/rx byte count in the following format
                  from MS bit to LS bit (note top 16-bits is always
                  0, since this is a 48-bit result):
                  0:WORD2:WORD1:WORD0 where each word is a 16-bit
                  unsigned value, and words are combined into 
                  a single, 48-bit result. NOTE: This parameter
                  is always 0 when whichCounter is set to
                  MTD_PKT_GET_ERR, since error counter is a packet
                  count only.

 Returns:
    MTD_OK or MTD_FAIL

 Description:
    This function is used to read the transmit/receive/error counter for the
    packet generator/checker in either the H unit or X unit.

 Side effects:
    None

 Notes/Warnings:
    byteCount is always 0 for MTD_PKT_GET_ERR, since the error counter
    only counts packets.

    If packet generator/checker was configured to clear counter(s) on read,
    this function will clear the counter being read.

******************************************************************************/
MTD_STATUS mtdPktGeneratorGetCounter
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    IN MTD_U16 whichCounter,
    OUT MTD_U64 *packetCount,
    OUT MTD_U64 *byteCount    
);

/******************************************************************************
 MTD_STATUS mtdPktGeneratorCheckerGetLinkDrop
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     IN MTD_U16 HorXunit,
     OUT MTD_U16 *linkDropCounter        
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31
    HorXunit - MTD_H_UNIT or MTD_X_UNIT, depending on which interface is
               being reset
    

 Outputs:
    linkDropCounter - number of link drops received

 Returns:
    MTD_OK or MTD_FAIL

 Description:
    This function reads the link drop counter register.

 Side effects:
    None

 Notes/Warnings:
    If packet generator/checker was configured to clear counter(s) on read,
    this function will clear the link drop counter.

******************************************************************************/
MTD_STATUS mtdPktGeneratorCheckerGetLinkDrop
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    OUT MTD_U16 *linkDropCounter        
);


/******************************************************************************
 Functions shared between H unit and X unit for 1000BX/SGMII
******************************************************************************/

/* defines for bit 6,13 in SGMII control register */
#define MTD_SGMII_SPEED_10M   0
#define MTD_SGMII_SPEED_100M  1
#define MTD_SGMII_SPEED_1G    2
#define MTD_SGMII_SPEED_ASIS  3 /* to leave the setting as it is */
/******************************************************************************
 MTD_STATUS mtdSet1000BXSGMIIControl
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     IN MTD_U16 HorXunit,
     IN MTD_BOOL loopback,
     IN MTD_U16 speed,
     IN MTD_BOOL an_enable,
     IN MTD_BOOL power_down,
     IN MTD_BOOL restart_an,
     IN MTD_BOOL sw_reset
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31
    HorXunit - MTD_H_UNIT or MTD_X_UNIT, depending on which interface is
                being configured
    loopback - MTD_TRUE to set to loopback, MTD_FALSE to undo loopback
    speed - one of the following (only applies when AN is disabled)
        MTD_SGMII_SPEED_10M
        MTD_SGMII_SPEED_100M  
        MTD_SGMII_SPEED_1G    
    an_enable - MTD_TRUE to enable autonegotiation, MTD_FALSE to disable it
                NOTE: Toggling this setting will cause link to go down
                and restart AN bit 9 automatically will get set to 1
    power_down - MTD_TRUE to power down all PCS/PMA of this interface, MTD_FALSE
                 otherwise
    restart_an - MTD_TRUE to restart the SGMII autonegotiation, MTD_FALSE otherwise,
                 this bit self clears
    sw_reset - MTD_TRUE to do a software reset on this interface, MTD_FALSE otherwise,
               this bit self clears           

 Outputs:
    None

 Returns:
    MTD_OK or MTD_FAIL if some error occurred

 Description:
    This is a multi-purpose control function for controlling the H Unit SGMII
    interface or the X Unit SGMII interface. Most parameters of this function
    should only be used individually. Using this function, you may

        1. Disable autoneg or Enable autoneg (changing this automatically restarts an) (an_enable)
        2. Restart autoneg (restart_an)
        3. Set the SGMII speed (which only takes effect if AN is disabled) (speed)
        4. Put the SGMII into loopback (loopback)
        5. Power down the SGMII interface (power_down)
        6. Perform a software reset on the SGMII MMD

 Side effects:
    None

 Notes/Warnings:
    Call mtdGet1000BXSGMIIControl() to get the current settings. If 
    software reset is set to MTD_TRUE, the software reset will be performed 
    last.

******************************************************************************/
MTD_STATUS mtdSet1000BXSGMIIControl
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    IN MTD_BOOL loopback,
    IN MTD_U16 speed,
    IN MTD_BOOL an_enable,
    IN MTD_BOOL power_down,
    IN MTD_BOOL restart_an,
    IN MTD_BOOL sw_reset
);


/******************************************************************************
 MTD_STATUS mtdGet1000BXSGMIIControl
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     IN MTD_U16 HorXunit,
     OUT MTD_BOOL *loopback,
     OUT MTD_U16 *speed,
     OUT MTD_BOOL *an_enable,
     OUT MTD_BOOL *power_down
 );


 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31
    HorXunit - MTD_H_UNIT or MTD_X_UNIT, depending on which interface is
                being queried

 Outputs:
    loopback - MTD_TRUE = loopback enabled, MTD_FALSE to otherwise
    speed - one of the following (only applies when AN is disabled)
        MTD_SGMII_SPEED_10M
        MTD_SGMII_SPEED_100M  
        MTD_SGMII_SPEED_1G    
    an_enable - MTD_TRUE autonegotiation enabled, MTD_FALSE otherwise
    power_down - MTD_TRUE if powered down, MTD_FALSE otherwise

 Returns:
    MTD_OK or MTD_FAIL if some error occurred

 Description:
    This function may be called to query the state of the bits in 3.2000
    (X unit SGMII control register) or 4.2000 (H unit SGMII control register).

 Side effects:
    None

 Notes/Warnings:
    Call mtdSet1000BXSGMIIControl() to change the current settings.

******************************************************************************/
MTD_STATUS mtdGet1000BXSGMIIControl
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    OUT MTD_BOOL *loopback,
    OUT MTD_U16 *speed,
    OUT MTD_BOOL *an_enable,
    OUT MTD_BOOL *power_down
);

/******************************************************************************
 MTD_STATUS mtdGet1000BXSGMIIStatus
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     IN MTD_U16 HorXunit,
     OUT MTD_BOOL *an_complete,
     OUT MTD_BOOL *remote_fault,
     OUT MTD_BOOL *link_status_latched,
     OUT MTD_BOOL *link_status_current
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31
    HorXunit - MTD_H_UNIT or MTD_X_UNIT, depending on which interface is
                being queried    

 Outputs:
    an_complete - MTD_TRUE if autonegotiation has completed (MTD_FALSE
                  indicates link down, autoneg disabled or link up due
                  to autoneg bypassed)
    remote_fault - MTD_TRUE if remote fault condition detected, MTD_FALSE
                   in SGMII mode, or no remote fault condition detected
                   (this parameter only valid in 1000BX mode)
    link_status_latched - MTD_FALSE, indicates link was lost since last read.
                  This bit is a latched low bit. This is first read
                  value.
    link_status_current - MTD_FALSE, indicates link is currently down. 
                  This bit is a latched low bit. This is second read of
                  register.
                  

 Returns:
    MTD_OK or MTD_FAIL

 Description:
    Queries the status register in 3.2001 (X unit 1000BX/SGMII Status)
    or 4.2001 (H unit SGMII Status) and returns the value of bits:
    5: autoneg complete (MTD_TRUE if link up and autoneg has completed)
    4: remote fault (1000BX only mode valid, always MTD_FALSE in SGMII mode)
    2: link status (value of first read of register returned in
       link_status_latched, value of second read returned in link_status_current)
    
    

 Side effects:
    None

 Notes/Warnings:
    The SGMII status is not valid unless the host interface is up at
    a speed of 1G/100M or 10M. Check the media side link up and speed
    before calling this function.

    This means if the media side link is down and the host link down
    speed is set to 10G, the host SGMII link status returned by this function
    is not valid and should be ignored.

******************************************************************************/
MTD_STATUS mtdGet1000BXSGMIIStatus
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    OUT MTD_BOOL *an_complete,
    OUT MTD_BOOL *remote_fault,
    OUT MTD_BOOL *link_status_latched,
    OUT MTD_BOOL *link_status_current
);




/******************************************************************************
 Functions shared between H unit and X unit for 10GBASE-R PCS
******************************************************************************/


/******************************************************************************
 MTD_STATUS mtdSet10GBRControl
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     IN MTD_U16 HorXunit,
     IN MTD_BOOL loopback,
     IN MTD_BOOL power_down,
     IN MTD_BOOL sw_reset
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31
    HorXunit - MTD_H_UNIT or MTD_X_UNIT, depending on which interface is
                being configured
    loopback - MTD_TRUE to set to loopback, MTD_FALSE to undo loopback
    power_down - MTD_TRUE to power down all PCS/PMA of this interface, MTD_FALSE
                 otherwise
    sw_reset - MTD_TRUE to do a software reset on this interface, MTD_FALSE otherwise,
               this bit self clears           
    

 Outputs:
    None

 Returns:
    MTD_OK or MTD_FAIL

 Description:
    This function is used to control the 10GBASE-R PCS interface on either the
    H unit (MAC interface) or the X unit (fiber interface) on the 88X3240.
    The PCS may be put in loopback, powered down, or a software reset may 
    be performed. The software reset bit is self clearing.

 Side effects:
    None

 Notes/Warnings:
    Use mtdGet10GBRControl() to read the current value of the loopback
    and power_down bits. In registers 3.1000 or 4.1000.

******************************************************************************/
MTD_STATUS mtdSet10GBRControl
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    IN MTD_BOOL loopback,
    IN MTD_BOOL power_down,
    IN MTD_BOOL sw_reset
);

/******************************************************************************
 MTD_STATUS mtdGet10GBRControl
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     IN MTD_U16 HorXunit,
     OUT MTD_BOOL *loopback,
     OUT MTD_BOOL *power_down,
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31
    HorXunit - MTD_H_UNIT or MTD_X_UNIT, depending on which interface is
                being read
 Outputs:
    loopback - MTD_TRUE if loopback is set, MTD_FALSE if not
    power_down - MTD_TRUE if PCS/PMA is powered down, MTD_FALSE
                 otherwise

 Returns:
    MTD_OK or MTD_FAIL

 Description:
    This function is used to read the values of bits in the 10GBASE-R
    PCS control register of the H or X unit.

 Side effects:
    None

 Notes/Warnings:
    Use mtdSet10GBRControl() to change the settings on the 10GBASE-R PCS
    control register, 3.1000 or 4.1000.

******************************************************************************/
MTD_STATUS mtdGet10GBRControl
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    OUT MTD_BOOL *loopback,
    OUT MTD_BOOL *power_down
);


/******************************************************************************
 MTD_STATUS mtdGet10GBRStatus1
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     IN MTD_U16 HorXunit,
     OUT MTD_BOOL *tx_lpi_latch,
     OUT MTD_BOOL *rx_lpi_latch,
     OUT MTD_BOOL *tx_lpi_current,
     OUT MTD_BOOL *rx_lpi_current,
     OUT MTD_BOOL *fault,
     OUT MTD_BOOL *link_status_latch,
     OUT MTD_BOOL *link_status_current    
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31
    HorXunit - MTD_H_UNIT or MTD_X_UNIT, depending on which interface is
               being read
 Outputs:
    tx_lpi_latch - value of bit 11, MTD_TRUE if Tx PCS has received LPI
                   since last read of this bit, MTD_FALSE if not
    rx_lpi_latch- value of bit 10, MTD_TRUE if Rx PCS has received LPI
                   since last read of this bit, MTD_FALSE if not
    tx_lpi_current - value of bit 9, MTD_TRUE if Tx PCS is receiving LPI
                     currently, MTD_FALSE if not
    rx_lpi_current - value of bit 8, MTD_TRUE if Rx PCS is receiving LPI
                     currently, MTD_FALSE if not
    fault - value of bit 7, MTD_TRUE if a fault is indicated by 
            bit 10 or 11 in register 0x1008, MTD_FALSE otherwise
    link_status_latch - first read value of bit 2, which is a latched
            low bit. MTD_FALSE if PCS has had a link down since last
            time this register was read, MTD_TRUE if no link down
            has occurred.
    link_status_current - second read of bit 2. MTD_TRUE if link is
            currently up, MTD_FALSE if link is currently down    

 Returns:
    MTD_OK or MTD_FAIL

 Description:
    This function returns the value of status bits in the 10GBASE-R PCS
    STATUS 1 Register in either the H unit or X unit (4.1001 or 3.1001).    

 Side effects:
    None

 Notes/Warnings:
    Latch bits may be cleared when calling this register if the underlying
    condition has changed.

    Clearing fault requires reading register 0x1008.

******************************************************************************/
MTD_STATUS mtdGet10GBRStatus1
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    OUT MTD_BOOL *tx_lpi_latch,
    OUT MTD_BOOL *rx_lpi_latch,
    OUT MTD_BOOL *tx_lpi_current,
    OUT MTD_BOOL *rx_lpi_current,
    OUT MTD_BOOL *fault,
    OUT MTD_BOOL *link_status_latch,
    OUT MTD_BOOL *link_status_current    
);

/******************************************************************************
 MTD_STATUS mtdGet10GBRFault
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     IN MTD_U16 HorXunit,         
     OUT MTD_BOOL *tx_fault_latch,
     OUT MTD_BOOL *rx_fault_latch,
     OUT MTD_BOOL *tx_fault_current,
     OUT MTD_BOOL *rx_fault_current
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31
    HorXunit - MTD_H_UNIT or MTD_X_UNIT, depending on which interface is
               being read
               
 Outputs:
    tx_fault_latch - MTD_TRUE if there was a fault on the tx path since the last
                     call of this function, MTD_FALSE if not (bit 11)
    rx_fault_latch - MTD_TRUE if there was a fault on the rx path since the last
                     call of this function, MTD_FALSE if not (bit 10)
    tx_fault_current - MTD_TRUE if there is currently a fault on the tx path, 
                       MTD_FALSE if not (bit 11)
    rx_fault_current - MTD_TRUE if there is currently a fault on the rx path, 
                       MTD_FALSE if not (bit 10)
                                          
 Returns:
    MTD_OK or MTD_FAIL

 Description:
    Reads the Status 2 register in either the H unit or X unit twice and
    returns the tx/rx fault latched value (first read) in the latch variables
    and the current tx/rx fault value (second read) in the current variables.

    Latched variables indicate if there was a fault since the last read, 
    whereas current variables indicate if there is currently a tx/rx fault.

 Side effects:
    None

 Notes/Warnings:
    Reading this register will clear the fault bit in register 0x1001 if
    the fault condition has cleared.

******************************************************************************/
MTD_STATUS mtdGet10GBRFault
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,    
    OUT MTD_BOOL *tx_fault_latch,
    OUT MTD_BOOL *rx_fault_latch,
    OUT MTD_BOOL *tx_fault_current,
    OUT MTD_BOOL *rx_fault_current
);

/******************************************************************************
 MTD_STATUS mtdGet10GBRReceiveStatus
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     IN MTD_U16 HorXunit, 
     OUT MTD_BOOL *link_up,
     OUT MTD_BOOL *high_ber,
     OUT MTD_BOOL *block_lock    
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31
    HorXunit - MTD_H_UNIT or MTD_X_UNIT, depending on which interface is
               being read    

 Outputs:
    link_up - MTD_TRUE if receive link is up, MTD_FALSE if down (bit 12)
    high_ber - MTD_TRUE if receive link is reporting high bit error rate,
               MTD_FALSE if not (bit 1)
    block_lock - MTD_TRUE if receive link is locked to received block,
                 MTD_FALSE if not (bit 0)

 Returns:
    MTD_OK or MTD_FAIL

 Description:
    Reads register 0x1020 in either the H unit or X unit and returns the
    flags above reporting the status of the 10GBASE-R PCS Receive path.

 Side effects:
    None

 Notes/Warnings:
    None

******************************************************************************/
MTD_STATUS mtdGet10GBRReceiveStatus
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit, 
    OUT MTD_BOOL *link_up,
    OUT MTD_BOOL *high_ber,
    OUT MTD_BOOL *block_lock    
);


/******************************************************************************
 MTD_STATUS mtdGet10GBRStatus2
 (
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 port,
     IN MTD_U16 HorXunit, 
     OUT MTD_BOOL *has_block_lock,
     OUT MTD_BOOL *reported_high_ber,
     OUT MTD_U8 *ber_counter,
     OUT MTD_U8 *errored_blocks_counter    
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - port number, 0-31
    HorXunit - MTD_H_UNIT or MTD_X_UNIT, depending on which interface is
               being read    
   
 Outputs:
    has_block_lock - MTD_FALSE if block lock was lost since the last time
                      this function was called (link encountered rx
                      errors to the point of losing lock), MTD_TRUE
                      if block lock was not lost since last time
                      this function was called.
    reported_high_ber - MTD_TRUE if a high ber condition was reported
                        since the last time this function was called,
                        MTD_FALSE if not (bit14)
    ber_counter - Number of bit errors since last read (0xFF indicates
                  counter saturated) (bits 13:8)
    errored_blocks_counter - Number of errored blocks since last read
                  (0xFF indicates counter saturated) (bits 7:0)

 Returns:
    MTD_OK or MTD_FAIL

 Description:
    This function reads register 0x1021 in either the H unit or X unit
    and returns the above values. Latched bits are cleared if the 
    condition has changed and counters are cleared when read.

    This function is useful to check if there have been errors on
    the rx path since the last time this function was called and how many
    errors may have occurred.

    Call once at the beginning of the period desired to monitor to clear
    the latched bits and counters, and then call again at the end of the
    period to check if any errors occurred during the period.

 Side effects:
    None

 Notes/Warnings:
    Calling this function clears the latched bits in register 0x1021 and
    the counters.

    Current values are not provided in this function. Current values
    for block lock and high ber can be fetched by calling 
    mtdGet10GBRReceiveStatus().

******************************************************************************/
MTD_STATUS mtdGet10GBRStatus2
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit, 
    OUT MTD_BOOL *has_block_lock,
    OUT MTD_BOOL *reported_high_ber,
    OUT MTD_U8 *ber_counter,
    OUT MTD_U8 *errored_blocks_counter    
);

#if C_LINKAGE
#if defined __cplusplus
}
#endif
#endif


#endif

