#!/bin/sh

###########################################################
# Linux_patch_kernel.sh
#
# DESCRIPTION:
#       patch kernel to improve mainOs performance
#
# ENVIRONMENT VARIABLES:
#       DIST           Linux kernel sources path
#       KPATCHLEVEL    Kernel version patch level (4,6)
#
##########################################################

DO_EXPORT_SYSCALL_TABLE="no"
DO_EXTEND_TASK_STRUCT="no"



#####################################################################
# do_patch_kernel_2_4
#
# DESCRIPTION:
#            - linux 2.4 patches
#
# ENVIRONMENT:
#            - LINUX_KERNEL_DEFAULT_IP - optional value for
#              kernel's ip parameter
#           
# OUTPUTS:
#           none
#
# RETURNS:
#           
#           
# COMMENTS:
#           
#
######################################################################
do_patch_kernel_2_4()
{
	if [ "$LINUX_KERNEL_DEFAULT_IP" != "" ]
	then
		# 2.4 fix CMDLINE ip
		mv .config .config.orig
		sed '/^CONFIG_CMDLINE=/ s/ ip=[^ ]* / ip='$LINUX_KERNEL_DEFAULT_IP' /' \
			.config.orig > .config
	fi

	if [ "$DO_EXPORT_SYSCALL_TABLE" = "yes" ]
	then
#		# patch kernel/sched.c: export sys_call_table
#		mv kernel/sched.c kernel/sched.c.orig
#		sed '$ a \
#extern long sys_call_table[];\
#EXPORT_SYMBOL(sys_call_table);
#		' kernel/sched.c.orig >kernel/sched.c

		echo "export KERNEL_SYSCALL_TABLE_EXPORTED=yes" >>kernel_patched_env.sh
	fi

	if [ "$DO_EXTEND_TASK_STRUCT" = "yes" ]
	then
		# patch include/linux/sched.h: add extra fields to task_struct 
		mv include/linux/sched.h include/linux/sched.h.orig
		sed '
			/struct task_struct {/,/^};/ {
				/^};/ i\
	void* mv_ptr;\
	void (*mv_cleanup)(struct task_struct *tsk);
			}
		' include/linux/sched.h.orig > include/linux/sched.h

		# patch kernel/fork.c: initialize pointers
		mv kernel/fork.c kernel/fork.c.orig
		sed '/\*p = \*current;/ a\
	p->mv_ptr = NULL;\
	p->mv_cleanup = NULL;
		' kernel/fork.c.orig > kernel/fork.c

		# patch kernel/exit.c: call cleanup function
		mv kernel/exit.c kernel/exit.c.orig
		sed '/sem_exit()/ i\
	if (tsk->mv_cleanup)\
		tsk->mv_cleanup(tsk);
		' kernel/exit.c.orig > kernel/exit.c

		echo "export KERNEL_TASK_STRUCT_EXTENDED=yes" >>kernel_patched_env.sh
	fi
}


#####################################################################
# do_patch_kernel_2_6
#
# DESCRIPTION:
#            - linux 2.6 patches
#
# OUTPUTS:
#           none
#
# RETURNS:
#           
#           
# COMMENTS:
#           
#
######################################################################
do_patch_kernel_2_6()
{
	if [ "$DO_EXPORT_SYSCALL_TABLE" = "yes" ]
	then
		# patch kernel/sched.c: export sys_call_table
		mv kernel/sched.c kernel/sched.c.orig
		sed '$ a \
extern long sys_call_table[];\
EXPORT_SYMBOL(sys_call_table);
		' kernel/sched.c.orig >kernel/sched.c

		echo "export KERNEL_SYSCALL_TABLE_EXPORTED=yes" >>kernel_patched_env.sh
	fi

	if [ "$DO_EXTEND_TASK_STRUCT" = "yes" ]
	then
		# patch include/linux/sched.h: add extra fields to task_struct 
		mv include/linux/sched.h include/linux/sched.h.orig
		sed '
			/struct task_struct {/,/^};/ {
				/^};/ i\
	void* mv_ptr;\
	void (*mv_cleanup)(struct task_struct *tsk);
			}
		' include/linux/sched.h.orig > include/linux/sched.h

		# patch kernel/fork.c: initialize pointers
		mv kernel/fork.c kernel/fork.c.orig
		sed '/\*tsk = \*orig;/ a\
	tsk->mv_ptr = NULL;\
	tsk->mv_cleanup = NULL;
		' kernel/fork.c.orig > kernel/fork.c

		# patch kernel/exit.c: call cleanup function
		mv kernel/exit.c kernel/exit.c.orig
		sed '/exit_sem/ i\
	if (tsk->mv_cleanup)\
		tsk->mv_cleanup(tsk);
		' kernel/exit.c.orig > kernel/exit.c

		echo "export KERNEL_TASK_STRUCT_EXTENDED=yes" >>kernel_patched_env.sh
	fi
}



######################
# main
case "2.$KPATCHLEVEL" in
	2.4 | 2.6)
		cd $DIST || exit 1
		rm -f kernel_patched_env.sh
		do_patch_kernel_2_$KPATCHLEVEL
		;;
	*)
		# unsupported kernel
		exit 0
		;;
esac
