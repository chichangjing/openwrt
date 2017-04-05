#
# Make target definitions for Microsoft Visual C++ 8
#

##flags for makedepend external utility - used for both BC and VC
#MDFLAGS0 = $(subst /D,-D, $(CFLAGS))
#MDFLAGS1 = $(subst /W3,, $(MDFLAGS0))
#MDFLAGS2 = $(subst /Od,, $(MDFLAGS1))
#MDFLAGS3 = $(subst /Gm,, $(MDFLAGS2))
#MDFLAGS4 = $(subst /GX,, $(MDFLAGS3))
#MDFLAGS5 = $(subst /ZI,, $(MDFLAGS4))
#MDFLAGS6 = $(subst /nologo,, $(MDFLAGS5))
#MDFLAGS7 = $(subst /GZ,, $(MDFLAGS6))
#MDFLAGS8 = $(subst /RTC1,, $(MDFLAGS7))
#MDFLAGS9 = $(subst /EHsc,, $(MDFLAGS8))
#MDFLAGS = $(subst /MTd,, $(MDFLAGS9))
MDFLAGS = $(subst /D,-D, $(filter /D%, $(CFLAGS)))


# template for creating library
# args
#   $1 lib name
#   $2 object list
define AR_template
	$(AR) /nologo /out:$(1) $(wildcard $(1)) $(2) /IGNORE:4006
endef

# the dependencies tool not work in VC --> need a fix
# in the meantime remove the building of *.dep files
#$(OBJ_FILE_LIST) : %.obj : %.c %.dep
$(OBJ_FILE_LIST) : %.obj : %.c
	$(CC) $(CFLAGS) $(OS_CFLAGS) $(INCLUDE_PATH) $(PROJECT_PATH) $(FC) /Fo$(SCRAP_DIR)/$@ /Fd$(SCRAP_DIR)/vc80.pdb $(filter %/$(notdir $<),$(C_FILE_LIST_AFTER_EXCLUDE))


 QAC_PROJECT_PATH = $(subst /I ,-i , $(PROJECT_PATH))
# QAC_FLAGS = $(subst /D,-d , $(AD_CFLAGS))
 QAC_FLAGS_ = $(filter /D%, $(CFLAGS)) 
 QAC_FLAGS = $(subst /D,-d , $(QAC_FLAGS_)) 
 QAC_PERSONALITIES = $(subst \,/,$(USER_BASE))/tools/Prqa/personalities.via 
