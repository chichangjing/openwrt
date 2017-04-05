#ifndef __OSLINUX_H__
#define __OSLINUX_H__

#include "madCopyright.h"
#include "madErrorDefs.h"
#include "madApi.h"
#include "madApiDefs.h"
#include "madApiPrototype.h"
#include "madApiTypes.h"
#include "madDSPVctApi.h"
#include "msecApi.h"
#include "gtMad.h"
#include "gtMad.h"
#include "gtPTP.h"
#include "msApi.h"
#include "msApiDefs.h"
#include "msApiPrototype.h"
#include "msApiSelect.h"
#include "msApiTypes.h"
#include "stdio.h"

#include "lnxRmu.h"
#include "lnxSem.h"
#if 0
#define DBG_MSG(format,args...) do{fprintf(stdout, format, ##args);}while(0)
#define LOG_MSG(format,args...) do{fprintf(stdout, format, ##args);}while(0)
#define TTY_MSG(format,args...) do{fprintf(stdout, format, ##args);}while(0)
#define ALM_MSG(format,args...) do{fprintf(stdout, format, ##args);}while(0)
#else
#define DBG_MSG(format,args...)
#define LOG_MSG(format,args...)
#define TTY_MSG(format,args...) do{fprintf(stdout, format, ##args);}while(0)
#define ALM_MSG(format,args...)
#endif
typedef enum {
    PF_OK                   = GT_OK,                        /* Operation succeeded                   */
    PF_FAIL                 = GT_FAIL,                      /* Operation failed                      */
    PF_BAD_VALUE            = GT_BAD_VALUE,                 /* Illegal value (general)               */
    PF_BAD_PARAM            = GT_BAD_PARAM,                 /* Illegal parameter in function called  */
    PF_NOT_FOUND            = GT_NOT_FOUND,                 /* Item not found                        */
    PF_NO_MORE              = GT_NO_MORE,                   /* No more items found                   */
    PF_NO_SUCH              = GT_NO_SUCH,                   /* No such item                          */
    PF_TIMEOUT              = GT_TIMEOUT,                   /* Time Out                              */
    PF_NOT_SUPPORTED        = GT_NOT_SUPPORTED,             /* This request is not support           */
    PF_NOT_SUPPORTED_PORT   = GT_NOT_SUPPORTED_PORT,        /* The pOort is not support on this request */
    PF_ALREADY_EXIST        = GT_ALREADY_EXIST,             /* Tried to create existing item         */
    PF_BAD_CPU_PORT         = GT_BAD_CPU_PORT,              /* Input CPU Port is not valid physical port number */
}PF_STATUS;

#endif
