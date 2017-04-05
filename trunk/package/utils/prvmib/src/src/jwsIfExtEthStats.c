/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.scalar.conf 11805 2005-01-07 09:37:18Z dts12 $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "jwsIfExtEthStats.h"
#include "stdlib.h"
#include "jws_ubus_if.h"

/** Initializes the jwsIfExtEthStats module */
void
init_jwsIfExtEthStats(void)
{
    static oid jwsIfExtEthStatsClearAction_oid[] = { 1,3,6,1,4,1,33444,2,2,2,2,1 };

  DEBUGMSGTL(("jwsIfExtEthStats", "Initializing\n"));

    netsnmp_register_scalar(
        netsnmp_create_handler_registration("jwsIfExtEthStatsClearAction", handle_jwsIfExtEthStatsClearAction,
                               jwsIfExtEthStatsClearAction_oid, OID_LENGTH(jwsIfExtEthStatsClearAction_oid),
                               HANDLER_CAN_RWRITE
        ));
}

int
handle_jwsIfExtEthStatsClearAction(netsnmp_mib_handler *handler,
                          netsnmp_handler_registration *reginfo,
                          netsnmp_agent_request_info   *reqinfo,
                          netsnmp_request_info         *requests)
{
    int ret;
    unsigned int data = 0;
    char str[100] = {0};
    /* We are never called for a GETNEXT if it's registered as a
       "instance", as it's "magically" handled for us.  */

    /* a instance handler also only hands us one request at a time, so
       we don't need to loop over a list of requests; we'll only get one. */
    
    switch(reqinfo->mode) {

        case MODE_GET:
            ret = ubus_invoke_command("eth_stats", "eth_stats_get", get_ret_handler_cb, 
                                "{\"get_args\":[\"clear\"]}");
            if (ret != 0)
            {
                return SNMP_ERR_NOSUCHNAME;
            }
            data = atoi(jws_read_buf_str());
            
            snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                                     (u_char *) &data,
                                     sizeof(data));
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
            //if (/* XXX if malloc, or whatever, failed: */) {
            //    netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_RESOURCEUNAVAILABLE);
            //}
            break;

        case MODE_SET_FREE:
            /* XXX: free resources allocated in RESERVE1 and/or
               RESERVE2.  Something failed somewhere, and the states
               below won't be called. */
            break;

        case MODE_SET_ACTION:
            data = *requests->requestvb->val.integer - 1;

            snprintf(str, sizeof(str), "{\"set_args\":[{\"clear\":\"%d\"}]}", data);
            ret = ubus_invoke_command("eth_stats", "eth_stats_set", set_ret_handler_cb, str);
            if (ret != 0)
            {
                    return SNMP_ERR_NOSUCHNAME;
            }
            /* XXX: perform the value change here */
            //if (/* XXX: error? */) {
            //    netsnmp_set_request_error(reqinfo, requests, /* some error */);
            //}
            break;

        case MODE_SET_COMMIT:
            /* XXX: delete temporary storage */
            //if (/* XXX: error? */) {
                /* try _really_really_ hard to never get to this point */
            //    netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_COMMITFAILED);
            //}
            break;

        case MODE_SET_UNDO:
            /* XXX: UNDO and return to previous value for the object */
            //if (/* XXX: error? */) {
                /* try _really_really_ hard to never get to this point */
            //    netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_UNDOFAILED);
            //}
            break;

        default:
            /* we should never get here, so this is a really bad error */
            snmp_log(LOG_ERR, "unknown mode (%d) in handle_jwsIfExtEthStatsClearAction\n", reqinfo->mode );
            return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}
