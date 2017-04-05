#ifndef __PARSE_FDB_H__
#define __PARSE_FDB_H__
int parse_mac_aging_time(char* argv);
int parse_if_fdb_enable(char* argv);
int parse_if_fdb_disable(char* argv);
int parse_if_fdb_mac(char* argv);
void repeat_fdb(const char * __idx);
#endif
