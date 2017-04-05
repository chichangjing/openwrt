TESTNAME1 = i2c_read
TESTNAME2 = i2c_write

all:
	$(CC) -o  $(TESTNAME1) $(TESTNAME1).c strfunc.c
	$(CC) -o  $(TESTNAME2) $(TESTNAME2).c strfunc.c
			
clean:
	rm -fr *.mod.* *.ko *.o .tmp* Module* modules* .*.cmd
	rm $(TESTNAME1) $(TESTNAME2)

