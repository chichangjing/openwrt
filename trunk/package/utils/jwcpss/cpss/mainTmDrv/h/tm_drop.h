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
 * @brief TM Drop Unit configuration, including WRED curves and Drop Profiles.
 *
 * @file tm_drop.h
 *
 * $Revision: 2.0 $
 */

#ifndef   	TM_DROP_H
#define   	TM_DROP_H

#define     TM_AGING_PROFILES  4 

#include "tm_defs.h"

/** Set the maximum RED drop probabilities at a level.
 *
 * @param[in]    hndl           TM lib handle
 * @param[in]    level          The level to apply max_* to
 * @param[in]    max_green      Max green probability
 * @param[in]    max_yellow     Max yellow probability
 * @param[in]    max_red        Max red probability
 *
 * @note Scale the RED curves so that the maximum possible value is as
 * given for a particular color. Above max_th the drop probability
 * will still be 100%.
 *
 * @return an integer return code
 * @retval -EINVAL              If hndl is NULL
 * @retval -EBADF               If hndl is an invalid handle
 * @retval -EDOM                If the level is invalid
 */
int tm_wred_max_prob(tm_handle hndl,
                     enum tm_level level,
                     enum tm_max_prob_mode max_green,
                     enum tm_max_prob_mode max_yellow,
                     enum tm_max_prob_mode max_red);

/** Set the RED curve values (the points on the curves)
 *
 * @param[in]    hndl           TM lib handle
 * @param[in]    level          The level to apply max_* to
 * @param[in]    curve_ind      The index of the curve
 * @param[in]    cos            The CoS the curve is used for
 * @param[in]    curve_green    The 32 curve points
 * @param[in]    curve_yellow   The 32 curve points
 * @param[in]    curve_red      The 32 curve points
 *
 * @note The RED curves consist of 32 points along the queue length
 * axis (x-axis). If three colors are used the maximum possible value
 * is given by 63 else 15, 0 means zero drop probability. Remember
 * that the RED curves will also be scaled by the values defined by
 * tm_red_max_prob(), thus max need not be 100%. Levels B, C, and P
 * does not have per color curves. Q, A, and B levels does not have
 * per cos curves and requires that TM_INVAL be given as the cos
 * value.
 *
 * @return an integer return code
 * @retval -EINVAL              If hndl is NULL
 * @retval -EBADF               If hndl is an invalid handle
 * @retval -EDOM                If the level is invalid
 * @retval -ENOSPC              If the curve_ind is out of range
 * @retval -EDOM                If the cos is out of range
 * @retval -ENODEV              No such color on this level
 * @retval -ENOSPC              To large curve_ind
 * @retval -EPERM               Curve value is too large
 */
int tm_wred_set_curves(tm_handle hndl,
                       enum tm_level level,
                       uint8_t curve_ind,
                       uint8_t cos,
                       const tm_wred_vec curve_green,
                       const tm_wred_vec curve_yellow,
                       const tm_wred_vec curve_red);

/**
 */
int tm_wred_default_curves(tm_handle hndl, enum tm_level level,
                           uint8_t curve_ind);

/** Create a WRED curve for a level.
 * @param[in]	        	hndl	      TM lib handle
 * @param[in]               level         A nodes level the WRED curve is created for 
 * @param[in]               cos	          CoS of RED Curve
 * @param[in]               prob	      Array of 32 probability points (0-100%)
 * @param[out]              curve_index   An index of a created WRED curve
 *
 * @note The cos parameter is relevant for C lvl only, else set TM_INVAL
 * @note Legacy API, incompatible with tm_red_max_prob() and tm_red_curve().
 *
 * @return an integer return code
 * @retval 0 on success
 * @retval -EINVAL        if hndl is NULL
 * @retval -EBADF         if hndl is an invalid handle
 * @retval -EADDRNOTAVAIL if level is invalid
 * @retval -EPERM         if one of probabilities is out of range
 * @retval -ENOSPC        if the WRED Curves table is fully allocated
 * @retval -EBADMSG       if AQM Mode params are not configured for this lvl
 * @retval TM_HW_WRED_CURVE_FAILED if download to HW fails
 */
int tm_create_wred_curve(tm_handle hndl,
                         enum tm_level level,
                         uint8_t cos,
                         uint8_t *prob,
                         uint8_t *curve_index);


/** Create a WRED traditional curve for a level.
 * @param[in]	        	hndl	      TM lib handle
 * @param[in]               level         A nodes level the WRED curve is created for 
 * @param[in]               cos	          CoS of RED Curve
 * @param[in]               mp  	      Non zero Max probability
 * @param[out]              curve_index   An index of a created WRED curve
 *
 * @note The cos parameter is relevant for C lvl only, else set TM_INVAL.
 * @note Legacy API, incompatible with tm_red_max_prob() and tm_red_curve().
 *
 * @return an integer return code
 * @retval 0 on success
 * @retval -EINVAL        if hndl is NULL
 * @retval -EBADF         if hndl is an invalid handle
 * @retval -EADDRNOTAVAIL if level is invalid
 * @retval -ENOSPC        if the WRED Curves table is fully allocated
 * @retval -EPERM         if max probability is not valid
 * @retval -EBADMSG       if AQM Mode params are not configured for this lvl
 * @retval TM_HW_WRED_CURVE_FAILED if download to HW fails
 */
int tm_create_wred_traditional_curve(tm_handle hndl,
                                     enum tm_level level,
                                     uint8_t cos,
                                     float mp,
                                     uint8_t *curve_index);


/** Create Drop profile 
 * @param[in] 		hndl      	 TM lib handle
 * @param[in]       level        A nodes level to create a Drop profile for   
 * @param[in]       cos	         CoS
 * @param[in]		profile 	 Drop Profile configuration structure pointer 
 * @param[out]      prof_index	 An index of the created Drop profile
 *
 * @note In case of Color Blind TD disabled  in CA/WRED modes set cbtdBytes=TM_INVAL.
 * @note Cos of Drop Profile matches Cos of given curve. 
 * @note The CoS parameter is relevant for C and P level only, 
 *       else set TM_INVAL.
 * @note For P level in Global mode set 'cos' = TM_INVAL, else 
 *       profile will be created for CoS mode.
 *
 * @return an integer return code
 * @retval 0 on success
 * @retval -EINVAL         if either hndl or profile is NULL
 * @retval -EBADF          hndl is an invalid handle
 * @retval -EPERM          if the level is invalid 
 * @retval -EACCES         if the wred_catd_bw or cbtd_bw is out of range 
 * @retval -EDOM           if wred/catd are not configured correctly 
 * @retval -EADDRNOTAVAIL  if RED curve index in the profile is out of range 
 * @retval -EDOM           if cos in the profile is out of range
 * @retval -ENODEV         if RED curve doesn't exist
 * @retval -EFAULT         if exponents in the profile are out of range
 * @retval -ENODATA        if wred_catd_mode is invalid
 * @retval -ERANGE         if max_th is less that min_th
 * @retval -ENOSPC         if level's Drop Profiles table is fully allocated
 * @retval -EBADMSG        if AQM Mode params are not configured for this lvl
 * @retval TM_HW_DROP_PROFILE_FAILED if download to HW fails
*/
int tm_create_drop_profile(tm_handle hndl,
                           enum tm_level level,
                           uint8_t cos,
                           struct tm_drop_profile_params *profile,
                           uint16_t *prof_index);

/* macro utilities for converting drop length to  BW and vice versa */

#define	RTT	200 /*Round Trip Time in mseconds */
/*
	convert bytes to bandwidth in kbits/sec:
    bandwidth = Bytes * 8 (bits in byte) * 1000 (msec in sec) / RTT(in msec) / 1000(kbits->bits)
*/	
#define DROP_BYTES_TO_BW_KBITS(drop_len_in_bytes)	(uint32_t)((double)drop_len_in_bytes * 8 * 1000/ RTT / 1000)

/*
	convert bandwidth in kbits/sec to bytes:
    bytes = bandwidth * 1000 (kbits->bits) * RTT(in msec) / 8 (bits in byte) / 1000 (msec in sec)
*/
#define DROP_BW_KBITS_TO_BYTES(drop_bw_in_kbits)	(uint32_t)((double)drop_bw_in_kbits * 1000 * RTT / 8 /1000)



/** Update Drop profile 
 * @param[in] 		hndl      	 TM lib handle
 * @param[in]       level        A nodes level to create a Drop profile for   
 * @param[in]       cos	         CoS
 * @param[in]		profile 	 Drop Profile configuration structure pointer 
 * @param[in]       prof_index	 An index of the Drop profile to be updated
 *
 * @note In case of Color Blind TD disabled  in CA/WRED modes set cbtdBytes=TM_INVAL.

 * @note Cos of Drop Profile matches Cos of given curve. 
 * @note The CoS parameter is relevant for C and P level only, 
 *       else set TM_INVAL.
 * @note For P level in Global mode set 'cos' = TM_INVAL, else 
 *       profile will be updated for CoS mode.
 * @note For Q/A/B/C levels user is aware to turn off traffic during profile update
 *       For P level update is possible only if the profile currently is not in use
 *
 * @return an integer return code
 * @retval 0 on success
 * @retval -EINVAL         if either hndl or profile is NULL
 * @retval -EBADF          hndl is an invalid handle
 * @retval -EPERM          if the level is invalid 
 * @retval -EACCES         if the wred_catd_bw or cbtd_bw is out of range 
 * @retval -EDOM           if wred/catd are not configured correctly 
 * @retval -EADDRNOTAVAIL  if RED curve index in the profile is out of range 
 * @retval -EDOM           if cos in the profile is out of range
 * @retval -ENODEV         if RED curve doesn't exist
 * @retval -EFAULT         if exponents in the profile are out of range
 * @retval -ENODATA        if wred_catd_mode is invalid
 * @retval -ERANGE         if max_th is less that min_th
 * @retval -ENOSPC         if level's Drop Profiles table is fully allocated
 * @retval -EBADMSG        if AQM Mode params are not configured for this lvl
 * @retval TM_HW_DROP_PROFILE_FAILED if download to HW fails
*/


int tm_update_drop_profile(tm_handle hndl,
                           enum tm_level level,
                           uint8_t cos,
						   uint16_t prof_index,
                           struct tm_drop_profile_params *profile_params);


/** Delete Drop profile
 * @param[in] 	   hndl 	    TM lib handle
 * @param[in]      level        A nodes level to delete a Drop profile for 
 * @param[in]      cos	        CoS
 * @param[in]	   prof_index	An index to a profile to be deleted
 *  
 * @note The CoS parameter is relevant for C and P level
 *      only, else set TM_INVAL.
 * @note For P level in Global mode set 'cos' = TM_INVAL, else
 *       profile will be deleted for CoS mode.
 *  
 * @return an integer return code
 * @retval 0 on success
 * @retval -EINVAL          if hndl is NULL
 * @retval -EBADF           if hndl is an invalid handle
 * @retval -EDOM            if the level is invalid 
 * @retval -EADDRNOTAVAIL   if prof_index is out of range 
 * @retval -EPERM           if profile is in use or reserved 
 * @retval -EFAULT          if no existing profile with this index
 * @retval TM_HW_DROP_PROFILE_FAILED if download to HW fails
*/
int tm_delete_drop_profile(tm_handle hndl,
                           enum tm_level level,
                           uint8_t cos,
                           uint16_t prof_index);

/** Read Drop profile
 * @param[in] 	   hndl	        TM lib handle
 * @param[in]      level        A nodes level to read a Drop profile for 
 * @param[in]      cos	        CoS
 * @param[in]	   prof_index	An index to a profile to be read out
 * @param[in]      profile      Drop profile configuration struct pointer 
 * 
 * @note The cbtd_rtt_ratio calculated aproximately from the register's values 
 * @note The CoS parameter is relevant for C and P level only, 
 *       else set TM_INVAL.
 * @note For P level in Global mode set 'cos' = TM_INVAL, else 
 *       profile will be read for CoS mode.
 *
 * @return an integer return code
 * @retval 0 on success
 * @retval -EINVAL           if hndl is NULL
 * @retval -EBADF            if hndl is an invalid handle
 * @retval -EPERM            if the level is invalid 
 * @retval -EADDRNOTAVAIL    if prof_index is out of range 
 * @retval -EFAULT           if no existing profile with this index
 * @retval -EBADMSG          if AQM Mode params are not configured for this lvl
 */
int tm_read_drop_profile(tm_handle hndl,
                         enum tm_level level,
                         uint8_t cos,
                         uint16_t prof_index,
                         struct tm_drop_profile_params *profile);



/** Set Drop (Egress) Colors number per level.
 * @param[in] 	   hndl	        TM lib handle
 * @param[in]      lvl          A nodes level to set colors number for 
 * @param[in]      num          Colors number 
 *
 * @note This API should be called before all the rest Drop APIs (if
 * need). By default there are two colors per each level.
 * @note In case TM2TM usage, instead if this API should be called 'tm2tm_set_egress_drop_aqm_mode'.
 *
 * @return an integer return code
 * @retval 0 on success
 * @retval -EINVAL           if hndl is NULL
 * @retval -EBADF            if hndl is an invalid handle
 * @retval -EPERM            if level is invalid
 * @retval -EFAULT           if num is invalid
 * @retval TM_HW_COLOR_NUM_CONFIG_FAIL if download to HW fails
 */
int tm_set_drop_color_num(tm_handle hndl,
                          enum tm_level lvl,
                          enum tm_color_num num);

/** get Drop (Egress) Colors number per level.
 * @param[in] 	   hndl						TM lib handle
 * @param[in]      lvl						A nodes level to get colors number for 
 * @param[out]     (pointer	to)num			Colors number for this level
 * @param[in]      (pointer	to)resolution	count of bits for WRED curves presentation for this level
 *
 * @return an integer return code
 * @retval 0 on success
 * @retval -EINVAL           if hndl is NULL
 * @retval -EBADF            if hndl is an invalid handle
 * @retval -EPERM            if level is invalid
 */
int tm_get_drop_color_num_and_resolution(tm_handle hndl,
										  enum tm_level lvl,
										  enum tm_color_num  * num, uint8_t * resolution);
										 

/**  Change the aging status.
 *
 *   @param[in]     hndl        TM lib handle.
 *   @param[in]     status      Aging status.                   
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if status is out of range.
 *   @retval TM_HW_AGING_CONFIG_FAIL if download to HW fails.
 */
int tm_aging_change_status(tm_handle hndl, uint8_t status);

/**  Retrieve aging status.
 *
 *   @param[in]     hndl        TM lib handle.
 *   @param[in]     status      pointer to aging status variable to retrieve status.                   
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if status is out of range.
 *   @retval TM_HW_AGING_CONFIG_FAIL if download to HW fails.
 */
int tm_aging_status_get(tm_handle hndl, uint8_t * status);

/**  Change Drop Probability (DP) source.
 *
 *   @param[in]     hndl        TM lib handle.
 *   @param[in]     lvl         A nodes level to set source for
 *   @param[in]     color       A color to set source for (0,1,2)
 *   @param[in]     source      QL or AQL (0 - use AQL, 1 - use QL to calculate DP)
 * 
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF  if hndl is invalid.
 *   @retval -EPERM  if level is invalid.
 *   @retval -ENODEV if color is invalid.
 *   @retval -EFAULT if status is out of range.
 *   @retval TM_HW_AQM_CONFIG_FAIL if download to HW fails
 */
int tm_dp_source_set(tm_handle hndl,
                        enum tm_level lvl,
                        uint8_t color,
                        enum tm_dp_source source);

/**  Retrieve Drop Probability (DP) source.
 *
 *   @param[in]     hndl        TM lib handle.
 *   @param[in]     lvl         A nodes level to set source for
 *   @param[in]     color       A color to set source for (0,1,2)
 *   @param[out]    source      (pointer to)QL or AQL (0 -  AQL used, 1 - QL used to calculate DP)
 * 
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF  if hndl is invalid.
 *   @retval -EPERM  if level is invalid.
 *   @retval -ENODEV if color is invalid.
 *   @retval -EFAULT if status is out of range.
 *   @retval TM_HW_AQM_CONFIG_FAIL if download to HW fails
 */
int tm_dp_source_get(tm_handle hndl,
                        enum tm_level lvl,
                        uint8_t color,
                        enum tm_dp_source *source);

/** Drop Query Response Select.
 *
 *   @param[in]     hndl        TM lib handle. 
 *   @param[in]     port_dp     0 - Global, 1 - CoS.
 *   @param[in]     lvl         Local response level (Q/A/B/C/Port).
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if port_dp is invalid.
 *   @retval -EPERM if lvl out of range.
 *   @retval TM_HW_DP_QUERY_RESP_CONF_FAILED if download to HW
 *           fails.
 */
int tm_set_drop_query_responce(tm_handle hndl,
                               uint8_t port_dp,
                               enum tm_level lvl);

/** Retrrieve Drop Query Response .
 *
 *   @param[in]     hndl        TM lib handle. 
 *   @param[out]    port_dp     (pointer to) 0 - Global, 1 - CoS.
 *   @param[out]    lvl         (pointer to) Local response level (Q/A/B/C/Port).
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if port_dp is invalid.
 *   @retval -EPERM if lvl out of range.
 *   @retval TM_HW_DP_QUERY_RESP_CONF_FAILED if download to HW
 *           fails.
 */
int tm_get_drop_query_responce(tm_handle hndl,
                               uint8_t * port_dp,
                               enum tm_level * lvl);

/* BC2 only */
/** Drop Queue Cos Select.
 *
 *   @param[in]     hndl        TM lib handle. 
 *   @param[in]     index       Queue index.
 *   @param[in]     cos         Cos.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if index or cos is invalid.
 *   @retval TM_HW_QUEUE_COS_CONF_FAILED if download to HW fails.
 */
int tm_set_drop_queue_cos(tm_handle hndl,
                          uint32_t index,
                          uint8_t cos);

/** Retrieve drop Queue Cos Selection.
 *
 *   @param[in]     hndl        TM lib handle. 
 *   @param[in]     index       Queue index.
 *   @param[out]    *cos        Cos.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if index or cos is invalid.
 *   @retval TM_HW_QUEUE_COS_CONF_FAILED if download to HW fails.
 */
int tm_get_drop_queue_cos(tm_handle hndl,
                          uint32_t index,
                          uint8_t * cos);


/** Create Aging  profile 
 * @param[in] 		hndl      	    TM lib handle
 * @param[in]		profile_arr 	Drop Profile array
 *                                  configuration structure pointer
 * @param[out]      prof_index	    the first index of the created
 *                                  aging profile
 *
 *
 * @return an integer return code
 * @retval 0 on success
 * @retval -EINVAL         if either hndl or profile is NULL
 * @retval -EBADF          hndl is an invalid handle
 * @retval -EPERM          if the level is invalid 
 * @retval -EACCES         if the wred_catd_bw or cbtd_bw is out of range 
 * @retval -EDOM           if wred/catd are not configured correctly 
 * @retval -EADDRNOTAVAIL  if RED curve index in the profile is out of range 
 * @retval -EDOM           if cos in the profile is out of range
 * @retval -ENODEV         if RED curve doesn't exist
 * @retval -EFAULT         if exponents in the profile are out of range
 * @retval -ENODATA        if wred_catd_mode is invalid
 * @retval -ERANGE         if max_th is less that min_th
 * @retval -ENOSPC         if level's Drop Profiles table is fully allocated
 * @retval -EBADMSG        if AQM Mode params are not configured for this lvl
 * @retval TM_HW_DROP_PROFILE_FAILED if download to HW fails
*/
int tm_create_aging_profile(tm_handle hndl,
                           struct tm_drop_profile_params profile_arr[TM_AGING_PROFILES],
                           uint16_t * prof_index);


 /** Update Aging  profile 
 * @param[in] 		hndl      	    TM lib handle
 *                                  configuration structure pointer
 * @param[in]       prof_index	    the first index of the updated
 *                                  aging profile
 * @param[in]       offset 			drop profile number in aging profiles set
 *                                  aging profile
 * @param[in]		profile_arr 	Drop Profile array
 *
 *
 * @return an integer return code
 * @retval 0 on success
 * @retval -EINVAL         if either hndl or profile is NULL
 * @retval -EBADF          hndl is an invalid handle
 * @retval -EPERM          if the level is invalid 
 * @retval -EACCES         if the wred_catd_bw or cbtd_bw is out of range 
 * @retval -EDOM           if wred/catd are not configured correctly 
 * @retval -EADDRNOTAVAIL  if RED curve index in the profile is out of range 
 * @retval -EDOM           if cos in the profile is out of range
 * @retval -ENODEV         if RED curve doesn't exist
 * @retval -EFAULT         if exponents in the profile are out of range
 * @retval -ENODATA        if wred_catd_mode is invalid
 * @retval -ERANGE         if max_th is less that min_th
 * @retval -ENOSPC         if level's Drop Profiles table is fully allocated
 * @retval -EBADMSG        if AQM Mode params are not configured for this lvl
 * @retval TM_HW_DROP_PROFILE_FAILED if download to HW fails
*/
int tm_update_aging_profile(tm_handle hndl,
							uint16_t prof_index,
							uint16_t offset , /* 0 :: 3 */
                           struct tm_drop_profile_params  * profile_params);



/** Read node drop profile 
 *  As a result of aging process the queue drop profile can be changed dynamically 
 * internally by TM. This function allows to monitor ths changes.
 * @param[in] 	   hndl 	            TM lib handle
 * @param[in]      lvl					A nodes level  
 * @param[in]      cos                  CoS (not relevant for Q/A/B levels , 0:7 for C level , 0:7 or TM_INVAL for P level)
 * @param[in]      node_index           The node number to read drop profile ptr.
 * @param[in]      prof_index (pointer) Current drop profile for the node
 *
 * @return an integer return code
 * @retval 0 on success
 * @retval -EINVAL                   if hndl is NULL
 * @retval -EBADF                    if hndl is an invalid handle
 * @retval -EADDRNOTAVAIL            if prof_index is out of range 
 * @retval TM_HW_DROP_PROFILE_FAILED if read from H/W failed
 *
 * @note The CoS parameter is relevant for C and P level only, else set TM_INVAL.
 * @note For P level in Global mode set 'cos' = TM_INVAL, else 	profile will be created for CoS mode.
 */
int tm_read_node_drop_profile(tm_handle hndl, enum tm_level lvl , uint8_t cos, uint32_t node_index, uint16_t * prof_index);


/** Delete Aging  profile
 * @param[in] 	   hndl 	    TM lib handle
 * @param[in]	   prof_index	The first index to the profile
 *       to be deleted
 *
 * @return an integer return code
 * @retval 0 on success
 * @retval -EINVAL          if hndl is NULL
 * @retval -EBADF           if hndl is an invalid handle
 * @retval -EDOM            if the level is invalid 
 * @retval -ENODEV          if cos is out of range
 * @retval -EADDRNOTAVAIL   if prof_index is out of range 
 * @retval -EPERM           if profile is in use or reserved 
 * @retval -EFAULT          if no existing profile with this index
 * @retval TM_HW_DROP_PROFILE_FAILED if download to HW fails
*/
int tm_delete_aging_profile(tm_handle hndl,
                           uint16_t prof_index);



/** Calculate Bandwidth Delay Product (BDP) given bw in kilo bits/s
 * and delay in seconds.
 *
 * @param[in]    bw      bandwidth in kilo bits per second (not bits/s)
 * @param[in]    delay   delay in seconds
 *
 * @return Calculated bandwidth delay product in bytes
 */
/*
uint64_t tm_bdp(uint32_t bw, double delay);
*/

/**********************************************************************/
/* functions for internal usage */

/** Set default Drop configuration (curves & profiles) per level in database
 * @param[in] 	   hndl	        TM lib handle
 * @return an integer return code
 * @retval 0 on success
 * @retval -EINVAL           if hndl is NULL
 * @retval -EBADF            if hndl is an invalid handle
 * @retval -ENOSPC           if level's Drop Profiles/WRED Curves table is fully allocated
 */
int _tm_config_default_drop_sw(tm_handle hndl);

/** Set default values to drop related registers		
 * @param[in]        hndl            TM lib handle		
 * @return an integer return code		
 * @retval 0 on success		
 * @retval -EINVAL           if hndl is NULL		
 * @retval -EBADF            if hndl is an invalid handle	
 * @retval TM_HW_WRED_CURVE_FAILED if download to HW Curves fails 
 * @retval TM_HW_DROP_PROFILE_FAILED if download to HW Drop Profiles fails 
 */ 
int _tm_config_default_drop_hw(tm_handle hndl);


/** Get Next Drop profile
 *  get next drop profile index
 * @param[in] 	   hndl	        TM lib handle
 * @param[in]      level        A nodes level to read a Drop profile for 
 * @param[in]      cos	        CoS
 * @param[in out] prof_index_ptr pointer of drop profile index
 * @return an integer return code
 * @retval 0 on success
 * @retval -EINVAL next profile index has not found. 
 */
int tm_get_next_drop_profile(tm_handle hndl, 
                             enum tm_level level, 
                             uint8_t cos, 
                             uint16_t *prof_index_ptr);


int tm_read_drop_profile_hw(tm_handle     hndl,
                            enum tm_level level,
                            uint8_t       cos,
                            uint32_t      dp_index,
                            struct        tm_drop_profile *dp_hw_params_ptr);


#if 0
/** Get wred probability by queue size, considering colors
 *  resolution and max probability level parameters.
 * @param[in] 	   hndl	        TM lib handle 
 * @param[in]      lvl          Node tree level: (Q/A/B/C/Port). 
 * @param[in]      drop_params_ptr drop profile HW params got 
 *                                 from tm_read_drop_profile_hw
 *  
 * @param[in]      queue_size      buffer threshold in bytes
 * @param[in]      hw_drop_probabilities_arr               
 *                                 array 0..31 of hw curve drop
 *                                 probabilities got from
 *                                 tm_get_hw_wred_curve
 *  
 * @param[out]    dp_per_color_arr drop profile probabilities 
 *                                 per color 
 * @return an integer return code
 * @retval 0 on success
 * @retval -EINVAL           if hndl is NULL
 * @retval -EBADF            if hndl is an invalid handle
 */

int tm_get_wred_drop_probability_by_queue_size(tm_handle     hndl,
                                               enum tm_level level,
                                               struct tm_drop_profile *hw_drop_params_ptr, 
                                               uint32_t queue_size, 
                                               uint8_t  *hw_drop_probabilities_arr,
                                               int      *dp_per_color_arr);         


#endif


#endif   /* TM_DROP_H */

