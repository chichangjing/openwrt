#
# Make target definitions for Borland compiler
#

##flags for makedepend external utility - used for both BC
MDFLAGS = $(filter -D%, $(CFLAGS)) -D_M_IX86

# template for creating library
# args
#   $1 lib name
#   $2 object list
define AR_template
	$(AR) $(subst /,\,$(1)) $(subst /,\,$(2))
endef

 QAC_PROJECT_PATH = $(subst -I,-i , $(PROJECT_PATH))
 QAC_FLAGS_ = $(filter -D%, $(CFLAGS)) 
 QAC_FLAGS  = $(subst -D,-d , $(QAC_FLAGS_))
 QAC_PERSONALITIES = $(USER_BASE)\tools\Prqa\personalities.via  

$(OBJ_FILE_LIST) : %.$(OBJ) : %.c
	$(CC) $(CFLAGS) $(OS_CFLAGS) $(INCLUDE_PATH) $(PROJECT_PATH) -c -o$(SCRAP_DIR)/$@ $(filter %/$(notdir $<),$(C_FILE_LIST_AFTER_EXCLUDE))
