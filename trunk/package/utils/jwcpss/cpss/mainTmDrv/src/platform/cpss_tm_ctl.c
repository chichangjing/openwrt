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
 * @brief tm_ctl  implementation for cpss
 *
 * @file cpss_tm_ctl.c
 *
 * $Revision: 2.0 $
 */


#include "tm_core_types.h"
#include "tm_ctl_internal.h"

#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>
#include <cpss/generic/cpssHwInit/private/prvCpssHwRegisters.h>

#include "tm_os_interface.h"
#include "tm_locking_interface.h"
#include "tm_connection_management.h"

#include "tm_errcodes.h"


/**
 */
int tm_lib_open_ext(GT_U8 devNum, struct tm_tree_structure *tree_struct_ptr, struct tm_lib_init_params *init_params_PTR, tm_handle * pHndl)
{
    GT_U32  data;  
    int rc = 0;
    GT_STATUS   ret;

	struct cpss_tm_environment * hEnv=NULL;

    if (!pHndl) return  -EINVAL;

    /*check whether we are in burst mode or non burst mode*/
    ret = cpssDrvHwPpResetAndInitControllerReadReg(devNum, 0x000F828C ,&data);
    if (GT_OK != ret)   return ret;

	hEnv = (struct cpss_tm_environment * )new_cpss_tm_environment_handle(devNum);
	/* copy TM tree structure parameters */
	tm_memcpy(&(hEnv->tree_structure),tree_struct_ptr,sizeof(struct tm_tree_structure));

    hEnv->burstMode =  ((data & 0x10)==0);
/* temporary   : burst mode is suppressed */
hEnv->burstMode = 0;

	rc =tm_create_locking_staff((void *) hEnv);
	if  (rc) goto out;

	rc = set_hw_connection((void *) hEnv);
	if  (rc) goto out;

	rc = tm_lib_sw_init((void *)hEnv, pHndl, init_params_PTR);
	if  (rc) goto out;

	rc = tm_lib_hw_init(*pHndl);

out:
    if (rc)
	{
		if (*pHndl)
		{
			tm_lib_close_sw(*pHndl);
			*pHndl = NULL;
		}
		if (hEnv) tm_free(hEnv);
	}
    return rc;
}

/**
 */
int tm_lib_open(GT_U8 devNum,struct tm_tree_structure *tree_struct_ptr, tm_handle *pHndl)
{
    struct tm_lib_init_params init_params;
    int rc;
    tm_memset(&init_params, 0, sizeof(init_params));

    rc = tm_lib_open_ext(devNum, tree_struct_ptr, &init_params, pHndl);

    return rc;
}



/**
 */
int tm_lib_close(tm_handle hndl)
{
	DECLARE_CPSS_ENV(hEnv)

	TM_CTL(ctl,hndl)

	ASSIGN_CPSS_ENV(hEnv,TM_ENV(ctl))

	tm_lib_close_sw(hndl);

	tm_destroy_locking_staff((void *)hEnv);

    close_hw_connection(hndl);

    return 0;
}

/**
 */
