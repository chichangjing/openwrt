/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.scalar.conf 11805 2005-01-07 09:37:18Z dts12 $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "jwsDevNetworkConfig.h"

#include "jws_ubus_if.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include "jwslog.h"

/** Initializes the jwsDevNetworkConfig module */
void
init_jwsDevNetworkConfig(void)
{
    static oid jwsNetworkIPAddress_oid[] = { 1,3,6,1,4,1,33444,2,2,1,3,1 };
    static oid jwsNetworkSubnetMask_oid[] = { 1,3,6,1,4,1,33444,2,2,1,3,2 };
    static oid jwsNetworkDefaultGateway_oid[] = { 1,3,6,1,4,1,33444,2,2,1,3,3 };
    static oid jwsNetworkProtocol_oid[] = { 1,3,6,1,4,1,33444,2,2,1,3,4 };

  DEBUGMSGTL(("jwsDevNetworkConfig", "Initializing\n"));

    netsnmp_register_scalar(
        netsnmp_create_handler_registration("jwsNetworkIPAddress", handle_jwsNetworkIPAddress,
                               jwsNetworkIPAddress_oid, OID_LENGTH(jwsNetworkIPAddress_oid),
                               HANDLER_CAN_RWRITE
        ));
    netsnmp_register_scalar(
        netsnmp_create_handler_registration("jwsNetworkSubnetMask", handle_jwsNetworkSubnetMask,
                               jwsNetworkSubnetMask_oid, OID_LENGTH(jwsNetworkSubnetMask_oid),
                               HANDLER_CAN_RWRITE
        ));
    netsnmp_register_scalar(
        netsnmp_create_handler_registration("jwsNetworkDefaultGateway", handle_jwsNetworkDefaultGateway,
                               jwsNetworkDefaultGateway_oid, OID_LENGTH(jwsNetworkDefaultGateway_oid),
                               HANDLER_CAN_RWRITE
        ));
    netsnmp_register_scalar(
        netsnmp_create_handler_registration("jwsNetworkProtocol", handle_jwsNetworkProtocol,
                               jwsNetworkProtocol_oid, OID_LENGTH(jwsNetworkProtocol_oid),
                               HANDLER_CAN_RWRITE
        ));
}

int
handle_jwsNetworkIPAddress(netsnmp_mib_handler *handler,
                          netsnmp_handler_registration *reginfo,
                          netsnmp_agent_request_info   *reqinfo,
                          netsnmp_request_info         *requests)
{
    int ret;
    in_addr_t ipaddr;
    struct in_addr _ipaddr;
    char str[100];
    /* We are never called for a GETNEXT if it's registered as a
       "instance", as it's "magically" handled for us.  */

    /* a instance handler also only hands us one request at a time, so
       we don't need to loop over a list of requests; we'll only get one. */
    
    switch(reqinfo->mode) {

        case MODE_GET:
            ret = ubus_invoke_command("network2", "network2_get", get_ret_handler_cb, 
                                "{\"get_args\":[\"ipaddr\"]}");
            if (ret != 0) {
                return SNMP_ERR_NOSUCHNAME;
            }

            ipaddr = inet_addr(jws_read_buf_str());
            snmp_set_var_typed_value(requests->requestvb, ASN_IPADDRESS,
                                     (u_char *) &ipaddr,
                                     4);
            break;

        /*
         * SET REQUEST
         *
         * multiple states in the transaction.  See:
         * http://www.net-snmp.org/tutorial-5/toolkit/mib_module/set-actions.jpg
         */
        case MODE_SET_RESERVE1:
                /* or you could use netsnmp_check_vb_type_and_size instead */
            ret = netsnmp_check_vb_type(requests->requestvb, ASN_IPADDRESS);
            if ( ret != SNMP_ERR_NOERROR ) {
                netsnmp_set_request_error(reqinfo, requests, ret );
            }
            break;

        case MODE_SET_RESERVE2:
            /* XXX malloc "undo" storage buffer */
            if (0/* XXX if malloc, or whatever, failed: */) {
                netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_RESOURCEUNAVAILABLE);
            }
            break;

        case MODE_SET_FREE:
            /* XXX: free resources allocated in RESERVE1 and/or
               RESERVE2.  Something failed somewhere, and the states
               below won't be called. */
            break;

        case MODE_SET_ACTION:
            memcpy((char *)&_ipaddr,  (char *)requests->requestvb->val.integer, 4);
            snprintf(str, sizeof(str), "{\"set_args\":[{\"ipaddr\":\"%s\"}]}", inet_ntoa(_ipaddr));
            ret = ubus_invoke_command("network2", "network2_set", set_ret_handler_cb, str);
            if (ret != 0) {
                return SNMP_ERR_NOSUCHNAME;
            }
            /* XXX: perform the value change here */
            if (0/* XXX: error? */) {
                //netsnmp_set_request_error(reqinfo, requests, /* some error */);
            }
            break;

        case MODE_SET_COMMIT:
            /* XXX: delete temporary storage */
            if (0/* XXX: error? */) {
                /* try _really_really_ hard to never get to this point */
                netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_COMMITFAILED);
            }
            break;

        case MODE_SET_UNDO:
            /* XXX: UNDO and return to previous value for the object */
            if (0/* XXX: error? */) {
                /* try _really_really_ hard to never get to this point */
                netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_UNDOFAILED);
            }
            break;

        default:
            /* we should never get here, so this is a really bad error */
            snmp_log(LOG_ERR, "unknown mode (%d) in handle_jwsNetworkIPAddress\n", reqinfo->mode );
            return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}
int
handle_jwsNetworkSubnetMask(netsnmp_mib_handler *handler,
                          netsnmp_handler_registration *reginfo,
                          netsnmp_agent_request_info   *reqinfo,
                          netsnmp_request_info         *requests)
{
    int ret;
    in_addr_t netmask;
    struct in_addr _netmask;
    char str[100];
    /* We are never called for a GETNEXT if it's registered as a
       "instance", as it's "magically" handled for us.  */

    /* a instance handler also only hands us one request at a time, so
       we don't need to loop over a list of requests; we'll only get one. */
    
    switch(reqinfo->mode) {

        case MODE_GET:
            ret = ubus_invoke_command("network2", "network2_get", get_ret_handler_cb, 
                                "{\"get_args\":[\"netmask\"]}");
            if (ret != 0) {
                return SNMP_ERR_NOSUCHNAME;
            }

            netmask = inet_addr(jws_read_buf_str());
            snmp_set_var_typed_value(requests->requestvb, ASN_IPADDRESS,
                                     (u_char *) &netmask,
                                     4);
            break;

        /*
         * SET REQUEST
         *
         * multiple states in the transaction.  See:
         * http://www.net-snmp.org/tutorial-5/toolkit/mib_module/set-actions.jpg
         */
        case MODE_SET_RESERVE1:
                /* or you could use netsnmp_check_vb_type_and_size instead */
            ret = netsnmp_check_vb_type(requests->requestvb, ASN_IPADDRESS);
            if ( ret != SNMP_ERR_NOERROR ) {
                netsnmp_set_request_error(reqinfo, requests, ret );
            }
            break;

        case MODE_SET_RESERVE2:
            /* XXX malloc "undo" storage buffer */
            if (0/* XXX if malloc, or whatever, failed: */) {
                netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_RESOURCEUNAVAILABLE);
            }
            break;

        case MODE_SET_FREE:
            /* XXX: free resources allocated in RESERVE1 and/or
               RESERVE2.  Something failed somewhere, and the states
               below won't be called. */
            break;

        case MODE_SET_ACTION:
            memcpy((char *)&_netmask,  (char *)requests->requestvb->val.integer, 4);
            snprintf(str, sizeof(str), "{\"set_args\":[{\"netmask\":\"%s\"}]}", inet_ntoa(_netmask));
            ret = ubus_invoke_command("network2", "network2_set", set_ret_handler_cb, str);
            if (ret != 0) {
                return SNMP_ERR_NOSUCHNAME;
            }
            /* XXX: perform the value change here */
            if (0/* XXX: error? */) {
                //netsnmp_set_request_error(reqinfo, requests, /* some error */);
            }
            break;

        case MODE_SET_COMMIT:
            /* XXX: delete temporary storage */
            if (0/* XXX: error? */) {
                /* try _really_really_ hard to never get to this point */
                netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_COMMITFAILED);
            }
            break;

        case MODE_SET_UNDO:
            /* XXX: UNDO and return to previous value for the object */
            if (0/* XXX: error? */) {
                /* try _really_really_ hard to never get to this point */
                netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_UNDOFAILED);
            }
            break;

        default:
            /* we should never get here, so this is a really bad error */
            snmp_log(LOG_ERR, "unknown mode (%d) in handle_jwsNetworkSubnetMask\n", reqinfo->mode );
            return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}
int
handle_jwsNetworkDefaultGateway(netsnmp_mib_handler *handler,
                          netsnmp_handler_registration *reginfo,
                          netsnmp_agent_request_info   *reqinfo,
                          netsnmp_request_info         *requests)
{
    int ret;
    in_addr_t gateway;
    struct in_addr _gateway;
    char str[100];
    /* We are never called for a GETNEXT if it's registered as a
       "instance", as it's "magically" handled for us.  */

    /* a instance handler also only hands us one request at a time, so
       we don't need to loop over a list of requests; we'll only get one. */
    
    switch(reqinfo->mode) {

        case MODE_GET:
            ret = ubus_invoke_command("network2", "network2_get", get_ret_handler_cb, 
                                "{\"get_args\":[\"gateway\"]}");
            if (ret != 0) {
                return SNMP_ERR_NOSUCHNAME;
            }

            gateway = inet_addr(jws_read_buf_str());
            snmp_set_var_typed_value(requests->requestvb, ASN_IPADDRESS,
                                     (u_char *) &gateway,
                                     4);
            break;

        /*
         * SET REQUEST
         *
         * multiple states in the transaction.  See:
         * http://www.net-snmp.org/tutorial-5/toolkit/mib_module/set-actions.jpg
         */
        case MODE_SET_RESERVE1:
                /* or you could use netsnmp_check_vb_type_and_size instead */
            ret = netsnmp_check_vb_type(requests->requestvb, ASN_IPADDRESS);
            if ( ret != SNMP_ERR_NOERROR ) {
                netsnmp_set_request_error(reqinfo, requests, ret );
            }
            break;

        case MODE_SET_RESERVE2:
            /* XXX malloc "undo" storage buffer */
            if (0/* XXX if malloc, or whatever, failed: */) {
                netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_RESOURCEUNAVAILABLE);
            }
            break;

        case MODE_SET_FREE:
            /* XXX: free resources allocated in RESERVE1 and/or
               RESERVE2.  Something failed somewhere, and the states
               below won't be called. */
            break;

        case MODE_SET_ACTION:
            memcpy((char *)&_gateway,  (char *)requests->requestvb->val.integer, 4);
            snprintf(str, sizeof(str), "{\"set_args\":[{\"gateway\":\"%s\"}]}", inet_ntoa(_gateway));
            ret = ubus_invoke_command("network2", "network2_set", set_ret_handler_cb, str);
            if (ret != 0) {
                return SNMP_ERR_NOSUCHNAME;
            }
            /* XXX: perform the value change here */
            if (0/* XXX: error? */) {
                //netsnmp_set_request_error(reqinfo, requests, /* some error */);
            }
            break;

        case MODE_SET_COMMIT:
            /* XXX: delete temporary storage */
            if (0/* XXX: error? */) {
                /* try _really_really_ hard to never get to this point */
                netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_COMMITFAILED);
            }
            break;

        case MODE_SET_UNDO:
            /* XXX: UNDO and return to previous value for the object */
            if (0/* XXX: error? */) {
                /* try _really_really_ hard to never get to this point */
                netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_UNDOFAILED);
            }
            break;

        default:
            /* we should never get here, so this is a really bad error */
            snmp_log(LOG_ERR, "unknown mode (%d) in handle_jwsNetworkDefaultGateway\n", reqinfo->mode );
            return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}
int
handle_jwsNetworkProtocol(netsnmp_mib_handler *handler,
                          netsnmp_handler_registration *reginfo,
                          netsnmp_agent_request_info   *reqinfo,
                          netsnmp_request_info         *requests)
{
    int ret;
    int protocol;
    char str[100];
    /* We are never called for a GETNEXT if it's registered as a
       "instance", as it's "magically" handled for us.  */

    /* a instance handler also only hands us one request at a time, so
       we don't need to loop over a list of requests; we'll only get one. */
    
    switch(reqinfo->mode) {

        case MODE_GET:
            ret = ubus_invoke_command("network2", "network2_get", get_ret_handler_cb, 
                                "{\"get_args\":[\"protocol\"]}");
            if (ret != 0) {
                return SNMP_ERR_NOSUCHNAME;
            }

            protocol = atoi(jws_read_buf_str());
            snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                                     (u_char *) &protocol,
                                     4);
            break;

        /*
         * SET REQUEST
         *
         * multiple states in the transaction.  See:
         * http://www.net-snmp.org/tutorial-5/toolkit/mib_module/set-actions.jpg
         */
        case MODE_SET_RESERVE1:
                /* or you could use netsnmp_check_vb_type_and_size instead */
            ret = netsnmp_check_vb_type(requests->requestvb, ASN_INTEGER);
            if ( ret != SNMP_ERR_NOERROR ) {
                netsnmp_set_request_error(reqinfo, requests, ret );
            }
            break;

        case MODE_SET_RESERVE2:
            /* XXX malloc "undo" storage buffer */
            if (0/* XXX if malloc, or whatever, failed: */) {
                netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_RESOURCEUNAVAILABLE);
            }
            break;

        case MODE_SET_FREE:
            /* XXX: free resources allocated in RESERVE1 and/or
               RESERVE2.  Something failed somewhere, and the states
               below won't be called. */
            break;

        case MODE_SET_ACTION:
            protocol = *requests->requestvb->val.integer;

            snprintf(str, sizeof(str), "{\"set_args\":[{\"protocol\":\"%d\"}]}", protocol);
            ret = ubus_invoke_command("network2", "network2_set", set_ret_handler_cb, str);
            if (ret != 0)
            {
                    return SNMP_ERR_NOSUCHNAME;
            }
            /* XXX: perform the value change here */
            if (0/* XXX: error? */) {
                //netsnmp_set_request_error(reqinfo, requests, /* some error */);
            }
            break;

        case MODE_SET_COMMIT:
            /* XXX: delete temporary storage */
            if (0/* XXX: error? */) {
                /* try _really_really_ hard to never get to this point */
                netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_COMMITFAILED);
            }
            break;

        case MODE_SET_UNDO:
            /* XXX: UNDO and return to previous value for the object */
            if (0/* XXX: error? */) {
                /* try _really_really_ hard to never get to this point */
                netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_UNDOFAILED);
            }
            break;

        default:
            /* we should never get here, so this is a really bad error */
            snmp_log(LOG_ERR, "unknown mode (%d) in handle_jwsNetworkProtocol\n", reqinfo->mode );
            return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}
