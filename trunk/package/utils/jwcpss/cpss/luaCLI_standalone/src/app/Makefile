SOURCES = cmdStreamSerial.c main.c
# SOURCES += cmdStreamGeneric.c
CFLAGS += -I$(LUACLI_ROOT)/h

OBJS = $(SOURCES:.c=.o)

all: $(OBJS)
	$(AR) crus app.a $(OBJS)


$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS)
	rm -f app.a
