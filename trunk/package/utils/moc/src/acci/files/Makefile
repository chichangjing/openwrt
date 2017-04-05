all: libcase.so
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

EXEC = libcase.so
SRC = src/libcase/*.o 


CFLAGS += -fPIC
LDFLAGS += -shared  

libcase.so: $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRC) $(LDLIBS)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o *.so
