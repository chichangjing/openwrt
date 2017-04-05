
include ../common.mk

DEFS=
CCFLAGS = $(CFLAGS) -D_REENTRANT -D__LINUX__ 

INCLUDES = -I.
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(CCFLAGS)
LIBS=
ARFLAGS = cr

JRPPLIBS = jrpp
JRPPLIBF = lib$(JRPPLIBS).a

# JRPPCFILES = statmch.c ring.c port.c rpp_in.c transmit.c
# JRPPOFILES = statmch.o ring.o port.o rpp_in.o transmit.o

JRPPCFILES = $(notdir $(wildcard *.c))
JRPPOFILES = $(patsubst %c,%o,$(JRPPCFILES))

CFILES= $(JRPPCFILES)
OFILES= $(JRPPOFILES)

.SUFFIXES: .S .c .o .s

.c.o:
	$(COMPILE) -c $<  -o $@

all:

$(JRPPLIBF): ${JRPPOFILES}
	-rm -f $@
	$(AR) $(ARFLAGS) $@ $(JRPPOFILES)
	$(RANLIB)  $@

clean:
	rm -f *.o *.a

depend:
	echo "# DO NOT DELETE THIS LINE -- make  depend  depends  on it." > make.depend
	makedepend -- $(DEFS) $(INCLUDES) $(CFLAGS) -- $(CFILES) -f make.depend

-include make.depend

