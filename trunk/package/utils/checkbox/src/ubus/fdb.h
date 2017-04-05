#ifndef __FDB_H__
#define __FDB_H__

void set_mac_aging_time(const char * time);
void set_fdb_mode(const char * port, const char * mode);
void get_fdb_mode_d(const char * port);
void get_fdb_num_d(void);
void set_fdb_mac(const char * __idx, const char * port, const char * mac);
void repeat_fdb(const char * __idx);

#endif
