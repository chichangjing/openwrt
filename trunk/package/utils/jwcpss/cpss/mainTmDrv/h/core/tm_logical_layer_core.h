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
 * @brief TM tm logical layer internal functions declarations.
 *
 * @file tm_logical_layer_core.h
 *
 * $Revision: 2.0 $
 */


#ifndef _TM_LOGICAL_LAYER_CORE_H_
#define _TM_LOGICAL_LAYER_CORE_H_

#include "tm_logical_layer_interface.h"

typedef  struct pairstruct_t
{
	unsigned int			number;
	const char *			name;
	struct pairstruct_t *	next ;
} pairstruct_t;


void 	_tm_logical_create_dictionary(void ** vp_dictionary_handle);
void	_tm_logical_clear_dictionary(void *   vp_dictionary_handle);

pairstruct_t *	_tm_logical_find_index(void * vp_dictionary_handle,int layer, const char*name);
pairstruct_t *	_tm_logical_find_name(void * vp_dictionary_handle,int layer, unsigned int index);



pairstruct_t *	_tm_logical_add_entry(void * vp_dictionary_handle,int layer, const char * name, unsigned int index);
int				_tm_logical_remove_name(void * vp_dictionary_handle,int layer, const char * name);
int				_tm_logical_remove_index(void * vp_dictionary_handle,int layer, unsigned int index);

void			_tm_logical_update_entry_index(void * vp_dictionary_handle,int layer, unsigned int old_index, unsigned int new_index);

#endif /* _TM_LOGICAL_LAYER_CORE_H_ */
