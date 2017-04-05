

            1 Step PTP API
           ================


Table of Content:
-----------------
1) Release History 
2) Source Code Organization
3) General Introduction
4) 1_step PTP APIs


1) Release History 
------------------
DSDT_3.3/src/macfunc/1_step_ptp - Dec. 4. 2012. 
          0. Added to support NTP.
DSDT_3.2x/src/macfunc/1_step_ptp - Nov. 23. 2011. 
          0. 1-step functions of 1548P.

2) Source Code Organization
--------------------------
  2.1) DSDT_3.3/src/macfunc/1_step_ptp
    1-step-ptp functions code and test programs.

  2.1) DSDT_3.3/phy/host
    1-step-ptp API application reference programs on different platforms.

3) General Introduction
-----------------------

The 1_step PTP is in Marvell Phy 1548P, 1680. The APIs are included in Marvell Phy driver (MAD).
Please first read <README_PHY.txt> to know how to use general PHY APIs, and how to access Phy registers.

In DSDT_3.3/src/macfunc/1_step_ptp,
<ptp_init.c> - Platform related functions to access 1step PTP registers.
<ptp_api.c> - Source code of API level.
<ptp_func.c> - Source code of sub functions of API.
<ptp_samples.c> - Source code of sample program of 1step PTP API.
Other related header and constants files.
In DSDT_3.3/phy/host,
<firebat/ptp1stepTestImpl.c> - Source code of reference program of 1step PTP API on Marvell Firebat system.
<vs/ptp1stepTest.c> - Source code of reference program of 1step PTP API on Windows Virtual Studio.
Above reference files are used as example files of 1step PTP application. Th programs are used in Marvell PTP Demo and test environment.

4) 1_step PTP APIs
-----------------
See Include <ptp_api.h> in detail.

For simple reference, following functions are listed. The functions are used in Marvell PTP test and Demo programs to support PTP application.
The application references in ptp_samples.c

A. 1step PTP initialization:
  <new_ptp_init> - Initialize 1_step functions in 1548P. It should be called first.

B. 1step PTP configuration:
  Wrapped programs in DSDT_3.3/phy/host:
  <ptp_1step_config_master> - Coiffure the 1548p port as PTP master device. 
    It can configure Ingress:
            a. 1 or 2 step.
            b. Piggyback, Ts Tag short, TS Tag long, TS queue.
            c. All messages for Master.
           Egress:
            a. 1 or 2 step.
            b. Inserted time stamp, Residence time, TS queue.
            c. All messages for Master. 
  <ptp_1step_config_slave> - Coiffure the 1548p port as PTP slave device.
    It can configure same as master configuration.


C. 1step PTP TAI trigger:
  <ptp_set_tai_clk_gen> - Set TAI to generate external clock signal
  <ptp_set_tai_clk_rec> - Set TAI to update TOD based on external clock signal

  TOD  TAI trigger related functions:
  <ptp_print_tai_tod_cfg_gen> - Show TAI TOD general configuration
  <ptp_set_tai_clk_mode> - Set clock mode: 1: clock generation,
                                           2: clock reception, 
                                           3: clock reception adv 
  <ptp_set_tai_synce_clk_sel> - Set port as recovered reference clock
  <ptp_disable_tai_clk_gen> - Disable TAI from generating external clock signal, keeping internal clock generation 
  <ptp_set_tai_clk_rec_adv> - Set TAI to update TOD based and auto adjust frequency based on ext clock signal


D. 1step PTP Clock adjustment:
  <ptp_set_drift_adj> - Simple clock adjust function. It set adjusted nanoseconds to <load timer> and adjusted sub-nanoseconds to drift adjust part.

  Clock adjust related functions:
  <ptp_tai_trig_load_timer> - 
  <get_ptp_tai_tod> - 
  <get_ptp_tai_frc> - 
  <set_ptp_tai_frc> - 
  <get_ptp_tai_step> - 
  <set_ptp_tai_step> - 
  <get_ptp_tai_drift_adjust> - 
  <set_ptp_tai_drift_adjust> - 
  <ptp_set_drift_adj> - 
  <ptp_get_drift_adj> - 

E. 1step PTP Time stamp:
  <ptp_get_ts_tsq> -

F. 1step PTP for NTP:
  <new_ntp_init> - Initialize 1_step NTP functions in 1548P, and 1680. 
                   It should be called first.
  <prgm_lut_ent_ntp> - Confige action in Lut entry.
  <prgm_lut_ent_ntp_list> - Confige multi actions in Lut entry
  <ptp_get_ntp_ts_tsq> - Get Timestamp in queue.