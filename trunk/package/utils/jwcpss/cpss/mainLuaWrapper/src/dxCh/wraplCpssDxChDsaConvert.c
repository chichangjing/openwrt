/*******************************************************************************
*              (C), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplCpssDxChDsaConvert.c
*
* DESCRIPTION:
*       conver DSA struct to a string and back. Wrappers over the functions:
*       - cpssDxChNetIfDsaTagBuild
*       - cpssDxChNetIfDsaTagParse
*
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/
#include <cpss/generic/cpssTypes.h>
#include <generic/private/prvWraplGeneral.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
#include <cpssCommon/wraplCpssExtras.h>
#include <cpssCommon/wrapCpssDebugInfo.h>

#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>

/* declarations */
use_prv_struct(CPSS_DXCH_NET_DSA_PARAMS_STC)

/* the longest DSA tag - eDSA - consists of  four 32-bit words */
#define DSA_MAX_LEN     (4*32)

#define QUIT_IF_NOT_OK(value, errmsg)           \
    if ((value) != GT_OK)                       \
    {                                           \
        lua_pushnumber(L, (value));             \
        lua_pushstring(L, (errmsg));            \
        return 2;                               \
    }
#define QUIT_IF_BAD_PARAM(value, errmsg)        \
    if ((value) == GT_BAD_PARAM)                \
    {                                           \
        lua_pushnumber(L, (value));             \
        lua_pushstring(L,(errmsg));             \
        return 2;                               \
    }

static int str2bin(char c)
{
    switch(c)
    {
        case '0':           return 0;
        case '1':           return 1;
        case '2':           return 2;
        case '3':           return 3;
        case '4':           return 4;
        case '5':           return 5;
        case '6':           return 6;
        case '7':           return 7;
        case '8':           return 8;
        case '9':           return 9;
        case 'a': case 'A': return 10;
        case 'b': case 'B': return 11;
        case 'c': case 'C': return 12;
        case 'd': case 'D': return 13;
        case 'e': case 'E': return 14;
        case 'f': case 'F': return 15;

        default: /* an error */
            return -1;
    }
}


/*******************************************************************************
* wrlDxChDsaToStruct
*
* DESCRIPTION:
*       Transform a DSA struct from a string that appropriate to a DSA tag's
*       hex view to a struct
*
* INPUTS:
*       L           - lua state
*       L[0]        - device number (different DXCH devices may support
*                    differently the DSA tag)
*       L[1]        - a  string with a hex view of DSA tag
*
* OUTPUTS:
*       1) an error code
*       2) an error message (if error code ~=0) or resulting DSA tag table
*
* RETURNS:
*       2 values are returned:
*           if failed - an error code and a message (string)
*           if ok     - a GT_OK and the resulting table appropriate to
*                       the DSA tag struct.

*
* COMMENTS:
*
*******************************************************************************/
int wrlDxChDsaToStruct
(
    lua_State *L
)
{
    const char *strPtr;
    int        b1;
    int        b2;

    CPSS_DXCH_NET_DSA_PARAMS_STC  dsaInfo;
    GT_U8     devNum;
    GT_U8     buf[DSA_MAX_LEN] = {0};
    size_t    len = 0;   /* actual length of the sting pointed by strPtr  */
    GT_32     i;
    GT_STATUS rc;

    cpssOsMemSet(&dsaInfo, 0, sizeof(dsaInfo));

    /* get the first parameter devNum */
    rc = GT_OK; /* required because PARAM_xxx don't modify an rc if no error */
    PARAM_NUMBER(rc, devNum, 1, GT_U8);
    QUIT_IF_BAD_PARAM(rc, "A bad devNum's format");

    /* get the second parameter str */
    if (!lua_isstring(L, 2))
    {
        QUIT_IF_BAD_PARAM(GT_BAD_PARAM, "DSA tag is not a string");
    }
    strPtr = lua_tolstring(L, 2, &len);

    if (len  > sizeof(buf) * 2)  /* sizeof(buf) = DSA_MAX_LEN */
    {
        QUIT_IF_BAD_PARAM(GT_BAD_PARAM, "Too long DSA string");
    }

    if (len % 2)
    {
        QUIT_IF_BAD_PARAM(GT_BAD_PARAM, "A length of DSA string should be even");
    }

    /* transform string to binary */
    for (i=0; *strPtr; i++)
    {
        b1 = str2bin(*strPtr++);
        b2 = str2bin(*strPtr++);
        if (b1 < 0 || b2 < 0)
        {
            QUIT_IF_BAD_PARAM(GT_BAD_PARAM,
                          "An unexpected symbol in the string (0-9, a-f, A-F)");
        }
        buf[i] = (GT_U8)((b1 << 4) | (b2 & 0x0f));
    }

    rc =  cpssDxChNetIfDsaTagParse(devNum, buf, &dsaInfo);
    QUIT_IF_NOT_OK(rc, "cpssDxChNetIfDsaTagParse failed");

    /* push the the results into the stack */
    lua_pushnumber(L, GT_OK);
    prv_c_to_lua_CPSS_DXCH_NET_DSA_PARAMS_STC(L, &dsaInfo);
    return 2;
}

/*******************************************************************************
* wrlDxChDsaToString
*
* DESCRIPTION:
*       Transform a DSA struct into a string appropriate to a tag's binary
*       representation.
*
* INPUTS:
*       L           - lua state
*       L[0]        - device number (different DXCH devices may support
*                    differently the DSA tag)
*       L[1]        - (pointer to) the DSA struct
*
* OUTPUTS:
*       1) an error code
*       2) an error message (if error code ~=0) or resulting DSA string
*          otherwise
*
* RETURNS:
*       2
*
* COMMENTS:
*
*******************************************************************************/
int wrlDxChDsaToString
(
    lua_State *L
)
{
    CPSS_DXCH_NET_DSA_PARAMS_STC  dsaInfo;
    GT_U8     devNum;
    GT_U8     buf[DSA_MAX_LEN] = {0};
    GT_U8     strBuf[2*DSA_MAX_LEN + 1]; /* an 8bit hex is represented by 2 chars. Plus '\0' */
    GT_U8     *charPtr;
    GT_32     bufLen = 0;   /* actual length of the buf  */
    GT_32     i;
    GT_STATUS rc;
    GT_U8 hex[] = "0123456789ABCDEF";

    cpssOsMemSet(&dsaInfo, 0, sizeof(dsaInfo));

    rc = GT_OK; /* required because PARAM_xxx don't modify an rc if no error */
    PARAM_NUMBER(rc, devNum, 1, GT_U8);
    QUIT_IF_BAD_PARAM(rc, "a bad devNum's format");

    PARAM_STRUCT(rc, &dsaInfo, 2, CPSS_DXCH_NET_DSA_PARAMS_STC);
    QUIT_IF_BAD_PARAM(rc, "a bad dsaInfo's format");

    rc = cpssDxChNetIfDsaTagBuild(devNum, &dsaInfo, buf);
    QUIT_IF_NOT_OK(rc, "cpssDxChNetIfDsaTagBuild failed");
    /* get actual length of the buf */
    switch (dsaInfo.commonParams.dsaTagType)
    {
        case CPSS_DXCH_NET_DSA_1_WORD_TYPE_ENT:
            bufLen = 4 * 1;
            break;
        case CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT:
            bufLen = 4 * 2;
            break;
        case CPSS_DXCH_NET_DSA_3_WORD_TYPE_ENT:
            bufLen = 4 * 3;
            break;
        case CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT:
            bufLen = 4 * 4;
            break;
        default:
            QUIT_IF_BAD_PARAM(GT_BAD_PARAM, "A bad DSA tag's type");
    }
    /* transform binary to hex string representation */
    charPtr = strBuf;
    for (i=0; i<bufLen; i++)
    {
        *charPtr++ = hex[buf[i] >> 4];
        *charPtr++ = hex[buf[i] & 0x0f];
    }
    *charPtr = '\0';
    /* push the results into the stack */
    lua_pushnumber(L, GT_OK);
    lua_pushstring(L, (char *)strBuf);
    return 2;
}
