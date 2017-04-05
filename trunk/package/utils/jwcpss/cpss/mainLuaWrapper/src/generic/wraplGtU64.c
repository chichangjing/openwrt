/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplGtU64.c
*
* DESCRIPTION:
*       Provides wrappers for GT_U64 type operation.
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*******************************************************************************/

#include <cpss/generic/cpssTypes.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <generic/private/prvWraplGeneral.h>
#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>
#include <cpssCommon/private/prvCpssMath.h>

#include <lua.h>

#define BITS 64

/*******************************************************************************
*   hight GT_U32 base of GT_U64 in decimal array                               *
*******************************************************************************/
const GT_U8 gtU64HightGtU32BaseDecimalArray[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                                                   4, 2, 9, 4, 9, 6, 7, 2, 9, 6};
const GT_U8 gtU64HightGtU32BaseDecimalArrayFisrtsPositiveCellNumber = 9;

/* externs */
use_prv_struct(GT_U64);

/* forward declarations */
static int is_zero(char *a);
static int div10(char a[BITS]);

                                             
/*******************************************************************************
*   GT_U64 wrapper errors                                                      *
*******************************************************************************/      
GT_CHAR gtU64AbsenceErrorMessage[] = "GT_U64 number absent.";


/*******************************************************************************
* pvrCpssGtU64LongArrayGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get's GT_U32 array of GT_U64 structure from lua stack
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                   - lua state
*       L_index             - entry GT_U64 structure index in lua stack 
*       error_message       - possible error message, relevant 
*                                   if status != GT_OK
* 
* OUTPUTS:
*       gtU64Ptr            - targed gtU64 number 
*       error_message       - output error message (always NULL)
*
* RETURNS:
*       GT_OK 
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssGtU64LongArrayGet
(
    IN  lua_State               *L,
    IN  GT_32                   L_index,
    OUT GT_U64                  *gtU64Ptr,
    OUT GT_CHAR_PTR             *error_message    
)
{
    GT_STATUS                   status = GT_OK;
    
    error_message = NULL;
    
    if (0 != lua_istable(L, L_index))
    {
        lua_pushvalue(L, L_index);
        prv_lua_to_c_GT_U64(L, gtU64Ptr);
        lua_pop(L, 1);
    }
   
    return status;
}

/*******************************************************************************
* pvrCpssGtU64LongArrayPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Push GT_U64 structure to lua stack
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       status              - caller status
*       L                   - lua state
*       gtU64Ptr            - gtU64 number 
* 
* OUTPUTS:
*       error_message       - output error message (always NULL)
*
* RETURNS:
*       number of pushed values: 1 
*
* COMMENTS:
*
*******************************************************************************/
int pvrCpssGtU64LongArrayPush
(
    IN  GT_STATUS               status,
    IN  lua_State               *L,
    IN  GT_U64                  *gtU64Ptr,
    OUT GT_CHAR_PTR             *error_message    
)
{
    *error_message = NULL;
    
    if (GT_OK != status)
    {
        cpssOsMemSet(gtU64Ptr, 0, sizeof(*gtU64Ptr)); 
    }
    
    prv_c_to_lua_GT_U64(L, gtU64Ptr);
    
    return 1;
}

/*******************************************************************************
* pvrCpssGtU32ToGtU64DecimalArrayConvert
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Convert's GT_U32 number to GT_U64 decimal array
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       number              - entry number
* 
* OUTPUTS:
*       byteArrayPtr        - converted byte array
*
* RETURNS:
*       GT_OK 
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssGtU32ToGtU64DecimalArrayConvert
(
    IN  GT_U32                  number,
    IN  GT_U8_PTR               byteArrayPtr  
)
{
    GT_U32                      current_number  = number;
    GT_U8                       index           = 20;
    GT_STATUS                   status          = GT_OK;    
    
    while ((0 < index) && (0 != current_number))
    {
        index--;
        
        byteArrayPtr[index] = (GT_U8) (current_number % 10);
        current_number /= 10;
    }
    
    while (0 < index)
    {
        index--;
        byteArrayPtr[index] = 0;
    }    
    
    return status;
}


/*******************************************************************************
* pvrCpssGtU64DecimalArrayToStringConvert
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Convert's GT_U64 decimal array to char string
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       inputArrayPtr       - entry byte array
* 
* OUTPUTS:
*       outputArrayPtr      - converted byte array
*
* RETURNS:
*       GT_OK 
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssGtU64DecimalArrayToStringConvert
(
    IN  GT_U8_PTR               inputArrayPtr,
    OUT GT_U8_PTR               outputArrayPtr
)
{
    GT_U8                       index;
    GT_U8                       leftShift       = 0;
    GT_STATUS                   status          = GT_OK; 
   
    while ((leftShift < 20) && (0 == inputArrayPtr[leftShift]))
    {
        leftShift++;
    }
   
    if (20 <= leftShift)
    {
        leftShift = 20 - 1;
    }


    for (index = 0; index < 20 - leftShift; index++)
    {
        outputArrayPtr[index] = 
            (GT_U8) ((GT_U8) inputArrayPtr[index + leftShift] + (GT_U8) '0');
    }
    
    inputArrayPtr[20 - leftShift] = 0;

    return status;
}


/*******************************************************************************
* pvrCpssGtU64DecimalArrayClear
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Set's all bytes of GT_U64 decimal array to zero.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       byteArrayPtr        - input array
* 
* OUTPUTS:
*       byteArrayPtr        - output array
*
* RETURNS:
*       GT_OK 
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssGtU64DecimalArrayClear
(
    INOUT GT_U8_PTR             byteArrayPtr  
)
{
    GT_U8                       index;
    GT_STATUS                   status          = GT_OK; 

    for (index = 0; index < 20; index++)    
    {
        byteArrayPtr[index] = 0;
    }
    
    return status;
}


/*******************************************************************************
* pvrCpssGtU64DecimalHightPositiveCellNumberGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of number of first non-zero byte in GT_U64 decimal array.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       byteArrayPtr        - input array
* 
* OUTPUTS:
*       hightPositiveCellNumberPtr        
*                           - number of first non-zero byte in GT_U64 decimal 
*                             array; if array contains only 0 than size of 
*                             array 
*
* RETURNS:
*       GT_OK 
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssGtU64DecimalHightPositiveCellNumberGet
(
    IN  GT_U8_PTR               byteArrayPtr,
    OUT GT_U8_PTR               hightPositiveCellNumberPtr
)
{
    GT_U8                       index           = 0;
    GT_STATUS                   status          = GT_OK; 

    while ((index < 20) && (0 == byteArrayPtr[index]))
    {
        index++;
    }
    
    if (index < 20)
    {
        index--;
    }
    
    *hightPositiveCellNumberPtr = index;
    
    return status;
}


/*******************************************************************************
* pvrCpssGtU64DecimalArraysLeftShift
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Shifts left GT_U64 decimal array
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       shiftValue          - shift
*       givenArrayPtr       - entry array
* 
* OUTPUTS:
*       givenArrayPtr       - resulting array
*
* RETURNS:
*       GT_OK 
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssGtU64DecimalArraysLeftShift
(
    IN    GT_U8                 shiftValue,
    INOUT GT_U8_PTR             givenArrayPtr
)
{
    GT_U8                       gtU64SymbolIndex;
    GT_STATUS                   status = GT_OK;
    
    for (gtU64SymbolIndex = 0; 
         gtU64SymbolIndex < 20 - shiftValue; 
         gtU64SymbolIndex++)
    {
        givenArrayPtr[gtU64SymbolIndex] = 
            givenArrayPtr[gtU64SymbolIndex + shiftValue];
    }
    
    for (gtU64SymbolIndex = (GT_U8) ((GT_U8) 20 - (GT_U8) shiftValue); 
         gtU64SymbolIndex < 20; 
         gtU64SymbolIndex++)
    {
        givenArrayPtr[gtU64SymbolIndex] = 0;
    }

    return status;     
}


/*******************************************************************************
* pvrCpssGtU64DecimalArraysCopy
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Copy one GT_U64 decimal arrays to another
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       sourceArrayPtr       - source GT_U64 decimal array
* 
* OUTPUTS:
*       destinationArrayPtr  - destination GT_U64 decimal array
*
* RETURNS:
*       GT_OK 
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssGtU64DecimalArraysCopy
(
    IN  GT_U8_PTR               sourceArrayPtr,     
    OUT GT_U8_PTR               destinationArrayPtr
)
{
    GT_U8                       gtU64SymbolIndex    = 20;
    GT_STATUS                   status              = GT_OK; 

    while(0 < gtU64SymbolIndex)
    {
        gtU64SymbolIndex--;
        
        destinationArrayPtr[gtU64SymbolIndex] = sourceArrayPtr[gtU64SymbolIndex];
    }    

    return status;    
}


/*******************************************************************************
* pvrCpssGtU64DecimalArraysSummarize
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Summarizes of two GT_U64 decimal arrays
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       firstArrayPtr       - first GT_U64 decimal array
*       secondArrayPtr      - second GT_U64 decimal array
* 
* OUTPUTS:
*       sumArrayPtr         - GT_U64 decimal array with resulting sum
*
* RETURNS:
*       GT_OK 
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssGtU64ArraysSummarize
(
    IN  GT_U8_PTR               firstArrayPtr,
    IN  GT_U8_PTR               secondArrayPtr,      
    OUT GT_U8_PTR               sumArrayPtr
)
{
    GT_U8                       gtU64Carry          = 0;
    GT_U8                       gtU64SymbolIndex    = 20;
    GT_STATUS                   status              = GT_OK; 

    while(0 < gtU64SymbolIndex)
    {       
        gtU64SymbolIndex--;
        
        sumArrayPtr[gtU64SymbolIndex] = (GT_U8) (firstArrayPtr[gtU64SymbolIndex] + 
            secondArrayPtr[gtU64SymbolIndex] + gtU64Carry);
            
        if(0 < gtU64Carry)
        {
            gtU64Carry--;
        }

        if (9 < sumArrayPtr[gtU64SymbolIndex])
        {
            sumArrayPtr[gtU64SymbolIndex] -= 10;
            gtU64Carry++;
        }        
    }    

    return status;    
}

/*******************************************************************************
* pvrCpssGtU64ArrayByDecimalNumberMultiply
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Multiples GT_U64 decimal array by decimal number (0 .. 9)
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       inputArrayPtr       - input GT_U64 decimal array
*       decimalNumber       - decimal number
* 
* OUTPUTS:
*       outputArrayPtr      - output GT_U64 decimal array
*
* RETURNS:
*       GT_OK 
*
* COMMENTS:
*       decimalNumber should be GT_U16, otherwise bug
*
*******************************************************************************/
GT_STATUS pvrCpssGtU64ArrayByDecimalNumberMultiply
(
    IN  GT_U8_PTR               inputArrayPtr,
    IN  GT_U16                  decimalNumber,      
    OUT GT_U8_PTR               outputArrayPtr
)
{
    GT_U8                       multiplyingIndex;
    GT_STATUS                   status = GT_OK; 
    
    if (9 < decimalNumber)
    {
        return GT_BAD_PARAM;
    }
    
    status = pvrCpssGtU64DecimalArrayClear(outputArrayPtr);
    
    for (multiplyingIndex = 0; 
         (multiplyingIndex < decimalNumber) && (GT_OK == status); 
         multiplyingIndex++)
    {
        status = pvrCpssGtU64ArraysSummarize(inputArrayPtr, outputArrayPtr, 
                                             outputArrayPtr);
    }
    
    return status; 
}


/*******************************************************************************
* wrlCpssGtU64StrGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Checking of trunk existance.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                   - lua state with GT_U64 structure in stack
* 
* OUTPUTS:
*
* RETURNS:
*       GT_OK 
*
* COMMENTS:
*
*******************************************************************************/ 
int wrlCpssGtU64StrGet
(
    IN lua_State *L
)
{
    GT_U64                  gtU64;
    GT_U8                   gtU64LowLDecimalArray[20]; 
    GT_U8                   gtU64HightLDecimalArray[20];    
    GT_U8                   gtU64ResultDecimalArray[20 + 1];        
    GT_U8                   gtU64TempDecimalArray[20];  
    GT_U8                   gtU64HightGtU32BaseDecimalArrayIndex    = 20;
    GT_STATUS               status = GT_OK;
    GT_CHAR_PTR             error_message = NULL;
       
    cpssOsMemSet(&gtU64, 0, sizeof(gtU64));       
    
    status = pvrCpssGtU64LongArrayGet(L, 1, &gtU64, &error_message);
    
    if (0 == gtU64.l[1])
    {
        if (GT_OK == status)
        {     
            cpssOsSprintf((GT_CHAR_PTR) gtU64ResultDecimalArray, "%u", 
                    (unsigned int)gtU64.l[0]);
        }
    }
    else
    {
        if (GT_OK == status)
        {    
            status = pvrCpssGtU32ToGtU64DecimalArrayConvert
                         (gtU64.l[0], gtU64LowLDecimalArray);
        }
        
        if (GT_OK == status)
        {        
            status = pvrCpssGtU32ToGtU64DecimalArrayConvert
                         (gtU64.l[1], gtU64HightLDecimalArray);    
        }
         
        if (GT_OK == status)
        {     
            status = pvrCpssGtU64DecimalArrayClear(gtU64ResultDecimalArray);            
        }
            
        while((gtU64HightGtU32BaseDecimalArrayFisrtsPositiveCellNumber < 
                   gtU64HightGtU32BaseDecimalArrayIndex) && 
              (GT_OK == status))
        {              
            gtU64HightGtU32BaseDecimalArrayIndex--;
            
            status = pvrCpssGtU64ArrayByDecimalNumberMultiply
                        (gtU64HightLDecimalArray,
                         gtU64HightGtU32BaseDecimalArray[gtU64HightGtU32BaseDecimalArrayIndex],      
                         gtU64TempDecimalArray);
                    
            if (GT_OK == status)
            {
                status = pvrCpssGtU64ArraysSummarize(gtU64TempDecimalArray, 
                                                     gtU64ResultDecimalArray, 
                                                     gtU64ResultDecimalArray);   
            }

            if (GT_OK == status)
            {
                status = pvrCpssGtU64DecimalArraysLeftShift
                            (1, gtU64HightLDecimalArray);
            }
        }
        
        if (GT_OK == status)
        {
            status = pvrCpssGtU64ArraysSummarize(gtU64LowLDecimalArray,
                                                 gtU64ResultDecimalArray,  
                                                 gtU64ResultDecimalArray);
        }
        
        if (GT_OK == status)
        {    
            status = pvrCpssGtU64DecimalArrayToStringConvert
                        (gtU64ResultDecimalArray, gtU64ResultDecimalArray);
        }
    }
    
    if (GT_OK == status)
    {
        lua_pushstring(L, (GT_CHAR_PTR) gtU64ResultDecimalArray);  
    }
    else
    {
        lua_pushstring(L, "Wrong GT_U64 value."); 
    }
    
    return 1;
}


/*******************************************************************************
* wrlCpssGtU64MathAdd
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Adds two GT_U64 numbers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                   - lua state with 2 GT_U64 structures in stack
* 
* OUTPUTS:
*
* RETURNS:
*       GT_OK 
*
* COMMENTS:
*
*******************************************************************************/ 
int wrlCpssGtU64MathAdd
(
    IN lua_State *L
)
{
    GT_U64                  gtU64a, gtU64b, gtU64sum;
    GT_STATUS               status = GT_OK;
    GT_CHAR_PTR             error_message = NULL;    
    
    cpssOsMemSet(&gtU64sum, 0, sizeof(gtU64sum));     
    
    status = pvrCpssGtU64LongArrayGet(L, 1, &gtU64a, &error_message);

    if (GT_OK == status)
    {
        status = pvrCpssGtU64LongArrayGet(L, 2, &gtU64b, &error_message);
    }
    else
    {
        cpssOsMemSet(&gtU64b, 0, sizeof(gtU64b)); 
    }

    if (GT_OK == status)
    {
        gtU64sum = prvCpssMathAdd64(gtU64a, gtU64b);
    }
    
    return pvrCpssGtU64LongArrayPush(status, L, &gtU64sum, &error_message);    
}


/*******************************************************************************
* wrl_GTU64_todecimal
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Convert GT_U64 structure (Lua) to decimal (via binary devision).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                   - lua state with GT_U64 structure in stack
* 
* OUTPUTS:
*
* RETURNS:
*       GT_OK 
*
* COMMENTS:
*
*******************************************************************************/ 
int wrl_GTU64_todecimal(
    lua_State *L
)
{
    char bits[BITS];
    char tmp[22];
    int k, n;
    GT_U64 value;

    if (lua_gettop(L) < 1)
        return 0;
    lua_settop(L, 1);
    prv_lua_to_c_GT_U64(L, &value);

    /* convert GT_U64 to binary */
    for (k = 0; k < 2; k++)
    {
        GT_U32 mask = 0x80000000;
        for (n = 0; n < 32; n++)
        {
            bits[k*32+n] = (char) ((value.l[k] & mask) ? (char) 1 : (char) 0);
            mask >>= 1;
        }
    }
    
    /* now convert */
    n = sizeof(tmp);
    while (!is_zero(bits))
        tmp[--n] = (char) ((char) div10(bits) + (char)  '0');
    if (n == sizeof(tmp))
        tmp[--n] = '0';
    lua_pushlstring(L, tmp + n, sizeof(tmp)-n);
    return 1;
}


/*******************************************************************************
* ge
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Binary comparision.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       a                   - first number
*       b                   - second number
*       n                   - bits count
* 
* OUTPUTS:
*
* RETURNS:
*       1 is great or equal, otherwice 0 
*
* COMMENTS:
*
*******************************************************************************/ 
static int ge(char *a, char *b, int n)
{
    int k;
    for (k = 0; k < n; k++)
    {
        if (a[k] > b[k])
            return 1;
        if (a[k] < b[k])
            return 0;
    }
    return 1;
}


/*******************************************************************************
* sub
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Binary sub.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       a                   - first number
*       b                   - second number
*       n                   - bits count
* 
* OUTPUTS:
*
* RETURNS:
*       1 
*
* COMMENTS:
*
*******************************************************************************/ 
static void sub(char *a, char *b, int n)
{
    int k, j;
    for (k = 0; k < n; k++)
    {
        if (b[k] > a[k])
        {
            a[k] = 1;
            for (j = k-1;j>=0;j--)
            {
                if (a[j])
                {
                    a[j] = 0;
                    break;
                }
                a[j] = 1;
            }
        }
		else
		{
	        a[k] = (char) (a[k] - b[k]);
		}
    }
}


/*******************************************************************************
* div10
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Binary division by 10.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       a                   - number
* 
* OUTPUTS:
*
* RETURNS:
*       division module
*
* COMMENTS:
*
*******************************************************************************/ 
static int div10(char a[BITS])
{
    static char ten[] = { 0,1,0,1,0 };
    char r[BITS];
    int n = 0;
    int mod, k;
    for (k = 0; k + 3 < BITS; k++)
    {
        if (k && a[k-1])
        {
            r[n++] = 1;
            sub(a+k-1,ten,5);
            continue;
        }
        if (ge(a+k, ten+1, 4))
        {
            r[n++] = 1;
            sub(a+k, ten+1, 4);
        }
        else
        {
            r[n++] = 0;
        }
    }
    mod = 0;
    for (k = BITS-4; k < BITS; k++)
        mod = (mod << 1) + a[k];
    for (k = 0; k < BITS; k++)
        a[k] = 0;
    for (k = 0; k < n; k++)
        a[k+BITS-n] = r[k];
    return mod;
}


/*******************************************************************************
* is_zero
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Binary comparision.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       a                   - number
* 
* OUTPUTS:
*
* RETURNS:
*       1 is zero, otherwice 0 
*
* COMMENTS:
*
*******************************************************************************/ 
static int is_zero(char *a)
{
    int k;
    for (k = 0; k < BITS; k++)
        if (a[k])
            return 0;
    return 1;
}

