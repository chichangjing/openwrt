/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* cmdParser.h
*
* DESCRIPTION:
*       parser engine api definitions
*
*
* DEPENDENCIES:
*       gtOs.h
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*******************************************************************************/

#ifndef __cmdParser_h__
#define __cmdParser_h__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/***** Include files ***************************************************/

#include <cmdShell/common/cmdCommon.h>



/*
 * enum CMD_DEFAULT_TOKENS
 *
 * Description:
 *      This enum defines default token types
 *
 * Fields:
 *  tokEOF  = -1,   - generic: END OF FILE
 *  tokUNKNOWN,     - item: unknown token
 *  tokDECIMAL,     - item: decimal numeric value
 *  tokHEXADECIMAL, - item: hexadecimal numeric value
 *  tokIDENTIFIER,  - item: identifier  constant
 *  tokSTRING,      - item: string constant
 *  tokDOT,         - operator: dot character
 *  tokCOMMA,       - operator: comma character
 *  tokSEMICOLON,   - operator: semicolon character
 *  tokLPAREN,      - operator: left parentheses
 *  tokRPAREN,      - operator; right parentheses
 *  tokLBRACK,      - operator: left brack
 *  tokRBRACK,      - operator: right brack
 *
 */
typedef enum
{
    tokEOF  = -1,   /* generic: END OF FILE             */
    tokUNKNOWN,     /* item: unknown token              */
    tokDECIMAL,     /* item: decimal numeric value      */
    tokHEXADECIMAL, /* item: hexadecimal numeric value  */
    tokIDENTIFIER,  /* item: identifier constant        */
    tokSTRING,      /* item: string constant            */
    tokDOT,         /* operator: dot character          */
    tokCOMMA,       /* operator: comma character        */
    tokSEMICOLON,   /* operator: semicolon character    */
    tokLPAREN,      /* operator: left parentheses       */
    tokRPAREN,      /* operator; right parentheses      */
    tokLBRACK,      /* operator: left brack             */
    tokRBRACK       /* operator: right brack            */
}CMD_DEFAULT_TOKENS;

/*******************************************************************************
* cmdParseInit
*
* DESCRIPTION:
*       initialize token reader module
*
* INPUTS:
*       buffer = input character buffer
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdParseInit
(
    IN  GT_CHAR*   buffer
);

/*******************************************************************************
* cmdParseDone
*
* DESCRIPTION:
*       shut down token reader module
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdParseDone(GT_VOID);

/*******************************************************************************
* cmdParseAddToken
*
* DESCRIPTION:
*       add new global token identifier
*
* INPUTS:
*       identifier  - token identifier string constant
*       token       - token identifier code
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdParseAddToken
(
    IN  GT_CHAR* identifier,
    IN  GT_U32   token
);

/*******************************************************************************
* cmdParseDelToken
*
* DESCRIPTION:
*       remove global token identifier
*
* INPUTS:
*       token       - token identifier code
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdParseDelToken
(
    IN  GT_U32  token
);

/*******************************************************************************
* cmdParseSkip
*
* DESCRIPTION:
*       skip single expected token
*
* INPUTS:
*       token       - token identifier code
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdParseSkip
(
    IN  GT_U32  token
);

/*******************************************************************************
* cmdParseRead
*
* DESCRIPTION:
*       read and parse next token
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdParseRead(GT_VOID);


/*******************************************************************************
* cmdParseGet
*
* DESCRIPTION:
*       get current token code
*
* INPUTS:
*       None
*
* OUTPUTS:
*       token   - token identifier code
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdParseGet
(
    OUT GT_U32* token
);

/*******************************************************************************
* cmdParseGetNumeric
*
* DESCRIPTION:
*       get current numeric value
*
* INPUTS:
*       None
*
* OUTPUTS:
*       numeric - current numeric value
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdParseGetNumeric
(
    OUT GT_UINTPTR* numeric
);

/*******************************************************************************
* cmdParseGetConstant
*
* DESCRIPTION:
*       get current constant identifier string
*
* INPUTS:
*       None
*
* OUTPUTS:
*       constant    - pointer to null terminated string
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdParseGetConstant
(
    OUT GT_CHAR** constant
);

/*******************************************************************************
* cmdParseGetString
*
* DESCRIPTION:
*       get current string identifier
*
* INPUTS:
*       None
*
* OUTPUTS:
*       constant    - pointer to null terminated string
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdParseGetString
(
    OUT GT_CHAR** constant
);


/*******************************************************************************
* cmdParseLine
*
* DESCRIPTION:
*       Parses single command stored in null terminated string.
*
* INPUTS:
*       inBuffer      - null terminated string holding command buffer
*       inFieldBuffer - null terminated string holding field values
*
* OUTPUTS:
*       isShellCmd - GT_TRUE, if it is shell command.
*
* RETURNS:
*       CMD_OK   - if there are no problems,
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS cmdParseLine
(
    IN  GT_CHAR      *inBuffer,
    IN  GT_CHAR      *inFieldBuffer,
    OUT GT_BOOL      *isShellCmd
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* __cmdParser_h__ */
