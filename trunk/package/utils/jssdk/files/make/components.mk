
# ***********************************************************
#    Copyright (C) 2016 Joyware
# ***********************************************************

ifeq (linux, $(OS))
# ------------------------------------------
  ifeq (KSLIB, $(MODULE_TYPE))
    ifeq (TRUE, $(KERNEL_MODE))
      COMPONENTS = HSL SAL INIT UTIL
      ifeq (TRUE, $(FAL))
        COMPONENTS += FAL
      endif
    else
      COMPONENTS = HSL SAL INIT
    endif

    ifeq (TRUE, $(UK_IF))
      COMPONENTS += API
    endif
  endif
# ------------------------------------------
  ifeq (USLIB, $(MODULE_TYPE))
    ifneq (TRUE, $(KERNEL_MODE))
      COMPONENTS = HSL SAL INIT UTIL
      ifeq (TRUE, $(FAL))
        COMPONENTS += FAL
      endif
    else
      COMPONENTS = UK_IF SAL
    endif

    COMPONENTS += API
    ifeq (MARVELL_88E6097, $(CHIP_TYPE))
      COMPONENTS += DSDT
    endif
  endif
# ------------------------------------------
  ifeq (SHELL, $(MODULE_TYPE))
    COMPONENTS = SHELL
  endif
endif
