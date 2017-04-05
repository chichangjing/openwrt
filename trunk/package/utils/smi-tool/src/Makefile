
all: smi-tool libsmi.so

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

libsmi.so: libsmi.o
	$(LD) -shared -fpic -o $@ libsmi.o

SMI_OBJS:= smi-tool.o strfunc.o libsmi.o
smi-tool: $(SMI_OBJS)
	$(CC) -o $@ $(SMI_OBJS)

.PHONY:clean
clean:
	rm -f *.o smi-tool libsmi.so
