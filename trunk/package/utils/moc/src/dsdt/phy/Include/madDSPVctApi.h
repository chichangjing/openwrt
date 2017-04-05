#include <madCopyright.h>

/***********************************************************************
* madDSPVctApi.h
*
* DESCRIPTION:
*       API definitions for Advanced VCT with GDSP Testing on 
*       Marvell Phy Device
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*
************************************************************************/

#ifndef __MADDSPVCTAPI_h
#define __MADDSPVCTAPI_h
#ifdef __cplusplus
extern "C" {
#endif

#ifdef MAD_DSP_VCT

extern void madDbgPrint(char* format, ...);

/* #define MAD_CRITIC_INFO(format,args...) madDbgPrint (format, ## args) */

typedef enum {
	MAD_DVCT_OK=0,
	MAD_DVCT_DOWNSHIFT_LONG_CABLE,
	MAD_DVCT_CHG_SLAVE_SILENT,
	MAD_DVCT_ATTEN_NO_DOWNSHIFT,
	MAD_DVCT_NO_LINKUP_LESS100,
	MAD_DVCT_ATTEN_DOWNSHIFT_NO_LINK,
	MAD_DVCT_CHK_LINKPARTNER,
	MAD_DVCT_LEN_GREAT100,
	MAD_DVCT_LEN_BIG_DIFF,
	MAD_DVCT_CHK_CONNECT_TOOLONG,
	MAD_DVCT_4_LEN_BIG_DIFF,
	MAD_DVCT_RLOSS_NO_LINKPARTNER,
	MAD_DVCT_IMPED_MISMATCH,
	MAD_DVCT_XTALK_NO_LINKPARTNER,
	MAD_DVCT_SPLITPAIR_NO_LINKPARTNER,
	MAD_DVCT_CHK_CABLE_LINKERPARTNER,
	MAD_DVCT_ERR_CABLE_LEN,
	MAD_DVCT_LEN_VERY_CHK_CONNECTION
} MAD_DSP_VCT_RUN_ST;


typedef enum
{
    MAD_ETH_CAT3,
    MAD_ETH_CAT5,
    MAD_ETH_UNKNOWN
} MAD_ETH_CABLE_TYPE;

typedef enum
{
	MAD_SPLITPAIR_NO=0,
    MAD_SPLITPAIR_A,
    MAD_SPLITPAIR_B,
    MAD_SPLITPAIR_C,
    MAD_SPLITPAIR_D
} MAD_SPLITPAIR_CHAN;

typedef enum
{
    MAD_NO_SPLIT_PAIR,
    MAD_SPLIT_PAIR

} MAD_SPLITPAIR_STAT;

#define MAD_CHANNEL_NUM  4


#define    MAD_DSP_VCT_ATTENUATION		 0x1
#define    MAD_DSP_VCT_CABLE_LEN         0x2
#define    MAD_DSP_VCT_XTALK		     0x4
#define    MAD_DSP_VCT_ETH_CABLE_TYPE	 0x8
#define    MAD_DSP_VCT_SPLIT_PAIR		 0x10
#define    MAD_DSP_VCT_RET_LOSS			 0x20
#define    MAD_DSP_VCT_NO                0


#define MAD_MAX_PLOT_DATA		256
#define MAD_MAX_REF_PLOT_DATA	64+4
#define MAD_MAX_FLOAT			999999.9
#define MAD_EndFloat			999.0

typedef struct
{
	MAD_DSP_VCT_RUN_ST status;
    float 	  data[MAD_CHANNEL_NUM][MAD_MAX_PLOT_DATA];
    float 	  refData[MAD_MAX_REF_PLOT_DATA];
} MAD_PLOT_DATA;

typedef struct
{
	MAD_SPLITPAIR_STAT pairStatus[MAD_CHANNEL_NUM];
	MAD_DSP_VCT_RUN_ST status; 
	MAD_U8			splitPair[MAD_CHANNEL_NUM][MAD_CHANNEL_NUM-1]; /* [chanal][pair number] */
} MAD_SPLIT_PAIR_DATA;

typedef struct
{
    float accCablen;
	MAD_DSP_VCT_RUN_ST status; 
} MAD_CABLELENGTH_DATA;

typedef struct
{
    MAD_ETH_CABLE_TYPE ethCableType;
	MAD_DSP_VCT_RUN_ST status; 
} MAD_ETHCABLETYPE_DATA;

typedef struct
{
	MAD_PLOT_DATA attenuation;	/* [Channel][attenuation] */
    MAD_PLOT_DATA xTalk;		/* [Channel][other_channels (3)*crossTalk] */
    MAD_PLOT_DATA retLoss;		/* [Channel][ReturnLoss] */
    MAD_CABLELENGTH_DATA	cableLen;
    MAD_ETHCABLETYPE_DATA	ethCable;
    MAD_SPLIT_PAIR_DATA		splitPair;

} MAD_DSP_VCT_RESULT;




/**********************************************************************
* mdDSPvct
*
* DESCRIPTION:
*       This routine performs the DSP virtual cable test. 
*       The supported VCT are
*           Attenuation Vs. frequency,
*           accuracy cable length,
*           CrossTalk Vs. frequency,
*           Checking Ethernet cable type,
*           checking split pair in cable,
*           checking return loss.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - logical port number.
*       aVctType - DSP VCT type, which can be set by any combination.
*	    dataLength - Required result data length. It is limited by 
*                    MAD_MAX_PLOT_DATA
*
* OUTPUTS:
*       DSPVctResult - The data and status of DSP VCT.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_[ERROR_STATUS] - on error
*
* COMMENTS:
*    
*
***********************************************************************/
MAD_STATUS mdDSPvct
(
    IN  MAD_DEV				*dev,
    IN  MAD_LPORT			port,
    IN  MAD_U32				aVctType,  
    IN  MAD_32				dataLength,
   OUT  MAD_DSP_VCT_RESULT	*DSPVctResult
);

/**********************************************************************
* mdDSPvctAttenVsFreq
*
* DESCRIPTION:
*       This routine performs the DSP virtual cable test  
*       for Attenuation Vs. frequency. The output data is a list of plot
*       data, and derived from DFE coefficient
*       
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*	    plotDataLength - Required plot data length. It is limited by 
*              MAD_MAX_PLOT_DATA
*
* OUTPUTS:
*       attenVsFreq - A list plot data for attenuation vs. frequency.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_[ERROR_STATUS] - on error
*
* COMMENTS:
*
***********************************************************************/
MAD_STATUS mdDSPvctAttenuation
(
    IN  MAD_DEV			*dev,
    IN  MAD_LPORT		port,
    IN  MAD_32			plotDataLength,
    OUT MAD_PLOT_DATA	*attenuation
);

/**********************************************************************
* mdDSPvctCableLen
*
* DESCRIPTION:
*       This routine performs the DSP virtual cable test
*       for accuracy cable length. The cable length is derived from 
*       ECHO coefficient.
*       
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*
* OUTPUTS:
*       cableLen - cable length structure.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_[ERROR_STATUS] - on error
*
* COMMENTS:
*
***********************************************************************/
MAD_STATUS mdDSPvctCableLen
(
    IN  MAD_DEV*				dev,
    IN  MAD_LPORT				port,
    OUT MAD_CABLELENGTH_DATA	*cableLen
);

/**********************************************************************
* mdDSPvctCrossTalkVsFreq
*
* DESCRIPTION:
*       This routine performs the DSP virtual cable test 
*       for CrossTalk Vs. frequency. The output data is a list of plot 
*       data, and derived from NEXT coefficient
*       
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*	    plotDataLength - Required plot data length. It is limited by 
*              MAD_MAX_PLOT_DATA
*
* OUTPUTS:
*       xTalkVsFreq - A list plot data for crossTalk vs. frequency.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_[ERROR_STATUS] - on error
*
* COMMENTS:
*
************************************************************************/
MAD_STATUS mdDSPvctCrossTalk
(
    IN  MAD_DEV*        dev,
    IN  MAD_LPORT       port,
    IN  MAD_32          plotDataLength,
    OUT MAD_PLOT_DATA	*xTalk
);

/**********************************************************************
* mdDSPvctEthCableType
*
* DESCRIPTION:
*       This routine performs the DSP virtual cable test 
*       for Ethernet cable type. The type is derived from 
*       NEXT coefficient.
*       
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*
* OUTPUTS:
*       ethCableType - Ethernet cable type structure
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_[ERROR_STATUS] - on error
*
* COMMENTS:
*
***********************************************************************/
MAD_STATUS mdDSPvctEthCableType
(
    IN  MAD_DEV*				dev,
    IN  MAD_LPORT				port,
    OUT MAD_ETHCABLETYPE_DATA	*ethCable
);

/**********************************************************************
* mdDSPvctCheckSplitPair
*
* DESCRIPTION:
*       This routine performs the DSP virtual cable test 
*       for checking split pair in cable. The status are derived from 
*       NEXT coefficient.
*       
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*
* OUTPUTS:
*       splitPairStatus - Split pair status.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_[ERROR_STATUS] - on error
*
* COMMENTS:
*
***********************************************************************/
MAD_STATUS mdDSPvctSplitPair
(
    IN  MAD_DEV*				dev,
    IN  MAD_LPORT				port,
    OUT MAD_SPLIT_PAIR_DATA		*splitPair
);

/**********************************************************************
* mdDSPvctRetLoss
*
* DESCRIPTION:
*       This routine performs the DSP virtual cable test 
*       for checking return loss. The output data is a list of plot 
*       data, and derived from ECHO coefficient
*       
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*	    plotDataLength - Required plot data length. It is limited by 
*              MAD_MAX_PLOT_DATA
*
* OUTPUTS:
*       retLoss - A list of plot data for return loss vs. frequency.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_[ERROR_STATUS] - on error
*
* COMMENTS:
*
***********************************************************************/
MAD_STATUS mdDSPvctRetLoss
(
    IN  MAD_DEV*        dev,
    IN  MAD_LPORT       port,
    IN  MAD_32          plotDataLength,
    OUT MAD_PLOT_DATA  *retLoss
);

#endif /* MAD_DSP_VCT */
#ifdef __cplusplus
}
#endif

#endif /* __MADDSPVCTAPI_h */
