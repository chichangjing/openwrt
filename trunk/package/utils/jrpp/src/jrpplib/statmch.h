
/* State machine */
 
#ifndef _RPP_STATE_MACHINE_H_
#define _RPP_STATE_MACHINE_H_

#include "base.h"

#define BEGIN  9999

typedef struct state_mach_t {
	struct state_mach_t   *next;

	char*                  name;
	Bool                   state_change;
	unsigned int           state;

#ifdef RPP_DBG
	char                   debug; /* 0- no dbg, 1 - port, 2 - ring */
	unsigned int           ignore_hop2state;
#endif

	void                   (*enter_state) (struct state_mach_t *);
	Bool                   (*check_condition) (struct state_mach_t *);
	char*                  (*get_state_name) (int);
	union {
	    struct rppRing  *ring;
	    struct rppPort  *port;
	    void            *owner;
	} owner;
	
} STATE_MACH_T;

#define RPP_STATE_MACH_IN_LIST(WHAT)                             \
{                                                                \
    STATE_MACH_T *abstr;                                         \
                                                                 \
    abstr = RPP_state_mach_create (RPP_##WHAT##_enter_state,     \
                                   RPP_##WHAT##_check_conditions, \
                                   RPP_##WHAT##_get_state_name,  \
                                   this,                         \
                                   #WHAT);                       \
    abstr->next = this->machines;                                \
    this->machines = abstr;                                      \
    this->WHAT = abstr;                                          \
}


STATE_MACH_T *RPP_state_mach_create (void (*enter_state)(STATE_MACH_T*), 
                                     Bool (*check_condition)(STATE_MACH_T*), 
                                     char *(*get_state_name)(int), 
                                     void *owner, char *name);

void RPP_state_mach_delete (STATE_MACH_T* this);
Bool RPP_check_condition (STATE_MACH_T* this);
Bool RPP_change_state (STATE_MACH_T* this);
Bool RPP_hop_2_state (STATE_MACH_T* this, unsigned int new_state);

#endif /* _RPP_STATE_MACHINE_H_ */


