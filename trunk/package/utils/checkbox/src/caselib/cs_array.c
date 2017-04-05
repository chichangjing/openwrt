#include "cs_type.h"

cs_32 cs_idx_str_array(char src, char ** array, cs_32 len){
    cs_32 i;
    for(i=0; i<len; i++){
        if(array[i] && 0==strcmp(src, array[i]))
            return i;
    }
    return -1;
}
