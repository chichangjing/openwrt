// ----------------------------------------------------------------------
//  Copyright (c) 2008 Marvell Semiconductor
//  All Rights Reserved
// ----------------------------------------------------------------------

/// \file mx_api_helper.h
#ifndef __mx_api_helper_h
#define __mx_api_helper_h
#ifdef __cplusplus
extern "C" {
#endif
//#include <stdint.h>
#include "msecApi.h"

#ifndef uint8_t
#define uint8_t unsigned char 
#endif
#ifndef uint16_t
#define uint16_t unsigned short 
#endif
#ifndef uint32_t
#define uint32_t unsigned int 
#endif
#ifndef uint64_t
#define uint64_t unsigned long long 
#endif
#ifndef int8_t
#define int8_t char 
#endif
#ifndef int16_t
#define int16_t short 
#endif
#ifndef int32_t
#define int32_t int 
#endif
#ifndef int64_t
#define int64_t  long long 
#endif
/*! \brief create array with uint8_t type
 *
 * @param  size int (array size)
 * @return Returns pointer to the array
 */
uint8_t * create_u8_arr(int size);
/*! \brief set uint8_t array field
 *
 * @param *arr uint8_t* (pointer to the array)
 * @param  idx int (index to the array element)
 * @param  val uint8_t (value to set to array)
 * @return Returns none
 */
void set_u8_arr_fld(uint8_t *arr, int idx, uint8_t val);
/*! \brief get uint8_t array field
 *
 * @param *arr uint8_t* (pointer to the array)
 * @param  idx int (index to the array element)
 * @return Returns value of the array element for given index
 */
uint8_t get_u8_arr_fld(uint8_t *arr, int idx);
/*! \brief delete array with uint8_t type
 *
 * @param *arr uint8_t* (pointer to the array)
 * @return Returns none
 */
void delete_u8_arr (uint8_t *arr);
/*! \brief create array with uint16_t type
 *
 * @param  size int (array size)
 * @return Returns pointer to the array
 */
uint16_t * create_u16_arr(int size);
/*! \brief set uint16_t array field
 *
 * @param *arr uint16_t* (pointer to the array)
 * @param  idx int (index to the array element)
 * @return Returns value of the array element for given index
 */
void set_u16_arr_fld(uint16_t *arr, int idx, uint16_t val);
/*! \brief get array element with uint16_t type
 *
 * @param *arr uint16_t* (pointer to the array)
 * @param  idx int (index to the array element)
 * @return Returns none
 */
uint16_t get_u16_arr_fld(uint16_t *arr, int idx);
/*! \brief delete array with uint16_t type
 *
 * @param *arr uint16_t* (pointer to the array)
 * @return Returns none
 */
void delete_u16_arr (uint16_t *arr);
/*! \brief create array with uint32_t type
 *
 * @param  size int (array size)
 * @return Returns pointer to the array
 */
uint32_t * create_u32_arr(int size);
/*! \brief set uint32_t array field
 *
 * @param *arr uint32_t* (pointer to the array)
 * @param  idx int (index to the array element)
 * @return Returns value of the array element for given index
 */
void set_u32_arr_fld(uint32_t *arr, int idx, uint32_t val);
/*! \brief get array element with uint32_t type
 *
 * @param *arr uint32_t* (pointer to the array)
 * @param  idx int (index to the array element)
 * @return Returns none
 */
uint32_t get_u32_arr_fld(uint32_t *arr, int idx);
/*! \brief delete array with uint32_t type
 *
 * @param *arr uint32_t* (pointer to the array)
 * @return Returns none
 */
void delete_u32_arr (uint32_t *arr);
/*! \brief set device control device number
 *
 * @param *ctrl MAD_DEV* (pointer to the MAD_DEV)
 * @param  dev_num (0-based PHY device number 0, 1, 2, 3, etc.)
 * @return Returns none
 */
void set_dev_ctrl_dev_num (MAD_DEV *ctrl, int msec_dev_num);
/*! \brief get device control device number
 *
 * @param *ctrl MAD_DEV* (pointer to the MAD_DEV)
 * @return Returns PHY device number
 */
int get_dev_ctrl_dev_num (MAD_DEV *ctrl);
/*! \brief set device control SMI base address
 *
 * @param *ctrl MAD_DEV* (pointer to the MAD_DEV)
 * @param  smiaddr (SMI base address 0, 4, 8, 12, etc.)
 * @return Returns none
 */
void set_dev_ctrl_smiaddr (MAD_DEV *ctrl, int smiaddr);
/*! \brief get device control SMI base address
 *
 * @param *ctrl MAD_DEV* (pointer to the MAD_DEV)
 * @return Returns SMI base address
 */
int get_dev_ctrl_smiaddr (MAD_DEV* ctrl);
/*! \brief set device control port number
 *
 * @param *ctrl MAD_DEV* (pointer to the MAD_DEV)
 * @param  port_num (port number 0 ~ 3 within phy device)
 * @return Returns none
 */
void set_dev_ctrl_port_num (MAD_DEV *ctrl, int port_num);
/*! \brief get device control port number
 *
 * @param *ctrl MAD_DEV* (pointer to the MAD_DEV)
 * @return Returns port number
 */
int get_dev_ctrl_port_num (MAD_DEV *ctrl);
/*! \brief set usb port in MAD_DEV 
 *
 * @param *ctrl MAD_DEV* (pointer to the MAD_DEV)
 * @param  usbport(usb port number for SMI interface)
 * @return Returns none
 */
void set_dev_ctrl_usbport (MAD_DEV *ctrl, int usbport);
/*! \brief get usb port in MAD_DEV 
 *
 * @param *ctrl MAD_DEV* (pointer to the MAD_DEV)
 * @return Returns usb port number from MAD_DEV
 */
int get_dev_ctrl_usbport (MAD_DEV *ctrl);
/*! \brief create new pointer with type MAC_CTRL* 
 *
 * @param  mac_type	int (MAC type)
 * @param  port_en  int (port enable)
 * @param  speed	int (speed)
 * @param  hfd		int (half or full duplex)
 * @param  mibcnt_en	int (MIB counter enable)
 * @param  lpbk_en	int (loopback enable)
 * @param  max_frame_sz	int (MAX frame size)
 * @return Returns a pointer to MAC_CTRL
 */
MAC_CTRL * new_mac_ctrl (int mac_type,  int port_en,  int speed,  int hfd,  int mibcnt_en,  int lpbk_en,  int max_frame_sz);
/*! \brief delete MAC_CTRL object 
 *
 * @param *mac_ctrl	MAC_CTRL (pointer to MAC_CTRL)
 * @return none
 */
void delete_mac_ctrl (MAC_CTRL *mac_ctrl);
/*! \brief create new pointer with type MAC_CTRL* 
 *
 * @param  en_def	int (enable default)
 * @param  en_mc	int (enable multicast)
 * @param  en_da	int (enable destination address)
 * @param  en_sa	int (enable source address)
 * @param  en_vlan	int (enable vlan)
 * @param  en_et	int (enable ethertype)
 * @param  en_scian	int (enable SCI-AN)
 * @return Return pointer to MATCH_EN
 */
MATCH_EN * new_match_en (int en_def, int en_mc, int en_da, int en_sa, int en_vlan, int en_et, 
						 int en_scian, int en_hdr);
/*! \brief delete object with type MAC_CTRL* 
 *
 * @param *match_en	MATCH_EN (pointer to MATCH_EN)
 * @return none
 */
void delete_match_en(MATCH_EN *match_en);
/*! \brief create new object with type MATCH_FLD* 
 *
 * @param  da	uint64_t (destionation address)
 * @param  sa	uint64_t (source address)
 * @param  vlan	uint16_t (vlan)
 * @param  et	uint16_t (ethertype)
 * @param  sci	uint64_t (Security Connection Identifier)
 * @param  tci	uint8_t  (Tag Control Information Field)
 * @param  tci_msk	uint8_t (TCI mask)
 * @return Return pointer to MATCH_FLD
 */
MATCH_FLD * new_match_fld(uint64_t da, uint64_t sa, uint16_t vlan, uint16_t et, uint64_t sci, 
						  uint8_t tci, uint8_t tci_msk, uint8_t offset, unsigned int hdr_fld);
/*! \brief delete object with type MATCH_FLD* 
 *
 * @param *match_fld	MATCH_FLD (pointer to MATCH_FLD)
 * @return none
 */
void delete_match_fld(MATCH_FLD *match_fld);
/*! \brief create new object with type ACT_FLD* 
 *
 * @param ctx_num	int (context number)
 * @param drop		int (drop)
 * @param redir		int (redirect)
 * @param auth_en	int (authenticate enable)
 * @param enc_en	int (encryption enable)
 * @return Return pointer to ACT_FLD
 */
ACT_FLD * new_act_fld(int ctx_num_sec_level, int drop_maxpn, int drop, int redir, int auth_en, int enc_en);
/*! \brief delete object with type ACT_FLD* 
 *
 * @param *act_fld	ACT_FLD (pointer to ACT_FLD)
 * @return none
 */
void delete_act_fld(ACT_FLD * act_fld);
/*! \brief create new object with type LKUP_T* 
 *
 * @param *lk_match_en	MATCH_EN* (pointer to struct MATCH_EN)
 * @param *lk_match_fld	MATCH_FLD* (pointer to struct MATCH_FLD)
 * @param *act_fld	ACT_FLD (pointer to ACT_FLD)
 * @return Return pointer to LKUP_T
 */
LKUP_T * new_lkup_t(MATCH_EN * lk_match_en, MATCH_FLD *lk_match_fld, ACT_FLD *act_fld);
/*! \brief get lk_match_en from LKUP_T* 
 *
 * @param *lkup_t	LKUP_T* (pointer to struct LKUP_T)
 * @return Return pointer to MATCH_EN
 */
MATCH_EN * get_match_en_from_lkup_t (LKUP_T *lkup_t);
/*! \brief get lk_match_fld from LKUP_T* 
 *
 * @param *lkup_t	LKUP_T* (pointer to struct LKUP_T)
 * @return Return pointer to MATCH_FLD
 */
MATCH_FLD *get_match_fld_from_lkup_t(LKUP_T *lkup_t);
/*! \brief get lk_act_fld from LKUP_T* 
 *
 * @param *lkup_t	LKUP_T* (pointer to struct LKUP_T)
 * @return Return pointer to ACT_FLD
 */
ACT_FLD *get_act_fld_from_lkup_t (LKUP_T *lkup_t);
/*! \brief delete object with type LKUP_T* 
 *
 * @param *lkup_t	LKUP_T (pointer to LKUP_T)
 * @return none
 */
void  delete_lkup_t (LKUP_T * lkup_t);
/*! \brief create new object with type CTX_T* 
 *
 * @param  sci	uint64_t (Security Connection Identifier)
 * @param  pn	uint32_t (Packet Number)
 * @param  tci	uint8_t (Tag Control Information field)
 * @param  key	uint8_t[16] (key array)
 * @return Return pointer to CTX_T
 */
CTX_T * new_ctx_t (uint64_t sci,  uint32_t pn, uint8_t tci, uint8_t offset, uint8_t key[16]);
/*! \brief delete object with type CTX_T* 
 *
 * @param *ctx	CTX_T (pointer to CTX_T)
 * @return none
 */
void delete_ctx_t (CTX_T *ctx);

/*! \brief create new object with type REDIR_HDR_T* 
 *
 * @param  sci	uint64_t (Security Connection Identifier)
 * @param  pn	uint32_t (Packet Number)
 * @param  tci	uint8_t (Tag Control Information field)
 * @param  key	uint8_t[16] (key array)
 * @return Return pointer to REDIR_HDR_T
 */
REDIR_HDR_T * new_redir_hdr_t (uint64_t da, uint16_t et, uint64_t sa0,
							   uint64_t sa1, uint64_t sa2, uint64_t sa3);
/*! \brief delete object with type REDIR_HDR_T* 
 *
 * @param *redir	REDIR_HDR_T (pointer to REDIR_HDR_T)
 * @return none
 */
void delete_redir_hdr_t(REDIR_HDR_T *redir);

CMN_CFG_T *new_cmn_cfg_t(uint8_t conf_offset, uint16_t def_vlan, uint64_t mc_high, uint16_t mc_low_msk, uint8_t trail_sz);
/*! \brief delete object with type CMN_CFG_T* 
 *
 * @param *cfg	CMN_CFG_T (pointer to CMN_CFG_T)
 * @return none
 */
void delete_cmn_cfg_t (CMN_CFG_T *cfg);
/*! \brief set conf_offset field in structure CMN_CFG_T* 
 *
 * @param *cfg	CMN_CFG_T (pointer to CMN_CFG_T)
 * @param  conf_offset	int (value of conf_offset)
 * @return none
 */
void set_cmn_cfg_offset (CMN_CFG_T *cfg, uint16_t conf_offset);
/*! \brief create new pointer with type EGR_CFG_T* 
 *
 * @param  def_drop	int (default drop)
 * @param  parse_ae	int (parse AE)
 * @param  parse_vlan	int (parse vlan)
 * @return Return pointer to EGR_CFG_T
 */
EGR_CFG_T *new_egr_cfg_t (int def_drop, int parse_ae, int parse_vlan, int hdr_as_pn);
/*! \brief delete object with type EGR_CFG_T* 
 *
 * @param *egr_cfg	EGR_CFG_T (pointer to EGR_CFG_T)
 * @return none
 */
void delete_egr_cfg_t(EGR_CFG_T *egr_cfg);
int msec_port_get_egr_hdr_as_pn (MAD_DEV *dev_ctrl, int port);
int msec_port_set_egr_hdr_as_pn (MAD_DEV *dev_ctrl, int port, int flag);
int msec_port_get_egr_parse_macsec (MAD_DEV *dev_ctrl, int port);
int msec_port_set_egr_parse_macsec (MAD_DEV *dev_ctrl, int port, int flag);
/*! \brief create new object with type IGR_DEF_T* 
 *
 * @param  def_drop		int (default drop)
 * @param  def_redir	int (default redirect)
 * @param  drop_badtag	int (drop badtag)
 * @param  retain_hdr	int (retain header)
 * @param  parse_vlan	int (parse_vlan)
 * @return Return pointer to IGR_DEF_T
 */
IGR_DEF_T *new_igr_def_t (int def_drop, int def_redir, int drop_badtag, int retain_hdr, 
						  int parse_vlan, int ins_hdr_pn);
/*! \brief delete object with type IGR_DEF_T* 
 *
 * @param *def_act	IGR_DEF_T (pointer to IGR_DEF_T)
 * @return none
 */
void delete_igr_def_t(IGR_DEF_T *def_act);
/*! \brief create new pointer with type REDIR_T* 
 *
 * @param  redir_untagged	int (redirect untagged)
 * @param  redir_badtag		int (redirect badtag)
 * @param  redir_unk_sci	int (redirect unknown SCI)
 * @param  redir_unused_sa	int (redirect unused SA)
 * @param  redir_delayed	int (redirect delayed)
 * @param  redir_invalid	int (redirect invalid)
 * @return Return pointer to REDIR_T
 */
REDIR_T *new_igr_redir_t (int redir_untagged, int redir_badtag, int redir_unk_sci, int redir_unused_sa, int redir_unchecked, 
	                           int redir_delayed, int redir_invalid);
/*! \brief delete object with type REDIR_T* 
 *
 * @param *redir_vec	REDIR_T (pointer to REDIR_T)
 * @return none
 */
void delete_igr_redir_t (REDIR_T *redir_vec);
/*! \brief create new pointer with type SCI_T* 
 *
 * @param  implicit_sci	uint64_t (implicit SCI)
 * @param  scb_port		uint16_t (scb port)
 * @param  def_port		uint16_t (default port)
 * @return Return pointer to SCI_T
 */
SCI_T * new_igr_sci_t (uint64_t implicit_sci, uint16_t scb_port, uint16_t def_port);
/*! \brief delete object with type SCI_T* 
 *
 * @param *sci_t	SCI_T (pointer to SCI_T)
 * @return none
 */
void delete_igr_sci_t (SCI_T *sci_t);
/*! \brief create new pointer with type RPY_T* 
 *
 * @param  validate_frames	int (3 - strict, 2 - check, 0 - disable)
 * @param  replay_protect	int (replay protect flag)
 * @param  replay_drop		int (replay drop flag)
 * @param  replay_window	int (replay window)
 * @return Return pointer to RPY_T
 */
RPY_T * new_igr_rpy_t (int validate_frames, int replay_protect, int replay_drop, int replay_window);
/*! \brief delete object with type RPY_T* 
 *
 * @param *rpy_t	RPY_T (pointer to RPY_T)
 * @return none
 */
void delete_igr_rpy_t (RPY_T *rpy_t);
/*! \brief create new pointer with type IGR_CFG_T* 
 *
 * @param  def_act		IGR_DEF_T* (pointer to IGR_DEF_T)
 * @param  redir_vec	REDIR_T* (pointer to REDIR_T)
 * @param  sci_info		SCI_T* (pointer to SCI_T)
 * @param  rpy_var		RPY* (pointer to RPY_T)
 * @return pointer to IGR_CFG_T
 */
IGR_CFG_T * new_igr_cfg_t (IGR_DEF_T *def_act, REDIR_T *redir_vec, SCI_T *sci_info, RPY_T *rpy_var);
/*! \brief create new pointer with type IGR_CFG_T* 
 *
 * @param *igr	IGR_CFG_T (pointer to IGR_CFG_T)
 * @return none
 */
void delete_igr_cfg_t (IGR_CFG_T *igr);
/*! \brief create new pointer with type ETM_CFG_T* 
 *
 * @param  etm[8]	uint16_t (array of uint16_t)
 * @param  etm_eg_en	uint8_t (ETM egress enable)
 * @param  etm_ig_en	uint8_t (ETM ingress enable)
 * @param  etm_eg_act	uint8_t (ETM egress action)
 * @param  etm_ig_act	uint8_t (ETM ingress action)
 * @return Return pointer to ETM_CFG_T
 */
ETM_CFG_T * new_etm_cfg_t (uint16_t etm[8], uint8_t etm_eg_en, uint8_t etm_ig_en, uint8_t etm_eg_act, uint8_t etm_ig_act);
/*! \brief set etm field in structure ETM_CFG_T* 
 *
 * @param *cfg	ETM_CFG_T* (pointer to ETM_CFG_T)
 * @param  ethertype	uint16_t (etm field value corresponding to array index)
 * @param  index		int (etm array index)
 * @return Return none
 */
void set_etm_cfg_ethertype (ETM_CFG_T *cfg, uint16_t ethertype, int index);
/*! \brief set etm_eg_act field in structure ETM_CFG_T* 
 *
 * @param *cfg	ETM_CFG_T* (pointer to ETM_CFG_T)
 * @param  action	int (etm_eg_action value)
 * @return Return none
 */
void set_etm_cfg_egr_action (ETM_CFG_T *cfg, int action);
/*! \brief set etm_ig_act field in structure ETM_CFG_T* 
 *
 * @param *cfg	ETM_CFG_T* (pointer to ETM_CFG_T)
 * @param  action	int (etm_ig_action field value)
 * @return Return none
 */
void set_etm_cfg_igr_action (ETM_CFG_T *cfg, int action);
/*! \brief set etm_eg_en field in structure ETM_CFG_T* 
 *
 * @param *cfg	ETM_CFG_T* (pointer to ETM_CFG_T)
 * @param  bitmsk	uint8_t (bitmask to enable corresponding etm field)
 * @return Return none
 */
void set_etm_cfg_egr_enable (ETM_CFG_T *cfg, uint8_t bitmsk);
/*! \brief set etm_ig_en field in structure ETM_CFG_T* 
 *
 * @param *cfg		ETM_CFG_T* (pointer to ETM_CFG_T)
 * @param  bitmsk	uint8_t (bitmask to enable corresponding etm field)
 * @return Return none
 */
void set_etm_cfg_igr_enable (ETM_CFG_T *cfg, uint8_t bitmsk);
/*! \brief get etm field in structure ETM_CFG_T* 
 *
 * @param *cfg			ETM_CFG_T* (pointer to ETM_CFG_T)
 * @param  index		int (etm array index)
 * @return Return etm field value corresponding to array index
 */
uint16_t get_etm_cfg_ethertype (ETM_CFG_T *cfg, int index);
/*! \brief get etm_eg_act field in structure ETM_CFG_T* 
 *
 * @param *cfg			ETM_CFG_T* (pointer to ETM_CFG_T)
 * @return Return etm_eg_act value
 */
int get_etm_cfg_egr_action (ETM_CFG_T *cfg);
/*! \brief get etm_ig_act field in structure ETM_CFG_T* 
 *
 * @param *cfg			ETM_CFG_T* (pointer to ETM_CFG_T)
 * @return Return etm_ig_act value
 */
int get_etm_cfg_igr_action (ETM_CFG_T *cfg);
/*! \brief get etm_eg_en field in structure ETM_CFG_T* 
 *
 * @param *cfg			ETM_CFG_T* (pointer to ETM_CFG_T)
 * @return Return etm_eg_en bitmask
 */
uint8_t get_etm_cfg_egr_enable (ETM_CFG_T *cfg);
/*! \brief get etm_eg_act field in structure ETM_CFG_T* 
 *
 * @param *cfg			ETM_CFG_T* (pointer to ETM_CFG_T)
 * @return Return etm_ig_en bitmask
 */
uint8_t get_etm_cfg_igr_enable (ETM_CFG_T *cfg);
/*! \brief delete object with type ETM_CFG_T* 
 *
 * @param *etm_cfg_t	ETM_CFG_T (pointer to ETM_CFG_T)
 * @return none
 */
void delete_etm_cfg_t (ETM_CFG_T * etm_cfg_t);
/*! \brief create new pointer with type FIPS_CFG_T* 
 *
 * @param  fips_vec[16]	uint8_t (array of uint8_t)
 * @param  key[16]		uint8_t (array of uint8_t)
 * @param  egr_igr		uint8_t (egress or ingress)
 * @param  eng_type		uint8_t (engine type)
 * @return Return pointer to FIPS_CFG_T
 */
FIPS_CFG_T * new_fips_cfg_t (uint8_t fips_vec[16], uint8_t key[16], uint8_t egr_igr, uint8_t eng_type);
/*! \brief delete object with type FIPS_CFG_T* 
 *
 * @param *fips	FIPS_CFG_T (pointer to FIPS_CFG_T)
 * @return none
 */
void delete_fips_cfg_t (FIPS_CFG_T *fips);
/*! \brief set fields in structure FIPS_CFG_T* 
 *
 * @param  fips	FIPS_CFG_T (pointer to FIPS_CFG_T)
 * @param  fips_vec[16]	uint8_t (fips vector)
 * @param  key[16]		uint8_t (key array)
 * @param  egr_igr		uint8_t (egr or igr)
 * @param  eng_type		uint8_t (engine type)
 * @return none
 */
void set_fips_cfg_t (FIPS_CFG_T *fips, uint8_t fips_vec[16], uint8_t key[16], uint8_t egr_igr, uint8_t eng_type);

/* set individual fields of structure ELU/ILU MATCH_EN, MATCH_FLD, ACT_FLD */
/*! \brief set en_def field in structure MATCH_EN for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in ELU table)
 * @param  en_def	int (enable default flag)
 * @return none
 */
void msec_port_set_elu_match_en_en_def (MAD_DEV *dev_ctrl, int port, int ent_num, int en_def);
/*! \brief set en_mc field in structure MATCH_EN for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in ELU table)
 * @param  en_mc	int (enable multicast flag)
 * @return none
 */
void msec_port_set_elu_match_en_en_mc (MAD_DEV *dev_ctrl, int port, int ent_num, int en_mc);
/*! \brief set en_da field in structure MATCH_EN for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in ELU table)
 * @param  en_da	int (enable DA flag)
 * @return none
 */
void msec_port_set_elu_match_en_en_da (MAD_DEV *dev_ctrl, int port, int ent_num, int en_da);
/*! \brief set en_sa field in structure MATCH_EN for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in ELU table)
 * @param  en_sa	int (enable SA flag)
 * @return none
 */
void msec_port_set_elu_match_en_en_sa (MAD_DEV *dev_ctrl, int port, int ent_num, int en_sa);
/*! \brief set en_vlan field in structure MATCH_EN for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in ELU table)
 * @param  en_vlan	int (enable VLAN flag)
 * @return none
 */
void msec_port_set_elu_match_en_en_vlan (MAD_DEV *dev_ctrl, int port, int ent_num, int en_vlan);
/*! \brief set en_et field in structure MATCH_EN for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in ELU table)
 * @param  en_et	int (enable ethertype flag)
 * @return none
 */
void msec_port_set_elu_match_en_en_et (MAD_DEV *dev_ctrl, int port, int ent_num, int en_et);
/*! \brief set en_def field in structure MATCH_EN for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in ILU table)
 * @param  en_et	int (enable default flag)
 * @return none
 */
void msec_port_set_ilu_match_en_en_def (MAD_DEV *dev_ctrl, int port, int ent_num, int en_def);
/*! \brief set en_mc field in structure MATCH_EN for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in ILU table)
 * @param  en_mc	int (enable multicast flag)
 * @return none
 */
void msec_port_set_ilu_match_en_en_mc (MAD_DEV *dev_ctrl, int port, int ent_num, int en_mc);
/*! \brief set en_da field in structure MATCH_EN for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in ILU table)
 * @param  en_da	int (enable DA flag)
 * @return none
 */
void msec_port_set_ilu_match_en_en_da (MAD_DEV *dev_ctrl, int port, int ent_num, int en_da);
/*! \brief set en_sa field in structure MATCH_EN for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in ILU table)
 * @param  en_sa	int (enable SA flag)
 * @return none
 */
void msec_port_set_ilu_match_en_en_sa (MAD_DEV *dev_ctrl, int port, int ent_num, int en_sa);
/*! \brief set en_vlan field in structure MATCH_EN for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in ILU table)
 * @param  en_vlan	int (enable VLAN flag)
 * @return none
 */
void msec_port_set_ilu_match_en_en_vlan (MAD_DEV *dev_ctrl, int port, int ent_num, int en_vlan);
/*! \brief set en_et field in structure MATCH_EN for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in ILU table)
 * @param  en_et	int (enable ethertype flag)
 * @return none
 */
void msec_port_set_ilu_match_en_en_et (MAD_DEV *dev_ctrl, int port, int ent_num, int en_et);
void msec_port_set_elu_match_en_hdr(MAD_DEV *dev_ctrl, int port, int ent_num, int en_hdr);
/*! \brief set da field in structure MATCH_FLD for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in ELU table)
 * @param  en_da	int (DA field value)
 * @return none
 */
void msec_port_set_elu_match_fld_da (MAD_DEV *dev_ctrl, int port, int ent_num, uint64_t da);

/*! \brief get da field in structure MATCH_FLD for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in ELU table)
 * @return Return DA field value
 */
uint64_t msec_port_get_elu_match_fld_da (MAD_DEV *dev_ctrl, int port, int ent_num);
/*! \brief set sa field in structure MATCH_FLD for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in ELU table)
 * @param  sa		uint64_t (source address)
 * @return none
 */
void msec_port_set_elu_match_fld_sa (MAD_DEV *dev_ctrl, int port, int ent_num, uint64_t sa);
/*! \brief set vlan field in structure MATCH_FLD for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in ELU table)
 * @param  vlan		uint16_t (vlan value)
 * @return none
 */
void msec_port_set_elu_match_fld_vlan (MAD_DEV *dev_ctrl, int port, int ent_num, uint16_t vlan);
/*! \brief set sci field in structure MATCH_FLD for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in ILU table)
 * @param  sci		uint64_t (sci value)
 * @return none
 */
void msec_port_set_ilu_match_fld_sci (MAD_DEV *dev_ctrl, int port, int ent_num, uint64_t sci);
/*! \brief set tci field in structure MATCH_FLD for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in ILU table)
 * @param  tci		uint8_t (tci value)
 * @return none
 */
void msec_port_set_ilu_match_fld_tci (MAD_DEV *dev_ctrl, int port, int ent_num, uint8_t tci);
/*! \brief set tci_msk field in structure MATCH_FLD for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in ILU table)
 * @param  tci_msk	uint8_t (tci mask)
 * @return none
 */
void msec_port_set_ilu_match_fld_tci_msk (MAD_DEV *dev_ctrl, int port, int ent_num, uint8_t tci_msk);
/*! \brief set offset_conf field in structure MATCH_FLD for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in ILU table)
 * @param  offset_conf	uint8_t (confidential offset)
 * @return none
 */
void msec_port_set_ilu_match_fld_conf_offset (MAD_DEV *dev_ctrl, int port, int ent_num, unsigned char offset);
void msec_port_set_ilu_match_fld_hdr_fld (MAD_DEV *dev_ctrl, int port, int ent_num, unsigned int hdr_fld);

/*! \brief set drop field in structure ACT_FLD for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in ELU table)
 * @param  drop		int (drop action)
 * @return none
 */

void msec_port_set_elu_act_fld_drop (MAD_DEV *dev_ctrl, int port, int ent_num, int drop);
/*! \brief set redir field in structure ACT_FLD for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in ELU table)
 * @param  redir	int (redirect action)
 * @return none
 */
void msec_port_set_elu_act_fld_redir (MAD_DEV *dev_ctrl, int port, int ent_num, int redir);
/*! \brief set auth_en field in structure ACT_FLD for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in ELU table)
 * @param  auth_en	int (authenticate enable action)
 * @return none
 */
void msec_port_set_elu_act_fld_auth_en (MAD_DEV *dev_ctrl, int port, int ent_num, int auth_en);
/*! \brief set enc_en field in structure ACT_FLD for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in ELU table)
 * @param  enc_en	int (encryption enable action)
 * @return none
 */
void msec_port_set_elu_act_fld_enc_en (MAD_DEV *dev_ctrl, int port, int ent_num, int enc_en);
/*! \brief set drop field in structure ACT_FLD for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in ILU table)
 * @param  drop		int (drop action)
 * @return none
 */
void msec_port_set_ilu_act_fld_drop (MAD_DEV *dev_ctrl, int port, int ent_num, int drop);
/*! \brief set redir field in structure ACT_FLD for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in ELU table)
 * @param  redir	int (redirect action)
 * @return none
 */
void msec_port_set_ilu_act_fld_redir (MAD_DEV *dev_ctrl, int port, int ent_num, int redir);
/*! \brief set redir field in structure ACT_FLD for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in ELU table)
 * @param  sec_level	int (security level)
 * @return none
 */
void msec_port_set_ilu_act_fld_sec_level (MAD_DEV *dev_ctrl, int port, int ent_num, int sec_level);
/* Acess individual fields of structure IGR_DEF_T */

/*! \brief get Ingress def_drop for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @return Return def_drop
 */
int msec_port_get_igr_def_drop (MAD_DEV *dev_ctrl, int port);
/*! \brief get Ingress def_redir for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @return Return def_redir
 */
int msec_port_get_igr_def_redir (MAD_DEV *dev_ctrl, int port);
/*! \brief get Ingress drop_badtag for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @return Return drop_badtag
 */
int msec_port_get_igr_drop_badtag (MAD_DEV *dev_ctrl, int port);
/*! \brief get Ingress retain_hdr for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @return Return retain_hdr 
 */
int msec_port_get_igr_retain_hdr (MAD_DEV *dev_ctrl, int port);
int msec_port_get_igr_ins_hdr_pn (MAD_DEV *dev_ctrl, int port);

/*! \brief set Ingress def_drop for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  def_drop	int (def_drop value)
 * @return none
 */
void msec_port_set_igr_def_drop (MAD_DEV *dev_ctrl, int port, int def_drop);
/*! \brief set Ingress def_redir for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  def_redir	int (def_redir value)
 * @return none
 */
void msec_port_set_igr_def_redir(MAD_DEV *dev_ctrl, int port, int def_redir);
/*! \brief set Ingress drop_badtag for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  drop_badtag	int (drop badtag)
 * @return none
 */
void msec_port_set_igr_drop_badtag(MAD_DEV *dev_ctrl, int port, int drop_badtag);
/*! \brief set Ingress retain_hdr for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  retain_hdr	int (retain header)
 * @return none
 */
void msec_port_set_igr_retain_hdr(MAD_DEV *dev_ctrl, int port, int retain_hdr);
void msec_port_set_igr_ins_hdr_pn (MAD_DEV *dev_ctrl, int port, int ins_hdr_pn);
/*! \brief get Ingress key for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in ILU table)
 * @param  key[16]	uint8_t	(key array)
 * @return none
 */
void msec_port_get_igr_key (MAD_DEV *dev_ctrl, int port, int ent_num, uint8_t key[16]);
/*! \brief get Ingress hkey for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in ILU table)
 * @param  hkey[16]	uint8_t	(hash key array)
 * @return none
 */
void msec_port_get_igr_hkey (MAD_DEV *dev_ctrl, int port, int ent_num, uint8_t hkey[16]);
/*! \brief get egress key for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in egress context table)
 * @param  key[16]	uint8_t	(key array)
 * @return none
 */
void msec_port_get_egr_key (MAD_DEV *dev_ctrl, int port, int ent_num, uint8_t key[16]);
/*! \brief get egress hkey for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num	int (entry number in egress context table)
 * @param  hkey[16]	uint8_t	(hash key array)
 * @return none
 */
void msec_port_get_egr_hkey (MAD_DEV *dev_ctrl, int port, int ent_num, uint8_t hkey[16]);

/* Access individual fields of structure igr REDIR_T */

/*! \brief get Ingress redir_untagged from structure REDIR_T for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @return redir_untagged
 */
int msec_port_get_igr_redir_untagged (MAD_DEV *dev_ctrl, int port);
/*! \brief get Ingress redir_badtag from structure REDIR_T for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @return redir_badtag
 */
int msec_port_get_igr_redir_badtag (MAD_DEV *dev_ctrl, int port);
/*! \brief get Ingress redir_unknown_sci from structure REDIR_T for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @return redir_unknown_sci
 */
int msec_port_get_igr_redir_unkown_sci(MAD_DEV *dev_ctrl, int port);
/*! \brief get Ingress redir_unused_sa from structure REDIR_T for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @return redir_unused_sa
 */
int msec_port_get_igr_redir_unused_sa (MAD_DEV *dev_ctrl, int port);
/*! \brief get Ingress redir_unchecked from structure REDIR_T for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @return redir_unchecked
 */
int msec_port_get_igr_redir_unchecked (MAD_DEV *dev_ctrl, int port);
/*! \brief get Ingress redir_delayed from structure REDIR_T for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @return redir_delayed
 */
int msec_port_get_igr_redir_delayed (MAD_DEV *dev_ctrl, int port);
/*! \brief get Ingress redir_invalid from structure REDIR_T for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @return redir_invalid
 */
int msec_port_get_igr_redir_invalid (MAD_DEV *dev_ctrl, int port);
/*! \brief set Ingress redir_untagged from structure REDIR_T for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  redir_untagged	int
 * @return none
 */
void msec_port_set_igr_redir_untagged (MAD_DEV *dev_ctrl, int port, int redir_untagged);
/*! \brief set Ingress redir_badtag from structure REDIR_T for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param redir_badtag
 * @return none
 */
void msec_port_set_igr_redir_badtag (MAD_DEV *dev_ctrl, int port, int redir_badtag);
/*! \brief set Ingress redir_unk_sci from structure REDIR_T for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  redir_unk_sci	int (unknown sci)
 * @return none
 */
void msec_port_set_igr_redir_unkown_sci(MAD_DEV *dev_ctrl, int port, int redir_unk_sci);
/*! \brief set Ingress redir_unused_sa from structure REDIR_T for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  redir_unused_sa	int (unused sa)
 * @return none
 */
void msec_port_set_igr_redir_unused_sa (MAD_DEV *dev_ctrl, int port, int redir_unused_sa);
/*! \brief set Ingress redir_unchecked from structure REDIR_T for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  redir_unchecked	int
 * @return none
 */
void msec_port_set_igr_redir_unchecked (MAD_DEV *dev_ctrl, int port, int redir_unchecked);
/*! \brief set Ingress redir_delayed from structure REDIR_T for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  redir_delayed	int
 * @return none
 */
void msec_port_set_igr_redir_delayed (MAD_DEV *dev_ctrl, int port, int redir_delayed);
/*! \brief set Ingress redir_invalid from structure REDIR_T for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  redir_invalid	int
 * @return none
 */
void msec_port_set_igr_redir_invalid (MAD_DEV *dev_ctrl, int port, int redir_invalid);

/* Access indivdual fields of structure igr RPY_T */
/*! \brief get Ingress validate_frames in structure RPY_T for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @return Return validate_frames
 */
int msec_port_get_igr_rpy_t_validate_frames (MAD_DEV *dev_ctrl, int port);
/*! \brief get Ingress replay_protect in structure RPY_T for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @return Return replay_protect 
 */
int msec_port_get_igr_rpy_t_replay_protect (MAD_DEV *dev_ctrl, int port);
/*! \brief get Ingress replay_drop in structure RPY_T for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @return Return replay_drop 
 */
int msec_port_get_igr_rpy_t_replay_drop (MAD_DEV *dev_ctrl, int port);
/*! \brief get Ingress replay_window in structure RPY_T for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @return Return replay_window 
 */
int msec_port_get_igr_rpy_t_replay_window (MAD_DEV *dev_ctrl, int port);
/*! \brief set Ingress validate_frames in structure RPY_T for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  validate_frames	int
 * @return none
 */
void msec_port_set_igr_rpy_t_validate_frames (MAD_DEV *dev_ctrl, int port, int validate_frames);
/*! \brief set Ingress replay_protect in structure RPY_T for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  replay_protect int
 * @return none
 */
void msec_port_set_igr_rpy_t_replay_protect (MAD_DEV *dev_ctrl, int port, int replay_protect);
/*! \brief set Ingress replay_drop in structure RPY_T for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl	MAD_DEV (pointer to MAD_DEV)
 * @param  replay_drop  int
 * @return none
 */
void msec_port_set_igr_rpy_t_replay_drop (MAD_DEV *dev_ctrl, int port, int replay_drop);
/*! \brief set Ingress replay_window in structure RPY_T for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl		MAD_DEV (pointer to MAD_DEV)
 * @param  replay_window	int
 * @return none
 */
void msec_port_set_igr_rpy_t_replay_window (MAD_DEV *dev_ctrl, int port, int reply_window);


/*! \brief set egress context packet number for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl		MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num		int (entry number in egress context table)
 * @param  pn			uint32_t (packet number)
 * @return none
 */
void msec_port_set_ect_pn (MAD_DEV *dev_ctrl, int port, int ent_num, uint32_t pn);
/*! \brief get egress context packet number for port defined in dev_ctrl* 
 *
 * @param *dev_ctrl		MAD_DEV (pointer to MAD_DEV)
 * @param  ent_num		int (entry number in egress context table)
 * @return Return packet number
 */
uint32_t msec_port_get_ect_pn (MAD_DEV *dev_ctrl, int port, int ent_num);

unsigned int msec_port_set_ect_tci (MAD_DEV *dev_ctrl, int port, int ent_num, unsigned int tci);
unsigned int msec_port_set_ect_offset_conf (MAD_DEV *dev_ctrl, int port, int ent_num, unsigned int offset);
unsigned int msec_port_get_ect_tci (MAD_DEV *dev_ctrl, int port, int ent_num);
unsigned int msec_port_get_ect_offset_conf (MAD_DEV *dev_ctrl, int port, int ent_num);
uint32_t port_get_macsec_stat_value (MAD_DEV *dev,  int port, int stat_name);

uint32_t cpu_read (MAD_DEV *dev, int port, unsigned short addr);

uint32_t cpu_port_read (MAD_DEV *dev, int port, unsigned short addr);

int msec_port_get_nxtpn_ent (MAD_DEV *dev, int port, int ent_num);


#ifdef __cplusplus
}
#endif

#endif /* __mx_api_helper_h */
