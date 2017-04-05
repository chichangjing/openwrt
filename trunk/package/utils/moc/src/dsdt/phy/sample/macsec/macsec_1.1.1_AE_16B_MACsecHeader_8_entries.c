#include <madCopyright.h>
#include "madSample.h"
#include "mx_api_helper.h"
#include "msec_defines.h"
#include "msecTest.h"
#include "setMACsec.h"

//####################################################################################
//# macsec_1.1.1_AE_16B_MACsecHeader_8_entry_table.tcl
//# This test covers packet parsing abilities, packet selection based on DA, SA, 
//# Vlan and ether type, insertion of 16B MACsec header, auth only and auth + encrypt
//# functionality on egress path. On ingress path it covers packet parsing capabilities.
//# MACSec packet selection based on SCI and AN, removal/retention of 16B MACSec
//# header, auth only and auth + decrypt functionality. It also covers confidentiality
//# offset (offconf) of 0, 30 on egress and ingress path.
//# Table 1: ELU Configuration Table
//# =============================================================
//# Entry	DA		SA		EtherType	VLAN
//#  0	FEEDD00DFFA0	FEEDD00DFFA1	8100		123
//#  1	FEEDD00DFFA0					123
//#  2			FEEDD00DFFA1	0800
//#  3	FEEDD00DFFB0	FEEDD00DFFB1			123
//#  4							456
//#  5			FEEDD00DFFB1
//#  6					0806
//#  7	FEEDD00DFFB0
//# =============================================================
//# 
//# Table 2: Context/Key Configuration
//# =====================================================================================
//# Entry	SCI			TCI	Key			Encrypt	
//#  0	F00DFEEDBEEF0001	2C	FEDCBA98FEDCBA98	1
//#					FEDCBA98FEDCBA98
//#  1	F00DFEEDBEEF0001	2D	FEDCBA98FEDCBA98	1
//#  					0001000200030004
//#  2	F00DFEEDBEEF0001	2E	FEDCBA98FEDCBA98	1
//#					9876543201234567		
//#  3	F00DFEEDBEEF0001	2F	9876543201234567	1
//#					FEDCBA98FEDCBA98
//#  4	FEDCBA98FEDCBA98	23	1246857965894521	0
//#					6986454744412569
//#  5	9876543201234567	22	1245369885476325	0
//#					9645312524869731	
//#  6	7676543298766789	21	1141559747469931	0
//#					FEDEBABADDEFFEDA
//#  7	D00D8888FEED1121	20	ABACDDECFEECCADB	0
//#					4745698521454786
//# =====================================================================================
//# Note: when sending traffic to exercise an entry with DA/SA not specified in ELU table,
//# use dmac/smac different than the DA/SA in other entries in ELU table.
//# MACSEC statistics checking:
//#  entry 0 ~ 3: EGR_HIT, EGR_ENC, IGR_HIT, IGR_OK incremented;
//#  entry 4 ~ 7: EGR_HIT, EGR_PROT, IGR_HIT, IGR_OK incremented;
//####################################################################################
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
static int  retain_hdr = 1;

static long long sci;
static int tci = 0x2C;
static int pn =1;

static long long sciList [4];
static char keyList [16];
static char * key ;
static char keyStr_0[16] ={0xFE, 0xDC, 0xBA, 0x98, 0xFE, 0xDC, 0xBA, 0x98, 0xFE, 0xDC, 0xBA, 0x98, 0xFE, 0xDC, 0xBA, 0x98};
static char keyStr_1[16] ={0xFE, 0xDC, 0xBA, 0x98, 0xFE, 0xDC, 0xBA, 0x98, 0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04};
static char keyStr_2[16] ={0xFE, 0xDC, 0xBA, 0x98, 0xFE, 0xDC, 0xBA, 0x98, 0x98, 0x76, 0x54, 0x32, 0x01, 0x23, 0x45, 0x67};
static char keyStr_3[16] ={0x98, 0x76, 0x54, 0x32, 0x01, 0x23, 0x45, 0x67, 0xFE, 0xDC, 0xBA, 0x98, 0xFE, 0xDC, 0xBA, 0x98};
static char keyStr_4[16] ={0x12, 0x46, 0x85, 0x79, 0x65, 0x89, 0x45, 0x21, 0x69, 0x86, 0x45, 0x47, 0x44, 0x41, 0x25, 0x69};
static char keyStr_5[16] ={0x12, 0x45, 0x36, 0x98, 0x85, 0x47, 0x63, 0x25, 0x96, 0x45, 0x31, 0x25, 0x24, 0x86, 0x97, 0x31};
static char keyStr_6[16] ={0x11, 0x41, 0x55, 0x97, 0x47, 0x46, 0x99, 0x31, 0xFE, 0xDE, 0xBA, 0xBA, 0xDD, 0xEF, 0xFE, 0xDA};
static char keyStr_7[16] ={0xAB, 0xAC, 0xDD, 0xEC, 0xFE, 0xEC, 0xCA, 0xDB, 0x47, 0x45, 0x69, 0x85, 0x21, 0x45, 0x47, 0x86};

static  int eports[8] = {0, 1, 2, 3, 4, 5, 6, 7}; // "0 1 2 3 4 5 6 7"
static  int iports[8] = {1, 0, 3, 2, 5, 4, 7, 6}; // "1 0 3 2 5 4 7 6"
static CTX_T *  ict, * ect;
static LKUP_T *  ilk_t, * elk_t;

static ent_num =0;

int macsec_1_1_1_AE_16B_MACsecHeader_8_entry_table(MAD_DEV *devp)
{
  int rc = 0;

  int i, j, port;
  int ent_num = 0;

//# initialize MACsec API on usbport 0
  printf("MACSec validation macsec_1_1_1_AE_16B_MACsecHeader_8_entry_table\n");
  printf("========================================================\n");

  // initialize MACsec API on usbport 0
  if (devp == NULL) {
	printf("Initialization of MAD failed.\n");
	return -1;
  }
  initialize_macsec_api(devp);
  stat_arr_init ();

  printf("Test in progress, please wait ...\n");

//# initialize MACSec module with link speed 1000Mbps
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
   conf_offset= 30;
  for (port=2; port<4; port++) {
    printf("port conf_offset def_vlan mc_high mc_low_msk\n");
  //#  printf("$port, conf_offset, def_vlan, mc_high, mc_low_msk"
    printf("%4d %11d %8d %7d %10d\n", port, conf_offset, def_vlan, mc_high, mc_low_msk);
  set_dev_ctrl_port_num(devp,  port);
  set_cmn_cfg_offset(cmn_cfg, conf_offset);
  msec_port_cmn_cfg(devp,  port, cmn_cfg);
  }
  }
printf("\n");
//###########################################################
//# Entry 0
//# setup egress on port 0 & 2
//###########################################################"

//# setup ELU match enables
  en_def =0;
  en_mc = 0;
  en_da = 1;
  en_sa = 1;
  en_vlan = 1;
  en_et = 1;
  en_scian =0;
  lk_match_en = new_match_en(en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian, en_hdr);

  printf("Setup ELU match enables:\n");
  printf("ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian\n");
  printf("%7d %6d %5d %5d %5d %7d %5d %8d\n",
          ent_num, en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian);


//# setup ELU match fields
  da_fld = 0xFEEDD00DFFA0ll;
  sa_fld = 0xFEEDD00DFFA1ll;
  vlan_fld = 123;
  et_fld = 0x0800;
  sci_fld = 0ll;
  tci_fld = 0;
  tci_msk_fld = 0;
  conf_offset = 0;
  en_hdr = 1;
  lk_match_fld = new_match_fld(da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld, conf_offset, en_hdr);
  printf("Setup ELU match fields:\n");
  printf("ent_num da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld\n");
  printf("%7d %6x %6x %8x %6x %7x %7x %11x\n", ent_num, da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld);


//# setup ELU action
  ctx_num = ent_num;
  drop = 0;
  redir = 0;
  auth_en = 1;
  enc_en =1;
  drop_maxpn = 1;
  lk_act_fld = new_act_fld(ctx_num, drop_maxpn, drop, redir, auth_en, enc_en);
  printf("Setup ELU action:\n");
  printf("ent_num ctx_num drop redir auth_en enc_en\n");
  printf("%7x %7x %4x %5x %7x %6x\n", ent_num, ctx_num, drop, redir, auth_en, enc_en);


//# setup LKUP_T
  elk_t = new_lkup_t(lk_match_en, lk_match_fld, lk_act_fld);
  printf("Setup Lookup Table as configured above.\n");

//# setup ECT
//# The key array and ct are shared structures for egress and ingress,
//# it is created here and deleted later.
  sci =0xF00DFEEDBEEF0001ll;
  tci = 0x2C;
  pn = 1;
  {
    key = create_u8_arr(16);
  //  foreach key_fld $keyStr {
    for (j=0; j<16; j++) {
      set_u8_arr_fld(key, j, keyStr_0[j]);
	}
  }

  ect = new_ctx_t(sci, pn, tci, conf_offset, key);
  printf("Setup ECT:\n");
  printf("Ent_num tci pn sci\n");
  printf("%-7d %3X %-3d 0x%llx", ent_num, tci, pn, sci);
  printf("\nkeyStr: ");
  for (j=0; j<16; j++) {
    printf("%02X ", keyStr_0[j]&0xff);
  }
  printf("\n");

//# setup ctrl port_num
  for(i=0;i<devp->numOfPorts; i+=2) {
    set_dev_ctrl_port_num(devp, i);
    //# Setup EGR entry 
    msec_port_set_egr_entry(devp, i, ent_num, elk_t, ect);
  }
  printf("Setup Egress port %d entry %d as configured above.\n", i, ent_num);

  //# release memory for ct, elk_t
  delete_ctx_t(ect);
  delete_lkup_t(elk_t);
	  
  printf("\n");
//###########################################################
//# Entry 0
//# setup Ingress on port 1 & 3
//###########################################################"

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
  printf("ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian\n");
  printf("%7d %6d %5d %5d %5d %7d %5d %8d\n", ent_num, en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian);
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
    printf("ent_num da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld\n");
    printf("%7d %6x %6x %8x %6x %7x %7x %11x\n", ent_num, da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld);

//# setup ILU action
  ctx_num =ent_num;
  drop =0;
  redir =0;
  auth_en =1;
  enc_en =1;
  drop_maxpn = 1;
  lk_act_fld = new_act_fld(ctx_num, drop_maxpn, drop, redir, auth_en, enc_en);
  printf("Setup ILU action field.\n");
  printf("ent_num ctx_num drop redir auth_en enc_en\n");
  printf("%7x %7x %4x %5x %7x %6x\n", ent_num, ctx_num, drop, redir, auth_en, enc_en);

  //# setup LKUP_T
  ilk_t = new_lkup_t(lk_match_en, lk_match_fld, lk_act_fld);
  printf("Setup ILU lookup table as configured above.\n");

//# setup CT
  sci =0ll;
  tci = 0;
  pn = 0;
//# IGR use same key as EGR, sci, tci, pn fields are not valid for ingress.
  ict = new_ctx_t(sci, pn, tci, conf_offset, key);
  printf("Setup ICT:\n");
  printf("Ent_num tci pn sci\n");
  printf("%-7d %3X %-3d 0x%llx", ent_num, tci, pn, sci);
  printf("\nkeyStr: ");
  for (j=0; j<16; j++) {
    printf("%02X ", keyStr_0[j]&0xff);
  }
  printf("\n");


//# setup ctrl port_num
  for(i=1;i<devp->numOfPorts; i+=2) {
    set_dev_ctrl_port_num(devp, i);
    //# Setup IGR entry 
    msec_port_set_egr_entry(devp, i, ent_num, ilk_t, ict);
    //# set port3 to retain encryption header
    if(i == 3) {
      msec_port_set_igr_retain_hdr(devp, i, retain_hdr);
	}
    printf("Setup port %d IGR entry %d as configured above.",i, ent_num);
  }

  //# release memory for ct, elk_t
  delete_u8_arr(key);
  delete_ctx_t(ict);
  delete_lkup_t(ilk_t);

  printf("\n");

//###########################################################
//# Entry 1
//# setup egress on port 0 & 2
//###########################################################"
  ent_num = 1;
//# setup ELU match enables
  en_def =0;
  en_mc = 0;
  en_da = 1;
  en_sa = 0;
  en_vlan =1;
  en_et = 0;
  en_scian =0;
  lk_match_en = new_match_en(en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian, en_hdr);

  printf("Setup ELU match enables:\n");
  printf("ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian\n");
  printf("%7d %6d %5d %5d %5d %7d %5d %8d\n", ent_num, en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian);


//# setup ELU match fields
  da_fld = 0xFEEDD00DFFA0ll;
  sa_fld = 0ll;
  vlan_fld = 123;
  et_fld = 0;
  sci_fld = 0ll;
  tci_fld = 0;
  tci_msk_fld = 0;
  conf_offset = 0;
  en_hdr = 1;
  lk_match_fld = new_match_fld(da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld, conf_offset, en_hdr);
  printf("Setup ELU match fields:\n");
  printf("ent_num da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld\n");
  printf("%7d %6x %6x %8x %6x %7x %7x %11x\n", ent_num, da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld);

//# setup ELU action
  ctx_num = ent_num;
  drop = 0;
  redir = 0;
  auth_en = 1;
  enc_en =1;
  drop_maxpn = 1;
  lk_act_fld = new_act_fld(ctx_num, drop_maxpn, drop, redir, auth_en, enc_en);
  printf("Setup ELU action:\n");
  printf("ent_num ctx_num drop redir auth_en enc_en\n");
  printf("%7x %7x %4x %5x %7x %6x\n", ent_num, ctx_num, drop, redir, auth_en, enc_en);


//# setup LKUP_T
  elk_t = new_lkup_t(lk_match_en, lk_match_fld, lk_act_fld);
  printf("Setup Lookup Table as configured above.\n");


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
  printf("ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian\n");
  printf("%7d %6d %5d %5d %5d %7d %5d %8d\n", ent_num, en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian);

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
  printf("ent_num da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld\n");
  printf("%7d %6x %6x %8x %6x %7x %7x %11x\n", ent_num, da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld);


//# setup ELU action
  ctx_num = ent_num;
  drop = 0;
  redir = 0;
  auth_en = 1;
  enc_en =1;
  drop_maxpn = 1;
  lk_act_fld = new_act_fld(ctx_num, drop_maxpn, drop, redir, auth_en, enc_en);
  printf("Setup ELU action:\n");
  printf("ent_num ctx_num drop redir auth_en enc_en\n");
  printf("%7x %7x %4x %5x %7x %6x\n", ent_num, ctx_num, drop, redir, auth_en, enc_en);

//# setup LKUP_T
  elk_t = new_lkup_t(lk_match_en, lk_match_fld, lk_act_fld);
  printf("Setup Lookup Table as configured above.\n");

//# setup ECT
//# The key array and ct are shared structures for egress and ingress,
//# it is created here and deleted later.
  sci =0xF00DFEEDBEEF0001ll;
  tci = 0x2D;
  pn = 1;
  {
    int j;
    key = create_u8_arr(16);
  //  foreach key_fld $keyStr {
    for (j=0; j<16; j++) {
      set_u8_arr_fld(key, j, keyStr_1[j]);
	}
  }

  ect = new_ctx_t(sci, pn, tci, conf_offset, key);
  printf("Setup ECT:\n");
  printf("Ent_num tci pn sci\n");
  printf("%-7d %3X %-3d 0x%llx ", ent_num, tci, pn, sci);
  printf("\nkeyStr: ");
  for (j=0; j<16; j++) {
    printf("%02X ", keyStr_1[j]&0xff);
  }
  printf("\n");

//# Setup EGR entry 
//# setup ctrl port_num
  for(i=0;i<devp->numOfPorts; i+=2) {
    int eport;
	eport = eports [i];
    set_dev_ctrl_port_num(devp, eport);
    ect = new_ctx_t(sciList[i], pn, tci, conf_offset, key);
    msec_port_set_egr_entry(devp, eport % 4, ent_num, elk_t, ect);
    delete_ctx_t(ect);
    printf("Setup Egress port eports %d entry ent_num %d as configured above.", eport, ent_num);
  }

  //# release memory for ct, elk_t
  delete_lkup_t(elk_t);

  printf("\n");
//###########################################################
//# Entry 1
//# setup Ingress entry 1 on port 1 & 3
//###########################################################"
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
  printf("ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian\n");
  printf("%7d %6d %5d %5d %5d %7d %5d %8d\n", ent_num, en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian);

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
  printf("ent_num da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld\n");
  printf("%7d %6x %6x %8x %6x %7x %7x %11x\n", ent_num, da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld);

//# setup ILU action
  ctx_num =ent_num;
  drop =0;
  redir =0;
  auth_en =1;
  enc_en =1;
  drop_maxpn = 1;
  lk_act_fld = new_act_fld(ctx_num, drop_maxpn, drop, redir, auth_en, enc_en);
  printf("Setup ILU action field.\n");
  printf("ent_num ctx_num drop redir auth_en enc_en\n");
  printf("%7x %7x %4x %5x %7x %6x\n", ent_num, ctx_num, drop, redir, auth_en, enc_en);

  //# setup LKUP_T
    ilk_t = new_lkup_t(lk_match_en, lk_match_fld, lk_act_fld);
    printf("Setup ILU lookup table as configured above.\n");

//# setup CT
  sci =0ll;
  tci = 0;
  pn = 0;
//# IGR use same key as EGR, sci, tci, pn fields are not valid for ingress.
  ict = new_ctx_t(sci, pn, tci, conf_offset, key);
  printf("Setup ICT:\n");
  printf("Ent_num tci pn sci\n");
  printf("%-7d %3X %-3d 0x%llx", ent_num, tci, pn, sci);
  printf("\nkeyStr: ");
  for (j=0; j<16; j++) {
    printf("%02X ", keyStr_0[j]&0xff);
  }
  printf("\n");


//# setup ctrl port_num
  for(i=1;i<devp->numOfPorts; i+=2) {
    set_dev_ctrl_port_num(devp, i);
    //# Setup IGR entry 
    msec_port_set_egr_entry(devp, i, ent_num, ilk_t, ict);
    //# set port3 to retain encryption header
    if(i == 3) {
      msec_port_set_igr_retain_hdr(devp, i, retain_hdr);
	}
    printf("Setup port %d IGR entry %d as configured above.",i, ent_num);
  }

  //# release memory for ct, elk_t
  delete_u8_arr(key);
  delete_ctx_t(ict);
  delete_lkup_t(ilk_t);

  printf("\n");


//###########################################################
//# Entry 2
//# setup egress on port 0 & 2
//###########################################################"

 ent_num = 2;
//# setup ELU match enables
  en_def =0;
  en_mc = 0;
  en_da = 0;
  en_sa = 1;
  en_vlan =0;
  en_et = 1;
  en_scian =0;
  lk_match_en = new_match_en(en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian, en_hdr);

  printf("Setup ELU match enables:\n");
  printf("ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian\n");
  printf("%7d %6d %5d %5d %5d %7d %5d %8d\n", ent_num, en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian);


//# setup ELU match fields
  da_fld = 0ll;
  sa_fld = 0xFEEDD00DFFA1ll;
  vlan_fld = 0;
  et_fld = 0x0800;
  sci_fld = 0ll;
  tci_fld = 0;
  tci_msk_fld = 0;
  conf_offset = 0;
  en_hdr = 1;
  lk_match_fld = new_match_fld(da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld, conf_offset, en_hdr);
  printf("Setup ELU match fields:\n");
  printf("ent_num da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld\n");
  printf("%7d %6x %6x %8x %6x %7x %7x %11x\n", ent_num, da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld);


//# setup ELU action
  ctx_num = ent_num;
  drop = 0;
  redir = 0;
  auth_en = 1;
  enc_en =1;
  drop_maxpn = 1;
  lk_act_fld = new_act_fld(ctx_num, drop_maxpn, drop, redir, auth_en, enc_en);
  printf("Setup ELU action:\n");
  printf("ent_num ctx_num drop redir auth_en enc_en\n");
  printf("%7x %7x %4x %5x %7x %6x\n", ent_num, ctx_num, drop, redir, auth_en, enc_en);


//# setup LKUP_T
  elk_t = new_lkup_t(lk_match_en, lk_match_fld, lk_act_fld);
  printf("Setup Lookup Table as configured above.\n");

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
  printf("ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian\n");
  printf("%7d %6d %5d %5d %5d %7d %5d %8d\n", ent_num, en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian);

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
  printf("ent_num da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld\n");
  printf("%7d %6x %6x %8x %6x %7x %7x %11x\n", ent_num, da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld);


//# setup ELU action
  ctx_num = ent_num;
  drop = 0;
  redir = 0;
  auth_en = 1;
  enc_en =1;
  drop_maxpn = 1;
  lk_act_fld = new_act_fld(ctx_num, drop_maxpn, drop, redir, auth_en, enc_en);
  printf("Setup ELU action:\n");
  printf("ent_num ctx_num drop redir auth_en enc_en\n");
  printf("%7x %7x %4x %5x %7x %6x\n", ent_num, ctx_num, drop, redir, auth_en, enc_en);

//# setup LKUP_T
  elk_t = new_lkup_t(lk_match_en, lk_match_fld, lk_act_fld);
  printf("Setup Lookup Table as configured above.\n");

//# setup ECT
//# The key array and ct are shared structures for egress and ingress,
//# it is created here and deleted later.
  sci =0xF00DFEEDBEEF0001ll;
  tci = 0x2E;
  pn = 1;
  {
    int j;
    key = create_u8_arr(16);
  //  foreach key_fld $keyStr {
    for (j=0; j<16; j++) {
      set_u8_arr_fld(key, j, keyStr_2[j]);
	}
  }

  ect = new_ctx_t(sci, pn, tci, conf_offset, key);
  printf("Setup ECT:\n");
  printf("Ent_num tci pn sci\n");
  printf("%-7d %3X %-3d 0x%llx", ent_num, tci, pn, sci);
  printf("\nkeyStr: ");
  for (j=0; j<16; j++) {
    printf("%02X ", keyStr_2[j]&0xff);
  }
  printf("\n");

//# Setup EGR entry 
//# setup ctrl port_num
  for(i=0;i<devp->numOfPorts; i+=2) {
    int eport;
	eport = eports [i];
    set_dev_ctrl_port_num(devp, eport);
    ect = new_ctx_t(sciList[i], pn, tci, conf_offset, key);
    msec_port_set_egr_entry(devp, eport % 4, ent_num, elk_t, ect);
    printf("Setup Egress port eports %d entry ent_num %d as configured above.", eport, ent_num);
    delete_ctx_t(ect);
  }

  //# release memory for ct, elk_t
  delete_lkup_t(elk_t);

  printf("\n");
//###########################################################
//# Entry 2
//# setup Ingress on port 1 & 3
//###########################################################"

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
  printf("ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian\n");
  printf("%7d %6d %5d %5d %5d %7d %5d %8d\n", ent_num, en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian);

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
    printf("ent_num  da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld\n");
    printf("%7d %6x %6x %8x %6x %7x %7x %11x\n", ent_num, da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld);

//# setup ILU action
  ctx_num =ent_num;
  drop =0;
  redir =0;
  auth_en =1;
  enc_en =1;
  drop_maxpn = 1;
  lk_act_fld = new_act_fld(ctx_num, drop_maxpn, drop, redir, auth_en, enc_en);
  printf("Setup ILU action field.\n");
  printf("ent_num ctx_num drop redir auth_en enc_en\n");
  printf("%7x %7x %4x %5x %7x %6x\n", ent_num, ctx_num, drop, redir, auth_en, enc_en);

  //# setup LKUP_T
    ilk_t = new_lkup_t(lk_match_en, lk_match_fld, lk_act_fld);
    printf("Setup ILU lookup table as configured above.\n");

//# setup CT
  sci =0ll;
  tci = 0;
  pn = 0;
//# IGR use same key as EGR, sci, tci, pn fields are not valid for ingress.
  ict = new_ctx_t(sci, pn, tci, conf_offset, key);
  printf("Setup ICT:\n");
  printf("Ent_num tci pn sci\n");
  printf("%-7d %3X %-3d 0x%llx", ent_num, tci, pn, sci);
  printf("\nkeyStr: ");
  for (j=0; j<16; j++) {
    printf("%02X ", keyStr_0[j]&0xff);
  }
  printf("\n");

//# setup ctrl port_num
  for(i=1;i<devp->numOfPorts; i+=2) {
    set_dev_ctrl_port_num(devp, i);
    //# Setup IGR entry 
    msec_port_set_egr_entry(devp, i, ent_num, ilk_t, ict);
    //# set port3 to retain encryption header
    if(i == 3) {
      msec_port_set_igr_retain_hdr(devp, i, retain_hdr);
	}
    printf("Setup port %d IGR entry %d as configured above.",i, ent_num);
  }

  //# release memory for ct, elk_t
  delete_u8_arr(key);
  delete_ctx_t(ict);
  delete_lkup_t(ilk_t);

  printf("\n");


//###########################################################
//# Entry 3
//# setup egress on port 0 & 2
//###########################################################"
  ent_num = 3;
//# setup ELU match enables
  en_def =0;
  en_mc = 0;
  en_da = 1;
  en_sa = 1;
  en_vlan = 1;
  en_et = 0;
  en_scian =0;
  lk_match_en = new_match_en(en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian, en_hdr);

  printf("Setup ELU match enables:\n");
  printf("ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian\n");
  printf("%7d %6d %5d %5d %5d %7d %5d %8d\n", ent_num, en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian);


//# setup ELU match fields
  da_fld = 0xFEEDD00DFFB0ll;
  sa_fld = 0xFEEDD00DFFB1ll;
  vlan_fld = 123;
  et_fld = 0x0800;
  sci_fld = 0ll;
  tci_fld = 0;
  tci_msk_fld = 0;
  conf_offset = 0;
  en_hdr = 1;
  lk_match_fld = new_match_fld(da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld, conf_offset, en_hdr);
  printf("Setup ELU match fields:\n");
    printf("ent_num  da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld\n");
    printf("%7d %6x %6x %8x %6x %7x %7x %11x\n", ent_num, da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld);


//# setup ELU action
  ctx_num = ent_num;
  drop = 0;
  redir = 0;
  auth_en = 1;
  enc_en =1;
  drop_maxpn = 1;
  lk_act_fld = new_act_fld(ctx_num, drop_maxpn, drop, redir, auth_en, enc_en);
  printf("Setup ELU action:\n");
  printf("ent_num ctx_num drop redir auth_en enc_en\n");
  printf("%7x %7x %4x %5x %7x %6x\n", ent_num, ctx_num, drop, redir, auth_en, enc_en);


//# setup LKUP_T
  elk_t = new_lkup_t(lk_match_en, lk_match_fld, lk_act_fld);
  printf("Setup Lookup Table as configured above.\n");

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
  printf("ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian\n");
  printf("%7d %6d %5d %5d %5d %7d %5d %8d\n", ent_num, en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian);

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
    printf("ent_num  da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld\n");
    printf("%7d %6x %6x %8x %6x %7x %7x %11x\n", ent_num, da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld);


//# setup ELU action
  ctx_num = ent_num;
  drop = 0;
  redir = 0;
  auth_en = 1;
  enc_en =1;
  drop_maxpn = 1;
  lk_act_fld = new_act_fld(ctx_num, drop_maxpn, drop, redir, auth_en, enc_en);
  printf("Setup ELU action:\n");
  printf("ent_num ctx_num drop redir auth_en enc_en\n");
  printf("%7x %7x %4x %5x %7x %6x\n", ent_num, ctx_num, drop, redir, auth_en, enc_en);

//# setup LKUP_T
  elk_t = new_lkup_t(lk_match_en, lk_match_fld, lk_act_fld);
  printf("Setup Lookup Table as configured above.\n");

//# setup ECT
//# The key array and ct are shared structures for egress and ingress,
//# it is created here and deleted later.
  sci =0xF00DFEEDBEEF0001ll;
  tci = 0x2F;
  pn = 1;
  {
    int j;
    key = create_u8_arr(16);
  //  foreach key_fld $keyStr {
    for (j=0; j<16; j++) {
      set_u8_arr_fld(key, j, keyStr_3[j]);
	}
  }

  ect = new_ctx_t(sci, pn, tci, conf_offset, key);
  printf("Setup ECT:\n");
  printf("Ent_num tci pn sci\n");
  printf("%-7d %3X %-3d 0x%llx", ent_num, tci, pn, sci);
  printf("\nkeyStr: ");
  for (j=0; j<16; j++) {
    printf("%02X ", keyStr_3[j]&0xff);
  }
  printf("\n");

//# Setup EGR entry 
//# setup ctrl port_num
  for(i=0;i<devp->numOfPorts; i+=2) {
    int eport;
	eport = eports [i];
    set_dev_ctrl_port_num(devp, eport);
    ect = new_ctx_t(sciList[i], pn, tci, conf_offset, key);
    msec_port_set_egr_entry(devp, eport % 4, ent_num, elk_t, ect);
    delete_ctx_t(ect);
    printf("Setup Egress port eports %d entry ent_num %d as configured above.", eport, ent_num);
  }

  //# release memory for ct, elk_t
  delete_lkup_t(elk_t);

  printf("\n");
//###########################################################
//# Entry 3
//# setup Ingress on port 1 & 3
//###########################################################"
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
  printf("ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian\n");
  printf("%7d %6d %5d %5d %5d %7d %5d %8d\n", ent_num, en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian);

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
    printf("ent_num  da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld\n");
    printf("%7d %6x %6x %8x %6x %7x %7x %11x\n", ent_num, da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld);

//# setup ILU action
  ctx_num =ent_num;
  drop =0;
  redir =0;
  auth_en =1;
  enc_en =1;
  drop_maxpn = 1;
  lk_act_fld = new_act_fld(ctx_num, drop_maxpn, drop, redir, auth_en, enc_en);
  printf("Setup ILU action field.\n");
  printf("ent_num ctx_num drop redir auth_en enc_en\n");
  printf("%7x %7x %4x %5x %7x %6x\n", ent_num, ctx_num, drop, redir, auth_en, enc_en);

  //# setup LKUP_T
    ilk_t = new_lkup_t(lk_match_en, lk_match_fld, lk_act_fld);
    printf("Setup ILU lookup table as configured above.\n");

//# setup CT
  sci =0ll;
  tci = 0;
  pn = 0;
//# IGR use same key as EGR, sci, tci, pn fields are not valid for ingress.
  ict = new_ctx_t(sci, pn, tci, conf_offset, key);
  printf("Setup ICT:\n");
  printf("Ent_num tci pn sci\n");
  printf("%-7d %3X %-3d 0x%llx", ent_num, tci, pn, sci);
  printf("\nkeyStr: ");
  for (j=0; j<16; j++) {
    printf("%02X ", keyStr_0[j]&0xff);
  }
  printf("\n");


//# setup ctrl port_num
  for(i=1;i<devp->numOfPorts; i+=2) {
    set_dev_ctrl_port_num(devp, i);
    //# Setup IGR entry 
    msec_port_set_egr_entry(devp, i, ent_num, ilk_t, ict);
    //# set port3 to retain encryption header
    if(i == 3) {
      msec_port_set_igr_retain_hdr(devp, i, retain_hdr);
	}
    printf("Setup port %d IGR entry %d as configured above.",i, ent_num);
  }

  //# release memory for ct, elk_t
  delete_u8_arr(key);
  delete_ctx_t(ict);
  delete_lkup_t(ilk_t);

  printf("\n");


//###########################################################
//# Entry 4
//# setup egress on port 0 & 2
//###########################################################"

  ent_num = 4;
//# setup ELU match enables
  en_def =0;
  en_mc = 0;
  en_da = 0;
  en_sa = 0;
  en_vlan =1;
  en_et = 0;
  en_scian =0;
  lk_match_en = new_match_en(en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian, en_hdr);

  printf("Setup ELU match enables:\n");
  printf("ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian\n");
  printf("%7d %6d %5d %5d %5d %7d %5d %8d\n", ent_num, en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian);


//# setup ELU match fields
  da_fld = 0ll;
  sa_fld = 0ll;
  vlan_fld = 456;
  et_fld = 0;
  sci_fld = 0ll;
  tci_fld = 0;
  tci_msk_fld = 0;
  conf_offset = 0;
  en_hdr = 1;
  lk_match_fld = new_match_fld(da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld, conf_offset, en_hdr);
  printf("Setup ELU match fields:\n");
    printf("ent_num  da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld\n");
    printf("%7d %6x %6x %8x %6x %7x %7x %11x\n", ent_num, da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld);


//# setup ELU action
  ctx_num = ent_num;
  drop = 0;
  redir = 0;
  auth_en = 1;
  enc_en =1;
  drop_maxpn = 1;
  lk_act_fld = new_act_fld(ctx_num, drop_maxpn, drop, redir, auth_en, enc_en);
  printf("Setup ELU action:\n");
  printf("ent_num ctx_num drop redir auth_en enc_en\n");
  printf("%7x %7x %4x %5x %7x %6x\n", ent_num, ctx_num, drop, redir, auth_en, enc_en);


//# setup LKUP_T
  elk_t = new_lkup_t(lk_match_en, lk_match_fld, lk_act_fld);
  printf("Setup Lookup Table as configured above.\n");

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
  printf("ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian\n");
  printf("%7d %6d %5d %5d %5d %7d %5d %8d\n", ent_num, en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian);

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
    printf("ent_num  da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld\n");
    printf("%7d %6x %6x %8x %6x %7x %7x %11x\n", ent_num, da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld);


//# setup ELU action
  ctx_num = ent_num;
  drop = 0;
  redir = 0;
  auth_en = 1;
  enc_en =1;
  drop_maxpn = 1;
  lk_act_fld = new_act_fld(ctx_num, drop_maxpn, drop, redir, auth_en, enc_en);
  printf("Setup ELU action:\n");
  printf("ent_num ctx_num drop redir auth_en enc_en\n");
  printf("%7x %7x %4x %5x %7x %6x\n", ent_num, ctx_num, drop, redir, auth_en, enc_en);

//# setup LKUP_T
  elk_t = new_lkup_t(lk_match_en, lk_match_fld, lk_act_fld);
  printf("Setup Lookup Table as configured above.\n");

//# setup ECT
//# The key array and ct are shared structures for egress and ingress,
//# it is created here and deleted later.
  sci =0xFEDCBA98FEDCBA98ll;
  tci = 0x23;
  pn = 1;
  {
    int j;
    key = create_u8_arr(16);
  //  foreach key_fld $keyStr {
    for (j=0; j<16; j++) {
      set_u8_arr_fld(key, j, keyStr_4[j]);
	}
  }

  ect = new_ctx_t(sci, pn, tci, conf_offset, key);
  printf("Setup ECT:\n");
  printf("Ent_num tci pn sci\n");
  printf("%-7d %3X %-3d 0x%llx", ent_num, tci, pn, sci);
  printf("\nkeyStr: ");
  for (j=0; j<16; j++) {
    printf("%02X ", keyStr_4[j]&0xff);
  }
  printf("\n");

//# Setup EGR entry 
//# setup ctrl port_num
  for(i=0;i<devp->numOfPorts; i+=2) {
    int eport;
	eport = eports [i];
    set_dev_ctrl_port_num(devp, eport);
    ect = new_ctx_t(sciList[i], pn, tci, conf_offset, key);
    msec_port_set_egr_entry(devp, eport % 4, ent_num, elk_t, ect);
    delete_ctx_t(ect);
    printf("Setup Egress port eports %d entry ent_num %d as configured above.", eport, ent_num);
  }

  //# release memory for ct, elk_t
  delete_lkup_t(elk_t);

  printf("\n");
//###########################################################
//# Entry 4
//# setup Ingress on port 1 & 3
//###########################################################"

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
  printf("ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian\n");
  printf("%7d %6d %5d %5d %5d %7d %5d %8d\n", ent_num, en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian);
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
    printf("ent_num  da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld\n");
    printf("%7d %6x %6x %8x %6x %7x %7x %11x\n", ent_num, da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld);

//# setup ILU action
  ctx_num =ent_num;
  drop =0;
  redir =0;
  auth_en =1;
  enc_en =0;
  drop_maxpn = 1;
  lk_act_fld = new_act_fld(ctx_num, drop_maxpn, drop, redir, auth_en, enc_en);
  printf("Setup ILU action field.\n");
  printf("ent_num ctx_num drop redir auth_en enc_en\n");
  printf("%7x %7x %4x %5x %7x %6x\n", ent_num, ctx_num, drop, redir, auth_en, enc_en);

  //# setup LKUP_T
    ilk_t = new_lkup_t(lk_match_en, lk_match_fld, lk_act_fld);
    printf("Setup ILU lookup table as configured above.\n");

//# setup CT
  sci =0ll;
  tci = 0;
  pn = 0;
//# IGR use same key as EGR, sci, tci, pn fields are not valid for ingress.
  ict = new_ctx_t(sci, pn, tci, conf_offset, key);
  printf("Setup ICT:\n");
  printf("Ent_num tci pn sci\n");
  printf("%-7d %3X %-3d 0x%llx", ent_num, tci, pn, sci);
  printf("\nkeyStr: ");
  for (j=0; j<16; j++) {
    printf("%02X ", keyStr_0[j]&0xff);
  }
  printf("\n");


//# setup ctrl port_num
  for(i=1;i<devp->numOfPorts; i+=2) {
    set_dev_ctrl_port_num(devp, i);
    //# Setup IGR entry 
    msec_port_set_egr_entry(devp, i, ent_num, ilk_t, ict);
    //# set port3 to retain encryption header
    if(i == 3) {
      msec_port_set_igr_retain_hdr(devp, i, retain_hdr);
	}
    printf("Setup port %d IGR entry %d as configured above.",i, ent_num);
  }

  //# release memory for ct, elk_t
  delete_u8_arr(key);
  delete_ctx_t(ict);
  delete_lkup_t(ilk_t);

  printf("\n");


//###########################################################
//# Entry 5
//# setup egress on port 0 & 2
//###########################################################"
  ent_num = 5;
//# setup ELU match enables
  en_def =0;
  en_mc = 0;
  en_da = 0;
  en_sa = 1;
  en_vlan =0;
  en_et = 0;
  en_scian =0;
  lk_match_en = new_match_en(en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian, en_hdr);

  printf("Setup ELU match enables:\n");
  printf("ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian\n");
  printf("%7d %6d %5d %5d %5d %7d %5d %8d\n", ent_num, en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian);


//# setup ELU match fields
  da_fld = 0ll;
  sa_fld = 0xFEEDD00DFFB1ll;
  vlan_fld = 0;
  et_fld = 0x0800;
  sci_fld = 0ll;
  tci_fld = 0;
  tci_msk_fld = 0;
  conf_offset = 0;
  en_hdr = 1;
  lk_match_fld = new_match_fld(da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld, conf_offset, en_hdr);
  printf("Setup ELU match fields:\n");
    printf("ent_num  da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld\n");
    printf("%7d %6x %6x %8x %6x %7x %7x %11x\n", ent_num, da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld);


//# setup ELU action
  ctx_num = ent_num;
  drop = 0;
  redir = 0;
  auth_en = 1;
  enc_en =0;
  drop_maxpn = 1;
  lk_act_fld = new_act_fld(ctx_num, drop_maxpn, drop, redir, auth_en, enc_en);
  printf("Setup ELU action:\n");
  printf("ent_num ctx_num drop redir auth_en enc_en\n");
  printf("%7x %7x %4x %5x %7x %6x\n", ent_num, ctx_num, drop, redir, auth_en, enc_en);


//# setup LKUP_T
  elk_t = new_lkup_t(lk_match_en, lk_match_fld, lk_act_fld);
  printf("Setup Lookup Table as configured above.\n");

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
  printf("ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian\n");
  printf("%7d %6d %5d %5d %5d %7d %5d %8d\n", ent_num, en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian);

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
    printf("ent_num  da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld\n");
    printf("%7d %6x %6x %8x %6x %7x %7x %11x\n", ent_num, da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld);


//# setup ELU action
  ctx_num = ent_num;
  drop = 0;
  redir = 0;
  auth_en = 1;
  enc_en =1;
  drop_maxpn = 1;
  lk_act_fld = new_act_fld(ctx_num, drop_maxpn, drop, redir, auth_en, enc_en);
  printf("Setup ELU action:\n");
  printf("ent_num ctx_num drop redir auth_en enc_en\n");
  printf("%7x %7x %4x %5x %7x %6x\n", ent_num, ctx_num, drop, redir, auth_en, enc_en);

//# setup LKUP_T
  elk_t = new_lkup_t(lk_match_en, lk_match_fld, lk_act_fld);
  printf("Setup Lookup Table as configured above.\n");

  //# setup ECT
//# The key array and ct are shared structures for egress and ingress,
//# it is created here and deleted later.
  sci =0x9876543201234567ll;
  tci = 0x22;
  pn = 1;
  {
    int j;
    key = create_u8_arr(16);
  //  foreach key_fld $keyStr {
    for (j=0; j<16; j++) {
      set_u8_arr_fld(key, j, keyStr_5[j]);
	}
  }

  ect = new_ctx_t(sci, pn, tci, conf_offset, key);
  printf("Setup ECT:\n");
  printf("Ent_num tci pn sci\n");
  printf("%-7d %3X %-3d 0x%llx", ent_num, tci, pn, sci);
  printf("\nkeyStr: ");
  for (j=0; j<16; j++) {
    printf("%02X ", keyStr_5[j]&0xff);
  }
  printf("\n");

//# Setup EGR entry 
//# setup ctrl port_num
  for(i=0;i<devp->numOfPorts; i+=2) {
    int eport;
	eport = eports [i];
    set_dev_ctrl_port_num(devp, eport);
    ect = new_ctx_t(sciList[i], pn, tci, conf_offset, key);
    msec_port_set_egr_entry(devp, eport % 4, ent_num, elk_t, ect);
    delete_ctx_t(ect);
    printf("Setup Egress port eports %d entry ent_num %d as configured above.", eport, ent_num);
  }

  //# release memory for ct, elk_t
  delete_lkup_t(elk_t);

  printf("\n");
//###########################################################
//# Entry 5
//# setup Ingress on port 1 & 3
//###########################################################
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
  printf("ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian\n");
  printf("%7d %6d %5d %5d %5d %7d %5d %8d\n", ent_num, en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian);

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
    printf("ent_num  da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld\n");
    printf("%7d %6x %6x %8x %6x %7x %7x %11x\n", ent_num, da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld);

//# setup ILU action
  ctx_num =ent_num;
  drop =0;
  redir =0;
  auth_en =1;
  enc_en =0;
  drop_maxpn = 1;
  lk_act_fld = new_act_fld(ctx_num, drop_maxpn, drop, redir, auth_en, enc_en);
  printf("Setup ILU action field.\n");
  printf("ent_num ctx_num drop redir auth_en enc_en\n");
  printf("%7x %7x %4x %5x %7x %6x\n", ent_num, ctx_num, drop, redir, auth_en, enc_en);

  //# setup LKUP_T
    ilk_t = new_lkup_t(lk_match_en, lk_match_fld, lk_act_fld);
    printf("Setup ILU lookup table as configured above.\n");

//# setup CT
  sci =0ll;
  tci = 0;
  pn = 0;
//# IGR use same key as EGR, sci, tci, pn fields are not valid for ingress.
  ict = new_ctx_t(sci, pn, tci, conf_offset, key);
  printf("Setup ICT:\n");
  printf("Ent_num tci pn sci\n");
  printf("%-7d %3X %-3d 0x%llx", ent_num, tci, pn, sci);
  printf("\nkeyStr: ");
  for (j=0; j<16; j++) {
    printf("%02X ", keyStr_0[j]&0xff);
  }
  printf("\n");


//# setup ctrl port_num
  for(i=1;i<devp->numOfPorts; i+=2) {
    set_dev_ctrl_port_num(devp, i);
    //# Setup IGR entry 
    msec_port_set_egr_entry(devp, i, ent_num, ilk_t, ict);
    //# set port3 to retain encryption header
    if(i == 3) {
      msec_port_set_igr_retain_hdr(devp, i, retain_hdr);
	}
    printf("Setup port %d IGR entry %d as configured above.",i, ent_num);
  }

  //# release memory for ct, elk_t
  delete_u8_arr(key);
  delete_ctx_t(ict);
  delete_lkup_t(ilk_t);

  printf("\n");


//###########################################################
//# Entry 6
//# setup egress on port 0 & 2
//###########################################################"

  ent_num = 6;
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
  printf("ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian\n");
  printf("%7d %6d %5d %5d %5d %7d %5d %8d\n", ent_num, en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian);


//# setup ELU match fields
  da_fld = 0ll;
  sa_fld = 0ll;
  vlan_fld = 0;
  et_fld = 0x0806;
  sci_fld = 0ll;
  tci_fld = 0;
  tci_msk_fld = 0;
  conf_offset = 0;
  en_hdr = 1;
  lk_match_fld = new_match_fld(da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld, conf_offset, en_hdr);
  printf("Setup ELU match fields:\n");
    printf("ent_num  da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld\n");
    printf("%7d %6x %6x %8x %6x %7x %7x %11x\n", ent_num, da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld);


//# setup ELU action
  ctx_num = ent_num;
  drop = 0;
  redir = 0;
  auth_en = 1;
  enc_en =0;
  drop_maxpn = 1;
  lk_act_fld = new_act_fld(ctx_num, drop_maxpn, drop, redir, auth_en, enc_en);
  printf("Setup ELU action:\n");
  printf("ent_num ctx_num drop redir auth_en enc_en\n");
  printf("%7x %7x %4x %5x %7x %6x\n", ent_num, ctx_num, drop, redir, auth_en, enc_en);


//# setup LKUP_T
  elk_t = new_lkup_t(lk_match_en, lk_match_fld, lk_act_fld);
  printf("Setup Lookup Table as configured above.\n");

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
  printf("ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian\n");
  printf("%7d %6d %5d %5d %5d %7d %5d %8d\n", ent_num, en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian);

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
    printf("ent_num  da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld\n");
    printf("%7d %6x %6x %8x %6x %7x %7x %11x\n", ent_num, da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld);


//# setup ELU action
  ctx_num = ent_num;
  drop = 0;
  redir = 0;
  auth_en = 1;
  enc_en =1;
  drop_maxpn = 1;
  lk_act_fld = new_act_fld(ctx_num, drop_maxpn, drop, redir, auth_en, enc_en);
  printf("Setup ELU action:\n");
  printf("ent_num ctx_num drop redir auth_en enc_en\n");
  printf("%7x %7x %4x %5x %7x %6x\n", ent_num, ctx_num, drop, redir, auth_en, enc_en);

//# setup LKUP_T
  elk_t = new_lkup_t(lk_match_en, lk_match_fld, lk_act_fld);
  printf("Setup Lookup Table as configured above.\n");


//# setup ECT
//# The key array and ct are shared structures for egress and ingress,
//# it is created here and deleted later.
  sci =0x7676543298766789ll;
  tci = 0x21;
  pn = 1;
  {
    int j;
    key = create_u8_arr(16);
  //  foreach key_fld $keyStr {
    for (j=0; j<16; j++) {
      set_u8_arr_fld(key, j, keyStr_6[j]);
	}
  }

  ect = new_ctx_t(sci, pn, tci, conf_offset, key);
  printf("Setup ECT:\n");
  printf("Ent_num tci pn sci\n");
  printf("%-7d %3X %-3d 0x%llx", ent_num, tci, pn, sci);
  printf("\nkeyStr: ");
  for (j=0; j<16; j++) {
    printf("%02X ", keyStr_6[j]&0xff);
  }
  printf("\n");

//# Setup EGR entry 
//# setup ctrl port_num
  for(i=0;i<devp->numOfPorts; i+=2) {
    int eport;
	eport = eports [i];
    set_dev_ctrl_port_num(devp, eport);
    ect = new_ctx_t(sciList[i], pn, tci, conf_offset, key);
    msec_port_set_egr_entry(devp, eport % 4, ent_num, elk_t, ect);
    delete_ctx_t(ect);
  printf("Setup Egress port eports %d entry ent_num %d as configured above.", eport, ent_num);
  }
  //# release memory for ct, elk_t
  delete_lkup_t(elk_t);

  printf("\n");
//###########################################################
//# Entry 6
//# setup Ingress on port 1 & 3
//###########################################################"

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
  printf("ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian\n");
  printf("%7d %6d %5d %5d %5d %7d %5d %8d\n", ent_num, en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian);
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
    printf("ent_num  da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld\n");
    printf("%7d %6x %6x %8x %6x %7x %7x %11x\n", ent_num, da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld);

//# setup ILU action
  ctx_num =ent_num;
  drop =0;
  redir =0;
  auth_en =1;
  enc_en =0;
  drop_maxpn = 1;
  lk_act_fld = new_act_fld(ctx_num, drop_maxpn, drop, redir, auth_en, enc_en);
  printf("Setup ILU action field.\n");
  printf("ent_num ctx_num drop redir auth_en enc_en\n");
  printf("%7x %7x %4x %5x %7x %6x\n", ent_num, ctx_num, drop, redir, auth_en, enc_en);

  //# setup LKUP_T
    ilk_t = new_lkup_t(lk_match_en, lk_match_fld, lk_act_fld);
    printf("Setup ILU lookup table as configured above.\n");

//# setup CT
  sci =0ll;
  tci = 0;
  pn = 0;
//# IGR use same key as EGR, sci, tci, pn fields are not valid for ingress.
  ict = new_ctx_t(sci, pn, tci, conf_offset, key);
  printf("Setup ICT:\n");
  printf("Ent_num tci pn sci\n");
  printf("%-7d %3X %-3d 0x%llx", ent_num, tci, pn, sci);
  printf("\nkeyStr: ");
  for (j=0; j<16; j++) {
    printf("%02X ", keyStr_0[j]&0xff);
  }
  printf("\n");


//# setup ctrl port_num
  for(i=1;i<devp->numOfPorts; i+=2) {
    set_dev_ctrl_port_num(devp, i);
    //# Setup IGR entry 
    msec_port_set_egr_entry(devp, i, ent_num, ilk_t, ict);
    //# set port3 to retain encryption header
    if(i == 3) {
      msec_port_set_igr_retain_hdr(devp, i, retain_hdr);
	}
    printf("Setup port %d IGR entry %d as configured above.",i, ent_num);
  }

  //# release memory for ct, elk_t
  delete_u8_arr(key);
  delete_ctx_t(ict);
  delete_lkup_t(ilk_t);

  printf("\n");


//###########################################################
//# Entry 7
//# setup egress on port 0 & 2
//###########################################################"
  ent_num = 7;
//# setup ELU match enables
  en_def =0;
  en_mc = 0;
  en_da = 1;
  en_sa = 0;
  en_vlan =0;
  en_et = 0;
  en_scian =0;
  lk_match_en = new_match_en(en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian, en_hdr);

  printf("Setup ELU match enables:\n");
  printf("ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian\n");
  printf("%7d %6d %5d %5d %5d %7d %5d %8d\n", ent_num, en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian);


//# setup ELU match fields
  da_fld = 0xFEEDD00DFFB0ll;
  sa_fld = 0ll;
  vlan_fld = 0;
  et_fld = 0;
  sci_fld = 0ll;
  tci_fld = 0;
  tci_msk_fld = 0;
  conf_offset = 0;
  en_hdr = 1;
  lk_match_fld = new_match_fld(da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld, conf_offset, en_hdr);
  printf("Setup ELU match fields:\n");
    printf("ent_num  da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld\n");
    printf("%7d %6x %6x %8x %6x %7x %7x %11x\n", ent_num, da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld);


//# setup ELU action
  ctx_num = ent_num;
  drop = 0;
  redir = 0;
  auth_en = 1;
  enc_en =0;
  drop_maxpn = 1;
  lk_act_fld = new_act_fld(ctx_num, drop_maxpn, drop, redir, auth_en, enc_en);
  printf("Setup ELU action:\n");
  printf("ent_num ctx_num drop redir auth_en enc_en\n");
  printf("%7x %7x %4x %5x %7x %6x\n", ent_num, ctx_num, drop, redir, auth_en, enc_en);


//# setup LKUP_T
  elk_t = new_lkup_t(lk_match_en, lk_match_fld, lk_act_fld);
  printf("Setup Lookup Table as configured above.\n");

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
  printf("ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian\n");
  printf("%7d %6d %5d %5d %5d %7d %5d %8d\n", ent_num, en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian);

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
    printf("ent_num  da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld\n");
    printf("%7d %6x %6x %8x %6x %7x %7x %11x\n", ent_num, da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld);


//# setup ELU action
  ctx_num = ent_num;
  drop = 0;
  redir = 0;
  auth_en = 1;
  enc_en =1;
  drop_maxpn = 1;
  lk_act_fld = new_act_fld(ctx_num, drop_maxpn, drop, redir, auth_en, enc_en);
  printf("Setup ELU action:\n");
  printf("ent_num ctx_num drop redir auth_en enc_en\n");
  printf("%7x %7x %4x %5x %7x %6x\n", ent_num, ctx_num, drop, redir, auth_en, enc_en);

//# setup LKUP_T
  elk_t = new_lkup_t(lk_match_en, lk_match_fld, lk_act_fld);
  printf("Setup Lookup Table as configured above.\n");

//# setup ECT
//# The key array and ct are shared structures for egress and ingress,
//# it is created here and deleted later.
  sci =0xD00D8888FEED1121ll;
  tci = 0x20;
  pn = 1;
  {
    int j;
    key = create_u8_arr(16);
  //  foreach key_fld $keyStr {
    for (j=0; j<16; j++) {
      set_u8_arr_fld(key, j, keyStr_7[j]);
	}
  }

  ect = new_ctx_t(sci, pn, tci, conf_offset, key);
  printf("Setup ECT:\n");
  printf("Ent_num tci pn sci\n");
  printf("%-7d %3X %-3d 0x%llx", ent_num, tci, pn, sci);
  printf("\nkeyStr: ");
  for (j=0; j<16; j++) {
    printf("%02X ", keyStr_7[j]&0xff);
  }
  printf("\n");

//# Setup EGR entry 
//# setup ctrl port_num
  for(i=1;i<devp->numOfPorts; i+=2) {
    int eport;
	eport = eports [i];
    set_dev_ctrl_port_num(devp, eport);
    ect = new_ctx_t(sciList[i], pn, tci, conf_offset, key);
    msec_port_set_egr_entry(devp, eport % 4, ent_num, elk_t, ect);
    delete_ctx_t(ect);
    printf("Setup Egress port eports %d entry ent_num %d as configured above.", eport, ent_num);
  }

  //# release memory for ct, elk_t
  delete_lkup_t(elk_t);

  printf("\n");
//###########################################################
//# Entry 7
//# setup Ingress on port 1 & 3
//###########################################################"
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
  printf("ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian\n");
  printf("%7d %6d %5d %5d %5d %7d %5d %8d\n", ent_num, en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian);

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
    printf("ent_num  da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld\n");
    printf("%7d %6x %6x %8x %6x %7x %7x %11x\n", ent_num, da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld);

//# setup ILU action
  ctx_num =ent_num;
  drop =0;
  redir =0;
  auth_en =1;
  enc_en =0;
  drop_maxpn = 1;
  lk_act_fld = new_act_fld(ctx_num, drop_maxpn, drop, redir, auth_en, enc_en);
  printf("Setup ILU action field.\n");
  printf("ent_num ctx_num drop redir auth_en enc_en\n");
  printf("%7x %7x %4x %5x %7x %6x\n", ent_num, ctx_num, drop, redir, auth_en, enc_en);

  //# setup LKUP_T
    ilk_t = new_lkup_t(lk_match_en, lk_match_fld, lk_act_fld);
    printf("Setup ILU lookup table as configured above.\n");

//# setup CT
  sci =0ll;
  tci = 0;
  pn = 0;
//# IGR use same key as EGR, sci, tci, pn fields are not valid for ingress.
  ict = new_ctx_t(sci, pn, tci, conf_offset, key);
  printf("Setup ICT:\n");
  printf("Ent_num tci pn sci\n");
  printf("%-7d %3X %-3d 0x%llx", ent_num, tci, pn, sci);
  printf("\nkeyStr: ");
  for (j=0; j<16; j++) {
    printf("%02X ", keyStr_0[j]&0xff);
  }
  printf("\n");


//# setup ctrl port_num
  for(i=1;i<devp->numOfPorts; i+=2) {
    set_dev_ctrl_port_num(devp, i);
    //# Setup IGR entry 
    msec_port_set_egr_entry(devp, i, ent_num, ilk_t, ict);
    //# set port3 to retain encryption header
    if(i == 3) {
      msec_port_set_igr_retain_hdr(devp, i, retain_hdr);
	}
    printf("Setup port %d IGR entry %d as configured above.",i, ent_num);
  }

  //# release memory for ct, elk_t
  delete_u8_arr(key);
  delete_ctx_t(ict);
  delete_lkup_t(ilk_t);

  printf("\n");


#if 0

//####################################################################
//# send traffic from N2X to system MAC of Egress port0, 2
//# receive traffic from N2X connected to system MAC of port1, 3
//# compare frame received shall be same as frame sent
//# setup WIRE side loopback between port0 and port1, port2 and port3
//####################################################################"
set portLabelList "101/1 101/2 101/3 101/4"
set portList "1 2 3 4"
set smac "FE:ED:D0:0D:FF:A1";#match SA
set dmac "FE:ED:D0:0D:FF:A0";#match DA
set vlan 123;#match vlan
set rc 0
global logFile

printf("\n
//# to exercise ent_num 0"
foreach eport $eports iport $iports {
  printf("send one tagged frame to port $eport, receive on port $iport."
  printf("vlan $vlan srcmac: $smac, destmac: $dmac"
  printf("Expect the frame to hit ent_num 0."
  # convert to 1-based port number for N2X
  set sport [expr $eport + 1]
  set dport [expr $iport + 1]
  # send one layer2 frame with vlan_tag, src_port, dest_port, src_mac, dest_mac as input
  set recvFrame [send_one_tag_frame_cap_recv $argv0 $portList $sport $dport  $smac $dmac $vlan]
  printf("Dump frame received on port $dport:\n$recvFrame\n"
  # check frame is received on destination port
  parse_log_file $logFile
  if {[frame_is_rcvd $sport $dport] == -1} {
     printf("Frame is not received on ingress port $iport, incorrect."
     set rc -1
  } else {
    printf("Frame is received on ingress port $iport correctly."
    # check retain hdr
    set_dev_ctrl_port_num $devp $iport
    if {[msec_port_get_igr_retain_hdr $devp] == 1 } {
      if { [check_macsec_retain_hdr $recvFrame] == 0} {
        printf("Retain hdr is expected but not found on port $iport, incorrect."
        set rc -1
      } else {
        printf("Retain hdr is expected and is found on port $iport, correct."
      }
    }
  }  

  after 100 
  dump_log_details $logFile $hResult
}

//# Dump all MACSec statistic counters
dump_macsec_statistics $hResult $devp

foreach eport $eports iport $iports {
  set egr_hit_cnt [get_macsec_stats_by_name $eport MX_SEC_EGR_HIT_E0]
  puts -nonewline $hResult "eport $eport MX_SEC_EGR_HIT_E0 $egr_hit_cnt, "
  if {$egr_hit_cnt == 1 } {
    printf("correct."
  } else {
    printf("incorrect."
    set egr_miss_cnt [get_macsec_stats_by_name $eport MX_SEC_EGR_MISS]
    puts -nonewline $hResult "eport $eport MX_SEC_EGR_MISS $egr_miss_cnt, "
    set rc -1
  }

  set egr_enc_cnt [get_macsec_stats_by_name $eport MX_SEC_EGR_PKT_ENC_E0]
  puts -nonewline $hResult "eport $eport MX_SEC_EGR_PKT_ENC_E0 $egr_enc_cnt, "
  if {$egr_enc_cnt == 1 } {
    printf("correct."
  } else {
    printf("incorrect."
    set rc -1
  }
  set igr_hit_cnt [get_macsec_stats_by_name $iport MX_SEC_IGR_HIT_E0]
  puts -nonewline $hResult "iport $iport MX_SEC_IGR_HIT_E0 $igr_hit_cnt, "
  if {$igr_hit_cnt == 1 } {
    printf("correct."
  } else {
    printf("incorrect."
    set igr_miss_cnt [get_macsec_stats_by_name $iport MX_SEC_IGR_MISS]
    puts -nonewline $hResult "iport $iport MX_SEC_IGR_MISS $igr_miss_cnt, "
    set rc -1
  }

  set igr_ok_cnt [get_macsec_stats_by_name $iport MX_SEC_IGR_OK_E0]
  puts -nonewline $hResult "iport $iport MX_SEC_IGR_OK_E0 $igr_ok_cnt, "
  if {$igr_ok_cnt == 1 } {
    printf("correct."
  } else {
    printf("incorrect."
    set rc -1
  }
}

printf("\n
//# to exercise ent_num 1"
set smac "FE:ED:D0:0D:FF:A3"
set dmac "FE:ED:D0:0D:FF:A0";#match DA
set vlan 123;#match vlan
foreach eport $eports iport $iports {
  printf("send tagged frame to port $eport, receive on port $iport."
  printf("vlan $vlan srcmac: $smac, destmac: $dmac"
  printf("Expect the frame to hit ent_num 1."
  # convert to 1-based port number for N2X
  set sport [expr $eport + 1]
  set dport [expr $iport + 1]
  set recvFrame [send_one_tag_frame_cap_recv $argv0 $portList $sport $dport  $smac $dmac $vlan]
  printf("Dump frame received on port $dport:\n$recvFrame\n"
  after 100 
  dump_log_details $logFile $hResult
  # check frame is received on destination port
  parse_log_file $logFile
  if {[frame_is_rcvd $sport $dport] == -1} {
     printf("Frame is not received on ingress port $iport, incorrect."
     set rc -1
  } else {
    printf("Frame is received on ingress port $iport correctly."
    # check retain hdr
    set_dev_ctrl_port_num $devp $iport
    if {[msec_port_get_igr_retain_hdr $devp] == 1 && [check_macsec_retain_hdr $recvFrame] == 0} {
       printf("Retain hdr is expected but not found on port $iport, incorrect."
       set rc -1
    }
  }  
}

//# Dump all MACSec statistic counters
dump_macsec_statistics $hResult $devp

foreach eport $eports iport $iports {
  set egr_hit_cnt [get_macsec_stats_by_name $eport MX_SEC_EGR_HIT_E1]
  puts -nonewline $hResult "eport $eport MX_SEC_EGR_HIT_E1 $egr_hit_cnt, "
  if {$egr_hit_cnt == 1 } {
	  printf("correct."
  } else {
	  printf("incorrect."
    set egr_miss_cnt [get_macsec_stats_by_name $eport MX_SEC_EGR_MISS]
    puts -nonewline $hResult "eport $eport MX_SEC_EGR_MISS $egr_miss_cnt, "
    set rc -1
  }

  set egr_enc_cnt [get_macsec_stats_by_name $eport MX_SEC_EGR_PKT_ENC_E1]
  puts -nonewline $hResult "eport $eport MX_SEC_EGR_PKT_ENC_E1 $egr_enc_cnt, "
  if {$egr_enc_cnt == 1 } {
    printf("correct."
  } else {
    printf("incorrect."
    set rc -1
  }
  set igr_hit_cnt [get_macsec_stats_by_name $iport MX_SEC_IGR_HIT_E1]
  puts -nonewline $hResult "iport $iport MX_SEC_IGR_HIT_E1 $igr_hit_cnt, "
  if {$igr_hit_cnt == 1 } {
	  printf("correct."
  } else {
	  printf("incorrect."
    set igr_miss_cnt [get_macsec_stats_by_name $iport MX_SEC_IGR_MISS]
    puts -nonewline $hResult "iport $iport MX_SEC_IGR_MISS $igr_miss_cnt, "
    set rc -1
  }

  set igr_ok_cnt [get_macsec_stats_by_name $iport MX_SEC_IGR_OK_E1]
  puts -nonewline $hResult "iport $iport MX_SEC_IGR_OK_E1 $igr_ok_cnt, "
  if {$igr_ok_cnt == 1 } {
    printf("correct."
  } else {
    printf("incorrect."
    set rc -1
  }
}

printf("\n
//# to exercise ent_num 2"
set smac "fe:ed:d0:0d:ff:a1"
set dmac "fe:ed:d0:0d:ff:a2"
foreach eport $eports iport $iports {
  printf("send one frame to port $eport, receive on port $iport."
  printf("Expect the frame to hit ent_num 2."
  # convert to 1-based port number for N2X
  set sport [expr $eport + 1]
  set dport [expr $iport + 1]
  set recvFrame [send_one_frame_cap_recv $argv0 $portList $sport $dport  $smac $dmac]
  printf("srcmac: $smac, destmac: $dmac"
  printf("Dump frame received on port $dport:\n$recvFrame\n"
  after 100 
  dump_log_details $logFile $hResult
  # check frame is received on destination port
  parse_log_file $logFile
  if {[frame_is_rcvd $sport $dport] == -1} {
     printf("Frame is not received on ingress port $iport, incorrect."
     set rc -1
  } else {
    printf("Frame is received on ingress port $iport correctly."
    # check retain hdr
    set_dev_ctrl_port_num $devp $iport
    if {[msec_port_get_igr_retain_hdr $devp] == 1 && [check_macsec_retain_hdr $recvFrame] == 0} {
       printf("Retain hdr is expected but not found on port $iport, incorrect."
       set rc -1
    }  
  }
}

//# Dump all MACSec statistic counters
dump_macsec_statistics $hResult $devp

foreach eport $eports iport $iports {
  set egr_hit_cnt [get_macsec_stats_by_name $eport MX_SEC_EGR_HIT_E2]
  puts -nonewline $hResult "eport $eport MX_SEC_EGR_HIT_E2 $egr_hit_cnt, "
  if {$egr_hit_cnt == 1 } {
	  printf("correct."
  } else {
	  printf("incorrect."
    set egr_miss_cnt [get_macsec_stats_by_name $eport MX_SEC_EGR_MISS]
    puts -nonewline $hResult "eport $eport MX_SEC_EGR_MISS $egr_miss_cnt, "
    set rc -1
  }

  set egr_enc_cnt [get_macsec_stats_by_name $eport MX_SEC_EGR_PKT_ENC_E2]
  puts -nonewline $hResult "eport $eport MX_SEC_EGR_PKT_ENC_E2 $egr_enc_cnt, "
  if {$egr_enc_cnt == 1 } {
    printf("correct."
  } else {
    printf("incorrect."
    set rc -1
  }
  set igr_hit_cnt [get_macsec_stats_by_name $iport MX_SEC_IGR_HIT_E2]
  puts -nonewline $hResult "iport $iport MX_SEC_IGR_HIT_E2 $igr_hit_cnt, "
  if {$igr_hit_cnt == 1 } {
	  printf("correct."
  } else {
	  printf("incorrect."
    set igr_miss_cnt [get_macsec_stats_by_name $iport MX_SEC_IGR_MISS]
    puts -nonewline $hResult "iport $iport MX_SEC_IGR_MISS $igr_miss_cnt, "
    set rc -1
  }

  set igr_ok_cnt [get_macsec_stats_by_name $iport MX_SEC_IGR_OK_E2]
  puts -nonewline $hResult "iport $iport MX_SEC_IGR_OK_E2 $igr_ok_cnt, "
  if {$igr_ok_cnt == 1 } {
    printf("correct."
  } else {
    printf("incorrect."
    set rc -1
  }
}

printf("\n
//# to exercise ent_num 3"
set smac "fe:ed:d0:0d:ff:b1";#match SA
set dmac "fe:ed:d0:0d:ff:b0";#match DA
set vlan 123;#match vlan
foreach eport $eports iport $iports {
  printf("send one tagged frame to port $eport, receive on port $iport."
  printf("Expect the frame to hit ent_num 3."
  # convert to 1-based port number for N2X
  set sport [expr $eport + 1]
  set dport [expr $iport + 1]
  set recvFrame [send_one_tag_frame_cap_recv $argv0 $portList $sport $dport  $smac $dmac $vlan]
  printf("vlan $vlan srcmac: $smac, destmac: $dmac"
  printf("Dump frame received on port $dport:\n$recvFrame\n"
  after 100 
  dump_log_details $logFile $hResult
  # check frame is received on destination port
  parse_log_file $logFile
  if {[frame_is_rcvd $sport $dport] == -1} {
     printf("Frame is not received on ingress port $iport, incorrect."
     set rc -1
  } else {
     printf("Frame is received on ingress port $iport correctly."
     # check retain hdr
     set_dev_ctrl_port_num $devp $iport
     if {[msec_port_get_igr_retain_hdr $devp] == 1 && [check_macsec_retain_hdr $recvFrame] == 0} {
       printf("Retain hdr is expected but not found on port $iport, incorrect."
       set rc -1
     }
  }  
}

//# Dump all MACSec statistic counters
dump_print_macsec_statistics $hResult $devp

foreach eport $eports iport $iports {
  set egr_hit_cnt [get_macsec_stats_by_name $eport MX_SEC_EGR_HIT_E3]
  puts -nonewline $hResult "eport $eport MX_SEC_EGR_HIT_E3 $egr_hit_cnt, "
  if {$egr_hit_cnt == 1 } {
	  printf("correct."
  } else {
	  printf("incorrect."
    set egr_miss_cnt [get_macsec_stats_by_name $eport MX_SEC_EGR_MISS]
    puts -nonewline $hResult "eport $eport MX_SEC_EGR_MISS $egr_miss_cnt, "
    set rc -1
  }

  set egr_enc_cnt [get_macsec_stats_by_name $eport MX_SEC_EGR_PKT_ENC_E3]
  puts -nonewline $hResult "eport $eport MX_SEC_EGR_PKT_ENC_E3 $egr_enc_cnt, "
  if {$egr_enc_cnt == 1 } {
    printf("correct."
  } else {
    printf("incorrect."
    set rc -1
  }
  set igr_hit_cnt [get_macsec_stats_by_name $iport MX_SEC_IGR_HIT_E3]
  puts -nonewline $hResult "iport $iport MX_SEC_IGR_HIT_E3 $igr_hit_cnt, "
  if {$igr_hit_cnt == 1 } {
	  printf("correct."
  } else {
	  printf("incorrect."
    set igr_miss_cnt [get_macsec_stats_by_name $iport MX_SEC_IGR_MISS]
    puts -nonewline $hResult "iport $iport MX_SEC_IGR_MISS $igr_miss_cnt, "
    set rc -1
  }

  set igr_ok_cnt [get_macsec_stats_by_name $iport MX_SEC_IGR_OK_E3]
  puts -nonewline $hResult "iport $iport MX_SEC_IGR_OK_E3 $igr_ok_cnt, "
  if {$igr_ok_cnt == 1 } {
    printf("correct."
  } else {
    printf("incorrect."
    set rc -1
  }
}

printf("\n
//# to exercise ent_num 4"
set smac "fe:ed:d0:0d:ff:41"
set dmac "fe:ed:d0:0d:ff:40"
set vlan 456;#match vlan
foreach eport $eports iport $iports {
  printf("send one tagged frame to port $eport, receive on port $iport."
  printf("Expect the frame to hit ent_num 4."
  # convert to 1-based port number for N2X
  set sport [expr $eport + 1]
  set dport [expr $iport + 1]
  set recvFrame [send_one_tag_frame_cap_recv $argv0 $portList $sport $dport  $smac $dmac $vlan]
  printf("vlan $vlan srcmac: $smac, destmac: $dmac"
  printf("Dump frame received on port $dport:\n$recvFrame\n"
  after 100 
  dump_log_details $logFile $hResult
  # check frame is received on destination port
  parse_log_file $logFile
  if {[frame_is_rcvd $sport $dport] == -1} {
     printf("Frame is not received on ingress port $iport, incorrect."
     set rc -1
  } else {
     printf("Frame is received on ingress port $iport correctly."
     # check retain hdr
     set_dev_ctrl_port_num $devp $iport
     if {[msec_port_get_igr_retain_hdr $devp] == 1 && [check_macsec_retain_hdr $recvFrame] == 0} {
       printf("Retain hdr is expected but not found on port $iport, incorrect."
       set rc -1
     }
  }  
}

//# Dump all MACSec statistic counters
dump_print_macsec_statistics $hResult $devp

foreach eport $eports iport $iports {
  set egr_hit_cnt [get_macsec_stats_by_name $eport MX_SEC_EGR_HIT_E4]
  puts -nonewline $hResult "eport $eport MX_SEC_EGR_HIT_E4 $egr_hit_cnt, "
  if {$egr_hit_cnt == 1 } {
	  printf("correct."
  } else {
	  printf("incorrect."
    set egr_miss_cnt [get_macsec_stats_by_name $eport MX_SEC_EGR_MISS]
    puts -nonewline $hResult "eport $eport MX_SEC_EGR_MISS $egr_miss_cnt, "
    set rc -1
  }

  set egr_prot_cnt [get_macsec_stats_by_name $eport MX_SEC_EGR_PKT_PROT_E4]
  puts -nonewline $hResult "eport $eport MX_SEC_EGR_PKT_PROT_E4 $egr_prot_cnt, "
  if {$egr_prot_cnt == 1 } {
    printf("correct."
  } else {
    printf("incorrect."
    set rc -1
  }
  set igr_hit_cnt [get_macsec_stats_by_name $iport MX_SEC_IGR_HIT_E4]
  puts -nonewline $hResult "iport $iport IGR Hit Cnt $igr_hit_cnt for Entry 4, "
  if {$igr_hit_cnt == 1 } {
	  printf("correct."
  } else {
	  printf("incorrect."
    set igr_miss_cnt [get_macsec_stats_by_name $iport MX_SEC_IGR_MISS]
    puts -nonewline $hResult "iport $iport MX_SEC_IGR_HIT_E4 $igr_miss_cnt, "
    set rc -1
  }

  set igr_ok_cnt [get_macsec_stats_by_name $iport MX_SEC_IGR_OK_E4]
  puts -nonewline $hResult "iport $iport MX_SEC_IGR_OK_E4 $igr_ok_cnt, "
  if {$igr_ok_cnt == 1 } {
    printf("correct."
  } else {
    printf("incorrect."
    set rc -1
  }
}

printf("\n
//# to exercise ent_num 5"
set smac "fe:ed:d0:0d:ff:b1";#match SA
set dmac "fe:ed:d0:0d:ff:50";
set vlan 125
foreach eport $eports iport $iports {
  printf("send one tagged frame to port $eport, receive on port $iport."
  printf("Expect the frame to hit ent_num 5."
  # convert to 1-based port number for N2X
  set sport [expr $eport + 1]
  set dport [expr $iport + 1]
  set recvFrame [send_one_tag_frame_cap_recv $argv0 $portList $sport $dport  $smac $dmac $vlan]
  printf("vlan $vlan srcmac: $smac, destmac: $dmac"
  printf("Dump frame received on port $dport:\n$recvFrame\n"
  after 100 
  dump_log_details $logFile $hResult
  # check frame is received on destination port
  parse_log_file $logFile
  if {[frame_is_rcvd $sport $dport] == -1} {
     printf("Frame is not received on ingress port $iport, incorrect."
     set rc -1
  } else {
     printf("Frame is received on ingress port $iport correctly."
     # check retain hdr
     set_dev_ctrl_port_num $devp $iport
     if {[msec_port_get_igr_retain_hdr $devp] == 1 && [check_macsec_retain_hdr $recvFrame] == 0} {
        printf("Retain hdr is expected but not found on port $iport, incorrect."
        set rc -1
     }
  }  

  printf("send one untagged frame to port $eport, receive on port $iport."
  printf("Expect the frame to hit ent_num 5."
  set recvFrame [send_one_frame_cap_recv $argv0 $portList $sport $dport  $smac $dmac]
  printf("srcmac: $smac, destmac: $dmac"
  printf("Dump frame received on port $dport:\n$recvFrame\n"
  after 100 
  dump_log_details $logFile $hResult
  # check frame is received on destination port
  parse_log_file $logFile
  if {[frame_is_rcvd $sport $dport] == -1} {
     printf("Frame is not received on ingress port $iport, incorrect."
     set rc -1
  } else {
     printf("Frame is received on ingress port $iport correctly."
     # check retain hdr
     set_dev_ctrl_port_num $devp $iport
     if {[msec_port_get_igr_retain_hdr $devp] == 1 && [check_macsec_retain_hdr $recvFrame] == 0} {
       printf("Retain hdr is expected but not found on port $iport, incorrect."
       set rc -1
     }
  }  
}

//# Dump all MACSec statistic counters
dump_print_macsec_statistics $hResult $devp

foreach eport $eports iport $iports {
  set egr_hit_cnt [get_macsec_stats_by_name $eport MX_SEC_EGR_HIT_E5]
  puts -nonewline $hResult "eport $eport MX_SEC_EGR_HIT_E5 $egr_hit_cnt, "
  if {$egr_hit_cnt == 2 } {
	  printf("correct."
  } else {
	  printf("incorrect."
    set egr_miss_cnt [get_macsec_stats_by_name $eport MX_SEC_EGR_MISS]
    puts -nonewline $hResult "eport $eport MX_SEC_EGR_MISS $egr_miss_cnt, "
    set rc -1
  }

  set egr_prot_cnt [get_macsec_stats_by_name $eport MX_SEC_EGR_PKT_PROT_E5]
  puts -nonewline $hResult "eport $eport MX_SEC_EGR_PKT_PROT_E5 $egr_prot_cnt, "
  if {$egr_prot_cnt == 2 } {
    printf("correct."
  } else {
    printf("incorrect."
    set rc -1
  }
  set igr_hit_cnt [get_macsec_stats_by_name $iport MX_SEC_IGR_HIT_E5]
  puts -nonewline $hResult "iport $iport MX_SEC_IGR_HIT_E5 $igr_hit_cnt, "
  if {$igr_hit_cnt == 2 } {
	  printf("correct."
  } else {
	  printf("incorrect."
    set igr_miss_cnt [get_macsec_stats_by_name $iport MX_SEC_IGR_MISS]
    puts -nonewline $hResult "iport $iport MX_SEC_IGR_MISS $igr_miss_cnt, "
    set rc -1
  }

  set igr_ok_cnt [get_macsec_stats_by_name $iport MX_SEC_IGR_OK_E5]
  puts -nonewline $hResult "iport $iport MX_SEC_IGR_MISS $igr_ok_cnt, "
  if {$igr_ok_cnt == 2 } {
    printf("correct."
  } else {
    printf("incorrect."
    set rc -1
  }
}

printf("\n
//# to exercise ent_num 6"
set smac "fe:ed:d0:0d:ff:61"
set dmac "fe:ed:d0:0d:ff:60"
//# match ethertype 0806
foreach eport $eports iport $iports {
  printf("send one arp frame to port $eport, receive on port $iport."
  printf("Expect the frame to hit ent_num 6."
  # convert to 1-based port number for N2X
  set sport [expr $eport + 1]
  set dport [expr $iport + 1]
  set recvFrame [send_one_arp_frame_cap_recv $argv0 $portList $sport $dport  $smac $dmac]
  printf("srcmac: $smac, destmac: $dmac"
  printf("Dump frame received on port $dport:\n$recvFrame\n"
  after 100 
  dump_log_details $logFile $hResult
  # check frame is received on destination port
  parse_log_file $logFile
  if {[frame_is_rcvd $sport $dport] == -1} {
     printf("Frame is not received on ingress port $iport, incorrect."
     set rc -1
  } else {
     printf("Frame is received on ingress port $iport correctly."
     # check retain hdr
     set_dev_ctrl_port_num $devp $iport
     if {[msec_port_get_igr_retain_hdr $devp] == 1 && [check_macsec_retain_hdr $recvFrame] == 0} {
        printf("Retain hdr is expected but not found on port $iport, incorrect."
        set rc -1
     }  
  }
}

//# Dump all MACSec statistic counters
dump_print_macsec_statistics $hResult $devp

foreach eport $eports iport $iports {
  set egr_hit_cnt [get_macsec_stats_by_name $eport MX_SEC_EGR_HIT_E6]
  puts -nonewline $hResult "eport $eport MX_SEC_EGR_HIT_E6 $egr_hit_cnt, "
  if {$egr_hit_cnt == 1 } {
	  printf("correct."
  } else {
	  printf("incorrect."
    set egr_miss_cnt [get_macsec_stats_by_name $eport MX_SEC_EGR_MISS]
    puts -nonewline $hResult "eport $eport MX_SEC_EGR_MISS $egr_miss_cnt, "
    set rc -1
  }

  set egr_prot_cnt [get_macsec_stats_by_name $eport MX_SEC_EGR_PKT_PROT_E6]
  puts -nonewline $hResult "eport $eport MX_SEC_EGR_PKT_PROT_E6 $egr_prot_cnt, "
  if {$egr_prot_cnt == 1 } {
    printf("correct."
  } else {
    printf("incorrect."
    set rc -1
  }
  set igr_hit_cnt [get_macsec_stats_by_name $iport MX_SEC_IGR_HIT_E6]
  puts -nonewline $hResult "iport $iport MX_SEC_IGR_HIT_E6 $igr_hit_cnt, "
  if {$igr_hit_cnt == 1 } {
	  printf("correct."
  } else {
	  printf("incorrect."
    set igr_miss_cnt [get_macsec_stats_by_name $iport MX_SEC_IGR_MISS]
    puts -nonewline $hResult "iport $iport MX_SEC_IGR_MISS $igr_miss_cnt, "
    set rc -1
  }

  set igr_ok_cnt [get_macsec_stats_by_name $iport MX_SEC_IGR_OK_E6]
  puts -nonewline $hResult "iport $iport MX_SEC_IGR_OK_E6 $igr_ok_cnt, "
  if {$igr_ok_cnt == 1 } {
    printf("correct."
  } else {
    printf("incorrect."
    set rc -1
  }
}

printf("\n
//# to exercise ent_num 7"
set smac "fe:ed:d0:0d:ff:71"
set dmac "fe:ed:d0:0d:ff:b0";#match DA
foreach eport $eports iport $iports {
  printf("send one frame to port $eport, receive on port $iport."
  printf("Expect the frame to hit ent_num 7."
  # convert to 1-based port number for N2X
  set sport [expr $eport + 1]
  set dport [expr $iport + 1]
  set recvFrame [send_one_frame_cap_recv $argv0 $portList $sport $dport  $smac $dmac]
  printf("srcmac: $smac, destmac: $dmac"
  printf("Dump frame received on port $dport:\n$recvFrame\n"
  after 100 
  dump_log_details $logFile $hResult
  # check frame is received on destination port
  parse_log_file $logFile
  if {[frame_is_rcvd $sport $dport] == -1} {
     printf("Frame is not received on ingress port $iport, incorrect."
     set rc -1
  } else {
     printf("Frame is received on ingress port $iport correctly."
    # check retain hdr
    set_dev_ctrl_port_num $devp $iport
    if {[msec_port_get_igr_retain_hdr $devp] == 1 && [check_macsec_retain_hdr $recvFrame] == 0} {
       printf("Retain hdr is expected but not found on port $iport, incorrect."
       set rc -1
    }  
  }
}

//# Dump all MACSec statistic counters
dump_print_macsec_statistics $hResult $devp

foreach eport $eports iport $iports {
  set egr_hit_cnt [get_macsec_stats_by_name $eport MX_SEC_EGR_HIT_E7]
  puts -nonewline $hResult "eport $eport MX_SEC_EGR_HIT_E7 $egr_hit_cnt, "
  if {$egr_hit_cnt == 1 } {
	  printf("correct."
  } else {
    printf("incorrect."
    set egr_miss_cnt [get_macsec_stats_by_name $eport MX_SEC_EGR_MISS]
    puts -nonewline $hResult "eport $eport MX_SEC_EGR_MISS $egr_miss_cnt, "
    set rc -1
  }

  set egr_prot_cnt [get_macsec_stats_by_name $eport MX_SEC_EGR_PKT_PROT_E7]
  puts -nonewline $hResult "eport $eport MX_SEC_EGR_PKT_PROT_E7 $egr_prot_cnt, "
  if {$egr_prot_cnt == 1 } {
    printf("correct."
  } else {
    printf("incorrect."
    set rc -1
  }
  set igr_hit_cnt [get_macsec_stats_by_name $iport MX_SEC_IGR_HIT_E7]
  puts -nonewline $hResult "iport $iport MX_SEC_IGR_HIT_E7 $igr_hit_cnt, "
  if {$igr_hit_cnt == 1 } {
    printf("correct."
  } else {
    printf("incorrect."
    set igr_miss_cnt [get_macsec_stats_by_name $iport MX_SEC_IGR_MISS]
    puts -nonewline $hResult "iport $iport MX_SEC_IGR_MISS $igr_miss_cnt, "
    set rc -1
  }

  set igr_ok_cnt [get_macsec_stats_by_name $iport MX_SEC_IGR_OK_E7]
  puts -nonewline $hResult "iport $iport MX_SEC_IGR_OK_E7 $igr_ok_cnt, "
  if {$igr_ok_cnt == 1 } {
    printf("correct."
  } else {
    printf("incorrect."
    set rc -1
  }
}


#endif  //0


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