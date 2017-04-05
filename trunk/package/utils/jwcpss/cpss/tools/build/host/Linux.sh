. $tool_build/host/posix.sh

host_shell ()
{
   echo "run bush"
   if [ -z $RUN_ENVBASE_SHELL ]
   then
      export RUN_ENVBASE_SHELL="YES"
      echo "Start envbase bash"
      bash
   else
      echo "Envbase bash is running"   
   fi

}
