#ifndef _TM_VIRTUAL_LAYER_CORE_H_
#define _TM_VIRTUAL_LAYER_CORE_H_
#ifdef VIRTUAL_LAYER

/*
 * (c), Copyright 2009-2014, Marvell International Ltd.  (Marvell)
 *
 * This code contains confidential information of Marvell.
 * No rights are granted herein under any patent, mask work right or copyright
 * of Marvell or any third party. Marvell reserves the right at its sole
 * discretion to request that this code be immediately returned to Marvell.
 * This code is provided "as is". Marvell makes no warranties, expressed,
 * implied or otherwise, regarding its accuracy, completeness or performance.
 */
/**
 * @brief interface to internal virtual layer API.
 *
 * @file tm_virtual_layer_core.h
 *
 * $Revision: 2.0 $
 */


#ifdef __cplusplus
extern "C" {
#endif



int 	allocate_virtual_layer(void ** vp_dictionary_handle);
void	release_virtual_layer (void *  vp_dictionary_handle);

/* returned result  :  new logical ID mapped to physical id */
uint32_t create_virtual_id(void * vp_dictionary_handle, enum tm_level level, uint32_t physical_id); 

/* returned result  : 0 - success , otherwise  error code */
int 	remove_virtual_node_record(void * vp_dictionary_handle,enum tm_level level, uint32_t physical_ID); /* or logical - which is more convinient */

uint32_t 	get_physical_id_from_virtual(void * vp_dictionary_handle,enum tm_level level, uint32_t virtual_node_ID);
uint32_t 	get_virtual_id(void * vp_dictionary_handle,enum tm_level level, uint32_t physical_node_ID);

/* returned result  : 0 - success , otherwise  error code */
int 	update_physical_id_for_virtual(void * vp_dictionary_handle,enum tm_level level, uint32_t virtual_node_ID, uint32_t new_physical_id);

/* returned result  : 0 - success , otherwise  error code */
int 	change_physical_id_for_virtual(void * vp_dictionary_handle,enum tm_level level, uint32_t old_ID, uint32_t new_ID);


/* utilities */
/*
void * get_map_entry_from_logical_id(void * vp_dictionary_handle,enum tm_level level, uint32_t logical_ID);
void * get_map_entry_from_physical_id(void * vp_dictionary_handle,enum tm_level level, uint32_t physical_ID);
uint32_t 	get_physical_id_from_map_entry(void * vpMapEntry);
uint32_t 	get_logical_id_from_map_entry(void * vpMapEntry);
//returned result  : 0 - success , otherwise  error code 
int   	update_map_entry_physical_id(void * vpMapEntry, uint32_t physical_ID); 
*/




/* + some useful macros, used to simplify propagation of logical layer to existing code */

/** Currently logical layers are supported  for A level & B level -  levels ,where reshuffling is performed
 *  for other levels logical layer operations are transparent
 */


#define	GENERATE_VIRTUAL_ID(level,index) \
{\
	switch(level)\
	{\
		case A_LEVEL:\
		case B_LEVEL:\
		{\
/*fprintf(stderr," GENERATE_LOGICAL_ID ::  %s   level=%d   physical_index=%d\n",__FUNCTION__, level, index) ;*/\
			index=create_virtual_id(ctl->hVirtualLayer,level,index);\
			break;\
		}\
		default: break;\
	}\
	if (index == TM_INVAL)\
	{\
		rc = -ENOMEM;\
		goto out;\
	}\
}

/**   */




#define	CONVERT_TO_PHYSICAL(level,index) \
{\
	switch(level)\
	{\
		case A_LEVEL:\
		case B_LEVEL:\
		{\
/*fprintf(stderr," CONVERT_TO_PHYSICAL ::  %s  %s  line=%d  :: level=%d   Logical_index=%d\n",__FUNCTION__, __FILE__, __LINE__ , level, index) ;*/\
			uint32_t result=get_physical_id_from_virtual(ctl->hVirtualLayer, level,index);\
			if (result==(uint32_t)(-1))  return -ENODATA;\
			index=result;\
			break;\
		}\
		default:break;\
	}\
}

#define	CONVERT_TO_VIRTUAL(level,index) \
{\
	switch(level)\
	{\
		case A_LEVEL:\
		case B_LEVEL:\
		{\
/*fprintf(stderr," CONVERT_TO_VIRTUAL ::  %s  %s  line=%d  :: level=%d   Logical_index=%d\n",__FUNCTION__, __FILE__, __LINE__ , level, index) ;*/\
			uint32_t result=get_virtual_id(ctl->hVirtualLayer, level,index);\
			if (result==(uint32_t)(-1))  return -ENODATA;\
			index=result;\
			break;\
		}\
		default:break;\
	}\
}

#define	REMOVE_VIRTUAL_ID(level,index) \
{\
	switch(level)\
	{\
		case A_LEVEL:\
		case B_LEVEL:\
		{\
			remove_virtual_node_record(ctl->hVirtualLayer,level,index);\
			/* here not necessary to process error - if index not found it already not exists  */\
			break;\
		}\
		default: break;\
	}\
}\

#ifdef __cplusplus
}
#endif

#endif /* defined(VIRTUAL_LAYER) */
#endif /*  _TM_VIRTUAL_LAYER_CORE_H_ */
