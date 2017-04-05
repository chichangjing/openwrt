SRC = $(wildcard *.c ./eap_common/*.c ./eap_server/*.c ./common/*.c ./crypto/*.c ./driver/*.c ./radius/*.c ./eapol_auth/*.c)
OBJS = $(patsubst %c,%o,$(SRC))
LIBS = -lubox -lubus -lblobmsg_json -ljwsconvert -luci -ljson-c -ljwf 
INC = -I./

all: nas

nas: $(OBJS)
	$(CC) -o $@ $^ $(LIBS) 

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INC)

.PHONY:clean
clean:
	rm -f *.o nas
