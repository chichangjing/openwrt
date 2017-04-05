/* some helper functions for TCL to create, delete, access fields of C object */
//#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include "madApi.h"
#include "msecApi.h"
#include "msecFunc.h"

unsigned char * create_u8_arr(int size)
{
  unsigned char *arr = malloc(size);
  return arr;
}

void set_u8_arr_fld(unsigned char *arr, int idx, unsigned char val)
{
	
		arr[idx] = val;
}
unsigned char get_u8_arr_fld(unsigned char *arr, int idx)
{
	
		return arr[idx];
}
void delete_u8_arr (unsigned char *arr)
{
	if (arr != NULL)
	{
		free(arr);
		arr = NULL;
	}
}
unsigned short * create_u16_arr(int size)
{
  unsigned short *arr = malloc(size);
  return arr;
}

void set_u16_arr_fld(unsigned short *arr, int idx, unsigned short val)
{
	
		arr[idx] = val;
}
unsigned short get_u16_arr_fld(unsigned short *arr, int idx)
{
	
		return arr[idx];
}
void delete_u16_arr (unsigned short *arr)
{
	if (arr != NULL)
	{
		free(arr);
		arr = NULL;
	}
}
unsigned int * create_u32_arr(int size)
{
  unsigned int *arr = malloc(size);
  return arr;
}

void set_u32_arr_fld(unsigned int *arr, int idx, unsigned int val)
{
	
		arr[idx] = val;
}
unsigned int get_u32_arr_fld(unsigned int *arr, int idx)
{
	
		return arr[idx];
}
void delete_u32_arr (unsigned int *arr)
{
	if (arr != NULL)
	{
		free(arr);
		arr = NULL;
	}
}
void set_dev_ctrl_dev_num (MAD_DEV *ctrl, int msec_dev_num)
{	
	if (ctrl != NULL)
		ctrl->msec_ctrl.dev_num = msec_dev_num;
}
int get_dev_ctrl_dev_num (MAD_DEV *ctrl)
{
	if (ctrl != NULL)
	   return	ctrl->msec_ctrl.dev_num;
	return 0;
}
void set_dev_ctrl_port_num (MAD_DEV *ctrl, int msec_port_num)
{
	if (ctrl != NULL)
		ctrl->msec_ctrl.port_num = msec_port_num;
}
int get_dev_ctrl_port_num (MAD_DEV *ctrl)
{	
	if (ctrl != NULL)
	   return	ctrl->msec_ctrl.port_num;
	return 0;
}
void set_dev_ctrl_usbport (MAD_DEV *ctrl, int usbport)
{
	if (ctrl != NULL)
		ctrl->appData = (void *)usbport;
}
int get_dev_ctrl_usbport (MAD_DEV *ctrl)
{	
	if (ctrl != NULL)
	   return	(int)ctrl->appData;
	return 0;
}
void set_dev_ctrl_smiaddr (MAD_DEV *ctrl, int smiaddr)
{
	if (ctrl != NULL)
		ctrl->baseRegAddr = smiaddr;
}
int get_dev_ctrl_smiaddr (MAD_DEV* ctrl)
{
	if (ctrl != NULL)
		return (ctrl->baseRegAddr);
	return 0;
}
MAC_CTRL * new_mac_ctrl (int mac_type,  int msec_port_en,  int speed,  int hfd,  int mibcnt_en,  int lpbk_en,  int max_frame_sz)
{
   MAC_CTRL * mac_ctrl = malloc (sizeof(MAC_CTRL));
   mac_ctrl->mac_type = mac_type;
   mac_ctrl->port_en = msec_port_en;
   mac_ctrl->speed = speed;
   mac_ctrl->hfd = hfd;
   mac_ctrl->mibcnt_en = mibcnt_en;
   mac_ctrl->lpbk_en = lpbk_en;
   mac_ctrl->max_frame_sz = max_frame_sz;
   return mac_ctrl;   
}

void delete_mac_ctrl (MAC_CTRL *mac_ctrl)
{
  if (mac_ctrl != NULL)
  {
	   free (mac_ctrl);
	   mac_ctrl = NULL;
  }  
}

MATCH_EN * new_match_en (int en_def, int en_mc, int en_da, int en_sa, int en_vlan, 
						 int en_et, int en_scian, int en_hdr)
{
   MATCH_EN * match = malloc (sizeof(MATCH_EN));
   match->en_def = en_def;
   match->en_mc = en_mc;
   match->en_da = en_da;
   match->en_sa = en_sa;
   match->en_vlan = en_vlan;
   match->en_et = en_et;
   match->en_scian = en_scian;
   match->en_hdr = en_hdr;
   return match;
}
void delete_match_en(MATCH_EN *match_en)
{
   if (match_en != NULL) 
   {
   	free(match_en);
	match_en = NULL;
   }
}
/* access individual fields in MATCH_EN structure */
void msec_port_set_elu_match_en_en_def (MAD_DEV *dev_ctrl, int port, int ent_num, int en_def)
{
	MATCH_EN match_en;
	msec_port_get_elu_match_en (dev_ctrl, port, ent_num, &match_en);
	match_en.en_def = en_def;
	msec_port_set_elu_match_en (dev_ctrl, port, ent_num, &match_en);
}
void msec_port_set_elu_match_en_en_mc (MAD_DEV *dev_ctrl, int port, int ent_num, int en_mc)
{
	MATCH_EN match_en;
	msec_port_get_elu_match_en (dev_ctrl, port, ent_num, &match_en);
	match_en.en_mc = en_mc;
	msec_port_set_elu_match_en (dev_ctrl, port, ent_num, &match_en);
}
void msec_port_set_elu_match_en_en_da (MAD_DEV *dev_ctrl, int port, int ent_num, int en_da)
{
	MATCH_EN match_en;
	msec_port_get_elu_match_en (dev_ctrl, port, ent_num, &match_en);
	match_en.en_da = en_da;
	msec_port_set_elu_match_en (dev_ctrl, port, ent_num, &match_en);
}
void msec_port_set_elu_match_en_en_sa (MAD_DEV *dev_ctrl, int port, int ent_num, int en_sa)
{
	MATCH_EN match_en;
	msec_port_get_elu_match_en (dev_ctrl, port, ent_num, &match_en);
	match_en.en_sa = en_sa;
	msec_port_set_elu_match_en (dev_ctrl, port, ent_num, &match_en);
}
void msec_port_set_elu_match_en_en_vlan (MAD_DEV *dev_ctrl, int port, int ent_num, int en_vlan)
{
	MATCH_EN match_en;
	msec_port_get_elu_match_en (dev_ctrl, port, ent_num, &match_en);
	match_en.en_vlan = en_vlan;
	msec_port_set_elu_match_en (dev_ctrl, port, ent_num, &match_en);
}
void msec_port_set_elu_match_en_en_et(MAD_DEV *dev_ctrl, int port, int ent_num, int en_et)
{
	MATCH_EN match_en;
	msec_port_get_elu_match_en (dev_ctrl, port, ent_num, &match_en);
	match_en.en_et = en_et;
	msec_port_set_elu_match_en (dev_ctrl, port, ent_num, &match_en);
}
void msec_port_set_elu_match_en_hdr(MAD_DEV *dev_ctrl, int port, int ent_num, int en_hdr)
{
	MATCH_EN match_en;
	msec_port_get_elu_match_en (dev_ctrl, port, ent_num, &match_en);
	match_en.en_hdr = en_hdr;
	msec_port_set_elu_match_en (dev_ctrl, port, ent_num, &match_en);
}
/* access individual fields in ILU MATCH_EN structure */
void msec_port_set_ilu_match_en_en_def (MAD_DEV *dev_ctrl, int port, int ent_num, int en_def)
{
	MATCH_EN match_en;
	msec_port_get_ilu_match_en (dev_ctrl, port, ent_num, &match_en);
	match_en.en_def = en_def;
	msec_port_set_ilu_match_en (dev_ctrl, port, ent_num, &match_en);
}
void msec_port_set_ilu_match_en_en_mc (MAD_DEV *dev_ctrl, int port, int ent_num, int en_mc)
{
	MATCH_EN match_en;
	msec_port_get_ilu_match_en (dev_ctrl, port, ent_num, &match_en);
	match_en.en_mc = en_mc;
	msec_port_set_ilu_match_en (dev_ctrl, port, ent_num, &match_en);
}
void msec_port_set_ilu_match_en_en_da (MAD_DEV *dev_ctrl, int port, int ent_num, int en_da)
{
	MATCH_EN match_en;
	msec_port_get_ilu_match_en (dev_ctrl, port, ent_num, &match_en);
	match_en.en_da = en_da;
	msec_port_set_ilu_match_en (dev_ctrl, port, ent_num, &match_en);
}
void msec_port_set_ilu_match_en_en_sa (MAD_DEV *dev_ctrl, int port, int ent_num, int en_sa)
{
	MATCH_EN match_en;
	msec_port_get_ilu_match_en (dev_ctrl, port, ent_num, &match_en);
	match_en.en_sa = en_sa;
	msec_port_set_ilu_match_en (dev_ctrl, port, ent_num, &match_en);
}
void msec_port_set_ilu_match_en_en_vlan (MAD_DEV *dev_ctrl, int port, int ent_num, int en_vlan)
{
	MATCH_EN match_en;
	msec_port_get_ilu_match_en (dev_ctrl, port, ent_num, &match_en);
	match_en.en_vlan = en_vlan;
	msec_port_set_ilu_match_en (dev_ctrl, port, ent_num, &match_en);
}
void msec_port_set_ilu_match_en_en_et (MAD_DEV *dev_ctrl, int port, int ent_num, int en_et)
{
	MATCH_EN match_en;
	msec_port_get_ilu_match_en (dev_ctrl, port, ent_num, &match_en);
	match_en.en_et = en_et;
	msec_port_set_ilu_match_en (dev_ctrl, port, ent_num, &match_en);
}
MATCH_FLD * new_match_fld(unsigned long long da, unsigned long long sa, unsigned short vlan, unsigned short et, unsigned long long sci, 
						  unsigned char tci, unsigned char tci_msk, unsigned char offset_conf, unsigned int hdr_fld)
{
   MATCH_FLD *fld = malloc (sizeof(MATCH_FLD));
   fld->da = da;
   fld->sa = sa;
   fld->vlan = vlan;
   fld->et = et;
   fld->sci = sci;
   fld->tci = tci;
   fld->tci_msk = tci_msk;
   fld->offset_conf = offset_conf;
   fld->hdr_fld = hdr_fld;
   return fld;
}

void delete_match_fld(MATCH_FLD *match_fld)
{
   if (match_fld != NULL)
   {
   	free (match_fld);
	match_fld = NULL;
   }		
}
/* access individual fields in ELU MATCH_FLD structure */
void msec_port_set_elu_match_fld_da (MAD_DEV *dev_ctrl, int port, int ent_num, unsigned long long da)
{
	MATCH_FLD match_fld;
	msec_port_get_elu_match_fld (dev_ctrl, port, ent_num, &match_fld);
	match_fld.da = da;
	msec_port_set_elu_match_fld (dev_ctrl, port, ent_num, &match_fld);
}
unsigned long long msec_port_get_elu_match_fld_da (MAD_DEV *dev_ctrl, int port, int ent_num)
{
	MATCH_FLD match_fld;
	msec_port_get_elu_match_fld (dev_ctrl, port, ent_num, &match_fld);
	return match_fld.da;
}
void msec_port_set_elu_match_fld_sa (MAD_DEV *dev_ctrl, int port, int ent_num, unsigned long long sa)
{
	MATCH_FLD match_fld;
	msec_port_get_elu_match_fld (dev_ctrl, port, ent_num, &match_fld);
	match_fld.sa = sa;
	msec_port_set_elu_match_fld (dev_ctrl, port, ent_num, &match_fld);
}
void msec_port_set_elu_match_fld_vlan (MAD_DEV *dev_ctrl, int port, int ent_num, unsigned short vlan)
{
	MATCH_FLD match_fld;
	msec_port_get_elu_match_fld (dev_ctrl, port, ent_num, &match_fld);
	match_fld.vlan = vlan;
	msec_port_set_elu_match_fld (dev_ctrl, port, ent_num, &match_fld);
}

/* access individual fields in ILU match_fld structure sci, tci, tci_msk */
void msec_port_set_ilu_match_fld_sci (MAD_DEV *dev_ctrl, int port, int ent_num, unsigned long long sci)
{
	MATCH_FLD match_fld;
	msec_port_get_ilu_match_fld (dev_ctrl, port, ent_num, &match_fld);
	match_fld.sci = sci;
	msec_port_set_ilu_match_fld (dev_ctrl, port, ent_num, &match_fld);
}
void msec_port_set_ilu_match_fld_tci (MAD_DEV *dev_ctrl, int port, int ent_num, unsigned char tci)
{
	MATCH_FLD match_fld;
	msec_port_get_ilu_match_fld (dev_ctrl, port, ent_num, &match_fld);
	match_fld.tci = tci;
	msec_port_set_ilu_match_fld (dev_ctrl, port, ent_num, &match_fld);
}
void msec_port_set_ilu_match_fld_tci_msk (MAD_DEV *dev_ctrl, int port, int ent_num, unsigned char tci_msk)
{
	MATCH_FLD match_fld;
	msec_port_get_ilu_match_fld (dev_ctrl, port, ent_num, &match_fld);
	match_fld.tci_msk = tci_msk;
	msec_port_set_ilu_match_fld (dev_ctrl, port, ent_num, &match_fld);
}
void msec_port_set_ilu_match_fld_conf_offset (MAD_DEV *dev_ctrl, int port, int ent_num, unsigned char offset)
{
	MATCH_FLD match_fld;
	msec_port_get_ilu_match_fld (dev_ctrl, port, ent_num, &match_fld);
	match_fld.offset_conf = offset;
	msec_port_set_ilu_match_fld (dev_ctrl, port, ent_num, &match_fld);
}
void msec_port_set_ilu_match_fld_hdr_fld (MAD_DEV *dev_ctrl, int port, int ent_num, unsigned int hdr_fld)
{
	MATCH_FLD match_fld;
	msec_port_get_ilu_match_fld (dev_ctrl, port, ent_num, &match_fld);
	match_fld.hdr_fld = hdr_fld;
	msec_port_set_ilu_match_fld (dev_ctrl, port, ent_num, &match_fld);
}
ACT_FLD * new_act_fld(int ctx_num_sec_level, int drop_maxpn, int drop, int redir, int auth_en, int enc_en)
{
    ACT_FLD * act_fld = malloc (sizeof(ACT_FLD));
	act_fld->ctx_num = ctx_num_sec_level;
	act_fld->sec_level = ctx_num_sec_level;
	act_fld->drop_maxpn = drop_maxpn;
	act_fld->drop = drop;
	act_fld->redir = redir;
	act_fld->auth_en = auth_en;
	act_fld->enc_en = enc_en;
	return act_fld;	
}

void delete_act_fld(ACT_FLD * act_fld)
{
   if (act_fld != NULL)
   	{
   	free (act_fld);
	act_fld = NULL;
   	}
}
void msec_port_set_elu_act_fld_drop (MAD_DEV *dev_ctrl, int port, int ent_num, int drop)
{
	ACT_FLD act_fld;
	msec_port_get_elu_act (dev_ctrl, port, ent_num, &act_fld);
	if (dev_ctrl->deviceId == MAD_88E1340)
	  act_fld.ctx_num = ent_num;
	act_fld.drop = drop;
	msec_port_set_elu_act_fld (dev_ctrl, port, ent_num, &act_fld);
}
void msec_port_set_elu_act_fld_redir (MAD_DEV *dev_ctrl, int port, int ent_num, int redir)
{
	ACT_FLD act_fld;
	msec_port_get_elu_act (dev_ctrl, port, ent_num, &act_fld);
	if (dev_ctrl->deviceId == MAD_88E1340)
	  act_fld.ctx_num = ent_num;
	act_fld.redir = redir;
	msec_port_set_elu_act_fld (dev_ctrl, port, ent_num, &act_fld);
}
void msec_port_set_elu_act_fld_auth_en (MAD_DEV *dev_ctrl, int port, int ent_num, int auth_en)
{
	ACT_FLD act_fld;
	msec_port_get_elu_act (dev_ctrl, port, ent_num, &act_fld);
	if (dev_ctrl->deviceId == MAD_88E1340)
	  act_fld.ctx_num = ent_num;
	act_fld.auth_en = auth_en;
	msec_port_set_elu_act_fld (dev_ctrl, port, ent_num, &act_fld);
}
void msec_port_set_elu_act_fld_enc_en (MAD_DEV *dev_ctrl, int port, int ent_num, int enc_en)
{
	ACT_FLD act_fld;
	msec_port_get_elu_act (dev_ctrl, port, ent_num, &act_fld);
	if (dev_ctrl->deviceId == MAD_88E1340)
	  act_fld.ctx_num = ent_num;
	act_fld.enc_en = enc_en;
	msec_port_set_elu_act_fld (dev_ctrl, port, ent_num, &act_fld);
}
/* ILU ACT_FLD */
void msec_port_set_ilu_act_fld_drop (MAD_DEV *dev_ctrl, int port, int ent_num, int drop)
{
	ACT_FLD act_fld;
	msec_port_get_ilu_act (dev_ctrl, port, ent_num, &act_fld);
	if (dev_ctrl->deviceId == MAD_88E1340)
	  act_fld.ctx_num = ent_num;
	act_fld.drop = drop;
	msec_port_set_ilu_act_fld (dev_ctrl, port, ent_num, &act_fld);
}
void msec_port_set_ilu_act_fld_redir (MAD_DEV *dev_ctrl, int port, int ent_num, int redir)
{
	ACT_FLD act_fld;
	msec_port_get_ilu_act(dev_ctrl, port, ent_num, &act_fld);
	if (dev_ctrl->deviceId == MAD_88E1340)
	  act_fld.ctx_num = ent_num;

	act_fld.redir = redir;
	msec_port_set_ilu_act_fld (dev_ctrl, port, ent_num, &act_fld);
}
void msec_port_set_ilu_act_fld_sec_level (MAD_DEV *dev_ctrl, int port, int ent_num, int sec_level)
{
	ACT_FLD act_fld;
	msec_port_get_ilu_act(dev_ctrl, port, ent_num, &act_fld);
	act_fld.sec_level = sec_level;
	msec_port_set_ilu_act_fld (dev_ctrl, port, ent_num, &act_fld);
}
LKUP_T * new_lkup_t(MATCH_EN * lk_match_en, MATCH_FLD *lk_match_fld, ACT_FLD *act_fld)
{
   /* avoid access lkup_t directly unless it's absolutely necessary. access individual field structure is prefered. */
  LKUP_T *lkup_t = malloc(sizeof(LKUP_T));
  lkup_t->lk_match_en = lk_match_en;
  lkup_t->lk_match_fld = lk_match_fld;
  lkup_t->lk_act_fld = act_fld;
  return lkup_t;
}

MATCH_EN * get_match_en_from_lkup_t (LKUP_T *lkup_t)
{
   return lkup_t->lk_match_en;
}
MATCH_FLD *get_match_fld_from_lkup_t(LKUP_T *lkup_t)
{
  return lkup_t->lk_match_fld;
  
}

ACT_FLD *get_act_fld_from_lkup_t (LKUP_T *lkup_t)
{
  return lkup_t->lk_act_fld;
}
void  delete_lkup_t (LKUP_T * lkup_t)
{
   if (lkup_t == NULL)
   	return;
   
   if (lkup_t->lk_match_fld != NULL)
   	{
   	free(lkup_t->lk_match_fld);
	lkup_t->lk_match_fld = NULL;
   	}
   if (lkup_t->lk_match_en != NULL)
   	{
   	free(lkup_t->lk_match_en);
	lkup_t->lk_match_en = NULL;
   	}
   if (lkup_t->lk_act_fld != NULL)
   	{
   	free(lkup_t->lk_act_fld);
	lkup_t->lk_act_fld = NULL;
   	}

  free(lkup_t);
  lkup_t = NULL;
		
}

CTX_T * new_ctx_t (unsigned long long sci,  unsigned int pn, unsigned char tci, unsigned char offset, unsigned char key[16])
{
  int i;
  CTX_T *ctx = malloc(sizeof(CTX_T));
  ctx->sci = sci;
  ctx->pn = pn;
  ctx->tci = tci;
  ctx->offset_conf = offset;
  for ( i=0;i<16;i++)
  {
    ctx->key[i] = key[i];
  }
  return ctx;
}
void delete_ctx_t (CTX_T *ctx)
{
  if (ctx == NULL)
    return;
  free(ctx);
  ctx = NULL;
}
void msec_port_get_igr_key (MAD_DEV *dev_ctrl, int port, int ent_num, unsigned char key[16])
{
  CTX_T igr_ctx;
  int i;
  msec_port_get_ikey_entry (dev_ctrl, port, ent_num, &igr_ctx);
  for (i=0;i<16;i++)
	  key[i] = igr_ctx.key[i];
  
}
void msec_port_get_igr_hkey (MAD_DEV *dev_ctrl, int port, int ent_num, unsigned char hkey[16])
{
  CTX_T igr_ctx;
  int i;
  msec_port_get_ihkey_entry (dev_ctrl, port, ent_num, &igr_ctx);
  for (i=0;i<16;i++)
	  hkey[i] = igr_ctx.hkey[i];
  
}
void msec_port_get_egr_key (MAD_DEV *dev_ctrl, int port, int ent_num, unsigned char key[16])
{
  CTX_T egr_ctx;
  int i;
  msec_port_get_ekey_entry (dev_ctrl, port, ent_num, &egr_ctx);
  for (i=0;i<16;i++)
	  key[i] = egr_ctx.key[i];
  
}
void msec_port_get_egr_hkey (MAD_DEV *dev_ctrl, int port, int ent_num, unsigned char hkey[16])
{
  CTX_T egr_ctx;
  int i;
  msec_port_get_ehkey_entry (dev_ctrl, port, ent_num, &egr_ctx);
  for (i=0;i<16;i++)
	  hkey[i] = egr_ctx.hkey[i];
  
}

void msec_port_set_ect_pn (MAD_DEV *dev_ctrl, int port, int ent_num, unsigned int pn)
{
	unsigned short my_addr;
    my_addr = VN_CFG_ELU_CTX(dev_ctrl) + (ent_num * 4) + 2;
    msecWritePortReg(dev_ctrl, port, my_addr, pn);
}

unsigned int msec_port_get_ect_pn (MAD_DEV *dev_ctrl, int port, int ent_num)
{
	CTX_T ctx;
	msec_port_get_ect_entry (dev_ctrl, port, ent_num, &ctx);
	return ctx.pn;
}

void msec_port_set_ect_offset_conf (MAD_DEV *dev_ctrl, int port, int ent_num, unsigned int offset)
{
	unsigned short my_addr;
	unsigned int data;
    my_addr = VN_CFG_ELU_CTX(dev_ctrl) + (ent_num * 4) + 3;
	msecReadPortReg(dev_ctrl, port, my_addr, &data);
	data = (data & 0xFF) | (offset << 8);
    msecWritePortReg(dev_ctrl, port, my_addr, data);
}
unsigned int msec_port_get_ect_offset_conf (MAD_DEV *dev_ctrl, int port, int ent_num)
{
	unsigned short my_addr;
	unsigned int data;
    my_addr = VN_CFG_ELU_CTX(dev_ctrl) + (ent_num * 4) + 3;
	msecReadPortReg(dev_ctrl, port, my_addr, &data);
	return ((data & 0xFF00) >> 8);
}
void msec_port_set_ect_tci (MAD_DEV *dev_ctrl, int port, int ent_num, unsigned int tci)
{
	unsigned short my_addr;
	unsigned int data;
    my_addr = VN_CFG_ELU_CTX(dev_ctrl) + (ent_num * 4) + 3;	
	msecReadPortReg(dev_ctrl, port, my_addr, &data);
	data = ((data & 0xFF00) | tci);
    msecWritePortReg(dev_ctrl, port, my_addr, data);
}
unsigned int msec_port_get_ect_tci (MAD_DEV *dev_ctrl, int port, int ent_num)
{
	unsigned short my_addr;
	unsigned int data;
    my_addr = VN_CFG_ELU_CTX(dev_ctrl) + (ent_num * 4) + 3;	
	msecReadPortReg(dev_ctrl, port, my_addr, &data);
	return (data & 0xFF);
}
REDIR_HDR_T * new_redir_hdr_t (unsigned long long da, unsigned short et, unsigned long long sa0,
							   unsigned long long sa1, unsigned long long sa2, unsigned long long sa3)
{
  REDIR_HDR_T *redir = malloc(sizeof(REDIR_HDR_T));
  redir->da = da;
  redir->et = et;
  
  redir->sa[0] = sa0;
  redir->sa[1] = sa1;
  redir->sa[2] = sa2;
  redir->sa[3] = sa3;
  return redir;
}
void delete_redir_hdr_t(REDIR_HDR_T *redir)
{
  if (redir == NULL)
	  return;
  free(redir);
  redir = NULL;
}

CMN_CFG_T *new_cmn_cfg_t(unsigned char conf_offset, unsigned short def_vlan, unsigned long long mc_high, unsigned short mc_low_msk, unsigned char trail_sz)
{
  CMN_CFG_T *cfg = malloc(sizeof(CMN_CFG_T));
  cfg->conf_offset = conf_offset;
  cfg->def_vlan = def_vlan;
  cfg->mc_high = mc_high;
  cfg->mc_low_msk = mc_low_msk;
  cfg->trail_sz = trail_sz;
  return cfg;
}
void delete_cmn_cfg_t (CMN_CFG_T *cfg)
{
   if (cfg==NULL)
     return;
   free(cfg);
   cfg = NULL;
}

void set_cmn_cfg_offset (CMN_CFG_T *cfg, unsigned short conf_offset)
{
  cfg->conf_offset = (MAD_U8)conf_offset;
}

EGR_CFG_T *new_egr_cfg_t (int def_drop, int parse_ae, int parse_vlan, int hdr_as_pn)
{
  EGR_CFG_T *egr_cfg  = malloc(sizeof(EGR_CFG_T));
  egr_cfg->def_drop   = def_drop;
  egr_cfg->parse_ae   = parse_ae;
  egr_cfg->parse_vlan = parse_vlan;
  egr_cfg->hdr_as_pn = hdr_as_pn;
  return egr_cfg;
}

void delete_egr_cfg_t(EGR_CFG_T *egr_cfg)
{
  if (egr_cfg != NULL)
  {
     free(egr_cfg);
     egr_cfg = NULL;
  }
}
int msec_port_get_egr_hdr_as_pn (MAD_DEV *dev_ctrl, int port)
{
	EGR_CFG_T egr_cfg;
	msec_port_get_egr_cfg(dev_ctrl, port, &egr_cfg);
	return egr_cfg.hdr_as_pn;
}

void msec_port_set_egr_hdr_as_pn (MAD_DEV *dev_ctrl, int port, int flag)
{	
	EGR_CFG_T egr_cfg;
	msec_port_get_egr_cfg(dev_ctrl, port, &egr_cfg);
	egr_cfg.hdr_as_pn = flag;
	msec_port_egr_cfg(dev_ctrl, port, &egr_cfg);
}
int msec_port_get_egr_parse_macsec (MAD_DEV *dev_ctrl, int port)
{
	EGR_CFG_T egr_cfg;
	msec_port_get_egr_cfg(dev_ctrl, port, &egr_cfg);
	return egr_cfg.parse_ae;
}
void msec_port_set_egr_parse_macsec (MAD_DEV *dev_ctrl, int port, int flag)
{
	EGR_CFG_T egr_cfg;
	msec_port_get_egr_cfg(dev_ctrl, port, &egr_cfg);
	egr_cfg.parse_ae = flag;
	msec_port_egr_cfg(dev_ctrl, port, &egr_cfg);
}

IGR_DEF_T *new_igr_def_t (int def_drop, int def_redir, int drop_badtag, int retain_hdr, 
						  int parse_vlan, int ins_hdr_pn)
{
  IGR_DEF_T *def_act = malloc(sizeof(IGR_DEF_T));
  def_act->def_drop = def_drop;
  def_act->def_redir = def_redir;
  def_act->drop_badtag = drop_badtag;
  def_act->retain_hdr = retain_hdr;
  def_act->parse_vlan = parse_vlan;
  def_act->ins_hdr_pn = ins_hdr_pn;
  return def_act;
}
void delete_igr_def_t(IGR_DEF_T *def_act)
{
  if (def_act != NULL)
  {
     free(def_act);
     def_act = NULL;
  }
}

/* Acess individual fields of structure IGR_DEF_T */
int msec_port_get_igr_def_drop (MAD_DEV *dev_ctrl, int port)
{
	IGR_DEF_T def_t;
	msec_port_get_igr_def_act (dev_ctrl, port, &def_t);
	return def_t.def_drop;
}
int msec_port_get_igr_def_redir (MAD_DEV *dev_ctrl, int port)
{	IGR_DEF_T def_t;
	msec_port_get_igr_def_act (dev_ctrl, port, &def_t);
	return def_t.def_redir;
}
int msec_port_get_igr_drop_badtag (MAD_DEV *dev_ctrl, int port)
{	IGR_DEF_T def_t;
	msec_port_get_igr_def_act (dev_ctrl, port, &def_t);
	return def_t.drop_badtag;
}
int msec_port_get_igr_retain_hdr (MAD_DEV *dev_ctrl, int port)
{	IGR_DEF_T def_t;
	msec_port_get_igr_def_act (dev_ctrl, port, &def_t);
	return def_t.retain_hdr;
}
int msec_port_get_igr_ins_hdr_pn (MAD_DEV *dev_ctrl, int port)
{	IGR_DEF_T def_t;
	msec_port_get_igr_def_act (dev_ctrl, port, &def_t);
	return def_t.ins_hdr_pn;
}

void msec_port_set_igr_def_drop (MAD_DEV *dev_ctrl, int port, int def_drop)
{	IGR_DEF_T def_t;
	msec_port_get_igr_def_act (dev_ctrl, port, &def_t);
	def_t.def_drop = def_drop;
	msec_port_set_igr_def_act (dev_ctrl, port, &def_t);
}
void msec_port_set_igr_def_redir(MAD_DEV *dev_ctrl, int port, int def_redir)
{
	IGR_DEF_T def_t;
	msec_port_get_igr_def_act (dev_ctrl, port, &def_t);
	def_t.def_redir = def_redir;
	msec_port_set_igr_def_act (dev_ctrl, port, &def_t);
}
void msec_port_set_igr_drop_badtag(MAD_DEV *dev_ctrl, int port, int drop_badtag)
{
	IGR_DEF_T def_t;
	msec_port_get_igr_def_act (dev_ctrl, port, &def_t);
	def_t.drop_badtag = drop_badtag;
	msec_port_set_igr_def_act (dev_ctrl, port, &def_t);
}
void msec_port_set_igr_retain_hdr(MAD_DEV *dev_ctrl, int port, int retain_hdr)
{
	IGR_DEF_T def_t;
	msec_port_get_igr_def_act (dev_ctrl, port, &def_t);
	def_t.retain_hdr = retain_hdr;
	msec_port_set_igr_def_act (dev_ctrl, port, &def_t);
}
void msec_port_set_igr_ins_hdr_pn (MAD_DEV *dev_ctrl, int port, int ins_hdr_pn)
{
	IGR_DEF_T def_t;
	msec_port_get_igr_def_act (dev_ctrl, port, &def_t);
	def_t.ins_hdr_pn = ins_hdr_pn;
	msec_port_set_igr_def_act (dev_ctrl, port, &def_t);
}

REDIR_T *new_igr_redir_t (int redir_untagged, int redir_badtag, int redir_unk_sci, int redir_unused_sa, int redir_unchecked, 
	                           int redir_delayed, int redir_invalid)
{
   REDIR_T * redir_t = malloc (sizeof (REDIR_T));
   redir_t->redir_untagged = redir_untagged;
   redir_t->redir_badtag = redir_badtag;
   redir_t->redir_unk_sci = redir_unk_sci;
   redir_t->redir_unused_sa = redir_unused_sa;
   redir_t->redir_unchecked = redir_unchecked;
   redir_t->redir_delayed = redir_delayed;
   redir_t->redir_invalid = redir_invalid;
   return redir_t;
}

void delete_igr_redir_t (REDIR_T *redir_vec)
{
  if (redir_vec != NULL)
  {
    free(redir_vec);
    redir_vec = NULL;
  }
}
int msec_port_get_igr_redir_untagged (MAD_DEV *dev_ctrl, int port)
{
	REDIR_T redir;
	msec_port_get_igr_redir_vec (dev_ctrl, port, &redir);
	return redir.redir_untagged;
}
int msec_port_get_igr_redir_badtag (MAD_DEV *dev_ctrl, int port)
{
	REDIR_T redir;
	msec_port_get_igr_redir_vec (dev_ctrl, port, &redir);
	return redir.redir_badtag;
}
int msec_port_get_igr_redir_unkown_sci(MAD_DEV *dev_ctrl, int port)
{
	REDIR_T redir;
	msec_port_get_igr_redir_vec (dev_ctrl, port, &redir);
	return redir.redir_unk_sci;
}
int msec_port_get_igr_redir_unused_sa (MAD_DEV *dev_ctrl, int port)
{
	REDIR_T redir;
	msec_port_get_igr_redir_vec (dev_ctrl, port, &redir);
	return redir.redir_unused_sa;
}
int msec_port_get_igr_redir_unchecked (MAD_DEV *dev_ctrl, int port)
{
	REDIR_T redir;
	msec_port_get_igr_redir_vec (dev_ctrl, port, &redir);
	return redir.redir_unchecked;
}
int msec_port_get_igr_redir_delayed (MAD_DEV *dev_ctrl, int port)
{
	REDIR_T redir;
	msec_port_get_igr_redir_vec (dev_ctrl, port, &redir);
	return redir.redir_delayed;
}
int msec_port_get_igr_redir_invalid (MAD_DEV *dev_ctrl, int port)
{
	REDIR_T redir;
	msec_port_get_igr_redir_vec (dev_ctrl, port, &redir);
	return redir.redir_invalid;
}
void msec_port_set_igr_redir_untagged (MAD_DEV *dev_ctrl, int port, int redir_untagged)
{
	REDIR_T redir;
	msec_port_get_igr_redir_vec (dev_ctrl, port, &redir);
	redir.redir_untagged = redir_untagged;
	msec_port_set_igr_redir_vec (dev_ctrl, port, &redir);
}
void msec_port_set_igr_redir_badtag (MAD_DEV *dev_ctrl, int port, int redir_badtag)
{
	REDIR_T redir;
	msec_port_get_igr_redir_vec (dev_ctrl, port, &redir);
	redir.redir_badtag = redir_badtag;
	msec_port_set_igr_redir_vec (dev_ctrl, port, &redir);
}
void msec_port_set_igr_redir_unkown_sci(MAD_DEV *dev_ctrl, int port, int redir_unk_sci)
{
	REDIR_T redir;
	msec_port_get_igr_redir_vec (dev_ctrl, port, &redir);
	redir.redir_unk_sci = redir_unk_sci;
	msec_port_set_igr_redir_vec (dev_ctrl, port, &redir);
}
void msec_port_set_igr_redir_unused_sa (MAD_DEV *dev_ctrl, int port, int redir_unused_sa)
{
	REDIR_T redir;
	msec_port_get_igr_redir_vec (dev_ctrl, port, &redir);
	redir.redir_unused_sa = redir_unused_sa;
	msec_port_set_igr_redir_vec (dev_ctrl, port, &redir);
}
void msec_port_set_igr_redir_unchecked (MAD_DEV *dev_ctrl, int port, int redir_unchecked)
{
	REDIR_T redir;
	msec_port_get_igr_redir_vec (dev_ctrl, port, &redir);
	redir.redir_unchecked = redir_unchecked;
	msec_port_set_igr_redir_vec (dev_ctrl, port, &redir);
}
void msec_port_set_igr_redir_delayed (MAD_DEV *dev_ctrl, int port, int redir_delayed)
{
	REDIR_T redir;
	msec_port_get_igr_redir_vec (dev_ctrl, port, &redir);
	redir.redir_delayed = redir_delayed;
	msec_port_set_igr_redir_vec (dev_ctrl, port, &redir);
}
void msec_port_set_igr_redir_invalid (MAD_DEV *dev_ctrl, int port, int redir_invalid)
{
	REDIR_T redir;
	msec_port_get_igr_redir_vec (dev_ctrl, port, &redir);
	redir.redir_invalid = redir_invalid;
	msec_port_set_igr_redir_vec (dev_ctrl, port, &redir);
}
SCI_T * new_igr_sci_t (unsigned long long implicit_sci, unsigned short scb_port, unsigned short def_port)
{
   SCI_T *sci_t = malloc(sizeof(SCI_T));
   sci_t->implicit_sci = implicit_sci;
   sci_t->scb_port = scb_port;
   sci_t->def_port = def_port;
   return sci_t;
}

void delete_igr_sci_t (SCI_T *sci_t)
{
   if (sci_t != NULL)
   {
   	free(sci_t);
	sci_t = NULL;
   }
}

RPY_T * new_igr_rpy_t (int validate_frames, int replay_protect, int replay_drop, int replay_window)
{
   RPY_T *rpy_t = malloc (sizeof (RPY_T));
   rpy_t->validate_frames = validate_frames;
   rpy_t->replay_protect = replay_protect;
   rpy_t->replay_drop = replay_drop;
   rpy_t->replay_window = replay_window;
   return rpy_t;
}

void delete_igr_rpy_t (RPY_T *rpy_t)
{
   if (rpy_t != NULL)
   { 
   	free (rpy_t);
	rpy_t = NULL;
   }
}

int msec_port_get_igr_rpy_t_validate_frames (MAD_DEV *dev_ctrl, int port)
{
  RPY_T rpy;
  msec_port_get_igr_rpy_var (dev_ctrl, port, &rpy);
  return rpy.validate_frames;
}
int msec_port_get_igr_rpy_t_replay_protect (MAD_DEV *dev_ctrl, int port)
{
  RPY_T rpy;
  msec_port_get_igr_rpy_var (dev_ctrl, port, &rpy);
  return rpy.replay_protect;
}
int msec_port_get_igr_rpy_t_replay_drop (MAD_DEV *dev_ctrl, int port)
{
  RPY_T rpy;
  msec_port_get_igr_rpy_var (dev_ctrl, port, &rpy);
  return rpy.replay_drop;
}
int msec_port_get_igr_rpy_t_replay_window (MAD_DEV *dev_ctrl, int port)
{
  RPY_T rpy;
  msec_port_get_igr_rpy_var (dev_ctrl, port, &rpy);
  return rpy.replay_window;
}
void msec_port_set_igr_rpy_t_validate_frames (MAD_DEV *dev_ctrl, int port, int validate_frames)
{
  RPY_T rpy;
  msec_port_get_igr_rpy_var (dev_ctrl, port, &rpy);
  rpy.validate_frames = validate_frames;
  msec_port_set_igr_rpy_var (dev_ctrl, port, &rpy);
}
void msec_port_set_igr_rpy_t_replay_protect (MAD_DEV *dev_ctrl, int port, int replay_protect)
{
  RPY_T rpy;
  msec_port_get_igr_rpy_var (dev_ctrl, port, &rpy);
  rpy.replay_protect = replay_protect;
  msec_port_set_igr_rpy_var (dev_ctrl, port, &rpy);
}
void msec_port_set_igr_rpy_t_replay_drop (MAD_DEV *dev_ctrl, int port, int replay_drop)
{
  RPY_T rpy;
  msec_port_get_igr_rpy_var (dev_ctrl, port, &rpy);
  rpy.replay_drop = replay_drop;
  msec_port_set_igr_rpy_var (dev_ctrl, port, &rpy);
}
void msec_port_set_igr_rpy_t_replay_window (MAD_DEV *dev_ctrl, int port, int reply_window)
{
  RPY_T rpy;
  msec_port_get_igr_rpy_var (dev_ctrl, port, &rpy);
  rpy.replay_window = reply_window;
  msec_port_set_igr_rpy_var (dev_ctrl, port, &rpy);
}
IGR_CFG_T * new_igr_cfg_t (IGR_DEF_T *def_act, REDIR_T *redir_vec, SCI_T *sci_info, RPY_T *rpy_var)
{
  IGR_CFG_T *igr;
  if (def_act == NULL || redir_vec == NULL || sci_info == NULL || rpy_var == NULL)
	 return NULL;
  igr = malloc(sizeof(IGR_CFG_T));
  igr->def_act = def_act;
  igr->redir_vec = redir_vec;
  igr->sci_info = sci_info;
  igr->rpy_var = rpy_var;
  return igr;
}

void delete_igr_cfg_t (IGR_CFG_T *igr)
{
  if (igr == NULL)
    return;
  delete_igr_def_t(igr->def_act);
  delete_igr_redir_t(igr->redir_vec);
  delete_igr_sci_t(igr->sci_info);
  delete_igr_rpy_t(igr->rpy_var);

  free(igr);
  igr=NULL;
}

ETM_CFG_T * new_etm_cfg_t (unsigned short etm[8], unsigned char etm_eg_en, unsigned char etm_ig_en, unsigned char etm_eg_act, unsigned char etm_ig_act)
{
   int i;
   ETM_CFG_T *etm_cfg_t = malloc (sizeof(ETM_CFG_T));
   for (i=0;i<8;i++)
   {
   	etm_cfg_t->etm[i] = etm[i];
   }
   etm_cfg_t->etm_eg_en = etm_eg_en;
   etm_cfg_t->etm_ig_en = etm_ig_en;
   etm_cfg_t->etm_eg_act = etm_eg_act;
   etm_cfg_t->etm_ig_act = etm_ig_act;
   return etm_cfg_t;
}

void delete_etm_cfg_t (ETM_CFG_T * etm_cfg_t)
{
   if (etm_cfg_t != NULL)
   	{
   	free (etm_cfg_t);
	etm_cfg_t = NULL;
   	}
}
void set_etm_cfg_ethertype (ETM_CFG_T *cfg, unsigned short ethertype, int index)
{
  cfg->etm[index] = ethertype;
}
void set_etm_cfg_egr_action (ETM_CFG_T *cfg, int action)
{
  cfg->etm_eg_act = action;
}
void set_etm_cfg_igr_action (ETM_CFG_T *cfg, int action)
{
  cfg->etm_ig_act = action;
}
void set_etm_cfg_egr_enable (ETM_CFG_T *cfg, unsigned char bitmsk)
{
  cfg->etm_eg_en = bitmsk;
}
void set_etm_cfg_igr_enable (ETM_CFG_T *cfg, unsigned char bitmsk)
{
  cfg->etm_ig_en = bitmsk;
}
unsigned short get_etm_cfg_ethertype (ETM_CFG_T *cfg, int index)
{
  return cfg->etm[index];
}
int get_etm_cfg_egr_action (ETM_CFG_T *cfg)
{
  return cfg->etm_eg_act;
}
int get_etm_cfg_igr_action (ETM_CFG_T *cfg)
{
  return cfg->etm_ig_act;
}
unsigned char get_etm_cfg_egr_enable (ETM_CFG_T *cfg)
{
  return cfg->etm_eg_en;
}
unsigned char get_etm_cfg_igr_enable (ETM_CFG_T *cfg)
{
  return cfg->etm_ig_en;
}
FIPS_CFG_T * new_fips_cfg_t (unsigned char fips_vec[16], unsigned char key[16], unsigned char egr_igr, unsigned char eng_type)
{
   int i;
   FIPS_CFG_T *fips = malloc(sizeof(FIPS_CFG_T));
   for (i=0;i<16;i++)
   {
   	fips->fips_vec[i] = fips_vec[i];
	fips->key[i] = key[i];
   }
   fips->egr_igr = egr_igr;
   fips->eng_type = eng_type;
   return fips;
}
void set_fips_cfg_t (FIPS_CFG_T *fips, unsigned char fips_vec[16], unsigned char key[16], unsigned char egr_igr, unsigned char eng_type)
{
   int i;
   for (i=0;i<16;i++)
   {
   	fips->fips_vec[i] = fips_vec[i];
	fips->key[i] = key[i];
   }
   fips->egr_igr = egr_igr;
   fips->eng_type = eng_type;
//   return fips;
}

void delete_fips_cfg_t (FIPS_CFG_T *fips)
{ 
    if (fips != NULL)
    {
    	free(fips);
	fips = NULL;
    }
}

	
MAD_U32 port_get_macsec_stat_value (MAD_DEV *dev,  int port, int stat_name)
{
  MAD_STATUS   retVal;
  MAD_U32 data;
  retVal = msec_port_get_macsec_stat_value (dev,  port, stat_name, &data);
  if (retVal!=MAD_OK)
  {
     printf("msec_port_get_macsec_stat_value failed for (port %i, stat_name %d)\n",(int)port,stat_name);
     return -1;             
  }
  return data;
}

unsigned int cpu_read (MAD_DEV *dev, int port, unsigned short addr)
{
  MAD_STATUS   retVal;
  MAD_U32 data;

  retVal = msecReadReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR(("Failed to read register \n"));
     return -1;
  }
  return data;
}

unsigned int cpu_port_read (MAD_DEV *dev, int port, unsigned short addr)
{
  MAD_STATUS   retVal;
  MAD_U32 data;

  retVal = msecReadPortReg (dev, port, addr, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR(("Failed to call msecReadPortReg \n"));
     return -1;
  }
  return data;
}

int msec_port_get_nxtpn_ent (MAD_DEV *dev, int port, int ent_num)
{
  MAD_STATUS   retVal;
  MAD_U32 data;

  retVal = msec_port_get_nxtpn_entry (dev, port, ent_num, &data);
  if(retVal != MAD_OK)
  {
     MAD_DBG_ERROR(("Failed to call msec_port_get_nxtpn_entry \n"));
     return -1;
  }
  return data;
}
