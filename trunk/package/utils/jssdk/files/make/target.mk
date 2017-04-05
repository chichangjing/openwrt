
# ***********************************************************
#    Copyright (C) 2016 Joyware
# ***********************************************************

include $(PRJ_PATH)/make/option.mk

obj: $(OBJ_LIST)
	$(OBJ_LOOP)

dep: build_dir $(DEP_LIST)
	$(DEP_LOOP)

$(OBJ_LIST): %.o : %.c %.d
	$(CC) $(CPU_CFLAGS) -c $< -o $(DST_DIR)/$@

$(DEP_LIST) : %.d : %.c
	$(CC) $(CPU_CFLAGS) -MM $< > $(DST_DIR)/$@.tmp
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $(DST_DIR)/$@.tmp > $(DST_DIR)/$@
	rm -f $(DST_DIR)/$@.tmp;

build_dir: $(DST_DIR)

$(DST_DIR):
	mkdir -p $(DST_DIR)
	@echo ===================================================================================================================
	@echo BUILD_DIR = $(BUILD_DIR)
	@echo STAGING_DIR = $(STAGING_DIR)
	@echo CPU = $(CPU)
	@echo OS = $(OS)
	@echo CC = $(CC)
	@echo AR = $(AR)
	@echo STRIP = $(STRIP)
	@echo CPU_CFLAGS = $(CPU_CFLAGS)
	@echo MODULE_CFLAG = $(MODULE_CFLAG)
	@echo MODULE_INC = $(MODULE_INC)
	@echo ===================================================================================================================

.PHONY: clean
clean: clean_o clean_d
	$(CLEAN_LOOP)

.PHONY: clean_o
clean_o: clean_obj
	$(CLEAN_OBJ_LOOP)

.PHONY: clean_d
clean_d: clean_dep
	$(CLEAN_DEP_LOOP)

clean_obj:
ifneq (,$(word 1, $(OBJ_FILE)))
	rm -f $(OBJ_FILE)
endif

clean_dep:
ifneq (,$(word 1, $(DEP_FILE)))
	rm -f $(DEP_FILE)
endif

ifneq (,$(word 1, $(DEP_FILE)))
  sinclude $(DEP_FILE)
endif
