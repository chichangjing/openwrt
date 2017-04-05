#ifndef __hwsEnv_h__
#define __hwsEnv_h__

#if defined(CHX_FAMILY) || defined(EXMXPM_FAMILY)
#  include "cpssEnv.h"
#  define HWS_ENV_DEFINED
#endif

#if defined(MV_HWS_REDUCED_BUILD) && !defined(HWS_ENV_DEFINED)
#  include <gtOs/gtGenTypes.h>
#  include <gtOs/gtOsSem.h>
#  define HWS_MUTEX     GT_MUTEX
#  define HWS_ENV_DEFINED
#endif

#if defined(WIN32) && !defined(HWS_ENV_DEFINED)
#  include "win32Env.h"
#  define HWS_ENV_DEFINED
#endif

/* fallback to posix/linux */
#ifndef HWS_ENV_DEFINED
#  include "posixEnv.h"
#  define HWS_ENV_DEFINED
#endif

#ifndef MV_HWS_REDUCED_BUILD
#define _1K            (0x400)
#define _2K            (0x800)
#define _3K            (0xC00)
#define _4K            (0x1000)
#define _5K            (0x1400)
#define _6K            (0x1800)
#define _8K            (0x2000)
#define _9K            (0x2400)
#define _10K           (0x2800)
#define _12K           (0x3000)
#define _16K           (0x4000)
#define _24K           (0x6000)
#define _28K           (0x7000)
#define _32K           (0x8000)
#define _36K           (0x9000)
#define _48K           (0xc000)
#define _64K           (0x10000)
#define _128K          (0x20000)
#define _256K          (0x40000)
#define _512K          (0x80000)
#define _1M            (0x100000)
#define _2M            (0x200000)
#define _3M            (0x300000)
#define _4M            (0x400000)
#define _6M            (0x600000)
#define _8M            (0x800000)
#define _12M           (0xC00000)
#define _16M           (0x1000000)
#define _24M           (0x1800000)
#define _32M           (0x2000000)
#define _64M           (0x4000000)
#define _128M          (0x8000000)
#define _256M          (0x10000000)
#define _512M          (0x20000000)
#define _1G            (0x40000000)
#endif /* MV_HWS_REDUCED_BUILD */


#endif /* __hwsEnv_h__ */
