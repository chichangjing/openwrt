/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* luaCiderPpAccess.c
*
* DESCRIPTION:
*       Provides wrappers for register/mem access and for bitfieds extract
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <lua.h>

static int hex2int(char ch);

/*******************************************************************************
* wrlReadDeviceMemory
*
* DESCRIPTION:
*       Read Pp memory
*
* INPUTS:
*       L                   - lua state
*          deviceId
*          portGroupId
*          addr
*          length           - in bits
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       Lua's userdata object
*
* COMMENTS:
*
*******************************************************************************/
static int wrlReadDeviceMemory(
    lua_State *L
)
{
    GT_STATUS   rc;
    GT_U8       devNum;
    GT_U32      portGroupId;
    GT_U32      addr;
    GT_U32      length;
    GT_U32      *data;

    devNum = (GT_U8)lua_tonumber(L, 1);
    portGroupId = (GT_U32)lua_tonumber(L, 2);
    addr = (GT_U32)lua_tonumber(L, 3);
    length = (GT_U32)lua_tonumber(L, 4);

    /* align to 32 bits, length == number_of_32bit_words */
    length = (length + 31) / 32;
    data = (GT_U32*)lua_newuserdata(L, length * 4);

    rc = cpssDrvPpHwRamRead(devNum, portGroupId, addr, length, data);

    if (rc == GT_OK)
    {
        return 1;
    }
    lua_pushinteger(L, rc);
    return 1;
}

/*******************************************************************************
* wrlWriteDeviceMemory
*
* DESCRIPTION:
*       Write Pp memory
*
* INPUTS:
*       L                   - lua state
*          deviceId
*          portGroupId
*          addr
*          length           - in bits
*          newData
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       Lua's number: status
*
* COMMENTS:
*
*******************************************************************************/
static int wrlWriteDeviceMemory(
    lua_State *L
)
{
    GT_STATUS   rc;
    GT_U8       devNum;
    GT_U32      portGroupId;
    GT_U32      addr;
    GT_U32      length;
    GT_U32      *data;

    devNum = (GT_U8)lua_tonumber(L, 1);
    portGroupId = (GT_U32)lua_tonumber(L, 2);
    addr = (GT_U32)lua_tonumber(L, 3);
    length = (GT_U32)lua_tonumber(L, 4);
    data = (GT_U32*)lua_touserdata(L, 5);
    length = (GT_U32)lua_objlen(L, 5);

    /* align to 32 bits */
    length &= 0xfffffffc;

    rc = cpssDrvPpHwRamWrite(devNum, portGroupId, addr, length/4, data);

    lua_pushinteger(L, rc);
    return 1;
}

/*******************************************************************************
* wrlGetBits
*
* DESCRIPTION:
*       Return bitfield of num/bytearray
*
* INPUTS:
*       L                   - lua state
*          value            - integer or userdata
*          msb
*          lsb
*          len           
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       Lua's integer
*
* COMMENTS:
*
*******************************************************************************/
static int wrlGetBits(
    lua_State *L
)
{
    int msb, lsb, len;
    GT_U32  data = 0;
    GT_U32  data_high = 0;
    msb = (int)lua_tonumber(L, 2);
    lsb = (int)lua_tonumber(L, 3);
    len = (int)lua_tonumber(L, 4);

    if (lua_isnumber(L, 1))
    {
        GT_U32 srcdata;
        srcdata = (GT_U32)lua_tonumber(L, 1);
        if (lsb)
            data = srcdata >> lsb;
        else
            data = srcdata;
        if (len && len < 32)
            data &= (1<<len)-1;
    }
    if (lua_isuserdata(L, 1))
    {
        GT_U8  *bytedata;
        GT_U32  bytelen;

        bytedata = (GT_U8*)lua_touserdata(L, 1);
        bytelen = (GT_U32)lua_objlen(L, 1);
        
        msb = lsb + len - 1;
        if ((GT_U32)msb > bytelen * 8)
        {
            msb = bytelen * 8 - 1;
        }

        for (;msb >= lsb; msb--,len--)
        {
            if (len == 32)
            {
                data_high = data;
                data = 0;
            }
            data = data + data;
            if (bytedata[msb/8] & (1 << (msb%8)))
                data |= 1;
        }
    }

    if (data_high == 0)
        lua_pushnumber(L, data);
    else
    {
        char hexstring[20];
        cpssOsSprintf(hexstring, "0x%08x%08x", data_high, data);
        lua_pushstring(L, hexstring);
    }
    return 1;
}

/*******************************************************************************
* wrlSetBits
*
* DESCRIPTION:
*       Set specivied bits in value to fieldValue
*       Return modified value
*
* INPUTS:
*       L                   - lua state
*          value            - integer or userdata
*          msb
*          lsb
*          len           
*          fieldValue
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       Lua's integer or userdata
*
* COMMENTS:
*
*******************************************************************************/
static int wrlSetBits(
    lua_State *L
)
{
    int msb, lsb, len;
    GT_U32  fieldValue = 0;
    GT_U32  fieldValue_high = 0;
    msb = (int)lua_tonumber(L, 2);
    lsb = (int)lua_tonumber(L, 3);
    len = (int)lua_tonumber(L, 4);
    if (lua_type(L, 5) == LUA_TSTRING)
    {
        /* value is hex string */
        const char *s;
        size_t l;
        int h;
        s = lua_tolstring(L, 5, &l);
        /* remove 0x prefix */
        if (l > 1 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
        {
            s += 2;
            l -= 2;
        }
        for (;l;s++,l--)
        {
            h = hex2int(*s);
            if (h < 0)
                break;
            fieldValue_high <<= 4;
            fieldValue_high |= (fieldValue >> 28) & 0x0000000f;
            fieldValue <<= 4;
            fieldValue |= h & 0x0000000f;
        }
    }
    else if (lua_isnumber(L, 5))
    {
        fieldValue = (GT_U32)lua_tonumber(L, 5);
    } else {
        lua_pushnil(L);
        return 1;
    }

    if (lua_isnumber(L, 1))
    {
        GT_U32 data, mask;
        data = (GT_U32)lua_tonumber(L, 1);

        if (len && len < 32)
            mask = (1<<len)-1;
        else
            mask = 0xffffffff;

        if (lsb)
        {
            fieldValue <<= lsb;
            mask <<= lsb;
        }
        data &= ~mask;
        data |= fieldValue & mask;
        lua_pushinteger(L, data);
        return 1;
    }
    if (lua_isuserdata(L, 1))
    {
        GT_U8  *bytedata;
        GT_U32  bytelen;
        GT_U32  bitno;

        lua_pushvalue(L, 1);
        bytedata = (GT_U8*)lua_touserdata(L, -1);
        bytelen = (GT_U32)lua_objlen(L, -1);
        
        if ((GT_U32)msb > bytelen * 8)
            msb = bytelen * 8 - 1;

        for (bitno = 0; lsb <= msb; lsb++, bitno++)
        {
            if (bitno == 32)
                fieldValue = fieldValue_high;

            bytedata[lsb/8] &= ~(1 << (lsb%8));
            if (fieldValue & 1)
                bytedata[lsb/8] |= (1 << (lsb%8));
            fieldValue >>= 1;
        }
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

/*******************************************************************************
* wrlDumpUserdata
*
* DESCRIPTION:
*       Dump userdata object to hex
*
* INPUTS:
*       L                   - lua state
*          value            - userdata
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       Lua's hex string
*
* COMMENTS:
*
*******************************************************************************/
static int wrlDumpUserdata(
    lua_State *L
)
{
    GT_U8   *data;
    GT_U32  len;
    GT_U32  k, p;
    char    buf[80];

    if (!lua_isuserdata(L, 1))
    {
        lua_pushliteral(L, "");
        return 1;
    }
    data = (GT_U8*)lua_touserdata(L, 1);
    len = (GT_U32)lua_objlen(L, 1);

    lua_pushliteral(L, "");
    for (k = p = 0; k < len; k++)
    {
        static const char hex[]="0123456789ABCDEF";
        buf[p++] = hex[(data[k]>>4) & 0x0f];
        buf[p++] = hex[data[k] & 0x0f];
        if (p >= 64)
        {
            lua_pushlstring(L, buf, p);
            lua_concat(L, 2);
            p = 0;
        }
    }
    lua_pushlstring(L, buf, p);
    lua_concat(L, 2);
    return 1;
}

static int hex2int(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    return -1;
}
/*******************************************************************************
* wrlToUserdata
*
* DESCRIPTION:
*       Convert hex string to userdata object
*
* INPUTS:
*       L                   - lua state
*          value            - hex string
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       Lua's userdata
*
* COMMENTS:
*
*******************************************************************************/
static int wrlToUserdata(
    lua_State *L
)
{
    const char *s;
    size_t l;

    GT_U8   buf[200];
    GT_U32  bufLen = 0;
    GT_U8   *data;

    if (!lua_isstring(L, 1))
    {
        return 0; /* nil */
    }
    s = lua_tolstring(L, 1, &l);

    /* remove leading 0x */
    if (l > 1 && s[0] == '0' && s[1] == 'x')
    {
        s+=2;
        l-=2;
    }
    if (l & 1 || l > sizeof(buf)*2)
    {
        /* odd len or too long, must be multiple of 2 */
        return 0; /* nil */
    }
    for (; l; s+=2,l-=2)
    {
        int h0, h1;
        h0 = hex2int(s[0]);
        h1 = hex2int(s[1]);
        if (h0 < 0 || h1 < 0)
        {
            return 0; /* nil */
        }
        buf[bufLen++] = (GT_U8)((h0 << 4) + h1);
    }
    data = (GT_U8*)lua_newuserdata(L, bufLen);
    for (l = 0; l < bufLen; l++)
        data[l] = buf[l];
    return 1;
}

/*******************************************************************************
* prvCiderPpAccess_register
*
* DESCRIPTION:
*       register tbuf functions
*
* INPUTS:
*       L                   - lua state
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
int prvCiderPpAccess_register(lua_State *L)
{
    lua_register(L, "cider_ReadPpMemory", wrlReadDeviceMemory);
    lua_register(L, "cider_WritePpMemory", wrlWriteDeviceMemory);
    lua_register(L, "cider_GetBits", wrlGetBits);
    lua_register(L, "cider_SetBits", wrlSetBits);
    lua_register(L, "cider_DumpUserdata", wrlDumpUserdata);
    lua_register(L, "cider_ToUserdata", wrlToUserdata);
    return 0;
}

/*
 * Short reference
 * ===============
 *
 * cider_ReadPpMemory(deviceId, portGroupId, addr, length)
 *      Read Pp memory, return userdata object
 *
 * cider_WritePpMemory(deviceId, portGroupId, addr, length, newData)
 *      Write Pp memory, return status
 *
 * cider_GetBits(value, msb, lsb, len)
 *      Return bitfield of num/bytearray
 *
 * cider_SetBits(value, msb, lsb, len, fieldValue)
 *      Set specivied bits in value to fieldValue
 *      Return modified value
 *
 * cider_DumpUserdata(value)
 *      Dump userdata object to hex string
 *
 */
