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
 * @brief basic TM read/write/reset  functions implementation and connection support  for cpss
 *
 * @file cpss_hw_interaction_implementation.c
 *
 * $Revision: 2.0 $
 */
#include "tm_core_types.h"
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>
#include <cpss/generic/cpssHwInit/private/prvCpssHwRegisters.h>


#include "cpss_tm_rw_registers_proc.h"

#include "tm_rw_registers_interface.h"
#include "tm_get_gen_param_interface.h"

#include "tm_errcodes.h"
#include "set_hw_registers.h"
#include <stdio.h>



#define CPSS_TO_XEL_ERR_CODE(x)     x


#define TM_FLUSH_MAX_RETRIES     16


/**
 */
int set_hw_connection(void * environment_handle)
{
    int rc = 0;
	(void)environment_handle;
    return rc;
}


/**
 */
int flush_hw_connection(void * environment_handle)
{
    int rc = 0;
	(void)environment_handle;
    return rc;
}


/**
 */
int reset_hw_connection(void * environment_handle)
{
    int rc = 0;
    (void)environment_handle;
    return rc;
}


/**
 */
int close_hw_connection(void * environment_handle)
{
    int rc = 0;
    (void)environment_handle;
    return rc;
}

int tm_register_reset(void * environment_handle , GT_U64   Address , void * vpData)
{
    (void)environment_handle;
	(void)Address;
	(void)vpData;

	return 0;
}

int tm_table_entry_reset(void * environment_handle , GT_U64 Address , long int index, void * vpData)
{
    (void)environment_handle;
	(void)Address;
	(void)index;
	(void)vpData;
	return 0;
}



/**
 */
int tm_register_read(void * environment_handle,
                                GT_U64 reg_addr,
                                void *data)
{
	CPSS_ENV(env,environment_handle, -EINVAL, -EBADF)
	return CPSS_TO_XEL_ERR_CODE(tm_read_register_proc(env->devNumber, env->burstMode ,  reg_addr,data));

}


/**
 */
int tm_register_write(void * environment_handle,
                                 GT_U64 reg_addr,
                                 void *data)
{
	CPSS_ENV(env,environment_handle, -EINVAL, -EBADF)
	return CPSS_TO_XEL_ERR_CODE(tm_write_register_proc(env->devNumber, env->burstMode ,  reg_addr,data));
}


/**
 */
int tm_table_entry_read(void * environment_handle,
                                   GT_U64 tbl_addr, /* table address */
                                   uint32_t index,  /* table entry index */
                                   void *data)      /* entry value */
{
    GT_U32      offset;
    GT_U32      tempAddr;
    GT_U64      entryAddr;


	CPSS_ENV(env,environment_handle, -EINVAL, -EBADF)

    /* Table index handle */
    offset = index * 8;
    tempAddr = tbl_addr.l[0]/8;

    /* TBD Assumption: index is not big enough to influence the MSB part of the table address */
    if((tempAddr + index) > 0x1FFFFFFF) {
        entryAddr.l[0] = (tempAddr + index - 0x1FFFFFFF)*8 + (tbl_addr.l[0] - tempAddr*8);
        entryAddr.l[1] = (tbl_addr.l[1] + 1);
    }
    else {
        entryAddr.l[0] = (tbl_addr.l[0] + offset);
        entryAddr.l[1] = tbl_addr.l[1];
    }

	return CPSS_TO_XEL_ERR_CODE(tm_read_register_proc(env->devNumber, env->burstMode ,  entryAddr, data));

}

/**
 */
int tm_table_entry_write(void * environment_handle,
                                    GT_U64 tbl_addr, /* table address */
                                    uint32_t index,  /* table entry index */
                                    void *data)      /* entry value */
{
    GT_U32      offset;
    GT_U32      tempAddr;
    GT_U64      entryAddr;

	CPSS_ENV(env,environment_handle, -EINVAL, -EBADF)

    /* Table index handle */
    offset = index * 8;
    tempAddr = tbl_addr.l[0]/8;


    /* TBD Assumption: index is not big enough to influence the MSB part of the table address */
    if((tempAddr + index) > 0x1FFFFFFF) {
        entryAddr.l[0] = (tempAddr + index - 0x1FFFFFFF)*8 + (tbl_addr.l[0] - tempAddr*8);
        entryAddr.l[1] = (tbl_addr.l[1] + 1);
    }
    else {
        entryAddr.l[0] = (tbl_addr.l[0] + offset);
        entryAddr.l[1] = tbl_addr.l[1];
    }

	return CPSS_TO_XEL_ERR_CODE(tm_write_register_proc(env->devNumber, env->burstMode ,  entryAddr, data));

}

int tm_get_gen_params(tm_handle hndl)
{
    int rc = 0;
    
	GT_STATUS ret;
    GT_U32 tmFreq;
	struct cpss_tm_environment * env ;
	
	TM_CTL(ctl, hndl)

	/* because of limitation of variable declarations the macro is not used here */
	env = (struct cpss_tm_environment *)(TM_ENV(ctl));

    /* Read TM frequency */
    ret = cpssDrvHwPpResetAndInitControllerReadReg(env->devNumber, 0x000f8204, &tmFreq);
    if (GT_OK != ret)
        return CPSS_TO_XEL_ERR_CODE(ret);

#ifdef ASIC_SIMULATION
    TM_U32_SET_FIELD(tmFreq,15,3,2/*466MHz*/);
#endif /* ASIC_SIMULATION*/

    switch(TM_U32_GET_FIELD(tmFreq, 15/*offset*/, 3/*length*/)) {
    case 0:
        return -EFAULT;
    case 1:
        ctl->freq = 400000000; /* 400MHz */
        break;
    case 2:
        ctl->freq = 466000000; /* 466MHz */
        break;
    case 3:
        ctl->freq = 333000000; /* 333MHz */
        break;
    default:
        return -EFAULT;
    }
    /* calculate TM frequency */
    /*ctl->freq = ctl->freq/2; */

    /* get other general parameters */
    rc = get_hw_gen_params(hndl);
    /*fprintf(stderr, "%s ----------------------------------- 3 rc=%d\n\n",__FUNCTION__,rc); */
    if (rc) {
        return TM_HW_GEN_CONFIG_FAILED;
    }

    return rc;
}
