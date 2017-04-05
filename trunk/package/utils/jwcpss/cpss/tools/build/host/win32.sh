# host OS defs for WIN32

######################
export HOST_WIN32=yes
######################

export RM=rm
export MAKE=gmake
export TOUCH=echo
export COPY=copy
export MKDIR=mkdir
export MD=makedepend

####################################################################
# detect shell type and set BUILD_SHELL variable
if [ \! -z "$ZSH_NAME" ]; then
    BUILD_SHELL=zsh
    # postpone path setting to not confuse other shells
    # don't pring warning if glob is not extracted, keep glob unchanged in this case
    set -3
elif [ \! -z "$BASH" ]; then
    BUILD_SHELL="$BASH"
else
    echo "This shell is not supported"
    echo "use tools/bin/zsh.exe or Cygwin's bash"
    exit 1
fi

# To prevent MS-DOS style path warning when cygwin installed
export CYGWIN=nodosfilewarning

# this function _MUST_ be used to add win32 path to PATH variable
win32_add_paths()
{
    for p in "$@"
    do
        if [ \! -z "$ZSH_NAME" ];
        then
            # zsh handles PATH in different way
            path=("$p" $path)
        else
            # for cygwin bash, ash
            case "$p" in
                *:*)
                    PATH="/cygdrive/${p%:*}/${p#*:}:$PATH"
                    ;;
                *)
                    PATH="$p:$PATH"
                    ;;
            esac
        fi
    done
}

win32_call_bat_file()
{
    key="$1"
    bat="$2"
    shift
    shift
    # After this point script became untraceable.
    # If you want to trace script then just add -x after $BUILD_SHELL
    eval is_batfile_was_called="\${BUILD_SCRIPT_BATFILE_${key}_CALLED}"
    if [ "$is_batfile_was_called" \!= "TRUE" ]; then
        if [ \! -z "$ZSH_NAME" ];
        then
            path=("${tool_build%/*}/bin" $path)
        fi
        temp_script="$TEMP\\build_temp$$.bat"
        temp_script_p=`win32_path_to_posix "$temp_script"`
        echo -E "call \"$bat\" $@" > $temp_script_p
        echo -E "set BUILD_SCRIPT_BATFILE_${key}_CALLED=TRUE" >>$temp_script_p
        echo -E "set tool_build=$tool_build" >>$temp_script_p
        echo -E "$BUILD_SHELL -$- $tool_build/main.sh $all_args" >>$temp_script_p
        cmd /c "$temp_script"
        cmd /c del "$temp_script"
        exit
    fi
 # show environment parameters
 #   cmd /c set
}

win32_subst_backslash()
{
    p="$1"
    if [ \! -z "$ZSH_NAME" ]; then
        p="${p:gs.\\./.}"
    elif [ \! -z "$BASH" ]; then
        p="${p//\\//}"
	fi
    echo -E "$p"
}

win32_path_to_posix()
{
    p="$1"
    if [ \! -z "$ZSH_NAME" ]; then
        p="${p:gs.\\./.}"
    elif [ \! -z "$BASH" ]; then
        p="${p//\\//}"
        case "$p" in
            *:*)
                p="/cygdrive/${p%:*}${p#*:}"
                ;;
        esac
    fi
    echo -E "$p"
}

win32_path_exists()
{
    p="$1"
    if [ \! -z "$ZSH_NAME" ]; then
        p="${p:gs.\\./.}"
    elif [ \! -z "$BASH" ]; then
        p="${p//\\//}"
        case "$p" in
            *:*)
                p="/cygdrive/${p%:*}${p#*:}"
                ;;
        esac
    fi

    test -e "$p"
    return $?
}

win32_path_not_exists()
{
    win32_path_exists "$@" && return 1
    return 0
}

host_shell()
{
   if [ "$RUN_COMPILE_CMD" = "YES" ]
   then
      RUN_COMPILE_CMD="RUN"
      start cmd.exe /K TITLE "COMPILE %CPU% %PP_TYPE%"
   fi
}

if [ -d $tool_build/../bin ] ; then
    if [ \! -z "$ZSH_NAME" ];
    then
        # zsh handles PATH in different way
        path=("${tool_build%/*}/bin" $path)
    else
        # for cygwin bash, ash
        PATH="${tool_build%/*}/bin:$PATH"
    fi
fi
	
host_pre_build()
{
    win32_add_paths $CPSS_TOOLS_PATH'\tools\bin'
    return 0
}
