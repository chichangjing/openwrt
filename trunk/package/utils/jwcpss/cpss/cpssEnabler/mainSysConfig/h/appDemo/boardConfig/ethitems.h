/*****************************************************************************
 * ethitems.h
 * Ethernet Configuration definitions
 *
 * Copyright (c) 1993-2000, Spirent Communications of Calabasas, Inc.
 * Copyright (c) 1996-1999, Netcom Systems, Inc.
 ****************************************************************************/

#ifndef _ETHITEMS_H_
#define _ETHITEMS_H_

#include "ettypes.h"

/****************************************************************************
 *  iType1 values
 ****************************************************************************/

/* iTypes for HTSetStructure */
#define ETH_TRANSMIT						0x5000	/* set up transmit parameters */
#define ETH_TRIGGER 						0x5001	/* set up triggers */
#define ETH_LATENCY						0x5002	/* set up latency test parameters */
#define ETH_COLLISION					0x5003	/* set up collision mode */
#define ETH_WRITE_MII					0x5004	/* write to a MII Address/Register */
#define ETH_FILL_PATTERN				0x5005	/* set background pattern, using an array of unsigned chars */
#define ETH_SET_SPEED					0x5006	/* set card tx speed, using unsigned long data and SPEED_XXX constants */
#define ETH_VLAN							0x5007	/* set one or more VLAN tags */
#define ETH_LINK_INIT					0x5008	/* initiate link process with link partner */
#define ETH_LINK_VERIFY					0x5009	/* verify successful link with link partner */
#define ETH_RSMII							0x500A	/* configure RMII/SMII parameters */
/* Change the value of ETH_PROTOCOL_PARAMETERS in SmartLib 5.00-32. Between SmartLib 5.00-3 and
   SmartLib 5.00-31, this command will always return -506 error. The reason was that this command
   shares the same iType1 value with FST_PROTOCOL_PARAMETERS while using different structures.
   The change made in SmartLib 5.00-3 (CR20369) cannot handle this case properly so that error -506
   is returned. Change the value of ETH_PROTOCOL_PARAMETERS fixes the problem. */
/*#define ETH_PROTOCOL_PARAMETERS		FST_PROTOCOL_PARAMETERS	*/ /* set up addresses, ARPS, PINGs, etc. */
#define ETH_PROTOCOL_PARAMETERS		0x501B	/* set up addresses, ARPS, PINGs, etc. */


/* iTypes for HTGetStructure */
#define ETH_CARD_INFO					0x5010	/* get information about a card */
#define ETH_COUNTER_INFO				0x5011	/* get counters */
#define ETH_ENHANCED_COUNTER_INFO	0x5012	/* get additional counters */
#define ETH_ENHANCED_STATUS_INFO		0x5013	/* get status information */
#define ETH_LATENCY_INFO  				0x5014	/* get latency measurement report */
#define ETH_FIND_MII_ADDR_INFO  		0x5015	/* find first MII PHY address with a legal device */
#define ETH_READ_MII_INFO				0x5016	/* read a specific MII address/reg */
#define ETH_EXTENDED_CARD_INFO		0x5017	/* get additional card information */
#define ETH_QOS_COUNTER_INFO			0x5018	/* get QoS counters */
#define ETH_EXTENDED_COUNTER_INFO	0x5019	/* get additional counters */
/* Change the value of ETH_PROTOCOL_PARAMETER_INFO in SmartLib 5.00-32. Between SmartLib 5.00-3 and
   SmartLib 5.00-31, this command will always return -506 error. The reason was that this command
   shares the same iType1 value with FST_PROTOCOL_PARAMETER_INFO while using different structures.
   The change made in SmartLib 5.00-3 (CR20369) cannot handle this case properly so that error -506
   is returned. Change the value of ETH_PROTOCOL_PARAMETER_INFO fixes the problem. */
/*#define ETH_PROTOCOL_PARAMETER_INFO	FST_PROTOCOL_PARAMETER_INFO */ /* retrieve current address, ARP, PINGs, etc. setup */
#define ETH_PROTOCOL_PARAMETER_INFO	0x501A /* retrieve current address, ARP, PINGs, etc. setup */

/* Readback functions: */
#define ETH_TRANSMIT_INFO				0xD000	/* read transmit parameters */
#define ETH_FILL_PATTERN_INFO			0xD002	/* read transmit parameters */

/* iTypes for HTSetCommand */
#define ETH_CLEAR_PORT					0x5020	/* clear counters */
#define ETH_RESET_PORT					0x5021	/* reset the card to a default condition; iType2 = reset type */
#define ETH_SELECT_RECEIVE				0x5022 	/* select port for receive data */
#define ETH_SELECT_TRANSMIT			0x5023 	/* select port for transmit data; iType2 = transmission mode */


/****************************************************************************
 *  structures to be used with HTSetStructure
 ****************************************************************************/

/******************************************************************************
Structure:	ETHTranmit
iType1:		ETH_TRANSMIT
Function:	HTSetStructure

ETHTransmit Description:

This structure sets all the basic transmit parameters.  Some of the fields
have constants defined for possible values.

Note:  If Auto Negotiation is turned on, it can be left untuoched when calling
ETH_TRANSMIT by setting the ucDuplexMode and/or ucSpeed to SPEED_DUPLEX_UNTOUCHED.
If Jumbo Frame is enabled for 3101A/3102A/3111A, the maximum number of streams
allowed will be 128. Any existing stream 128 will be deactivated by the 
firmware. The streams will be restored once the port comes out of the 
jumbo Mode. The number of IGMP groups is also reduced to 300. When jumbo frame
is enabled the number of PPPoE sessions are reduced.

******************************************************************************/
typedef struct tagETHTransmit
{
 unsigned char	ucTransmitMode;		/* transmit mode:
															CONTINUOUS_PACKET_MODE
															SINGLE_BURST_MODE
															MULTI_BURST_MODE
															CONTINUOUS_BURST_MODE
															ECHO_MODE */
 unsigned short	uiDataLength;       /* number of bytes per frame, not
													including 4 byte CRC */

 unsigned char	ucDuplexMode;		/* duplex mode:
															FULLDUPLEX_MODE
															HALFDUPLEX_MODE
															SPEED_DUPLEX_UNTOUCHED */
 unsigned char	ucSpeed;			/* set card speed:
															SPEED_10MHZ
															SPEED_100MHZ
															SPEED_4MHZ
															SPEED_16MHZ
															SPEED_DUPLEX_UNTOUCHED */
 unsigned short	uiCollisionBackoffAggressiveness;	/* wait factor for
														backing off from multiple collisions;
														uses powers of 2 with this factor */

 unsigned long	ulBurstCount;			/* number of frames per burst */
 unsigned long	ulMultiBurstCount;		/* number of bursts in multi-burst */

 unsigned long	ulInterFrameGap;		/* length of gap between frames
											based on scale set in uiInterFrameGapScale;
											1 to 1,600,000,000 in nanosecond scale
											1 to 1,600,000 in microsecond scale
											1 to 1600 in millisecond scale
											0 = random gap */
 unsigned short	uiInterFrameGapScale;	/* units for ulInterFrameGap:
											NANO_SCALE
											MICRO_SCALE
											MILLI_SCALE
											0 = bit times */

 unsigned long	ulInterBurstGap;		/* length of gap between bursts
											based on scale set in uiInterBurstGapScale;
											1 to 1,600,000,000 in nanosecond scale
											1 to 1,600,000 in microsecond scale
											1 to 1600 in millisecond scale */
 unsigned short	uiInterBurstGapScale;	/* units for ulInterBurstGap:
											NANO_SCALE
											MICRO_SCALE
											MILLI_SCALE
											0 = bit times */

 unsigned char	ucRandomBackground;		/* 1 = enable, 0 = disable */
 unsigned char	ucRandomLength;			/* 1 = enable, 0 = disable */

 unsigned char	ucCRCErrors;			/* 1 = enable, 0 = disable */
 unsigned char	ucAlignErrors;			/* number of alignment bits, */
										/* 0 = disable alignment errors */
 unsigned char 	ucSymbolErrors;			/* 1 = enable, 0 = disable */
 unsigned short	uiDribbleBits; 			/* number of dribble bits to transmit,
														valid from 0 - 7 */

 unsigned char	ucVFD1Mode;				/* VFD1 mode:
														HVFD_NONE
														HVFD_RANDOM
														HVFD_INCR
														HVFD_DECR
														HVFD_STATIC */
 unsigned short	uiVFD1Offset;			/* offset in bits */
 short			iVFD1Range;				/* size of VFD1 pattern, in bytes,
														6 bytes maximum;  negative value
														signifies bit size field */
 unsigned char	ucVFD1Pattern[6];		/* VFD1 pattern */
 unsigned short	uiVFD1CycleCount;		/*	if mode is increment or decrement,
														specifies the number of patterns to
														generate before repeating */

 unsigned short uiVFD1BlockCount;		/* no. of VFD1 pattern repeats
													before next pattern: 1=default;
													only supported by ML-7710,
													ML-5710, LAN-3100, and LAN-3101
													cards */

 unsigned char	ucVFD2Mode;				/* VFD2 mode, see values above */
 unsigned short	uiVFD2Offset;			/* offset in bits */
 short				iVFD2Range;				/* size of VFD2 pattern, in bytes,
														6 bytes maximum;  negative value
														signifies bit size field */
 unsigned char	ucVFD2Pattern[6];		/* VFD2 pattern */
 unsigned short uiVFD2CycleCount;		/*	if mode is increment or decrement,
														specifies the number of patterns to
														generate before repeating */

 unsigned short	uiVFD2BlockCount;		/* no. of VFD2 pattern repeats
													before next pattern: 1=default;
													only supported by ML-7710,
													ML-5710, LAN-3100, and LAN-3101
													cards */

 unsigned char	ucVFD3Mode;				/* VFD3 mode:
														HVFD_NONE
														HVFD_ENABLED */
 unsigned short	uiVFD3Offset;     		/* offset in bits */
 unsigned short	uiVFD3Range;        	/* size of one VFD3 pattern, in bytes,
														up to 2044 bytes maximum */
 unsigned short	uiVFD3DataCount;		/* size of total VFD3 buffer */

 unsigned short uiVFD3BlockCount;		/* no. of VFD3 pattern repeats
													before next pattern: 1=default;
													only supported by ML-7710,
													ML-5710, LAN-3100, and LAN-3101
													cards */

 unsigned char	ucVFD3Buffer[2044];		/* VFD3 data */

 unsigned char	ucAntiPartitioningEnable;	/* enable
                                               anti-partitioning
                                               mode, LAN-3100A only */
 unsigned char  ucJumboFrameEnable; /* To enable Jumbo Frame Mode,
											enable = 1, disable = 0(default),
											for LAN-3101A/3102A/3111A cards.
									Should be enabled on TX and RX*/
 unsigned char  ucARPOptimizationEnable; /* Enable=1, disable=0 (default),
											 When enabled, ARP exchange is 
											 performed with optimization, 
											 not per stream, but per 
											 destination MAC. A single ARP
											 request will be sent for multiple
											 streams with the same destination.
											For LAN-3101A/3102A/3111A*/
 unsigned char ucExtendedPPPoEEnable;	/* Reserved - This field is not supported */
 unsigned char	ucReserved[60];				
} ETHTransmit;


/******************************************************************************
Structure:	ETHTrigger
iType1:		ETH_TRIGGER
Function:	HTSetStructure

ETHTrigger Description:

This structure sets up the triggers for an ethernet card.
******************************************************************************/
typedef struct tagETHTrigger
{
 unsigned char	ucTriggerMode;			/* use ETH_TRIGGERXXX defines 
												ETH_TRIGGERS_OFF
												ETH_TRIGGER1_ONLY
												ETH_TRIGGER2_ONLY
												ETH_TRIGGER1_OR_TRIGGER2
												ETH_TRIGGER1_AND_TRIGGER2 */

 unsigned short	uiTrigger1Offset; 		/* offset of the trigger pattern,
															in bits */
 unsigned short uiTrigger1Range;		/* size of the trigger pattern,
										   in bytes */
 unsigned char	ucTrigger1Pattern[6];	/* pattern for trigger 1 */

 unsigned short	uiTrigger2Offset;		/* offset of the trigger pattern,
															in bits */
 unsigned short uiTrigger2Range;		/* size of the trigger pattern,
															in bytes */
 unsigned char	ucTrigger2Pattern[6];	/* pattern for trigger 2 */
} ETHTrigger;

/* Trigger combinations (for ucTriggerMode) */
#define ETH_TRIGGERS_OFF				0
#define ETH_TRIGGER1_ONLY				1
#define ETH_TRIGGER2_ONLY				2
#define ETH_TRIGGER1_OR_TRIGGER2		3
#define ETH_TRIGGER1_AND_TRIGGER2		4


/******************************************************************************
Structure:	ETHLatency
iType1:		ETH_LATENCY
Function:	HTSetStructure

ETHLatency Description:

This structure sets up an ethernet card for latency measurements.
******************************************************************************/
typedef struct tagETHLatency
{
 unsigned short	uiMode;			/* latency mode:
				HT_LATENCY_OFF		-- remove the card from latency measurements
				HT_LATENCY_RX		-- set the card as a latency receiver
				HT_LATENCY_RXTX		-- set the card as a latency receiver and a
									   latency transmitter
				HT_LATENCY_TX		-- set the card as a latency transmitter */
 unsigned short uiRange;		/*	size of the pattern, in bytes */
 unsigned short	uiOffset;		/* offset of the pattern, in bits */
 unsigned char		ucPattern[12];	/* data pattern that will stop the latency
										counter */
} ETHLatency;


/******************************************************************************
Structure:	ETHCollision
iType1:		ETH_COLLISION
Function:	HTSetStructure

ETHCollision Description:

This structure sets up the collision mode for an ethernet card.  The
following constants have been defined:

	uiMode:	COLLISION_OFF		-- collision off
				COLLISION_LONG		-- long collision
				COLLISION_ADJ		-- adjustable collision
				CORP_A				-- collision on receive packet, Port A
				CORP_B				-- collision on receive packet, Port B

******************************************************************************/
typedef struct tagETHCollision
{
 unsigned int uiOffset;		/* offset in bits where collision will take place */
 unsigned int uiDuration;	/* duration in bits of the collision */
 unsigned int uiCount;		/* specifies the number of consecutive collisions,
										up to 1024; 0 = continuous collisions */
 unsigned int uiMode;		/* collision mode, see values above */
} ETHCollision;


/******************************************************************************
Structure:	ETHMII
iType1:		ETH_WRITE_MII
            ETH_READ_MII_INFO
				ETH_FIND_MII_ADDR_INFO
Function:	HTSetStructure

ETHMII Description:

This structure writes or reads an MII register.  When used with
ETH_WRITE_MII, all three fields are inputs.  When used with
ETH_READ_MII_INFO, input uiAddress and uiRegister; uiValue will be
returned.  When used with ETH_FIND_MII_ADDR_INFO, the uiAddress field
will be returned.
******************************************************************************/
typedef struct tagETHMII
{
 unsigned short	uiAddress;	/*	specific address */
 unsigned short	uiRegister;	/* specific register */
 unsigned short	uiValue;	/* bit values to write to address/register */
} ETHMII;


/******************************************************************************
Structure:	ETHProtocolParameters
iType1:		ETH_PROTOCOL_PARAMETERS
Function:	HTSetStructure

ETHProtocolParameters Description:

This structure sets address, ARP, and PING information to the card.
It is identical to the FSTProtocolParameters structure.  See
FSTProtocolParameters for a more complete description.
******************************************************************************/
/* typedef FSTProtocolParameters ETHProtocolParameters; */

#define  ETH_PROTOCOL_DISABLED    		0
#define  ETH_PROTOCOL_GENERATE_ARP     0x00000001
#define  ETH_PROTOCOL_GENERATE_PING    0x00000002
#define  ETH_PROTOCOL_RESPOND_ARP      0x00000001
#define  ETH_PROTOCOL_RESPOND_PING     0x00000002


/******************************************************************************
Structure: ETHVLAN
iType1:  ETH_VLAN
Function: HTSetStructure
ETHVLAN Description:

This structure sets VLAN tags to a VLAN-capable SmartCard port.
Multiple VLAN tags can be set by sending an array of ETHVLAN
structures.

******************************************************************************/
typedef struct tagETHVLAN
{
		unsigned short	uiTPID;	/* VLAN type (2 bytes), for now always
                                 assumes 0x8100 */
		unsigned char 	ucPRI;	/* user priority 0-7 (3 bits), use
											ETH_VLAN_PRI_XXX definitions */
		unsigned char	ucCFI;	/* 1 = RIF present, 0 = RIF absent (1
											bit), use ETH_VLAN_CFI_XXX
											definitions */
		unsigned short	uiVID;	/* VLAN ID (12 bits) */
} ETHVLAN;
/******************************************************************************
ETHVLAN Comment:

When used with a GX-1420 SmartCard, the VLAN tag cannot be used
concurrently with a VFD3 buffer.
******************************************************************************/


/*  ucPRI values */
#define ETH_VLAN_PRI_0			0x00
#define ETH_VLAN_PRI_1			0x01
#define ETH_VLAN_PRI_2			0x02
#define ETH_VLAN_PRI_3			0x03
#define ETH_VLAN_PRI_4			0x04
#define ETH_VLAN_PRI_5			0x05
#define ETH_VLAN_PRI_6			0x06
#define ETH_VLAN_PRI_7			0x07
#define ETH_VLAN_PRI_LOWEST		VLAN_PRI_0
#define ETH_VLAN_PRI_HIGHEST		VLAN_PRI_7

/*  ucCFI values 	*/
#define ETH_VLAN_CFI_RIF_ABSENT		0x00
#define ETH_VLAN_CFI_RIF_PRESENT	0x01



/******************************************************************************
Structure: ETHLink
iType1:  ETH_LINK_INIT, ETH_LINK_VERIFY
Function: HTSetStructure
ETHLink Description:

This structure contains speed, duplex, flow control, and
autonegotiation information needed to establish and verify a link.
The iTimeout field is used with the ETH_LINK_VERIFY iType to indicate
how long to wait for a link to be established before timing out.

******************************************************************************/
typedef struct tagETHLink
{
		short iSpeed;			/* desired speed of link: SPEED_10MHZ,
                              SPEED_100MHZ, SPEED_1000MHZ */
		short iDuplex;      	/* desired duplex mode of link:
                              FULLDUPLEX_MODE, HALFDUPLEX_MODE */
		short iFlowControl; 	/* desired flowcontrol setting for link:
                              FLOWCONTROL_ENABLED,
                              FLOWCONTROL_DISABLED */
		short iMode;        	/* desired autonegotiation mode
                              AN_FORCE_AUTONEGOTIATION,
                              AN_DISABLE_AUTONEGOTIATION,
                              AN_REGISTERS_UNTOUCHED */
		short iTimeout;     	/* timeout in seconds (for verify phase
                              only) */
		short iRetryDecision;	/* decision input in case of non-fatal
										   error condition; use one of
										   NS_DECISION_ABORT,
										   NS_DECISION_RETRY_ONCE,
										   NS_DECISION_IGNORE,
										   NS_DECISION_USE_CALLBACK */
		short iReserved[1018];	/* reserved */

} ETHLink;


#define FLOWCONTROL_DISABLED	0
#define FLOWCONTROL_ENABLED 	1

#define AN_DISABLE_AUTONEGOTIATION	0
#define AN_FORCE_AUTONEGOTIATION 	1
#define AN_REGISTERS_UNTOUCHED		2

#define NS_DECISION_ABORT			0
#define NS_DECISION_RETRY_ONCE	1
#define NS_DECISION_IGNORE			2
#define NS_DECISION_USE_CALLBACK	3


/******************************************************************************
Structure:	ETHRSMII
iType1:		ETH_RSMII
Function:	HTSetStructure

ETHRSMII Description:
Set RMII/SMII parameters.
******************************************************************************/
typedef struct tagETHRSMII
{
	unsigned char ucRSMIISelect;	  		/* MII_SELECT_RMII or
                                          MII_SELECT_SMII */
	unsigned char ucMDIOAccessSelect;	/* MDIO_ACCESS_8_VIA_0_3 =
                                          8-port device accessed via
                                          port 0-3;
                                          MDIO_ACCESS_8_VIA_4_7 =
                                          8-port device accessed via
                                          port 4-7;
                                          MDIO_ACCESS_4_PLUS_4 = 4+4
                                          port device accessed
                                          separately via ports 0-3 and
                                          ports 4-7. */
	unsigned char ucReserved[4];			/* reserved */
} ETHRSMII;
/******************************************************************************
ETHRSMII Comment:
The parameters in this structure apply to all ports on the card.  The
port number will be ignored.
*******************************************************************************/

#define MII_SELECT_RMII			 	1
#define MII_SELECT_SMII				0

#define MDIO_ACCESS_8_VIA_0_3		1
#define MDIO_ACCESS_8_VIA_4_7		2
#define MDIO_ACCESS_4_PLUS_4		3



/* scheduling modes */
#define	SCHEDULE_MODE_GAP 			0
#define	SCHEDULE_MODE_FRAME_RATE	1
#define	SCHEDULE_MODE_ADVANCED	   3


/****************************************************************************
 * structures to be used with HTGetStructure
 ****************************************************************************/

/******************************************************************************
Structure:	ETHCardInfo
iType1:		ETH_CARD_INFO
Function:	HTGetStructure

ETHCardInfo Description:

This structure returns information about an ethernet card.
******************************************************************************/
typedef struct tagETHCardInfo
{
 unsigned short	uiCardModel;		/* card model:
									CM_UNKOWN, CM_NOT_PRESENT,
									CM_SE_6205, CM_SC_6305,	CM_ST_6405,	CM_ST_6410,
									CM_SX_7205, CM_SX_7405,	CM_SX_7410,	CM_TR_8405,
									CM_VG_7605,	CM_L3_6705, CM_GX_1405,CM_WN_3405,
									CM_L3_6710,	CM_SX_7210,	CM_ML_7710, CM_ML_5710A */
 char					szCardModel[32];	/* card model identifier string */
 char					cPortID;				/* card type character:
									'A'	-- 10Mb Ethernet
									'F'	-- 10/100Mb Fast Ethernet
									'T'	-- 4/16Mb TokenRing
									'V'	-- VG/AnyLan
									'3'	-- Layer 3 10Mb Ethernet
									'G'	-- Gigabit Ethernet
									'S'	-- ATM Signaling
									'N'	-- Not present */
 unsigned short	uiPortType;			/* card type:
									CT_ACTIVE, CT_PASSIVE, CT_FASTX, CT_TOKENRING,
									CT_VG, CT_L3, CT_ATM, CT_GIGABIT, CT_ATM_SIGNALING,
									CT_NOT_PRESENT */
 unsigned long		ulPortProperties;	/* card attributes bit values:
									CA_SIGNALRATE_100MB	-- 100 MB capable
									CA_DUPLEX_FULL			-- Full Duplex capable
									CA_DUPLEX_HALF			-- Half Duplex capable
									CA_CONNECT_MII			-- MMI connector
									CA_CONNECT_TP			-- Twisted Pair connector
									CA_CONNECT_BNC			-- BNC connector
									CA_CONNECT_AUI			-- AUI connector
									CA_CAN_ROUTE			-- Routing capable
									CA_VFDRESETCOUNT		-- Resets VFD1 &2 counter
									CA_SIGNALRATE_4MB		-- 4 MB capable
									CA_SIGNALRATE_16MB	-- 16 MB capable
									CA_CAN_COLLIDE			-- Generates collisions
									CA_SIGNALRATE_25MB	-- 25 MB capable
									CA_SIGNALRATE_155MB	-- 155 MB capable
									CA_BUILT_IN_ADDRESS	-- Has a built in address
									CA_HAS_DEBUG_MONITOR	-- Allows Debug monitoring
									CA_SIGNALRATE_1000MB	-- 1 GB capable
									CA_CONNECT_FIBER	  	-- Fiber optic connector
									CA_CAN_CAPTURE			-- Has capture capability
									CA_ATM_SIGNALING		-- Performs ATM Signaling */
 unsigned long		ulHWVersions[32];	/* card version information */
} ETHCardInfo;


/******************************************************************************
Structure:	ETHCounterInfo
iType1:		ETH_COUNTER_INFO
Function:	HTGetStructure

ETHCounterInfo Description:

This structure returns counter information about an ethernet card.

NOTES:

To generate collision errors on the ML-7710, ML-5710A and LAN-6101A cards when testing
back-to-back, set the transmitting and receiving ports to half-duplex mode before transmitting.

******************************************************************************/
typedef struct tagETHCounterInfo
{
 unsigned long	ulRxFrames;			/* number of frames received */
 unsigned long	ulTxFrames;			/* number of frames transmitted */
 unsigned long	ulCollisions;		/* number of collisions */
 unsigned long	ulRxTriggers;		/* number of triggers received */
 unsigned long	ulRxBytes;			/* number of bytes received */
 unsigned long	ulCRCErrors;		/* number of CRC errors received */
 unsigned long	ulAlignErrors;		/* number of alignment errors detected */
 unsigned long	ulOversize;			/* number of oversize errors detected */
 unsigned long	ulUndersize;		/* number of undersize errors detected */
 unsigned long	ulTxFrameRate;		/* number of frames transmitted per second */
 unsigned long	ulRxFrameRate;		/* number of frames received per second */
 unsigned long	ulCRCErrorRate;	/* number of CRC errors received per second */
 unsigned long	ulOversizeRate;	/* number of oversize errors per second */
 unsigned long	ulUndersizeRate;	/* number of undersize errors per second */
 unsigned long	ulCollisionErrorRate;	/* number of collisions per second */
 unsigned long	ulAlignErrorRate;	/* number of alignment errors per second */
 unsigned long	ulRxTriggerRate;	/* number of triggers received per second */
 unsigned long	ulRxByteRate;		/* number of bytes receive per second */
} ETHCounterInfo;


/******************************************************************************
Structure:	ETHEnhancedCounterInfo
iType1:		ETH_ENHANCED_COUNTER_INFO
Function:	HTGetStructure

ETHEnhancedCounterInfo Description:

This structure returns additional counter information about an
ethernet card.  Use the same constants defined for
HTGetEnhancedCounters in et1000.h.

******************************************************************************/
typedef struct tagETHEnhancedCounterInfo
{
		unsigned int	uiMode;		/* 0 = count, 1 = rate */
		unsigned int	uiPortType;	/* card type returned, CT_XXX */
		unsigned long	ulMask1;		/* bit mask for standard counters */
		unsigned long	ulMask2;		/* bit mask for card-specific
                                    counters */
		unsigned long	ulData[64];	/* array of counters returned */
} ETHEnhancedCounterInfo;


/******************************************************************************
Structure:	ETHEnhancedStatusInfo
iType1:		ETH_ENHANCED_STATUS_INFO
Function:	HTGetStructure

ETHEnhancedStatusInfo Description:

This structure returns status information about an ethernet card.
******************************************************************************/
typedef struct tagETHEnhancedStatusInfo
{
 unsigned long	ulStatus;	/* status bit values:
											TR_STATUS_ACCESSED
											TR_STATUS_BADSTREAM
											TR_STATUS_BURST_MODE
											TR_STATUS_BEACONING
											TR_STATUS_DEVICE
											TR_STATUS_EARLY_TOKEN_RELEASE
											TR_STATUS_FULL_DUPLEX
											TR_STATUS_16MB
											TR_STATUS_RING_ALIVE
											TR_STATUS_LATENCY_STABLE
											TR_STATUS_TRANSMITTING
											GIG_STATUS_LINK
											GIG_STATUS_TX_PAUSE
											GIG_STATUS_CAPTURED_FRAMES
											GIG_STATUS_CAPTURE_STOPPED
											FAST7410_STATUS_TX_PAUSE
											L3_STATUS_6710
											VG_STATUS_MODE
											FR_STATUS_LINK_OK
											FR_STATUS_GROUP_MEMBER
											FR_STATUS_UNI_UP
											FR_STATUS_EIA_DSR
											FR_STATUS_EIA_CTS
											FR_STATUS_EIA_DCD
											FR_STATUS_EIA_TM
											FR_STATUS_EIA_DTR
											FR_STATUS_EIA_RTS
											FR_STATUS_EIA_RDL
											FR_STATUS_EIA_LLB */
} ETHEnhancedStatusInfo;


/******************************************************************************
Structure:	ETHLatencyInfo
iType1:		ETH_LATENCY_INFO
Function:	HTGetStructure

ETHLatencyInfo Description:

This structure returns the latency measurement for an ethernet card.
******************************************************************************/
typedef struct tagETHLatencyInfo
{
 unsigned long	ulLatency;	/* latency value for the preceding latency test */
} ETHLatencyInfo;


/******************************************************************************
Structure:	ETHMIIInfo
iType1:		ETH_FIND_MII_ADDR_INFO
				ETH_READ_MII_INFO
Function:	HTGetStructure

ETHMIIInfo Description:

This structure returns MII information.  It is defined identically to the
ETHMII structure.
******************************************************************************/
typedef ETHMII ETHMIIInfo;


/******************************************************************************
Structure: ETHExtendedCardInfo
iType1:  ETH_EXTENDED_CARD_INFO
Function: HTGetStructure
ETHExtendedCardInfo Description:

This structure returns additional information about the current state
of the Ethernet SmartCard.  Currently this function only applies to
GX-1420 SmartCards.

******************************************************************************/
typedef struct tagETHExtendedCardInfo
{
		unsigned long ulLinkStateChanges;	/* number of times link has
														been re-established */

		unsigned long ulTxMgmtFrames;		/* number of management frames
                                          transmitted from the local
                                          stack */
		unsigned long ulRxMgmtFrames;		/* number of management frames
                                          received by the local stack */

		unsigned long ulRxARPRequests;	/* number of ARP requests
                                          received */
		unsigned long ulTxARPRequests;	/* number of ARP requests
                                          transmitted */
		unsigned long ulTxARPReplies;		/*	number of ARP replies sent */
		unsigned long ulRxARPReplies;		/* number of ARP replies
                                          received */

		unsigned long ulTxPingReplies;	/* number of Ping replies sent */
		unsigned long ulTxPingRequests;	/* number of Ping requests sent */
		unsigned long ulRxPingRequests;	/* number of Ping requests
                                          received */
		unsigned long ulRxPingReplies;	/* number of Ping replies
                                          received */

		unsigned long ulRxVLANFrames;		/* number of VLAN frames
                                          received */
		unsigned long ulRxIPFrames;		/* number of IP frames received */
		unsigned long ulIPChecksumErrors;	/* number of IP frames
														received containing bad
														header checksums */

		unsigned long ulMgmtFrameCRCErrors;	/* number of management
                                          frames received containing
                                          CRC errors */
		unsigned long ulMgmtFrameIPChecksumErrors;	/* number of
                                          management frames received
                                          containing bad IP header
                                          checksums */
		unsigned long ulMgmtFrameUnknown;	/* number of management
													   frames received that
													   couldn't be processed
													   because the frame was an
													   unrecognized type */

		unsigned long ulTimecount;			/* tick counter, optionally to
                                          compute rate counts */

		unsigned long ulReceiveErrors;	/* number of non-collision
                                          packets with receive errors */
		unsigned long ulFalseCarrierSense;	/* number of false carrier
                                          sense events */
		unsigned long ulRxNOKs;			 	/* number of receiver NOKs */
		unsigned long ulExtendedPHYStatus;	/* additional PHY status
                                             bits, see ETH_PHY_XXX
                                             defines */

		unsigned long ulReserved[2];		/* reserved -- not currently in
                                          use */

} ETHExtendedCardInfo;

/******************************************************************************
ETHExtendedCardInfo Comment:

The bit meanings for the ulExtendedPHYStatus field are given by the
ETH_PHY_XXX defines.  The 3 bits from 26-24, given by
ETH_PHY_AUTONEGOTIATE_MODE_XXX, describe the current autonegotiation
operating mode: "111" = 1000MHz full duplex, "101" = 100MHz full
duplex, "011" = 100MHz half duplex (unused).  All other bit
combinations for these 3 bits are unused.

******************************************************************************/

#define ETH_PHY_AUTONEGOTIATE_COMPLETE 			0x80000000
#define ETH_PHY_AUTONEGOTIATE_LINK_GOOD_CHECK	0x40000000
#define ETH_PHY_AUTONEGOTIATE_ACK_DETECT			0x20000000
#define ETH_PHY_AUTONEGOTIATE_ABILITY_DETECT		0x10000000
#define ETH_PHY_AUTONEGOTIATE_NEED_PAGE_WAIT	 	0x08000000
#define ETH_PHY_AUTONEGOTIATE_MODE_26				0x04000000
#define ETH_PHY_AUTONEGOTIATE_MODE_25				0x02000000
#define ETH_PHY_AUTONEGOTIATE_MODE_24				0x01000000
#define ETH_PHY_PARALLEL_LINK_FAULT				 	0x00800000
#define ETH_PHY_LINK_PARTNER_REMOTE_FAULT			0x00400000
#define ETH_PHY_LINK_PARTNER_RX_NEW_PAGE			0x00200000
#define ETH_PHY_LINK_PARTNER_AUTONEG_CAPABLE		0x00100000
#define ETH_PHY_LINK_PARTNER_NEXT_PAGE_CAPABLE	0x00080000
#define ETH_PHY_LINK_UP									0x00040000
#define ETH_PHY_PAUSE_RX_ENABLED						0x00020000
#define ETH_PHY_PAUSE_TX_ENABLED						0x00010000
#define ETH_PHY_UNUSED_15								0x00008000
#define ETH_PHY_UNUSED_14								0x00004000
#define ETH_PHY_MDI_CROSSOVER							0x00002000
#define ETH_PHY_UNUSED_12								0x00001000
#define ETH_PHY_REMOTE_RECEIVER_OK					0x00000800
#define ETH_PHY_LOCAL_RECEIVER_OK					0x00000400
#define ETH_PHY_DESCRAMBLER_LOCKED					0x00000200
#define ETH_PHY_LINK_GOOD								0x00000100
#define ETH_PHY_REMOTE_FAULT							0x00000080
#define ETH_PHY_CARRIER_EXTENSION_ERROR			0x00000040
#define ETH_PHY_BAD_SSD_ERROR							0x00000020
#define ETH_PHY_BAD_ESD_ERROR							0x00000010
#define ETH_PHY_RX_ERROR								0x00000008
#define ETH_PHY_TX_ERROR								0x00000004
#define ETH_PHY_LOCK_ERROR								0x00000002
#define ETH_PHY_MLT3_CODE_ERROR						0x00000001	


/******************************************************************************
Structure: ETHQoSCounterInfo
iType1:  ETH_QOS_COUNTER_INFO
Function: HTGetStructure
ETHQoSCounterInfo Description:

Retrieve counters from the 8 one-byte QoS triggers.

******************************************************************************/
typedef struct tagETHQoSCounterInfo
{
		U64	u64RxTriggers[8];	/* counters for the 8 one-byte triggers
                                 used for QoS measurement */
		unsigned long	ulReserved[4];	/* reserved */
} ETHQoSCounterInfo;


/*********************************************************************************
Structure:   ETHExtendedCounterInfo
iType1:      ETH_EXTENDED_COUNTER_INFO
Function:    HTGetStructure
ETHExtendedCounterInfo Description:

Retrieve additional counters.
***********************************************************************************/
typedef struct tagETHExtendedCounterInfo {
	U64				u64RxVLANFrames;			/*	number of VLAN frames received;
												not available on
												ML-7710/ML-5710/ATM-345x */
	U64				u64RxIPFrames;				/* number of IP frames received;
												not available on
												ML-7710/ML-5710/ATM-345x */
	U64				u64RxIPChecksumErrors;		/* number of frames with bad
												IP checksums received;
												not available on
												ML-7710/ML-5710 */
	unsigned long	ulTxARPReplies;	/* number of ARP replies sent;
												not available on ATM-345x */
	unsigned long	ulRxARPReplies;	/* number of ARP replies received
												not available on ATM-345x */
	unsigned long	ulTxARPRequests;	/* number of ARP requests sent
												not available on ATM-345x */
	unsigned long	ulRxARPRequests;	/* number of ARP requests
												received; not available on
												ML-7710/ML-5710/ATM-345x */
	unsigned long	ulTxPingReplies;	/* number of Ping replies sent
												not available on ATM-345x */
	unsigned long	ulRxPingReplies;			/* number of Ping replies
												received; not available on
												ML-7710/ML-5710/ATM-345x */
	unsigned long	ulTxPingRequests;			/* number of Ping requests sent
												not available on ATM-345x */
	unsigned long	ulRxPingRequests;			/* number of Ping requests received
												not available on ATM-345x */
	U64				u64RxDataIntegrityErrors;	/* number of frames with
                                                data integrity errors
                                                received; only
                                                available on POS,
                                                LAN-3201, LAN-3101 and
                                                ATM-345x cards */
	U64				u64TxSignatureFrames;		/* number of frames with
															signature tags sent; only
															available on POS,
															LAN-3201, LAN-3101 and
															ATM-345x cards */
	U64				u64RxSignatureFrames;		/* number of frames with
															signature tags received;
															not available on
															LAN-3100A, GX-1420,
															LAN-3300A, LAN-3301A,
															LAN-3310A, LAN-3311A,
															POS-3504A, or POS-3505A
															cards */
	U64				u64RxJumboFrames;			/* number of jumbo frames
									received, for Terametrics and 3101 cards */
											     
	U64				u64TxJumboFrames;			/* number of jumbo frames received; only available on LAN-3101*/
	U64				u64RxIPv6Frames;			/* number of IPv6 frames received ; only available on LAN-3710*/
	U64				u64RxMPLSFrames;			/* number of MPLS frames received ; only available on LAN-3710*/
	U64				u64RxPauseFrames;			/* number of Pause frames received; only available on LAN-3710*/
	unsigned long	ulTxJumboRate;		/* rate of jumbo frames transmitted only available on LAN-3101*/
	unsigned long	ulRxJumboRate;		/* rate of jumbo frames received only available on LAN-3101*/
} ETHExtendedCounterInfo;







#endif	/* _ETHITEMS_H_ */


