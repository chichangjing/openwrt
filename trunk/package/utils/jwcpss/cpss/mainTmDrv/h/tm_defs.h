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
 * @brief TM Configuration Library Public Definitions.
 *
 * @file tm_defs.h
 *
 * $Revision: 2.0 $
 */

#ifndef   	TM_DEFS_H
#define   	TM_DEFS_H

#include "platform/tm_os_related_definitions.h" 

#ifndef MIN
	#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#endif /* MIN */

#ifndef MAX
	#define MAX(x,y) (((x) > (y)) ? (x) : (y))
#endif  /* MAX */

#define PER_NODE_SHAPING
/* #define VIRTUAL_LAYER */
#define USE_DEFAULT_CURVES



/** BW resolution in Kbits */
#define TM_KBITS   1000

/*---------------- Constants definitions-------------*/

/** Number of Linecards */
#define TM_LC_NUM       10

#define TM_MTU          2000   /* bytes */
#define TM_RTT          200    /* mseconds */
/** Node Quantum Unit in units of 256 bytes */
#define TM_NODE_QUANTUM_UNIT  256
/** Port Quantum Unit in units of 64 bytes */
#define TM_PORT_QUANTUM_UNIT  64

/** Infinite Profile */
#define TM_INF_SHP_PROFILE  0

#ifdef PER_NODE_SHAPING
	#define TM_DIRECT_NODE_SHAPING	(uint16_t)(-1) /* read only profile for node, set automatically if node shaping was configured directly */
#endif

/* infinite shaping BW */
#define TM_INF_SHP_BW 0xFFFFFFFF

/** No Drop  */
#define TM_NO_DROP_PROFILE  0

#define TM_MAX_DROP_THRESHOLD 0xFFFFFFFF

/* TM INVALID constants */
/** 32 bit Invalid data indicator */
#define TM_INVAL         0xFFFFFFFF


/* Status constants */
/** Enable indicator */
#define TM_ENABLE  1
/** Disable indicator */
#define TM_DISABLE 0


/** Maximum Bandwidth (in Kbits/sec) */
#define TM_MAX_BW 100000000 /* 100GBit/sec */

/** Maximum Burst (in Kbytes) */
#define TM_MAX_BURST (100 * 1024)

enum
{ 
    /** Eligible function for dequeue disable (the node is not eligible)
    *  - the last function in eligible function array - reserved for queues and nodes**/
	TM_ELIG_DEQ_DISABLE = 63,
	/** The size of  eligible functions array  **/
};


enum  elig_func_node {
    /** Eligible Priority 1 **/
    TM_ELIG_N_PRIO1 = 0,
    /** Eligible function priority 5 **/
    TM_ELIG_N_PRIO5 = 1,
    /** Eligible function shaping **/
    TM_ELIG_N_SHP = 2,
    /** Eligible function min shaping **/
    TM_ELIG_N_MIN_SHP = 3,
    /** Eligible function propagated priority **/
    TM_ELIG_N_PPA = 4,
    /** Eligible function propagated priority with strict priority
     * (priority = 5) **/
    TM_ELIG_N_PPA_SP = 5,
    /** Eligible function propagated with shaping  **/
    TM_ELIG_N_PPA_SHP = 6,
    /** Eligible function propagated priority with strict priority
     * (priority = 5) with min shaper **/
    TM_ELIG_N_PPA_SP_MIN_SHP = 7,
    /** Eligible function propagated priority and shaping with shaper
     * ignore for propagated priority = 3 **/
    TM_ELIG_N_PPA_SHP_IGN = 8,
    /** Eligible function propagated priority with strict priority
     * (priority = 5) with min shaper with shaper
     * ignore for propagated priority = 3 **/
    TM_ELIG_N_PPA_MIN_SHP_SP_IGN = 9,
    /** Node is always eligible in priority 0 **/
    TM_ELIG_N_FP0 = 10,
    /** Node is always eligible in priority 1 **/
    TM_ELIG_N_FP1 = 11,
    /** Node is always eligible in priority 2 **/
    TM_ELIG_N_FP2 = 12,
    /** Node is always eligible in priority 3 **/
    TM_ELIG_N_FP3 = 13,
    /** Node is always eligible in priority 4 **/
    TM_ELIG_N_FP4 = 14,
    /** Node is always eligible in priority 5 **/
    TM_ELIG_N_FP5 = 15,
    /** Node is always eligible in priority 6 **/
    TM_ELIG_N_FP6 = 16,
    /** Node is always eligible in priority 7 **/
    TM_ELIG_N_FP7 = 17,
    /** Node is eligible in priority 0 when minTB is positive **/
    TM_ELIG_N_MIN_SHP_FP0 = 18,
    /** Node is eligible in priority 1 when minTB is positive **/
    TM_ELIG_N_MIN_SHP_FP1 = 19,
    /** Node is eligible in priority 2 when minTB is positive **/
    TM_ELIG_N_MIN_SHP_FP2 = 20,
    /** Node is eligible in priority 3 when minTB is positive **/
    TM_ELIG_N_MIN_SHP_FP3 = 21,
    /** Node is eligible in priority 4 when minTB is positive **/
    TM_ELIG_N_MIN_SHP_FP4 = 22,
    /** Node is eligible in priority 5 when minTB is positive **/
    TM_ELIG_N_MIN_SHP_FP5 = 23,
    /** Node is eligible in priority 6 when minTB is positive **/
    TM_ELIG_N_MIN_SHP_FP6 = 24,
    /** Node is eligible in priority 7 when minTB is positive **/
    TM_ELIG_N_MIN_SHP_FP7 = 25,
    /** Eligible function propagated with MaxTB include MinTB shaping **/
    TM_ELIG_N_MAX_INC_MIN_SHP = 26,
    /** Node is always eligible in propagated priority **/
    TM_ELIG_N_PP = 27,
    /** Node is eligible when MinTB is positive **/
    TM_ELIG_N_MIN_SHP_PP = 28,
    /** Eligible function with propagated priority and shaping **/
    TM_ELIG_N_PP_SHP = 29,
    /** Eligible function with min max shaping **/
    TM_ELIG_N_SHP_4P_MIN_4P_MAX = 30,
    /** Eligible function with min max shaping, the pp influences
     * which shaper will be used **/
    TM_ELIG_N_SHP_PP_TB = 31,
    /** Eligible function with min max shaping,SchedPrio0 = Prop0(Min)+Prop0..7(Max)
     * SchedPrio1..7 = SchedPrio1..7(Min), PropOut = Prop  **/
    TM_ELIG_N_SHP_PP_MAX_TB_0 = 32
/** PPA - Propagated priority according to RevA . i.e. - only
 * propagated priority 0...3 are supported. when node gets propagated
 * priority 4...7 it is not eligible **/
};

/** Eligible functions for queue nodes enumerator */
enum elig_func_queue {
    /** Eligible function priority 0 **/
    TM_ELIG_Q_PRIO0 = 0,
    /** Eligible function priority 1 **/
    TM_ELIG_Q_PRIO1 = 1,
    /** Eligible function priority 2 **/
    TM_ELIG_Q_PRIO2 = 2,
    /** Eligible function priority 3 **/
    TM_ELIG_Q_PRIO3 = 3,
    /** Eligible function min shaping priority 0 **/
    TM_ELIG_Q_MIN_SHP_PRIO0 = 4,
    /** Eligible function min shaping priority 1 **/
    TM_ELIG_Q_MIN_SHP_PRIO1 = 5,
    /** Eligible function min shaping priority 2 **/
    TM_ELIG_Q_MIN_SHP_PRIO2 = 6,
    /** Eligible function min shaping priority 3 **/
    TM_ELIG_Q_MIN_SHP_PRIO3 = 7,
    /** Eligible function priority 4 **/
    TM_ELIG_Q_PRIO4 = 8,
    /** Eligible function priority 5 **/
    TM_ELIG_Q_PRIO5 = 9,
    /** Eligible function priority 6 **/
    TM_ELIG_Q_PRIO6 = 10,
    /** Eligible function priority 7 **/
    TM_ELIG_Q_PRIO7 = 11,
    /** Eligible function min shaping priority 4 **/
    TM_ELIG_Q_MIN_SHP_PRIO4 = 12,
    /** Eligible function min shaping priority 5 **/
    TM_ELIG_Q_MIN_SHP_PRIO5 = 13,
    /** Eligible function min shaping priority 6 **/
    TM_ELIG_Q_MIN_SHP_PRIO6 = 14,
    /** Eligible function min shaping priority 7 **/
    TM_ELIG_Q_MIN_SHP_PRIO7 = 15,
    /** Eligible function with shaping **/
    TM_ELIG_Q_SHP = 16,
    /** Eligible function propagated with MaxTB include MinTB shaping **/
    TM_ELIG_Q_MAX_INC_MIN_SHP = 17,
    /** Eligible function min max shaping, scheduling priority 0/0, propagated priority 0/0 **/
    TM_ELIG_Q_SHP_SCHED00_PROP00 = 18,
    /** Eligible function min max shaping, scheduling priority 1/0, propagated priority 1/0 **/
    TM_ELIG_Q_SHP_SCHED10_PROP10 = 19,
    /** Eligible function min max shaping, scheduling priority 2/0, propagated priority 2/0 **/
    TM_ELIG_Q_SHP_SCHED20_PROP20 = 20,
    /** Eligible function min max shaping, scheduling priority 3/0, propagated priority 3/0 **/
    TM_ELIG_Q_SHP_SCHED30_PROP30 = 21,
    /** Eligible function min max shaping, scheduling priority 4/0, propagated priority 4/0 **/
    TM_ELIG_Q_SHP_SCHED40_PROP40 = 22,
    /** Eligible function min max shaping, scheduling priority 5/0, propagated priority 5/0 **/
    TM_ELIG_Q_SHP_SCHED50_PROP50 = 23,
    /** Eligible function min max shaping, scheduling priority 6/0, propagated priority 6/0 **/
    TM_ELIG_Q_SHP_SCHED60_PROP60 = 24,
    /** Eligible function min max shaping, scheduling priority 7/0, propagated priority 7/0 **/
    TM_ELIG_Q_SHP_SCHED70_PROP70 = 25
};


/*---------------------- Enumerated Types---------------- */

/** TM levels */
enum tm_level
{
    Q_LEVEL = 0, /**< Queue Level */
    A_LEVEL,     /**< A-nodes Level */
    B_LEVEL,     /**< B-nodes Level */
    C_LEVEL,     /**< C-nodes Level */
    P_LEVEL      /**< Ports Level */
};

#define	PARENT_LEVEL(level) level+1
#define	CHILD_LEVEL(level) level-1

/** Port's physical bandwidth */
enum tm_port_bw {
    TM_1G_PORT = 0, /**< 1G bit/sec */
    TM_2HG_PORT,    /**< 2.5G bit/sec*/
    TM_10G_PORT,    /**< 10G bit/sec */
    TM_40G_PORT,    /**< 40G bit/sec */
    TM_50G_PORT,    /**< 50G bit/sec */
    TM_100G_PORT    /**< 100G bit/sec */
};

/** Token bucket usage */
enum token_bucket {
    MIN_TOKEN_BUCKET = 0, /**< Use Min token bucket */
    MAX_TOKEN_BUCKET      /**< Use Max tiken bucket */
};

/** TM Port Basic Periodic update rate */
enum tm_port_update_rate {
    TM_PORT_RATE_1M = 1, /**< 1MHz */
    TM_MIN_PORT_RATE = TM_PORT_RATE_1M, /**< Port Minimum update rate */
    TM_PORT_RATE_2M,     /**< 2MHz */
    TM_PORT_RATE_3M,     /**< 3MHz */
    TM_PORT_RATE_4M,     /**< 4MHz */
    TM_PORT_RATE_5M,     /**< 5MHz */
    TM_MAX_PORT_RATE = TM_PORT_RATE_5M  /**< Port Maximum update rate */
};

/** Drop WRED/CATD mode */
enum tm_drop_mode {
    DISABLED = 0,               /**< Both modes are disabled */
    REGULAR_TD_ONLY = DISABLED, /**< No WRED or CATD (td_limit only) */
    WRED = 1,                   /**< WRED */
    CATD = 2                    /**< Color Aware TD */
};

/** Max Probability Mode */
enum tm_max_prob_mode {
    TM_MAX_PROB_100 = 0,   /* 100%  */
    TM_MAX_PROB_50,        /*  50%  */
    TM_MAX_PROB_25,        /*  25%  */
    TM_MAX_PROB_12H,       /* 12.5% */
};

/** TM2TM channel */
enum tm2tm_channel {
    TM2TM_NODE_CH = 0,
    TM2TM_PORT_CH
};

/** TM2TM Port/Node channel's mode */
enum tm2tm_mode {
    TM2TM_WRED = 0,
    TM2TM_BP
};


/* Max number of packet colors  : Green, Yellow , Red */
#define TM_MAX_PACKET_COLORS	3

/** Number of colors */
enum tm_color_num {
    TM_1_COLORS = 0,
    TM_2_COLORS,
    TM_3_COLORS
};

/** Drop Probability Source */
enum tm_dp_source {
    TM_AQL = 0,
    TM_QL
};

/** RED curve described as a vector of 32 points. Max value is 15 or 63. */
typedef uint8_t tm_wred_vec[32];

/*------------------- Global Parameters Data structures-----------------*/

/** tm lib init params */
struct tm_lib_init_params {
    uint32_t tm_mtu;            /**< tm MTU unit, if 0 then MTU is TM_MTU */
    /* ... more tm lib init params here */
}; 

/** Periodic Update Rate configuration params per level */
struct tm_level_periodic_params 
{
    uint8_t per_state;  /** Periodic shaper update Enable/Disable */
    uint8_t shaper_dec;   /**< Shaper Decoupling Enable/Disable */
};



struct  tm_ca_td_drop_profile_params
{
        uint32_t catdBytes[TM_MAX_PACKET_COLORS];    /*Color Aware TD in Bytes Per Color  (APPLICABLE RANGES: 0..max drop threshold) */
};

struct  tm_wred_drop_profile_params
{
	uint8_t	aqlExponent;    /*Exponent value to determine new average queue length
		                        as function of previous average and the current size
		                        of the queue
		                        (APPLICABLE RANGES: 0..15)
							*/
	uint8_t	curveIndex[TM_MAX_PACKET_COLORS] ;   /* RED curve index per color   (APPLICABLE RANGES: 0..7)  */
	uint8_t	dpCurveScale[TM_MAX_PACKET_COLORS];  /* Drop Probabiliry Curve scaling:
													0 - no scaling, DP[i] = DPCurve[i].
													2 colors - scale down from DPCurve[i] * 1/64 to DPCurve[i] * 63/64
													3 colors - scale down from DPCurve[i] * 1/16 to DPCurve[i] * 15/16
													(APPLICABLE RANGES: 2 colors: 0..63, 3 colors: 0..15)
													*/
	uint32_t	caWredTdMinThreshold[TM_MAX_PACKET_COLORS];  /* Color Aware Wred Min TD in Bytes Per Color      (APPLICABLE RANGES: 0..caWredTdMaxThreshold[i]) */
	uint32_t	caWredTdMaxThreshold[TM_MAX_PACKET_COLORS];  /* Color Aware Max Wred TD in Bytes Per Color      (APPLICABLE RANGES: caWredTdMinThreshold[i]..max drop threshold) */
};

/* finally structure which group the 3 drop modes */
struct tm_drop_profile_params {
	enum tm_drop_mode drop_mode;   /**< Color Aware/TD/WRED */
    uint32_t cbtdBytes;   /*Color Blind TD in Bytes   (APPLICABLE RANGES: 0..max drop threshold) */
    struct tm_ca_td_drop_profile_params		caTdDp;
    struct tm_wred_drop_profile_params		wredDp;
};

/** Drop profile hw info 
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
};



/** Shared Shaper Profile configuration structure */
struct tm_shaping_profile_params {
    uint32_t cir_bw; /**< CIR BW in Kbits/sec */
    uint32_t cbs;    /**< CBS in kbytes */
    uint32_t eir_bw; /**< EIR BW in Kbits/sec */
    uint32_t ebs;    /**< EBS in kbytes */
};


/*----------------- Nodes Parameters Data Structures---------------*/
/* Note: only drop mode 0 is supported in current version.
 *  Drop profile reference fields are present only for queues and
 *  ports
*/

/** Queue Parameters Data Structure */
struct tm_queue_params {
    uint32_t shaping_profile_ref;  /**< Index of Shaping profile */
    uint16_t quantum;              /**< Queue DWRR Quantum in TM_NODE_QUANTUM_UNIT */
    uint16_t wred_profile_ref;     /**< Index of Drop profile */
    uint8_t elig_prio_func_ptr;    /**< Eligible Priority Function pointer */
};


/** A-Node Parameters Data Structure */
struct tm_a_node_params {
    uint32_t shaping_profile_ref;  /**< Index of Shaping profile */
    uint16_t quantum;              /**< DWRR Quantum in TM_NODE_QUANTUM_UNIT */
    uint16_t wred_profile_ref;     /**< Index of Drop profile */
    uint8_t dwrr_priority[8];      /**< DWRR Priority for Queue Scheduling */
    uint8_t elig_prio_func_ptr;    /**< Eligible Priority Function pointer */
    uint32_t num_of_children;      /**< Number of children nodes */
};


/** B-Node Parameters Data Structure */
struct tm_b_node_params {
    uint32_t shaping_profile_ref;  /**< Index of Shaping profile */
    uint16_t quantum;              /**< DWRR Quantum in TM_NODE_QUANTUM_UNIT */
    uint16_t wred_profile_ref;     /**< Index of Drop profile */
    uint8_t dwrr_priority[8];      /**< DWRR Priority for A-Node Scheduling */
    uint8_t elig_prio_func_ptr;    /**< Eligible Priority Function pointer */
    uint16_t num_of_children;      /**< Number of children nodes */
};


/** C-Node Parameters Data Structure */
struct tm_c_node_params {
    uint32_t shaping_profile_ref;  /**< Index of Shaping profile */
    uint16_t quantum;              /**< DWRR Quantum in TM_NODE_QUANTUM_UNIT */
    uint8_t wred_profile_ref[8];  /**< Index of Drop profile per CoS */
    uint8_t dwrr_priority[8];      /**< DWRR Priority for B-Node Scheduling */
    uint8_t wred_cos;              /**< C-node CoS bit map for WRED */
    uint8_t elig_prio_func_ptr;    /**< Eligible Priority Function pointer */
    uint16_t num_of_children;      /**< Number of children nodes */
};


/** Port Parameters Data Structure */
struct tm_port_params {
    uint32_t cir_bw;               /**< CIR BW in Kbits/sec */
    uint32_t eir_bw;               /**< EIR BW in Kbits/sec */
    uint32_t cbs;                  /**< CBS in bytes */
    uint32_t ebs;                  /**< EBS in bytes */
    uint16_t quantum[8];           /**< DWRR Quantum for each instance in TM_PORT_QUANTUM_UNIT */
    uint8_t wred_profile_ref;     /**< Index of Drop profile */
    uint8_t dwrr_priority[8];      /**< DWRR Priority for C-Node Scheduling */
    uint8_t elig_prio_func_ptr;    /**< Eligible Priority Function pointer */
    uint16_t num_of_children;      /**< Number of children nodes */
};

struct tm_port_drop_per_cos {
    uint8_t wred_cos;              /**< C-node CoS bit map for WRED */
    uint8_t wred_profile_ref[8];   /**< Index of Drop profile per CoS */
};


/** Port status data structure */
struct tm_port_status {
    uint32_t max_bucket_level;  /**< Maximal Shaper Bucket level */
    uint32_t min_bucket_level;  /**< Minimal Shaper Bucket level */
    uint32_t deficit[8];        /**< DWRR Deficit per instance */
};


/** Node status data structure */
struct tm_node_status {
    uint32_t max_bucket_level;  /**< Maximal Shaper Bucket level */
    uint32_t min_bucket_level;  /**< Minimal Shaper Bucket level */
    uint32_t deficit;           /**< DWRR Deficit */
};



/** TM Blocks Error Information */
struct tm_error_info {
    uint16_t error_counter; /**< TM Block Error counter */
    uint16_t exception_counter; /**< TM Block Exception Counter */
};

/** TM Blocks Error Status Information */
struct tm_units_error_status_info {
    uint32_t qmgr_status;
    uint32_t drop_status;
    uint32_t sched_status;
    uint32_t rcb_status;
};

struct tm_bap_unit_error_status_info {
	uint32_t bap_status;
};


/** TM2TM External Headers */
struct tm_ext_hdr {
    uint8_t size;               /**< only fixed values - 3, 7, 11 or 15 */
    uint8_t	content[32];		/**< header data */
};

/** TM2TM Control Packet Structure */
struct tm_ctrl_pkt_str {
    uint8_t ports;	/**< Ports */
    uint8_t nodes;	/**< Nodes */
};

/** TM2TM Delta Range Mapping to Priority */
struct tm_delta_range {
    uint8_t upper_range0;	/**< Range 0 */
    uint8_t upper_range1;	/**< Range 1 */
    uint8_t upper_range2;	/**< Range 2 */
};

#ifdef VIRTUAL_LAYER_TO_REMOVE_IN_FUTURE
#else
	/** Reshuffling index/range change structure */
	struct tm_tree_change {
		uint32_t index; /**< Index of changed parent node */
		uint32_t old_index;   /**< Old index/range */
		uint32_t new_index;   /**< New index/range */
		struct tm_tree_change *next; /**< Pointer to the next change */
	};
#endif

/** Eligible Priority Function Data structures */
struct tm_elig_prio_func_out {
	uint8_t max_tb;             /**< Use Max Token Bucket   */
    uint8_t min_tb;             /**< Use Min Token Bucket   */
    uint8_t prop_prio;          /**< Propagated priority    */
    uint8_t sched_prio;         /**< Scheduling priority    */
    uint8_t elig;               /**< Eligibility            */
};

/** Eligible Priority Function storage */
union tm_elig_prio_func{
	struct tm_elig_prio_func_out queue_elig_prio_func[4];		/**< Eligible Priority Functions for queues   */
	struct tm_elig_prio_func_out node_elig_prio_func[8][4];		/**< Eligible Priority Functions for intermediate nodes   */
};




struct tm_quantum_limits /* applicable for port or nodes  quantum limits */
{
    uint32_t minQuantum;	/* minimal possible quantum value in bytes */
    uint32_t maxQuantum;	/* maximal possible quantum value in bytes */
    uint32_t resolution;	/* quantum granularity in bytes*/
};


typedef void * tm_handle;


struct tm_tree_structure
{
	/* TM tree structure parameters */
	uint16_t	numOfQueues;
	uint16_t	numOfAnodes;
	uint16_t	numOfBnodes;
	uint16_t	numOfCnodes;
	uint16_t	numOfPorts;
	/* default TM tree mapping */
	uint8_t		queuesToAnode;
	uint8_t		aNodesToBnode;
	uint8_t		bNodesToCnode;
	uint8_t		cNodesToPort;
	uint8_t		installedQueuesPerPort;
};



#endif   /* TM_DEFS_H */

