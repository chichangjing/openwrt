ACCI_OBJS = acType.o acRand.o acArray.o acCrease.o

%.o: %.c
	$(CC) $(IFLAGS) -c $<
	
all: $(ACCI_OBJS)
	@ $(LD) $(LDFLAGS) -o $(OBJ_ACC)  $^
	 
	 
.PHONY : clean	 
clean:
	@ $(RM) -f *.o