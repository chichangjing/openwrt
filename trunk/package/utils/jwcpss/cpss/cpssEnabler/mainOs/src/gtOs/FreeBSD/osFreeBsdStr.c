/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* osFreeBsdStr.c
*
* DESCRIPTION:
*       FreeBsd User Mode Operating System wrapper. String facility.
*
* DEPENDENCIES:
*       FreeBsd, CPU independed.
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*******************************************************************************/

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include <gtOs/gtOsStr.h>

/***** Public Functions ************************************************/

/*******************************************************************************
* osStrlen
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
GT_U32 osStrlen
(
    IN const GT_VOID * source
)
{
    return strlen(source);
}

/*******************************************************************************
* osStrCpy
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
GT_CHAR *osStrCpy
(
    IN GT_CHAR         *dest,
    IN const GT_CHAR   *source
)
{
        return strcpy(dest, source);
}

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
GT_CHAR *osStrNCpy
(
    IN GT_CHAR         *dest,
    IN const GT_CHAR   *source,
    IN GT_U32          len
)
{
    return strncpy(dest, source, len);
}

/*******************************************************************************
* osStrChr
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
GT_CHAR *osStrChr
(
    IN const GT_CHAR   *source,
    IN GT_32           character
)
{
    return strchr(source, character);
}

/*******************************************************************************
* osStrStr
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
GT_CHAR *osStrStr
(
    IN const GT_CHAR *source,
    IN const GT_CHAR *needle
)
{
    return strstr(source, needle);
}

/*******************************************************************************
* osStrrChr
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
GT_CHAR *osStrrChr
(
    IN const GT_CHAR   *source,
    IN GT_32           character
)
{
    return strrchr(source, character);
}

/*******************************************************************************
* osStrCmp
*
* DESCRIPTION:
*       Compares lexicographically the null terminating strings str1 and str2.
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
GT_32 osStrCmp
(
    IN const GT_CHAR   *str1,
    IN const GT_CHAR   *str2
)
{
    return strcmp(str1, str2);
}

/*******************************************************************************
* osStrNCmp
*
* DESCRIPTION:
*       Compares lexicographically the null terminating strings str1 and str2.
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
GT_32 osStrNCmp
(
    IN const GT_CHAR   *str1,
    IN const GT_CHAR   *str2,
    IN GT_U32       len
)
{
    return strncmp(str1, str2, len);
}

/*******************************************************************************
* osStrCat
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
GT_CHAR *osStrCat
(
    IN GT_CHAR         *str1,
    IN const GT_CHAR   *str2
)
{
    return strcat(str1, str2);
}

/*******************************************************************************
* osStrNCat
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
GT_CHAR *osStrNCat
(
    IN GT_CHAR         *str1,
    IN const GT_CHAR   *str2,
    IN GT_U32          len
)
{
    return strncat(str1, str2, len);
}

/*******************************************************************************
* osToUpper
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
GT_32 osToUpper
(
    IN const GT_32 character
)
{
    return toupper(character);
}

/*******************************************************************************
* osStrTo32
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
GT_32 osStrTo32
(
    IN const GT_CHAR *string
)
{
    return atoi(string);
}

/*******************************************************************************
* osStrToU32
*
* DESCRIPTION:
*       Converts the initial portion of the string s to unsigned long integer
*       representation.
*
* INPUTS:
*       string   - a string to convert
*       endPtr   - ptr to final string
*       base     - radix
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       The converted value or ZERO, if no conversion could be performed.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_U32 osStrToU32
(
    IN const GT_CHAR *string,
    IN GT_CHAR **    endPtr,
    IN GT_32         base
)
{
    return strtoul(string, endPtr, base);
}


