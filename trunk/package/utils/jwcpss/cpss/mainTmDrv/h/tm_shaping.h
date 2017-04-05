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
 * @brief TM Shaping configuration, including General registers and Shaping Profiles.
 *
 * @file tm_shaping.h
 *
 * $Revision: 2.0 $
 */

#ifndef   	TM_SHAPING_H
#define   	TM_SHAPING_H

#include "tm_defs.h"



/** Create a Shaping Profile
 * @param[in]	   hndl	           TM lib handle
 * @param[in]	   level	       A level to configure the profile for
 * @param[in]      profile 	       Shaping profile configuration struct pointer
 * @param[out]     prof_index      Index of the created Shaping profile
 *
 * @return an integer return code
 * @retval 0 on success
 * @retval -EINVAL        if hndl is NULL
 * @retval -EBADF         if hndl is an invalid handle
 * @retval -EADDRNOTAVAIL if level is invalid
 * @retval  TM_CONF_UPD_RATE_NOT_CONF_FOR_LEVEL if periodic process disabled 
 * @retval TM_BW_OUT_OF_RANGE  if  impossible to configure desired b/w
 * @retval TM_CONF_BURST_TOO_SMALL_FOR_GIVEN_RATE	desired cbs or ebs value is too small for desired cir/eir
 * @retval TM_BW_UNDERFLOW - if cir & eir are too different to configure together
 * TM_CONF_BURST_TOO_SMALL_FOR_GIVEN_RATE	desired cbs or ebs value is too small for desired cir/eir
 * @retval TM_HW_SHAPING_PROF_FAILED if download to HW fails
*/
int tm_create_shaping_profile(tm_handle hndl,
                              enum tm_level level,
                              struct tm_shaping_profile_params *profile,
                              uint32_t * prof_index);

/** verify shaping profile
 * @param[in]	   hndl	           TM lib handle
 * @param[in]	   level	       A level to configure the profile for
 * @param[in]      profile 	       Shaping profile configuration struct pointer
 *
 * @return an integer return code
 * @retval 0 on success
 * @retval -EINVAL        if hndl is NULL
 * @retval -EBADF         if hndl is an invalid handle
 * @retval -EADDRNOTAVAIL if level is invalid
 * @retval TM_BW_OUT_OF_RANGE  if  impossible to configure desired b/w
 * @retval TM_CONF_BURST_TOO_SMALL_FOR_GIVEN_RATE	desired cbs or ebs value is too small for desired cir/eir
 * @retval TM_BW_UNDERFLOW - if cir & eir are too different to configure together
 * TM_CONF_BURST_TOO_SMALL_FOR_GIVEN_RATE	desired cbs or ebs value is too small for desired cir/eir
 *		in this case   values in  pcbs_kb/pebs_kb are updated to minimum possible value.
*/
int tm_verify_shaping_profile(tm_handle hndl,
                              enum tm_level level,
                              struct tm_shaping_profile_params *profile);



/** Delete a Shaping Profile.
 * @param[in]		hndl        	TM lib handle
 * @param[in]       prof_index      index of the Shaping profile to be deleted
 *
 * @return an integer return code
 * @retval 0 on success
 * @retval -EINVAL          if either hndl is NULL
 * @retval -EBADF           if hndl is an invalid handle
 * @retval -EACCES          if prof_index is 0
 * @retval -EADDRNOTAVAIL   if prof_index is out of range 
 * @retval -EFAULT          if no existing profile with this index
 * @retval -EPERM           if profile in use
 * @retval TM_HW_SHAPING_PROF_FAILED if HW download fails
*/
int tm_delete_shaping_profile(tm_handle hndl, uint32_t prof_index);


/** Read Shaping profile
 * @param[in] 	   hndl	       TM lib handle
 * @param[in]	   level       A level to read a profile for
 * @param[in]	   prof_index  An index to a profile to be read out
 * @param[in]      profile 	   Shaping profile configuration struct pointer
 * 
 * @note The CIR and EIR bw may deviate from the originally configured
 * by tm_create_shaping_profile or tm_update_shaping_profile by the 
 * bw accuracy parameter provisioned in tm_configure_periodic_scheme API.
 *
 * @return an integer return code 
 * @retval 0 on success
 * @retval -EINVAL 		    if hndl is NULL
 * @retval -EBADF 		    if hndl is an invalid handle
 * @retval -EDOM             if level is invalid
 * @retval -EADDRNOTAVAIL    if prof_index is out of range 
 * @retval -EFAULT           if no existing profile with this index
*/
int tm_read_shaping_profile(tm_handle hndl,
                            enum tm_level level,
                            uint32_t prof_index,
                            struct tm_shaping_profile_params *profile);


/** Update a Shaping Profile
 * @param[in]	  hndl	          TM lib handle
 * @param[in]	  level	          A level to configure the profile for
 * @param[in]     prof_index      Index of the Shaping profile to be updated
 * @param[in]     profile  	      Shaping profile configuration struct pointer
 *
 * @note   The profile parameter's fields which don't need to be
 * updated must be set to TM_INVAL. 
 *
 * @return an integer return code
 * @retval 0 on success
 * @retval -EINVAL        if hndl is NULL
 * @retval -EBADF         if hndl is an invalid handle
 * @retval -EACCES        if prof_index is out of range
 * @retval -EPERM         if prof_index doesn't exist
 * @retval -EADDRNOTAVAIL if level is invalid
 * @retval -EFAULT        if CBS/EBS is out of range
 * 
 * @retval  TM_BW_OUT_OF_RANGE  if desired banwidth is to big to be configured
 * @retval TM_CONF_MIN_TOKEN_TOO_LARGE if 
 *		  profile->MinBurstSize < MinToken or profile->MinBurstSize < TM_MTU
 * @retval TM_CONF_MAX_TOKEN_TOO_LARGE if
 *		  profile->MaxBurstSize < MaxToken or profile->MaxBurstSize < TM_MTU
 * @retval TM_HW_SHAPING_PROF_FAILED if download to HW fails
*/
int tm_update_shaping_profile(tm_handle hndl,
                              enum tm_level level,
                              uint32_t prof_index,
                              struct tm_shaping_profile_params *profile);




#ifdef PER_NODE_SHAPING

/** Update shaping for node - the NODE shaping parameters are updated 
 * according to parameters passed through API.  If  success - the node 
 * is excluded from clients of shaping profile which it was belong ,
 * the node shaping profile ref is set to TM_DIRECT_NODE_SHAPING 

 * @param[in]     hndl            TM lib handle.
 * @param[in]     level     		a level
 * @param[in]     node_index      index of node to configure shaping for it.        
 * @param[in]     node_shaping_params  	      Shaping profile configuration struct pointer
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -ERANGE if level is invalid.
 *   @retval -EADDRNOTAVAIL if index is out of range.
 *   @retval -ENODATA if index is not in use.
 *   @retval -EFAULT  if CBS/EBS  is out of range
 *   @retval TM_BW_OUT_OF_RANGE  if  cir/eir  too big to be configured
 *   @retval TM_CONF_MIN_TOKEN_TOO_LARGE - the cbs value is less than calculated cir token
 *   @retval TM_CONF_MAX_TOKEN_TOO_LARGE - the ebs value is less than calculated eir token
 *   @retval TM_HW_SHAPING_PROF_FAILED if download to HW fails
*/
int tm_update_node_shaping(tm_handle hndl,
                              enum tm_level level,
                              uint32_t node_index,
                              struct tm_shaping_profile_params * node_shaping_params);

/** read shaping for node 
 * according to parameters passed through API.  If  success - the node 
 * is excluded from clients of shaping profile which it was belong ,
 * the node shaping profile ref is set to TM_DIRECT_NODE_SHAPING 

 * @param[in]     hndl            	TM lib handle.
 * @param[in]     level     		a level
 * @param[in]     node_index      	index of node to read shaping parameters
 * @param[out]    *shaping_profile  shaping profile reference  for node
 * @param[out]     node_shaping  	      Shaping profile configuration struct pointer - 
 *
 *   @return an integer return code.
 *   @retval zero on success.
 * 	in this case  *shaping_profile contains  shaping profile reference for  this node
 *  if *shaping_profile == TM_DIRECT_NODE_SHAPING  than node_shaping structure is filled by 
 *  values used for node shaping configuration.
 * 
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -ERANGE if level is invalid.
 *   @retval -EADDRNOTAVAIL if index is out of range.
 *   @retval -ENODATA if index is not in use.
*/

int tm_read_node_shaping(tm_handle hndl,
                              enum tm_level level,
                              uint32_t node_index,
                              uint16_t * shaping_profile,
                              struct tm_shaping_profile_params * node_shaping);

#endif /* PER_NODE_SHAPING */







#endif   /* TM_SHAPING_H */

