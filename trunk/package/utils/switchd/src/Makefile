SRC = $(notdir $(wildcard *.c))
OBJS = $(patsubst %c,%o,$(SRC))
ifeq ($(product_type),mv3336)
LIBS  = -lubox -lubus -lblobmsg_json  -ljwsconvert -luci -ljson-c -ljwf -ljssdk_us -lcpss -lhelper
else
LIBS  = -lubox -lubus -lblobmsg_json -ljssdk_us -ljwsconvert -luci -ljson-c -ljwf
endif 
JSSDK_INC = -I$(STAGING_DIR)/usr/include/jssdk -I$(STAGING_DIR)/usr/include/libjwsconvert

all: switchd

switchd: $(OBJS)
	$(CC) -o $@ $^ $(LIBS) 

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(JSSDK_INC)

.PHONY:clean
clean:
	rm -f *.o switchd
