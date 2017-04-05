MVL_DRV_DIRS := $(DSDT_USER_BASE)/..

MVL_QD_DIR := $(MVL_DRV_DIRS)/switch
MVL_MAD_DIR := $(MVL_DRV_DIRS)/phy


all:	SW PHY


SW:
	make -C $(MVL_QD_DIR)/src;


PHY:
	make -C $(MVL_MAD_DIR)/src;


clean :
	make -C $(MVL_QD_DIR)/src clean
	make -C $(MVL_MAD_DIR)/src clean
