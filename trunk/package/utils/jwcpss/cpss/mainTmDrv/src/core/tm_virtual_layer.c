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
 * @brief tm_virtual_layer implementation.
 *
 * @file tm_virtual_layer.c
 *
 * $Revision: 2.00 $
 */
#include "tm_core_types.h"

#ifdef VIRTUAL_LAYER	

	#include "tm_virtual_layer_core.h"
	#include "tm_os_interface.h"
	#include <stdlib.h>
	#include <string.h>
	#include <errno.h>

	typedef struct 
	{
		uint32_t	physical;
		uint32_t	virtual;
		void *		next;
	}pair_t;


	pair_t *	__create_new_virtual_pair(uint32_t physical_ID, uint32_t virtual_ID)
	{
		pair_t * new_entry;
		new_entry=(pair_t*)tm_malloc(sizeof(pair_t));
		if (new_entry != NULL) {
			new_entry->physical = physical_ID;
			new_entry->virtual = virtual_ID;
			new_entry->next = NULL;
		}
		return new_entry;
	}

	void __release_virtual_pair(pair_t * entry)
	{
		tm_free(entry);
	}

	uint32_t 	__get_physical_id_from_map_entry(void * vpMapEntry)
	{
		pair_t*	entry;
		entry=(pair_t*)vpMapEntry;
		return entry->physical;
	}

	uint32_t 	__get_virtual_id_from_map_entry(void * vpMapEntry)
	{
		pair_t*	entry;
		entry=(pair_t*)vpMapEntry;
		return entry->virtual;
	}

	int   __update_map_entry_physical_id(void * vpMapEntry, uint32_t physical_ID)
	{
		pair_t*	entry;
		if (vpMapEntry)
		{
			entry=(pair_t*)vpMapEntry;
			entry->physical=physical_ID;
			return 0;
		}
		return 1;
	}

	int   __update_map_entry_virtual_id(void * vpMapEntry, uint32_t virtual_ID)
	{
		pair_t*	entry;
		if (vpMapEntry)
		{
			entry=(pair_t*)vpMapEntry;
			entry->virtual=virtual_ID;
			return 0;
		}
		return 1;
	}

	void __init_ID_generator();
	/* dictionary creation / release */

	int 	allocate_virtual_layer(void ** vp_dictionary_handle)
	{
		pair_t ** dict_ptr;
		if (*vp_dictionary_handle==NULL)
		{
			dict_ptr=(pair_t**)(tm_malloc(sizeof(pair_t)*5));
			if (dict_ptr == NULL)
				return -ENOMEM;
			dict_ptr[0]=NULL;
			dict_ptr[1]=NULL;
			dict_ptr[2]=NULL;
			dict_ptr[3]=NULL;
			dict_ptr[4]=NULL;
			*vp_dictionary_handle=(void*)dict_ptr;
			__init_ID_generator();
		}
		return 0;
	}


	void	release_virtual_layer(void *   vp_dictionary_handle)
	{
		pair_t ** dict_ptr;
		int i;
		pair_t * ptr;
		pair_t * next_entry;
		if (vp_dictionary_handle)
		{
			dict_ptr=(pair_t**)vp_dictionary_handle;

			for (i=0; i < 4; i++)
			{
				ptr=dict_ptr[i];
				while (ptr)
				{
					next_entry=(pair_t *)ptr->next;
					__release_virtual_pair(ptr);
					ptr=next_entry;
				}
			}
			tm_free(dict_ptr);
		}	
	}

	static pair_t * __add_new_virtual_pair(void * vp_dictionary_handle, int layer, uint32_t physical, uint32_t virtual)
	{
		pair_t * new_entry;
		pair_t ** dict_ptr;
		pair_t * ptr;
		new_entry=NULL;
		if (vp_dictionary_handle)
		{
			dict_ptr=(pair_t **)vp_dictionary_handle;
			new_entry=__create_new_virtual_pair(physical,virtual);
			if (new_entry == NULL)
				return NULL;
			ptr=dict_ptr[layer];
			if (ptr)
			{
				while (ptr->next) ptr=(pair_t*)ptr->next;
				ptr->next=(struct pair_t *)new_entry;
			}
			else	dict_ptr[layer]=new_entry;
		}
		return new_entry;
	}

	#define TEST_FOUND	 if (is_virtual) { found=((*entry)->virtual==index) ; } else { found=((*entry)->physical==index) ; }
	int		__find_virtual_pair(void * vp_dictionary_handle,int layer, uint32_t index, int is_virtual, pair_t ** 	previous, pair_t ** entry )
	{
		pair_t ** 	dict_ptr;
		int			found;
		
		*previous	=NULL;
		
		if (vp_dictionary_handle)
		{
			dict_ptr=(pair_t **)vp_dictionary_handle;
			*entry=dict_ptr[layer];
			if (*entry)
			{
				TEST_FOUND 
				if (found) return 1;/* first element found */
				while ((*entry)->next)
				{
					(*previous)= (*entry);
					(*entry)=(*entry)->next;
					TEST_FOUND 
					if (found) return 1;
				}
			}
		}
		return 0;
	}

	int		__remove_virtual_pair(void * vp_dictionary_handle,int layer, uint32_t index, int is_virtual)
	{
		pair_t * 	entry;
		pair_t ** 	dict_ptr;
		pair_t * 	previous;	
		
		if (vp_dictionary_handle)
		{
			if (__find_virtual_pair(vp_dictionary_handle,layer, index, is_virtual, & previous, &entry ))
			{
				/* found entry */
				if (previous) 	previous->next=entry->next;
				else 
				{
					/* first element found*/
					dict_ptr=(pair_t **)vp_dictionary_handle;
					dict_ptr[layer]=entry->next;
					return 1;
				}
				__release_virtual_pair(entry);
				return 1;
			}
		}
		return 0;
	}

	int 	update_physical_id_for_virtual(void * vp_dictionary_handle,enum tm_level level, uint32_t virtual_node_ID, uint32_t new_physical_id)
	{
		pair_t * 	entry;
		pair_t * 	previous;	
		if (__find_virtual_pair(vp_dictionary_handle,level, virtual_node_ID, 1 , &previous, &entry))
		{
			return __update_map_entry_physical_id(entry,new_physical_id);
		}
		return 1;
	}

	int 	change_physical_id_for_virtual(void * vp_dictionary_handle,enum tm_level level, uint32_t old_ID, uint32_t new_ID)
	{
		pair_t * 	entry;
		pair_t * 	previous;	
		if (__find_virtual_pair(vp_dictionary_handle,level, old_ID, 0 , &previous, &entry))
		{
			return __update_map_entry_physical_id(entry,new_ID);
		}
		return 1;
	}

	uint32_t 	get_physical_id_from_virtual(void * vp_dictionary_handle,enum tm_level level, uint32_t virtual_node_ID)
	{
		pair_t * 	entry;
		pair_t * 	previous;		
		if (__find_virtual_pair(vp_dictionary_handle,level, virtual_node_ID, 1 , &previous, &entry))
		{
			return __get_physical_id_from_map_entry(entry);
		}
		return -1;
	}

	uint32_t 	get_virtual_id(void * vp_dictionary_handle,enum tm_level level, uint32_t physical_node_ID)
	{
		pair_t * 	entry;
		pair_t * 	previous;	
		if (__find_virtual_pair(vp_dictionary_handle,level, physical_node_ID, 0 , &previous, &entry))
		{
			return __get_virtual_id_from_map_entry(entry);
		}
		return -1;
	}



	int 	remove_virtual_node_record(void * vp_dictionary_handle,enum tm_level level, uint32_t node_ID)
	{
		return __remove_virtual_pair(vp_dictionary_handle,level, node_ID, 0 /* physical ID */);
	}


	/*****************************************************************************************************************************
	 *	 logical ID  generator 
	 * 
	 *****************************************************************************************************************************/
	/* temporary  let do them  sequence numbers in range >  nodes count */

	static uint32_t q_virtual = 10000;
	static uint32_t a_virtual = 10000;
	static uint32_t b_virtual = 10000;
	static uint32_t c_virtual = 10000;
	
	void __init_ID_generator()
	{
		q_virtual=10000;
		a_virtual=10000;
		b_virtual=10000;
		c_virtual=10000;
	}

	uint32_t create_virtual_id(void * vp_dictionary_handle, enum tm_level level, uint32_t physical_id)
	{
		uint32_t virtual_ID; 
		switch(level)
		{
			case Q_LEVEL : virtual_ID=q_virtual++;break;
			case A_LEVEL : virtual_ID=a_virtual++;break;
			case B_LEVEL : virtual_ID=b_virtual++;break;
			case C_LEVEL : virtual_ID=c_virtual++;break;
			case P_LEVEL:
			default:return -1;
		}
		
		if (__add_new_virtual_pair(vp_dictionary_handle, level, physical_id, virtual_ID) != NULL)
			return virtual_ID;
		else
			return TM_INVAL;
	}

	/*****************************************************************************************************************************
	 *	 Interface functions  implementation
	 * 
	 *****************************************************************************************************************************/

	int		tm_get_node_physical_id(tm_handle hndl, enum tm_level level, uint32_t virtual_ID , uint32_t * pPhysical_ID)
	{
		uint32_t ID;
		TM_CTL(ctl, hndl);
		
		if (pPhysical_ID)
		{
			ID= get_physical_id_from_virtual(ctl->hVirtualLayer,(int)level,virtual_ID);
			if (ID != ((uint32_t)-1))
			{
				*pPhysical_ID = ID;
				return 0;
			}
		}
		return 1;
	}

	int		tm_get_node_virtual_id (tm_handle hndl, enum tm_level level, uint32_t physical_ID, uint32_t * pVirtual_ID)
	{
		uint32_t ID;	
		TM_CTL(ctl, hndl);
		
		if (pVirtual_ID)
		{
			ID = get_virtual_id(ctl->hVirtualLayer,level,physical_ID);
			if (ID != ((uint32_t)-1))
			{
				*pVirtual_ID = ID;
				return 0;
			}
		}
		return 1;
	}

#endif

