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
 * @brief tm2tm implementation.
 *
 * @file tm2tm.c
 *
 * $Revision: 2.0 $
 */

#include "tm_core_types.h"
#include "tm_errcodes.h"
#include "tm_locking_interface.h"
#include "tm_errcodes.h"
#include "tm_hw_configuration_interface.h"
#include "set_hw_registers.h"

/** WRED Max Probability Mode */
enum wred_max_p {
    MAX_P_12H = 0,   /* 12.5% */
    MAX_P_25,        /* 25%   */
    MAX_P_50,        /* 50%   */
    MAX_P_100        /* 100%  */
};

/****************/
/* System Setup */
/****************/

/**
 */
int tm2tm_global_config(tm_handle hndl,
                        uint8_t cos_sel,
                        struct tm_ext_hdr *ext_hdr,
                        struct tm_ctrl_pkt_str *control_pkt,
                        struct tm_delta_range *range)
{
    int rc;

    TM_CTL(ctl, hndl)

    rc = tm_glob_lock(hndl);
    if (rc)  return rc;

	/* check parameters validity */
    if (cos_sel >= TM_WRED_COS) {
        rc = TM_CONF_DP_COS_SEL_OOR;
        goto out;
    }

    if ((ext_hdr->size != 3) &&
        (ext_hdr->size != 7) &&
        (ext_hdr->size != 11) &&
        (ext_hdr->size != 15))
	{
        rc = TM_CONF_EXT_HDR_SIZE_OOR;
        goto out;
    }

    if ((control_pkt->ports > 31) ||
        (control_pkt->nodes > 31))
	{ /* 5 bits field */
        rc = TM_CONF_CTRL_PKT_STR_OOR;
    }

    if ((range->upper_range0 > 63) ||
        (range->upper_range1 > 63) ||
        (range->upper_range2 > 63))
	{ /* 6 bits field */
 /*       rc = TM_CONF_DELTA_RANGE_OOR; */
    }

    /* Download to HW */
    rc = set_hw_tm2tm_glob_config(ctl, cos_sel, ext_hdr, control_pkt, range);
    if (rc)
        rc = TM_HW_TM2TM_GLOB_CONF_FAILED;
out:
    tm_glob_unlock(hndl);
    return rc;
}


/**
 */
int tm2tm_port_channel_config(tm_handle hndl,
                              uint8_t egr_elems,
                              enum tm_level src_lvl,
                              enum tm2tm_mode mode,
                              enum tm_level bp_lvl,
                              uint16_t bp_offset,
                              uint8_t bp_xon,
                              uint8_t bp_xoff)
{
    int rc;

	TM_CTL(ctl, hndl)

    rc = tm_glob_lock(hndl);
    if (rc) return rc;

	/* check parameters validity */
    if ((egr_elems > 63) || (egr_elems%4 != 0))
	{
        rc = TM_CONF_TM2TM_EGR_ELEMS_OOR;
        goto out;
    }

    if (src_lvl == Q_LEVEL)
	{
        rc = TM_CONF_TM2TM_SRC_LVL_OOR;
        goto out;
    }
    if (mode == TM2TM_BP)
	{
        /* TBD: Is resolution check is needed here? */
        if ((bp_lvl != Q_LEVEL) && (bp_lvl != C_LEVEL))
		{
            rc = TM_CONF_TM2TM_BP_LVL_OOR;
            goto out;
        }
        if ((bp_xon > 63) || (bp_xoff > 63))
		{
            rc = TM_CONF_TM2TM_BP_THRESH_OOR;
            goto out;
        }
    }

    /* Set SW image */
    ctl->port_ch.egr_elems = egr_elems;
    ctl->port_ch.src_lvl = src_lvl;
    ctl->port_ch.mode = mode;
    if (mode == TM2TM_BP)
	{
        ctl->port_ch.bp_map = bp_lvl;
        ctl->port_ch.bp_offset = bp_offset;
        ctl->port_ch.bp_xon = bp_xon;
        ctl->port_ch.bp_xoff = bp_xoff;
    }
    ctl->port_ch.configured = TM_ENABLE;

    if (ctl->node_ch.configured == TM_ENABLE)
	{
        rc = set_hw_tm2tm_channels(ctl); /* Download to HW */
        if (rc) rc = TM_HW_TM2TM_CHANNEL_CONF_FAILED;
    }
out:
    tm_glob_unlock(hndl);
    return rc;
}


/**
 */
int tm2tm_node_channel_config(tm_handle hndl,
                              uint16_t egr_elems,
                              enum tm_level src_lvl,
                              enum tm2tm_mode mode,
                              enum tm_level bp_lvl,
                              uint16_t bp_offset,
                              uint8_t bp_xon,
                              uint8_t bp_xoff)
{
    int rc;

	TM_CTL(ctl, hndl)

    rc = tm_glob_lock(hndl);
    if (rc)  return rc;

    /* check parameters validity */
    if (egr_elems%16 != 0)
	{
        rc = TM_CONF_TM2TM_EGR_ELEMS_OOR;
        goto out;
    }

    if (src_lvl == P_LEVEL)
	{
        rc = TM_CONF_TM2TM_SRC_LVL_OOR;
        goto out;
    }

    if (mode == TM2TM_BP)
	{
        /* TBD: Is resolution check is needed here? Additional
         * resolution check is performed in AQM ingress mode set */
        if ((bp_lvl != Q_LEVEL) && (bp_lvl != C_LEVEL))
		{
            rc = TM_CONF_TM2TM_BP_LVL_OOR;
            goto out;
        }

        if ((bp_xon > 64) || (bp_xoff > 64))
		{
            rc = TM_CONF_TM2TM_BP_THRESH_OOR;
            goto out;
        }
    }

    /* Set SW image */
    ctl->node_ch.egr_elems = egr_elems;
    ctl->node_ch.src_lvl = src_lvl;
    ctl->node_ch.mode = mode;
    if (mode == TM2TM_BP)
	{
        ctl->node_ch.bp_map = bp_lvl;
        ctl->node_ch.bp_offset = bp_offset;
        ctl->node_ch.bp_xon = bp_xon;
        ctl->node_ch.bp_xoff = bp_xoff;
    }
    ctl->node_ch.configured = TM_ENABLE;

    if (ctl->port_ch.configured == TM_ENABLE)
	{
        rc = set_hw_tm2tm_channels(ctl); /* Download to HW */
        if (rc) rc = TM_HW_TM2TM_CHANNEL_CONF_FAILED;
    }
out:
    tm_glob_unlock(hndl);
    return rc;
}


/**
 */
int tm_set_dp_query_resp_lvl(tm_handle hndl,
                             uint8_t local_or_remote,
                             uint8_t port_dp,
                             enum tm_level local_lvl, /* Q/A/B/C/Port */
                             enum tm2tm_channel remote_lvl) /* Node/Port */
{
    int rc;

    TM_CTL(ctl, hndl)

    rc = tm_glob_lock(hndl);
    if (rc)  return rc;

    /* check parameters validity */
    if (local_lvl > P_LEVEL)
	{
        rc = TM_CONF_TM2TM_DP_LVL_OOR;
        goto out;
    }

    if ((remote_lvl != TM2TM_PORT_CH) && (remote_lvl != TM2TM_NODE_CH))
	{
        rc = TM_CONF_TM2TM_DP_LVL_OOR;
        goto out;
    }

    switch (local_or_remote)
	{
    case 2:
        rc = set_hw_dp_remote_resp(ctl, remote_lvl);
        if (rc)
		{
            rc = TM_HW_DP_QUERY_RESP_CONF_FAILED;
            goto out;
        }
    case 0:
        rc = set_hw_dp_local_resp(ctl,
                                  port_dp,
                                  local_lvl);
        if (rc)
		{
            rc = TM_HW_DP_QUERY_RESP_CONF_FAILED;
            goto out;
        }
        break;
    case 1:
        rc = set_hw_dp_remote_resp(ctl, remote_lvl);
        if (rc)
		{
            rc = TM_HW_DP_QUERY_RESP_CONF_FAILED;
            goto out;
        }
        break;
    default:
        rc = TM_CONF_TM2TM_DP_LVL_OOR;
    }
out:
    tm_glob_unlock(hndl);
    return rc;
}


/************/
/* LC Setup */
/************/

/**
 */
int tm2tm_lc_config(tm_handle hndl,
                    uint16_t quantum,
                    void *  qmap_hdr_ptr,
                    uint8_t ctrl_hdr,
                    uint8_t tm_port)
{
    int rc;
    TM_CTL(ctl, hndl)

    rc = tm_glob_lock(hndl);
    if (rc) return rc;

    /* check parameters validity */
    if (ctrl_hdr >= TM_LC_NUM)
	{
        rc = TM_CONF_TM2TM_CTRL_HDR_OOR;
        goto out;
    }

    if (tm_port >= ctl->tm_total_ports)
	{
        rc = TM_CONF_TM2TM_PORT_FOR_CTRL_PKT_OOR;
        goto out;
    }

    /* Download to HW */
    rc = set_hw_tm2tm_lc_config(ctl, quantum, qmap_hdr_ptr, ctrl_hdr, tm_port);
    if (rc) rc = TM_HW_TM2TM_LC_CONF_FAILED;
out:
    tm_glob_unlock(hndl);
    return rc;
}


/**
 */
int tm2tm_set_egress_drop_aqm_mode(tm_handle hndl,
                                   enum tm_level lvl,
                                   enum tm_color_num num,
                                   enum tm_dp_source *src)
{
    uint8_t color_num = 3;
    int rc;
    int i;

    TM_CTL(ctl, hndl)

    rc = tm_glob_lock(hndl);
    if (rc)  return rc;

	/* check parameters validity */
    if (lvl > P_LEVEL)
	{
        rc = -EPERM;
        goto out;
    }

    if (num > TM_3_COLORS)
	{
        rc = -EFAULT;
        goto out;
    }

    switch (num)
	{
    case TM_1_COLORS:
        color_num = 1;
        break;
    case TM_2_COLORS:
        color_num = 2;
        break;
    case TM_3_COLORS:
        color_num = 3;
        break;
    }

    for (i=0; i<color_num; i++)
	{
        if (src[i] > TM_QL)
		{
            rc = -EADDRNOTAVAIL;
            goto out;
        }
    }

#if 0 /* AAB */
    /* check if the level mapped to some channel */
    if (ctl->node_ch.src_lvl == lvl) /* Node Channel */
        ch = TM2TM_NODE;
    else /* Port Channel */
        if (ctl->port_ch.src_lvl == lvl)
            ch = TM2TM_PORT;
#endif

    /* Should be called before Ingress AQM Mode settings, so
     * no check for Ingress configuration */

    /* TBD: set the resolution mode, if it's clear at this stage !!! */

    /* TBD: What about Egress case only? Ingress must be called with
     * default configuration at least in order to enable the TM2TM. Add
     * comment to User Guide.  */

    ctl->dp_unit.local[lvl].color_num = color_num;

    for (i=0; i<color_num; i++)
        ctl->dp_unit.local[lvl].dp_src[i] = src[i];

    /* TBD: how to reduce the num of these calls? */
    rc = set_hw_tm2tm_aqm_mode(hndl);
    if (rc) {
        rc = TM_HW_AQM_CONFIG_FAIL;
        goto out;
    }

    ctl->dp_unit.local[lvl].configured = TM_ENABLE;
out:
    tm_glob_unlock(hndl);
    return rc;
}


/**
 */
int tm2tm_set_ingress_drop_aqm_mode(tm_handle hndl,
                                    enum tm2tm_channel ch,
                                    enum tm_color_num num)
{
    enum tm_level src_lvl;
    uint8_t color_num;
    int rc;

    TM_CTL(ctl, hndl)

	rc = tm_glob_lock(hndl);
    if (rc)  return rc;
    /* check parameters validity */
    /* No need to check the enum values - ch & num */

    /* Channel must be already configured at system setup! */
    if (ch == TM2TM_NODE_CH)
	{ /* Node Channel */
        if (ctl->node_ch.configured == TM_DISABLE)
		{
            rc = TM_CONF_TM2TM_CHANNEL_NOT_CONFIGURED;
            goto out;
        }
    }
	else
	{ /* Port Channel */
        if (ctl->port_ch.configured == TM_DISABLE)
		{
            rc = TM_CONF_TM2TM_CHANNEL_NOT_CONFIGURED;
            goto out;
        }
    }


    /* Check if Egress AQM mode settings for mapped level already configured */
    if (ch == TM2TM_NODE_CH)
	{ /* Node Channel */
        if ((ctl->port_ch.mode == TM2TM_BP) && (num != TM_1_COLORS))
		{
            rc = TM_CONF_TM2TM_AQM_INVALID_COLOR_NUM;
            goto out;
        }
        src_lvl = ctl->node_ch.src_lvl;
    }
	else
	{ /* Port Channel */
        if ((ctl->node_ch.mode == TM2TM_BP) && (num != TM_1_COLORS))
		{
            rc = TM_CONF_TM2TM_AQM_INVALID_COLOR_NUM;
            goto out;
        }
        src_lvl = ctl->port_ch.src_lvl;
    }

    if (ctl->dp_unit.local[src_lvl].configured == TM_ENABLE)
	{
        color_num = ctl->dp_unit.local[src_lvl].color_num;
        if (color_num == 3)
		{
            rc = TM_CONF_TM2TM_AQM_INVALID_COLOR_NUM;
            goto out;
        }
        if ((num == TM_2_COLORS) && (color_num == 2))
		{
            rc = TM_CONF_TM2TM_AQM_INVALID_COLOR_NUM;
            goto out;
        }
    }

    /* TDB: Ingress case only. Add coment to User Guide to call
     * egress AQM mode with some simple config in order to enable TM2TM */

    switch(num)
	{
    case TM_1_COLORS:
        ctl->dp_unit.remote[ch].color_num = 1;
        break;
    case TM_2_COLORS:
        ctl->dp_unit.remote[ch].color_num = 2;
        break;
    case TM_3_COLORS:
        ctl->dp_unit.remote[ch].color_num = 3;
        break;
    }

    /* TBD: set the resolution mode!!! */

    /* Download to HW */
    rc = set_hw_tm2tm_aqm_mode(hndl);
    if (rc)
	{
        rc = TM_HW_AQM_CONFIG_FAIL;
        goto out;
    }

    ctl->dp_unit.remote[ch].configured = TM_ENABLE;
out:
    tm_glob_unlock(hndl);
    return rc;
}


/**
 */
int tm2tm_enable(tm_handle hndl)
{
    enum tm_level lvl;
    enum tm_level lvl_t;
    enum tm2tm_channel ch;
    uint8_t color_num;
    int rc;
    int i;
    uint8_t flag = TM_DISABLE;
    uint8_t flag_glob = TM_DISABLE;

    TM_CTL(ctl, hndl)

    rc = tm_glob_lock(hndl);
    if (rc) return rc;

    /* Check that AQM is configured for all levels! */
    /* Local AQM */
    for (lvl = Q_LEVEL; lvl <= P_LEVEL; lvl++)
	{
        if (ctl->dp_unit.local[lvl].configured == TM_DISABLE)
		{
            /* No dependencies between color num and resolution from level to level */
            if (lvl == Q_LEVEL)
			{
                for (lvl_t = A_LEVEL; lvl_t <= P_LEVEL; lvl_t++)
				{
                    if (ctl->dp_unit.local[lvl_t].configured == TM_ENABLE)
					{
                        /* copy aqm mode setting from this level */
                        ctl->dp_unit.local[lvl].color_num =
                            ctl->dp_unit.local[lvl_t].color_num;
                        color_num = ctl->dp_unit.local[lvl].color_num;
                        for (i=0; i<color_num; i++)
                            ctl->dp_unit.local[lvl].dp_src[i] =
                                ctl->dp_unit.local[lvl_t].dp_src[i];
                        /* No need to update max_p_mode */
                        flag = TM_ENABLE;
                        break;
                    }
                }
                if (flag == TM_DISABLE)
				{ /* No local aqm config at all */
                    ctl->dp_unit.local[lvl].color_num = 2; /* 2x6 */
                    /* No need to update dp_src & max_p_mode, they are
                     * alredy zero by default */
                }
            }
			else
			{
                /* aqm mode setting as in prev level */
                ctl->dp_unit.local[lvl].color_num = ctl->dp_unit.local[lvl-1].color_num;
                for (i=0; i<3; i++)
                    ctl->dp_unit.local[lvl].dp_src[i] = ctl->dp_unit.local[lvl - 1].dp_src[i];
                /* No need to update max_p_mode */
            }
            ctl->dp_unit.local[lvl].configured = TM_ENABLE;
            flag_glob = TM_ENABLE;
        }
    }

    for (ch = TM2TM_NODE_CH; ch <= TM2TM_PORT_CH; ch++)
	{
        if (ctl->dp_unit.remote[ch].configured == TM_DISABLE)
		{
            ctl->dp_unit.remote[ch].color_num = 0;
            ctl->dp_unit.remote[ch].configured = TM_ENABLE;
            flag_glob = TM_ENABLE;
        }
    }

    if (flag_glob == TM_ENABLE)
	{
        rc = set_hw_tm2tm_aqm_mode(hndl);
        if (rc)
		{
            rc = TM_HW_AQM_CONFIG_FAIL;
            goto out;
        }
    }

    /* Enable TM2TM */
    rc = set_hw_tm2tm_enable(ctl);
    if (rc)
        rc = TM_HW_TM2TM_ENABLE_FAILED;
out:
    tm_glob_unlock(hndl);
    return rc;
}
