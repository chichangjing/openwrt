SYS_OBJS = lnxRmu.o lnxSem.o

%.o: %.c
	$(CC) $(IFLAGS) -c $<
	#$(CC) $(IFLAGS) -c -o build/$@ $< 
	
all: $(SYS_OBJS)
	@ $(LD) $(LDFLAGS) -o $(OBJ_SYS)  $^ 
	 
	 
.PHONY : clean	 
clean:
	@ $(RM) -f *.o