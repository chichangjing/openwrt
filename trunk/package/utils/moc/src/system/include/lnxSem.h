#ifndef __LNXSEM_H__
#define __LNXSEM_H__
#include "semaphore.h"
#include "osLinux.h"

GT_STATUS lnxSemCreate(GT_SEM * semId, GT_SEM_BEGIN_STATE state);
GT_STATUS lnxSemDelete(GT_SEM *smid);
GT_STATUS lnxSemTake(GT_SEM *smid);
GT_STATUS lnxSemGive(GT_SEM *smid);

#endif
