
include ./common.mk

INCLUDES = -I. -I./rstplib -I$(STAGING_DIR)/usr/include/jssdk
GCCFLAGS = $(CFLAGS) $(INCLUDES) -D__LINUX__ -DSTP_DBG=1 -DRSTP_JWS=1 -g -DOLD_READLINE

CFILES_RSTPD = log.c utils.c libnetlink.c epoll_loop.c brmon.c brstate.c packet.c ctl_socket.c rstp_if.c rstpd.c 
OFILES_RSTPD = $(CFILES_RSTPD:.c=.o)
CFILES_CTL = utils.c ctl_client.c ctl_main.c log.c ctl_wrap.c
OFILES_CTL = $(CFILES_CTL:.c=.o)

.SUFFIXES: .c .o
.c.o:
	$(CC) $(GCCFLAGS) -c $<  -o $@

all: rstpd rstpctl

rstplib:
	make -C rstplib librstp.a

.PHONY: rstplib

rstpd: $(OFILES_RSTPD) rstplib
	$(CC) $(GCCFLAGS) -o $@ $(OFILES_RSTPD) -L ./rstplib -lrstp -ljssdk_us

rstpctl: $(OFILES_CTL)
	$(CC) $(GCCFLAGS) -o $@ $(OFILES_CTL)

clean:
	rm -f *.o rstpd rstpctl
	make -C rstplib clean
