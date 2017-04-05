all: librmi.so rmi-tool

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
	
CFLAGS += -fPIC
LDFLAGS += -shared

EXEC = librmi.so

PWD_DIR	=$(shell cd ./; pwd)

RMI_O=Rmu.o RmiAtu.o RmiMibs.o RmiRegs.o Rmi.o

librmi.so: $(RMI_O)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(RMI_O)

rmi-tool: $(RMI_O) rmi-tool.o
	$(CC) -o $@ rmi-tool.o -lubox -lpthread librmi.so 


.PHONY:clean
clean:
	-rm -f $(EXEC) *.elf *.gdb *.o *.so
