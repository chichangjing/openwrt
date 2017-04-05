
all: obring obring_server

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

OBJS = obring.o
obring: $(OBJS)
	$(CC) -o $@ $(OBJS)

OBR_LIBS = -lubox -lubus -lblobmsg_json
OBJSER = obring_server.o count.o
obring_server: $(OBJSER)
	$(CC) -o $@ $(OBJSER) $(OBR_LIBS)

.PHONY:clean
clean:
	rm -f rbcfg *.o obring obring_server
