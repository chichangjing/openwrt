#include <mvlPtpCopyright.h> 

/********************************************************************************
* ptp_api.h
* 
* DESCRIPTION:
*       API Functions of PTP in Marvell chips.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __Ptp_Api_h
#define __Ptp_Api_h
#ifdef __cplusplus
extern "C" {
#endif

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
*       ptp_cfg  - pointer to 1-step PTP configuration
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
*       ptp_cfg  - pointer to 1-step PTP configuration
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
* ptp_1step_V2_IP6_UDP_init
*
* DESCRIPTION:
*		Set configuration for PTP version 2, and over UDP/IP6.
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
DSDT_STATUS ptp_1step_V2_IP6_UDP_init(int port, PTP_1STEP_CFG *ptp_cfg, int numIngLut, int numEgLut);

/*******************************************************************************
* ptp_1step_config
*
* DESCRIPTION:
*		General function to set configuration for LUT
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
* new_ptp_init
*
* DESCRIPTION:
*		Completely Initialize 1-step PTP block. It should be called bedore using any
*       new PTP function.
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
DSDT_STATUS new_ptp_init (void* dev, int port);

/*******************************************************************************
* new_ntp_init
*
* DESCRIPTION:
*		Completely Initialize 1-step NTP block. It should be called bedore using any
*       new NTP function.
*
* INPUTS:
*		dev - point to device configuration 
*       port - port number
*       udp_src_port - second NTP source UDP port number
*                      First UDP port is 123, that is default on NTP standard.
*       udp_dst_port - second NTP destination UDP port number
*                      First UDP port is 123, that is default on NTP standard.
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
DSDT_STATUS new_ntp_init (void* dev, int port, int udp_src_port_2, int udp_dst_port_2);

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
n*   
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

/*******************************************************************************
* ptp_get_ntp_ts_tsq
*
* DESCRIPTION:
*       This routine gets Time stamp from TS Queue for NTP.
*
* INPUTS:
*	portnum - port number.
*       path  - NewPtp block number: igr or egr.
*       queue - NewPtp queue number: high or low.
*   
* OUTPUTS:
*       numwords - number of TS elements in Queue.
*       ts - NewPtp Time stamp data.
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on no ts or error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_get_ntp_ts_tsq
(
  IN  int portnum, 
  IN  PTP_Blk path, 
  IN  PTP_1STEP_QUEUE_TYPE queue, 
  OUT int *numwords,
  OUT Ptp1StepTsStruct *ts);

/* TAI / TOD */

/*******************************************************************************
* ptp_print_tai_tod_cfg_gen
*
* DESCRIPTION:
*		 Show TAI TOD cfg general
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
* ptp_set_tai_clk_mode
*
* DESCRIPTION:
*   Set TAI TOD cfg general 
*   clk_mode: 1: clock generation, 2: clock reception, 3: clock reception adv 
*
* INPUTS:
*       port - port number
*       clk_mode - clk_mode: 
*          1: clock generation, 2: clock reception, 3: clock reception adv 
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
/* Set TAI TOD cfg gen */
/* clk_mode: 1: clock generation, 2: clock reception, 3: clock reception adv */
DSDT_STATUS ptp_set_tai_clk_mode
(
   IN int port, 
   IN int clk_mode
);

/*******************************************************************************
* ptp_set_tai_synce_clk_sel
*
* DESCRIPTION:
*   Set port as recovered reference clock
*
* INPUTS:
*       port - port number
*       refPort - selected port as recovered reference clock 
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
DSDT_STATUS ptp_set_tai_synce_clk_sel
(
   IN int port, 
   IN int refPort
);

/*******************************************************************************
* ptp_disable_tai_clk_gen
*
* DESCRIPTION:
     Disable TAI from generating external clock signal, 
*    keeping internal clock generation 
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
/* Disable TAI from generating external clock signal, keeping internal clock generation */
DSDT_STATUS ptp_disable_tai_clk_gen 
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
* ptp_tai_get_timer_operation
*
* DESCRIPTION:
*		 Get tod function timer operation
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*       timerOp - timer operation
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_tai_get_timer_operation 
(
  IN int port,
  OUT TOD_FUNC_TIMER_OP *timerOp
);

/*******************************************************************************
* ptp_tai_set_timer_operation
*
* DESCRIPTION:
*		 Set tod function timer operation
*
* INPUTS:
*       port - port number
*       timerOp - timer operation
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
DSDT_STATUS ptp_tai_set_timer_operation 
(
  IN int port,
  IN TOD_FUNC_TIMER_OP timerOp
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
* ptp_tai_get_pulse_width
*
* DESCRIPTION:
*		 Get pulse width
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*       width_ns - width nanoseconds
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
DSDT_STATUS ptp_tai_get_pulse_width 
(
  IN int port,
  OUT int *width_ns
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
* ptp_go_pulse_gen
*
* DESCRIPTION:
*		 pulse generation setup (master)
*
* INPUTS:
*       port - port number
*       width_ns - width nanoseconds
*       trigsec - trigger time seconds
*       trignsec - trigger time nenoseconds
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
/* pulse generation setup (master) */
DSDT_STATUS ptp_go_pulse_gen (int port, int width_ns, int trigsec, int trignsec);

/*******************************************************************************
* ptp_get_pulse_gen
*
* DESCRIPTION:
*        Get pulse generation setup (master)
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*       width_ns - width nanoseconds
*       trigsec - trigger time seconds
*       trignsec - trigger time nenoseconds
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
/* get pulse generation setup (master) */
DSDT_STATUS ptp_get_pulse_gen (int port, int*width_ns, unsigned int *trigsec, unsigned int *trignsec);

/*******************************************************************************
* ptp_go_pulse_rec
*
* DESCRIPTION:
*		 pulse reception setup (slave) 
*
* INPUTS:
*       port - port number
*       delay - Pulse delay time 
*       nextTime - Next Rise time 
*       ldTime - load time 
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
/* pulse reception setup (slave) */
DSDT_STATUS ptp_go_pulse_rec (int port, int delay, PtpTimeStruct *nextTime, PtpTimeStruct * ldTime);

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
* disable_ptp_watchdog_timer
*
* DESCRIPTION:
*		 Set new PTP ptp_watchdog_timer disable.
*
* INPUTS:
*       port - port number
*       path  - NewPtp block number: igr or egr.
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
DSDT_STATUS disable_ptp_watchdog_timer ( int portnum, PTP_Blk path);

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
* set_ptp_tai_tod
*
* DESCRIPTION:
*       Set TOD
*
* INPUTS:
*       port - port number
*       ns - nanoseconds of TOD 
*       sec_lo - low seconds of TOD
*       sec_hi - high seconds of TOD
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
/* set TOD */
DSDT_STATUS set_ptp_tai_tod
(
  IN int port,
  IN unsigned int ns,
  IN unsigned int sec_lo,
  IN unsigned int sec_hi 
);

/*******************************************************************************
* ptp_set_drift_adj
*
* DESCRIPTION:
*       Clock drift adjustment
*
* INPUTS:
*       port - port number
*       drift_base_adj_sl - base drift adjust low sec
*       drift_base_adj_ns - base drift adjust ns (invlude sign)
*       drift_base_adj_fs - base drift adjust fs
*       drift_fine_adj_fs - fine drift adjust fs
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
/* set drift adjustment */
DSDT_STATUS ptp_set_drift_adj
(
  IN int port,
  IN int drift_base_adj_sl,
  IN int drift_base_adj_ns,
  IN int drift_base_adj_fs,
  IN int drift_fine_adj_fs
);

/*******************************************************************************
* ptp_get_drift_adj
*
* DESCRIPTION:
*	Get clock drift adjustment
*
* INPUTS:
*       port - port number
*
* OUTPUTS:
*       drift_base_adj_sl - base drift adjust low sec
*       drift_base_adj_ns - base drift adjust ns (invlude sign)
*       drift_base_adj_fs - base drift adjust fs
*       drift_fine_adj_fs - fine drift adjust fs
*
* RETURNS:
*       DSDT_OK          - on success
*       DSDT_FAIL        - on error
*
* COMMENTS:
*
*******************************************************************************/
/* get drift adjustment */
DSDT_STATUS ptp_get_drift_adj
(
  IN int port,
  OUT int *drift_base_adj_sl,
  OUT int *drift_base_adj_ns,
  OUT int *drift_base_adj_fs,
  OUT int *drift_fine_adj_fs
);

/*******************************************************************************
* prgm_ptp_lut_ent_msgType
*
* DESCRIPTION:
*		 program an lut entry that matches on specified msgtype and actions
*        msgid: 0 - Sync, 1 - DelayReq, 2 - PdelayReq, 3 - PdelayResp
*               8 - FollowUp, 9 - DelayResp, A - PdelayRespFU, 
*               B - Announce, C - Signaling, D - Management
*               multiple actions are defined in a list 
*
* INPUTS:
*       port - port number
*       idx  - Lut entry number.
*       msgId - PTP Message Id.
*       actions - Lut actions.
*       path - PTP_igr or PTP_egr.
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
DSDT_STATUS prgm_ptp_lut_ent_msgType(int port, int idx, PTPv2_message_type msgId, PTP_1STEP_LUT_ACT actions, PTP_Blk path);

/*******************************************************************************
* prgm_ptp_lut_ent_domain_num
*
* DESCRIPTION:
*     program an lut entry that matches on specified domain number and actions
*     domain_num: domain number used as lut key
*     Action abreviation
*     DR - drop, INI - IniPiggyback, INS - InsertTST
*     SH - StampHigh, AA - AddAsymmetry, NO - no action
*     multiple actions are defined in a list 
*
* INPUTS:
*       port - port number
*       idx  - Lut entry number.
*       domain_num - PTP domain number.
*       actions - Lut actions.
*       path - PTP_igr or PTP_egr.
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
DSDT_STATUS prgm_ptp_lut_ent_domain_num(int port, int idx, int domain_num, PTP_1STEP_LUT_ACT actions, PTP_Blk path);

/*******************************************************************************
* prgm_ptp_lut_ent_transspec
*
* DESCRIPTION:
*		 program an lut entry that matches on specified transspec and actions
*        multiple actions are defined in a list 
*
* INPUTS:
*       port - port number
*       idx  - Lut entry number.
*       transspec - PTP transspec.
*       actions - Lut actions.
*       path - PTP_igr or PTP_egr.
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
DSDT_STATUS prgm_ptp_lut_ent_transspec(int port, int idx, int transspec, PTP_1STEP_LUT_ACT actions, PTP_Blk path);

/*******************************************************************************
* prgm_lut_ent_ipv4
*
* DESCRIPTION:
*		 program an lut entry that matches IPV4 and actions
*               multiple actions are defined in a list 
*
* INPUTS:
*       port - port number
*       idx  - Lut entry number.
*       actions - Lut actions.
*       path - PTP_igr or PTP_egr.
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
DSDT_STATUS prgm_lut_ent_ipv4(int port, int idx, PTP_1STEP_LUT_ACT actions, PTP_Blk path);

/*******************************************************************************
* prgm_lut_ent_ntp
*
* DESCRIPTION:
*		 program an lut entry that matches NTP and actions
*
* INPUTS:
*       port - port number
*       idx  - Lut entry number.
*       actions - Lut actions.
*       path - PTP_igr or PTP_egr.
*       udp_port - UDP port
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
DSDT_STATUS prgm_lut_ent_ntp(int port, int idx, PTP_1STEP_LUT_ACT actions, PTP_Blk path, int udp_port);

/*******************************************************************************
* prgm_lut_ent_ntp_list
*
* DESCRIPTION:
*		 program an lut entry that matches NTP and actions
*               multiple actions are defined in a list 
*
* INPUTS:
*       port - port number
*       idx  - Lut entry number.
*       actions - List of Lut actions. The last action is PTP_Act_No_Action.
*       path - PTP_igr or PTP_egr.
*       udp_port - UDP port
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
DSDT_STATUS prgm_lut_ent_ntp_list(int port, int idx, PTP_1STEP_LUT_ACT *actions, PTP_Blk path, int udp_port);

/*******************************************************************************
* prgm_lut_ent_ptpv2
*
* DESCRIPTION:
*		 program an lut entry that matches PTPv2 and actions
*               multiple actions are defined in a list 
*
* INPUTS:
*       port - port number
*       idx  - Lut entry number.
*       actions - Lut actions.
*       path - PTP_igr or PTP_egr.
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
DSDT_STATUS prgm_lut_ent_ptpv2(int port, int idx, PTP_1STEP_LUT_ACT actions, PTP_Blk path);

/*******************************************************************************
* prgm_lut_ent_ptpv1
*
* DESCRIPTION:
*		 program an lut entry that matches PTPv1 and actions
*               multiple actions are defined in a list 
*
* INPUTS:
*       port - port number
*       idx  - Lut entry number.
*       actions - Lut actions.
*       path - PTP_igr or PTP_egr.
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
DSDT_STATUS prgm_lut_ent_ptpv1(int port, int idx, PTP_1STEP_LUT_ACT actions, PTP_Blk path);

/* use the IP-in-IP for Y1731 */
/*******************************************************************************
* prgm_lut_ent_1731
*
* DESCRIPTION:
*		 program an lut entry that matches Y1731 and actions
*               multiple actions are defined in a list 
*
* INPUTS:
*       port - port number
*       idx  - Lut entry number.
*       actions - Lut actions.
*       path - PTP_igr or PTP_egr.
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
DSDT_STATUS prgm_lut_ent_1731(int port, int idx, PTP_1STEP_LUT_ACT actions, PTP_Blk path);

/*******************************************************************************
* prgm_lut_ent_1731_opcode
*
* DESCRIPTION:
*		 program an lut entry that matches the IP-in-IP for Y1731, Version PTP for opcode and actions 
*        multiple actions are defined in a list 
*
* INPUTS:
*       port - port number
*       idx  - Lut entry number.
*       opcode - opcode on Y1731, Version PTP.
*       actions - Lut actions.
*       path - PTP_igr or PTP_egr.
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
DSDT_STATUS prgm_lut_ent_1731_opcode(int port, int idx, int opcode, PTP_1STEP_LUT_ACT actions, PTP_Blk path);

/*******************************************************************************
* prgm_lut_ent_stag
*
* DESCRIPTION:
*		 program an lut entry that matches stag and actions
*        multiple actions are defined in a list 
*
* INPUTS:
*       port - port number
*       idx  - Lut entry number.
*       actions - Lut actions.
*       path - PTP_igr or PTP_egr.
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
DSDT_STATUS prgm_lut_ent_stag(int port, int idx, PTP_1STEP_LUT_ACT actions, PTP_Blk path);

/*******************************************************************************
* prgm_lut_ent_always0
*
* DESCRIPTION:
*		 program an lut entry that do nothing
*
* INPUTS:
*       port - port number
*       idx  - Lut entry number.
*       actions - Lut actions.
*       path - PTP_igr or PTP_egr.
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
DSDT_STATUS prgm_lut_ent_always0(int port, int idx, PTP_1STEP_LUT_ACT actions, PTP_Blk path);

#ifdef __cplusplus
}
#endif

#endif /* __Ptp_Api_h */






