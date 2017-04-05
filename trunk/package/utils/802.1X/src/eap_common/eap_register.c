#include "eap_server/eap_methods.h"
#include "eap_register.h"


/**
 * eap_server_register_methods - Register statically linked EAP server methods
 * Returns: 0 on success, -1 or -2 on failure
 *
 * This function is called at program initialization to register all EAP
 * methods that were linked in statically.
 * 注册所有支持的EAP_SERVER的加密方法
 * 备注：identify类型是特殊的，不属于加密方法，必然被注册
 *       当工作在pass-through模式时，通常任何加密方法都不需要
 */
int eap_server_register_methods(void)
{
	int ret = 0;

	if (ret == 0)
		ret = eap_server_identity_register();   // identify类型的eap方法必然被注册

#ifdef EAP_SERVER_MD5
	if (ret == 0)
		ret = eap_server_md5_register();    // 注册md5类型的eap方法
#endif /* EAP_SERVER_MD5 */

#ifdef EAP_SERVER_TLS
	if (ret == 0)
		ret = eap_server_tls_register();
#endif /* EAP_SERVER_TLS */

#ifdef EAP_SERVER_MSCHAPV2
	if (ret == 0)
		ret = eap_server_mschapv2_register();
#endif /* EAP_SERVER_MSCHAPV2 */

#ifdef EAP_SERVER_PEAP
	if (ret == 0)
		ret = eap_server_peap_register();
#endif /* EAP_SERVER_PEAP */

#ifdef EAP_SERVER_TLV
	if (ret == 0)
		ret = eap_server_tlv_register();
#endif /* EAP_SERVER_TLV */

#ifdef EAP_SERVER_GTC
	if (ret == 0)
		ret = eap_server_gtc_register();
#endif /* EAP_SERVER_GTC */

#ifdef EAP_SERVER_TTLS
	if (ret == 0)
		ret = eap_server_ttls_register();
#endif /* EAP_SERVER_TTLS */

#ifdef EAP_SERVER_SIM
	if (ret == 0)
		ret = eap_server_sim_register();
#endif /* EAP_SERVER_SIM */

#ifdef EAP_SERVER_AKA
	if (ret == 0)
		ret = eap_server_aka_register();
#endif /* EAP_SERVER_AKA */

#ifdef EAP_SERVER_AKA_PRIME
	if (ret == 0)
		ret = eap_server_aka_prime_register();
#endif /* EAP_SERVER_AKA_PRIME */

#ifdef EAP_SERVER_PAX
	if (ret == 0)
		ret = eap_server_pax_register();
#endif /* EAP_SERVER_PAX */

#ifdef EAP_SERVER_PSK
	if (ret == 0)
		ret = eap_server_psk_register();
#endif /* EAP_SERVER_PSK */

#ifdef EAP_SERVER_SAKE
	if (ret == 0)
		ret = eap_server_sake_register();
#endif /* EAP_SERVER_SAKE */

#ifdef EAP_SERVER_GPSK
	if (ret == 0)
		ret = eap_server_gpsk_register();
#endif /* EAP_SERVER_GPSK */

#ifdef EAP_SERVER_VENDOR_TEST
	if (ret == 0)
		ret = eap_server_vendor_test_register();
#endif /* EAP_SERVER_VENDOR_TEST */

#ifdef EAP_SERVER_FAST
	if (ret == 0)
		ret = eap_server_fast_register();
#endif /* EAP_SERVER_FAST */

#ifdef EAP_SERVER_WSC
	if (ret == 0)
		ret = eap_server_wsc_register();
#endif /* EAP_SERVER_WSC */

#ifdef EAP_SERVER_IKEV2
	if (ret == 0)
		ret = eap_server_ikev2_register();
#endif /* EAP_SERVER_IKEV2 */

#ifdef EAP_SERVER_TNC
	if (ret == 0)
		ret = eap_server_tnc_register();
#endif /* EAP_SERVER_TNC */

	return ret;
}
