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


source setPhy.tcl

********************************************************************************/
#include "madSample.h"
#include "mx_api_helper.h"
#include "msec_defines.h"
#include "msecFunc.h"
#include "msecApi.h"
#include "msecTest.h"
#include "setMACsec.h"

int macsec_version;


//MAD_DEV *devp = maddev;
int usbport;
int did_speed;


char * statsNmList_a0 [] ={
	"MX_SEC_A0_IGR_HIT_E0",
    "MX_SEC_A0_IGR_HIT_E1",
    "MX_SEC_A0_IGR_HIT_E2",
 	"MX_SEC_A0_IGR_HIT_E3",
    "MX_SEC_A0_IGR_HIT_E4",
    "MX_SEC_A0_IGR_HIT_E5",
	"MX_SEC_A0_IGR_HIT_E6",
    "MX_SEC_A0_IGR_HIT_E7",
	"MX_SEC_A0_IGR_OK_E0",
    "MX_SEC_A0_IGR_OK_E1",
    "MX_SEC_A0_IGR_OK_E2",
	"MX_SEC_A0_IGR_OK_E3",
    "MX_SEC_A0_IGR_OK_E4",
    "MX_SEC_A0_IGR_OK_E5",
	"MX_SEC_A0_IGR_OK_E6",
	"MX_SEC_A0_IGR_OK_E7",
    "MX_SEC_A0_IGR_UNCHK_E0",
    "MX_SEC_A0_IGR_UNCHK_E1",
    "MX_SEC_A0_IGR_UNCHK_E2",
	"MX_SEC_A0_IGR_UNCHK_E3",
    "MX_SEC_A0_IGR_UNCHK_E4",
    "MX_SEC_A0_IGR_UNCHK_E5",
	"MX_SEC_A0_IGR_UNCHK_E6",
    "MX_SEC_A0_IGR_UNCHK_E7",
	"MX_SEC_A0_IGR_DELAY_E0",
    "MX_SEC_A0_IGR_DELAY_E1",
    "MX_SEC_A0_IGR_DELAY_E2",
	"MX_SEC_A0_IGR_DELAY_E3",
    "MX_SEC_A0_IGR_DELAY_E4",
    "MX_SEC_A0_IGR_DELAY_E5",
	"MX_SEC_A0_IGR_DELAY_E6",
    "MX_SEC_A0_IGR_DELAY_E7",
	"MX_SEC_A0_IGR_LATE_E0",
    "MX_SEC_A0_IGR_LATE_E1",
    "MX_SEC_A0_IGR_LATE_E2",
	"MX_SEC_A0_IGR_LATE_E3",
    "MX_SEC_A0_IGR_LATE_E4",
    "MX_SEC_A0_IGR_LATE_E5",
	"MX_SEC_A0_IGR_LATE_E6",
    "MX_SEC_A0_IGR_LATE_E7",
	"MX_SEC_A0_IGR_INVLD_E0",
    "MX_SEC_A0_IGR_INVLD_E1",
    "MX_SEC_A0_IGR_INVLD_E2",
	"MX_SEC_A0_IGR_INVLD_E3",
    "MX_SEC_A0_IGR_INVLD_E4",
    "MX_SEC_A0_IGR_INVLD_E5",
	"MX_SEC_A0_IGR_INVLD_E6",
    "MX_SEC_A0_IGR_INVLD_E7",
	"MX_SEC_A0_IGR_NOTVLD_E0",
    "MX_SEC_A0_IGR_NOTVLD_E1",
    "MX_SEC_A0_IGR_NOTVLD_E2",
	"MX_SEC_A0_IGR_NOTVLD_E3",
    "MX_SEC_A0_IGR_NOTVLD_E4",
    "MX_SEC_A0_IGR_NOTVLD_E5",
	"MX_SEC_A0_IGR_NOTVLD_E6",
    "MX_SEC_A0_IGR_NOTVLD_E7",
	"MX_SEC_A0_EGR_PKT_PROT_E0",
    "MX_SEC_A0_EGR_PKT_PROT_E1",
    "MX_SEC_A0_EGR_PKT_PROT_E2",
	"MX_SEC_A0_EGR_PKT_PROT_E3",
    "MX_SEC_A0_EGR_PKT_PROT_E4",
    "MX_SEC_A0_EGR_PKT_PROT_E5",
	"MX_SEC_A0_EGR_PKT_PROT_E6",
    "MX_SEC_A0_EGR_PKT_PROT_E7",
	"MX_SEC_A0_EGR_PKT_ENC_E0",
    "MX_SEC_A0_EGR_PKT_ENC_E1",
	"MX_SEC_A0_EGR_PKT_ENC_E2",
	"MX_SEC_A0_EGR_PKT_ENC_E3",
    "MX_SEC_A0_EGR_PKT_ENC_E4",
    "MX_SEC_A0_EGR_PKT_ENC_E5",
	"MX_SEC_A0_EGR_PKT_ENC_E6",
    "MX_SEC_A0_EGR_PKT_ENC_E7",
	"MX_SEC_A0_EGR_HIT_E0",
    "MX_SEC_A0_EGR_HIT_E1",
    "MX_SEC_A0_EGR_HIT_E2",
	"MX_SEC_A0_EGR_HIT_E3",
    "MX_SEC_A0_EGR_HIT_E4",
    "MX_SEC_A0_EGR_HIT_E5",
	"MX_SEC_A0_EGR_HIT_E6",
    "MX_SEC_A0_EGR_HIT_E7",
	"MX_SEC_A0_IGR_OCT_VAL",
    "MX_SEC_A0_IGR_OCT_DEC",
    "MX_SEC_A0_IGR_UNTAG",
    "MX_SEC_A0_IGR_NOTAG",
	"MX_SEC_A0_IGR_BADTAG",
    "MX_SEC_A0_IGR_UNKSCI",
    "MX_SEC_A0_IGR_NOSCI",
    "MX_SEC_A0_IGR_UNUSSA",
	"MX_SEC_A0_IGR_NOUSSA",
    "MX_SEC_A0_IGR_OCT_TOT",
    "MX_SEC_A0_EGR_OCT_PROT",
	"MX_SEC_A0_EGR_OCT_ENC",
    "MX_SEC_A0_EGR_OCT_TOTAL",
	"MX_SEC_A0_IGR_MISS",
    "MX_SEC_A0_EGR_MISS",
    "MX_SEC_A0_IGR_REDIR"};

char * statsNmList_a0_up [] ={
	"MX_SEC_A0_UP_IGR_HIT_E0",
	"MX_SEC_A0_UP_IGR_HIT_E1",
	"MX_SEC_A0_UP_IGR_HIT_E2",
	"MX_SEC_A0_UP_IGR_HIT_E3",
	"MX_SEC_A0_UP_IGR_HIT_E4",
	"MX_SEC_A0_UP_IGR_HIT_E5",
	"MX_SEC_A0_UP_IGR_HIT_E6",
	"MX_SEC_A0_UP_IGR_HIT_E7",
	"MX_SEC_A0_UP_IGR_HIT_E8",
	"MX_SEC_A0_UP_IGR_HIT_E9",
	"MX_SEC_A0_UP_IGR_HIT_E10",
	"MX_SEC_A0_UP_IGR_HIT_E11",
	"MX_SEC_A0_UP_IGR_HIT_E12",
	"MX_SEC_A0_UP_IGR_HIT_E13",
	"MX_SEC_A0_UP_IGR_HIT_E14",
	"MX_SEC_A0_UP_IGR_HIT_E15",
	"MX_SEC_A0_UP_IGR_OK_E0",
	"MX_SEC_A0_UP_IGR_OK_E1",
	"MX_SEC_A0_UP_IGR_OK_E2",
	"MX_SEC_A0_UP_IGR_OK_E3",
	"MX_SEC_A0_UP_IGR_OK_E4",
	"MX_SEC_A0_UP_IGR_OK_E5",
	"MX_SEC_A0_UP_IGR_OK_E6",
	"MX_SEC_A0_UP_IGR_OK_E7",
	"MX_SEC_A0_UP_IGR_OK_E8",
	"MX_SEC_A0_UP_IGR_OK_E9",
	"MX_SEC_A0_UP_IGR_OK_E10",
	"MX_SEC_A0_UP_IGR_OK_E11",
	"MX_SEC_A0_UP_IGR_OK_E12",
	"MX_SEC_A0_UP_IGR_OK_E13",
	"MX_SEC_A0_UP_IGR_OK_E14",
	"MX_SEC_A0_UP_IGR_OK_E15",
	"MX_SEC_A0_UP_IGR_UNCHK_E0",
	"MX_SEC_A0_UP_IGR_UNCHK_E1",
	"MX_SEC_A0_UP_IGR_UNCHK_E2",
	"MX_SEC_A0_UP_IGR_UNCHK_E3",
	"MX_SEC_A0_UP_IGR_UNCHK_E4",
	"MX_SEC_A0_UP_IGR_UNCHK_E5",
	"MX_SEC_A0_UP_IGR_UNCHK_E6",
	"MX_SEC_A0_UP_IGR_UNCHK_E7",
	"MX_SEC_A0_UP_IGR_UNCHK_E8",
	"MX_SEC_A0_UP_IGR_UNCHK_E9",
	"MX_SEC_A0_UP_IGR_UNCHK_E10",
	"MX_SEC_A0_UP_IGR_UNCHK_E11",
	"MX_SEC_A0_UP_IGR_UNCHK_E12",
	"MX_SEC_A0_UP_IGR_UNCHK_E13",
	"MX_SEC_A0_UP_IGR_UNCHK_E14",
	"MX_SEC_A0_UP_IGR_UNCHK_E15",
	"MX_SEC_A0_UP_IGR_DELAY_E0",
	"MX_SEC_A0_UP_IGR_DELAY_E1",
	"MX_SEC_A0_UP_IGR_DELAY_E2",
	"MX_SEC_A0_UP_IGR_DELAY_E3",
	"MX_SEC_A0_UP_IGR_DELAY_E4",
	"MX_SEC_A0_UP_IGR_DELAY_E5",
	"MX_SEC_A0_UP_IGR_DELAY_E6",
	"MX_SEC_A0_UP_IGR_DELAY_E7",
	"MX_SEC_A0_UP_IGR_DELAY_E8",
	"MX_SEC_A0_UP_IGR_DELAY_E9",
	"MX_SEC_A0_UP_IGR_DELAY_E10",
	"MX_SEC_A0_UP_IGR_DELAY_E11",
	"MX_SEC_A0_UP_IGR_DELAY_E12",
	"MX_SEC_A0_UP_IGR_DELAY_E13",
	"MX_SEC_A0_UP_IGR_DELAY_E14",
	"MX_SEC_A0_UP_IGR_DELAY_E15",
	"MX_SEC_A0_UP_IGR_LATE_E0",
	"MX_SEC_A0_UP_IGR_LATE_E1",
	"MX_SEC_A0_UP_IGR_LATE_E2",
	"MX_SEC_A0_UP_IGR_LATE_E3",
	"MX_SEC_A0_UP_IGR_LATE_E4",
	"MX_SEC_A0_UP_IGR_LATE_E5",
	"MX_SEC_A0_UP_IGR_LATE_E6",
	"MX_SEC_A0_UP_IGR_LATE_E7",
	"MX_SEC_A0_UP_IGR_LATE_E8",
	"MX_SEC_A0_UP_IGR_LATE_E9",
	"MX_SEC_A0_UP_IGR_LATE_E10",
	"MX_SEC_A0_UP_IGR_LATE_E11",
	"MX_SEC_A0_UP_IGR_LATE_E12",
	"MX_SEC_A0_UP_IGR_LATE_E13",
	"MX_SEC_A0_UP_IGR_LATE_E14",
	"MX_SEC_A0_UP_IGR_LATE_E15",
	"MX_SEC_A0_UP_IGR_INVLD_E0",
	"MX_SEC_A0_UP_IGR_INVLD_E1",
	"MX_SEC_A0_UP_IGR_INVLD_E2",
	"MX_SEC_A0_UP_IGR_INVLD_E3",
	"MX_SEC_A0_UP_IGR_INVLD_E4",
	"MX_SEC_A0_UP_IGR_INVLD_E5",
	"MX_SEC_A0_UP_IGR_INVLD_E6",
	"MX_SEC_A0_UP_IGR_INVLD_E7",
	"MX_SEC_A0_UP_IGR_INVLD_E8",
	"MX_SEC_A0_UP_IGR_INVLD_E9",
	"MX_SEC_A0_UP_IGR_INVLD_E10",
	"MX_SEC_A0_UP_IGR_INVLD_E11",
	"MX_SEC_A0_UP_IGR_INVLD_E12",
	"MX_SEC_A0_UP_IGR_INVLD_E13",
	"MX_SEC_A0_UP_IGR_INVLD_E15",
	"MX_SEC_A0_UP_IGR_INVLD_E15",
	"MX_SEC_A0_UP_IGR_NOTVLD_E0",
	"MX_SEC_A0_UP_IGR_NOTVLD_E1",
	"MX_SEC_A0_UP_IGR_NOTVLD_E2",
	"MX_SEC_A0_UP_IGR_NOTVLD_E3",
	"MX_SEC_A0_UP_IGR_NOTVLD_E4",
	"MX_SEC_A0_UP_IGR_NOTVLD_E5",
	"MX_SEC_A0_UP_IGR_NOTVLD_E6",
	"MX_SEC_A0_UP_IGR_NOTVLD_E7",
	"MX_SEC_A0_UP_IGR_NOTVLD_E8",
	"MX_SEC_A0_UP_IGR_NOTVLD_E9",
	"MX_SEC_A0_UP_IGR_NOTVLD_E10",
	"MX_SEC_A0_UP_IGR_NOTVLD_E11",
	"MX_SEC_A0_UP_IGR_NOTVLD_E12",
	"MX_SEC_A0_UP_IGR_NOTVLD_E13",
	"MX_SEC_A0_UP_IGR_NOTVLD_E14",
	"MX_SEC_A0_UP_IGR_NOTVLD_E15",
	"MX_SEC_A0_UP_EGR_PKT_PROT_E0",
	"MX_SEC_A0_UP_EGR_PKT_PROT_E1",
	"MX_SEC_A0_UP_EGR_PKT_PROT_E2",
	"MX_SEC_A0_UP_EGR_PKT_PROT_E3",
	"MX_SEC_A0_UP_EGR_PKT_PROT_E4",
	"MX_SEC_A0_UP_EGR_PKT_PROT_E5",
	"MX_SEC_A0_UP_EGR_PKT_PROT_E6",
	"MX_SEC_A0_UP_EGR_PKT_PROT_E7",
	"MX_SEC_A0_UP_EGR_PKT_PROT_E8",
	"MX_SEC_A0_UP_EGR_PKT_PROT_E9",
	"MX_SEC_A0_UP_EGR_PKT_PROT_E10",
	"MX_SEC_A0_UP_EGR_PKT_PROT_E11",
	"MX_SEC_A0_UP_EGR_PKT_PROT_E12",
	"MX_SEC_A0_UP_EGR_PKT_PROT_E13",
	"MX_SEC_A0_UP_EGR_PKT_PROT_E14",
	"MX_SEC_A0_UP_EGR_PKT_PROT_E15",
	"MX_SEC_A0_UP_EGR_PKT_ENC_E0",
	"MX_SEC_A0_UP_EGR_PKT_ENC_E1",
	"MX_SEC_A0_UP_EGR_PKT_ENC_E2",
	"MX_SEC_A0_UP_EGR_PKT_ENC_E3",
	"MX_SEC_A0_UP_EGR_PKT_ENC_E4",
	"MX_SEC_A0_UP_EGR_PKT_ENC_E5",
	"MX_SEC_A0_UP_EGR_PKT_ENC_E6",
	"MX_SEC_A0_UP_EGR_PKT_ENC_E7",
	"MX_SEC_A0_UP_EGR_PKT_ENC_E8",
	"MX_SEC_A0_UP_EGR_PKT_ENC_E9",
	"MX_SEC_A0_UP_EGR_PKT_ENC_E10",
	"MX_SEC_A0_UP_EGR_PKT_ENC_E11",
	"MX_SEC_A0_UP_EGR_PKT_ENC_E12",
	"MX_SEC_A0_UP_EGR_PKT_ENC_E13",
	"MX_SEC_A0_UP_EGR_PKT_ENC_E14",
	"MX_SEC_A0_UP_EGR_PKT_ENC_E15",
	"MX_SEC_A0_UP_EGR_HIT_E0",
	"MX_SEC_A0_UP_EGR_HIT_E1",
	"MX_SEC_A0_UP_EGR_HIT_E2",
	"MX_SEC_A0_UP_EGR_HIT_E3",
	"MX_SEC_A0_UP_EGR_HIT_E4",
	"MX_SEC_A0_UP_EGR_HIT_E5",
	"MX_SEC_A0_UP_EGR_HIT_E6",
	"MX_SEC_A0_UP_EGR_HIT_E7",
	"MX_SEC_A0_UP_EGR_HIT_E8",
	"MX_SEC_A0_UP_EGR_HIT_E9",
	"MX_SEC_A0_UP_EGR_HIT_E10",
	"MX_SEC_A0_UP_EGR_HIT_E11",
	"MX_SEC_A0_UP_EGR_HIT_E12",
	"MX_SEC_A0_UP_EGR_HIT_E13",
	"MX_SEC_A0_UP_EGR_HIT_E14",
	"MX_SEC_A0_UP_EGR_HIT_E15",
	"MX_SEC_A0_UP_IGR_OCT_VAL",
	"MX_SEC_A0_UP_IGR_OCT_DEC",
	"MX_SEC_A0_UP_IGR_UNTAG",
	"MX_SEC_A0_UP_IGR_NOTAG",
	"MX_SEC_A0_UP_IGR_BADTAG",
	"MX_SEC_A0_UP_IGR_UNKSCI",
	"MX_SEC_A0_UP_IGR_NOSCI",
	"MX_SEC_A0_UP_IGR_UNUSSA",
	"MX_SEC_A0_UP_IGR_NOUSSA",
	"MX_SEC_A0_UP_IGR_OCT_TOT",
	"MX_SEC_A0_UP_EGR_OCT_PROT",
	"MX_SEC_A0_UP_EGR_OCT_ENC",
	"MX_SEC_A0_UP_EGR_OCT_TOTAL",
	"MX_SEC_A0_UP_IGR_MISS",
	"MX_SEC_A0_UP_EGR_MISS",
	"MX_SEC_A0_UP_IGR_REDIR"
};


char * statsNmList_1540 [] ={
	"MX_SEC_1540_IGR_HIT_E0",
	"MX_SEC_1540_IGR_HIT_E1",
	"MX_SEC_1540_IGR_HIT_E2",
	"MX_SEC_1540_IGR_HIT_E3",
	"MX_SEC_1540_IGR_HIT_E4",
	"MX_SEC_1540_IGR_HIT_E5",
	"MX_SEC_1540_IGR_HIT_E6",
	"MX_SEC_1540_IGR_HIT_E7",
	"MX_SEC_1540_IGR_HIT_E8",
	"MX_SEC_1540_IGR_HIT_E9",
	"MX_SEC_1540_IGR_HIT_E10",
	"MX_SEC_1540_IGR_HIT_E11",
	"MX_SEC_1540_IGR_HIT_E12",
	"MX_SEC_1540_IGR_HIT_E13",
	"MX_SEC_1540_IGR_HIT_E14",
	"MX_SEC_1540_IGR_HIT_E15",
	"MX_SEC_1540_IGR_HIT_E16",
	"MX_SEC_1540_IGR_HIT_E17",
	"MX_SEC_1540_IGR_HIT_E18",
	"MX_SEC_1540_IGR_HIT_E19",
	"MX_SEC_1540_IGR_HIT_E20",
	"MX_SEC_1540_IGR_HIT_E21",
	"MX_SEC_1540_IGR_HIT_E22",
	"MX_SEC_1540_IGR_HIT_E23",
	"MX_SEC_1540_IGR_HIT_E24",
	"MX_SEC_1540_IGR_HIT_E25",
	"MX_SEC_1540_IGR_HIT_E26",
	"MX_SEC_1540_IGR_HIT_E27",
	"MX_SEC_1540_IGR_HIT_E28",
	"MX_SEC_1540_IGR_HIT_E29",
	"MX_SEC_1540_IGR_HIT_E30",
	"MX_SEC_1540_IGR_HIT_E31",
	"MX_SEC_1540_IGR_OK_E0",
	"MX_SEC_1540_IGR_OK_E1",
	"MX_SEC_1540_IGR_OK_E2",
	"MX_SEC_1540_IGR_OK_E3",
	"MX_SEC_1540_IGR_OK_E4",
	"MX_SEC_1540_IGR_OK_E5",
	"MX_SEC_1540_IGR_OK_E6",
	"MX_SEC_1540_IGR_OK_E7",
	"MX_SEC_1540_IGR_OK_E8",
	"MX_SEC_1540_IGR_OK_E9",
	"MX_SEC_1540_IGR_OK_E10",
	"MX_SEC_1540_IGR_OK_E11",
	"MX_SEC_1540_IGR_OK_E12",
	"MX_SEC_1540_IGR_OK_E13",
	"MX_SEC_1540_IGR_OK_E14",
	"MX_SEC_1540_IGR_OK_E15",
	"MX_SEC_1540_IGR_OK_E16",
	"MX_SEC_1540_IGR_OK_E17",
	"MX_SEC_1540_IGR_OK_E18",
	"MX_SEC_1540_IGR_OK_E19",
	"MX_SEC_1540_IGR_OK_E20",
	"MX_SEC_1540_IGR_OK_E21",
	"MX_SEC_1540_IGR_OK_E22",
	"MX_SEC_1540_IGR_OK_E23",
	"MX_SEC_1540_IGR_OK_E24",
	"MX_SEC_1540_IGR_OK_E25",
	"MX_SEC_1540_IGR_OK_E26",
	"MX_SEC_1540_IGR_OK_E27",
	"MX_SEC_1540_IGR_OK_E28",
	"MX_SEC_1540_IGR_OK_E29",
	"MX_SEC_1540_IGR_OK_E30",
	"MX_SEC_1540_IGR_OK_E31",
	"MX_SEC_1540_IGR_UNCHK_E0",
	"MX_SEC_1540_IGR_UNCHK_E1",
	"MX_SEC_1540_IGR_UNCHK_E2",
	"MX_SEC_1540_IGR_UNCHK_E3",
	"MX_SEC_1540_IGR_UNCHK_E4",
	"MX_SEC_1540_IGR_UNCHK_E5",
	"MX_SEC_1540_IGR_UNCHK_E6",
	"MX_SEC_1540_IGR_UNCHK_E7",
	"MX_SEC_1540_IGR_UNCHK_E8",
	"MX_SEC_1540_IGR_UNCHK_E9",
	"MX_SEC_1540_IGR_UNCHK_E10",
	"MX_SEC_1540_IGR_UNCHK_E11",
	"MX_SEC_1540_IGR_UNCHK_E12",
	"MX_SEC_1540_IGR_UNCHK_E13",
	"MX_SEC_1540_IGR_UNCHK_E14",
	"MX_SEC_1540_IGR_UNCHK_E15",
	"MX_SEC_1540_IGR_UNCHK_E16",
	"MX_SEC_1540_IGR_UNCHK_E17",
	"MX_SEC_1540_IGR_UNCHK_E18",
	"MX_SEC_1540_IGR_UNCHK_E19",
	"MX_SEC_1540_IGR_UNCHK_E20",
	"MX_SEC_1540_IGR_UNCHK_E21",
	"MX_SEC_1540_IGR_UNCHK_E22",
	"MX_SEC_1540_IGR_UNCHK_E23",
	"MX_SEC_1540_IGR_UNCHK_E24",
	"MX_SEC_1540_IGR_UNCHK_E25",
	"MX_SEC_1540_IGR_UNCHK_E26",
	"MX_SEC_1540_IGR_UNCHK_E27",
	"MX_SEC_1540_IGR_UNCHK_E28",
	"MX_SEC_1540_IGR_UNCHK_E29",
	"MX_SEC_1540_IGR_UNCHK_E30",
	"MX_SEC_1540_IGR_UNCHK_E31",
	"MX_SEC_1540_IGR_DELAY_E0",
	"MX_SEC_1540_IGR_DELAY_E1",
	"MX_SEC_1540_IGR_DELAY_E2",
	"MX_SEC_1540_IGR_DELAY_E3",
	"MX_SEC_1540_IGR_DELAY_E4",
	"MX_SEC_1540_IGR_DELAY_E5",
	"MX_SEC_1540_IGR_DELAY_E6",
	"MX_SEC_1540_IGR_DELAY_E7",
	"MX_SEC_1540_IGR_DELAY_E8",
	"MX_SEC_1540_IGR_DELAY_E9",
	"MX_SEC_1540_IGR_DELAY_E10",
	"MX_SEC_1540_IGR_DELAY_E11",
	"MX_SEC_1540_IGR_DELAY_E12",
	"MX_SEC_1540_IGR_DELAY_E13",
	"MX_SEC_1540_IGR_DELAY_E14",
	"MX_SEC_1540_IGR_DELAY_E15",
	"MX_SEC_1540_IGR_DELAY_E16",
	"MX_SEC_1540_IGR_DELAY_E17",
	"MX_SEC_1540_IGR_DELAY_E18",
	"MX_SEC_1540_IGR_DELAY_E19",
	"MX_SEC_1540_IGR_DELAY_E20",
	"MX_SEC_1540_IGR_DELAY_E21",
	"MX_SEC_1540_IGR_DELAY_E22",
	"MX_SEC_1540_IGR_DELAY_E23",
	"MX_SEC_1540_IGR_DELAY_E24",
	"MX_SEC_1540_IGR_DELAY_E25",
	"MX_SEC_1540_IGR_DELAY_E26",
	"MX_SEC_1540_IGR_DELAY_E27",
	"MX_SEC_1540_IGR_DELAY_E28",
	"MX_SEC_1540_IGR_DELAY_E29",
	"MX_SEC_1540_IGR_DELAY_E30",
	"MX_SEC_1540_IGR_DELAY_E31",
	"MX_SEC_1540_IGR_LATE_E0",
	"MX_SEC_1540_IGR_LATE_E1",
	"MX_SEC_1540_IGR_LATE_E2",
	"MX_SEC_1540_IGR_LATE_E3",
	"MX_SEC_1540_IGR_LATE_E4",
	"MX_SEC_1540_IGR_LATE_E5",
	"MX_SEC_1540_IGR_LATE_E6",
	"MX_SEC_1540_IGR_LATE_E7",
	"MX_SEC_1540_IGR_LATE_E8",
	"MX_SEC_1540_IGR_LATE_E9",
	"MX_SEC_1540_IGR_LATE_E10",
	"MX_SEC_1540_IGR_LATE_E11",
	"MX_SEC_1540_IGR_LATE_E12",
	"MX_SEC_1540_IGR_LATE_E13",
	"MX_SEC_1540_IGR_LATE_E14",
	"MX_SEC_1540_IGR_LATE_E15",
	"MX_SEC_1540_IGR_LATE_E16",
	"MX_SEC_1540_IGR_LATE_E17",
	"MX_SEC_1540_IGR_LATE_E18",
	"MX_SEC_1540_IGR_LATE_E19",
	"MX_SEC_1540_IGR_LATE_E20",
	"MX_SEC_1540_IGR_LATE_E21",
	"MX_SEC_1540_IGR_LATE_E22",
	"MX_SEC_1540_IGR_LATE_E23",
	"MX_SEC_1540_IGR_LATE_E24",
	"MX_SEC_1540_IGR_LATE_E25",
	"MX_SEC_1540_IGR_LATE_E26",
	"MX_SEC_1540_IGR_LATE_E27",
	"MX_SEC_1540_IGR_LATE_E28",
	"MX_SEC_1540_IGR_LATE_E29",
	"MX_SEC_1540_IGR_LATE_E30",
	"MX_SEC_1540_IGR_LATE_E31",
	"MX_SEC_1540_IGR_INVLD_E0",
	"MX_SEC_1540_IGR_INVLD_E1",
	"MX_SEC_1540_IGR_INVLD_E2",
	"MX_SEC_1540_IGR_INVLD_E3",
	"MX_SEC_1540_IGR_INVLD_E4",
	"MX_SEC_1540_IGR_INVLD_E5",
	"MX_SEC_1540_IGR_INVLD_E6",
	"MX_SEC_1540_IGR_INVLD_E7",
	"MX_SEC_1540_IGR_INVLD_E8",
	"MX_SEC_1540_IGR_INVLD_E9",
	"MX_SEC_1540_IGR_INVLD_E10",
	"MX_SEC_1540_IGR_INVLD_E11",
	"MX_SEC_1540_IGR_INVLD_E12",
	"MX_SEC_1540_IGR_INVLD_E13",
	"MX_SEC_1540_IGR_INVLD_E15",
	"MX_SEC_1540_IGR_INVLD_E15",
	"MX_SEC_1540_IGR_INVLD_E16",
	"MX_SEC_1540_IGR_INVLD_E17",
	"MX_SEC_1540_IGR_INVLD_E18",
	"MX_SEC_1540_IGR_INVLD_E19",
	"MX_SEC_1540_IGR_INVLD_E20",
	"MX_SEC_1540_IGR_INVLD_E21",
	"MX_SEC_1540_IGR_INVLD_E22",
	"MX_SEC_1540_IGR_INVLD_E23",
	"MX_SEC_1540_IGR_INVLD_E24",
	"MX_SEC_1540_IGR_INVLD_E25",
	"MX_SEC_1540_IGR_INVLD_E26",
	"MX_SEC_1540_IGR_INVLD_E27",
	"MX_SEC_1540_IGR_INVLD_E28",
	"MX_SEC_1540_IGR_INVLD_E29",
	"MX_SEC_1540_IGR_INVLD_E30",
	"MX_SEC_1540_IGR_INVLD_E31",
	"MX_SEC_1540_IGR_NOTVLD_E0",
	"MX_SEC_1540_IGR_NOTVLD_E1",
	"MX_SEC_1540_IGR_NOTVLD_E2",
	"MX_SEC_1540_IGR_NOTVLD_E3",
	"MX_SEC_1540_IGR_NOTVLD_E4",
	"MX_SEC_1540_IGR_NOTVLD_E5",
	"MX_SEC_1540_IGR_NOTVLD_E6",
	"MX_SEC_1540_IGR_NOTVLD_E7",
	"MX_SEC_1540_IGR_NOTVLD_E8",
	"MX_SEC_1540_IGR_NOTVLD_E9",
	"MX_SEC_1540_IGR_NOTVLD_E10",
	"MX_SEC_1540_IGR_NOTVLD_E11",
	"MX_SEC_1540_IGR_NOTVLD_E12",
	"MX_SEC_1540_IGR_NOTVLD_E13",
	"MX_SEC_1540_IGR_NOTVLD_E14",
	"MX_SEC_1540_IGR_NOTVLD_E15",
	"MX_SEC_1540_IGR_NOTVLD_E16",
	"MX_SEC_1540_IGR_NOTVLD_E17",
	"MX_SEC_1540_IGR_NOTVLD_E18",
	"MX_SEC_1540_IGR_NOTVLD_E19",
	"MX_SEC_1540_IGR_NOTVLD_E20",
	"MX_SEC_1540_IGR_NOTVLD_E21",
	"MX_SEC_1540_IGR_NOTVLD_E22",
	"MX_SEC_1540_IGR_NOTVLD_E23",
	"MX_SEC_1540_IGR_NOTVLD_E24",
	"MX_SEC_1540_IGR_NOTVLD_E25",
	"MX_SEC_1540_IGR_NOTVLD_E26",
	"MX_SEC_1540_IGR_NOTVLD_E27",
	"MX_SEC_1540_IGR_NOTVLD_E28",
	"MX_SEC_1540_IGR_NOTVLD_E29",
	"MX_SEC_1540_IGR_NOTVLD_E30",
	"MX_SEC_1540_IGR_NOTVLD_E31",
	"MX_SEC_1540_EGR_PKT_PROT_E0",
	"MX_SEC_1540_EGR_PKT_PROT_E1",
	"MX_SEC_1540_EGR_PKT_PROT_E2",
	"MX_SEC_1540_EGR_PKT_PROT_E3",
	"MX_SEC_1540_EGR_PKT_PROT_E4",
	"MX_SEC_1540_EGR_PKT_PROT_E5",
	"MX_SEC_1540_EGR_PKT_PROT_E6",
	"MX_SEC_1540_EGR_PKT_PROT_E7",
	"MX_SEC_1540_EGR_PKT_PROT_E8",
	"MX_SEC_1540_EGR_PKT_PROT_E9",
	"MX_SEC_1540_EGR_PKT_PROT_E10",
	"MX_SEC_1540_EGR_PKT_PROT_E11",
	"MX_SEC_1540_EGR_PKT_PROT_E12",
	"MX_SEC_1540_EGR_PKT_PROT_E13",
	"MX_SEC_1540_EGR_PKT_PROT_E14",
	"MX_SEC_1540_EGR_PKT_PROT_E15",
	"MX_SEC_1540_EGR_PKT_PROT_E16",
	"MX_SEC_1540_EGR_PKT_PROT_E17",
	"MX_SEC_1540_EGR_PKT_PROT_E18",
	"MX_SEC_1540_EGR_PKT_PROT_E19",
	"MX_SEC_1540_EGR_PKT_PROT_E20",
	"MX_SEC_1540_EGR_PKT_PROT_E21",
	"MX_SEC_1540_EGR_PKT_PROT_E22",
	"MX_SEC_1540_EGR_PKT_PROT_E23",
	"MX_SEC_1540_EGR_PKT_PROT_E24",
	"MX_SEC_1540_EGR_PKT_PROT_E25",
	"MX_SEC_1540_EGR_PKT_PROT_E26",
	"MX_SEC_1540_EGR_PKT_PROT_E27",
	"MX_SEC_1540_EGR_PKT_PROT_E28",
	"MX_SEC_1540_EGR_PKT_PROT_E29",
	"MX_SEC_1540_EGR_PKT_PROT_E30",
	"MX_SEC_1540_EGR_PKT_PROT_E31",
	"MX_SEC_1540_EGR_PKT_ENC_E0",
	"MX_SEC_1540_EGR_PKT_ENC_E1",
	"MX_SEC_1540_EGR_PKT_ENC_E2",
	"MX_SEC_1540_EGR_PKT_ENC_E3",
	"MX_SEC_1540_EGR_PKT_ENC_E4",
	"MX_SEC_1540_EGR_PKT_ENC_E5",
	"MX_SEC_1540_EGR_PKT_ENC_E6",
	"MX_SEC_1540_EGR_PKT_ENC_E7",
	"MX_SEC_1540_EGR_PKT_ENC_E8",
	"MX_SEC_1540_EGR_PKT_ENC_E9",
	"MX_SEC_1540_EGR_PKT_ENC_E10",
	"MX_SEC_1540_EGR_PKT_ENC_E11",
	"MX_SEC_1540_EGR_PKT_ENC_E12",
	"MX_SEC_1540_EGR_PKT_ENC_E13",
	"MX_SEC_1540_EGR_PKT_ENC_E14",
	"MX_SEC_1540_EGR_PKT_ENC_E15",
	"MX_SEC_1540_EGR_PKT_ENC_E16",
	"MX_SEC_1540_EGR_PKT_ENC_E17",
	"MX_SEC_1540_EGR_PKT_ENC_E18",
	"MX_SEC_1540_EGR_PKT_ENC_E19",
	"MX_SEC_1540_EGR_PKT_ENC_E20",
	"MX_SEC_1540_EGR_PKT_ENC_E21",
	"MX_SEC_1540_EGR_PKT_ENC_E22",
	"MX_SEC_1540_EGR_PKT_ENC_E23",
	"MX_SEC_1540_EGR_PKT_ENC_E24",
	"MX_SEC_1540_EGR_PKT_ENC_E25",
	"MX_SEC_1540_EGR_PKT_ENC_E26",
	"MX_SEC_1540_EGR_PKT_ENC_E27",
	"MX_SEC_1540_EGR_PKT_ENC_E28",
	"MX_SEC_1540_EGR_PKT_ENC_E29",
	"MX_SEC_1540_EGR_PKT_ENC_E30",
	"MX_SEC_1540_EGR_PKT_ENC_E31",
	"MX_SEC_1540_EGR_HIT_E0",
	"MX_SEC_1540_EGR_HIT_E1",
	"MX_SEC_1540_EGR_HIT_E2",
	"MX_SEC_1540_EGR_HIT_E3",
	"MX_SEC_1540_EGR_HIT_E4",
	"MX_SEC_1540_EGR_HIT_E5",
	"MX_SEC_1540_EGR_HIT_E6",
	"MX_SEC_1540_EGR_HIT_E7",
	"MX_SEC_1540_EGR_HIT_E8",
	"MX_SEC_1540_EGR_HIT_E9",
	"MX_SEC_1540_EGR_HIT_E10",
	"MX_SEC_1540_EGR_HIT_E11",
	"MX_SEC_1540_EGR_HIT_E12",
	"MX_SEC_1540_EGR_HIT_E13",
	"MX_SEC_1540_EGR_HIT_E14",
	"MX_SEC_1540_EGR_HIT_E15",
	"MX_SEC_1540_EGR_HIT_E16",
	"MX_SEC_1540_EGR_HIT_E17",
	"MX_SEC_1540_EGR_HIT_E18",
	"MX_SEC_1540_EGR_HIT_E19",
	"MX_SEC_1540_EGR_HIT_E20",
	"MX_SEC_1540_EGR_HIT_E21",
	"MX_SEC_1540_EGR_HIT_E22",
	"MX_SEC_1540_EGR_HIT_E23",
	"MX_SEC_1540_EGR_HIT_E24",
	"MX_SEC_1540_EGR_HIT_E25",
	"MX_SEC_1540_EGR_HIT_E26",
	"MX_SEC_1540_EGR_HIT_E27",
	"MX_SEC_1540_EGR_HIT_E28",
	"MX_SEC_1540_EGR_HIT_E29",
	"MX_SEC_1540_EGR_HIT_E30",
	"MX_SEC_1540_EGR_HIT_E31",
	"MX_SEC_1540_IGR_OCT_VAL",
	"MX_SEC_1540_IGR_OCT_DEC",
	"MX_SEC_1540_IGR_UNTAG",
	"MX_SEC_1540_IGR_NOTAG",
	"MX_SEC_1540_IGR_BADTAG",
	"MX_SEC_1540_IGR_UNKSCI",
	"MX_SEC_1540_IGR_NOSCI",
	"MX_SEC_1540_IGR_UNUSSA",
	"MX_SEC_1540_IGR_NOUSSA",
	"MX_SEC_1540_IGR_OCT_TOT",
	"MX_SEC_1540_EGR_OCT_PROT",
	"MX_SEC_1540_EGR_OCT_ENC",
	"MX_SEC_1540_EGR_OCT_TOTAL",
	"MX_SEC_1540_IGR_MISS",
	"MX_SEC_1540_EGR_MISS",
	"MX_SEC_1540_IGR_REDIR"
};

//#define NUM_OF_MACSEC_STAT 0x60 /* in msec_defines.h */
#define MAX_NUM_OF_MACSEC_STAT 0x150 /* maximun number */
unsigned int statsValList[4][MAX_NUM_OF_MACSEC_STAT];
unsigned int statsAccValList[4][MAX_NUM_OF_MACSEC_STAT];
char **statsNmList;

unsigned int sysmacValList[8][MAX_NUM_OF_MACSEC_STAT];


char * macNmList[32] ={
"RxGoodOctLo",
"RxGoodOctHi",
"RxBadOct",
"TxFifoUnder",
"RxUnicast",
"SentDeferred",
"BroadRx",
"MultiRx",
"Fr64 Fr65_127",
"Fr128_255",
"Fr256_511",
"Fr512_1023",
"Fr1024_Max",
"GoodTxOctLo",
"GoodTxOctHi",
"TxUnicast",
"ExCol",
"TxMulti",
"TxBroad",
"SentMultiple",
"FCSent",
"FCReceived",
"RxFifoOver",
"Undersize",
"Fragments",
"Oversize",
"Jabber",
"RxError",
"BadCrc",
"Collisions",
"LateCol"
};
#define MAX_DEV_NUM 2

unsigned int macValList[MAX_DEV_NUM*4][NUM_OF_MAC_STAT];
//unsigned int macValList[4][NUM_OF_MAC_STAT];

#define Msec_Speed_Gigabit     2
#define Msec_Speed_100MB       1
#define Msec_Speed_10MB        0



static unsigned int devList;
static  int portList[8] = {0,1, 2, 3, 4, 5, 6, 7};  /* port 0,1,2,3, 4, 5, 6, 7 */
static unsigned int init_speed;

void Debug (char * msg) {
  if (0) {
    printf("Debug: %s\n", msg);
  }
}

void Error (char * msg) {
  if (1) {
     printf("Error: %s\n", msg); }
}
void print_reg(MAD_DEV *devp, int port, unsigned int x) {
  printf(" %x", cpu_read(devp, port,x));
}

void echo_reg (MAD_DEV *devp, int port, unsigned int rg, unsigned short data) {
  
  msecWriteReg(devp, port,  rg, data);
  printf(" %x", cpu_read(devp, port,rg));
}

void wrchk_reg (MAD_DEV *devp, int port, unsigned int rg, unsigned short data) {
   unsigned short chk;
 
  msecWriteReg(devp, port,  rg, data);
  chk = cpu_read(devp, port,rg);
  if (chk!=data) {
    printf("ERROR -- Addr %x Read back %x, expected %x\n", rg, chk, data);
  }
}


MAD_DEV * initialize_macsec_api(MAD_DEV * devp) {
//  devp = macsec_api_init_def(usbport, smiaddr);
  if(devp == NULL)  {
    Error ("MACsec API initialization failed.\n");
	return NULL;
  } else {
	printf( "MACsec API initialization success.\n");
  }
  //# check test chip revision for 1340L: 1 - A0B, 0xC800 - Y0A, 0xC000 - Z0A
  if ((devp->deviceId==MAD_88E1540) ||
	  (devp->deviceId==MAD_88E1548) ||
	  (devp->deviceId==MAD_88E1680) ||
	  (devp->deviceId==MAD_88E1680M) ||
	  (devp->msec_ctrl.msec_rev==MAD_MSEC_REV_A0B)) {
      if ((devp->deviceId==MAD_88E1540)||(devp->deviceId==MAD_88E1548)||
		  (devp->deviceId==MAD_88E1680)||(devp->deviceId==MAD_88E1680M)||
		  (devp->deviceId==MAD_88E151x)) {
        macsec_version = PROD_1540L;
	  } 
      else
         macsec_version = REV_A0_UP;
  } else {
         macsec_version = REV_A0;
  }
  return devp;
}


void print_u8_arr(char * arr, int dim) {
int i;
  for (i=0; i<dim; i++) {
    printf("%2X\n", get_u8_arr_fld((char *)arr, i));
  }
}

void print_u16_arr(short * arr, int dim) {
int i;
  for (i=0; i<dim; i++) {
    printf("%4X\n", get_u16_arr_fld(arr, i));
  }
}

void print_u32_arr(unsigned int * arr, int dim) {
int i;
  for (i=0; i<dim; i++) {
    printf("%4X\n", get_u32_arr_fld(arr, i));
  }
}

/*
# arr[0] - bit127:96
# arr[1] - bit95:64
# arr[2] - bit63:32
# arr[3] - bit31:0
*/

unsigned char * convert_u32_arr_to_list(unsigned int arr[], int dim) {
  static unsigned char out_list [Max_List_Num];
  int i;
  unsigned int fld;
  for (i=0; i<dim; i++) {
    fld = get_u32_arr_fld(arr, i);
	out_list[0+i*4] = fld >> 24 & 0xFF;
	out_list[1+i*4] = fld >> 16 & 0xFF;
	out_list[2+i*4] = fld >> 8 & 0xFF;
	out_list[3+i*4] = fld & 0xFF;
  
//    set out_list [join $out];
  }
  //printf( "output list: $out_list\n");
  return out_list;
}

int compare_list_octet(char *list1, char * list2) {
  int rc = 0;
  int i = 0;
  do {
    if(list1[i] != list2[i]) {
      rc = -1;
      break;
    }
  } while (i++<(Max_List_Num-1));
  return rc;
}
/*
# Generate random number of octets and return as a list of hex octets "%02X"
# This can be used to generate random 16-Byte keys 
*/
char * gen_random_octs(int cnt) {
  static char octs [16];
int i;
  for (i=0; i<cnt; i++) {
    octs[i] = (rand() * pow(2,8))&0xff;
  }
  return octs;
}
/*
# Generate random number of octets and return as a list of hex octets in 
# Hex format "0x%02X"
# This can be used to generate random 8-Byte SCI
*/
char * gen_random_hex_octs(int cnt) {
static  char octs [8];
int i;
  cnt = 8;
  for (i=0; i<cnt; i++) {
    octs[i] = (rand() * pow(2,8))&0xff;
  }
  return octs;
}

/*
# Generate random 8-byte SCI string
*/
char * gen_random_sci(void) {
  char *sci = gen_random_hex_octs(8);
  static  char sciStr[16];
  sprintf( sciStr, "0x%02X%02X%02X%02X%02X%02X%02X%02X",
	       sci[0], sci[1], sci[2], sci[3], sci[4], sci[5], sci[6], sci[7]);
  return sciStr;
}

/*
# Generate random long long SCI 
*/
long long gen_random_64_octs(void) {
  long long scill;
  char *sci = gen_random_hex_octs(8);

  scill =  *(long long *)sci;
  return scill;
}






int validate_fips_compliance (MAD_DEV *devp, unsigned int *ports, FIPS_CFG_T * fips_cfg, 
							   int *out_arr, unsigned char * obsv_vec) {
  int rc = 0;
  int i, port = 0;
  char * out_list;
  int eq;

  for(i=0; i<devp->numOfPorts; i++) {
	if(!ports [port]) continue;
	port = ports[i];
    set_dev_ctrl_port_num(devp, port);
    //clear_fips_vec $devp
    msec_port_fips_chk(devp, port, fips_cfg, out_arr);
    out_list = convert_u32_arr_to_list(out_arr, 4);
    eq = compare_list_octet(out_list, obsv_vec);
    if (eq == 0) {
      printf("port %d output two list is same as obsv_vec, correct.\n", port);
    } else {
      printf("port %d output two list is diff from obsv_vec, incorrect.\n", port);
      rc = -1;
    }
  }
  return rc;
}

MATCH_EN * new_match_en_ent(int ent_num, int en_def, int en_mc, int en_da, int en_sa, int en_vlan, int en_et, int en_scian, int en_hdr) {
  
  MATCH_EN *lk_match_en=new_match_en(en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian, en_hdr);
  en_hdr = 0;
  printf("ent_num  en_def en_mc en_da en_sa en_vlan en_et en_scian en_hdr\n");
  printf("%7d %6d %5d %5d %5d %7d %5d %8d %6d\n",
	  ent_num,en_def,en_mc,en_da,en_sa,en_vlan,en_et,en_scian,en_hdr);
  return lk_match_en;
}

MATCH_FLD *  new_match_fld_ent(int ent_num, unsigned long long da, unsigned long long sa, 
							   unsigned short vlan, unsigned short et, 
							   unsigned long long sci, unsigned char tci, 
							   unsigned char tci_msk, 
							   int conf_offset, int hdr_fld)
{
	
//  if {[regexp {^(0x)} $sciStr]} {
//    set sci $sciStr
//  } else {
//    set sci "0x%s" $sciStr]
//  }
  MATCH_FLD * lk_match_fld;
  hdr_fld = 0;
  lk_match_fld = new_match_fld(da, sa, vlan, et, sci, tci, tci_msk, conf_offset, hdr_fld);
  printf("Setup ELU match fields:");
  printf("ent_num	da_fld       sa_fld       vlan_fld et_fld sci_fld tci_fld tci_msk_fld conf_offset hdr_fld\n");
  printf("%-7d %12s %12s %-8d %-6X %18s %-7X %X %11d %7d", ent_num, da, sa, vlan, et, sci, tci, tci_msk, conf_offset, hdr_fld);
  return lk_match_fld;
}

ACT_FLD *  new_act_fld_ent(int ctx_num_sec_level, int drop_maxpn, int drop,
						   int redir, int auth_en, int enc_en) {
  ACT_FLD * lk_act_fld;
  lk_act_fld = new_act_fld(ctx_num_sec_level, drop_maxpn, drop, redir, auth_en, enc_en);
  printf("ctx_num_sec_level drop_maxpn drop redir auth_en enc_en\n");
  printf("%17d %10d %4d %5d %7d %6d", ctx_num_sec_level, drop_maxpn, drop, redir, auth_en, enc_en);
  return lk_act_fld;
}
CTX_T *  new_ctx_ent(int ent_num, int keyStr, unsigned long long sci,  unsigned int pn, 
					 unsigned char tci, unsigned char offset) {
//  global hResult
//  # setup ECT
//  # The key array and ct are shared structures for egress and ingress,
//  # it is created here and deleted later.
//  if {[regexp {^(0x)} $sciStr]} {
//    set sci $sciStr
//  } else {
//    set sci "0x%s" $sciStr]
//  }
CTX_T * ctx;
//char *keyArr = convert_keystr_to_arr(keyStr);
char keyArr[16] = {0};

ctx = new_ctx_t(sci, pn, tci, offset, keyArr);
  printf("Ent_num sci              tci pn\n");
  printf("%-7d %-16s %-3x %-d\n", ent_num, sci, tci, pn);
  printf("keystr: %s \n", keyStr);

  //#delete_u8_arr $keyArr
  return ctx;
}


void stat_arr_init (void) {
//  global  REV_A0_UP
//  global  PROD_1540L
//  global  statsNmList
  if(macsec_version == PROD_1540L)
    statsNmList = statsNmList_1540;
  else if(macsec_version == REV_A0_UP)
    statsNmList = statsNmList_a0_up;
  else
    statsNmList = statsNmList_a0;
}


void dump_macsec_statistics(MAD_DEV *devp) {
  int ports = 0xf; //"0 1 2 3"
  int port, i;
//  global statsNmList
//  global statsValList
//  global NUM_OF_MACSEC_STAT

/*  # first retrieve the statistis data and save in the list */
  printf("\nDump all MACsec statistics counters.\n");
  for(port=0; port<devp->numOfPorts; port++) {
	if(!(ports & (1<<port))) continue;
    set_dev_ctrl_port_num(devp, port);
    for(i=0; i<NUM_OF_MACSEC_STAT(devp); i++) {
       statsValList[port][i] = port_get_macsec_stat_value(devp, port, i);
    }
  }
}

void print_macsec_statistics(MAD_DEV *devp) {
  int ports = 0xf; //"0 1 2 3"
  int port;
//  set ports "0 1 2 3"
//  global statsNmList
//  global statsValList
//  set dev_num [get_dev_ctrl_dev_num $devp]
  int dev_num = get_dev_ctrl_dev_num(devp);
  int statNm;

  printf("dev_num = %d\n", dev_num);
  for(port=0; port<devp->numOfPorts; port++) {
	if(!(ports & (1<<port))) continue;
//    foreach statNm $statsNmList statVal $statsValList([expr $dev_num * 4+port]) {
    for(statNm=0; statNm<NUM_OF_MACSEC_STAT(devp); statNm++) {
      printf("port %d %25s %10d\n", port, statsNmList[statNm], statsValList[dev_num * 4+port][statNm]);
    }
  }
}

void dump_print_macsec_statistics(MAD_DEV *devp) {
  int ports = 0xf; //"0 1 2 3"
  int port, i, statNm, statVal;
//  global statsNmList
//  global statsValList
//  global NUM_OF_MACSEC_STAT
/*  # first retrieve the statistis data and save in the list */
  printf("\nDump all MACsec statistics counters.\n");
  for(port=0; port<devp->numOfPorts; port++) {
	if(!(ports & (1<<port))) continue;
    set_dev_ctrl_port_num(devp, port);
    for(i=0; i<NUM_OF_MACSEC_STAT(devp); i++) {
       statsValList[port][i] = port_get_macsec_stat_value(devp, port, i);
    }
  }

  if (1) {
    for(port=0; port<devp->numOfPorts; port++) {
      if(!(ports & (1<<port))) continue;
	  for(statNm=0; statNm<NUM_OF_MACSEC_STAT(devp); statNm++) {
	    statVal=statNm;
	    printf("port %d %25s %10d\n", port, statsNmList[statNm], statsValList[port][statVal]);
      }
    }
  }
}

void dump_print_macsec_stats(MAD_DEV * devp) {
  dump_print_macsec_statistics(devp);
}

void clr_acc_macsec_stats (MAD_DEV * devp) {
//  global statsAccValList
//  global NUM_OF_MACSEC_STAT
//  set ports "0 1 2 3"
  int ports = 0xf; //"0 1 2 3"
  int port, i;

  for(port=0; port<devp->numOfPorts; port++) {
	if(!(ports & (1<<port))) continue;
    for(i=0; i<NUM_OF_MACSEC_STAT(devp); i++) {
      statsAccValList[port][i] = 0;
    }
  }
}
//# To accumulate statistics counters over long time,
//# tcl8.4 or upper version is needed to support wide integer
void acc_macsec_stats (MAD_DEV * devp) {
//  set ports "0 1 2 3"
//  global statsValList
//  global statsAccValList
  int ports = 0xf; //"0 1 2 3"
//  int tmpList[MAX_NUM_OF_MACSEC_STAT];
  int port, statVal, statAccVal;

  for(port=0; port<devp->numOfPorts; port++) {
	if(!(ports & (1<<port))) continue;
    for(statVal=0; statVal<NUM_OF_MACSEC_STAT(devp); statVal++)
	{
      statAccVal=statVal;
      statsAccValList[port][statAccVal] = statsValList[port][statVal]+statsAccValList[port][statAccVal];
	}
  }
}
//# port here is defined as logical port number when multiple devices are present on SVB
//# for example, a SVB with 4 1340L devices, the 1st device has port 0~3, 
//# 2nd device 4~7, 3rd device 8~11, 4th device has port 12~15
void dump_accumulate_macsec_statistics(MAD_DEV *devp) {
  int ports = 0xf; //"0 1 2 3"
//  global statsNmList
//  global statsValList
//  global NUM_OF_MACSEC_STAT

/*  # first retrieve the statistis data and save in the list */
  dump_print_macsec_statistics(devp);
/*  # Then add up the current counters to accumulated statistics list */
  acc_macsec_stats(devp);
}

#if 0

void dump_print_mac_statistics(MAD_DEV *devp) {
  int n;
  int name, i;
  printf("\nDump all MAC statistics counters.\n");
  for (n=0; n<4; n++) {
    i = 0x1000 + 0x20 * n;
    for(name=0; name<32; name++) {
      printf("port%1d %4x %20s %10d\n", n, i, nlist[name], cpu_read(devp, port, i));
      i++;
    }
  }
}

#endif

void dump_mac_statistics(MAD_DEV *devp) {
//  global macNmList
//  global macValList
//  global VN_MAC_STATS
//  global NUM_OF_MAC_STAT
//  #set NUM_OF_MAC_STAT 0x20
  int n;
  int dev_num = get_dev_ctrl_dev_num(devp);
  printf("\nDump all MAC statistics counters on dev %d\n", dev_num);
  for (n=0; n<devp->numOfPorts; n++) {
	int name, i;
//    set macValList([expr $dev_num * 4+n]) [list]
//    set i = VN_MAC_STATS+NUM_OF_MAC_STAT * $n]
	i = VN_MAC_STATS(devp) + NUM_OF_MAC_STAT * (n%4);
	for(name =0; name <NUM_OF_MAC_STAT; name++) {
      unsigned int val = cpu_read(devp, n, i);
      macValList[dev_num * 4 + n][name] = val;
    }
  }
}


void print_mac_statistics(MAD_DEV * devp) {
//  global  macNmList
//  global  macValList
//  set dev_num [get_dev_ctrl_dev_num $devp]
  int n, name;
  unsigned int val;
  int dev_num = get_dev_ctrl_dev_num(devp);
  printf("\nprint all MAC statistics counters on dev %d\n", dev_num);
  for (n=0; n<devp->numOfPorts; n++) {
	  for(name=0; name<NUM_OF_MACSEC_STAT(devp); name++) {
	    val=macValList[dev_num * 4 + n][name];
        printf("port%1d %20s %10d", n, macNmList[name], val);
      }
  }
}

void dump_print_mac_statistics(MAD_DEV * devp) {
//  global  macNmList
//  global  macValList

  dump_mac_statistics(devp);
  print_mac_statistics(devp);
}
void dump_print_mac_stats(MAD_DEV * devp) {
  dump_print_mac_statistics(devp);
}

//# port here is defined as logical port number when multiple devices are present on SVB
//# for example, a SVB with 4 1340L devices, the 1st device has port 0~3, 
//# 2nd device 4~7, 3rd device 8~11, 4th device has port 12~15
int get_macsec_stats_by_name(MAD_DEV * devp, int port, MAC_NAME statName) {
//  global statsNmList
//  global statsValList
//  foreach statNm $statsNmList statVal $statsValList($port) {
//    if [string match $statNm $statName] {
//	  printf("port %d %25s %X\n"); $port $statNm $statVal]
//	  return $statVal
//    }
//  }
  int statNm, statVal;
  for(statNm=0; statNm<NUM_OF_MACSEC_STAT(devp); statNm++) {
    statVal=statNm;
    if (statNm==statName) {
	  printf("port %d %25s %X\n", port, statsNmList[statNm], statsValList[port][statVal]);
	  return statsValList[port][statVal];
	}
  }
  return -1;
}

//# port here is defined as logical port number when multiple devices are present on SVB
//# for example, a SVB with 4 1340L devices, the 1st device has port 0~3, 
//# 2nd device 4~7, 3rd device 8~11, 4th device has port 12~15
int check_mac_err (MAD_DEV * devp, int port) {
  int rc = 0, err;
//  # check MAC RxError 
  if (err=get_macsec_stats_by_name (devp, port, RxError)!= 0) {
    printf("port %d has %d RxError.\n", port, err);
    rc++;
  }
//  # check MAC badCRC
  if (err=get_macsec_stats_by_name(devp, port, BadCrc) != 0) {
    printf("port %d has %d  BadCRC.\n", port, err);
    rc++;
  }
  return rc;
}

//# port here is defined as logical port number when multiple devices are present on SVB
//# for example, a SVB with 4 1340L devices, the 1st device has port 0~3, 
//# 2nd device 4~7, 3rd device 8~11, 4th device has port 12~15
int get_macsec_acc_stats_by_name (MAD_DEV *devp, int port, int statName) {
//  global statsNmList
//  global statsAccValList
//  foreach statNm $statsNmList statVal $statsAccValList($port) {
//    if [string match $statNm $statName] {
//	  printf("port %d %25s %X\n"); $port $statNm $statVal]
//	  return $statVal
//    }
//  }
  int statNm, statVal=-1;
  for(statNm=0; statNm<NUM_OF_MACSEC_STAT(devp); statNm++) {
    statVal=statNm;
    if (statNm==statName) {
	  printf("port %d %25s %X\n", port, statsNmList[statNm], statsAccValList[port][statVal]);
	  return statsAccValList[port][statVal];
	}
  }
  return -1;
}


//# port here is defined as logical port number when multiple devices are present on SVB
//# for example, a SVB with 4 1340L devices, the 1st device has port 0~3, 
//# 2nd device 4~7, 3rd device 8~11, 4th device has port 12~15
int get_macsec_stats_by_range (MAD_DEV *devp, int port, int statName, int cnt) {
//  global statsNmList
//  global statsValList
//  foreach statNm $statsNmList statVal $statsValList($port) {
//    if [string match $statNm $statName] {
//	  printf("port %d %25s %X\n"); $port $statNm $statVal]
//	  # use the index statName of statsNmList as it is same as 
//	  # index statsVal of statsValList(port)
//	  set idx [lsearch $statsNmList $statNm]
//	  set retVal [lrange $statsValList($port) $idx , idx+cnt -1]]
//	  return $retVal
//    }
//  }
  int statNm, statVal;
  int idx, retVal;
  for(statNm=0; statNm<NUM_OF_MACSEC_STAT(devp); statNm++) {
    statVal=statNm;
    if (statNm==statName) {
	  printf("port %d %25s %X\n", port, statsNmList[statNm], statsValList[port][statVal]);
	  idx =statNm;
	  retVal = statsValList[port][idx + cnt -1];
	  return retVal;
	}
  }
  return -1;
}


//# port here is defined as logical port number when multiple devices are present on SVB
//# for example, a SVB with 4 1340L devices, the 1st device has port 0~3, 
//# 2nd device 4~7, 3rd device 8~11, 4th device has port 12~15
int get_macsec_acc_stats_by_range(MAD_DEV *devp, int port, int statName, int cnt) {
//  global statsNmList
//  global statsAccValList
//  foreach statNm $statsNmList statVal $statsAccValList($port) {
//    if [string match $statNm $statName] {
//	  printf("port %d %25s %X\n"); $port $statNm $statVal]
//	  # use the index statName of statsNmList as it is same as 
//	  # index statsVal of statsValList(port)
//	  set idx [lsearch $statsNmList $statNm]
//	  set retVal [lrange $statsAccValList($port) $idx , idx+cnt -1]]
//	  return $retVal
//    }printf("%-4d %-7d %-12X %-12X %-8d %-6X %-16s %-7X %-7X\n"); $iport $ent_num  $da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld]
//  }
  int statNm, statVal;
  int idx, retVal;
  for(statNm=0; statNm<NUM_OF_MACSEC_STAT(devp); statNm++) {
    statVal=statNm;
    if (statNm==statName) {
	  printf("port %d %25s %X\n", port, statsNmList[statNm], statsAccValList[port][statVal]);
	  idx =statNm;
	  retVal = statsAccValList[port][idx + cnt -1];
	  return retVal;
	}
  }
  return -1;
}


int compare_egr_igr_hit_counters (MAD_DEV * devp, int eport, int iport, int ent) {
//  set igrStatName"MX_SEC_IGR_HIT_E%d" $ent]
//  set egrStatName"MX_SEC_EGR_HIT_E%d" $ent]
//  set igrStat [get_macsec_stats_by_name $iport $igrStatName]
//  set egrStat [get_macsec_stats_by_name $eport $egrStatName]
  int igrStatName;
  int egrStatName;
  int igrStat;
  int egrStat;
  if(macsec_version == PROD_1540L)
  {
    igrStatName = MX_SEC_1540_IGR_HIT_E0 + ent;
    egrStatName = MX_SEC_1540_EGR_HIT_E0 + ent;
  }
  else if(macsec_version == REV_A0_UP)
  {
    igrStatName = MX_SEC_A0_UP_IGR_HIT_E0 + ent;
    egrStatName = MX_SEC_A0_UP_EGR_HIT_E0 + ent;
  }
  else
  {
    igrStatName = MX_SEC_A0_IGR_HIT_E0 + ent;
    egrStatName = MX_SEC_A0_EGR_HIT_E0 + ent;
  }

  igrStat = get_macsec_stats_by_name(devp, iport, igrStatName);
  egrStat = get_macsec_stats_by_name(devp, eport, egrStatName);
  printf( "%s=%x %s=%x\n", statsNmList[igrStatName], igrStat, statsNmList[egrStatName], egrStat);
  if (igrStat == egrStat && igrStat != 0) {
    return 0;
  } else {
    return -1;
  }
}

int compare_igr_hit_ok_counters(MAD_DEV * devp, int iport, int ent) {
//  global hResult
//  set igrHitName "MX_SEC_IGR_HIT_E%d" $ent]
//  set igrOkName "MX_SEC_IGR_OK_E%d" $ent]

  int igrHitName;
  int igrOkName;
  int igrHit;
  int igrOk;
  if(macsec_version == PROD_1540L)
  {
    igrHitName = MX_SEC_1540_IGR_HIT_E0 + ent;
    igrOkName = MX_SEC_1540_IGR_OK_E0 + ent;
  }
  else if(macsec_version == REV_A0_UP)
  {
    igrHitName = MX_SEC_A0_UP_IGR_HIT_E0 + ent;
    igrOkName = MX_SEC_A0_UP_IGR_OK_E0 + ent;
  }
  else
  {
    igrHitName = MX_SEC_A0_IGR_HIT_E0 + ent;
    igrOkName = MX_SEC_A0_IGR_OK_E0 + ent;
  }



  igrHit = get_macsec_stats_by_name(devp, iport, igrHitName);
  igrOk = get_macsec_stats_by_name(devp, iport, igrOkName);
  printf( "%s=%x %s=%x\n", statsNmList[igrHitName], igrHit, statsNmList[igrOkName], igrOk);
  if (igrHit == igrOk && igrHit != 0 ) {
    printf("ent_num %d igr_hit matches igr_ok, correct.", ent);
    return 0;
  } else {
    printf("ent_num %d egr_hit mismatches igr_ok, or igr_hit is 0, incorrect.", ent);
    return -1;
  }
}

int compare_egr_igr_acc_hit_counters(MAD_DEV * devp, int eport, int iport,  int ent) {
//  set igrStatName"MX_SEC_IGR_HIT_E%d" $ent]
//  set egrStatName"MX_SEC_EGR_HIT_E%d" $ent]
//  set igrStat [get_macsec_acc_stats_by_name $iport $igrStatName]
//  set egrStat [get_macsec_acc_stats_by_name $eport $egrStatName]
  int igrStatName;
  int egrStatName;
  int igrStat;
  int egrStat;
  if(macsec_version == PROD_1540L)
  {
    igrStatName = MX_SEC_1540_IGR_HIT_E0 + ent;
    egrStatName = MX_SEC_1540_EGR_HIT_E0 + ent;
  }
  else if(macsec_version == REV_A0_UP)
  {
	igrStatName = MX_SEC_A0_UP_IGR_HIT_E0 + ent;
    egrStatName = MX_SEC_A0_UP_EGR_HIT_E0 + ent;
  }
  else
  {
    igrStatName = MX_SEC_A0_IGR_HIT_E0 + ent;
    egrStatName = MX_SEC_A0_EGR_HIT_E0 + ent;
  }

  igrStat = get_macsec_acc_stats_by_name(devp, iport, igrStatName);
  egrStat = get_macsec_acc_stats_by_name(devp, eport, egrStatName);
  printf("eport %d %s=%d\n", eport, statsNmList[egrStatName], egrStat);
  printf("iport %d %s=%d\n", iport, statsNmList[igrStatName], igrStat);
  if (igrStat == egrStat && igrStat != 0 ) {
    return 0;
  } else {
    return -1;
  }
}

void set_sysmac_mibcnt_en(MAD_DEV * devp, int port, int flag) {
//  global  MEM_PER_SLICE(devp)
//  global  VN_CFG_SYS_MAC
    
  unsigned short reg = VN_CFG_SYS_MAC(devp) + MEM_PER_SLICE(devp) * port;
  unsigned int data = cpu_read(devp, port, reg);
  if(flag) {
    data |= 0x10000;
  } else {
    data &= 0x0FFFF;
  }
  msecWriteReg(devp, port,  reg, data);
}

void dump_sysmac_statistics(MAD_DEV * devp) {
//  global  macNmList
//  global  sysmacValList
//  global  VN_SYSMAC_STATS
//  global  NUM_OF_MAC_STAT

//  set dev_num [get_dev_ctrl_dev_num $devp]
  int n, name, i;
  int dev_num = get_dev_ctrl_dev_num(devp);
  printf("\nDump all SYSMAC statistics counters on dev %d\n", dev_num);
  for (n=0; n<2; n++) {
//    set sysmacValList([expr $dev_num * 4+n]) [list]
    i = VN_SYSMAC_STATS(devp) + NUM_OF_MAC_STAT * n%4;
//    foreach name $macNmList {
    for(name=0; name<NUM_OF_MACSEC_STAT(devp); name++) {
      sysmacValList[dev_num * 4 + n][name] = cpu_read(devp, n, i);
      i++;
    }
  }
}

void print_sysmac_statistics(MAD_DEV * devp) {
//  global  macNmList
//  global  sysmacValList
//  set dev_num [get_dev_ctrl_dev_num $devp]
  int n, name;
  int dev_num = get_dev_ctrl_dev_num(devp);
  for (n=0; n<2; n++) {
    for(name=0; name<NUM_OF_MACSEC_STAT(devp); name++) {
      printf("port%1d %20s %10d\n", n, macNmList[name], sysmacValList[dev_num * 4 + n][name]);
    }
  }
}

void dump_print_sysmac_statistics(MAD_DEV * devp) {
//  global  macNmList
//  global  sysmacValList

  dump_sysmac_statistics(devp);
  print_sysmac_statistics(devp);
}

void dump_print_sysmac_stats(MAD_DEV * devp) {
  dump_print_sysmac_statistics(devp);
}

#if 0
//# check the macsec retain header
void check_macsec_retain_hdr {recvFrame} {
  if {[llength $recvFrame] <20} {
	 return 0
  } else {
    set etype [lrange $recvFrame 12 13]
    printf( "etype $etype\n");
    if { [string compare $etype "88 E5"] == 0} {
      return 1
    } else {
      return 0
    }
  }  
}

#endif
void printportigr_key(MAD_DEV *devp,  int ent_num) {
 unsigned char * key = create_u8_arr(16);
 int  i, port = get_dev_ctrl_port_num(devp);
  char keyByte[16];
  msec_port_get_igr_key(devp, port, ent_num, key);
  printf("port %d IGR Key:\n", port);
  for(i=0; i <16; i++) {
    keyByte[i] = get_u8_arr_fld(key, i);
    printf("%02X\n", keyByte[i]);
  }
  printf("\n"); 
  delete_u8_arr(key);
}
void printportigr_hkey(MAD_DEV *devp,  int ent_num) {
 unsigned char * hkey = create_u8_arr(16);
 int  i, port = get_dev_ctrl_port_num(devp);
 char keyByte;
  msec_port_get_igr_hkey(devp, port, ent_num, hkey);
  printf("port %d IGR HarshKey:\n", port);
  for(i=0; i <16; i++) {
    keyByte = get_u8_arr_fld(hkey, i);
    printf("%02X", keyByte);
  }
  printf("\n"); 
  delete_u8_arr(hkey);
}

void printportegr_key(MAD_DEV *devp,  int ent_num) {
 unsigned char * key = create_u8_arr(16);
 int i, port = get_dev_ctrl_port_num(devp);
 char keyByte;
  msec_port_get_egr_key(devp, port, ent_num, key);
  printf("port %d EGR Key:\n", port);
  for(i=0; i <16; i++) {
    keyByte = get_u8_arr_fld(key, i);
    printf("%02X\n", keyByte);
  }
  printf("\n"); 
  delete_u8_arr(key);
}

void printportegr_hkey(MAD_DEV *devp,  int ent_num) {
 unsigned char * hkey = create_u8_arr(16);
 int i, port = get_dev_ctrl_port_num(devp);
 char keyByte;
  msec_port_get_egr_hkey(devp, port, ent_num, hkey);
  printf("port %d EGR HarshKey:\n", port);
  for(i=0; i <16; i++) {
    keyByte = get_u8_arr_fld(hkey, i);
    printf("%02X", keyByte);
  }
  printf("\n"); 
  delete_u8_arr(hkey);
}



void force_flow_ctrl_en(MAD_DEV * devp) {
//  global  SYS_AUTO_NEG_CONTROL(devp)
//  global  SYS_MAC_CONTROL1(devp)
//  global  PAUSE_CTL(devp)
//  global  MEM_PER_SLICE(devp)
    unsigned short reg1;
    unsigned short reg2;
    unsigned short reg3;
    unsigned int data1;
    unsigned int data2;
    unsigned int data3;
	int portnum;
  for(portnum=0; portnum<devp->numOfPorts; portnum++) {
    reg1 = MEM_PER_SLICE(devp) * (portnum%4) + SYS_AUTO_NEG_CONTROL(devp);
    reg2 = MEM_PER_SLICE(devp) * (portnum%4) + PAUSE_CTL(devp);
    reg3 = MEM_PER_SLICE(devp) * (portnum%4) + SYS_MAC_CONTROL1(devp);
    data1 =  cpu_read(devp, portnum, reg1);
    data2 =  cpu_read(devp, portnum, reg2);
    data3 =  cpu_read(devp, portnum, reg3);
    if(0) {
    printf("before enable flow control: port %d\n", portnum);
    printf("reg %X = %X\n", reg1, data1);
    printf("reg %X = %X\n", reg2, data2);
    printf("reg %X = %X\n", reg3, data3);
    }
    msecWriteReg(devp, portnum,  reg1, data1|0x8);
    msecWriteReg(devp, portnum,  reg2, 0x8);
    if(1) {
    //# reduce sysmac IPG from default 12 to 11
    //#msecWriteReg(devp, port,  reg3 0x200B1
    msecWriteReg(devp, portnum,  reg3, (data3 & 0xFF80F) | (11 << 4));
    }
    if(0) {
    printf("after enable flow control: port %d\n", portnum);
    printf("reg %X = %X\n", reg1, data1);
    printf("reg %X = %X\n", reg2, data2);
    printf("reg %X = %X\n", reg3, data3);
    }
  }
}

void force_flow_ctrl_dis(MAD_DEV * devp) {
//  global  SYS_AUTO_NEG_CONTROL(devp)
//  global  SYS_MAC_CONTROL1(devp)
//  global  PAUSE_CTL(devp)
//  global  MEM_PER_SLICE(devp)
    unsigned short reg1;
    unsigned short reg2;
    unsigned short reg3;
    unsigned int data1;
    unsigned int data2;
    unsigned int data3;
	int portnum;
  for(portnum=0; portnum<devp->numOfPorts; portnum++) {
    reg1 = MEM_PER_SLICE(devp) * (portnum%4) + SYS_AUTO_NEG_CONTROL(devp);
    reg2 = MEM_PER_SLICE(devp) * (portnum%4) + PAUSE_CTL(devp);
    reg3 = MEM_PER_SLICE(devp) * (portnum%4) + SYS_MAC_CONTROL1(devp);
    data1 =  cpu_read(devp, portnum, reg1);
    data2 =  cpu_read(devp, portnum, reg2);
    data3 =  cpu_read(devp, portnum, reg3);
    if(0) {
    printf("before disable flow control: port %d\n", portnum);
    printf("reg %X = %X\n", reg1, data1);
    printf("reg %X = %X\n", reg2, data2);
    printf("reg %X = %X\n", reg3, data3);
    }
    msecWriteReg(devp, portnum,  reg1, data1 & 0xFFF7);
    if(0) {
    printf("after disable flow control: port %d\n", portnum);
    printf("reg %X = %X\n", reg1, data1);
    printf("reg %X = %X\n", reg2, data2);
    printf("reg %X = %X\n", reg3, data3);
    }
  }
}

void force_wire_flow_ctrl_en(MAD_DEV * devp) {
//  global  WIR_AUTO_NEG_CONTROL(devp)
//  global  WIR_MAC_CONTROL2(devp)
//  global  PAUSE_CTL(devp)
//  global  MEM_PER_SLICE(devp)
	int portnum;
	unsigned int data;
  for(portnum=0; portnum<devp->numOfPorts; portnum++) {
    data =  cpu_read(devp, portnum, MEM_PER_SLICE(devp) * (portnum%4) + WIR_AUTO_NEG_CONTROL(devp));
    printf("before enable flow control: port %d\n", portnum);
    printf("WIR_AUTO_NEG_CONTROL %d = %X\n", WIR_AUTO_NEG_CONTROL(devp), data);
    printf("PAUSE_CTL %d = %X\n", PAUSE_CTL(devp), cpu_read(devp, portnum, (MEM_PER_SLICE(devp) * portnum + PAUSE_CTL(devp))));
    printf("WIR_MAC_CONTROL1 %d = %X\n", WIR_MAC_CONTROL1(devp), cpu_read(devp, portnum, (MEM_PER_SLICE(devp) * portnum + WIR_MAC_CONTROL1(devp))));
    msecWriteReg(devp, portnum,  (MEM_PER_SLICE(devp) * (portnum%4) + WIR_AUTO_NEG_CONTROL(devp)), data | 0x8);
    msecWriteReg(devp, portnum,  (MEM_PER_SLICE(devp) * (portnum%4) + PAUSE_CTL(devp)), 0x8);
    //# reduce sysmac IPG from default 12 to 6
    //#msecWriteReg(devp, portnum,  (MEM_PER_SLICE(devp) * portnum + WIR_MAC_CONTROL1(devp)] 0x20061
    printf("after enable flow control: port %d\n", portnum);
    printf("WIR_AUTO_NEG_CONTROL %d = %X\n", WIR_AUTO_NEG_CONTROL(devp), data);
    printf("PAUSE_CTL %d = %X\n", PAUSE_CTL(devp), cpu_read(devp, portnum, (MEM_PER_SLICE(devp) * portnum + PAUSE_CTL(devp))));
    printf("WIR_MAC_CONTROL1 %d = %X\n", WIR_MAC_CONTROL1(devp), cpu_read(devp, portnum, (MEM_PER_SLICE(devp) * portnum + WIR_MAC_CONTROL1(devp))));
  }
}

void set_sym_flow_ctrl(MAD_DEV * devp) {
//  global  WIR_AUTO_NEG_CONTROL(devp)
//  global  WIR_MAC_CONTROL2(devp)
//  global  WIR_HIDDEN0(devp)
//  global  SYS_AUTO_NEG_CONTROL(devp)
//  global  SYS_MAC_CONTROL2(devp)
//  global  SYS_HIDDEN0(devp)
//  global  PAUSE_CTL(devp)
//  global  MEM_PER_SLICE(devp)
	int port;
    unsigned short reg1;
    unsigned short reg2;
    unsigned short reg3;
    unsigned short reg4;
    unsigned short reg5;
    unsigned int data1;
    unsigned int data2;
    unsigned int data3;
    unsigned int data4;
    unsigned int data5;
    unsigned int data6;
    unsigned int data7;
  port = get_dev_ctrl_port_num(devp);
  reg1 = MEM_PER_SLICE(devp) * (port%4) + SYS_AUTO_NEG_CONTROL(devp);
  reg2 = MEM_PER_SLICE(devp) * (port%4) + WIR_AUTO_NEG_CONTROL(devp);
  reg3 = MEM_PER_SLICE(devp) * (port%4) + PAUSE_CTL(devp);
  reg4 = MEM_PER_SLICE(devp) * (port%4) + SYS_MAC_CONTROL2(devp);
  reg5 = MEM_PER_SLICE(devp) * (port%4) + WIR_MAC_CONTROL2(devp);
  data1 =  cpu_read(devp, port, reg1);
  data2 =  cpu_read(devp, port, reg2);
  data3 =  cpu_read(devp, port, reg3);
  data4 =  cpu_read(devp, port, reg4);
  data5 =  cpu_read(devp, port, reg5);
  //# 1. AutoNegCtrl3:2=b'11 for both mac 
  data1 = data1 | 0xC;
  msecWriteReg(devp, port,  reg1, data1);
  data2 = data2 | 0xC;
  msecWriteReg(devp, port,  reg2, data2);
  //# 2. PAUSE_CTL(devp) = 0x000A
  data3 = 0x000A;
  msecWriteReg(devp, port,  reg3, data3);
  //# 3. mac_control2.8=0 and enable drop_fc
  data4 = (data4 & 0xFF) | 0x40;
  data5 = (data5 & 0xFF) | 0x40;
  msecWriteReg(devp, port,  reg4, data4);
  msecWriteReg(devp, port,  reg5, data5);

  data6  = cpu_port_read(devp, port, WIR_HIDDEN0(devp));
  data6 = bit_set(data6, 3);
  msecWritePortReg(devp, port, WIR_HIDDEN0(devp), data6);
  data7  = cpu_port_read(devp, port, SYS_HIDDEN0(devp));
  data7 = bit_set(data7, 3);
  msecWritePortReg(devp, port, SYS_HIDDEN0(devp), data7);
}



void check_mac_status(MAD_DEV * devp) {
//  global  WIR_MAC_STATUS0(devp)
//  global  SYS_MAC_STATUS0(devp)
//  global  MEM_PER_SLICE(devp)
//  usbport [get_dev_ctrl_usbport $devp;
  int port = get_dev_ctrl_port_num(devp);
  unsigned short sreg, wreg = (port%4) * MEM_PER_SLICE(devp) + WIR_MAC_STATUS0(devp);
  unsigned int sdata, wdata =  cpu_read(devp, port, wreg);
  printf("\nport %d wiremac status reg %x\n", port, wdata);
  if(wdata & 0x8) {
    printf("RxFcEn = 1\n");
  } else {
    printf("RxFcEn = 0\n");
  }
  if(wdata & 0x4) {
    printf("Full Duplex\n");
  } else {
    printf("Half Duplex\n");
  }
  if(wdata & 0x1) {
    printf("speed = 1000\n");
  } else if(wdata & 0x2) {
    printf("speed = 100\n");
  } else {
    printf("speed = 10\n");
  }
  sreg = (port%4) * MEM_PER_SLICE(devp)+SYS_MAC_STATUS0(devp);
  sdata =  cpu_read(devp, port, sreg);
  printf("\nport %d sysmac status reg %x\n", port, sdata);
  if(sdata & 0x10) {
    printf("TxFcEn = 1\n");
  } else {
    printf("TxFcEn = 0\n");
  }
  if(sdata & 0x4) {
    printf("Full Duplex\n");
  } else {
    printf("Half Duplex\n");
  }
  if(sdata & 0x1) {
    printf("port speed 1000\n");
  } else if(sdata & 0x2) {
    printf("port speed 100\n");
  } else {
    printf("port speed 10\n");
  }
  
}

typedef enum
{
	  SysMac,
	  WirMac
} MAC_INTERRUPT;

char *mac_intr_name [2] ={"SysMac", "WirMac"};

//# check mac_intr on all ports then clear intr bits
int check_mac_fifo(MAD_DEV * devp) {
//  global  MEM_PER_SLICE(devp)
//  global  WIR_MAC_INTR(devp)
//  global  SYS_MAC_INTR(devp)
  int rc, p;
  unsigned short addr;
  unsigned int ires;
  MAC_INTERRUPT mac = SysMac;
  printf("\nCheck mac interrupts.\n");
  rc = 0;
  while ( mac != WirMac) {
  for(p=0; p<devp->numOfPorts; p++) {
    addr = MEM_PER_SLICE(devp) * (p%4) + WIR_MAC_INTR(devp);
    if (mac==SysMac) { addr = MEM_PER_SLICE(devp) * (p%4) + SYS_MAC_INTR(devp); }

    ires =  cpu_read(devp, p, addr);
    if (ires & 1) {
      printf("%s %d RxFIFOOverrun\n", mac_intr_name[mac], p);;
      rc = -1;
    }
    if(ires & 2) {
      printf("%s %d TxFIFOOverrun\n", mac_intr_name[mac], p);;
      rc = -1;
    }
    if(ires & 4) {
      printf("%s %d TxFIFOUnderrun\n", mac_intr_name[mac], p);;
      rc = -1;
    }
    if(ires & 8) {
      printf("%s %d TxUnderrun\n", mac_intr_name[mac], p);;
      rc = -1;
    }
    msecWriteReg(devp, p,  addr, ires);
  }
  }
  return rc;
}

//# check lc_intr on one port then clear intr bits
int check_lc_intr_port(MAD_DEV * devp) {
//  global  MEM_PER_SLICE(devp)
//  global  LC_INTR(devp)
//  global  LC_INTR_MSK(devp)
//  global  VN_CFG_RUMI_REGS(devp)
  int i, rc = 0;
  unsigned short reg;
  unsigned int data, p = get_dev_ctrl_port_num(devp);
  printf("\nCheck port %d MACsec lc_intr.", p);
  reg = MEM_PER_SLICE(devp) * p + LC_INTR(devp);
  data =  cpu_read(devp, p, reg);
  printf("port%d lc_intr %X\n", p, data & 0x3F);
  printf("port%d lc_intr_msk %X\n", p, cpu_read(devp, p, LC_INTR_MSK(devp)) & 0x7F);
  for(i=0; i<6; i++) {
      switch (i) {
	  case 0: printf("replay counter almost full\n"); break;
      case 1: printf("replay counter full\n"); break;
      case 2: printf("packet number counter almost full\n"); break;
      case 3: printf("packet number counter full\n"); break;
      case 4: printf("isc sequence error\n"); break;
      case 5: printf("idr sequence error\n"); break;
	  default: break;
      }
      if (bit_get(data, i)) {
        printf(" intr active.\n");
        rc = 1;
      } else {
        printf(" intr inactive.\n");
      }
  }
  msecWriteReg(devp, p,  reg, data);
  return data;
}

//# check lc_intr on all ports then clear intr bits
int check_lc_intr(MAD_DEV * devp) {
//  global  MEM_PER_SLICE(devp)
//  global  LC_INTR(devp)
//  global  LC_INTR_MSK(devp)
//  global  VN_CFG_RUMI_REGS(devp)
  int i, p, rc = 0;
  unsigned short reg;
  unsigned int data;
  printf("\nCheck MACsec lc_intr all.\n");
  for(p=0; p<devp->numOfPorts; p++) {
    reg = MEM_PER_SLICE(devp) * (p%4) + LC_INTR(devp);;
    data =  cpu_read(devp, p, reg);
    printf("port%d lc_intr %X\n", p, data & 0x3F);
    printf("port%d lc_intr_msk %X\n", p, cpu_read(devp, p, LC_INTR_MSK(devp)) & 0x7F);
    for(i=0; i<6; i++) {
      switch (i) {
	  case 0: printf("replay counter almost full\n"); break;
      case 1: printf("replay counter full\n"); break;
      case 2: printf("packet number counter almost full\n"); break;
      case 3: printf("packet number counter full\n"); break;
      case 4: printf("isc sequence error\n"); break;
      case 5: printf("idr sequence error\n"); break;
	  default: break;
      }
      if (bit_get(data, i)) {
        printf(" intr active.\n");
        rc = 1;
      } else {
        printf(" intr inactive.\n");
      }
    }
    msecWriteReg(devp, p,  reg, data);
  }
  return rc;
}

//# Note: check src before get/clear intr bits.
int get_intr_src(MAD_DEV * devp, int port) {
//  global  INTR_SOURCE(devp)
//  global  VN_GLOBAL(devp)
  int i;
  unsigned short reg = INTR_SOURCE(devp);
  unsigned int data;
  printf("\nCheck Interrupt Source.\n");
  data = cpu_read(devp, port, reg) & 0x1F;
  printf("intr_src %X\n", data);
  for(i=0; i<devp->numOfPorts; i++) {
    if(bit_get(data, i)) {
      printf("port%d interrupt active.\n", i);
    } else {
      printf("port%d interrupt inactive.\n", i);
    }
  }
  if (bit_get(data, 4)) {
    printf("Interrupt from global_intr active.\n");
  } else {
    printf("Interrupt from global_intr inactive.\n");
  }
  return data;
}

//# check global_intr then clear intr bits
int check_global_intr(MAD_DEV * devp, int port) {
//  global  GLOBAL_INTR
//  global  GLOBAL_INTR_MSK(devp)
//  global  VN_GLOBAL(devp)
  int i;
  unsigned short reg = GLOBAL_INTR(devp);
  unsigned int data;
  printf("\nCheck Global Interrupt.\n");
  data =  cpu_read(devp, port, reg);
  printf("global_intr %X\n",  data & 0x7F);
  printf("global_intr_msk %X", cpu_read(devp, port, GLOBAL_INTR_MSK(devp)) & 0x7F);
  for(i=0; i<7; i++) {
    switch (i) {
     case 0: printf("macsec stats almost full\n"); break;
     case 1: printf("macsec stats full\n"); break;
     case 2: printf("MAC counter wrapped around\n"); break;
     case 3: printf("MAC counter address error\n"); break;
     case 4: printf("MAC MIB copy done\n"); break;
     case 5: printf("Rcvd bad flow ctrl frame w/ good CRC\n"); break;
     case 6: printf("Software test interrupt\n"); break;
	 default: break;
    }
    if(bit_get(data, i)) {
      printf(" intr active.\n");
    } else {
      printf(" intr inactive.\n");
    }
  }
  msecWriteReg(devp, port,  reg, data);
  return data;
}

void bist_test(MAD_DEV * devp) {
//  global  SLC_RAM_CTRL1(devp)
//  global  SLC_BIST_RESULT(devp)
//  global  MEM_PER_SLICE(devp)

//  foreach port [list 0 1 2 3] {
  int port;
  unsigned short reg;
  unsigned int rst, data;
  for(port=0; port<devp->numOfPorts; port++) {
    reg = (port%4) * MEM_PER_SLICE(devp) + SLC_RAM_CTRL1(devp);
    data =  cpu_read(devp, port, reg);
    msecWriteReg(devp, port,  reg, data | 0x1F00);
    rst =  cpu_read(devp, port, port * MEM_PER_SLICE(devp) + SLC_BIST_RESULT(devp));
    printf("slice bist test\n%d %x\n", port, rst);
    msecWriteReg(devp, port,  reg, data);

    //# macsec reg 0x19 bit4
    data = cpu_port_read(devp, port, 0x19);
    data = bit_set(data, 4);
    msecWriteReg(devp, port,  reg, data);
    rst  = cpu_port_read(devp, port, 0x1b);
    printf("macsec bist_test\n%d %x\n", port, rst);
    data = cpu_port_read(devp, port, 0x19);
    data = bit_clr(data, 4);
    msecWriteReg(devp, port,  reg, data);
  }

  //# global bist test 0x2100 bit 14:12
}


void toggle_smac_autoneg(MAD_DEV * devp) {
//  global  MEM_PER_SLICE(devp)
//  global  SYS_AUTO_NEG_CONTROL(devp)
//  global  SYS_HIDDEN0(devp)
//  global  VN_CFG_SYS_MAC
  int portnum;
  unsigned short reg;
  unsigned int sdata;

  for(portnum=0; portnum<devp->numOfPorts; portnum++) {
    reg = MEM_PER_SLICE(devp) * (portnum%4) + SYS_AUTO_NEG_CONTROL(devp);
    sdata =  cpu_read(devp, portnum, reg);
    printf("smac reg $reg = %d, before clear bit5\n", sdata);
    sdata = sdata & 0xFFDF;
    msecWriteReg(devp, portnum,  reg, sdata);
  }
  //# toggle bit10 of sys hidden0
  for(portnum=0; portnum<devp->numOfPorts; portnum++) {
    reg = MEM_PER_SLICE(devp) * (portnum%4) + SYS_HIDDEN0(devp);
    sdata =  cpu_read(devp, portnum, reg);
    printf("smac reg %d = %d, toggle bit10 from 1 to 0, then back to 1.\n", reg, sdata);
    sdata = sdata & 0xFFBFF;
    msecWriteReg(devp, portnum,  reg, sdata);
    sdata = sdata |MEM_PER_SLICE(devp);
    msecWriteReg(devp, portnum,  reg, sdata);
  }
  //# set bit5 of sysmac autoneg
  for(portnum=0; portnum<devp->numOfPorts; portnum++) {
    reg = MEM_PER_SLICE(devp) * (portnum%4) + SYS_AUTO_NEG_CONTROL(devp);
    sdata =  cpu_read(devp, portnum, reg);
    printf("smac reg %d = %d, set bit5 to 1.\n", reg, sdata);
    sdata = sdata | 0x20;
    msecWriteReg(devp, portnum,  reg, sdata);
  }
}
void toggle_wmac_autoneg(MAD_DEV * devp) {
//  global  MEM_PER_SLICE(devp)
//  global  WIR_AUTO_NEG_CONTROL(devp)
//  global  WIR_HIDDEN0(devp)
//  global  VN_CFG_WIR_MAC(devp)
  int portnum;
  unsigned short reg;
  unsigned int sdata;
  for(portnum=0; portnum<devp->numOfPorts; portnum++) {
    reg = MEM_PER_SLICE(devp) * (portnum%4) + WIR_AUTO_NEG_CONTROL(devp);
    sdata =  cpu_read(devp, portnum, reg);
    printf("wmac reg %d = %d, before clear bit5\n", reg, sdata);
    sdata = sdata & 0xFFDF;
    msecWriteReg(devp, portnum,  reg, sdata);
  }
  //# toggle bit10 of wire hidden0
  for(portnum=0; portnum<devp->numOfPorts; portnum++) {
    reg = MEM_PER_SLICE(devp) * (portnum%4) + WIR_HIDDEN0(devp);
    sdata =  cpu_read(devp, portnum, reg);
    printf("wmac reg reg %d = %d, toggle bit10 from 1 to 0, then back to 1.\n", reg, sdata);
    sdata = sdata & 0xFFBFF;
    msecWriteReg(devp, portnum,  reg, sdata);
    sdata = sdata | MEM_PER_SLICE(devp);
    msecWriteReg(devp, portnum,  reg, sdata);
  }
  //# set bit5 of wirmac autoneg
  for(portnum=0; portnum<devp->numOfPorts; portnum++) {
    reg = MEM_PER_SLICE(devp) * (portnum%4) + WIR_AUTO_NEG_CONTROL(devp);
    sdata =  cpu_read(devp, portnum, reg);
    printf("wmac reg reg %d = %d, set bit5 to 1.\n", reg, sdata);
    sdata = sdata | 0x20;
    msecWriteReg(devp, portnum,  reg, sdata);
  }
}

void set_mac_frame_size_limit(MAD_DEV * devp, int sizeLimit){
//  global  MEM_PER_SLICE(devp)
//  global  SYS_MAC_CONTROL0(devp)
//  global  WIR_MAC_CONTROL0(devp)
//  global  VN_CFG_SYS_MAC
//  global  VN_CFG_WIR_MAC(devp)
  int portnum;
  unsigned int sdata, wdata;
  for(portnum=0; portnum<devp->numOfPorts; portnum++) {
    wdata =  cpu_read(devp, portnum, (MEM_PER_SLICE(devp) * (portnum%4) + WIR_MAC_CONTROL0(devp)));
    wdata = (wdata & 0xF0003) | ((sizeLimit << 2) & 0xFFFC);
    msecWriteReg(devp, portnum,  (MEM_PER_SLICE(devp) * (portnum%4) + WIR_MAC_CONTROL0(devp)),wdata);

    sdata =  cpu_read(devp, portnum, (MEM_PER_SLICE(devp) * (portnum%4) + SYS_MAC_CONTROL0(devp)));
    sdata = (sdata & 0xF0003) | ((sizeLimit << 2) & 0xFFFC);
    msecWriteReg(devp, portnum,  (MEM_PER_SLICE(devp) * (portnum%4) + SYS_MAC_CONTROL0(devp)), sdata);
  }
}

void enable_non_disruptive_loopback(MAD_DEV * devp) {
//  global  MEM_PER_SLICE(devp)
//  global  SLC_CFG_GEN(devp)
//  global  VN_CFG_SLC(devp)
  int portnum;
  unsigned short reg;
  unsigned int data;
  for(portnum=0; portnum<devp->numOfPorts; portnum++) {
    reg = MEM_PER_SLICE(devp) * (portnum%4) ;
//    reg = reg + SLC_CFG_GEN(devp);
    reg = SLC_CFG_GEN(devp);
    data =  cpu_read(devp, portnum, reg);
    //# set bit5,4,3,2
    data |=  0x3C;
    msecWriteReg(devp, portnum,  reg, data);
  }
}

void set_elu_redir_to_imb(MAD_DEV * devp, int ent_num, int flag) {
//  global  MEM_PER_SLICE(devp)
//  global  VN_CFG_ELU(devp)
  int port;
  unsigned short reg;
  unsigned int data;
  reg = VN_CFG_ELU(devp) + ent_num * 8 + 3;
  port = get_dev_ctrl_port_num(devp);
  data  = cpu_port_read(devp, port, reg);
  //# set bit29 to redir to imb
  if(flag) {
     data = bit_set(data, 29);
  } else {
     data = bit_clr(data, 29);
  }
  msecWritePortReg(devp, port, reg, data);
}

//# set default_drop for igr and egr path
//# igr - igr_gen def_drop: 0 - pass, 1 - drop
//# egr - egr_gen def_drop: 0 - pass, 1 - drop
void set_def_drop(MAD_DEV * devp, int igr, int egr) {
//  global  CFG_EGR_GEN(devp)
//  global  CFG_IGR_GEN(devp)
//  global  MEM_PER_SLICE(devp)
  int portnum;
  unsigned int data;
  for(portnum=0; portnum<devp->numOfPorts; portnum++) {
    //# set IGR_GEN def_drop
    data  = cpu_port_read(devp, portnum, CFG_IGR_GEN(devp));
    if(igr) { 
	  data = bit_set(data, 0); 
	}
	else {
	  data = bit_clr(data, 0);
	}
    msecWritePortReg(devp, portnum, CFG_IGR_GEN(devp), data);
    //# set EGR_GEN def_drop
    data  = cpu_port_read(devp, portnum, CFG_EGR_GEN(devp));
    if(egr) { data = bit_set(data, 0); } else {data = bit_clr(data, 0);}
    msecWritePortReg(devp, portnum, CFG_EGR_GEN(devp), data);
  }
}

//# flag = b'11 store forward for both igr and egr
//# flag = b'00 cut through mode for both igr and egr
//# flag = b'01 egr store forward
//# flag = b'10 igr store forward
void set_store_forward_mode(MAD_DEV * devp, int flag){
//  global  MEM_PER_SLICE(devp)
//  global  SLC_CFG_GEN(devp)
  int portnum;
  unsigned int data;
  for(portnum=0; portnum<devp->numOfPorts; portnum++) {
    data =  cpu_read(devp, portnum, (MEM_PER_SLICE(devp) * (portnum%4) + SLC_CFG_GEN(devp)));
    //# set bit1,0
    data = (data & 0xFFC0) | flag;
    msecWriteReg(devp, portnum,  MEM_PER_SLICE(devp) * (portnum%4) + SLC_CFG_GEN(devp), data);
  }
}

void clear_macsec_stats(MAD_DEV *devp, int portnum) {
//  global  devp
//  global  NUM_OF_MACSEC_STAT
//  global  VN_SEC_STATS(devp)
  int i;
  for(i=(VN_SEC_STATS(devp)+NUM_OF_MACSEC_STAT(devp)*(portnum%4)); i<(VN_SEC_STATS(devp)+NUM_OF_MACSEC_STAT(devp)+NUM_OF_MACSEC_STAT(devp)*(portnum%4)); i++) {
	  msecWriteReg(devp, portnum,  i, 0);
  }
}

void clear_tables(MAD_DEV *devp, int portnum) {
//  global  devp
//  global  MEM_PER_SLICE(devp)
//  global  NUM_OF_SA(devp)
//  global  VN_CFG_ELU(devp)
//  global  VN_CFG_ILU(devp)

  //# clear ELU
  //#for {i = portnum * 0x400 + 0x80]} {$i<[expr $portnum * 0x400 + 0x0bd]} {incr i} {cpu_write(devp, $i 0}
  int e, w;
  unsigned int addr;
//  side = 0;
  for(e=0; e<NUM_OF_SA(devp); e++) {
//    cval [list]
    for(w=0; w<5; w++) {
     addr = (portnum%4) * MEM_PER_SLICE(devp) + VN_CFG_ELU(devp) +e*NUM_OF_SA(devp) + w;
      //#printf("Addr: %x\n", addr]
      wrchk_reg(devp, portnum, addr, 0);
    }
  }

  //# clear ILU
  for(e=0; e<NUM_OF_SA(devp); e++) {
//    cval [list]
    addr = (portnum%4) * MEM_PER_SLICE(devp) + VN_CFG_ILU(devp) + e*NUM_OF_SA(devp) + 7;
    msecWriteReg(devp, portnum,  addr, 0);
    //#for {w 0} {$w<8} {incr w} {
    //#  addr = portnum * 0x400 + 0x80 + 0x80 * $side+e*8+w]
      //#printf("Addr: %x\n", addr]
      //#wrchk_reg $addr 0
    //#}
  }
}

//# ported from ghinit, add duplex control
void shinit(MAD_DEV * devp, int speed, int duplex) {
//  global  devp	
//  global  SLC_RAM_CTRL1(devp)
//  global  SLC_BIST_RESULT(devp)
//  global  MEM_PER_SLICE(devp)
//  global  FIPS_CTL(devp)
//  global  RESET_CTL(devp)
//  global  STAT_GLOBAL(devp)
//  global  VN_GLOBAL(devp)
  //# fix for soft failure
  int portnum;
  for(portnum=0; portnum<devp->numOfPorts; portnum++) {
    echo_reg(devp, portnum, MEM_PER_SLICE(devp) * (portnum%4) + SLC_RAM_CTRL1(devp), 0x2406);
    print_reg(devp, portnum, MEM_PER_SLICE(devp) * (portnum%4) + SLC_BIST_RESULT(devp)); 
    echo_reg(devp, portnum, MEM_PER_SLICE(devp) * (portnum%4) + SLC_RAM_CTRL1(devp), 0x2006);
  }
  force_macsec_rst(devp);

 for(portnum=0; portnum<devp->numOfPorts; portnum++) {
  printf("port %d init\n", portnum);
  set_dev_ctrl_port_num(devp, portnum);
//  set_phy_speed_duplex(devp, speed "Full"
  msec_set_phy_speed_duplex(devp, portnum, speed, (duplex)?MAD_TRUE:MAD_FALSE);
  //#set_phy_speed_duplex(devp, $speed $duplex

  //# clear macsec stats
  clear_macsec_stats(devp, portnum);
  msec_clear_mac_stats(devp, portnum);

  //# take macsec out of reset
  msecWriteReg(devp, portnum,  MEM_PER_SLICE(devp) * (portnum%4) + FIPS_CTL(devp), 1);
  msecWriteReg(devp, portnum,  MEM_PER_SLICE(devp) * (portnum%4) + RESET_CTL(devp), 7);
  msecWriteReg(devp, portnum,  MEM_PER_SLICE(devp) * (portnum%4) + FIPS_CTL(devp), 0);
  msecWriteReg(devp, portnum,  MEM_PER_SLICE(devp) * (portnum%4) + RESET_CTL(devp), 0);
  clear_tables(devp, portnum);
//  set_macsec_speed_duplex(devp, $speed $duplex
  msec_set_phy_speed_duplex(devp, portnum, speed, (duplex)?MAD_TRUE:MAD_FALSE);
 }

  //#enable mac clear on read
  echo_reg(devp, portnum, STAT_GLOBAL(devp), 0x31);

  //# sanity check
  print_reg(devp, portnum, MEM_PER_SLICE(devp));
}


void clr_etm_cfg(MAD_DEV * devp, ETM_CFG_T * etm) {
  int eg_act=0, eg_en=0, ig_act=0, ig_en=0;
  int port, i;
//  foreach port [list 0 1 2 3] {
  for(port=0; port<devp->numOfPorts; port++) {
    set_dev_ctrl_port_num(devp, port);
    set_etm_cfg_egr_action(etm, eg_act);
    set_etm_cfg_egr_enable(etm, eg_en);
    set_etm_cfg_igr_action(etm, ig_act);
    set_etm_cfg_igr_enable(etm, ig_en);
    for(i=0; i<8; i++) {
      set_etm_cfg_ethertype(etm, 0, i);
    }
    msec_port_etm_cfg(devp, port, etm);
  }
}

void set_etm_cfg(MAD_DEV * devp, ETM_CFG_T * etm, int etype, int eg_act, int eg_en, int ig_act, int ig_en) {
  int port, i;
  for(port=0; port<devp->numOfPorts; port++) {
    set_dev_ctrl_port_num(devp, port);
    set_etm_cfg_egr_action(etm, eg_act);
    set_etm_cfg_egr_enable(etm, eg_en);
    set_etm_cfg_igr_action(etm, ig_act);
    set_etm_cfg_igr_enable(etm, ig_en);
    for(i=0; i<8; i++) {
      set_etm_cfg_ethertype(etm, 0, i);
    }
    msec_port_etm_cfg(devp, port, etm);
  }
}

void print_etm_cfg (MAD_DEV * devp, ETM_CFG_T * etm) {
  int i, port;
  int eg_act=0, eg_en=0, ig_act=0, ig_en=0;
  char etypeL[8][16];
  printf("  egr_act:  0 = bypass, 1 = drop  igr_act   0 = bypass, 1 = drop, 2 = redirect\n");
  printf(" port eg_en eg_act ig_en ig_act ethertype\n");

  printf("============================================\n");
  for(port=0; port<devp->numOfPorts; port++) {
    set_dev_ctrl_port_num(devp, port);
    msec_port_get_etm_cfg(devp, port, etm);
    eg_act  = get_etm_cfg_egr_action(etm);
    eg_en  = get_etm_cfg_egr_enable(etm);
    ig_act  = get_etm_cfg_igr_action(etm);
    ig_en  = get_etm_cfg_igr_enable(etm);
    printf("%4d %6X %7d %6X %7d \n", port, eg_en, eg_act, ig_en, ig_act);
    for(i=0; i<8; i++) {
      sprintf(etypeL[i], "%X", get_etm_cfg_ethertype(etm, i));
    }
    for(i=0; i<8; i++) {
      printf("%s\n", etypeL[i]);
    }
  }
}

void print_elu_tbl(MAD_DEV * devp) {
//  global  MEM_PER_SLICE(devp)
//  global  VN_CFG_ELU(devp)
//  global  NUM_OF_SA(devp)

//  set dev_num [get_dev_ctrl_dev_num $devp]
  int port;
  int addr;
  int dev_num = get_dev_ctrl_dev_num(devp);
  printf("dev_num = %d\n", dev_num);
  for(port=0; port<devp->numOfPorts; port++) {
    printf("\nport%d Egress Lookup(ELU) Table\n", port);
    for(addr=(port%4)*MEM_PER_SLICE(devp)+VN_CFG_ELU(devp); addr <=((port%4)*MEM_PER_SLICE(devp)+VN_CFG_ELU(devp)+NUM_OF_SA(devp)*8-1); addr++) {
      if((addr%8) == 0) {
        printf("\n%X: %X\n", addr, cpu_read(devp, port, addr));
      } else if(((addr+1)%8)==0) {
        printf(" %X\n", cpu_read(devp, port, addr));
      } else {
        printf(" %X\n", cpu_read(devp, port, addr));
      }
    }
  }
}
void print_egr_ctx_tbl(MAD_DEV * devp) {
//  global  MEM_PER_SLICE(devp)
//  global  VN_CFG_ELU_CTX(devp)
//  global  NUM_OF_SA(devp)
//  set dev_num [get_dev_ctrl_dev_num $devp]
  int port;
  int addr;
  int dev_num = get_dev_ctrl_dev_num(devp);
  printf("dev_num = %d\n", dev_num);
  for(port=0; port<devp->numOfPorts; port++) {
    printf("\nport%d Egress Context Table\n", port);
    for(addr=(port%4)*MEM_PER_SLICE(devp)+VN_CFG_ELU_CTX(devp); addr<= (port%4) * MEM_PER_SLICE(devp)+VN_CFG_ELU_CTX(devp)+NUM_OF_SA(devp) * 8 -1; addr++) {
      if((addr%8) == 0) {
        printf("\n%X: %X\n", addr, cpu_read(devp, port, addr));
      } else if(((addr+1)%8)==0) {
        printf(" %X\n", cpu_read(devp, port, addr));
      } else {
        printf(" %X\n", cpu_read(devp, port, addr));
      }
    }
  }
}
void print_ilu_tbl(MAD_DEV * devp) {
//  global  MEM_PER_SLICE(devp)
//  global  VN_CFG_ILU(devp)
//  global  NUM_OF_SA(devp)
//  set dev_num [get_dev_ctrl_dev_num $devp]
  int port;
  int addr;
  int dev_num = get_dev_ctrl_dev_num(devp);
  printf("dev_num = %d\n", dev_num);
  for(port=0; port<devp->numOfPorts; port++) {
    printf("\nport%d Ingress Lookup(ILU) Table\n", port);
    for(addr=(port%4)*MEM_PER_SLICE(devp)+VN_CFG_ILU(devp); addr <=((port%4)*MEM_PER_SLICE(devp)+VN_CFG_ILU(devp)+NUM_OF_SA(devp)*8-1); addr++) {
      if((addr%8) == 0) {
        printf("\n%X: %X\n", addr, cpu_read(devp, port, addr));
      } else if(((addr+1)%8)==0) {
        printf(" %X\n", cpu_read(devp, port, addr));
      } else {
        printf(" %X\n", cpu_read(devp, port, addr));
      }
    }
  }
}
void print_enc_key_tbl(MAD_DEV * devp) {
//  global MEM_PER_SLICE(devp)
//  global VN_CFG_EGR_KEY(devp)
//  global NUM_OF_SA(devp)
//  set dev_num [get_dev_ctrl_dev_num $devp]
  int port;
  int addr;
  int dev_num = get_dev_ctrl_dev_num(devp);
  printf("dev_num = %d\n", dev_num);
  for(port=0; port<devp->numOfPorts; port++) {
    printf("\nport%d Encrypt Key Table\n", port);
    for(addr=(port%4)*MEM_PER_SLICE(devp)+VN_CFG_EGR_KEY(devp); addr <= (port%4) * MEM_PER_SLICE(devp)+VN_CFG_EGR_KEY(devp) + 0x1F; addr++) {
      if((addr%8) == 0) {
        printf("\n%X: %X\n", addr, cpu_read(devp, port, addr));
      } else if(((addr+1)%8)==0) {
        printf(" %X\n", cpu_read(devp, port, addr));
      } else {
        printf(" %X\n", cpu_read(devp, port, addr));
      }
    }
  }
}
void print_dec_key_tbl(MAD_DEV * devp) {
//  global MEM_PER_SLICE(devp)
//  global VN_CFG_IGR_KEY(devp)
//  global NUM_OF_SA(devp)
//  set dev_num [get_dev_ctrl_dev_num $devp]
  int port;
  int addr;
  int dev_num = get_dev_ctrl_dev_num(devp);
  printf("dev_num = %d\n", dev_num);
  for(port=0; port<devp->numOfPorts; port++) {
    printf("\nport%d Decrypt Key Table\n", port);
    for(addr=(port%4)*MEM_PER_SLICE(devp)+VN_CFG_IGR_KEY(devp); addr <=((port%4) * MEM_PER_SLICE(devp) +VN_CFG_IGR_KEY(devp) + 0x1F); addr++) {
      if((addr%8) == 0) {
        printf("\n%X: %X\n", addr, cpu_read(devp, port, addr));
      } else if(((addr+1)%8)==0) {
        printf(" %X\n", cpu_read(devp, port, addr));
      } else {
        printf(" %X\n", cpu_read(devp, port, addr));
      }
    }
  }
}

void dump_macsec_regs(MAD_DEV * devp) {
//  global MEM_PER_SLICE(devp)
//  global VN_CFG_RUMI_REGS(devp)       
//  global VN_CFG_EGR_KEY(devp)        
//  global VN_CFG_EGR_HKEY(devp)      
//  global VN_CFG_ELU(devp)          
//  global VN_CFG_ILU(devp)         
//  global VN_CFG_IGR_KEY(devp)    
//  global VN_CFG_IGR_HKEY(devp)  
//  global VN_CFG_RPY(devp)      
//  global VN_CFG_WIR_MAC(devp) 
//  global VN_CFG_SYS_MAC
//  global VN_CFG_SLC(devp)   
//  global VN_CFG_CIP(devp)  
//  global VN_CFG_ELU_CTX(devp)
//  global MEM_PER_SA(devp)
//  global NUM_OF_SA(devp)
  int port;
  int addr;
  port = get_dev_ctrl_port_num(devp);
  printf("\nport %d register dump.\n", port);
  printf("===========================================");
  printf("\nLinkCrypt General registers");
  for(addr=(port%4)*MEM_PER_SLICE(devp)+VN_CFG_RUMI_REGS(devp); addr< (port%4) * MEM_PER_SLICE(devp)+VN_CFG_RUMI_REGS(devp)+MEM_PER_SA(devp); addr++) {
    printf("%X: %X\n", addr, cpu_read(devp, port, addr));
  }
  printf("\nEncrypt Key Table");
  for(addr=(port%4)*MEM_PER_SLICE(devp)+VN_CFG_EGR_KEY(devp); addr< (port%4) * MEM_PER_SLICE(devp)+VN_CFG_EGR_HKEY(devp); addr++) {
    printf("%X: %X\n", addr, cpu_read(devp, port, addr));
  }
  printf("\nDecrypt Key Table");
  for(addr=(port%4)*MEM_PER_SLICE(devp)+VN_CFG_IGR_KEY(devp); addr< (port%4) * MEM_PER_SLICE(devp)+VN_CFG_IGR_HKEY(devp); addr++) {
    printf("%X: %X\n", addr, cpu_read(devp, port, addr));
  }
  printf("\nEgress Hash Key Table");
  for(addr=(port%4)*MEM_PER_SLICE(devp)+VN_CFG_EGR_HKEY(devp); addr< (port%4) * MEM_PER_SLICE(devp)+VN_CFG_EGR_HKEY(devp)+MEM_PER_SA(devp); addr++) {
    printf("%X: %X\n", addr, cpu_read(devp, port, addr));
  }
  printf("\nIngress Hash Key Table");
  for(addr=(port%4)*MEM_PER_SLICE(devp)+VN_CFG_IGR_HKEY(devp); addr< (port%4) * MEM_PER_SLICE(devp)+VN_CFG_IGR_HKEY(devp) + 0x20; addr++) {
    printf("%X: %X\n", addr, cpu_read(devp, port, addr));
  }
  printf("\nEgress Lookup(ELU) Table");
  for(addr=(port%4)*MEM_PER_SLICE(devp)+VN_CFG_ELU(devp); addr< (port%4) * MEM_PER_SLICE(devp)+VN_CFG_ELU(devp) + 0x7F; addr++) {
    printf("%X: %X\n", addr, cpu_read(devp, port, addr));
  }
  printf("\nEgress Context Table");
  for(addr=(port%4)*MEM_PER_SLICE(devp)+VN_CFG_ELU_CTX(devp); addr< (port%4) * MEM_PER_SLICE(devp)+VN_CFG_ELU_CTX(devp)+NUM_OF_SA(devp) * 8 -1; addr++) {
    printf("%X: %X\n", addr, cpu_read(devp, port, addr));
  }
  printf("\nIngress Lookup(ILU) Table");
  for(addr=(port%4)*MEM_PER_SLICE(devp)+VN_CFG_ILU(devp); addr< (port%4) * MEM_PER_SLICE(devp)+VN_CFG_ILU(devp)+NUM_OF_SA(devp) * 8 -1; addr++) {
    printf("%X: %X\n", addr, cpu_read(devp, port, addr));
  }
  printf("\nIngress Replay Values");
  for(addr=(port%4)*MEM_PER_SLICE(devp)+VN_CFG_RPY(devp); addr< (port%4) * MEM_PER_SLICE(devp)+VN_CFG_RPY(devp) + 0x7; addr++) {
    printf("%X: %X\n", addr, cpu_read(devp, port, addr));
  }
  printf("\nWire MAC Registers");
  for(addr=(port%4)*MEM_PER_SLICE(devp)+VN_CFG_WIR_MAC(devp); addr< (port%4) * MEM_PER_SLICE(devp)+VN_CFG_WIR_MAC(devp) + 8; addr++) {
    printf("%X: %X\n", addr, cpu_read(devp, port, addr));
  }
  printf("\nSlice Config Registers");
  for(addr=(port%4)*MEM_PER_SLICE(devp)+VN_CFG_SLC(devp); addr< (port%4) * MEM_PER_SLICE(devp)+VN_CFG_SLC(devp) + 0xE; addr++) {
    printf("%X: %X\n", addr, cpu_read(devp, port, addr));
  }
  printf("\nSystem MAC Registers");
  for(addr=(port%4)*MEM_PER_SLICE(devp)+VN_CFG_SYS_MAC(devp); addr< (port%4) * MEM_PER_SLICE(devp)+VN_CFG_SYS_MAC(devp) + 8; addr++) {
    printf("%X: %X\n", addr, cpu_read(devp, port, addr));
  }
  printf("\nGlobal registers");
  for(addr=0x1100; addr <= 0x11FF; addr++) {
    printf("%X: %X\n", addr, cpu_read(devp, port, addr));
  }
}

//# clear bit13 to enable ptp parsing, set bit13 disable ptp parsing
void set_macsec_parse_ptp(MAD_DEV * devp, int flag) {
//  global usbport
//  global SLC_RAM_CTRL1(devp)
  int port;
  unsigned short reg;
  unsigned int data;

  reg = SLC_RAM_CTRL1(devp);
  port = get_dev_ctrl_port_num(devp);
  data  = cpu_port_read(devp, port, reg);
  if(flag) {
    data = bit_clr(data, 13);
  } else {
    data = bit_set(data, 13);
  }
  msecWritePortReg(devp, port, reg, data);
}


void cfg_egr_ent(MAD_DEV * devp, int ent_num, int key_in, long long sci, int pn, int tci) {
//  global devp
//  global portList
//  global hResult
  int en_def = 1;
  int en_mc = 0;
  int en_da = 0;
  int en_sa = 0;
  int en_vlan= 0;
  int en_et = 0;
  int en_scian = 0;
  int en_hdr = 1;
  MATCH_EN *lk_match_en ;

  long long  da_fld = 0ll;
  long long  sa_fld = 0ll;
  int vlan_fld = 0;
  int et_fld = 0;
  long long sci_fld = 0ll;
  int tci_fld = 0;
  int tci_msk_fld = 0;
  int conf_offset = 0;
  int hdr_fld = 1;
  MATCH_FLD * lk_match_fld;

  int sec_level = 0x3;
  int ctx_num = sec_level;
  int drop = 0;
  int redir = 0;
  int auth_en = 1;
  int enc_en = 1;
  int drop_maxpn = 1;
  ACT_FLD * lk_act_fld;

  LKUP_T * elk_t;
  CTX_T * ect;
  unsigned int *eports = portList;
  int eport;
  int i;

  lk_match_en = new_match_en(en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian, en_hdr);
  printf("Setup ELU match enables:\n");
  printf("ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian\n");
  printf("%7d %6d %5d %5d %5d %7d %5d %8d\n", ent_num, en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian);
  //# setup ELU match fields
  lk_match_fld = new_match_fld(da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld, conf_offset, hdr_fld);
  printf("Setup ELU match fields:\n");
  printf("ent_num da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld conf_offset\n");
  printf("%7d %6d %6d %8d %6d %7d %7d %10d %11d\n", ent_num, da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld, conf_offset);
  //# setup ELU action
  lk_act_fld = new_act_fld(ctx_num, drop_maxpn, drop, redir, auth_en, enc_en);
  printf("Setup ELU action:\n");
  printf("ent_num ctx_num drop redir auth_en enc_en\n");
  printf(" %7d %7d %4d %5d %7d %6d\n", ent_num, ctx_num, drop, redir, auth_en, enc_en);
  //# setup LKUP_T
  elk_t = new_lkup_t(lk_match_en, lk_match_fld, lk_act_fld);
  printf("Setup Lookup Table as configured above.\n");
  
  ect = new_ctx_ent(ent_num, key_in, sci, pn, tci, conf_offset);
  //# Setup EGR entry 
  //# setup ctrl port_num
  for(i=0;i<4; i++) {
	eport = eports[i];
    set_dev_ctrl_dev_num(devp, eport / 4);
    set_dev_ctrl_smiaddr(devp, eport / 4 * 4);//#0,4,8,12,etc
    set_dev_ctrl_port_num(devp, eport % 4);
    msec_port_set_egr_entry(devp, eport % 4, ent_num, elk_t, ect);
    printf("Setup Egress port %d  EGR entry %d as configured above.\n", eports[i], ent_num);
  }

  //# release memory for ct, elk_t
  delete_match_en(lk_match_en);  //
  delete_match_fld(lk_match_fld);  //
  delete_act_fld(lk_act_fld);  //
  delete_lkup_t(elk_t);  //

  delete_ctx_t(ect);  //
}

//###########################################################
// setup Ingress 
//#########################################################
void cfg_igr_ent(MAD_DEV * devp, int ent_num, int key_in, long long sci, int pn, int tci) {
//  global devp
//  global portList
//  global hResult
  //# setup ILU match enables
  int en_def = 0;
  int en_mc = 0;
  int en_da = 0;
  int en_sa = 0;
  int en_vlan= 0;
  int en_et = 0;
  int en_scian = 1;
  int en_hdr = 1;
  MATCH_EN *lk_match_en;

  int da_fld = 0;
  int sa_fld = 0;
  int vlan_fld = 0;
  int et_fld = 0;
  long long  sci_fld = sci;
  int tci_fld = tci;
  int tci_msk_fld = 0xFF;
  int conf_offset = 0;
  int hdr_fld = 1;
  MATCH_FLD * lk_match_fld;

  int sec_level = 0x3;
  int ctx_num = ent_num;
  int drop = 0;
  int redir = 0;
  int auth_en = 1;
  int enc_en = 1;
  int drop_maxpn = 1;
  ACT_FLD * lk_act_fld;
  
  LKUP_T * ilk_t;
  CTX_T * ict;
  unsigned int *iports = portList;
  int iport;
  int i;
  
  lk_match_en = new_match_en(en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian, en_hdr);
  printf("Setup ILU match enables:\n");
  printf("ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian\n");
  printf("%7d %6d %5d %5d %5d %7d %5d %8d\n", ent_num, en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian);
  //# setup ILU match fields
  lk_match_fld = new_match_fld(da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld, conf_offset, hdr_fld);
  printf("Setup ILU match fields:\n");
  printf("ent_num da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld conf_offset\n");
  printf("%7d %6d %6d %8d %6d %7d %7d %10d %11d\n", ent_num, da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld, conf_offset);
 //# setup ILU action
  lk_act_fld = new_act_fld(ctx_num, drop_maxpn, drop, redir, auth_en, enc_en);
  printf("Setup ILU action:\n");
  printf("ent_num ctx_num drop redir auth_en enc_en\n");
  printf(" %7d %7d %4d %5d %7d %6d\n", ent_num, ctx_num, drop, redir, auth_en, enc_en);
  //# setup LKUP_T
  ilk_t = new_lkup_t(lk_match_en, lk_match_fld, lk_act_fld);
  printf("Setup ILU Lookup Table as configured above.\n");

//# setup ICT"
  sci = 0ll;
  tci = 0;
  pn = 1;
  ict = new_ctx_ent(ent_num, key_in, sci, pn, tci, conf_offset);
//# setup ctrl port_num
  for(i=0;i<4; i++) {
	iport = iports[i];
    set_dev_ctrl_dev_num(devp, iport / 4);
    set_dev_ctrl_smiaddr(devp, iport / 4 * 4);//#0,4,8,12,etc
    set_dev_ctrl_port_num(devp, iport % 4);
    msec_port_set_egr_entry(devp, iport % 4, ent_num, ilk_t, ict);
    printf("Setup Egress port %d  IGR entry %d as configured above.\n", iport, ent_num);
  }

  //# release memory for key, ct, 
  //#delete_u8_arr $keyArr
  delete_match_en(lk_match_en);  //
  delete_match_fld(lk_match_fld);  //
   delete_act_fld(lk_act_fld);  //
   delete_lkup_t(ilk_t);  //
  delete_ctx_t(ict);
}

typedef enum {
	MSEC_IGR,
	MSEC_EGR
} MSEC_GR_TYPE;

void cfg_macsec_ent(MAD_DEV * devp, MSEC_GR_TYPE gr, int ent_num, int key_in, long long sci, int pn, int tci) {
 if(gr==MSEC_EGR) {
   //# configure egress entry
   cfg_egr_ent(devp, ent_num, key_in, sci, pn, tci);
 } else if (gr==MSEC_IGR) {
   cfg_igr_ent(devp,ent_num, key_in, sci, pn, tci);
 }
}


void parseportcfg(int title, int fid) {
#if 1

  devList =          0x1;
//  portList =         0xf;  /* port 0,1,2,3 */
  init_speed =       Msec_Speed_Gigabit;

  printf("devList = %x\n", devList);
  printf("portList = %02d %02d %02d %02d\n", portList[0], portList[1], portList[2], portList[3]);
//  printf("speedList = %x\n", speedList);
//  printf("bypassmsecList = %x\n", bypassmsecList);
//  printf( "flowctrlList = %x\n", flowctrlList);
  printf("init_speed = %x\n", init_speed);

#else
//  global devp
//  global portList
//  global devList
//  global speedList
//  global bypassmsecList
//  global flowctrlList
//  global delim
//  global init_speed
//  global hResult
  Debug "parseportcfg"
  flush stdout
  portList [list];  speedList [list];
  bypassmsecList [list]; flowctrlList [list]
  while {[gets $fid line] >= 0} {
    Debug "pline=$line"
    if {[string compare [string range $line 0 9] $delim] != 0} {
      for {i 0} {$i < [llength $line]} {incr i} {
        fieldTitle [string totitle [lindex $title $i]] 
	fieldValue($fieldTitle) [lindex $line $i]
	//# The fieldTitle has the first letter capitalized and rest letters in lower case.
        switch $fieldTitle {
	port {
	  port $fieldValue($fieldTitle)
          lappend portList $port
	  }
        Bypassmsec {
            if {[regexp "def_msec" $fieldValue($fieldTitle)]} {
              bypassmsec 0
            } else {
              bypassmsec $fieldValue($fieldTitle)
	    }
	    lappend bypassmsecList $bypassmsec
	}
	Speed   {
	  speed $fieldValue($fieldTitle)
          lappend speedList $speed
          }
        Flowctrl {
          if {[regexp "def_rc" $fieldValue($fieldTitle)] == 0} {
            flowctrl 0
          } else {
            flowctrl $fieldValue($fieldTitle)
	  }
	  lappend flowctrlList $flowctrl 
	}
	default  { Error "Invalid field $fieldTitle";break; }
        }
      }
    } else {
       break	
    }
  }; //# endof while loop
  switch [lindex $speedList 0] {    
    def_speed { init_speed 2}
    Gigabit { init_speed 2 }
    100MB { init_speed 1 }
    10MB { init_speed 0 }
    default { init_speed 2 }
  }
  foreach port $portList {
    dev_num = port / 4]
    if {[lsearch $devList $dev_num] == -1} {
      lappend devList $dev_num
    }
  }
  Debug "devList = $devList"
  Debug "portList = $portList"
  Debug "speedList = $speedList"
  Debug "bypassmsecList = $bypassmsecList"
  Debug  "flowctrlList = $flowctrlList"
  Debug "init_speed = $init_speed"

    macsec_start(devp, $init_speed
#endif

}

void macsec_start_all (MAD_DEV * devp) {
#if 1
  int port;
//  int bypassmsec = 0;
  int flowctrl = 1;

  macsec_start(devp, init_speed);
  for(port=0; port<devp->numOfPorts; port++) {

//    set_phy_bypass_macsec(port, bypassmsec);
    if (flowctrl) {
      force_flow_ctrl_en(devp);
    } else {
      force_flow_ctrl_dis(devp);
    }
  }
#else

//  global devp
//  global portList
//  global devList
//  global speedList
//  global bypassmsecList
//  global flowctrlList
//  global init_speed
//  global hResult
  printf(" initialize macsec module with initial speed $init_speed\n");
  foreach dev_num $devList {
    set_dev_ctrl_dev_num(devp, $dev_num
    set_dev_ctrl_smiaddr(devp, dev_num * 4]
    set_dev_ctrl_port_num(devp, 0
    macsec_start(devp, $init_speed
  }
  printf("port bypass_msec init_speed flow_ctrl"
  foreach port $portList bypassmsec $bypassmsecList flowctrl $flowctrlList {
    //# set def value if not updated by application.
    dev_num = port / 4]
    set_dev_ctrl_dev_num(devp, $dev_num
    set_dev_ctrl_smiaddr(devp, dev_num * 4]
    set_phy_bypass_macsec $port $bypassmsec
    printf("%-4d %-10d %-8d %-8d\n", port, bypassmsec, init_speed, flowctrl]
    if(0) {
    if {$flowctrl} {
      force_flow_ctrl_en $devp
    } else {
      force_flow_ctrl_dis $devp
    }
    }
  }
#endif
}

void parse_entry_cfg (MAD_DEV *devp, int title, int fid) {
#if 1
  //# configure egr or igr entry
  MSEC_GR_TYPE gr = MSEC_IGR;
  int ent=0, key=0, sci=0ll, pn=0, tci=0;
  //gr = MSEC_IGR/MSEC_EGR  Xu
  cfg_macsec_ent(devp, gr,ent, key, sci, pn, tci);

#else
//  global delim
  Debug "parse_entry_cfg"
  flush stdout
  while {[gets $fid line] >= 0} {
    //#Debug "pline=$line"
    if {[string compare [string range $line 0 9] $delim] != 0} {
      for {i 0} {$i < [llength $line]} {incr i} {
        fieldTitle [string totitle [lindex $title $i]] 
	fieldValue($fieldTitle) [lindex $line $i]
	//# The fieldTitle has the first letter capitalized and rest letters in lower case.
        switch $fieldTitle {
	Entry {
	     if [regexp {^(e|E|i|I).*([0-7])} $fieldValue($fieldTitle) match gr ent] { 
              Debug "gr=$gr, ent=$ent"
             } else { printf("invalid fieldValue $fieldValue($fieldTitle)"; return -1 }
             }
        Key {
            if [regexp {([A-Fa-f0-9]+)} $fieldValue($fieldTitle) match key] {
              Debug "key=$key"
            } else { printf("invalid fieldValue $fieldValue($fieldTitle)"; return -1 }
          }
        Sci {
            if [regexp {([A-Fa-f0-9]+)} $fieldValue($fieldTitle) match sci] {
              Debug "sci=$sci"
            } else { printf("invalid fieldValue $fieldValue($fieldTitle)"; return -1 }
          }
        Pn {
            if [regexp {([0-9]+)} $fieldValue($fieldTitle) match pn] {
              Debug "pn=$pn"
            } else { printf("invalid fieldValue $fieldValue($fieldTitle)"; return -1 }
          }
        Tci {
            if [regexp {([0-9A-Fa-f]+)} $fieldValue($fieldTitle) match tciStr] {
              Debug "tci=$tciStr"
	      scan $tciStr %x tci
            } else { printf("invalid fieldValue $fieldValue($fieldTitle)"; return -1 }
          }
        }
       }
       //# configure egr or igr entry
	  //gr = MSEC_IGR/MSEC_EGR  Xu
       cfg_macsec_ent $gr $ent, key, sci, pn, tci
    } else { return 0 }
  }
#endif
} 

int is_elu_en(MAD_DEV * devp, int port, int ent_num) {
	int elu_en;
    unsigned short addr;
    unsigned int reg;
 //  global MEM_PER_SLICE(devp)
 //  global VN_CFG_ELU(devp)
   addr = VN_CFG_ELU(devp)+(port%4) * MEM_PER_SLICE(devp)+ent_num * 8 + 4;
   reg  = cpu_port_read(devp, port, addr);
   elu_en = msec_bits_get(reg, 31, 24);
   return elu_en;
}

#if 0

proc rotate_elu_0_1(MAD_DEV * devp devList} {
//  global hResult
  foreach dev_num $devList {
    iports [list 0 1 2 3]
    eports [list 0 1 2 3]
    smiaddr = dev_num * 4]
    set_dev_ctrl_dev_num(devp, $dev_num
    set_dev_ctrl_smiaddr(devp, $smiaddr
    //# first find the current active entry to rotate from;
    //# if ent_num 0 is enabled, then enable ent_num 1 and disable ent_num 0
    //# else enable ent_num 0 and disable ent_num 1
    if {[is_elu_en(devp, 0 0]} {
      old_ent 0
      new_ent 1
    } else {
      old_ent 1
      new_ent 0
    }
    foreach eport $eports iport $iports {
      //# enable default for new entry
      msec_port_set_elu_match_en_en_def(devp, $eport $new_ent 1      
      //# disable old ELU and ILU entries
      msec_port_dis_elu_ent(devp, $eport, old_ent
      msec_port_set_ect_pn(devp, eport, old_ent 1
      //#msec_port_dis_ilu_ent(devp, $iport $old_ent
    }
  }
}
//# find out the one active entry and rotate to next entry
proc rotate_egr_igr_ent(MAD_DEV * devp devList} {
 //  global MEM_PER_SLICE(devp)
 //  global VN_CFG_ELU(devp)
//  global hResult
  sec_level 0x3
  foreach dev_num $devList {
    iports [list 0 1 2 3]
    eports [list 0 1 2 3]
    smiaddr = dev_num * 4]
    set_dev_ctrl_dev_num(devp, $dev_num
    set_dev_ctrl_smiaddr(devp, $smiaddr
    //# first find the current active entry to rotate from;
    //# then find the next free entry to rotate to
    old_ent -1; new_ent -1;
    for {port 0 } {$port < 4} {incr port} {
      for {ent_num 0} {$ent_num < 8} {incr ent_num} {
        addr = VN_CFG_ELU(devp)+(port%4) * MEM_PER_SLICE(devp)+ent_num * 8 + 4]
        reg  = cpu_port_read(devp, port, addr);
        elu_en = msec_bits_get(reg, 31, 24);
        if {$elu_en != 0 && $old_ent == -1} {
          old_ent $ent_num
        } elseif {$old_ent != -1 && $elu_en == 0} {
          new_ent $ent_num
	  break
        }
      }
      if {$new_ent != -1 && $old_ent != -1} {
        printf("prepare to rotate keys from old_ent $old_ent to new_ent $new_ent."
        break
      } else {
        printf("Failed to rotate entry due to invalid old_ent $old_ent, or new_ent $new_ent."
        return
      }
   }

   //# create new entry
   keystr [gen_random_hex_octs 16]
   key [create_u8_arr 16]
   m 0
   foreach key_fld $keystr {
      set_u8_arr_fld $key $m $key_fld
      incr m
    }
   sci [gen_random_sci]
   tci 0x2c
   pn 1
   //# actions shared by ELU and ILU
   drop 0; redir 0; auth_en 1; enc_en 1; drop_maxpn 1
   //#elk match enables
   en_def 1;en_mc 0;en_da 0;en_sa 0;en_vlan 0;en_et 0;en_scian 0;
   da_fld 0;sa_fld 0;et_fld 0;vlan_fld 0; sci_fld 0;tci_fld 0;tci_msk_fld 0; conf_off0
   elk_match_en [new_match_en, en_def, en_mc, en_da, en_sa, en_vlan, en_et, en_scian]
   elk_match_fld [new_match_fld $da_fld, sa_fld, vlan_fld, et_fld, sci_fld, tci_fld, tci_msk_fld, conf_offset]
      ect [new_ctx_t, sci, pn, tci, conf_off$key]
      //# setup ELU action
      ctx_num $new_ent
      ctx_num $sec_level
      elk_act_fld [new_act_fld $ctx_num, drop_maxpn, drop, redir, auth_en, enc_en]

      //# setup LKUP_T
      elk_t [new_lkup_t, elk_match_en, elk_match_fld, elk_act_fld]

      //# setup ICT
      ict [new_ctx_t, sci, pn, tci, conf_off$key]
      //# setup ILU match enables to match scian only
      ilk_match_en [new_match_en 0 0 0 0 0 0 1]
      //# setup ILU match fields 
      ilk_match_fld [new_match_fld, da_fld, sa_fld, vlan_fld, et_fld, sci, tci 0xff, conf_offset]

      //# setup ILU action
      ctx_num $new_ent
      ctx_num $sec_level
      ilk_act_fld [new_act_fld $ctx_num, drop_maxpn, drop, redir, auth_en, enc_en]
      //# setup LKUP_T
      ilk_t [new_lkup_t $ilk_match_en, ilk_match_fld, ilk_act_fld]
    foreach eport $eports iport $iports {
      //# egress port configuration
      msec_port_set_egr_entry(devp, $eport, new_ent, elk_t, ect
      printf("%7d %7d %5d \n", new_ent, ctx_num, eport]

      //# ingress port configuration
      //# Setup IGR entry 
      msec_port_set_igr_entry(devp, iport, new_ent, ilk_t, ict
      printf("%5d %3d %3X %18s %s\n", iport, pn, tci, sci, keystr]
    }

    printf("\nDisable old entry $old_ent on both egress and ingress."
    foreach eport $eports iport $iports {
      //# disable old ELU and ILU entries
      msec_port_dis_elu_ent(devp, $eport, old_ent
      msec_port_dis_ilu_ent(devp, iport, old_ent
    }
  }
}
#endif

void recover_locked_port(MAD_DEV * devp, int port) {
//  global SYS_HIDDEN0(devp)
//  global WIR_HIDDEN0(devp)
//  global SLC_CFG_GEN(devp)
//  global RESET_CTL(devp)
 
  //# link down sysmac and wiremac
  msecWritePortReg(devp, port, SYS_HIDDEN0(devp), cpu_port_read(devp, port, SYS_HIDDEN0(devp))& 0xFFFBF);
  msecWritePortReg(devp, port, WIR_HIDDEN0(devp), cpu_port_read(devp, port, WIR_HIDDEN0(devp))& 0xFFFBF);
  //# reindividual segments - toggle igrportreset, igr_fifo_reset, igr_pre_reset
  msecWritePortReg(devp, port, SLC_CFG_GEN(devp), cpu_port_read(devp, port, SLC_CFG_GEN(devp))| 0xa100);
  //# toggle reset_ctrl bring out igr_reset
  msecWritePortReg(devp, port, RESET_CTL(devp), 0x4);
  
  //# bring out segment reset
  msecWritePortReg(devp, port, SLC_CFG_GEN(devp), cpu_port_read(devp, port, SLC_CFG_GEN(devp))& 0xFF);
  //# toggle reset_ctrl bring out igr_reset
  msecWritePortReg(devp, port, RESET_CTL(devp), 0);
  //# link up sysmac and wiremac
  msecWritePortReg(devp, port, SYS_HIDDEN0(devp), cpu_port_read(devp, port, SYS_HIDDEN0(devp))| 0x40);
  msecWritePortReg(devp, port, WIR_HIDDEN0(devp), cpu_port_read(devp, port, WIR_HIDDEN0(devp))| 0x40);
}

//# macsec global register 0x1111 bit3~0 corresponding clk_en for port3~0
void set_macsec_clk_en(MAD_DEV * devp, int port, int flag) {
//  global POWER_CTL(devp)
  int reg = POWER_CTL(devp);
  unsigned int data;
  data =  cpu_read(devp, port, reg);
  if(flag) {
    data = bit_set(data, port);
  } else {
    data = bit_clr(data, port);
  }
  msecWriteReg(devp, port,  reg, data);
}
//# macsec global register 0x1111 bit7~4 corresponding power_down for port3~0
void set_macsec_power_down(MAD_DEV * devp, int port, int flag) {
//  global POWER_CTL(devp)
  int reg = POWER_CTL(devp);
  unsigned int data =  cpu_read(devp, port, reg);
  if(flag) {
    data = bit_set(data, port+4);
  } else {
    data = bit_clr(data, port+4);
  }
  msecWriteReg(devp, port,  reg, data);
}



unsigned int imb_read(MAD_DEV * devp, int port, short addr) {
//  global IMB_CTL(devp)
//  global IMB_DATA(devp)
  msecWriteReg(devp, port, IMB_CTL(devp), addr | 0x8000);
  return (cpu_read(devp, port, IMB_DATA(devp)));
}

void imb_write(MAD_DEV * devp, int port, short addr, unsigned int data) {
//  global IMB_CTL(devp)
//  global IMB_DATA(devp)
  msecWriteReg(devp, port, IMB_DATA(devp), data);
  msecWriteReg(devp, port, IMB_CTL(devp), addr | 0x8000);
}

//# ent_num 0~7, elu entry number
//# idx 0~4, index for ELU dword
void set_elu_content(MAD_DEV * devp, int port, int ent_num, int idx, int data) {
  imb_write(devp, port, (0x1000+ent_num * 8+idx), data);
}
//# ent_num 0~7, elu entry number
//# idx 0~7, index for ILU dword
void set_ilu_content(MAD_DEV * devp, int port, int ent_num, int idx, int data) {
  imb_write(devp, port, (0x1028 + (idx * 8)+ent_num + 1), data);
  //#imb_write(devp, [expr 0x1028+ent_num+idx * 8], data);
  //#imb_write(devp, [expr 0x1028+ent_num * 8+idx], data);
}

