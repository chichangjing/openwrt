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
 * @brief TM node reordering API.
 *
 * @file tm_nodes_reorder.h
 *
 * $Revision: 2.0 $
 */

#ifndef TM_NODES_REORDER_H
#define TM_NODES_REORDER_H

#include "tm_defs.h"


 /** Switch children between two nodes.
  *
  *  @param[in]    hndl                 TM lib handle
  *  @param[in]    level                Node level
  *  @param[in]    node_a               Node A index in switch
  *  @param[in]    node_b               Node B index in switch
  *
  *  @return an integer return code
  *  @retval 0 on success
  *  @retval -EINVAL if hndl is NULL
  *  @retval -EBADF if hndl is invalid
  *  @retval -ERANGE if level is invalid
  *  @retval TM_CONF_[LEVEL]_NODE_IND_OOR if node index is not available on level
  */                  
int tm_nodes_switch(tm_handle hndl,
                    enum tm_level level,
                    uint16_t node_a,
                    uint16_t node_b);


#endif   /* TM_NODES_REORDER_H */

