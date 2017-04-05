/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssOsStr.h
*
* DESCRIPTION:
*       Operating System wrapper. String facility.
*
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*******************************************************************************/

#ifndef __cpssOsStrh
#define __cpssOsStrh

#ifdef __cplusplus
extern "C" {
#endif

/************* Includes *******************************************************/

#include <cpss/extServices/os/gtOs/gtGenTypes.h>

/************* Defines ********************************************************/

/*******************************************************************************
* CPSS_OS_STR_LEN_FUNC
*
* DESCRIPTION:
*       Determine the length of a string.
*
* INPUTS:
*       source  - string
*
* OUTPUTS:
*       None
*
* RETURNS:
*       size    - number of characters in string, not including EOS.
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_U32 (*CPSS_OS_STR_LEN_FUNC)
(
    IN const GT_VOID * source
);

/*******************************************************************************
* CPSS_OS_STR_CPY_FUNC
*
* DESCRIPTION:
*       Copies string 'source' (including EOS) to string 'dest'.
*
* INPUTS:
*       dest    - pointer to a buffer for the copied string
*       source  - string to copy
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Pointer to the 'dest'.
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_CHAR * (*CPSS_OS_STR_CPY_FUNC)
(
    IN GT_CHAR         *dest,
    IN const GT_CHAR   *source
);

/*******************************************************************************
* osStrNCpy
*
* DESCRIPTION:
*       Copies string 'source' (including EOS) to string 'dest'.
*
* INPUTS:
*       dest    - pointer to a buffer for the copied string
*       source  - string to copy
*       len     - copy no more than len characters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Pointer to the 'dest'.
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_CHAR * (*CPSS_OS_STR_N_CPY_FUNC)
(
    IN GT_CHAR         *dest,
    IN const GT_CHAR   *source,
    IN GT_U32       len
);

/*******************************************************************************
* CPSS_OS_STR_CHR_FUNC
*
* DESCRIPTION:
*       Find the first occurrence of a character in a string.
*
* INPUTS:
*       source      - string to look in to
*       character   - character to look for
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       pointer to the found character or
*       NULL - if character were not found
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_CHAR * (*CPSS_OS_STR_CHR_FUNC)
(
    IN const GT_CHAR   *source,
    IN GT_32        character
);

/*******************************************************************************
* CPSS_OS_STR_STR_FUNC
*
* DESCRIPTION:
*       Locate a substring
*
* INPUTS:
*       source      - string to look in to
*       needle      - substring to find
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       pointer to the found substring or
*       NULL - if character were not found
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_CHAR * (*CPSS_OS_STR_STR_FUNC)
(
    IN const GT_CHAR *source,
    IN const GT_CHAR *needle
);

/*******************************************************************************
* CPSS_OS_STR_REV_CHR_FUNC
*
* DESCRIPTION:
*       Find the last occurrence of character in a string.
*
* INPUTS:
*       source      - string to look in to
*       character   - character to look for
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       pointer to the found character or
*       NULL - if character were not found
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_CHAR * (*CPSS_OS_STR_REV_CHR_FUNC)
(
    IN const GT_CHAR   *source,
    IN GT_32        character
);

/*******************************************************************************
* CPSS_OS_STR_CMP_FUNC
*
* DESCRIPTION:
*       Compares lexicographically tow null terminating strings.
*
* INPUTS:
*       str1   - string to look in to
*       str2   - character to look for
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       > 0  - if str1 is alfabetic bigger than str2
*       == 0 - if str1 is equal to str2
*       < 0  - if str1 is alfabetic smaller than str2
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_32 (*CPSS_OS_STR_CMP_FUNC)
(
    IN const GT_CHAR   *str1,
    IN const GT_CHAR   *str2
);

/*******************************************************************************
* osStrNCmp
*
* DESCRIPTION:
*       Compares lexicographically tow null terminating strings.
*
* INPUTS:
*       str1   - string to look in to
*       str2   - character to look for
*       len    - number of characters to compare
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       > 0  - if str1 is alfabetic bigger than str2
*       == 0 - if str1 is equal to str2
*       < 0  - if str1 is alfabetic smaller than str2
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_32 (*CPSS_OS_STR_N_CMP_FUNC)
(
    IN const GT_CHAR *str1,
    IN const GT_CHAR *str2,
    IN GT_U32        len
);

/*******************************************************************************
* CPSS_OS_STR_CAT_FUNC
*
* DESCRIPTION:
*       Appends a copy of string 'str2' to the end of string 'str1'.
*
* INPUTS:
*       str1   - destination string
*       str2   - sthring to add the destination string
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Pointer to the destination string (str1)
*
* COMMENTS:
*       The resulting string is null-terminated.
*
*******************************************************************************/
typedef GT_CHAR * (*CPSS_OS_STR_CAT_FUNC)
(
    IN GT_CHAR         *str1,
    IN const GT_CHAR   *str2
);

/*******************************************************************************
* CPSS_OS_STR_N_CAT_FUNC
*
* DESCRIPTION:
*       Appends up to 'len' characters from string 'str1' to the end
*       of string 'str2'.
*
* INPUTS:
*       str1   - destination string
*       str2   - sthring to add the destination string
*       len    - number of characters to concat
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Pointer to the destination string (str1)
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_CHAR * (*CPSS_OS_STR_N_CAT_FUNC)
(
    IN GT_CHAR         *str1,
    IN const GT_CHAR   *str2,
    IN GT_U32          len
);

/*******************************************************************************
* CPSS_OS_TO_UPPER_FUNC
*
* DESCRIPTION:
*       Converts a lower-case letter to the corresponding upper-case letter.
*
* INPUTS:
*       character   - a character to convert to upper case
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       The upper case character
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_32 (*CPSS_OS_TO_UPPER_FUNC)
(
    IN const GT_32 character
);

/*******************************************************************************
* CPSS_OS_STR_TO_32_FUNC
*
* DESCRIPTION:
*       Converts the initial portion of the string s to long integer
*       representation.
*
* INPUTS:
*       string   - a string to convert
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       The converted value represented as a long.
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_32 (*CPSS_OS_STR_TO_32_FUNC)
(
    IN const GT_CHAR *string
);


/*******************************************************************************
* CPSS_OS_STR_TO_U32_FUNC
*
* DESCRIPTION:
*       Converts the initial portion of the string s to unsigned long integer
*       representation.
*
* INPUTS:
*       string   - a string to convert
*       endptr   - ptr to final string
*       base     - radix
*
* OUTPUTS:
*       endptr   - ptr to final string
*
* RETURNS:
*       The converted value or ZERO, if no conversion could be performed.
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_U32 (*CPSS_OS_STR_TO_U32_FUNC)
(
    IN const GT_CHAR *string,
    INOUT GT_CHAR **  endptr,
    IN GT_32          base
);

/* CPSS_OS_STR_BIND_STC -
    structure that hold the "os strings" functions needed be bound to cpss.

*/
typedef struct{
    CPSS_OS_STR_LEN_FUNC               osStrlenFunc;
    CPSS_OS_STR_CPY_FUNC               osStrCpyFunc;
    CPSS_OS_STR_N_CPY_FUNC             osStrNCpyFunc;
    CPSS_OS_STR_CHR_FUNC               osStrChrFunc;
    CPSS_OS_STR_STR_FUNC               osStrStrFunc;
    CPSS_OS_STR_REV_CHR_FUNC           osStrRevChrFunc;
    CPSS_OS_STR_CMP_FUNC               osStrCmpFunc;
    CPSS_OS_STR_N_CMP_FUNC             osStrNCmpFunc;
    CPSS_OS_STR_CAT_FUNC               osStrCatFunc;
    CPSS_OS_STR_N_CAT_FUNC             osStrStrNCatFunc;
    CPSS_OS_TO_UPPER_FUNC              osStrChrToUpperFunc;
    CPSS_OS_STR_TO_32_FUNC             osStrTo32Func;
    CPSS_OS_STR_TO_U32_FUNC            osStrToU32Func;
}CPSS_OS_STR_BIND_STC;

#ifdef __cplusplus
}
#endif

#endif  /* __cpssOsStrh */


