## here can't be /bin/sh, because our scripts use bash
## usual place for bash is /bin/bash, but in our system it's not there, so better calculate it each time
bashPath=`which bash`
#!$bashPath

######################################################################
# link.sh
#
# DESCRIPTION:
#       Execute linkage of cpss libs to executable, while check if extention 
#	libraries exist and if they do include them in linkage
#
# INPUTS:
#         Mandatory parameters :
#		$(CC) 			- compiler name
#		${COMPILATION_ROOT_FIX} - objects dir
#		$(CPU_DIR) 		- cpu
#		$(CFLAGS) 		- compilation flags 
#		$(MACH_EXTRA) 		- libs list for link
#		$(LIB_EXTRA) 		- libs list for link
#		$(LDFLAGS) 		- linker flags
#
# OUTPUTS:
#       none
#
# RETURNS:
#       none
#
# COMMENTS:
#       none
# Note:
#       none
#
# FILE REVISION NUMBER:
#       $Revision: 4 $
#####################################################################

echo "start link.sh"

    if [ $# -lt 6 ]  # number of parameters < 6
    then
        echo "Number of parameters - " $#
        echo "NOT ENOUGH PARAMETERS."
    	return 1
    fi

CC=${1}
# Shift all the parameters down by one
shift   
COMPILATION_ROOT_FIX=${1}
shift
CPU_DIR=${1}
shift
LINK_PARAMS="$*"
LIBEXT=a

if [ -d $COMPILATION_ROOT_FIX/extension ]
then
    cd $COMPILATION_ROOT_FIX/extension
    for file in *
    do
	if [ -d "$file" ]; 
	then
    	    echo "$file"
	    cd "$file";
	    pwd

	    if [ -f mainPpDrv/libs/${CPU_DIR}/mainPpDrv.$LIBEXT ]
	    then
		## use this for GNU and DIAB linkers
		EXTERN_SYMBOL="-u _"
		EXT_LIBS=""

		if [ -f mainUT/libs/${CPU_DIR}/mainUT.$LIBEXT ]
		then
		    EXT_LIBS="${EXT_LIBS} $COMPILATION_ROOT_FIX/extension/${file}/mainUT/libs/${CPU_DIR}/mainUT.$LIBEXT"
		    EXT_LIBS="${EXT_LIBS} $COMPILATION_ROOT_FIX/mainUT/libs/${CPU_DIR}/mainUT.$LIBEXT"
		fi
		if [ -f mainPpDrv/libs/${CPU_DIR}/mainPpDrv.$LIBEXT ]
		then
		    EXT_LIBS="${EXT_LIBS} $COMPILATION_ROOT_FIX/extension/${file}/mainPpDrv/libs/${CPU_DIR}/mainPpDrv.$LIBEXT"
		fi

		if [ "$OS_RUN" = "FreeBSD" ] && [ "${LINUX_SIM:-0}" -ne 1 ]
		then
		    if [ -n "${EXT_LIBS}" ]
		    then
			echo "ppc-fbsd-e500-ranlib ${EXT_LIBS}"
			ppc-fbsd-e500-ranlib ${EXT_LIBS}
		    fi
		fi
		
		## !!! pay attantion to keep following libs in this order !!!
		if [ -f cpssEnabler/libs/${CPU_DIR}/cpssEnabler.o ]
		then
		    LINK_PARAMS="${LINK_PARAMS} ${COMPILATION_ROOT_FIX}/extension/${file}/cpssEnabler/libs/${CPU_DIR}/cpssEnabler.o"
		fi  
		if [ -f mainGaltisWrapper/libs/${CPU_DIR}/mainGaltisWrapper.o ]
		then
		    LINK_PARAMS="${LINK_PARAMS} ${COMPILATION_ROOT_FIX}/extension/${file}/mainGaltisWrapper/libs/${CPU_DIR}/mainGaltisWrapper.o"
		fi  

		LINK_PARAMS="${LINK_PARAMS} ${EXT_LIBS}"
		
		#set value of EXTENSION_SYMBOL in the environment variables
		if [ -z ${EXTENSION_SYMBOL} ]
		then
		    EXTENSION_SYMBOL=extensionLib
		fi
		
		if [ -z ${EXTENSION_SYMBOL1} ]
		then
		    EXTENSION_SYMBOL1=extensionLib1
		fi
    
		if [ -z ${EXTENSION_SYMBOL2} ]
		then
		    EXTENSION_SYMBOL2=extensionLib2
		fi
    
		if [ -z ${EXTENSION_SYMBOL3} ]
		then
		    EXTENSION_SYMBOL3=extensionLib3
		fi
    
		if [ -z ${EXTENSION_SYMBOL4} ]
		then
		    EXTENSION_SYMBOL4=extensionLib4
		fi
		
		# linker will add this symbol to force it into image.
		LINK_PARAMS="${EXTERN_SYMBOL}${EXTENSION_SYMBOL4} ${LINK_PARAMS}"
		LINK_PARAMS="${EXTERN_SYMBOL}${EXTENSION_SYMBOL3} ${LINK_PARAMS}"
		LINK_PARAMS="${EXTERN_SYMBOL}${EXTENSION_SYMBOL2} ${LINK_PARAMS}"
		LINK_PARAMS="${EXTERN_SYMBOL}${EXTENSION_SYMBOL1} ${LINK_PARAMS}"
		LINK_PARAMS="${EXTERN_SYMBOL}${EXTENSION_SYMBOL} ${LINK_PARAMS}"

	    fi ##if [ -f mainPpDrv/libs/${CPU_DIR}/mainPpDrv.$LIBEXT ]

	    cd ..;

	fi ## if [ -d "$file" ]; 
    done ## for file in *
fi ## if [ -d $COMPILATION_ROOT_FIX/extension ]

if [ "$GALTIS_NO_APP_DEMO" = "EXISTS" ]; then
	if [ -f tmp_main.c ]; then
		rm -f tmp_main.c
	fi
	echo "#include <stdio.h>" > tmp_main.c
	echo "int main(void) { return 0; }" > tmp_main.c
	${CC} -c tmp_main.c
	LINK_PARAMS="${LINK_PARAMS} tmp_main.o"
fi

if [ "${LINUX_SIM:-0}" -eq 1 ]; then
   echo "${CC} -o ${COMPILATION_ROOT_FIX}/appDemoSim ${LINK_PARAMS}"
   ${CC} -o ${COMPILATION_ROOT_FIX}/appDemoSim ${LINK_PARAMS}
else
   echo "${CC} -o ${COMPILATION_ROOT_FIX}/appDemo ${LINK_PARAMS}"
   ${CC} -o ${COMPILATION_ROOT_FIX}/appDemo ${LINK_PARAMS}
fi
ret=$?

if [ -f tmp_main.c ]; then
rm -f tmp_main.c
fi

exit $ret
