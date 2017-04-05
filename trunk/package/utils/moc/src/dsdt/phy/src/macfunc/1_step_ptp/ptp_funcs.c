#include <mvlPtpCopyright.h>

/********************************************************************************
* ptp_func.c
* 
* DESCRIPTION:
*       Internal functions of 1-step PTP.
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


#include "pmeu.rx.inc"
#include "pmeu.tx.inc"
#include "umeu.rx.inc"
#include "umeu.tx.inc"
#include "pmeu.rx.ntp.inc"
#include "pmeu.tx.ntp.inc"
#include "umeu.rx.ntp.inc"
#include "umeu.tx.ntp.inc"

/*************************************************************************/

DSDT_STATUS ptp_1step_dump_mac_statistics (int port, unsigned int *macValList) {
  DSDT_STATUS   retVal = DSDT_OK;
  int n;
  PTP_DBG_INFO(("\nDump all MAC statistics counters on dev dev_num.\n"));
  for (n=0; n<1; n++) {
    int i = 0x2000;
	int macName = 0;
    while(ptp_1step_macNmList[macName]!=0) {
      unsigned int val = ptp_read_reg(port, i);
      macValList[macName] = val;
      i++;
	  macName++;
    }
  }
  return retVal;
}

DSDT_STATUS ptp_1step_dump_sysmac_statistics (int port, unsigned int *sysmacValList){
  DSDT_STATUS   retVal = DSDT_OK;
	int macName = 0;

  int n;
  PTP_DBG_INFO(("\nDump all SYSMAC statistics counters on dev dev_num.\n"));
  for (n=0; n<1; n++) {
    int i =(0x2000 + 32 * 4);
    while(ptp_1step_macNmList[macName]!=0) {
      unsigned int val = ptp_read_reg(port, i);
      sysmacValList[macName] = val;
      i++;
	  macName++;
    }
  }
  return retVal;
}

DSDT_STATUS ptp_1step_set_smac_loopback (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short regaddr = 0x50*PTP_ADDR_ADJ + PTP_PORT_OFF(port);
  unsigned int data = ptp_read_reg (port, regaddr);
  msec_bit_set (data, 1);
  retVal = ptp_write_reg(port, regaddr, data);
  return retVal;
}

/* Enable PTP by clearing PTP bypass */
DSDT_STATUS enable_ptp (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
/*  retVal = ptp_writecheck_reg(port, 0x3080+PTP_PORT_OFF(port), 1);
  retVal = ptp_writecheck_reg(port, 0x3480+PTP_PORT_OFF(port), 1); 
*/
  retVal = ptp_writecheck_reg(port, VN_EG0_PTP+PTP_RF+PTP_PORT_OFF(port), 9);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, VN_IG0_PTP+PTP_RF+PTP_PORT_OFF(port), 9); 
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, VN_EG0_PTP+PTP_RF+PTP_PORT_OFF(port)+9*PTP_ADDR_ADJ, 0x100);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, VN_IG0_PTP+PTP_RF+PTP_PORT_OFF(port)+9*PTP_ADDR_ADJ, 0x100); 
  return retVal;
}
/* Disable PTP by setting PTP bypass */
DSDT_STATUS disable_ptp (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  retVal = ptp_writecheck_reg(port, VN_EG0_PTP+PTP_RF+PTP_PORT_OFF(port), 8);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, VN_IG0_PTP+PTP_RF+PTP_PORT_OFF(port), 8);
  return retVal;
}


/* Enable RX CRC check */
DSDT_STATUS ptp_1step_enable_rx_crc_check (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  retVal = set_bits (port, 0x41*PTP_ADDR_ADJ+PTP_PORT_OFF(port), 1);
  return retVal;
}
/* Disable RX CRC check */
DSDT_STATUS ptp_1step_disable_rx_crc_check (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  retVal = clear_bits (port, 0x41*PTP_ADDR_ADJ+PTP_PORT_OFF(port), 1);
  return retVal;
}

/* set wiremac Inter frame gap */
DSDT_STATUS set_wiremac_ipg (int port, int ipg) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short regaddr;
  unsigned int data;
  regaddr = 0x41*PTP_ADDR_ADJ+PTP_PORT_OFF(port);
  data = ptp_read_reg (port, regaddr);
  msec_bits_write(&data, ipg, 10, 4);
  retVal = ptp_writecheck_reg(port, regaddr, data);
  
  return retVal;
}
DSDT_STATUS set_cutthrough_mode (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short regaddr;
  unsigned int data;
  regaddr = 0x70*PTP_ADDR_ADJ+PTP_PORT_OFF(port);
  data = ptp_read_reg (port, regaddr);
  msec_bits_write(&data, 0, 1, 0);
  retVal = ptp_writecheck_reg(port, regaddr, data);
  
  return retVal;
}
DSDT_STATUS ptp_set_store_forward_mode (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short regaddr;
  unsigned int data;
  regaddr = 0x70*PTP_ADDR_ADJ+PTP_PORT_OFF(port);
  data = ptp_read_reg (port, regaddr);
  msec_bits_write(&data, 3, 1, 0);
  retVal = ptp_writecheck_reg(port, regaddr, data);
  return retVal;
}
/* workaround on macsec */
DSDT_STATUS ptp_macsec_workaround_1 (void) {
  DSDT_STATUS   retVal = DSDT_OK;
  int port;
  for(port=0; port<4; port++)
  {
    retVal = ptp_writecheck_reg(port, 0xA*PTP_ADDR_ADJ+PTP_PORT_OFF(port), 0x3A);
	if(retVal!=DSDT_OK) break;
  }
  return retVal;
}

/* Some PTP definitions */

/* common PTP init  */
DSDT_STATUS ptp_common_init (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  /* Setup STT_CTL */

  retVal = ptp_writecheck_reg(port, VN_EG0_PTP+PTP_RF+5*PTP_ADDR_ADJ+PTP_PORT_OFF(port), 0x145);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, VN_IG0_PTP+PTP_RF+5*PTP_ADDR_ADJ+PTP_PORT_OFF(port), 0x145);
  enable_ptp(port);
  return retVal;
}

/* PTP parser init */
DSDT_STATUS ptp_parser_init (int port) {
  DSDT_STATUS   retVal = DSDT_OK;

  
  /* Egress only */
/*  retVal = ptp_writecheck_reg(port, (VN_EG0_PTP+PTP_PORT_OFF(port)+PTP_PMEU_PAM+27), 0x81ff); */
  retVal = ptp_writecheck_reg(port, (VN_EG0_PTP+PTP_PORT_OFF(port)+PTP_PMEU_PAM+25*PTP_ADDR_ADJ), 0xca00);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, (VN_EG0_PTP+PTP_PORT_OFF(port)+PTP_PMEU_PAM+26*PTP_ADDR_ADJ), 0x3b9a);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, (VN_EG0_PTP+PTP_PORT_OFF(port)+PTP_PMEU_PAM+27*PTP_ADDR_ADJ), 0x8007);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, (VN_EG0_PTP+PTP_PORT_OFF(port)+PTP_PMEU_PAM+28*PTP_ADDR_ADJ), 0x88b5);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, (VN_EG0_PTP+PTP_PORT_OFF(port)+PTP_PMEU_PAM+29*PTP_ADDR_ADJ), 0x88b6);
  if(retVal!=DSDT_OK) return retVal;
/*  retVal = ptp_writecheck_reg(port, (VN_EG0_PTP+PTP_PORT_OFF(port)+PTP_PMEU_PAM+30*PTP_ADDR_ADJ), 0xca00);
  retVal = ptp_writecheck_reg(port, (VN_EG0_PTP+PTP_PORT_OFF(port)+PTP_PMEU_PAM+31*PTP_ADDR_ADJ), 0x3b9a); */

/* .............. */

  /* Both ingress and egress paths */
  /* Write `PPED+16 `ENV_IPV4_LEN_MSK=0xf00 */
  retVal = ptp_writecheck_reg(port, (VN_EG0_PTP+PTP_PORT_OFF(port)+16*PTP_ADDR_ADJ), 0xf00);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, (VN_IG0_PTP+PTP_PORT_OFF(port)+16*PTP_ADDR_ADJ), 0xf00);
  /* Write `PPED+18 `LLC_HDR_MAX_LEN=1500 (decimal) */
  retVal = ptp_writecheck_reg(port, (VN_EG0_PTP+PTP_PORT_OFF(port)+18*PTP_ADDR_ADJ), 1500);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, (VN_IG0_PTP+PTP_PORT_OFF(port)+18*PTP_ADDR_ADJ), 1500);
  if(retVal!=DSDT_OK) return retVal;
  /* Write `PPED+19 `ENV_MPLS_LBL_19_4=0 */
  retVal = ptp_writecheck_reg(port, (VN_EG0_PTP+PTP_PORT_OFF(port)+19*PTP_ADDR_ADJ), 0);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, (VN_IG0_PTP+PTP_PORT_OFF(port)+19*PTP_ADDR_ADJ), 0);
  if(retVal!=DSDT_OK) return retVal;
  /* Write `PPED+20 `ENV_MPLS_LBL_3_0=VN_EG0_PTP */
  retVal = ptp_writecheck_reg(port, (VN_EG0_PTP+PTP_PORT_OFF(port)+20*PTP_ADDR_ADJ), 0x3000);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, (VN_IG0_PTP+PTP_PORT_OFF(port)+20*PTP_ADDR_ADJ), 0x3000);
  if(retVal!=DSDT_OK) return retVal;
  /* Write `PPED+21 `ENV_MPLS_LBL_MSK=0xf000 */
  retVal = ptp_writecheck_reg(port, (VN_EG0_PTP+PTP_PORT_OFF(port)+21*PTP_ADDR_ADJ), 0xf000);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, (VN_IG0_PTP+PTP_PORT_OFF(port)+21*PTP_ADDR_ADJ), 0xf000);
  if(retVal!=DSDT_OK) return retVal;
  /* Write `PPED+22 `ENV_DSAP_SSAP_23_8=0xaaaa */
  retVal = ptp_writecheck_reg(port, (VN_EG0_PTP+PTP_PORT_OFF(port)+22*PTP_ADDR_ADJ), 0xaaaa);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, (VN_IG0_PTP+PTP_PORT_OFF(port)+22*PTP_ADDR_ADJ), 0xaaaa);
  if(retVal!=DSDT_OK) return retVal;
  /* Write `PPED+23 `ENV_DSAP_SSAP_7_0=0x300 */
  retVal = ptp_writecheck_reg(port, (VN_EG0_PTP+PTP_PORT_OFF(port)+23*PTP_ADDR_ADJ), 0x300);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, (VN_IG0_PTP+PTP_PORT_OFF(port)+23*PTP_ADDR_ADJ), 0x300);
  if(retVal!=DSDT_OK) return retVal;
  /* Write `PPED+24 `ENV_DSAP_SSAP_MSK=0xff00 */
  retVal = ptp_writecheck_reg(port, (VN_EG0_PTP+PTP_PORT_OFF(port)+24*PTP_ADDR_ADJ), 0xff00);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, (VN_IG0_PTP+PTP_PORT_OFF(port)+24*PTP_ADDR_ADJ), 0xff00);
  if(retVal!=DSDT_OK) return retVal;
  /* Write `PPED+30 `NTP_DST_PRT=0x7b */
  retVal = ptp_writecheck_reg(port, (VN_EG0_PTP+PTP_PORT_OFF(port)+30*PTP_ADDR_ADJ), 0x7b);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, (VN_IG0_PTP+PTP_PORT_OFF(port)+30*PTP_ADDR_ADJ), 0x7b);
  if(retVal!=DSDT_OK) return retVal;
  /* Write `PPED+31 `PTPG_DST_PRT=0x140 */
  retVal = ptp_writecheck_reg(port, (VN_EG0_PTP+PTP_PORT_OFF(port)+31*PTP_ADDR_ADJ), 0x140);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, (VN_IG0_PTP+PTP_PORT_OFF(port)+31*PTP_ADDR_ADJ), 0x140);
  return retVal;
}
DSDT_STATUS ptp_set_udata ( int port, unsigned int * filedata, unsigned int baseaddr );

DSDT_STATUS ptp_parser_set_udata (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int eg_addr = (VN_EG0_PTP+PTP_PORT_OFF(port)+PTP_PMEU_UDATA);
  unsigned int ig_addr = (VN_IG0_PTP+PTP_PORT_OFF(port)+PTP_PMEU_UDATA);
  /* Load egress parser udata */
  retVal = ptp_set_udata( port, pt_init_data, eg_addr);
  if(retVal!=DSDT_OK) return retVal;
  /* Load ingress parser udata */
  retVal = ptp_set_udata( port, pr_init_data, ig_addr);
  return retVal;
}

DSDT_STATUS ptp_parser_set_udata_ntp (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int eg_addr = (VN_EG0_PTP+PTP_PORT_OFF(port)+PTP_PMEU_UDATA);
  unsigned int ig_addr = (VN_IG0_PTP+PTP_PORT_OFF(port)+PTP_PMEU_UDATA);
  /* Load egress parser udata */
  retVal = ptp_set_udata( port, pt_ntp_init_data, eg_addr);
  if(retVal!=DSDT_OK) return retVal;
  /* Load ingress parser udata */
  retVal = ptp_set_udata( port, pr_ntp_init_data, ig_addr);
  return retVal;
}
DSDT_STATUS ptp_check_udata ( int port, unsigned int * filedata, unsigned int baseaddr );


DSDT_STATUS ptp_parser_check_udata (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int eg_addr = VN_EG0_PTP+PTP_PORT_OFF(port)+PTP_PMEU_UDATA;
  unsigned int  ig_addr = VN_IG0_PTP+PTP_PORT_OFF(port)+PTP_PMEU_UDATA;
  /* Check egress parser udata */
  retVal = ptp_check_udata( port, pt_init_data, eg_addr);
  if(retVal!=DSDT_OK) return retVal;
  /* Check ingress parser udata */
  retVal = ptp_check_udata( port, pr_init_data, ig_addr);
  return retVal;
}

DSDT_STATUS ptp_parser_check_udata_ntp (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int eg_addr = VN_EG0_PTP+PTP_PORT_OFF(port)+PTP_PMEU_UDATA;
  unsigned int  ig_addr = VN_IG0_PTP+PTP_PORT_OFF(port)+PTP_PMEU_UDATA;
  /* Check egress parser udata */
  retVal = ptp_check_udata( port, pt_ntp_init_data, eg_addr);
  if(retVal!=DSDT_OK) return retVal;
  /* Check ingress parser udata */
  retVal = ptp_check_udata( port, pr_ntp_init_data, ig_addr);
  return retVal;
}

/* PTP init for test*/
/* Set external clock in/out */
DSDT_STATUS ptp_test_clk_in_out_init (int port, int isMaster) {

  DSDT_STATUS   retVal = DSDT_OK;
#if 0
  if(isMaster)
    retVal = ptp_writecheck_reg(port, 0x230b+PTP_PORT_OFF(port), 0x0205);
/*  retVal = ptp_set_tai(port, 1, 1, 0); */
  else
  {
PTP_DBG_DEMO((">>> @@@@@@@@@@@ ptp_test_clk_in_out_init slave 230b to 207\n"));
    retVal = ptp_writecheck_reg(port, 0x230b+PTP_PORT_OFF(port), 0x0207);
/*  retVal = ptp_set_tai(port, 3, 1, 0); */
  }
  if(retVal!=DSDT_OK) return retVal;
      /* test only */
  /* set Clock cycle */
/*  retVal = ptp_writecheck_reg(port, 0x232c+PTP_PORT_OFF(port), 0x3800); */
/*  retVal = ptp_writecheck_reg(port, 0x232c+PTP_PORT_OFF(port), 0x3d0900); 1ms */
  retVal = ptp_writecheck_reg(port, 0x232c+PTP_PORT_OFF(port), 0x27d78400); /*100ms */
  if(retVal!=DSDT_OK) return retVal;
  /* set Drift threshold */
  retVal = ptp_writecheck_reg(port, 0x2327+PTP_PORT_OFF(port), 0x0aff000f);
  if(retVal!=DSDT_OK) return retVal;
#else
      /* test only */
  /* set Clock cycle */
/*  retVal = ptp_writecheck_reg(port, 0x232c+PTP_PORT_OFF(port), 0x3800); */
  retVal = ptp_writecheck_reg(port, 0x232c, 0xffffff00);
  if(retVal!=DSDT_OK) return retVal;
  /* set Drift threshold */
/*  retVal = ptp_writecheck_reg(port, 0x2327+PTP_PORT_OFF(port), 0x0aff000f); */
/*  retVal = ptp_writecheck_reg(port, 0x2327, 0x000f0002);  */
/*  retVal = ptp_writecheck_reg(port, 0x2327, 0x08ffffff);  */
  retVal = ptp_writecheck_reg(port, 0x2327, 0x00ffffff);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, 0x2336, 0x0000001f);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, 0x2324, 0x000ff801);
  if(retVal!=DSDT_OK) return retVal;

  if(isMaster)
    retVal = ptp_writecheck_reg(port, 0x230b, 0x0205);
/*  retVal = ptp_set_tai(port, 1, 1, 0); */
  else
  {
PTP_DBG_DEMO((">>> @@@@@@@@@@@ ptp_test_clk_in_out_init slave 230b to 207\n"));
    retVal = ptp_writecheck_reg(port, 0x230b, 0x0207);
/*  retVal = ptp_set_tai(port, 3, 1, 0); */
  }
  if(retVal!=DSDT_OK) return retVal;
#endif

  return retVal;
}



/* PTP update init */
DSDT_STATUS ptp_update_init (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  /*Both ingress and egress paths */
  /*Write `PPDD+23 Asymmetry[15:0] */
  retVal = ptp_writecheck_reg(port, (VN_EG0_PTP+PTP_UMEU_PAM+PTP_PORT_OFF(port)+23*PTP_ADDR_ADJ), 0x0);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, (VN_IG0_PTP+PTP_UMEU_PAM+PTP_PORT_OFF(port)+23*PTP_ADDR_ADJ), 0x0);
  if(retVal!=DSDT_OK) return retVal;
  /*Write `PPDD+24 Asymmetry[31:16] */
  retVal = ptp_writecheck_reg(port, (VN_EG0_PTP+PTP_UMEU_PAM+PTP_PORT_OFF(port)+24*PTP_ADDR_ADJ), 0x0);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, (VN_IG0_PTP+PTP_UMEU_PAM+PTP_PORT_OFF(port)+24*PTP_ADDR_ADJ), 0x0);
  if(retVal!=DSDT_OK) return retVal;
  /*Write `PPDD+25 `ENV_ALL_ONE_VAL=0xffff */
  retVal = ptp_writecheck_reg(port, (VN_EG0_PTP+PTP_UMEU_PAM+PTP_PORT_OFF(port)+25*PTP_ADDR_ADJ), 0xffff);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, (VN_IG0_PTP+PTP_UMEU_PAM+PTP_PORT_OFF(port)+25*PTP_ADDR_ADJ), 0xffff);
  if(retVal!=DSDT_OK) return retVal;
  /*Write `PPDD+30 one_sec[15:0] */
  retVal = ptp_writecheck_reg(port, (VN_EG0_PTP+PTP_UMEU_PAM+PTP_PORT_OFF(port)+30*PTP_ADDR_ADJ), 0xca00);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, (VN_IG0_PTP+PTP_UMEU_PAM+PTP_PORT_OFF(port)+30*PTP_ADDR_ADJ), 0xca00);
  if(retVal!=DSDT_OK) return retVal;
  /*Write `PPDD+31 one_sec[31:16] */
  retVal = ptp_writecheck_reg(port, (VN_EG0_PTP+PTP_UMEU_PAM+PTP_PORT_OFF(port)+31*PTP_ADDR_ADJ), 0x3b9a);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, (VN_IG0_PTP+PTP_UMEU_PAM+PTP_PORT_OFF(port)+31*PTP_ADDR_ADJ), 0x3b9a);
  if(retVal!=DSDT_OK) return retVal;

  /*Egress path */
  /*Write `PPDD+26 `ENV_ONE_VAL */
  retVal = ptp_writecheck_reg(port, (VN_EG0_PTP+PTP_UMEU_PAM+PTP_PORT_OFF(port)+26*PTP_ADDR_ADJ), 1);
  if(retVal!=DSDT_OK) return retVal;
 /*Write `PPDD+27 lat_adj[15:0] */
  retVal = ptp_writecheck_reg(port, (VN_EG0_PTP+PTP_UMEU_PAM+PTP_PORT_OFF(port)+27*PTP_ADDR_ADJ), (0x145*8+24));
  if(retVal!=DSDT_OK) return retVal;
  /*Write `PPDD+28 lat_adj[31:16] */
  retVal = ptp_writecheck_reg(port, (VN_EG0_PTP+PTP_UMEU_PAM+PTP_PORT_OFF(port)+28*PTP_ADDR_ADJ), 0x0);
  if(retVal!=DSDT_OK) return retVal;
  /*Write `PPDD+29 `ENV_ZERO_VAL */
  retVal = ptp_writecheck_reg(port, (VN_EG0_PTP+PTP_UMEU_PAM+PTP_PORT_OFF(port)+29*PTP_ADDR_ADJ), 0);
  if(retVal!=DSDT_OK) return retVal;

  /*Ingress */
  /*Write `PPDD+26 LinkDelay[15:0] */
  retVal = ptp_writecheck_reg(port, (VN_IG0_PTP+PTP_UMEU_PAM+PTP_PORT_OFF(port)+26*PTP_ADDR_ADJ), 0x0);
  if(retVal!=DSDT_OK) return retVal;
  /*Write `PPDD+27 LinkDelay[31:16] */
  retVal = ptp_writecheck_reg(port, (VN_IG0_PTP+PTP_UMEU_PAM+PTP_PORT_OFF(port)+27*PTP_ADDR_ADJ), 0x0);
  if(retVal!=DSDT_OK) return retVal;
  /*Write `PPDD+28 `TST_HDR_ETYPE */
  retVal = ptp_writecheck_reg(port, (VN_IG0_PTP+PTP_UMEU_PAM+PTP_PORT_OFF(port)+28*PTP_ADDR_ADJ), 0x88b5);
  if(retVal!=DSDT_OK) return retVal;
  /*Write `PPDD+29 `ENV_TST_RSV_MSK */
/*  retVal = ptp_writecheck_reg(port, (VN_IG0_PTP+PTP_UMEU_PAM+PTP_PORT_OFF(port)+29*PTP_ADDR_ADJ), 0x8007); */
  retVal = ptp_writecheck_reg(port, (VN_IG0_PTP+PTP_UMEU_PAM+PTP_PORT_OFF(port)+29*PTP_ADDR_ADJ), 0x88b6);
  return retVal;
}

DSDT_STATUS ptp_update_set_udata (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int  eg_addr = VN_EG0_PTP+PTP_PORT_OFF(port)+PTP_UMEU_UDATA;
  unsigned int  ig_addr = VN_IG0_PTP+PTP_PORT_OFF(port)+PTP_UMEU_UDATA;

  /* Load egress update udata */
  retVal = ptp_set_udata( port, ut_init_data, eg_addr);
  if(retVal!=DSDT_OK) return retVal;
  /* Load ingress update udata */
  retVal = ptp_set_udata( port, ur_init_data, ig_addr);
  return retVal;
}
DSDT_STATUS ptp_update_check_udata (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int  eg_addr = VN_EG0_PTP+PTP_PORT_OFF(port)+PTP_UMEU_UDATA;
  unsigned int  ig_addr = VN_IG0_PTP+PTP_PORT_OFF(port)+PTP_UMEU_UDATA;

  /* Check egress update udata */
  retVal = ptp_check_udata( port, ut_init_data, eg_addr);
  if(retVal!=DSDT_OK) return retVal;
  /* Check ingress update udata */
  retVal = ptp_check_udata( port, ur_init_data, ig_addr);
  return retVal;
}

DSDT_STATUS ptp_update_set_udata_ntp (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int  eg_addr = VN_EG0_PTP+PTP_PORT_OFF(port)+PTP_UMEU_UDATA;
  unsigned int  ig_addr = VN_IG0_PTP+PTP_PORT_OFF(port)+PTP_UMEU_UDATA;

  /* Load egress update udata */
  retVal = ptp_set_udata( port, ut_ntp_init_data, eg_addr);
  if(retVal!=DSDT_OK) return retVal;
  /* Load ingress update udata */
  retVal = ptp_set_udata( port, ur_ntp_init_data, ig_addr);
  return retVal;
}
DSDT_STATUS ptp_update_check_udata_ntp (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int  eg_addr = VN_EG0_PTP+PTP_PORT_OFF(port)+PTP_UMEU_UDATA;
  unsigned int  ig_addr = VN_IG0_PTP+PTP_PORT_OFF(port)+PTP_UMEU_UDATA;

  /* Check egress update udata */
  retVal = ptp_check_udata( port, ut_ntp_init_data, eg_addr);
  if(retVal!=DSDT_OK) return retVal;
  /* Check ingress update udata */
  retVal = ptp_check_udata( port, ur_ntp_init_data, ig_addr);
  return retVal;
}

/* Load PTP udata into udata space */
DSDT_STATUS ptp_set_udata (int port, unsigned int * filedata, unsigned int baseaddr ) {
  DSDT_STATUS   retVal = DSDT_OK;
  int word_index = 0;
  unsigned int word;
  PTP_DBG_INFO( ("Loading PTP microcode data addr 0x%x\n", baseaddr));
  while ((word=*filedata)!=-1) {
    filedata++;
    /*PTP_DBG_INFO( ("0x%x 0x%x 0x%x" baseaddr word_index val] */
/*    retVal = ptp_writecheck_reg (baseaddr+word_index, word); */
    retVal = ptp_write_reg (port, baseaddr+word_index*PTP_ADDR_ADJ, word);
    if(retVal!=DSDT_OK) return retVal;
    word_index ++;
  }
  return retVal;
}

/* Check PTP udata from file against udata space */
DSDT_STATUS ptp_check_udata (int port, unsigned int * filedata, unsigned int baseaddr ) {
  DSDT_STATUS   retVal = DSDT_OK;
  int word_index= 0;
  int err = 0;
  unsigned int word;
  PTP_DBG_INFO( ("Checking PTP microcode data addr 0x%x\n",  baseaddr));
  while ((word=*filedata)!=-1) {
    unsigned int expect_val = word;
    unsigned short addr = baseaddr+word_index*PTP_ADDR_ADJ;
    unsigned int val = ptp_read_reg(port, addr);
/*    unsigned short val = ptp_readcheck_reg(addr); */
    if(val != expect_val) {
      PTP_DBG_DEMO( ("ERROR: memory addr 0x%x value 0x%x does not match file value 0x%x\n", addr, 
	  val, expect_val));
	  err++;
    }
    filedata++;
    word_index++;
  }

  if(err != 0) { 
    PTP_DBG_DEMO( ("ERROR: found %x mismatches\n", err));
    return DSDT_FAIL;
  }
  return retVal;
}

/* Write DONE to udata spaces to just make PTP pass packet along */
/* (Enric modified this as follows) */
/* for parser (both ingress and egress): addr 0 & 1 => HALT */
/* for update: for ingress: addr 0 => HALT, addr 1 => OUT EOP, addr 2 => HALT */
/*             for egress : addr 0 => HALT, addr 1 => OUT HFT, addr 2 => OUT EOP, addr 3 => HALT  */
DSDT_STATUS ptp_1step_setup_simple_udata (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
/*   Egress parser udata */
/*  retVal = ptp_writecheck_reg(port, 0x3100, 0x30000); */
/*  retVal = ptp_writecheck_reg(port, 0x3101, 0x30000); */
/*  retVal = ptp_writecheck_reg(port, 0x3102, 0x30000); */
/*   Egress update udata */
/*  retVal = ptp_writecheck_reg(port, 0x3200, 0x30000); */
/*  retVal = ptp_writecheck_reg(port, 0x3201, 0x30000); */
/*  retVal = ptp_writecheck_reg(port, 0x3202, 0x30000); */
/*   Ingress parser udata */
/*  retVal = ptp_writecheck_reg(port, 0x3500, 0x30000); */
/*  retVal = ptp_writecheck_reg(port, 0x3501, 0x30000); */
/*  retVal = ptp_writecheck_reg(port, 0x3502, 0x30000); */
/*   Ingress update udata */
/*  retVal = ptp_writecheck_reg(port, 0x3600, 0x30000); */
/*  retVal = ptp_writecheck_reg(port, 0x3601, 0x30000); */
/*  retVal = ptp_writecheck_reg(port, 0x3602, 0x30000); */

  /* Parser Egress */
  retVal = ptp_writecheck_reg(port, VN_EG0_PTP+PTP_PMEU_UDATA+PTP_PORT_OFF(port), 0x30000);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, VN_EG0_PTP+PTP_PMEU_UDATA+1*PTP_ADDR_ADJ+PTP_PORT_OFF(port), 0x30000);
  if(retVal!=DSDT_OK) return retVal;
  /* Parser Ingress */
  retVal = ptp_writecheck_reg(port, VN_IG0_PTP+PTP_PMEU_UDATA+PTP_PORT_OFF(port), 0x30000);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, VN_IG0_PTP+PTP_PMEU_UDATA+1*PTP_ADDR_ADJ+PTP_PORT_OFF(port), 0x30000);
  if(retVal!=DSDT_OK) return retVal;
  /* Update Egress */
  retVal = ptp_writecheck_reg(port, VN_EG0_PTP+PTP_UMEU_UDATA+PTP_PORT_OFF(port), 0x30000);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, VN_EG0_PTP+PTP_UMEU_UDATA+1*PTP_ADDR_ADJ+PTP_PORT_OFF(port), 0x42400);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, VN_EG0_PTP+PTP_UMEU_UDATA+2*PTP_ADDR_ADJ+PTP_PORT_OFF(port), 0x40000);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port,  VN_EG0_PTP+PTP_UMEU_UDATA+3*PTP_ADDR_ADJ+PTP_PORT_OFF(port), 0x30000);
  if(retVal!=DSDT_OK) return retVal;
  /* Update Ingress */
  retVal = ptp_writecheck_reg(port, VN_IG0_PTP+PTP_UMEU_UDATA+PTP_PORT_OFF(port), 0x30000);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, VN_IG0_PTP+PTP_UMEU_UDATA+1*PTP_ADDR_ADJ+PTP_PORT_OFF(port), 0x40000);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, VN_IG0_PTP+PTP_UMEU_UDATA+2*PTP_ADDR_ADJ+PTP_PORT_OFF(port), 0x30000);
  return retVal;
}


/* return LUT value adjusted for field position */
long long lut_match_fld_val(PTP_MATCH field, long long value ) {

  long long  mask = ((1 << lut_match_fld_sz[field])-1);
  PTP_DBG_INFO(("lut_match_fld_val\n"));
  return ((value&mask) << lut_match_fld_pos[field]);
}
/* return LUT value extracted from vector */
long long  lut_match_fld_extract( PTP_MATCH field, long long vector) {

  long long  mask = ((1 << lut_match_fld_sz[field])-1);
  return ((vector >> lut_match_fld_pos[field])&mask);
}

/* return RX LUT value adjusted for field position */
unsigned short lut_rx_action_fld_val( PTP_RX_ACTION field, unsigned short value ) {

  unsigned short mask = ((1 << lut_rx_action_fld_sz[field])-1);
  return ((value&mask) << lut_rx_action_fld_pos[field]);
}

/* return RX LUT value extracted from vector  */
unsigned long long lut_rx_action_fld_extract (PTP_RX_ACTION field, unsigned long long vector ) {

  unsigned long long  mask = ((1 << lut_rx_action_fld_sz[field])-1);
  return ((vector >> lut_rx_action_fld_pos[field])&mask);
}

/* return TX LUT value adjusted for field position */
unsigned short lut_tx_action_fld_val( PTP_TX_ACTION field, unsigned short value ) {

  unsigned short mask = ((1 << lut_tx_action_fld_sz[field])-1);
  return ((value&mask) << lut_tx_action_fld_pos[field]);
}

/* return TX LUT value extracted from vector  */
unsigned long long lut_tx_action_fld_extract (PTP_TX_ACTION field, unsigned long long vector ) {

  unsigned long long  mask = ((1 << lut_tx_action_fld_sz[field])-1);
  return ((vector >> lut_tx_action_fld_pos[field])&mask);
}

/* return flag value adjusted for flag position */
unsigned long long lut_flag_fld_val(int flag, unsigned long long value ) {

/*   return ((value&1) << [lsearch pam_out_flags flag] ]; */
   return ((value&1) << flag);
}
/*DSDT_STATUS read_lut_entry (long long *lut_match, long long *lut_valid, long long *lut_action, short lut_index, PTP_TYPE path ); */

/*******************************************************************************
* prgm_lut_entry
*
* DESCRIPTION:
*     program lut entry
*      lut_key: 80 bit match fields
*      lut_valid: 80 bit valid fields (correspond to lut_match)
*      lut_action: 32 bit action fields
*      lut_index: index of LUT entry to program
*      path: eg0_ptp for egress or ig0_ptp for ingress
*
* INPUTS:
*       lut_match - 80 bit match fields
*       lut_valid: 80 bit valid fields (correspond to lut_match)
*       lut_action: 32 bit action fields
*       lut_index: index of LUT entry to program
*       path: eg0_ptp for egress or ig0_ptp for ingress
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
DSDT_STATUS prgm_lut_entry (int port, unsigned long long lut_key, unsigned long long lut_valid, unsigned long lut_key_upper, unsigned long lut_valid_upper, unsigned long long lut_action, 
short lut_index, int path ) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short match_base_addr =(path+PTP_LUT_KEYMASK+8*PTP_ADDR_ADJ*lut_index);
  unsigned short action_base_addr;

  PTP_DBG_INFO(("prgm_lut_entry %x\n", lut_index));

  PTP_DBG_INFO(("port %x path %x, index %2d => key %0llx, vld %0llx, act %0llx\n", port, path, lut_index, lut_key, lut_valid, lut_action));
  /* Have to write multiple words at a time, so don't use retVal = ptp_writecheck_reg */
  retVal = ptp_write_reg (port, match_base_addr+0*PTP_ADDR_ADJ,(unsigned int)((lut_key>>0)&0xffffffff));
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_write_reg (port, match_base_addr+1*PTP_ADDR_ADJ,(unsigned int)((lut_key>>32)&0xffffffff));
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_write_reg (port, match_base_addr+2*PTP_ADDR_ADJ,(unsigned int)((lut_key_upper)&0xffffffff));
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_write_reg (port, match_base_addr+4*PTP_ADDR_ADJ,(unsigned int)((lut_valid>>0)&0xffffffff));
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_write_reg (port, match_base_addr+5*PTP_ADDR_ADJ,(unsigned int)((lut_valid>>32)&0xffffffff));
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_write_reg (port, match_base_addr+6*PTP_ADDR_ADJ,(unsigned int)((lut_valid_upper>>0)&0xffffffff));
  if(retVal!=DSDT_OK) return retVal;

  /* set action_base_addr [expr $base_addr($path)+$ptp_addr(LUT_ACTION)+$lut_index]; */
  action_base_addr = (path+PTP_LUT_ACTION+lut_index);

  /*# Have to write multiple words at a time, so don't use writecheck_reg */
  /*write_reg [expr $action_base_addr+0] [expr $lut_action & $mask_low]; */
  retVal = ptp_write_reg (port, action_base_addr+0,(unsigned int)((lut_action)&0xffffffff));
  if(retVal!=DSDT_OK) return retVal;
/*  retVal = ptp_write_reg (port, action_base_addr+1,(lut_action>>32)&0xffffffff); */
#if 0
{
  unsigned long long tmp_lut_match;
  unsigned int tmp_lut_match_hi;
  unsigned long long tmp_lut_valid;
  unsigned int tmp_lut_valid_hi;
  unsigned long long tmp_lut_action;

  retVal = read_lut_entry (&tmp_lut_match, &tmp_lut_valid, &tmp_lut_match_hi, &tmp_lut_valid_hi, 
	  &tmp_lut_action, lut_index, path );
  if(retVal!=DSDT_OK) return retVal;
  PTP_DBG_DEMO(("\nMATCH/VALID vectors 0x%llx 0x%llx\n", tmp_lut_match, tmp_lut_valid));
  PTP_DBG_DEMO(("\nACTION vector 0x%llx\n", tmp_lut_action));

}
#endif
  return retVal;
}

/* Read lut entry */
DSDT_STATUS read_lut_entry (int port, unsigned long long *lut_key, unsigned long long *lut_valid, unsigned long *lut_key_top, unsigned long *lut_valid_top, unsigned long long *lut_action, short lut_index, int path ) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short match_base_addr =(path+PTP_LUT_KEYMASK+8*PTP_ADDR_ADJ*lut_index);
  unsigned short action_base_addr;
  unsigned int data;
  unsigned long long dataLong;

  PTP_DBG_INFO(("read_lut_entry %x\n", lut_index));

  /* Have to write multiple words at a time, so don't use retVal = ptp_writecheck_reg */
  data = ptp_read_reg (port, match_base_addr+1*PTP_ADDR_ADJ);
  dataLong = data;
  dataLong <<=32;
  data = ptp_read_reg (port, match_base_addr+0*PTP_ADDR_ADJ);
  dataLong|= data;
  *lut_key = dataLong;
  data = ptp_read_reg (port, match_base_addr+2*PTP_ADDR_ADJ);
  *lut_key_top = data;

  data = ptp_read_reg (port, match_base_addr+5*PTP_ADDR_ADJ);
  dataLong = data;
  dataLong <<=32;
  data = ptp_read_reg (port, match_base_addr+4*PTP_ADDR_ADJ);
  dataLong|= data;
  *lut_valid = dataLong;
  data = ptp_read_reg (port, match_base_addr+6*PTP_ADDR_ADJ);
  *lut_valid_top = data;

  action_base_addr = (path+PTP_LUT_ACTION+lut_index);
/*   action_base_addr = (path+PTP_LUT_ACTION+2*lut_index); */
  /* Have to read multiple words at a time, so don't use ptp_readcheck_reg */
/*  data = ptp_read_reg (port, action_base_addr+1);
  dataLong = data;
  dataLong <<=32;
*/
  dataLong = 0LL;
  data = ptp_read_reg (port, action_base_addr+0);
  dataLong = data;
  *lut_action = dataLong;
  PTP_DBG_INFO((">>> Read %s lut entry %x : key %llx  vld %llx  act %llx\n", 
		(path==VN_IG0_PTP+PTP_PORT_OFF(port))?"igr": "egr", lut_index,   *lut_key, *lut_valid, *lut_action));
  return retVal;
}

/* Check lut entry in memory */
DSDT_STATUS check_lut_entry ( int port, int lut_index, int path ) {
  DSDT_STATUS   retVal = DSDT_OK;
#ifdef DEBUG_PTP
/*   unsigned int match_base_addr; */
  unsigned long long lut_match;
  unsigned long lut_match_hi;
  unsigned long long lut_valid;
  unsigned long lut_valid_hi;
  int field = 0;
/*   unsigned int action_base_addr; */
  unsigned long long lut_action;
/*   long long dataLong; */

  PTP_DBG_DEMO(("check_lut_entry %x\n", lut_index));
  read_lut_entry (port, &lut_match, &lut_valid, &lut_match_hi, &lut_valid_hi, 
	  &lut_action, lut_index, path );
  PTP_DBG_DEMO(("\nMATCH/VALID vectors 0x%llx 0x%llx\n", lut_match, lut_valid));

/*  foreach field [array names lut_match_fld_pos] { */
  while (lut_match_fld_pos_name[field]!=0) {
     unsigned long long match_fld = lut_match_fld_extract( field, lut_match);
     unsigned long long valid_fld = lut_match_fld_extract( field, lut_valid);
     PTP_DBG_DEMO(("MATCH/VALID field %s 0x%llx %llx\n", lut_match_fld_pos_name[field], match_fld, valid_fld));
	 field++;
  }

  PTP_DBG_DEMO(("\nACTION vectors 0x%llx\n", lut_action));
  if((path==VN_IG0_PTP)||(path==VN_IG1_PTP)||(path==VN_IG2_PTP)||(path==VN_IG3_PTP))
  {
    field = 0;
    while (lut_rx_action_fld_pos_name[field]!=0) {
      long long action_fld = lut_rx_action_fld_extract (field, lut_action);
      PTP_DBG_DEMO(("Ingress ACTION field %s 0x%llx\n", lut_rx_action_fld_pos_name[field], action_fld));
	  field ++;
	}
  }
  else if((path==VN_EG0_PTP)||(path==VN_EG1_PTP)||(path==VN_EG2_PTP)||(path==VN_EG3_PTP))
  {
    field = 0;
    while (lut_tx_action_fld_pos_name[field]!=0) {
      long long action_fld = lut_tx_action_fld_extract (field, lut_action);
      PTP_DBG_DEMO(("Egress ACTION field %s 0x%llx\n", lut_tx_action_fld_pos_name[field], action_fld));
	  field ++;
	}
  }
#endif
  return retVal;
};

DSDT_STATUS ptp_find_lut_action(int port, PTP_1STEP_LUT_ACT action, PTP_Blk path, int *entryNum) 
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_top;
  unsigned long lut_vld_top;
  unsigned long long lut_act; 
  unsigned long long lut_act_tmp; 
  int idx =0;
/*  int addr = (path == PTP_igr)?VN_IG0_PTP:VN_EG0_PTP; */
  PTP_DBG_TRACE(("PTP enable_lut_action port %x path %x idx %x action %x\n", port, path, idx, action));


  lut_act_tmp = prgm_ptp_lut_acts(action);
  for(idx=0; idx<15; idx++)
  {
    retVal = read_lut_entry (port, &lut_key, &lut_vld, &lut_key_top, &lut_vld_top, &lut_act, idx, path );
    if(retVal!=DSDT_OK) return retVal;

    if(lut_act&lut_act_tmp)
	{
		*entryNum = idx;
		break;
	}
	}
  return retVal;
}

DSDT_STATUS ptp_enable_lut_action(int port, int idx, PTP_1STEP_LUT_ACT action, PTP_Blk path,DSDT_BOOL en ) 
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_top;
  unsigned long lut_vld_top;
  unsigned long long lut_act; 
  unsigned long long lut_act_tmp; 
  int addr = (path == PTP_igr)?VN_IG0_PTP:VN_EG0_PTP;
  PTP_DBG_TRACE(("PTP enable_lut_action port %x path %x idx %x action %x\n", port, path, idx, action));


  retVal = read_lut_entry (port, &lut_key, &lut_vld, &lut_key_top, &lut_vld_top, &lut_act, idx, path );
  if(retVal!=DSDT_OK) return retVal;

  lut_act_tmp = prgm_ptp_lut_acts(action);
  if(lut_act&lut_act_tmp)
  {
      if(en == DSDT_TRUE)
	  {
        lut_act |= lut_act_tmp;
	  }
      else
	  {
        lut_act &= ~lut_act_tmp;
	  }
  }
  retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_top, lut_vld_top, lut_act, idx, addr+PTP_PORT_OFF(port));

  return retVal;
}

/* init LUT */
DSDT_STATUS lut_init(void) {
  DSDT_STATUS   retVal = DSDT_OK;
  /* Ingress */
  /* Default => do nothing */
  return retVal;
};



/* print PTP stats */
DSDT_STATUS get_ptp_stats1 (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short baddr = (VN_EG0_PTP+PTP_PORT_OFF(port)+PTP_STATS);
  int index = 0;
  PTP_DBG_DEMO(("\nEgress PTP STATS: \n"));
  /*foreach name ptp_stat_name { */
  while (ptp_stat_name[index]!=0) {
     PTP_DBG_DEMO(("name %s = 0x%x\n", ptp_stat_name[index],  ptp_read_reg(port, (baddr+index))));
     index++;
  }
  PTP_DBG_DEMO(("\nIngress PTP STATS: \n"));
  baddr = baddr+0x40;
  index = 0;
/*  foreach name ptp_stat_name { */
  while (ptp_stat_name[index]!=0) {
     PTP_DBG_DEMO(("name %s 0x%x\n", ptp_stat_name[index],  ptp_read_reg(port, baddr+index)));
     index++;
  }

  return retVal;
}
/* dump memory */
DSDT_STATUS ptp_1step_dump_mem (int port, unsigned int addr, int range) {
  DSDT_STATUS   retVal = DSDT_OK;
#ifdef DEBUG_PTP
  int i;

  for ( i=0; i < range; i++) {
    unsigned int full_addr =addr+i*PTP_ADDR_ADJ;
    PTP_DBG_INFO(("0x%x 0x%x\n", full_addr, ptp_readcheck_reg(port, full_addr)));
  }
#endif
  return retVal;
}

/* Clear memory */
DSDT_STATUS ptp_1step_write_zeroes (int port, unsigned int addr, int range)  {
  DSDT_STATUS   retVal = DSDT_OK;
  int i;

  for ( i=0; i < range; i++) {
    unsigned int full_addr =addr+i*PTP_ADDR_ADJ;
    retVal = ptp_writecheck_reg(port, full_addr, 0);
    if(retVal!=DSDT_OK) return retVal;
  }
  return retVal;
};

/* memory test */
DSDT_STATUS ptp_1step_write_fs_check (int port, unsigned int addr, int range, unsigned int mask) {
  DSDT_STATUS   retVal = DSDT_OK;
  int err = 0;
  int i;

  unsigned short expect_val =((0xffffffff)&mask);
  for ( i=0; i < range; i++) {
    unsigned int full_addr =addr+i*PTP_ADDR_ADJ;
    retVal = ptp_writecheck_reg(port, full_addr, expect_val);
    if(retVal!=DSDT_OK) return retVal;
  }
/*  for {set i 0} {i < range} {incr i} { */
/*    set full_addr (addr+i] */
/*    set val ptp_read_reg(port, full_addr] */
/*    if(val != expect_val} { */
/*      PTP_DBG_INFO( ("ERROR: memory addr 0x%x value 0x%x does not match expected value 0x%x" full_addr val expect_val] */
/*      incr err */
/*    } */
/*  } */
  if(err != 0) { 
    PTP_DBG_DEMO( ("ERROR: found %x mismatches\n", err));
    return DSDT_FAIL;
  }
  return retVal;
}

/* memory test */
DSDT_STATUS ptp_1step_write_unique_check (int port, unsigned short addr, int range, unsigned int mask) {
  DSDT_STATUS   retVal = DSDT_OK;
  int err = 0;
  int i;

  for ( i=0; i < range; i++) {
    unsigned short full_addr =addr+i*PTP_ADDR_ADJ;
    unsigned int expect_val = (((full_addr<<16)|(full_addr))&mask);
    retVal = ptp_writecheck_reg(port, full_addr, expect_val);
    if(retVal!=DSDT_OK) return retVal;
  }
/*  for {set i 0} {i < range} {incr i} { */
/*    set full_addr (addr+i] */
/*    set expect_val full_addr */
/*    set val ptp_read_reg(port, full_addr] */
/*    if(val != expect_val} { */
/*      PTP_DBG_INFO( ("ERROR: memory addr 0x%x value 0x%x does not match expected value 0x%x" full_addr val expect_val] */
/*      incr err */
/*    } */
/*  } */
  if(err != 0) { 
    PTP_DBG_DEMO( ("ERROR: found %x mismatches\n", err));
    return DSDT_FAIL;
  }
  return retVal;
}

/* udata mem dump */
DSDT_STATUS ptp_1step_mem_dump_update_udata (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int eg_addr =(VN_EG0_PTP+PTP_PORT_OFF(port)+PTP_UMEU_UDATA);

  retVal = ptp_1step_dump_mem(port,  eg_addr, 0x40);
  return retVal;
}

/* udata mem test */
DSDT_STATUS ptp_1step_mem_test_update_udata (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short  eg_addr = (VN_EG0_PTP+PTP_PORT_OFF(port)+PTP_UMEU_UDATA);
  unsigned short  ig_addr = (VN_IG0_PTP+PTP_PORT_OFF(port)+PTP_UMEU_UDATA);

  retVal = ptp_1step_write_zeroes(port, eg_addr, 0x100); 
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_1step_write_unique_check(port, eg_addr, 0x100, 0xfffff);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_1step_write_fs_check(port, eg_addr, 0x100, 0xfffff);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_1step_write_zeroes(port, ig_addr, 0x100); 
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_1step_write_unique_check(port, ig_addr, 0x100, 0xfffff);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_1step_write_fs_check(port, ig_addr, 0x100, 0xfffff);
  return retVal;
}
/* udata mem test */
DSDT_STATUS ptp_1step_mem_test_parser_udata (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short  eg_addr = (VN_EG0_PTP+PTP_PORT_OFF(port)+PTP_PMEU_UDATA);
  unsigned short  ig_addr = (VN_IG0_PTP+PTP_PORT_OFF(port)+PTP_PMEU_UDATA); 

  retVal = ptp_1step_write_zeroes(port, eg_addr, 0x100);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_1step_write_unique_check(port, eg_addr, 0x100, 0xfffff);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_1step_write_fs_check(port, eg_addr, 0x100, 0xfffff);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_1step_write_zeroes(port, ig_addr, 0x100);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_1step_write_unique_check(port, ig_addr, 0x100, 0xfffff);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_1step_write_fs_check(port, ig_addr, 0x100, 0xfffff);
  return retVal;
}

 
PTP_TYPE get_ptp_base_addr_id ( int portnum, PTP_Blk path ) {
   PTP_TYPE base_addr_id;
  int path_igr = (path == PTP_igr)?1:0;
  int path_egr = (path == PTP_egr)?1:0;

  if((portnum < 0) || (portnum > 7)) {
    PTP_DBG_INFO(("bogus port number portnum\n"));
    return -1;
  }
  portnum %= 4;

/*  set path_igr (![string compare -nocase path "igr"]] */
/*  set path_egr (![string compare -nocase path "egr"]] */

  if(path_igr) {
   base_addr_id = IG0_PTP;
  } else if(path_egr) {
   base_addr_id = EG0_PTP;
  } else {
    PTP_DBG_DEMO(("ERROR: path path not recognized\n"));
    return -1;
  }

  base_addr_id = base_addr_id+ 2 * portnum;

  return base_addr_id;
}

/* prints out the ptp registers (portnumber 'portnum') */
DSDT_STATUS get_ptp_regs ( int portnum, PTP_Blk path ) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short my_base_addr;
  
  int index = 0;
  PTP_TYPE base_addr_id;

  PTP_DBG_INFO(("PTP regs (port %x, path %x)\n", portnum, (unsigned int)path));

  base_addr_id = get_ptp_base_addr_id(portnum, path);
  my_base_addr = ptp_1step_base_addr[base_addr_id] + PTP_RF;
  
  index = 0;
#ifdef DEBUG_PTP
/*  foreach reg ptp_rf_names_list { */
  while (ptp_rf_names_list[index]!=0) {
    unsigned short reg_addr = my_base_addr + index*PTP_ADDR_ADJ;
    PTP_DBG_INFO( ("%04x %15s %08x\n", reg_addr, ptp_rf_names_list[index], ptp_read_reg(portnum, reg_addr)));
    index++;
  }
#endif
  return retVal;
}

/* prints out the ptp statistics (portnumber 'portnum' and path 'egr|igr') */
DSDT_STATUS get_ptp_stats ( int portnum, PTP_Blk path ) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short my_base_addr;

/*  set path_igr (![string compare -nocase path "igr"]] */
  int path_igr;
  int index = 0;
  PTP_TYPE base_addr_id;
  char **stats_names_list;

  PTP_DBG_DEMO(("PTP stats (port %x, path %x)\n", portnum, (unsigned int)path));

  /*set base_addr_id [get_ptp_base_addr_id portnum path] */
  /* both egr & igr stats are on the egr space !! */
  base_addr_id = get_ptp_base_addr_id( portnum, path);
  my_base_addr = (VN_EG0_PTP+PTP_PORT_OFF(portnum)+PTP_STATS);

/*  set path_igr (![string compare -nocase path "igr"]] */
  path_igr = (path == PTP_igr)?1:0;
  if(path_igr) {
   stats_names_list = ptp_igr_stats_names_list;
   my_base_addr = my_base_addr + 0x20*PTP_ADDR_ADJ;   ;/* igr stats come right after egr stats */
  } else {
   stats_names_list = ptp_egr_stats_names_list;
  }

  index = 0;
/*  foreach reg stats_names_list { */
#ifdef DEBUG_PTP
  while (stats_names_list[index]!=0) {
    unsigned short reg_addr = my_base_addr + index*PTP_ADDR_ADJ;
    PTP_DBG_DEMO( ("%04x %15s %08x\n", reg_addr, stats_names_list[index], ptp_read_reg(portnum, reg_addr)));
    index++;
  }
#endif
  return retVal;
}

DSDT_STATUS ptp_1step_display_contents (int port, unsigned short addr, int size)  {
  DSDT_STATUS   retVal = DSDT_OK;
#ifdef DEBUG_PTP
  int index;
  for ( index=0; index < size; index++) {
    unsigned short reg_addr = addr + index*PTP_ADDR_ADJ;
    PTP_DBG_DEMO( ("%04x %02d %08x\n", reg_addr, index, ptp_read_reg(port, reg_addr)));
  }
#endif
  return retVal;
}

/* prints out the ptp pam (parser and update) contents (portnumber 'portnum' and path 'egr|igr') */
DSDT_STATUS get_ptp_pam ( int portnum, PTP_Blk path ) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short my_base_addr;

  PTP_TYPE base_addr_id = get_ptp_base_addr_id (portnum, path);

  PTP_DBG_INFO(("PTP Parser PAM (port portnum, path path)\n"));
  my_base_addr = ptp_1step_base_addr[base_addr_id] + PTP_PMEU_PAM;
  ptp_1step_display_contents(portnum, my_base_addr, 32);

  PTP_DBG_INFO(("PTP Update PAM (port %x, path path)\n", portnum));
  my_base_addr = ptp_1step_base_addr[base_addr_id] + PTP_UMEU_PAM;
  ptp_1step_display_contents(portnum, my_base_addr, 32);
  return retVal;
}

/* prints out the ptp lut (key, mask and action) contents (portnumber 'portnum' and path 'egr|igr') */
DSDT_STATUS get_ptp_lut ( int portnum, PTP_Blk path ) {
  DSDT_STATUS   retVal = DSDT_OK;
#ifdef DEBUG_PTP
  unsigned short my_base_addr;

  PTP_TYPE base_addr_id = get_ptp_base_addr_id( portnum, path);

  PTP_DBG_INFO(("PTP LUT key (port portnum, path path)\n"));
  my_base_addr = ptp_1step_base_addr[base_addr_id] + PTP_LUT_KEYMASK;
  ptp_1step_display_contents( portnum, my_base_addr, 32);

  PTP_DBG_INFO(("PTP LUT mask (port %x, path path)\n", portnum));
  my_base_addr = ptp_1step_base_addr[base_addr_id] + PTP_LUT_KEYMASK + 32*PTP_ADDR_ADJ;
  ptp_1step_display_contents( portnum,my_base_addr, 32);

  PTP_DBG_INFO(("PTP LUT action (port portnum, path path)\n"));
  my_base_addr = ptp_1step_base_addr[base_addr_id] + PTP_LUT_ACTION;
  ptp_1step_display_contents(portnum, my_base_addr, 32);
#endif
  return retVal;
}


unsigned long long lut_key_adjust ( PTP_KEY name, unsigned long long value ) {

  unsigned long long dataLong = 0LL;
  dataLong = value&0xffffffffLL;
  return (dataLong << lut_key_pos[name] );
}

unsigned long long lut_vld_adjust ( PTP_KEY name ) {
unsigned long long dataLong = 0LL;
  dataLong = 1LL << lut_key_size[name];
  dataLong = dataLong - 1;
  dataLong = dataLong << lut_key_pos[name];
  return (dataLong);
}

unsigned long long lut_act_adjust ( PTP_1STEP_LUT_ACT name, unsigned long long value ) {
  unsigned long long dataLong = 0LL;
  dataLong = value&0xffffffffLL;
  return (dataLong << lut_act_pos[name] );
}

DSDT_STATUS ptp_lut_clear (int port) {
  DSDT_STATUS   retVal = DSDT_OK;

	int index;

 /* Ingress  */
  for (index = 0; index < 16;  index++) {
    retVal = prgm_lut_entry( port, 0ll, 0ll, 0, 0, 0, index, VN_IG0_PTP+PTP_PORT_OFF(port));
    if(retVal!=DSDT_OK) return retVal;
  }
 /* Egress  */
  for (index = 0; index < 16;  index++) {
    retVal = prgm_lut_entry( port, 0ll, 0ll, 0, 0, 0, index, VN_EG0_PTP+PTP_PORT_OFF(port));
    if(retVal!=DSDT_OK) return retVal;
  }
  return retVal;
}


/* demo LUT */
DSDT_STATUS ptp_lut_init (int port)
{
  DSDT_STATUS   retVal = DSDT_OK;

  retVal = ptp_lut_clear(port);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_lut_default(port);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_lut_interrupt(port);
  if(retVal!=DSDT_OK) return retVal;

#if 0 /* for Y1731 */
  /* overwrite entry 30 in parser MEU PAM with 0x8902(etype for y1731) */
  base_addr_id = get_ptp_base_addr_id( port, PTP_egr);
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_PMEU_PAM + 30*PTP_ADDR_ADJ;
  retVal = ptp_writecheck_reg(port, reg_addr, 0x8902); 
  if(retVal!=DSDT_OK) return retVal;
  base_addr_id = get_ptp_base_addr_id( port, PTP_igr);
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_PMEU_PAM + 30*PTP_ADDR_ADJ;
  retVal = ptp_writecheck_reg(port, reg_addr, 0x8902); 
#endif
  return retVal;
}

DSDT_STATUS ptp_lut_init_ntp (int port)
{
  DSDT_STATUS   retVal = DSDT_OK;
  PTP_TYPE base_addr_id;
  unsigned short reg_addr;

  /*
  ptp_lut_clear(port);
  ptp_lut_default(port);
  ptp_lut_interrupt(port);
  */

  /* overwrite entry 30 in parser MEU PAM with 0x007b (for ntp)  */
  base_addr_id = get_ptp_base_addr_id( port, PTP_egr);
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_PMEU_PAM + 30*PTP_ADDR_ADJ;
  retVal = ptp_writecheck_reg(port, reg_addr, 0x007b); 
  if(retVal!=DSDT_OK) return retVal;
  base_addr_id = get_ptp_base_addr_id( port, PTP_igr);
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_PMEU_PAM + 30*PTP_ADDR_ADJ;
  retVal = ptp_writecheck_reg(port, reg_addr, 0x007b); 
  return retVal;
}

DSDT_STATUS ptp_lut_init_ntp_2 (int port, int udp_src_port, int udp_dst_port)
{
  DSDT_STATUS   retVal = DSDT_OK;
  PTP_TYPE base_addr_id;
  unsigned short reg_addr;

  /*
  ptp_lut_clear(port);
  ptp_lut_default(port);
  ptp_lut_interrupt(port);
  */

  /* overwrite entry 31 in parser MEU PAM with 0x007b (for ntp)  */
  base_addr_id = get_ptp_base_addr_id( port, PTP_egr);
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_PMEU_PAM + 31*PTP_ADDR_ADJ;
  retVal = ptp_writecheck_reg(port, reg_addr, udp_src_port); 
  if(retVal!=DSDT_OK) return retVal;
  base_addr_id = get_ptp_base_addr_id( port, PTP_igr);
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_PMEU_PAM + 31*PTP_ADDR_ADJ;
  retVal = ptp_writecheck_reg(port, reg_addr, udp_dst_port); 
  return retVal;
}


/* init example */


DSDT_STATUS get_ptp_tai (void) {
  DSDT_STATUS   retVal = DSDT_OK;
#ifdef DEBUG_PTP
  unsigned short  my_base_addr = ptp_1step_base_addr[TAI_CFG];
  int index = 0;
int port =0;
  PTP_DBG_INFO(("PTP TAI regs\n"));

/*  foreach reg ptp_tai_regs_list { */
  while (ptp_tai_regs_list_name[index]!=0) {
    unsigned short reg_addr = my_base_addr + index*PTP_ADDR_ADJ;
    PTP_DBG_INFO( ("%04x %15s %08x\n\n", reg_addr, ptp_tai_regs_list_name[index], ptp_read_reg(port, reg_addr)));
    index++;
  }
#endif
  return retVal;
}

DSDT_STATUS get_ptp_tai_tod (int port, unsigned int *_ns, unsigned int *_sec_lo, unsigned int *_sec_hi ) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short my_base_addr = ptp_1step_base_addr[TAI_CFG];
  
  unsigned int  frac;   
  unsigned int  ns     = ptp_read_reg(port, (my_base_addr + REG_TOD_1));
  unsigned int  sec_lo = ptp_read_reg(port, (my_base_addr + REG_TOD_2));
  unsigned int  sec_hi = ptp_read_reg(port, (my_base_addr + REG_TOD_3));

  unsigned long long  sec;
  frac   = ptp_read_reg(port, (my_base_addr + REG_TOD_0));   
  sec =  sec_hi;
  sec =  (sec << 32);
  sec = sec + sec_lo + ns/1000000000;

  PTP_DBG_INFO(("sec_hi:%08x sec_lo:%08x ns:%08x => \n", sec_hi, sec_lo, ns));
  PTP_DBG_INFO((" => seconds sec %lld\n", sec));
  *_ns=ns;
  *_sec_lo=sec_lo;
  *_sec_hi=sec_hi;
  return retVal;
}

DSDT_STATUS _set_ptp_tai_tod (int port, unsigned int ns, unsigned int sec_lo, unsigned int sec_hi ) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short my_base_addr = ptp_1step_base_addr[TAI_CFG];
  unsigned long long sec;

  retVal = ptp_write_reg (port, my_base_addr + REG_TOD_0, 0x0);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_write_reg (port, my_base_addr + REG_TOD_1, ns);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_write_reg (port, my_base_addr + REG_TOD_2, sec_lo);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_write_reg (port, my_base_addr + REG_TOD_3, sec_hi);
  if(retVal!=DSDT_OK) return retVal;

  sec =  sec_hi;
  sec =  (sec << 32);
  sec = sec + sec_lo + ns/1000000000;

  PTP_DBG_INFO(("sec_hi:%08x sec_lo:%08x ns:%08x => seconds sec\n", sec_hi, sec_lo, ns));

  return retVal;
}

DSDT_STATUS get_ptp_tai_frc (int port, unsigned int *_ns, unsigned int *_sec_lo, unsigned int *_sec_hi ) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short my_base_addr = ptp_1step_base_addr[TAI_CFG];
  
  unsigned int  frac;  
  unsigned int  ns     = ptp_read_reg(port, (my_base_addr + REG_FRC_1));
  unsigned int  sec_lo = ptp_read_reg(port, (my_base_addr + REG_FRC_2));
  unsigned int  sec_hi = ptp_read_reg(port, (my_base_addr + REG_FRC_3));

  unsigned long long  sec;
  frac   = ptp_read_reg(port, (my_base_addr + REG_FRC_0));  
  sec =  sec_hi;
  sec =  (sec << 32);
  sec = sec + sec_lo + ns/1000000000;

  PTP_DBG_INFO(("sec_hi:%08x sec_lo:%08x ns:%08x => \n", sec_hi, sec_lo, ns));
  PTP_DBG_INFO((" => seconds sec %lld\n", sec));
  *_ns=ns;
  *_sec_lo=sec_lo;
  *_sec_hi=sec_hi;
  return retVal;
}

DSDT_STATUS set_ptp_tai_frc (int port, unsigned int ns, unsigned int sec_lo, unsigned int sec_hi ) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short my_base_addr = ptp_1step_base_addr[TAI_CFG];
  unsigned long long sec;

  retVal = ptp_write_reg (port, my_base_addr + REG_FRC_0, 0x0);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_write_reg (port, my_base_addr + REG_FRC_1, ns);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_write_reg (port, my_base_addr + REG_FRC_2, sec_lo);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_write_reg (port, my_base_addr + REG_FRC_3, sec_hi);
  if(retVal!=DSDT_OK) return retVal;

  sec =  sec_hi;
  sec =  (sec << 32);
  sec = sec + sec_lo + ns/1000000000;

  PTP_DBG_INFO(("sec_hi:%08x sec_lo:%08x ns:%08x => seconds sec\n", sec_hi, sec_lo, ns));

  return retVal;
}

DSDT_STATUS get_ptp_tai_drift_adjust (int port, long *adj)
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short my_base_addr = ptp_1step_base_addr[TAI_CFG];
  /* drift adjustment, default is 0 */
  unsigned short reg_addr = my_base_addr + REG_DRIFT_ADJ_CFG;
  *adj   = ptp_read_reg(port, reg_addr);
  return retVal;
}

DSDT_STATUS set_ptp_tai_drift_adjust (int port, long adj)
{
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short my_base_addr = ptp_1step_base_addr[TAI_CFG];
  /* drift adjustment, default is 0 */
  unsigned short reg_addr = my_base_addr + REG_DRIFT_ADJ_CFG;
  retVal = ptp_writecheck_reg(port, reg_addr, adj); 
  return retVal;
}

DSDT_STATUS get_ptp_tai_step (int port, long *nanosec, long *fracnanosec) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short my_base_addr = ptp_1step_base_addr[TAI_CFG];
  /* nanosecond, default is 8 */
  unsigned short reg_addr = my_base_addr + REG_TOD_STEP_NS;
  *nanosec   = ptp_read_reg(port, reg_addr);
  /* fraction of nanoseconds, default is 0 */
  reg_addr = my_base_addr + REG_TOD_STEP_FS;
  *fracnanosec   = ptp_read_reg(port, reg_addr);
  return retVal;
}

DSDT_STATUS set_ptp_tai_step (int port, long nanosec, long fracnanosec) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short my_base_addr = ptp_1step_base_addr[TAI_CFG];
  /* nanosecond, default is 8 */
  unsigned short reg_addr = my_base_addr + REG_TOD_STEP_NS;
  retVal = ptp_writecheck_reg(port, reg_addr, nanosec); 
  /* fraction of nanoseconds, default is 0 */
  reg_addr = my_base_addr + REG_TOD_STEP_FS;
  retVal = ptp_writecheck_reg(port, reg_addr, fracnanosec);
  return retVal;
}

int get_ptp_tsql_usage ( int portnum,  PTP_Blk path ) {
  unsigned short reg_addr;
  unsigned int result;
  
  PTP_TYPE base_addr_id;

  PTP_DBG_INFO(( "PTP timestamp queue low usage (port %x, path %x)\n",portnum, (unsigned int)path));

  base_addr_id = get_ptp_base_addr_id( portnum, path);
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_RF + PTP_TS_USAGE;
  
  result = ptp_read_reg (portnum, reg_addr) & 0xffff;

  PTP_DBG_INFO(("tsql usage %04x %04x (%x)\n", reg_addr, result, result));

  return result;
}

int get_ptp_tsqh_usage ( int portnum,  PTP_Blk path ) {
   PTP_TYPE base_addr_id;
  unsigned short reg_addr;
  unsigned int result;

  PTP_DBG_INFO(( "PTP timestamp queue high usage port %x path %x\n", portnum, (unsigned int)path));
  base_addr_id = get_ptp_base_addr_id( portnum, path);
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_RF + PTP_TS_USAGE;

/*  set base_addr_id [get_ptp_base_addr_id $portnum $path] */
/*  set reg_addr [expr $base_addr($base_addr_id) + $ptp_addr(RF) + [lsearch $ptp_rf_names_list "ts_usage"]] */

  result = (ptp_read_reg(portnum, reg_addr)>>16) & 0xffff;

  PTP_DBG_INFO(("tsqh usage %04x %04x (%x)\n", reg_addr, result, result));

  return result;
}


static DSDT_STATUS getPTPTsQueue(PtpTsQueueStruct *tsQueue, unsigned short *buf)
{
   DSDT_STATUS   retVal = DSDT_OK;
   tsQueue->seqId = *(buf + 5);
    tsQueue->domainNum  = (*(buf + 6)>>8)&0xff;
    tsQueue->transpSpec  = (*(buf + 6)>>4)&0xf;
    tsQueue->msgId  = (*(buf + 6))&0xf;
    tsQueue->ts.extTs.nanoseconds = *(buf + 0);
    tsQueue->ts.extTs.nanoseconds <<= 16;
    tsQueue->ts.extTs.nanoseconds |= *(buf + 1);
    tsQueue->ts.extTs.topseconds = *(buf + 2);
    tsQueue->ts.extTs.seconds = *(buf + 3);
    tsQueue->ts.extTs.seconds <<= 16;
    tsQueue->ts.extTs.seconds |= *(buf + 4);
  return retVal;
}
DSDT_STATUS displayPTPTsQueue(PtpTsQueueStruct *tsQueue)
{
  DSDT_STATUS   retVal = DSDT_OK;
    PTP_DBG_DEMO(("Get PTP TS Queue:\n"));
    PTP_DBG_DEMO(("sequence Id  %x\n", tsQueue->seqId));
    PTP_DBG_DEMO(("domain Number  %x\n", tsQueue->domainNum));
    PTP_DBG_DEMO(("transport Specific  %x\n", tsQueue->transpSpec));
    PTP_DBG_DEMO(("message Id  %x\n", tsQueue->msgId));
    PTP_DBG_DEMO(("ts.extTs.nanoseconds %x\n", tsQueue->ts.extTs.nanoseconds));
    PTP_DBG_DEMO(("ts.extTs.topseconds %x\n", tsQueue->ts.extTs.topseconds));
    PTP_DBG_DEMO(("ts.extTs.seconds %x\n", tsQueue->ts.extTs.seconds));
  return retVal;
}


DSDT_STATUS ptp_get_tsql_part ( int portnum,  PTP_Blk path, int count, PtpTsQueueStruct *tsQueue)
{
  DSDT_STATUS   retVal = DSDT_OK;
  PTP_TYPE base_addr_id;
  unsigned short reg_addr;
  int j;
  unsigned short tsQuBuf[8];
/*   PtpTsQueueStruct ptpTsQueue; */

  PTP_DBG_TRACE(("PTP get queue low data port %x, path %x\n", portnum, path));

  base_addr_id = get_ptp_base_addr_id( portnum, path);
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_RF + PTP_TSQL_DATA;

  for(j=0; j<count; j++)
  {
    tsQuBuf[j]=(unsigned short)(ptp_read_reg(portnum, reg_addr)&0xffff);
    PTP_DBG_VERB(("ptp_get_tsql: reg %04x = %04x\n", reg_addr, tsQuBuf[j]));
  }
  getPTPTsQueue(tsQueue, tsQuBuf);        
/* test */
/*    displayPTPTsQueue(tsQueue);         */
  return retVal;
}

DSDT_STATUS ptp_get_tsqh_part ( int portnum,  PTP_Blk path, int count, PtpTsQueueStruct *tsQueue)
{
  DSDT_STATUS   retVal = DSDT_OK;
  PTP_TYPE base_addr_id;
  unsigned short reg_addr;
  int j;
  unsigned short tsQuBuf[8];
/*   PtpTsQueueStruct ptpTsQueue; */

  PTP_DBG_TRACE(("PTP get queue high data port %x, path %x\n", portnum, path));

  base_addr_id = get_ptp_base_addr_id( portnum, path);
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_RF + PTP_TSQH_DATA;

  for(j=0; j<count; j++)
  {
    tsQuBuf[j]=(unsigned short)(ptp_read_reg(portnum, reg_addr)&0xffff);
    PTP_DBG_VERB(("ptp_get_tsqh: reg %04x = %04x\n", reg_addr, tsQuBuf[j]));
  }
  getPTPTsQueue(tsQueue, tsQuBuf);        
/* test */
/*    displayPTPTsQueue(tsQueue);         */
  return retVal;
}

DSDT_STATUS ptp_get_tsql ( int portnum,  PTP_Blk path, PtpTsQueueStruct *tsQueue)
{
  return ptp_get_tsql_part(portnum, path, 7, tsQueue);
}

DSDT_STATUS ptp_get_tsqh ( int portnum,  PTP_Blk path, PtpTsQueueStruct *tsQueue)
{
  return ptp_get_tsqh_part(portnum, path, 7, tsQueue);
}

DSDT_STATUS get_ptp_tsql_data ( int portnum,  PTP_Blk path, int numhwords){
  DSDT_STATUS   retVal = DSDT_OK;
   PTP_TYPE base_addr_id;
  unsigned short reg_addr;
  int i, j;
int port =0;
  PtpTsQueueStruct ptpTsQueue;
  unsigned short tsQuBuf[16];

  PTP_DBG_DEMO(( "PTP timestamp queue low data port %x , path %x , hwords %x\n", portnum, path, numhwords));

/*  set base_addr_id [get_ptp_base_addr_id $portnum $path] */
/*  set reg_addr [expr $base_addr($base_addr_id) + $ptp_addr(RF) + [lsearch $ptp_rf_names_list "ts_tsql_data"]] */
  base_addr_id = get_ptp_base_addr_id( portnum, path);
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_RF + PTP_TSQL_DATA;

  for ( i=0; i < numhwords; i+=7) {
	for(j=0; j<7; j++)
	{
      tsQuBuf[j]=(unsigned short)(ptp_read_reg(port, reg_addr)&0xffff);
      PTP_DBG_INFO(("%04x %02d %04x\n", reg_addr, i,  tsQuBuf[j]));
	}
    getPTPTsQueue(&ptpTsQueue, tsQuBuf);        
    displayPTPTsQueue(&ptpTsQueue);        
  }
  return retVal;
}

DSDT_STATUS get_ptp_tsqh_data ( int portnum,  PTP_Blk path, int numhwords){
  DSDT_STATUS   retVal = DSDT_OK;
   PTP_TYPE base_addr_id;
  unsigned short reg_addr;
  int i, j;
int port =0;
  PtpTsQueueStruct ptpTsQueue;
  unsigned short tsQuBuf[16];

  PTP_DBG_INFO(( "PTP timestamp queue high data port %x , path %x , hwords %x\n", portnum, path, numhwords));

/*  set base_addr_id [get_ptp_base_addr_id $portnum $path] */
/*  set reg_addr [expr $base_addr($base_addr_id) + $ptp_addr(RF) + [lsearch $ptp_rf_names_list "ts_tsqh_data"]] */
  base_addr_id = get_ptp_base_addr_id( portnum, path);
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_RF + PTP_TSQH_DATA;

  for ( i=0; i < numhwords; i+=7) {
	for(j=0; j<7; j++)
	{
      tsQuBuf[j]=(unsigned short)(ptp_read_reg(port, reg_addr)&0xffff);
      PTP_DBG_INFO(("%04x %02d %04x\n", reg_addr, i,  tsQuBuf[j]));
	}
    getPTPTsQueue(&ptpTsQueue, tsQuBuf);        
    displayPTPTsQueue(&ptpTsQueue);        
  }
  return retVal;
}

DSDT_STATUS get_ptp_tsqh_drain ( int portnum,  PTP_Blk path ) {
  DSDT_STATUS   retVal = DSDT_OK;
  int numhwords = get_ptp_tsqh_usage(portnum, path);
  /*get_ptp_tsqh_data $portnum $path [expr $numhwords/2] */
  get_ptp_tsqh_data(portnum, path, numhwords);
  return retVal;
}

DSDT_STATUS get_ptp_tsql_drain ( int portnum,  PTP_Blk path ) {
  DSDT_STATUS   retVal = DSDT_OK;
  int numhwords = get_ptp_tsql_usage(portnum, path);
  /*get_ptp_tsql_data $portnum $path [expr $numhwords/2] */
  get_ptp_tsql_data(portnum, path, numhwords);
  return retVal;
}

DSDT_STATUS get_ptp_latadj (int portnum ) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int latadj_low, latadj_high;
/*  set base_addr_id [get_ptp_base_addr_id $portnum EGR] */
   PTP_TYPE base_addr_id;
  unsigned short reg_addr;
int port =0;

  PTP_DBG_INFO(( "PTP Latency Adjustment (port %x \n", portnum));
/*  set reg_addr [expr $base_addr($base_addr_id) + $ptp_addr(UMEU_PAM) + 27] */
  base_addr_id = get_ptp_base_addr_id( portnum, PTP_egr);
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_UMEU_PAM + 27*PTP_ADDR_ADJ;

  latadj_low = ptp_read_reg (port, reg_addr);
/*  set reg_addr [expr $base_addr($base_addr_id) + $ptp_addr(UMEU_PAM) + 28] */
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_UMEU_PAM + 28*PTP_ADDR_ADJ;
  latadj_high = ptp_read_reg (port, reg_addr);
  PTP_DBG_INFO(( "%08x", (latadj_high << 16)|latadj_low));
  return retVal;
}

DSDT_STATUS set_ptp_latadj (int portnum, int ns ) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short latadj_low, latadj_high;
   PTP_TYPE base_addr_id;
  unsigned short reg_addr;

/*  set base_addr_id [get_ptp_base_addr_id $portnum EGR] */
  base_addr_id = get_ptp_base_addr_id( portnum, PTP_egr);
  latadj_low = ns & 0xffff;
  latadj_high = (ns >> 16) & 0xffff;
/*  set reg_addr [expr $base_addr($base_addr_id) + $ptp_addr(UMEU_PAM) + 27] */
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_UMEU_PAM + 27*PTP_ADDR_ADJ;
/*  writecheck_req $reg_addr $latadj_low */
  retVal = ptp_writecheck_reg(portnum, reg_addr, latadj_low);
  if(retVal!=DSDT_OK) return retVal;
/*  set reg_addr [expr $base_addr($base_addr_id) + $ptp_addr(UMEU_PAM) + 28] */
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_UMEU_PAM + 28*PTP_ADDR_ADJ;
/*  writecheck_req $reg_addr $latadj_high */
  retVal = ptp_writecheck_reg(portnum, reg_addr, latadj_high);
  return retVal;
}

DSDT_STATUS get_ptp_stt_timer (int portnum ) {
  DSDT_STATUS   retVal = DSDT_OK;
   PTP_TYPE base_addr_id;
  unsigned short reg_addr;

/*  set base_addr_id [get_ptp_base_addr_id $portnum EGR] */
  base_addr_id = get_ptp_base_addr_id( portnum, PTP_egr);

  PTP_DBG_INFO(( "PTP STT_Timer port %x\n", portnum));
/*  set reg_addr [expr $base_addr($base_addr_id) + $ptp_addr(RF) + [lsearch $ptp_rf_names_list "stt_ctl"]] */
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_RF + PTP_STT_CTL;
  PTP_DBG_INFO(("%04x %08x\n", reg_addr, ptp_read_reg(portnum, reg_addr)&0xffff));
  return retVal;
}

DSDT_STATUS set_ptp_stt_timer (int portnum, unsigned int cycles) {
  DSDT_STATUS   retVal = DSDT_OK;
   PTP_TYPE base_addr_id;
  unsigned short reg_addr;

/*  set base_addr_id [get_ptp_base_addr_id $portnum EGR] */
  base_addr_id = get_ptp_base_addr_id( portnum, PTP_egr);
/*  set reg_addr [expr $base_addr($base_addr_id) + $ptp_addr(RF) + [lsearch $ptp_rf_names_list "stt_ctl"]] */
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_RF + PTP_STT_CTL;
/*  writecheck_req $reg_addr $cycles */
  retVal = ptp_writecheck_reg(portnum, reg_addr, cycles);
  return retVal;
}


DSDT_STATUS get_ptp_disp_timer (int portnum ) {
  DSDT_STATUS   retVal = DSDT_OK;
   PTP_TYPE base_addr_id;
  unsigned short reg_addr;

/*  set base_addr_id [get_ptp_base_addr_id $portnum EGR] */
  base_addr_id = get_ptp_base_addr_id( portnum, PTP_egr);

  PTP_DBG_INFO(("PTP DISP_Timer (port %x)\n", portnum));
/*  set reg_addr [expr $base_addr($base_addr_id) + $ptp_addr(RF) + [lsearch $ptp_rf_names_list "disp"]] */
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_RF + PTP_DISP;
  PTP_DBG_INFO(("%04x %08x\n", reg_addr, ptp_read_reg(portnum, reg_addr)&0xffff));
  return retVal;
}


DSDT_STATUS set_ptp_disp_timer (int portnum, unsigned int cycles) {
  DSDT_STATUS   retVal = DSDT_OK;
   PTP_TYPE base_addr_id;
  unsigned short reg_addr;


/*  set base_addr_id [get_ptp_base_addr_id $portnum EGR] */
  base_addr_id = get_ptp_base_addr_id( portnum, PTP_egr);

/*  set reg_addr [expr $base_addr($base_addr_id) + $ptp_addr(RF) + [lsearch $ptp_rf_names_list "disp"]] */
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_RF + PTP_DISP;
/*  retVal = ptp_writecheck_reg $reg_addr $cycles */
  retVal = ptp_writecheck_reg(portnum,reg_addr, cycles);
  return retVal;
}

DSDT_STATUS get_ptp_stuff (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
	get_ptp_stats( port, PTP_igr);
	get_ptp_stats (port, PTP_egr);
	get_ptp_tsql_drain (port, PTP_igr);
	get_ptp_tsqh_drain (port, PTP_igr);
	get_ptp_tsql_drain (port, PTP_egr);
	get_ptp_tsqh_drain (port, PTP_egr);
  return retVal;
}

DSDT_STATUS ptp_lut_update (int port, 
							unsigned long long delete_lut_key , 
                     unsigned long long delete_lut_vld,
                     unsigned long long delete_lut_act,
                     unsigned long long add_lut_key , 
                     unsigned long long add_lut_vld,
                     unsigned long long add_lut_act,
					 short lut_index, int path ) {
  DSDT_STATUS   retVal = DSDT_OK;
 unsigned long long tmp_lut_key;
 unsigned long long tmp_lut_vld;
 unsigned long tmp_lut_key_top;
 unsigned long tmp_lut_vld_top;
 unsigned long long tmp_lut_act; 
	
  read_lut_entry( port, &tmp_lut_key, &tmp_lut_vld, &tmp_lut_key_top, &tmp_lut_vld_top, &tmp_lut_act, lut_index, path);
  PTP_DBG_INFO((">>> read lut entry : key %x:%llx  vld %x:%llx  act %llx\n", 
		tmp_lut_key_top, tmp_lut_key, tmp_lut_vld_top, tmp_lut_vld, tmp_lut_act));
  PTP_DBG_INFO((">>> delete lut entry : key %llx  vld %llx  act %llx\n", 
		delete_lut_key, delete_lut_vld, delete_lut_act));
  PTP_DBG_INFO((">>> add lut entry : key %llx  vld %llx  act %llx\n",
		add_lut_key, add_lut_vld, add_lut_act));
  tmp_lut_key &= ~delete_lut_key;
  tmp_lut_vld &= ~delete_lut_vld;
  tmp_lut_act &= ~delete_lut_act;
  tmp_lut_key |= add_lut_key;
  tmp_lut_vld |= add_lut_vld;
  tmp_lut_act |= add_lut_act;
  PTP_DBG_INFO((">>> updated lut entry : key %x:%llx  vld %x:%llx  act %llx\n", 
		tmp_lut_key_top, tmp_lut_key, tmp_lut_vld_top, tmp_lut_vld, tmp_lut_act));
  prgm_lut_entry( port, tmp_lut_key, tmp_lut_vld, tmp_lut_key_top, tmp_lut_vld_top, tmp_lut_act, lut_index, path);
  return retVal;
}

/* Set PTP register */
DSDT_STATUS set_ptp_tai_reg (int port, PTP_1STEP_TAI_REGS regname, unsigned int value ) {
  DSDT_STATUS   retVal = DSDT_OK;
/*  set reg_addr [lsearch $ptp_tai_regs_list $regname] */
  unsigned int reg_addr = regname;
  unsigned int my_base_addr;
  if (reg_addr < 0) {
    PTP_DBG_INFO(( "Error: register %x not found", (unsigned int)regname));
  } 
  else {
    my_base_addr = ptp_1step_base_addr[TAI_CFG]+ reg_addr;
    retVal = ptp_write_reg(port,  my_base_addr, value);
  }
  return retVal;
}

/* Set PTP register */
DSDT_STATUS get_ptp_tai_reg (int port, PTP_1STEP_TAI_REGS regname, unsigned int *value) {
  DSDT_STATUS   retVal = DSDT_OK;
/*  set reg_addr [lsearch $ptp_tai_regs_list $regname] */
  unsigned int reg_addr = regname;
  unsigned int my_base_addr;
  if (reg_addr < 0) {
    PTP_DBG_INFO(( "Error: register %x not found", (unsigned int)regname));
  } else {
    my_base_addr = ptp_1step_base_addr[TAI_CFG]+ reg_addr;
    PTP_DBG_INFO(( " address is %x\n", my_base_addr));
    *value = ptp_read_reg(port,  my_base_addr);
  }
  return retVal;
}


/* set drift adjustment (frac nsec) */
DSDT_STATUS set_drift_adj (int port, unsigned int frac_ns) {
  DSDT_STATUS   retVal = DSDT_OK;
  set_ptp_tai_reg(port,  REG_DRIFT_ADJ_CFG, frac_ns); 
  return retVal;
}
/* get drift adjustment (frac nsec) */
DSDT_STATUS get_drift_adj (int port, unsigned int *frac_ns) {
  DSDT_STATUS   retVal = DSDT_OK;
  get_ptp_tai_reg(port,  REG_DRIFT_ADJ_CFG, frac_ns);
  return retVal;
}
/* set drift adjustment via step size (frac nsec) */
DSDT_STATUS set_drift_adj_step (int port, unsigned int frac_ns) {
  DSDT_STATUS   retVal = DSDT_OK;
  set_ptp_tai_reg(port, REG_TOD_STEP_FS, frac_ns);
  return retVal;
}
/* get drift adjustment via step size (frac nsec) */
DSDT_STATUS get_drift_adj_step (int port, unsigned int *frac_ns) {
  DSDT_STATUS   retVal = DSDT_OK;
  get_ptp_tai_reg(port, REG_TOD_STEP_FS, frac_ns);
  return retVal;
}

/* Clear ELU entry */
DSDT_STATUS clr_elu_entry (int portnum, int num) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int  base = PTP_PORT_OFF(portnum) + 0x100 + 8*num*PTP_ADDR_ADJ;
  base = base + 4;
  retVal = ptp_write_reg(portnum,  base, 0);
  return retVal;
}

/* Clear ILU entry */
DSDT_STATUS clr_ilu_entry (int portnum, int num)  {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int  base = PTP_PORT_OFF(portnum) + 0x200 + 8*num*PTP_ADDR_ADJ;
  retVal = ptp_write_reg(portnum,  base, 0);
  if(retVal!=DSDT_OK) return retVal;
  base++;
  retVal = ptp_write_reg(portnum,  base, 0);
  if(retVal!=DSDT_OK) return retVal;
  base = PTP_PORT_OFF(portnum) + 0x200 + 8*num*PTP_ADDR_ADJ + 6*num*PTP_ADDR_ADJ;
  retVal = ptp_write_reg(portnum,  base, 0);
  if(retVal!=DSDT_OK) return retVal;
  base++;
  retVal = ptp_write_reg(portnum,  base, 0);
  return retVal;
}

/* programs igr replay table entry to 1 */
DSDT_STATUS set_igr_rply (int portnum, int num)  {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int  base = PTP_PORT_OFF(portnum) + 0x80*num*PTP_ADDR_ADJ + num*PTP_ADDR_ADJ;
  retVal = ptp_write_reg(portnum,  base, 1);
  return retVal;
}

DSDT_STATUS disable_macsec_both (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
	clr_elu_entry(port, 0);
	clr_ilu_entry(port, 0);
	set_igr_rply(port, 0);
  return retVal;
}

/* TAI / TOD */

/* Get PTP time register  */
/* Valid regname is one of: tod_0, frc_0, trig_gen_tod_0, trig_gen_msk_0, tod_load_0, tod_cap_0, tod_cap1_0, next_rt_0 */
DSDT_STATUS get_ptp_tai_timereg (int port, PTP_1STEP_TAI_REGS regname,unsigned int *sec_hi,unsigned int *sec_lo,unsigned int *ns, unsigned int *fs) {
  DSDT_STATUS   retVal = DSDT_OK;
/*  set my_base_addr [expr $base_addr(TAI_CFG)+ [lsearch $ptp_tai_regs_list $regname] ] */
  unsigned int my_base_addr = ptp_1step_base_addr[TAI_CFG]+regname;
  unsigned long long  sec;
  
/*  set frac   [read_reg [expr $my_base_addr + 0]] */
/*  set ns     [read_reg [expr $my_base_addr + 1]] */
/*  set sec_lo [read_reg [expr $my_base_addr + 2]] */
/*  set sec_hi [read_reg [expr $my_base_addr + 3]] */
  *fs  =ptp_read_reg(port, my_base_addr + 0); 
  *ns    =ptp_read_reg(port, my_base_addr + 1*PTP_ADDR_ADJ);
  *sec_lo  =ptp_read_reg(port, my_base_addr + 2*PTP_ADDR_ADJ);
  *sec_hi  =ptp_read_reg(port, my_base_addr + 3*PTP_ADDR_ADJ);

/*  set sec [expr  [expr $sec_hi << 32] + $sec_lo + $ns/10e9] */
  sec =  *sec_hi;
  sec =  (sec << 32);
  sec = sec + *sec_lo + *ns/1000000000;

/*  PTP_DBG_INFO(("$regname: sec_hi:%08x sec_lo:%08x ns:%08x => seconds $sec" $sec_hi $sec_lo $ns] */
  PTP_DBG_INFO(("Reg: %x: sec_hi:%08x sec_lo:%08x ns:%08x fs:%08x => \n", regname, *sec_hi, *sec_lo, *ns, *fs));
  PTP_DBG_INFO((" => seconds sec %lld\n", sec)); 
  return retVal;
}
/* Set PTP time register  */
/* Valid regname is one of: tod_0, frc_0, trig_gen_tod_0, trig_gen_msk_0, tod_load_0, tod_cap_0, tod_cap1_0, next_rt_0 */
DSDT_STATUS set_ptp_tai_timereg (int port, PTP_1STEP_TAI_REGS regname,unsigned int sec_hi, unsigned int sec_lo,unsigned int ns, unsigned int fs) {
  DSDT_STATUS   retVal = DSDT_OK;
/*  set my_base_addr [expr $base_addr(TAI_CFG)+ [lsearch $ptp_tai_regs_list $regname] ] */
  unsigned int my_base_addr = ptp_1step_base_addr[TAI_CFG]+regname;
  unsigned long long  sec;

/*  retVal = ptp_write_reg(port,  [expr $my_base_addr + 0] 0  */
/*  retVal = ptp_write_reg(port,  [expr $my_base_addr + 1] $ns */
/*  retVal = ptp_write_reg(port,  [expr $my_base_addr + 2] $sec_lo */
/*  retVal = ptp_write_reg(port,  [expr $my_base_addr + 3] $sec_hi */
  retVal = ptp_write_reg (port, my_base_addr+0, fs);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_write_reg (port, my_base_addr+1*PTP_ADDR_ADJ, ns);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_write_reg (port, my_base_addr+2*PTP_ADDR_ADJ, sec_lo);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_write_reg (port, my_base_addr+3*PTP_ADDR_ADJ, sec_hi);
  if(retVal!=DSDT_OK) return retVal;

/*  set sec [expr  [expr $sec_hi << 32] + $sec_lo + $ns/10e9] */
  sec =  sec_hi;
  sec =  (sec << 32);
  sec = sec + sec_lo + ns/1000000000;

  PTP_DBG_INFO(("sec_hi:%08x sec_lo:%08x ns:%08x fs:%08x => seconds", sec_hi, sec_lo, ns, fs));
  return retVal;
}


/* return value with modified field  */
unsigned int tai_tod_cfg_gen_set_field (unsigned int oldvalue, int field, unsigned int value) {
/*  PTP_DBG_INFO(( "tai_tod_cfg_gen_val" */

/*   mask = ((1 << tai_tod_cfg_gen_sz[field])-1) << tai_tod_cfg_gen_pos[field]] ; */
  unsigned int mask = ((1 << tai_tod_cfg_gen_sz[field])-1) << tai_tod_cfg_gen_pos[field];
  unsigned int adjvalue = value << tai_tod_cfg_gen_pos[field];
  return ((oldvalue&~mask)|(adjvalue&mask));
}

/* return value extracted from vector */
unsigned int tai_tod_cfg_gen_extract(int field, int vector) {

  unsigned int mask = ((1 << tai_tod_cfg_gen_sz[field])-1);
  return ((vector >> tai_tod_cfg_gen_pos[field])&mask);
}



/* return value with modified field  */
unsigned int tai_tod_func_cfg_set_field(unsigned int oldvalue, int field, unsigned int value) {
  unsigned int mask = ((1 << tai_tod_func_cfg_sz[field])-1) << tai_tod_func_cfg_pos[field];
  unsigned int adjvalue = value << tai_tod_func_cfg_pos[field];
  PTP_DBG_INFO(("tai_tod_func_cfg_val"));

  return((oldvalue&~mask)|(adjvalue&mask));
}

/* return value extracted from vector */
unsigned int tai_tod_func_cfg_extract(int field, int vector) {

  unsigned int mask = (1 << tai_tod_func_cfg_sz[field])-1;
  return((vector >> tai_tod_func_cfg_pos[field])&mask);
}

/* Show TAI TOD cfg gen */


/* ------------------------------------------------------------------------------------ zzz */
DSDT_STATUS set_tai_pulse_in_cnt (int port, unsigned int val) {
  DSDT_STATUS   retVal = DSDT_OK;
/*  set addr [expr $base_addr(TAI_CFG)+ [lsearch $ptp_tai_regs_list pulse_in_cnt] ] */
/*   unsigned int addr = ptp_1step_base_addr[TAI_CFG]+ pulse_in_cnt; */
  unsigned int addr = PULSE_IN_CNT;
/*   writecheck_reg addr val */
  retVal = ptp_writecheck_reg(port, addr, val);
  return retVal;
}

DSDT_STATUS get_tai_pulse_in_cnt (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
#ifdef DEBUG_PTP
/*  set addr [expr $base_addr(TAI_CFG)+ [lsearch $ptp_tai_regs_list pulse_in_cnt] ] */
/*   unsigned int addr = ptp_1step_base_addr[TAI_CFG]+ pulse_in_cnt; */
  unsigned int addr = PULSE_IN_CNT;
  unsigned int cnt = ptp_read_reg (port, addr);
  PTP_DBG_INFO(("%08h (%x)", cnt, cnt));
#endif
  return retVal;
}

DSDT_STATUS set_tai_pulse_delay (int port, unsigned int val) {
  DSDT_STATUS   retVal = DSDT_OK;
/*  set addr [expr $base_addr(TAI_CFG)+ [lsearch $ptp_tai_regs_list pulse_delay] ] */
/*   unsigned int addr = ptp_1step_base_addr[TAI_CFG]+ pulse_delay; */
  unsigned int addr = PULSE_DELAY;
/*  unsigned int cnt = ptp_read_reg (port, addr); */
/*   writecheck_reg addr val */
  retVal = ptp_writecheck_reg(port, addr, val);
  return retVal;
}


DSDT_STATUS hard_reset (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  retVal = ptp_writecheck_reg(port, 22*PTP_ADDR_ADJ, 4);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_writecheck_reg(port, 27*PTP_ADDR_ADJ, 0x8000);
  return retVal;
}
DSDT_STATUS msec_reset (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int data;
  data = ptp_read_reg(port, 13*PTP_ADDR_ADJ);
  retVal = ptp_writecheck_reg(port, 22*PTP_ADDR_ADJ, 253);
  if(retVal!=DSDT_OK) return retVal;
  msec_bits_write(&data, 15, 14, 0x3);
  retVal = ptp_writecheck_reg(port, 13*PTP_ADDR_ADJ, data);
  if(retVal!=DSDT_OK) return retVal;
  msec_bits_write(&data, 15, 14, 0x2);
  retVal = ptp_writecheck_reg(port, 13*PTP_ADDR_ADJ, data);
  return retVal;
}

DSDT_STATUS ptp_lut_default (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_top=0;
  unsigned long lut_vld_top=0;
  unsigned long long lut_act; 

  /* Ingress */
  /* Default => do nothing */

  lut_key = lut_key_adjust( K_AlwaysZero, 0);
  lut_vld = lut_vld_adjust(K_AlwaysZero);
  lut_act = 0x0;
  retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_top, lut_vld_top, lut_act, 15, VN_IG0_PTP+PTP_PORT_OFF(port));
  if(retVal!=DSDT_OK) return retVal;

  /* Egress */
  /* Default => do nothing */
  lut_key = lut_key_adjust( K_AlwaysZero, 0);
  lut_vld = lut_vld_adjust(K_AlwaysZero);
  lut_act = 0x0;
  retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_top, lut_vld_top, lut_act, 15, VN_EG0_PTP+PTP_PORT_OFF(port));
  return retVal;
}

/* entry 14 */
/* if parser is interrupted => do nothing */
DSDT_STATUS ptp_lut_interrupt (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned long long lut_key;
  unsigned long long lut_vld;
  unsigned long lut_key_top=0;
  unsigned long lut_vld_top=0;
  unsigned long long lut_act; 

  /* Ingress */
  lut_key = lut_key_adjust( K_AlwaysZero, 1);
  lut_vld = lut_vld_adjust(K_AlwaysZero);
  lut_act = 0x0;
  retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_top, lut_vld_top, lut_act, 14, VN_IG0_PTP+PTP_PORT_OFF(port));
  if(retVal!=DSDT_OK) return retVal;

  /* Egress */
  /* Default => do nothing */
  lut_key = lut_key_adjust( K_AlwaysZero, 1);
  lut_vld = lut_vld_adjust(K_AlwaysZero);
  lut_act = 0x0;
  retVal = prgm_lut_entry( port, lut_key, lut_vld, lut_key_top, lut_vld_top, lut_act, 14, VN_EG0_PTP+PTP_PORT_OFF(port));
  return retVal;
}

DSDT_STATUS set_hard_drop_size (int port, unsigned int size) {
  DSDT_STATUS   retVal = DSDT_OK;

  unsigned short regaddr = 0xa*PTP_ADDR_ADJ + PTP_PORT_OFF(port);
  unsigned int data = ptp_read_reg (port, regaddr);
  msec_bits_write(&data, 15, 2, size);
  retVal = ptp_write_reg(port, regaddr, data);
  return retVal;
}


DSDT_STATUS set_tai_ntp_mode (int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  retVal = set_ntp_tod_mode(port, 1);

  retVal = ptp_write_reg(port, 0x2309, 0x22);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_write_reg(port, 0x230a, 0x5c17d04e);
  return retVal;
}

unsigned int get_version_number (int port) {
  unsigned short reg_addr;
  unsigned int data;
  PTP_DBG_INFO(("Version Number"));
  reg_addr = ptp_1step_base_addr[GLOBAL_PTP] + 20*PTP_ADDR_ADJ;
  data = ptp_read_reg (port, reg_addr);
  PTP_DBG_INFO(("%04x %02x", reg_addr, data));
  return data;
}

DSDT_STATUS set_wmac_ipg (int port, unsigned int ipg ) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short addr;
  unsigned int regval;
  addr = port * 0x800 + 0x41;
  regval = ptp_read_reg (port, addr);

  regval = msec_bits_write(&regval, 10, 4, ipg);
  retVal = ptp_write_reg(port, addr, regval);
  if(retVal!=DSDT_OK) return retVal;
  return retVal;
}
DSDT_STATUS set_smac_ipg (int port, unsigned int ipg ) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short addr;
  unsigned int regval;
  addr = port * 0x800 + 0x51;
  regval = ptp_read_reg (port, addr);

  regval = msec_bits_write(&regval, 10, 4, ipg);
  retVal = ptp_write_reg(port, addr, regval);
  if(retVal!=DSDT_OK) return retVal;
  return retVal;
}

unsigned int get_wmac_ipg (int port) {
  unsigned short reg_addr;
  unsigned int data;
  reg_addr = port * 0x800 + 0x41;
  data = ptp_read_reg (port, reg_addr);
  return (msec_bits_get (data, 10, 4));
}

unsigned int get_smac_ipg (int port) {
  unsigned short reg_addr;
  unsigned int data;
  reg_addr = port * 0x800 + 0x51;
  data = ptp_read_reg (port, reg_addr);
  return (msec_bits_get (data, 10, 4));
}

DSDT_STATUS set_wiremac_ipg_10 (int port) {
  return (set_wmac_ipg (port, 10));
}
DSDT_STATUS set_wiremac_ipg_11 (int port) {
  return (set_wmac_ipg (port, 11));
}
DSDT_STATUS set_wiremac_ipg_12 (int port) {
  return (set_wmac_ipg (port, 12));
}

DSDT_STATUS set_sysmac_ipg_11 (int port) {
  return (set_smac_ipg (port, 11));
}
DSDT_STATUS set_sysmac_ipg_12 (int port) {
  return (set_smac_ipg (port, 12));
}










/* tod_func value: 0 - update, 1 -increment, 2- decrement, 3-capture */
/* trig - tod_func_trig, default 0, triggers tod function when set to 1 */
DSDT_STATUS set_tai_tod_func (int port, unsigned int tod_func, unsigned int trig){
  DSDT_STATUS   retVal = DSDT_OK;

  unsigned int addr = TOD_FUNC_CFG;
  unsigned int regval = ptp_read_reg (port, addr);

  regval = tai_tod_func_cfg_set_field(regval, TAI_TOD_TOD_FUNC, tod_func);
  regval = tai_tod_func_cfg_set_field(regval, TAI_TOD_TOD_FUNC_TRIG, trig);

  retVal = ptp_writecheck_reg(port, addr, regval);
  if(retVal!=DSDT_OK) return retVal;

  regval = tai_tod_func_cfg_set_field(regval, TAI_TOD_TOD_FUNC_TRIG, 0);
  retVal = ptp_writecheck_reg(port, addr, regval);
  if(retVal!=DSDT_OK) return retVal;

  ptp_1step_print_tai_tod_func_cfg(port);
  return retVal;
}

/* set tod_cap_cfg bits cap_ */
DSDT_STATUS set_tod_cap_cfg (int port, unsigned int val) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned int addr = TOD_CAP_CFG;
  unsigned int regval = ptp_read_reg (port, addr);

  regval = msec_bits_write(&regval, 3, 2, val & 3);
  retVal = ptp_writecheck_reg(port, addr, regval);
  if(retVal!=DSDT_OK) return retVal;
  return retVal;
}

DSDT_STATUS set_elu_entry_match_da (int portnum, int en, int da) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short base0 = portnum * 0x800 + 0x100 + 8*en*PTP_ADDR_ADJ;
  unsigned int te = (1 << 17) | (1 << 18) | (1 << 28);
  unsigned short base = base0 + 4;
  retVal = ptp_write_reg (portnum, base,te);
  if(retVal!=DSDT_OK) return retVal;

  te = (da & 0xFFFF) << 16;
  base = base0 + 1*PTP_ADDR_ADJ;
  retVal = ptp_write_reg (portnum, base,te);
  if(retVal!=DSDT_OK) return retVal;

  te = (da >> 16) & 0xFFFFFFFF;
  base = base0 + 2*PTP_ADDR_ADJ;
  retVal = ptp_write_reg (portnum, base,te);
  if(retVal!=DSDT_OK) return retVal;

  return retVal;
}

DSDT_STATUS set_egr_def_drop (int port, int flag, int portnum) {
  DSDT_STATUS   retVal = DSDT_OK;
  unsigned short reg = 0x9*PTP_ADDR_ADJ;
  unsigned int data;
  if (flag) {
	data = ptp_read_reg(port, reg);
	data |= 0x1;
    retVal = ptp_write_reg (port, reg,data);
    if(retVal!=DSDT_OK) return retVal;
  } else {
	data = ptp_read_reg(port, reg);
	data &= 0xFFFFFFFE;
    retVal = ptp_write_reg (port, reg,data);
    if(retVal!=DSDT_OK) return retVal;
  }
  return retVal;
}

/* Action abreviation
# multiple actions are defined in a list 
*/
unsigned long long  prgm_ptp_lut_acts (PTP_1STEP_LUT_ACT action) {
 unsigned long long lut_act = 0L; 

 if (action == PTP_Act_No_Action) {
   lut_act = 0L;
 } else {
   lut_act = lut_act | lut_act_adjust(action, 1);
 }
 return lut_act;
}

/* 
The time ctl shall be disabled when PTP is running in 10/100mbps 
so that the watchdog timer does not interrupt the ptp actions. 
Also set_asymmtry, set_link_delay are used to adjust the delay on the link.
*/ 
 
/* watchdog timeout in clocks
 15:8 PPD watchdog timeout
 7:0 PPE watchdog timeout */
DSDT_STATUS set_ptp_timeout_ctrl(int cycles, int portnum) {
  DSDT_STATUS   retVal = DSDT_OK;
   PTP_TYPE base_addr_id;
  unsigned short reg_addr;
  base_addr_id = get_ptp_base_addr_id( portnum, PTP_egr);
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_RF + PTP_TIMEOUT_CTL;
  retVal = ptp_writecheck_reg(portnum, reg_addr, 9);
  if(retVal!=DSDT_OK) return retVal;

  base_addr_id = get_ptp_base_addr_id( portnum, PTP_igr);
  reg_addr = ptp_1step_base_addr[base_addr_id] + PTP_RF + PTP_TIMEOUT_CTL;
  retVal = ptp_writecheck_reg(portnum, reg_addr, 9);
  if(retVal!=DSDT_OK) return retVal;

  return retVal;
}

/* set ingress or egress asymmetry */
DSDT_STATUS set_asymmetry(unsigned int asym, PTP_Blk path, int port) {
  DSDT_STATUS   retVal = DSDT_OK;
   /* Both ingress and egress paths */
  if (path == PTP_igr) {
  retVal = ptp_write_reg(port, (VN_IG0_PTP+PTP_UMEU_PAM+PTP_PORT_OFF(port)+23*PTP_ADDR_ADJ), asym&0xffff);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_write_reg(port, (VN_IG0_PTP+PTP_UMEU_PAM+PTP_PORT_OFF(port)+24*PTP_ADDR_ADJ), asym>>16);
  if(retVal!=DSDT_OK) return retVal;
  } else {
  retVal = ptp_write_reg(port, (VN_EG0_PTP+PTP_UMEU_PAM+PTP_PORT_OFF(port)+23*PTP_ADDR_ADJ), asym&0xffff);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_write_reg(port, (VN_EG0_PTP+PTP_UMEU_PAM+PTP_PORT_OFF(port)+24*PTP_ADDR_ADJ), asym>>16);
  if(retVal!=DSDT_OK) return retVal;
  }
  return retVal;
}
unsigned int get_asymmetry(PTP_Blk path, int port){
  unsigned int  val;

  if (path == PTP_igr) {
    val = ptp_read_reg(port, (VN_IG0_PTP+PTP_UMEU_PAM+PTP_PORT_OFF(port)+23*PTP_ADDR_ADJ));
  } else {
    val = ptp_read_reg(port, (VN_EG0_PTP+PTP_UMEU_PAM+PTP_PORT_OFF(port)+23*PTP_ADDR_ADJ));
  }
  return val;

}
/* set ingress linkdelay */
DSDT_STATUS set_link_delay(int ldelay, int port) {
  DSDT_STATUS   retVal = DSDT_OK;
  retVal = ptp_write_reg(port, (VN_IG0_PTP+PTP_UMEU_PAM+PTP_PORT_OFF(port)+26*PTP_ADDR_ADJ), ldelay&0xffff);
  if(retVal!=DSDT_OK) return retVal;
  retVal = ptp_write_reg(port, (VN_EG0_PTP+PTP_UMEU_PAM+PTP_PORT_OFF(port)+27*PTP_ADDR_ADJ), ldelay>>16);
  if(retVal!=DSDT_OK) return retVal;
  return retVal;
}
