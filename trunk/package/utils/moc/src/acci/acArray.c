#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "acType.h"

AC_U32 acIdxStrArray(char * src, char ** array, AC_U32 len){
    AC_U32 i;
    for(i=0; i<len; i++){
        if(array[i] && 0==strcmp(src, array[i]))
            return i;
    }
    return -1;
}
