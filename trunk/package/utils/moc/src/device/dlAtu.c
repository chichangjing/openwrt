#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "dlAtu.h"

DL_STATUS dlAddrTableShow(DL_ADDRTABLE * addrtable){
    int i = 0;
    DBG_MSG("%s called. \n", __func__);
    for(; i<addrtable->valid_entries; i++){
        TTY_MSG("%8s %02x  %02x%02x-%02x%02x-%02x%02x   %5s-%08x  %04x:%02x\n",
                addrtable->entries[i].macAddr.arEther[0] & 0x01 ? "Multicast":"Unicast", addrtable->entries[i].entryState,
                addrtable->entries[i].macAddr.arEther[0], addrtable->entries[i].macAddr.arEther[1],
                addrtable->entries[i].macAddr.arEther[2], addrtable->entries[i].macAddr.arEther[3],
                addrtable->entries[i].macAddr.arEther[4], addrtable->entries[i].macAddr.arEther[5],
                addrtable->entries[i].trunkMember ? "trunk":"port", addrtable->entries[i].portVec,
                addrtable->entries[i].DBNum, addrtable->entries[i].prio);
    }
    TTY_MSG("Address Total: %ld.\n", addrtable->valid_entries);
    return DL_OK;
}

DL_STATUS dlAtuDump(GT_QD_DEV * dev, DL_ADDRTABLE * addrtable){
    DL_STATUS retVal;
    RMU_DEV_DUMP_ATU atu;
    DBG_MSG("%s called. \n", __func__);
    memset(&atu, 0, sizeof(RMU_DEV_DUMP_ATU));
    memset(addrtable, 0, sizeof(DL_ADDRTABLE));
    do{
        if(DL_OK != (retVal = gtRmuAtuDump(dev, &atu))){
            LOG_MSG("gtRmuAtuDump failed");
            return retVal;
        }
        memcpy(&addrtable->entries[addrtable->valid_entries], &atu.entries, sizeof(GT_ATU_ENTRY)*atu.valid_entries);
        memset(&atu.entries, 0, sizeof(GT_ATU_ENTRY)*RMU_MAX_ATU_ENTRIES);
        addrtable->valid_entries += atu.valid_entries;
        atu.valid_entries=0;
    }while(atu.from);
    return DL_OK;
}
