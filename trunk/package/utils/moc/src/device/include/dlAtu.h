#ifndef __DLATU_H__
#define __DLATU_H__
#include "dlDev.h"

#define _88E6390X_MAX_ATU_ENTRIES (16384)

typedef struct {
    unsigned long   valid_entries;
    GT_ATU_ENTRY    entries[_88E6390X_MAX_ATU_ENTRIES];
}DL_ADDRTABLE;

DL_STATUS dlAddrTableShow(DL_ADDRTABLE * addrtable);
DL_STATUS dlAtuDump(GT_QD_DEV * dev, DL_ADDRTABLE * addrtable);
#endif
