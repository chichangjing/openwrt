#include <pthread.h>
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "RmiAtu.h"
#include "RmiMibs.h"
#include "RmiRegs.h"
#include "Rmu.h"
#include "Rmi.h"

static void usage(void){
    printf("  Usage: rmi-tool [options]\n"
        "\tOptions:\n"
        "\t-s         Show the mibs describe \n"
        "\t-h         Show the rmi-tool usage \n"
        "\t-a         Display the mac address of the device \n"
        "\t-m <port>  Display the MIB Statistics of the select port \n"
        "\t-c <port>  Clear the MIB Statistics of the select port \n"
        "\t-R         Display the speed on real time(1s) \n"
        "\t-g         Display the all of the registers on the device \n"
        "\n");
}

int main(int argc, char ** argv){
    if(0>(rmi_init("eth0")))
        return -1;
    rmi_id_init(0x000000ff&getpid());
    loop_rmi();
    int32_t ch;
    while ((ch = getopt(argc, argv, "sham:c:Rg")) != -1) {
            switch (ch) {
            case 's':
                show_mib_help();
                break;
                return 0;
            case 'h':
                usage();
                break;
                return 0;
            case 'a':
                ATU_dump(DEV_MV6097);
                show_atu_entry();
                break;
                return 0;
            case 'm':
                MIB_dump(DEV_MV6097, (int16_t)(L_GE_Z20208D[(atoi(optarg))]));
                show_mibs(L_GE_Z20208D[(atoi(optarg))]);
                break;
                return 0;
            case 'c':
                MIB_clear(DEV_MV6097, (int16_t)(L_GE_Z20208D[(atoi(optarg))]));
                break;
                return 0;
            case 'R':
                printf("\033[2J");
                while(1){
                    show_rates();
                    usleep(1000000);
                }
                break;
                return 0;
            case 'g':
                regs_dump(DEV_MV6097);
                show_regs();
                break;
                return 0;
            default:
                usage();
                return 0;
            }
        }

    return 0;
}
