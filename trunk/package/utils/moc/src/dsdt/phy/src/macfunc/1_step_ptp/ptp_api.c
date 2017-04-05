#include <mvlPtpCopyright.h>

/********************************************************************************
* ptp_api.c
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

#include <msec_defines.h>
#include <msecFunc.h>
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
DSDT_STATUS  ptp_lut_set_udp (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
	 
    retVal = ptp_lut_update (port, lut_key_adjust(K_FlagUDP, 1), lut_vld_adjust(K_FlagUDP), 0ll,
                  0ll, 0ll, 0ll,
				  0, VN_IG0_PTP+PTP_PORT_OFF(port));
  return retVal;
}
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
DSDT_STATUS  ptp_lut_clear_udp (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
	 
   retVal =  ptp_lut_update (port, lut_key_adjust(K_FlagUDP, 1), lut_vld_adjust(K_FlagUDP), 0ll,
                  lut_key_adjust(K_FlagUDP, 0), lut_vld_adjust(K_FlagUDP), 0ll,
				  0, VN_IG0_PTP+PTP_PORT_OFF(port));
  return retVal;
}




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
DSDT_STATUS ptp_1step_V2_L2_init(int port, PTP_1STEP_CFG *ptp_cfg, int numIngLut, int numEgLut)
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_upper=0;
  unsigned long lut_vld_upper=0;
  unsigned long long lut_act; 
  int i;

  retVal = ptp_lut_clear_udp (port);
  if(retVal!=DSDT_OK)
    return retVal;

  for(i=0; i<numIngLut; i++)
  {
    /* Set Ingress configuration */
   lut_act = lut_act_adjust(PTP_Act_TST_action, 0x7);
   if((ptp_cfg[i].ingaction)&PTP_1STEP_ING_ACT_TS_TAG_EX)
    lut_act |= lut_act_adjust(PTP_Act_InsertTST, 1) | 
             lut_act_adjust(PTP_Act_TST_E, 1);

   if((ptp_cfg[i].ingaction)&PTP_1STEP_ING_ACT_TS_TAG)
   {
    lut_act |= lut_act_adjust(PTP_Act_InsertTST, 1) | 
             lut_act_adjust(PTP_Act_TST_E, 0);
   }


   if((ptp_cfg[i].ingaction)&PTP_1STEP_ING_ACT_PIGGY)
    lut_act |= lut_act_adjust(PTP_Act_TST_E, 0) | 
             lut_act_adjust(PTP_Act_IniPiggyback, 0x1);


   if((ptp_cfg[i].ingaction)&PTP_1STEP_ING_ACT_LOW_QUEUE)
    lut_act |= lut_act_adjust(PTP_Act_TST_E, 0) | 
            lut_act_adjust(PTP_Act_StampLow, 1);
 

   if((ptp_cfg[i].ingaction)&PTP_1STEP_ING_ACT_HIGH_QUEUE)
    lut_act |= lut_act_adjust(PTP_Act_TST_E, 0) | 
            lut_act_adjust(PTP_Act_StampHigh, 1);



    if ((ptp_cfg[i].msgTypes)&PTP_1STEP_MSG_SYNC)
    {
       lut_key = lut_key_adjust( K_FlagPTPv2, 1);
       lut_vld = lut_vld_adjust(K_FlagPTPv2);
       lut_key = lut_key | lut_key_adjust( K_MessageType, PTPv2_MSG_Sync);
       lut_vld = lut_vld | lut_vld_adjust(K_MessageType);
       retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, i, VN_IG0_PTP+PTP_PORT_OFF(port));
       if(retVal!=DSDT_OK)
         return retVal;
       PTP_DBG_INFO((">>>Write igr lut entry %x : key %llx  vld %llx  act %llx\n", i, lut_key, lut_vld, lut_act));
    }
    if ((ptp_cfg[i].msgTypes)&PTP_1STEP_MSG_DELAY_REQ)
    {
       lut_key = lut_key_adjust( K_FlagPTPv2, 1);
       lut_vld = lut_vld_adjust(K_FlagPTPv2);
       lut_key = lut_key | lut_key_adjust( K_MessageType, PTPv2_MSG_Delay_Req);
       lut_vld = lut_vld | lut_vld_adjust(K_MessageType);
       retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, i, VN_IG0_PTP+PTP_PORT_OFF(port));
       if(retVal!=DSDT_OK)
         return retVal;
       PTP_DBG_INFO((">>>Write igr lut entry %x : key %llx  vld %llx  act %llx\n", i, lut_key, lut_vld, lut_act));
    }
    if ((ptp_cfg[i].msgTypes)&PTP_1STEP_MSG_PDELAY_REQ)
    {
       lut_key = lut_key_adjust( K_FlagPTPv2, 1);
       lut_vld = lut_vld_adjust(K_FlagPTPv2);
       lut_key = lut_key | lut_key_adjust( K_MessageType, PTPv2_MSG_Pdelay_Req);
       lut_vld = lut_vld | lut_vld_adjust(K_MessageType);
       retVal = prgm_lut_entry(port,  lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, i, VN_IG0_PTP+PTP_PORT_OFF(port));
       if(retVal!=DSDT_OK)
         return retVal;
       PTP_DBG_INFO((">>>Write igr lut entry %x : key %llx  vld %llx  act %llx\n", i, lut_key, lut_vld, lut_act));
    }
    if ((ptp_cfg[i].msgTypes)&PTP_1STEP_MSG_PDELAY_RESP)
    {
       lut_key = lut_key_adjust( K_FlagPTPv2, 1);
       lut_vld = lut_vld_adjust(K_FlagPTPv2);
       lut_key = lut_key | lut_key_adjust( K_MessageType, PTPv2_MSG_Pdelay_Resp);
       lut_vld = lut_vld | lut_vld_adjust(K_MessageType);
       retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, i, VN_IG0_PTP+PTP_PORT_OFF(port));
       if(retVal!=DSDT_OK)
         return retVal;
       PTP_DBG_INFO((">>>Write igr lut entry %x : key %llx  vld %llx  act %llx\n", i, lut_key, lut_vld, lut_act));
    }
    if ((ptp_cfg[i].msgTypes)&PTP_1STEP_MSG_DELAY_RESP)
    {
       lut_key = lut_key_adjust( K_FlagPTPv2, 1);
       lut_vld = lut_vld_adjust(K_FlagPTPv2);
       lut_key = lut_key | lut_key_adjust( K_MessageType, PTPv2_MSG_Delay_Resp);
       lut_vld = lut_vld | lut_vld_adjust(K_MessageType);
       retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, i, VN_IG0_PTP+PTP_PORT_OFF(port));
       if(retVal!=DSDT_OK)
         return retVal;
       PTP_DBG_INFO((">>>Write igr lut entry %x : key %llx  vld %llx  act %llx\n", i, lut_key, lut_vld, lut_act));
    }
    if ((ptp_cfg[i].msgTypes)&PTP_1STEP_MSG_FOLLOWUP)
    {
       lut_key = lut_key_adjust( K_FlagPTPv2, 1);
       lut_vld = lut_vld_adjust(K_FlagPTPv2);
       lut_key = lut_key | lut_key_adjust( K_MessageType, PTPv2_MSG_Follow_Up);
       lut_vld = lut_vld | lut_vld_adjust(K_MessageType);
       retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, i, VN_IG0_PTP+PTP_PORT_OFF(port));
       if(retVal!=DSDT_OK)
         return retVal;
       PTP_DBG_INFO((">>>Write igr lut entry %x : key %llx  vld %llx  act %llx\n", i, lut_key, lut_vld, lut_act));
    }
    if ((ptp_cfg[i].msgTypes)&PTP_1STEP_MSG_PDELAY_RESP_FOLLOWUP)
    {
       lut_key = lut_key_adjust( K_FlagPTPv2, 1);
       lut_vld = lut_vld_adjust(K_FlagPTPv2);
       lut_key = lut_key | lut_key_adjust( K_MessageType, PTPv2_MSG_Pdelay_Resp_Follow_Up);
       lut_vld = lut_vld | lut_vld_adjust(K_MessageType);
       retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, i, VN_IG0_PTP+PTP_PORT_OFF(port));
       if(retVal!=DSDT_OK)
         return retVal;
       PTP_DBG_INFO((">>>Write igr lut entry %x : key %llx  vld %llx  act %llx\n", i, lut_key, lut_vld, lut_act));
    }
    if ((ptp_cfg[i].msgTypes)&PTP_1STEP_MSG_ANNOUNCE)
    {
       lut_key = lut_key_adjust( K_FlagPTPv2, 1);
       lut_vld = lut_vld_adjust(K_FlagPTPv2);
       lut_key = lut_key | lut_key_adjust( K_MessageType, PTPv2_MSG_Announce);
       lut_vld = lut_vld | lut_vld_adjust(K_MessageType);
       retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, i, VN_IG0_PTP+PTP_PORT_OFF(port));
       if(retVal!=DSDT_OK)
         return retVal;
       PTP_DBG_INFO((">>>Write igr lut entry %x : key %llx  vld %llx  act %llx\n", i, lut_key, lut_vld, lut_act));
    }
    /* could add management and Signaling */
  }
  /*  Default => do nothing */
  lut_key = lut_key_adjust( K_AlwaysZero, 0);
  lut_vld = lut_vld_adjust(K_AlwaysZero);
  lut_act = 0x0;
  retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 15, VN_IG0_PTP+PTP_PORT_OFF(port));
  if(retVal!=DSDT_OK)
    return retVal;
  PTP_DBG_INFO((">>>Write igr lut entry 15: key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));


  /* Set Egress configuration */
  for(i=0; i<numEgLut; i++)
  {
    lut_act = lut_act_adjust(PTP_Act_HoldForTransmission, 1);
    if (ptp_cfg[i].egaction & PTP_1STEP_EG_ACT_REMOVE_TST)
    lut_act |= lut_act_adjust(PTP_Act_RemoveTST, 1) ;

   if((ptp_cfg[i].egaction)&PTP_1STEP_EG_ACT_LOW_QUEUE)
    lut_act |= lut_act_adjust(PTP_Act_StampLow, 0x1);

   if((ptp_cfg[i].egaction)& PTP_1STEP_EG_ACT_HIGH_QUEUE)
    lut_act |= lut_act_adjust(PTP_Act_StampHigh, 0x1);

   if((ptp_cfg[i].egaction)&PTP_1STEP_EG_ACT_UPDATE_TS)
    lut_act |= lut_act_adjust(PTP_Act_UpdateTxTime, 1); 
 
   if((ptp_cfg[i].egaction)&PTP_1STEP_EG_ACT_SET_RES_TS)
    lut_act |= lut_act_adjust(PTP_Act_UpdateResidence, 1);

   if ((ptp_cfg[i].msgTypes)&PTP_1STEP_MSG_SYNC)
   {
     lut_key = lut_key_adjust( K_FlagPTPv2, 1);
     lut_vld = lut_vld_adjust(K_FlagPTPv2);
     lut_key = lut_key | lut_key_adjust( K_MessageType, PTPv2_MSG_Sync);
     lut_vld = lut_vld | lut_vld_adjust(K_MessageType);
     retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, i, VN_EG0_PTP+PTP_PORT_OFF(port));
     if(retVal!=DSDT_OK)
       return retVal;
     PTP_DBG_INFO((">>>Write egr lut entry %x : key %llx  vld %llx  act %llx\n", i, lut_key, lut_vld, lut_act));
   }
   if ((ptp_cfg[i].msgTypes)&PTP_1STEP_MSG_DELAY_REQ)
   {
     lut_key = lut_key_adjust( K_FlagPTPv2, 1);
     lut_vld = lut_vld_adjust(K_FlagPTPv2);
     lut_key = lut_key | lut_key_adjust( K_MessageType, PTPv2_MSG_Delay_Req);
     lut_vld = lut_vld | lut_vld_adjust(K_MessageType);
     retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, i, VN_EG0_PTP+PTP_PORT_OFF(port));
     if(retVal!=DSDT_OK)
       return retVal;
     PTP_DBG_INFO((">>>Write egr lut entry %x : key %llx  vld %llx  act %llx\n", i, lut_key, lut_vld, lut_act));
   }
   if ((ptp_cfg[i].msgTypes)&PTP_1STEP_MSG_PDELAY_REQ)
   {
     lut_key = lut_key_adjust( K_FlagPTPv2, 1);
     lut_vld = lut_vld_adjust(K_FlagPTPv2);
     lut_key = lut_key | lut_key_adjust( K_MessageType, PTPv2_MSG_Pdelay_Req);
     lut_vld = lut_vld | lut_vld_adjust(K_MessageType);
     retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, i, VN_EG0_PTP+PTP_PORT_OFF(port));
     if(retVal!=DSDT_OK)
       return retVal;
     PTP_DBG_INFO((">>>Write egr lut entry %x : key %llx  vld %llx  act %llx\n", i, lut_key, lut_vld, lut_act));
   }
   if ((ptp_cfg[i].msgTypes)&PTP_1STEP_MSG_PDELAY_RESP)
   {
     lut_key = lut_key_adjust( K_FlagPTPv2, 1);
     lut_vld = lut_vld_adjust(K_FlagPTPv2);
     lut_key = lut_key | lut_key_adjust( K_MessageType, PTPv2_MSG_Pdelay_Resp);
     lut_vld = lut_vld | lut_vld_adjust(K_MessageType);
     retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, i, VN_EG0_PTP+PTP_PORT_OFF(port));
     if(retVal!=DSDT_OK)
       return retVal;
     PTP_DBG_INFO((">>>Write egr lut entry %x : key %llx  vld %llx  act %llx\n", i, lut_key, lut_vld, lut_act));
   }
   if ((ptp_cfg[i].msgTypes)&PTP_1STEP_MSG_DELAY_RESP)
   {
     lut_key = lut_key_adjust( K_FlagPTPv2, 1);
     lut_vld = lut_vld_adjust(K_FlagPTPv2);
     lut_key = lut_key | lut_key_adjust( K_MessageType, PTPv2_MSG_Delay_Resp);
     lut_vld = lut_vld | lut_vld_adjust(K_MessageType);
     retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, i, VN_EG0_PTP+PTP_PORT_OFF(port));
     if(retVal!=DSDT_OK)
       return retVal;
     PTP_DBG_INFO((">>>Write egr lut entry %x : key %llx  vld %llx  act %llx\n", i, lut_key, lut_vld, lut_act));
   }
  }

/*   prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 0, VN_EG0_PTP+PTP_PORT_OFF(port)); */
/*   PTP_DBG_INFO((">>>Write egr lut entry 0 : key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act)); */
  /*  Default => do nothing */
  lut_key = lut_key_adjust( K_AlwaysZero, 0);
  lut_vld = lut_vld_adjust(K_AlwaysZero);
  lut_act = 0x0;
  retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_upper, lut_vld_upper, lut_act, 15, VN_EG0_PTP+PTP_PORT_OFF(port));
  if(retVal!=DSDT_OK)
    return retVal;
  PTP_DBG_INFO((">>>Write egr lut entry 15: key %llx  vld %llx  act %llx\n", lut_key, lut_vld, lut_act));
  
  return retVal; 
}

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
DSDT_STATUS ptp_1step_V2_IP4_UDP_init(int port, PTP_1STEP_CFG *ptp_cfg, int numIngLut, int numEgLut)
{
  DSDT_STATUS   retVal = DSDT_OK;

  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_top=0;
  unsigned long lut_vld_top=0;
  unsigned long long lut_act; 
  int i;

  retVal = ptp_lut_clear_udp (port);
  if(retVal!=DSDT_OK)
    return retVal;
  retVal = ptp_1step_V2_L2_init (port, ptp_cfg, numIngLut, numEgLut);
  if(retVal!=DSDT_OK)
    return retVal;

  for(i=0; i<numIngLut; i++)
  {
    retVal = read_lut_entry(port, &lut_key, &lut_vld, &lut_key_top, &lut_vld_top, &lut_act, i, VN_IG0_PTP+PTP_PORT_OFF(port));
    if(retVal!=DSDT_OK)
      return retVal;
    PTP_DBG_INFO((">>> read lut entry : key %x:%llx  vld %x:%llx  act %llx\n", 
	lut_key_top, lut_key, lut_vld_top, lut_vld, lut_act));
    lut_key = lut_key | lut_key_adjust( K_FlagUDP, 1);
    lut_vld = lut_vld | lut_vld_adjust(K_FlagUDP);
    lut_key = lut_key | lut_key_adjust( K_FlagIPv4, 1);
    lut_vld = lut_vld | lut_vld_adjust(K_FlagIPv4);
    retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_top, lut_vld_top, lut_act, i, VN_IG0_PTP+PTP_PORT_OFF(port));
    if(retVal!=DSDT_OK)
      return retVal;
    PTP_DBG_INFO((">>>Write igr lut entry %x : key %llx  vld %llx  act %llx\n", i, lut_key, lut_vld, lut_act));
  }
  for(i=0; i<numEgLut; i++)
  {
    retVal = read_lut_entry(port, &lut_key, &lut_vld, &lut_key_top, &lut_vld_top, &lut_act, i, VN_EG0_PTP+PTP_PORT_OFF(port));
    if(retVal!=DSDT_OK)
      return retVal;
    PTP_DBG_INFO((">>> read lut entry : key %x:%llx  vld %x:%llx  act %llx\n", 
	lut_key_top, lut_key, lut_vld_top, lut_vld, lut_act));
    lut_key = lut_key | lut_key_adjust( K_FlagUDP, 1);
    lut_vld = lut_vld | lut_vld_adjust(K_FlagUDP);
    lut_key = lut_key | lut_key_adjust( K_FlagIPv4, 1);
    lut_vld = lut_vld | lut_vld_adjust(K_FlagIPv4);
    retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_top, lut_vld_top, lut_act, i, VN_EG0_PTP+PTP_PORT_OFF(port));
    if(retVal!=DSDT_OK)
      return retVal;
    PTP_DBG_INFO((">>>Write igr lut entry %x : key %llx  vld %llx  act %llx\n", i, lut_key, lut_vld, lut_act));
  }

  return retVal; 
}


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
DSDT_STATUS ptp_1step_V2_IP6_UDP_init(int port, PTP_1STEP_CFG *ptp_cfg, int numIngLut, int numEgLut)
{
  DSDT_STATUS   retVal = DSDT_OK;

  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_top=0;
  unsigned long lut_vld_top=0;
  unsigned long long lut_act; 
  int i;

  retVal = ptp_lut_clear_udp (port);
  if(retVal!=DSDT_OK)
    return retVal;
  retVal = ptp_1step_V2_L2_init (port, ptp_cfg, numIngLut, numEgLut);
  if(retVal!=DSDT_OK)
    return retVal;

  for(i=0; i<numIngLut; i++)
  {
    retVal = read_lut_entry(port, &lut_key, &lut_vld, &lut_key_top, &lut_vld_top, &lut_act, i, VN_IG0_PTP+PTP_PORT_OFF(port));
    if(retVal!=DSDT_OK)
      return retVal;
    PTP_DBG_INFO((">>> read lut entry : key %x:%llx  vld %x:%llx  act %llx\n", 
	lut_key_top, lut_key, lut_vld_top, lut_vld, lut_act));
    lut_key = lut_key | lut_key_adjust( K_FlagUDP, 1);
    lut_vld = lut_vld | lut_vld_adjust(K_FlagUDP);
    lut_key = lut_key | lut_key_adjust( K_FlagIPv6, 1);
    lut_vld = lut_vld | lut_vld_adjust(K_FlagIPv6);
    retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_top, lut_vld_top, lut_act, i, VN_IG0_PTP+PTP_PORT_OFF(port));
    if(retVal!=DSDT_OK)
      return retVal;
    PTP_DBG_INFO((">>>Write igr lut entry %x : key %llx  vld %llx  act %llx\n", i, lut_key, lut_vld, lut_act));
  }
  for(i=0; i<numEgLut; i++)
  {
    retVal = read_lut_entry( port, &lut_key, &lut_vld, &lut_key_top, &lut_vld_top, &lut_act, i, VN_EG0_PTP+PTP_PORT_OFF(port));
    if(retVal!=DSDT_OK)
      return retVal;
    PTP_DBG_INFO((">>> read lut entry : key %x:%llx  vld %x:%llx  act %llx\n", 
	lut_key_top, lut_key, lut_vld_top, lut_vld, lut_act));
    lut_key = lut_key | lut_key_adjust( K_FlagUDP, 1);
    lut_vld = lut_vld | lut_vld_adjust(K_FlagUDP);
    lut_key = lut_key | lut_key_adjust( K_FlagIPv6, 1);
    lut_vld = lut_vld | lut_vld_adjust(K_FlagIPv6);
    retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_top, lut_vld_top, lut_act, i, VN_EG0_PTP+PTP_PORT_OFF(port));
    if(retVal!=DSDT_OK)
      return retVal;
    PTP_DBG_INFO((">>>Write igr lut entry %x : key %llx  vld %llx  act %llx\n", i, lut_key, lut_vld, lut_act));
  }

  return retVal; 
}

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
DSDT_STATUS ptp_1step_config(int port, PTP_1STEP_CFG *ptp_cfg, int numIngLut, int numEgLut)
{
  DSDT_STATUS   retVal = DSDT_OK;

 if (ptp_cfg[0].version==PTP_1STEP_VER_V2)
 {
  switch (ptp_cfg[0].overLayer)
  {
   case PTP_1STEP_OVER_L2:
     return ptp_1step_V2_L2_init (port, ptp_cfg, numIngLut, numEgLut);

   case PTP_1STEP_OVER_IP4_UDP:
     return ptp_1step_V2_IP4_UDP_init (port, ptp_cfg, numIngLut, numEgLut);

   case PTP_1STEP_OVER_IP6_UDP:
     return ptp_1step_V2_IP6_UDP_init (port, ptp_cfg, numIngLut, numEgLut);

   default:
     return DSDT_FAIL;
  }	
 }
 else
  return DSDT_FAIL;

 return retVal;
}

/*******************************************************************************
* new_ptp_init
*
* DESCRIPTION:
*		Completely Initialize new PTP block. It should be called before using any
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
DSDT_STATUS new_ptp_init (void* dev, int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int ver;
/*  int i; */
  PTP_DBG_DEMO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> init\n"));
  retVal = ptp_dev_init(dev);
/*	PTP_DBG_INFO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> disable_ptp\n")); */
/* 	disable_ptp(0); */
/* 	PTP_DBG_INFO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ptp_1step_set_smac_loopback\n")); */
/* 	ptp_1step_set_smac_loopback(); */

/* test only */
/*
  PTP_DBG_DEMO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ptp_test_clk_in_out_init\n"));
  for (i=0; i<4; i++)
  {
    retVal = ptp_test_clk_in_out_init(port, 0);
    if(retVal!=DSDT_OK) return retVal;
  }
*/
  PTP_DBG_DEMO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ptp_update_init\n"));
  retVal = ptp_update_init(port);
  if(retVal!=DSDT_OK) return retVal;
  PTP_DBG_DEMO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ptp_parser_init\n"));
  retVal = ptp_parser_init(port);
  if(retVal!=DSDT_OK) return retVal;
  PTP_DBG_DEMO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ptp_update_set_udata\n"));
 retVal = ptp_update_set_udata(port);
  if(retVal!=DSDT_OK) return retVal;
  PTP_DBG_DEMO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ptp_update_check_udata\n"));
  retVal = ptp_update_check_udata(port);
  if(retVal!=DSDT_OK) return retVal;
  PTP_DBG_DEMO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ptp_parser_set_udata\n"));
  retVal = ptp_parser_set_udata(port);
  if(retVal!=DSDT_OK) return retVal;
  PTP_DBG_DEMO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ptp_parser_check_udata\n"));
  retVal = ptp_parser_check_udata(port);
  if(retVal!=DSDT_OK) return retVal;
/*  PTP_DBG_DEMO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ptp_lut_clear\n"));
  retVal = ptp_lut_clear(port);
  if(retVal!=DSDT_OK) return retVal;
*/
  PTP_DBG_DEMO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ptp_lut_init\n"));
  retVal = ptp_lut_init(port);
  if(retVal!=DSDT_OK) return retVal;

  PTP_DBG_INFO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> correcting latadj & stt for fpga (100Mhz ptp core, var delay fifo)\n"));
  retVal = set_ptp_latadj(port, 0x1878);  /* For 1G speed */
/*  retVal = set_ptp_latadj(port, 0x1878);  For 100M speed*/
  if(retVal!=DSDT_OK) return retVal;
  retVal = set_ptp_stt_timer(port, 0x300);
  if(retVal!=DSDT_OK) return retVal;
  retVal = set_ptp_disp_timer(port, 0x300);
  if(retVal!=DSDT_OK) return retVal;
  PTP_DBG_INFO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> drift set to 0\n"));
  retVal = set_drift_adj(port, 0);
  if(retVal!=DSDT_OK) return retVal;
  retVal = set_drift_adj_step(port, 0);
  if(retVal!=DSDT_OK) return retVal;
  PTP_DBG_INFO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> wiremac ipg to 11\n"));
  retVal = set_wiremac_ipg(port, 11);
  if(retVal!=DSDT_OK) return retVal;
  PTP_DBG_INFO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> cut through mode\n"));
  retVal = set_cutthrough_mode(port);
  if(retVal!=DSDT_OK) return retVal;

  retVal = ptp_macsec_workaround_1();
  if(retVal!=DSDT_OK) return retVal;

  /* keep hard_drop bit and adjust hard_drop_size to 16B */
  retVal = set_hard_drop_size (port,14);
  if(retVal!=DSDT_OK) return retVal;

  PTP_DBG_INFO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> enable_ptp\n"));
  retVal = enable_ptp(port);
  if(retVal!=DSDT_OK) return retVal;
  PTP_DBG_DEMO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> disable macsec\n"));
  retVal = disable_macsec_both(port);
  if(retVal!=DSDT_OK) return retVal;
  ver = get_version_number(port);

  return retVal;
}

/*******************************************************************************
* new_ntp_init
*
* DESCRIPTION:
*		Completely Initialize new NTP block. It should be called before using any
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
DSDT_STATUS new_ntp_init (void* dev, int port, int udp_src_port_2, int udp_dst_port_2) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int ver;
  PTP_DBG_DEMO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> init\n"));
  retVal = ptp_dev_init(dev);
  PTP_DBG_DEMO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> disable_ptp\n"));
  disable_ptp(port); 
/*  PTP_DBG_DEMO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> set_smac_loopback\n"));
  ptp_1step_set_smac_loopback(port);
	#PTP_DBG_DEMO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> set_mac_bypass\n"));
	#set_mac_bypass
*/
  PTP_DBG_DEMO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ptp_update_init\n"));
  retVal = ptp_update_init(port);
  if(retVal!=DSDT_OK) return retVal;
  PTP_DBG_DEMO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ptp_parser_init\n"));
  retVal = ptp_parser_init(port);
  if(retVal!=DSDT_OK) return retVal;
  PTP_DBG_DEMO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ptp_update_set_udata_ntp\n"));
 retVal = ptp_update_set_udata_ntp(port);
  if(retVal!=DSDT_OK) return retVal;
  PTP_DBG_DEMO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ptp_update_check_udata_ntp\n"));
  retVal = ptp_update_check_udata_ntp(port);
  if(retVal!=DSDT_OK) return retVal;
  PTP_DBG_DEMO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ptp_parser_set_udata_ntp\n"));
  retVal = ptp_parser_set_udata_ntp(port);
  if(retVal!=DSDT_OK) return retVal;
  PTP_DBG_DEMO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ptp_parser_check_udata_ntp\n"));
  retVal = ptp_parser_check_udata_ntp(port);
  if(retVal!=DSDT_OK) return retVal;
  PTP_DBG_DEMO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ptp_lut_init_ntp\n"));
  retVal = ptp_lut_init_ntp(port);
  if(retVal!=DSDT_OK) return retVal;
  PTP_DBG_DEMO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ptp_lut_init_ntp_2\n"));
  retVal = ptp_lut_init_ntp_2(port, udp_src_port_2, udp_dst_port_2);
  if(retVal!=DSDT_OK) return retVal;
  PTP_DBG_DEMO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> set_tai_ntp_mode"));
  set_tai_ntp_mode(port);

  PTP_DBG_INFO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> correcting latadj & stt for fpga (100Mhz ptp core, var delay fifo)\n"));
  retVal = set_ptp_latadj(port, 0x1878);  /* For 1G speed */
/*  retVal = set_ptp_latadj(port, 0x1878);  For 100M speed*/
  if(retVal!=DSDT_OK) return retVal;
  retVal = set_ptp_stt_timer(port, 0x300);
  if(retVal!=DSDT_OK) return retVal;
  retVal = set_ptp_disp_timer(port, 0x300);
  if(retVal!=DSDT_OK) return retVal;

  PTP_DBG_INFO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> enable_ptp\n"));
  retVal = enable_ptp(port);
  if(retVal!=DSDT_OK) return retVal;
  PTP_DBG_DEMO((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> disable macsec\n"));
  retVal = disable_macsec_both(port);
  if(retVal!=DSDT_OK) return retVal;

  retVal = set_wmac_ipg (port, 11);
  if(retVal!=DSDT_OK) return retVal;

  ver = get_version_number(port);

/*	# keep hard_drop bit and adjust hard_drop_size to 14B */
  retVal = set_hard_drop_size (port,14);
  if(retVal!=DSDT_OK) return retVal;

  return retVal;
}

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
  OUT Ptp1StepTsStruct *ts)
{
  DSDT_STATUS   retVal = DSDT_OK;
  int numhwords;
  PtpTsQueueStruct tsQueue;
  int i;
  int numdata;
  PTP_1STEP_LUT_ACT action;

  if(queue == PTP_1STEP_QUEUE_HIGH)
  {
    numhwords = get_ptp_tsqh_usage(portnum, path);
    action = PTP_Act_StampHigh;
  }
  else
  {
    numhwords = get_ptp_tsql_usage(portnum, path);
    action = PTP_Act_StampLow;
  }
  numdata = numhwords;

/*  PTP_DBG_INFO(("@@@@@@@@@@@@  ptp_get_ts_tsq get number %x\n", numhwords)); */
  numhwords /= 7;

  if(numhwords == 0)
  {
    PTP_DBG_INFO(("TS queue is empty\n"));
#if 1
    if((numdata/7==0)&&(numdata%7))
    {
	  int idx =0;
      PtpTsQueueStruct tsQueue;
/*printf("!!!!!!!!!!! Clear invalid TS bytes %x  !!!!!!!!!!!!!\n", numdata%7);
*/
      retVal = ptp_find_lut_action(portnum, action, path, &idx); 
      if(retVal!=DSDT_OK) return DSDT_FAIL;
      retVal = ptp_enable_lut_action(portnum, idx, action, path,DSDT_FALSE); 
      if(retVal!=DSDT_OK) return DSDT_FAIL;
      if(queue == PTP_1STEP_QUEUE_HIGH)
        retVal = ptp_get_tsqh_part(portnum, path, numdata%7, &tsQueue);
	  else
        retVal = ptp_get_tsql_part(portnum, path, numdata%7, &tsQueue);

      if(retVal!=DSDT_OK) return DSDT_FAIL;
      retVal = ptp_enable_lut_action(portnum, idx, action, path,DSDT_TRUE); 
      if(retVal!=DSDT_OK) return DSDT_FAIL;
    }
#endif
    return DSDT_FAIL;
  }
  tsQueue.seqId=seqId - 1;
  i = 0;
  while (tsQueue.seqId<seqId)
  {
    if(queue == PTP_1STEP_QUEUE_HIGH)
      retVal = ptp_get_tsqh ( portnum, path, &tsQueue);
    else
      retVal = ptp_get_tsql ( portnum, path, &tsQueue);
    i++;
    if(i >= numhwords) 
      break;
    if((tsQueue.seqId==seqId)&&(tsQueue.msgId==msgId))
     break;
  }

/*printk("@@@@@@@@@@@@  ptp_get_ts_tsq get seqId %x, msgId %x\n", tsQueue.seqId, tsQueue.msgId==msgId);*/
  if((tsQueue.seqId==seqId)&&(tsQueue.msgId==msgId))
  {
    ts->extTs.nanoseconds = tsQueue.ts.extTs.nanoseconds;
    ts->extTs.topseconds = tsQueue.ts.extTs.topseconds;
    ts->extTs.seconds = tsQueue.ts.extTs.seconds;
/*printk("@@@@@@@@@@@ msg %x Queue TS Second %x:%x nanosecond %x\n", tsQueue.msgId, ts->extTs.topseconds, ts->extTs.seconds,ts->extTs.nanoseconds);*/

    return DSDT_OK;
  }
  else
  {
    ts->extTs.nanoseconds = 0;
    ts->extTs.topseconds = 0;
    ts->extTs.seconds = 0;
    return DSDT_FAIL;
  }
  return retVal;
}

#if 1
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
  OUT Ptp1StepTsStruct *ts)
{
  DSDT_STATUS   retVal = DSDT_OK;
  PtpTsQueueStruct tsQueue;
  int numdata;
  PTP_1STEP_LUT_ACT action;

  if(queue == PTP_1STEP_QUEUE_HIGH)
  {
    numdata = get_ptp_tsqh_usage(portnum, path);
    action = PTP_Act_StampHigh;
  }
  else
  {
    numdata = get_ptp_tsql_usage(portnum, path);
    action = PTP_Act_StampLow;
  }
  *numwords = numdata;
/*  PTP_DBG_INFO(("@@@@@@@@@@@@  ptp_get_ts_tsq get number %x\n", *numwords)); */
  *numwords /= 7;

  if(*numwords == 0)
  {
#if 1
    if((numdata/7==0)&&(numdata%7))
    {
	  int idx =0;
      PtpTsQueueStruct tsQueue;
/*printf("!!!!!!!!!!! Clear invalid TS bytes %x  !!!!!!!!!!!!!\n", numdata%7);
*/
      retVal = ptp_find_lut_action(portnum, action, path, &idx); 
      if(retVal!=DSDT_OK) return DSDT_FAIL;
      retVal = ptp_enable_lut_action(portnum, idx, action, path,DSDT_FALSE); 
      if(retVal!=DSDT_OK) return DSDT_FAIL;
      if(queue == PTP_1STEP_QUEUE_HIGH)
        retVal = ptp_get_tsqh_part(portnum, path, numdata%7, &tsQueue);
	  else
        retVal = ptp_get_tsql_part(portnum, path, numdata%7, &tsQueue);

      if(retVal!=DSDT_OK) return DSDT_FAIL;
      retVal = ptp_enable_lut_action(portnum, idx, action, path,DSDT_TRUE); 
      if(retVal!=DSDT_OK) return DSDT_FAIL;
    }
#endif
    PTP_DBG_INFO(("TS queue is empty\n"));
    ts->extTs.nanoseconds = 0;
    ts->extTs.topseconds = 0;
    ts->extTs.seconds = 0;
    return DSDT_FAIL;
  }
  {
    if(queue == PTP_1STEP_QUEUE_HIGH)
      retVal = ptp_get_tsqh ( portnum, path, &tsQueue);
    else
      retVal = ptp_get_tsql ( portnum, path, &tsQueue);
  }

/*printk("@@@@@@@@@@@@  ptp_get_ts_tsq get seqId %x, msgId %x\n", tsQueue.seqId, tsQueue.msgId==msgId);*/
  ts->extTs.nanoseconds = tsQueue.ts.extTs.nanoseconds;
  ts->extTs.topseconds = tsQueue.ts.extTs.topseconds;
  ts->extTs.seconds = tsQueue.ts.extTs.seconds;
/*printk("@@@@@@@@@@@ msg %x Queue TS Second %x:%x nanosecond %x\n", tsQueue.msgId, ts->extTs.topseconds, ts->extTs.seconds,ts->extTs.nanoseconds);*/
  *numwords = *numwords - 1;
    return DSDT_OK;
}
#endif
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
)
{
  DSDT_STATUS   retVal = DSDT_OK;
/*  set addr [expr $base_addr(TAI_CFG)+ [lsearch $ptp_tai_regs_list tod_cfg_gen] ] */
/*  unsigned int addr = ptp_1step_base_addr[TAI_CFG]+ tod_cfg_gen; */
  unsigned int addr = TOD_CFG_GEN;
  unsigned int regval = ptp_read_reg (port, addr);
  int field;
  unsigned long long field_val ;
  PTP_DBG_INFO(("%x %x\n", addr, regval));
/*  foreach field [array names tai_tod_cfg_gen_pos] { */
  field = 0;
  while (tai_tod_cfg_gen_pos[field]!=0) {
    field_val = tai_tod_cfg_gen_extract( field, regval);
    PTP_DBG_INFO(("TAI TOD CFG GEN field 0x%x", field_val));
    field++;
  }
	return retVal;
}

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
) 
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int addr = TOD_CFG_GEN;
  unsigned int regval = ptp_read_reg (port, addr);
  regval = tai_tod_cfg_gen_set_field(regval,  CLK_MODE, clk_mode);
  regval = tai_tod_cfg_gen_set_field(regval,  TAI_CLKOUT_SEL, 1);
  retVal = ptp_writecheck_reg(port, addr, regval);
/*
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_print_tai_tod_cfg_gen(port);
  if(retVal!=DSDT_OK) return retVal;
*/
  return retVal;
}

DSDT_STATUS set_ntp_tod_mode (int port, unsigned int ntp_mode){
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int addr = TOD_CFG_GEN;
  unsigned int regval = ptp_read_reg (port, addr);
  regval = tai_tod_cfg_gen_set_field(regval,  NTP_TOD_MODE, ntp_mode);
  retVal = ptp_writecheck_reg(port, addr, regval);
/*
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_print_tai_tod_cfg_gen(port);
  if(retVal!=DSDT_OK) return retVal;
*/
  return retVal;
}

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
) 
{
  DSDT_STATUS   retVal = DSDT_OK;
/*  set addr [expr $base_addr(TAI_CFG)+ [lsearch $ptp_tai_regs_list tod_cfg_gen] ] */
  unsigned int addr = TOD_CFG_GEN;
  unsigned int regval = ptp_read_reg (port, addr);
  regval = tai_tod_cfg_gen_set_field(regval,  SYNCE_CLK_SEL, refPort);
  regval = tai_tod_cfg_gen_set_field(regval,  TAI_CLKOUT_SEL, 1);
  retVal = ptp_writecheck_reg(port, addr, regval);
/*
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_print_tai_tod_cfg_gen(port);
  if(retVal!=DSDT_OK) return retVal;
*/
  return retVal;
}

/*******************************************************************************
* ptp_disable_tai_clk_gen
*
* DESCRIPTION:
*    Disable TAI from generating external clock signal, 
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
)
{
  DSDT_STATUS   retVal = DSDT_OK;
/*  set addr [expr $base_addr(TAI_CFG)+ [lsearch $ptp_tai_regs_list tod_cfg_gen] ] */
  unsigned int addr = TOD_CFG_GEN;
  unsigned int regval = ptp_read_reg (port, addr);
  regval = tai_tod_cfg_gen_set_field(regval,  CLK_MODE, 0);
  regval = tai_tod_cfg_gen_set_field(regval,  IN_CLK_EN, 0);
  regval = tai_tod_cfg_gen_set_field(regval,  TAI_CLKOUT_SEL, 1);
  retVal = ptp_writecheck_reg(port, addr, regval);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_print_tai_tod_cfg_gen(port);
  if(retVal!=DSDT_OK) return retVal;
	return retVal;
}

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
)
{
  DSDT_STATUS   retVal = DSDT_OK;
/*  set addr [expr $base_addr(TAI_CFG)+ [lsearch $ptp_tai_regs_list tod_cfg_gen] ] */
  unsigned int addr = TOD_CFG_GEN;
  unsigned int regval = ptp_read_reg (port, addr);
  regval = tai_tod_cfg_gen_set_field(regval,  CLK_MODE, 1);
  regval = tai_tod_cfg_gen_set_field(regval,  IN_CLK_EN, 1);
  regval = tai_tod_cfg_gen_set_field(regval,  TAI_CLKOUT_SEL, 1);
  retVal = ptp_writecheck_reg(port, addr, regval);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_print_tai_tod_cfg_gen(port);
  if(retVal!=DSDT_OK) return retVal;
	return retVal;
}

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
)
{
	DSDT_STATUS   retVal = DSDT_OK;
/*  set addr [expr $base_addr(TAI_CFG)+ [lsearch $ptp_tai_regs_list tod_cfg_gen] ] */
  unsigned int addr = TOD_CFG_GEN;
  unsigned int regval = ptp_read_reg (port, addr);
  regval = tai_tod_cfg_gen_set_field(regval,  CLK_MODE, 2);
  regval = tai_tod_cfg_gen_set_field(regval,  IN_CLK_EN, 1);
/*  regval = tai_tod_cfg_gen_set_field(regval,  TAI_CLKOUT_SEL 1] */
  retVal = ptp_writecheck_reg(port, addr, regval);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_print_tai_tod_cfg_gen(port);
  if(retVal!=DSDT_OK) return retVal;
	return retVal;
}

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
)
{
  DSDT_STATUS   retVal = DSDT_OK;
/*  set addr [expr $base_addr(TAI_CFG)+ [lsearch $ptp_tai_regs_list tod_cfg_gen] ] */
  unsigned int addr = TOD_CFG_GEN;
  unsigned int regval = ptp_read_reg (port, addr);
  regval = tai_tod_cfg_gen_set_field(regval,  CLK_MODE, 3);
  regval = tai_tod_cfg_gen_set_field(regval,  IN_CLK_EN, 1);
/*  regval = tai_tod_cfg_gen_set_field(regval,  tai_clkout_sel 1] */
  retVal = ptp_writecheck_reg(port, addr, regval);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_print_tai_tod_cfg_gen(port);
  if(retVal!=DSDT_OK) return retVal;
	return retVal;
}

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
)
{
  DSDT_STATUS   retVal = DSDT_OK;
/*  set addr [expr $base_addr(TAI_CFG)+ [lsearch $ptp_tai_regs_list tod_func_cfg] ] */
  unsigned int addr = TOD_FUNC_CFG;
  unsigned int regval = ptp_read_reg (port, addr);
  regval = tai_tod_func_cfg_set_field(regval, TAI_TOD_TOD_FUNC_TRIG, 1);
/* PTP_DBG_DEMO(("ptp_tai_trig_load_timer set TOD_FUNC_TRIG %x\n", regval));  */

  retVal = ptp_writecheck_reg(port, addr, regval);
  if(retVal!=DSDT_OK) return retVal;
  regval = tai_tod_func_cfg_set_field(regval, TAI_TOD_TOD_FUNC_TRIG, 0);
  retVal = ptp_writecheck_reg(port, addr, regval);

#if 0
  if(retVal!=DSDT_OK) return retVal;
  PTP_DBG_DEMO(("ptp_tai_trig_load_timer\n"));
  ptp_1step_print_tai_tod_func_cfg(port);
#endif
	return retVal;
}

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
)
{
  DSDT_STATUS   retVal = DSDT_OK;
#ifdef DEBUG_PTP
/*  set addr [expr $base_addr(TAI_CFG)+ [lsearch $ptp_tai_regs_list tai_intr] ] */
  unsigned int addr = TAI_INTR;
  unsigned int regval = ptp_read_reg (port, addr);
  PTP_DBG_INFO(("trig_gen_int  %x", (regval >> 0)&1));
  PTP_DBG_INFO(("pulse_in_int  %x", (regval >> 1)&1));
  PTP_DBG_INFO(("clk_gen_int   %x", (regval >> 2)&1));
  PTP_DBG_INFO(("req_drift_int %x", (regval >> 3)&1));
#endif
	return retVal;
}
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
)
{
  DSDT_STATUS   retVal = DSDT_OK;
/*  set addr [expr $base_addr(TAI_CFG)+ [lsearch $ptp_tai_regs_list drift_thr_cfg] ] */
  unsigned int addr = DRIFT_THR_CFG;
  retVal = ptp_writecheck_reg(port, addr, ((minimal_drift<<24)&0xff)|(drift_thr&0xffffff));
  return retVal;
}

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
)
{
  DSDT_STATUS   retVal = DSDT_OK;
/*  set addr [expr $base_addr(TAI_CFG)+ [lsearch $ptp_tai_regs_list clock_cyc] ] */
  unsigned int addr = CLOCK_CYC;
  retVal = ptp_writecheck_reg(port, addr, (cyc_ns<<2)|(cyc_s&0x3));
  return retVal;
}

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
)
{
  DSDT_STATUS   retVal = DSDT_OK;
  /* Disable TAI clock gen first, start internal clock */
  retVal = ptp_disable_tai_clk_gen(port);
  if(retVal!=DSDT_OK) return retVal;
  /* Setup time registers */
  retVal = set_ptp_tai_timereg(port,  REG_TOD_0, 0, 0, 0, 0);  
  if(retVal!=DSDT_OK) return retVal;
/*  retVal = set_ptp_tai_timereg(port, REG_NEXT_RT_0, 0, 30, 0, 0);  */
  retVal = set_ptp_tai_timereg(port, REG_NEXT_RT_0, ntTime->highsec,                                ntTime->lowsec, ntTime->nanosec, ntTime->subnano);  
  if(retVal!=DSDT_OK) return retVal;
  /* setup clock cycle */
/*  retVal = ptp_set_clock_cyc(port, 2, 0); */
  retVal = ptp_set_clock_cyc(port, cyc->lowsec, cyc->nanosec);
  if(retVal!=DSDT_OK) return retVal;
  /* trigger loading of NEXT RT */
  /* Start TAI clock gen */
  retVal = ptp_set_tai_clk_gen(port);
  if(retVal!=DSDT_OK) return retVal;
	return retVal;
}

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
)
{
  DSDT_STATUS   retVal = DSDT_OK;
  /* Disable TAI clock gen first, start internal clock */
  retVal = ptp_disable_tai_clk_gen(port);
  if(retVal!=DSDT_OK) return retVal;
  /* Setup time registers */
/*  retVal = set_ptp_tai_timereg(port, REG_TOD_0, 0xabcd, 0x12345678, 0, 0);  */
  retVal = set_ptp_tai_timereg(port, REG_TOD_0, todTime->highsec,                                todTime->lowsec, todTime->nanosec, todTime->subnano);  
  if(retVal!=DSDT_OK) return retVal;
  /*  retVal = set_ptp_tai_timereg(port, REG_NEXT_RT_0, 0, 30, 0);  */
  retVal = set_ptp_tai_timereg(port, REG_NEXT_RT_0, ntTime->highsec, 	                                ntTime->lowsec, ntTime->nanosec, ntTime->subnano);  
if(retVal!=DSDT_OK) return retVal;
  /* setup clock cycle */
/*  retVal = ptp_set_clock_cyc(port, 2, 0); */
  retVal = ptp_set_clock_cyc(port, cyc->lowsec, cyc->nanosec);
  if(retVal!=DSDT_OK) return retVal;
 
  /* Start TAI clock reception */
  retVal = ptp_set_tai_clk_rec(port);

  return retVal;
}

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
)
{
  DSDT_STATUS   retVal = DSDT_OK;
  retVal = set_ptp_tai_reg(port,  REG_PCLK_CFG, (period << 1) | 1);
  return retVal;
}
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
)
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int period = (1000000000/freq);
  PTP_DBG_INFO(("Setting PCLK frequency to %x HZ", freq));
  retVal = ptp_set_pclk_period(port, period-8);
	return retVal;
}

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
)
{
  DSDT_STATUS   retVal = DSDT_OK;
  /* fs = ppm*(8ns)*(2^32)/(1000000) */
  unsigned int  drift_adj_fs = (ppm*3435974/100);
  unsigned int  drift_adj_step;
  if (ppm < 0) {
    drift_adj_step = (drift_adj_fs)&0xffffffff;
    retVal = set_ptp_tai_reg(port,  REG_TOD_STEP_NS, 7);
    if(retVal!=DSDT_OK) return retVal;
  } else {
    drift_adj_step = (drift_adj_fs)&0xffffffff;
    retVal = set_ptp_tai_reg(port,  REG_TOD_STEP_NS, 8);
    if(retVal!=DSDT_OK) return retVal;
  }
  PTP_DBG_INFO(("Setting drift to 0x%x (%x ppm)", drift_adj_step, ppm));
  retVal = set_drift_adj_step(port,  drift_adj_step);
	return retVal;
}

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
)
{
  DSDT_STATUS   retVal = DSDT_OK;
/*  set addr [expr $base_addr(TAI_CFG)+ [lsearch $ptp_tai_regs_list pulse_delay] ] */
  unsigned int addr = PULSE_DELAY;
  unsigned int cnt = ptp_read_reg (port, addr);
  PTP_DBG_INFO(("%08h (%x)", cnt, cnt));
  *pdelay = cnt;

	return retVal;
}

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
)
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int addr = TOD_CFG_GEN;
  retVal = ptp_writecheck_reg(port, addr, kpTodCfg);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_print_tai_tod_cfg_gen(port);
  return retVal;
}

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
)
{
  DSDT_STATUS   retVal = DSDT_OK;
/*  set addr [expr $base_addr(TAI_CFG)+ [lsearch $ptp_tai_regs_list tod_cfg_gen] ] */
  unsigned int addr = TOD_CFG_GEN;
  unsigned int regval = ptp_read_reg (port, addr);
  *kpTodCfg = regval;
  regval = tai_tod_cfg_gen_set_field(regval,  CLK_MODE, 0);
  regval = tai_tod_cfg_gen_set_field(regval,  IN_CLK_EN, 0);
  regval = tai_tod_cfg_gen_set_field(regval,  EN_SYNCE_TAI_CLK, 0);
  regval = tai_tod_cfg_gen_set_field(regval,  EXT_CLK_EN, 0);
  retVal = ptp_writecheck_reg(port, addr, regval);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_print_tai_tod_cfg_gen(port);
  return retVal;
}

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
)
{
  DSDT_STATUS   retVal = DSDT_OK;
/*  set addr [expr $base_addr(TAI_CFG)+ [lsearch $ptp_tai_regs_list tod_cfg_gen] ] */
  unsigned int addr = TOD_CFG_GEN;
  unsigned int regval = ptp_read_reg (port, addr);
  regval = tai_tod_cfg_gen_set_field(regval,  CLK_MODE, clk_mode);
  regval = tai_tod_cfg_gen_set_field(regval,  IN_CLK_EN, in_clk_en);
  regval = tai_tod_cfg_gen_set_field(regval,  TAI_CLKOUT_SEL, tai_clkout_sel);
  retVal = ptp_writecheck_reg(port, addr, regval);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_print_tai_tod_cfg_gen(port);
  if(retVal!=DSDT_OK) return retVal;
	return retVal;
}

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
)
{
  unsigned int addr = TOD_FUNC_CFG;
  unsigned int regval = ptp_read_reg (port, addr);
  *timerOp = tai_tod_func_cfg_extract( TAI_TOD_TOD_FUNC, regval);
  return DSDT_OK;
}

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
)
{
  DSDT_STATUS   retVal = DSDT_OK;
/*  set addr [expr $base_addr(TAI_CFG)+ [lsearch $ptp_tai_regs_list tod_func_cfg] ] */
  unsigned int addr = TOD_FUNC_CFG;
  unsigned int regval = ptp_read_reg (port, addr);
  regval = tai_tod_func_cfg_set_field(regval, TAI_TOD_TOD_FUNC, timerOp);
  retVal = ptp_writecheck_reg(port, addr, regval);
#if 0
  regval = ptp_read_reg (port, addr);
  PTP_DBG_DEMO(("ptp_tai_set_timer_operation read back 0x%08x\n", regval));
/*  ptp_1step_print_tai_tod_func_cfg(port); */
#endif 
  return retVal;
}

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
)
{
  DSDT_STATUS   retVal = DSDT_OK;
/*  set addr [expr $base_addr(TAI_CFG)+ [lsearch $ptp_tai_regs_list tod_func_cfg] ] */
  unsigned int addr = TOD_FUNC_CFG;
  unsigned int regval = ptp_read_reg (port, addr);
  regval = tai_tod_func_cfg_set_field(regval, TAI_TOD_PULSE_WIDTH, (width_ns/PTP_CYCLE)+1);
  retVal = ptp_writecheck_reg(port, addr, regval);
#if 0
  if(retVal!=DSDT_OK) return retVal;
  PTP_DBG_DEMO(("ptp_tai_set_pulse_width\n"));
  ptp_1step_print_tai_tod_func_cfg(port);
#endif 
	return retVal;
}

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
)
{
  DSDT_STATUS   retVal = DSDT_OK;
/*  set addr [expr $base_addr(TAI_CFG)+ [lsearch $ptp_tai_regs_list tod_func_cfg] ] */
  unsigned int addr = TOD_FUNC_CFG;
  unsigned int regval = ptp_read_reg (port, addr);
  *width_ns = tai_tod_func_cfg_extract( TAI_TOD_PULSE_WIDTH, regval);
  if(*width_ns>0)
  {
    *width_ns -= 1;
    *width_ns *= PTP_CYCLE;
  }
  else
  {
    *width_ns = 0;
  }
  return retVal;
}

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
)
{
  DSDT_STATUS   retVal = DSDT_OK;
/*  set addr [expr $base_addr(TAI_CFG)+ [lsearch $ptp_tai_regs_list tod_func_cfg] ] */
  unsigned int addr = TOD_FUNC_CFG;
  unsigned int regval = ptp_read_reg (port, addr);
  regval = tai_tod_func_cfg_set_field(regval, TAI_TOD_TRIG_GEN_EN, bitval);
  retVal = ptp_writecheck_reg(port, addr, regval);
#if 0
  if(retVal!=DSDT_OK) return retVal;
  PTP_DBG_DEMO(("ptp_tai_set_trig_gen_en\n"));
  ptp_1step_print_tai_tod_func_cfg(port);
#endif
	return retVal;
}

static unsigned int ptpGetMask(int value)
{
  unsigned int data = 0;
  unsigned int mask = 0;
  while (value)
  {
    data +=1;
	value>>=1;
  }
  if (data)
  {
/*    mask =1<<data; */
    mask =1<<(data-1);
    mask = mask - 1;
  }
  return mask;
}

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
DSDT_STATUS ptp_get_pulse_gen (int port, int*width_ns, unsigned int *trigsec, unsigned int *trignsec)
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int sechi, fnsec;

  /* get pulse length */
  retVal = ptp_tai_get_pulse_width(port,  width_ns);
  if(retVal!=DSDT_OK) return retVal;
/*PTP_DBG_DEMO(("trig pulse width %x nsec\n", *width_ns)); */

  retVal = get_ptp_tai_timereg(port, REG_TRIG_GEN_TOD_0,&sechi, trigsec, trignsec, &fnsec);
  if(retVal!=DSDT_OK) return retVal;
/*PTP_DBG_DEMO(("trig time %x %x sec %x nsec %x fsec\n", sechi, *trigsec, *trignsec, fnsec)); */

  return retVal;
}


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
DSDT_STATUS ptp_go_pulse_gen (int port, int width_ns, int trigsec, int trignsec)
{
  DSDT_STATUS   retVal = DSDT_OK;
  int sechi, seclo, nsec, fnsec, secMask, nsecMask;
  unsigned int kpTodCfg;

  /* disable tai  */
  retVal = ptp_disable_tai(port, &kpTodCfg);
  if(retVal!=DSDT_OK) return retVal;

  /* set pulse length */
  retVal = ptp_tai_set_pulse_width(port,  width_ns);
  if(retVal!=DSDT_OK) return retVal;

  /* set trig_gen_en */
  retVal = ptp_tai_set_trig_gen_en(port,  1);
  if(retVal!=DSDT_OK) return retVal;

  /* set Trig TOD */
  sechi = 0;
  seclo = trigsec;
  nsec = trignsec;
  fnsec = 0;

PTP_DBG_DEMO(("trig time %x %x sec %x nsec %x fsec\n", sechi, seclo, nsec, fnsec));
  retVal = set_ptp_tai_timereg(port, REG_TRIG_GEN_TOD_0, sechi, seclo, nsec, fnsec);
  if(retVal!=DSDT_OK) return retVal;

  /* set TGT mask*/
  secMask=ptpGetMask(trigsec);
  if((secMask)||(trigsec))
  {
	  nsecMask=0xffffffff;
  }
  else
  {
    nsecMask=ptpGetMask(trignsec);
  }

  nsecMask = nsecMask & ~( ptpGetMask(width_ns));

PTP_DBG_DEMO(("mask time %x %x sec %x nsec %x fsec\n", 0, secMask, nsecMask, 0));
  retVal = set_ptp_tai_timereg(port, REG_TRIG_GEN_MSK_0, 0, secMask, nsecMask, 0);
  if(retVal!=DSDT_OK) return retVal;
  /* enable tai pulse generation with internal clock */
  retVal = ptp_enable_tai(port, kpTodCfg);
/*  retVal = ptp_set_tai(port, 0, 0, 0); */
  if(retVal!=DSDT_OK) return retVal;

  return retVal;
}


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
DSDT_STATUS ptp_go_pulse_rec (int port, int delay, PtpTimeStruct *nextTime, PtpTimeStruct * ldTime) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int kpTodCfg;
  /* disable tai */
  retVal = ptp_disable_tai(port, &kpTodCfg);
  if(retVal!=DSDT_OK) return retVal;
  /* clear pulse counter */
  retVal = set_tai_pulse_in_cnt(port, 0);
  if(retVal!=DSDT_OK) return retVal;
  /* set the pulse delay (in ns) */
/*  retVal = set_tai_pulse_delay(port, 1000000000); */
  retVal = set_tai_pulse_delay(port, delay);
  if(retVal!=DSDT_OK) return retVal;
  /* set the NRT with a bogus value */
/*  retVal = set_ptp_tai_timereg(port,  REG_NEXT_RT_0, 0xa1a2a3a4, 0xb1b2b3b4, 0xc1c2c3c4, 0); */
  retVal = set_ptp_tai_timereg(port,  REG_NEXT_RT_0, nextTime->sec_hi, nextTime->sec_lo, nextTime->nanosec, nextTime->frec);
  if(retVal!=DSDT_OK) return retVal;
  /* set TLV with the same value set in the TGT of the master */
/*  retVal = set_ptp_tai_timereg(port, REG_TOD_LOAD_0, 0, 10, 0, 0); */
  retVal = set_ptp_tai_timereg(port, REG_TOD_LOAD_0, ldTime->sec_hi, ldTime->sec_lo, ldTime->nanosec, ldTime->frec);
  if(retVal!=DSDT_OK) return retVal;
  /* enable tai pulse reception with internal clock (is this really needed?) Need to set correct pulse_in_sel !! */
  /* set_tai 2 1 0 */
  
  /* make sure that the NRT is set with TLV+pulse_delay when the pulse arrives */
  /* enable tai pulse generation with internal clock */
  retVal = ptp_enable_tai(port, kpTodCfg);
/*  retVal = ptp_set_tai(port, 0, 0, 0); */
  if(retVal!=DSDT_OK) return retVal;

  return retVal;
}

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
)
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int kpTodCfg;
  /* disable tai */
  retVal = ptp_disable_tai(port, &kpTodCfg);
  if(retVal!=DSDT_OK) return retVal;

  retVal = _set_ptp_tai_tod (port, ns, sec_lo, sec_hi);
  if(retVal!=DSDT_OK) return retVal;

  /* enable tai pulse generation with internal clock */
  retVal = ptp_enable_tai(port, kpTodCfg);
/*  retVal = ptp_set_tai(port, 0, 0, 0); */
  if(retVal!=DSDT_OK) return retVal;
	return retVal;
  return retVal;
}


/*******************************************************************************
* ptp_set_drift_adj
*
* DESCRIPTION:
*       Clock drift adjustment
*
* INPUTS:
*       port - port number
*       drift_base_adj_sl - base drift adjust low sec
*       drift_base_adj_ns - base drift adjust ns (include sign)
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
#if 0
DSDT_STATUS ptp_set_drift_adj
(
  IN int port,
  IN int drift_base_adj_ns,
  IN int drift_base_adj_fs,
  IN int drift_fine_adj_fs
)
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int kpTodCfg;
PTP_DBG_DEMO(("\n!!!!!^^^^^^^^^^^^^^^^^^^^^^^^^^^^!!!!!!!!!!!!!!!!!!!\n")); 
PTP_DBG_DEMO(("!!!!! ptp_set_drift_adj %x %x %x\n", drift_base_adj_ns, drift_base_adj_fs, drift_fine_adj_fs)); 
  /* disable tai */
  retVal = ptp_disable_tai(port, &kpTodCfg);
  if(retVal!=DSDT_OK) return retVal;

  if(!((drift_base_adj_ns==0)&& (drift_base_adj_fs==0)))
  {
    retVal = set_ptp_tai_timereg(port, REG_TOD_LOAD_0, 0, 0, drift_base_adj_ns, drift_base_adj_fs);
    if(retVal!=DSDT_OK) return retVal;
   PTP_DBG_DEMO(("set_ptp_tai_timereg TOD_LOAD read 0x%08x 0x%08x 0x%08x 0x%08x\n", ptp_read_reg (port, TOD_LOAD_0),  ptp_read_reg (port, TOD_LOAD_1), ptp_read_reg (port, TOD_LOAD_2),  ptp_read_reg (port, TOD_LOAD_3)));
 


    if(drift_base_adj_fs>0)
    {
      retVal = ptp_tai_set_timer_operation(port, TOD_FUNC_TIMER_INC);
    }
    else
    {
      retVal = ptp_tai_set_timer_operation(port, TOD_FUNC_TIMER_DEC);
/*      drift_base_adj_fs = -drift_base_adj_fs; */
    }
    PTP_DBG_DEMO(("ptp_tai_set_timer_operation read 2 0x%08x\n", ptp_read_reg (port, TOD_FUNC_CFG))); 
  
    retVal = ptp_tai_trig_load_timer(port);
    if(retVal!=DSDT_OK) return retVal;
  }
  if(!((drift_fine_adj_fs==0)))
  {
    retVal = set_drift_adj(port, drift_fine_adj_fs);
    if(retVal!=DSDT_OK) return retVal;
    retVal = set_drift_adj_step(port,  0);
  }

  /* enable tai pulse generation with internal clock */
  retVal = ptp_enable_tai(port, kpTodCfg);
/*  retVal = ptp_set_tai(port, 0, 0, 0); */
  if(retVal!=DSDT_OK) return retVal;

  return retVal;
}
#else
DSDT_STATUS ptp_set_drift_adj
(
  IN int port,
  IN int drift_base_adj_sl,
  IN int drift_base_adj_ns,
  IN int drift_base_adj_fs,
  IN int drift_fine_adj_fs
)
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int kpTodCfg;
  /* disable tai */
  retVal = ptp_disable_tai(port, &kpTodCfg);
  if(retVal!=DSDT_OK) return retVal;
/*
PTP_DBG_DEMO(("\n!!!!!^^^^^^^^^^^^^^^^^^^^^^^^^^^^!!!!!!!!!!!!!!!!!!!\n")); 
#ifdef DEBUG_MAD
printf("!!!!! ptp_set_drift_adj %x %x %x %x\n", drift_base_adj_sl, drift_base_adj_ns, drift_base_adj_fs, drift_fine_adj_fs); 
#else
printk("!!!!! ptp_set_drift_adj %x %x %x %x\n", drift_base_adj_sl, drift_base_adj_ns, drift_base_adj_fs, drift_fine_adj_fs); 
#endif
PTP_DBG_DEMO(("!!!!! ptp_set_drift_adj %x %x %x %x\n", drift_base_adj_sl, drift_base_adj_ns, drift_base_adj_fs, drift_fine_adj_fs)); 
*/

/*PTP_DBG_DEMO(("\n!!!!!! disable TAI done !!!!!!!!!\n"));  */
  if(!((drift_base_adj_sl==0)&& (drift_base_adj_ns==0)&& (drift_base_adj_fs==0)))
  {
    int ns;
    ns = (drift_base_adj_ns>0)?drift_base_adj_ns:-drift_base_adj_ns;
    retVal = set_ptp_tai_timereg(port, REG_TOD_LOAD_0, 0, drift_base_adj_sl, ns, drift_base_adj_fs);
    if(retVal!=DSDT_OK) return retVal;
/*   PTP_DBG_DEMO(("set_ptp_tai_timereg TOD_LOAD read 0x%08x 0x%08x 0x%08x 0x%08x\n", ptp_read_reg (port, TOD_LOAD_0),  ptp_read_reg (port, TOD_LOAD_1), ptp_read_reg (port, TOD_LOAD_2),  ptp_read_reg (port, TOD_LOAD_3))); */
 
{
  unsigned int addr = TOD_FUNC_CFG;
  unsigned int regval = ptp_read_reg (port, addr);
    regval = regval & ~0xc0000000 ;
    if(drift_base_adj_ns>0)
    {
/*      regval = regval | 0x40000000 ; */
      regval = regval | 0x80000000 ;
    }
    else
    {
      regval = regval | 0x40000000 ;
/*      regval = regval | 0x80000000  */;
    }
  
  regval = regval | 0x10000000 ; 
PTP_DBG_DEMO(("!!!!! ptp_set_drift_adj trig config %x addr %xn", regval, addr)); 
  retVal = ptp_writecheck_reg(port, addr, regval);

  if(retVal!=DSDT_OK) return retVal;
  regval = regval & ~0x10000000 ;
  retVal = ptp_writecheck_reg(port, addr, regval);


}

  }

  if(!((drift_fine_adj_fs==0)))
  {

   static int last_drift_fine_adj_fs;
/*
#ifdef DEBUG_MAD
printf("\n!!!!!set_drift_adj %x !!!!!!!!!\n", drift_fine_adj_fs); 
#else
printk("\n!!!!!set_drift_adj %x !!!!!!!!!\n", drift_fine_adj_fs); 
#endif
*/
    if(drift_fine_adj_fs!=last_drift_fine_adj_fs) 
    {
/*   PTP_DBG_DEMO(("\n************ set_drift_adj 0x%08x \n", drift_fine_adj_fs)); */
      retVal = set_drift_adj(port, drift_fine_adj_fs);
      if(retVal!=DSDT_OK) return retVal; 
      last_drift_fine_adj_fs = drift_fine_adj_fs;
    }
/*    retVal = set_drift_adj_step(port,  0); */
  }

  /* enable tai pulse generation with internal clock */
  retVal = ptp_enable_tai(port, kpTodCfg); 
/*  retVal = ptp_set_tai(port, 0, 0, 0);  */
  if(retVal!=DSDT_OK) return retVal;
	return retVal;

  return retVal;
}
#endif
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
*       drift_base_adj_ns - base drift adjust ns (include sign)
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
/* set drift adjustment */
DSDT_STATUS ptp_get_drift_adj
(
  IN int port,
  OUT int *drift_base_adj_sl,
  OUT int *drift_base_adj_ns,
  OUT int *drift_base_adj_fs,
  OUT int *drift_fine_adj_fs
)
{
  DSDT_STATUS   retVal = DSDT_OK;
  TOD_FUNC_TIMER_OP  timeOp;
  unsigned int sec_hi;
/*PTP_DBG_DEMO(("!!!!! ptp_get_drift_adj %x %x\n", drift_base_adj_fs,drift_fine_adj_fs)); */

  retVal = get_ptp_tai_timereg(port, REG_TOD_LOAD_0, &sec_hi, (unsigned int *) drift_base_adj_sl,(unsigned int *) drift_base_adj_ns, (unsigned int *)drift_base_adj_fs);
  if(retVal!=DSDT_OK) return retVal;
/*PTP_DBG_DEMO(("get_ptp_tai_timereg TOD_LOAD read 0x%08x 0x%08x 0x%08x 0x%08x\n", sec_hi, sec_lo, *drift_base_adj_ns, *drift_base_adj_fs)); */
  retVal = ptp_tai_get_timer_operation(port, &timeOp);

/*PTP_DBG_DEMO(("!!!!! ptp_tai_get_timer_operation %x\n", timeOp)); */ 
  if(timeOp==TOD_FUNC_TIMER_DEC)
  {
    *drift_base_adj_ns = -(*drift_base_adj_ns);
  }

  retVal = get_drift_adj(port, (unsigned int *)drift_fine_adj_fs);
/*PTP_DBG_DEMO(("!!!!! get_drift_adj %x\n", drift_fine_adj_fs));  */
PTP_DBG_DEMO(("!!!!! ptp_get_drift_adj %x %x %x %x\n", *drift_base_adj_sl, *drift_base_adj_ns, *drift_base_adj_fs, *drift_fine_adj_fs)); 
  return retVal;
}

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
DSDT_STATUS get_ptp_intr_msk ( int portnum, PTP_Blk path, unsigned int *mask)
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short reg_addr;
  PTP_TYPE base_addr_id;
  unsigned int data;

  PTP_DBG_TRACE(("Get PTP Interrupt mask (port %x, path %x)\n", portnum, path));
  base_addr_id = get_ptp_base_addr_id( portnum, path);
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_RF + PTP_INTR_SET_MSK;

  data = ptp_read_reg (portnum, reg_addr);
  data &= 0xffff;
  *mask = data;
  
  return retVal;
}

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
DSDT_STATUS set_ptp_intr_msk ( int portnum, PTP_Blk path, unsigned int mask)
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short reg_addr;
  PTP_TYPE base_addr_id;
  unsigned int data;

  PTP_DBG_TRACE(("Set PTP Interrupt mask (port %x, path %x mask %x)\n", portnum, path,mask));
  base_addr_id = get_ptp_base_addr_id( portnum, path);
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_RF + PTP_INTR_SET_MSK;

  data = ptp_read_reg (portnum, reg_addr);
  data &= 0xffff;
  PTP_DBG_DEMO(("port%x path %x old int_set_msk %04x = %08x\n", portnum, path, reg_addr, data));
  retVal = ptp_write_reg(portnum, reg_addr,mask);
  data = ptp_read_reg (portnum, reg_addr);
  data &= 0xffff;
  PTP_DBG_DEMO(("port%x path %x new int_set_msk %04x = %08x\n", portnum, path, reg_addr, data));
  
  return retVal;
}

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
DSDT_STATUS disable_ptp_watchdog_timer ( int portnum, PTP_Blk path)
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short reg_addr;
  PTP_TYPE base_addr_id;

  PTP_DBG_TRACE(("Set PTP watchdog timer disable (port %x, path %x)\n", portnum, path));
  base_addr_id = get_ptp_base_addr_id( portnum, path);
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_RF + PTP_TIMEOUT_CTL;

  retVal = ptp_write_reg(portnum, reg_addr,0);
  
  return retVal;
}

#ifdef bit_get
#define nptp_bit_get bit_get
#else
#define NPTP_BIT(x)           ((unsigned int)0x00000001 << (x))
#define nptp_bit_get(p,x)     (((p) &  (NPTP_BIT(x))) >> (x))
#endif

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
DSDT_STATUS get_ptp_interrupts (int portnum, unsigned int *igrInt, unsigned int *egrInt)
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short reg_addr;
  PTP_TYPE base_addr_id;
  unsigned int data;

  PTP_DBG_TRACE(("Get PTP Interrupt (port %x)\n", portnum));
  base_addr_id = get_ptp_base_addr_id( portnum, PTP_igr);
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_RF + PTP_INTR_SET;

  data = ptp_read_reg (portnum, reg_addr);
  *igrInt = data&0xffff;
  PTP_DBG_DEMO(("port%x igr int_set %04x = %08x\n", portnum, reg_addr, data));
  PTP_DBG_DEMO(("%04x %04x\n", reg_addr, data));
  PTP_DBG_DEMO(("test %X\n", (unsigned int)nptp_bit_get(data, 15)));
  PTP_DBG_DEMO(("ppd %X\n", (unsigned int)nptp_bit_get(data, 14)));
  PTP_DBG_DEMO(("ppe %X\n", (unsigned int)nptp_bit_get(data, 13)));
  PTP_DBG_DEMO(("late_dispatch %X\n", (unsigned int)nptp_bit_get(data, 11)));
  PTP_DBG_DEMO(("stat_full %X\n", (unsigned int)nptp_bit_get(data, 10)));
  PTP_DBG_DEMO(("stat_almost_full %X\n", (unsigned int)nptp_bit_get(data, 9)));
  PTP_DBG_DEMO(("pbuff_stat %X", (unsigned int)msec_bits_get(data, 8, 4)));
  PTP_DBG_DEMO(("ts_low_act %X\n", (unsigned int)nptp_bit_get(data, 3)));
  PTP_DBG_DEMO(("ts_high_act %X\n", (unsigned int)nptp_bit_get(data, 2)));
  PTP_DBG_DEMO(("ts_low_full %X\n", (unsigned int)nptp_bit_get(data, 1)));
  PTP_DBG_DEMO(("ts_high_full %X\n", (unsigned int)nptp_bit_get(data, 0)));
  base_addr_id = get_ptp_base_addr_id( portnum, PTP_egr);
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_RF + PTP_INTR_SET;

  data = ptp_read_reg (portnum, reg_addr);
  *egrInt = data&0xffff;
  PTP_DBG_DEMO(("port%x egr int_set %04x = %08x\n", portnum, reg_addr, data));
  PTP_DBG_DEMO(("%04x %04x", reg_addr, data));
  PTP_DBG_DEMO(("%04x %04x", reg_addr, data));
  PTP_DBG_DEMO(("test %X\n", (unsigned int)nptp_bit_get(data, 15)));
  PTP_DBG_DEMO(("ppd %X\n", (unsigned int)nptp_bit_get(data, 14)));
  PTP_DBG_DEMO(("ppe %X\n", (unsigned int)nptp_bit_get(data, 13)));
  PTP_DBG_DEMO(("late_dispatch %X\n", (unsigned int)nptp_bit_get(data, 11)));
  PTP_DBG_DEMO(("stat_full %X\n", (unsigned int)nptp_bit_get(data, 10)));
  PTP_DBG_DEMO(("stat_almost_full %X\n", (unsigned int)nptp_bit_get(data, 9)));
  PTP_DBG_DEMO(("pbuff_stat %X", (unsigned int)msec_bits_get(data, 8, 4)));
  PTP_DBG_DEMO(("ts_low_act %X\n", (unsigned int)nptp_bit_get(data, 3)));
  PTP_DBG_DEMO(("ts_high_act %X\n", (unsigned int)nptp_bit_get(data, 2)));
  PTP_DBG_DEMO(("ts_low_full %X\n", (unsigned int)nptp_bit_get(data, 1)));
  PTP_DBG_DEMO(("ts_high_full %X\n", (unsigned int)nptp_bit_get(data, 0)));
  return retVal;
}


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
DSDT_STATUS clear_ptp_interrupts ( int portnum)
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short reg_addr;
  PTP_TYPE base_addr_id;

  PTP_DBG_TRACE(("PTP IGR clearing interrupts (port %x)\n", portnum));
  base_addr_id = get_ptp_base_addr_id( portnum, PTP_igr);
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_RF + PTP_INTR_SET;
  retVal = ptp_write_reg(portnum, reg_addr, 0xffffffff);
  if(retVal!=DSDT_OK) return retVal;
  
  base_addr_id = get_ptp_base_addr_id( portnum, PTP_egr);
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_RF + PTP_INTR_SET;
  retVal = ptp_write_reg(portnum, reg_addr, 0xffffffff);

  return retVal;
}

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
DSDT_STATUS set_tai_intr_msk (int port, unsigned int mask)
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short reg_addr = TAI_INTR_MASK;
  unsigned int data;
  
  data = ptp_read_reg(port, reg_addr);
  data &= ~0xf;
  retVal = ptp_write_reg(port, reg_addr, data | (mask&0xf));
  return retVal;
}

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
DSDT_STATUS get_tai_intr_msk (int port, unsigned int *mask)
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short reg_addr = TAI_INTR_MASK;
  
  *mask = ptp_read_reg(port, reg_addr);
  *mask &= 0xf;
  return retVal;
}

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
DSDT_STATUS ptp_global_intr_en ( int portnum, int en)
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short reg_addr;
  PTP_TYPE base_addr_id;
  unsigned int data;
  
  PTP_DBG_TRACE(("PTP set PTP interrupt enable (port %x enable %x)\n", portnum, en&1));
  base_addr_id = get_ptp_base_addr_id( portnum, PTP_igr);
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_RF + PTP_CFG_GEN;
  data = ptp_read_reg(portnum, reg_addr);
  data &= ~0x2;
  if(en)
    data |= 2;
  retVal = ptp_write_reg(portnum, reg_addr, en);
  if(retVal!=DSDT_OK) return retVal;
  
  base_addr_id = get_ptp_base_addr_id( portnum, PTP_egr);
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_RF + PTP_CFG_GEN;
  data = ptp_read_reg(portnum, reg_addr);
  data &= ~0x2;
  if(en)
    data |= 2;
  retVal = ptp_write_reg(portnum, reg_addr, en);

  return retVal;
}

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
DSDT_STATUS set_top_intr_msk (int port, unsigned int mask)
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short reg_addr = MSEC_PTP_STAT_GLOBAL; /*0x210a */
  unsigned int data;
  data = ptp_read_reg(port, reg_addr);
  msec_bits_write (&data, mask&0x1f, 12, 8);
  retVal = ptp_write_reg(port, reg_addr, data);
  return retVal;
}

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
DSDT_STATUS get_top_intr_msk (int port, unsigned int *mask)
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short reg_addr = MSEC_PTP_STAT_GLOBAL; /*0x210a */
  unsigned int data;
  
  data = ptp_read_reg(port, reg_addr);
  *mask = msec_bits_get (data, 12, 8);
  return retVal;
}

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
DSDT_STATUS msec_get_intr_src (int port, unsigned *intSrc)
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short reg_addr = MSEC_PTP_INTR_SOURCE; /*0x210B; */
  unsigned int data;
  
  data = ptp_read_reg(port, reg_addr);
  PTP_DBG_DEMO(("TAI Intr %X\n", (unsigned int)nptp_bit_get(data, 5)));
  PTP_DBG_DEMO(("Global Intr %X\n", (unsigned int)nptp_bit_get(data, 4)));
  PTP_DBG_DEMO(("port3 Intr %X\n", (unsigned int)nptp_bit_get(data, 3)));
  PTP_DBG_DEMO(("port2 Intr %X\n", (unsigned int)nptp_bit_get(data, 2)));
  PTP_DBG_DEMO(("port1 Intr %X\n", (unsigned int)nptp_bit_get(data, 1)));
  PTP_DBG_DEMO(("port0 Intr %X\n", (unsigned int)nptp_bit_get(data, 0)));
  *intSrc = data;
  return retVal;
}

#if 1

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
DSDT_STATUS prgm_ptp_lut_ent_msgType(int port, int idx, PTPv2_message_type msgId, PTP_1STEP_LUT_ACT actions, PTP_Blk path)
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_top=0;
  unsigned long lut_vld_top=0;
  unsigned long long lut_act; 
  int addr = (path == PTP_igr)?VN_IG0_PTP:VN_EG0_PTP;
  PTP_DBG_TRACE(("PTP prgm_ptp_lut_ent_msgType port %x path %x idx %x msgId %x action %x\n", port, path, idx, msgId, actions));

  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_key = lut_key | lut_key_adjust( K_MessageType, msgId);
  lut_vld = lut_vld | lut_vld_adjust(K_MessageType);
  lut_act = prgm_ptp_lut_acts(actions);

  retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_top, lut_vld_top, lut_act, idx, addr+PTP_PORT_OFF(port));
  return retVal;
}
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
DSDT_STATUS prgm_ptp_lut_ent_domain_num(int port, int idx, int domain_num, PTP_1STEP_LUT_ACT actions, PTP_Blk path)
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_top=0;
  unsigned long lut_vld_top=0;
  unsigned long long lut_act; 
  int addr = (path == PTP_igr)?VN_IG0_PTP:VN_EG0_PTP;
  PTP_DBG_TRACE(("PTP prgm_ptp_lut_ent_domain_num port %x path %x idx %x domain_num %x action %x\n", port, path, idx, domain_num, actions));

  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_key = lut_key | lut_key_adjust( K_DomainNumber, domain_num);
  lut_vld = lut_vld | lut_vld_adjust(K_DomainNumber);
  lut_act = prgm_ptp_lut_acts(actions);

  retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_top, lut_vld_top, lut_act, idx, addr+PTP_PORT_OFF(port));
  return retVal;
}

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
DSDT_STATUS prgm_ptp_lut_ent_transspec(int port, int idx, int transspec, PTP_1STEP_LUT_ACT actions, PTP_Blk path)
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_top=0;
  unsigned long lut_vld_top=0;
  unsigned long long lut_act; 
  int addr = (path == PTP_igr)?VN_IG0_PTP:VN_EG0_PTP;
  PTP_DBG_TRACE(("PTP prgm_ptp_lut_ent_transspec port %x path %x idx %x transspec %x action %x\n", port, path, idx, transspec, actions));

  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_key = lut_key | lut_key_adjust( K_TransportSpecific, transspec);
  lut_vld = lut_vld | lut_vld_adjust(K_TransportSpecific);
  lut_act = prgm_ptp_lut_acts(actions);

  retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_top, lut_vld_top, lut_act, idx, addr+PTP_PORT_OFF(port));
  return retVal;
}
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
DSDT_STATUS prgm_lut_ent_ipv4(int port, int idx, PTP_1STEP_LUT_ACT actions, PTP_Blk path) 
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_top=0;
  unsigned long lut_vld_top=0;
  unsigned long long lut_act; 
  int addr = (path == PTP_igr)?VN_IG0_PTP:VN_EG0_PTP;
  PTP_DBG_TRACE(("PTP prgm_lut_ent_ipv4 port %x path %x idx %x action %x\n", port, path, idx, actions));

  lut_key = lut_key_adjust( K_FlagIPv4, 1);
  lut_vld = lut_vld_adjust(K_FlagIPv4);
  lut_act = prgm_ptp_lut_acts(actions);

  retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_top, lut_vld_top, lut_act, idx, addr+PTP_PORT_OFF(port));
  return retVal;
}
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
DSDT_STATUS prgm_lut_ent_ntp(int port, int idx, PTP_1STEP_LUT_ACT actions, PTP_Blk path, int udp_port) 
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_top=0;
  unsigned long lut_vld_top=0;
  unsigned long long lut_act; 
  int addr = (path == PTP_igr)?VN_IG0_PTP:VN_EG0_PTP;
  PTP_DBG_TRACE(("PTP prgm_lut_ent_ntp port %x path %x idx %x action %x udp_port %x\n", port, path, idx, actions, udp_port));

  if(udp_port==0)
  {
    lut_key = lut_key_adjust( K_FlagNTP, 1);
    lut_vld = lut_vld_adjust(K_FlagNTP);
  }
  else
  {
    lut_key = lut_key_adjust( K_FlagPTPv1, 1);
    lut_vld = lut_vld_adjust(K_FlagPTPv1);
  }

  lut_key = lut_key | lut_key_adjust( K_FlagUDP, 1);
  lut_vld = lut_vld | lut_vld_adjust(K_FlagUDP);
  lut_act = prgm_ptp_lut_acts(actions);
  if((path==PTP_egr)&&(actions!=PTP_Act_No_Action))
    lut_act |= prgm_ptp_lut_acts(PTP_Act_HoldForTransmission);

  retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_top, lut_vld_top, lut_act, idx, addr+PTP_PORT_OFF(port));
  return retVal;
}
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
*       actions - List of  Lut actions. The last action is PTP_Act_No_Action.
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
DSDT_STATUS prgm_lut_ent_ntp_list(int port, int idx, PTP_1STEP_LUT_ACT *actions, PTP_Blk path, int udp_port) 
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_top=0;
  unsigned long lut_vld_top=0;
  unsigned long long lut_act; 
  int i;
  int addr = (path == PTP_igr)?VN_IG0_PTP:VN_EG0_PTP;
  PTP_DBG_TRACE(("PTP prgm_lut_ent_ntp port %x path %x idx %x action %x\n", port, path, idx, *actions));

  if(udp_port==0)
  {
    lut_key = lut_key_adjust( K_FlagNTP, 1);
    lut_vld = lut_vld_adjust(K_FlagNTP);
  }
  else
  {
    lut_key = lut_key_adjust( K_FlagPTPv1, 1);
    lut_vld = lut_vld_adjust(K_FlagPTPv1);
  }
  lut_key = lut_key | lut_key_adjust( K_FlagUDP, 1);
  lut_vld = lut_vld | lut_vld_adjust(K_FlagUDP);
  lut_act = 0LL;
  i=0;
  do
  {
    lut_act |= prgm_ptp_lut_acts(actions[i]);
	i++;
  }while (actions[i] != PTP_Act_No_Action);

  if((path==PTP_egr)&&(i>0))
    lut_act |= prgm_ptp_lut_acts(PTP_Act_HoldForTransmission);

  retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_top, lut_vld_top, lut_act, idx, addr+PTP_PORT_OFF(port));
  return retVal;
}




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
DSDT_STATUS prgm_lut_ent_ptpv2(int port, int idx, PTP_1STEP_LUT_ACT actions, PTP_Blk path) 
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_top=0;
  unsigned long lut_vld_top=0;
  unsigned long long lut_act; 
  int addr = (path == PTP_igr)?VN_IG0_PTP:VN_EG0_PTP;
  PTP_DBG_TRACE(("PTP prgm_lut_ent_ptpv2 port %x path %x idx %x action %x\n", port, path, idx, actions));

  lut_key = lut_key_adjust( K_FlagPTPv2, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv2);
  lut_act = prgm_ptp_lut_acts(actions);

  retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_top, lut_vld_top, lut_act, idx, addr+PTP_PORT_OFF(port));
  return retVal;
}
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
DSDT_STATUS prgm_lut_ent_ptpv1(int port, int idx, PTP_1STEP_LUT_ACT actions, PTP_Blk path) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_top=0;
  unsigned long lut_vld_top=0;
  unsigned long long lut_act; 
  int addr = (path == PTP_igr)?VN_IG0_PTP:VN_EG0_PTP;
  PTP_DBG_TRACE(("PTP prgm_lut_ent_ptpv1 port %x path %x idx %x action %x\n", port, path, idx, actions));

  lut_key = lut_key_adjust( K_FlagPTPv1, 1);
  lut_vld = lut_vld_adjust(K_FlagPTPv1);
  lut_act = prgm_ptp_lut_acts(actions);

  retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_top, lut_vld_top, lut_act, idx, addr+PTP_PORT_OFF(port));
  return retVal;
}
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
DSDT_STATUS prgm_lut_ent_1731(int port, int idx, PTP_1STEP_LUT_ACT actions, PTP_Blk path) 
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_top=0;
  unsigned long lut_vld_top=0;
  unsigned long long lut_act; 
  int addr = (path == PTP_igr)?VN_IG0_PTP:VN_EG0_PTP;
  PTP_DBG_TRACE(("PTP prgm_lut_ent_1731 port %x path %x idx %x action %x\n", port, path, idx, actions));

  lut_key = lut_key_adjust( K_FlagY1731, 1);
  lut_vld = lut_vld_adjust(K_FlagY1731);
  lut_act = prgm_ptp_lut_acts(actions);

  retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_top, lut_vld_top, lut_act, idx, addr+PTP_PORT_OFF(port));
  return retVal;
}
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
DSDT_STATUS prgm_lut_ent_1731_opcode(int port, int idx, int opcode, PTP_1STEP_LUT_ACT actions, PTP_Blk path) 
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_top=0;
  unsigned long lut_vld_top=0;
  unsigned long long lut_act; 
  int addr = (path == PTP_igr)?VN_IG0_PTP:VN_EG0_PTP;
  PTP_DBG_TRACE(("PTP prgm_lut_ent_1731_opcode port %x path %x idx %x opcode %x action %x\n", port, path, idx, opcode,  actions));

  lut_key = lut_key_adjust( K_VersionPTP, opcode);
  lut_vld = lut_vld_adjust(K_VersionPTP);
  lut_act = prgm_ptp_lut_acts(actions);

  retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_top, lut_vld_top, lut_act, idx, addr+PTP_PORT_OFF(port));
  return retVal;
}
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
DSDT_STATUS prgm_lut_ent_stag(int port, int idx, PTP_1STEP_LUT_ACT actions, PTP_Blk path) 
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_top=0;
  unsigned long lut_vld_top=0;
  unsigned long long lut_act; 
  int addr = (path == PTP_igr)?VN_IG0_PTP:VN_EG0_PTP;
  PTP_DBG_TRACE(("PTP prgm_lut_ent_stag port %x path %x idx %x action %x\n", port, path, idx, actions));

  lut_key = lut_key_adjust( K_FlagSTAG, 1);
  lut_vld = lut_vld_adjust(K_FlagSTAG);
  lut_act = prgm_ptp_lut_acts(actions);

  retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_top, lut_vld_top, lut_act, idx, addr+PTP_PORT_OFF(port));
  return retVal;
}
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
DSDT_STATUS prgm_lut_ent_always0(int port, int idx, PTP_1STEP_LUT_ACT actions, PTP_Blk path) 
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_top=0;
  unsigned long lut_vld_top=0;
  unsigned long long lut_act; 
  int addr = (path == PTP_igr)?VN_IG0_PTP:VN_EG0_PTP;
  PTP_DBG_TRACE(("PTP prgm_lut_ent_always0 port %x path %x idx %x action %x\n", port, path, idx, actions));

  lut_key = lut_key_adjust( K_AlwaysZero, 0);
  lut_vld = lut_vld_adjust(K_AlwaysZero);
  lut_act = prgm_ptp_lut_acts(actions);

  retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_top, lut_vld_top, lut_act, idx, addr+PTP_PORT_OFF(port));
  return retVal;
}


#endif



