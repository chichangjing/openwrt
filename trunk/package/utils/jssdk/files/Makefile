
# ***********************************************************
#    Copyright (C) 2016 Joyware
#    Joyware Switch Software Development Kit
#    Top makefile for openwrt environment
#    Version 1.0, hejianguo@obtelecom.com
# ***********************************************************

include ./config

ifndef PRJ_PATH
  PRJ_PATH=$(shell pwd)
endif
export PRJ_PATH

include ./make/config.mk
include ./make/option.mk

SUB_DIR=$(patsubst %/, %, $(dir $(wildcard src/*/Makefile)))
SUB_LIB=$(subst src/, , $(SUB_DIR))

all: $(JSSDK_BIN_DIR) config2h uslib shell
	@echo "---Build [JSSDK-$(JSSDK_BUILD_VERSION)] at $(JSSDK_BUILD_DATE) finished."
	@echo "SUB_DIR = $(SUB_DIR), SUB_LIB = $(SUB_LIB)."

uslib:uslib_o
	$(AR) -r $(JSSDK_BIN_DIR)/$(JSSDK_US_MOD).a $(wildcard $(JSSDK_BLD_DIR)/USLIB/*.o)
	$(LD) -shared -fpic -o $(JSSDK_BIN_DIR)/$(JSSDK_US_MOD).so $(LIBFLAGS) $(wildcard $(JSSDK_BLD_DIR)/USLIB/*.o)

uslib_o:
	$(foreach i, $(SUB_LIB), $(MAKE) MODULE_TYPE=USLIB -C src/$i all || exit 1;)

shell:uslib shell_o
	cp $(JSSDK_BLD_DIR)/SHELL/$(JSSDK_SHELL_OBJ) $(JSSDK_BIN_DIR)/$(JSSDK_SHELL_OBJ)
	$(STRIP) $(JSSDK_BIN_DIR)/$(JSSDK_SHELL_OBJ)

shell_o:
	$(foreach i, $(SUB_LIB), make -S MODULE_TYPE=SHELL -C src/$i all || exit 1;)

$(JSSDK_BIN_DIR):
	mkdir -p $@

config2h:config
	perl $(PRJ_PATH)/board/config2h.pl $(PRJ_PATH)

release:
	@cd make; ./release.sh $(VER)

clean:
	rm -f $(JSSDK_BLD_DIR)/KSLIB/*
	rm -f $(JSSDK_BLD_DIR)/USLIB/*
	rm -f $(JSSDK_BLD_DIR)/SHELL/*
	rm -f $(JSSDK_BIN_DIR)/*




