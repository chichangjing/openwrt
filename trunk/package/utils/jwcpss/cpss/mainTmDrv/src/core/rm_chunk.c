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
 * @brief Resource Manager chunk managing functions implementation.
 *
 * @file rm_chunk.c
 *
 * $Revision: 2.0 $
 */

#include <stdlib.h>
#include <stdio.h>
#include "rm_chunk.h"
#include "rm_internal_types.h"
#include "tm_os_interface.h"

struct rm_chunk {
    uint32_t index;
    uint32_t size;
    struct rm_chunk *next_free;
};

/*
 chunk list is always in decending order - the biggest is first
 chunk list always have member - if no free nodes so it has one chunk with 0 size
*/

void clear_chunk_list(chunk_ptr list)
{
	chunk_ptr tmp;
	while (list)
	{
		tmp=list->next_free;
		tm_free(list);
		list=tmp;
	}
}

static void prv_swap_chunk_content(chunk_ptr a,chunk_ptr b )
{
	uint32_t tmp;
	tmp=a->index; a->index = b->index ; b->index=tmp;
	tmp=a->size; a->size = b->size ; b->size=tmp;
}

static void prv_reorder_decreased(chunk_ptr ptr)
{
	while((ptr->next_free) && (ptr->next_free->size > ptr->size))
	{
		prv_swap_chunk_content(ptr,ptr->next_free);
		ptr=ptr->next_free;
	}
	/* remove extra zero sizes chunks  */
	if (ptr->size==0) 
	/* this is first zero size chunk - all chunks after must be also zero-sized and should be removed */
	{
		clear_chunk_list(ptr->next_free);
		/* terminate  list */
		ptr->next_free=NULL;
	}
}


static void prv_reorder_increased( chunk_ptr list, chunk_ptr ptr)
{
	while (list!=ptr)
	{
		if (list->size < ptr->size )  prv_swap_chunk_content(list,ptr);
		list=list->next_free;
	}
}

chunk_ptr rm_new_chunk(uint32_t start_index, uint32_t length, struct rm_chunk * chunk_list)
{
	struct rm_chunk*  pchunk=(struct rm_chunk *)tm_malloc(sizeof(struct rm_chunk));
	if (pchunk)
	{
		pchunk->index=start_index;
		pchunk->size=length;
		pchunk->next_free=chunk_list;
		prv_reorder_decreased(pchunk);
	}
	return pchunk;
}

int rm_release_chunk(rmctl_t hndl, enum tm_level lvl, uint32_t size, uint32_t index)
{
	chunk_ptr ptr;
	chunk_ptr ptr_m;
 	/* declare and check rmctl */
    RM_HANDLE(ctl, hndl);
    /* validate space for release */
    for (ptr=ctl->rm_free_nodes[lvl]; ptr && ptr->size ; ptr = ptr->next_free)
    {
		/* check if free range starts inside some chunk */
		if (index >= ptr->index && index < (ptr->index+ptr->size) ) return 1;
		/* check if free range ends inside some chunk */
		if ((index+size) > ptr->index && (index+size) <= (ptr->index+ptr->size)) return 1;
	}

    /* find if the chunk can be merged with already existing free chunk */
    for (ptr=ctl->rm_free_nodes[lvl]; ptr ; ptr = ptr->next_free)
	{
		if (ptr->index+ptr->size == index) /* chunk is merged after existing free space */
		{ 
			ptr->size+=size;
			/* find chunk , which index is the next of the chunk above */
			/* it can be only in the rest of list ,otherwise it should happend case below - merge before */
			for (ptr_m=ptr->next_free ; ptr_m ; ptr_m = ptr_m->next_free)
			{
				if (ptr->index+ptr->size == ptr_m->index) 
				{
					 /* merge them */
					ptr->size +=ptr_m->size;
					ptr_m->size=0;
					prv_reorder_decreased(ptr_m);
					break;
				}
			}
			break;
		}
		if (ptr->index == index + size) /* chunk is merged before existing free space */
		{ 
            ptr->index=index;
			ptr->size+=size;
			/* find chunk , for which the chunk above is the next*/
			/* it can be only in the rest of list ,otherwise it should happend case above - merge after*/
			for (ptr_m=ptr->next_free ; ptr_m ; ptr_m = ptr_m->next_free)
			{
				if (ptr_m->index+ptr_m->size == ptr->index) 
				{
					 /* merge them */
					ptr->size +=ptr_m->size;
					ptr->index-=ptr_m->size;
					ptr_m->size=0;
					prv_reorder_decreased(ptr_m);
					break;
				}
			}
			break;
		}
	}
    if (ptr)
	{
		prv_reorder_increased(ctl->rm_free_nodes[lvl],ptr);  /* chunk merged , chunk size increased , need reorder */
	}
	else 
	{
		ctl->rm_free_nodes[lvl]= rm_new_chunk(index,size,ctl->rm_free_nodes[lvl]);/*new free chunk added , reordering inside chunk creation function*/
	}
	return 0;
}

int rm_allocate_chunk(rmctl_t hndl, enum tm_level lvl, uint32_t size, uint32_t *index)
{
	chunk_ptr	ptr;
 	/*  declare and check rmctl */
    RM_HANDLE(ctl, hndl);

	ptr=ctl->rm_free_nodes[lvl];

	if (ptr->size < size ) return -ENOMEM; /* no enought continuous nodes pool */
	/* if we want to utilize smallest possible chunk  -  launch string below */
		while ((ptr->next_free) &&  ( ptr->next_free->size >= size)) ptr= ptr->next_free;
		/* here ptr is the last chunk with size  >= required size */
		/* if it exactly matched  - let get it, */
		/* otherwise let get from biggest chunk  */
		if (ptr->size > size) ptr=ctl->rm_free_nodes[lvl];
	/* otherwize we always will get resources from biggest chunk; */
	/* let get pool from chunk  and return it's index */
	ptr->size-=size;
	/* we can extract pool from start of chunk */
		*index=ptr->index;
		ptr->index+=size;
	/* or extract it fom the end */
		/* 	*index=ptr->index+ptr->size; */
	prv_reorder_decreased(ptr);
	return 0;
}

int rm_expand_chunk_right(rmctl_t hndl, enum tm_level lvl, uint32_t index)
{
	chunk_ptr	ptr;
 	/* declare and check rmctl */
    RM_HANDLE(ctl, hndl);

	ptr=ctl->rm_free_nodes[lvl];
	while ((ptr) &&  ( ptr->index != index)) ptr = ptr->next_free;
	if (ptr && ptr->size) /* found */
	{
		ptr->index+=1;
		ptr->size-=1;
		prv_reorder_decreased(ptr);
		return 0;
	}
	else return -ENOMEM;
}

int rm_expand_chunk_left(rmctl_t hndl, enum tm_level lvl, uint32_t index)
{
	chunk_ptr	ptr;
 	/* declare and check rmctl */
    RM_HANDLE(ctl, hndl);

	ptr=ctl->rm_free_nodes[lvl];
	while (ptr) 
	{
		if ((ptr->index+ptr->size - 1) == index) break;
		 ptr= ptr->next_free;
	}
	if (ptr && ptr->size) /* found */
	{
		ptr->size-=1;
		prv_reorder_decreased(ptr);
		return 0;
	}
	else return -ENOMEM;
}

int			rm_get_chunk(rmctl_t hndl, enum tm_level lvl, uint32_t first_index, uint32_t size)
{
	chunk_ptr	ptr;
	uint32_t	tail_pos;
	uint32_t	tail_size;

 	/*  declare and check rmctl */
    RM_HANDLE(ctl, hndl);

	ptr=ctl->rm_free_nodes[lvl];

	while (ptr && (ptr->size  >= size ) )
	{
		if ((first_index >= ptr->index) && ((ptr->index+ptr->size) >= (first_index +size) ))
		{
			/* free range  found */
			/*   chunk_index->**********************************************************************************<-chunk_index+chunk_size   */
			/*                 first_undex->||||||||||||||||||||||||||||||||||||||||||||||||||||<-first_index+size                         */
			/*                                                                                                                             */
			/* after extraction of required range the head is remained part of the chunk and tail is new chunk should be added             */
			/*                 head                                                                     tail                               */ 
			/*                **************---------------------------------------------------*****************                           */
			/*   chunk_index->              <-first_undex                     first_index+size>                 <-chunk_index+chunk_size   */
			/* calculate tail chunk parameters */
			tail_pos = first_index+size;
			tail_size = ptr->index + ptr->size - tail_pos;
			/* reduce current chunk to head size */
			ptr->size = first_index-ptr->index;
			/* chunk size decreased - reorder it */
			prv_reorder_decreased(ptr);
			/* if tail is not zero size chunk - add it to list */
			if (tail_size) 
			{
				ctl->rm_free_nodes[lvl]= rm_new_chunk(tail_pos,tail_size,ctl->rm_free_nodes[lvl]); /*new free chunk added , reordering inside chunk creation function*/
			}
			return 0; 
		}
		ptr= ptr->next_free;
	}
	/* free range not found */
	return -ENOMEM;
}





int rm_dump_chunk(rmctl_t hndl, enum tm_level lvl)
{
	chunk_ptr	ptr;
    
	/* declare and check rmctl */
    RM_HANDLE(ctl, hndl);
	
    if (!ctl)
        return -EINVAL;
    if (ctl->magic != RM_MAGIC)
        return -EBADF;
	
	ptr = ctl->rm_free_nodes[lvl];    
    if (ptr == NULL) 
    { 
		cpssOsFprintf(CPSS_OS_FILE_STDERR,"CL level=%d --- \n", lvl);
		cpssOsFprintf(CPSS_OS_FILE_STDERR,"\tEmpty\n");
		/* empty list */
        return 0;
    }
		
    cpssOsFprintf(CPSS_OS_FILE_STDERR," CL level=%d --- \n",lvl);
	while (ptr)
    {
        cpssOsFprintf(CPSS_OS_FILE_STDERR," [%d:%d]", ptr->index,ptr->size);
        ptr= ptr->next_free;
    }
    cpssOsFprintf(CPSS_OS_FILE_STDERR,"\n");

    return 0;
}

uint32_t	rm_get_max_chunk_size(rmctl_t hndl, enum tm_level lvl)
{
	chunk_ptr	ptr;
 	/* declare and check rmctl */
    RM_HANDLE(ctl, hndl);

	ptr=ctl->rm_free_nodes[lvl];
	return ptr->size;
	
}

int get_chunk_data(rmctl_t hndl, enum tm_level lvl, int chunk_number_in_list, uint32_t *index , uint32_t * size)
{
	int i=0;
	chunk_ptr	ptr;
    RM_HANDLE(ctl, hndl);
		
	ptr = ctl->rm_free_nodes[lvl];    
	while (ptr)
    {
		if (i==chunk_number_in_list)
		{
			*index = ptr->index;
			*size  = ptr->size;
			return 0;
		}
		i++;
        ptr= ptr->next_free;
    }
	return 1;

}
