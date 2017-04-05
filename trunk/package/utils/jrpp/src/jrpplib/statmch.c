
/* State machine */
 
#include "base.h"
#include "statmch.h"

#if RPP_DBG
#include "ring.h"
#endif

STATE_MACH_T *RPP_state_mach_create (void (*enter_state) (STATE_MACH_T*),
                                     Bool (*check_condition) (STATE_MACH_T*),
                                     char *(*get_state_name) (int),
                                     void *owner, char *name)
{
	STATE_MACH_T *this;

	if((this = (STATE_MACH_T*)RPP_CALLOC(1, sizeof(STATE_MACH_T))) == NULL) {
		return NULL;
	}

	this->state = BEGIN;
	this->name = (char*) strdup (name);
	this->state_change = False;
#if RPP_DBG
	this->debug = False;
	this->ignore_hop2state = BEGIN;
#endif	
	this->enter_state = enter_state;
	this->check_condition = check_condition;
	this->get_state_name = get_state_name;
	this->owner.owner = owner;

	return this;
}
                              
void RPP_state_mach_delete (STATE_MACH_T *this)
{
	RPP_FREE(this->name);
	RPP_FREE(this);
}

Bool RPP_check_condition (STATE_MACH_T* this)
{
	Bool bret;

	bret = (*(this->check_condition)) (this);
	if (bret) {
		this->state_change = True;
	}

	return bret;
}
        
Bool RPP_change_state (STATE_MACH_T* this)
{
	register int number_of_loops;

	for (number_of_loops = 0; ; number_of_loops++) {
		if (! this->state_change) 
			return number_of_loops;
		
		(*(this->enter_state)) (this);
		
		this->state_change = False;
		RPP_check_condition (this);
	}

	return number_of_loops;
}

Bool RPP_hop_2_state (STATE_MACH_T* this, unsigned int new_state)
{
	this->state = new_state;
	this->state_change = True;

	return True;
}


