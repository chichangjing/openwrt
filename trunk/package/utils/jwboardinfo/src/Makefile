
all: jwboardinfo

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY:clean
clean:
	rm -f *.o jwboardinfo
