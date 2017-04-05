/*
 * hostapd / EAP Authenticator state machine internal structures (RFC 4137)
 * Copyright (c) 2004-2007, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#ifndef EAP_I_H
#define EAP_I_H

#include "common/wpabuf.h"
#include "eap_server/eap.h"
#include "eap_common/eap_common.h"

/* RFC 4137 - EAP Standalone Authenticator */

/**
 * struct eap_method - EAP method interface
 * This structure defines the EAP method interface. Each method will need to
 * register its own EAP type, EAP name, and set of function pointers for method
 * specific operations. This interface is based on section 5.4 of RFC 4137.
 */
struct eap_method {
	int vendor;
	EapType method;
	const char *name;

	void * (*init)(struct eap_sm *sm);                                  // eap_identity_init        / eap_md5_init
	void * (*initPickUp)(struct eap_sm *sm);                            // eap_identity_initPickUp
	void (*reset)(struct eap_sm *sm, void *priv);                       // eap_identity_reset       / eap_md5_reset 

	struct wpabuf * (*buildReq)(struct eap_sm *sm, void *priv, uint8_t id);  // eap_identity_buildReq    / eap_md5_buildReq
	int (*getTimeout)(struct eap_sm *sm, void *priv);
	Boolean (*check)(struct eap_sm *sm, void *priv,                     
			 struct wpabuf *respData);                                  // eap_identity_check       / eap_md5_check
	void (*process)(struct eap_sm *sm, void *priv,
			struct wpabuf *respData);                                   // eap_identity_process     / eap_md5_process          
	Boolean (*isDone)(struct eap_sm *sm, void *priv);                   // eap_identity_isDone      / eap_md5_isDone           
	uint8_t * (*getKey)(struct eap_sm *sm, void *priv, size_t *len);
	/* isSuccess is not specified in draft-ietf-eap-statemachine-05.txt,
	 * but it is useful in implementing Policy.getDecision() */
	Boolean (*isSuccess)(struct eap_sm *sm, void *priv);                // eap_identity_isSuccess   / eap_md5_isSuccess

	/**
	 * free - Free EAP method data
	 * @method: Pointer to the method data registered with
	 * eap_server_method_register().
	 *
	 * This function will be called when the EAP method is being
	 * unregistered. If the EAP method allocated resources during
	 * registration (e.g., allocated struct eap_method), they should be
	 * freed in this function. No other method functions will be called
	 * after this call. If this function is not defined (i.e., function
	 * pointer is %NULL), a default handler is used to release the method
	 * data with free(method). This is suitable for most cases.
	 */
	void (*free)(struct eap_method *method);

#define EAP_SERVER_METHOD_INTERFACE_VERSION 1
	/**
	 * version - Version of the EAP server method interface
	 *
	 * The EAP server method implementation should set this variable to
	 * EAP_SERVER_METHOD_INTERFACE_VERSION. This is used to verify that the
	 * EAP method is using supported API version when using dynamically
	 * loadable EAP methods.
	 */
	int version;

	/**
	 * next - Pointer to the next EAP method
	 *  指向下一种EAP方法
	 * This variable is used internally in the EAP method registration code
	 * to create a linked list of registered EAP methods.
	 */
	struct eap_method *next;

	/**
	 * get_emsk - Get EAP method specific keying extended material (EMSK)
	 * @sm: Pointer to EAP state machine allocated with eap_sm_init()
	 * @priv: Pointer to private EAP method data from eap_method::init()
	 * @len: Pointer to a variable to store EMSK length
	 * Returns: EMSK or %NULL if not available
	 *
	 * This function can be used to get the extended keying material from
	 * the EAP method. The key may already be stored in the method-specific
	 * private data or this function may derive the key.
	 */
	uint8_t * (*get_emsk)(struct eap_sm *sm, void *priv, size_t *len);
};

/**
 * struct eap_sm - EAP server state machine data
 * EAP服务器状态机(意味着只供认证系统方使用)
 */
struct eap_sm {
	enum {
		EAP_DISABLED, EAP_INITIALIZE, EAP_IDLE, EAP_RECEIVED,
		EAP_INTEGRITY_CHECK, EAP_METHOD_RESPONSE, EAP_METHOD_REQUEST,
		EAP_PROPOSE_METHOD, EAP_SELECT_ACTION, EAP_SEND_REQUEST,
		EAP_DISCARD, EAP_NAK, EAP_RETRANSMIT, EAP_SUCCESS, EAP_FAILURE,
		EAP_TIMEOUT_FAILURE, EAP_PICK_UP_METHOD,
		EAP_INITIALIZE_PASSTHROUGH, EAP_IDLE2, EAP_RETRANSMIT2,
		EAP_RECEIVED2, EAP_DISCARD2, EAP_SEND_REQUEST2,
		EAP_AAA_REQUEST, EAP_AAA_RESPONSE, EAP_AAA_IDLE,
		EAP_TIMEOUT_FAILURE2, EAP_FAILURE2, EAP_SUCCESS2
	} EAP_state;

	/* Constants */
	int MaxRetrans;     // EAP报文最大的重传次数，默认设置5

	struct eap_eapol_interface eap_if;      // 认证者eap<-->eapol层交互接口

	/* Full authenticator state machine local variables */

	/* Long-term (maintained betwen packets) */
	EapType currentMethod;      // 当前的EAP-TYPE，也可理解为当前使用的EAP方法
	int currentId;              // 当前使用的EAP-ID (0~255)，来源有2处：从旧的currentId中计算得到；从AAA层收到的EAP报文中得到
	enum {
		METHOD_PROPOSED, METHOD_CONTINUE, METHOD_END
	} methodState;              // 当前EAP方法所处的状态
	int retransCount;           // 当前的重传次数
	struct wpabuf *lastReqData; // 保存了最近一次发送给eapol层的eap-req数据，触发重传的时候需要被用到
	int methodTimeout;          // 当前EAP方法的超时值，来源有2种：通过EAP方法自带的getTimeout函数计算得到；通过aaaMethodTimeout赋值，通常为0表示不使用

	/* Short-term (not maintained between packets) */
	Boolean rxResp;         // 如果接收到eap-resp包设置TRUE
	int respId;             // 当前接收到的eap-resp包中的ID
	EapType respMethod;     // 当前接收到的eap-resp包中的TYPE
	int respVendor;         // 当前接收到的eap-resp包中的Vendor (TYPE = 254的EAP报文中使用)
	uint32_t respVendorMethod;   // 当前接收到的eap-resp包中的VendorMethod(TYPE = 254的EAP报文中使用)
	Boolean ignore;         // 此标志位用于决定是否丢弃当前收到的报文
	enum {
		DECISION_SUCCESS, DECISION_FAILURE, DECISION_CONTINUE,
		DECISION_PASSTHROUGH
	} decision;             // SELECT_ACTION状态中使用，存储作出的决定

	/* Miscellaneous variables */
	const struct eap_method *m;         // 指向被采用的 EAP 方法控制块
	/* not defined in RFC 4137 */
	Boolean changed;
	void *eapol_ctx, *msg_ctx;          // 分别指向状态机统一管理块
	struct eapol_callbacks *eapol_cb;   // 指向依赖的eapol层接口函数集合
	void *eap_method_priv;              // 指向所采用的EAP方法携带的私有数据，比如MD5校验数据
	uint8_t *identity;                       // 存储了获得的用户名信息
	size_t identity_len;                // 存储了获得的用户名长度
	/* Whether Phase 2 method should validate identity match */
	int require_identity_match;
	int lastId; /* Identifier used in the last EAP-Packet 最近的EAP包中的ID号，通常来自currentId*/
	struct eap_user *user;              // 指向eap层记录的用户信息管理块
	int user_eap_method_index;
	int init_phase2;
	void *eap_sim_db_priv;
	Boolean backend_auth;       // RADIUS后台认证服务器使能标志，默认不使能
	Boolean update_user;        // 是否更新eap_user信息标志
	int eap_server;             // 是否使用内部集成的EAP认证服务器，默认使用外部RADIUS服务器

	int num_rounds;             // eap层记录的往返次数：发出eap-req 到收到 eap-resp 报文算一次往返
	enum {
		METHOD_PENDING_NONE, METHOD_PENDING_WAIT, METHOD_PENDING_CONT
	} method_pending;

	uint8_t *auth_challenge;
	uint8_t *peer_challenge;


	Boolean start_reauth;

	uint8_t peer_addr[ETH_ALEN];
};

int eap_user_get(struct eap_sm *sm, const uint8_t *identity, size_t identity_len,
		 int phase2);
void eap_sm_process_nak(struct eap_sm *sm, const uint8_t *nak_list, size_t len);

#endif /* EAP_I_H */
