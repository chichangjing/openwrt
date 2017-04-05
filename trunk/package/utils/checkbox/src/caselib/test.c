


#include <math.h>
#include <time.h>
#include <stdio.h>

#include "cs_type.h"
#include "cs_rand.h"

int main(int argc, char ** argv){
    //char a[] = {0x00, 0xff,0x11};
    cs_u8 b = rand_cs_8();
    cs_mac a = rand_mac();
    cs_ip4 c = rand_ip4();
    cs_u8 d = 0;
    cs_8 e = 0;
    for(;;a = rand_mac(), b = rand_cs_u8(), c = rand_ip4()){

     //   printf("%3u is cs_u8 : %d\n", rand_cs_u8(), 0xff/(int)(pow(10, 3)));

#if 0
        printf("d : %x \t e : %llx\n", d++, e++);
        usleep(300);
#endif

#if 1
        printf("a   : %02x %02x %02x %02x %02x %02x\t",
                __p_index(a, 0), __p_index(a, 1), __p_index(a, 2),\
                __p_index(a, 3), __p_index(a, 4), __p_index(a, 5));
        printf("mac : %18s\t", mac2str(a, _ms_range_rand(_base_mac_2, _base_mac_4)));
        printf("ip4 : %16s\n", ip42str(c));
#endif

#if 0
        printf("CS8  : %4s \t", cs82str(rand_cs_8()));
        printf("CS16 : %8s \t", cs162str(rand_cs_16()));
        printf("CS32 : %16s \t", cs322str(rand_cs_32()));
        printf("CS64 : %32s \n", cs642str(rand_cs_64()));
#endif
        //if(!is_csu8(csu82str(b)))
#if 0
        if(is_cs8("1"))
            printf("CSu8 : %u, %4s %s \n", b, csu82str(b), is_ip4("0.0.0..0") ?"true":"false");
#endif

        #if 0

        printf("CSu16: %8s \t", csu162str(rand_cs_u16()));
        printf("CSu32: %16s \t", csu322str(rand_cs_u32()));
        printf("CSu64: %32s \n", csu642str(rand_cs_u64()));
#endif

    }
}
