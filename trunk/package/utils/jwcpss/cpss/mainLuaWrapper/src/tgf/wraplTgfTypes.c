/*************************************************************************
* wraplTgfTypes.c
*
* DESCRIPTION:
*       A lua type wrapper
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
**************************************************************************/

#include <string.h>

#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

#include <extUtils/trafficEngine/tgfTrafficEngine.h>
#include <extUtils/trafficEngine/tgfTrafficGenerator.h>


use_prv_struct(TGF_PACKET_UDP_STC);
use_prv_struct(TGF_PACKET_TCP_STC);
use_prv_struct(TGF_PACKET_PTP_V2_STC);
use_prv_struct(TGF_PACKET_TRILL_GEN_OPTION_STC);
use_prv_array_type(TGF_IPV6_ADDR);
use_prv_struct(TGF_PACKET_ARPV6_STC);
use_prv_struct(TGF_PACKET_GRE_STC);
use_prv_struct(TGF_PACKET_L2_STC);
use_prv_struct(TGF_PACKET_ICMP_STC);
use_prv_struct(TGF_PACKET_ETHERTYPE_STC);
use_prv_struct(TGF_PACKET_VLAN_TAG_STC);
use_prv_struct(TGF_PACKET_MPLS_STC);
use_prv_struct(TGF_PACKET_TRILL_STC);
use_prv_struct(TGF_PACKET_TRILL_FIRST_OPTION_STC);
use_prv_enum(TGF_PROTOCOL_ENT);
use_prv_struct(TGF_PACKET_ARP_STC);
use_prv_array_type(TGF_IPV4_ADDR);
use_prv_enum(TGF_PACKET_PART_ENT);
use_prv_struct(TGF_PACKET_IPV4_STC);
use_prv_struct(TGF_PACKET_IPV6_STC);
use_prv_enum(PRV_TGF_BRG_VLAN_PORT_TAG_CMD_ENT);
use_prv_enum(TGF_VFD_MODE_ENT);
use_prv_array_type(TGF_MAC_ADDR);
use_prv_struct(TGF_PACKET_TUNNEL_HEADER_FORCE_TILL_PASSENGER_STC)
use_prv_struct(TGF_PACKET_CRC_STC)
use_prv_enum(CPSS_NET_RX_CPU_CODE_ENT);


static int hex_to_int(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
        return -2;
    return -1;
}


/*************************************************************************
* TGF_MAC_ADDR
*
* Description:
*       Convert to TGF_MAC_ADDR
*
* Parameters:
*       value at the top of stack
*       string in form "00:01:02:03:04:05" (hex digits)
*       colons may be ommited
*
* Returns:
*
*
*************************************************************************/
void prv_lua_to_c_TGF_MAC_ADDR(
    lua_State *L,
    TGF_MAC_ADDR val
)
{
    const char  *s;
    int k = 0;
    int state = 0;
    GT_U32 sum = 0;
    int i;

    if (!lua_isstring(L, -1))
        return;
    s = lua_tostring(L, -1);
    for (;*s && k < 6; s++)
    {
        if (*s == ':')
        {
            val[k++] = (GT_U8)sum;
            sum = 0;
            state = 0;
            continue;
        }
        i = hex_to_int(*s);
        if (i < 0)
            break;
        sum = sum * 16 + (unsigned)i;
        state++;
        if (state == 2)
        {
            val[k++] = (GT_U8)sum;
            sum = 0;
            state = 0;
            if (s[1] == ':')
                s++;
        }
    }
    if (state && k < 6)
    {
        val[k] = (GT_U8)sum;
    }
}
void prv_c_to_lua_TGF_MAC_ADDR(
    lua_State *L,
    TGF_MAC_ADDR val
)
{
    char s[32];
    cpssOsSprintf(s, "%02x:%02x:%02x:%02x:%02x:%02x",
            val[0], val[1], val[2], val[3], val[4], val[5]);
    lua_pushstring(L, s);
}
add_mgm_array_type(TGF_MAC_ADDR);

/*************************************************************************
* TGF_IPV4_ADDR
*
* Description:
*       Convert to TGF_IPV4_ADDR
*
* Parameters:
*       value at the top of stack
*       string in form "127.0.0.1" (decimal)
*
* Returns:
*
*
*************************************************************************/
void prv_lua_to_c_TGF_IPV4_ADDR(
    lua_State *L,
    TGF_IPV4_ADDR val
)
{
    const char  *s;
    int k = 0;
    int state = 0;
    GT_U32 sum = 0;
    int i;

    if (!lua_isstring(L, -1))
        return;
    s = lua_tostring(L, -1);
    for (;*s && k < 4; s++)
    {
        if (*s == '.')
        {
            val[k++] = (GT_U8)sum;
            sum = 0;
            state = 0;
            continue;
        }
        i = hex_to_int(*s);
        if (i < 0 || i > 9)
            break;
        sum = sum * 10 + (unsigned)i;
        state = 1;
        if (sum > 255)
            break;
    }
    if (state && k < 4)
    {
        val[k] = (GT_U8)sum;
    }
}
void prv_c_to_lua_TGF_IPV4_ADDR(
    lua_State *L,
    TGF_IPV4_ADDR val
)
{
    char s[32];
    cpssOsSprintf(s, "%d.%d.%d.%d",
            val[0], val[1], val[2], val[3]);
    lua_pushstring(L, s);
}
add_mgm_array_type(TGF_IPV4_ADDR);


/*************************************************************************
* TGF_IPV6_ADDR
*
* Description:
*       Convert to TGF_IPV6_ADDR
*
* Parameters:
*       value at the top of stack
*       string in form "2001:0db8:11a3:09d7:1f34:8a2e:07a0:765d"
*       or "2001:db8::ae21:ad12" or "::ae21:ad12"
*
* Returns:
*
*
*************************************************************************/
void prv_lua_to_c_TGF_IPV6_ADDR(
    lua_State *L,
    TGF_IPV6_ADDR val
)
{
    const char  *s;
    int k = 0;
    int state = 0;
    GT_U32 sum = 0;
    int i;
    int ddot = -1;

    if (!lua_isstring(L, -1))
        return;
    s = lua_tostring(L, -1);
    for (;*s && k < 8; s++)
    {
        if (*s == ':')
        {
            if (state)
            {
                if (ddot == -1)
                {
                    val[k++] = (GT_U16)sum;
                }
                else
                {
                    for (i = ddot; i < 7; i++)
                        val[i] = val[i+1];
                    val[7] = (GT_U16)sum;
                }
                sum = 0;
                state = 0;
                if (s[1] != ':')
                    continue;
            }
            if (s[1] == ':')
            {
                if (ddot != -1)
                    break; /* "::" twice */
                ddot = k;
                s++;
                continue;
            }
            else
            {
                break; /* ":" without digits */
            }
        }
        i = hex_to_int(*s);
        if (i < 0)
            break;
        sum = sum * 16 + (unsigned)i;
        if (sum > 0xffff)
            break;
        state = 1;
    }
    if (state && k < 8)
    {
        if (ddot == -1)
        {
            val[k++] = (GT_U16)sum;
        }
        else
        {
            for (i = ddot; i < 7; i++)
                val[i] = val[i+1];
            val[7] = (GT_U16)sum;
        }
    }
}
void prv_c_to_lua_TGF_IPV6_ADDR(
    lua_State *L,
    TGF_IPV6_ADDR val
)
{
    char s[64];
    int k;
    s[0] = 0;
    for (k = 0; k < 8; k++)
    {
        cpssOsSprintf(strchr(s, 0), "%x%c", val[k],((k < 7) ? ':':0));
    }
    lua_pushstring(L, s);
    /* TODO: short form (hide zeros) */
}
add_mgm_array_type(TGF_IPV6_ADDR);

/* TGF_VFD_INFO_STC */
void prv_lua_to_c_TGF_VFD_INFO_STC(
    lua_State *L,
    TGF_VFD_INFO_STC *val
)
{
    F_ENUM(val, -1, mode, TGF_VFD_MODE_ENT);
    F_NUMBER(val, -1, modeExtraInfo, GT_U32);
    F_NUMBER(val, -1, incValue, GT_U32);
    F_NUMBER(val, -1, offset, GT_U32);
    F_ARRAY_START(val, -1, patternPtr);
    {
        int idx;
        for (idx = 0; idx < 16; idx++) {
            F_ARRAY_NUMBER(val, patternPtr, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, patternPtr);
    F_ARRAY_START(val, -1, bitMaskPtr);
    {
        int idx;
        for (idx = 0; idx < 16; idx++) {
            F_ARRAY_NUMBER(val, bitMaskPtr, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, bitMaskPtr);
    F_NUMBER(val, -1, cycleCount, GT_U32);
#if 0
    /* not supported:
     * GT_U8 *arrayPatternPtr;
     */
    F_NUMBER(val, -1, arrayPatternOffset, GT_U32);
    F_NUMBER(val, -1, arrayPatternSize, GT_U32);
#endif
}

void prv_c_to_lua_TGF_VFD_INFO_STC(
    lua_State *L,
    TGF_VFD_INFO_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, mode, TGF_VFD_MODE_ENT);
    FO_NUMBER(val, t, modeExtraInfo, GT_U32);
    FO_NUMBER(val, t, incValue, GT_U32);
    FO_NUMBER(val, t, offset, GT_U32);
    FO_ARRAY_START(val, t, patternPtr);
    {
        int idx;
        for (idx = 0; idx < 16; idx++) {
            FO_ARRAY_NUMBER(val, patternPtr, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, patternPtr);
    FO_ARRAY_START(val, t, bitMaskPtr);
    {
        int idx;
        for (idx = 0; idx < 16; idx++) {
            FO_ARRAY_NUMBER(val, bitMaskPtr, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, bitMaskPtr);
    FO_NUMBER(val, t, cycleCount, GT_U32);
#if 0
    /* not supported:
     * GT_U8 *arrayPatternPtr;
     */
    FO_NUMBER(val, t, arrayPatternOffset, GT_U32);
    FO_NUMBER(val, t, arrayPatternSize, GT_U32);
#endif
    lua_settop(L, t);
}
add_mgm_type(TGF_VFD_INFO_STC);


/*************************************************************************
* loa_to_c_data
*
* Description:
*       Convert data (hex, userdata, etc) to C data
*
* Parameters:
*
* Returns:
*
*
*************************************************************************/
static GT_U32 hex2bin(const char *str, size_t strLen, GT_U8 *buf, GT_U32 bufLen)
{
    size_t p;
    int byte = 0, s, digit;
    GT_U32 len = 0;

    s = 0;
    for (p = 0; p < strLen && len < bufLen; p++)
    {
        digit = hex_to_int(str[p]);
        if (digit == -2 || str[p] == '-') /* space or comment */
        {
            if (s)
            {
                if (buf != NULL)
                {
                    buf[len] = (GT_U8)byte;
                }
                len++;
            }
            s = 0;
            if (str[p] == '-')
            {
                /* comment, skip to EOL */
                while (p < strLen && str[p] != '\n')
                    p++;
            }
            continue;
        }
        if (digit == -1) /* wrong character */
            break;
        if (s)
        {
            if (buf != NULL)
            {
                buf[len] = (GT_U8) (byte * 16 + digit);
            }
            len++;
        }
        else
            byte = digit;
        s ^= 1;
    }
    return len;
}

static void lua_to_c_data(
    lua_State *L,
    const char *lenFname,
    const char *dataFname,
    GT_U32     *lenPtr,
    GT_U8*     *dataPtr
)
{
    int ltype;
    ltype = lua_type(L, -1);
    if (ltype != LUA_TTABLE && ltype != LUA_TSTRING && ltype != LUA_TUSERDATA)
    {
        return;
    }
    if (ltype == LUA_TTABLE)
    {
        /* table: { "$lenFname": LUA_TNUMBER, "$dataFname": LUA_TSTRING in hex or LUA_TUSERDATA } */
        lua_getfield(L, -1, lenFname);
        if (lua_type(L, -1) == LUA_TNUMBER)
        {
            *lenPtr = (GT_U32)lua_tonumber(L, -1);
            lua_pop(L, 1);
            *dataPtr = (GT_U8*)cmdOsMalloc(*lenPtr);
            cmdOsMemSet(*dataPtr, 0, *lenPtr);
            lua_getfield(L, -1, dataFname);
            if (lua_type(L, -1) == LUA_TUSERDATA)
            {
                GT_U32 dataLen = lua_objlen(L, -1);
                if (dataLen > *lenPtr)
                    dataLen = *lenPtr;
                cmdOsMemCpy(*dataPtr, lua_touserdata(L, -1), dataLen);
            }
            if (lua_type(L, -1) == LUA_TSTRING)
            {
                const char *str;
                size_t strLen;
                str = lua_tolstring(L, -1, &strLen);
                hex2bin(str, strLen, *dataPtr, *lenPtr);
            }
            lua_pop(L, 1);
            return;
        }
        /* "$lenFname not defined or not a number */
        lua_getfield(L, -1, dataFname);
        if (lua_type(L, -1) != LUA_TSTRING && lua_type(L, -1) != LUA_TUSERDATA)
        {
            lua_pop(L, -1);
            return;
        }
    }
    if (lua_type(L, -1) == LUA_TUSERDATA)
    {
        /* userdata, copy */
        *lenPtr = (GT_U32)lua_objlen(L, -1);
        *dataPtr = (GT_U8*)cmdOsMalloc(*lenPtr);
        cmdOsMemCpy(*dataPtr, lua_touserdata(L, -1), *lenPtr);
        return;
    }
    if (lua_type(L, -1) == LUA_TSTRING)
    {
        const char *str;
        size_t strLen;
        str = lua_tolstring(L, -1, &strLen);
        *lenPtr = hex2bin(str, strLen, NULL, 0xffff);
        *dataPtr = (GT_U8*)cmdOsMalloc(*lenPtr);
        *lenPtr = hex2bin(str, strLen, *dataPtr, *lenPtr);
        return;
    }
    if (ltype == LUA_TTABLE)
    {
        /* table: { "$dataFname": LUA_TSTRING in hex or LUA_TUSERDATA } */
        lua_pop(L, 1);
    }
}

static void c_to_lua_data(
    lua_State *L,
    const char *lenFname,
    const char *dataFname,
    GT_U32     len,
    GT_U8*     dataPtr
)
{
    int t;
    GT_U32 i;
    char buf[66];
    GT_U32 p = 0;
    lua_newtable(L);
    t = lua_gettop(L);
    lua_pushnumber(L, len);
    lua_setfield(L, t, lenFname);

    if (!dataPtr)
        len = 0;
    lua_pushliteral(L, "");
    for (i = 0; i < len; i++)
    {
        cmdOsSprintf(buf + p, "%02X", dataPtr[i]);
        p += 2;
        if (p >= 64)
        {
            lua_pushlstring(L, buf, p);
            lua_concat(L, 2);
            p = 0;
        }
    }
    if (p > 0)
    {
        lua_pushlstring(L, buf, p);
        lua_concat(L, 2);
    }
    lua_setfield(L, t, dataFname);
    lua_settop(L, t);
}

/*************************************************************************
* lua <=> C functions for TGF_PACKET_PAYLOAD_STC
*
*************************************************************************/
void prv_gc_TGF_PACKET_PAYLOAD_STC(
    TGF_PACKET_PAYLOAD_STC *val
)
{
    if (val->dataPtr != NULL)
    {
        cmdOsFree(val->dataPtr);
    }
}

void prv_lua_to_c_TGF_PACKET_PAYLOAD_STC(
    lua_State *L,
    TGF_PACKET_PAYLOAD_STC *val
)
{
    lua_to_c_data(L, "dataLength", "dataPtr", &(val->dataLength), &(val->dataPtr));
}

void prv_c_to_lua_TGF_PACKET_PAYLOAD_STC(
    lua_State *L,
    TGF_PACKET_PAYLOAD_STC *val
)
{
    c_to_lua_data(L, "dataLength", "dataPtr", val->dataLength, val->dataPtr);
}
add_mgm_type_gc(TGF_PACKET_PAYLOAD_STC);

/*************************************************************************
* lua <=> C functions for TGF_PACKET_TEMPLATE16_STC
*
*************************************************************************/
void prv_gc_TGF_PACKET_TEMPLATE16_STC(
    TGF_PACKET_TEMPLATE16_STC *val
)
{
    if (val->dataPtr != NULL)
    {
        cmdOsFree(val->dataPtr);
    }
}

void prv_lua_to_c_TGF_PACKET_TEMPLATE16_STC(
    lua_State *L,
    TGF_PACKET_TEMPLATE16_STC *val
)
{
    lua_to_c_data(L, "dataLength", "dataPtr", &(val->dataLength), &(val->dataPtr));
}

void prv_c_to_lua_TGF_PACKET_TEMPLATE16_STC(
    lua_State *L,
    TGF_PACKET_TEMPLATE16_STC *val
)
{
    c_to_lua_data(L, "dataLength", "dataPtr", val->dataLength, val->dataPtr);
}
add_mgm_type_gc(TGF_PACKET_TEMPLATE16_STC);

/*************************************************************************
* lua <=> C functions for TGF_PACKET_WILDCARD_STC
*
*************************************************************************/
void prv_gc_TGF_PACKET_WILDCARD_STC(
    TGF_PACKET_WILDCARD_STC *val
)
{
    if (val->bytesPtr != NULL)
    {
        cmdOsFree(val->bytesPtr);
    }
}

void prv_lua_to_c_TGF_PACKET_WILDCARD_STC(
    lua_State *L,
    TGF_PACKET_WILDCARD_STC *val
)
{
    lua_to_c_data(L, "numOfBytes", "bytesPtr", &(val->numOfBytes), &(val->bytesPtr));
}

void prv_c_to_lua_TGF_PACKET_WILDCARD_STC(
    lua_State *L,
    TGF_PACKET_WILDCARD_STC *val
)
{
    c_to_lua_data(L, "numOfBytes", "bytesPtr", val->numOfBytes, val->bytesPtr);
}
add_mgm_type_gc(TGF_PACKET_WILDCARD_STC);

/*************************************************************************
* lua <=> C functions for TGF_PACKET_CRC_STC
*
*************************************************************************/
void prv_gc_TGF_PACKET_CRC_STC(
    TGF_PACKET_CRC_STC *val
)
{
    if (val->bytesPtr != NULL)
    {
        cmdOsFree(val->bytesPtr);
    }
}

void prv_lua_to_c_TGF_PACKET_CRC_STC(
    lua_State *L,
    TGF_PACKET_CRC_STC *val
)
{
    lua_to_c_data(L, "numOfBytes", "bytesPtr", &(val->numOfBytes), &(val->bytesPtr));
}

void prv_c_to_lua_TGF_PACKET_CRC_STC(
    lua_State *L,
    TGF_PACKET_CRC_STC *val
)
{
    c_to_lua_data(L, "numOfBytes", "bytesPtr", val->numOfBytes, val->bytesPtr);
}
add_mgm_type_gc(TGF_PACKET_CRC_STC);

/*************************************************************************
* lua <=> C functions for TGF_PACKET_TUNNEL_HEADER_FORCE_TILL_PASSENGER_STC
*
*************************************************************************/
void prv_gc_TGF_PACKET_TUNNEL_HEADER_FORCE_TILL_PASSENGER_STC(
    TGF_PACKET_TUNNEL_HEADER_FORCE_TILL_PASSENGER_STC *val
)
{
    if (val->bytesPtr != NULL)
    {
        cmdOsFree(val->bytesPtr);
    }
}

void prv_lua_to_c_TGF_PACKET_TUNNEL_HEADER_FORCE_TILL_PASSENGER_STC(
    lua_State *L,
    TGF_PACKET_TUNNEL_HEADER_FORCE_TILL_PASSENGER_STC *val
)
{
    lua_to_c_data(L, "numOfBytes", "bytesPtr", &(val->numOfBytes), &(val->bytesPtr));
}

void prv_c_to_lua_TGF_PACKET_TUNNEL_HEADER_FORCE_TILL_PASSENGER_STC(
    lua_State *L,
    TGF_PACKET_TUNNEL_HEADER_FORCE_TILL_PASSENGER_STC *val
)
{
    c_to_lua_data(L, "numOfBytes", "bytesPtr", val->numOfBytes, val->bytesPtr);
}
add_mgm_type_gc(TGF_PACKET_TUNNEL_HEADER_FORCE_TILL_PASSENGER_STC);


/*************************************************************************
* prv_gc_TGF_PACKET_PART_STC
*
* Description:
*       Garbage collection function
*
* Parameters:
*       val
*
* Returns:
*
*
*************************************************************************/
void prv_gc_TGF_PACKET_PART_STC(
    TGF_PACKET_PART_STC *val
)
{
    if (val->partPtr == NULL)
        return;
    switch (val->type)
    {
        case TGF_PACKET_PART_PAYLOAD_E:
            prv_gc_TGF_PACKET_PAYLOAD_STC((TGF_PACKET_PAYLOAD_STC*)(val->partPtr));
            break;
        case TGF_PACKET_PART_TEMPLATE_E:
            prv_gc_TGF_PACKET_TEMPLATE16_STC((TGF_PACKET_TEMPLATE16_STC*)(val->partPtr));
            break;
        case TGF_PACKET_PART_WILDCARD_E:
            prv_gc_TGF_PACKET_WILDCARD_STC((TGF_PACKET_WILDCARD_STC*)(val->partPtr));
            break;
        case TGF_PACKET_PART_CRC_E:
            prv_gc_TGF_PACKET_CRC_STC((TGF_PACKET_CRC_STC*)(val->partPtr));
            break;
        case TGF_PACKET_PART_TUNNEL_HEADER_FORCE_TILL_PASSENGER_E:
            prv_gc_TGF_PACKET_TUNNEL_HEADER_FORCE_TILL_PASSENGER_STC((TGF_PACKET_TUNNEL_HEADER_FORCE_TILL_PASSENGER_STC*)(val->partPtr));
            break;
        /* TODO */
        case TGF_PACKET_PART_L2_E:
        case TGF_PACKET_PART_VLAN_TAG_E:
        case TGF_PACKET_PART_DSA_TAG_E:
        case TGF_PACKET_PART_ETHERTYPE_E:
        case TGF_PACKET_PART_MPLS_E:
        case TGF_PACKET_PART_IPV4_E:
        case TGF_PACKET_PART_IPV6_E:
        case TGF_PACKET_PART_TCP_E:
        case TGF_PACKET_PART_UDP_E:
        case TGF_PACKET_PART_ARP_E:
        case TGF_PACKET_PART_ICMP_E:
        case TGF_PACKET_PART_TRILL_E:
        case TGF_PACKET_PART_TRILL_FIRST_OPTION_E:
        case TGF_PACKET_PART_TRILL_GENERAL_OPTION_E:
        case TGF_PACKET_PART_GRE_E:
        case TGF_PACKET_PART_PTP_V2_E:
        case TGF_PACKET_PART_SKIP_E:
        case TGF_PACKET_PART_MAX_E:
            break;
    }
    cmdOsFree(val->partPtr);
}
/*************************************************************************
* prv_lua_to_c_TGF_PACKET_PART_STC
*
* Description:
*       Convert to TGF_PACKET_PART_STC
*
* Parameters:
*       value at the top of stack
*
* Returns:
*
*
*************************************************************************/
void prv_lua_to_c_TGF_PACKET_PART_STC(
    lua_State *L,
    TGF_PACKET_PART_STC *val
)
{
    F_ENUM(val, -1, type, TGF_PACKET_PART_ENT);
    switch (val->type)
    {
        case TGF_PACKET_PART_L2_E:
#define PART_TYPE(typeName) \
            lua_getfield(L, -1, "partPtr"); \
            if (lua_type(L, -1) == LUA_TTABLE) \
            { \
                val->partPtr = cmdOsMalloc(sizeof(typeName)); \
                cmdOsMemSet(val->partPtr, 0, sizeof(typeName)); \
                prv_lua_to_c_ ## typeName(L, (typeName*)(val->partPtr)); \
            } \
            lua_pop(L, 1);
            PART_TYPE(TGF_PACKET_L2_STC);
            break;
        case TGF_PACKET_PART_VLAN_TAG_E:
            PART_TYPE(TGF_PACKET_VLAN_TAG_STC);
            break;
        case TGF_PACKET_PART_ETHERTYPE_E:
            PART_TYPE(TGF_PACKET_ETHERTYPE_STC);
            break;
        case TGF_PACKET_PART_MPLS_E:
            PART_TYPE(TGF_PACKET_MPLS_STC);
            break;
        case TGF_PACKET_PART_IPV4_E:
            PART_TYPE(TGF_PACKET_IPV4_STC);
            break;
        case TGF_PACKET_PART_IPV6_E:
            PART_TYPE(TGF_PACKET_IPV6_STC);
            break;
        case TGF_PACKET_PART_TCP_E:
            PART_TYPE(TGF_PACKET_TCP_STC);
            break;
        case TGF_PACKET_PART_UDP_E:
            PART_TYPE(TGF_PACKET_UDP_STC);
            break;
        case TGF_PACKET_PART_ARP_E:
            PART_TYPE(TGF_PACKET_ARP_STC);
            break;
        case TGF_PACKET_PART_ICMP_E:
            PART_TYPE(TGF_PACKET_ICMP_STC);
            break;
        case TGF_PACKET_PART_TRILL_E:
            PART_TYPE(TGF_PACKET_TRILL_STC);
            break;
        case TGF_PACKET_PART_TRILL_FIRST_OPTION_E:
            PART_TYPE(TGF_PACKET_TRILL_FIRST_OPTION_STC);
            break;
        case TGF_PACKET_PART_TRILL_GENERAL_OPTION_E:
            PART_TYPE(TGF_PACKET_TRILL_GEN_OPTION_STC);
            break;
        case TGF_PACKET_PART_GRE_E:
            PART_TYPE(TGF_PACKET_GRE_STC);
            break;
        case TGF_PACKET_PART_PTP_V2_E:
            PART_TYPE(TGF_PACKET_PTP_V2_STC);
            break;
        case TGF_PACKET_PART_SKIP_E:
            break;
        case TGF_PACKET_PART_PAYLOAD_E:
#define PART_TYPE_CUSTOM(typeName) \
            val->partPtr = cmdOsMalloc(sizeof(typeName)); \
            cmdOsMemSet(val->partPtr, 0, sizeof(typeName)); \
            lua_getfield(L, -1, "partPtr"); \
            prv_lua_to_c_ ## typeName(L, (typeName*)(val->partPtr)); \
            lua_pop(L, 1);
            PART_TYPE_CUSTOM(TGF_PACKET_PAYLOAD_STC);
            break;
        case TGF_PACKET_PART_TEMPLATE_E:
            PART_TYPE_CUSTOM(TGF_PACKET_TEMPLATE16_STC);
            break;
        case TGF_PACKET_PART_WILDCARD_E:
            PART_TYPE_CUSTOM(TGF_PACKET_WILDCARD_STC);
            break;
        case TGF_PACKET_PART_CRC_E:
            PART_TYPE_CUSTOM(TGF_PACKET_CRC_STC);
            break;
        case TGF_PACKET_PART_TUNNEL_HEADER_FORCE_TILL_PASSENGER_E:
            PART_TYPE_CUSTOM(TGF_PACKET_TUNNEL_HEADER_FORCE_TILL_PASSENGER_STC);
            break;
        /* TODO */
        case TGF_PACKET_PART_DSA_TAG_E:
            break;
        case TGF_PACKET_PART_MAX_E:
            break;
#undef PART_TYPE
#undef PART_TYPE_CUSTOM
    }
}

/*************************************************************************
* prv_c_to_lua_TGF_PACKET_PART_STC
*
* Description:
*       Convert TGF_PACKET_PART_STC to lua
*
* Parameters:
*
* Returns:
*       value at the top of stack
*
*************************************************************************/
void prv_c_to_lua_TGF_PACKET_PART_STC(
    lua_State *L,
    TGF_PACKET_PART_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, type, TGF_PACKET_PART_ENT);
    switch (val->type)
    {
        case TGF_PACKET_PART_L2_E:
#define PART_TYPE(typeName) \
            prv_c_to_lua_ ## typeName(L, (typeName*)(val->partPtr)); \
            lua_setfield(L, t, "partPtr");
            PART_TYPE(TGF_PACKET_L2_STC);
            break;
        case TGF_PACKET_PART_VLAN_TAG_E:
            PART_TYPE(TGF_PACKET_VLAN_TAG_STC);
            break;
        case TGF_PACKET_PART_ETHERTYPE_E:
            PART_TYPE(TGF_PACKET_ETHERTYPE_STC);
            break;
        case TGF_PACKET_PART_MPLS_E:
            PART_TYPE(TGF_PACKET_MPLS_STC);
            break;
        case TGF_PACKET_PART_IPV4_E:
            PART_TYPE(TGF_PACKET_IPV4_STC);
            break;
        case TGF_PACKET_PART_IPV6_E:
            PART_TYPE(TGF_PACKET_IPV6_STC);
            break;
        case TGF_PACKET_PART_TCP_E:
            PART_TYPE(TGF_PACKET_TCP_STC);
            break;
        case TGF_PACKET_PART_UDP_E:
            PART_TYPE(TGF_PACKET_UDP_STC);
            break;
        case TGF_PACKET_PART_ARP_E:
            PART_TYPE(TGF_PACKET_ARP_STC);
            break;
        case TGF_PACKET_PART_ICMP_E:
            PART_TYPE(TGF_PACKET_ICMP_STC);
            break;
        case TGF_PACKET_PART_TRILL_E:
            PART_TYPE(TGF_PACKET_TRILL_STC);
            break;
        case TGF_PACKET_PART_TRILL_FIRST_OPTION_E:
            PART_TYPE(TGF_PACKET_TRILL_FIRST_OPTION_STC);
            break;
        case TGF_PACKET_PART_TRILL_GENERAL_OPTION_E:
            PART_TYPE(TGF_PACKET_TRILL_GEN_OPTION_STC);
            break;
        case TGF_PACKET_PART_GRE_E:
            PART_TYPE(TGF_PACKET_GRE_STC);
            break;
        case TGF_PACKET_PART_PTP_V2_E:
            PART_TYPE(TGF_PACKET_PTP_V2_STC);
            break;
        case TGF_PACKET_PART_SKIP_E:
            break;
        case TGF_PACKET_PART_PAYLOAD_E:
            PART_TYPE(TGF_PACKET_PAYLOAD_STC);
            break;
        case TGF_PACKET_PART_TEMPLATE_E:
            PART_TYPE(TGF_PACKET_TEMPLATE16_STC);
            break;
        case TGF_PACKET_PART_WILDCARD_E:
            PART_TYPE(TGF_PACKET_WILDCARD_STC);
            break;
        case TGF_PACKET_PART_CRC_E:
            PART_TYPE(TGF_PACKET_CRC_STC);
            break;
        case TGF_PACKET_PART_TUNNEL_HEADER_FORCE_TILL_PASSENGER_E:
            PART_TYPE(TGF_PACKET_TUNNEL_HEADER_FORCE_TILL_PASSENGER_STC);
            break;
        /* TODO */
        case TGF_PACKET_PART_DSA_TAG_E:
            break;
        case TGF_PACKET_PART_MAX_E:
            break;
#undef PART_TYPE
    }

    lua_settop(L, t);
}
add_mgm_type_gc(TGF_PACKET_PART_STC);



static GT_U32 prv_TGF_PACKET_PART_len(
    TGF_PACKET_PART_STC *part
)
{
    switch (part->type)
    {
        case TGF_PACKET_PART_L2_E:
            return TGF_L2_HEADER_SIZE_CNS;
        case TGF_PACKET_PART_VLAN_TAG_E:
            return TGF_VLAN_TAG_SIZE_CNS;
        case TGF_PACKET_PART_ETHERTYPE_E:
            return TGF_ETHERTYPE_SIZE_CNS;
        case TGF_PACKET_PART_MPLS_E:
            return TGF_MPLS_HEADER_SIZE_CNS;
        case TGF_PACKET_PART_IPV4_E:
            return TGF_IPV4_HEADER_SIZE_CNS;
        case TGF_PACKET_PART_IPV6_E:
            return TGF_IPV6_HEADER_SIZE_CNS;
        case TGF_PACKET_PART_TCP_E:
            return TGF_TCP_HEADER_SIZE_CNS;
        case TGF_PACKET_PART_UDP_E:
            return TGF_UDP_HEADER_SIZE_CNS;
        case TGF_PACKET_PART_ARP_E:
            return TGF_ARP_HEADER_SIZE_CNS;
        case TGF_PACKET_PART_ICMP_E:
            return TGF_ICMP_HEADER_SIZE_CNS;
        case TGF_PACKET_PART_TRILL_E:
            return TGF_TRILL_HEADER_SIZE_CNS;
        case TGF_PACKET_PART_GRE_E:
            return (((TGF_PACKET_GRE_STC*) part->partPtr)->checkSumPresent) ? 8 : 4;
        case TGF_PACKET_PART_PTP_V2_E:
            return TGF_PTP_V2_HDR_SIZE_CNS;
        case TGF_PACKET_PART_SKIP_E:
            return 0;
        case TGF_PACKET_PART_PAYLOAD_E:
            if (part->partPtr == NULL)
                return 0;
            return ((TGF_PACKET_PAYLOAD_STC*)part->partPtr)->dataLength;
        case TGF_PACKET_PART_TEMPLATE_E:
            if (part->partPtr == NULL)
                return 0;
            return ((TGF_PACKET_TEMPLATE16_STC*)part->partPtr)->dataLength;
        case TGF_PACKET_PART_WILDCARD_E:
            if (part->partPtr == NULL)
                return 0;
            return ((TGF_PACKET_WILDCARD_STC*)part->partPtr)->numOfBytes;
        case TGF_PACKET_PART_CRC_E:
            if (part->partPtr == NULL)
                return 0;
            return ((TGF_PACKET_CRC_STC*)part->partPtr)->numOfBytes;
        case TGF_PACKET_PART_TUNNEL_HEADER_FORCE_TILL_PASSENGER_E:
            if (part->partPtr == NULL)
                return 0;
            return ((TGF_PACKET_TUNNEL_HEADER_FORCE_TILL_PASSENGER_STC*)part->partPtr)->numOfBytes;
        /* TODO */
        case TGF_PACKET_PART_DSA_TAG_E:
            return TGF_DSA_NUM_BYTES_MAC(part->partPtr);
        case TGF_PACKET_PART_MAX_E:
            break;
        case TGF_PACKET_PART_TRILL_FIRST_OPTION_E:
            break;
        case TGF_PACKET_PART_TRILL_GENERAL_OPTION_E:
            break;
    }
    return 0;
}




void prv_gc_TGF_PACKET_STC(
    TGF_PACKET_STC *val
)
{
    GT_U32 i;
    if (val->partsArray == NULL)
        return;
    for (i = 0; i < val->numOfParts; i++)
    {
        prv_gc_TGF_PACKET_PART_STC(&(val->partsArray[i]));
    }
    cmdOsFree(val->partsArray);
}

/*************************************************************************
* prv_lua_to_c_TGF_PACKET_STC
*
* Description:
*       Convert to TGF_PACKET_STC
*
* Parameters:
*       value at the top of stack
*
* Returns:
*
*
*************************************************************************/
void prv_lua_to_c_TGF_PACKET_STC(
    lua_State *L,
    TGF_PACKET_STC *val
)
{
    int nparts = -1;
    int calcTotal;

    /* now parse lua table */
    F_NUMBER(val, -1, totalLen, GT_U32);
    lua_getfield(L, -1, "totalLen");
    if (lua_type(L, -1) == LUA_TNUMBER)
    {
        val->totalLen = (int)lua_tonumber(L, -1);
        calcTotal = 0;
    }
    else
    {
        calcTotal = 1;
        val->totalLen = 0;
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "numOfParts");
    if (lua_type(L, -1) == LUA_TNUMBER)
    {
        nparts = (int)lua_tonumber(L, -1);
    }
    lua_pop(L, 1);
    /* allocate partsArray */
    if (nparts == -1)
    {
        lua_getfield(L, -1, "partsArray");
        nparts = (int)lua_objlen(L, -1);
        lua_pop(L, 1);
    }
    val->numOfParts = (GT_U32)nparts;
    val->partsArray = (TGF_PACKET_PART_STC*)cmdOsMalloc(nparts * sizeof(*(val->partsArray)));
    cmdOsMemSet(val->partsArray, 0, nparts * sizeof(*(val->partsArray)));
    F_ARRAY_START(val, -1, partsArray);
    {
        int i;
        for (i = 0; i < nparts; i++) {
            lua_rawgeti(L, -1, i+1);
            if (lua_type(L, -1) == LUA_TTABLE)
            {
                prv_lua_to_c_TGF_PACKET_PART_STC(L, &(val->partsArray[i]));
                /* TODO: calculate totalLen */
                if (calcTotal)
                {
                    val->totalLen += prv_TGF_PACKET_PART_len(&(val->partsArray[i]));
                }
            }
            lua_pop(L, 1);
        }
    }
    F_ARRAY_END(L, -1, partsArray);
}


/*************************************************************************
* prv_c_to_lua_TGF_PACKET_STC
*
* Description:
*       Convert TGF_PACKET_STC to lua
*
* Parameters:
*
* Returns:
*       value at the top of stack
*
*************************************************************************/
void prv_c_to_lua_TGF_PACKET_STC(
    lua_State *L,
    TGF_PACKET_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, totalLen, GT_U32);
    FO_NUMBER(val, t, numOfParts, GT_U32);
    FO_ARRAY_START(val, t, partsArray);
    {
        GT_U32 i;
        for (i = 0; i < val->numOfParts; i++) {
            prv_c_to_lua_TGF_PACKET_PART_STC(L, &(val->partsArray[i]));
            lua_rawseti(L, -2, i+1);
        }
    }
    FO_ARRAY_END(L, t, partsArray);
    lua_settop(L, t);
}
add_mgm_type_gc(TGF_PACKET_STC);


void prv_gc_TGF_NET_DSA_STC(
    TGF_NET_DSA_STC *val
)
{
    /* nothing to do */
}

/*************************************************************************
* prv_lua_to_c_TGF_NET_DSA_STC
*
* Description:
*       Convert to TGF_NET_DSA_STC
*
* Parameters:
*
* Returns:
*       value at the top of stack
*
*************************************************************************/
void prv_lua_to_c_TGF_NET_DSA_STC(
    lua_State *L,
    TGF_NET_DSA_STC *val
)
{
    FO_BOOL(val,   -1, dsaCmdIsToCpu);
    FO_ENUM(val,   -1, cpuCode, CPSS_NET_RX_CPU_CODE_ENT);
    FO_BOOL(val,   -1, srcIsTrunk);
    FO_NUMBER(val, -1, hwDevNum, GT_HW_DEV_NUM);
    FO_NUMBER(val, -1, portNum, GT_PHYSICAL_PORT_NUM);
    FO_NUMBER(val, -1, originByteCount, GT_U32);
    FO_NUMBER(val, -1, trunkId, GT_TRUNK_ID);
    FO_NUMBER(val, -1, ePortNum, GT_PORT_NUM);
}

/*************************************************************************
* prv_c_to_lua_TGF_NET_DSA_STC
*
* Description:
*       Convert TGF_NET_DSA_STC to lua
*
* Parameters:
*
* Returns:
*       value at the top of stack
*
*************************************************************************/
void prv_c_to_lua_TGF_NET_DSA_STC(
    lua_State *L,
    TGF_NET_DSA_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, dsaCmdIsToCpu);
    FO_ENUM(val, t, cpuCode, CPSS_NET_RX_CPU_CODE_ENT);
    FO_NUMBER_N(val, t, cpuCode, cpuCodeNumber, GT_U32);
    FO_BOOL(val, t, srcIsTrunk);
    FO_NUMBER(val, t, hwDevNum, GT_HW_DEV_NUM);
    FO_NUMBER(val, t, portNum, GT_PHYSICAL_PORT_NUM);
    FO_NUMBER(val, t, originByteCount, GT_U32);
    FO_NUMBER(val, t, trunkId, GT_TRUNK_ID);
    FO_NUMBER(val, t, ePortNum, GT_PORT_NUM);
    lua_settop(L, t);
}
add_mgm_type_gc(TGF_NET_DSA_STC);



int mgmType_to_c_TGF_VFD_INFO_STC_ARR(
    lua_State *L
)
{
    TGF_VFD_INFO_STC *val = NULL;
    GT_U32            arrSize = 1;
    GT_U32            i;

    if (lua_istable(L, 1))
    {
        arrSize = (GT_U32)lua_objlen(L, 1);
        if (arrSize == 0)
            arrSize = 1;
    }

    lua_newuserdata(L, sizeof(TGF_VFD_INFO_STC)*arrSize);

    val = (TGF_VFD_INFO_STC*)lua_touserdata(L, -1);
    cmdOsMemSet(val, 0, sizeof(*val)*arrSize);
    if (lua_isnil(L, 1))
    {
        return 1;
    }
    for (i = 0; i < arrSize; i++)
    {
        lua_rawgeti(L, 1, i+1);
        if (lua_type(L, -1) == LUA_TTABLE)
        {
            prv_lua_to_c_TGF_VFD_INFO_STC(L, &(val[i]));
        }
        lua_pop(L, 1);
    }
    return 1;
}

int mgmType_to_lua_TGF_VFD_INFO_STC_ARR(
    lua_State *L
)
{
    TGF_VFD_INFO_STC *val;
    GT_U32           arrSize, i;
    if (lua_isnil(L, 1))
        return 1;
    if (!lua_isuserdata(L, 1))
    {
        lua_pushnil(L);
        return 1;
    }
    arrSize = (GT_U32)(lua_objlen(L, 1)/sizeof(TGF_VFD_INFO_STC));
    if (arrSize * sizeof(TGF_VFD_INFO_STC) != lua_objlen(L, 1))
    {
        lua_pushnil(L);
        return 1;
    }
    val = (TGF_VFD_INFO_STC*)lua_touserdata(L, 1);
    lua_newtable(L);
    for (i = 0; i < arrSize; i++)
    {
        prv_c_to_lua_TGF_VFD_INFO_STC(L, &(val[i]));
        lua_rawseti(L, -2, i+1);
    }
    return 1;
}
