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
 * @brief declaration of internal TM structures.
 *
 * @file tm_core_types.h
 *
 * $Revision: 2.0 $
 */

#ifndef   	TM_CORE_TYPES_H
#define   	TM_CORE_TYPES_H

#include <errno.h>

#include "tm_platform_implementation_definitions.h"

#include "tm_defs.h"

#ifdef VIRTUAL_LAYER
	/** all macro definitions are in file virtual_layer_core.h*/
	#include "tm_virtual_layer_core.h"
#else
	/** here are dummy definitions for case without logical layer*/
	#define	CONVERT_TO_PHYSICAL(level,index) /* do nothing*/
	#define	CONVERT_TO_VIRTUAL(level,index)  /* do nothing*/
	#define	GENERATE_VIRTUAL_ID(level,index) /* do nothing*/
	#define	REMOVE_VIRTUAL_ID(level,index) /* do nothing*/
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define	TM_ELIG_FUNC_TABLE_SIZE	64

#define	TM_NODE_DISABLED_FUN		62

#define VALIDATE_ELIG_FUNCTION(elig_fun) \
    if ((elig_fun >= TM_ELIG_FUNC_TABLE_SIZE) || (elig_fun == TM_NODE_DISABLED_FUN))\
	{ /* maximum function id 0..63 */\
        rc = TM_CONF_ELIG_PRIO_FUNC_ID_OOR;\
        goto out;\
    }\



#include "tm_hw_configuration_interface.h"


#define TM_128M_kB 128*1024  	/*in Kb units*/
#define TM_4M_kB   4*1024 		/*in Kb units*/


#define TM_1K   1024 

/** Delay Size Multiplier (used to calculate TD Threshold)  
 */
#define TM_DELAY_SIZE_MULT   1


/* Physical port capacity (speed in Kbits) */
#define TM_1G_SPEED   1000000  /* 1050000000UL */   /* 1G + 5% */
#define TM_2HG_SPEED  2500000  /* 2625000000UL */   /* 2.5G + 5% */
#define TM_10G_SPEED  10000000 /* 10500000000ULL */ /* 10G + 5% */
#define TM_40G_SPEED  40000000 /* 42000000000ULL */ /* 40G + 5% */
#define TM_50G_SPEED  50000000
#define TM_100G_SPEED 100000000

#define TM_MAX_SPEED TM_100G_SPEED


/** WRED CoS number */
#define TM_WRED_COS              8

/** Drop Profile Pointer Table values */
#define TM_Q_DRP_PROF_PER_ENTRY  4
#define TM_A_DRP_PROF_PER_ENTRY  4
#define TM_B_DRP_PROF_PER_ENTRY  8
#define TM_C_DRP_PROF_PER_ENTRY  8


/** Max number of Scrubbing slots */
#define TM_SCRUB_SLOTS 64


/*********************************/
/* Internal Databases Structures */
/*********************************/



/** Global arrays structures definitions */

/** Shaping Profile is inside plapform-dependent part
 */
#include "tm_core_types_platform.h"

/** Range of children nodes in symetric distribution 
 */
struct ranges {
    uint32_t norm_range[C_LEVEL+1];
    uint32_t last_range[C_LEVEL+1];
};

/** WRED curve 
 */
struct tm_wred_curve {
    uint8_t prob[32];
};

struct dwrr_port_quantum_t {
    uint16_t quantum;/*9 bits actually */
};

#if 0
/** Drop profile 
 */
struct tm_drop_profile
{
    uint8_t		aql_exp /*:4*/;				/**< Forget factor exponent */							/*4 bits actually */
    uint8_t		color_td_en/*:1*/;			/**< Colored Tail Drop Enable */						/*1 bit actually */

    uint8_t		scale_exp[3];			/**< Used for scaling of AQL range */
    uint16_t	scale_ratio[3];			/**< Used for scaling of DP range */
    uint8_t		curve_id[3];			/**< RED curve index per color[0..2] */
    uint8_t		dp_ratio[3];			/**< Used for scaling of DP */

    uint16_t	min_threshold[3];		/**< RED curve Min threshold per color [0..2] */		/*1 bit actually */
    uint8_t		td_thresh_res/*:1*/;		/**< Tail Drop Threshold resolution - 16B or 16KB */
    uint32_t	td_threshold_bursts/*:19*/;/**< Hard Limit on queue length */						/*19 bits actually */
    uint32_t	use_counter;
    struct tm_drop_profile_params	drop_profile_source;
}__ATTRIBUTE_PACKED__;
#endif
typedef enum 
{
	TM_NODE_FREE=-1,
	TM_NODE_USED=0,	
	TM_NODE_RESERVED=1,
	TM_NODE_DELETED=2,
}tm_node_status_t;

struct queue_mapping_t
{
	tm_node_status_t	nodeStatus;
    uint16_t 			nodeParent;
};

struct node_mapping_t
{
	tm_node_status_t	nodeStatus;
    uint16_t 			nodeParent;
    int 				childLo;
    int 				childHi;
};

struct port_mapping_t
{
	tm_node_status_t	nodeStatus;
    int 				childLo;
    int 				childHi;
};


/** Queue data structure 
 */
struct tm_queue
{
	struct queue_mapping_t		mapping;
    uint8_t elig_prio_func/*:6*/;				/*6 bits actually */
    /* this value is used for debugging moving nodes/ reshuffling  */ 
    uint8_t node_temporary_disabled/*:1*/;		/*1 bit actually */
	
    uint8_t installed/*:1*/;					/*1 bit actually */
    uint16_t shaping_profile_ref;
    uint16_t dwrr_quantum/*:14*/;				/*14 bits actually */
    uint16_t wred_profile_ref;
#ifdef PER_NODE_SHAPING
   /* for using in read_node_shaping() */
    uint32_t cir_bw; /**< CIR BW in Kbits/sec */
    uint32_t eir_bw; /**< EIR BW in Kbits/sec */
    uint16_t cbs;/**< CBS in kbytes */
    uint16_t ebs;/**< EBS in kbytes */
#endif
}__ATTRIBUTE_PACKED__;


/** A-node data structure 
 */
struct tm_a_node
{
	struct node_mapping_t		mapping;
    uint8_t elig_prio_func/*:6*/;						/*6 bits actually */
    /* this value is used for debugging moving nodes/ reshuffling  */ 
    uint8_t node_temporary_disabled/*:1*/;				/*1 bit actually */
    uint16_t shaping_profile_ref;
    uint16_t dwrr_quantum/*:14*/;						/*14 bits actually */
    uint8_t dwrr_enable_per_priority_mask; 
    uint16_t wred_profile_ref;
   
#ifdef PER_NODE_SHAPING
   /* for using in read_node_shaping() */
    uint32_t cir_bw; /**< CIR BW in Kbits/sec */
    uint32_t eir_bw; /**< EIR BW in Kbits/sec */
    uint16_t cbs;/**< CBS in kbytes */
    uint16_t ebs;/**< EBS in kbytes */
#endif
}__ATTRIBUTE_PACKED__;


/** B-node data structure 
 */
struct tm_b_node
{
	struct node_mapping_t		mapping;
    uint8_t elig_prio_func/*:6*/;					/*6 bits actually */
    /* this value is used for debugging moving nodes/ reshuffling  */ 
    uint8_t node_temporary_disabled/*:1*/;			/*1 bit actually */
 
    uint16_t shaping_profile_ref;
    uint16_t dwrr_quantum/*:14*/;					/*14 bits actually */
    uint8_t dwrr_enable_per_priority_mask;
    uint16_t wred_profile_ref;
#ifdef PER_NODE_SHAPING
   /* for using in read_node_shaping() */
    uint32_t cir_bw; /**< CIR BW in Kbits/sec */
    uint32_t eir_bw; /**< EIR BW in Kbits/sec */
    uint16_t cbs;/**< CBS in kbytes */
    uint16_t ebs;/**< EBS in kbytes */
#endif
}__ATTRIBUTE_PACKED__;


/** C-node data structure 
 */
struct tm_c_node 
{
	struct node_mapping_t		mapping;
    uint8_t elig_prio_func/*:6*/;			/*6 bits actually */
    /* this value is used for debugging moving nodes/ reshuffling  */ 
    uint8_t node_temporary_disabled/*:1*/;  /*1 bit actually */
	
    uint8_t wred_cos;   /* bit map */
    uint16_t shaping_profile_ref;
    uint16_t dwrr_quantum/*:14*/;			/*14 bits actually */
    uint8_t dwrr_enable_per_priority_mask;
    uint8_t wred_profile_ref[TM_WRED_COS];
#ifdef PER_NODE_SHAPING
   /* for using in read_node_shaping() */
    uint32_t cir_bw; /**< CIR BW in Kbits/sec */
    uint32_t eir_bw; /**< EIR BW in Kbits/sec */
    uint16_t cbs;/**< CBS in kbytes */
    uint16_t ebs;/**< EBS in kbytes */
#endif
}__ATTRIBUTE_PACKED__;


/** Port data structure 
 */
struct tm_port
{
	struct port_mapping_t		mapping;
    uint8_t elig_prio_func/*:6*/;				/*6 bits actually */
    /* this bit is used for debugging moving nodes/ reshuffling  */ 
    uint8_t node_temporary_disabled/*:1*/;		/*1 bit actually */
 
    struct dwrr_port_quantum_t dwrr_quantum_per_level[8];
    uint8_t dwrr_enable_per_priority_mask;
    uint8_t port_speed;

    /* Shaping config */
    uint16_t cir_token;
    uint16_t eir_token;
    uint16_t periods;
    uint8_t min_token_res;
    uint8_t max_token_res;
    uint32_t cir_burst_size;/**< CBS in kbytes */
    uint32_t eir_burst_size;/**< EBS in kbytes */

    struct ranges children_range;   /* for symetric distribution only */
    uint8_t sym_mode/*:1*/;         /* assymetric/symetric tree */ /*1 bit actually */

    uint8_t rcb_high_range_limit;
    uint8_t rcb_low_range_limit;

    /* WRED profile */
    uint8_t wred_cos;   /* bit map */
    uint8_t wred_profile_ref; /* Global port Drop profile */
    uint8_t wred_profile_ref_cos[TM_WRED_COS];
    
    /*save the values as configured for usage in read api*/
    uint32_t cir_sw;                  /**< Cir in Kbit/sec */
    uint32_t eir_sw;                  /**< EBS in Kbit/sec */


}__ATTRIBUTE_PACKED__;



/* Eligible Priority Functions structures */

struct tm_elig_prio_func_node_entry {
    uint16_t func_out[4];
};


/* Eligible Priority Functions Definitions */

struct tm_elig_prio_func_node
{
    struct tm_elig_prio_func_node_entry tbl_entry[8];
};

struct tm_elig_prio_func_queue
{
    struct tm_elig_prio_func_node_entry tbl_entry;
};


/** TM2TM Channel */
struct tm2tm_port_channel {
	/* first 32 bit word */
    uint32_t configured:1;
    uint32_t egr_elems:6;        /* x4, max 64, reset 0x30, 0=all 64 */
    uint32_t src_lvl:3;          /* A/B/C/Port */
    uint32_t mode:1;             /* WRED/BP */
    /* relevant for BP mode only */
    uint32_t bp_map:1;           /* Queue/C */
    uint32_t bp_offset:16;
	/* end of  word  - 28 bits are used */
    uint8_t bp_xon;           /* BP Xon (6 bits)*/
    uint8_t bp_xoff;          /* BP Xoff (6 bits)*/
}__ATTRIBUTE_PACKED__;

struct tm2tm_node_channel {
	/* first 32 bit word */
    uint32_t configured:1;
    uint32_t egr_elems:16;         /* x16, max 64K, reset 0x0, 0=all 64K */
    uint32_t src_lvl:3;          /* Queue/A/B/C */
    uint32_t mode:1;             /* WRED/BP */
    /* relevant for BP mode only */
    uint32_t bp_map:1;           /* Queue/C */
	/* end of  word  - 22 bits are used */
	/* second 32 bit word */
    uint32_t bp_offset : 16;
    uint32_t bp_xon:6;           /* BP Xon */
    uint32_t bp_xoff:6;          /* BP Xoff */
	/* end of  word  - 28 bits are used */
}__ATTRIBUTE_PACKED__;


/** TM2TM Egress AQM mode settings */
struct tm_aqm_local_params {
    uint8_t configured/*:1*/;			/* actually  1 bit used */
    uint8_t color_num; /* 1 or 2 or 3 */
    enum tm_dp_source dp_src[TM_MAX_PACKET_COLORS]; /* QL/AQL per color [0..2] */
    uint8_t resolution; /* 4 or 6 bits */
    enum tm_max_prob_mode max_p_mode[TM_MAX_PACKET_COLORS]; /* Max Probability Mode - 100/50/25/12.5% per Color*/
}__ATTRIBUTE_PACKED__;


/** TM2TM Ingress AQM mode settings - Channels */
struct tm_aqm_remote_params {
    uint8_t configured /*:1 */;/* actually  1 bit used */
    uint8_t color_num; /* 0 or 1 or 2 */
}__ATTRIBUTE_PACKED__;

/** TM2TM Drop Unit structure */
struct dp_unit {
    struct tm_aqm_local_params local[P_LEVEL+1]; /* per Port/C/B/A/Queue level */
    struct tm_aqm_remote_params remote[TM2TM_PORT_CH+1]; /* per Port/Node level */
};

/** TM Scheduler Port SMS pBase Attribute structure */
struct port_sms_attr_pbase {
    uint8_t pbase;
    uint8_t pshift;
};

/** TM Scheduler Port SMS Qmap parsing Attribute structure */
struct port_sms_attr_qmap_parsing {
    uint8_t mode;
    uint16_t base_q;
    uint8_t dcolor;
};



struct tm_ctl {
  uint32_t magic;
  tm_handle rm;                  /**< rm hndl */	

  /* Nodes arrays */
  uint32_t 		 	tm_total_queues;
  uint32_t			tm_total_a_nodes;
  uint32_t			tm_total_b_nodes;
  uint32_t			tm_total_c_nodes;
  uint32_t			tm_total_ports;
  struct tm_queue	*tm_queue_array;
  struct tm_a_node	*tm_a_node_array;
  struct tm_b_node 	*tm_b_node_array;
  struct tm_c_node 	*tm_c_node_array;
  struct tm_port 	*tm_port_array;
  
  /* Global arrays */
  struct tm_shaping_profile *tm_shaping_profiles;
  struct tm_drop_profile *tm_q_lvl_drop_profiles;
  struct tm_drop_profile *tm_a_lvl_drop_profiles;
  struct tm_drop_profile *tm_b_lvl_drop_profiles;
  struct tm_drop_profile *tm_c_lvl_drop_profiles[TM_WRED_COS];
  struct tm_drop_profile *tm_p_lvl_drop_profiles;
  struct tm_drop_profile *tm_p_lvl_drop_profiles_cos[TM_WRED_COS];
 /*--------------*/
  struct tm_wred_curve *tm_wred_q_lvl_curves;
  struct tm_wred_curve *tm_wred_a_lvl_curves;
  struct tm_wred_curve *tm_wred_b_lvl_curves;
  struct tm_wred_curve *tm_wred_c_lvl_curves[TM_WRED_COS];
  struct tm_wred_curve *tm_wred_ports_curves;
  struct tm_wred_curve *tm_wred_ports_curves_cos[TM_WRED_COS];
   /*--------------*/
  
  /* Eligible Priority Function Table arrays */
  struct tm_elig_prio_func_node tm_elig_prio_a_lvl_tbl[TM_ELIG_FUNC_TABLE_SIZE];
  struct tm_elig_prio_func_node tm_elig_prio_b_lvl_tbl[TM_ELIG_FUNC_TABLE_SIZE];
  struct tm_elig_prio_func_node tm_elig_prio_c_lvl_tbl[TM_ELIG_FUNC_TABLE_SIZE];
  struct tm_elig_prio_func_node tm_elig_prio_p_lvl_tbl[TM_ELIG_FUNC_TABLE_SIZE];
  struct tm_elig_prio_func_queue tm_elig_prio_q_lvl_tbl[TM_ELIG_FUNC_TABLE_SIZE];
  

  uint16_t *tm_q_lvl_drop_prof_ptr;   /* 64K mirror for Q Drop prof. ptr table
                                       * to avoid read-modify-write during
                                       * single pointer update */
  uint16_t *tm_a_lvl_drop_prof_ptr;   /* 16K mirror for A Drop prof. ptr table
									   * to avoid read-modify-write during
									   * single pointer update */
  uint16_t *tm_b_lvl_drop_prof_ptr;    /* 2K mirror for B Drop prof. ptr table
                                       * to avoid read-modify-write during
                                       * single pointer update */
  uint8_t *tm_c_lvl_drop_prof_ptr[TM_WRED_COS];
  /* 512 entries mirror for C Drop prof. ptr table to avoid read-modify-write during
   * single pointer update */
  /* Abstraction for Port Drop profiles */
  uint8_t *tm_p_lvl_drop_prof_ptr;
  uint8_t *tm_p_lvl_drop_prof_ptr_cos[TM_WRED_COS]; /* CoS mode */
 /*--------------*/

  /* Scheduling parameters */
  uint32_t freq;                 /**< LAD frequency */
  /* global update states */
  uint8_t periodic_scheme_state; /**< periodic scheme updated/not updated */
  /* RCB sections */
  uint8_t rcb_section_used[4];   /**< 4 sections, max range 0..0xFF each */
  
  /* per level data including port level */
  struct level_config level_data[P_LEVEL+1];
  /* Tree data */
  uint8_t tree_deq_status;       /**< tree DeQ status */
  uint8_t tree_dwrr_enable_per_priority_mask;    /**< tree DWRR priority bitmap for port scheduling */

  /* TM2TM */
  struct dp_unit dp_unit;
  struct tm2tm_port_channel port_ch;
  struct tm2tm_node_channel node_ch;

#ifdef VIRTUAL_LAYER
#else
	  /* Reshuffling changes */
	  struct tm_tree_change list;
#endif
  
  /* Other registers */
  uint8_t port_ext_bp_en;
  uint8_t dwrr_bytes_burst_limit;
  uint16_t min_pkg_size;         /**<  minimum package size */
  uint8_t port_ch_emit;          /**<  port chunks emitted per scheduler selection */
  uint8_t aging_status;          /**< aging status */

  /* TM-SMS mapping */
  struct port_sms_attr_pbase        *tm_port_sms_attr_pbase;
  struct port_sms_attr_qmap_parsing *tm_port_sms_attr_qmap_pars;
  

  /* environment */
  tm_handle hEnv;
#ifdef VIRTUAL_LAYER
  /* virtual layer handle */
  tm_handle hVirtualLayer;
#endif
  tm_handle hLogicalLayer;
   
  struct tm_lib_init_params init_params;

  /*  quantum limits calculated  according to init parameters  - calculated during libInit*/
  uint32_t	min_port_quantum_bytes;
  uint32_t	max_port_quantum_bytes;
  uint32_t	min_node_quantum_bytes;
  uint32_t	max_node_quantum_bytes;
  /* constant values , defined by platform */
  uint32_t	node_quantum_chunk_size_bytes;
  uint32_t	port_quantum_chunk_size_bytes;

};



/** Internal TM control structure 
 */

#define TM_MAGIC 0x24051974
/* following macro declares and checks validity of tmctl */
#define DECLARE_TM_CTL_PTR(name,value)	struct tm_ctl * name= (struct tm_ctl *)value;

#define CHECK_TM_CTL_PTR(ptr)	\
	if (!ptr) return -EINVAL;\
    if (ptr->magic != TM_MAGIC) return -EBADF;

#define TM_CTL(name,handle)	DECLARE_TM_CTL_PTR(name,handle);CHECK_TM_CTL_PTR(name);


#define TM_ENV(var) var->hEnv


#ifdef __cplusplus
}
#endif


#endif   /* TM_TYPES_H */

