#include <mvlPtpCopyright.h>

/********************************************************************************
* ptp_func.c
* 
* DESCRIPTION:
*       Functions to access platform layer by PTP in MacSec control program.
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

/*
static unsigned short macValList[256];
static unsigned short sysmacValList[256];
*/

void ptp_1step_print_reg (int port, unsigned short addr) {
  PTP_DBG_DEMO(("%x ", ptp_read_reg(port, addr)));
}  

void ptp_1step_print_mac_statistics (int port, unsigned int *macValList){
  int n;
  PTP_DBG_DEMO(("\nprint all MAC statistics counters on dev dev_num.\n"));
  n = port; /*for (n=0; n<1; n++) */
  {
	int macName = 0;
    while(ptp_1step_macNmList[macName]!=0) {
/*	for (name = RxGoodOctLo; name < EndMacName; name++) { */
      PTP_DBG_DEMO( ("port%1d %20s %10d\n", n, ptp_1step_macNmList[macName], macValList[macName]));
	  macName++;
    }
  }
/*  for {set n 4} {n<1} {incr n} { */
  n = port; /*for (n=0; n<1; n++)   */
  {
/*	for (macName = RxGoodOctLo; macName < EndMacName; macName++) { */
    int macName = 0;
    while(ptp_1step_macNmList[macName]!=0) {
/*      val = macValList[macName]; */
      PTP_DBG_DEMO( ("port%1d %20s %10d\n", n, ptp_1step_macNmList[macName], macValList[macName]));
	  macName++;
    }
  }
}

void ptp_1step_print_sysmac_statistics (int port, unsigned int *sysmacValList) {
	MacName macName=0;
  /* for (n=0; n<1; n++) */
  {
/*	for (macName = RxGoodOctLo; macName < EndMacName; macName++) { */
    while(ptp_1step_macNmList[macName]!=0) {
/*      val = sysmacValList[macName]; */
      PTP_DBG_DEMO( ("port%1d %20s %10d\n",port, ptp_1step_macNmList[macName], sysmacValList[macName]));
	  macName++;
    }
  }
}

void get_mac_stats (int port, unsigned int *macValList) {
  ptp_1step_dump_mac_statistics(port, macValList);
  ptp_1step_print_mac_statistics(port, macValList);
}

void get_sysmac_stats (int port, unsigned int *sysmacValList) {
  ptp_1step_dump_sysmac_statistics(port, sysmacValList);
  ptp_1step_print_sysmac_statistics(port, sysmacValList);
}

void ptp_1step_print_rumi_stats (int port) {
  PTP_DBG_DEMO( ("IGR_MISS:    %10d\n", ptp_read_reg(port, 0x294D)));
  PTP_DBG_DEMO( ("IGR_OCT_TOT: %10d\n", ptp_read_reg(port, 0x2949)));
  PTP_DBG_DEMO( ("EGR_MISS:    %10d\n", ptp_read_reg(port, 0x294E)));
  PTP_DBG_DEMO( ("EGR_OCT_TOT: %10d\n", ptp_read_reg(port, 0x294C)));
}

/* Print TOD */
void ptp_1step_print_tod (int port) {
#ifdef DEBUG_PTP
  unsigned short frac  =ptp_read_reg(port, 0x2300);
  unsigned short nano  =ptp_read_reg(port, 0x2301);
  unsigned short sec_lo  =ptp_read_reg(port, 0x2302);
  unsigned short sec_hi  =ptp_read_reg(port, 0x2303);
  PTP_DBG_DEMO( ("port %d sec 0x%04x%08x nano 0x%x frac 0x%x\n", port, sec_hi, sec_lo, nano, frac));
#endif
}
/* Print FRC */
void ptp_1step_print_frc (int port) {
#ifdef DEBUG_PTP
  unsigned short  frac  =ptp_read_reg(port, 0x2304 );
  unsigned short  nano  =ptp_read_reg(port, 0x2305);
  unsigned short  sec_lo  =ptp_read_reg(port, 0x2306);
  unsigned short  sec_hi  =ptp_read_reg(port, 0x2307);
  PTP_DBG_DEMO( ("port %d sec 0x%04x%08x nano 0x%x frac 0x%x\n", port, sec_hi, sec_lo, nano, frac));
#endif
}


/* Show TAI TOD cfg gen */
void ptp_1step_print_tai_tod_func_cfg (int port) {
/*  set addr [expr $base_addr(TAI_CFG)+ [lsearch $ptp_tai_regs_list tod_func_cfg] ] */
/*   unsigned int addr = ptp_1step_base_addr[TAI_CFG]+ tod_func_cfg; */
#ifdef DEBUG_PTP
  unsigned int addr = TOD_FUNC_CFG;
  unsigned int regval = ptp_read_reg (port, addr);

/*   foreach field [array names tai_tod_func_cfg_pos] { */
/*     set field_val [tai_tod_func_cfg_extract field regval); */
  int field = 0;
  PTP_DBG_DEMO(("\n"));
  while (tai_tod_func_cfg_pos[field]!=-1) {
    /* set field_val [tai_tod_cfg_gen_extract field regval] */
    unsigned int field_val = tai_tod_func_cfg_extract( field, regval);
    PTP_DBG_DEMO(("TAI TOD FUNC CFG field %s = 0x%x\n", tai_tod_func_cfg_name[field], field_val));
    field++;
  }
#endif
}
