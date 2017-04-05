/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.notify.conf 10110 2004-04-15 12:29:19Z dts12 $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "jwsTrapPowerChannelStatus.h"

#include "jws_ubus_if.h"
#include "stdlib.h"
#include "jwsDevBaseInfo.h"
#include "jwslog.h"

#include "gpio_i2c.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

enum ePowerStatus
{
    POWER_UP = 1,
    POWER_DOWN = 2,
};

static int i2c_fd = -1;
static struct stJwsDevBaseInfo jwsDevBaseInfo;

static oid snmptrap_oid[] = {1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0};

static int i2c_read(const int fd, const unsigned int devaddr,
        const unsigned int regaddr, char *reg_value)
{
    unsigned int value = 0;
    int ret;

    value = ((devaddr & 0xff) << 24)|((regaddr & 0xff) << 16);
    ret = ioctl(fd, GPIO_I2C_READ, &value);
    *reg_value = value & 0xff;

    return ret;
}

/*
 * our initialization routine
 * (to get called, the function name must match init_FILENAME()
 */
void
init_jwsTrapPowerChannelStatus(void)
{
    LogWrite(INF, "file:%s line:%d func:%s\n",
            __FILENAME__, __LINE__, __FUNCTION__);

    getJwsDevBaseInfo(&jwsDevBaseInfo);

    if(0 == strcmp(jwsDevBaseInfo.serial_num, "54-10-01-06-01-01-00-00") ||
       0 == strcmp(jwsDevBaseInfo.serial_num, "54-20-01-06-01-02-00-00") )
    {
        i2c_fd = open("/dev/gpioi2c", 0);
        if (i2c_fd<0)
        {
            LogWrite(ERROR, "file:%s line:%d func:%s, Open gpioi2c dev error!\n",
                    __FILENAME__, __LINE__, __FUNCTION__);
        }
    }else{
        return;
    }

    DEBUGMSGTL(("jwsTrapPortStatus_notification",
                "initializing (setting callback alarm)\n"));
    snmp_alarm_register(1,     /* seconds */
            SA_REPEAT,      /* repeat (every 1 seconds). */
            send_jwsTrapPowerChannelStatus_notification,      /* our callback */
            NULL    /* no callback data needed */
            );

}

int
send_jwsTrapPowerChannelStatus_trap( int channelId, enum ePowerStatus status )
{
    netsnmp_variable_list  *var_list = NULL;
    const oid jwsTrapPowerChannelStatus_oid[] = { 1,3,6,1,4,1,33444,2,2,100,3,2000 };
    const oid jwsTrapPowerChannelId_oid[] = { 1,3,6,1,4,1,33444,2,2,100,2,2000, 0 };
    const oid jwsTrapPowerChannelIdStatus_oid[] = { 1,3,6,1,4,1,33444,2,2,100,2,2001, 0 };

    /*
     * Set the snmpTrapOid.0 value
     */
    snmp_varlist_add_variable(&var_list,
        snmptrap_oid, OID_LENGTH(snmptrap_oid),
        ASN_OBJECT_ID,
        (u_char *)jwsTrapPowerChannelStatus_oid, sizeof(jwsTrapPowerChannelStatus_oid));
    
    /*
     * Add any objects from the trap definition
     */
    snmp_varlist_add_variable(&var_list,
        jwsTrapPowerChannelId_oid, OID_LENGTH(jwsTrapPowerChannelId_oid),
        ASN_INTEGER,
        /* Set an appropriate value for jwsTrapPowerChannelId */
        (u_char *) &channelId, sizeof(channelId));
    snmp_varlist_add_variable(&var_list,
        jwsTrapPowerChannelIdStatus_oid, OID_LENGTH(jwsTrapPowerChannelIdStatus_oid),
        ASN_INTEGER,
        /* Set an appropriate value for jwsTrapPowerChannelIdStatus */
        (u_char *) &status, sizeof(status));

    /*
     * Add any extra (optional) objects here
     */

    /*
     * Send the trap to the list of configured destinations
     *  and clean up
     */
    send_v2trap( var_list );
    snmp_free_varbind( var_list );

    return SNMP_ERR_NOERROR;
}

void
send_jwsTrapPowerChannelStatus_notification(unsigned int clientreg, void *clientarg)
{
    char fpga_reg_value = 0;
    int ret;

    /* power alarm  */
    ret = i2c_read(i2c_fd, 0x40, 0x1a, &fpga_reg_value);
    if(ret == -1)
    {
        LogWrite(ERROR, "file:%s line:%d func:%s, i2c_read failed\n",
                __FILENAME__, __LINE__, __FUNCTION__);
    }

    LogWrite(DEBUG, "file:%s line:%d func:%s, k_status :%d\n",
            __FILENAME__, __LINE__, __FUNCTION__, fpga_reg_value & 0x01);
    if(1 == (fpga_reg_value & 0x01))
    {
        send_jwsTrapPowerChannelStatus_trap( 1, POWER_DOWN);
    }
}