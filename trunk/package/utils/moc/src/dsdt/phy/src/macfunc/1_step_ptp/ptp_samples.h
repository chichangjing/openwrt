#include <mvlPtpCopyright.h>

/********************************************************************************
* ptp_api.h
*
* DESCRIPTION:
*       Definitions for New PTP control functions's
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#ifndef __PTP_SAMPLE_H
#define __PTP_SAMPLE_H
#ifdef __cplusplus
extern "C" {
#endif


/********************************************************************************
* ptp_api.h
* 
* DESCRIPTION:
*       API Functions of PTP in MacSec.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#include <msec_defines.h>
#include <ptp_1step.h>
#include "ptp_api.h"


/*******************************************************************************
* ptp_lut_set_udp
*
* DESCRIPTION:
*		Set configuration over UDP
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS  ptp_lut_set_udp (int port); 

/*******************************************************************************
* ptp_lut_clear_udp
*
* DESCRIPTION:
*		Reset configuration over UDP
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS  ptp_lut_clear_udp (int port);

/*******************************************************************************
* ptp_lut_config_TS_tag
*
* DESCRIPTION:
*		Set configuration sample 1 as following,
* - ingress: 
*      - PTPv2 and InsertTST(long)
* - egress:
*      - PTPv2 and UpdateTxTime
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_lut_config_TS_tag (int port);

/*******************************************************************************
* ptp_lut_config_transp
*
* DESCRIPTION:
*		Set configuration sample 2 as following,
* - ingress: 
*     - if PTPv2 & TransportSpecific == 0 => insert TS Tag (E=1, U=0, action=0x7, offsetType=0x1f)
*     - if PTPv2 & TransportSpecific == 1 => insert TS Tag (E=1, U=0, action=0x7, offsetType=0x1f), initialize piggyback
*     - if PTPv2 & TransportSpecific == 2 => insert TS Tag (E=0, U=0, action=0x7, offsetType=0x1f), enqueue timestamp (high queue)
*     - if PTPv2 & TransportSpecific == 3 => insert TS Tag (E=0, U=0, action=0x7, offsetType=0x1f), initialize piggyback
*     - else do nothing
* - egress:
*     - if PTPv2 & TransportSpecific == 0 => drop
*     - if PTPv2 & TransportSpecific == 1 => update residence time, enqueue timestamp (low queue)
*     - if PTPv2 & TransportSpecific == 2 => remove TS tag, update transmit time
*     - if PTPv2 & TransportSpecific == 3 => do nothing
*     - else do nothing
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_lut_config_transp (int port);

/*******************************************************************************
* ptp_lut_config_drop
*
* DESCRIPTION:
*		Set configuration sample 3 as following,
* - ingress: 
*      - if not PTPv2 and not PTPv1 => drop
*      - else do nothing
* - egress:
*      - PTPv2 and (MessageType == Sync or Delay_Req or Delay_Resp or Pdelay_Req or Pdelay_Resp) => update residence time
*      - else do nothing
*      - if PTPv2 or PTPv1 => drop
*      - else do nothing
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_lut_config_drop (int port);

/*******************************************************************************
* ptp_lut_config_enqueue
*
* DESCRIPTION:
*		Set configuration sample 6 as following,
* - ingress: 
*      - (PTPv2 or PTPv1) and FlagField == 0xabcd => enqueue timestamp (high)
*      - else enqueue timestamp (low)
* - egress:
*      - if TST and TST_action == 0x7 => remove TS tag, update transit time
*      - if TST => remove TS tag
*      - else do nothing
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_lut_config_enqueue (int  port);


/*******************************************************************************
* ptp_lut_config_piggyback
*
* DESCRIPTION:
*		Set configuration sample 9 as following,
* - ingress: 
*      - PTPv2 => initialize piggyback
*      - else do nothing
* - egress:
*      - do nothing
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_lut_config_piggyback (int port);

/*******************************************************************************
* ptp_lut_config_set_orig_ts
*
* DESCRIPTION:
*		Set configuration sample 9b as following,
* - ingress: 
*      - do nothing
* - egress:
*      - PTPv2 => initialize the OriginTimestamp field
*      - else do nothing
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_lut_config_set_orig_ts (int port);

/*******************************************************************************
* ptp_lut_config_eg_TS_queue_hi
*
* DESCRIPTION:
*		Set configuration sample 9c as following,
* - ingress: 
*      - do nothing
* - egress:
*      - PTPv2 => timestamp queue (high)
*      - else do nothing
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_lut_config_eg_TS_queue_hi (int port);

/*******************************************************************************
* ptp_lut_config_ing_enqueue_lo
*
* DESCRIPTION:
*		Set configuration sample 10 as following,
* - ingress: 
*      - PTPv2 => enqueue timestamp (low priority queue)
*      - else do nothing
* - egress:
*      - do nothing
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_lut_config_ing_enqueue_lo (int port);

/*******************************************************************************
* ptp_lut_config_eg_enqueue_lo
*
* DESCRIPTION:
*		Set configuration sample 10b as following,
* - ingress: 
*      - do nothing
* - egress:
*      - PTPv2 => enqueue timestamp (low priority queue)
*      - else do nothing
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_lut_config_eg_enqueue_lo (int port);
 
/*******************************************************************************
* ptp_lut_config_eg_enqueue_hi
*
* DESCRIPTION:
*		Set configuration sample 10c as following,
* - ingress: 
*      - do nothing
* - egress:
*      - PTPv2 => enqueue timestamp (High priority queue)
*      - else do nothing
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_lut_config_eg_enqueue_hi (int port);

/*******************************************************************************
* ptp_lut_config_ing_enqueue_hi
*
* DESCRIPTION:
*		Set configuration sample 11 as following,
* - ingress: 
*      - PTPv2 => enqueue timestamp (high priority queue)
*      - else do nothing
* - egress:
*      - do nothing
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_lut_config_ing_enqueue_hi (int port);

/*******************************************************************************
* ptp_lut_config_update_res_time
*
* DESCRIPTION:
*		Set configuration sample 12 as following,
* - ingress: 
*      - do nothing
* - egress:
*      - if PTPv2 => update residence time
*      - else do nothing
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_lut_config_update_res_time (int port);

/*******************************************************************************
* ptp_lut_config_TS_tag_piggyback
*
* DESCRIPTION:
*		Set configuration sample 16 as following,
* - ingress: 
*      - PTPv2 => insert TS Tag (E=0, U=0, action=0x7, offsetType=0x1f), initialize piggyback
*      - else do nothing
* - egress:
*      - PTPv2 => update residence time
*      - else do nothing
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_lut_config_TS_tag_piggyback (int port);

/*******************************************************************************
* ptp_lut_config_msg_type
*
* DESCRIPTION:
*		Set configuration sample 20 as following,
* - ingress: 
*      - PTPv2 and (MessageType == Sync or Delay_Req or Delay_Resp or Pdelay_Req or Pdelay_Resp) => insert TS Tag (E=1, U=0, action=0x7, offsetType=0x1f), initialize piggyback
*      - else do nothing
* - egress:
*      - PTPv2 and (MessageType == Sync or Delay_Req or Delay_Resp or Pdelay_Req or Pdelay_Resp) => update residence time
*      - else do nothing
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_lut_config_msg_type (int port);

/* ptp_1step_config initialize 1-step function based on ptp_cfg.
   return 0: success.
          1: no support.
		  -1: failure
*/
/*******************************************************************************
* ptp_1step_V2_L2_init
*
* DESCRIPTION:
*		Set configuration for PTP version 2, and over L2.
*
* INPUTS:
*       port - port number
*       ptp_cfg  - pointer to 1-step PTPconfiguration
*       numIngLut - number of ingress Lut
*       numEgLut - number of Egress Lut
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_1step_V2_L2_init(int port, PTP_1STEP_CFG *ptp_cfg, int numIngLut, int numEgLut);

/*******************************************************************************
* ptp_1step_V2_IP4_UDP_init
*
* DESCRIPTION:
*		Set configuration for PTP version 2, and over UDP/IP4.
*
* INPUTS:
*       port - port number
*       ptp_cfg  - pointer to 1-step PTPconfiguration
*       numIngLut - number of ingress Lut
*       numEgLut - number of Egress Lut
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_1step_V2_IP4_UDP_init(int port, PTP_1STEP_CFG *ptp_cfg, int numIngLut, int numEgLut);

/*******************************************************************************
* ptp_1step_config
*
* DESCRIPTION:
*		Set configuration for LUT
*
* INPUTS:
*       port - port number
*       ptp_cfg  - pointer to 1-step PTPconfiguration
*       numIngLut - number of ingress Lut
*       numEgLut - number of Egress Lut
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_1step_config(int port, PTP_1STEP_CFG *ptp_cfg, int numIngLut, int numEgLut);

/*******************************************************************************
* ptp_default_init
*
* DESCRIPTION:
*		Initialize new PTP block with default selection
*       That is to complete init_access, simple udata, default LUT setup
*
* INPUTS:
*		dev - point to device configuration 
*       port - port number
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_default_init (void * dev, int port);

/*******************************************************************************
* new_ptp_init
*
* DESCRIPTION:
*		Completely Initialize new PTP block
*
* INPUTS:
*		dev - point to device configuration 
*       port - port number
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS new_ptp_init (void * dev, int port);


/*******************************************************************************
* nptpWriteEgPortReg
*
* DESCRIPTION:
*       This routine write new PTP Egress Port register.
*
* INPUTS:
*		port - port number.
*       addr - NewPtp register addr.
*       data - data value to be written
*   
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS nptpWriteEgPortReg 
(
	IN	int		   port,
    IN  unsigned short    addr,
    IN  unsigned long    data
);

/*******************************************************************************
* nptpReadEgPortReg
*
* DESCRIPTION:
*       This routine read new Ptp Egress Port register.
*
* INPUTS:
*		port - port number.
*       addr - NewPtp register addr.
*   
* OUTPUTS:
*       data - read data value
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS nptpReadEgPortReg 
(
	IN	int		   port,
    IN  unsigned short   addr,
	OUT unsigned long   *data
);

/*******************************************************************************
* nptpWriteIngPortReg
*
* DESCRIPTION:
*       This routine write new PTP Ingress Port register.
*
* INPUTS:
*		port - port number.
*       addr - NewPtp register addr.
*       data - data value to be written
*   
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS nptpWriteIngPortReg 
(
	IN	int		   port,
    IN  unsigned short    addr,
    IN  unsigned long    data
);

/*******************************************************************************
* nptpReadIngPortReg
*
* DESCRIPTION:
*       This routine read new Ptp Ingress Port register.
*
* INPUTS:
*		port - port number.
*       addr - NewPtp register addr.
*   
* OUTPUTS:
*       data - read data value
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS nptpReadIngPortReg 
(
	IN	int		   port,
    IN  unsigned short   addr,
	OUT unsigned long   *data
);

/*******************************************************************************
* ptp_get_ts_tsq
*
* DESCRIPTION:
*       This routine gets Time stamp from TS Queue.
*
* INPUTS:
*	portnum - port number.
*       path  - NewPtp block number: igr or egr.
*       queue - NewPtp queue number: high or low.
*       seqId - sequence id of PTP frame .
*       msgId - message id of PTP frame .
*   
* OUTPUTS:
*       ts - NewPtp Time stamp data.
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on no ts or error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_get_ts_tsq 
(
  IN  int portnum, 
  IN  PTP_Blk path, 
  IN  PTP_1STEP_QUEUE_TYPE queue, 
  IN  unsigned short seqId, 
  IN  unsigned short msgId, 
  OUT Ptp1StepTsStruct *ts);

/* TAI / TOD */

/*******************************************************************************
* ptp_print_tai_tod_cfg_gen
*
* DESCRIPTION:
*		 Show TAI TOD cfg gen
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
/* Show TAI TOD cfg gen */
DSDT_STATUS ptp_print_tai_tod_cfg_gen 
(
   IN int port
);

/*******************************************************************************
* ptp_set_tai_clk_gen
*
* DESCRIPTION:
*		 Set TAI to generate external clock signal
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
/* Set TAI to generate external clock signal */
DSDT_STATUS ptp_set_tai_clk_gen 
(
  IN int port
);

/*******************************************************************************
* ptp_set_tai_clk_rec
*
* DESCRIPTION:
*		 Set TAI to update TOD based on external clock signal
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
/* Set TAI to update TOD based on external clock signal */
DSDT_STATUS ptp_set_tai_clk_rec 
(
  IN int port
);

/*******************************************************************************
* ptp_set_tai_clk_rec_adv
*
* DESCRIPTION:
*		 Set TAI to update TOD based and auto adjust frequency based on ext clock signal
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
/* Set TAI to update TOD based and auto adjust frequency based on ext clock signal */
DSDT_STATUS ptp_set_tai_clk_rec_adv 
(
  IN int port
);

/*******************************************************************************
* ptp_tai_trig_load_timer
*
* DESCRIPTION:
*		 Set TOD func trig to copy LOAD into NEXT RT 
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
/* Set TOD func trig to copy LOAD into NEXT RT */
DSDT_STATUS ptp_tai_trig_load_timer 
(
  IN int port
);

/*******************************************************************************
* ptp_print_tai_intr
*
* DESCRIPTION:
*		 Print TAI interrupts
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
/* Print TAI interrupts */
DSDT_STATUS ptp_print_tai_intr 
(
  IN int port
);

/*******************************************************************************
* ptp_set_drift_thr_cfg
*
* DESCRIPTION:
*		 Set drift threshold config
*
* INPUTS:
*       port - port number
*       minimal_drift - min drift in ns that triggers drift adj 
*       drift_thr - min drift in ns that causes interrupt to be invoked to CPU 
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
/* Set drift threshold config */
/* minimal_drift: min drift in ns that triggers drift adj */
/* drift_thr: min drift in ns that causes interrupt to be invoked to CPU */
DSDT_STATUS ptp_set_drift_thr_cfg
(
  IN int port,
  IN unsigned int minimal_drift,
  IN unsigned int drift_thr
);

/*******************************************************************************
* ptp_set_clock_cyc
*
* DESCRIPTION:
*		 Set clock cycle
*        Due to rtl incompleteness, cyc_ns need to be 0,  
*        and cyc_s needs to be 2 
*
* INPUTS:
*       port - port number
*       cyc_s  - clock cycle seconds
*       cyc_ns - clock cycle nanoseconds
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
/* Set clock cycle */
/* Due to rtl incompleteness, cyc_ns need to be 0,  */
/* and cyc_s needs to be 2 */
DSDT_STATUS ptp_set_clock_cyc
(
  IN int port,
  IN unsigned int cyc_s,
  IN unsigned int cyc_ns
);

/*******************************************************************************
* ptp_go_clock_gen
*
* DESCRIPTION:
*		 clock generation setup
*
* INPUTS:
*       port - port number
*       ntTime - next rise time
*       cyc - clock cycle
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
/* clock generation setup */
DSDT_STATUS ptp_go_clock_gen 
(
  IN int port,
  IN NPTP_TIME_STRUCT *ntTime,
  IN NPTP_TIME_STRUCT *cyc
);

/*******************************************************************************
* ptp_go_clock_rec
*
* DESCRIPTION:
*		 clock reception setup 
*
* INPUTS:
*       port - port number
*       todTime - TOD time
*       ntTime - next rise time
*       cyc - clock cycle
*       deley - clock propagation delay
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
/* clock reception setup */
DSDT_STATUS ptp_go_clock_rec 
(
  IN int port,
  IN NPTP_TIME_STRUCT *todTime,
  IN NPTP_TIME_STRUCT *ntTime,
  IN NPTP_TIME_STRUCT *cyc,
  IN unsigned int delay
);

/*******************************************************************************
* ptp_set_pclk_period
*
* DESCRIPTION:
*		 set pclk period
*
* INPUTS:
*       port - port number
*       period - pclk period
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
/* set pclk period */
DSDT_STATUS ptp_set_pclk_period 
(
  IN int port,
  IN int period
);

/* pclk period generated by 1548P Z1 ASIC is actually  */
/* 8ns more than pclk_cfg setting so write a pclk_cfg  */
/* setting that is 8ns less than what you want */

/*******************************************************************************
* ptp_set_pclk_freq
*
* DESCRIPTION:
*		 set pclk period to 10Mhz
*
* INPUTS:
*       port - port number
*       freq - pclk period 
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
/* set pclk period to 10Mhz */
DSDT_STATUS ptp_set_pclk_freq
(
  IN int port,
  IN int freq
);

/*******************************************************************************
* ptp_set_pclk_ppm
*
* DESCRIPTION:
*		 Set ppm
*
* INPUTS:
*       port - port number
*       ppm - 
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
/* Set drift adjustment */
DSDT_STATUS ptp_set_pclk_ppm 
(
  IN int port,
  IN int ppm
);

/*******************************************************************************
* ptp_get_tai_pulse_delay
*
* DESCRIPTION:
*		 Get pulse delay
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*       pdelay - pulse delay
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_get_tai_pulse_delay 
(
  IN int port,
  OUT unsigned int *pdelay
);

/*******************************************************************************
* ptp_enable_tai
*
* DESCRIPTION:
*		 Enable TAI
*
* INPUTS:
*      port - port number
*      kpTodCfg	- keep original TOD comfiguration
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_enable_tai 
(
  IN int port,
  IN unsigned int kpTodCfg
);

/*******************************************************************************
* ptp_disable_tai
*
* DESCRIPTION:
*		 Disable TAI
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*      kpTodCfg	- keep original TOD comfiguration
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_disable_tai 
(
  IN int port,
  OUT unsigned int *kpTodCfg
);

/*******************************************************************************
* ptp_set_tai
*
* DESCRIPTION:
*		 Set TAI TOD cfg 
*
* INPUTS:
*       port - port number
*       clk_mode - 
*       in_clk_en - 
*       tai_clkout_sel - 
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_set_tai 
(
  IN int port,
  IN int clk_mode,
  IN int in_clk_en,
  IN int tai_clkout_sel
);

/*******************************************************************************
* ptp_tai_set_pulse_width
*
* DESCRIPTION:
*		 Set pulse width
*
* INPUTS:
*       port - port number
*       width_ns - width nanoseconds
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_tai_set_pulse_width 
(
  IN int port,
  IN int width_ns
);

/*******************************************************************************
* ptp_tai_set_trig_gen_en
*
* DESCRIPTION:
*		 Set enable trig generator gen
*
* INPUTS:
*       port - port number
*       bitval - 
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_tai_set_trig_gen_en 
(
  IN int port,
  IN int bitval
);


/*******************************************************************************
* ptp_go_drift_adj
*
* DESCRIPTION:
*		 demo drift adjustment
*
* INPUTS:
*       port - port number
*       drift_frac_ns - port number
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
/* demo drift adjustment */
DSDT_STATUS ptp_go_drift_adj
(
  IN int port,
  IN unsigned int drift_frac_ns
);

/*******************************************************************************
* ptp_go_tod_update_workaround
*
* DESCRIPTION:
*		 TOD update workaround
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
/* TOD update workaround */
DSDT_STATUS ptp_go_tod_update_workaround (int port);

/* PTP Interrupt */
/*******************************************************************************
* get_ptp_intr_msk
*
* DESCRIPTION:
*		 Get new PTP interrupt mask.
*
* INPUTS:
*       port - port number
*       path  - NewPtp block number: igr or egr.
*
* OUTPUTS:
*       mask  - New PTP interrupt mask.
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS get_ptp_intr_msk ( int portnum, PTP_Blk path, unsigned int *mask);

/*******************************************************************************
* set_ptp_intr_msk
*
* DESCRIPTION:
*		 Set new PTP interrupt mask.
*
* INPUTS:
*       port - port number
*       path  - NewPtp block number: igr or egr.
*       mask  - New PTP interrupt mask.
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS set_ptp_intr_msk ( int portnum, PTP_Blk path, unsigned int mask);

/*******************************************************************************
* get_ptp_interrupts
*
* DESCRIPTION:
*		 Get new PTP interrupts.
*
* INPUTS:
*       portnum - port number
*
* OUTPUTS:
*		igrInt - IGR interupts
*		egrInt - EGR interupts
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS get_ptp_interrupts (int portnum, unsigned int *igrInt, unsigned int *egrInt);

/*******************************************************************************
* clear_ptp_interrupts
*
* DESCRIPTION:
*		 Clear new PTP interrupts.
*
* INPUTS:
*       portnum - port number
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS clear_ptp_interrupts ( int portnum);

/*******************************************************************************
* set_tai_intr_msk
*
* DESCRIPTION:
*		 Set new PTP TAI interrupt mask.
*
* INPUTS:
*       port - port number
*       mask  - New PTP TAI interrupt mask.
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS set_tai_intr_msk (int port, unsigned int mask);

/*******************************************************************************
* get_tai_intr_msk
*
* DESCRIPTION:
*		 Get new PTP TAI interrupt mask.
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*       mask  - New PTP TAI interrupt mask.
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS get_tai_intr_msk (int port, unsigned int *mask);

/*******************************************************************************
* ptp_global_intr_en
*
* DESCRIPTION:
*		 Set new PTP Global interrupt enable.
*
* INPUTS:
*       port - port number
*       en  - New PTP interrupt enable/disable.
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_global_intr_en ( int portnum, int en);

/*******************************************************************************
* set_top_intr_msk
*
* DESCRIPTION:
*		 Set new PTP top interrupt mask.
*
* INPUTS:
*       port - port number
*       mask  - New PTP top interrupt mask.
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS set_top_intr_msk (int port, unsigned int mask);

/*******************************************************************************
* get_top_intr_msk
*
* DESCRIPTION:
*		 Get new PTP top interrupt mask.
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*       mask  - New PTP top interrupt mask.
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS get_top_intr_msk (int port, unsigned int *mask);

/*******************************************************************************
* msec_get_intr_src
*
* DESCRIPTION:
*		 Get new PTP interrupt source.
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*       intSrc  - New PTP interrupt source.
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS msec_get_intr_src (int port, unsigned *intSrc);

/*******************************************************************************
* ptp_test_interrupt
*
* DESCRIPTION:
*		Test new PTP interrupt functions
*
* INPUTS:
*		dev - point to device configuration 
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_test_interrupt (void * dev, int port);

/*******************************************************************************
* ptp_disp_interrupt
*
* DESCRIPTION:
*		Get new PTP interrupt enable/disable status
*
* INPUTS:
*		dev - point to device configuration 
*		port - port number 
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_disp_interrupt (void * dev, int port);

/*******************************************************************************
* ptp_set_interrupt
*
* DESCRIPTION:
*		Set new PTP interrupt enable/disable
*
* INPUTS:
*		dev - point to device configuration 
*		port - port number 
*		en   - enable =1, disable = 0.
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_set_interrupt (void * dev, int port,int en);

/*******************************************************************************
* ptp_lut_config_udp_1731_ntp
*
* DESCRIPTION:
*		Set configuration sample of UDP, Y1731, and NTP,
* - ingress: 
*      - UDP do nothing
*      - Y1731 set Insert TST 
*      - NTP set Time stamplow
*
* - egress:
*      - do nothing
*
* INPUTS:
*		None
*
* OUTPUTS:
*		None
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_lut_config_udp_1731_ntp (int port);

extern DSDT_STATUS ntp_test_config (int port);
extern void ptp_get_tsql_drain(int port);
extern void ptp_get_tsqh_drain(int port);

#ifdef __cplusplus
}
#endif

#endif /* __PTP_SAMPLE_H */





