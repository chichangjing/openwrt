/*****************************************************************************
 * fstitems.h
 * Fast Ethernet SmartCard Configuration definitions
 *
 * Copyright (c) 1993-2000, Spirent Communications of Calabasas, Inc.
 * Copyright (c) 1996-1999, Netcom Systems, Inc.
 ****************************************************************************/

/****************************************************************************/
#ifndef _FST_ITEMS_H_
#define _FST_ITEMS_H_

#include "ettypes.h"


/****************************************************************************
 *	input data, may be one of the:
 ****************************************************************************/
#define CAPTURE_ALL_TYPE		0x2011	/* Capture all frames received */

#define CAPTURE_BAD_TYPE		0x2012	/* Capture errors frames only */

#define CAPTURE_TRIGGERS_TYPE	0x2014	/* Capture Trigger frames only */ 


/****************************************************************************
 *  iType1 values
 ****************************************************************************/
#define FST_BASE_ITYPE				0x500
#define FST_VLAN					(FST_BASE_ITYPE + 0)	/* Send VLAN tag information      */
#define FST_CONTROL_AUX				(FST_BASE_ITYPE + 2)  	/* Set Flow Control and Preamble Len */
#define FST_CAPTURE_PARAMS			(FST_BASE_ITYPE + 3)	/* Set capture filters and controls  */
#define FST_CAPTURE_COUNT_INFO 		(FST_BASE_ITYPE + 5)	/* Get the number of captured frames */
#define FST_CAPTURE_INFO			(FST_BASE_ITYPE + 6)	/* Get info about captured frames.  Note that "index" is the first frame and "count" is the total number of frames to get information for. */
#define FST_CAPTURE_DATA_INFO		(FST_BASE_ITYPE + 7)	/* Get captured frame data           */
#define FST_ALTERNATE_TX 			(FST_BASE_ITYPE + 8)	/* Setup the Alt Tx stream           */
#define FST_PROTOCOL_PARAMETERS     (FST_BASE_ITYPE + 9) 	/* Setup ARP and/or PING             */
#define FST_PROTOCOL_PARAMETER_INFO	(FST_BASE_ITYPE + 10) 	/* Get Protocol Params               */
#define FST_PROTOCOL_COUNTER_INFO   (FST_BASE_ITYPE + 11)	/* Get Protocol Counters             */

/****************************************************************************/
#define FST_MAX_CAPTURE_FRAMES	96


/****************************************************************************
 *  structures to be used with HTSetStructure
 ****************************************************************************/
/****************************************************************************/
/* FSTAlternateTx Description:

Used to control the Alternate Transmit stream. This stream will be
sent out after a number of normal Layer 2 frames have been sent.

*/
typedef struct tagFSTAlternateTx
 {
 unsigned char	ucEnabled;			/* 1 = enable, 0 = disable    */
 unsigned char	ucCRCErrors;		/* 1 = enable, 0 = disable    */
 unsigned char	ucErrorSymbol;		/* 1 = enable, 0 = disable    */
 unsigned char	ucDribble;			/* 1 = enable, 0 = disable    */
 unsigned long	ulAlternateCount;	/* no. normal frames between  */
                                    /* each alternate frame       */
 unsigned short uiDataLength;		/* length of data in bytes    */
 unsigned char	ucData[2048];		/* the background data buffer */
 } FSTAlternateTx;

/****************************************************************************/
/* FSTControlAux Description:

Auxiliary control parameters to enable flow control pause frames and to set
the preamble length.

*/
typedef struct tagFSTControlAux
 {
 unsigned char	ucFlowControlPause;	/* 1 = enable, 0 = disable   */
 unsigned char	ucPreambleLen;		/* preamble length in bits   */
									/* (valid: 16, 32, 48, 64)   */
                                    /* ML-7710 and LAN-3101 have */
                                    /* a fixed preamble, so for  */
                                    /* these cards, set to 0     */
 } FSTControlAux;


/****************************************************************************/
/* FSTCaptureParams Description:

Configure the Capture engine to capture the correct frames.

*/
typedef struct tagFSTCaptureParams
 {
 unsigned char	ucCRCErrors;		/* 1 = enable, 0 = disable    */
 unsigned char	ucOnTrigger;		/* 1 = enable, 0 = disable    */
 unsigned char  ucFilterMode;		/* 1 = capture filtered frames
												0 = capture all frames     */
 unsigned char	ucStartStopOnConditionXMode;/* 1 = stop on cond. 0 =
                                              start on condition */
 unsigned char	uc64BytesOnly;		/* 1 = enable, 0 = disable    */
 unsigned char	ucLast64Bytes;		/* 1 = last 64 bytes,         
											0 = first 64 bytes         */
 unsigned char	ucCollisions;		/* 1 = enable, 0 = disable    */
 unsigned char	ucStartStop;      /* 1 = start capture,         */
											/* 0 = stop capture           */
 } FSTCaptureParams;
/******************************************************************************
 * FSTCaptureParams Comment:

Enabling ucFilterMode will cause the card to only capture 64 bytes,
even if uc64BytesOnly is disabled.  Also, ucCRCErrors, ucOnTrigger,
ucCollisions, and ucStartStopOnConditionXMode all require ucFilterMode
to be enabled.

******************************************************************************/

/* ucFilterMode values */
#define FST_CAP_FILTERED_FRAMES_ONLY		1
#define FST_CAP_ALL_FRAMES					0

/* ucStartStopOnConditionXMode values */
#define FST_CAP_STOP_ON_CONDITION			1
#define FST_CAP_START_ON_CONDITION			0

/* uc64BytesOnly values */
#define FST_CAP_64_BYTES_ONLY				1
#define FST_CAP_ENTIRE_FRAME				0

/* ucLast64Bytes values */
#define FST_CAP_LAST_64_BYTES				1
#define FST_CAP_FIRST_64_BYTES				0

/****************************************************************************/
/* FSTVLAN Description:

Enables and configures all Virtual LAN header values.

*/
typedef struct tagFSTVLAN
 {
 unsigned char  ucVLANEnable;       /* 1 = enable, 0 = disable.   
                                       If configuring with an 
                                       array of Virtual LAN 
                                       headers, this field of the 
                                       first element in the array 
                                       is used to enable or 
                                       disable VLAN on the entire 
                                       port.                      */
 unsigned short uiTPID;     /* VLAN type (2 bytes), always 0x8100 */
 unsigned char  ucPRI;      /* user priority 0-7 (3 bits)         */
									 /* use VLAN_PRI_XXX definitions       */
 unsigned char  ucCFI;      /* 1=RIF present, 0=RIF absent (1 bit)*/
									 /* use VLAN_CFI_XXX definitions       */
 unsigned short uiVID;      /* VLAN ID (12 bits)                  */
 } FSTVLAN;

/*  ucPRI values */
#define VLAN_PRI_0				0x00
#define VLAN_PRI_1				0x01
#define VLAN_PRI_2				0x02
#define VLAN_PRI_3				0x03
#define VLAN_PRI_4				0x04
#define VLAN_PRI_5				0x05
#define VLAN_PRI_6				0x06
#define VLAN_PRI_7				0x07
#define VLAN_PRI_LOWEST			VLAN_PRI_0
#define VLAN_PRI_HIGHEST		VLAN_PRI_7

/*  ucCFI values 	*/
#define VLAN_CFI_RIF_ABSENT	0x00
#define VLAN_CFI_RIF_PRESENT	0x01


/****************************************************************************
 *	structures to be used with HTGetStructure
 ****************************************************************************/
/****************************************************************************/
/* FSTCaptureCountInfo Description:

Provides a structure in which to get how many frames have
been captured on this SmartCard.

*/
typedef struct tagFSTCaptureCountInfo
 {
 unsigned long	ulCaptureCount;   /* number of captured packets */
 } FSTCaptureCountInfo;

/****************************************************************************/
/* FSTCaptureFrameInfo Description:

This structure stores information about frames which have been
captured on the SmartCard.

This is not for use as an individual structure. An array of these
are used in the FSTCaptureInfo structure to retrieve information
about many captured frames at a time.

*/
typedef struct tagFSTCaptureFrameInfo
 {
 unsigned long	 ulIndex;         /* index of this frame           */
 unsigned char	 ucStatus;	      /* undersize, oversize, trigger, CRC
												error, or run-disparity (symbol
												error) use FST_CAPTURE_STATUS_XXX
												definitions; for ML-7710/ML-5710A
												use L3_CAPTURE_STATUS_XXX
												definitions */
 unsigned char	 ucPreambleCount; /* preamble length in nibbles, not
												available on the ML-7710/ML-5710A */
 unsigned short uiLength;        /* data length in bytes          */
 unsigned long	 ulTimestamp;     /* timestamp in units of 100ns */
 } FSTCaptureFrameInfo;

/*  ucStatus values (FSTCaptureInfo) */
#define FST_CAPTURE_STATUS_CRC			0x0001
#define FST_CAPTURE_STATUS_ALIGNMENT	0x0002
#define FST_CAPTURE_STATUS_UNDERSIZE	0x0004
#define FST_CAPTURE_STATUS_OVERSIZE		0x0008
/* #define FST_CAPTURE_STATUS_COLLISION	0x0010 */
#define FST_CAPTURE_STATUS_TRIGGER		0x0020
/* #define FST_CAPTURE_STATUS_VLAN			0x0040 */

/****************************************************************************/
/* FSTCaptureInfo Description:

The structure FSTCaptureInfo contains information about the 
captured frames as specified by index and count.

The structure FSTCaptureFrameInfo contains information about a
captured frame.

typedef struct tagFSTCaptureFrameInfo
{
 unsigned long	 ulIndex;          // index of this frame     
 unsigned char	 ucStatus;         // undersize, oversize, trigger,
                                   // CRC error, or run-disparity (symbol error)
                                   // use FST_CAPTURE_STATUS_XXX definitions    
 unsigned char	 ucPreambleCount;  // preamble length in nibbles
 unsigned short  uiLength;         // data length in bytes
 unsigned long	 ulTimestamp;      // timestamp in units of 100ns
} FSTCaptureFrameInfo;

*/
typedef struct tagFSTCaptureInfo
	{
	FSTCaptureFrameInfo	FrameInfo[FST_MAX_CAPTURE_FRAMES];
	} FSTCaptureInfo;

/****************************************************************************/
/****************************************************************************/
/* FSTCaptureDataInfo Description:

Used to retrieve the data of the frame which has been captured.

*/
typedef struct tagFSTCaptureDataInfo
 {
 unsigned long	ulFrameNum;			/* frame number (input) */
 unsigned char	ucData[2048];
 } FSTCaptureDataInfo;

/*********************************************************************/
/*	FSTProtocolParameters Description:
   This structure controls the card's interaction with the network with regards
   to the PING and ARP protocols, as well as IP checksums.

   ulProtocolGenerate can be filled with one of two values:
   FST_PROTOCOL_GENERATE_PING (2), or FST_PROTOCOL_GENERATE_ARP (1).

   FST_PROTOCOL_GENERATE_PING causes the card to generate ping frames at the
   interval specified in ulPINGPeriod sent to the address given in ucDstIP. The
   card does not use the destination MAC when transmitting the PING frame.
   Instead, it determines the MAC address by issuing ARP requests at the
   interval given by ulARPPeriod. Periodic ARPs continue until a response is
   received, at which time peroodic PING frames are transmitted at the interval
   specified in ulPINGPeriod. When the ARP response is received, the returned
   MAC address is stored in the ucDstMAC field. This can be later retrieved
   using the FST_PROTOCOL_PARAMS_INFO message function.

   FST_PROTOCOL_GENERATE_ARP causes the card to generate ARP frames at the
   interval specified in ulARPPeriod. The ARP requests are sent to ucDstIP. The
   returned results are stored and retrievable as described above. If the
   ucDstIP address is not on the local subnet as given by ucNetmask, the ARP
   request is sent to the ucDefaultGateway address instead.

   If the FST_PROTOCOL_PARAMETERS message function is invoked more than once,
   invocations after the first may start, stop, or restart ARP or PING
   generation, as follows:
                        +-------------------------------+
                        |       Current State           |
                        +-------------------------------+
                        | Stopped     |  In Progress    |
   +-----------------+--+-------------+-----------------+
   | Requested State |0 | No Action   |     Stops       |
   |                 +--+-------------+-----------------+
   |                 |1 |   Start     |    Restart      |
   +-----------------+--+-------------+-----------------+


   ulProtocolResponse should be filled with a bitwise OR of the desired flags:
   FST_PROTOCOL_RESPOND_ARP and FST_PROTOCOL_RESPOND_PING. The
   FST_PROTOCOL_RESPOND_ARP flag instructs the card whether to respond to
   incoming ARP requests. If 1, it responds to ARP requests destined for an IP
   address that matches the cards ucSrcIP for the bits where ucResponseMask is
   one. (Thus it responds to all ARPs, equivalent to "general ARP response" on
   the 7710, when ucResponseMask is all zero.) The provided MAC address for the
   ARP response is always ucSrcMAC.

   Turning on the FST_PROTOCOL_RESPOND_PING flag causes the card to respond to
   PING requests received in the case that the requested IP address matches the
   cards ucSrcIP for the bits where ucResponseMask is zero.
*/
/*********************************************************************/
#define  FST_PROTOCOL_DISABLED                      0
#define  FST_PROTOCOL_GENERATE_ARP                  0x00000001
#define  FST_PROTOCOL_GENERATE_PING                 0x00000002
#define  FST_PROTOCOL_RESPOND_ARP                   0x00000001
#define  FST_PROTOCOL_RESPOND_PING                  0x00000002
typedef struct tagFSTProtocolParameters
{
                                       /* Notice: 
                                       HTResetPort uses the following
                                       default values to initialize ports.  It 
                                       doesn't mean to initialize this structure 
                                       with the default values.
                                       */                                          
   unsigned char  ucDstMAC[6];         /* Destination MAC Address             */
                                       /* (default=0.0.0.0.0.0)               */
   unsigned char  ucSrcMAC[6];         /* source MAC Address                  */
                                       /* (default=0.0.0.0.0.0)               */
   unsigned char  ucDstIP[4];          /* Destination IP Address              */
                                       /* (default=192.168.98.21)             */
   unsigned char  ucSrcIP[4];          /* Source IP Address                   */
                                       /* (default=192.168.98.22)             */
   unsigned char  ucNetMask[4];        /* Source IP Mask (default:0.0.0.0)    */
   unsigned char  ucResponseMask[4];   /* Response IP Mask (default:0.0.0.0)  */
   unsigned char  ucDefaultGateway[4]; /* Default Gateway (default:0.0.0.0)   */
   unsigned long  ulProtocolResponse;  /* FST_PROTOCOL_RESPOND_xxx            */
                                       /* (default=FST_PROTOCOL_DISABLED)     */
   unsigned long  ulProtocolGenerate;  /* FST_PROTOCOL_GENERATE_xxx           */
                                       /* (default=FST_PROTOCOL_DISABLED)     */                                     
   unsigned long  ulARPPeriod;         /* time to wait before re-sending ARP  */
                                       /* (default = 100 unit= 1/10 second)   */
   unsigned long  ulPINGPeriod;        /* time to wait before re-sending PING */
                                       /* (default = 20  unit= 1/10 second)   */
   unsigned char  ucIPChecksumEnable;     /* Enable=1, disable=0 (default)    */
   unsigned char  ucIPChecksumErrorEnable;/* Enable=1, disalbe=0 (default)    */
} FSTProtocolParameters;

/*********************************************************************/
/*	FSTProtocolCountersInfo Description:
*/
/*********************************************************************/
typedef struct tagFSTProtocolCounterInfo
{
	U64 u64RxVLANFrames;			/* number of VLAN tag frames received       */
	U64 u64RxIPFrames;				/* number of IP frames received             */
	U64	u64IPChecksumErrors;		/* number of bad IP Checksum frames received*/
	unsigned long	ulRxARPReply;   /* number of ARP Reply frames received      */
	unsigned long	ulTxARPReply;   /* number of ARP Reply frames sent          */
	unsigned long	ulTxARPRequest; /* number of ARP Request frames sent        */
	unsigned long	ulRxARPRequest; /* number of ARP Request frames received    */
	unsigned long	ulRxPingReply;  /* number of PING Reply frames received     */
	unsigned long	ulTxPingReply;  /* number of PING Reply frames sent         */
	unsigned long	ulTxPingRequest;/* number of PING Request frames sent       */
	unsigned long	ulRxPingRequest;/* number of PING Request frames received   */
} FSTProtocolCounterInfo;

#endif	/* _FST_ITEMS_H_ */

