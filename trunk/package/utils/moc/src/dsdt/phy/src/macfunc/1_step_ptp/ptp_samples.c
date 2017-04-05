#include <mvlPtpCopyright.h>

/********************************************************************************
* ptp_samples.c
* 
* DESCRIPTION:
*       Samples of PTP in MacSec.
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
#include "ptp_samples.h"



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
DSDT_STATUS ptp_lut_config_TS_tag (int port)
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_upper=0;
  unsigned long lut_vld_upper=0;
  unsigned long long lut_act; 
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_InsertTST, 1) | 
                    lut_act_adjust(PTP_Act_TST_E, 0) | 
                    lut_act_adjust(PTP_Act_TST_action, 0x7) | 
                    lut_act_adjust(PTP_Act_StampLow, 0x1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1);

  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 0, VN_IG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write igr lut entry 0 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
  /*  Default => do nothing */
  lut_key = lut_key_adjust( K_AlwaysZero, 0);
  lut_vld = lut_vld_adjust(K_AlwaysZero);
  lut_act = 0x0;
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 15, VN_IG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write igr lut entry 15: key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
 

  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_UpdateTxTime, 1) |
            lut_act_adjust(PTP_Act_StampHigh, 0x1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1);
  lut_key = lut_key | lut_key_adjust( K_TransportSpecific, 0x2);
  lut_vld = lut_vld | lut_vld_adjust(K_TransportSpecific);

  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 0, VN_EG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write egr lut entry 0: key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));


  /*  Default => do nothing */
  lut_key = lut_key_adjust( K_AlwaysZero, 0);
  lut_vld = lut_vld_adjust(K_AlwaysZero);
  lut_act = 0x0;
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 15, VN_EG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write egr lut entry 15: key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
/*   enable_ptp(); */
  return retVal;
}

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
DSDT_STATUS ptp_lut_config_transp (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_upper=0;
  unsigned long lut_vld_upper=0;
  unsigned long long lut_act; 
/*   disable_ptp(); */

  /*  Ingress */
  /*  PTPv2 & TransportSpecific == 0 => InsertTST(long) */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_InsertTST, 1) | 
                    lut_act_adjust(PTP_Act_TST_E, 1) | 
                    lut_act_adjust(PTP_Act_TST_action, 0x7) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1);
  lut_key = lut_key | lut_key_adjust( K_TransportSpecific, 0x0);
  lut_vld = lut_vld | lut_vld_adjust(K_TransportSpecific);

  prgm_lut_entry(port,  lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 0, VN_IG0_PTP+PTP_PORT_OFF(port)+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write igr lut entry 0 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));

  /*  PTPv2 & TransportSpecific == 1 => InsertTST(long) & IniPiggyback */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_InsertTST, 1) | 
                    lut_act_adjust(PTP_Act_TST_E, 1) | 
                    lut_act_adjust(PTP_Act_TST_action, 0x7) | 
                    lut_act_adjust(PTP_Act_IniPiggyback, 0x1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1);
  lut_key = lut_key | lut_key_adjust( K_TransportSpecific, 0x1);
  lut_vld = lut_vld | lut_vld_adjust(K_TransportSpecific);

  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 1, VN_IG0_PTP+PTP_PORT_OFF(port)+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write igr lut entry 1 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
  /*  PTPv2 & TransportSpecific == 2 => InsertTST(short) & StampHigh */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_InsertTST, 1) | 
                    lut_act_adjust(PTP_Act_TST_E, 0) | 
                    lut_act_adjust(PTP_Act_TST_action, 0x7) | 
                    lut_act_adjust(PTP_Act_StampHigh, 0x1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1);
  lut_key = lut_key | lut_key_adjust( K_TransportSpecific, 0x2);
  lut_vld = lut_vld | lut_vld_adjust(K_TransportSpecific);

  prgm_lut_entry(port,  lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 2, VN_IG0_PTP+PTP_PORT_OFF(port)+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write igr lut entry 2 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
  /*  PTPv2 & TransportSpecific == 3 => InsertTST(short) & IniPiggyback */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_InsertTST, 1) | 
                    lut_act_adjust(PTP_Act_TST_E, 0) | 
                    lut_act_adjust(PTP_Act_TST_action, 0x7) | 
                    lut_act_adjust(PTP_Act_IniPiggyback, 0x1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1);
  lut_key = lut_key | lut_key_adjust( K_TransportSpecific, 0x3);
  lut_vld = lut_vld | lut_vld_adjust(K_TransportSpecific);

  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 3, VN_IG0_PTP+PTP_PORT_OFF(port)+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write igr lut entry 3 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
  /*  Default => do nothing */
  lut_key = lut_key_adjust( K_AlwaysZero, 0);
  lut_vld = lut_vld_adjust(K_AlwaysZero);
  lut_act = 0x0;
  prgm_lut_entry(port,  lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 15, VN_IG0_PTP+PTP_PORT_OFF(port)+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write igr lut entry 15: key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));

  /*  Egress */
  /*  PTPv2 & TransportSpecific == 0 => Drop */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_Drop, 1);
  lut_key = lut_key | lut_key_adjust( K_TransportSpecific, 0x0);
  lut_vld = lut_vld | lut_vld_adjust(K_TransportSpecific);
  prgm_lut_entry(port,  lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 0, VN_EG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write egr lut entry 0 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
  /*  PTPv2 & TransportSpecific == 1 => UpdateResidence & StampLow */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_UpdateResidence, 1) |
            lut_act_adjust(PTP_Act_StampLow, 1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1);
  lut_key = lut_key | lut_key_adjust( K_TransportSpecific, 0x1);
  lut_vld = lut_vld | lut_vld_adjust(K_TransportSpecific);
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 1, VN_EG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write egr lut entry 1 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
  /*  PTPv2 & TransportSpecific == 2 => UpdateTxTime & RemoveTST */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_UpdateTxTime, 1) |
            lut_act_adjust(PTP_Act_RemoveTST, 1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1);
  lut_key = lut_key | lut_key_adjust( K_TransportSpecific, 0x2);
  lut_vld = lut_vld | lut_vld_adjust(K_TransportSpecific);
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 2, VN_EG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write egr lut entry 2 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
  /*  PTPv2 & TransportSpecific == 3 => do nothing */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = 0;
  lut_key = lut_key | lut_key_adjust( K_TransportSpecific, 0x3);
  lut_vld = lut_vld | lut_vld_adjust(K_TransportSpecific);
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 3, VN_EG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write egr lut entry 0 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
  /*  Default => do nothing */
  lut_key = lut_key_adjust( K_AlwaysZero, 0);
  lut_vld = lut_vld_adjust(K_AlwaysZero);
  lut_act = 0x0;
  prgm_lut_entry(port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 15, VN_EG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write egr lut entry 15: key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
/*   enable_ptp(); */
  return retVal;
}

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
DSDT_STATUS ptp_lut_config_drop (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_upper=0;
  unsigned long lut_vld_upper=0;
  unsigned long long lut_act; 
/*   disable_ptp(); */
 /*  Ingress */
  /*  !PTPv2 && !PTPv1 => drop */
 lut_key = lut_key_adjust( K_FlagPTPv2, 0);
 lut_vld = lut_vld_adjust(K_FlagPTPv2);
 lut_act = lut_act_adjust(PTP_Act_Drop, 1);
  lut_key = lut_key | lut_key_adjust( K_FlagPTPv1, 0x0);
  lut_vld = lut_vld | lut_vld_adjust(K_FlagPTPv1);
  prgm_lut_entry(port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 0, VN_IG0_PTP+PTP_PORT_OFF(port)+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write igr lut entry 0 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
  lut_key = lut_key_adjust( K_AlwaysZero, 0x0);
  lut_vld = lut_vld_adjust(K_AlwaysZero);
  lut_act = 0;
  prgm_lut_entry(port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 15, VN_IG0_PTP+PTP_PORT_OFF(port)+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write igr lut entry 0 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));


  /*  Egress */
  /*  PTPv2 => drop */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_Drop, 1);
  prgm_lut_entry(port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 0, VN_EG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write egr lut entry 0 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
  /*  PTPv1 => drop */
  lut_key = lut_key_adjust( K_FlagPTPv1, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv1);
  lut_act = lut_act_adjust(PTP_Act_Drop, 1);
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 1, VN_EG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write egr lut entry 0 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
  /*  Default => do nothing */
  lut_key = lut_key_adjust( K_AlwaysZero, 0);
  lut_vld = lut_vld_adjust(K_AlwaysZero);
  lut_act = 0x0;
  prgm_lut_entry( port,lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 15, VN_EG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write egr lut entry 15: key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
/*   enable_ptp(); */
  return retVal;
}

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
DSDT_STATUS ptp_lut_config_enqueue (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_upper=0;
  unsigned long lut_vld_upper=0;
  unsigned long long lut_act; 
/*   disable_ptp(); */

  /*  Ingress */
  /*  PTPv2 && FlagField==0xabcd  => StampHigh */
   lut_key = lut_key_adjust( K_FlagPTPv2, 1);
   lut_vld = lut_vld_adjust(K_FlagPTPv2);
   lut_act = lut_act_adjust(PTP_Act_StampHigh, 1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1);
  lut_key = lut_key | lut_key_adjust( K_FlagField, 0xabcd);
  lut_vld = lut_vld | lut_vld_adjust(K_FlagField);

  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 0, VN_IG0_PTP+PTP_PORT_OFF(port)+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write igr lut entry 0 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));

  /*  PTPv1 && FlagField==0xabcd  => StampHigh */
  lut_key = lut_key_adjust( K_FlagPTPv1, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv1);
  lut_act = lut_act_adjust(PTP_Act_StampHigh, 1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1);
  lut_key = lut_key | lut_key_adjust( K_FlagField, 0xabcd);
  lut_vld = lut_vld | lut_vld_adjust(K_FlagField);

  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 1, VN_IG0_PTP+PTP_PORT_OFF(port)+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write igr lut entry 1 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));

  /*  Default => StampLow */
  lut_key = lut_key_adjust( K_AlwaysZero, 0);
  lut_vld = lut_vld_adjust(K_AlwaysZero);
  lut_act = lut_act_adjust(PTP_Act_StampLow, 1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1);
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 15, VN_IG0_PTP+PTP_PORT_OFF(port)+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write igr lut entry 15: key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));

  /*  Egress */
  /*  TST && TST_action==0x7 => RemoveTST, UpdateTxTime */
  lut_key = lut_key_adjust( K_FlagTST, 1);
  lut_vld = lut_vld_adjust(K_FlagTST);
  lut_act = lut_act_adjust(PTP_Act_RemoveTST, 1) |
            lut_act_adjust(PTP_Act_UpdateTxTime, 1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1);
  lut_key = lut_key | lut_key_adjust( K_TST_action, 0x7);
  lut_vld = lut_vld | lut_vld_adjust(K_TST_action);
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 0, VN_EG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write egr lut entry 0 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
  /*  TST => RemoveTST */
  lut_key = lut_key_adjust( K_FlagTST, 1);
  lut_vld = lut_vld_adjust(K_FlagTST);
  lut_act = lut_act_adjust(PTP_Act_RemoveTST, 1)  | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1);
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 1, VN_EG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write egr lut entry 1 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
  /*  Default => do nothing */
  lut_key = lut_key_adjust( K_AlwaysZero, 0);
  lut_vld = lut_vld_adjust(K_AlwaysZero);
  lut_act = 0x0;
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 15, VN_EG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write egr lut entry 15: key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
/*   enable_ptp(); */
  return retVal;
}


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
DSDT_STATUS ptp_lut_config_piggyback (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_upper=0;
  unsigned long lut_vld_upper=0;
  unsigned long long lut_act; 
/*   disable_ptp(); */

  /*  Ingress */
  /*  PTPv2 => IniPiggyback */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_IniPiggyback, 1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1); 
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 0, VN_IG0_PTP+PTP_PORT_OFF(port)+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write igr lut entry 0 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));

  /*  Default => do nothing */
  lut_key = lut_key_adjust( K_AlwaysZero, 0);
  lut_vld = lut_vld_adjust(K_AlwaysZero);
  lut_act = 0x0;
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 15, VN_IG0_PTP+PTP_PORT_OFF(port)+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write igr lut entry 15: key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
/*   enable_ptp(); */
  return retVal;
}





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
DSDT_STATUS ptp_lut_config_set_orig_ts (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_upper=0;
  unsigned long lut_vld_upper=0;
  unsigned long long lut_act; 
/*   disable_ptp(); */

  /* Egress */
  /*  PTPv2 => OriginTimestamp */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_UpdateTxTime, 1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1); 
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 0, VN_EG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write egr lut entry 0 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
/*   enable_ptp(); */
  return retVal;
}

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
DSDT_STATUS ptp_lut_config_eg_TS_queue_hi (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_upper=0;
  unsigned long lut_vld_upper=0;
  unsigned long long lut_act; 
/*   disable_ptp(); */

  /* Egress */
  /*  PTPv2 =>  */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_StampHigh, 1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1); 
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 0, VN_IG0_PTP+PTP_PORT_OFF(port)+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write egr lut entry 0 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
/*   enable_ptp(); */
  return retVal;
}
/*******************************************************************************
* ptp_lut_config_eg_TS_queue_lo
*
* DESCRIPTION:
*		Set configuration sample 9c as following,
* - ingress: 
*      - do nothing
* - egress:
*      - PTPv2 => timestamp queue (low)
*      - else do nothing
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
DSDT_STATUS ptp_lut_config_eg_TS_queue_lo (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_upper=0;
  unsigned long lut_vld_upper=0;
  unsigned long long lut_act; 
/*   disable_ptp(); */

  /* Egress */
  /*  PTPv2 =>  */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_StampLow, 1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1); 
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 0, VN_IG0_PTP+PTP_PORT_OFF(port)+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write egr lut entry 0 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
/*   enable_ptp(); */
  return retVal;
}


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
DSDT_STATUS ptp_lut_config_ing_enqueue_lo (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_upper=0;
  unsigned long lut_vld_upper=0;
  unsigned long long lut_act; 
/*   disable_ptp(); */

  /*  Ingress */
  /*  PTPv2 => enqueue timestamp (low priority queue) */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_StampLow, 1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1); 
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 0, VN_IG0_PTP+PTP_PORT_OFF(port)+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write igr lut entry 0 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));

  /*  Default => do nothing */
  lut_key = lut_key_adjust( K_AlwaysZero, 0);
  lut_vld = lut_vld_adjust(K_AlwaysZero);
  lut_act = 0x0;
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 15, VN_IG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write igr lut entry 15: key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
/*   enable_ptp(); */
  return retVal;
}
 

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
DSDT_STATUS ptp_lut_config_eg_enqueue_lo (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_upper=0;
  unsigned long lut_vld_upper=0;
  unsigned long long lut_act; 
/*   disable_ptp(); */

  /*  Egress */
  /*  PTPv2 => enqueue timestamp (low priority queue) */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_StampLow, 1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1); 
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 0, VN_EG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write egr lut entry 0 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));

/*   enable_ptp(); */
  return retVal;
}
 
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
DSDT_STATUS ptp_lut_config_eg_enqueue_hi (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_upper=0;
  unsigned long lut_vld_upper=0;
  unsigned long long lut_act; 
/*   disable_ptp(); */

  /*  Egress */
  /*  PTPv2 => enqueue timestamp (High priority queue) */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_StampHigh, 1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1); 
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 0, VN_EG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write egr lut entry 0 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));

/*   enable_ptp(); */
  return retVal;
}
 

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
DSDT_STATUS ptp_lut_config_ing_enqueue_hi (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_upper=0;
  unsigned long lut_vld_upper=0;
  unsigned long long lut_act; 
/*   disable_ptp(); */

  /*  Ingress */
  /*  PTPv2 => IniPiggyback */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_StampHigh, 1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1); 
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 0, VN_IG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write igr lut entry 0 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));

  lut_key = lut_key_adjust( K_AlwaysZero, 0);
  lut_vld = lut_vld_adjust(K_AlwaysZero);
  lut_act = 0x0;
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 15, VN_IG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write igr lut entry 15: key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
/*   enable_ptp(); */
  return retVal;
}

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
DSDT_STATUS ptp_lut_config_update_res_time (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_upper=0;
  unsigned long lut_vld_upper=0;
  unsigned long long lut_act; 
/*   disable_ptp(); */
  /*  Egress */
  /*  PTPv2 => UpdateResidence */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_UpdateResidence, 1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1); 
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 0, VN_EG0_PTP+PTP_PORT_OFF(port)+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write egr lut entry 0 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));

  /*  Default => do nothing  */
  lut_key = lut_key_adjust( K_AlwaysZero, 0);
  lut_vld = lut_vld_adjust(K_AlwaysZero);
  lut_act = 0x0;
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 15, VN_EG0_PTP+PTP_PORT_OFF(port)+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write egr lut entry 15: key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
/*   enable_ptp(); */
  return retVal;
} 



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
DSDT_STATUS ptp_lut_config_TS_tag_piggyback (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_upper=0;
  unsigned long lut_vld_upper=0;
  unsigned long long lut_act; 
/*   disable_ptp(); */

  /*  Ingress */
  /*  PTPv2 && Sync => InsertTST (long) & IniPiggyback */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_InsertTST, 0) | 
                    lut_act_adjust(PTP_Act_TST_E, 0) | 
                    lut_act_adjust(PTP_Act_TST_action, 0x7) | 
                    lut_act_adjust(PTP_Act_IniPiggyback, 0x1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1);

  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 0, VN_IG0_PTP+PTP_PORT_OFF(port));
#if 0
  PTP_DBG_INFO((">>>Write igr lut entry 0 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
  lut_key = lut_key_adjust( K_AlwaysZero, 0);
  lut_vld = lut_vld_adjust(K_AlwaysZero);
  lut_act = 0x0;
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 15, VN_IG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write igr lut entry 15: key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
#endif

  /*  Egress */
  /*  PTPv2 && Sync => UpdateResidence */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_UpdateResidence, 1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1);
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 0, VN_EG0_PTP+PTP_PORT_OFF(port)+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write egr lut entry 0 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));

#if 0
  /*  Default => do nothing */
  lut_key = lut_key_adjust( K_AlwaysZero, 0);
  lut_vld = lut_vld_adjust(K_AlwaysZero);
  lut_act = 0x0;
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 15, VN_EG0_PTP+PTP_PORT_OFF(port)+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write egr lut entry 15: key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
#endif
/*   enable_ptp(); */
  return retVal;
}


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
DSDT_STATUS ptp_lut_config_msg_type (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_upper=0;
  unsigned long lut_vld_upper=0;
  unsigned long long lut_act; 
/*   disable_ptp(); */

  /*  Ingress */
  /*  PTPv2 && Sync => InsertTST (long) & IniPiggyback */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_InsertTST, 1) | 
                    lut_act_adjust(PTP_Act_TST_E, 1) | 
                    lut_act_adjust(PTP_Act_TST_action, 0x7) | 
                    lut_act_adjust(PTP_Act_IniPiggyback, 0x1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1);
  lut_key = lut_key | lut_key_adjust( K_MessageType, PTPv2_MSG_Sync);
  lut_vld = lut_vld | lut_vld_adjust(K_MessageType);

  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 0, VN_IG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write igr lut entry 0 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
  /*  PTPv2 && Delay_Req => InsertTST (long) & IniPiggyback */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_InsertTST, 1) | 
                    lut_act_adjust(PTP_Act_TST_E, 1) | 
                    lut_act_adjust(PTP_Act_TST_action, 0x7) | 
                    lut_act_adjust(PTP_Act_IniPiggyback, 0x1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1);
  lut_key = lut_key | lut_key_adjust( K_MessageType, PTPv2_MSG_Delay_Req);
  lut_vld = lut_vld | lut_vld_adjust(K_MessageType);

  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 1, VN_IG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write igr lut entry 1 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
  /*  PTPv2 && Delay_Resp => InsertTST (long) & IniPiggyback */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_InsertTST, 1) | 
                    lut_act_adjust(PTP_Act_TST_E, 1) | 
                    lut_act_adjust(PTP_Act_TST_action, 0x7) | 
                    lut_act_adjust(PTP_Act_IniPiggyback, 0x1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1);
  lut_key = lut_key | lut_key_adjust( K_MessageType, PTPv2_MSG_Delay_Resp);
  lut_vld = lut_vld | lut_vld_adjust(K_MessageType);

  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 2, VN_IG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write igr lut entry 2 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
  /*  PTPv2 && Pdelay_Req => InsertTST (long) & IniPiggyback */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_InsertTST, 1) | 
                    lut_act_adjust(PTP_Act_TST_E, 1) | 
                    lut_act_adjust(PTP_Act_TST_action, 0x7) | 
                    lut_act_adjust(PTP_Act_IniPiggyback, 0x1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1);
  lut_key = lut_key | lut_key_adjust( K_MessageType, PTPv2_MSG_Pdelay_Req);
  lut_vld = lut_vld | lut_vld_adjust(K_MessageType);

  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 3, VN_IG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write igr lut entry 3 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
  /*  PTPv2 && Pdelay_Resp => InsertTST (long) & IniPiggyback */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_InsertTST, 1) | 
                    lut_act_adjust(PTP_Act_TST_E, 1) | 
                    lut_act_adjust(PTP_Act_TST_action, 0x7) | 
                    lut_act_adjust(PTP_Act_IniPiggyback, 0x1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1);
  lut_key = lut_key | lut_key_adjust( K_MessageType, PTPv2_MSG_Pdelay_Resp);
  lut_vld = lut_vld | lut_vld_adjust(K_MessageType);

  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 4, VN_IG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write igr lut entry 4 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
  /*  Default => do nothing */
  lut_key = lut_key_adjust( K_AlwaysZero, 0);
  lut_vld = lut_vld_adjust(K_AlwaysZero);
  lut_act = 0x0;
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 15, VN_IG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write igr lut entry 15: key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));

  /*  Egress */
  /*  PTPv2 && Sync => UpdateResidence */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_UpdateResidence, 1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1);
  lut_key = lut_key | lut_key_adjust( K_MessageType, PTPv2_MSG_Sync);
  lut_vld = lut_vld | lut_vld_adjust(K_MessageType);
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 0, VN_EG0_PTP+PTP_PORT_OFF(port)+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write egr lut entry 0 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
  /*  PTPv2 && Delay_Req => UpdateResidence */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_UpdateResidence, 1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1);
  lut_key = lut_key | lut_key_adjust( K_MessageType, PTPv2_MSG_Delay_Req);
  lut_vld = lut_vld | lut_vld_adjust(K_MessageType);
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 1, VN_EG0_PTP+PTP_PORT_OFF(port)+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write egr lut entry 1 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
  /*  PTPv2 && Delay_Resp => UpdateResidence */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_UpdateResidence, 1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1);
  lut_key = lut_key | lut_key_adjust( K_MessageType, PTPv2_MSG_Delay_Resp);
  lut_vld = lut_vld | lut_vld_adjust(K_MessageType);
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 2, VN_EG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write egr lut entry 2 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
  /*  PTPv2 && Pdelay_Req => UpdateResidence */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_UpdateResidence, 1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1);
  lut_key = lut_key | lut_key_adjust( K_MessageType, PTPv2_MSG_Pdelay_Req);
  lut_vld = lut_vld | lut_vld_adjust(K_MessageType);
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 3, VN_EG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write egr lut entry 3 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
  /*  PTPv2 && Pdelay_Resp => UpdateResidence */
  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = lut_act_adjust(PTP_Act_UpdateResidence, 1) | 
 		            lut_act_adjust(PTP_Act_HoldForTransmission, 1);
  lut_key = lut_key | lut_key_adjust( K_MessageType, PTPv2_MSG_Pdelay_Resp);
  lut_vld = lut_vld | lut_vld_adjust(K_MessageType);
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 4, VN_EG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write egr lut entry 4 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
  /*  Default => do nothing */
  lut_key = lut_key_adjust( K_AlwaysZero, 0);
  lut_vld = lut_vld_adjust(K_AlwaysZero);
  lut_act = 0x0;
  prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 15, VN_EG0_PTP+PTP_PORT_OFF(port));
  PTP_DBG_INFO((">>>Write egr lut entry 15: key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
/*   enable_ptp(); */
  return retVal;
}

/*******************************************************************************
* ptp_default_init
*
* DESCRIPTION:
*		Initialize new PTP block with default selection
*       That is to complete init_access, simple udata, default LUT setup
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
DSDT_STATUS ptp_default_init (void * dev, int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  ptp_dev_init(dev);
  disable_ptp(port);
  ptp_1step_set_smac_loopback(port);
  ptp_1step_setup_simple_udata(port);
  lut_init();
  enable_ptp(port);
  return retVal;
}

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
DSDT_STATUS ptp_test_interrupt (void * dev, int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int mask, chMask;

  PTP_DBG_DEMO((">>>Test PTP Interrupt\n"));

  {
	mask = (port+1) | (port+1)*0x10 | (port+1)*0x100 | (port+1)*0x1000;
    set_ptp_intr_msk (port, PTP_igr, mask);
    get_ptp_intr_msk (port, PTP_igr, &chMask);
	if(mask!=chMask)
	{
      PTP_DBG_DEMO((">>>Test IGR PTP Interrupt error. read %x write %x\n", chMask,mask));
	  retVal=DSDT_FAIL;
	}

	mask = (port+2) | (port+2)*0x10 | (port+2)*0x100 | (port+2)*0x1000;
    set_ptp_intr_msk (port, PTP_egr, mask);
    get_ptp_intr_msk (port, PTP_egr, &chMask);
	if(mask!=chMask)
	{
      PTP_DBG_DEMO((">>>Test EGR PTP Interrupt error. read %x write %x\n", chMask,mask));
	  retVal=DSDT_FAIL;
	}

	mask = 0xa;
    set_tai_intr_msk (port, mask);
    get_tai_intr_msk (port, &chMask);
	if(mask!=chMask)
	{
      PTP_DBG_DEMO((">>>Test PTP TAI Interrupt error. read %x write %x\n", chMask,mask));
	  retVal=DSDT_FAIL;
	}
	mask = 0x5;
    set_tai_intr_msk (port, mask);
    get_tai_intr_msk (port, &chMask);
	if(mask!=chMask)
	{
      PTP_DBG_DEMO((">>>Test PTP TAI Interrupt error. read %x write %x\n", chMask,mask));
	  retVal=DSDT_FAIL;
	}

	mask = 0x1f;
    set_top_intr_msk (port, mask);
    get_top_intr_msk (port, &chMask);
	if(mask!=chMask)
	{
      PTP_DBG_DEMO((">>>Test PTP top Interrupt error. read %x write %x\n", chMask,mask));
	  retVal=DSDT_FAIL;
	}

	
	if (retVal==DSDT_OK)
      PTP_DBG_DEMO((">>>Test PTP All Interrupt successful\n"));

  }
  return retVal;
}

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
DSDT_STATUS ptp_set_interrupt (void * dev, int port,int en) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int mask;

  PTP_DBG_DEMO((">>>Set PTP Interrupt %s\n", (en)?"enable":"disable"));
  {
	if(en)
	  mask = 0;
	else
	  mask = 0xffff;
    set_ptp_intr_msk (port, PTP_igr, mask);
    set_ptp_intr_msk (port, PTP_egr, mask);
	if(en)
	  mask = 0;
	else
      mask = 0x1f;
    set_top_intr_msk (port, mask);

	PTP_DBG_DEMO((">>>Set PTP All Interrupt done\n"));

  }
  return retVal;
}

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
DSDT_STATUS ptp_disp_interrupt (void * dev, int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int mask;

  {
    get_ptp_intr_msk (port, PTP_igr, &mask);
	PTP_DBG_DEMO((">>>Get PTP IGR Interrupt %x\n", mask));
	PTP_DBG_DEMO((">>>PTP IGR Interrupt 0 is enabled\n"));

    get_ptp_intr_msk (port, PTP_egr, &mask);
	PTP_DBG_DEMO((">>>Get PTP EGR Interrupt %x\n", mask));
	PTP_DBG_DEMO((">>>PTP EGR Interrupt 0 is enabled\n"));

    get_top_intr_msk (port, &mask);
	PTP_DBG_DEMO((">>>Get PTP Top Interrupt %x\n", mask));
	PTP_DBG_DEMO((">>>PTP top Interrupt 0 is enabled\n"));

	PTP_DBG_DEMO((">>>Get PTP All Interrupt done\n"));

  }
  return retVal;
}

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
)
{
  DSDT_STATUS   retVal = DSDT_OK;
  retVal = set_ptp_tai_timereg(port, REG_TOD_0, 0,0,0, 0);
  if(retVal!=DSDT_OK) return retVal;
  retVal = set_drift_adj(port, 0);
  if(retVal!=DSDT_OK) return retVal;
  retVal = set_drift_adj_step(port,  drift_frac_ns);
	return retVal;
}

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


/* demo E LUT */
DSDT_STATUS ptp_lut_config_udp_1731_ntp (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_top=0;
  unsigned long lut_vld_top=0;
  unsigned long long lut_act; 
  int addr = VN_IG0_PTP;
  PTP_DBG_TRACE(("PTP ptp_lut_config_udp_1731_ntp port %d \n", port));


  /* Ingress
  # UDP => none */
  lut_key = lut_key_adjust( K_FlagUDP, 1);
  lut_vld = lut_vld_adjust(K_FlagUDP);
  lut_act = 0;

  retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_top, lut_vld_top, lut_act, 0, addr+PTP_PORT_OFF(port));
  if(retVal!=DSDT_OK) return retVal;
  /* IPSec (std) => InsertTST (long) */
  lut_key = lut_key_adjust( K_FlagY1731, 1);
  lut_vld = lut_vld_adjust(K_FlagY1731);
  lut_act = lut_act_adjust(PTP_Act_InsertTST, 1) | 
             lut_act_adjust(PTP_Act_TST_E, 1) |
             lut_act_adjust(PTP_Act_TST_action, 0x7);

  retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_top, lut_vld_top, lut_act, 1, addr+PTP_PORT_OFF(port));
  if(retVal!=DSDT_OK) return retVal;
  /* IPSec (timed) => StampLow */
  lut_key = lut_key_adjust( K_FlagNTP, 1);
  lut_vld = lut_vld_adjust(K_FlagNTP);
  lut_act = lut_act_adjust(PTP_Act_StampLow, 1);

  retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_top, lut_vld_top, lut_act, 2, addr+PTP_PORT_OFF(port));
  return retVal;
  /* Egress (none) */
}
/*******************************************************************************
* ntp_test_config
*
* DESCRIPTION:
*		Set configuration sample of NTP,
* - ingress: 
*      - NTP set Time stamphigh and piggyback
*
* - egress:
*      - NTP set Time stamplow and updateTS
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

/*  NTP test configuration */
PTP_1STEP_LUT_ACT igrActionList [3] = {PTP_Act_StampHigh,
	                                   PTP_Act_IniPiggyback,
                                       PTP_Act_No_Action};
PTP_1STEP_LUT_ACT egrActionList [4] = {PTP_Act_StampLow,
                                       PTP_Act_UpdateTxTime,
                                       PTP_Act_HoldForTransmission,
                                       PTP_Act_No_Action};
/* For second UDP port */
PTP_1STEP_LUT_ACT igrActionList_1 [3] = {PTP_Act_StampLow,
	                                   PTP_Act_IniPiggyback,
                                       PTP_Act_No_Action};
PTP_1STEP_LUT_ACT egrActionList_1 [4] = {PTP_Act_StampHigh,
                                       PTP_Act_UpdateTxTime,
                                       PTP_Act_HoldForTransmission,
                                       PTP_Act_No_Action};


#if 0 /* set actions in different LUT entry */
DSDT_STATUS ntp_test_config (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  int i;
/*
# lut entry 0, match on ntp frame
# lut entry 1, all other frames
*/
  for(i=0; i<2; i++)
  {
    retVal = prgm_lut_ent_ntp(port, i, igrActionList[i], PTP_igr, 0); 
    if(retVal!=DSDT_OK) return retVal;
    retVal = prgm_lut_ent_ntp(port, i, egrActionList[i], PTP_egr, 0); 
    if(retVal!=DSDT_OK) return retVal;
  }
  retVal = prgm_lut_ent_always0(port, i, PTP_Act_No_Action, PTP_igr); 
  if(retVal!=DSDT_OK) return retVal;
  retVal = prgm_lut_ent_always0(port, i, PTP_Act_No_Action, PTP_egr); 
  if(retVal!=DSDT_OK) return retVal;
  for (i=0; i<2; i++) {
    check_lut_entry(port, i, PTP_igr);
     check_lut_entry(port, i, PTP_egr);
  }
  return retVal;
}
#else  /* set more actions in one entry */
DSDT_STATUS ntp_test_config (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  int i;
  retVal = prgm_lut_ent_ntp_list(port, 0, igrActionList, PTP_igr, 0); 
  if(retVal!=DSDT_OK) return retVal;
  retVal = prgm_lut_ent_ntp_list(port, 0, egrActionList, PTP_egr, 0); 
  if(retVal!=DSDT_OK) return retVal;

/* For second UDP port */
  retVal = prgm_lut_ent_ntp_list(port, 1, igrActionList_1, PTP_igr, 1); 
  if(retVal!=DSDT_OK) return retVal;
  retVal = prgm_lut_ent_ntp_list(port, 1, egrActionList_1, PTP_egr, 1); 
  if(retVal!=DSDT_OK) return retVal;

  retVal = prgm_lut_ent_always0(port, 2, PTP_Act_No_Action, PTP_igr); 
  if(retVal!=DSDT_OK) return retVal;
  retVal = prgm_lut_ent_always0(port, 2, PTP_Act_No_Action, PTP_egr); 
  if(retVal!=DSDT_OK) return retVal;
  for (i=0; i<2; i++) {
    check_lut_entry(port, i, PTP_igr);
     check_lut_entry(port, i, PTP_egr);
 }
 return retVal;
}
#endif

#define USE_CLEAR_QUEUE 0

/*   GET_PTP_TSQL_DRAIN */
void ptp_get_tsql_drain(int port) {

#if 0  /* Call read all queue and display */
//        PTP_DBG_DEMO(("Read PTP IG0 enqueue low\n"));
        get_ptp_tsql_drain (port, PTP_igr);
//        PTP_DBG_DEMO(("Read PTP EG0 enqueue low\n"));
        get_ptp_tsql_drain (port, PTP_egr);
#else  /* Repeat to call read queue */
DSDT_STATUS retVal;
Ptp1StepTsStruct ts;
int numwords;

  do
  {
    retVal = ptp_get_ntp_ts_tsq(port, PTP_igr,PTP_1STEP_QUEUE_LOW, &numwords, &ts);
    if(retVal!=DSDT_OK) break;
	PTP_DBG_DEMO(("IGR Queue Low Ts %d%d sec 0x%x ns\n", ts.extTs.topseconds, ts.extTs.seconds, ts.extTs.nanoseconds));
#if USE_CLEAR_QUEUE
	if((numwords/7==0)&&(numwords%7))
	{
	  int idx =0;
      retVal = ptp_find_lut_action(port, PTP_Act_StampLow, PTP_igr, &idx); 
      if(retVal!=DSDT_OK) break;
      retVal = ptp_enable_lut_action(port, idx, PTP_Act_StampLow, PTP_igr,DSDT_FALSE); 
      if(retVal!=DSDT_OK) break;
      retVal = ptp_get_tsql_part(port, PTP_igr, numwords%7, &ts);
      if(retVal!=DSDT_OK) break;
      retVal = ptp_enable_lut_action(port, idx, PTP_Act_StampLow, PTP_igr,DSDT_TRUE); 
      if(retVal!=DSDT_OK) break;
	}
#endif
  } while (numwords!=0);

  do
  {
    retVal = ptp_get_ntp_ts_tsq(port, PTP_egr,PTP_1STEP_QUEUE_LOW, &numwords, &ts);
    if(retVal!=DSDT_OK) return;
	PTP_DBG_DEMO(("EGR Queue Low Ts %d%d sec 0x%x ns\n", ts.extTs.topseconds, ts.extTs.seconds, ts.extTs.nanoseconds));
#if USE_CLEAR_QUEUE
	if((numwords/7==0)&&(numwords%7))
	{
	  int idx =0;
      retVal = ptp_find_lut_action(port, PTP_Act_StampLow, PTP_egr, &idx); 
      if(retVal!=DSDT_OK) break;
      retVal = ptp_enable_lut_action(port, idx, PTP_Act_StampLow, PTP_egr,DSDT_FALSE); 
      if(retVal!=DSDT_OK) break;
      retVal = ptp_get_tsql_part(port, PTP_egr, numwords%7, &ts);
      if(retVal!=DSDT_OK) break;
      retVal = ptp_enable_lut_action(port, idx, PTP_Act_StampLow, PTP_egr,DSDT_TRUE); 
      if(retVal!=DSDT_OK) break;
	}
#endif
  } while (numwords!=0);


#endif
}

/*    GET_PTP_TSQH_DRAIN */
void ptp_get_tsqh_drain(int port) {
#if 0 /* Call read all queue and display */
//        PTP_DBG_DEMO(("Read PTP IG0 enqueue High\n"));
        get_ptp_tsqh_drain (port, PTP_igr);
//        PTP_DBG_DEMO(("Read PTP EG0 enqueue High\n"));
        get_ptp_tsqh_drain (port, PTP_egr);
#else /* Repeat to call read queue */
DSDT_STATUS retVal;
Ptp1StepTsStruct ts;
int numwords;

  do
  {
    retVal = ptp_get_ntp_ts_tsq(port, PTP_igr,PTP_1STEP_QUEUE_HIGH, &numwords, &ts);
    if(retVal!=DSDT_OK) break;
	PTP_DBG_DEMO(("IGR Queue High Ts %d%d sec 0x%x ns\n", ts.extTs.topseconds, ts.extTs.seconds, ts.extTs.nanoseconds));
#if USE_CLEAR_QUEUE
	if((numwords/7==0)&&(numwords%7))
	{
	  int idx =0;
      retVal = ptp_find_lut_action(port, PTP_Act_StampHigh, PTP_igr, &idx) ;
      if(retVal!=DSDT_OK) break;
      retVal = ptp_enable_lut_action(port, idx, PTP_Act_StampHigh, PTP_igr,DSDT_FALSE); 
      if(retVal!=DSDT_OK) break;
      retVal = ptp_get_tsql_part(port, PTP_igr, numwords%7, &ts);
      if(retVal!=DSDT_OK) break;
      retVal = ptp_enable_lut_action(port, idx, PTP_Act_StampHigh, PTP_igr,DSDT_TRUE); 
      if(retVal!=DSDT_OK) break;
	}
#endif

  } while (numwords!=0);

  do
  {
    retVal = ptp_get_ntp_ts_tsq(port, PTP_egr,PTP_1STEP_QUEUE_HIGH, &numwords, &ts);
    if(retVal!=DSDT_OK) return;
	PTP_DBG_DEMO(("EGR Queue High Ts %d%d sec 0x%x ns\n", ts.extTs.topseconds, ts.extTs.seconds, ts.extTs.nanoseconds));
#if USE_CLEAR_QUEUE
	if((numwords/7==0)&&(numwords%7))
	{
	  int idx =0;
      retVal = ptp_find_lut_action(port, PTP_Act_StampHigh, PTP_egr, &idx); 
      if(retVal!=DSDT_OK) break;
      retVal = ptp_enable_lut_action(port, idx, PTP_Act_StampHigh, PTP_egr,DSDT_FALSE); 
      if(retVal!=DSDT_OK) break;
      retVal = ptp_get_tsql_part(port, PTP_egr, numwords%7, &ts);
      if(retVal!=DSDT_OK) break;
      retVal = ptp_enable_lut_action(port, idx, PTP_Act_StampHigh, PTP_egr,DSDT_TRUE); 
      if(retVal!=DSDT_OK) break;
	}
#endif
  } while (numwords!=0);

#endif
}


