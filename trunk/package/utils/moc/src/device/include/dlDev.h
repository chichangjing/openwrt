#ifndef __DLDEV_H__
#define __DLDEV_H__

#include "madCopyright.h"
#include "madErrorDefs.h"
#include "madApi.h"
#include "madApiDefs.h"
#include "madApiPrototype.h"
#include "madApiTypes.h"
#include "madDSPVctApi.h"
#include "msecApi.h"
#include "gtMad.h"
#include "gtPTP.h"
#include "msApi.h"
#include "msApiDefs.h"
#include "msApiPrototype.h"
#include "msApiSelect.h"
#include "msApiTypes.h"
#include "gtRmuCntl.h"

#include "osLinux.h"
typedef enum {
    DL_OK                   = GT_OK,                        /* Operation succeeded                   */
    DL_FAIL                 = GT_FAIL,                      /* Operation failed                      */
    DL_BAD_VALUE            = GT_BAD_VALUE,                 /* Illegal value (general)               */
    DL_BAD_PARAM            = GT_BAD_PARAM,                 /* Illegal parameter in function called  */
    DL_NOT_FOUND            = GT_NOT_FOUND,                 /* Item not found                        */
    DL_NO_MORE              = GT_NO_MORE,                   /* No more items found                   */
    DL_NO_SUCH              = GT_NO_SUCH,                   /* No such item                          */
    DL_TIMEOUT              = GT_TIMEOUT,                   /* Time Out                              */
    DL_NOT_SUPPORTED        = GT_NOT_SUPPORTED,             /* This request is not support           */
    DL_NOT_SUPPORTED_PORT   = GT_NOT_SUPPORTED_PORT,        /* The pOort is not support on this request */
    DL_ALREADY_EXIST        = GT_ALREADY_EXIST,             /* Tried to create existing item         */
    DL_BAD_CPU_PORT         = GT_BAD_CPU_PORT,              /* Input CPU Port is not valid physical port number */
    DL_STATUS_MAX,
} DL_STATUS;

typedef enum {
    DL_FALSE = 0,
    DL_TRUE  = 1
} DL_BOOL;

DL_STATUS dlDevInit(GT_QD_DEV * dev, GT_SYS_CONFIG * cfg);
DL_STATUS dlSwReset(GT_QD_DEV *dev);

#include "dlAtu.h"
#include "dlPort.h"
#include "dlQos.h"
#include "dlVlan.h"
#endif /*__DLDEV_H__*/
