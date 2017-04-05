#include <madCopyright.h>
/**************************************************************************
# This file have procedures to configure MACsec either via calling mx_api from
# C library, or directly to access registers.
global usbport
global did_speed

set usbport 0

########################################################################################
# !!! Setting did_speed before starting the test
#
# please modify the did-speed value according to Marvell Virtual Bench Service
# launch VBService.exe and check the string start with "USB:" and has format 
# 0x????-<Hi/Full>-Speed[5-SMI]
# for example, 
# if it shows "USB:0x0000:Hi-Speed[5-SMI]", then set did_speed to "0x0000:Hi-Speed"
# if it shows "USB:0x0016:Full-Speed[5-SMI]", then set did_speed to "0x0016:Full-Speed"
########################################################################################
set did_speed "0x0000:Hi-Speed"



********************************************************************************/
#ifndef __SET_MACSEC_H
#define __SET_MACSEC_H

void Debug (char * msg);

void Error (char * msg);
void print_reg(MAD_DEV *devp, int port, unsigned int x);

void echo_reg (MAD_DEV *devp, int port, unsigned int rg, unsigned short data);

void wrchk_reg (MAD_DEV *devp, int port, unsigned int rg, unsigned short data);

MAD_DEV * initialize_macsec_api(MAD_DEV * devp);
void print_u8_arr(char * arr, int dim);
void print_u16_arr(short * arr, int dim);
void print_u32_arr(unsigned int * arr, int dim);
unsigned char * convert_u32_arr_to_list(unsigned int arr[], int dim);
int compare_list_octet(char *list1, char * list2);
char * gen_random_octs(int cnt);
char * gen_random_hex_octs(int cnt);
char * gen_random_sci(void);
long long gen_random_64_octs(void);

int validate_fips_compliance (MAD_DEV *devp, unsigned int *ports, FIPS_CFG_T * fips_cfg, 
							   int *out_arr, unsigned char * obsv_vec) ;
MATCH_EN * new_match_en_ent(int ent_num, int en_def, int en_mc, int en_da, int en_sa, int en_vlan, int en_et, int en_scian, int en_hdr);
MATCH_FLD *  new_match_fld_ent(int ent_num, unsigned long long da, unsigned long long sa, 
							   unsigned short vlan, unsigned short et, 
							   unsigned long long sci, unsigned char tci, 
							   unsigned char tci_msk, 
							   int conf_offset, int hdr_fld);

ACT_FLD *  new_act_fld_ent(int ctx_num_sec_level, int drop_maxpn, int drop,
						   int redir, int auth_en, int enc_en);
CTX_T *  new_ctx_ent(int ent_num, int keyStr, unsigned long long sci,  unsigned int pn, 
					 unsigned char tci, unsigned char offset);
void stat_arr_init (void);
void dump_macsec_statistics(MAD_DEV *devp);
void print_macsec_statistics(MAD_DEV *devp);
void dump_print_macsec_statistics(MAD_DEV *devp);
void dump_print_macsec_stats(MAD_DEV * devp);
void clr_acc_macsec_stats (MAD_DEV * devp);
void acc_macsec_stats (MAD_DEV * devp);
void dump_accumulate_macsec_statistics(MAD_DEV *devp) ;

void dump_mac_statistics(MAD_DEV *devp);
void print_mac_statistics(MAD_DEV * devp);
void dump_print_mac_statistics(MAD_DEV * devp);
void dump_print_mac_stats(MAD_DEV * devp);
int get_macsec_stats_by_name(MAD_DEV * devp, int port, MAC_NAME statName);
int check_mac_err (MAD_DEV * devp, int port);
int get_macsec_acc_stats_by_name (MAD_DEV *devp, int port, int statName) ;

int get_macsec_stats_by_range (MAD_DEV *devp, int port, int statName, int cnt);
int get_macsec_acc_stats_by_range(MAD_DEV *devp, int port, int statName, int cnt);
int compare_egr_igr_hit_counters (MAD_DEV * devp, int eport, int iport, int ent);
int compare_igr_hit_ok_counters(MAD_DEV * devp, int iport, int ent);
int compare_egr_igr_acc_hit_counters(MAD_DEV * devp, int eport, int iport,  int ent);
void set_sysmac_mibcnt_en(MAD_DEV * devp, int port, int flag);
void dump_sysmac_statistics(MAD_DEV * devp);
void print_sysmac_statistics(MAD_DEV * devp);
void dump_print_sysmac_statistics(MAD_DEV * devp);
void printportigr_key(MAD_DEV *devp,  int ent_num);
void printportigr_hkey(MAD_DEV *devp,  int ent_num);
void printportegr_key(MAD_DEV *devp,  int ent_num);
void printportegr_hkey(MAD_DEV *devp,  int ent_num);
void force_flow_ctrl_en(MAD_DEV * devp);
void force_flow_ctrl_dis(MAD_DEV * devp);
void force_wire_flow_ctrl_en(MAD_DEV * devp);
void set_sym_flow_ctrl(MAD_DEV * devp);
void check_mac_status(MAD_DEV * devp) ;
int check_mac_fifo(MAD_DEV * devp);
int check_lc_intr_port(MAD_DEV * devp);
int check_lc_intr(MAD_DEV * devp);
int get_intr_src(MAD_DEV * devp, int port);
int check_global_intr(MAD_DEV * devp, int port);
void bist_test(MAD_DEV * devp);
void toggle_wmac_autoneg(MAD_DEV * devp);
void set_mac_frame_size_limit(MAD_DEV * devp, int sizeLimit);
void enable_non_disruptive_loopback(MAD_DEV * devp);
void set_elu_redir_to_imb(MAD_DEV * devp, int ent_num, int flag);
void set_def_drop(MAD_DEV * devp, int igr, int egr);
void set_store_forward_mode(MAD_DEV * devp, int flag);
void clear_macsec_stats(MAD_DEV *devp, int portnum) ;
void clear_tables(MAD_DEV *devp, int portnum);
void shinit(MAD_DEV * devp, int speed, int duplex);
void clr_etm_cfg(MAD_DEV * devp, ETM_CFG_T * etm) ;
void set_etm_cfg(MAD_DEV * devp, ETM_CFG_T * etm, int etype, int eg_act, int eg_en, int ig_act, int ig_en);
void print_etm_cfg (MAD_DEV * devp, ETM_CFG_T * etm);
void print_elu_tbl(MAD_DEV * devp);
void print_egr_ctx_tbl(MAD_DEV * devp);
void print_ilu_tbl(MAD_DEV * devp);
void print_enc_key_tbl(MAD_DEV * devp);
void print_dec_key_tbl(MAD_DEV * devp);
void dump_macsec_regs(MAD_DEV * devp);
void set_macsec_parse_ptp(MAD_DEV * devp, int flag);

void cfg_egr_ent(MAD_DEV * devp, int ent_num, int key_in, long long sci, int pn, int tci);
void cfg_igr_ent(MAD_DEV * devp, int ent_num, int key_in, long long sci, int pn, int tci);
void parseportcfg(int title, int fid);
void macsec_start_all (MAD_DEV * devp);
void parse_entry_cfg (MAD_DEV *devp, int title, int fid);
int is_elu_en(MAD_DEV * devp, int port, int ent_num);

void recover_locked_port(MAD_DEV * devp, int port);
void set_macsec_clk_en(MAD_DEV * devp, int port, int flag);
void set_macsec_power_down(MAD_DEV * devp, int port, int flag) ;
unsigned int imb_read(MAD_DEV * devp, int port, short addr);
void imb_write(MAD_DEV * devp, int port, short addr, unsigned int data);
void set_elu_content(MAD_DEV * devp, int port, int ent_num, int idx, int data);
void set_ilu_content(MAD_DEV * devp, int port, int ent_num, int idx, int data);

#endif   /* __SET_MACSEC_H */

