/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplCpssDxChIp.c
*
* DESCRIPTION:
*       A lua wrappers for debug information printing from lua wrappers
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*******************************************************************************/

#ifndef __wrapCpssDebugInfo__
#define __wrapCpssDebugInfo__

#include <cpss/generic/cpssTypes.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <generic/private/prvWraplGeneral.h>


/* Lua environment debug info printing variable     */
#define LUA_ENVIRONMENT_DEBUG_INFO_PRINTING_VARIABLE    "myCPSSGenWrapperDebug"

/* Maximum size of Lua debug printing name buffer   */
#define LUA_DEBUG_PRINT_NAME_BUFFER_MAX_SIZE            255

/* Lua debug printing name empty string separator   */
#define LUA_DEBUG_PRINT_NAME_EMPTY_SEPARATOR            ""

/* Lua debug printing name separator                */
#define LUA_DEBUG_PRINT_NAME_SEPARATOR                  "."


/* Support of debug info printing in lua- wrappers  */
#ifdef LUA_DEBUG_PRINT
    /************************************************************************
    * Macros for debug variables of lua envitronment getting
    ************************************************************************/
    #define P_CALLING_CHECKING                                              \
        GT_BOOL   __printDebugInformation   = GT_FALSE;                     \
        lua_State *__L                      = L;                            \
        lua_getglobal(L, LUA_ENVIRONMENT_DEBUG_INFO_PRINTING_VARIABLE);     \
        __printDebugInformation             =                               \
            (GT_BOOL) lua_toboolean(L, -1);                                 \
        lua_remove(L, -1);    

    /************************************************************************
    * Macros for debug properties as formal function parameters declaration
    ************************************************************************/
    #define P_CALLING_FORMAL_DATA    , IN GT_BOOL __printDebugInformation,  \
                                       lua_State *__L    

    /************************************************************************
    * Macros for debug properties as actual function parameters declaration
    ************************************************************************/  
    #define P_CALLING_ACTUAL_DATA    , __printDebugInformation, __L
        
    /************************************************************************
    * General macros for debug printing definition
    ************************************************************************/    
    #define P_CALLING_API(NAME, PARAMS, RESULT)                             \
        { if (GT_TRUE == __printDebugInformation) {                         \
            P_CALLING_API_NAME(NAME)                                        \
            P_CALLING_API_PARAMS(PARAMS)                                    \
            P_CALLING_API_RESULT(RESULT)                                    \
        } }
        
    /************************************************************************
    * Macros for definition of traced API funcnion name in general macros for
    * debug printing  
    ************************************************************************/     
    #define P_CALLING_API_NAME(API_NAME)                                    \
        cpssOsPrintf("Calling " #API_NAME "\n"); 
        
    /************************************************************************
    * Macros for definition of traced API funcnion params in general macros 
    * for debug printing  
    ************************************************************************/         
    #define P_CALLING_API_PARAMS(PARAMS)                                    \
        PARAMS 
        
    /************************************************************************
    * Macros for definition of traced API funcnion bool param in general  
    * macros for debug printing  
    ************************************************************************/      
    #define PC_BOOL(param, name, OTHER_PARAMS)                              \
        cpssOsPrintf("(GT_BOOL) " #name ":\n%s\n",                          \
                     GT_TRUE == param? "GT_TRUE":"GT_FALSE");               \
        OTHER_PARAMS

    /************************************************************************
    * Macros for definition of traced API funcnion number param in general  
    * macros for debug printing  
    ************************************************************************/    
    #define PC_NUMBER(param, name, fieldtype, OTHER_PARAMS)                \
        cpssOsPrintf("(" #fieldtype ") " #name ":\n%d\n", param);          \
        OTHER_PARAMS

    /************************************************************************
    * Macros for definition of traced API funcnion enum param in general  
    * macros for debug printing  
    ************************************************************************/  
    #define PC_ENUM(param, name, fieldtype, OTHER_PARAMS)                  \
        {                                                                  \
            const GT_CHAR* string_ ## fieldtype    =                       \
                prvCpssGetEnumString((GT_U32) (param),                     \
                                     enumDescr_ ## fieldtype);             \
            if (NULL != string_ ## fieldtype)                              \
            {                                                              \
                cpssOsPrintf("(" #fieldtype ") " #name ":\n%s\n",          \
                             string_ ## fieldtype);                        \
            }                                                              \
            else                                                           \
            {                                                              \
                cpssOsPrintf("(" #fieldtype ") " #name ":\n%u\n",          \
                             (unsigned int) (param));                      \
            }                                                              \
        }                                                                  \
        OTHER_PARAMS        
        
    /************************************************************************
    * Macros for definition of traced API funcnion array param in general  
    * macros for debug printing  
    ************************************************************************/
    #define PC_ARRAY(count, param, name, fieldtype, OTHER_PARAMS)           \
        cpssOsPrintf("(" #fieldtype ") " #name ":\n");                      \
        prv_lua_c_array_print_ ## fieldtype(#name, NULL, (GT_U32) (count),  \
                                            (fieldtype *) (param));         \
        OTHER_PARAMS    
        
    /************************************************************************
    * Macros for definition of traced API funcnion complex type param in   
    * general macros for debug printing  
    ************************************************************************/      
    #define PC_STRUCT(param, name, fieldtype, OTHER_PARAMS)                 \
        cpssOsPrintf("(" #fieldtype ") " #name ":\n");                      \
        prv_lua_c_print_ ## fieldtype(#name, NULL, (param));                \
        OTHER_PARAMS    
        
    /************************************************************************
    * Macros for definition of traced API function struct pointer param
    * (stub) in general macros for debug printing  
    ************************************************************************/    
    #define PC_STRUCTPTR(param, name, fieldtype, OTHER_PARAMS)              \
        cpssOsPrintf("(" #fieldtype "*) " #name ":\n%p\n", param);          \
        OTHER_PARAMS

    /************************************************************************
    * Macros for definition of traced API funcnion union param in general  
    * macros for debug printing  
    ************************************************************************/
    #define PC_UNION(key, param, name, fieldtype, OTHER_PARAMS)             \
        cpssOsPrintf("(" #fieldtype ") " #name ":\n");                      \
        prv_lua_c_union_print_ ## fieldtype(#name, NULL, key, (param));     \
        OTHER_PARAMS           
        
    /************************************************************************
    * Macros for termination of traced API funcnion params definition in 
    * general macros for debug printing  
    ************************************************************************/     
    #define PC_LAST_PARAM 
        
    /************************************************************************
    * Macros for definition of traced API funcnion execution result in 
    * general macros for debug printing  
    ************************************************************************/      
    #define P_CALLING_API_RESULT(status)                                    \
        cpssOsPrintf("The return code is: %d", status);                     \
        {                                                                   \
            GT_CHAR_PTR api_return_code_string  = NULL;                     \
            pvrCpssAPIResultCodeStringFromLuaGet(                           \
                    __L, status, &api_return_code_string);                  \
            if (NULL != api_return_code_string)                             \
            {                                                               \
                cpssOsPrintf(" (%s)", api_return_code_string);              \
            }                                                               \
            else                                                            \
            {                                                               \
                cpssOsPrintf(" (%d)", status);                              \
            }                                                               \
        }                                                                   \
        cpssOsPrintf("\n\n");
        
    /************************************************************************
    * Macros for status variable as API funcnion execution result 
    ************************************************************************/     
    #define PC_STATUS status    
#else    
    /************************************************************************
    * Stub of macros for debug variables of lua envitronment getting
    ************************************************************************/
    #define P_CALLING_CHECKING 

    /************************************************************************
    * Stub of macros for debug properties as formal function parameters 
    * declaration
    ************************************************************************/
    #define P_CALLING_FORMAL_DATA       

    /************************************************************************
    * Stub of macros for debug properties as actual function parameters 
    * declaration
    ************************************************************************/  
    #define P_CALLING_ACTUAL_DATA    
        
    /************************************************************************
    * Stub of general macros for debug printing definition
    ************************************************************************/    
    #define P_CALLING_API(NAME, PARAMS, RESULT)  
        
    /************************************************************************
    * Stub of macros for definition of traced API funcnion name in general 
    * macros for debug printing  
    ************************************************************************/     
    #define P_CALLING_API_NAME(API_NAME)     
        
    /************************************************************************
    * Stub of macros for definition of traced API funcnion params in general  
    * macros for debug printing  
    ************************************************************************/         
    #define P_CALLING_API_PARAMS(PARAMS)      
    
    /************************************************************************
    * Stub of macros for definition of traced API funcnion bool param in   
    * general macros for debug printing  
    ************************************************************************/      
    #define PC_BOOL(param, OTHER_PARAMS)    

    /************************************************************************
    * Stub of macros for definition of traced API funcnion number param in   
    * general macros for debug printing  
    ************************************************************************/    
    #define PC_NUMBER(param, fieldtype, OTHER_PARAMS) 

    /************************************************************************
    * Stub of macros for definition of traced API funcnion enum param in   
    * general macros for debug printing  
    ************************************************************************/  
    #define PC_ENUM(param, name, fieldtype, OTHER_PARAMS)                  

    /************************************************************************
    * Stub of macros for definition of traced API funcnion array param in   
    * general macros for debug printing  
    ************************************************************************/
    #define PC_ARRAY(param, name, count, fieldtype, OTHER_PARAMS)        
    
    /************************************************************************
    * Stub of macros for definition of traced API funcnion complex type param    
    * in general macros for debug printing  
    ************************************************************************/      
    #define PC_STRUCT(param, name, fieldtype, OTHER_PARAMS) 
        
    /************************************************************************
    * Stub of macros for definition of traced API funcnion complex type param    
    * in general macros for debug printing  
    ************************************************************************/      
    #define PC_STRUCTPTR(param, name, fieldtype, OTHER_PARAMS) 
        
    /************************************************************************
    * Stub of macros for definition of traced API funcnion union param in   
    * general macros for debug printing  
    ************************************************************************/
    #define PC_UNION(key, param, name, fieldtype, OTHER_PARAMS)              
        
    /************************************************************************
    * Stub of macros for termination of traced API funcnion params definition  
    * in general macros for debug printing  
    ************************************************************************/     
    #define PC_LAST_PARAM 
        
    /************************************************************************
    * Stub of macros for definition of traced API funcnion execution result 
    * in general macros for debug printing  
    ************************************************************************/      
    #define P_CALLING_API_RESULT(status)   
    /************************************************************************
    * Stub of macros for status variable as API funcnion execution result 
    ************************************************************************/     
    #define PC_STATUS status    
#endif

#endif

