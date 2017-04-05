#ifndef _TM_VIRTUAL_LAYER_INTERFACE_H_
#define _TM_VIRTUAL_LAYER_INTERFACE_H_
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
 * @brief interface for support of  virtual  ID's
 *
 * @file tm_virtual_layer_interface.h
 *
 * $Revision: 2.0 $
 */

#include "tm_defs.h"

int		tm_get_node_physical_id(tm_handle hndl, enum tm_level level, uint32_t virtual_ID , uint32_t * pPhysical_ID);
int		tm_get_node_virtual_id (tm_handle hndl, enum tm_level level, uint32_t physical_ID, uint32_t * pVirtual_ID);

#endif /*_TM_VIRTUAL_LAYER_INTERFACE_H_ */
