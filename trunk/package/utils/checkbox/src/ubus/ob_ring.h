#ifndef __OB_RING_H__
#define __OB_RING_H__
#include "../caselib/cs_type.h"

void obring_get_status(void);
void obring_get_num(void);
void obring_get_global_ring(void);
void obring_get_general(void);
void dev_ring_list(cs_u32 ring_id);

void obring_set_status(const char * mode);
void obring_name(const char * _idx, const char * name);
void obring_set_mode(const char * _idx, const char * mode);
void obring_set_priority(const char * _idx, const char * priority);
void obring_set_primary_port(const char * _idx, const char * port);
void obring_set_second_port(const char * _idx, const char * port);
void obring_delete(const char * _idx);

#endif
