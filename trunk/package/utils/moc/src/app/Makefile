TEST_OBJS = main.o
MDIO_OBJS = mdio.o 
XMDIO_OBJS = xmdio.o 
PPHY_OBJS = pphy.o 

all: $(TEST_OBJS) $(MDIO_OBJS) $(XMDIO_OBJS) $(PPHY_OBJS)


%.o: %.c
	$(CC) $(IFLAGS) -c $<
	

	 
	 
.PHONY : clean	 
clean:
	@ $(RM) -f *.o