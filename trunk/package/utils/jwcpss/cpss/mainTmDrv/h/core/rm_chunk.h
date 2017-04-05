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
 * @brief rm_chunk interface.
 *
 * @file rm_chunk.h
 *
 * $Revision: 2.0 $
 */

#ifndef RM_CHUNK_H
#define RM_CHUNK_H

#include "rm_interface.h"
/* for definition of tm_level */
#include "tm_defs.h"

/** RM chunk data structure.
 */
/*
struct rm_chunk
{
    uint32_t index;
    uint32_t size;
    struct rm_chunk *next_free;
};
*/

typedef struct rm_chunk *chunk_ptr;


/***************************************************************
  new chunk storage
***************************************************************/




/** Find index of matching chunk and update internal DB.
 * 
 *   @param[in]		hndl		      Resource Manager handle.	
 *   @param[in]		lvl		      Hierarchy level. 
 *   @param[in]		size		      Length of needed range.
 *   @param[out]	index		      Index of node.	
 *
 *   @return an integer return code.
 *   @retval zero on success. 
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -ENOMEM if no free space in needed size.
 */
int			rm_allocate_chunk(rmctl_t hndl, enum tm_level lvl, uint32_t size, uint32_t *index);


/** if range first_index::first_index+size is free , allocates it  and update internal DB.
 * 
 *   @param[in]		hndl		      Resource Manager handle.	
 *   @param[in]		lvl		          Hierarchy level. 
 *   @param[out]	index		      first index of range.	
 *   @param[in]		size		      Length of needed range.
 *
 *   @return an integer return code.
 *   @retval zero on success. 
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -ENOMEM if required range is occupied.
 */
int			rm_get_chunk(rmctl_t hndl, enum tm_level lvl, uint32_t first_index, uint32_t size);



/** Add chunk of the nodes that became free and update internal DB.
 * 
 *   @param[in]		hndl			  Resource Manager handle.	
 *   @param[in]		lvl		          Hierarchy level. 
 *   @param[in]		size		      Length of needed range.
 *   @param[in]		index		      Index of node.	
 *
 *   @return an integer return code.
 *   @retval zero on success. 
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -ENOMEM when out of memory space.
 */
int			rm_release_chunk(rmctl_t hndl, enum tm_level lvl, uint32_t size, uint32_t index);


/** Find free chunk that includes index and get from it node to expand used nodes range.
 * 
 *   @param[in]		hndl          Resource Manager handle.	
 *   @param[in]		lvl		      Hierarchy level. 
 *   @param[in]	    index		  Index of node.	
 *
 *   @return an integer return code.
 *   @retval zero on success. 
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 *   @retval -ENOMEM if index not found.
 */
int			rm_expand_chunk_right(rmctl_t hndl, enum tm_level lvl, uint32_t index);

int			rm_expand_chunk_left(rmctl_t hndl, enum tm_level lvl, uint32_t index);


/** create new chunk and add it to free chunk list ( list elements are stay in decending order of chunk size)
 * 
 *   @param[in]	    index		  Index of starting node.	
 *   @param[in]	    length		  length of chunk
 *   @param[in]	    chunk_list	  Pointer to chunk list to add to
 *
 *   @return  new pointer to updated chunk list
 */
chunk_ptr	rm_new_chunk(uint32_t start_index, uint32_t length, struct rm_chunk * chunk_list);


/** deallocate chunk list and releases allocated memory
 * 
 *   @param[in]	    chunk_list	  Pointer to chunk list to free *
 */
void		clear_chunk_list(chunk_ptr list);

uint32_t	rm_get_max_chunk_size(rmctl_t hndl, enum tm_level lvl);


/* debugging utilities */

/* prints dump of chunk list for debugging proposes
*/
int rm_dump_chunk(rmctl_t hndl, enum tm_level lvl);

/* following function return content of N-th chunk in the list - index & size
*  for ut creation 
*  if chunk exists - returned value = 0 & index & size contains data from chunk,
*  otherwise  1 is returned
*/
int get_chunk_data(rmctl_t hndl, enum tm_level lvl, int chunk_number_in_list, uint32_t *index , uint32_t * size);




#endif                          /* RM_CHUNK_H */


