#include <madCopyright.h>
#include "madSample.h"
#include "mx_api_helper.h"
#include "msec_defines.h"
#include "msecTest.h"
#include "setMACsec.h"
/*
//####################################################################################
//# macsec_2.2.3_AE_speed1000_fdx_test.tcl
//# This test set link speed to 1000, duplex to full 
//# enable link encryption on all port ports with random keys.
//# 
//# The test is described in terms of four pairs, each of which has an encrypt and a decrypt
//# operation.
//# Pair A is ports 0 (egress) -> 1 (ingress)
//# Pair B is ports 1 (egress) -> 0 (ingress)
//# Pair C is ports 2 (egress) -> 3 (ingress)
//# Pair D is ports 3 (egress) -> 2 (ingress)
//# Configuration
//# 1. Create 4 SCIs and 4 keys.
//# 2. Set each AN to 0.
//# 3. Initialize the egress and ingress sides of each pair with sci[n] and key[n] in entry 0
//# 4. Set link speed to 1000, full duplex.
//# Traffic generation
//# 1. Start traffic on all 4 ports.
//# 2. Stop traffic and read all MAC and MACsec counters.
//# 3. Check that Egress and Ingress hit counters match.
//#  
//# 
//# ELU Configuration Table
//# ======================================================================
//# Entry	DA		SA		EtherType	VLAN	Default
//#  0					0x0800			
//# 
//# Context/Key Configuration
//# ======================================================================
//# Entry	SCI	TCI	Key			Encrypt	
//#  0	random	2C	random			1	
//#					 
//#					 
//# Result analysis
//# When both way traffic are sent, the egress hit counter and ingress hit counter
//# on each pair of traffic shall match.
//##########################################################################
*/
static int en_def;
static int en_mc;
static int en_da;
static int en_sa;
static int en_vlan;
static int en_et;
static int en_scian;
static int en_hdr = 1;
static MATCH_EN *  lk_match_en;

static long long da_fld;
static long long sa_fld;
static int vlan_fld;
static int et_fld;
static long long sci_fld;
static int tci_fld;
static int tci_msk_fld;
static int conf_offset;
static MATCH_FLD *  lk_match_fld;

static int ctx_num;
static int drop;
static int redir;
static int auth_en;
static int enc_en;
static int drop_maxpn;
static ACT_FLD *  lk_act_fld;

static int an;
static int tci = 0x2C;
static int pn =1;

static long long sciList [4];
static char keyList [16];
static char * key ;

  static  unsigned int eports[4] = {0, 1, 2, 3}; // "0 1 2 3"
  static  unsigned int iports[4] = {1, 0, 3, 2}; // "1 0 3 2"
static CTX_T *  ict;
static LKUP_T *  ilk_t, * elk_t;

static ent_num =0;

int macsec_2_2_3_AE_speed1000_fdx_tes(MAD_DEV *devp)
{
  int rc = 0;

  int i, port;
  int ent_num=0;

 //  setMACsec();

  printf("MACSec validation macsec_2.2.3_AE_speed1000_fdx_tes\n");
  printf("========================================================\n");

  // initialize MACsec API on usbport 0
  if (devp == NULL) {
	printf("Initialization of MAD failed.\n");
	return -1;
  }
  initialize_macsec_api(devp);
  stat_arr_init ();

//# initialize MACsec module with speed
//# 2 - 1000Mbps
//# 1 - 100Mbps
//# 0 - 10Mbps
//#macsec_start $devp 1

  // initialize MACSec moduel with link speed 1000Mbps
  macsec_start(devp, 2);

  printf("\n");
//###########################################################
//# setup per-port Common config
//###########################################################"
  {
  int conf_offset =0, def_vlan =0, mc_high=0, mc_low_msk =0, trail_sz =0;
  CMN_CFG_T * cmn_cfg = new_cmn_cfg_t(conf_offset, def_vlan, mc_high, mc_low_msk,trail_sz);
  for (port=0; port<2; port++) {
    printf("port conf_offset def_vlan mc_high mc_low_msk\n");
    printf("%4d %11d %8d %7d %10d\n", port, conf_offset, def_vlan, mc_high, mc_low_msk);
  //#  printf("$port, conf_offset, def_vlan, mc_high, mc_low_msk"
  set_dev_ctrl_port_num(devp,  port);
  msec_port_cmn_cfg(devp,  port, cmn_cfg);
  }
   conf_offset= 50;
  for (port=2; port<devp->numOfPorts; port++) {
    printf("port conf_offset def_vlan mc_high mc_low_msk\n");
  //#  printf("$port, conf_offset, def_vlan, mc_high, mc_low_msk"
    printf("%4d %11d %8d %7d %10d\n", port, conf_offset, def_vlan, mc_high, mc_low_msk);
  set_dev_ctrl_port_num(devp,  port);
  set_cmn_cfg_offset(cmn_cfg, conf_offset);
  msec_port_cmn_cfg(devp,  port, cmn_cfg);
  }
  }
 if (0) {
//# 1. replay_protect 1, replay_drop 1
int replay_protect =1;
int replay_drop =1;
int rply_prot;
int iport;

  for (iport=0; iport<devp->numOfPorts; iport++) {
  set_dev_ctrl_port_num (devp,  iport);
    msec_port_set_igr_rpy_t_replay_protect(devp,  iport, replay_protect);
    msec_port_set_igr_rpy_t_replay_drop(devp,  iport, replay_drop);
  }
  printf("Set IGR Replay protect to %d, Replay drop to %d\n", replay_protect, replay_drop);

  for (iport=0; iport<devp->numOfPorts; iport++) {
    set_dev_ctrl_port_num (devp,  iport);
    rply_prot = msec_port_get_igr_rpy_t_replay_protect(devp, iport);
    replay_drop = msec_port_get_igr_rpy_t_replay_drop(devp,  iport);
    printf("\nAfter configuration, port $iport rply_prot = %d, rply_drop = %d.\n", 
		    rply_prot, replay_drop);
  }
 }
  printf("\n");
//###################################################################
//# Entry 0
//# setup egress on port 0 1 2 3 in that order
//# create 4 sci and 4 key values and set to each egress port
//# forming four encrypt/decrypt pairs of 0->1, 1->0, 2->3, 3->2
//# make sure egress and ingress port pairs use the same key and sci
//###################################################################"
  {
//# setup ELU match enables
  en_def =0;
  en_mc = 0;
  en_da = 0;
  en_sa = 0;
  en_vlan =0;
  en_et = 1;
  en_scian =0;
  lk_match_en = new_match_en(en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian, en_hdr);

  printf("Setup ELU match enables:\n");
  printf("ent_num %d en_def %d en_mc %d en_da %d en_sa %d en_vlan %d en_et %d en_scian %d\n",
          ent_num, en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian);

//# setup ELU match fields
  da_fld = 0ll;
  sa_fld = 0ll;
  vlan_fld = 0;
  et_fld = 0x0800;
  sci_fld = 0ll;
  tci_fld = 0;
  tci_msk_fld = 0;
  conf_offset = 0;
  en_hdr = 1;
  lk_match_fld = new_match_fld(da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld, conf_offset, en_hdr);
  printf("Setup ELU match fields:\n");
  printf("ent_num	da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld\n",
     ent_num, da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld);


//# setup ELU action
  ctx_num = ent_num;
  drop = 0;
  redir = 0;
  auth_en = 1;
  enc_en =1;
  drop_maxpn = 1;
  lk_act_fld = new_act_fld(ctx_num, drop_maxpn, drop, redir, auth_en, enc_en);
  printf("Setup ELU action:\n");
  printf("ent_num ctx_num drop redir auth_en enc_en\n", 
          ent_num, ctx_num, drop, redir, auth_en, enc_en);

//# setup LKUP_T
  elk_t = new_lkup_t(lk_match_en, lk_match_fld, lk_act_fld);
  printf("Setup Lookup Table as configured above.\n");

//# setup ECT
//# The key array and ct are shared structures for egress and ingress,
//# it is created here and deleted later.
  an =0;
  tci = 0x2C;
  pn = 1;

  for (i=0; i<devp->numOfPorts; i++) {
  char *keyStr;
  int j;
    sciList[i] = gen_random_64_octs();

    key = create_u8_arr(16);
    keyStr = gen_random_hex_octs(16);
  //  foreach key_fld $keyStr {
    for (j=0; j<16; j++) {
      set_u8_arr_fld(key, j, keyStr[j]);
	}
     keyList[i] = *key;
  }

//# Setup EGR entry 
//# setup ctrl port_num
  for(i=0;i<devp->numOfPorts; i++) {
    CTX_T        *ect;
	int eport;
	eport = eports [i];
    set_dev_ctrl_port_num(devp, eport);
    ect = new_ctx_t(sciList[i], pn, tci, conf_offset, key);
    msec_port_set_egr_entry(devp, eport % 4, ent_num, elk_t, ect);
    delete_ctx_t(ect);
  }
  printf("Setup Egress port $eports entry $ent_num as configured above.");

  //# release memory for ct, elk_t
  delete_lkup_t(elk_t);

  printf("\n");
//###############################################################
//# Entry 0
//# setup Ingress on port 1 0 3 2 in that order
//# forming four encrypt/decrypt pairs of 0->1, 1->0, 2->3, 3->2
//# make sure egress and ingress port pairs use the same key and sci
//###############################################################"

//# setup ILU match enables
  en_def =0;
  en_mc =0;
  en_da =0;
  en_sa =0;
  en_vlan =0;
  en_et =0;
  en_scian =1;
  lk_match_en = new_match_en(en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian, en_hdr);
  printf("Setup ILU match enables:\n");
  printf("ent_num  en_def en_mc en_da en_sa en_vlan en_et en_scian\n",
          ent_num , en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian);


//# setup ILU action
  ctx_num =ent_num;
  drop =0;
  redir =0;
  auth_en =1;
  enc_en =1;
  drop_maxpn = 1;
  lk_act_fld = new_act_fld(ctx_num, drop_maxpn, drop, redir, auth_en, enc_en);
  printf("Setup ILU action field.\n");
  printf("ent_num ctx_num drop redir auth_en enc_en\n",
          ent_num, ctx_num, drop, redir, auth_en, enc_en);

//  int iports [list 1 0 3 2]
//foreach iport $iports sci $sciList key $keyList {
//  set_dev_ctrl_port_num(devp,  iport


  for(i=0;i<devp->numOfPorts; i++) {
    set_dev_ctrl_port_num(devp,  iports[i]);

  //# setup ICT
    ict = new_ctx_t(sciList[i], pn, tci, conf_offset, key);
  //# setup ILU match fields 
    da_fld =0ll;
    sa_fld =0ll;
    vlan_fld =0;
    et_fld =0;
    sci_fld = sciList[i];
    tci_fld = tci;
    tci_msk_fld = 0xFF;
    conf_offset = 0;
    en_hdr = 1;
    lk_match_fld = new_match_fld(da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld, conf_offset, en_hdr);
    printf("Setup ILU match fields:\n");
    printf("ent_num  da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld\n",
            ent_num, da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld);

  //# setup LKUP_T
    ilk_t = new_lkup_t(lk_match_en, lk_match_fld, lk_act_fld);
    printf("Setup ILU lookup table as configured above.\n");
  
    //# Setup IGR entry 
    msec_port_set_igr_entry(devp, iports[i], ent_num, ilk_t, ict);
    printf("Setup port %d IGR entry, ent_num %d as configured above.\n", iports[i], ent_num);
  }

  //# release memory for key, ct, 
  delete_u8_arr(key);
  delete_ctx_t(ict);
  delete_lkup_t(ilk_t);

  printf("\nForce flow control enable on all ports and reduce sysMAC IPG to 6.\n");
  force_flow_ctrl_en(devp);


  printf("\n");
//###############################################################################
//# send traffic from N2X to DUT receive traffic from N2X connected to DUT ports
//# compare frame received shall be same as frame sent
//# setup WIRE side loopback between port0 and port1, port2 and port3
//###############################################################################"
  {
  int portLabelList[4] = {101/1, 101/2, 101/3, 101/4};
  int portList = 0xf; //"1 2 3 4"
//  int smacList [list AA:BB:CC:DD:EE:00 AA:BB:CC:DD:EE:01 AA:BB:CC:DD:EE:02 AA:BB:CC:DD:EE:03]
//  int dmacList [list FE:ED:D0:0D:FF:A0 FE:ED:D0:0D:FF:A1 FE:ED:D0:0D:FF:A2 FE:ED:D0:0D:FF:A3]
  char smacList [4][6] = { {0xAA,0xBB,0xCC,0xDD,0xEE,0x00},
                           {0xAA,0xBB,0xCC,0xDD,0xEE,0x01}, 
                           {0xAA,0xBB,0xCC,0xDD,0xEE,0x02},
                           {0xAA,0xBB,0xCC,0xDD,0xEE,0x03}};
  char dmacList [4][6] = { {0xFE,0xED,0xD0,0x0D,0xFF,0xA0},
                           {0xFE,0xED,0xD0,0x0D,0xFF,0xA1}, 
                           {0xFE,0xED,0xD0,0x0D,0xFF,0xA2}, 
                           {0xFE,0xED,0xD0,0x0D,0xFF,0xA3}};
  int load = 100;
  int size = 64;
  int rc =0;
  char *smac;
  char *dmac;
  int iport, eport;


//  printf("\nInitial link speed %d, full duplex\n\n", speed Mbps);
  for(i=0; i<devp->numOfPorts; i++) {
    int sport;
    int dport;
    eport = eports[i];
    iport = iports[i];
	smac  = smacList[i];
	dmac  = dmacList[i];


    printf("send continuous frames with average load %d", load);
    printf("%d -> %d, destmac %02d:%02d:%02d:%02d:%02d:%02d, srcmac %02d:%02d:%02d:%02d:%02d:%02d\n",
    eport, iport, dmac[0],dmac[1],dmac[2],dmac[3],dmac[4],dmac[5], smac[0],smac[1],smac[2],smac[3],smac[4],smac[5]);

  //# convert to 1-based port number for N2X
    sport = eport + 1;
    dport = iport + 1;
  //#send_nonstop_frames $argv0 $portList $sport $dport, smac, dmac, load, size
//  send_nonstop_frames_fc_en(argv0, portList, sport, dport, smac, dmac, load, size);
//  set_TesterSpeedDuplex(dport, speed, duplex);
}
madWait(20000);
  }
  if (1) {
//start_test 
//  int start_tm [clock seconds]
//  printf("Test started on\n[clock format $start_tm]\n");
//madWait(1000);
//stop_test $argv0
//AgtInvoke AgtStreamGroupList Clear
//madWait(1000);
  dump_print_macsec_statistics(devp);
//  dump_mac_statistics(devp);
  dump_macsec_statistics(devp);

  for(i=0; i<devp->numOfPorts; i++) {
	  int st_num;
    int eport = eports[i];
    int iport = iports[i];
	for(st_num=0; st_num<15; st_num++)
	{
      if(compare_egr_igr_hit_counters(devp, eport, iport, st_num) != 0) {
        printf("EGR port %d and IGR port %d Hit counter mismatch, incorrect.\n", eport, iport);
        rc = -1;
	  }
	}
	rc = 0;
  }
  }
  printf("\n");
  //###########################################################
  //# Check and print test result
  //###########################################################"
  if(rc == 0) {
    printf("Test pass.\n");
  } else {
    printf("Test fail.\n");
  }
  }
}