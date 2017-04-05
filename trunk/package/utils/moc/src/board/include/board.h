#ifndef __BORAD_H__
#define __BORAD_H__
#include "dlDev.h"
#include "osLinux.h"

/*
 *
 * LogicPort :  10  9  2  3  4  5  6  7  4   5   6   7   8   1
 * HwPort :      1  2  3  4  5  6  7  8  9  10  11  12  13  14
 *
 * */

extern GT_QD_DEV        brDev;
extern GT_SYS_CONFIG    brCfg;
extern GT_QD_DEV        brDev;
extern DL_ADDRTABLE     brAddrTable;
extern GT_QoS_WEIGHT    brQosWeight;

DL_STATUS brInit(void);
DL_STATUS brSwReset(void);

#include "brQos.h"
#include "brAtu.h"
#endif
