
all: jw_setmac libjwf.so

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

libjwf.so: libjwf.o
	$(LD) -shared -fpic -o $@ libjwf.o

JWF_OBJS:= jw_setmac.o libjwf.o
jw_setmac: $(JWF_OBJS)
	$(CC) -o $@ $(JWF_OBJS)

.PHONY:clean
clean:
	rm -f *.o jw_setmac libjwf.so
