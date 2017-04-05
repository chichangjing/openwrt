/*************************************************************************
* all_common_lua_c_type_wrappers.c
*
* DESCRIPTION:
*       A lua type wrapper
*       It implements support for the following types:
*           enum    GT_BOOL
*           struct  WRAP_DEV_PORT_STC
*           struct  GT_ETHERADDR
*           struct  CPSS_INTERFACE_INFO_STC
*           struct  GT_IPADDR
*           struct  GT_IPV6ADDR
*           struct  PACKET_STC
*           struct  TX_PARAMS_STC
*           struct  RX_DATA_STC
*           struct  CPSS_MAC_ENTRY_EXT_KEY_STC
*           struct  CPSS_RLDRAM_CFG_STC
*           struct  CPSS_DRAM_CFG_STC
*           enum    CPSS_SHIFT_OPERATION_ENT
*           enum    CPSS_ADJUST_OPERATION_ENT
*           enum    PRV_CPSS_SIP_ENT
*           struct  CPSS_GEN_DRAGONITE_DATA_STC
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 12 $
**************************************************************************/

#include <generic/private/prvWraplGeneral.h>
#include <cpssCommon/wraplCpssDeviceInfo.h>
#include <cpssCommon/wrapCpssDebugInfo.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>
#include <cpss/generic/config/cpssGenCfg.h>
#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpss/generic/trunk/cpssGenTrunkTypes.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/dragonite/cpssGenDragonite.h>
#include <mainLuaWrapper/wraplNetIf.h>

#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>
#include <string.h>



/***** MACROSES *****/
#define F_UNION_MEMBER_NUMBER(unionObject,Lindex,field,fieldtype) \
    lua_getfield(L, Lindex, #field); \
    if (lua_type(L, -1) == LUA_TNUMBER) \
    { \
        unionObject.field = (fieldtype)lua_tointeger(L, -1); \
    } \
    else \
    { \
        unionObject.field = (fieldtype)0; \
    } \
    lua_pop(L, 1);

#define F_UNION_MEMBER_STRUCT(unionObject,Lindex,field,fieldtype) \
    lua_getfield(L, Lindex, #field); \
    if (lua_type(L, -1) == LUA_TTABLE) \
    { \
        prv_lua_to_c_ ## fieldtype(L, &(unionObject.field)); \
    } \
    lua_pop(L, 1);

#define FO_UNION_MEMBER_NUMBER(unionObject,Lindex,field,fieldtype) \
    lua_pushnumber(L, unionObject.field); \
    lua_setfield(L, Lindex, #field);

#define FO_UNION_MEMBER_STRUCT(unionObject,Lindex,field,fieldtype) \
    prv_c_to_lua_ ## fieldtype(L, &(unionObject.field)); \
    lua_setfield(L, Lindex, #field);



#define F_UNION_MEMBER_ENUM(unionObject, Lindex, field, fieldtype) \
    lua_getfield(L, Lindex, #field); \
    unionObject.field = (fieldtype)prv_lua_to_c_enum(L, enumDescr_ ## fieldtype); \
    lua_pop(L, 1);

#define F_UNION_MEMBER_BOOL(unionObject, Lindex, field) \
    lua_getfield(L, Lindex, #field); \
    if (lua_type(L, -1) == LUA_TBOOLEAN) \
    { \
        unionObject.field = lua_toboolean(L, -1) ? GT_TRUE : GT_FALSE; \
    } \
    else \
    { \
        unionObject.field = GT_FALSE; \
    } \
    lua_pop(L, 1);


#define FO_UNION_MEMBER_ENUM(unionObject, Lindex, field, fieldtype) \
    prv_c_to_lua_enum(L, enumDescr_ ## fieldtype, unionObject.field); \
    if (lua_isnil(L, -1)) \
        lua_pop(L, 1); \
    else \
        lua_setfield(L, Lindex, #field);

#define FO_UNION_MEMBER_BOOL(unionObject, Lindex, field) \
    lua_pushboolean(L, (unionObject.field == GT_TRUE) ? 1 : 0); \
    lua_setfield(L, Lindex, #field);


#define PRV_UNION_LUA_TO_C_START(uniontype,member) \
void prv_lua_to_c_ ## uniontype ## _ ## member( \
    lua_State         *L, \
    uniontype         *val \
) \
{
#define PRV_UNION_LUA_TO_C_END() \
}

#define PRV_UNION_C_TO_LUA_START(uniontype,member) \
void prv_c_to_lua_ ## uniontype ## _ ## member( \
    lua_State         *L, \
    uniontype         *val \
) \
{ \
    int t; \
    lua_newtable(L); \
    t = lua_gettop(L);
#define PRV_UNION_C_TO_LUA_END() \
}


#define add_mgm_type_union(typeName,member) \
int mgmType_to_c_ ## typeName ## _ ## member( \
    lua_State *L \
) \
{ \
    typeName *val = NULL; \
    lua_newuserdata(L, sizeof(typeName)); \
    if (lua_isnil(L, 1)) \
    { \
        return 1; \
    } \
 \
    val = (typeName*)lua_touserdata(L, -1); \
    cmdOsMemSet(val, 0, sizeof(*val)); \
    lua_pushvalue(L, 1); \
    prv_lua_to_c_ ## typeName ## _ ## member(L, val); \
    lua_pop(L, 1); \
    return 1; \
} \
 \
int mgmType_to_lua_ ## typeName ## _ ## member( \
    lua_State *L \
) \
{ \
    typeName *val; \
    if (lua_isnil(L, 1)) \
        return 1; \
    if (!lua_isuserdata(L, 1)) \
    { \
        lua_pushnil(L); \
        return 1; \
    } \
 \
    val = (typeName*)lua_touserdata(L, 1); \
    prv_c_to_lua_ ## typeName ## _ ## member(L, val); \
 \
    return 1; \
}


#define UNION_MEMBER_STRUCT(typename,member,membertype) \
PRV_UNION_LUA_TO_C_START(typename,member) \
    F_STRUCT(val, -1, member, membertype); \
PRV_UNION_LUA_TO_C_END() \
 \
PRV_UNION_C_TO_LUA_START(typename,member) \
    FO_STRUCT(val, t, member, membertype); \
PRV_UNION_C_TO_LUA_END() \
 \
add_mgm_type_union(typename,member);

/* Slice bounds of IP v6 array  */
#define GT_IPV6_BEGIN_OCTET     0
#define GT_IPV6_END_OCTET       3

/***** declarations ********/

use_prv_enum(CPSS_INTERFACE_TYPE_ENT);
use_prv_enum(CPSS_MAC_TABLE_CMD_ENT);
use_prv_enum(CPSS_MAC_ENTRY_EXT_TYPE_ENT);
use_prv_enum(CPSS_NET_RX_CPU_CODE_ENT);
use_prv_enum(CPSS_DP_LEVEL_ENT);
use_prv_enum(CPSS_INTERFACE_TYPE_ENT)
use_prv_enum(CPSS_PACKET_CMD_ENT)
use_prv_enum(CPSS_PACKET_CMD_ENT);
use_prv_enum(CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
use_prv_enum(CPSS_DRAM_SIZE_ENT);
use_prv_enum(CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
use_prv_enum(CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT);
use_prv_enum(CPSS_DRAM_FREQUENCY_ENT);
use_prv_enum(CPSS_DRAM_PAGE_SIZE_ENT);
use_prv_enum(CPSS_POLICER_COLOR_MODE_ENT);
use_prv_enum(CPSS_INTERFACE_TYPE_ENT);
use_prv_enum(CPSS_BRG_IPM_MODE_ENT)
use_prv_enum(CPSS_IPV6_PREFIX_SCOPE_ENT)
use_prv_enum(CPSS_IP_CNT_SET_ENT);
use_prv_enum(CPSS_IP_SITE_ID_ENT);

use_prv_struct(WRAP_DEV_PORT_STC);
use_prv_struct(GT_ETHERADDR);
use_prv_struct(CPSS_INTERFACE_INFO_STC);
use_prv_struct(CPSS_PORTS_BMP_STC);
use_prv_struct(GT_IPADDR);
use_prv_struct(GT_IPV6ADDR);
use_prv_struct(CPSS_MAC_ENTRY_EXT_KEY_STC);
use_prv_struct(CPSS_MAC_ENTRY_EXT_STC);
use_prv_struct(CPSS_RLDRAM_CFG_STC);
use_prv_struct(CPSS_GEN_CFG_DEV_INFO_STC);
use_prv_struct(CPSS_DRAM_CFG_STC);

use_prv_print_struct(CPSS_INTERFACE_INFO_STC);
use_prv_print_struct(CPSS_PORTS_BMP_STC)
use_prv_print_struct(GT_ETHERADDR);
use_prv_print_struct(GT_IPADDR);
use_prv_print_struct(GT_IPV6ADDR);
use_prv_print_struct(CPSS_MAC_ENTRY_EXT_KEY_STC);
use_prv_print_struct(CPSS_MAC_ENTRY_EXT_STC);

/**
use_prv_enum(CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT);
use_prv_enum(CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT);
use_prv_enum(CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT); */


enumDescr enumDescr_GT_BOOL[] = {
    ENUM_ENTRY(GT_FALSE),
    ENUM_ENTRY(GT_TRUE),
    { 0, NULL }
};
add_mgm_enum(GT_BOOL);

/*************************************************************************
* prv_lua_to_c_DEV_PORT_STC
*
* Description:
*       Convert to "c" WRAP_DEV_PORT_STC
*
* Parameters:
*       value at the top of stack
*
* Returns:
*
*
*************************************************************************/
void prv_lua_to_c_WRAP_DEV_PORT_STC(
    lua_State         *L,
    WRAP_DEV_PORT_STC *val
)
{
    F_NUMBER(val, -1, hwDevNum,   GT_HW_DEV_NUM);
    F_NUMBER(val, -1, portNum,  GT_PORT_NUM);
}

/*************************************************************************
* prv_c_to_lua_DEV_PORT_STC
*
* Description:
*       Convert WRAP_DEV_PORT_STC to lua
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
void prv_c_to_lua_WRAP_DEV_PORT_STC(
    lua_State         *L,
    WRAP_DEV_PORT_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, hwDevNum,   GT_HW_DEV_NUM);
    FO_NUMBER(val, t, portNum,  GT_PORT_NUM);
}

static int hexdigit2int(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
        return -2;
    return -1;
}


/*******************************************/
/* GT_ETHERADDR                            */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_GT_ETHERADDR
*
* Description:
*       Convert to GT_ETHERADDR
*
* Parameters:
*       value at the top of stack
*       in lua it can be CLI_types["mac-address"]                   or
*                        "01:02:03:04:05:06"                        or
*                        {[0]=0, [1]=1, [2]=2, [3]=3, [4]=4, [5]=5} or
*                        {arEther =
*                           {[0]=0, [1]=1, [2]=2, [3]=3, [4]=4, [5]=5}}
*
* Returns:
*
*
*************************************************************************/
void prv_lua_to_c_GT_ETHERADDR(
    lua_State *L,
    GT_ETHERADDR *e
)
{
    int k;
    GT_BOOL                 pop_flag = GT_FALSE;
    const GT_CHAR           *s;
    cpssOsMemSet(e, 0, sizeof(*e));

    if (lua_istable(L, -1))
    {
        lua_getfield(L, -1, "string");

        if (lua_isstring(L, -1))
        {
            pop_flag = GT_TRUE;
        }
        else
        {
            lua_pop(L, 1);
        }
    }

    if (lua_type(L, -1) != LUA_TSTRING)
    {
        if (lua_istable(L, -1))
        {
            if (5 > lua_objlen(L, -1))
            {
                lua_getfield(L, -1, "arEther");
                pop_flag = GT_TRUE;
            }

            for (k = 0; k < 6; k++)
            {
                F_ARRAY_NUMBER(e, arEther, k, GT_U8);
            }
        }
        /*F_ARRAY_END(e, -1, arEther);*/
    }
    else
    {
        int state = 0;
        GT_U32 sum = 0;
        int d;

        s = lua_tostring(L, -1);

        for (k=0; *s && k < 6; s++)
        {
            if (*s == ':')
            {
                e->arEther[k++] = (GT_U8)sum;
                sum = 0;
                state = 0;
                continue;
            }
            d = hexdigit2int(*s);
            if (d < 0)
                break;

            sum = sum * 16 + d;
            state++;
            if (state == 2)
            {
                e->arEther[k++] = (GT_U8)sum;
                sum = 0;
                state = 0;
                if (s[1] == ':')
                    s++;
            }
        }
        if (state && k < 6)
        {
            e->arEther[k] = (GT_U8)sum;
        }
    }

    if (GT_TRUE == pop_flag)
    {
        lua_pop(L, 1);
    }
}


/*************************************************************************
* prv_lua_to_c_mask_GT_ETHERADDR
*
* Description:
*       Convert lua GT_ETHERADDR comparision mask to "C"
*
* Parameters:
*       value at the top of stack
*       in lua it can be params["mac-address"]                      or
*                        params["mac-address"]["string"]            or
*                        {[0]=*, [1]=*, [2]=*, [3]=*, [4]=*, [5]=*} or
*                        {arEther =
*                           {[0]=*, [1]=*, [2]=*, [3]=*, [4]=*, [5]=*}}
*
* Returns:
*
*
*************************************************************************/
void prv_lua_to_c_mask_GT_ETHERADDR(
    lua_State               *L,
    GT_ETHERADDR            *val
)
{
    GT_BOOL                 pop_flag = GT_FALSE;
    GT_BOOL                 isMasked = GT_FALSE;

    if (lua_istable(L, -1))
    {
        lua_getfield(L, -1, "string");

        if (lua_isstring(L, -1))
        {
            isMasked = GT_TRUE;
            pop_flag = GT_TRUE;
        }
        else
        {
            lua_pop(L, 1);
        }
    }

    if (0 == lua_isstring(L, -1))
    {
        if (lua_istable(L, -1))
        {
            if (5 > lua_objlen(L, -1))
            {
                lua_getfield(L, -1, "arEther");
                pop_flag = GT_TRUE;
            }

            if (5 > lua_objlen(L, -1))
            {
                isMasked = GT_TRUE;
            }
        }
    }
    else
    {
        isMasked = GT_TRUE;
    }

    if (GT_TRUE == pop_flag)
    {
        lua_pop(L, 1);
    }

    if (GT_TRUE == isMasked)
    {
        cpssOsMemSet(val, 0x00, sizeof(*val));
    }
    else
    {
        cpssOsMemSet(val, 0xff, sizeof(*val));
    }
}


/*************************************************************************
* prv_c_to_lua_GT_ETHERADDR
*
* Description:
*       Convert GT_ETHERADDR to lua
*
* Parameters:
*
* Returns:
*       value at the top of stack
*
*************************************************************************/
void prv_c_to_lua_GT_ETHERADDR(
    lua_State *L,
    GT_ETHERADDR *e
)
{
    char s[32];
    cpssOsSprintf(s, "%02x:%02x:%02x:%02x:%02x:%02x",
            e->arEther[0], e->arEther[1], e->arEther[2],
            e->arEther[3], e->arEther[4], e->arEther[5]);
    lua_pushstring(L, s);
}
add_mgm_type(GT_ETHERADDR);


/*************************************************************************
* prv_lua_c_print_GT_ETHERADDR
*
* Description:
*       Prints GT_ETHERADDR fields
*
* Parameters:
*       name                  - structure variable name or path to
*                               structure field
*       nameExtension         - structure field name (could be NULL)
*       val                   - pointer to structure
*
*
*************************************************************************/
void prv_lua_c_print_GT_ETHERADDR
(
    const GT_CHAR_PTR           name,
    const GT_CHAR_PTR           nameExtension,
    const GT_ETHERADDR          *val
)
{
    P_NAME_CONVERSIONS(fullName, name, nameExtension);
    if (fullName)
    {
        cpssOsPrintf("%s\n", fullName);
    }
    P_MESSAGE6("%02x:%02x:%02x:%02x:%02x:%02x",
               val->arEther[0], val->arEther[1], val->arEther[2],
               val->arEther[3], val->arEther[4], val->arEther[5]);
}


/*******************************************/
/* CPSS_INTERFACE_INFO_STC                 */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_CPSS_INTERFACE_INFO_STC
*
* Description:
*       Convert to CPSS_INTERFACE_INFO_STC
*
* Parameters:
*       value at the top of stack
*
* Returns:
*
*
*************************************************************************/
void prv_lua_to_c_CPSS_INTERFACE_INFO_STC(
    lua_State *L,
    CPSS_INTERFACE_INFO_STC *info
)
{
    F_ENUM(info, -1, type, CPSS_INTERFACE_TYPE_ENT);
    lua_getfield(L, -1, "devPort");
    if (lua_istable(L, -1))
    {
        F_NUMBER_N(&(info->devPort), -1, hwDevNum, devNum, GT_HW_DEV_NUM);
        F_NUMBER(&(info->devPort), -1, portNum, GT_PORT_NUM);
    }
    lua_pop(L, 1);
    F_NUMBER(info, -1, trunkId, GT_TRUNK_ID);
    F_NUMBER(info, -1, vidx, GT_U16);
    F_NUMBER(info, -1, vlanId, GT_U16);
    F_NUMBER_N(info, -1, hwDevNum, devNum, GT_U8);
    F_NUMBER(info, -1, fabricVidx, GT_U8);
    F_NUMBER(info, -1, index, GT_U32);

}

/*************************************************************************
* prv_lua_to_c_mask_CPSS_INTERFACE_INFO_STC
*
* Description:
*       Convert lua CPSS_INTERFACE_INFO_STC comparision mask to "C"
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
void prv_lua_to_c_mask_CPSS_INTERFACE_INFO_STC(
    lua_State *L,
    CPSS_INTERFACE_INFO_STC *val
)
{
    cpssOsMemSet(val, 0xff, sizeof(*val));
    FM_ENUM(val, -1, type, CPSS_INTERFACE_TYPE_ENT);
    lua_getfield(L, -1, "devPort");
    if (lua_istable(L, -1))
    {
        FM_NUMBER_N(&(val->devPort), -1, hwDevNum, devNum, GT_HW_DEV_NUM);
        FM_NUMBER(&(val->devPort), -1, portNum, GT_PORT_NUM);
    }
    lua_pop(L, 1);
    FM_NUMBER(val, -1, trunkId, GT_TRUNK_ID);
    FM_NUMBER(val, -1, vidx, GT_U16);
    FM_NUMBER(val, -1, vlanId, GT_U16);
    FM_NUMBER_N(val, -1, hwDevNum, devNum, GT_HW_DEV_NUM);
    FM_NUMBER(val, -1, fabricVidx, GT_U16);
    FM_NUMBER(val, -1, index, GT_U32);
}

/*************************************************************************
* prv_c_to_lua_CPSS_INTERFACE_INFO_STC
*
* Description:
*       Convert CPSS_INTERFACE_INFO_STC to lua
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
void prv_c_to_lua_CPSS_INTERFACE_INFO_STC(
    lua_State *L,
    CPSS_INTERFACE_INFO_STC *info
)
{
    int t, t1;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(info, t, type, CPSS_INTERFACE_TYPE_ENT);
    lua_newtable(L);
    t1 = lua_gettop(L);
        FO_NUMBER_N(&(info->devPort), t1, hwDevNum, devNum,  GT_HW_DEV_NUM);
        FO_NUMBER(&(info->devPort), t1, portNum, GT_PORT_NUM);
    lua_setfield(L, t, "devPort");
    FO_NUMBER(info, t, trunkId, GT_TRUNK_ID);
    FO_NUMBER(info, t, vidx, GT_U16);
    FO_NUMBER(info, t, vlanId, GT_U16);
    FO_NUMBER_N(info, t, hwDevNum, devNum, GT_HW_DEV_NUM);
    FO_NUMBER(info, t, fabricVidx, GT_U16);
    FO_NUMBER(info, t, index, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_INTERFACE_INFO_STC)


/*************************************************************************
* prv_lua_c_print_CPSS_INTERFACE_INFO_STC
*
* Description:
*       Prints CPSS_INTERFACE_INFO_STC fields
*
* Parameters:
*       name                      - structure variable name or path to
*                                   structure field
*       nameExtension             - structure field name (could be NULL)
*       val                       - pointer to structure
*
*************************************************************************/
void prv_lua_c_print_CPSS_INTERFACE_INFO_STC
(
    const GT_CHAR_PTR               name,
    const GT_CHAR_PTR               nameExtension,
    const CPSS_INTERFACE_INFO_STC   *val
)
{
    P_NAME_CONVERSIONS(fullName, name, nameExtension);
    P_ENUM(val, fullName, type, CPSS_INTERFACE_TYPE_ENT);

    switch(val->type)
    {
        case CPSS_INTERFACE_PORT_E:
            P_NAME_CONVERSIONS_BEGIN(devPortName, fullName, "devPort");
            {
                P_NUMBER_N(&(val->devPort), devPortName, hwDevNum, devNum,
                                                                GT_HW_DEV_NUM);
                P_NUMBER(&(val->devPort),   devPortName, portNum, GT_PORT_NUM);
            }
            P_NAME_CONVERSIONS_END(devPortName, fullName, "devPort");
            break;

        case CPSS_INTERFACE_TRUNK_E:
            P_NUMBER(val,   fullName, trunkId, GT_TRUNK_ID);
            break;

        case CPSS_INTERFACE_VIDX_E:
            P_NUMBER(val,   fullName, vidx, GT_U16);
            break;

        case CPSS_INTERFACE_VID_E:
            P_NUMBER(val,   fullName, vlanId, GT_U16);
            break;

        case CPSS_INTERFACE_DEVICE_E:
            P_NUMBER_N(val,   fullName, hwDevNum, devNum, GT_U16);
            break;

        case CPSS_INTERFACE_FABRIC_VIDX_E:
            P_NUMBER(val,   fullName, fabricVidx, GT_U8);
            break;

        case CPSS_INTERFACE_INDEX_E:
            P_NUMBER(val,   fullName, index, GT_U32);
            break;

        default:
            P_MESSAGE("Wrong interface type");
            break;
    }
}


/*************************************************************************
* prv_lua_to_c_mask_CPSS_PORTS_BMP_STC
*
* Description:
*       Convert lua CPSS_PORTS_BMP_STC comparision mask to "C"
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
void prv_lua_to_c_mask_CPSS_PORTS_BMP_STC(
    lua_State                               *L,
    CPSS_PORTS_BMP_STC                      *val
)
{
    GT_U32 portMaskByte = 0;

    cpssOsMemSet(val, 0xff, sizeof(*val));

    FM_ARRAY_START(val, -1, ports);
    {
        int idx;
        for (idx = 0; idx < CPSS_MAX_PORTS_BMP_NUM_CNS; idx++)
        {
            lua_rawgeti(L, -1, idx);
            if (lua_type(L, -1) == LUA_TNUMBER)
            {
                portMaskByte    = (GT_U32) lua_tointeger(L, -1);

                if (0 != portMaskByte)
                {
                    val->ports[idx] = (GT_U32) 0;
                }
            }
            lua_pop(L, 1);
        }
    }
    FM_ARRAY_END(val, -1, ports);
}


/*************************************************************************
* prv_lua_c_print_CPSS_PORTS_BMP_STC
*
* Description:
*       Prints CPSS_PORTS_BMP_STC fields
*
* Parameters:
*       name                  - structure variable name or path to
*                               structure field
*       nameExtension         - structure field name (could be NULL)
*       val                   - pointer to structure
*
*************************************************************************/
void prv_lua_c_print_CPSS_PORTS_BMP_STC
(
    const GT_CHAR_PTR           name,
    const GT_CHAR_PTR           nameExtension,
    const CPSS_PORTS_BMP_STC    *val
)
{
    P_NAME_CONVERSIONS(fullName, name, nameExtension);
    P_ARRAY_START(val, fullName, ports);
    {
        int idx;
        for (idx = 0; idx < 8; idx++)
        {
            P_ARRAY_NUMBER(val, fullName, ports, idx, GT_U32);
        }
    }
    P_ARRAY_END(val, fullName, ports);
}



/*******************************************/
/* GT_IPV4                                   */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_GT_IPV4
*
* Description:
*       Convert to IP v4 array
*
* Parameters:
*       value at the top of stack
*
* Returns:
*
*
*************************************************************************/
void prv_lua_to_c_GT_IPV4
(
    lua_State   *L,
    GT_U8_PTR   ipPtr
)
{
    GT_BOOL                 pop_flag = GT_FALSE;

    if (lua_istable(L, -1))
    {
        lua_getfield(L, -1, "string");

        if (lua_isstring(L, -1))
        {
            pop_flag = GT_TRUE;
        }
        else
        {
            lua_pop(L, 1);
        }
    }

    cpssOsMemSet(ipPtr, 0, sizeof(ipPtr));
    if (lua_type(L, -1) == LUA_TSTRING)
    {
        /* parse dotted IP string */
        int k;
        int n;
        const char *s;
        s = lua_tostring(L, -1);

        k = 0;
        for (; *s && k < 4; s++)
        {
            if (*s == '.')
            {
                k++;
            }
            else if ((*s) >= '0' && (*s) <= '9')
            {
                n = ipPtr[k] * 10 + (*s) - '0';
                if (n > 255)
                    break;
                ipPtr[k] = (GT_U8) n;
            }
            else
            {
                break;
            }
        }
    }

    if (GT_TRUE == pop_flag)
    {
        lua_pop(L, 1);
    }
}


/*************************************************************************
* prv_lua_to_c_mask_GT_IPV4
*
* Description:
*       Convert to IP v4 array
*
* Parameters:
*       value at the top of stack
*
* Returns:
*
*
*************************************************************************/
void prv_lua_to_c_mask_GT_IPV4
(
    lua_State   *L,
    GT_U8_PTR   ipPtr
)
{
    GT_BOOL     isMasked = GT_FALSE;

    if (lua_istable(L, -1))
    {
        lua_getfield(L, -1, "string");

        if (lua_isstring(L, -1))
        {
            isMasked = GT_TRUE;
        }

        lua_pop(L, 1);
    }

    if (lua_type(L, -1) == LUA_TSTRING)
    {
        isMasked = GT_TRUE;
    }


    if (GT_TRUE == isMasked)
    {
        cpssOsMemSet(ipPtr, 0x00, sizeof(ipPtr));
    }
    else
    {
        cpssOsMemSet(ipPtr, 0xff, sizeof(ipPtr));
    }
}


/*************************************************************************
* prv_c_to_lua_GT_IPV4
*
* Description:
*       Convert IP v4 array to lua
*
* Parameters:
*
* Returns:
*       value at the top of stack
*
*************************************************************************/
void prv_c_to_lua_GT_IPV4
(
    lua_State   *L,
    GT_U8_PTR   ipPtr
)
{
    char s[16];
    cpssOsSprintf(s, "%d.%d.%d.%d",
            ipPtr[0], ipPtr[1],
            ipPtr[2], ipPtr[3]);
    lua_pushstring(L, s);
}


/*************************************************************************
* prv_lua_c_print_GT_IPV4
*
* Description:
*       Prints IP v4 array
*
* Parameters:
*
* Returns:
*       name                  - array variable name or path to array field
*       nameExtension         - array field name
*       val                   - pointer to array
*
*************************************************************************/
void prv_lua_c_print_GT_IPV4
(
    const GT_CHAR_PTR           name,
    const GT_CHAR_PTR           nameExtension,
    const GT_U8                 *val
)
{
    P_NAME_CONVERSIONS(fullName, name, nameExtension);
    if (fullName)
    {
        cpssOsPrintf("%s\n", fullName);
    }
    P_MESSAGE4("%d.%d.%d.%d", val[0], val[1], val[2], val[3]);
}


/*******************************************/
/* GT_IPV6                                   */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_GT_IPV6
*
* Description:
*       Convert to IP v6 array
*
* Parameters:
*       value at the top of stack
*
* Returns:
*
*
*************************************************************************/
void prv_lua_to_c_GT_IPV6
(
    lua_State   *L,
    GT_U8_PTR   ipPtr
)
{
    GT_IPV6ADDR ip;
    GT_U16      octetIndex;

    prv_lua_to_c_GT_IPV6ADDR(L, &ip);

    cpssOsMemSet(ipPtr, 0, sizeof(ipPtr));

    for (octetIndex = GT_IPV6_BEGIN_OCTET;
         octetIndex <= GT_IPV6_END_OCTET;
         octetIndex++)
    {
        ipPtr[GT_IPV6_END_OCTET - (octetIndex - GT_IPV6_BEGIN_OCTET)] =
                                                            ip.arIP[octetIndex];
    }
}


/*************************************************************************
* prv_lua_to_c_mask_GT_IPV6
*
* Description:
*       Convert to IP v6 array
*
* Parameters:
*       value at the top of stack
*
* Returns:
*
*
*************************************************************************/
void prv_lua_to_c_mask_GT_IPV6
(
    lua_State   *L,
    GT_U8_PTR   ipPtr
)
{
    GT_BOOL     isMasked = GT_FALSE;

    if (lua_istable(L, -1))
    {
        lua_getfield(L, -1, "string");

        if (lua_isstring(L, -1))
        {
            isMasked = GT_TRUE;
        }

        lua_pop(L, 1);
    }

    if (lua_type(L, -1) == LUA_TSTRING)
    {
        isMasked = GT_TRUE;
    }


    if (GT_TRUE == isMasked)
    {
        cpssOsMemSet(ipPtr, 0x00, sizeof(ipPtr));
    }
    else
    {
        cpssOsMemSet(ipPtr, 0xff, sizeof(ipPtr));
    }
}


/*************************************************************************
* prv_c_to_lua_GT_IPV6
*
* Description:
*       Convert IP v6 array to lua
*
* Parameters:
*
* Returns:
*       value at the top of stack
*
*************************************************************************/
void prv_c_to_lua_GT_IPV6
(
    lua_State   *L,
    GT_U8_PTR   ipPtr
)
{
    GT_IPV6ADDR ip;
    GT_U16      octetIndex;

    cpssOsMemSet(&ip, 0, sizeof(ip));

    for (octetIndex = GT_IPV6_BEGIN_OCTET;
         octetIndex <= GT_IPV6_END_OCTET;
         octetIndex++)
    {
        ip.arIP[octetIndex] =
            ipPtr[GT_IPV6_END_OCTET - (octetIndex - GT_IPV6_BEGIN_OCTET)];
    }

    prv_c_to_lua_GT_IPV6ADDR(L, &ip);
}


/*************************************************************************
* prv_lua_c_print_GT_IPV6
*
* Description:
*       Prints IP v6 array
*
* Parameters:
*
* Returns:
*       name                  - array variable name or path to array field
*       nameExtension         - array field name
*       val                   - pointer to array
*
*************************************************************************/
void prv_lua_c_print_GT_IPV6
(
    const GT_CHAR_PTR           name,
    const GT_CHAR_PTR           nameExtension,
    const GT_U8                 *val
)
{
    GT_IPV6ADDR ip;
    GT_U16      octetIndex;

    P_NAME_CONVERSIONS(fullName, name, nameExtension);
    cpssOsMemSet(&ip, 0, sizeof(ip));

    for (octetIndex = GT_IPV6_BEGIN_OCTET;
         octetIndex <= GT_IPV6_END_OCTET;
         octetIndex++)
    {
        ip.arIP[octetIndex] =
            val[GT_IPV6_END_OCTET - (octetIndex - GT_IPV6_BEGIN_OCTET)];
    }

    prv_lua_c_print_GT_IPV6ADDR(fullName, NULL, &ip);
}


/*******************************************/
/* GT_IPADDR                               */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_GT_IPADDR
*
* Description:
*       Convert to GT_IPADDR
*
* Parameters:
*       value at the top of stack
*
* Returns:
*
*
*************************************************************************/
void prv_lua_to_c_GT_IPADDR(
    lua_State *L,
    GT_IPADDR *ip
)
{
    cpssOsMemSet(ip, 0, sizeof(*ip));
    if (lua_type(L, -1) == LUA_TNUMBER)
    {
        ip->u32Ip = (GT_U32)lua_tointeger(L, -1);
    }
            else
            {
        prv_lua_to_c_GT_IPV4(L, ip->arIP);
    }
}


/*************************************************************************
* prv_c_to_lua_GT_IPADDR
*
* Description:
*       Convert GT_IPADDR to lua
*
* Parameters:
*
* Returns:
*       value at the top of stack
*
*************************************************************************/
void prv_c_to_lua_GT_IPADDR(
    lua_State *L,
    GT_IPADDR *ip
)
{
    prv_c_to_lua_GT_IPV4(L, ip->arIP);
}


int mgmType_to_c_GT_IPADDR(
    lua_State *L
)
{
    GT_IPADDR ipAddr;
    lua_pushvalue(L, 1);
    prv_lua_to_c_GT_IPADDR(L, &ipAddr);
    lua_pop(L, 1);
    lua_pushnumber(L, ipAddr.u32Ip);
    return 1;
}
int mgmType_to_lua_GT_IPADDR(
    lua_State *L
)
{
    GT_IPADDR ipAddr;
    if (lua_isnil(L, 1))
        return 1;
    if (!lua_isnumber(L, 1))
    {
        lua_pushnil(L);
        return 1;
    }
    ipAddr.u32Ip = lua_tointeger(L, 1);
    prv_c_to_lua_GT_IPADDR(L, &ipAddr);
    return 1;
}

/*************************************************************************
* prv_lua_c_print_GT_IPADDR
*
* Description:
*       Prints GT_IPADDR fields
*
* Parameters:
*       name                  - structure variable name or path to
*                               structure field
*       nameExtension         - structure field name (could be NULL)
*       val                   - pointer to structure
*
*************************************************************************/
void prv_lua_c_print_GT_IPADDR
(
    const GT_CHAR_PTR           name,
    const GT_CHAR_PTR           nameExtension,
    const GT_IPADDR             *val
)
{
    P_NAME_CONVERSIONS(fullName, name, nameExtension);
    prv_lua_c_print_GT_IPV4(fullName, "arIP", val->arIP);
}


/* GT_IPADDRPTR */
int mgmType_to_c_GT_IPADDRPTR(
    lua_State *L
)
{
    GT_IPADDR *val;
    lua_newuserdata(L, sizeof(GT_IPADDR));
    if (lua_isnil(L, 1))
    {
        return 1;
    }

    val = (GT_IPADDR*)lua_touserdata(L, -1);
    lua_pushvalue(L, 1);
    prv_lua_to_c_GT_IPADDR(L, val);
    lua_pop(L, 1);
    return 1;
}

int mgmType_to_lua_GT_IPADDRPTR(
    lua_State *L
)
{
    GT_IPADDR *val;
    if (lua_isnil(L, 1))
        return 1;
    if (!lua_isuserdata(L, 1))
    {
        lua_pushnil(L);
        return 1;
    }

    val = (GT_IPADDR*)lua_touserdata(L, 1);
    prv_c_to_lua_GT_IPADDR(L, val);

    return 1;
}

/*******************************************/
/* GT_IPV6ADDR                             */
/*******************************************/

/*************************************************************************
* prv_string_to_IPV6_arr
*
* DESCRIPTION:
*       Convert to IPV6 string to GT_U16[8]
*
* INPUTS:
*       s   - string in form "2001:0db8:11a3:09d7:1f34:8a2e:07a0:765d"
*             or "2001:db8::ae21:ad12" or "::ae21:ad12"
*
* OUTPUTS:
*       addr - GT_U16[8] array
*
* RETURNS:
*       None
*
*
*************************************************************************/
void prv_string_to_IPV6_arr(
  IN  const char    *s,
  OUT GT_U16        *addr
)
{
    int k;
    int state = 0;
    GT_U32 sum = 0;
    int i;
    int ddot = -1;

    for (k = 0; k < 8; k++)
        addr[k] = 0;
    for (k = 0; *s && k < 8; s++)
    {
        if (*s == ':')
        {
            if (state)
            {
                if (ddot == -1)
                {
                    addr[k++] = (GT_U16)sum;
                }
                else
                {
                    for (i = ddot; i < 7; i++)
                        addr[i] = addr[i+1];
                    addr[7] = (GT_U16)sum;
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
        i = hexdigit2int(*s);
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
            addr[k++] = (GT_U16)sum;
        }
        else
        {
            for (i = ddot; i < 7; i++)
                addr[i] = addr[i+1];
            addr[7] = (GT_U16)sum;
        }
    }
}

/*************************************************************************
* prv_IPV6_arr_to_string
*
* DESCRIPTION:
*       Convert IPV6 GT_U16[8] to string
*
* INPUTS:
*       addr - GT_U16[8] array
*       useShortForm    - use short form (remove zeroes
*
* OUTPUTS:
*       s   - string in form "2001:0db8:11a3:09d7:1f34:8a2e:07a0:765d"
*             or "2001:db8::ae21:ad12" or "::ae21:ad12"
*
* RETURNS:
*       None
*
*
*************************************************************************/
void prv_IPV6_arr_to_string(
  IN  GT_U16        *addr,
  IN  GT_BOOL       useShortForm,
  OUT char          *s
)
{
    int k;
    int zind = -1;
    int zlen = 0;

    if (useShortForm == GT_TRUE)
    {
        int l;
        for (k = 0; k < 8;k++)
        {
            for (l = 0; k+l < 8; l++)
                if (addr[k+l] != 0)
                    break;
            if (l > zlen)
            {
                zind = k;
                zlen = l;
                k += l-1;
            }
        }
    }
    s[0] = 0;
    for (k = 0; k < 8; k++)
    {
        if (k == zind)
        {
            strcat(s, "::");
            k += zlen - 1;
            continue;
        }
        cpssOsSprintf(strchr(s, 0), "%x", addr[k]);
        if ((k < 7) && (k+1 != zind))
        {
            strcat(s, ":");
        }
    }
}


/*************************************************************************
* prv_lua_to_c_GT_IPV6ADDR
*
* Description:
*       Convert to GT_IPV6ADDR
*
* Parameters:
*       value at the top of stack
*
* Returns:
*
*
*************************************************************************/
void prv_lua_to_c_GT_IPV6ADDR
(
    lua_State *L,
    GT_IPV6ADDR *ip
)
{
    GT_U16      ipArr[8];
    int         i;
    GT_BOOL     pop_flag            = GT_FALSE;

    if (lua_istable(L, -1))
    {
        lua_getfield(L, -1, "string");

        if (lua_isstring(L, -1))
        {
            pop_flag = GT_TRUE;
        }
        else
        {
            lua_pop(L, 1);
        }
    }

    cpssOsMemSet(ip, 0, sizeof(*ip));
    if (lua_type(L, -1) != LUA_TSTRING)
    {
        return;
    }
    prv_string_to_IPV6_arr(lua_tostring(L, -1), ipArr);

    for (i = 0; i < 8; i++)
    {
        ip->arIP[i+i] = (GT_U8)((ipArr[i] >> 8) & 0x00ff);
        ip->arIP[i+i+1] = (GT_U8)(ipArr[i] & 0x00ff);
    }

    if (GT_TRUE == pop_flag)
    {
        lua_pop(L, 1);
    }
}


/*************************************************************************
* prv_c_to_lua_GT_IPV6ADDR
*
* Description:
*       Convert GT_IPV6ADDR to lua
*
* Parameters:
*
* Returns:
*       value at the top of stack
*
*************************************************************************/
void prv_c_to_lua_GT_IPV6ADDR
(
    lua_State *L,
    GT_IPV6ADDR *ip
)
{
    GT_CHAR         s[40];
    GT_U16          ipArr[8];
    int             i;
    for (i = 0; i < 8; i++)
    {
        ipArr[i] = ip->arIP[i+i] << 8 | ip->arIP[i+i+1];
    }

    prv_IPV6_arr_to_string(ipArr, GT_TRUE, s);

    lua_pushstring(L, s);
}
add_mgm_type(GT_IPV6ADDR)


/*************************************************************************
* prv_lua_c_print_GT_IPV6ADDR
*
* Description:
*       Prints GT_IPV6ADDR fields
*
* Parameters:
*       name                          - structure variable name or path
*                                       to structure field
*       nameExtension                 - structure field name (could be
*                                       NULL)
*       val                           - pointer to structure
*
*************************************************************************/
void prv_lua_c_print_GT_IPV6ADDR
(
    const GT_CHAR_PTR                   name,
    const GT_CHAR_PTR                   nameExtension,
    const GT_IPV6ADDR                   *val
)
{
    GT_CHAR                         s[40];
    GT_U16          ipArr[8];
    int             i;
    for (i = 0; i < 8; i++)
    {
        ipArr[i] = val->arIP[i+i] << 8 | val->arIP[i+i+1];
    }

    prv_IPV6_arr_to_string(ipArr, GT_TRUE, s);

    P_MESSAGE1("%s", s);
}



/*******************************************/
/* PACKET_STC                              */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_PACKET_STC
*
* Description:
*       Convert to "c" PACKET_STC
*
* Parameters:
*       value at the top of stack
*
* Returns:
*
*
*************************************************************************/
void prv_lua_to_c_PACKET_STC(
    lua_State *L,
    PACKET_STC *val
)
{
    int ltype;
    int actual_len = 0;
    ltype = lua_type(L, -1);
    if (ltype != LUA_TTABLE && ltype != LUA_TSTRING && ltype != LUA_TUSERDATA)
    {
        return;
    }
    if (ltype == LUA_TTABLE)
    {
        /* table: { "len": LUA_TNUMBER, "data": LUA_TSTRING in hex or LUA_TUSERDATA } */
        lua_getfield(L, -1, "data");
        if (lua_type(L, -1) != LUA_TSTRING && lua_type(L, -1) != LUA_TUSERDATA)
        {
            lua_pop(L, -1);
            F_NUMBER(val, -1, len, int);
            if (val->len > PACKET_MAX_SIZE)
                val->len = PACKET_MAX_SIZE;
            return;
        }
        F_NUMBER(val, -1, len, int);
    }
    if (lua_type(L, -1) == LUA_TUSERDATA)
    {
        /* userdata, copy */
        val->len = lua_objlen(L, -1);
        if (val->len > PACKET_MAX_SIZE)
            val->len = PACKET_MAX_SIZE;
        cmdOsMemCpy(val->data, lua_touserdata(L, -1), val->len);
        actual_len = val->len;
    }
    if (lua_type(L, -1) == LUA_TSTRING)
    {
        /* hexdump, decode */
        const char *str;
        size_t strLen, p;
        int byte = 0, s, digit;

        val->len = 0;
        str = lua_tolstring(L, -1, &strLen);
        s = 0;
        for (p = 0; p < strLen && val->len < PACKET_MAX_SIZE; p++)
        {
            digit = hexdigit2int(str[p]);
            if (digit == -2 || str[p] == '-') /* space or comment */
            {
                if (s)
                    val->data[val->len++] = (GT_U8) byte;
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
                val->data[val->len++] = (GT_U8) (byte * 16 + digit);
            else
                byte = digit;
            s ^= 1;
        }
        actual_len = val->len;
    }
    if (ltype == LUA_TTABLE)
    {
        lua_pop(L, 1);
        /* table: { LUA_TNUMBER "len", LUA_TSTRING "data" in hex } */
        F_NUMBER(val, -1, len, int);
        if (val->len > PACKET_MAX_SIZE)
            val->len = PACKET_MAX_SIZE;
        /* pad with zeros */
        for (; actual_len < val->len; actual_len++)
            val->data[actual_len] = 0;
    }
}

/*************************************************************************
* prv_c_to_lua_PACKET_STC
*
* Description:
*       Convert PACKET_STC to lua
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
void prv_c_to_lua_PACKET_STC(
    lua_State *L,
    PACKET_STC *val
)
{
    int t;
    int i;
    char buf[66];
    int p = 0;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, len, int);

    lua_pushliteral(L, "");
    for (i = 0; i < val->len && i < PACKET_MAX_SIZE; i++)
    {
        cmdOsSprintf(buf + p, "%02X", (unsigned char)(val->data[i]));
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
    lua_setfield(L, t, "data");
}

add_mgm_type(PACKET_STC);

/*******************************************/
/* TX_PARAMS_STC                           */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_TX_PARAMS_STC
*
* Description:
*       Convert to "c" TX_PARAMS_STC
*
* Parameters:
*       value at the top of stack
*
* Returns:
*
*
*************************************************************************/
void prv_lua_to_c_TX_PARAMS_STC(
    lua_State *L,
    TX_PARAMS_STC *val
)
{
    F_NUMBER(val, -1, replyQ, GT_MSGQ_ID);
    F_STRUCT(val, -1, srcInterface, CPSS_INTERFACE_INFO_STC);
    /* special handling for packet: can be table, string, userdata */
    F_STRUCT_CUSTOM(val, -1, packet, PACKET_STC);
}

/*************************************************************************
* prv_c_to_lua_TX_PARAMS_STC
*
* Description:
*       Convert TX_PARAMS_STC to lua
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
void prv_c_to_lua_TX_PARAMS_STC(
    lua_State *L,
    TX_PARAMS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, replyQ, GT_MSGQ_ID);
    FO_STRUCT(val, t, srcInterface, CPSS_INTERFACE_INFO_STC);
    FO_STRUCT(val, t, packet, PACKET_STC);
}
add_mgm_type(TX_PARAMS_STC);


/*******************************************/
/* RX_DATA_STC                             */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_RX_DATA_STC
*
* Description:
*       Convert to "c" RX_DATA_STC
*
* Parameters:
*       value at the top of stack
*
* Returns:
*
*
*************************************************************************/
void prv_lua_to_c_RX_DATA_STC(
    lua_State *L,
    RX_DATA_STC *val
)
{
    F_NUMBER(val, -1, devNum, GT_U8);
    F_NUMBER(val, -1, portNum, GT_PHYSICAL_PORT_NUM);
    F_BOOL(val, -1, isTagged);
    F_NUMBER(val, -1, vlanId, GT_U16);
    F_BOOL(val, -1, packetIsLooped);
    F_BOOL(val, -1, srcIsTrunk);
    F_NUMBER(val, -1, srcTrunkId, GT_TRUNK_ID);
    F_BOOL(val, -1, hasCpuCode);
    F_ENUM(val, -1, cpuCode, CPSS_NET_RX_CPU_CODE_ENT);
    F_BOOL(val, -1, wasTruncated);
    F_NUMBER(val, -1, originByteCount, GT_U32);
    F_NUMBER(val, -1, timestamp, GT_U32);
    /* special handling for packet: can be table, string, userdata */
    F_STRUCT_CUSTOM(val, -1, packet, PACKET_STC);
}

/*************************************************************************
* prv_c_to_lua_RX_DATA_STC
*
* Description:
*       Convert RX_DATA_STC to lua
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
void prv_c_to_lua_RX_DATA_STC(
    lua_State *L,
    RX_DATA_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, devNum, GT_U8);
    FO_NUMBER(val, t, portNum, GT_PHYSICAL_PORT_NUM);
    FO_BOOL(val, t, isTagged);
    FO_NUMBER(val, t, vlanId, GT_U16);
    FO_BOOL(val, t, packetIsLooped);
    FO_BOOL(val, t, srcIsTrunk);
    FO_NUMBER(val, t, srcTrunkId, GT_TRUNK_ID);
    FO_BOOL(val, t, hasCpuCode);
    FO_ENUM(val, t, cpuCode, CPSS_NET_RX_CPU_CODE_ENT);
    FO_BOOL(val, t, wasTruncated);
    FO_NUMBER(val, t, originByteCount, GT_U32);
    FO_NUMBER(val, t, timestamp, GT_U32);
    FO_STRUCT(val, t, packet, PACKET_STC);
    lua_settop(L, t);
}
add_mgm_type(RX_DATA_STC);

/*************************************************************************
* prv_lua_to_c_CPSS_MAC_ENTRY_EXT_KEY_STC
*
* Description:
*       Convert lua CPSS_MAC_ENTRY_EXT_KEY_STC to "C"
*
* Parameters:
*       value at the top of stack
*
* Returns:
*
*
*************************************************************************/
void prv_lua_to_c_CPSS_MAC_ENTRY_EXT_KEY_STC
(
    lua_State *L,
    CPSS_MAC_ENTRY_EXT_KEY_STC *key
)
{
    F_ENUM(key, -1, entryType, CPSS_MAC_ENTRY_EXT_TYPE_ENT);
    F_NUMBER(key, -1, vid1,   GT_U32);
    switch (key->entryType)
    {
        case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_FID_VID1_E:
            F_STRUCT_CUSTOM(&(key->key.macVlan), -1, macAddr, GT_ETHERADDR)
            F_NUMBER(&(key->key.macVlan), -1, vlanId, GT_U16);
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_FID_VID1_E:
            lua_getfield(L, -1, "sip");
            prv_lua_to_c_GT_IPV4(L, key->key.ipMcast.sip);
            lua_pop(L, 1);
            lua_getfield(L, -1, "dip");
            prv_lua_to_c_GT_IPV4(L, key->key.ipMcast.dip);
            lua_pop(L, 1);
            F_NUMBER(&(key->key.ipMcast), -1, vlanId, GT_U16);
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_FID_VID1_E:
            F_ARRAY_START(&(key->key.ipMcast), -1, sip);
            {
                int idx;
                for (idx = 0; idx < 4; idx++) {
                    F_ARRAY_NUMBER(&(key->key.ipMcast), sip, idx, GT_U8);
                }
            }
            F_ARRAY_END(&(key->key.ipMcast), -1, sip);
            F_ARRAY_START(&(key->key.ipMcast), -1, dip);
            {
                int idx;
                for (idx = 0; idx < 4; idx++) {
                    F_ARRAY_NUMBER(&(key->key.ipMcast), dip, idx, GT_U8);
                }
            }
            F_ARRAY_END(&(key->key.ipMcast), -1, dip);
            F_NUMBER(&(key->key.ipMcast), -1, vlanId, GT_U16);
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E:
            lua_getfield(L, -1, "dip");
            prv_lua_to_c_GT_IPV4(L, key->key.ipv4Unicast.dip);
            lua_pop(L, 1);
            F_NUMBER(&(key->key.ipv4Unicast), -1, vrfId, GT_U32);
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E:
            lua_getfield(L, -1, "dip");
            prv_lua_to_c_GT_IPV6(L, key->key.ipv6Unicast.dip);
            lua_pop(L, 1);
            F_NUMBER(&(key->key.ipv6Unicast), -1, vrfId, GT_U32);
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E:
            /* do nothing no key */
            break;
    }
}


/*************************************************************************
* prv_lua_to_c_mask_CPSS_MAC_ENTRY_EXT_KEY_STC
*
* Description:
*       Convert lua CPSS_MAC_ENTRY_EXT_KEY_STC comparision mask to "C"
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
void prv_lua_to_c_mask_CPSS_MAC_ENTRY_EXT_KEY_STC
(
    lua_State *L,
    CPSS_MAC_ENTRY_EXT_KEY_STC *val
)
{
    cpssOsMemSet(val, 0xff, sizeof(*val));
    FM_ENUM(val, -1, entryType, CPSS_MAC_ENTRY_EXT_TYPE_ENT);
    FM_NUMBER(val, -1, vid1, GT_U32);
    if (0 == val->entryType)
    {
        F_ENUM(val, -1, entryType, CPSS_MAC_ENTRY_EXT_TYPE_ENT);
        switch (val->entryType)
        {
            case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E:
            case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_FID_VID1_E:
                FM_STRUCT_CUSTOM(&(val->key.macVlan), -1, macAddr, GT_ETHERADDR)
                FM_NUMBER(&(val->key.macVlan), -1, vlanId, GT_U16);
                break;

            case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E:
            case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_FID_VID1_E:
                lua_getfield(L, -1, "sip");
                prv_lua_to_c_mask_GT_IPV4(L, val->key.ipMcast.sip);
                lua_pop(L, 1);
                lua_getfield(L, -1, "dip");
                prv_lua_to_c_mask_GT_IPV4(L, val->key.ipMcast.dip);
                lua_pop(L, 1);
                FM_NUMBER(&(val->key.ipMcast), -1, vlanId, GT_U16);
                break;

            case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E:
            case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_FID_VID1_E:
                FM_ARRAY_START(&(val->key.ipMcast), -1, sip);
                {
                    int idx;
                    for (idx = 0; idx < 4; idx++) {
                        FM_ARRAY_NUMBER(&(val->key.ipMcast), sip, idx, GT_U8);
                    }
                }
                FM_ARRAY_END(&(val->key.ipMcast), -1, sip);
                FM_ARRAY_START(&(val->key.ipMcast), -1, dip);
                {
                    int idx;
                    for (idx = 0; idx < 4; idx++) {
                        FM_ARRAY_NUMBER(&(val->key.ipMcast), dip, idx, GT_U8);
                    }
                }
                FM_ARRAY_END(&(val->key.ipMcast), -1, dip);
                FM_NUMBER(&(val->key.ipMcast), -1, vlanId, GT_U16);
                break;

            case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E:
                lua_getfield(L, -1, "dip");
                prv_lua_to_c_mask_GT_IPV4(L, val->key.ipv4Unicast.dip);
                lua_pop(L, 1);
                F_NUMBER(&(val->key.ipv4Unicast), -1, vrfId, GT_U32);
                break;

            case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E:
                lua_getfield(L, -1, "dip");
                prv_lua_to_c_mask_GT_IPV6(L, val->key.ipv6Unicast.dip);
                lua_pop(L, 1);
                F_NUMBER(&(val->key.ipv6Unicast), -1, vrfId, GT_U32);
                break;

            case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E:
                /* do nothing no key */
                break;
        }
        val->entryType = 0;
    }
}


/*************************************************************************
* prv_c_to_lua_CPSS_MAC_ENTRY_EXT_KEY_STC
*
* Description:
*       Convert CPSS_MAC_ENTRY_EXT_KEY_STC to lua
*
* Parameters:
*
* Returns:
*       value at the top of stack
*
*
*************************************************************************/
void prv_c_to_lua_CPSS_MAC_ENTRY_EXT_KEY_STC
(
    lua_State *L,
    CPSS_MAC_ENTRY_EXT_KEY_STC *key
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(key, t, entryType, CPSS_MAC_ENTRY_EXT_TYPE_ENT);
    FO_NUMBER(key, t, vid1,   GT_U32);
    switch (key->entryType)
    {
        case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_FID_VID1_E:
            prv_c_to_lua_GT_ETHERADDR(L, &(key->key.macVlan.macAddr));
            lua_setfield(L, t, "macAddr");
            FO_NUMBER(&(key->key.macVlan), t, vlanId, GT_U16);
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_FID_VID1_E:
            prv_c_to_lua_GT_IPV4(L, key->key.ipMcast.sip);
            lua_setfield(L, t, "sip");
            prv_c_to_lua_GT_IPV4(L, key->key.ipMcast.dip);
            lua_setfield(L, t, "dip");
            FO_NUMBER(&(key->key.ipMcast), t, vlanId, GT_U16);
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_FID_VID1_E:
            FO_ARRAY_START(&(key->key.ipMcast), t, sip);
            {
                int idx;
                for (idx = 0; idx < 4; idx++) {
                    FO_ARRAY_NUMBER(&(key->key.ipMcast), sip, idx, GT_U8);
                }
            }
            FO_ARRAY_END(&(key->key.ipMcast), t, sip);
            FO_ARRAY_START(&(key->key.ipMcast), t, dip);
            {
                int idx;
                for (idx = 0; idx < 4; idx++) {
                    FO_ARRAY_NUMBER(&(key->key.ipMcast), dip, idx, GT_U8);
                }
            }
            FO_ARRAY_END(&(key->key.ipMcast), t, dip);
            FO_NUMBER(&(key->key.ipMcast), t, vlanId, GT_U16);
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E:
            prv_c_to_lua_GT_IPV4(L, key->key.ipv4Unicast.dip);
            lua_setfield(L, t, "dip");
            FO_NUMBER(&(key->key.ipv4Unicast), t, vrfId, GT_U32);
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E:
            prv_c_to_lua_GT_IPV6(L, key->key.ipv6Unicast.dip);
            lua_setfield(L, t, "dip");
            FO_NUMBER(&(key->key.ipv6Unicast), t, vrfId, GT_U32);
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E:
            /* do nothing no key */
            break;
    }

    lua_settop(L, t);
}
add_mgm_type(CPSS_MAC_ENTRY_EXT_KEY_STC);


/*************************************************************************
* prv_lua_c_print_CPSS_MAC_ENTRY_EXT_KEY_STC
*
* Description:
*       Prints CPSS_MAC_ENTRY_EXT_KEY_STC fields
*
* Parameters:
*       name                          - structure variable name or path to
*                                       structure field
*       nameExtension                 - structure field name (could be
*                                       NULL)
*       val                           - pointer to structure
*
*************************************************************************/
void prv_lua_c_print_CPSS_MAC_ENTRY_EXT_KEY_STC
(
    const GT_CHAR_PTR                   name,
    const GT_CHAR_PTR                   nameExtension,
    const CPSS_MAC_ENTRY_EXT_KEY_STC    *val
)
{
    P_NAME_CONVERSIONS(fullName, name, nameExtension);
    P_ENUM(val, fullName, entryType, CPSS_MAC_ENTRY_EXT_TYPE_ENT);
    P_NUMBER(val, fullName, vid1, GT_U32);
    switch (val->entryType)
    {
        case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_FID_VID1_E:
            P_NAME_CONVERSIONS_BEGIN(keyMacVlanName, fullName, "key.macVlan");
            {
                P_STRUCT(&(val->key.macVlan),   keyMacVlanName, macAddr,
                         GT_ETHERADDR);
                P_NUMBER(&(val->key.macVlan),   keyMacVlanName, vlanId, GT_U16);
            }
            P_NAME_CONVERSIONS_END(keyMacVlanName, fullName, "key.macVlan");
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_FID_VID1_E:
            P_NAME_CONVERSIONS_BEGIN(keyIpMcastName, fullName, "key.ipMcast");
            {
                prv_lua_c_print_GT_IPV4(keyIpMcastName, NULL,
                                        (const GT_U8 *) &(val->key.ipMcast.sip));
                prv_lua_c_print_GT_IPV4(keyIpMcastName, NULL,
                                        (const GT_U8 *) &(val->key.ipMcast.dip));
                P_NUMBER(&(val->key.ipMcast),   keyIpMcastName, vlanId, GT_U16);
            }
            P_NAME_CONVERSIONS_END(keyIpMcastName, fullName, "key.ipMcast");
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_FID_VID1_E:
            P_NAME_CONVERSIONS_BEGIN(keyIpMcastName, fullName, "key.ipMcast");
            {
                prv_lua_c_print_GT_IPV6(keyIpMcastName, NULL,
                                        (const GT_U8 *) &(val->key.ipMcast.sip));
                prv_lua_c_print_GT_IPV6(keyIpMcastName, NULL,
                                        (const GT_U8 *) &(val->key.ipMcast.dip));
                P_NUMBER(&(val->key.ipMcast),   keyIpMcastName, vlanId, GT_U16);
            }
            P_NAME_CONVERSIONS_END(keyIpMcastName, fullName, "key.ipMcast");
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E:
            P_NAME_CONVERSIONS_BEGIN(keyIpv4UnicastName, fullName, "key.ipv4Unicast");
            {
                prv_lua_c_print_GT_IPV4(keyIpv4UnicastName, NULL,
                                        (const GT_U8 *) &(val->key.ipv4Unicast.dip));
                P_NUMBER(&(val->key.ipv4Unicast),   keyIpv4UnicastName, vrfId, GT_U32);
            }
            P_NAME_CONVERSIONS_END(keyIpv6UnicastName, fullName, "key.ipv4Unicast");
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E:
            P_NAME_CONVERSIONS_BEGIN(keyIpv6UnicastName, fullName, "key.ipv6Unicast");
            {
                prv_lua_c_print_GT_IPV4(keyIpv6UnicastName, NULL,
                                        (const GT_U8 *) &(val->key.ipv6Unicast.dip));
                P_NUMBER(&(val->key.ipv6Unicast),   keyIpv6UnicastName, vrfId, GT_U32);
            }
            P_NAME_CONVERSIONS_END(keyIpv6UnicastName, fullName, "key.ipv6Unicast");
            break;

    case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E:
            /* do nothing no key */
            break;

        default:
            P_MESSAGE("Wrong mac entry type");
            break;
    }
}


/*******************************************/
/* CPSS_MAC_ENTRY_EXT_STC                  */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_mask_CPSS_MAC_ENTRY_EXT_STC
*
* Description:
*       Convert lua CPSS_MAC_ENTRY_EXT_STC comparision mask to "C"
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
void prv_lua_to_c_mask_CPSS_MAC_ENTRY_EXT_STC
(
    lua_State               *L,
    CPSS_MAC_ENTRY_EXT_STC  *val
)
{
    cpssOsMemSet(val, 0xff, sizeof(*val));
    FM_STRUCT(val, -1, key, CPSS_MAC_ENTRY_EXT_KEY_STC);
    FM_STRUCT(val, -1, dstInterface, CPSS_INTERFACE_INFO_STC);
    FM_BOOL(val, -1, isStatic);
    FM_ENUM(val, -1, daCommand, CPSS_MAC_TABLE_CMD_ENT);
    FM_ENUM(val, -1, saCommand, CPSS_MAC_TABLE_CMD_ENT);
    FM_BOOL(val, -1, daRoute);
    FM_BOOL(val, -1, mirrorToRxAnalyzerPortEn);
    FM_NUMBER(val, -1, sourceID, GT_U32);
    FM_NUMBER(val, -1, userDefined, GT_U32);
    FM_NUMBER(val, -1, daQosIndex, GT_U32);
    FM_NUMBER(val, -1, saQosIndex, GT_U32);
    FM_NUMBER(val, -1, daSecurityLevel, GT_U32);
    FM_NUMBER(val, -1, saSecurityLevel, GT_U32);
    FM_BOOL(val, -1, appSpecificCpuCode);
    FM_BOOL(val, -1, age);
    FM_BOOL(val, -1, spUnknown);
    FM_BOOL(val, -1, saMirrorToRxAnalyzerPortEn);
    FM_BOOL(val, -1, daMirrorToRxAnalyzerPortEn);
}


/*************************************************************************
* prv_lua_c_print_CPSS_MAC_ENTRY_EXT_STC
*
* Description:
*       Prints CPSS_MAC_ENTRY_EXT_STC fields
*
* Parameters:
*       name                      - structure variable name or path to
*                                   structure field
*       nameExtension             - structure field name (could be NULL)
*       val                       - pointer to structure
*
*************************************************************************/
void prv_lua_c_print_CPSS_MAC_ENTRY_EXT_STC
(
    const GT_CHAR_PTR               name,
    const GT_CHAR_PTR               nameExtension,
    const CPSS_MAC_ENTRY_EXT_STC    *val
)
{
    P_NAME_CONVERSIONS(fullName, name, nameExtension);
    P_STRUCT(val,   fullName, key, CPSS_MAC_ENTRY_EXT_KEY_STC);
    P_STRUCT(val,   fullName, dstInterface, CPSS_INTERFACE_INFO_STC);
    P_BOOL(val,     fullName, isStatic);
    P_ENUM(val,     fullName, daCommand, CPSS_MAC_TABLE_CMD_ENT);
    P_ENUM(val,     fullName, saCommand, CPSS_MAC_TABLE_CMD_ENT);
    P_BOOL(val,     fullName, daRoute);
    P_BOOL(val,     fullName, mirrorToRxAnalyzerPortEn);
    P_NUMBER(val,   fullName, sourceID, GT_U32);
    P_NUMBER(val,   fullName, userDefined, GT_U32);
    P_NUMBER(val,   fullName, daQosIndex, GT_U32);
    P_NUMBER(val,   fullName, saQosIndex, GT_U32);
    P_NUMBER(val,   fullName, daSecurityLevel, GT_U32);
    P_NUMBER(val,   fullName, saSecurityLevel, GT_U32);
    P_BOOL(val,     fullName, appSpecificCpuCode);
    P_BOOL(val,     fullName, age);
    P_BOOL(val,     fullName, spUnknown);
    P_BOOL(val,     fullName, saMirrorToRxAnalyzerPortEn);
    P_BOOL(val,     fullName, daMirrorToRxAnalyzerPortEn);
}


void prv_lua_to_c_CPSS_RLDRAM_CFG_STC(
    lua_State *L,
    CPSS_RLDRAM_CFG_STC *val
)
{
    F_ENUM(val, -1, dramSize, CPSS_DRAM_SIZE_ENT);
    F_BOOL(val, -1, overwriteDef);
    F_NUMBER(val, -1, mrs, GT_U32);
    F_NUMBER(val, -1, refreshLimit, GT_U32);
    /* done manually [[[ */
    lua_getfield(L, -1, "dramTiming");
    if (lua_istable(L, -1))
    {
        F_NUMBER(&(val->dramTiming), -1, tWL, GT_U32);
        F_NUMBER(&(val->dramTiming), -1, tRL, GT_U32);
        F_NUMBER(&(val->dramTiming), -1, tRC, GT_U32);
        F_NUMBER(&(val->dramTiming), -1, tMPSC, GT_U32);
    }
    lua_pop(L, 1);
    /* ]]] done manually */
    F_NUMBER(val, -1, pwrUpDelay, GT_U32);
    F_NUMBER(val, -1, pwrUpRefDelay, GT_U32);
}

void prv_c_to_lua_CPSS_RLDRAM_CFG_STC(
    lua_State *L,
    CPSS_RLDRAM_CFG_STC *val
)
{
    int t, t1;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, dramSize, CPSS_DRAM_SIZE_ENT);
    FO_BOOL(val, t, overwriteDef);
    FO_NUMBER(val, t, mrs, GT_U32);
    FO_NUMBER(val, t, refreshLimit, GT_U32);
    /* done manually [[[ */
    lua_newtable(L);
    t1 = lua_gettop(L);
        FO_NUMBER(&(val->dramTiming), t1, tWL, GT_U32);
        FO_NUMBER(&(val->dramTiming), t1, tRL, GT_U32);
        FO_NUMBER(&(val->dramTiming), t1, tRC, GT_U32);
        FO_NUMBER(&(val->dramTiming), t1, tMPSC, GT_U32);
    lua_setfield(L, t, "dramTiming");
    /* ]]] done manually */
    FO_NUMBER(val, t, pwrUpDelay, GT_U32);
    FO_NUMBER(val, t, pwrUpRefDelay, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_RLDRAM_CFG_STC);

void prv_lua_to_c_CPSS_DRAM_CFG_STC(
    lua_State *L,
    CPSS_DRAM_CFG_STC *val
)
{
    F_ENUM(val, -1, dramSize, CPSS_DRAM_SIZE_ENT);
    F_ENUM(val, -1, dramFrequency, CPSS_DRAM_FREQUENCY_ENT);
    F_BOOL(val, -1, overwriteDef);
    F_NUMBER(val, -1, mrs, GT_U16);
    F_NUMBER(val, -1, emrs, GT_U16);
    /* done manually [[[ */
    lua_getfield(L, -1, "dramTiming");
    if (lua_istable(L, -1))
    {
        F_NUMBER(&(val->dramTiming), -1, tdqss, GT_U8);
        F_NUMBER(&(val->dramTiming), -1, trcd, GT_U8);
        F_NUMBER(&(val->dramTiming), -1, trp, GT_U8);
        F_NUMBER(&(val->dramTiming), -1, twr, GT_U8);
        F_NUMBER(&(val->dramTiming), -1, twtr, GT_U8);
        F_NUMBER(&(val->dramTiming), -1, tras, GT_U8);
        F_NUMBER(&(val->dramTiming), -1, trrd, GT_U8);
        F_NUMBER(&(val->dramTiming), -1, tmrd, GT_U8);
        F_NUMBER(&(val->dramTiming), -1, trfc, GT_U8);
        F_NUMBER(&(val->dramTiming), -1, rd2rdDelay, GT_U8);
        F_NUMBER(&(val->dramTiming), -1, rd2wrDelay, GT_U8);
        F_NUMBER(&(val->dramTiming), -1, writeCount, GT_U8);
        F_NUMBER(&(val->dramTiming), -1, readCount, GT_U8);
    }
    lua_pop(L, 1);
    /* ]]] done manually */
    F_NUMBER(val, -1, refreshCounter, GT_U16);
    F_NUMBER(val, -1, trCdWr, GT_U8);
    F_NUMBER(val, -1, operation, GT_U8);
    F_ENUM(val, -1, dramPageSize, CPSS_DRAM_PAGE_SIZE_ENT);
}

void prv_c_to_lua_CPSS_DRAM_CFG_STC(
    lua_State *L,
    CPSS_DRAM_CFG_STC *val
)
{
    int t,t1;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, dramSize, CPSS_DRAM_SIZE_ENT);
    FO_ENUM(val, t, dramFrequency, CPSS_DRAM_FREQUENCY_ENT);
    FO_BOOL(val, t, overwriteDef);
    FO_NUMBER(val, t, mrs, GT_U16);
    FO_NUMBER(val, t, emrs, GT_U16);
    /* done manually [[[ */
    lua_newtable(L);
    t1 = lua_gettop(L);
        FO_NUMBER(&(val->dramTiming), t1, tdqss, GT_U8);
        FO_NUMBER(&(val->dramTiming), t1, trcd, GT_U8);
        FO_NUMBER(&(val->dramTiming), t1, trp, GT_U8);
        FO_NUMBER(&(val->dramTiming), t1, twr, GT_U8);
        FO_NUMBER(&(val->dramTiming), t1, twtr, GT_U8);
        FO_NUMBER(&(val->dramTiming), t1, tras, GT_U8);
        FO_NUMBER(&(val->dramTiming), t1, trrd, GT_U8);
        FO_NUMBER(&(val->dramTiming), t1, tmrd, GT_U8);
        FO_NUMBER(&(val->dramTiming), t1, trfc, GT_U8);
        FO_NUMBER(&(val->dramTiming), t1, rd2rdDelay, GT_U8);
        FO_NUMBER(&(val->dramTiming), t1, rd2wrDelay, GT_U8);
        FO_NUMBER(&(val->dramTiming), t1, writeCount, GT_U8);
        FO_NUMBER(&(val->dramTiming), t1, readCount, GT_U8);
    lua_setfield(L, t, "dramTiming");
    /* ]]] done manually */
    FO_NUMBER(val, t, refreshCounter, GT_U16);
    FO_NUMBER(val, t, trCdWr, GT_U8);
    FO_NUMBER(val, t, operation, GT_U8);
    FO_ENUM(val, t, dramPageSize, CPSS_DRAM_PAGE_SIZE_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_DRAM_CFG_STC);

enumDescr enumDescr_CPSS_SHIFT_OPERATION_ENT[] = {
    ENUM_ENTRY(CPSS_SHIFT_OPERATION_SHIFT_LEFT_E),
    ENUM_ENTRY(CPSS_SHIFT_OPERATION_SHIFT_RIGHT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_SHIFT_OPERATION_ENT);

enumDescr enumDescr_CPSS_ADJUST_OPERATION_ENT[] = {
    ENUM_ENTRY(CPSS_ADJUST_OPERATION_ADD_E),
    ENUM_ENTRY(CPSS_ADJUST_OPERATION_SUBSTRUCT_E),
    { 0, NULL }
};
add_mgm_enum(CPSS_ADJUST_OPERATION_ENT);

use_prv_enum(PRV_CPSS_SIP_ENT);
enumDescr enumDescr_PRV_CPSS_SIP_ENT[] = {
    ENUM_ENTRY(PRV_CPSS_SIP_LEGACY_E),
    ENUM_ENTRY(PRV_CPSS_SIP_5_E),
    ENUM_ENTRY(PRV_CPSS_SIP_5_10_E),
    ENUM_ENTRY(PRV_CPSS_SIP_5_15_E),
    ENUM_ENTRY(PRV_CPSS_SIP_5_20_E),
    ENUM_ENTRY(PRV_CPSS_SIP_MAX_NUM_E),
    { 0, NULL }
};
add_mgm_enum(PRV_CPSS_SIP_ENT);


/*************************************************************************
* prv_lua_c_print_CPSS_TRUNK_MEMBER_STC
*
* Description:
*       Prints CPSS_TRUNK_MEMBER_STC fields
*
* Parameters:
*       name                          - structure variable name or path to
*                                       structure field
*       nameExtension                 - structure field name (could be
*                                       NULL)
*       val                           - pointer to structure
*
*************************************************************************/
void prv_lua_c_print_CPSS_TRUNK_MEMBER_STC
(
    const GT_CHAR_PTR                   name,
    const GT_CHAR_PTR                   nameExtension,
    const CPSS_TRUNK_MEMBER_STC         *val
)
{
    P_NAME_CONVERSIONS(fullName, name, nameExtension);
    P_NUMBER(val,   fullName, port, GT_PHYSICAL_PORT_NUM);
    P_NUMBER(val,   fullName, hwDevice, GT_HW_DEV_NUM);
}


/*************************************************************************
* prv_lua_c_array_print_CPSS_TRUNK_MEMBER_STC
*
* Description:
*       Prints CPSS_TRUNK_MEMBER_STC array item
*
* Parameters:
*       name                          - structure variable name or path to
*                                       structure field
*       nameExtension                 - structure field name (could be
*                                       NULL)
*       array_items_count             - array items count
*       array                         - pointer to array
*
*************************************************************************/
void prv_lua_c_array_print_CPSS_TRUNK_MEMBER_STC
(
    const GT_CHAR_PTR                   name,
    const GT_CHAR_PTR                   nameExtension,
    const GT_U32                        array_items_count,
    const CPSS_TRUNK_MEMBER_STC         *array
)
{
    P_NAME_CONVERSIONS(fullName, name, nameExtension);
    P_STRUCT_ARRAY_START(array, fullName);
    {
        GT_U32 idx;
        for (idx = 0; idx < array_items_count; idx++)
        {
            P_ARRAY_ITEM_NAME_CONVERSIONS_BEGIN(arrayItemName, fullName, idx);
            {
                P_STRUCT_ARRAY_ITEM(&(array[idx]), arrayItemName,
                                    CPSS_TRUNK_MEMBER_STC)
            }
            P_ARRAY_ITEM_NAME_CONVERSIONS_END(arrayItemName, fullName, idx);
        }
    }
    P_STRUCT_ARRAY_END(array, fullName);

    P_CONDITIONAL_MESSAGE(0 == array_items_count, "No trunk members.");
}

use_prv_enum(CPSS_GEN_DRAGONITE_DATA_TYPE_ENT);
use_prv_struct(CPSS_GEN_DRAGONITE_CONFIG_STC);
use_prv_struct(CPSS_GEN_DRAGONITE_SYSTEM_STC);
use_prv_struct(CPSS_GEN_DRAGONITE_CHIP_STC);
use_prv_struct(CPSS_GEN_DRAGONITE_PORT_COMMAND_STC);
use_prv_struct(CPSS_GEN_DRAGONITE_PORT_COUNTERS_STC);
use_prv_struct(CPSS_GEN_DRAGONITE_PORT_STATUSES_STC);
use_prv_struct(CPSS_GEN_DRAGONITE_PORT_MEASUREMENTS_STC);
use_prv_struct(CPSS_GEN_DRAGONITE_PORT_LAYER2_STC);

void prv_lua_to_c_CPSS_GEN_DRAGONITE_DATA_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_DATA_STC *val
)
{
    F_ENUM(val, -1, dataType, CPSS_GEN_DRAGONITE_DATA_TYPE_ENT);
    /* done manually [[[ */
    lua_getfield(L, -1, "dragoniteData");
    if (lua_istable(L, -1))
    {
        switch (val->dataType)
        {
            case CPSS_GEN_DRAGONITE_DATA_TYPE_CONFIG_E:
                F_STRUCT(&(val->dragoniteData), -1, config, CPSS_GEN_DRAGONITE_CONFIG_STC);
                break;
            case CPSS_GEN_DRAGONITE_DATA_TYPE_SYSTEM_E:
                F_STRUCT(&(val->dragoniteData), -1, system, CPSS_GEN_DRAGONITE_SYSTEM_STC);
                break;
            case CPSS_GEN_DRAGONITE_DATA_TYPE_CHIP_E:
#define ARR(name,cnt,type) \
                F_ARRAY_START(&(val->dragoniteData), -1, name); \
                { \
                    int idx; \
                    for (idx = 0; idx < cnt; idx++) { \
                        F_ARRAY_STRUCT(&(val->dragoniteData), name, idx, type); \
                    } \
                } \
                F_ARRAY_END(&(val->dragoniteData), -1, name);
                ARR(chip, CPSS_GEN_DRAGONITE_CHIPS_NUM_CNS, CPSS_GEN_DRAGONITE_CHIP_STC);
                break;
            case CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_COMMANDS_E:
                ARR(portCmd, CPSS_GEN_DRAGONITE_PORTS_NUM_CNS, CPSS_GEN_DRAGONITE_PORT_COMMAND_STC);
                break;
            case CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_CNTRS_E:
                F_STRUCT(&(val->dragoniteData), -1, portCntrs, CPSS_GEN_DRAGONITE_PORT_COUNTERS_STC);
                break;
            case CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_STATUS_E:
                ARR(portStat, CPSS_GEN_DRAGONITE_PORTS_NUM_CNS, CPSS_GEN_DRAGONITE_PORT_STATUSES_STC);
                break;
            case CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_MEASURE_E:
                ARR(portMeasure, CPSS_GEN_DRAGONITE_PORTS_NUM_CNS, CPSS_GEN_DRAGONITE_PORT_MEASUREMENTS_STC);
                break;
            case CPSS_GEN_DRAGONITE_DATA_TYPE_LAYER2_E:
                ARR(layer2, CPSS_GEN_DRAGONITE_PORTS_NUM_CNS, CPSS_GEN_DRAGONITE_PORT_LAYER2_STC);
                break;
            default:
                /* CPSS_GEN_DRAGONITE_DATA_TYPE_DEBUG_E */
                /* CPSS_GEN_DRAGONITE_DATA_TYPE_OTHER_E */
                break;
#undef ARR
        }
    }
    lua_pop(L, 1);
    /* ]]] done manually */
}

void prv_c_to_lua_CPSS_GEN_DRAGONITE_DATA_STC(
    lua_State *L,
    CPSS_GEN_DRAGONITE_DATA_STC *val
)
{
    int t, t1;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(val, t, dataType, CPSS_GEN_DRAGONITE_DATA_TYPE_ENT);
    /* done manually [[[ */
    lua_newtable(L);
    t1 = lua_gettop(L);
        switch (val->dataType)
        {
            case CPSS_GEN_DRAGONITE_DATA_TYPE_CONFIG_E:
                FO_STRUCT(&(val->dragoniteData), t1, config, CPSS_GEN_DRAGONITE_CONFIG_STC);
                break;
            case CPSS_GEN_DRAGONITE_DATA_TYPE_SYSTEM_E:
                FO_STRUCT(&(val->dragoniteData), t1, system, CPSS_GEN_DRAGONITE_SYSTEM_STC);
                break;
            case CPSS_GEN_DRAGONITE_DATA_TYPE_CHIP_E:
#define ARR(name,cnt,type) \
                FO_ARRAY_START(&(val->dragoniteData), t1, name); \
                { \
                    int idx; \
                    for (idx = 0; idx < cnt; idx++) { \
                        FO_ARRAY_STRUCT(&(val->dragoniteData), name, idx, type); \
                    } \
                } \
                FO_ARRAY_END(&(val->dragoniteData), t1, name);
                ARR(chip, CPSS_GEN_DRAGONITE_CHIPS_NUM_CNS, CPSS_GEN_DRAGONITE_CHIP_STC);
                break;
            case CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_COMMANDS_E:
                ARR(portCmd, CPSS_GEN_DRAGONITE_PORTS_NUM_CNS, CPSS_GEN_DRAGONITE_PORT_COMMAND_STC);
                break;
            case CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_CNTRS_E:
                FO_STRUCT(&(val->dragoniteData), t1, portCntrs, CPSS_GEN_DRAGONITE_PORT_COUNTERS_STC);
                break;
            case CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_STATUS_E:
                ARR(portStat, CPSS_GEN_DRAGONITE_PORTS_NUM_CNS, CPSS_GEN_DRAGONITE_PORT_STATUSES_STC);
                break;
            case CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_MEASURE_E:
                ARR(portMeasure, CPSS_GEN_DRAGONITE_PORTS_NUM_CNS, CPSS_GEN_DRAGONITE_PORT_MEASUREMENTS_STC);
                break;
            case CPSS_GEN_DRAGONITE_DATA_TYPE_LAYER2_E:
                ARR(layer2, CPSS_GEN_DRAGONITE_PORTS_NUM_CNS, CPSS_GEN_DRAGONITE_PORT_LAYER2_STC);
                break;
            default:
                /* CPSS_GEN_DRAGONITE_DATA_TYPE_DEBUG_E */
                /* CPSS_GEN_DRAGONITE_DATA_TYPE_OTHER_E */
                break;
        }
#undef ARR

    lua_settop(L, t1);
    lua_setfield(L, t, "dragoniteData");
    /* ]]] done manually */
    lua_settop(L, t);
}
add_mgm_type(CPSS_GEN_DRAGONITE_DATA_STC);
