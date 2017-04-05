# Check CPSS headers to be C++ compatible

. $tool_build/product/cpss_common.inc

os_pre_build()
{
   cd $CPSS_PATH
   C=$COMPILATION_ROOT/hdrcheck.cc


   rm -f $C
   i=0
   (
     find common/h -type f -a -name '*.h' |sed 's/^common\/h\/\(.*\)$/#include <\1>/'

     find mainPpDrv/h -type f -a -name '*.h' |sed 's/^mainPpDrv\/h\/\(.*\)$/#include <\1>/'
   
     find mainTmDrv/h -type f -a -name '*.h' |sed 's/^mainTmDrv\/h\/\(.*\)$/#include <\1>/'
   ) | while read incl_line
   do
     i=$(($i+1))
	 echo "$incl_line" >$COMPILATION_ROOT/hdr_$i.cc
	 echo "int var_$i=0;" >> $COMPILATION_ROOT/hdr_$i.cc
	 echo "$incl_line" >>$C
   done



   ### find mainExtUtils/h -type f -a -name '*.h' |sed 's/^mainExtUtils\/h\/\(.*\)$/#include <\1>/' >>$C
   ### find cpssEnabler/h -type f -a -name '*.h' |sed 's/^cpssEnabler\/h\/\(.*\)$/#include <\1>/' >>$C
   
   #find miniXcat3/h -type f -a -name '*.h' |sed 's/^miniXcat3\/h\/\(.*\)$/#include <\1>/' >>$C
   #find simulation/h -type f -a -name '*.h' |sed 's/^simulation\/h\/\(.*\)$/#include <\1>/' >>$C
   #find mainUT/h -type f -a -name '*.h' |sed 's/^mainUT\/h\/\(.*\)$/#include <\1>/' >>$C
   #find mainGaltisWrapper/h -type f -a -name '*.h' |sed 's/^mainGaltisWrapper\/h\/\(.*\)$/#include <\1>/' >>$C
   #find luaCLI_standalone/h -type f -a -name '*.h' |sed 's/^mainGaltisWrapper\/h\/\(.*\)$/#include <\1>/' >>$C
   #find mainLuaWrapper/h -type f -a -name '*.h' |sed 's/^mainLuaWrapper\/h\/\(.*\)$/#include <\1>/' >>$C
   
   
   echo >>$C
   echo "int main(void)" >>$C
   echo "{" >> $C
   echo " return 0;" >> $C
   echo "}" >>$C
}

USER_MAKE=Makefile
MAKE_TARGET=checkCpp

os_link()
{
    return 0
}

product_post_build()
{
    trace_message "=> END build CPP check"
    exit 0
}
