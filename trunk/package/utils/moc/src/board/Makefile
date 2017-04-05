BOARD_OBJS = board.o brQos.o brAtu.o

%.o: %.c
	$(CC) $(IFLAGS) -c $<
	#$(CC) $(IFLAGS) -c -o build/$@ $< 
	
all: $(BOARD_OBJS)
	@ $(LD) $(LDFLAGS) -o $(OBJ_BRD)  $^
	 
	 
.PHONY : clean	 
clean:
	@ $(RM) -f *.o