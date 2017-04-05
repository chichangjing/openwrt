#ifndef __LNXRMU_H__
#define __LNXRMU_H__
#include "osLinux.h"

GT_STATUS lnxRmuInit(char * ifName);
GT_BOOL lnxRmuHwAcess(GT_QD_DEV * dev, HW_DEV_REG_ACCESS * regList);
GT_STATUS lnxRmuTxRx(GT_U8 * reqPkt, GT_UINT reqPktLen, GT_U8 ** rspPkt, GT_UINT * rspPktLen);
GT_BOOL lnxRmuReadMii(GT_QD_DEV * dev, unsigned int phyAddr, unsigned int miiReg, unsigned int * value);
GT_BOOL lnxRmuWriteMii(GT_QD_DEV * dev, unsigned int phyAddr, unsigned int miiReg, unsigned int value);
#endif
