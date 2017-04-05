#include <madCopyright.h>

/***********************************************************************
* madDSPVct.c
*
* DESCRIPTION:
*       Functions for Advanced VCT with GDSP Testing on 
*       Marvell Phy Device
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*
************************************************************************/
#ifndef __MADDSPVCT_h
#define __MADDSPVCT_h
#ifdef __cplusplus
extern "C" {
#endif


#ifdef MAD_DSP_VCT


/*************************************************************************************/
/*   madWriteAFE                                                                                */
/*************************************************************************************/
extern MAD_STATUS madWriteAFE(
	IN  MAD_DEV     *dev,
    IN  MAD_U8      hwPort,
    IN  MAD_32      reg, 
	IN  MAD_32      data
);

/*************************************************************************************/
/*   madReadAFE                                                                                */
/*************************************************************************************/
extern MAD_STATUS madReadAFE(
	IN  MAD_DEV     *dev,
    IN  MAD_U8      hwPort,
    IN  MAD_32      reg, 
	IN  MAD_32      *data
);

/*************************************************************************************/
/*   madWriteGDSP                                                                                */
/*************************************************************************************/
extern MAD_STATUS madWriteGDSP(
	IN  MAD_DEV     *dev,
    IN  MAD_U8      hwPort,
    IN  MAD_32      reg, 
	IN  MAD_32      data
);

/*************************************************************************************/
/*   madWriteOnlyGDSP                                                                                */
/*************************************************************************************/
extern MAD_STATUS madWriteOnlyGDSP(
	IN  MAD_DEV     *dev,
    IN  MAD_U8      hwPort,
    IN  MAD_32      reg, 
	IN  MAD_32      data
);

/*************************************************************************************/
/*    madReadGDSP                                                                               */
/*************************************************************************************/
extern MAD_STATUS madReadGDSP(
	IN  MAD_DEV     *dev,
    IN  MAD_U8      hwPort,
    IN  MAD_32      reg, 
	IN  MAD_32      *data
);

/*************************************************************************************/
/*   madWriteOnlyEN                                                                                */
/*************************************************************************************/
extern MAD_STATUS madWriteOnlyEN(
	IN  MAD_DEV     *dev,
    IN  MAD_U8      hwPort,
    IN  MAD_32      reg, 
	IN  MAD_32          data
);

/*************************************************************************************/
/*    madWriteEN                                                                               */
/*************************************************************************************/

extern MAD_STATUS madWriteEN(
	IN  MAD_DEV     *dev,
    IN  MAD_U8      hwPort,
    IN  MAD_32      reg, 
	IN  MAD_32          data
);

/*************************************************************************************/
/*   madReadEN                                                                                */
/*************************************************************************************/
extern MAD_STATUS madReadEN(
	IN  MAD_DEV     *dev,
    IN  MAD_U8      hwPort,
    IN  MAD_32      reg, 
	IN  MAD_32      *data
);

extern void madDelay (MAD_32 count);
extern MAD_STATUS madCheck_1181_pwrup(MAD_DEV *dev, MAD_U8 port);
extern void madClearPlotData (MAD_PLOT_DATA * plotData );
extern void  madFFT_func(float data[], MAD_32 nn, MAD_32 isign);

struct MAD_Complex 
{
 float x;
 float y;
};

#define MAD_E1149_ECHO_NUM 96
#define MAD_E1149_NEXT_NUM 60
#define MAD_NEXT_NUM MAD_E1149_NEXT_NUM 
#define MAD_FFT_minIdx	64
#define MAD_E1149_DFSE_NUM 8
#define MAD_FFE_NUM 5

extern float madE1149EchoFile [MAD_CHANNEL_NUM][MAD_E1149_ECHO_NUM+4];
extern float madE1149NextFile[MAD_CHANNEL_NUM] [MAD_E1149_NEXT_NUM+4];
extern float madE1149FFEFile [MAD_CHANNEL_NUM][MAD_FFT_minIdx+4];
extern float madE1149DFSEFile [MAD_CHANNEL_NUM][MAD_FFT_minIdx+4];
extern float madE1149GainFile [MAD_CHANNEL_NUM][MAD_FFT_minIdx+4];

extern float  madCrosstalk_Files[MAD_CHANNEL_NUM][3*128+4];
extern float  madCrosstalk_ieee[64+4];

extern float  madRloss_Files[MAD_CHANNEL_NUM][128+4];
extern float  madRloss_ieee[64+4];
extern float madIloss_Files[MAD_CHANNEL_NUM][MAD_FFT_minIdx+4];
extern float madIloss_ieee[MAD_FFT_minIdx+4];

/* lpf_h_coeff_re  */
extern float mad_coeff_Files_0[];
/* lpf_h_coeff_im */
extern float mad_coeff_Files_1[];
/* pr_h_coeff_re */
extern float mad_coeff_Files_2[];
/* pr_h_coeff_im */
extern float mad_coeff_Files_3[];
/* lpf_h_coeff2_re */
extern float mad_coeff2_Files_0[];
/* lpf_h_coeff2_im */
extern float mad_coeff2_Files_1[];
/* pr_h_coeff2_re */
extern float mad_coeff2_Files_2[];
/* pr_h_coeff2_im */
extern float mad_coeff2_Files_3[];


extern void  madCnvToComplex(float data[], struct MAD_Complex freq_ffe[], MAD_32 len);
extern float madCmplxAmplitude (struct MAD_Complex cmplx );
extern void  madCmplxAddCmplx(struct MAD_Complex v1, struct MAD_Complex v2, struct MAD_Complex *v3);
extern void  madCmplxAddFlt(struct MAD_Complex v1, float d, struct MAD_Complex *v3);
extern void  madCmplxSubCmplx(struct MAD_Complex v1, struct MAD_Complex v2, struct MAD_Complex *v3);
extern void  madCmplxSubFlt(struct MAD_Complex v1, float d, struct MAD_Complex *v3);
extern void  madCmplxMulCmplx(struct MAD_Complex v1, struct MAD_Complex v2, struct MAD_Complex *v3);
extern void  madCmplxMulFlt(struct MAD_Complex v1, float d, struct MAD_Complex *v3);
extern void  madCmplxDivCmplx(struct MAD_Complex v1, struct MAD_Complex v2, struct MAD_Complex *v3);
extern void  madCmplxDivFlt(struct MAD_Complex v1, float d, struct MAD_Complex *v3);

extern MAD_STATUS madE1149_Link_nFRC(MAD_DEV *dev, MAD_U8 hwPort);
extern MAD_STATUS madE1149_EN(MAD_DEV *dev, MAD_U8 hwPort);
extern MAD_STATUS madE1149_FFE(MAD_DEV *dev, MAD_U8 hwPort);
extern MAD_STATUS madE1149_DFSE(MAD_DEV *dev, MAD_U8 hwPort);
extern MAD_STATUS madE1149_Gain(MAD_DEV *dev, MAD_U8 hwPort, float gain[]);
extern MAD_STATUS madReadNextOnly(MAD_DEV *dev, MAD_U8 hwPort);
extern MAD_STATUS madReadEchoOnly(MAD_DEV *dev, MAD_U8 hwPort);

extern void madLoad_PR_LPF_coeff(struct MAD_Complex lpf_h[], struct MAD_Complex pr_h[]);
extern void madLoad_PR_LPF_coeff_2(struct MAD_Complex lpf_h[], struct MAD_Complex pr_h[]);

extern MAD_STATUS madSet_SlaveSilent1149(MAD_DEV *dev, MAD_LPORT hwPort);
extern MAD_STATUS madUnset_SlaveSilent1149(MAD_DEV *dev, MAD_LPORT hwPort); 

extern void madImplDSPVctInitData ( void);

extern void madInterp8(float din[], float dout[], MAD_32 din_len);
extern void madSq_and_log10(float data[], MAD_32 len);
extern float madSearch_peak(float data[], MAD_32 len, MAD_32 *idx_1stpk);
extern float madFind_max_threshold(MAD_DEV *dev, float data[], MAD_32 len, MAD_32 idx_1stpk, MAD_32 *idx_2ndpk);

extern MAD_STATUS madImplDSPVctAttenuation(MAD_DEV *dev, MAD_U8 hwPort, MAD_PLOT_DATA *attenuation);
extern MAD_STATUS madImplDSPVctCableLen(MAD_DEV *dev, MAD_U8 hwPort, MAD_CABLELENGTH_DATA *cableLen);
extern MAD_STATUS madImplDSPVctXTalk(MAD_DEV *dev, MAD_U8 hwPort, MAD_PLOT_DATA *XTalk);
extern MAD_STATUS madImplDSPVctEthCableType(MAD_DEV *dev, MAD_U8 hwPort, MAD_ETHCABLETYPE_DATA *ethCable);
extern MAD_STATUS madImplDSPVctSplitPair(MAD_DEV *dev, MAD_U8 hwPort, MAD_SPLIT_PAIR_DATA *splitPair);
extern MAD_STATUS madImplDSPVctRetLoss(MAD_DEV *dev, MAD_U8 hwPort, MAD_PLOT_DATA *retLoss);
extern MAD_STATUS madRunTDRTest(MAD_DEV *dev, MAD_U8 port, MAD_32 TDR_Status[], MAD_32 TDR_Length[]);

#ifdef MAD_MVL_MATH
#include "mathf.h"
#define pow mvl_powf
#define sin  mvl_sinf
#define log10  mvl_log10f
#define sqrt  mvl_sqrtf
#else
#include <math.h>
#define pow pow
#define sin  sin
#define log10  log10
#define sqrt  sqrt
#endif


#endif /* MAD_DSP_VCT */

#ifdef __cplusplus
}
#endif


#endif /* __MADDSPVCT_h */


/********************************************************************/





