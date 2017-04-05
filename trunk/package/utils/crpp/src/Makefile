SRC = $(notdir $(wildcard *.c))
OBJS = $(patsubst %c,%o,$(SRC))
LIBS = -lubox -lubus -lblobmsg_json -ljwsconvert -luci -ljson-c -ljwf 
all: crpp

crpp: $(OBJS)
	$(CC) -o $@ $^ $(LIBS) 

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@ 

.PHONY:clean
clean:
	rm -f *.o crpp
