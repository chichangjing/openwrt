/*
 * (c), Copyright 2009-2013, Marvell International Ltd.  (Marvell)
 *
 * This code contains confidential information of Marvell.
 * No rights are granted herein under any patent, mask work right or copyright
 * of Marvell or any third party. Marvell reserves the right at its sole
 * discretion to request that this code be immediately returned to Marvell.
 * This code is provided "as is". Marvell makes no warranties, expressed,
 * implied or otherwise, regarding its accuracy, completeness or performance.
 */
/**
 * @brief tm_logical_layer implementation.
 *
 * @file tm_logical_layer.c
 *
 * $Revision: 2.00 $
 */
#include "tm_logical_layer_core.h"
#include "tm_core_types.h"
#include "tm_nodes_create.h"
#include "tm_os_interface.h"
#include <string.h>
#include "tm_errcodes.h"
#ifdef VIRTUAL_LAYER
#else
	#include "tm_nodes_ctl.h"
#endif


int				tm_get_node_number(tm_handle hndl, enum tm_level level, const char * logical_name)
{
	pairstruct_t * entry;

	TM_CTL(ctl,hndl) 

	if ((level < Q_LEVEL) || (level > P_LEVEL)) return -1;
	if (!logical_name) return -1;
	entry=_tm_logical_find_index(ctl->hLogicalLayer,level,logical_name);
	if (entry) return entry->number;
	return -1;
}
const char *	tm_get_node_logical_name(tm_handle hndl,enum tm_level level, unsigned int node_number)
{
	pairstruct_t * entry;
	DECLARE_TM_CTL_PTR(ctl,hndl) 
    if ((!ctl) || (ctl->magic != TM_MAGIC))return NULL;

	if ((level < Q_LEVEL) || (level > P_LEVEL)) return NULL;
	entry=_tm_logical_find_name(ctl->hLogicalLayer,level,node_number);
	if (entry) return entry->name;
	return NULL;
}


#define	CHECK_LOGICAL_CONDITIONS(level, name)	\
	if (!name) return TM_CONF_NULL_LOGICAL_NAME;\
	_tm_logical_create_dictionary(&(ctl->hLogicalLayer));\
	if (tm_get_node_number ( hndl, level, name) != -1 ) return TM_CONF_WRONG_LOGICAL_NAME;\

	
int tm_create_logical_port (tm_handle hndl, uint8_t port_index,
							enum tm_port_bw port_speed,
							struct tm_port_params *params,
							uint16_t num_of_c_nodes,
							uint16_t num_of_b_nodes,
							uint16_t num_of_a_nodes,
							uint32_t num_of_queues,
							const char * port_name)
{
	int rc;
	TM_CTL(ctl,hndl) 
	
	CHECK_LOGICAL_CONDITIONS(P_LEVEL, port_name)
	rc=tm_create_port (	hndl,
						port_index,
						port_speed,
						params,
						num_of_c_nodes,
						num_of_b_nodes,
						num_of_a_nodes,
						num_of_queues);
	if (!rc)
	{
		_tm_logical_add_entry(ctl->hLogicalLayer,P_LEVEL,port_name,port_index);
	}
	return rc;
}

int tm_create_logical_asym_port(tm_handle hndl, uint8_t port_index,
								enum tm_port_bw port_speed,
								struct tm_port_params *params,
								const char * port_name)
{
	int rc;
	TM_CTL(ctl,hndl)

	CHECK_LOGICAL_CONDITIONS(P_LEVEL, port_name)

	rc=tm_create_asym_port( hndl,
							port_index,
							port_speed,
							params);
	if (!rc)
	{
		_tm_logical_add_entry(ctl->hLogicalLayer,P_LEVEL,port_name,port_index);
	}
	return rc;
}

#ifdef VIRTUAL_LAYER
	#define UPDATE_LOGICAL_LAYER(level)	\
	{\
		_tm_logical_add_entry(ctl->hLogicalLayer,level,node_name,* node_index);\
	}
#else
	#define UPDATE_LOGICAL_LAYER(level)	\
	{\
		struct tm_tree_change change;\
		while(!tm_nodes_read_next_change(hndl,&change))\
		{\
			_tm_logical_update_entry_index(ctl->hLogicalLayer,level,change.old_index,change.new_index);\
		}\
		_tm_logical_add_entry(ctl->hLogicalLayer,level,node_name,* node_index);\
	}
#endif

int tm_create_logical_c_node_to_port (tm_handle hndl, uint8_t port_index,
										struct tm_c_node_params *c_params,
										const char * node_name,
										uint32_t * node_index)
{
	int rc;
	TM_CTL(ctl,hndl)
		
	CHECK_LOGICAL_CONDITIONS(C_LEVEL,node_name)

#ifdef VIRTUAL_LAYER
	/* do nothing */
#else
	rc=tm_clean_list(hndl);
#endif

	rc=tm_create_c_node_to_port(hndl,
								port_index,
								c_params,
								node_index);
	if (!rc) UPDATE_LOGICAL_LAYER(C_LEVEL)
	return rc;
}
int tm_create_logical_b_node_to_c_node (	tm_handle hndl, uint32_t parent_index,
											struct tm_b_node_params *b_params,
											const char * node_name,
											uint32_t * node_index)
{
	int rc;
	TM_CTL(ctl,hndl)
		
	CHECK_LOGICAL_CONDITIONS(B_LEVEL, node_name)

#ifdef VIRTUAL_LAYER
	/* do nothing */
#else
	rc=tm_clean_list(hndl);
#endif

	rc=tm_create_b_node_to_c_node(hndl,
									parent_index,
									b_params,
									node_index);
	if (!rc) UPDATE_LOGICAL_LAYER(B_LEVEL)
	return rc;
}

int tm_create_logical_a_node_to_b_node (	tm_handle hndl, uint32_t parent_index,
											struct tm_a_node_params *a_params,
											const char * node_name,
											uint32_t * node_index)
{
	int rc;
	TM_CTL(ctl,hndl)

	CHECK_LOGICAL_CONDITIONS(A_LEVEL, node_name)

#ifdef VIRTUAL_LAYER
	/* do nothing */
#else
	rc=tm_clean_list(hndl);
#endif

	rc=tm_create_a_node_to_b_node(hndl,
									parent_index,
									a_params,
									node_index);
	if (!rc)	UPDATE_LOGICAL_LAYER(A_LEVEL)
	return rc;
}

int tm_create_logical_queue_to_a_node (	tm_handle hndl, uint32_t parent_index,
											struct tm_queue_params *q_params,
											const char * node_name,
											uint32_t * node_index)

{
	int rc;
	TM_CTL(ctl,hndl)

	CHECK_LOGICAL_CONDITIONS(Q_LEVEL, node_name)

#ifdef VIRTUAL_LAYER
	/* do nothing */
#else
	rc=tm_clean_list(hndl);
#endif

	rc=tm_create_queue_to_a_node(hndl,
									parent_index,
									q_params,
									node_index);
	if (!rc) UPDATE_LOGICAL_LAYER(Q_LEVEL)
	return rc;
}



/*
 core functionality
*/

pairstruct_t *	create_pairstruct_entry(const char * name, unsigned int index)
{
	pairstruct_t * new_entry;
	unsigned int namelen;
	new_entry=(pairstruct_t*)tm_malloc(sizeof(pairstruct_t));
	namelen=(unsigned int)cpssOsStrlen(name)+1; /* terminating zero */
	new_entry->name=(char *)tm_malloc(namelen);
	tm_memcpy((void*)new_entry->name, (void*)name, namelen);
	new_entry->number=index;
	new_entry->next=NULL;
	return new_entry;
}

void release_pairstruct_entry(pairstruct_t * entry)
{
	tm_free((void*)entry->name);
	tm_free(entry);
}



void 	_tm_logical_create_dictionary(void ** vp_dictionary_handle)
{
	pairstruct_t ** dict_ptr;
	if (*vp_dictionary_handle==NULL)
	{
		dict_ptr=(pairstruct_t**)(tm_malloc(sizeof(pairstruct_t)*5));
		dict_ptr[0]=NULL;
		dict_ptr[1]=NULL;
		dict_ptr[2]=NULL;
		dict_ptr[3]=NULL;
		dict_ptr[4]=NULL;
		*vp_dictionary_handle=(void*)dict_ptr;
	}
}
void	_tm_logical_clear_dictionary(void * vp_dictionary_handle)
{
	pairstruct_t ** dict_ptr;
	int i;
	pairstruct_t * ptr;
	pairstruct_t * next_entry;
	if (vp_dictionary_handle)
	{
		dict_ptr=(pairstruct_t**)vp_dictionary_handle;

		for (i=0; i < 4; i++)
		{
			ptr=dict_ptr[i];
			while (ptr)
			{
				next_entry=(pairstruct_t *)ptr->next;
				release_pairstruct_entry(ptr);
				ptr=next_entry;
			}
		}
	}
}

pairstruct_t *	_tm_logical_find_index(void * vp_dictionary_handle, int layer, const char*name)
{
	pairstruct_t ** dict_ptr;
	pairstruct_t * ptr=NULL;
	if (vp_dictionary_handle)
	{
		dict_ptr=(pairstruct_t **)vp_dictionary_handle;
		ptr=dict_ptr[layer];
		while (ptr)
		{
			if (cpssOsStrCmp(ptr->name,name)==0) break;
			ptr=(pairstruct_t*)ptr->next;
		}
	}
	return ptr;
}
pairstruct_t *	_tm_logical_find_name(void * vp_dictionary_handle, int layer, unsigned int index)
{
	pairstruct_t ** dict_ptr;
	pairstruct_t * ptr=NULL;
	if (vp_dictionary_handle)
	{
		dict_ptr=(pairstruct_t **)vp_dictionary_handle;
		ptr=dict_ptr[layer];
		while (ptr)
		{
			if (ptr->number==index)break;
			ptr=(pairstruct_t*)ptr->next;
		}
	}
	return ptr;
}

pairstruct_t *	_tm_logical_add_entry(void * vp_dictionary_handle, int layer, const char * name, unsigned int index)
{
	pairstruct_t * new_entry;
	pairstruct_t ** dict_ptr;
	pairstruct_t * ptr;
	new_entry=NULL;
	if (vp_dictionary_handle)
	{
		dict_ptr=(pairstruct_t **)vp_dictionary_handle;
		new_entry=create_pairstruct_entry(name,index);
		ptr=dict_ptr[layer];
		if (ptr)
		{
			while (ptr->next) ptr=(pairstruct_t*)ptr->next;
			ptr->next=(struct pairstruct_t *)new_entry;
		}
		else	dict_ptr[layer]=new_entry;
	}
	return new_entry;
}
int				_tm_logical_remove_name(void * vp_dictionary_handle,int layer, const char * name)
{
	pairstruct_t * entry;
	const char * entry_name;
	pairstruct_t ** dict_ptr;
	pairstruct_t * ptr;
	if (vp_dictionary_handle)
	{
		dict_ptr=(pairstruct_t **)vp_dictionary_handle;
		ptr=dict_ptr[layer];
		if (ptr)
		{
			if (cpssOsStrCmp(ptr->name,name)==0)
			{
				entry=ptr;
				dict_ptr[layer]=(pairstruct_t*)ptr->next;
				release_pairstruct_entry(entry);
				return 1;
			}
			while (ptr->next)
			{
				entry=(pairstruct_t*)(ptr->next);
				entry_name=((pairstruct_t*)(ptr->next))->name;
				if (cpssOsStrCmp(entry_name,name)==0)
				{
					ptr->next=entry->next;
					release_pairstruct_entry(entry);
					return 1;
				}
				ptr=(pairstruct_t*)ptr->next;
			}
		}
	}
	return 0;
}
int				_tm_logical_remove_index(void * vp_dictionary_handle , int layer, unsigned int index)
{
	pairstruct_t * entry;
	unsigned int entry_index;
	pairstruct_t * ptr;
	pairstruct_t ** dict_ptr;
	if (vp_dictionary_handle)
	{
		dict_ptr=(pairstruct_t **)vp_dictionary_handle;
		ptr=dict_ptr[layer];
		if (ptr)
		{
			if (ptr->number==index)
			{
				entry=ptr;
				dict_ptr[layer]=(pairstruct_t*)ptr->next;
				release_pairstruct_entry(entry);
				return 1;
			}
			while (ptr->next)
			{
				entry=(pairstruct_t*)ptr->next;
				entry_index=entry->number;
				if (entry_index==index)
				{
					ptr->next=entry->next;
					release_pairstruct_entry(entry);
					return 1;
				}
				ptr=(pairstruct_t*)ptr->next;
			}
		}
	}
	return 0;
}

void			_tm_logical_update_entry_index(void * vp_dictionary_handle , int layer, unsigned int old_index, unsigned int new_index)
{
	pairstruct_t *	entry = _tm_logical_find_name(vp_dictionary_handle,layer, old_index);
	if (entry)		entry->number=new_index;
}
