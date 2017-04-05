/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDrvExMxEventsPuma3.h
*
* DESCRIPTION:
*       This file includes all different hardware driven Event types - Puma3
*
*       Applicable devices:
*                           Puma3
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*
*******************************************************************************/
#ifndef __prvCpssDrvExMxEventsPuma3h
#define __prvCpssDrvExMxEventsPuma3h

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* generates range of reserved values to be skipped in ENUM */
#define PRV_CPSS_PM3INT_RESERVED_BITS_MAC(_reg_id, _start_bit, _end_bit) \
    PRV_CPSS_PM3INT_##_reg_id##_RESERVED_##_start_bit##_E,               \
    PRV_CPSS_PM3INT_##_reg_id##_RESERVED_##_end_bit##_E =                \
    (PRV_CPSS_PM3INT_##_reg_id##_RESERVED_##_start_bit##_E + (_end_bit - _start_bit))

/* align next ENUM value to _end_bit */
#define PRV_CPSS_PM3INT_BIT_ALIGN_MAC(_reg_id, _end_bit)       \
    PRV_CPSS_PM3INT_##_reg_id##_BIT_ALIGN_##_end_bit##_0_E,    \
    PRV_CPSS_PM3INT_##_reg_id##_BIT_ALIGN_##_end_bit##_1_E =   \
    ((PRV_CPSS_PM3INT_##_reg_id##_BIT_ALIGN_##_end_bit##_0_E & (~ 0x1F))| _end_bit)

/* align next ENUM value to 32 */
#define PRV_CPSS_PM3INT_ALIGN31_MAC(_reg_id)    \
    PRV_CPSS_PM3INT_##_reg_id##_ALIGN32_0_E,    \
    PRV_CPSS_PM3INT_##_reg_id##_ALIGN32_1_E =   \
    ((PRV_CPSS_PM3INT_##_reg_id##_ALIGN32_0_E - 1) | 0x1F)

/* typical register (array member): bit0 - general               */
/* bits 1-32 related to index ((reg_index*31)+(bit_index-1))     */
#define PRV_CPSS_PM3INT_INDEXED_BIT_REG_MAC(_reg_id, _val_id, _idx)         \
    PRV_CPSS_PM3INT_##_reg_id##_##_idx##_GENERAL_E,                         \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_0_E,       \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_1_E,       \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_2_E,       \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_3_E,       \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_4_E,       \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_5_E,       \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_6_E,       \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_7_E,       \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_8_E,       \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_9_E,       \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_10_E,      \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_11_E,      \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_12_E,      \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_13_E,      \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_14_E,      \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_15_E,      \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_16_E,      \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_17_E,      \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_18_E,      \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_19_E,      \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_20_E,      \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_21_E,      \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_22_E,      \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_23_E,      \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_24_E,      \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_25_E,      \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_26_E,      \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_27_E,      \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_28_E,      \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_29_E,      \
    PRV_CPSS_PM3INT_##_reg_id##_##_val_id##_##_idx##_MUL_31_PLUS_30_E

/***************************************************************************/

/*
 * Typedef: enum PRV_CPSS_EXMXPM3_INT_CAUSE_ENT
 *
 * Description: All interrupt cause indexes, to be used for interrupts handling,
 *              and parameters change.
 *
 * NOTE : If needed, new interrupt bits can replace the XXX_RES_i bits, in case
 *        that these bits fall into the same cause registers.
 *
 * Comment:
 *      Please see the description of each of the following interrupt event
 *      cause registers in the packet processor datasheet.
 */
typedef enum{
    /*Global Interrupt Cause  address[0x00000030]*/
    /* Tree place: .*/
    /*INTERRUPT ID BASE:[0]*/
    PRV_CPSS_PM3INT_GLOBAL_GENERAL_SUM_E                             /* = 0*/ ,
    PRV_CPSS_PM3INT_GLOBAL_PEX_INT_SUM_E                             /* = 1*/ ,  /* SUBTREE 0x00001900*/
    PRV_CPSS_PM3INT_GLOBAL_PEX_ERR_SUM_E                             /* = 2*/ ,  /* SUBTREE 0x000003F8*/
    PRV_CPSS_PM3INT_GLOBAL_FUNC_UNITS_INT_SUM_E                      /* = 3*/ ,  /* SUBTREE 0x000003F8*/
    PRV_CPSS_PM3INT_GLOBAL_DATA_PATH_INT_SUM_E                       /* = 4*/ ,  /* SUBTREE 0x000000A4*/
    PRV_CPSS_PM3INT_GLOBAL_PORTS_INT_SUM_E                           /* = 5*/ ,  /* SUBTREE 0x00000080*/
    PRV_CPSS_PM3INT_GLOBAL_DFX_INT_SUM_E                             /* = 6*/ ,  /* SUBTREE 0x000000AC*/
    PRV_CPSS_PM3INT_GLOBAL_MG_INTERNAL_INT_SUM_E                     /* = 7*/ ,  /* SUBTREE 0x00000038*/
    PRV_CPSS_PM3INT_GLOBAL_TX_SDMA_SUM_E                             /* = 8*/ ,  /* SUBTREE 0x00002810*/
    PRV_CPSS_PM3INT_GLOBAL_RX_SDMA_SUM_E                             /* = 9*/ ,  /* SUBTREE 0x0000280C*/
    PRV_CPSS_PM3INT_GLOBAL_DFX1_INT_SUM_E                            /* = 10*/ , /* SUBTREE 0x000000B8*/
    PRV_CPSS_PM3INT_GLOBAL_FUNC_UNITS1_INT_SUM_E                     /* = 11*/ , /* SUBTREE 0x000003F0*/
    PRV_CPSS_PM3INT_GLOBAL_XSMI_INT_SUM_E                            /* = 12*/ , /* SUBTREE 0x00040010*/
    PRV_CPSS_PM3INT_ALIGN31_MAC(GLOBAL)                              /* = 13-31*/,

    /*PEX Interrupt Cause  address[0x00001900]*/
    /* Tree place: .1(bits[31:24]) and .2(bits[23:0]) */
    /*INTERRUPT ID BASE:[32]*/
    PRV_CPSS_PM3INT_PEX_TX_REQ_WHEN_DL_DOWN_E                        /* = 32*/ ,
    PRV_CPSS_PM3INT_PEX_TRANS_WHEN_MASTER_DISABLE_E                  /* = 33*/ ,
    PRV_CPSS_PM3INT_PEX_RESERVED2_E                                  /* = 34*/ ,
    PRV_CPSS_PM3INT_PEX_ERR_WRITE_INTERN_REG_E                       /* = 35*/ ,
    PRV_CPSS_PM3INT_PEX_HIT_DEFAULT_WIN_ERR_E                        /* = 36*/ ,
    PRV_CPSS_PM3INT_PEX_RESERVED5_E                                  /* = 37*/ ,
    PRV_CPSS_PM3INT_PEX_RX_RAM_PARITY_ERR_E                          /* = 38*/ ,
    PRV_CPSS_PM3INT_PEX_TX_RAM_PARITY_ERR_E                          /* = 39*/ ,
    PRV_CPSS_PM3INT_PEX_CORRECTABLE_ERR_E                            /* = 40*/ ,
    PRV_CPSS_PM3INT_PEX_NON_FATAL_ERR_E                              /* = 41*/ ,
    PRV_CPSS_PM3INT_PEX_FATAL_ERR_E                                  /* = 42*/,
    PRV_CPSS_PM3INT_PEX_D_STATE_CHANGED_E                            /* = 43*/,
    PRV_CPSS_PM3INT_PEX_BIST_ACTIVATED_E                             /* = 44*/,
    PRV_CPSS_PM3INT_PEX_RESERVED13_E                                 /* = 45*/ ,
    PRV_CPSS_PM3INT_PEX_FLOW_CONTROL_ERR_E                           /* = 46*/ ,
    PRV_CPSS_PM3INT_PEX_RCV_UR_OR_CA_ERR_E                           /* = 47*/ ,
    PRV_CPSS_PM3INT_PEX_RCV_FATAL_ERR_MSG_E                          /* = 48*/ ,
    PRV_CPSS_PM3INT_PEX_RCV_NON_FATAL_ERR_MSG_E                      /* = 49*/ ,
    PRV_CPSS_PM3INT_PEX_RCV_ERR_COR_MSG_E                            /* = 50*/ ,
    PRV_CPSS_PM3INT_PEX_RCV_CFG_REQ_RETRY_E                          /* = 51*/ ,
    PRV_CPSS_PM3INT_PEX_RCV_HOT_RESET_MSG_E                          /* = 52*/ ,
    PRV_CPSS_PM3INT_PEX_SLAVE_DIABLE_LINK_E                          /* = 53*/ ,
    PRV_CPSS_PM3INT_PEX_SLAVE_LOOPBACK_E                             /* = 54*/ ,
    PRV_CPSS_PM3INT_PEX_LINK_FAIL_E                                  /* = 55*/ ,
    PRV_CPSS_PM3INT_PEX_RCV_INT_A_MSG_E                              /* = 56*/ ,
    PRV_CPSS_PM3INT_PEX_RCV_INT_B_MSG_E                              /* = 57*/ ,
    PRV_CPSS_PM3INT_PEX_RCV_INT_C_MSG_E                              /* = 58*/ ,
    PRV_CPSS_PM3INT_PEX_RCV_INT_D_MSG_E                              /* = 59*/ ,
    PRV_CPSS_PM3INT_PEX_RCV_PM_PME_MSG_E                             /* = 60*/ ,
    PRV_CPSS_PM3INT_PEX_RCV_TURN_OFF_MSG_E                           /* = 61*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PEX)                                 /* = 62-63*/,

    /*FUNC Interrupt Cause  address[0x000003F8]*/
    /* Tree place: .3*/
    /*INTERRUPT ID BASE:[64]*/
    PRV_CPSS_PM3INT_FUNC_GENERAL_SUM_E                               /* = 64*/ ,
    PRV_CPSS_PM3INT_FUNC_PCL_SUM_E                                   /* = 65*/ ,
    PRV_CPSS_PM3INT_FUNC_TTI_SUM_E                                   /* = 66*/ ,
    PRV_CPSS_PM3INT_FUNC_PCL_TCAM0_SUM_E                             /* = 67*/ ,
    PRV_CPSS_PM3INT_FUNC_PCL_TCAM1_SUM_E                             /* = 68*/ ,
    PRV_CPSS_PM3INT_FUNC_ETC_SUM_E                                   /* = 69*/ ,
    PRV_CPSS_PM3INT_FUNC_LOOPBACK_PORT_SUM_E                         /* = 70*/ ,
    PRV_CPSS_PM3INT_FUNC_EPLR_SUM_E                                  /* = 71*/ ,
    PRV_CPSS_PM3INT_FUNC_L2I_SUM_E                                   /* = 72*/ ,
    PRV_CPSS_PM3INT_FUNC_FDB_SUM_E                                   /* = 73*/ ,
    PRV_CPSS_PM3INT_FUNC_EQ_SUM_E                                    /* = 74*/ ,
    PRV_CPSS_PM3INT_FUNC_IPLR_SUM_E                                  /* = 75*/ ,
    PRV_CPSS_PM3INT_FUNC_IPVX_SUM_E                                  /* = 76*/ ,
    PRV_CPSS_PM3INT_FUNC_LPM0_SUM_E                                  /* = 77*/ ,
    PRV_CPSS_PM3INT_FUNC_LPM1_SUM_E                                  /* = 78*/ ,
    PRV_CPSS_PM3INT_FUNC_LPM2_SUM_E                                  /* = 79*/ ,
    PRV_CPSS_PM3INT_FUNC_LPM3_SUM_E                                  /* = 80*/ ,
    PRV_CPSS_PM3INT_FUNC_ETI_SUM_E                                   /* = 81*/ ,
    PRV_CPSS_PM3INT_FUNC_BMA_SUM_E                                   /* = 82*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(FUNC, 23)                          /* = 83-87*/ ,
    PRV_CPSS_PM3INT_FUNC_CTC_SUM_E                                   /* = 88*/ ,
    PRV_CPSS_PM3INT_FUNC_EFT0_SUM_E                                  /* = 89*/ ,
    PRV_CPSS_PM3INT_FUNC_EFT1_SUM_E                                  /* = 90*/ ,
    PRV_CPSS_PM3INT_FUNC_EFT2_SUM_E                                  /* = 91*/ ,
    PRV_CPSS_PM3INT_FUNC_EFT3_SUM_E                                  /* = 92*/ ,
    PRV_CPSS_PM3INT_FUNC_EMC_FWD_SUM_E                               /* = 93*/ ,
    PRV_CPSS_PM3INT_FUNC_EMC_LU_SUM_E                                /* = 94*/ ,
    PRV_CPSS_PM3INT_FUNC_MLL_SUM_E                                   /* = 95*/ ,

    /*Start PCL -- Policy Engine Interrupt Cause address[0x00200004]*/
    /* Tree place: .3.1*/
    /*INTERRUPT ID BASE:[96]*/
    PRV_CPSS_PM3INT_POLICY_ENGINE_SUM_E                              /* = 96*/ ,
    PRV_CPSS_PM3INT_POLICY_ENGINE_MG_ADDR_OUTOF_RANGE_E              /* = 97*/ ,
    PRV_CPSS_PM3INT_POLICY_ENGINE_PCL0_TCAM_ACCESS_DATA_ERROR_LU0_E  /* = 98*/ ,
    PRV_CPSS_PM3INT_POLICY_ENGINE_PCL0_TCAM_ACCESS_DATA_ERROR_LU1_E  /* = 99*/ ,
    PRV_CPSS_PM3INT_POLICY_ENGINE_RESERVED4_E                        /* = 100*/ ,
    PRV_CPSS_PM3INT_POLICY_ENGINE_RESERVED5_E                        /* = 101*/ ,
    PRV_CPSS_PM3INT_POLICY_ENGINE_PCL1_TCAM_ACCESS_DATA_ERROR_LU0_E  /* = 102*/ ,
    PRV_CPSS_PM3INT_POLICY_ENGINE_PCL1_TCAM_ACCESS_DATA_ERROR_LU1_E  /* = 103*/ ,
    PRV_CPSS_PM3INT_POLICY_ENGINE_RESERVED8_E                        /* = 104*/ ,
    PRV_CPSS_PM3INT_POLICY_ENGINE_RESERVED9_E                        /* = 105*/ ,
    PRV_CPSS_PM3INT_POLICY_ENGINE_FIFO_FULL_LU0_E                    /* = 106*/ ,
    PRV_CPSS_PM3INT_POLICY_ENGINE_FIFO_FULL_LU1_E                    /* = 107*/ ,
    PRV_CPSS_PM3INT_POLICY_ENGINE_IPV6_MC_FIFO_FULL_E                /* = 108*/ ,
    PRV_CPSS_PM3INT_POLICY_ENGINE_INLIF_FIFO_FULL_E                  /* = 109*/ ,
    PRV_CPSS_PM3INT_POLICY_ENGINE_INLIF_TABLE_ECC_ERROR_E            /* = 110*/ ,
    PRV_CPSS_PM3INT_POLICY_ENGINE_PCL_CONF_TABLE_PARITY_ERROR_E      /* = 111*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(POLICY_ENGINE)                       /* = 112-127*/,

    /*Start TTIEngine_IntSum address[0x00100004]*/
    /* Tree place: .3.2*/
    /*INTERRUPT ID BASE:[128]*/
    PRV_CPSS_PM3INT_TTI_GENERAL_SUM_E                                /* = 128*/ ,
    PRV_CPSS_PM3INT_TTI_CPU_ADDRESS_OUT_OF_RANGE_E                   /* = 129*/ ,
    PRV_CPSS_PM3INT_TTI_LOOKUP_TTI_FIFO_FULL_E                       /* = 130*/ ,
    PRV_CPSS_PM3INT_TTI_TTI_TCAM_ACCESS_DATA_ERROR_E                 /* = 131*/ ,
    PRV_CPSS_PM3INT_TTI_PKT_DSC_FIFO_FULL_E                          /* = 132*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TTI)                                 /* = 133-159*/,

    /*Start TCC0_IntSum address[0x01400060]*/
    /* Tree place: .3.3*/
    /*INTERRUPT ID BASE:[160]*/
    PRV_CPSS_PM3INT_TCC0_GENERAL_SUM_E                               /* = 160*/ ,
    PRV_CPSS_PM3INT_TCC0_TCAM_ERROR_DETECTED_E                       /* = 161*/ ,
    PRV_CPSS_PM3INT_TCC0_ACTION_ERROR_DETECTED_E                     /* = 162*/ ,
    PRV_CPSS_PM3INT_TCC0_MG_LOOKUP_RESULTS_READY_E                   /* = 163*/ ,
    PRV_CPSS_PM3INT_TCC0_TBL_TC_BIST2RF_BAD_ADDR_E                   /* = 164*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TCC0)                                /* = 165-191*/,

    /*Start TCC1_IntSum address[0x01500060]*/
    /* Tree place: .3.4*/
    /*INTERRUPT ID BASE:[192]*/
    PRV_CPSS_PM3INT_TCC1_GENERAL_SUM_E                               /* = 192*/ ,
    PRV_CPSS_PM3INT_TCC1_TCAM_ERROR_DETECTED_E                       /* = 193*/ ,
    PRV_CPSS_PM3INT_TCC1_ACTION_ERROR_DETECTED_E                     /* = 194*/ ,
    PRV_CPSS_PM3INT_TCC1_MG_LOOKUP_RESULTS_READY_E                   /* = 195*/ ,
    PRV_CPSS_PM3INT_TCC1_TBL_TC_BIST2RF_BAD_ADDR_E                   /* = 196*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TCC1)                                /* = 197-223*/,

    /*Start EtcIntSum -  External TCAM Interrupt Cause address[0x02000008]*/
    /* Tree place: .3.5*/
    /*INTERRUPT ID BASE:[224]*/
    PRV_CPSS_PM3INT_EXT_TCAM_GENERAL_SUM_E                           /* = 224*/ ,
    PRV_CPSS_PM3INT_EXT_TCAM_REGFILE_ACCESS_ERROR_E                  /* = 225*/ ,
    PRV_CPSS_PM3INT_EXT_TCAM_IO_ERROR_E                              /* = 226*/ ,
    PRV_CPSS_PM3INT_EXT_TCAM_RESERVED3_E                             /* = 227*/ ,
    PRV_CPSS_PM3INT_EXT_TCAM_MG_CMD_EXE_DONE_E                       /* = 228*/ ,
    PRV_CPSS_PM3INT_EXT_TCAM_NEW_DATA_IN_MAILBOX_E                   /* = 229*/ ,
    PRV_CPSS_PM3INT_EXT_TCAM_MG_WRONG_EXP_RES_LEN_E                  /* = 230*/ ,
    PRV_CPSS_PM3INT_EXT_TCAM_RESPONSE_WITHOUT_REQUEST_E              /* = 231*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(EXT_TCAM)                            /* = 232-255*/,

    /*Start LbPortIntSum -  Loopback Port Interrupt Cause address[0x04F00040]*/
    /* Tree place: .3.6*/
    /*INTERRUPT ID BASE:[256]*/
    PRV_CPSS_PM3INT_LOOPBACK_PORT_GENERAL_SUM_E                      /* = 256*/ ,
    PRV_CPSS_PM3INT_LOOPBACK_PORT_SER_ERROR_E                        /* = 257*/ ,
    PRV_CPSS_PM3INT_LOOPBACK_PORT_OVERSIZE_PKT_DROP_E                /* = 258*/ ,
    PRV_CPSS_PM3INT_LOOPBACK_PORT_UNDERSIZE_PKT_DROP_E               /* = 259*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(LOOPBACK_PORT)                       /* = 260-287*/,

    /*Start EgrPlrIntsum - Egress Policer Interrupt Cause address[0x00E00100]*/
    /* Tree place: .3.7*/
    /*INTERRUPT ID BASE:[288]*/
    PRV_CPSS_PM3INT_EGRESS_POLICER_SUMMARY_E                         /* = 288*/ ,
    PRV_CPSS_PM3INT_EGRESS_POLICER_DATA_ERROR_E                      /* = 289*/ ,
    PRV_CPSS_PM3INT_EGRESS_POLICER_ADDRESS_OUT_OF_MEMORY_E           /* = 290*/ ,
    PRV_CPSS_PM3INT_EGRESS_POLICER_IPFIX_COUNTER_WRAPAROUND_E        /* = 291*/ ,
    PRV_CPSS_PM3INT_EGRESS_POLICER_IPFIX_ALARM_E                     /* = 292*/ ,
    PRV_CPSS_PM3INT_EGRESS_POLICER_IPFIX_ALARM_FIFO_WRAPAROUND_E     /* = 293*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(EGRESS_POLICER)                      /* = 294-319*/,

    /*Start L2IIntSum - Bridge Interrupt Cause address[0x00340130]*/
    /* Tree place: .3.8*/
    /*INTERRUPT ID BASE:[320]*/
    PRV_CPSS_PM3INT_BRIDGE_SUM_E                                     /* = 320*/ ,
    PRV_CPSS_PM3INT_BRIDGE_RESERVED1_E                               /* = 321*/ ,
    PRV_CPSS_PM3INT_BRIDGE_ADDRESS_OUT_OF_RANGE_E                    /* = 322*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(BRIDGE, 27)                        /* = 323-347*/ ,
    PRV_CPSS_PM3INT_BRIDGE_UPDATE_SECURITY_BREACH_REGISTER_E         /* = 348*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(BRIDGE)                              /* = 349-351*/,

    /*Start FDBIntSum - FDB Interrupt Cause address[0x00400308]*/
    /* Tree place: .3.9*/
    /*INTERRUPT ID BASE:[352]*/
    PRV_CPSS_PM3INT_FDB_SUM_E                                        /* = 352*/ ,
    PRV_CPSS_PM3INT_FDB_NUM_OF_HOP_EXP_E                             /* = 353*/ ,
    PRV_CPSS_PM3INT_FDB_NA_LEARNT_E                                  /* = 354*/ ,
    PRV_CPSS_PM3INT_FDB_NA_NOT_LEARNT_E                              /* = 355*/ ,
    PRV_CPSS_PM3INT_FDB_AGE_VIA_TRIGGER_ENDED_E                      /* = 356*/ ,
    PRV_CPSS_PM3INT_FDB_DATA_ERROR_E                                 /* = 357*/ ,
    PRV_CPSS_PM3INT_FDB_ADDRESS_OUT_OF_MEMORY_E                      /* = 358*/ ,
    PRV_CPSS_PM3INT_FDB_RESERVED7_E                                  /* = 359*/ ,
    PRV_CPSS_PM3INT_FDB_RESERVED8_E                                  /* = 360*/ ,
    PRV_CPSS_PM3INT_FDB_AU_PROC_COMPLETED_E                          /* = 361*/ ,
    PRV_CPSS_PM3INT_FDB_AU_MSG_TO_CPU_READY_E                        /* = 362*/ ,
    PRV_CPSS_PM3INT_FDB_RESERVED11_E                                 /* = 363*/ ,
    PRV_CPSS_PM3INT_FDB_NA_SELF_LEARNT_E                             /* = 364*/ ,
    PRV_CPSS_PM3INT_FDB_NA_FROM_CPU_LEARNED_E                        /* = 365*/ ,
    PRV_CPSS_PM3INT_FDB_NA_FROM_CPU_DROPPED_E                        /* = 366*/ ,
    PRV_CPSS_PM3INT_FDB_AGED_OUT_E                                   /* = 367*/ ,
    PRV_CPSS_PM3INT_FDB_AU_FIFO_TO_CPU_IS_FULL_E                     /* = 368*/ ,
    PRV_CPSS_PM3INT_FDB_NA_DISCARDED_E                               /* = 369*/ ,
    PRV_CPSS_PM3INT_FDB_QA_DISCARDED_E                               /* = 370*/ ,
    PRV_CPSS_PM3INT_FDB_VLT_DATA_ERROR_E                             /* = 371*/ ,
    PRV_CPSS_PM3INT_FDB_TC_BIST_BAD_ADDR_E                           /* = 372*/ ,
    PRV_CPSS_PM3INT_FDB_VLAN_ECC_ERROR_E                             /* = 373*/ ,
    PRV_CPSS_PM3INT_FDB_VIDX_ECC_ERROR_E                             /* = 374*/ ,
    PRV_CPSS_PM3INT_FDB_SPAN_PARITY_ERROR_E                          /* = 375*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(FDB)                                 /* = 376-383*/,

    /*Start EqIntSum - Eq Global Interrupt Cause address[0x00D00390]*/
    /* Tree place: .3.10*/
    /*INTERRUPT ID BASE:[384]*/
    PRV_CPSS_PM3INT_EQ_GLOBAL_EQ_GLOBAL_SUM_E                        /* = 384*/ ,
    PRV_CPSS_PM3INT_EQ_GLOBAL_STC_SUM0_E                             /* = 385*/ ,
    PRV_CPSS_PM3INT_EQ_GLOBAL_STC_SUM1_E                             /* = 386*/ ,
    PRV_CPSS_PM3INT_EQ_GLOBAL_RATE_LIMIT_SUM0_E                      /* = 387*/ ,
    PRV_CPSS_PM3INT_EQ_GLOBAL_RATE_LIMIT_SUM1_E                      /* = 388*/ ,
    PRV_CPSS_PM3INT_EQ_GLOBAL_RATE_LIMIT_SUM2_E                      /* = 389*/ ,
    PRV_CPSS_PM3INT_EQ_GLOBAL_RATE_LIMIT_SUM3_E                      /* = 390*/ ,
    PRV_CPSS_PM3INT_EQ_GLOBAL_RATE_LIMIT_SUM4_E                      /* = 391*/ ,
    PRV_CPSS_PM3INT_EQ_GLOBAL_RATE_LIMIT_SUM5_E                      /* = 392*/ ,
    PRV_CPSS_PM3INT_EQ_GLOBAL_RATE_LIMIT_SUM6_E                      /* = 393*/ ,
    PRV_CPSS_PM3INT_EQ_GLOBAL_RATE_LIMIT_SUM7_E                      /* = 394*/ ,
    PRV_CPSS_PM3INT_EQ_GLOBAL_RATE_LIMIT_SUM8_E                      /* = 395*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(EQ)                                  /* = 396-415*/,

    /*Start Stc0IntSum - STC Interrupt Cause address[0x00D00060]*/
    /* Tree place: .3.10.1*/
    /*INTERRUPT ID BASE:[416]*/
    PRV_CPSS_PM3INT_INDEXED_BIT_REG_MAC(EQ_STC, PORT, 0),

    /*Start Stc1IntSum - STC Interrupt Cause address[0x00D00064]*/
    /* Tree place: .3.10.2*/
    /*INTERRUPT ID BASE:[448]*/
    PRV_CPSS_PM3INT_INDEXED_BIT_REG_MAC(EQ_STC, PORT, 1),

    /*Start RateLimit0IntSum - Rate Limit Interrupt Cause address[0x00D00080]*/
    /* Tree place: .3.10.3*/
    /*INTERRUPT ID BASE:[480]*/
    PRV_CPSS_PM3INT_INDEXED_BIT_REG_MAC(EQ_RATE_LIMIT, RATE, 0),

    /*Start RateLimit1IntSum - Rate Limit Interrupt Cause address[0x00D00084]*/
    /* Tree place: .3.10.4*/
    /*INTERRUPT ID BASE:[512]*/
    PRV_CPSS_PM3INT_INDEXED_BIT_REG_MAC(EQ_RATE_LIMIT, RATE, 1),

    /*Start RateLimit2IntSum - Rate Limit Interrupt Cause address[0x00D00088]*/
    /* Tree place: .3.10.5*/
    /*INTERRUPT ID BASE:[544]*/
    PRV_CPSS_PM3INT_INDEXED_BIT_REG_MAC(EQ_RATE_LIMIT, RATE, 2),

    /*Start RateLimit3IntSum - Rate Limit Interrupt Cause address[0x00D0008C]*/
    /* Tree place: .3.10.6*/
    /*INTERRUPT ID BASE:[576]*/
    PRV_CPSS_PM3INT_INDEXED_BIT_REG_MAC(EQ_RATE_LIMIT, RATE, 3),

    /*Start RateLimit4IntSum - Rate Limit Interrupt Cause address[0x00D00090]*/
    /* Tree place: .3.10.7*/
    /*INTERRUPT ID BASE:[608]*/
    PRV_CPSS_PM3INT_INDEXED_BIT_REG_MAC(EQ_RATE_LIMIT, RATE, 4),

    /*Start RateLimit5IntSum - Rate Limit Interrupt Cause address[0x00D00094]*/
    /* Tree place: .3.10.8*/
    /*INTERRUPT ID BASE:[640]*/
    PRV_CPSS_PM3INT_INDEXED_BIT_REG_MAC(EQ_RATE_LIMIT, RATE, 5),

    /*Start RateLimit6IntSum - Rate Limit Interrupt Cause address[0x00D00098]*/
    /* Tree place: .3.10.9*/
    /*INTERRUPT ID BASE:[672]*/
    PRV_CPSS_PM3INT_INDEXED_BIT_REG_MAC(EQ_RATE_LIMIT, RATE, 6),

    /*Start RateLimit7IntSum - Rate Limit Interrupt Cause address[0x00D0009C]*/
    /* Tree place: .3.10.10*/
    /*INTERRUPT ID BASE:[704]*/
    PRV_CPSS_PM3INT_INDEXED_BIT_REG_MAC(EQ_RATE_LIMIT, RATE, 7),

    /*Start RateLimit8IntSum - Rate Limit Interrupt Cause address[0x00D000A0]*/
    /* Tree place: .3.10.11*/
    /*INTERRUPT ID BASE:[736]*/
    PRV_CPSS_PM3INT_INDEXED_BIT_REG_MAC(EQ_RATE_LIMIT, RATE, 8),

    /*Start IngrPlrIntsum - Ingress Policer Interrupt Cause address[0x00B00100]*/
    /* Tree place: .3.11*/
    /*INTERRUPT ID BASE:[768]*/
    PRV_CPSS_PM3INT_INGRESS_POLICER_SUMMARY_E                        /* = 768*/ ,
    PRV_CPSS_PM3INT_INGRESS_POLICER_DATA_ERROR_E                     /* = 769*/ ,
    PRV_CPSS_PM3INT_INGRESS_POLICER_ADDRESS_OUT_OF_MEMORY_E          /* = 770*/ ,
    PRV_CPSS_PM3INT_INGRESS_POLICER_IPFIX_COUNTER_WRAPAROUND_E       /* = 771*/ ,
    PRV_CPSS_PM3INT_INGRESS_POLICER_IPFIX_ALARM_E                    /* = 772*/ ,
    PRV_CPSS_PM3INT_INGRESS_POLICER_IPFIX_ALARM_FIFO_WRAPAROUND_E    /* = 773*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(INGRESS_POLICER)                     /* = 774-799*/,

    /*Start IpvxIntSum - IPVX Interrupt Cause address[0x00600E80]*/
    /* Tree place: .3.12*/
    /*INTERRUPT ID BASE:[800]*/
    PRV_CPSS_PM3INT_IPVX_SUM_E                                       /* = 800*/ ,
    PRV_CPSS_PM3INT_IPVX_DATA_ERROR_E                                /* = 801*/ ,
    PRV_CPSS_PM3INT_IPVX_ADDRESS_OUT_OF_MEMORY_E                     /* = 802*/ ,
    PRV_CPSS_PM3INT_IPVX_LPM0_DATA_ERROR_E                           /* = 803*/ ,
    PRV_CPSS_PM3INT_IPVX_LPM1_DATA_ERROR_E                           /* = 804*/ ,
    PRV_CPSS_PM3INT_IPVX_LPM2_DATA_ERROR_E                           /* = 805*/ ,
    PRV_CPSS_PM3INT_IPVX_LPM3_DATA_ERROR_E                           /* = 806*/ ,
    PRV_CPSS_PM3INT_IPVX_LPM4_DATA_ERROR_E                           /* = 807*/ ,
    PRV_CPSS_PM3INT_IPVX_LPM5_DATA_ERROR_E                           /* = 808*/ ,
    PRV_CPSS_PM3INT_IPVX_LPM6_DATA_ERROR_E                           /* = 809*/ ,
    PRV_CPSS_PM3INT_IPVX_LPM7_DATA_ERROR_E                           /* = 810*/ ,
    PRV_CPSS_PM3INT_IPVX_LOG_FLOW_VALID_E                            /* = 811*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(IPVX)                                /* = 812-831*/,

    /*Start Lpm0IntSum - LPM0 Interrupt Cause address[0x0078000C]*/
    /* Tree place: .3.13*/
    /*INTERRUPT ID BASE:[832]*/
    PRV_CPSS_PM3INT_LPM0_SUM_E                                       /* = 832*/ ,
    PRV_CPSS_PM3INT_LPM0_PARITY_ERROR_E                              /* = 833*/ ,
    PRV_CPSS_PM3INT_LPM0_ECC_ERROR_E                                 /* = 834*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(LPM0)                                /* = 835-863*/,

    /*Start Lpm1IntSum - LPM1 Interrupt Cause address[0x0088000C]*/
    /* Tree place: .3.14*/
    /*INTERRUPT ID BASE:[864]*/
    PRV_CPSS_PM3INT_LPM1_SUM_E                                       /* = 864*/ ,
    PRV_CPSS_PM3INT_LPM1_PARITY_ERROR_E                              /* = 865*/ ,
    PRV_CPSS_PM3INT_LPM1_ECC_ERROR_E                                 /* = 866*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(LPM1)                                /* = 867-895*/,

    /*Start Lpm2IntSum - LPM2 Interrupt Cause address[0x0098000C]*/
    /* Tree place: .3.15*/
    /*INTERRUPT ID BASE:[896]*/
    PRV_CPSS_PM3INT_LPM2_SUM_E                                       /* = 896*/ ,
    PRV_CPSS_PM3INT_LPM2_PARITY_ERROR_E                              /* = 897*/ ,
    PRV_CPSS_PM3INT_LPM2_ECC_ERROR_E                                 /* = 898*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(LPM2)                                /* = 899-927*/,

    /*Start Lpm3IntSum - LPM3 Interrupt Cause address[0x00A8000C]*/
    /* Tree place: .3.16*/
    /*INTERRUPT ID BASE:[928]*/
    PRV_CPSS_PM3INT_LPM3_SUM_E                                       /* = 928*/ ,
    PRV_CPSS_PM3INT_LPM3_PARITY_ERROR_E                              /* = 929*/ ,
    PRV_CPSS_PM3INT_LPM3_ECC_ERROR_E                                 /* = 930*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(LPM3)                                /* = 931-959*/,

    /*Start EtiIntSum - ETI Interrupt Cause address[0x0460003C]*/
    /* Tree place: .3.17*/
    /*INTERRUPT ID BASE:[960]*/
    PRV_CPSS_PM3INT_ETI_SUM_E                                        /* = 960*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(ETI,        2)                     /* = 961-962*/ ,
    PRV_CPSS_PM3INT_ETI_LATENCY_FIFO_FULL_E                          /* = 963*/ ,
    PRV_CPSS_PM3INT_ETI_CMD_LATENCY_FIFO_NOT_READY_E                 /* = 964*/ ,
    PRV_CPSS_PM3INT_ETI_RESERVED5_E                                  /* = 965*/ ,
    PRV_CPSS_PM3INT_ETI_ILKN_LINK_STATUS_CHANGED_E                   /* = 966*/ ,
    PRV_CPSS_PM3INT_ETI_RX_CMD_ERROR_TCAM_E                          /* = 967*/ ,
    PRV_CPSS_PM3INT_ETI_RX_CMD_ERROR_INDEX_E                         /* = 968*/ ,
    PRV_CPSS_PM3INT_ETI_RX_ILKN_IP_ERROR_E                           /* = 969*/ ,
    PRV_CPSS_PM3INT_ETI_TX_SPACE_IN_SYNC_E                           /* = 970*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(ETI)                                 /* = 971-991*/,

    /*Start CncIntSum - CNC Interrupt Cause address[0x02E00100]*/
    /* Tree place: .3.24*/
    /*INTERRUPT ID BASE:[992]*/
    PRV_CPSS_PM3INT_CNC_GENERAL_E                                    /* = 992*/ ,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND0_SUM_E                            /* = 993*/ ,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND1_SUM_E                            /* = 994*/ ,
    PRV_CPSS_PM3INT_CNC_WRAPAROUND2_SUM_E                            /* = 995*/ ,
    PRV_CPSS_PM3INT_CNC_RATE_LIMIT0_SUM_E                            /* = 996*/ ,
    PRV_CPSS_PM3INT_CNC_RATE_LIMIT1_SUM_E                            /* = 997*/ ,
    PRV_CPSS_PM3INT_CNC_RATE_LIMIT2_SUM_E                            /* = 998*/ ,
    PRV_CPSS_PM3INT_CNC_MISC_SUM_E                                   /* = 999*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(CNC)                                 /* = 1000-1023*/,

    /*Start CncWa0IntSum - CNC WRAPAROUND0 Interrupt Cause address[0x002E00190]*/
    /* Tree place: .3.24.1*/
    /*INTERRUPT ID BASE:[1024]*/
    PRV_CPSS_PM3INT_INDEXED_BIT_REG_MAC(CNC_WRAPAROUND, BLOCK, 0),

    /*Start CncWa1IntSum - CNC WRAPAROUND1 Interrupt Cause address[0x002E00194]*/
    /* Tree place: .3.24.2*/
    /*INTERRUPT ID BASE:[1056]*/
    PRV_CPSS_PM3INT_INDEXED_BIT_REG_MAC(CNC_WRAPAROUND, BLOCK, 1),

    /*Start CncWa2IntSum - CNC WRAPAROUND2 Interrupt Cause address[0x002E00198]*/
    /* Tree place: .3.24.3*/
    /*INTERRUPT ID BASE:[1088]*/
    PRV_CPSS_PM3INT_INDEXED_BIT_REG_MAC(CNC_WRAPAROUND, BLOCK, 2),

    /*Start CncRl0IntSum - CNC RateLimit0 Interrupt Cause address[0x002E001B8]*/
    /* Tree place: .3.24.4*/
    /*INTERRUPT ID BASE:[1120]*/
    PRV_CPSS_PM3INT_INDEXED_BIT_REG_MAC(CNC_RATE_LIMIT, BLOCK, 0),

    /*Start CncRl1IntSum - CNC RateLimit1 Interrupt Cause address[0x002E001BC]*/
    /* Tree place: .3.24.5*/
    /*INTERRUPT ID BASE:[1152]*/
    PRV_CPSS_PM3INT_INDEXED_BIT_REG_MAC(CNC_RATE_LIMIT, BLOCK, 1),

    /*Start CncRl2IntSum - CNC RateLimit2 Interrupt Cause address[0x002E001C0]*/
    /* Tree place: .3.24.6*/
    /*INTERRUPT ID BASE:[1184]*/
    PRV_CPSS_PM3INT_INDEXED_BIT_REG_MAC(CNC_RATE_LIMIT, BLOCK, 2),

    /*Start CncMiscIntSum - CNC MISC Interrupt Cause address[0x02E001E0]*/
    /* Tree place: .3.24.7*/
    /*INTERRUPT ID BASE:[1216]*/
    PRV_CPSS_PM3INT_CNC_MISC_GENERAL_E                               /* = 1216*/ ,
    PRV_CPSS_PM3INT_CNC_MISC_DUMP_FINISHED_E                         /* = 1217*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(CNC_MISC)                            /* = 1218-1247*/,

    /*Start EgrFltIntSum - Egress Filtering0 Interrupt Cause address[0x035000A0]*/
    /* Tree place: .3.25*/
    /*INTERRUPT ID BASE:[1248]*/
    PRV_CPSS_PM3INT_EGR_FLT0_GENERAL_E                               /* = 1248*/ ,
    PRV_CPSS_PM3INT_EGR_FLT0_MC_FIFO0_PARITY_ERR_E                   /* = 1249*/ ,
    PRV_CPSS_PM3INT_EGR_FLT0_MC_FIFO1_PARITY_ERR_E                   /* = 1250*/ ,
    PRV_CPSS_PM3INT_EGR_FLT0_MC_FIFO2_PARITY_ERR_E                   /* = 1251*/ ,
    PRV_CPSS_PM3INT_EGR_FLT0_MC_FIFO3_PARITY_ERR_E                   /* = 1252*/ ,
    PRV_CPSS_PM3INT_EGR_FLT0_EGR_WRONG_ADDR_E                        /* = 1253*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(EGR_FLT0, 7)                       /* = 1254-1255*/ ,
    PRV_CPSS_PM3INT_EGR_FLT0_MC_FIFO0_FULL_E                         /* = 1256*/ ,
    PRV_CPSS_PM3INT_EGR_FLT0_MC_FIFO1_FULL_E                         /* = 1257*/ ,
    PRV_CPSS_PM3INT_EGR_FLT0_MC_FIFO2_FULL_E                         /* = 1258*/ ,
    PRV_CPSS_PM3INT_EGR_FLT0_MC_FIFO3_FULL_E                         /* = 1259*/ ,
    PRV_CPSS_PM3INT_EGR_FLT0_INC_FIFO_FULL_E                         /* = 1260*/ ,
    PRV_CPSS_PM3INT_EGR_FLT0_EXT_INC_FIFO_FULL_E                     /* = 1261*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(EGR_FLT0)                            /* = 1262-1279*/,

    /*Start EgrFltIntSum - Egress Filtering1 Interrupt Cause address[0x036000A0]*/
    /* Tree place: .3.26*/
    /*INTERRUPT ID BASE:[1280]*/
    PRV_CPSS_PM3INT_EGR_FLT1_GENERAL_E                               /* = 1280*/ ,
    PRV_CPSS_PM3INT_EGR_FLT1_MC_FIFO0_PARITY_ERR_E                   /* = 1281*/ ,
    PRV_CPSS_PM3INT_EGR_FLT1_MC_FIFO1_PARITY_ERR_E                   /* = 1282*/ ,
    PRV_CPSS_PM3INT_EGR_FLT1_MC_FIFO2_PARITY_ERR_E                   /* = 1283*/ ,
    PRV_CPSS_PM3INT_EGR_FLT1_MC_FIFO3_PARITY_ERR_E                   /* = 1284*/ ,
    PRV_CPSS_PM3INT_EGR_FLT1_EGR_WRONG_ADDR_E                        /* = 1285*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(EGR_FLT1, 7)                       /* = 1286-1287*/ ,
    PRV_CPSS_PM3INT_EGR_FLT1_MC_FIFO0_FULL_E                         /* = 1288*/ ,
    PRV_CPSS_PM3INT_EGR_FLT1_MC_FIFO1_FULL_E                         /* = 1289*/ ,
    PRV_CPSS_PM3INT_EGR_FLT1_MC_FIFO2_FULL_E                         /* = 1290*/ ,
    PRV_CPSS_PM3INT_EGR_FLT1_MC_FIFO3_FULL_E                         /* = 1291*/ ,
    PRV_CPSS_PM3INT_EGR_FLT1_INC_FIFO_FULL_E                         /* = 1292*/ ,
    PRV_CPSS_PM3INT_EGR_FLT1_EXT_INC_FIFO_FULL_E                     /* = 1293*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(EGR_FLT1)                            /* = 1294-1311*/,

    /*Start EgrFltIntSum - Egress Filtering2 Interrupt Cause address[0x037000A0]*/
    /* Tree place: .3.27*/
    /*INTERRUPT ID BASE:[1312]*/
    PRV_CPSS_PM3INT_EGR_FLT2_GENERAL_E                               /* = 1312*/ ,
    PRV_CPSS_PM3INT_EGR_FLT2_MC_FIFO0_PARITY_ERR_E                   /* = 1313*/ ,
    PRV_CPSS_PM3INT_EGR_FLT2_MC_FIFO1_PARITY_ERR_E                   /* = 1314*/ ,
    PRV_CPSS_PM3INT_EGR_FLT2_MC_FIFO2_PARITY_ERR_E                   /* = 1315*/ ,
    PRV_CPSS_PM3INT_EGR_FLT2_MC_FIFO3_PARITY_ERR_E                   /* = 1316*/ ,
    PRV_CPSS_PM3INT_EGR_FLT2_EGR_WRONG_ADDR_E                        /* = 1317*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(EGR_FLT2, 7)                       /* = 1318-1319*/ ,
    PRV_CPSS_PM3INT_EGR_FLT2_MC_FIFO0_FULL_E                         /* = 1320*/ ,
    PRV_CPSS_PM3INT_EGR_FLT2_MC_FIFO1_FULL_E                         /* = 1321*/ ,
    PRV_CPSS_PM3INT_EGR_FLT2_MC_FIFO2_FULL_E                         /* = 1322*/ ,
    PRV_CPSS_PM3INT_EGR_FLT2_MC_FIFO3_FULL_E                         /* = 1323*/ ,
    PRV_CPSS_PM3INT_EGR_FLT2_INC_FIFO_FULL_E                         /* = 1324*/ ,
    PRV_CPSS_PM3INT_EGR_FLT2_EXT_INC_FIFO_FULL_E                     /* = 1325*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(EGR_FLT2)                            /* = 1326-1343*/,

    /*Start EgrFltIntSum - Egress Filtering3 Interrupt Cause address[0x038000A0]*/
    /* Tree place: .3.28*/
    /*INTERRUPT ID BASE:[1344]*/
    PRV_CPSS_PM3INT_EGR_FLT3_GENERAL_E                               /* = 1344*/ ,
    PRV_CPSS_PM3INT_EGR_FLT3_MC_FIFO0_PARITY_ERR_E                   /* = 1345*/ ,
    PRV_CPSS_PM3INT_EGR_FLT3_MC_FIFO1_PARITY_ERR_E                   /* = 1346*/ ,
    PRV_CPSS_PM3INT_EGR_FLT3_MC_FIFO2_PARITY_ERR_E                   /* = 1347*/ ,
    PRV_CPSS_PM3INT_EGR_FLT3_MC_FIFO3_PARITY_ERR_E                   /* = 1348*/ ,
    PRV_CPSS_PM3INT_EGR_FLT3_EGR_WRONG_ADDR_E                        /* = 1349*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(EGR_FLT3, 7)                       /* = 1350-1351*/ ,
    PRV_CPSS_PM3INT_EGR_FLT3_MC_FIFO0_FULL_E                         /* = 1352*/ ,
    PRV_CPSS_PM3INT_EGR_FLT3_MC_FIFO1_FULL_E                         /* = 1353*/ ,
    PRV_CPSS_PM3INT_EGR_FLT3_MC_FIFO2_FULL_E                         /* = 1354*/ ,
    PRV_CPSS_PM3INT_EGR_FLT3_MC_FIFO3_FULL_E                         /* = 1355*/ ,
    PRV_CPSS_PM3INT_EGR_FLT3_INC_FIFO_FULL_E                         /* = 1356*/ ,
    PRV_CPSS_PM3INT_EGR_FLT3_EXT_INC_FIFO_FULL_E                     /* = 1357*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(EGR_FLT3)                            /* = 1358-1375*/,

    /*Start EmcIntSum - EMC Forward Interrupt Cause address[0x03200200]*/
    /* Tree place: .3.29*/
    /*INTERRUPT ID BASE:[1376]*/
    PRV_CPSS_PM3INT_EMC_FW_GENERAL_E                                 /* = 1376*/ ,
    PRV_CPSS_PM3INT_EMC_FW_CAUSE0_SUM_E                              /* = 1377*/ ,
    PRV_CPSS_PM3INT_EMC_FW_CAUSE1_SUM_E                              /* = 1378*/ ,
    PRV_CPSS_PM3INT_EMC_FW_UNIT2_ERR_DOMAIN_E                        /* = 1379*/ ,
    PRV_CPSS_PM3INT_EMC_FW_UNIT12_ERR_DOMAIN_E                       /* = 1380*/ ,
    PRV_CPSS_PM3INT_EMC_FW_UNIT22_ERR_DOMAIN_E                       /* = 1381*/ ,
    PRV_CPSS_PM3INT_EMC_FW_UNIT32_ERR_DOMAIN_E                       /* = 1382*/ ,
    PRV_CPSS_PM3INT_EMC_FW_UNIT42_ERR_DOMAIN_E                       /* = 1383*/ ,
    PRV_CPSS_PM3INT_EMC_FW_UNIT52_ERR_DOMAIN_E                       /* = 1384*/ ,
    PRV_CPSS_PM3INT_EMC_FW_UNIT62_ERR_DOMAIN_E                       /* = 1385*/ ,
    PRV_CPSS_PM3INT_EMC_FW_UNIT72_ERR_DOMAIN_E                       /* = 1386*/ ,
    PRV_CPSS_PM3INT_EMC_FW_UNIT82_ERR_DOMAIN_E                       /* = 1387*/ ,
    PRV_CPSS_PM3INT_EMC_FW_UNIT92_ERR_DOMAIN_E                       /* = 1388*/ ,
    PRV_CPSS_PM3INT_EMC_FW_UNIT102_ERR_DOMAIN_E                      /* = 1389*/ ,
    PRV_CPSS_PM3INT_EMC_FW_UNIT112_ERR_DOMAIN_E                      /* = 1390*/ ,
    PRV_CPSS_PM3INT_EMC_FW_UNIT122_ERR_DOMAIN_E                      /* = 1391*/ ,
    PRV_CPSS_PM3INT_EMC_FW_UNIT132_ERR_DOMAIN_E                      /* = 1392*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(EMC_FW)                              /* = 1393-1407*/,

    /*Start EmcC0IntSum - EMC Forward Cause0 Interrupt Cause address[0x03200208]*/
    /* Tree place: .3.29.1*/
    /*INTERRUPT ID BASE:[1408]*/
    PRV_CPSS_PM3INT_EMC_FW_CAUSE0_GENERAL_E                          /* = 1408*/ ,
    PRV_CPSS_PM3INT_EMC_FW_CAUSE0_ACCESS_ERR_E                       /* = 1409*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(EMC_FW_CAUSE0, 20)                 /* = 1410-1428*/ ,
    PRV_CPSS_PM3INT_EMC_FW_CAUSE0_ECC_E                              /* = 1429*/ ,
    PRV_CPSS_PM3INT_EMC_FW_CAUSE0_DRAM_FIFO_UNDERRUN0_E              /* = 1430*/ ,
    PRV_CPSS_PM3INT_EMC_FW_CAUSE0_DRAM_FIFO_UNDERRUN1_E              /* = 1431*/ ,
    PRV_CPSS_PM3INT_EMC_FW_CAUSE0_DRAM_FIFO_UNDERRUN2_E              /* = 1432*/ ,
    PRV_CPSS_PM3INT_EMC_FW_CAUSE0_DRAM_FIFO_UNDERRUN3_E              /* = 1433*/ ,
    PRV_CPSS_PM3INT_EMC_FW_CAUSE0_DRAM_FIFO_UNDERRUN4_E              /* = 1434*/ ,
    PRV_CPSS_PM3INT_EMC_FW_CAUSE0_DRAM_FIFO_UNDERRUN5_E              /* = 1435*/ ,
    PRV_CPSS_PM3INT_EMC_FW_CAUSE0_DRAM_FIFO_UNDERRUN6_E              /* = 1436*/ ,
    PRV_CPSS_PM3INT_EMC_FW_CAUSE0_DRAM_FIFO_UNDERRUN7_E              /* = 1437*/ ,
    PRV_CPSS_PM3INT_EMC_FW_CAUSE0_DRAM_FIFO_UNDERRUN8_E              /* = 1438*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(EMC_FW_CAUSE0)                       /* = 1439*/,

    /*Start EmcC0IntSum - EMC Forward Cause1 Interrupt Cause address[0x03200210]*/
    /* Tree place: .3.29.2*/
    /*INTERRUPT ID BASE:[1440]*/
    PRV_CPSS_PM3INT_EMC_FW_CAUSE1_GENERAL_E                          /* = 1440*/ ,
    PRV_CPSS_PM3INT_EMC_FW_CAUSE1_DRAM_FIFO_OVERRUN_E                /* = 1441*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(EMC_FW_CAUSE1)                       /* = 1442-1471*/,

    /*Start EmcIntSum - EMC Lookup Interrupt Cause address[0x03000200]*/
    /* Tree place: .3.30*/
    /*INTERRUPT ID BASE:[1472]*/
    PRV_CPSS_PM3INT_EMC_LU_GENERAL_E                                 /* = 1472*/ ,
    PRV_CPSS_PM3INT_EMC_LU_CAUSE0_SUM_E                              /* = 1473*/ ,
    PRV_CPSS_PM3INT_EMC_LU_CAUSE1_SUM_E                              /* = 1474*/ ,
    PRV_CPSS_PM3INT_EMC_LU_UNIT2_ERR_DOMAIN_E                        /* = 1475*/ ,
    PRV_CPSS_PM3INT_EMC_LU_UNIT12_ERR_DOMAIN_E                       /* = 1476*/ ,
    PRV_CPSS_PM3INT_EMC_LU_UNIT22_ERR_DOMAIN_E                       /* = 1477*/ ,
    PRV_CPSS_PM3INT_EMC_LU_UNIT32_ERR_DOMAIN_E                       /* = 1478*/ ,
    PRV_CPSS_PM3INT_EMC_LU_UNIT42_ERR_DOMAIN_E                       /* = 1479*/ ,
    PRV_CPSS_PM3INT_EMC_LU_UNIT52_ERR_DOMAIN_E                       /* = 1480*/ ,
    PRV_CPSS_PM3INT_EMC_LU_UNIT62_ERR_DOMAIN_E                       /* = 1481*/ ,
    PRV_CPSS_PM3INT_EMC_LU_UNIT72_ERR_DOMAIN_E                       /* = 1482*/ ,
    PRV_CPSS_PM3INT_EMC_LU_UNIT82_ERR_DOMAIN_E                       /* = 1483*/ ,
    PRV_CPSS_PM3INT_EMC_LU_UNIT92_ERR_DOMAIN_E                       /* = 1484*/ ,
    PRV_CPSS_PM3INT_EMC_LU_UNIT102_ERR_DOMAIN_E                      /* = 1485*/ ,
    PRV_CPSS_PM3INT_EMC_LU_UNIT112_ERR_DOMAIN_E                      /* = 1486*/ ,
    PRV_CPSS_PM3INT_EMC_LU_UNIT122_ERR_DOMAIN_E                      /* = 1487*/ ,
    PRV_CPSS_PM3INT_EMC_LU_UNIT132_ERR_DOMAIN_E                      /* = 1488*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(EMC_LU)                              /* = 1489-1503*/,

    /*Start EmcC0IntSum - EMC Lookup Cause0 Interrupt Cause address[0x03000208]*/
    /* Tree place: .3.30.1*/
    /*INTERRUPT ID BASE:[1504]*/
    PRV_CPSS_PM3INT_EMC_LU_CAUSE0_GENERAL_E                          /* = 1504*/ ,
    PRV_CPSS_PM3INT_EMC_LU_CAUSE0_ACCESS_ERR_E                       /* = 1505*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(EMC_LU_CAUSE0, 20)                 /* = 1506-1524*/ ,
    PRV_CPSS_PM3INT_EMC_LU_CAUSE0_ECC_E                              /* = 1525*/ ,
    PRV_CPSS_PM3INT_EMC_LU_CAUSE0_DRAM_FIFO_UNDERRUN0_E              /* = 1526*/ ,
    PRV_CPSS_PM3INT_EMC_LU_CAUSE0_DRAM_FIFO_UNDERRUN1_E              /* = 1527*/ ,
    PRV_CPSS_PM3INT_EMC_LU_CAUSE0_DRAM_FIFO_UNDERRUN2_E              /* = 1528*/ ,
    PRV_CPSS_PM3INT_EMC_LU_CAUSE0_DRAM_FIFO_UNDERRUN3_E              /* = 1529*/ ,
    PRV_CPSS_PM3INT_EMC_LU_CAUSE0_DRAM_FIFO_UNDERRUN4_E              /* = 1530*/ ,
    PRV_CPSS_PM3INT_EMC_LU_CAUSE0_DRAM_FIFO_UNDERRUN5_E              /* = 1531*/ ,
    PRV_CPSS_PM3INT_EMC_LU_CAUSE0_DRAM_FIFO_UNDERRUN6_E              /* = 1532*/ ,
    PRV_CPSS_PM3INT_EMC_LU_CAUSE0_DRAM_FIFO_UNDERRUN7_E              /* = 1533*/ ,
    PRV_CPSS_PM3INT_EMC_LU_CAUSE0_DRAM_FIFO_UNDERRUN8_E              /* = 1534*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(EMC_LU_CAUSE0)                       /* = 1535*/,

    /*Start EmcC0IntSum - EMC Lookup Cause1 Interrupt Cause address[0x03000210]*/
    /* Tree place: .3.30.2*/
    /*INTERRUPT ID BASE:[1536]*/
    PRV_CPSS_PM3INT_EMC_LU_CAUSE1_GENERAL_E                          /* = 1536*/ ,
    PRV_CPSS_PM3INT_EMC_LU_CAUSE1_DRAM_FIFO_OVERRUN_E                /* = 1537*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(EMC_LU_CAUSE1)                       /* = 1538-1567*/,

    /*Start DitIntSum - DIT Interrupt Cause address[0x01D00290]*/
    /* Tree place: .3.31*/
    /*INTERRUPT ID BASE:[1568]*/
    PRV_CPSS_PM3INT_DIT_GENERAL_E                                    /* = 1568*/ ,
    PRV_CPSS_PM3INT_DIT_MC_TTL_THRESHOLD_E                           /* = 1569*/ ,
    PRV_CPSS_PM3INT_DIT_UC_TTL_THRESHOLD_E                           /* = 1570*/ ,
    PRV_CPSS_PM3INT_DIT_INPUT_FIFO_FULL_E                            /* = 1571*/ ,
    PRV_CPSS_PM3INT_DIT_PFC_LATENCY_FIFO_FULL_E                      /* = 1572*/ ,
    PRV_CPSS_PM3INT_DIT_PFC_ANSWER_FIFO_FULL_E                       /* = 1573*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(DIT)                                 /* = 1574-1599*/,

    /*Start DataPathIntSum - Data Path Interrupt Cause address[0x000000A4]*/
    /* Tree place: .4*/
    /*INTERRUPT ID BASE:[1600]*/
    PRV_CPSS_PM3INT_DATA_PATH_GENERAL_E                              /* = 1600*/ ,
    PRV_CPSS_PM3INT_DATA_PATH_BM0_SUM_E                              /* = 1601*/ ,
    PRV_CPSS_PM3INT_DATA_PATH_BM1_SUM_E                              /* = 1602*/ ,
    PRV_CPSS_PM3INT_DATA_PATH_FB_BM0_SUM_E                           /* = 1603*/ ,
    PRV_CPSS_PM3INT_DATA_PATH_FB_BM1_SUM_E                           /* = 1604*/ ,
    PRV_CPSS_PM3INT_DATA_PATH_BMEM0_MA_SUM_E                         /* = 1605*/ ,
    PRV_CPSS_PM3INT_DATA_PATH_BMEM1_MA_SUM_E                         /* = 1606*/ ,
    PRV_CPSS_PM3INT_DATA_PATH_BMEM0_LMS_SUM_E                        /* = 1607*/ ,
    PRV_CPSS_PM3INT_DATA_PATH_BMEM1_LMS_SUM_E                        /* = 1608*/ ,
    PRV_CPSS_PM3INT_DATA_PATH_BMEM0_TX_DMA_SUM_E                     /* = 1609*/ ,
    PRV_CPSS_PM3INT_DATA_PATH_BMEM1_TX_DMA_SUM_E                     /* = 1610*/ ,
    PRV_CPSS_PM3INT_DATA_PATH_RX_DMA_SUM_E                           /* = 1611*/ ,
    PRV_CPSS_PM3INT_DATA_PATH_FB_RX_DMA_SUM_E                        /* = 1612*/ ,
    PRV_CPSS_PM3INT_DATA_PATH_MPPM0_BANK0_SUM_E                      /* = 1613*/ ,
    PRV_CPSS_PM3INT_DATA_PATH_MPPM0_BANK1_SUM_E                      /* = 1614*/ ,
    PRV_CPSS_PM3INT_DATA_PATH_MPPM1_BANK0_SUM_E                      /* = 1615*/ ,
    PRV_CPSS_PM3INT_DATA_PATH_MPPM1_BANK1_SUM_E                      /* = 1616*/ ,
    PRV_CPSS_PM3INT_DATA_PATH_MPPM2_BANK0_SUM_E                      /* = 1617*/ ,
    PRV_CPSS_PM3INT_DATA_PATH_MPPM2_BANK1_SUM_E                      /* = 1618*/ ,
    PRV_CPSS_PM3INT_DATA_PATH_MPPM3_BANK0_SUM_E                      /* = 1619*/ ,
    PRV_CPSS_PM3INT_DATA_PATH_MPPM3_BANK1_SUM_E                      /* = 1620*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(DATA_PATH)                           /* = 1621-1631*/,

    /*Start BufMan0Cause0IntSum - Buffer Management0Cause0 */
    /* Interrupt Cause address[0x01600040]          */
    /* Tree place: .4.1*/
    /*INTERRUPT ID BASE:[1632]*/
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_GENERAL_E                        /* = 1632*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_RESERVED1_E                      /* = 1633*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_AGED_PKT_E                       /* = 1634*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_GLOBAL_LIMIT_E                   /* = 1635*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_PORT_CNT_UNDERRUN_E              /* = 1636*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_PORT_CNT_OVERRUN_E               /* = 1637*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_GE_GROUP_CNT_UNDERRUN_E          /* = 1638*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_GE_GROUP_CNT_OVERRUN_E           /* = 1639*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(BUF_MAN0_CAUSE0, 9)                /* = 1640-1641*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_GLOBAL_CNT_UNDERRUN_E            /* = 1642*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_GLOBAL_CNT_OVERRUN_E             /* = 1643*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_TRIGGER_AGING_DONE_E             /* = 1644*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(BUF_MAN0_CAUSE0, 16)               /* = 1645-1648*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_WRONG_SRC_PORT_E                 /* = 1649*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(BUF_MAN0_CAUSE0, 19)               /* = 1650-1651*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_PORT_RX_FULL_E                   /* = 1652*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_CTRL_ACCESS_ERR_E                /* = 1653*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_LL_P1_ACCESS_ERR_E               /* = 1654*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE0_LL_P2_ACCESS_ERR_E               /* = 1655*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(BUF_MAN0_CAUSE0)                     /* = 1656-1663*/,

    /*Start BufMan0Cause1IntSum - Buffer Management0Cause1 */
    /* Interrupt Cause address[0x01600048]          */
    /* Tree place: .4.2*/
    /*INTERRUPT ID BASE:[1664]*/
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_GENERAL_E                        /* = 1664*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_PORT0_BUF_LIMIT_E                /* = 1665*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_PORT1_BUF_LIMIT_E                /* = 1666*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_PORT2_BUF_LIMIT_E                /* = 1667*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_PORT3_BUF_LIMIT_E                /* = 1668*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_PORT4_BUF_LIMIT_E                /* = 1669*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_PORT5_BUF_LIMIT_E                /* = 1670*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_PORT6_BUF_LIMIT_E                /* = 1671*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_PORT7_BUF_LIMIT_E                /* = 1672*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_PORT8_BUF_LIMIT_E                /* = 1673*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_PORT9_BUF_LIMIT_E                /* = 1674*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_PORT10_BUF_LIMIT_E               /* = 1675*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_PORT11_BUF_LIMIT_E               /* = 1676*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(BUF_MAN0_CAUSE1, 28)               /* = 1677-1692*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_PORTGROUP_BUF_LIMIT_E            /* = 1693*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_RESERVED30_E                     /* = 1694*/ ,
    PRV_CPSS_PM3INT_BUF_MAN0_CAUSE1_CPU_PORT_BUF_LIMIT_E             /* = 1695*/ ,

    /*Start BufMan1Cause0IntSum - Buffer Management1 Cause0 */
    /* Interrupt Cause address[0x01900040]          */
    /* Tree place: .4.3 */
    /*INTERRUPT ID BASE:[1696]*/
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_GENERAL_E                        /* = 1696*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_RESERVED1_E                      /* = 1697*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_AGED_PKT_E                       /* = 1698*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_GLOBAL_LIMIT_E                   /* = 1699*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_PORT_CNT_UNDERRUN_E              /* = 1700*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_PORT_CNT_OVERRUN_E               /* = 1701*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_GE_GROUP_CNT_UNDERRUN_E          /* = 1702*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_GE_GROUP_CNT_OVERRUN_E           /* = 1703*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(BUF_MAN1_CAUSE0, 9)                /* = 1704-1705*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_GLOBAL_CNT_UNDERRUN_E            /* = 1706*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_GLOBAL_CNT_OVERRUN_E             /* = 1707*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_TRIGGER_AGING_DONE_E             /* = 1708*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(BUF_MAN1_CAUSE0, 16)               /* = 1709-1712*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_WRONG_SRC_PORT_E                 /* = 1713*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(BUF_MAN1_CAUSE0, 19)               /* = 1714-1715*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_PORT_RX_FULL_E                   /* = 1716*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_CTRL_ACCESS_ERR_E                /* = 1717*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_LL_P1_ACCESS_ERR_E               /* = 1718*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE0_LL_P2_ACCESS_ERR_E               /* = 1719*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(BUF_MAN1_CAUSE0)                     /* = 1720-1727*/,

    /*Start BufMan1Cause1IntSum - Buffer Management1Cause1 */
    /* Interrupt Cause address[0x01900048]          */
    /* Tree place: .4.4*/
    /*INTERRUPT ID BASE:[1728]*/
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE1_GENERAL_E                        /* = 1728*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE1_PORT0_BUF_LIMIT_E                /* = 1729*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE1_PORT1_BUF_LIMIT_E                /* = 1730*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE1_PORT2_BUF_LIMIT_E                /* = 1731*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE1_PORT3_BUF_LIMIT_E                /* = 1732*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE1_PORT4_BUF_LIMIT_E                /* = 1733*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE1_PORT5_BUF_LIMIT_E                /* = 1734*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE1_PORT6_BUF_LIMIT_E                /* = 1735*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE1_PORT7_BUF_LIMIT_E                /* = 1736*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE1_PORT8_BUF_LIMIT_E                /* = 1737*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE1_PORT9_BUF_LIMIT_E                /* = 1738*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE1_PORT10_BUF_LIMIT_E               /* = 1739*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE1_PORT11_BUF_LIMIT_E               /* = 1740*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(BUF_MAN1_CAUSE1, 28)               /* = 1741-1756*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE1_PORTGROUP_BUF_LIMIT_E            /* = 1757*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE1_RESERVED30_E                     /* = 1758*/ ,
    PRV_CPSS_PM3INT_BUF_MAN1_CAUSE1_CPU_PORT_BUF_LIMIT_E             /* = 1759*/ ,

    /*Start MrgAlignerIntSum - Merger Aligner Interrupt Cause address */
    /* MA0(network)[0x02500004] and MA1(fabric)[0x02B00004]           */
    /* Tree place: .4.5 and 4.6 */
    /*INTERRUPT ID BASE:[1760]*/
    PRV_CPSS_PM3INT_MA0_SUM_GENERAL_E                                /* = 1760*/ ,
    PRV_CPSS_PM3INT_MA0_SUM_MERGER_E                                 /* = 1761*/ ,
    PRV_CPSS_PM3INT_MA0_SUM_MERGER_PORTS_E                           /* = 1762*/ ,
    PRV_CPSS_PM3INT_MA0_SUM_ALIGNER_E                                /* = 1763*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(MA0_SUM)                             /* = 1764-1791*/,

    /*INTERRUPT ID BASE:[1792]*/
    PRV_CPSS_PM3INT_MA1_SUM_GENERAL_E                                /* = 1792*/ ,
    PRV_CPSS_PM3INT_MA1_SUM_MERGER_E                                 /* = 1793*/ ,
    PRV_CPSS_PM3INT_MA1_SUM_MERGER_PORTS_E                           /* = 1794*/ ,
    PRV_CPSS_PM3INT_MA1_SUM_ALIGNER_E                                /* = 1795*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(MA1_SUM)                             /* = 1796-1823*/,

    /*Start MAMergerIntSum - MA Merger Interrupt Cause address */
    /* MA0(network)[0x02580100] and MA1(fabric)[0x02B80100]    */
    /* Tree place: .4.5.1 and 4.6.1*/
    /*INTERRUPT ID BASE:[1824]*/
    PRV_CPSS_PM3INT_MA0_MERGER_GLOBAL_GENERAL_E                      /* = 1824*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_GLOBAL_EPCL_LATENCY_FIFO_FULL_E       /* = 1825*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_GLOBAL_DESCRIPTOR_FIFO_FULL_E         /* = 1826*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_GLOBAL_TARG_DEV_ANSWER_FIFO_OVERRUN_E /* = 1827*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_GLOBAL_CPU_CODE_ANSWER_FIFO_OVERRUN_E /* = 1828*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(MA0_MERGER_GLOBAL)                   /* = 1829-1855*/,

    /*INTERRUPT ID BASE:[1856]*/
    PRV_CPSS_PM3INT_MA1_MERGER_GLOBAL_GENERAL_E                      /* = 1856*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_GLOBAL_EPCL_LATENCY_FIFO_FULL_E       /* = 1857*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_GLOBAL_DESCRIPTOR_FIFO_FULL_E         /* = 1858*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_GLOBAL_TARG_DEV_ANSWER_FIFO_OVERRUN_E /* = 1859*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_GLOBAL_CPU_CODE_ANSWER_FIFO_OVERRUN_E /* = 1860*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(MA1_MERGER_GLOBAL)                   /* = 1861-1887*/,

    /*Start MAMergerPortsIntSum - MA Merger Ports Interrupt Cause address */
    /* MA0(network)[0x02580108] and MA1(fabric)[0x02B80108]               */
    /* Tree place: .4.5.2 and 4.6.2*/
    /*INTERRUPT ID BASE:[1888]*/
    PRV_CPSS_PM3INT_MA0_MERGER_PORTS_GENERAL_E                       /* = 1888*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_PORTS_PORT0_E                         /* = 1889*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_PORTS_PORT1_E                         /* = 1890*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_PORTS_PORT2_E                         /* = 1891*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_PORTS_PORT3_E                         /* = 1892*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_PORTS_PORT4_E                         /* = 1893*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_PORTS_PORT5_E                         /* = 1894*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_PORTS_PORT6_E                         /* = 1895*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_PORTS_PORT7_E                         /* = 1896*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_PORTS_PORT8_E                         /* = 1897*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_PORTS_PORT9_E                         /* = 1898*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_PORTS_PORT10_E                        /* = 1899*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_PORTS_PORT11_E                        /* = 1900*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_PORTS_PORT63_E                        /* = 1901*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_PORTS_DISP0_E                         /* = 1902*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_PORTS_DISP1_E                         /* = 1903*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_PORTS_DISP2_E                         /* = 1904*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_PORTS_DISP3_E                         /* = 1905*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_PORTS_DISP4_E                         /* = 1906*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_PORTS_DISP5_E                         /* = 1907*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_PORTS_DISP6_E                         /* = 1908*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_PORTS_DISP7_E                         /* = 1909*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_PORTS_DISP8_E                         /* = 1910*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_PORTS_DISP9_E                         /* = 1911*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_PORTS_DISP10_E                        /* = 1912*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_PORTS_DISP11_E                        /* = 1913*/ ,
    PRV_CPSS_PM3INT_MA0_MERGER_PORTS_DISP63_E                        /* = 1914*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(MA0_MERGER_PORTS)                    /* = 1915-1919*/,

    /*INTERRUPT ID BASE:[1920]*/
    PRV_CPSS_PM3INT_MA1_MERGER_PORTS_GENERAL_E                       /* = 1920*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_PORTS_PORT0_E                         /* = 1921*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_PORTS_PORT1_E                         /* = 1922*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_PORTS_PORT2_E                         /* = 1923*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_PORTS_PORT3_E                         /* = 1924*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_PORTS_PORT4_E                         /* = 1925*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_PORTS_PORT5_E                         /* = 1926*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_PORTS_PORT6_E                         /* = 1927*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_PORTS_PORT7_E                         /* = 1928*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_PORTS_PORT8_E                         /* = 1929*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_PORTS_PORT9_E                         /* = 1930*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_PORTS_PORT10_E                        /* = 1931*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_PORTS_PORT11_E                        /* = 1932*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_PORTS_PORT63_E                        /* = 1933*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_PORTS_DISP0_E                         /* = 1934*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_PORTS_DISP1_E                         /* = 1935*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_PORTS_DISP2_E                         /* = 1936*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_PORTS_DISP3_E                         /* = 1937*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_PORTS_DISP4_E                         /* = 1938*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_PORTS_DISP5_E                         /* = 1939*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_PORTS_DISP6_E                         /* = 1940*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_PORTS_DISP7_E                         /* = 1941*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_PORTS_DISP8_E                         /* = 1942*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_PORTS_DISP9_E                         /* = 1943*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_PORTS_DISP10_E                        /* = 1944*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_PORTS_DISP11_E                        /* = 1945*/ ,
    PRV_CPSS_PM3INT_MA1_MERGER_PORTS_DISP63_E                        /* = 1946*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(MA1_MERGER_PORTS)                    /* = 1947-1951*/,

    /*Start MAAlignerIntSum - MA Aligner Interrupt Cause address */
    /* MA0(network)[0x02500100] and MA1(fabric)[0x02B00100]      */
    /* Tree place: .4.5.3 and 4.6.3*/
    /*INTERRUPT ID BASE:[1952]*/
    PRV_CPSS_PM3INT_MA0_ALIGNER_GENERAL_E                            /* = 1952*/ ,
    PRV_CPSS_PM3INT_MA0_ALIGNER_READ_VALID_RAMS_E                    /* = 1953*/ ,
    PRV_CPSS_PM3INT_MA0_ALIGNER_ADDRESS_OUT_OF_RANGE_E               /* = 1954*/ ,
    PRV_CPSS_PM3INT_MA0_ALIGNER_READ_FIFO_FULL_E                     /* = 1955*/ ,
    PRV_CPSS_PM3INT_MA0_ALIGNER_EPCL_WRONG_TRG_PORT_E                /* = 1956*/ ,
    PRV_CPSS_PM3INT_MA0_ALIGNER_TXSDMA_WRONG_TRG_PORT_E              /* = 1957*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(MA0_ALIGNER)                         /* = 1958-1983*/,

    /*INTERRUPT ID BASE:[1984]*/
    PRV_CPSS_PM3INT_MA1_ALIGNER_GENERAL_E                            /* = 1984*/ ,
    PRV_CPSS_PM3INT_MA1_ALIGNER_READ_VALID_RAMS_E                    /* = 1985*/ ,
    PRV_CPSS_PM3INT_MA1_ALIGNER_ADDRESS_OUT_OF_RANGE_E               /* = 1986*/ ,
    PRV_CPSS_PM3INT_MA1_ALIGNER_READ_FIFO_FULL_E                     /* = 1987*/ ,
    PRV_CPSS_PM3INT_MA1_ALIGNER_EPCL_WRONG_TRG_PORT_E                /* = 1988*/ ,
    PRV_CPSS_PM3INT_MA1_ALIGNER_TXSDMA_WRONG_TRG_PORT_E              /* = 1989*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(MA1_ALIGNER)                         /* = 1990-2015*/,

    /*Start LmsIntSum - LMS Interrupt Cause address */
    /* LMS0(network)[0x02200000] and LMS1(fabric)[0x02800000]      */
    /* Tree place: .4.7 and 4.8*/
    /*INTERRUPT ID BASE:[2016]*/
    PRV_CPSS_PM3INT_LMS0_GENERAL_E                                   /* = 2016*/ ,
    PRV_CPSS_PM3INT_LMS0_ADDRESS_OUT_OF_RANGE_E                      /* = 2017*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(LMS0)                                /* = 2018-2047*/,

    /*INTERRUPT ID BASE:[2048]*/
    PRV_CPSS_PM3INT_LMS1_GENERAL_E                                   /* = 2048*/ ,
    PRV_CPSS_PM3INT_LMS1_ADDRESS_OUT_OF_RANGE_E                      /* = 2049*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(LMS1)                                /* = 2050-2079*/,

    /*Start TxDmaIntSum - TX_DMA Interrupt Summary address */
    /* TX_DMA0(network)[0x02600130] and TX_DMA1(fabric)[0x02C00130]      */
    /* Tree place: .4.9 and 4.10*/
    /*INTERRUPT ID BASE:[2080]*/
    PRV_CPSS_PM3INT_TX_DMA0_SUM_GENERAL_E                            /* = 2080*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_SUM_INTERRUPT0_E                         /* = 2081*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_SUM_ERROR_E                              /* = 2082*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_SUM_INTERRUPT1_E                         /* = 2083*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_SUM_INTERRUPT2_E                         /* = 2084*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TX_DMA0_SUM)                         /* = 2085-2111*/,

    /*INTERRUPT ID BASE:[2112]*/
    PRV_CPSS_PM3INT_TX_DMA1_SUM_GENERAL_E                            /* = 2112*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_SUM_INTERRUPT0_E                         /* = 2113*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_SUM_ERROR_E                              /* = 2114*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_SUM_INTERRUPT1_E                         /* = 2115*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_SUM_INTERRUPT2_E                         /* = 2116*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TX_DMA1_SUM)                         /* = 2117-2143*/,

    /*Start TxDmaIntCause0 - TX_DMA Interrupt Cause0 address */
    /* TX_DMA0(network)[0x02600138] and TX_DMA1(fabric)[0x02C00138]      */
    /* Tree place: .4.9.1 and 4.10.1*/
    /*INTERRUPT ID BASE:[2144]*/
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_GENERAL_E                         /* = 2144*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_ADDRESS_OUT_OF_RANGE_E            /* = 2145*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_BANK0_ECC_ERR_E                   /* = 2146*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(TX_DMA0_CAUSE0, 9)                 /* = 2147-2153*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_BANK1_ECC_ERR_E                   /* = 2154*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(TX_DMA0_CAUSE0, 17)                /* = 2155-2161*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_BURST_FIFO_UNDERRUN_E             /* = 2162*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO0_UNDERRUN_E               /* = 2163*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO1_UNDERRUN_E               /* = 2164*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO2_UNDERRUN_E               /* = 2165*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO3_UNDERRUN_E               /* = 2166*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO4_UNDERRUN_E               /* = 2167*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO5_UNDERRUN_E               /* = 2168*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO6_UNDERRUN_E               /* = 2169*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO7_UNDERRUN_E               /* = 2170*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO8_UNDERRUN_E               /* = 2171*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO9_UNDERRUN_E               /* = 2172*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO10_UNDERRUN_E              /* = 2173*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO11_UNDERRUN_E              /* = 2174*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE0_TX_FIFO12_UNDERRUN_E              /* = 2175*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TX_DMA0_CAUSE0)                      /* = 2175-2175*/,

    /*INTERRUPT ID BASE:[2176]*/
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_GENERAL_E                         /* = 2176*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_ADDRESS_OUT_OF_RANGE_E            /* = 2177*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_BANK0_ECC_ERR_E                   /* = 2178*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(TX_DMA1_CAUSE0, 9)                 /* = 2179-2185*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_BANK1_ECC_ERR_E                   /* = 2186*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(TX_DMA1_CAUSE0, 17)                /* = 2187-2193*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_BURST_FIFO_UNDERRUN_E             /* = 2194*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO0_UNDERRUN_E               /* = 2195*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO1_UNDERRUN_E               /* = 2196*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO2_UNDERRUN_E               /* = 2197*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO3_UNDERRUN_E               /* = 2198*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO4_UNDERRUN_E               /* = 2199*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO5_UNDERRUN_E               /* = 2200*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO6_UNDERRUN_E               /* = 2201*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO7_UNDERRUN_E               /* = 2202*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO8_UNDERRUN_E               /* = 2203*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO9_UNDERRUN_E               /* = 2204*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO10_UNDERRUN_E              /* = 2205*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO11_UNDERRUN_E              /* = 2206*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE0_TX_FIFO12_UNDERRUN_E              /* = 2207*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TX_DMA1_CAUSE0)                      /* = 2207-2207*/,

    /*Start TxDmaIntError - TX_DMA Interrupt Error address */
    /* TX_DMA0(network)[0x02600140] and TX_DMA1(fabric)[0x02C00140]      */
    /* Tree place: .4.9.2 and 4.10.2*/
    /*INTERRUPT ID BASE:[2208]*/
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_GENERAL_E                          /* = 2208*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_CLEAR_FIFO_FULL_E                  /* = 2209*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_CUT_THROW_ERR_E                    /* = 2210*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_HA_INFO_FIFO_ECC_ONE_ERR_E         /* = 2211*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_HA_INFO_FIFO_ECC_MORE_ERR_E        /* = 2212*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO0_BK0_OVERRUN_E             /* = 2213*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO1_BK0_OVERRUN_E             /* = 2214*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO2_BK0_OVERRUN_E             /* = 2215*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO3_BK0_OVERRUN_E             /* = 2216*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO4_BK0_OVERRUN_E             /* = 2217*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO5_BK0_OVERRUN_E             /* = 2218*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO6_BK0_OVERRUN_E             /* = 2219*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO7_BK0_OVERRUN_E             /* = 2220*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO0_BK1_OVERRUN_E             /* = 2221*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO1_BK1_OVERRUN_E             /* = 2222*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO2_BK1_OVERRUN_E             /* = 2223*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO3_BK1_OVERRUN_E             /* = 2224*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO4_BK1_OVERRUN_E             /* = 2225*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO5_BK1_OVERRUN_E             /* = 2226*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO6_BK1_OVERRUN_E             /* = 2227*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_RD_FIFO7_BK1_OVERRUN_E             /* = 2228*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_TXDMA2HA_ECC_SINGLE_ERR_E          /* = 2229*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_ERROR_TXDMA2HA_ECC_DOUBLE_ERR_E          /* = 2230*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TX_DMA0_ERROR)                       /* = 2231-2239*/,

    /*INTERRUPT ID BASE:[2240]*/
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_GENERAL_E                          /* = 2240*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_CLEAR_FIFO_FULL_E                  /* = 2241*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_CUT_THROW_ERR_E                    /* = 2242*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_HA_INFO_FIFO_ECC_ONE_ERR_E         /* = 2243*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_HA_INFO_FIFO_ECC_MORE_ERR_E        /* = 2244*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO0_BK0_OVERRUN_E             /* = 2245*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO1_BK0_OVERRUN_E             /* = 2246*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO2_BK0_OVERRUN_E             /* = 2247*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO3_BK0_OVERRUN_E             /* = 2248*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO4_BK0_OVERRUN_E             /* = 2249*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO5_BK0_OVERRUN_E             /* = 2250*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO6_BK0_OVERRUN_E             /* = 2251*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO7_BK0_OVERRUN_E             /* = 2252*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO0_BK1_OVERRUN_E             /* = 2253*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO1_BK1_OVERRUN_E             /* = 2254*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO2_BK1_OVERRUN_E             /* = 2255*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO3_BK1_OVERRUN_E             /* = 2256*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO4_BK1_OVERRUN_E             /* = 2257*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO5_BK1_OVERRUN_E             /* = 2258*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO6_BK1_OVERRUN_E             /* = 2259*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_RD_FIFO7_BK1_OVERRUN_E             /* = 2260*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_TXDMA2HA_ECC_SINGLE_ERR_E          /* = 2261*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_ERROR_TXDMA2HA_ECC_DOUBLE_ERR_E          /* = 2262*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TX_DMA1_ERROR)                       /* = 2263-2271*/,

    /*Start TxDmaIntCause1 - TX_DMA Interrupt Cause1 address */
    /* TX_DMA0(network)[0x02600530] and TX_DMA1(fabric)[0x02C00530]      */
    /* Tree place: .4.9.3 and 4.10.3*/
    /*INTERRUPT ID BASE:[2272]*/
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE1_GENERAL_E                         /* = 2272*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE1_TXFIFO0_AIR_CTRL0_UNDERRUN_E      /* = 2273*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE1_TXFIFO0_AIR_CTRL1_UNDERRUN_E      /* = 2274*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE1_TXFIFO1_AIR_CTRL0_UNDERRUN_E      /* = 2275*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE1_TXFIFO1_AIR_CTRL1_UNDERRUN_E      /* = 2276*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE1_TXFIFO2_AIR_CTRL0_UNDERRUN_E      /* = 2277*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE1_TXFIFO2_AIR_CTRL1_UNDERRUN_E      /* = 2278*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE1_TXFIFO3_AIR_CTRL0_UNDERRUN_E      /* = 2279*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE1_TXFIFO3_AIR_CTRL1_UNDERRUN_E      /* = 2280*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE1_TXFIFO4_AIR_CTRL0_UNDERRUN_E      /* = 2281*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE1_TXFIFO4_AIR_CTRL1_UNDERRUN_E      /* = 2282*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE1_TXFIFO5_AIR_CTRL0_UNDERRUN_E      /* = 2283*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE1_TXFIFO5_AIR_CTRL1_UNDERRUN_E      /* = 2284*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE1_TXFIFO6_AIR_CTRL0_UNDERRUN_E      /* = 2285*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE1_TXFIFO6_AIR_CTRL1_UNDERRUN_E      /* = 2286*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE1_TXFIFO7_AIR_CTRL0_UNDERRUN_E      /* = 2287*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE1_TXFIFO7_AIR_CTRL1_UNDERRUN_E      /* = 2288*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE1_TXFIFO8_AIR_CTRL0_UNDERRUN_E      /* = 2289*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE1_TXFIFO8_AIR_CTRL1_UNDERRUN_E      /* = 2290*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TX_DMA0_CAUSE1)                      /* = 2291-2303*/,

    /*INTERRUPT ID BASE:[2304]*/
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE1_GENERAL_E                         /* = 2304*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE1_TXFIFO0_AIR_CTRL0_UNDERRUN_E      /* = 2305*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE1_TXFIFO0_AIR_CTRL1_UNDERRUN_E      /* = 2306*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE1_TXFIFO1_AIR_CTRL0_UNDERRUN_E      /* = 2307*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE1_TXFIFO1_AIR_CTRL1_UNDERRUN_E      /* = 2308*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE1_TXFIFO2_AIR_CTRL0_UNDERRUN_E      /* = 2309*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE1_TXFIFO2_AIR_CTRL1_UNDERRUN_E      /* = 2310*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE1_TXFIFO3_AIR_CTRL0_UNDERRUN_E      /* = 2311*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE1_TXFIFO3_AIR_CTRL1_UNDERRUN_E      /* = 2312*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE1_TXFIFO4_AIR_CTRL0_UNDERRUN_E      /* = 2313*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE1_TXFIFO4_AIR_CTRL1_UNDERRUN_E      /* = 2314*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE1_TXFIFO5_AIR_CTRL0_UNDERRUN_E      /* = 2315*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE1_TXFIFO5_AIR_CTRL1_UNDERRUN_E      /* = 2316*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE1_TXFIFO6_AIR_CTRL0_UNDERRUN_E      /* = 2317*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE1_TXFIFO6_AIR_CTRL1_UNDERRUN_E      /* = 2318*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE1_TXFIFO7_AIR_CTRL0_UNDERRUN_E      /* = 2319*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE1_TXFIFO7_AIR_CTRL1_UNDERRUN_E      /* = 2320*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE1_TXFIFO8_AIR_CTRL0_UNDERRUN_E      /* = 2321*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE1_TXFIFO8_AIR_CTRL1_UNDERRUN_E      /* = 2322*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TX_DMA1_CAUSE1)                      /* = 2323-2335*/,

    /*Start TxDmaIntCause2 - TX_DMA Interrupt Cause2 address */
    /* TX_DMA0(network)[0x02600538] and TX_DMA1(fabric)[0x02C00538]      */
    /* Tree place: .4.9.4 and 4.10.4*/
    /*INTERRUPT ID BASE:[2336]*/
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE2_GENERAL_E                         /* = 2336*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE2_PREF_CTRL0_OVERRUN_E              /* = 2337*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE2_PREF_CTRL1_OVERRUN_E              /* = 2338*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE2_PREF_CTRL2_OVERRUN_E              /* = 2339*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE2_PREF_CTRL3_OVERRUN_E              /* = 2340*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE2_PREF_CTRL4_OVERRUN_E              /* = 2341*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE2_PREF_CTRL5_OVERRUN_E              /* = 2342*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE2_PREF_CTRL6_OVERRUN_E              /* = 2343*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE2_PREF_CTRL7_OVERRUN_E              /* = 2344*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE2_PREF_CTRL8_OVERRUN_E              /* = 2345*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE2_PREF_CTRL9_OVERRUN_E              /* = 2346*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE2_PREF_CTRL10_OVERRUN_E             /* = 2347*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE2_PREF_CTRL11_OVERRUN_E             /* = 2348*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE2_PREF_CTRL_CPU_OVERRUN_E           /* = 2349*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE2_BURST_FIFO0_FULL_E                /* = 2350*/ ,
    PRV_CPSS_PM3INT_TX_DMA0_CAUSE2_BURST_FIFO1_FULL_E                /* = 2351*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TX_DMA0_CAUSE2)                      /* = 2352-2367*/,

    /*INTERRUPT ID BASE:[2368]*/
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE2_GENERAL_E                         /* = 2368*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE2_PREF_CTRL0_OVERRUN_E              /* = 2369*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE2_PREF_CTRL1_OVERRUN_E              /* = 2370*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE2_PREF_CTRL2_OVERRUN_E              /* = 2371*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE2_PREF_CTRL3_OVERRUN_E              /* = 2372*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE2_PREF_CTRL4_OVERRUN_E              /* = 2373*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE2_PREF_CTRL5_OVERRUN_E              /* = 2374*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE2_PREF_CTRL6_OVERRUN_E              /* = 2375*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE2_PREF_CTRL7_OVERRUN_E              /* = 2376*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE2_PREF_CTRL8_OVERRUN_E              /* = 2377*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE2_PREF_CTRL9_OVERRUN_E              /* = 2378*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE2_PREF_CTRL10_OVERRUN_E             /* = 2379*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE2_PREF_CTRL11_OVERRUN_E             /* = 2380*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE2_PREF_CTRL_CPU_OVERRUN_E           /* = 2381*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE2_BURST_FIFO0_FULL_E                /* = 2382*/ ,
    PRV_CPSS_PM3INT_TX_DMA1_CAUSE2_BURST_FIFO1_FULL_E                /* = 2383*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TX_DMA1_CAUSE2)                      /* = 2384-2399*/,


    /*Start RxDmaIntSum - RX_DMA Interrupt Summary address */
    /* RX_DMA0(network)[0x01700570] and RX_DMA1(fabric)[0x01A00570]      */
    /* Tree place: .4.11 and 4.12*/
    /*INTERRUPT ID BASE:[2400]*/
    PRV_CPSS_PM3INT_RX_DMA0_SUM_GENERAL_E                            /* = 2400*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SUM_INT0_E                               /* = 2401*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SUM_SCDMA0_E                             /* = 2402*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SUM_SCDMA1_E                             /* = 2403*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SUM_SCDMA2_E                             /* = 2404*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SUM_SCDMA3_E                             /* = 2405*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SUM_SCDMA4_E                             /* = 2406*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SUM_SCDMA5_E                             /* = 2407*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SUM_SCDMA6_E                             /* = 2408*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SUM_SCDMA7_E                             /* = 2409*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SUM_SCDMA8_E                             /* = 2410*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SUM_SCDMA9_E                             /* = 2411*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SUM_SCDMA10_E                            /* = 2412*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SUM_SCDMA11_E                            /* = 2413*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SUM_SCDMA12_E                            /* = 2414*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA0_SUM)                         /* = 2415-2431*/,

    /*INTERRUPT ID BASE:[2432]*/
    PRV_CPSS_PM3INT_RX_DMA1_SUM_GENERAL_E                            /* = 2432*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SUM_INT0_E                               /* = 2433*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SUM_SCDMA0_E                             /* = 2434*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SUM_SCDMA1_E                             /* = 2435*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SUM_SCDMA2_E                             /* = 2436*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SUM_SCDMA3_E                             /* = 2437*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SUM_SCDMA4_E                             /* = 2438*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SUM_SCDMA5_E                             /* = 2439*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SUM_SCDMA6_E                             /* = 2440*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SUM_SCDMA7_E                             /* = 2441*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SUM_SCDMA8_E                             /* = 2442*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SUM_SCDMA9_E                             /* = 2443*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SUM_SCDMA10_E                            /* = 2444*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SUM_SCDMA11_E                            /* = 2445*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SUM_SCDMA12_E                            /* = 2446*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA1_SUM)                         /* = 2447-2463*/,


    /*Start RxDmaInt0 - RX_DMA Interrupt0 address */
    /* RX_DMA0(network)[0x01700500] and RX_DMA1(fabric)[0x01A00500]      */
    /* Tree place: .4.11.1 and 4.12.1*/
    /*INTERRUPT ID BASE:[2464]*/
    PRV_CPSS_PM3INT_RX_DMA0_INT0_GENERAL_E                           /* = 2464*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_INT0_IBUF_HDR_FIFO_OVERRUN_E             /* = 2465*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_INT0_RXDMA_RF_ERR_E                      /* = 2466*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_INT0_HDR_RAM_RD_OVERFLOW_E               /* = 2467*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA0_INT0)                        /* = 2468-2495*/,

    /*INTERRUPT ID BASE:[2496]*/
    PRV_CPSS_PM3INT_RX_DMA1_INT0_GENERAL_E                           /* = 2496*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_INT0_IBUF_HDR_FIFO_OVERRUN_E             /* = 2497*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_INT0_RXDMA_RF_ERR_E                      /* = 2498*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_INT0_HDR_RAM_RD_OVERFLOW_E               /* = 2499*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA1_INT0)                        /* = 2500-2527*/,

    /*Start RxDmaScDma0 - RX_DMA SCDMA0 address */
    /* RX_DMA0(network)[0x01700508] and RX_DMA1(fabric)[0x01A00508]      */
    /* Tree place: .4.11.2 and 4.12.2*/
    /*INTERRUPT ID BASE:[2528]*/
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA0_GENERAL_E                         /* = 2528*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA0_PKT_LESS_64B_E                    /* = 2529*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA0_NO_FREE_BUFFERS_E                 /* = 2530*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA0_SCDMA0)                      /* = 2531-2559*/,

    /*INTERRUPT ID BASE:[2560]*/
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA0_GENERAL_E                         /* = 2560*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA0_PKT_LESS_64B_E                    /* = 2561*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA0_NO_FREE_BUFFERS_E                 /* = 2562*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA1_SCDMA0)                      /* = 2563-2591*/,

    /*Start RxDmaScDma1 - RX_DMA SCDMA1 address */
    /* RX_DMA0(network)[0x0170050C] and RX_DMA1(fabric)[0x01A0050C]      */
    /* Tree place: .4.11.3 and 4.12.3*/
    /*INTERRUPT ID BASE:[2592]*/
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA1_GENERAL_E                         /* = 2592*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA1_PKT_LESS_64B_E                    /* = 2593*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA1_NO_FREE_BUFFERS_E                 /* = 2594*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA0_SCDMA1)                      /* = 2595-2623*/,

    /*INTERRUPT ID BASE:[2624]*/
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA1_GENERAL_E                         /* = 2624*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA1_PKT_LESS_64B_E                    /* = 2625*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA1_NO_FREE_BUFFERS_E                 /* = 2626*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA1_SCDMA1)                      /* = 2627-2655*/,

    /*Start RxDmaScDma2 - RX_DMA SCDMA2 address */
    /* RX_DMA0(network)[0x01700510] and RX_DMA1(fabric)[0x01A00510]      */
    /* Tree place: .4.11.4 and 4.12.4*/
    /*INTERRUPT ID BASE:[2656]*/
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA2_GENERAL_E                         /* = 2656*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA2_PKT_LESS_64B_E                    /* = 2657*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA2_NO_FREE_BUFFERS_E                 /* = 2658*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA0_SCDMA2)                      /* = 2659-2687*/,

    /*INTERRUPT ID BASE:[2688]*/
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA2_GENERAL_E                         /* = 2688*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA2_PKT_LESS_64B_E                    /* = 2689*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA2_NO_FREE_BUFFERS_E                 /* = 2690*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA1_SCDMA2)                      /* = 2691-2719*/,

    /*Start RxDmaScDma3 - RX_DMA SCDMA3 address */
    /* RX_DMA0(network)[0x01700514] and RX_DMA1(fabric)[0x01A00514]      */
    /* Tree place: .4.11.5 and 4.12.5*/
    /*INTERRUPT ID BASE:[2720]*/
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA3_GENERAL_E                         /* = 2720*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA3_PKT_LESS_64B_E                    /* = 2721*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA3_NO_FREE_BUFFERS_E                 /* = 2722*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA0_SCDMA3)                      /* = 2723-2751*/,

    /*INTERRUPT ID BASE:[2752]*/
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA3_GENERAL_E                         /* = 2752*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA3_PKT_LESS_64B_E                    /* = 2753*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA3_NO_FREE_BUFFERS_E                 /* = 2754*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA1_SCDMA3)                      /* = 2755-2783*/,

    /*Start RxDmaScDma4 - RX_DMA SCDMA4 address */
    /* RX_DMA0(network)[0x01700518] and RX_DMA1(fabric)[0x01A00518]      */
    /* Tree place: .4.11.6 and 4.12.6*/
    /*INTERRUPT ID BASE:[2784]*/
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA4_GENERAL_E                         /* = 2784*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA4_PKT_LESS_64B_E                    /* = 2785*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA4_NO_FREE_BUFFERS_E                 /* = 2786*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA0_SCDMA4)                      /* = 2787-2815*/,

    /*INTERRUPT ID BASE:[2816]*/
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA4_GENERAL_E                         /* = 2816*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA4_PKT_LESS_64B_E                    /* = 2817*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA4_NO_FREE_BUFFERS_E                 /* = 2818*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA1_SCDMA4)                      /* = 2819-2847*/,

    /*Start RxDmaScDma5 - RX_DMA SCDMA5 address */
    /* RX_DMA0(network)[0x0170051C] and RX_DMA1(fabric)[0x01A0051C]      */
    /* Tree place: .4.11.7 and 4.12.7*/
    /*INTERRUPT ID BASE:[2848]*/
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA5_GENERAL_E                         /* = 2848*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA5_PKT_LESS_64B_E                    /* = 2849*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA5_NO_FREE_BUFFERS_E                 /* = 2850*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA0_SCDMA5)                      /* = 2851-2879*/,

    /*INTERRUPT ID BASE:[2880]*/
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA5_GENERAL_E                         /* = 2880*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA5_PKT_LESS_64B_E                    /* = 2881*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA5_NO_FREE_BUFFERS_E                 /* = 2882*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA1_SCDMA5)                      /* = 2883-2911*/,

    /*Start RxDmaScDma6 - RX_DMA SCDMA6 address */
    /* RX_DMA0(network)[0x01700520] and RX_DMA1(fabric)[0x01A00520]      */
    /* Tree place: .4.11.8 and 4.12.8*/
    /*INTERRUPT ID BASE:[2912]*/
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA6_GENERAL_E                         /* = 2912*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA6_PKT_LESS_64B_E                    /* = 2913*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA6_NO_FREE_BUFFERS_E                 /* = 2914*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA0_SCDMA6)                      /* = 2915-2943*/,

    /*INTERRUPT ID BASE:[2944]*/
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA6_GENERAL_E                         /* = 2944*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA6_PKT_LESS_64B_E                    /* = 2945*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA6_NO_FREE_BUFFERS_E                 /* = 2946*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA1_SCDMA6)                      /* = 2947-2975*/,

    /*Start RxDmaScDma7 - RX_DMA SCDMA7 address */
    /* RX_DMA0(network)[0x01700524] and RX_DMA1(fabric)[0x01A00524]      */
    /* Tree place: .4.11.9 and 4.12.9*/
    /*INTERRUPT ID BASE:[2976]*/
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA7_GENERAL_E                         /* = 2976*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA7_PKT_LESS_64B_E                    /* = 2977*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA7_NO_FREE_BUFFERS_E                 /* = 2978*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA0_SCDMA7)                      /* = 2979-3007*/,

    /*INTERRUPT ID BASE:[3008]*/
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA7_GENERAL_E                         /* = 3008*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA7_PKT_LESS_64B_E                    /* = 3009*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA7_NO_FREE_BUFFERS_E                 /* = 3010*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA1_SCDMA7)                      /* = 3011-3039*/,

    /*Start RxDmaScDma8 - RX_DMA SCDMA8 address */
    /* RX_DMA0(network)[0x01700528] and RX_DMA1(fabric)[0x01A00528]      */
    /* Tree place: .4.11.10 and 4.12.10*/
    /*INTERRUPT ID BASE:[3040]*/
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA8_GENERAL_E                         /* = 3040*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA8_PKT_LESS_64B_E                    /* = 3041*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA8_NO_FREE_BUFFERS_E                 /* = 3042*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA0_SCDMA8)                      /* = 3043-3071*/,

    /*INTERRUPT ID BASE:[3072]*/
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA8_GENERAL_E                         /* = 3072*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA8_PKT_LESS_64B_E                    /* = 3073*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA8_NO_FREE_BUFFERS_E                 /* = 3074*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA1_SCDMA8)                      /* = 3075-3103*/,

    /*Start RxDmaScDma9 - RX_DMA SCDMA9 address */
    /* RX_DMA0(network)[0x0170052C] and RX_DMA1(fabric)[0x01A0052C]      */
    /* Tree place: .4.11.11 and 4.12.11*/
    /*INTERRUPT ID BASE:[3104]*/
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA9_GENERAL_E                         /* = 3104*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA9_PKT_LESS_64B_E                    /* = 3105*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA9_NO_FREE_BUFFERS_E                 /* = 3106*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA0_SCDMA9)                      /* = 3107-3135*/,

    /*INTERRUPT ID BASE:[3136]*/
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA9_GENERAL_E                         /* = 3136*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA9_PKT_LESS_64B_E                    /* = 3137*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA9_NO_FREE_BUFFERS_E                 /* = 3138*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA1_SCDMA9)                      /* = 3139-3167*/,

    /*Start RxDmaScDma10 - RX_DMA SCDMA10 address */
    /* RX_DMA0(network)[0x01700530] and RX_DMA1(fabric)[0x01A00530]      */
    /* Tree place: .4.11.12 and 4.12.12*/
    /*INTERRUPT ID BASE:[3168]*/
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA10_GENERAL_E                        /* = 3168*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA10_PKT_LESS_64B_E                   /* = 3169*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA10_NO_FREE_BUFFERS_E                /* = 3170*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA0_SCDMA10)                     /* = 3171-3199*/,

    /*INTERRUPT ID BASE:[3200]*/
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA10_GENERAL_E                        /* = 3200*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA10_PKT_LESS_64B_E                   /* = 3201*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA10_NO_FREE_BUFFERS_E                /* = 3202*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA1_SCDMA10)                     /* = 3203-3231*/,

    /*Start RxDmaScDma11 - RX_DMA SCDMA11 address */
    /* RX_DMA0(network)[0x01700534] and RX_DMA1(fabric)[0x01A00534]      */
    /* Tree place: .4.11.13 and 4.12.13*/
    /*INTERRUPT ID BASE:[3232]*/
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA11_GENERAL_E                        /* = 3232*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA11_PKT_LESS_64B_E                   /* = 3233*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA11_NO_FREE_BUFFERS_E                /* = 3234*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA0_SCDMA11)                     /* = 3235-3263*/,

    /*INTERRUPT ID BASE:[3264]*/
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA11_GENERAL_E                        /* = 3264*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA11_PKT_LESS_64B_E                   /* = 3265*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA11_NO_FREE_BUFFERS_E                /* = 3266*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA1_SCDMA11)                     /* = 3267-3295*/,

    /*Start RxDmaScDma12 - RX_DMA SCDMA12 address */
    /* RX_DMA0(network)[0x01700538] and RX_DMA1(fabric)[0x01A00538]      */
    /* Tree place: .4.11.14 and 4.12.14*/
    /*INTERRUPT ID BASE:[3296]*/
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA12_GENERAL_E                        /* = 3296*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA12_PKT_LESS_64B_E                   /* = 3297*/ ,
    PRV_CPSS_PM3INT_RX_DMA0_SCDMA12_NO_FREE_BUFFERS_E                /* = 3298*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA0_SCDMA12)                     /* = 3299-3327*/,

    /*INTERRUPT ID BASE:[3328]*/
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA12_GENERAL_E                        /* = 3328*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA12_PKT_LESS_64B_E                   /* = 3329*/ ,
    PRV_CPSS_PM3INT_RX_DMA1_SCDMA12_NO_FREE_BUFFERS_E                /* = 3330*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_DMA1_SCDMA12)                     /* = 3331-3359*/,

    /*Start PortsIntSum - Ports interrupt Summary address [0x00000080]*/
    /* Tree place: .5*/
    /*INTERRUPT ID BASE:[3360]*/
    PRV_CPSS_PM3INT_PORTS_SUM_GENERAL_E                              /* = 3360*/ ,
    PRV_CPSS_PM3INT_PORTS_SUM_NW_CG_PORTS_E                          /* = 3361*/ ,
    PRV_CPSS_PM3INT_PORTS_SUM_FB_CG_PORTS_E                          /* = 3362*/ ,
    PRV_CPSS_PM3INT_PORTS_SUM_FB_MSM_PORT0_E                         /* = 3363*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_SUM, 6)                      /* = 3364-3366*/ ,
    PRV_CPSS_PM3INT_PORTS_SUM_FB_MSM_PORT1_E                         /* = 3367*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_SUM, 10)                     /* = 3368-3370*/ ,
    PRV_CPSS_PM3INT_PORTS_SUM_FB_MSM_PORT2_E                         /* = 3371*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_SUM, 14)                     /* = 3372-3374*/ ,
    PRV_CPSS_PM3INT_PORTS_SUM_FB_MSM_PORT3_E                         /* = 3375*/ ,
    PRV_CPSS_PM3INT_PORTS_SUM_RESERVED16_E                           /* = 3376*/ ,
    PRV_CPSS_PM3INT_PORTS_SUM_NW_MSM_PORT0_E                         /* = 3377*/ ,
    PRV_CPSS_PM3INT_PORTS_SUM_NW_MSM_PORT1_E                         /* = 3378*/ ,
    PRV_CPSS_PM3INT_PORTS_SUM_NW_MSM_PORT2_E                         /* = 3379*/ ,
    PRV_CPSS_PM3INT_PORTS_SUM_NW_MSM_PORT3_E                         /* = 3380*/ ,
    PRV_CPSS_PM3INT_PORTS_SUM_NW_MSM_PORT4_E                         /* = 3381*/ ,
    PRV_CPSS_PM3INT_PORTS_SUM_NW_MSM_PORT5_E                         /* = 3382*/ ,
    PRV_CPSS_PM3INT_PORTS_SUM_NW_MSM_PORT6_E                         /* = 3383*/ ,
    PRV_CPSS_PM3INT_PORTS_SUM_NW_MSM_PORT7_E                         /* = 3384*/ ,
    PRV_CPSS_PM3INT_PORTS_SUM_NW_MSM_PORT8_E                         /* = 3385*/ ,
    PRV_CPSS_PM3INT_PORTS_SUM_NW_MSM_PORT9_E                         /* = 3386*/ ,
    PRV_CPSS_PM3INT_PORTS_SUM_NW_MSM_PORT10_E                        /* = 3387*/ ,
    PRV_CPSS_PM3INT_PORTS_SUM_NW_MSM_PORT11_E                        /* = 3388*/ ,
    PRV_CPSS_PM3INT_PORTS_SUM_FB_ILKN_PORT_E                         /* = 3389*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_SUM)                           /* = 3390-3391*/,

    /*Start CgPortsIntSum - CG Ports Interrupts Summary address */
    /* NW_CG(network)[0x01800040] and FB_CG(fabric)[0x01B00040]      */
    /* Tree place: .5.1 and 5.2*/
    /*INTERRUPT ID BASE:[3392]*/
    PRV_CPSS_PM3INT_PORTS_NW_CG_GENERAL_E                        /* = 3392*/ ,
    PRV_CPSS_PM3INT_PORTS_NW_CG_OVERSIZE_PKT_DROP_E              /* = 3393*/ ,
    PRV_CPSS_PM3INT_PORTS_NW_CG_UNDERSIZE_PKT_DROP_E             /* = 3394*/ ,
    PRV_CPSS_PM3INT_PORTS_NW_CG_LINK_FAIL_E                      /* = 3395*/ ,
    PRV_CPSS_PM3INT_PORTS_NW_CG_TX_SER_ERR_E                     /* = 3396*/ ,
    PRV_CPSS_PM3INT_PORTS_NW_CG_RX_SER_ERR0_E                    /* = 3397*/ ,
    PRV_CPSS_PM3INT_PORTS_NW_CG_RX_SER_ERR1_E                    /* = 3398*/ ,
    PRV_CPSS_PM3INT_PORTS_NW_CG_TX_FIFO_OVERLOW_E                /* = 3399*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_NW_CG)                     /* = 3400-3423*/,

    /*INTERRUPT ID BASE:[3424]*/
    PRV_CPSS_PM3INT_PORTS_FB_CG_GENERAL_E                        /* = 3424*/ ,
    PRV_CPSS_PM3INT_PORTS_FB_CG_OVERSIZE_PKT_DROP_E              /* = 3425*/ ,
    PRV_CPSS_PM3INT_PORTS_FB_CG_UNDERSIZE_PKT_DROP_E             /* = 3426*/ ,
    PRV_CPSS_PM3INT_PORTS_FB_CG_LINK_FAIL_E                      /* = 3427*/ ,
    PRV_CPSS_PM3INT_PORTS_FB_CG_TX_SER_ERR_E                     /* = 3428*/ ,
    PRV_CPSS_PM3INT_PORTS_FB_CG_RX_SER_ERR0_E                    /* = 3429*/ ,
    PRV_CPSS_PM3INT_PORTS_FB_CG_RX_SER_ERR1_E                    /* = 3430*/ ,
    PRV_CPSS_PM3INT_PORTS_FB_CG_TX_FIFO_OVERLOW_E                /* = 3431*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_FB_CG)                     /* = 3432-3455*/,

    /*Start ExtUnitsumInt - External Units Ports Interrupts Summary address */
    /* NW0[0x010C0058], NW1[0x010C1058] , NW2[0x010C2058], NW3[0x010C3058]    */
    /* NW4[0x010C4058], NW5[0x010C5058] , NW6[0x010C6058], NW7[0x010C7058]    */
    /* NW8[0x010C8058], NW9[0x010C9058] , NW10[0x010CA058], NW11[0x01BC0058]  */
    /* FB0[0x010D0058], FB1[0x010D1058] , FB2[0x010D2058], FB3[0x010D3058]    */
    /* Tree place: .5.17-28(NW0-11) and .5.3(FB0), .5.7(FB1), .5.11(FB2), .5.15(FB3) */
    /*INTERRUPT ID BASE:[3456]*/
    PRV_CPSS_PM3INT_PORTS_NW0_SUM_GENERAL_E                          /* = 3456*/ ,
    PRV_CPSS_PM3INT_PORTS_NW0_SUM_XLG_E                              /* = 3457*/ ,
    PRV_CPSS_PM3INT_PORTS_NW0_SUM_GIGE_E                             /* = 3458*/ ,
    PRV_CPSS_PM3INT_PORTS_NW0_SUM_HLG_E                              /* = 3459*/ ,
    PRV_CPSS_PM3INT_PORTS_NW0_SUM_XPCS_E                             /* = 3460*/ ,
    PRV_CPSS_PM3INT_PORTS_NW0_SUM_MPCS_E                             /* = 3461*/ ,
    PRV_CPSS_PM3INT_PORTS_NW0_SUM_MMPCS_E                            /* = 3462*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_NW0_SUM)                       /* = 3464-3487*/,

    /*INTERRUPT ID BASE:[3488]*/
    PRV_CPSS_PM3INT_PORTS_NW1_SUM_GENERAL_E                          /* = 3488*/ ,
    PRV_CPSS_PM3INT_PORTS_NW1_SUM_XLG_E                              /* = 3489*/ ,
    PRV_CPSS_PM3INT_PORTS_NW1_SUM_GIGE_E                             /* = 3490*/ ,
    PRV_CPSS_PM3INT_PORTS_NW1_SUM_HLG_E                              /* = 3491*/ ,
    PRV_CPSS_PM3INT_PORTS_NW1_SUM_XPCS_E                             /* = 3492*/ ,
    PRV_CPSS_PM3INT_PORTS_NW1_SUM_MPCS_E                             /* = 3493*/ ,
    PRV_CPSS_PM3INT_PORTS_NW1_SUM_MMPCS_E                            /* = 3494*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_NW1_SUM)                       /* = 3496-3519*/,

    /*INTERRUPT ID BASE:[3520]*/
    PRV_CPSS_PM3INT_PORTS_NW2_SUM_GENERAL_E                          /* = 3520*/ ,
    PRV_CPSS_PM3INT_PORTS_NW2_SUM_XLG_E                              /* = 3521*/ ,
    PRV_CPSS_PM3INT_PORTS_NW2_SUM_GIGE_E                             /* = 3522*/ ,
    PRV_CPSS_PM3INT_PORTS_NW2_SUM_HLG_E                              /* = 3523*/ ,
    PRV_CPSS_PM3INT_PORTS_NW2_SUM_XPCS_E                             /* = 3524*/ ,
    PRV_CPSS_PM3INT_PORTS_NW2_SUM_MPCS_E                             /* = 3525*/ ,
    PRV_CPSS_PM3INT_PORTS_NW2_SUM_MMPCS_E                            /* = 3526*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_NW2_SUM)                       /* = 3528-3551*/,

    /*INTERRUPT ID BASE:[3552]*/
    PRV_CPSS_PM3INT_PORTS_NW3_SUM_GENERAL_E                          /* = 3552*/ ,
    PRV_CPSS_PM3INT_PORTS_NW3_SUM_XLG_E                              /* = 3553*/ ,
    PRV_CPSS_PM3INT_PORTS_NW3_SUM_GIGE_E                             /* = 3554*/ ,
    PRV_CPSS_PM3INT_PORTS_NW3_SUM_HLG_E                              /* = 3555*/ ,
    PRV_CPSS_PM3INT_PORTS_NW3_SUM_XPCS_E                             /* = 3556*/ ,
    PRV_CPSS_PM3INT_PORTS_NW3_SUM_MPCS_E                             /* = 3557*/ ,
    PRV_CPSS_PM3INT_PORTS_NW3_SUM_MMPCS_E                            /* = 3558*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_NW3_SUM)                       /* = 3560-3583*/,

    /*INTERRUPT ID BASE:[3584]*/
    PRV_CPSS_PM3INT_PORTS_NW4_SUM_GENERAL_E                          /* = 3584*/ ,
    PRV_CPSS_PM3INT_PORTS_NW4_SUM_XLG_E                              /* = 3585*/ ,
    PRV_CPSS_PM3INT_PORTS_NW4_SUM_GIGE_E                             /* = 3586*/ ,
    PRV_CPSS_PM3INT_PORTS_NW4_SUM_HLG_E                              /* = 3587*/ ,
    PRV_CPSS_PM3INT_PORTS_NW4_SUM_XPCS_E                             /* = 3588*/ ,
    PRV_CPSS_PM3INT_PORTS_NW4_SUM_MPCS_E                             /* = 3589*/ ,
    PRV_CPSS_PM3INT_PORTS_NW4_SUM_MMPCS_E                            /* = 3590*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_NW4_SUM)                       /* = 3592-3615*/,

    /*INTERRUPT ID BASE:[3616]*/
    PRV_CPSS_PM3INT_PORTS_NW5_SUM_GENERAL_E                          /* = 3616*/ ,
    PRV_CPSS_PM3INT_PORTS_NW5_SUM_XLG_E                              /* = 3617*/ ,
    PRV_CPSS_PM3INT_PORTS_NW5_SUM_GIGE_E                             /* = 3618*/ ,
    PRV_CPSS_PM3INT_PORTS_NW5_SUM_HLG_E                              /* = 3619*/ ,
    PRV_CPSS_PM3INT_PORTS_NW5_SUM_XPCS_E                             /* = 3620*/ ,
    PRV_CPSS_PM3INT_PORTS_NW5_SUM_MPCS_E                             /* = 3621*/ ,
    PRV_CPSS_PM3INT_PORTS_NW5_SUM_MMPCS_E                            /* = 3622*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_NW5_SUM)                       /* = 3624-3647*/,

    /*INTERRUPT ID BASE:[3648]*/
    PRV_CPSS_PM3INT_PORTS_NW6_SUM_GENERAL_E                          /* = 3648*/ ,
    PRV_CPSS_PM3INT_PORTS_NW6_SUM_XLG_E                              /* = 3649*/ ,
    PRV_CPSS_PM3INT_PORTS_NW6_SUM_GIGE_E                             /* = 3650*/ ,
    PRV_CPSS_PM3INT_PORTS_NW6_SUM_HLG_E                              /* = 3651*/ ,
    PRV_CPSS_PM3INT_PORTS_NW6_SUM_XPCS_E                             /* = 3652*/ ,
    PRV_CPSS_PM3INT_PORTS_NW6_SUM_MPCS_E                             /* = 3653*/ ,
    PRV_CPSS_PM3INT_PORTS_NW6_SUM_MMPCS_E                            /* = 3654*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_NW6_SUM)                       /* = 3656-3679*/,

    /*INTERRUPT ID BASE:[3680]*/
    PRV_CPSS_PM3INT_PORTS_NW7_SUM_GENERAL_E                          /* = 3680*/ ,
    PRV_CPSS_PM3INT_PORTS_NW7_SUM_XLG_E                              /* = 3681*/ ,
    PRV_CPSS_PM3INT_PORTS_NW7_SUM_GIGE_E                             /* = 3682*/ ,
    PRV_CPSS_PM3INT_PORTS_NW7_SUM_HLG_E                              /* = 3683*/ ,
    PRV_CPSS_PM3INT_PORTS_NW7_SUM_XPCS_E                             /* = 3684*/ ,
    PRV_CPSS_PM3INT_PORTS_NW7_SUM_MPCS_E                             /* = 3685*/ ,
    PRV_CPSS_PM3INT_PORTS_NW7_SUM_MMPCS_E                            /* = 3686*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_NW7_SUM)                       /* = 3688-3711*/,

    /*INTERRUPT ID BASE:[3712]*/
    PRV_CPSS_PM3INT_PORTS_NW8_SUM_GENERAL_E                          /* = 3712*/ ,
    PRV_CPSS_PM3INT_PORTS_NW8_SUM_XLG_E                              /* = 3713*/ ,
    PRV_CPSS_PM3INT_PORTS_NW8_SUM_GIGE_E                             /* = 3714*/ ,
    PRV_CPSS_PM3INT_PORTS_NW8_SUM_HLG_E                              /* = 3715*/ ,
    PRV_CPSS_PM3INT_PORTS_NW8_SUM_XPCS_E                             /* = 3716*/ ,
    PRV_CPSS_PM3INT_PORTS_NW8_SUM_MPCS_E                             /* = 3717*/ ,
    PRV_CPSS_PM3INT_PORTS_NW8_SUM_MMPCS_E                            /* = 3718*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_NW8_SUM)                       /* = 3720-3743*/,

    /*INTERRUPT ID BASE:[3744]*/
    PRV_CPSS_PM3INT_PORTS_NW9_SUM_GENERAL_E                          /* = 3744*/ ,
    PRV_CPSS_PM3INT_PORTS_NW9_SUM_XLG_E                              /* = 3745*/ ,
    PRV_CPSS_PM3INT_PORTS_NW9_SUM_GIGE_E                             /* = 3746*/ ,
    PRV_CPSS_PM3INT_PORTS_NW9_SUM_HLG_E                              /* = 3747*/ ,
    PRV_CPSS_PM3INT_PORTS_NW9_SUM_XPCS_E                             /* = 3748*/ ,
    PRV_CPSS_PM3INT_PORTS_NW9_SUM_MPCS_E                             /* = 3749*/ ,
    PRV_CPSS_PM3INT_PORTS_NW9_SUM_MMPCS_E                            /* = 3750*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_NW9_SUM)                       /* = 3752-3775*/,

    /*INTERRUPT ID BASE:[3776]*/
    PRV_CPSS_PM3INT_PORTS_NW10_SUM_GENERAL_E                         /* = 3776*/ ,
    PRV_CPSS_PM3INT_PORTS_NW10_SUM_XLG_E                             /* = 3777*/ ,
    PRV_CPSS_PM3INT_PORTS_NW10_SUM_GIGE_E                            /* = 3778*/ ,
    PRV_CPSS_PM3INT_PORTS_NW10_SUM_HLG_E                             /* = 3779*/ ,
    PRV_CPSS_PM3INT_PORTS_NW10_SUM_XPCS_E                            /* = 3780*/ ,
    PRV_CPSS_PM3INT_PORTS_NW10_SUM_MPCS_E                            /* = 3781*/ ,
    PRV_CPSS_PM3INT_PORTS_NW10_SUM_MMPCS_E                           /* = 3782*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_NW10_SUM)                      /* = 3784-3807*/,

    /*INTERRUPT ID BASE:[3808]*/
    PRV_CPSS_PM3INT_PORTS_NW11_SUM_GENERAL_E                         /* = 3808*/ ,
    PRV_CPSS_PM3INT_PORTS_NW11_SUM_XLG_E                             /* = 3809*/ ,
    PRV_CPSS_PM3INT_PORTS_NW11_SUM_GIGE_E                            /* = 3810*/ ,
    PRV_CPSS_PM3INT_PORTS_NW11_SUM_HLG_E                             /* = 3811*/ ,
    PRV_CPSS_PM3INT_PORTS_NW11_SUM_XPCS_E                            /* = 3812*/ ,
    PRV_CPSS_PM3INT_PORTS_NW11_SUM_MPCS_E                            /* = 3813*/ ,
    PRV_CPSS_PM3INT_PORTS_NW11_SUM_MMPCS_E                           /* = 3814*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_NW11_SUM)                      /* = 3816-3839*/,

    /*INTERRUPT ID BASE:[3840]*/
    PRV_CPSS_PM3INT_PORTS_FB0_SUM_GENERAL_E                          /* = 3840*/ ,
    PRV_CPSS_PM3INT_PORTS_FB0_SUM_XLG_E                              /* = 3841*/ ,
    PRV_CPSS_PM3INT_PORTS_FB0_SUM_GIGE_E                             /* = 3842*/ ,
    PRV_CPSS_PM3INT_PORTS_FB0_SUM_HLG_E                              /* = 3843*/ ,
    PRV_CPSS_PM3INT_PORTS_FB0_SUM_XPCS_E                             /* = 3844*/ ,
    PRV_CPSS_PM3INT_PORTS_FB0_SUM_MPCS_E                             /* = 3845*/ ,
    PRV_CPSS_PM3INT_PORTS_FB0_SUM_MMPCS_E                            /* = 3846*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_FB0_SUM)                       /* = 3848-3871*/,

    /*INTERRUPT ID BASE:[3872]*/
    PRV_CPSS_PM3INT_PORTS_FB1_SUM_GENERAL_E                          /* = 3872*/ ,
    PRV_CPSS_PM3INT_PORTS_FB1_SUM_XLG_E                              /* = 3873*/ ,
    PRV_CPSS_PM3INT_PORTS_FB1_SUM_GIGE_E                             /* = 3874*/ ,
    PRV_CPSS_PM3INT_PORTS_FB1_SUM_HLG_E                              /* = 3875*/ ,
    PRV_CPSS_PM3INT_PORTS_FB1_SUM_XPCS_E                             /* = 3876*/ ,
    PRV_CPSS_PM3INT_PORTS_FB1_SUM_MPCS_E                             /* = 3877*/ ,
    PRV_CPSS_PM3INT_PORTS_FB1_SUM_MMPCS_E                            /* = 3878*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_FB1_SUM)                       /* = 3880-3903*/,

    /*INTERRUPT ID BASE:[3904]*/
    PRV_CPSS_PM3INT_PORTS_FB2_SUM_GENERAL_E                          /* = 3904*/ ,
    PRV_CPSS_PM3INT_PORTS_FB2_SUM_XLG_E                              /* = 3905*/ ,
    PRV_CPSS_PM3INT_PORTS_FB2_SUM_GIGE_E                             /* = 3906*/ ,
    PRV_CPSS_PM3INT_PORTS_FB2_SUM_HLG_E                              /* = 3907*/ ,
    PRV_CPSS_PM3INT_PORTS_FB2_SUM_XPCS_E                             /* = 3908*/ ,
    PRV_CPSS_PM3INT_PORTS_FB2_SUM_MPCS_E                             /* = 3909*/ ,
    PRV_CPSS_PM3INT_PORTS_FB2_SUM_MMPCS_E                            /* = 3910*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_FB2_SUM)                       /* = 3912-3935*/,

    /*INTERRUPT ID BASE:[3936]*/
    PRV_CPSS_PM3INT_PORTS_FB3_SUM_GENERAL_E                          /* = 3936*/ ,
    PRV_CPSS_PM3INT_PORTS_FB3_SUM_XLG_E                              /* = 3937*/ ,
    PRV_CPSS_PM3INT_PORTS_FB3_SUM_GIGE_E                             /* = 3938*/ ,
    PRV_CPSS_PM3INT_PORTS_FB3_SUM_HLG_E                              /* = 3939*/ ,
    PRV_CPSS_PM3INT_PORTS_FB3_SUM_XPCS_E                             /* = 3940*/ ,
    PRV_CPSS_PM3INT_PORTS_FB3_SUM_MPCS_E                             /* = 3941*/ ,
    PRV_CPSS_PM3INT_PORTS_FB3_SUM_MMPCS_E                            /* = 3942*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_FB3_SUM)                       /* = 3944-3967*/,

    /*Start GigePortInt - GIGE (three spid) Ports Interrupts address */
    /* NW0[0x01000020], NW1[0x01001020] , NW2[0x01002020], NW3[0x01003020]    */
    /* NW4[0x01004020], NW5[0x01005020] , NW6[0x01006020], NW7[0x01007020]    */
    /* NW8[0x01008020], NW9[0x01009020] , NW10[0x0100A020], NW11[0x0100B020]  */
    /* Tree place: .5.17-28.2(NW0-11) */
    /*INTERRUPT ID BASE:[3968]*/
    PRV_CPSS_PM3INT_PORTS_GIGE_NW0_GENERAL_E                         /* = 3968*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW0_LINK_STATUS_CHANGE_E              /* = 3969*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_GIGE_NW0, 4)                 /* = 3970-3972*/,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW0_ADDRESS_OUT_OF_RANGE_E            /* = 3973*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW0_SYNC_STATUS_CHANGE_E              /* = 3974*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW0_PRBS_ERROR_E                      /* = 3975*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW0_RESERVED8_E                       /* = 3976*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW0_TX_FIFO_UNDERRUN_E                /* = 3977*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_GIGE_NW0)                      /* = 3978-3999*/,

    /*INTERRUPT ID BASE:[4000]*/
    PRV_CPSS_PM3INT_PORTS_GIGE_NW1_GENERAL_E                         /* = 4000*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW1_LINK_STATUS_CHANGE_E              /* = 4001*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_GIGE_NW1, 4)                 /* = 4002-4004*/,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW1_ADDRESS_OUT_OF_RANGE_E            /* = 4005*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW1_SYNC_STATUS_CHANGE_E              /* = 4006*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW1_PRBS_ERROR_E                      /* = 4007*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW1_RESERVED8_E                       /* = 4008*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW1_TX_FIFO_UNDERRUN_E                /* = 4009*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_GIGE_NW1)                      /* = 4010-4031*/,

    /*INTERRUPT ID BASE:[4032]*/
    PRV_CPSS_PM3INT_PORTS_GIGE_NW2_GENERAL_E                         /* = 4032*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW2_LINK_STATUS_CHANGE_E              /* = 4033*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_GIGE_NW2, 4)                 /* = 4034-4036*/,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW2_ADDRESS_OUT_OF_RANGE_E            /* = 4037*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW2_SYNC_STATUS_CHANGE_E              /* = 4038*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW2_PRBS_ERROR_E                      /* = 4039*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW2_RESERVED8_E                       /* = 4040*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW2_TX_FIFO_UNDERRUN_E                /* = 4041*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_GIGE_NW2)                      /* = 4042-4063*/,

    /*INTERRUPT ID BASE:[4064]*/
    PRV_CPSS_PM3INT_PORTS_GIGE_NW3_GENERAL_E                         /* = 4064*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW3_LINK_STATUS_CHANGE_E              /* = 4065*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_GIGE_NW3, 4)                 /* = 4066-4068*/,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW3_ADDRESS_OUT_OF_RANGE_E            /* = 4069*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW3_SYNC_STATUS_CHANGE_E              /* = 4070*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW3_PRBS_ERROR_E                      /* = 4071*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW3_RESERVED8_E                       /* = 4072*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW3_TX_FIFO_UNDERRUN_E                /* = 4073*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_GIGE_NW3)                      /* = 4074-4095*/,

    /*INTERRUPT ID BASE:[4096]*/
    PRV_CPSS_PM3INT_PORTS_GIGE_NW4_GENERAL_E                         /* = 4096*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW4_LINK_STATUS_CHANGE_E              /* = 4097*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_GIGE_NW4, 4)                 /* = 4098-4100*/,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW4_ADDRESS_OUT_OF_RANGE_E            /* = 4101*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW4_SYNC_STATUS_CHANGE_E              /* = 4102*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW4_PRBS_ERROR_E                      /* = 4103*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW4_RESERVED8_E                       /* = 4104*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW4_TX_FIFO_UNDERRUN_E                /* = 4105*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_GIGE_NW4)                      /* = 4106-4127*/,

    /*INTERRUPT ID BASE:[4128]*/
    PRV_CPSS_PM3INT_PORTS_GIGE_NW5_GENERAL_E                         /* = 4128*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW5_LINK_STATUS_CHANGE_E              /* = 4129*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_GIGE_NW5, 4)                 /* = 4130-4132*/,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW5_ADDRESS_OUT_OF_RANGE_E            /* = 4133*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW5_SYNC_STATUS_CHANGE_E              /* = 4134*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW5_PRBS_ERROR_E                      /* = 4135*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW5_RESERVED8_E                       /* = 4136*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW5_TX_FIFO_UNDERRUN_E                /* = 4137*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_GIGE_NW5)                      /* = 4138-4159*/,

    /*INTERRUPT ID BASE:[4160]*/
    PRV_CPSS_PM3INT_PORTS_GIGE_NW6_GENERAL_E                         /* = 4160*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW6_LINK_STATUS_CHANGE_E              /* = 4161*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_GIGE_NW6, 4)                 /* = 4162-4164*/,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW6_ADDRESS_OUT_OF_RANGE_E            /* = 4165*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW6_SYNC_STATUS_CHANGE_E              /* = 4166*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW6_PRBS_ERROR_E                      /* = 4167*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW6_RESERVED8_E                       /* = 4168*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW6_TX_FIFO_UNDERRUN_E                /* = 4169*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_GIGE_NW6)                      /* = 4170-4191*/,

    /*INTERRUPT ID BASE:[4192]*/
    PRV_CPSS_PM3INT_PORTS_GIGE_NW7_GENERAL_E                         /* = 4192*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW7_LINK_STATUS_CHANGE_E              /* = 4193*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_GIGE_NW7, 4)                 /* = 4194-4196*/,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW7_ADDRESS_OUT_OF_RANGE_E            /* = 4197*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW7_SYNC_STATUS_CHANGE_E              /* = 4198*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW7_PRBS_ERROR_E                      /* = 4199*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW7_RESERVED8_E                       /* = 4200*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW7_TX_FIFO_UNDERRUN_E                /* = 4201*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_GIGE_NW7)                      /* = 4202-4223*/,

    /*INTERRUPT ID BASE:[4224]*/
    PRV_CPSS_PM3INT_PORTS_GIGE_NW8_GENERAL_E                         /* = 4224*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW8_LINK_STATUS_CHANGE_E              /* = 4225*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_GIGE_NW8, 4)                 /* = 4226-4228*/,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW8_ADDRESS_OUT_OF_RANGE_E            /* = 4229*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW8_SYNC_STATUS_CHANGE_E              /* = 4230*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW8_PRBS_ERROR_E                      /* = 4231*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW8_RESERVED8_E                       /* = 4232*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW8_TX_FIFO_UNDERRUN_E                /* = 4233*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_GIGE_NW8)                      /* = 4234-4255*/,

    /*INTERRUPT ID BASE:[4256]*/
    PRV_CPSS_PM3INT_PORTS_GIGE_NW9_GENERAL_E                         /* = 4256*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW9_LINK_STATUS_CHANGE_E              /* = 4257*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_GIGE_NW9, 4)                 /* = 4258-4260*/,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW9_ADDRESS_OUT_OF_RANGE_E            /* = 4261*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW9_SYNC_STATUS_CHANGE_E              /* = 4262*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW9_PRBS_ERROR_E                      /* = 4263*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW9_RESERVED8_E                       /* = 4264*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW9_TX_FIFO_UNDERRUN_E                /* = 4265*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_GIGE_NW9)                      /* = 4266-4287*/,

    /*INTERRUPT ID BASE:[4288]*/
    PRV_CPSS_PM3INT_PORTS_GIGE_NW10_GENERAL_E                        /* = 4288*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW10_LINK_STATUS_CHANGE_E             /* = 4289*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_GIGE_NW10, 4)                /* = 4290-4292*/,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW10_ADDRESS_OUT_OF_RANGE_E           /* = 4293*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW10_SYNC_STATUS_CHANGE_E             /* = 4294*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW10_PRBS_ERROR_E                     /* = 4295*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW10_RESERVED8_E                      /* = 4296*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW10_TX_FIFO_UNDERRUN_E               /* = 4297*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_GIGE_NW10)                     /* = 4298-4319*/,

    /*INTERRUPT ID BASE:[4320]*/
    PRV_CPSS_PM3INT_PORTS_GIGE_NW11_GENERAL_E                        /* = 4320*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW11_LINK_STATUS_CHANGE_E             /* = 4321*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_GIGE_NW11, 4)                /* = 4322-4324*/,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW11_ADDRESS_OUT_OF_RANGE_E           /* = 4325*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW11_SYNC_STATUS_CHANGE_E             /* = 4326*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW11_PRBS_ERROR_E                     /* = 4327*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW11_RESERVED8_E                      /* = 4328*/ ,
    PRV_CPSS_PM3INT_PORTS_GIGE_NW11_TX_FIFO_UNDERRUN_E               /* = 4329*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_GIGE_NW11)                     /* = 4330-4351*/,

    /*Start PortPcs40GInt - Ports PCS40G Interrupts address */
    /* NW0[0x010C0408], NW1[0x010C1408] , NW2[0x010C2408], NW3[0x010C3408]    */
    /* NW4[0x010C4408], NW5[0x010C5408] , NW6[0x010C6408], NW7[0x010C7408]    */
    /* NW8[0x010C8408], NW9[0x010C9408] , NW10[0x010CA408], NW11[0x010CB408]  */
    /* FB0_L0[0x010D0A08], FB1_L0[0x010D0C08] , FB2_L0[0x010D0408], FB3_L0[0x010D0808]  */
    /* FB0_L1[0x010D1A08], FB1_L1[0x010D1C08] , FB2_L1[0x010D1408], FB3_L1[0x010D1808]  */
    /* FB0_L2[0x010D2A08], FB1_L2[0x010D2C08] , FB2_L2[0x010D2408], FB3_L2[0x010D2808]  */
    /* FB0_L3[0x010D3A08], FB1_L3[0x010D3C08] , FB2_L3[0x010D3408], FB3_L3[0x010D3808]  */
    /* Tree place: .5.17-28.5(NW0-11) and .5.3.2-5(FB0), .5.7.2-5(FB1), .5.11.2-5(FB2), .5.15.2-5(FB3) */
    /*INTERRUPT ID BASE:[4352]*/
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW0_GENERAL_E                       /* = 4352*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW0_REG_ACCESS_ERR_E                /* = 4353*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW0_ALIGN_LOCK_LOST_E               /* = 4354*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW0_LANE_LOCK_LOST_E                /* = 4355*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW0_RX_GB_ALIGN_FIFO_FULL_E         /* = 4356*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW0_TX_GB_FIFO_FULL_E               /* = 4357*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW0_SIGNAL_DETECT_CHANGE_E          /* = 4358*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW0_AP_INT_SUM_E                    /* = 4359*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW0_SD_INTERRUPT_0_E                /* = 4360*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW0_SD_INTERRUPT_1_E                /* = 4361*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW0_RX_DECODER_ERROR_E              /* = 4362*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW0_PCS_LB_FIFO_EMPTY_E             /* = 4363*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW0_PCS_LB_FIFO_FULL_E              /* = 4364*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW0_TX_GB_FIFO_READ_ERROR_E         /* = 4365*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW0_GB_LOCK_SYNC_CHANGE_E           /* = 4366*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_PCS40G_NW0)                    /* = 4367-4383*/,

    /*INTERRUPT ID BASE:[4384]*/
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW1_GENERAL_E                       /* = 4384*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW1_REG_ACCESS_ERR_E                /* = 4385*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW1_ALIGN_LOCK_LOST_E               /* = 4386*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW1_LANE_LOCK_LOST_E                /* = 4387*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW1_RX_GB_ALIGN_FIFO_FULL_E         /* = 4388*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW1_TX_GB_FIFO_FULL_E               /* = 4389*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW1_SIGNAL_DETECT_CHANGE_E          /* = 4390*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW1_AP_INT_SUM_E                    /* = 4391*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW1_SD_INTERRUPT_0_E                /* = 4392*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW1_SD_INTERRUPT_1_E                /* = 4393*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW1_RX_DECODER_ERROR_E              /* = 4394*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW1_PCS_LB_FIFO_EMPTY_E             /* = 4395*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW1_PCS_LB_FIFO_FULL_E              /* = 4396*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW1_TX_GB_FIFO_READ_ERROR_E         /* = 4397*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW1_GB_LOCK_SYNC_CHANGE_E           /* = 4398*/ , 
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_PCS40G_NW1)                    /* = 4399-4415*/,           


    /*INTERRUPT ID BASE:[4416]*/
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW2_GENERAL_E                       /* = 4416*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW2_REG_ACCESS_ERR_E                /* = 4417*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW2_ALIGN_LOCK_LOST_E               /* = 4418*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW2_LANE_LOCK_LOST_E                /* = 4419*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW2_RX_GB_ALIGN_FIFO_FULL_E         /* = 4420*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW2_TX_GB_FIFO_FULL_E               /* = 4421*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW2_SIGNAL_DETECT_CHANGE_E          /* = 4422*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW2_AP_INT_SUM_E                    /* = 4423*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW2_SD_INTERRUPT_0_E                /* = 4424*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW2_SD_INTERRUPT_1_E                /* = 4425*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW2_RX_DECODER_ERROR_E              /* = 4426*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW2_PCS_LB_FIFO_EMPTY_E             /* = 4427*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW2_PCS_LB_FIFO_FULL_E              /* = 4428*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW2_TX_GB_FIFO_READ_ERROR_E         /* = 4429*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW2_GB_LOCK_SYNC_CHANGE_E           /* = 4430*/ , 
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_PCS40G_NW2)                    /* = 4431-4447*/,      



    /*INTERRUPT ID BASE:[4448]*/
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW3_GENERAL_E                       /* = 4448*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW3_REG_ACCESS_ERR_E                /* = 4449*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW3_ALIGN_LOCK_LOST_E               /* = 4450*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW3_LANE_LOCK_LOST_E                /* = 4451*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW3_RX_GB_ALIGN_FIFO_FULL_E         /* = 4452*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW3_TX_GB_FIFO_FULL_E               /* = 4453*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW3_SIGNAL_DETECT_CHANGE_E          /* = 4454*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW3_AP_INT_SUM_E                    /* = 4455*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW3_SD_INTERRUPT_0_E                /* = 4456*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW3_SD_INTERRUPT_1_E                /* = 4457*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW3_RX_DECODER_ERROR_E              /* = 4458*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW3_PCS_LB_FIFO_EMPTY_E             /* = 4459*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW3_PCS_LB_FIFO_FULL_E              /* = 4460*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW3_TX_GB_FIFO_READ_ERROR_E         /* = 4461*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW3_GB_LOCK_SYNC_CHANGE_E           /* = 4462*/ , 
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_PCS40G_NW3)                    /* = 4463-4479*/,   

    /*INTERRUPT ID BASE:[4480]*/
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW4_GENERAL_E                       /* = 4480*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW4_REG_ACCESS_ERR_E                /* = 4481*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW4_ALIGN_LOCK_LOST_E               /* = 4482*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW4_LANE_LOCK_LOST_E                /* = 4483*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW4_RX_GB_ALIGN_FIFO_FULL_E         /* = 4484*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW4_TX_GB_FIFO_FULL_E               /* = 4485*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW4_SIGNAL_DETECT_CHANGE_E          /* = 4486*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW4_AP_INT_SUM_E                    /* = 4487*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW4_SD_INTERRUPT_0_E                /* = 4488*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW4_SD_INTERRUPT_1_E                /* = 4489*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW4_RX_DECODER_ERROR_E              /* = 4490*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW4_PCS_LB_FIFO_EMPTY_E             /* = 4491*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW4_PCS_LB_FIFO_FULL_E              /* = 4492*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW4_TX_GB_FIFO_READ_ERROR_E         /* = 4493*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW4_GB_LOCK_SYNC_CHANGE_E           /* = 4494*/ , 
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_PCS40G_NW4)                    /* = 4495-4511*/,


    /*INTERRUPT ID BASE:[4512]*/
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW5_GENERAL_E                       /* = 4512*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW5_REG_ACCESS_ERR_E                /* = 4513*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW5_ALIGN_LOCK_LOST_E               /* = 4514*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW5_LANE_LOCK_LOST_E                /* = 4515*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW5_RX_GB_ALIGN_FIFO_FULL_E         /* = 4516*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW5_TX_GB_FIFO_FULL_E               /* = 4517*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW5_SIGNAL_DETECT_CHANGE_E          /* = 4518*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW5_AP_INT_SUM_E                    /* = 4519*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW5_SD_INTERRUPT_0_E                /* = 4520*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW5_SD_INTERRUPT_1_E                /* = 4521*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW5_RX_DECODER_ERROR_E              /* = 4522*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW5_PCS_LB_FIFO_EMPTY_E             /* = 4523*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW5_PCS_LB_FIFO_FULL_E              /* = 4524*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW5_TX_GB_FIFO_READ_ERROR_E         /* = 4525*/ , 
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW5_GB_LOCK_SYNC_CHANGE_E           /* = 4526*/ , 
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_PCS40G_NW5)                    /* = 4527-4543*/,


    /*INTERRUPT ID BASE:[4544]*/
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW6_GENERAL_E                       /* = 4544*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW6_REG_ACCESS_ERR_E                /* = 4545*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW6_ALIGN_LOCK_LOST_E               /* = 4546*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW6_LANE_LOCK_LOST_E                /* = 4547*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW6_RX_GB_ALIGN_FIFO_FULL_E         /* = 4548*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW6_TX_GB_FIFO_FULL_E               /* = 4549*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW6_SIGNAL_DETECT_CHANGE_E          /* = 4550*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW6_AP_INT_SUM_E                    /* = 4551*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW6_SD_INTERRUPT_0_E                /* = 4552*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW6_SD_INTERRUPT_1_E                /* = 4553*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW6_RX_DECODER_ERROR_E              /* = 4554*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW6_PCS_LB_FIFO_EMPTY_E             /* = 4555*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW6_PCS_LB_FIFO_FULL_E              /* = 4556*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW6_TX_GB_FIFO_READ_ERROR_E         /* = 4557*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW6_GB_LOCK_SYNC_CHANGE_E           /* = 4558*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_PCS40G_NW6)                    /* = 4559-4575*/,         


    /*INTERRUPT ID BASE:[4576]*/
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW7_GENERAL_E                       /* = 4576*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW7_REG_ACCESS_ERR_E                /* = 4577*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW7_ALIGN_LOCK_LOST_E               /* = 4578*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW7_LANE_LOCK_LOST_E                /* = 4579*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW7_RX_GB_ALIGN_FIFO_FULL_E         /* = 4580*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW7_TX_GB_FIFO_FULL_E               /* = 4581*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW7_SIGNAL_DETECT_CHANGE_E          /* = 4582*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW7_AP_INT_SUM_E                    /* = 4583*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW7_SD_INTERRUPT_0_E                /* = 4584*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW7_SD_INTERRUPT_1_E                /* = 4585*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW7_RX_DECODER_ERROR_E              /* = 4586*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW7_PCS_LB_FIFO_EMPTY_E             /* = 4587*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW7_PCS_LB_FIFO_FULL_E              /* = 4588*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW7_TX_GB_FIFO_READ_ERROR_E         /* = 4589*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW7_GB_LOCK_SYNC_CHANGE_E           /* = 4590*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_PCS40G_NW7)                    /* = 4591-4607*/,


    /*INTERRUPT ID BASE:[4608]*/
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW8_GENERAL_E                       /* = 4608*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW8_REG_ACCESS_ERR_E                /* = 4609*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW8_ALIGN_LOCK_LOST_E               /* = 4610*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW8_LANE_LOCK_LOST_E                /* = 4611*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW8_RX_GB_ALIGN_FIFO_FULL_E         /* = 4612*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW8_TX_GB_FIFO_FULL_E               /* = 4613*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW8_SIGNAL_DETECT_CHANGE_E          /* = 4614*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW8_AP_INT_SUM_E                    /* = 4615*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW8_SD_INTERRUPT_0_E                /* = 4616*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW8_SD_INTERRUPT_1_E                /* = 4617*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW8_RX_DECODER_ERROR_E              /* = 4618*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW8_PCS_LB_FIFO_EMPTY_E             /* = 4619*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW8_PCS_LB_FIFO_FULL_E              /* = 4620*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW8_TX_GB_FIFO_READ_ERROR_E         /* = 4621*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW8_GB_LOCK_SYNC_CHANGE_E           /* = 4622*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_PCS40G_NW8)                    /* = 4623-4639*/,



    /*INTERRUPT ID BASE:[4640]*/
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW9_GENERAL_E                       /* = 4640*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW9_REG_ACCESS_ERR_E                /* = 4641*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW9_ALIGN_LOCK_LOST_E               /* = 4642*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW9_LANE_LOCK_LOST_E                /* = 4643*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW9_RX_GB_ALIGN_FIFO_FULL_E         /* = 4644*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW9_TX_GB_FIFO_FULL_E               /* = 4645*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW9_SIGNAL_DETECT_CHANGE_E          /* = 4646*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW9_AP_INT_SUM_E                    /* = 4647*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW9_SD_INTERRUPT_0_E                /* = 4648*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW9_SD_INTERRUPT_1_E                /* = 4649*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW9_RX_DECODER_ERROR_E              /* = 4650*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW9_PCS_LB_FIFO_EMPTY_E             /* = 4651*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW9_PCS_LB_FIFO_FULL_E              /* = 4652*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW9_TX_GB_FIFO_READ_ERROR_E         /* = 4653*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW9_GB_LOCK_SYNC_CHANGE_E           /* = 4654*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_PCS40G_NW9)                    /* = 4655-4671*/,


    /*INTERRUPT ID BASE:[4672]*/
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW10_GENERAL_E                       /* = 4672*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW10_REG_ACCESS_ERR_E                /* = 4673*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW10_ALIGN_LOCK_LOST_E               /* = 4674*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW10_LANE_LOCK_LOST_E                /* = 4675*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW10_RX_GB_ALIGN_FIFO_FULL_E         /* = 4676*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW10_TX_GB_FIFO_FULL_E               /* = 4677*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW10_SIGNAL_DETECT_CHANGE_E          /* = 4678*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW10_AP_INT_SUM_E                    /* = 4679*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW10_SD_INTERRUPT_0_E                /* = 4680*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW10_SD_INTERRUPT_1_E                /* = 4681*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW10_RX_DECODER_ERROR_E              /* = 4682*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW10_PCS_LB_FIFO_EMPTY_E             /* = 4683*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW10_PCS_LB_FIFO_FULL_E              /* = 4684*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW10_TX_GB_FIFO_READ_ERROR_E         /* = 4685*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW10_GB_LOCK_SYNC_CHANGE_E           /* = 4686*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_PCS40G_NW10)                    /* = 4687-4703*/,


    /*INTERRUPT ID BASE:[4704]*/
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW11_GENERAL_E                       /* = 4704*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW11_REG_ACCESS_ERR_E                /* = 4705*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW11_ALIGN_LOCK_LOST_E               /* = 4706*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW11_LANE_LOCK_LOST_E                /* = 4707*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW11_RX_GB_ALIGN_FIFO_FULL_E         /* = 4708*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW11_TX_GB_FIFO_FULL_E               /* = 4709*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW11_SIGNAL_DETECT_CHANGE_E          /* = 4710*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW11_AP_INT_SUM_E                    /* = 4711*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW11_SD_INTERRUPT_0_E                /* = 4712*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW11_SD_INTERRUPT_1_E                /* = 4713*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW11_RX_DECODER_ERROR_E              /* = 4714*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW11_PCS_LB_FIFO_EMPTY_E             /* = 4715*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW11_PCS_LB_FIFO_FULL_E              /* = 4716*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW11_TX_GB_FIFO_READ_ERROR_E         /* = 4717*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_NW11_GB_LOCK_SYNC_CHANGE_E           /* = 4718*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_PCS40G_NW11)                    /* = 4719-4735*/,


    /*INTERRUPT ID BASE:[4736]*/
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L0_GENERAL_E                    /* = 4736*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_PCS40G_FB0_L0, 4)            /* = 4737-4740*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L0_REG_ACCESS_ERR_E             /* = 4741*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L0_ALIGN_LOCK_LOST_E            /* = 4742*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L0_LANE_LOCK_LOST_E             /* = 4743*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L0_RX_LOW_GB_FIFO_FULL_E        /* = 4744*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L0_RX_GB_ALIGN_FIFO_FULL_E      /* = 4745*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L0_TX_LOW_GB_FIFO_FULL_E        /* = 4746*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L0_TX_UPPER_GB_FIFO_FULL_E      /* = 4747*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L0_SIGNAL_DETECT_CHANGE_E       /* = 4748*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L0_AP_INT_SUM_E                 /* = 4749*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_PCS40G_FB0_L0)                 /* = 4750-4767*/,

    /*INTERRUPT ID BASE:[4768]*/
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L1_GENERAL_E                    /* = 4768*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_PCS40G_FB0_L1, 4)            /* = 4769-4772*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L1_REG_ACCESS_ERR_E             /* = 4773*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L1_ALIGN_LOCK_LOST_E            /* = 4774*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L1_LANE_LOCK_LOST_E             /* = 4775*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L1_RX_LOW_GB_FIFO_FULL_E        /* = 4776*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L1_RX_GB_ALIGN_FIFO_FULL_E      /* = 4777*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L1_TX_LOW_GB_FIFO_FULL_E        /* = 4778*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L1_TX_UPPER_GB_FIFO_FULL_E      /* = 4779*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L1_SIGNAL_DETECT_CHANGE_E       /* = 4780*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L1_AP_INT_SUM_E                 /* = 4781*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_PCS40G_FB0_L1)                 /* = 4782-4799*/,

    /*INTERRUPT ID BASE:[4800]*/
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L2_GENERAL_E                    /* = 4800*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_PCS40G_FB0_L2, 4)            /* = 4801-4804*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L2_REG_ACCESS_ERR_E             /* = 4805*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L2_ALIGN_LOCK_LOST_E            /* = 4806*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L2_LANE_LOCK_LOST_E             /* = 4807*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L2_RX_LOW_GB_FIFO_FULL_E        /* = 4808*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L2_RX_GB_ALIGN_FIFO_FULL_E      /* = 4809*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L2_TX_LOW_GB_FIFO_FULL_E        /* = 4810*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L2_TX_UPPER_GB_FIFO_FULL_E      /* = 4811*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L2_SIGNAL_DETECT_CHANGE_E       /* = 4812*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L2_AP_INT_SUM_E                 /* = 4813*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_PCS40G_FB0_L2)                 /* = 4814-4831*/,

    /*INTERRUPT ID BASE:[4832]*/
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L3_GENERAL_E                    /* = 4832*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_PCS40G_FB0_L3, 4)            /* = 4833-4836*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L3_REG_ACCESS_ERR_E             /* = 4837*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L3_ALIGN_LOCK_LOST_E            /* = 4838*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L3_LANE_LOCK_LOST_E             /* = 4839*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L3_RX_LOW_GB_FIFO_FULL_E        /* = 4840*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L3_RX_GB_ALIGN_FIFO_FULL_E      /* = 4841*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L3_TX_LOW_GB_FIFO_FULL_E        /* = 4842*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L3_TX_UPPER_GB_FIFO_FULL_E      /* = 4843*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L3_SIGNAL_DETECT_CHANGE_E       /* = 4844*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB0_L3_AP_INT_SUM_E                 /* = 4845*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_PCS40G_FB0_L3)                 /* = 4846-4863*/,

    /*INTERRUPT ID BASE:[4864]*/
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L0_GENERAL_E                    /* = 4864*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_PCS40G_FB1_L0, 4)            /* = 4865-4868*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L0_REG_ACCESS_ERR_E             /* = 4869*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L0_ALIGN_LOCK_LOST_E            /* = 4870*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L0_LANE_LOCK_LOST_E             /* = 4871*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L0_RX_LOW_GB_FIFO_FULL_E        /* = 4872*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L0_RX_GB_ALIGN_FIFO_FULL_E      /* = 4873*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L0_TX_LOW_GB_FIFO_FULL_E        /* = 4874*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L0_TX_UPPER_GB_FIFO_FULL_E      /* = 4875*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L0_SIGNAL_DETECT_CHANGE_E       /* = 4876*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L0_AP_INT_SUM_E                 /* = 4877*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_PCS40G_FB1_L0)                 /* = 4878-4895*/,

    /*INTERRUPT ID BASE:[4896]*/
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L1_GENERAL_E                    /* = 4896*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_PCS40G_FB1_L1, 4)            /* = 4897-4900*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L1_REG_ACCESS_ERR_E             /* = 4901*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L1_ALIGN_LOCK_LOST_E            /* = 4902*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L1_LANE_LOCK_LOST_E             /* = 4903*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L1_RX_LOW_GB_FIFO_FULL_E        /* = 4904*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L1_RX_GB_ALIGN_FIFO_FULL_E      /* = 4905*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L1_TX_LOW_GB_FIFO_FULL_E        /* = 4906*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L1_TX_UPPER_GB_FIFO_FULL_E      /* = 4907*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L1_SIGNAL_DETECT_CHANGE_E       /* = 4908*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L1_AP_INT_SUM_E                 /* = 4909*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_PCS40G_FB1_L1)                 /* = 4910-4927*/,

    /*INTERRUPT ID BASE:[4928]*/
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L2_GENERAL_E                    /* = 4928*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_PCS40G_FB1_L2, 4)            /* = 4929-4932*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L2_REG_ACCESS_ERR_E             /* = 4933*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L2_ALIGN_LOCK_LOST_E            /* = 4934*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L2_LANE_LOCK_LOST_E             /* = 4935*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L2_RX_LOW_GB_FIFO_FULL_E        /* = 4936*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L2_RX_GB_ALIGN_FIFO_FULL_E      /* = 4937*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L2_TX_LOW_GB_FIFO_FULL_E        /* = 4938*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L2_TX_UPPER_GB_FIFO_FULL_E      /* = 4939*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L2_SIGNAL_DETECT_CHANGE_E       /* = 4940*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L2_AP_INT_SUM_E                 /* = 4941*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_PCS40G_FB1_L2)                 /* = 4942-4959*/,

    /*INTERRUPT ID BASE:[4960]*/
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L3_GENERAL_E                    /* = 4960*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_PCS40G_FB1_L3, 4)            /* = 4961-4964*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L3_REG_ACCESS_ERR_E             /* = 4965*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L3_ALIGN_LOCK_LOST_E            /* = 4966*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L3_LANE_LOCK_LOST_E             /* = 4967*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L3_RX_LOW_GB_FIFO_FULL_E        /* = 4968*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L3_RX_GB_ALIGN_FIFO_FULL_E      /* = 4969*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L3_TX_LOW_GB_FIFO_FULL_E        /* = 4970*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L3_TX_UPPER_GB_FIFO_FULL_E      /* = 4971*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L3_SIGNAL_DETECT_CHANGE_E       /* = 4972*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB1_L3_AP_INT_SUM_E                 /* = 4973*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_PCS40G_FB1_L3)                 /* = 4974-4991*/,

    /*INTERRUPT ID BASE:[4992]*/
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L0_GENERAL_E                    /* = 4992*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_PCS40G_FB2_L0, 4)            /* = 4993-4996*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L0_REG_ACCESS_ERR_E             /* = 4997*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L0_ALIGN_LOCK_LOST_E            /* = 4998*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L0_LANE_LOCK_LOST_E             /* = 4999*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L0_RX_LOW_GB_FIFO_FULL_E        /* = 5000*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L0_RX_GB_ALIGN_FIFO_FULL_E      /* = 5001*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L0_TX_LOW_GB_FIFO_FULL_E        /* = 5002*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L0_TX_UPPER_GB_FIFO_FULL_E      /* = 5003*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L0_SIGNAL_DETECT_CHANGE_E       /* = 5004*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L0_AP_INT_SUM_E                 /* = 5005*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_PCS40G_FB2_L0)                 /* = 5006-5023*/,

    /*INTERRUPT ID BASE:[5024]*/
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L1_GENERAL_E                    /* = 5024*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_PCS40G_FB2_L1, 4)            /* = 5025-5028*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L1_REG_ACCESS_ERR_E             /* = 5029*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L1_ALIGN_LOCK_LOST_E            /* = 5030*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L1_LANE_LOCK_LOST_E             /* = 5031*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L1_RX_LOW_GB_FIFO_FULL_E        /* = 5032*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L1_RX_GB_ALIGN_FIFO_FULL_E      /* = 5033*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L1_TX_LOW_GB_FIFO_FULL_E        /* = 5034*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L1_TX_UPPER_GB_FIFO_FULL_E      /* = 5035*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L1_SIGNAL_DETECT_CHANGE_E       /* = 5036*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L1_AP_INT_SUM_E                 /* = 5037*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_PCS40G_FB2_L1)                 /* = 5038-5055*/,

    /*INTERRUPT ID BASE:[5056]*/
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L2_GENERAL_E                    /* = 5056*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_PCS40G_FB2_L2, 4)            /* = 5057-5060*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L2_REG_ACCESS_ERR_E             /* = 5061*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L2_ALIGN_LOCK_LOST_E            /* = 5062*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L2_LANE_LOCK_LOST_E             /* = 5063*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L2_RX_LOW_GB_FIFO_FULL_E        /* = 5064*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L2_RX_GB_ALIGN_FIFO_FULL_E      /* = 5065*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L2_TX_LOW_GB_FIFO_FULL_E        /* = 5066*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L2_TX_UPPER_GB_FIFO_FULL_E      /* = 5067*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L2_SIGNAL_DETECT_CHANGE_E       /* = 5068*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L2_AP_INT_SUM_E                 /* = 5069*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_PCS40G_FB2_L2)                 /* = 5070-5087*/,

    /*INTERRUPT ID BASE:[5088]*/
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L3_GENERAL_E                    /* = 5088*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_PCS40G_FB2_L3, 4)            /* = 5089-5092*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L3_REG_ACCESS_ERR_E             /* = 5093*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L3_ALIGN_LOCK_LOST_E            /* = 5094*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L3_LANE_LOCK_LOST_E             /* = 5095*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L3_RX_LOW_GB_FIFO_FULL_E        /* = 5096*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L3_RX_GB_ALIGN_FIFO_FULL_E      /* = 5097*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L3_TX_LOW_GB_FIFO_FULL_E        /* = 5098*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L3_TX_UPPER_GB_FIFO_FULL_E      /* = 5099*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L3_SIGNAL_DETECT_CHANGE_E       /* = 5100*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB2_L3_AP_INT_SUM_E                 /* = 5101*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_PCS40G_FB2_L3)                 /* = 5102-5119*/,

    /*INTERRUPT ID BASE:[5120]*/
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L0_GENERAL_E                    /* = 5120*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_PCS40G_FB3_L0, 4)            /* = 5121-5124*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L0_REG_ACCESS_ERR_E             /* = 5125*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L0_ALIGN_LOCK_LOST_E            /* = 5126*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L0_LANE_LOCK_LOST_E             /* = 5127*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L0_RX_LOW_GB_FIFO_FULL_E        /* = 5128*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L0_RX_GB_ALIGN_FIFO_FULL_E      /* = 5129*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L0_TX_LOW_GB_FIFO_FULL_E        /* = 5130*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L0_TX_UPPER_GB_FIFO_FULL_E      /* = 5131*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L0_SIGNAL_DETECT_CHANGE_E       /* = 5132*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L0_AP_INT_SUM_E                 /* = 5133*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_PCS40G_FB3_L0)                 /* = 5134-5151*/,

    /*INTERRUPT ID BASE:[5152]*/
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L1_GENERAL_E                    /* = 5152*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_PCS40G_FB3_L1, 4)            /* = 5153-5156*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L1_REG_ACCESS_ERR_E             /* = 5157*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L1_ALIGN_LOCK_LOST_E            /* = 5158*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L1_LANE_LOCK_LOST_E             /* = 5159*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L1_RX_LOW_GB_FIFO_FULL_E        /* = 5160*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L1_RX_GB_ALIGN_FIFO_FULL_E      /* = 5161*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L1_TX_LOW_GB_FIFO_FULL_E        /* = 5162*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L1_TX_UPPER_GB_FIFO_FULL_E      /* = 5163*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L1_SIGNAL_DETECT_CHANGE_E       /* = 5164*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L1_AP_INT_SUM_E                 /* = 5165*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_PCS40G_FB3_L1)                 /* = 5166-5183*/,

    /*INTERRUPT ID BASE:[5184]*/
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L2_GENERAL_E                    /* = 5184*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_PCS40G_FB3_L2, 4)            /* = 5185-5188*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L2_REG_ACCESS_ERR_E             /* = 5189*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L2_ALIGN_LOCK_LOST_E            /* = 5190*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L2_LANE_LOCK_LOST_E             /* = 5191*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L2_RX_LOW_GB_FIFO_FULL_E        /* = 5192*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L2_RX_GB_ALIGN_FIFO_FULL_E      /* = 5193*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L2_TX_LOW_GB_FIFO_FULL_E        /* = 5194*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L2_TX_UPPER_GB_FIFO_FULL_E      /* = 5195*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L2_SIGNAL_DETECT_CHANGE_E       /* = 5196*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L2_AP_INT_SUM_E                 /* = 5197*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_PCS40G_FB3_L2)                 /* = 5198-5215*/,

    /*INTERRUPT ID BASE:[5216]*/
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L3_GENERAL_E                    /* = 5216*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_PCS40G_FB3_L3, 4)            /* = 5217-5220*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L3_REG_ACCESS_ERR_E             /* = 5221*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L3_ALIGN_LOCK_LOST_E            /* = 5222*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L3_LANE_LOCK_LOST_E             /* = 5223*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L3_RX_LOW_GB_FIFO_FULL_E        /* = 5224*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L3_RX_GB_ALIGN_FIFO_FULL_E      /* = 5225*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L3_TX_LOW_GB_FIFO_FULL_E        /* = 5226*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L3_TX_UPPER_GB_FIFO_FULL_E      /* = 5227*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L3_SIGNAL_DETECT_CHANGE_E       /* = 5228*/ ,
    PRV_CPSS_PM3INT_PORTS_PCS40G_FB3_L3_AP_INT_SUM_E                 /* = 5229*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_PCS40G_FB3_L3)                 /* = 5230-5247*/,

    /*Start PortXlgInt - Ports XLG Interrupts address */
    /* NW0[0x010C0014], NW1[0x010C1014] , NW2[0x010C2014], NW3[0x010C3014]    */
    /* NW4[0x010C4014], NW5[0x010C5014] , NW6[0x010C6014], NW7[0x010C7014]    */
    /* NW8[0x010C8014], NW9[0x010C9014] , NW10[0x010CA014], NW11[0x010CB014]  */
    /* FB0[0x010D0014], FB1[0x010D1014] , FB2[0x010D2014], FB3[0x010D3014]  */
    /* Tree place: .5.17-28.1(NW0-11) and .5.3.1(FB0), .5.7.1(FB1), .5.11.1(FB2), .5.15.1(FB3) */
    /*INTERRUPT ID BASE:[5248]*/
    PRV_CPSS_PM3INT_PORTS_XLG_NW0_GENERAL_E                          /* = 5248*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW0_LINK_STATUS_CHANGE_E               /* = 5249*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_NW0, 3)                  /* = 5250-5251*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW0_FC_STATUS_CHANGE_E                 /* = 5252*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW0_UNKNOWN_SEQUENCE_E                 /* = 5253*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW0_FAULT_CHANGE_E                     /* = 5254*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW0_ADDRESS_OUT_OF_RANGE_E             /* = 5255*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW0_RESERVED8_E                        /* = 5256*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW0_NO_BUFFER_PKT_DROP_E               /* = 5257*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_NW0, 11)                 /* = 5258-5259*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW0_PFC_SYNC_FIFO_OVERRUN_E            /* = 5260*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XLG_NW0)                       /* = 5261-5279*/,

    /*INTERRUPT ID BASE:[5280]*/
    PRV_CPSS_PM3INT_PORTS_XLG_NW1_GENERAL_E                          /* = 5280*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW1_LINK_STATUS_CHANGE_E               /* = 5281*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_NW1, 3)                  /* = 5282-5283*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW1_FC_STATUS_CHANGE_E                 /* = 5284*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW1_UNKNOWN_SEQUENCE_E                 /* = 5285*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW1_FAULT_CHANGE_E                     /* = 5286*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW1_ADDRESS_OUT_OF_RANGE_E             /* = 5287*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW1_RESERVED8_E                        /* = 5288*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW1_NO_BUFFER_PKT_DROP_E               /* = 5289*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_NW1, 11)                 /* = 5290-5291*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW1_PFC_SYNC_FIFO_OVERRUN_E            /* = 5292*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XLG_NW1)                       /* = 5293-5311*/,

    /*INTERRUPT ID BASE:[5312]*/
    PRV_CPSS_PM3INT_PORTS_XLG_NW2_GENERAL_E                          /* = 5312*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW2_LINK_STATUS_CHANGE_E               /* = 5313*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_NW2, 3)                  /* = 5314-5315*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW2_FC_STATUS_CHANGE_E                 /* = 5316*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW2_UNKNOWN_SEQUENCE_E                 /* = 5317*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW2_FAULT_CHANGE_E                     /* = 5318*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW2_ADDRESS_OUT_OF_RANGE_E             /* = 5319*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW2_RESERVED8_E                        /* = 5320*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW2_NO_BUFFER_PKT_DROP_E               /* = 5321*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_NW2, 11)                 /* = 5322-5323*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW2_PFC_SYNC_FIFO_OVERRUN_E            /* = 5324*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XLG_NW2)                       /* = 5325-5343*/,

    /*INTERRUPT ID BASE:[5344]*/
    PRV_CPSS_PM3INT_PORTS_XLG_NW3_GENERAL_E                          /* = 5344*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW3_LINK_STATUS_CHANGE_E               /* = 5345*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_NW3, 3)                  /* = 5346-5347*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW3_FC_STATUS_CHANGE_E                 /* = 5348*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW3_UNKNOWN_SEQUENCE_E                 /* = 5349*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW3_FAULT_CHANGE_E                     /* = 5350*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW3_ADDRESS_OUT_OF_RANGE_E             /* = 5351*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW3_RESERVED8_E                        /* = 5352*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW3_NO_BUFFER_PKT_DROP_E               /* = 5353*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_NW3, 11)                 /* = 5354-5355*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW3_PFC_SYNC_FIFO_OVERRUN_E            /* = 5356*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XLG_NW3)                       /* = 5357-5375*/,

    /*INTERRUPT ID BASE:[5376]*/
    PRV_CPSS_PM3INT_PORTS_XLG_NW4_GENERAL_E                          /* = 5376*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW4_LINK_STATUS_CHANGE_E               /* = 5377*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_NW4, 3)                  /* = 5378-5379*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW4_FC_STATUS_CHANGE_E                 /* = 5380*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW4_UNKNOWN_SEQUENCE_E                 /* = 5381*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW4_FAULT_CHANGE_E                     /* = 5382*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW4_ADDRESS_OUT_OF_RANGE_E             /* = 5383*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW4_RESERVED8_E                        /* = 5384*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW4_NO_BUFFER_PKT_DROP_E               /* = 5385*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_NW4, 11)                 /* = 5386-5387*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW4_PFC_SYNC_FIFO_OVERRUN_E            /* = 5388*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XLG_NW4)                       /* = 5389-5407*/,

    /*INTERRUPT ID BASE:[5408]*/
    PRV_CPSS_PM3INT_PORTS_XLG_NW5_GENERAL_E                          /* = 5408*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW5_LINK_STATUS_CHANGE_E               /* = 5409*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_NW5, 3)                  /* = 5410-5411*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW5_FC_STATUS_CHANGE_E                 /* = 5412*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW5_UNKNOWN_SEQUENCE_E                 /* = 5413*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW5_FAULT_CHANGE_E                     /* = 5414*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW5_ADDRESS_OUT_OF_RANGE_E             /* = 5415*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW5_RESERVED8_E                        /* = 5416*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW5_NO_BUFFER_PKT_DROP_E               /* = 5417*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_NW5, 11)                 /* = 5418-5419*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW5_PFC_SYNC_FIFO_OVERRUN_E            /* = 5420*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XLG_NW5)                       /* = 5421-5439*/,

    /*INTERRUPT ID BASE:[5440]*/
    PRV_CPSS_PM3INT_PORTS_XLG_NW6_GENERAL_E                          /* = 5440*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW6_LINK_STATUS_CHANGE_E               /* = 5441*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_NW6, 3)                  /* = 5442-5443*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW6_FC_STATUS_CHANGE_E                 /* = 5444*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW6_UNKNOWN_SEQUENCE_E                 /* = 5445*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW6_FAULT_CHANGE_E                     /* = 5446*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW6_ADDRESS_OUT_OF_RANGE_E             /* = 5447*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW6_RESERVED8_E                        /* = 5448*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW6_NO_BUFFER_PKT_DROP_E               /* = 5449*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_NW6, 11)                 /* = 5450-5451*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW6_PFC_SYNC_FIFO_OVERRUN_E            /* = 5452*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XLG_NW6)                       /* = 5453-5471*/,

    /*INTERRUPT ID BASE:[5472]*/
    PRV_CPSS_PM3INT_PORTS_XLG_NW7_GENERAL_E                          /* = 5472*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW7_LINK_STATUS_CHANGE_E               /* = 5473*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_NW7, 3)                  /* = 5474-5475*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW7_FC_STATUS_CHANGE_E                 /* = 5476*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW7_UNKNOWN_SEQUENCE_E                 /* = 5477*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW7_FAULT_CHANGE_E                     /* = 5478*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW7_ADDRESS_OUT_OF_RANGE_E             /* = 5479*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW7_RESERVED8_E                        /* = 5480*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW7_NO_BUFFER_PKT_DROP_E               /* = 5481*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_NW7, 11)                 /* = 5482-5483*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW7_PFC_SYNC_FIFO_OVERRUN_E            /* = 5484*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XLG_NW7)                       /* = 5485-5503*/,

    /*INTERRUPT ID BASE:[5504]*/
    PRV_CPSS_PM3INT_PORTS_XLG_NW8_GENERAL_E                          /* = 5504*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW8_LINK_STATUS_CHANGE_E               /* = 5505*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_NW8, 3)                  /* = 5506-5507*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW8_FC_STATUS_CHANGE_E                 /* = 5508*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW8_UNKNOWN_SEQUENCE_E                 /* = 5509*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW8_FAULT_CHANGE_E                     /* = 5510*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW8_ADDRESS_OUT_OF_RANGE_E             /* = 5511*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW8_RESERVED8_E                        /* = 5512*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW8_NO_BUFFER_PKT_DROP_E               /* = 5513*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_NW8, 11)                 /* = 5514-5515*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW8_PFC_SYNC_FIFO_OVERRUN_E            /* = 5516*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XLG_NW8)                       /* = 5517-5535*/,

    /*INTERRUPT ID BASE:[5536]*/
    PRV_CPSS_PM3INT_PORTS_XLG_NW9_GENERAL_E                          /* = 5536*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW9_LINK_STATUS_CHANGE_E               /* = 5537*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_NW9, 3)                  /* = 5538-5539*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW9_FC_STATUS_CHANGE_E                 /* = 5540*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW9_UNKNOWN_SEQUENCE_E                 /* = 5541*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW9_FAULT_CHANGE_E                     /* = 5542*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW9_ADDRESS_OUT_OF_RANGE_E             /* = 5543*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW9_RESERVED8_E                        /* = 5544*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW9_NO_BUFFER_PKT_DROP_E               /* = 5545*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_NW9, 11)                 /* = 5546-5547*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW9_PFC_SYNC_FIFO_OVERRUN_E            /* = 5548*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XLG_NW9)                       /* = 5549-5567*/,

    /*INTERRUPT ID BASE:[5568]*/
    PRV_CPSS_PM3INT_PORTS_XLG_NW10_GENERAL_E                         /* = 5568*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW10_LINK_STATUS_CHANGE_E              /* = 5569*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_NW10, 3)                 /* = 5570-5571*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW10_FC_STATUS_CHANGE_E                /* = 5572*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW10_UNKNOWN_SEQUENCE_E                /* = 5573*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW10_FAULT_CHANGE_E                    /* = 5574*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW10_ADDRESS_OUT_OF_RANGE_E            /* = 5575*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW10_RESERVED8_E                       /* = 5576*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW10_NO_BUFFER_PKT_DROP_E              /* = 5577*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_NW10, 11)                /* = 5578-5579*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW10_PFC_SYNC_FIFO_OVERRUN_E           /* = 5580*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XLG_NW10)                      /* = 5581-5599*/,

    /*INTERRUPT ID BASE:[5600]*/
    PRV_CPSS_PM3INT_PORTS_XLG_NW11_GENERAL_E                         /* = 5600*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW11_LINK_STATUS_CHANGE_E              /* = 5601*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_NW11, 3)                 /* = 5602-5603*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW11_FC_STATUS_CHANGE_E                /* = 5604*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW11_UNKNOWN_SEQUENCE_E                /* = 5605*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW11_FAULT_CHANGE_E                    /* = 5606*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW11_ADDRESS_OUT_OF_RANGE_E            /* = 5607*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW11_RESERVED8_E                       /* = 5608*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW11_NO_BUFFER_PKT_DROP_E              /* = 5609*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_NW11, 11)                /* = 5610-5611*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_NW11_PFC_SYNC_FIFO_OVERRUN_E           /* = 5612*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XLG_NW11)                      /* = 5613-5631*/,

    /*INTERRUPT ID BASE:[5632]*/
    PRV_CPSS_PM3INT_PORTS_XLG_FB0_GENERAL_E                          /* = 5632*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB0_LINK_STATUS_CHANGE_E               /* = 5633*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_FB0, 3)                  /* = 5634-5635*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB0_FC_STATUS_CHANGE_E                 /* = 5636*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB0_UNKNOWN_SEQUENCE_E                 /* = 5637*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB0_FAULT_CHANGE_E                     /* = 5638*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB0_ADDRESS_OUT_OF_RANGE_E             /* = 5639*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB0_RESERVED8_E                        /* = 5640*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB0_NO_BUFFER_PKT_DROP_E               /* = 5641*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_FB0, 11)                 /* = 5642-5643*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB0_PFC_SYNC_FIFO_OVERRUN_E            /* = 5644*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XLG_FB0)                       /* = 5645-5663*/,

    /*INTERRUPT ID BASE:[5664]*/
    PRV_CPSS_PM3INT_PORTS_XLG_FB1_GENERAL_E                          /* = 5664*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB1_LINK_STATUS_CHANGE_E               /* = 5665*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_FB1, 3)                  /* = 5666-5667*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB1_FC_STATUS_CHANGE_E                 /* = 5668*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB1_UNKNOWN_SEQUENCE_E                 /* = 5669*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB1_FAULT_CHANGE_E                     /* = 5670*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB1_ADDRESS_OUT_OF_RANGE_E             /* = 5671*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB1_RESERVED8_E                        /* = 5672*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB1_NO_BUFFER_PKT_DROP_E               /* = 5673*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_FB1, 11)                 /* = 5674-5675*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB1_PFC_SYNC_FIFO_OVERRUN_E            /* = 5676*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XLG_FB1)                       /* = 5677-5695*/,

    /*INTERRUPT ID BASE:[5696]*/
    PRV_CPSS_PM3INT_PORTS_XLG_FB2_GENERAL_E                          /* = 5696*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB2_LINK_STATUS_CHANGE_E               /* = 5697*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_FB2, 3)                  /* = 5698-5699*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB2_FC_STATUS_CHANGE_E                 /* = 5700*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB2_UNKNOWN_SEQUENCE_E                 /* = 5701*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB2_FAULT_CHANGE_E                     /* = 5702*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB2_ADDRESS_OUT_OF_RANGE_E             /* = 5703*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB2_RESERVED8_E                        /* = 5704*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB2_NO_BUFFER_PKT_DROP_E               /* = 5705*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_FB2, 11)                 /* = 5706-5707*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB2_PFC_SYNC_FIFO_OVERRUN_E            /* = 5708*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XLG_FB2)                       /* = 5709-5727*/,

    /*INTERRUPT ID BASE:[5728]*/
    PRV_CPSS_PM3INT_PORTS_XLG_FB3_GENERAL_E                          /* = 5728*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB3_LINK_STATUS_CHANGE_E               /* = 5729*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_FB3, 3)                  /* = 5730-5731*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB3_FC_STATUS_CHANGE_E                 /* = 5732*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB3_UNKNOWN_SEQUENCE_E                 /* = 5733*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB3_FAULT_CHANGE_E                     /* = 5734*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB3_ADDRESS_OUT_OF_RANGE_E             /* = 5735*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB3_RESERVED8_E                        /* = 5736*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB3_NO_BUFFER_PKT_DROP_E               /* = 5737*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(PORTS_XLG_FB3, 11)                 /* = 5738-5739*/ ,
    PRV_CPSS_PM3INT_PORTS_XLG_FB3_PFC_SYNC_FIFO_OVERRUN_E            /* = 5740*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XLG_FB3)                       /* = 5741-5759*/,

    /*Start PortXpcsGlobalInt - Ports XPCS Global Interrupts address */
    /* NW0[0x011C0214], NW2[0x011C2214], NW4[0x011C4214], NW6[0x011C6214]    */
    /* NW8[0x011C8214], NW10[0x011CA214]                                     */
    /* Tree place: .5.<17,19,21,23,25,27>.4(NW 0,2,4,6,8,10) */
    /*INTERRUPT ID BASE:[5760]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_NW0_GENERAL_E                         /* = 5760*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW0_LINK_STATUS_CHANGE_E              /* = 5761*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW0_DESKEW_TIMEOUT_E                  /* = 5762*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW0_DETECTED_COLUMN_IIAII_E           /* = 5763*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW0_DESKEW_ERROR_E                    /* = 5764*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW0_PPM_FIFO_UNDERRUN_E               /* = 5765*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW0_PPM_FIFO_OVERRUN_E                /* = 5766*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW0_LANE0_SUM_E                       /* = 5767*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW0_LANE1_SUM_E                       /* = 5768*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW0_LANE2_SUM_E                       /* = 5769*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW0_LANE3_SUM_E                       /* = 5770*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_NW0)                      /* = 5771-5791*/,

    /*INTERRUPT ID BASE:[5792]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_NW2_GENERAL_E                         /* = 5792*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW2_LINK_STATUS_CHANGE_E              /* = 5793*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW2_DESKEW_TIMEOUT_E                  /* = 5794*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW2_DETECTED_COLUMN_IIAII_E           /* = 5795*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW2_DESKEW_ERROR_E                    /* = 5796*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW2_PPM_FIFO_UNDERRUN_E               /* = 5797*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW2_PPM_FIFO_OVERRUN_E                /* = 5798*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW2_LANE0_SUM_E                       /* = 5799*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW2_LANE1_SUM_E                       /* = 5800*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW2_LANE2_SUM_E                       /* = 5801*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW2_LANE3_SUM_E                       /* = 5802*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_NW2)                      /* = 5803-5823*/,

    /*INTERRUPT ID BASE:[5824]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_NW4_GENERAL_E                         /* = 5824*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW4_LINK_STATUS_CHANGE_E              /* = 5825*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW4_DESKEW_TIMEOUT_E                  /* = 5826*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW4_DETECTED_COLUMN_IIAII_E           /* = 5827*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW4_DESKEW_ERROR_E                    /* = 5828*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW4_PPM_FIFO_UNDERRUN_E               /* = 5829*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW4_PPM_FIFO_OVERRUN_E                /* = 5830*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW4_LANE0_SUM_E                       /* = 5831*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW4_LANE1_SUM_E                       /* = 5832*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW4_LANE2_SUM_E                       /* = 5833*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW4_LANE3_SUM_E                       /* = 5834*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_NW4)                      /* = 5835-5855*/,

    /*INTERRUPT ID BASE:[5856]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_NW6_GENERAL_E                         /* = 5856*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW6_LINK_STATUS_CHANGE_E              /* = 5857*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW6_DESKEW_TIMEOUT_E                  /* = 5858*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW6_DETECTED_COLUMN_IIAII_E           /* = 5859*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW6_DESKEW_ERROR_E                    /* = 5860*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW6_PPM_FIFO_UNDERRUN_E               /* = 5861*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW6_PPM_FIFO_OVERRUN_E                /* = 5862*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW6_LANE0_SUM_E                       /* = 5863*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW6_LANE1_SUM_E                       /* = 5864*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW6_LANE2_SUM_E                       /* = 5865*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW6_LANE3_SUM_E                       /* = 5866*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_NW6)                      /* = 5867-5887*/,

    /*INTERRUPT ID BASE:[5888]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_NW8_GENERAL_E                         /* = 5888*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW8_LINK_STATUS_CHANGE_E              /* = 5889*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW8_DESKEW_TIMEOUT_E                  /* = 5890*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW8_DETECTED_COLUMN_IIAII_E           /* = 5891*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW8_DESKEW_ERROR_E                    /* = 5892*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW8_PPM_FIFO_UNDERRUN_E               /* = 5893*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW8_PPM_FIFO_OVERRUN_E                /* = 5894*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW8_LANE0_SUM_E                       /* = 5895*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW8_LANE1_SUM_E                       /* = 5896*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW8_LANE2_SUM_E                       /* = 5897*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW8_LANE3_SUM_E                       /* = 5898*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_NW8)                      /* = 5899-5919*/,

    /*INTERRUPT ID BASE:[5920]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_NW10_GENERAL_E                        /* = 5920*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW10_LINK_STATUS_CHANGE_E             /* = 5921*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW10_DESKEW_TIMEOUT_E                 /* = 5922*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW10_DETECTED_COLUMN_IIAII_E          /* = 5923*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW10_DESKEW_ERROR_E                   /* = 5924*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW10_PPM_FIFO_UNDERRUN_E              /* = 5925*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW10_PPM_FIFO_OVERRUN_E               /* = 5926*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW10_LANE0_SUM_E                      /* = 5927*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW10_LANE1_SUM_E                      /* = 5928*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW10_LANE2_SUM_E                      /* = 5929*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_NW10_LANE3_SUM_E                      /* = 5930*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_NW10)                     /* = 5931-5951*/,

    /*Start PortXpcsLaneInt - Ports XPCS Lane Interrupts address */
    /* NW0_L0[0x011C0260],  NW0_L1[0x011C02A4], NW0_L2[0x011C02E8], NW0_L3[0x011C032C], */
    /* NW2_L0[0x011C2260],  NW2_L1[0x011C22A4], NW2_L2[0x011C22E8], NW2_L3[0x011C232C], */
    /* NW4_L0[0x011C4260],  NW4_L1[0x011C42A4], NW4_L2[0x011C42E8], NW4_L3[0x011C432C], */
    /* NW6_L0[0x011C6260],  NW6_L1[0x011C62A4], NW6_L2[0x011C62E8], NW6_L3[0x011C632C], */
    /* NW8_L0[0x011C8260],  NW8_L1[0x011C82A4], NW8_L2[0x011C82E8], NW8_L3[0x011C832C], */
    /* NW10_L0[0x011CA260],  NW10_L1[0x011CA2A4], NW10_L2[0x011CA2E8], NW10_L3[0x011CA32C], */
    /* Tree place: .5.<17,19,21,23,25,27>.4.7-12(NW 0,2,4,6,8,10.L0-3) */
    /*INTERRUPT ID BASE:[5952]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L0_GENERAL_E                 /* = 5952*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L0_PRBS_ERROR_E              /* = 5953*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L0_DISPARITY_ERROR_E         /* = 5954*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L0_SYMBOL_ERROR_E            /* = 5955*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L0_CJR_PAT_ERROR_E           /* = 5956*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L0_SYGNAL_DETECT_CHANGED_E   /* = 5957*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L0_SYNC_STATUS_CHANGED_E     /* = 5958*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L0_DETECTED_IIAII_E          /* = 5959*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_LANE_NW0_L0)              /* = 5960-5983*/,

    /*INTERRUPT ID BASE:[5984]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L1_GENERAL_E                 /* = 5984*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L1_PRBS_ERROR_E              /* = 5985*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L1_DISPARITY_ERROR_E         /* = 5986*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L1_SYMBOL_ERROR_E            /* = 5987*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L1_CJR_PAT_ERROR_E           /* = 5988*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L1_SYGNAL_DETECT_CHANGED_E   /* = 5989*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L1_SYNC_STATUS_CHANGED_E     /* = 5990*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L1_DETECTED_IIAII_E          /* = 5991*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_LANE_NW0_L1)              /* = 5992-6015*/,

    /*INTERRUPT ID BASE:[6016]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L2_GENERAL_E                 /* = 6016*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L2_PRBS_ERROR_E              /* = 6017*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L2_DISPARITY_ERROR_E         /* = 6018*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L2_SYMBOL_ERROR_E            /* = 6019*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L2_CJR_PAT_ERROR_E           /* = 6020*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L2_SYGNAL_DETECT_CHANGED_E   /* = 6021*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L2_SYNC_STATUS_CHANGED_E     /* = 6022*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L2_DETECTED_IIAII_E          /* = 6023*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_LANE_NW0_L2)              /* = 6024-6047*/,

    /*INTERRUPT ID BASE:[6048]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L3_GENERAL_E                 /* = 6048*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L3_PRBS_ERROR_E              /* = 6049*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L3_DISPARITY_ERROR_E         /* = 6050*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L3_SYMBOL_ERROR_E            /* = 6051*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L3_CJR_PAT_ERROR_E           /* = 6052*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L3_SYGNAL_DETECT_CHANGED_E   /* = 6053*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L3_SYNC_STATUS_CHANGED_E     /* = 6054*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW0_L3_DETECTED_IIAII_E          /* = 6055*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_LANE_NW0_L3)              /* = 6056-6079*/,

    /*INTERRUPT ID BASE:[6080]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L0_GENERAL_E                 /* = 6080*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L0_PRBS_ERROR_E              /* = 6081*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L0_DISPARITY_ERROR_E         /* = 6082*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L0_SYMBOL_ERROR_E            /* = 6083*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L0_CJR_PAT_ERROR_E           /* = 6084*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L0_SYGNAL_DETECT_CHANGED_E   /* = 6085*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L0_SYNC_STATUS_CHANGED_E     /* = 6086*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L0_DETECTED_IIAII_E          /* = 6087*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_LANE_NW2_L0)              /* = 6088-6111*/,

    /*INTERRUPT ID BASE:[6112]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L1_GENERAL_E                 /* = 6112*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L1_PRBS_ERROR_E              /* = 6113*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L1_DISPARITY_ERROR_E         /* = 6114*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L1_SYMBOL_ERROR_E            /* = 6115*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L1_CJR_PAT_ERROR_E           /* = 6116*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L1_SYGNAL_DETECT_CHANGED_E   /* = 6117*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L1_SYNC_STATUS_CHANGED_E     /* = 6118*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L1_DETECTED_IIAII_E          /* = 6119*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_LANE_NW2_L1)              /* = 6120-6143*/,

    /*INTERRUPT ID BASE:[6144]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L2_GENERAL_E                 /* = 6144*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L2_PRBS_ERROR_E              /* = 6145*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L2_DISPARITY_ERROR_E         /* = 6146*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L2_SYMBOL_ERROR_E            /* = 6147*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L2_CJR_PAT_ERROR_E           /* = 6148*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L2_SYGNAL_DETECT_CHANGED_E   /* = 6149*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L2_SYNC_STATUS_CHANGED_E     /* = 6150*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L2_DETECTED_IIAII_E          /* = 6151*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_LANE_NW2_L2)              /* = 6152-6175*/,

    /*INTERRUPT ID BASE:[6176]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L3_GENERAL_E                 /* = 6176*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L3_PRBS_ERROR_E              /* = 6177*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L3_DISPARITY_ERROR_E         /* = 6178*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L3_SYMBOL_ERROR_E            /* = 6179*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L3_CJR_PAT_ERROR_E           /* = 6180*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L3_SYGNAL_DETECT_CHANGED_E   /* = 6181*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L3_SYNC_STATUS_CHANGED_E     /* = 6182*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW2_L3_DETECTED_IIAII_E          /* = 6183*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_LANE_NW2_L3)              /* = 6184-6207*/,

    /*INTERRUPT ID BASE:[6208]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L0_GENERAL_E                 /* = 6208*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L0_PRBS_ERROR_E              /* = 6209*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L0_DISPARITY_ERROR_E         /* = 6210*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L0_SYMBOL_ERROR_E            /* = 6211*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L0_CJR_PAT_ERROR_E           /* = 6212*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L0_SYGNAL_DETECT_CHANGED_E   /* = 6213*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L0_SYNC_STATUS_CHANGED_E     /* = 6214*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L0_DETECTED_IIAII_E          /* = 6215*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_LANE_NW4_L0)              /* = 6216-6239*/,

    /*INTERRUPT ID BASE:[6240]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L1_GENERAL_E                 /* = 6240*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L1_PRBS_ERROR_E              /* = 6241*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L1_DISPARITY_ERROR_E         /* = 6242*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L1_SYMBOL_ERROR_E            /* = 6243*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L1_CJR_PAT_ERROR_E           /* = 6244*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L1_SYGNAL_DETECT_CHANGED_E   /* = 6245*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L1_SYNC_STATUS_CHANGED_E     /* = 6246*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L1_DETECTED_IIAII_E          /* = 6247*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_LANE_NW4_L1)              /* = 6248-6271*/,

    /*INTERRUPT ID BASE:[6272]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L2_GENERAL_E                 /* = 6272*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L2_PRBS_ERROR_E              /* = 6273*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L2_DISPARITY_ERROR_E         /* = 6274*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L2_SYMBOL_ERROR_E            /* = 6275*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L2_CJR_PAT_ERROR_E           /* = 6276*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L2_SYGNAL_DETECT_CHANGED_E   /* = 6277*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L2_SYNC_STATUS_CHANGED_E     /* = 6278*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L2_DETECTED_IIAII_E          /* = 6279*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_LANE_NW4_L2)              /* = 6280-6303*/,

    /*INTERRUPT ID BASE:[6304]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L3_GENERAL_E                 /* = 6304*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L3_PRBS_ERROR_E              /* = 6305*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L3_DISPARITY_ERROR_E         /* = 6306*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L3_SYMBOL_ERROR_E            /* = 6307*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L3_CJR_PAT_ERROR_E           /* = 6308*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L3_SYGNAL_DETECT_CHANGED_E   /* = 6309*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L3_SYNC_STATUS_CHANGED_E     /* = 6310*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW4_L3_DETECTED_IIAII_E          /* = 6311*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_LANE_NW4_L3)              /* = 6312-6335*/,

    /*INTERRUPT ID BASE:[6336]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L0_GENERAL_E                 /* = 6336*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L0_PRBS_ERROR_E              /* = 6337*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L0_DISPARITY_ERROR_E         /* = 6338*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L0_SYMBOL_ERROR_E            /* = 6339*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L0_CJR_PAT_ERROR_E           /* = 6340*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L0_SYGNAL_DETECT_CHANGED_E   /* = 6341*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L0_SYNC_STATUS_CHANGED_E     /* = 6342*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L0_DETECTED_IIAII_E          /* = 6343*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_LANE_NW6_L0)              /* = 6344-6367*/,

    /*INTERRUPT ID BASE:[6368]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L1_GENERAL_E                 /* = 6368*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L1_PRBS_ERROR_E              /* = 6369*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L1_DISPARITY_ERROR_E         /* = 6370*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L1_SYMBOL_ERROR_E            /* = 6371*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L1_CJR_PAT_ERROR_E           /* = 6372*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L1_SYGNAL_DETECT_CHANGED_E   /* = 6373*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L1_SYNC_STATUS_CHANGED_E     /* = 6374*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L1_DETECTED_IIAII_E          /* = 6375*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_LANE_NW6_L1)              /* = 6376-6399*/,

    /*INTERRUPT ID BASE:[6400]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L2_GENERAL_E                 /* = 6400*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L2_PRBS_ERROR_E              /* = 6401*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L2_DISPARITY_ERROR_E         /* = 6402*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L2_SYMBOL_ERROR_E            /* = 6403*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L2_CJR_PAT_ERROR_E           /* = 6404*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L2_SYGNAL_DETECT_CHANGED_E   /* = 6405*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L2_SYNC_STATUS_CHANGED_E     /* = 6406*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L2_DETECTED_IIAII_E          /* = 6407*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_LANE_NW6_L2)              /* = 6408-6431*/,

    /*INTERRUPT ID BASE:[6432]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L3_GENERAL_E                 /* = 6432*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L3_PRBS_ERROR_E              /* = 6433*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L3_DISPARITY_ERROR_E         /* = 6434*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L3_SYMBOL_ERROR_E            /* = 6435*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L3_CJR_PAT_ERROR_E           /* = 6436*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L3_SYGNAL_DETECT_CHANGED_E   /* = 6437*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L3_SYNC_STATUS_CHANGED_E     /* = 6438*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW6_L3_DETECTED_IIAII_E          /* = 6439*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_LANE_NW6_L3)              /* = 6440-6463*/,

    /*INTERRUPT ID BASE:[6464]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L0_GENERAL_E                 /* = 6464*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L0_PRBS_ERROR_E              /* = 6465*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L0_DISPARITY_ERROR_E         /* = 6466*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L0_SYMBOL_ERROR_E            /* = 6467*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L0_CJR_PAT_ERROR_E           /* = 6468*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L0_SYGNAL_DETECT_CHANGED_E   /* = 6469*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L0_SYNC_STATUS_CHANGED_E     /* = 6470*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L0_DETECTED_IIAII_E          /* = 6471*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_LANE_NW8_L0)              /* = 6472-6495*/,

    /*INTERRUPT ID BASE:[6496]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L1_GENERAL_E                 /* = 6496*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L1_PRBS_ERROR_E              /* = 6497*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L1_DISPARITY_ERROR_E         /* = 6498*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L1_SYMBOL_ERROR_E            /* = 6499*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L1_CJR_PAT_ERROR_E           /* = 6500*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L1_SYGNAL_DETECT_CHANGED_E   /* = 6501*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L1_SYNC_STATUS_CHANGED_E     /* = 6502*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L1_DETECTED_IIAII_E          /* = 6503*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_LANE_NW8_L1)              /* = 6504-6527*/,

    /*INTERRUPT ID BASE:[6528]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L2_GENERAL_E                 /* = 6528*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L2_PRBS_ERROR_E              /* = 6529*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L2_DISPARITY_ERROR_E         /* = 6530*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L2_SYMBOL_ERROR_E            /* = 6531*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L2_CJR_PAT_ERROR_E           /* = 6532*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L2_SYGNAL_DETECT_CHANGED_E   /* = 6533*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L2_SYNC_STATUS_CHANGED_E     /* = 6534*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L2_DETECTED_IIAII_E          /* = 6535*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_LANE_NW8_L2)              /* = 6536-6559*/,

    /*INTERRUPT ID BASE:[6560]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L3_GENERAL_E                 /* = 6560*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L3_PRBS_ERROR_E              /* = 6561*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L3_DISPARITY_ERROR_E         /* = 6562*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L3_SYMBOL_ERROR_E            /* = 6563*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L3_CJR_PAT_ERROR_E           /* = 6564*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L3_SYGNAL_DETECT_CHANGED_E   /* = 6565*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L3_SYNC_STATUS_CHANGED_E     /* = 6566*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW8_L3_DETECTED_IIAII_E          /* = 6567*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_LANE_NW8_L3)              /* = 6568-6591*/,

    /*INTERRUPT ID BASE:[6592]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L0_GENERAL_E                /* = 6592*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L0_PRBS_ERROR_E             /* = 6593*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L0_DISPARITY_ERROR_E        /* = 6594*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L0_SYMBOL_ERROR_E           /* = 6595*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L0_CJR_PAT_ERROR_E          /* = 6596*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L0_SYGNAL_DETECT_CHANGED_E  /* = 6597*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L0_SYNC_STATUS_CHANGED_E    /* = 6598*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L0_DETECTED_IIAII_E         /* = 6599*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_LANE_NW10_L0)             /* = 6600-6623*/,

    /*INTERRUPT ID BASE:[6624]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L1_GENERAL_E                /* = 6624*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L1_PRBS_ERROR_E             /* = 6625*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L1_DISPARITY_ERROR_E        /* = 6626*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L1_SYMBOL_ERROR_E           /* = 6627*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L1_CJR_PAT_ERROR_E          /* = 6628*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L1_SYGNAL_DETECT_CHANGED_E  /* = 6629*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L1_SYNC_STATUS_CHANGED_E    /* = 6630*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L1_DETECTED_IIAII_E         /* = 6631*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_LANE_NW10_L1)             /* = 6632-6655*/,

    /*INTERRUPT ID BASE:[6656]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L2_GENERAL_E                /* = 6656*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L2_PRBS_ERROR_E             /* = 6657*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L2_DISPARITY_ERROR_E        /* = 6658*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L2_SYMBOL_ERROR_E           /* = 6659*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L2_CJR_PAT_ERROR_E          /* = 6660*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L2_SYGNAL_DETECT_CHANGED_E  /* = 6661*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L2_SYNC_STATUS_CHANGED_E    /* = 6662*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L2_DETECTED_IIAII_E         /* = 6663*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_LANE_NW10_L2)             /* = 6664-6687*/,

    /*INTERRUPT ID BASE:[6688]*/
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L3_GENERAL_E                /* = 6688*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L3_PRBS_ERROR_E             /* = 6689*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L3_DISPARITY_ERROR_E        /* = 6690*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L3_SYMBOL_ERROR_E           /* = 6691*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L3_CJR_PAT_ERROR_E          /* = 6692*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L3_SYGNAL_DETECT_CHANGED_E  /* = 6693*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L3_SYNC_STATUS_CHANGED_E    /* = 6694*/ ,
    PRV_CPSS_PM3INT_PORTS_XPCS_LANE_NW10_L3_DETECTED_IIAII_E         /* = 6695*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(PORTS_XPCS_LANE_NW10_L3)             /* = 6696-6719*/,

    /*Start InterlakenInt - Interlaken Interrupt Cause address[0x01F00870]*/
    /* Tree place: .5.29*/
    /*INTERRUPT ID BASE:[6720]*/
    PRV_CPSS_PM3INT_INTERLAKEN_GENERAL_E                             /* = 6720*/ ,
    PRV_CPSS_PM3INT_INTERLAKEN_ILKN_SUM_E                            /* = 6721*/ ,
    PRV_CPSS_PM3INT_INTERLAKEN_RX_FIFO_FULL_E                        /* = 6722*/ ,
    PRV_CPSS_PM3INT_INTERLAKEN_RX_OVERSIZE_PKT_DROP_E                /* = 6723*/ ,
    PRV_CPSS_PM3INT_INTERLAKEN_RX_UNDERSIZE_PKT_DROP_E               /* = 6724*/ ,
    PRV_CPSS_PM3INT_INTERLAKEN_SER_ERROR_E                           /* = 6725*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(INTERLAKEN)                          /* = 6726-6751*/,

    /*Start DfxIntSum - DFX Interrupt Summary Cause address[0x000000AC]*/
    /* Tree place: .6*/
    /*temporary all bits reserved*/
    /*INTERRUPT ID BASE:[6752]*/
    PRV_CPSS_PM3INT_DFX_SUM_GENERAL_E                                /* = 6752*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(DFX_SUM)                             /* = 6753-6783*/,

    /*Start MgIntSum - MG Interrupt Summary Cause address[0x00000038]*/
    /* Tree place: .7*/
    /*INTERRUPT ID BASE:[6784]*/
    PRV_CPSS_PM3INT_MG_GENERAL_E                                     /* = 6784*/ ,
    PRV_CPSS_PM3INT_MG_TWSI_TIMEOUT_E                                /* = 6785*/ ,
    PRV_CPSS_PM3INT_MG_TWSI_STATUS_E                                 /* = 6786*/ ,
    PRV_CPSS_PM3INT_MG_RESERVED3_E                                   /* = 6787*/ ,
    PRV_CPSS_PM3INT_MG_RX_FIFO_OVERRUN_E                             /* = 6788*/ ,
    PRV_CPSS_PM3INT_MG_READ_TIMEOUT_E                                /* = 6789*/ ,
    PRV_CPSS_PM3INT_MG_RESERVED6_E                                   /* = 6790*/ ,
    PRV_CPSS_PM3INT_MG_AUQ_FIFO_OVERRUN_E                            /* = 6791*/ ,
    PRV_CPSS_PM3INT_MG_RESERVED8_E                                   /* = 6792*/ ,
    PRV_CPSS_PM3INT_MG_AU_QUEUE_FULL_E                               /* = 6793*/ ,
    PRV_CPSS_PM3INT_MG_AU_QUEUE_PENDING_E                            /* = 6794*/ ,
    PRV_CPSS_PM3INT_MG_FU_QUEUE_FULL_E                               /* = 6795*/ ,
    PRV_CPSS_PM3INT_MG_FU_QUEUE_PENDING_E                            /* = 6796*/ ,
    PRV_CPSS_PM3INT_MG_GENXS_READ_DMA_DONE_E                         /* = 6797*/ ,
    PRV_CPSS_PM3INT_MG_RESERVED14_E                                  /* = 6798*/ ,
    PRV_CPSS_PM3INT_MG_PEX_ADDR_UNMAPPED_E                           /* = 6799*/ ,
    PRV_CPSS_PM3INT_MG_GPP0_E                                        /* = 6800*/ ,
    PRV_CPSS_PM3INT_MG_GPP1_E                                        /* = 6801*/ ,
    PRV_CPSS_PM3INT_MG_GPP2_E                                        /* = 6802*/ ,
    PRV_CPSS_PM3INT_MG_GPP3_E                                        /* = 6803*/ ,
    PRV_CPSS_PM3INT_MG_GPP4_E                                        /* = 6804*/ ,
    PRV_CPSS_PM3INT_MG_GPP5_E                                        /* = 6805*/ ,
    PRV_CPSS_PM3INT_MG_GPP6_E                                        /* = 6806*/ ,
    PRV_CPSS_PM3INT_MG_GPP7_E                                        /* = 6807*/ ,
    PRV_CPSS_PM3INT_MG_Z80_E                                         /* = 6808*/ ,
    PRV_CPSS_PM3INT_MG_TWSI_MASTER_E                                 /* = 6809*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(MG)                                  /* = 6810-6815*/,

    /*Start Z80GlobalInt - Z80 Global Interrupt Cause address[0x00000508]*/
    /* Tree place: .7.24*/
    /*INTERRUPT ID BASE:[6816]*/
    PRV_CPSS_PM3INT_Z80_GLOBAL_GENERAL_E                             /* = 6816*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_MG_SUM_E                              /* = 6817*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_DOOR_BELL_REG0_E                      /* = 6818*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_DOOR_BELL_REG1_E                      /* = 6819*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_DOOR_BELL_REG2_E                      /* = 6820*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_DOOR_BELL_REG3_E                      /* = 6821*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_DOOR_BELL_REG4_E                      /* = 6822*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_DOOR_BELL_REG5_E                      /* = 6823*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_DOOR_BELL_REG6_E                      /* = 6824*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_DOOR_BELL_REG7_E                      /* = 6825*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_DOOR_BELL_REG8_E                      /* = 6826*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_DOOR_BELL_REG9_E                      /* = 6827*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_DOOR_BELL_REG10_E                     /* = 6828*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_DOOR_BELL_REG11_E                     /* = 6829*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_DOOR_BELL_REG12_E                     /* = 6830*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_DOOR_BELL_REG13_E                     /* = 6831*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_DOOR_BELL_REG14_E                     /* = 6832*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_DOOR_BELL_REG15_E                     /* = 6833*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_DOOR_BELL_REG16_E                     /* = 6834*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_DOOR_BELL_REG17_E                     /* = 6835*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_DOOR_BELL_REG18_E                     /* = 6836*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_DOOR_BELL_REG19_E                     /* = 6837*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_DOOR_BELL_REG20_E                     /* = 6838*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_DOOR_BELL_REG21_E                     /* = 6839*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_DOOR_BELL_REG22_E                     /* = 6840*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_DOOR_BELL_REG23_E                     /* = 6841*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_DOOR_BELL_REG24_E                     /* = 6842*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_DOOR_BELL_REG25_E                     /* = 6843*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_DOOR_BELL_REG26_E                     /* = 6844*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_DOOR_BELL_REG27_E                     /* = 6845*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_DOOR_BELL_REG28_E                     /* = 6846*/ ,
    PRV_CPSS_PM3INT_Z80_GLOBAL_WRAPPER_SUM_E                         /* = 6847*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(Z80_GLOBAL)                          /* = 6847-6847*/,

    /*Start Z80Int - Z80 Interrupt Cause address[0x00000510]*/
    /* Tree place: .7.24.31*/
    /*INTERRUPT ID BASE:[6848]*/
    PRV_CPSS_PM3INT_Z80_GENERAL_E                                    /* = 6848*/ ,
    PRV_CPSS_PM3INT_Z80_PROC_RAM_PARITY_ERR_E                        /* = 6849*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(Z80)                                 /* = 6850-6879*/,

    /*Start I2CBridgeInt - I2C Bridge Interrupt Cause address[0x000910D8]*/
    /* Tree place: .7.25*/
    /*INTERRUPT ID BASE:[6880]*/
    PRV_CPSS_PM3INT_I2C_BRIDGE_TRANSACTION_END_OR_ERR_E              /* = 6880*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(I2C_BRIDGE)                          /* = 6881-6911*/,

    /*Start TxSdmaInt - TX SDMA Interrupt Cause address[0x00002810]*/
    /* Tree place: .8*/
    /*INTERRUPT ID BASE:[6912]*/
    PRV_CPSS_PM3INT_TX_SDMA_GENERAL_E                                /* = 6912*/ ,
    PRV_CPSS_PM3INT_TX_SDMA_BUFFER_QUEUE0_E                          /* = 6913*/ ,
    PRV_CPSS_PM3INT_TX_SDMA_BUFFER_QUEUE1_E                          /* = 6914*/ ,
    PRV_CPSS_PM3INT_TX_SDMA_BUFFER_QUEUE2_E                          /* = 6915*/ ,
    PRV_CPSS_PM3INT_TX_SDMA_BUFFER_QUEUE3_E                          /* = 6916*/ ,
    PRV_CPSS_PM3INT_TX_SDMA_BUFFER_QUEUE4_E                          /* = 6917*/ ,
    PRV_CPSS_PM3INT_TX_SDMA_BUFFER_QUEUE5_E                          /* = 6918*/ ,
    PRV_CPSS_PM3INT_TX_SDMA_BUFFER_QUEUE6_E                          /* = 6919*/ ,
    PRV_CPSS_PM3INT_TX_SDMA_BUFFER_QUEUE7_E                          /* = 6920*/ ,
    PRV_CPSS_PM3INT_TX_SDMA_ERROR_QUEUE0_E                           /* = 6921*/ ,
    PRV_CPSS_PM3INT_TX_SDMA_ERROR_QUEUE1_E                           /* = 6922*/ ,
    PRV_CPSS_PM3INT_TX_SDMA_ERROR_QUEUE2_E                           /* = 6923*/ ,
    PRV_CPSS_PM3INT_TX_SDMA_ERROR_QUEUE3_E                           /* = 6924*/ ,
    PRV_CPSS_PM3INT_TX_SDMA_ERROR_QUEUE4_E                           /* = 6925*/ ,
    PRV_CPSS_PM3INT_TX_SDMA_ERROR_QUEUE5_E                           /* = 6926*/ ,
    PRV_CPSS_PM3INT_TX_SDMA_ERROR_QUEUE6_E                           /* = 6927*/ ,
    PRV_CPSS_PM3INT_TX_SDMA_ERROR_QUEUE7_E                           /* = 6928*/ ,
    PRV_CPSS_PM3INT_TX_SDMA_END_QUEUE0_E                             /* = 6929*/ ,
    PRV_CPSS_PM3INT_TX_SDMA_END_QUEUE1_E                             /* = 6930*/ ,
    PRV_CPSS_PM3INT_TX_SDMA_END_QUEUE2_E                             /* = 6931*/ ,
    PRV_CPSS_PM3INT_TX_SDMA_END_QUEUE3_E                             /* = 6932*/ ,
    PRV_CPSS_PM3INT_TX_SDMA_END_QUEUE4_E                             /* = 6933*/ ,
    PRV_CPSS_PM3INT_TX_SDMA_END_QUEUE5_E                             /* = 6934*/ ,
    PRV_CPSS_PM3INT_TX_SDMA_END_QUEUE6_E                             /* = 6935*/ ,
    PRV_CPSS_PM3INT_TX_SDMA_END_QUEUE7_E                             /* = 6936*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TX_SDMA)                             /* = 6937-6943*/,

    /*Start RxSdmaInt - RX SDMA Interrupt Cause address[0x0000280C]*/
    /* Tree place: .9*/
    /*INTERRUPT ID BASE:[6944]*/
    PRV_CPSS_PM3INT_RX_SDMA_GENERAL_E                                /* = 6944*/ ,
    PRV_CPSS_PM3INT_RX_SDMA_RESERVED1_E                              /* = 6945*/ ,
    PRV_CPSS_PM3INT_RX_SDMA_BUFFER_QUEUE0_E                          /* = 6946*/ ,
    PRV_CPSS_PM3INT_RX_SDMA_BUFFER_QUEUE1_E                          /* = 6947*/ ,
    PRV_CPSS_PM3INT_RX_SDMA_BUFFER_QUEUE2_E                          /* = 6948*/ ,
    PRV_CPSS_PM3INT_RX_SDMA_BUFFER_QUEUE3_E                          /* = 6949*/ ,
    PRV_CPSS_PM3INT_RX_SDMA_BUFFER_QUEUE4_E                          /* = 6950*/ ,
    PRV_CPSS_PM3INT_RX_SDMA_BUFFER_QUEUE5_E                          /* = 6951*/ ,
    PRV_CPSS_PM3INT_RX_SDMA_BUFFER_QUEUE6_E                          /* = 6952*/ ,
    PRV_CPSS_PM3INT_RX_SDMA_BUFFER_QUEUE7_E                          /* = 6953*/ ,
    PRV_CPSS_PM3INT_RX_SDMA_RESERVED10_E                             /* = 6954*/ ,
    PRV_CPSS_PM3INT_RX_SDMA_ERROR_QUEUE0_E                           /* = 6955*/ ,
    PRV_CPSS_PM3INT_RX_SDMA_ERROR_QUEUE1_E                           /* = 6956*/ ,
    PRV_CPSS_PM3INT_RX_SDMA_ERROR_QUEUE2_E                           /* = 6957*/ ,
    PRV_CPSS_PM3INT_RX_SDMA_ERROR_QUEUE3_E                           /* = 6958*/ ,
    PRV_CPSS_PM3INT_RX_SDMA_ERROR_QUEUE4_E                           /* = 6959*/ ,
    PRV_CPSS_PM3INT_RX_SDMA_ERROR_QUEUE5_E                           /* = 6960*/ ,
    PRV_CPSS_PM3INT_RX_SDMA_ERROR_QUEUE6_E                           /* = 6961*/ ,
    PRV_CPSS_PM3INT_RX_SDMA_ERROR_QUEUE7_E                           /* = 6962*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(RX_SDMA)                             /* = 6963-6975*/,

    /*Start Dfx1IntSum - DFX1 Interrupt Summary Cause address[0x000000B8]*/
    /* Tree place: .10*/
    /*temporary all bits reserved*/
    /*INTERRUPT ID BASE:[6976]*/
    PRV_CPSS_PM3INT_DFX1_SUM_GENERAL_E                               /* = 6976*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(DFX1_SUM)                            /* = 6977-7007*/,

    /*Start Func1IntSum - FUNC1 Interrupt Summary Cause address[0x000003F4]*/
    /* Tree place: .11*/
    /*INTERRUPT ID BASE:[7008]*/
    PRV_CPSS_PM3INT_FUNC1_SUM_GENERAL_E                              /* = 7008*/ ,
    PRV_CPSS_PM3INT_FUNC1_SUM_RESERVED1_E                            /* = 7009*/ ,
    PRV_CPSS_PM3INT_FUNC1_SUM_HA_EPCL_E                              /* = 7010*/ ,
    PRV_CPSS_PM3INT_FUNC1_SUM_RESERVED3_E                            /* = 7011*/ ,
    PRV_CPSS_PM3INT_FUNC1_SUM_PFC_E                                  /* = 7012*/ ,
    PRV_CPSS_PM3INT_FUNC1_SUM_QAG_E                                  /* = 7013*/ ,
    PRV_CPSS_PM3INT_FUNC1_SUM_DFX_SERVER_E                           /* = 7014*/ ,
    PRV_CPSS_PM3INT_FUNC1_SUM_SHT0_E                                 /* = 7015*/ ,
    PRV_CPSS_PM3INT_FUNC1_SUM_SHT1_E                                 /* = 7016*/ ,
    PRV_CPSS_PM3INT_FUNC1_SUM_TXQ_LL_E                               /* = 7017*/ ,
    PRV_CPSS_PM3INT_FUNC1_SUM_TXQ_PFC_E                              /* = 7018*/ ,
    PRV_CPSS_PM3INT_FUNC1_SUM_TXQ_QCN_E                              /* = 7019*/ ,
    PRV_CPSS_PM3INT_FUNC1_SUM_TXQ_QUEUE_E                            /* = 7020*/ ,
    PRV_CPSS_PM3INT_FUNC1_SUM_TXQ_DQ0_E                              /* = 7021*/ ,
    PRV_CPSS_PM3INT_FUNC1_SUM_TXQ_DQ1_E                              /* = 7022*/ ,
    PRV_CPSS_PM3INT_FUNC1_SUM_TXQ_DQ2_E                              /* = 7023*/ ,
    PRV_CPSS_PM3INT_FUNC1_SUM_TXQ_DQ3_E                              /* = 7024*/ ,
    PRV_CPSS_PM3INT_FUNC1_SUM_TXQ_ETA_E                              /* = 7025*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(FUNC1_SUM)                           /* = 7026-7039*/,

    /*Start HaEpclIntSum - HA and EPCL Interrupt Summary Cause address[0x01C003A8]*/
    /* Tree place: .11.2*/
    /*INTERRUPT ID BASE:[7040]*/
    PRV_CPSS_PM3INT_HEP_SUM_GENERAL_E                                /* = 7040*/ ,
    PRV_CPSS_PM3INT_HEP_SUM_RESERVED1_E                              /* = 7041*/ ,
    PRV_CPSS_PM3INT_HEP_SUM_DFX_E                                    /* = 7042*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(HEP_SUM)                             /* = 7043-7071*/,

    /*Start TtiEngineInt - TTI Engine Interrupt  Cause address[0x01E00004]*/
    /* Tree place: .11.4*/
    /*INTERRUPT ID BASE:[7072]*/
    PRV_CPSS_PM3INT_TTI_ENGINE_GENERAL_E                             /* = 7072*/ ,
    PRV_CPSS_PM3INT_TTI_ENGINE_CPU_ADDR_OUT_OF_SPASE_E               /* = 7073*/ ,
    PRV_CPSS_PM3INT_TTI_ENGINE_RESERVED2_E                           /* = 7074*/ ,
    PRV_CPSS_PM3INT_TTI_ENGINE_TTI_ACCESS_DATA_ERROR_E               /* = 7075*/ ,
    PRV_CPSS_PM3INT_TTI_ENGINE_PKT_DESCRIPTOR_FIFO_FULL_E            /* = 7076*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TTI_ENGINE)                          /* = 7077-7103*/,

    /*Start QagInt - QAG Interrupt  Cause address[0x03B00010]*/
    /* Tree place: .11.5*/
    /*INTERRUPT ID BASE:[7104]*/
    PRV_CPSS_PM3INT_QAG_GENERAL_E                                    /* = 7104*/ ,
    PRV_CPSS_PM3INT_QAG_WRONG_ADDRESS_E                              /* = 7105*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(QAG)                                 /* = 7106-7135*/,

    /*Start DfxServerSumInt - DFX Server Interrupt Summary Cause address[0x044F8100]*/
    /* Tree place: .11.6*/
    /*INTERRUPT ID BASE:[7136]*/
    PRV_CPSS_PM3INT_DFX_SERVER_SUM_GENERAL_E                         /* = 7136*/ ,
    PRV_CPSS_PM3INT_DFX_SERVER_SUM_INT_E                             /* = 7137*/ ,
    PRV_CPSS_PM3INT_DFX_SERVER_SUM_SPARE_SUM_E                       /* = 7138*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(DFX_SERVER_SUM)                      /* = 7139-7167*/,

    /*Start DfxServerInt - DFX Server Interrupt  Cause address[0x044F8108]*/
    /* Tree place: .11.6.1*/
    /*INTERRUPT ID BASE:[7168]*/
    PRV_CPSS_PM3INT_DFX_SERVER_GENERAL_E                             /* = 7168*/ ,
    PRV_CPSS_PM3INT_DFX_SERVER_WRONG_ADDR_FROM_PIPE0_E               /* = 7169*/ ,
    PRV_CPSS_PM3INT_DFX_SERVER_WRONG_ADDR_FROM_PIPE1_E               /* = 7170*/ ,
    PRV_CPSS_PM3INT_DFX_SERVER_WRONG_ADDR_FROM_PIPE2_E               /* = 7171*/ ,
    PRV_CPSS_PM3INT_DFX_SERVER_WRONG_ADDR_FROM_PIPE3_E               /* = 7172*/ ,
    PRV_CPSS_PM3INT_DFX_SERVER_WRONG_ADDR_FROM_PIPE4_E               /* = 7173*/ ,
    PRV_CPSS_PM3INT_DFX_SERVER_WRONG_ADDR_FROM_PIPE5_E               /* = 7174*/ ,
    PRV_CPSS_PM3INT_DFX_SERVER_WRONG_ADDR_FROM_PIPE6_E               /* = 7175*/ ,
    PRV_CPSS_PM3INT_DFX_SERVER_WRONG_ADDR_FROM_PIPE7_E               /* = 7176*/ ,
    PRV_CPSS_PM3INT_DFX_SERVER_READ_FROM_MC_OR_BC_ADDR_PIPE0_E       /* = 7177*/ ,
    PRV_CPSS_PM3INT_DFX_SERVER_READ_FROM_MC_OR_BC_ADDR_PIPE1_E       /* = 7178*/ ,
    PRV_CPSS_PM3INT_DFX_SERVER_READ_FROM_MC_OR_BC_ADDR_PIPE2_E       /* = 7179*/ ,
    PRV_CPSS_PM3INT_DFX_SERVER_READ_FROM_MC_OR_BC_ADDR_PIPE3_E       /* = 7180*/ ,
    PRV_CPSS_PM3INT_DFX_SERVER_READ_FROM_MC_OR_BC_ADDR_PIPE4_E       /* = 7181*/ ,
    PRV_CPSS_PM3INT_DFX_SERVER_READ_FROM_MC_OR_BC_ADDR_PIPE5_E       /* = 7182*/ ,
    PRV_CPSS_PM3INT_DFX_SERVER_READ_FROM_MC_OR_BC_ADDR_PIPE6_E       /* = 7183*/ ,
    PRV_CPSS_PM3INT_DFX_SERVER_READ_FROM_MC_OR_BC_ADDR_PIPE7_E       /* = 7184*/ ,
    PRV_CPSS_PM3INT_DFX_SERVER_HD_EFUSE_FULL_E                       /* = 7185*/ ,
    PRV_CPSS_PM3INT_DFX_SERVER_WRONG_REG_ADDR_E                      /* = 7186*/ ,
    PRV_CPSS_PM3INT_DFX_SERVER_BIST_FAIL_E                           /* = 7187*/ ,
    PRV_CPSS_PM3INT_DFX_SERVER_TEMPERATURE_THRESHOLD_E               /* = 7188*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(DFX_SERVER)                          /* = 7189-7199*/,

    /*Start DfxServerSpareInt - DFX Server Spare Interrupt  Cause address[0x044F8110]*/
    /* Tree place: .11.6.2*/
    /* bit0 - self summary, bits [31:1] for spares 30-0 */
    /*INTERRUPT ID BASE:[7200]*/
    PRV_CPSS_PM3INT_INDEXED_BIT_REG_MAC(DFX_SRV, SPARE, 0/*reg index*/),

    /*Start SharedTabInt - SHT Interrupt Cause address */
    /* SHT0[0x039F0010], SHT1[0x03AF0010]              */
    /* Tree place: .11.7-8*/
    /*INTERRUPT ID BASE:[7232]*/
    PRV_CPSS_PM3INT_SHT0_GENERAL_E                                   /* = 7232*/ ,
    PRV_CPSS_PM3INT_SHT0_WRONG_ADDR_E                                /* = 7233*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(SHT0)                                /* = 7234-7263*/,

    /*INTERRUPT ID BASE:[7264]*/
    PRV_CPSS_PM3INT_SHT1_GENERAL_E                                   /* = 7264*/ ,
    PRV_CPSS_PM3INT_SHT1_WRONG_ADDR_E                                /* = 7265*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(SHT1)                                /* = 7266-7295*/,

    /*Start TxqLlInt - TXQ Link List Interrupt  Cause address[0x03DA0508]*/
    /* Tree place: .11.9*/
    /*INTERRUPT ID BASE:[7296]*/
    PRV_CPSS_PM3INT_TXQ_LL_GENERAL_E                                 /* = 7296*/ ,
    PRV_CPSS_PM3INT_TXQ_LL_ID_FIFO_OVERRUN_E                         /* = 7297*/ ,
    PRV_CPSS_PM3INT_TXQ_LL_RESERVED2_E                               /* = 7298*/ ,
    PRV_CPSS_PM3INT_TXQ_LL_FBUF_ECC_ERR_E                            /* = 7299*/ ,
    PRV_CPSS_PM3INT_TXQ_LL_LATENCY_FIFO_OVERRUN_E                    /* = 7300*/ ,
    PRV_CPSS_PM3INT_TXQ_LL_LIMIT_ID_FIFO_OVERRUN_E                   /* = 7301*/ ,
    PRV_CPSS_PM3INT_TXQ_LL_WRONG_ADDRESS_E                           /* = 7302*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_LL)                              /* = 7303-7327*/,

    /*Start TxqPfcIntSum - TXQ PFC Interrupt Summary Cause address[0x03E01300]*/
    /* Tree place: .11.10*/
    /*INTERRUPT ID BASE:[7328]*/
    PRV_CPSS_PM3INT_TXQ_PFC_SUM_GENERAL_E                            /* = 7328*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_SUM_COUNERS_PARITY_E                     /* = 7329*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_SUM_OVER_OR_UNDER_FLOW_E                 /* = 7330*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_SUM_PIPE0_GLOBAL_OVERFLOW_E              /* = 7331*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_SUM_PIPE1_GLOBAL_OVERFLOW_E              /* = 7332*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_SUM_PIPE2_GLOBAL_OVERFLOW_E              /* = 7333*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_SUM_PIPE3_GLOBAL_OVERFLOW_E              /* = 7334*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_SUM_PIPE4_GLOBAL_OVERFLOW_E              /* = 7335*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_SUM_PIPE5_GLOBAL_OVERFLOW_E              /* = 7336*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_SUM_PIPE6_GLOBAL_OVERFLOW_E              /* = 7337*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_SUM_PIPE7_GLOBAL_OVERFLOW_E              /* = 7338*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_SUM_RESERVED11_E                         /* = 7339*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_SUM_PIPE0_GLOBAL_UNDERFLOW_E             /* = 7340*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_SUM_PIPE1_GLOBAL_UNDERFLOW_E             /* = 7341*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_SUM_PIPE2_GLOBAL_UNDERFLOW_E             /* = 7342*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_SUM_PIPE3_GLOBAL_UNDERFLOW_E             /* = 7343*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_SUM_PIPE4_GLOBAL_UNDERFLOW_E             /* = 7344*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_SUM_PIPE5_GLOBAL_UNDERFLOW_E             /* = 7345*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_SUM_PIPE6_GLOBAL_UNDERFLOW_E             /* = 7346*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_SUM_PIPE7_GLOBAL_UNDERFLOW_E             /* = 7347*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_SUM_FIFOS_OVERRUN_E                      /* = 7348*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_SUM_MSGS_SETS_OVERRUN_EGR3_CAUSE0_E      /* = 7349*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_SUM_MSGS_SETS_OVERRUN_EGR3_CAUSE1_E      /* = 7350*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_SUM_MSGS_SETS_OVERRUN_EGR7_CAUSE0_E      /* = 7351*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_SUM_MSGS_SETS_OVERRUN_EGR7_CAUSE1_E      /* = 7352*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_PFC_SUM)                         /* = 7353-7359*/,

    /*Start TxqPfcParityInt - TXQ PFC Parity Interrupt Cause address[0x03E01310]*/
    /* Tree place: .11.10.1*/
    /*INTERRUPT ID BASE:[7360]*/
    PRV_CPSS_PM3INT_TXQ_PFC_PARITY_GENERAL_E                         /* = 7360*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_PARITY_PIPE0_COUTERS_ERR_E               /* = 7361*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_PARITY_PIPE1_COUTERS_ERR_E               /* = 7362*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_PARITY_PIPE2_COUTERS_ERR_E               /* = 7363*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_PARITY_PIPE3_COUTERS_ERR_E               /* = 7364*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_PARITY_PIPE4_COUTERS_ERR_E               /* = 7365*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_PARITY_PIPE5_COUTERS_ERR_E               /* = 7366*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_PARITY_PIPE6_COUTERS_ERR_E               /* = 7367*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_PARITY_PIPE7_COUTERS_ERR_E               /* = 7368*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_PARITY_WRONG_ADDRESS_E                   /* = 7369*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_PFC_PARITY)                      /* = 7370-7391*/,

    /*Start TxqPfcOverflowInt - TXQ PFC Overflow Interrupt Cause address[0x03E01320]*/
    /* Tree place: .11.10.2*/
    /*INTERRUPT ID BASE:[7392]*/
    PRV_CPSS_PM3INT_TXQ_PFC_OVERFLOW_GENERAL_E                       /* = 7392*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERFLOW_PIPE0_OVERFLOW_E                /* = 7393*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERFLOW_PIPE0_UNDERFLOW_E               /* = 7394*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERFLOW_PIPE1_OVERFLOW_E                /* = 7395*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERFLOW_PIPE1_UNDERFLOW_E               /* = 7396*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERFLOW_PIPE2_OVERFLOW_E                /* = 7397*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERFLOW_PIPE2_UNDERFLOW_E               /* = 7398*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERFLOW_PIPE3_OVERFLOW_E                /* = 7399*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERFLOW_PIPE3_UNDERFLOW_E               /* = 7400*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERFLOW_PIPE4_OVERFLOW_E                /* = 7401*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERFLOW_PIPE4_UNDERFLOW_E               /* = 7402*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERFLOW_PIPE5_OVERFLOW_E                /* = 7403*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERFLOW_PIPE5_UNDERFLOW_E               /* = 7404*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERFLOW_PIPE6_OVERFLOW_E                /* = 7405*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERFLOW_PIPE6_UNDERFLOW_E               /* = 7406*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERFLOW_PIPE7_OVERFLOW_E                /* = 7407*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERFLOW_PIPE7_UNDERFLOW_E               /* = 7408*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_PFC_OVERFLOW)                    /* = 7409-7423*/,

    /*Start TxqPfcGlobalOverflowInt - TXQ PFC Global Overflow Interrupt Cause address */
    /* PIPE0[0x03E01330], PIPE1[0x03E01334], PIPE2[0x03E01338], PIPE3[0x03E0133C], */
    /* PIPE4[0x03E01340], PIPE5[0x03E01344], PIPE6[0x03E01348], PIPE7[0x03E0134C], */
    /* Tree place: .11.10.3-10*/
    /*INTERRUPT ID BASE:[7424]*/
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE0_GENERAL_E          /* = 7424*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE0_TC0_E              /* = 7425*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE0_TC1_E              /* = 7426*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE0_TC2_E              /* = 7427*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE0_TC3_E              /* = 7428*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE0_TC4_E              /* = 7429*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE0_TC5_E              /* = 7430*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE0_TC6_E              /* = 7431*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE0_TC7_E              /* = 7432*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_PFC_GLOBAL_OVERFLOW_PIPE0)       /* = 7433-7455*/,

    /*INTERRUPT ID BASE:[7456]*/
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE1_GENERAL_E          /* = 7456*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE1_TC0_E              /* = 7457*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE1_TC1_E              /* = 7458*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE1_TC2_E              /* = 7459*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE1_TC3_E              /* = 7460*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE1_TC4_E              /* = 7461*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE1_TC5_E              /* = 7462*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE1_TC6_E              /* = 7463*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE1_TC7_E              /* = 7464*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_PFC_GLOBAL_OVERFLOW_PIPE1)       /* = 7465-7487*/,

    /*INTERRUPT ID BASE:[7488]*/
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE2_GENERAL_E          /* = 7488*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE2_TC0_E              /* = 7489*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE2_TC1_E              /* = 7490*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE2_TC2_E              /* = 7491*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE2_TC3_E              /* = 7492*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE2_TC4_E              /* = 7493*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE2_TC5_E              /* = 7494*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE2_TC6_E              /* = 7495*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE2_TC7_E              /* = 7496*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_PFC_GLOBAL_OVERFLOW_PIPE2)       /* = 7497-7519*/,

    /*INTERRUPT ID BASE:[7520]*/
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE3_GENERAL_E          /* = 7520*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE3_TC0_E              /* = 7521*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE3_TC1_E              /* = 7522*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE3_TC2_E              /* = 7523*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE3_TC3_E              /* = 7524*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE3_TC4_E              /* = 7525*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE3_TC5_E              /* = 7526*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE3_TC6_E              /* = 7527*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE3_TC7_E              /* = 7528*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_PFC_GLOBAL_OVERFLOW_PIPE3)       /* = 7529-7551*/,

    /*INTERRUPT ID BASE:[7552]*/
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE4_GENERAL_E          /* = 7552*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE4_TC0_E              /* = 7553*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE4_TC1_E              /* = 7554*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE4_TC2_E              /* = 7555*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE4_TC3_E              /* = 7556*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE4_TC4_E              /* = 7557*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE4_TC5_E              /* = 7558*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE4_TC6_E              /* = 7559*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE4_TC7_E              /* = 7560*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_PFC_GLOBAL_OVERFLOW_PIPE4)       /* = 7561-7583*/,

    /*INTERRUPT ID BASE:[7584]*/
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE5_GENERAL_E          /* = 7584*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE5_TC0_E              /* = 7585*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE5_TC1_E              /* = 7586*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE5_TC2_E              /* = 7587*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE5_TC3_E              /* = 7588*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE5_TC4_E              /* = 7589*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE5_TC5_E              /* = 7590*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE5_TC6_E              /* = 7591*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE5_TC7_E              /* = 7592*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_PFC_GLOBAL_OVERFLOW_PIPE5)       /* = 7593-7615*/,

    /*INTERRUPT ID BASE:[7616]*/
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE6_GENERAL_E          /* = 7616*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE6_TC0_E              /* = 7617*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE6_TC1_E              /* = 7618*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE6_TC2_E              /* = 7619*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE6_TC3_E              /* = 7620*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE6_TC4_E              /* = 7621*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE6_TC5_E              /* = 7622*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE6_TC6_E              /* = 7623*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE6_TC7_E              /* = 7624*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_PFC_GLOBAL_OVERFLOW_PIPE6)       /* = 7625-7647*/,

    /*INTERRUPT ID BASE:[7648]*/
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE7_GENERAL_E          /* = 7648*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE7_TC0_E              /* = 7649*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE7_TC1_E              /* = 7650*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE7_TC2_E              /* = 7651*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE7_TC3_E              /* = 7652*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE7_TC4_E              /* = 7653*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE7_TC5_E              /* = 7654*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE7_TC6_E              /* = 7655*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_OVERFLOW_PIPE7_TC7_E              /* = 7656*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_PFC_GLOBAL_OVERFLOW_PIPE7)       /* = 7657-7679*/,

    /*Start TxqPfcGlobalUnderflowInt - TXQ PFC Global Underflow Interrupt Cause address */
    /* PIPE0[0x03E01370], PIPE1[0x03E01374], PIPE2[0x03E01378], PIPE3[0x03E0137C], */
    /* PIPE4[0x03E01380], PIPE5[0x03E01384], PIPE6[0x03E01388], PIPE7[0x03E0138C], */
    /* Tree place: .11.10.12-19*/
    /*INTERRUPT ID BASE:[7680]*/
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE0_GENERAL_E         /* = 7680*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE0_TC0_E             /* = 7681*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE0_TC1_E             /* = 7682*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE0_TC2_E             /* = 7683*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE0_TC3_E             /* = 7684*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE0_TC4_E             /* = 7685*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE0_TC5_E             /* = 7686*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE0_TC6_E             /* = 7687*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE0_TC7_E             /* = 7688*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_PFC_GLOBAL_UNDERFLOW_PIPE0)      /* = 7689-7711*/,

    /*INTERRUPT ID BASE:[7712]*/
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE1_GENERAL_E         /* = 7712*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE1_TC0_E             /* = 7713*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE1_TC1_E             /* = 7714*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE1_TC2_E             /* = 7715*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE1_TC3_E             /* = 7716*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE1_TC4_E             /* = 7717*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE1_TC5_E             /* = 7718*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE1_TC6_E             /* = 7719*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE1_TC7_E             /* = 7720*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_PFC_GLOBAL_UNDERFLOW_PIPE1)      /* = 7721-7743*/,

    /*INTERRUPT ID BASE:[7744]*/
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE2_GENERAL_E         /* = 7744*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE2_TC0_E             /* = 7745*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE2_TC1_E             /* = 7746*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE2_TC2_E             /* = 7747*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE2_TC3_E             /* = 7748*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE2_TC4_E             /* = 7749*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE2_TC5_E             /* = 7750*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE2_TC6_E             /* = 7751*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE2_TC7_E             /* = 7752*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_PFC_GLOBAL_UNDERFLOW_PIPE2)      /* = 7753-7775*/,

    /*INTERRUPT ID BASE:[7776]*/
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE3_GENERAL_E         /* = 7776*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE3_TC0_E             /* = 7777*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE3_TC1_E             /* = 7778*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE3_TC2_E             /* = 7779*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE3_TC3_E             /* = 7780*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE3_TC4_E             /* = 7781*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE3_TC5_E             /* = 7782*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE3_TC6_E             /* = 7783*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE3_TC7_E             /* = 7784*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_PFC_GLOBAL_UNDERFLOW_PIPE3)      /* = 7785-7807*/,

    /*INTERRUPT ID BASE:[7808]*/
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE4_GENERAL_E         /* = 7808*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE4_TC0_E             /* = 7809*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE4_TC1_E             /* = 7810*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE4_TC2_E             /* = 7811*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE4_TC3_E             /* = 7812*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE4_TC4_E             /* = 7813*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE4_TC5_E             /* = 7814*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE4_TC6_E             /* = 7815*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE4_TC7_E             /* = 7816*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_PFC_GLOBAL_UNDERFLOW_PIPE4)      /* = 7817-7839*/,

    /*INTERRUPT ID BASE:[7840]*/
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE5_GENERAL_E         /* = 7840*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE5_TC0_E             /* = 7841*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE5_TC1_E             /* = 7842*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE5_TC2_E             /* = 7843*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE5_TC3_E             /* = 7844*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE5_TC4_E             /* = 7845*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE5_TC5_E             /* = 7846*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE5_TC6_E             /* = 7847*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE5_TC7_E             /* = 7848*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_PFC_GLOBAL_UNDERFLOW_PIPE5)      /* = 7849-7871*/,

    /*INTERRUPT ID BASE:[7872]*/
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE6_GENERAL_E         /* = 7872*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE6_TC0_E             /* = 7873*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE6_TC1_E             /* = 7874*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE6_TC2_E             /* = 7875*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE6_TC3_E             /* = 7876*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE6_TC4_E             /* = 7877*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE6_TC5_E             /* = 7878*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE6_TC6_E             /* = 7879*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE6_TC7_E             /* = 7880*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_PFC_GLOBAL_UNDERFLOW_PIPE6)      /* = 7881-7903*/,

    /*INTERRUPT ID BASE:[7904]*/
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE7_GENERAL_E         /* = 7904*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE7_TC0_E             /* = 7905*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE7_TC1_E             /* = 7906*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE7_TC2_E             /* = 7907*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE7_TC3_E             /* = 7908*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE7_TC4_E             /* = 7909*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE7_TC5_E             /* = 7910*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE7_TC6_E             /* = 7911*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_GLOBAL_UNDERFLOW_PIPE7_TC7_E             /* = 7912*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_PFC_GLOBAL_UNDERFLOW_PIPE7)      /* = 7913-7935*/,

    /*Start TxqPfcOverrunsInt - TXQ PFC Overruns Interrupt Cause address[0x03E013B0]*/
    /* Tree place: .11.10.20*/
    /*INTERRUPT ID BASE:[7936]*/
    PRV_CPSS_PM3INT_TXQ_PFC_OVERRUN_GENERAL_E                        /* = 7936*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERRUN_PIPE0_EQ_FIFO_E                  /* = 7937*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERRUN_PIPE0_FC_IND_FIFO_E              /* = 7938*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERRUN_PIPE1_EQ_FIFO_E                  /* = 7939*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERRUN_PIPE1_FC_IND_FIFO_E              /* = 7940*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERRUN_PIPE2_EQ_FIFO_E                  /* = 7941*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERRUN_PIPE2_FC_IND_FIFO_E              /* = 7942*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERRUN_PIPE3_EQ_FIFO_E                  /* = 7943*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERRUN_PIPE3_FC_IND_FIFO_E              /* = 7944*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERRUN_PIPE4_EQ_FIFO_E                  /* = 7945*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERRUN_PIPE4_FC_IND_FIFO_E              /* = 7946*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERRUN_PIPE5_EQ_FIFO_E                  /* = 7947*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERRUN_PIPE5_FC_IND_FIFO_E              /* = 7948*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERRUN_PIPE6_EQ_FIFO_E                  /* = 7949*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERRUN_PIPE6_FC_IND_FIFO_E              /* = 7950*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERRUN_PIPE7_EQ_FIFO_E                  /* = 7951*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_OVERRUN_PIPE7_FC_IND_FIFO_E              /* = 7952*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_PFC_OVERRUN)                     /* = 7953-7967*/,

    /*Start TxqPfcMsgSetsOverrunErr3_0Int - TXQ PFC Msg Sets Overruns Egress3 Cause0 */
    /* Interrupt Cause address[0x03E013B8]                                           */
    /* Tree place: .11.10.21*/
    /*INTERRUPT ID BASE:[7968]*/
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_0_GENERAL_E            /* = 7968*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_0_PIPE0_QCN_E          /* = 7969*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_0_PIPE0_GLOBAL_CNT_E   /* = 7970*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_0_PIPE0_EGR0_E         /* = 7971*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_0_PIPE0_EGR1_E         /* = 7972*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_0_PIPE0_EGR2_E         /* = 7973*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_0_PIPE0_EGR3_E         /* = 7974*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_0_PIPE1_QCN_E          /* = 7975*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_0_PIPE1_GLOBAL_CNT_E   /* = 7976*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_0_PIPE1_EGR0_E         /* = 7977*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_0_PIPE1_EGR1_E         /* = 7978*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_0_PIPE1_EGR2_E         /* = 7979*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_0_PIPE1_EGR3_E         /* = 7980*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_0_PIPE2_QCN_E          /* = 7981*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_0_PIPE2_GLOBAL_CNT_E   /* = 7982*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_0_PIPE2_EGR0_E         /* = 7983*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_0_PIPE2_EGR1_E         /* = 7984*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_0_PIPE2_EGR2_E         /* = 7985*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_0_PIPE2_EGR3_E         /* = 7986*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_0_PIPE3_QCN_E          /* = 7987*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_0_PIPE3_GLOBAL_CNT_E   /* = 7988*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_0_PIPE3_EGR0_E         /* = 7989*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_0_PIPE3_EGR1_E         /* = 7990*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_0_PIPE3_EGR2_E         /* = 7991*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_0_PIPE3_EGR3_E         /* = 7992*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_PFC_MSG_OVERRRUN_EGR3_0)         /* = 7993-7999*/,

    /*Start TxqPfcMsgSetsOverrunErr3_1Int - TXQ PFC Msg Sets Overruns Egress3 Cause1 */
    /* Interrupt Cause address[0x03E013C0]                                           */
    /* Tree place: .11.10.22*/
    /*INTERRUPT ID BASE:[8000]*/
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_1_GENERAL_E            /* = 8000*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_1_PIPE4_QCN_E          /* = 8001*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_1_PIPE4_GLOBAL_CNT_E   /* = 8002*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_1_PIPE4_EGR0_E         /* = 8003*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_1_PIPE4_EGR1_E         /* = 8004*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_1_PIPE4_EGR2_E         /* = 8005*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_1_PIPE4_EGR3_E         /* = 8006*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_1_PIPE5_QCN_E          /* = 8007*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_1_PIPE5_GLOBAL_CNT_E   /* = 8008*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_1_PIPE5_EGR0_E         /* = 8009*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_1_PIPE5_EGR1_E         /* = 8010*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_1_PIPE5_EGR2_E         /* = 8011*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_1_PIPE5_EGR3_E         /* = 8012*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_1_PIPE6_QCN_E          /* = 8013*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_1_PIPE6_GLOBAL_CNT_E   /* = 8014*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_1_PIPE6_EGR0_E         /* = 8015*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_1_PIPE6_EGR1_E         /* = 8016*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_1_PIPE6_EGR2_E         /* = 8017*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_1_PIPE6_EGR3_E         /* = 8018*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_1_PIPE7_QCN_E          /* = 8019*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_1_PIPE7_GLOBAL_CNT_E   /* = 8020*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_1_PIPE7_EGR0_E         /* = 8021*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_1_PIPE7_EGR1_E         /* = 8022*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_1_PIPE7_EGR2_E         /* = 8023*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR3_1_PIPE7_EGR3_E         /* = 8024*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_PFC_MSG_OVERRRUN_EGR3_1)         /* = 8025-8031*/,

    /*Start TxqPfcMsgSetsOverrunErr7_0Int - TXQ PFC Msg Sets Overruns Egress7 Cause0 */
    /* Interrupt Cause address[0x03E013D0]                                           */
    /* Tree place: .11.10.23*/
    /*INTERRUPT ID BASE:[8032]*/
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_0_GENERAL_E            /* = 8032*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_0_PIPE0_EGR4_E         /* = 8033*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_0_PIPE0_EGR5_E         /* = 8034*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_0_PIPE0_EGR6_E         /* = 8035*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_0_PIPE0_EGR7_E         /* = 8036*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_0_RESERVED5_E          /* = 8037*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_0_RESERVED6_E          /* = 8038*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_0_PIPE1_EGR4_E         /* = 8039*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_0_PIPE1_EGR5_E         /* = 8040*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_0_PIPE1_EGR6_E         /* = 8041*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_0_PIPE1_EGR7_E         /* = 8042*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_0_RESERVED11_E         /* = 8043*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_0_RESERVED12_E         /* = 8044*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_0_PIPE2_EGR4_E         /* = 8045*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_0_PIPE2_EGR5_E         /* = 8046*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_0_PIPE2_EGR6_E         /* = 8047*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_0_PIPE2_EGR7_E         /* = 8048*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_0_RESERVED17_E         /* = 8049*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_0_RESERVED18_E         /* = 8050*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_0_PIPE3_EGR4_E         /* = 8051*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_0_PIPE3_EGR5_E         /* = 8052*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_0_PIPE3_EGR6_E         /* = 8053*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_0_PIPE3_EGR7_E         /* = 8054*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_PFC_MSG_OVERRRUN_EGR7_0)         /* = 8055-8063*/,

    /*Start TxqPfcMsgSetsOverrunErr7_1Int - TXQ PFC Msg Sets Overruns Egress7 Cause1 */
    /* Interrupt Cause address[0x03E013D8]                                           */
    /* Tree place: .11.10.24*/
    /*INTERRUPT ID BASE:[8064]*/
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_1_GENERAL_E            /* = 8064*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_1_RESERVED1_E          /* = 8065*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_1_PIPE4_EGR4_E         /* = 8066*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_1_PIPE4_EGR5_E         /* = 8067*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_1_PIPE4_EGR6_E         /* = 8068*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_1_PIPE4_EGR7_E         /* = 8069*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_1_RESERVED6_E          /* = 8070*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_1_RESERVED7_E          /* = 8071*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_1_PIPE5_EGR4_E         /* = 8072*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_1_PIPE5_EGR5_E         /* = 8073*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_1_PIPE5_EGR6_E         /* = 8074*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_1_PIPE5_EGR7_E         /* = 8075*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_1_RESERVED12_E         /* = 8076*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_1_RESERVED13_E         /* = 8077*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_1_PIPE6_EGR4_E         /* = 8078*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_1_PIPE6_EGR5_E         /* = 8079*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_1_PIPE6_EGR6_E         /* = 8080*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_1_PIPE6_EGR7_E         /* = 8081*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_1_RESERVED18_E         /* = 8082*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_1_RESERVED19_E         /* = 8083*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_1_PIPE7_EGR4_E         /* = 8084*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_1_PIPE7_EGR5_E         /* = 8085*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_1_PIPE7_EGR6_E         /* = 8086*/ ,
    PRV_CPSS_PM3INT_TXQ_PFC_MSG_OVERRRUN_EGR7_1_PIPE7_EGR7_E         /* = 8087*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_PFC_MSG_OVERRRUN_EGR7_1)         /* = 8088-8095*/,

    /*Start TxqQcnInt - TXQ QCN Interrupt Cause address[0x03F00100]*/
    /* Tree place: .11.11*/
    /*INTERRUPT ID BASE:[8096]*/
    PRV_CPSS_PM3INT_TXQ_QCN_GENERAL_E                                /* = 8096*/ ,
    PRV_CPSS_PM3INT_TXQ_QCN_BUFFER_FIFO_OVERRUN_E                    /* = 8097*/ ,
    PRV_CPSS_PM3INT_TXQ_QCN_BUFFER_FIFO_PARITY_ERROR_E               /* = 8098*/ ,
    PRV_CPSS_PM3INT_TXQ_QCN_DROP_E                                   /* = 8099*/ ,
    PRV_CPSS_PM3INT_TXQ_QCN_WRONG_ADDR_E                             /* = 8100*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_QCN)                             /* = 8101-8127*/,

    /*Start TxqTxqSumInt - TXQ Transmit Queue Summary Interrupt Cause address[0x03C90000]*/
    /* Tree place: .11.12*/
    /*INTERRUPT ID BASE:[8128]*/
    PRV_CPSS_PM3INT_TXQ_TXQ_SUM_GENERAL_E                            /* = 8128*/ ,
    PRV_CPSS_PM3INT_TXQ_TXQ_SUM_GEN_E                                /* = 8129*/ ,
    PRV_CPSS_PM3INT_TXQ_TXQ_SUM_PORT_DESC_FULL0_E                    /* = 8130*/ ,
    PRV_CPSS_PM3INT_TXQ_TXQ_SUM_PORT_DESC_FULL1_E                    /* = 8131*/ ,
    PRV_CPSS_PM3INT_TXQ_TXQ_SUM_PORT_DESC_FULL2_E                    /* = 8132*/ ,
    PRV_CPSS_PM3INT_TXQ_TXQ_SUM_PORT_BUF_FULL0_E                     /* = 8133*/ ,
    PRV_CPSS_PM3INT_TXQ_TXQ_SUM_PORT_BUF_FULL1_E                     /* = 8134*/ ,
    PRV_CPSS_PM3INT_TXQ_TXQ_SUM_PORT_BUF_FULL2_E                     /* = 8135*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_TXQ_SUM)                         /* = 8136-8159*/,

    /*Start TxqTxqGenInt - TXQ Transmit Queue General Interrupt Cause address[0x03C90008]*/
    /* Tree place: .11.12.1*/
    /*INTERRUPT ID BASE:[8160]*/
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_GENERAL_E                            /* = 8160*/ ,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_GLOBAL_DESC_FULL_E                   /* = 8161*/ ,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_GLOBAL_BUFF_FULL_E                   /* = 8162*/ ,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_RESERVED3_E                          /* = 8163*/ ,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_RESERVED4_E                          /* = 8164*/ ,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_TAIL_DROP_LATENCY_FIFO_OVERFLOW_E    /* = 8165*/ ,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_LATENCY_FIFO_OVERRUN_E               /* = 8166*/ ,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_LIMIT_ID_FIFO_OVERRUN_E              /* = 8167*/ ,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_CNC_UPDATE0_DROP_E                   /* = 8168*/ ,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_CNC_UPDATE1_DROP_E                   /* = 8169*/ ,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_CNC_UPDATE2_DROP_E                   /* = 8170*/ ,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_CNC_UPDATE3_DROP_E                   /* = 8171*/ ,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_VLAN_UPDATE0_DROP_E                  /* = 8172*/ ,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_VLAN_UPDATE1_DROP_E                  /* = 8173*/ ,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_VLAN_UPDATE2_DROP_E                  /* = 8174*/ ,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_VLAN_UPDATE3_DROP_E                  /* = 8175*/ ,
    PRV_CPSS_PM3INT_BIT_ALIGN_MAC(TXQ_TXQ_GEN, 19)                   /* = 8176-8179*/,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_WRONG_ADDRESS_E                      /* = 8180*/ ,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_CLEAR_DEMUX_FIFO0_E                  /* = 8181*/ ,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_CLEAR_DEMUX_FIFO1_E                  /* = 8182*/ ,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_CLEAR_DEMUX_FIFO2_E                  /* = 8183*/ ,
    PRV_CPSS_PM3INT_TXQ_TXQ_GEN_CLEAR_DEMUX_FIFO3_E                  /* = 8184*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_TXQ_GEN)                         /* = 8185-8191*/,

    /*Start TxqTxqPortFullBuf0Int - TXQ Transmit Queue Port Full Buf0 (ports 0-31)*/
    /* Interrupt Cause address[0x03C90060]*/
    /* Tree place: .11.12.2*/
    /*INTERRUPT ID BASE:[8192]*/
    PRV_CPSS_PM3INT_INDEXED_BIT_REG_MAC(TXQ_TXQ_BUF_FULL, PORT, 0),

    /*Start TxqTxqPortFullBuf1Int - TXQ Transmit Queue Port Full Buf1 (ports 32-61)*/
    /* Interrupt Cause address[0x03C90068]*/
    /* Tree place: .11.12.3*/
    /*INTERRUPT ID BASE:[8224]*/
    PRV_CPSS_PM3INT_INDEXED_BIT_REG_MAC(TXQ_TXQ_BUF_FULL, PORT, 1),

    /*Start TxqTxqPortFullBuf2Int - TXQ Transmit Queue Port Full Buf2 (ports 62-71)*/
    /* Interrupt Cause address[0x03C90070]*/
    /* Tree place: .11.12.4*/
    /*INTERRUPT ID BASE:[8256]*/
    PRV_CPSS_PM3INT_INDEXED_BIT_REG_MAC(TXQ_TXQ_BUF_FULL, PORT, 2),

    /*Start TxqTxqPortFullDesc0Int - TXQ Transmit Queue Port Full Desc0 (ports 0-31)*/
    /* Interrupt Cause address[0x03C90040]*/
    /* Tree place: .11.12.5*/
    /*INTERRUPT ID BASE:[8288]*/
    PRV_CPSS_PM3INT_INDEXED_BIT_REG_MAC(TXQ_TXQ_DESC_FULL, PORT, 0),

    /*Start TxqTxqPortFullDesc1Int - TXQ Transmit Queue Port Full Desc1 (ports 32-61)*/
    /* Interrupt Cause address[0x03C90048]*/
    /* Tree place: .11.12.6*/
    /*INTERRUPT ID BASE:[8320]*/
    PRV_CPSS_PM3INT_INDEXED_BIT_REG_MAC(TXQ_TXQ_DESC_FULL, PORT, 1),

    /*Start TxqTxqPortFullDesc2Int - TXQ Transmit Queue Port Full Desc2 (ports 62-71)*/
    /* Interrupt Cause address[0x03C90050]*/
    /* Tree place: .11.12.7*/
    /*INTERRUPT ID BASE:[8352]*/
    PRV_CPSS_PM3INT_INDEXED_BIT_REG_MAC(TXQ_TXQ_DESC_FULL, PORT, 2),

    /*Start TxqDqSumInt - TXQ DQ Summary Interrupt Cause address */
    /*DQ0[0x04000400], DQ1[0x04100400], DQ2[0x04200400], DQ3[0x04300400]*/
    /* Tree place: .11.13-16*/
    /*INTERRUPT ID BASE:[8384]*/
    PRV_CPSS_PM3INT_TXQ_DQ0_SUM_GENERAL_E                            /* = 8384*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_SUM_FLUSH_DONE_E                         /* = 8385*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_SUM_MEMORY_ERROR_E                       /* = 8386*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_SUM_EGRESS_STC0_E                        /* = 8387*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_SUM_RESERVED4_E                          /* = 8388*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_SUM_RESERVED5_E                          /* = 8389*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_SUM_GEN_E                                /* = 8390*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_DQ0_SUM)                         /* = 8391-8415*/,

    /*INTERRUPT ID BASE:[8416]*/
    PRV_CPSS_PM3INT_TXQ_DQ1_SUM_GENERAL_E                            /* = 8416*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_SUM_FLUSH_DONE_E                         /* = 8417*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_SUM_MEMORY_ERROR_E                       /* = 8418*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_SUM_EGRESS_STC0_E                        /* = 8419*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_SUM_EGRESS_STC1_E                        /* = 8420*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_SUM_EGRESS_STC2_E                        /* = 8421*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_SUM_GEN_E                                /* = 8422*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_DQ1_SUM)                         /* = 8423-8447*/,

    /*INTERRUPT ID BASE:[8448]*/
    PRV_CPSS_PM3INT_TXQ_DQ2_SUM_GENERAL_E                            /* = 8448*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_SUM_FLUSH_DONE_E                         /* = 8449*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_SUM_MEMORY_ERROR_E                       /* = 8450*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_SUM_EGRESS_STC0_E                        /* = 8451*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_SUM_EGRESS_STC1_E                        /* = 8452*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_SUM_EGRESS_STC2_E                        /* = 8453*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_SUM_GEN_E                                /* = 8454*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_DQ2_SUM)                         /* = 8455-8479*/,

    /*INTERRUPT ID BASE:[8480]*/
    PRV_CPSS_PM3INT_TXQ_DQ3_SUM_GENERAL_E                            /* = 8480*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_SUM_FLUSH_DONE_E                         /* = 8481*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_SUM_MEMORY_ERROR_E                       /* = 8482*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_SUM_EGRESS_STC0_E                        /* = 8483*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_SUM_EGRESS_STC1_E                        /* = 8484*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_SUM_EGRESS_STC2_E                        /* = 8485*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_SUM_GEN_E                                /* = 8486*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_DQ3_SUM)                         /* = 8487-8511*/,

    /*Start TxqDqFlushDoneInt - TXQ DQ Flus Done Interrupt Cause address */
    /* DQ0[0x04000408], DQ1[0x04100408], DQ2[0x04200408], DQ3[0x04300408]*/
    /* Tree place: .11.13-16.1*/
    /*INTERRUPT ID BASE:[8512]*/
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_GENERAL_E                     /* = 8512*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_0_E                           /* = 8513*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_1_E                           /* = 8514*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_2_E                           /* = 8515*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_3_E                           /* = 8516*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_4_E                           /* = 8517*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_5_E                           /* = 8518*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_6_E                           /* = 8519*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_7_E                           /* = 8520*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_8_E                           /* = 8521*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_9_E                           /* = 8522*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_10_E                          /* = 8523*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_11_E                          /* = 8524*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_12_E                          /* = 8525*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_13_E                          /* = 8526*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_14_E                          /* = 8527*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_FLUSH_DONE_15_E                          /* = 8528*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_DQ0_FLUSH_DONE)                  /* = 8529-8543*/,

    /*INTERRUPT ID BASE:[8544]*/
    PRV_CPSS_PM3INT_TXQ_DQ1_FLUSH_DONE_GENERAL_E                     /* = 8544*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_FLUSH_DONE_0_E                           /* = 8545*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_FLUSH_DONE_1_E                           /* = 8546*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_FLUSH_DONE_2_E                           /* = 8547*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_FLUSH_DONE_3_E                           /* = 8548*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_FLUSH_DONE_4_E                           /* = 8549*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_FLUSH_DONE_5_E                           /* = 8550*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_FLUSH_DONE_6_E                           /* = 8551*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_FLUSH_DONE_7_E                           /* = 8552*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_FLUSH_DONE_8_E                           /* = 8553*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_FLUSH_DONE_9_E                           /* = 8554*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_FLUSH_DONE_10_E                          /* = 8555*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_FLUSH_DONE_11_E                          /* = 8556*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_FLUSH_DONE_12_E                          /* = 8557*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_FLUSH_DONE_13_E                          /* = 8558*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_FLUSH_DONE_14_E                          /* = 8559*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_FLUSH_DONE_15_E                          /* = 8560*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_DQ1_FLUSH_DONE)                  /* = 8561-8575*/,

    /*INTERRUPT ID BASE:[8576]*/
    PRV_CPSS_PM3INT_TXQ_DQ2_FLUSH_DONE_GENERAL_E                     /* = 8576*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_FLUSH_DONE_0_E                           /* = 8577*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_FLUSH_DONE_1_E                           /* = 8578*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_FLUSH_DONE_2_E                           /* = 8579*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_FLUSH_DONE_3_E                           /* = 8580*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_FLUSH_DONE_4_E                           /* = 8581*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_FLUSH_DONE_5_E                           /* = 8582*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_FLUSH_DONE_6_E                           /* = 8583*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_FLUSH_DONE_7_E                           /* = 8584*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_FLUSH_DONE_8_E                           /* = 8585*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_FLUSH_DONE_9_E                           /* = 8586*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_FLUSH_DONE_10_E                          /* = 8587*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_FLUSH_DONE_11_E                          /* = 8588*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_FLUSH_DONE_12_E                          /* = 8589*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_FLUSH_DONE_13_E                          /* = 8590*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_FLUSH_DONE_14_E                          /* = 8591*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_FLUSH_DONE_15_E                          /* = 8592*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_DQ2_FLUSH_DONE)                  /* = 8593-8607*/,

    /*INTERRUPT ID BASE:[8608]*/
    PRV_CPSS_PM3INT_TXQ_DQ3_FLUSH_DONE_GENERAL_E                     /* = 8608*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_FLUSH_DONE_0_E                           /* = 8609*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_FLUSH_DONE_1_E                           /* = 8610*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_FLUSH_DONE_2_E                           /* = 8611*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_FLUSH_DONE_3_E                           /* = 8612*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_FLUSH_DONE_4_E                           /* = 8613*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_FLUSH_DONE_5_E                           /* = 8614*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_FLUSH_DONE_6_E                           /* = 8615*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_FLUSH_DONE_7_E                           /* = 8616*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_FLUSH_DONE_8_E                           /* = 8617*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_FLUSH_DONE_9_E                           /* = 8618*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_FLUSH_DONE_10_E                          /* = 8619*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_FLUSH_DONE_11_E                          /* = 8620*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_FLUSH_DONE_12_E                          /* = 8621*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_FLUSH_DONE_13_E                          /* = 8622*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_FLUSH_DONE_14_E                          /* = 8623*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_FLUSH_DONE_15_E                          /* = 8624*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_DQ3_FLUSH_DONE)                  /* = 8625-8639*/,

    /*Start TxqDqMemoryErrInt - TXQ DQ Memory Error Interrupt Cause address */
    /* DQ0[0x04000410], DQ1[0x04100410], DQ2[0x04200410], DQ3[0x04300410]*/
    /* Tree place: .11.13-16.2*/
    /*INTERRUPT ID BASE:[8640]*/
    PRV_CPSS_PM3INT_TXQ_DQ0_MEMO_ERR_GENERAL_E                       /* = 8640*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_MEMO_ERR_STATE_VARIABLE_PARITY_ERR_E     /* = 8641*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_MEMO_ERR_TOKEN_BUCKET_PRIO_PARITY_ERR_E  /* = 8642*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_DQ0_MEMO_ERR)                    /* = 8643-8671*/,

    /*INTERRUPT ID BASE:[8672]*/
    PRV_CPSS_PM3INT_TXQ_DQ1_MEMO_ERR_GENERAL_E                       /* = 8672*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_MEMO_ERR_STATE_VARIABLE_PARITY_ERR_E     /* = 8673*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_MEMO_ERR_TOKEN_BUCKET_PRIO_PARITY_ERR_E  /* = 8674*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_DQ1_MEMO_ERR)                    /* = 8675-8703*/,

    /*INTERRUPT ID BASE:[8704]*/
    PRV_CPSS_PM3INT_TXQ_DQ2_MEMO_ERR_GENERAL_E                       /* = 8704*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_MEMO_ERR_STATE_VARIABLE_PARITY_ERR_E     /* = 8705*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_MEMO_ERR_TOKEN_BUCKET_PRIO_PARITY_ERR_E  /* = 8706*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_DQ2_MEMO_ERR)                    /* = 8707-8735*/,

    /*INTERRUPT ID BASE:[8736]*/
    PRV_CPSS_PM3INT_TXQ_DQ3_MEMO_ERR_GENERAL_E                       /* = 8736*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_MEMO_ERR_STATE_VARIABLE_PARITY_ERR_E     /* = 8737*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_MEMO_ERR_TOKEN_BUCKET_PRIO_PARITY_ERR_E  /* = 8738*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_DQ3_MEMO_ERR)                    /* = 8739-8767*/,

    /*Start TxqDqEgrStcInt - TXQ DQ Egress STC Interrupt Cause address */
    /* DQ0[0x04000418], DQ1[0x04100418], DQ2[0x04200418], DQ3[0x04300418]*/
    /* Tree place: .11.13-16.3*/
    /*INTERRUPT ID BASE:[8768]*/
    PRV_CPSS_PM3INT_TXQ_DQ0_EGR_STC_GENERAL_E                        /* = 8768*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_EGR_STC_PORT0_E                          /* = 8769*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_EGR_STC_PORT1_E                          /* = 8770*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_EGR_STC_PORT2_E                          /* = 8771*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_EGR_STC_PORT3_E                          /* = 8772*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_EGR_STC_PORT4_E                          /* = 8773*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_EGR_STC_PORT5_E                          /* = 8774*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_EGR_STC_PORT6_E                          /* = 8775*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_EGR_STC_PORT7_E                          /* = 8776*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_EGR_STC_PORT8_E                          /* = 8777*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_EGR_STC_PORT9_E                          /* = 8778*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_EGR_STC_PORT10_E                         /* = 8779*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_EGR_STC_PORT11_E                         /* = 8780*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_EGR_STC_PORT12_E                         /* = 8781*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_EGR_STC_PORT13_E                         /* = 8782*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_EGR_STC_PORT14_E                         /* = 8783*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_EGR_STC_PORT15_E                         /* = 8784*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_DQ0_EGR_STC)                     /* = 8785-8799*/,

    /*INTERRUPT ID BASE:[8800]*/
    PRV_CPSS_PM3INT_TXQ_DQ1_EGR_STC_GENERAL_E                        /* = 8800*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_EGR_STC_PORT0_E                          /* = 8801*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_EGR_STC_PORT1_E                          /* = 8802*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_EGR_STC_PORT2_E                          /* = 8803*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_EGR_STC_PORT3_E                          /* = 8804*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_EGR_STC_PORT4_E                          /* = 8805*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_EGR_STC_PORT5_E                          /* = 8806*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_EGR_STC_PORT6_E                          /* = 8807*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_EGR_STC_PORT7_E                          /* = 8808*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_EGR_STC_PORT8_E                          /* = 8809*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_EGR_STC_PORT9_E                          /* = 8810*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_EGR_STC_PORT10_E                         /* = 8811*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_EGR_STC_PORT11_E                         /* = 8812*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_EGR_STC_PORT12_E                         /* = 8813*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_EGR_STC_PORT13_E                         /* = 8814*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_EGR_STC_PORT14_E                         /* = 8815*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_EGR_STC_PORT15_E                         /* = 8816*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_DQ1_EGR_STC)                     /* = 8817-8831*/,

    /*INTERRUPT ID BASE:[8832]*/
    PRV_CPSS_PM3INT_TXQ_DQ2_EGR_STC_GENERAL_E                        /* = 8832*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_EGR_STC_PORT0_E                          /* = 8833*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_EGR_STC_PORT1_E                          /* = 8834*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_EGR_STC_PORT2_E                          /* = 8835*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_EGR_STC_PORT3_E                          /* = 8836*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_EGR_STC_PORT4_E                          /* = 8837*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_EGR_STC_PORT5_E                          /* = 8838*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_EGR_STC_PORT6_E                          /* = 8839*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_EGR_STC_PORT7_E                          /* = 8840*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_EGR_STC_PORT8_E                          /* = 8841*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_EGR_STC_PORT9_E                          /* = 8842*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_EGR_STC_PORT10_E                         /* = 8843*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_EGR_STC_PORT11_E                         /* = 8844*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_EGR_STC_PORT12_E                         /* = 8845*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_EGR_STC_PORT13_E                         /* = 8846*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_EGR_STC_PORT14_E                         /* = 8847*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_EGR_STC_PORT15_E                         /* = 8848*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_DQ2_EGR_STC)                     /* = 8849-8863*/,

    /*INTERRUPT ID BASE:[8864]*/
    PRV_CPSS_PM3INT_TXQ_DQ3_EGR_STC_GENERAL_E                        /* = 8864*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_EGR_STC_PORT0_E                          /* = 8865*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_EGR_STC_PORT1_E                          /* = 8866*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_EGR_STC_PORT2_E                          /* = 8867*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_EGR_STC_PORT3_E                          /* = 8868*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_EGR_STC_PORT4_E                          /* = 8869*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_EGR_STC_PORT5_E                          /* = 8870*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_EGR_STC_PORT6_E                          /* = 8871*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_EGR_STC_PORT7_E                          /* = 8872*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_EGR_STC_PORT8_E                          /* = 8873*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_EGR_STC_PORT9_E                          /* = 8874*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_EGR_STC_PORT10_E                         /* = 8875*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_EGR_STC_PORT11_E                         /* = 8876*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_EGR_STC_PORT12_E                         /* = 8877*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_EGR_STC_PORT13_E                         /* = 8878*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_EGR_STC_PORT14_E                         /* = 8879*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_EGR_STC_PORT15_E                         /* = 8880*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_DQ3_EGR_STC)                     /* = 8881-8895*/,

    /*Start TxqDqGenInt - TXQ DQ General Interrupt Cause address */
    /* DQ0[0x04000430], DQ1[0x04100430], DQ2[0x04200430], DQ3[0x04300430]*/
    /* Tree place: .11.13-16.6*/
    /*INTERRUPT ID BASE:[8896]*/
    PRV_CPSS_PM3INT_TXQ_DQ0_EGR_GEN_GENERAL_E                        /* = 8896*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_EGR_GEN_EGR_MIRROR_DESC_DROP_E           /* = 8897*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ0_EGR_GEN_WRONG_ADDRESS_E                  /* = 8898*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_DQ0_EGR_GEN)                     /* = 8899-8927*/,

    /*INTERRUPT ID BASE:[8928]*/
    PRV_CPSS_PM3INT_TXQ_DQ1_EGR_GEN_GENERAL_E                        /* = 8928*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_EGR_GEN_EGR_MIRROR_DESC_DROP_E           /* = 8929*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ1_EGR_GEN_WRONG_ADDRESS_E                  /* = 8930*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_DQ1_EGR_GEN)                     /* = 8931-8959*/,

    /*INTERRUPT ID BASE:[8960]*/
    PRV_CPSS_PM3INT_TXQ_DQ2_EGR_GEN_GENERAL_E                        /* = 8960*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_EGR_GEN_EGR_MIRROR_DESC_DROP_E           /* = 8961*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ2_EGR_GEN_WRONG_ADDRESS_E                  /* = 8962*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_DQ2_EGR_GEN)                     /* = 8963-8991*/,

    /*INTERRUPT ID BASE:[8992]*/
    PRV_CPSS_PM3INT_TXQ_DQ3_EGR_GEN_GENERAL_E                        /* = 8992*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_EGR_GEN_EGR_MIRROR_DESC_DROP_E           /* = 8993*/ ,
    PRV_CPSS_PM3INT_TXQ_DQ3_EGR_GEN_WRONG_ADDRESS_E                  /* = 8994*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(TXQ_DQ3_EGR_GEN)                     /* = 8995-9023*/,

    /*Start EtaInt - External TCAM Action Table Interrupt Cause address [0x04501104]*/
    /* Tree place: .11.17*/
    /*INTERRUPT ID BASE:[9024]*/
    PRV_CPSS_PM3INT_ETA_GENERAL_E                                    /* = 9024*/ ,
    PRV_CPSS_PM3INT_ETA_WRONG_ADDRESS_E                              /* = 9025*/ ,
    PRV_CPSS_PM3INT_ETA_TAC0_WRONG_ADDRESS_E                         /* = 9026*/ ,
    PRV_CPSS_PM3INT_ETA_TAC1_WRONG_ADDRESS_E                         /* = 9027*/ ,
    PRV_CPSS_PM3INT_ETA_TAC2_WRONG_ADDRESS_E                         /* = 9028*/ ,
    PRV_CPSS_PM3INT_ETA_TAC3_WRONG_ADDRESS_E                         /* = 9029*/ ,
    PRV_CPSS_PM3INT_ETA_TAC4_WRONG_ADDRESS_E                         /* = 9030*/ ,
    PRV_CPSS_PM3INT_ETA_TAC5_WRONG_ADDRESS_E                         /* = 9031*/ ,
    PRV_CPSS_PM3INT_ETA_TAC6_WRONG_ADDRESS_E                         /* = 9032*/ ,
    PRV_CPSS_PM3INT_ETA_TAC7_WRONG_ADDRESS_E                         /* = 9033*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(ETA)                                 /* = 9034-9055*/,

    /*Start XSmiInt - XSMI Interrupt Cause address [0x00040010]*/
    /* Tree place: .12*/
    /*INTERRUPT ID BASE:[9056]*/
    PRV_CPSS_PM3INT_XSMI_GENERAL_E                                   /* = 9056*/ ,
    PRV_CPSS_PM3INT_XSMI_XG_SMI_WRITE_E                              /* = 9057*/ ,
    PRV_CPSS_PM3INT_ALIGN31_MAC(XSMI)                                /* = 9058-9087*/,

    /*INTERRUPT ID BASE:[9088]*/
    PRV_CPSS_PM3INT_LAST_INT_E,
    PRV_CPSS_EXMXPM3_LAST_INT_E = PRV_CPSS_PM3INT_LAST_INT_E


}PRV_CPSS_EXMXPM3_INT_CAUSE_ENT;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDrvExMxEventsPuma3h */

