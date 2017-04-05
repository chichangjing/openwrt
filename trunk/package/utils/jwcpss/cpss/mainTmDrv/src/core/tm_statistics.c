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
 * @brief tm_statistics implementation.
 *
 * @file tm_statistics.c
 *
 * $Revision: 2.0 $
 */

#include "tm_statistics.h"
#include "tm_errcodes.h"
#include <errno.h>
#include "set_hw_registers.h"

/**
 */
int tm_qmr_get_num_pkts_to_unins_queue(tm_handle hndl , U64 *  num_pkts_to_unins_queue)
{
   int rc = 0;
   TM_CTL(ctl, hndl);

	if (!num_pkts_to_unins_queue)
	{
        rc = -EFAULT;
        goto out;
    }

    rc = get_hw_qmr_num_pkts_to_unins_queue(ctl, num_pkts_to_unins_queue);
    if (rc)
        rc = TM_HW_GET_QMR_PKT_STAT_FAILED;

out:
    return rc;
}




/**
 */
int tm_rcb_get_num_pkts_to_sms(tm_handle hndl,  U64 *  num_pkts_to_sms)
{
    int rc = 0;

    TM_CTL(ctl, hndl);

    if (!num_pkts_to_sms)
	{
        rc = -EFAULT;
        goto out;
    }

    rc = get_hw_rcb_get_num_pkts_to_sms(ctl, num_pkts_to_sms);
    if (rc)
        rc = TM_HW_GET_RCB_PKT_STAT_FAILED;

out:
    return rc;
}

int tm_rcb_get_num_crc_err_pkts_to_sms(tm_handle hndl,  U64 * num_crc_err_pkts_to_sms)
{
    int rc = 0;

    TM_CTL(ctl, hndl);

    if (!num_crc_err_pkts_to_sms)
	{
        rc = -EFAULT;
        goto out;
    }

    rc = get_hw_rcb_get_num_num_crc_err_pkts_to_sms(ctl, num_crc_err_pkts_to_sms);
    if (rc)
        rc = TM_HW_GET_RCB_PKT_STAT_FAILED;

out:
    return rc;
}


int tm_rcb_get_num_errs_from_sms_to_dram(tm_handle hndl,  U64 * num_errs_from_sms_to_dram)
{
    int rc = 0;

    TM_CTL(ctl, hndl);

    if (!num_errs_from_sms_to_dram)
	{
        rc = -EFAULT;
        goto out;
    }

    rc = get_hw_rcb_get_num_errs_from_sms_to_dram(ctl, num_errs_from_sms_to_dram);
    if (rc)
        rc = TM_HW_GET_RCB_PKT_STAT_FAILED;

out:
    return rc;
}


int tm_rcb_get_num_port_flush_drp_pkts(tm_handle hndl,  U64 * num_port_flush_drp_pkts)
{
    int rc = 0;

    TM_CTL(ctl, hndl);

    if (!num_port_flush_drp_pkts)
	{
        rc = -EFAULT;
        goto out;
    }

    rc = get_hw_rcb_get_num_port_flush_drp_pkts(ctl, num_port_flush_drp_pkts);
    if (rc)
        rc = TM_HW_GET_RCB_PKT_STAT_FAILED;

out:
    return rc;
}
