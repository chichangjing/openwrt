DEV_OBJS = dlDev.o dlAtu.o dlQos.o

%.o: %.c
	$(CC) $(IFLAGS) -c $<
	
all: $(DEV_OBJS)
	@ $(LD) $(LDFLAGS) -o $(OBJ_DEV)  $^


.PHONY : clean	 
clean:
	@ $(RM) -f *.o