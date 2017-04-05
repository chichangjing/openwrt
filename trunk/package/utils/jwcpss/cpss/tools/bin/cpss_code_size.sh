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
#       3) CPSS code root full path
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
#       $Revision: 7 $
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
    CPSS_CODE_ROOT_PATH=${3}
 
    echo "Bourne shell script to count the code size"
    echo "Four parameters should be used:"
    echo "1  -  cpss libraries path"
    echo "2  -  working directory "
    echo "3  -  cpss root path"
    echo "4   - pp_family (EX/DX)"	
}
#####################################################################
# SizeCount_Function
#
# DESCRIPTION:
#           count the code size for defined libraries
#
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
SizeCount_Function()
{
    echo -e "SizeCount_Function "
    if [ $# -lt 1 ]  # number of parameters < 1
    then
        echo "Number of parameters - " $#   
        echo "Not enough parameters for SizeCount_Function"
        exit -1
    fi
  
    echo " " >> ${PROTOCOL_FILE}
    echo " " >> ${PROTOCOL_FILE}
    echo "::::::::  ${LIB_PATH}" >> ${PROTOCOL_FILE} 
    echo " " >> ${PROTOCOL_FILE}

#
#	run size utilite for all object files and libs
#
    ${CALC_UTILITY} ${LIB_PATH}/*.o > ${TEMP_FILE}
    ${CALC_UTILITY} ${LIB_PATH}/*.lib >> ${TEMP_FILE}

#
#   run awk script cpss_code_size.spt to count the data summary
#
    awk -f ${CPSS_CODE_ROOT_PATH}/tools/bin/cpss_code_size.spt ${TEMP_FILE} >> ${PROTOCOL_FILE}

}
#####################################################################
#  main stream
####################################################################
    
    CPSS_LIB_ROOT_PATH=${1}
    WORKING_DIRECTORY=${2}
    CPSS_CODE_ROOT_PATH=${3}
    PP_FAMILY=${4}
    
    PROTOCOL_FILE=${WORKING_DIRECTORY}/code_size.txt
    TEMP_FILE=${WORKING_DIRECTORY}/step.txt

	HOST_OS=`uname`
	if [ "$HOST_OS" == "FreeBSD" ]; then
		CALC_UTILITY=ppc-fbsd-e500-size
	else
		CALC_UTILITY=ppc-linux-size
	fi

    echo " " > ${PROTOCOL_FILE}
    echo " " > ${TEMP_FILE}


    if [ ${PP_FAMILY} = "DX" ]
    then     

	for CPU in "PPC603"  "ARM926EJ" "PPC85XX"
	do
    
		#
		#  count code section size for COMMON libs
		#
		LIB_PATH="${CPSS_LIB_ROOT_PATH}/common/libs/${CPU}"
		SizeCount_Function ${LIB_PATH} 

		#
		#  count code section size for mainPpDrv libs
		#
		LIB_PATH="${CPSS_LIB_ROOT_PATH}/mainPpDrv/libs/${CPU}"

		## remove libs of Pp because they are in mainPpDrv.lib 
		if [ -f "${CPSS_LIB_ROOT_PATH}/mainPpDrv/libs/${CPU}/mainPpDrv.lib" ]
		then
		    rm ${CPSS_LIB_ROOT_PATH}/mainPpDrv/libs/${CPU}/cpss*.lib
		    rm ${CPSS_LIB_ROOT_PATH}/mainPpDrv/libs/${CPU}/mainPpDrv.o
		fi
		SizeCount_Function ${LIB_PATH} 

	done
    fi


   if [ ${PP_FAMILY} = "EX" ]
   then     
	for CPU in "PPC603" 
	do
    
		#
		#  count code section size for COMMON libs
		#
		LIB_PATH="${CPSS_LIB_ROOT_PATH}/common/libs/${CPU}"
		SizeCount_Function ${LIB_PATH} 

		#
		#  count code section size for mainPpDrv libs
		#
		LIB_PATH="${CPSS_LIB_ROOT_PATH}/mainPpDrv/libs/${CPU}"

		## remove libs of Pp because they are in mainPpDrv.lib 
		if [ -f "${CPSS_LIB_ROOT_PATH}/mainPpDrv/libs/${CPU}/mainPpDrv.lib" ]
		then
		    rm ${CPSS_LIB_ROOT_PATH}/mainPpDrv/libs/${CPU}/cpss*.lib
		    rm ${CPSS_LIB_ROOT_PATH}/mainPpDrv/libs/${CPU}/mainPpDrv.o
		fi
		SizeCount_Function ${LIB_PATH} 


		#  count code section size for mainFaDrv libs
		LIB_PATH="${CPSS_LIB_ROOT_PATH}/mainFaDrv/libs/${CPU}"
		SizeCount_Function ${LIB_PATH} 

		#  count code section size for mainXbar libs
		LIB_PATH="${CPSS_LIB_ROOT_PATH}/mainXbarDrv/libs/${CPU}"
		SizeCount_Function ${LIB_PATH} 

	done
    fi

    if [ ${PP_FAMILY} = "EXMXPM" ]
    then     

	for CPU in "PPC85XX"  "ARM926EJ"
	do
    
		#
		#  count code section size for COMMON libs
		#
		LIB_PATH="${CPSS_LIB_ROOT_PATH}/common/libs/${CPU}"
		SizeCount_Function ${LIB_PATH} 

		#
		#  count code section size for mainPpDrv libs
		#
		LIB_PATH="${CPSS_LIB_ROOT_PATH}/mainPpDrv/libs/${CPU}"

		## remove libs of Pp because they are in mainPpDrv.lib 
		if [ -f "${CPSS_LIB_ROOT_PATH}/mainPpDrv/libs/${CPU}/mainPpDrv.lib" ]
		then
		    rm ${CPSS_LIB_ROOT_PATH}/mainPpDrv/libs/${CPU}/cpss*.lib
		    rm ${CPSS_LIB_ROOT_PATH}/mainPpDrv/libs/${CPU}/mainPpDrv.o
		fi
		SizeCount_Function ${LIB_PATH} 

                #
		#  count code section size for mainFxDrv libs
		#
		LIB_PATH="${CPSS_LIB_ROOT_PATH}/mainFxDrv/libs/${CPU}"
		SizeCount_Function ${LIB_PATH} 

	done
    fi



