/*
 * (c), Copyright 2009-2014, Marvell International Ltd.  (Marvell)
 *
 * This code contains confidential information of Marvell.
 * No rights are granted herein under any patent, mask work right or copyright
 * of Marvell or any third party. Marvell reserves the right at its sole
 * discretion to request that this code be immediately returned to Marvell.
 * This code is provided "as is". Marvell makes no warranties, expressed,
 * implied or otherwise, regarding its accuracy, completeness or performance.
 */
/**
 * @brief declaration & macro expanding of TM platform dependent parts of code
 *
 * @file tm_platform_implementation_definitions.h
 *
 * $Revision: 2.0 $
 */
#ifndef TM_PLATFORM_IMPLEMENTATION_DEFINITIONS_H 
#define TM_PLATFORM_IMPLEMENTATION_DEFINITIONS_H



#include  "tm_defs.h"


/**
 * @brief   Data structure for cpss adaptation
 */


struct cpss_tm_environment
{
	GT_U32	magicNumber;
	GT_U8	devNumber;
	GT_U8	burstMode;

	struct tm_tree_structure		tree_structure;
};


#define DECLARE_CPSS_ENV(env)		struct cpss_tm_environment * env;
#define ASSIGN_CPSS_ENV(env,hndl)	env = (struct cpss_tm_environment *)hndl;

#define HENV_MAGIC 0x15072015

#define CHECK_HENV_PTR(ptr, NULL_PTR_ERROR, WRONG_MAGIC_ERROR)	\
	if (!ptr) return NULL_PTR_ERROR;\
	if (ptr->magicNumber != HENV_MAGIC) return WRONG_MAGIC_ERROR;

#define CPSS_ENV(env,hndl, NULL_PTR_ERROR, WRONG_MAGIC_ERROR)	\
	struct cpss_tm_environment * env = (struct cpss_tm_environment *)hndl;\
	CHECK_HENV_PTR(env, NULL_PTR_ERROR, WRONG_MAGIC_ERROR)

/* function below creates new cpss_tm_environment handle */
void*	new_cpss_tm_environment_handle(GT_U8	devNumber);



#define	TM_REGVAR(_type, _var)  struct _type _var
#define	TM_REGVAR_ADDR(_var)    &(_var)

/*
	if the structure is defined it can be zero memored 
	the Linux platform  generates warning for this action
*/

/* Linux */
#define STRUCTURE_INITIALIZATION_CODE

#endif  /* TM_PLATFORM_IMPLEMENTATION_DEFINITIONS_H */

