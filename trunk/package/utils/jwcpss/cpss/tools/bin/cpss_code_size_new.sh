#!/bin/bash

######################################################################
# cpss_code_size.sh
#
# DESCRIPTION:
#       Bourne shell script countint the cpss release libs size  
#
# INPUTS:
#        
#       1) CPSS libs root directory full path
#       2) code size protocol target directory full path
#       3) CPU
#           
#
# OUTPUTS:
#       none
#
# RETURNS:
#       none
#
# COMMENTS:
#
# FILE REVISION NUMBER:
#       $Revision: 9 $
#####################################################################
#
# Internal Functions
#
#####################################################################
# help
#
# DESCRIPTION:
#           print the script's using help
# INPUTS:
#
# OUTPUTS:
#           none
#
# RETURNS:
#           
#
# COMMENTS:
#           none
#
#####################################################################
help()
{
    CPSS_LIB_ROOT_PATH=${1}
    WORKING_DIRECTORY=${2}
 
    echo "Bourne shell script to count the code size"
    echo "Four parameters should be used:"
    echo "1  -  cpss libraries path"
    echo "2  -  working directory "
}
#####################################################################
# SizeCount_Function
#
# DESCRIPTION:
#           count the code size for defined libraries
#
# INPUTS:
#           CPU_SUBDIR
#           LIB_PATH
#           PROTOCOL_FILE
#
# OUTPUTS:
#           none
#
# RETURNS:
#           
#
# COMMENTS:
#           none
#
#####################################################################
SizeCount_Function()
{
    if [ $# -lt 3 ]  # number of parameters < 1
    then
        echo "Number of parameters - " $#   
        echo "Not enough parameters for SizeCount_Function"
        exit -1
    elif [ $# -eq 3 ]  
    then
        CPU_SUBDIR=$1
        LIB_PATH=$2
        PROTOCOL_FILE=$3
    elif [ $# -eq 4 ]
    then
       CPU_SUBDIR=$1
       LIB_PATH=$2
       PROTOCOL_FILE=$3
       INSPECTED_FILE=$4
    elif [ $# -gt 4 ]  
    then
       echo "Number of parameters - " $#   
       echo "Too many parameters for SizeCount_Function"
       exit -1
    fi

    if [ "`uname`" == "FreeBSD" ]; then
        CALC_UTILITY=ppc-fbsd-e500-size
        if [ $CPU_SUBDIR = i386 ]; then
            CALC_UTILITY=size
        fi
    else
        case $CPU_SUBDIR in
            PPC603 | PPC85XX)
                CALC_UTILITY=ppc-linux-size
                ;;
            ARM926EJ)
                CALC_UTILITY=arm-linux-size
                # there is no arm-linux-size in gnuEabi,
                # use arm-none-linux-gnueabi-size instead
                which $CALC_UTILITY >/dev/null 2>&1 || {
                    CALC_UTILITY=arm-none-linux-gnueabi-size
                    case $CPU in
                        XCAT_BE | ARM78100 )
                            CALC_UTILITY=armeb-none-linux-gnueabi-size
                            ;;
                    esac
                }
                ;;
            i386)
                CALC_UTILITY=size
                ;;
            *)
                echo "Wrong CPU_SUBDIR - ${CPU_SUBDIR}, use ppc-linux-size"
                CALC_UTILITY=ppc-linux-size
                ;;
        esac
    fi


    echo " " >> ${PROTOCOL_FILE}
    echo " " >> ${PROTOCOL_FILE}
    echo "::::::::  ${LIB_PATH}" >> ${PROTOCOL_FILE} 
    echo " " >> ${PROTOCOL_FILE}

    (
#
#   run size utilite for all object files and libs
#


    if [ $# -eq 4 ]
    then
      ${CALC_UTILITY} ${LIB_PATH}/$INSPECTED_FILE 2>/dev/null
    else
      ${CALC_UTILITY} ${LIB_PATH}/*.o 2>/dev/null
      ${CALC_UTILITY} ${LIB_PATH}/*.lib 2>/dev/null
      ${CALC_UTILITY} ${LIB_PATH}/*.a 2>/dev/null
    fi 
    ) | tee -a aaa |
#
#   run awk script imported from cpss_code_size.spt to count the data summary
#
    awk '
##############################################################################
#   cpss_code_size.spt   -    AWK program file 
#
#       used to count the CPSS libs text, data and bss section size
#       this AWK file is called from cpss_code_size.sh     
#
#       Revision: 1 
#############################################################################

BEGIN {
    text = 0;
    data = 0;
    bss = 0;
    dec = 0;
} 
{
    if($1 != "text")
    {
        text = text + $1
        data = data + $2
        bss = bss + $3
        dec = dec + $4      
    }
}
END{
    printf  "        text         data          bss        total  \n"
     printf  "       --------------------------------------------  \n"
     printf  "%12d %12d %12d %12d \n",text,data,bss,dec 
    printf  "       --------------------------------------------  \n"
    printf "\n"
    printf "\n"                     
}
' >> ${PROTOCOL_FILE}

}
#####################################################################
#  main stream
####################################################################

    echo "code_size - on server"
    
    CPSS_LIB_ROOT_PATH=${1}
    WORKING_DIRECTORY=${2}
    CPU=${3}
    
    for CPU_SUBDIR in `(cd ${CPSS_LIB_ROOT_PATH}/common/libs;echo *)`
    do
        REPORT_SUFFIX=$CPU_SUBDIR
        if [ $CPU_SUBDIR = ARM926EJ ]; then
            case $CPU in
                XCAT_BE | ARM78100 )
                    REPORT_SUFFIX=ARM926EJ_BE
                    ;;
            esac
        fi
    
        #
        #  count code section size for COMMON libs
        #
        LIB_PATH=$CPSS_LIB_ROOT_PATH/common/libs/$CPU_SUBDIR
        SizeCount_Function $CPU_SUBDIR $LIB_PATH $WORKING_DIRECTORY/code_size_$REPORT_SUFFIX.txt

        #
        #  count code section size for mainPpDrv libs
        #
        LIB_PATH="${CPSS_LIB_ROOT_PATH}/mainPpDrv/libs/${CPU_SUBDIR}"

        ## remove libs of Pp because they are in mainPpDrv.lib 
        if [ -f "${CPSS_LIB_ROOT_PATH}/mainPpDrv/libs/${CPU_SUBDIR}/mainPpDrv.lib" ]
        then
            rm -f ${CPSS_LIB_ROOT_PATH}/mainPpDrv/libs/${CPU_SUBDIR}/cpss*.lib
            rm -f ${CPSS_LIB_ROOT_PATH}/mainPpDrv/libs/${CPU_SUBDIR}/mainPpDrv.o
        fi
        SizeCount_Function $CPU_SUBDIR $LIB_PATH $WORKING_DIRECTORY/code_size_$REPORT_SUFFIX.txt

        if [ $CPU = "i386" ]
        then
            echo " run for simulation"
            LIB_PATH="${CPSS_LIB_ROOT_PATH}/simulation/libs/${CPU_SUBDIR}"
            SizeCount_Function $CPU_SUBDIR $LIB_PATH $WORKING_DIRECTORY/code_size_$REPORT_SUFFIX.txt simulation.a

        fi 

    done
