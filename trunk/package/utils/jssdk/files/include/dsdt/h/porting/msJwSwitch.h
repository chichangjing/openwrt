
#include <Copyright.h>

#ifndef __MS_JWSWITCH_H
#define __MS_JWSWITCH_H
#ifdef __cplusplus
extern "C" {
#endif

#ifdef _VXWORKS
#include "vxWorks.h"
#include "logLib.h"
#endif
#ifndef __KERNEL__
#include "stdio.h"
#include "stdarg.h"
#include "stdlib.h"
#include "time.h"
#include "string.h"
#else
#include "linux/string.h"
#include "linux/kernel.h"
#include "linux/random.h"
#include "gtMiiSmiIf.h"
#endif

#include "msApi.h"


extern GT_QD_DEV *pJwDev;

#define USE_SEMAPHORE
#ifdef USE_SEMAPHORE
GT_SEM osSemCreate(GT_SEM_BEGIN_STATE state);
GT_STATUS osSemDelete(GT_SEM smid);
GT_STATUS osSemWait(GT_SEM smid, GT_U32 timeOut);
GT_STATUS osSemSignal(GT_SEM smid);
#endif

GT_BOOL jwMiiRead(GT_QD_DEV* dev, unsigned int portNumber , unsigned int MIIReg, unsigned int* value);
GT_BOOL jwMiiWrite(GT_QD_DEV* dev, unsigned int portNumber , unsigned int MIIReg, unsigned int value);
GT_BOOL jwMiiInit(void);
void jwMiiFree(void);

GT_STATUS jwSwitchStart(int cpuPort, int en_init_chip);

#ifdef __cplusplus
}
#endif
#endif

