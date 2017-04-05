#ifndef __UTIILS_H
#define __UTIILS_H

#include <stdint.h>
#include <errno.h>

#include "log.h"

// 字节对齐设置
#define ALIGN_ARG   2
#define ALIGN1(len)  (((len) + (1 << ALIGN_ARG) - 1) & ~((1 << ALIGN_ARG) -1))  // 这里是按 2^ALIGN_ARG字节对齐
#define ALIGN2(len)  ( ((len) + ALIGN_ARG - 1) & ~(ALIGN_ARG - 1))              // ALIGN_ARAG字节对齐

#ifndef Bool
#define Bool int
#define False 0
#define True 1
#endif

#endif
