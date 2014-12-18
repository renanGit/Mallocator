TARGET= shlib
SUBTARGET= testcases
CC= gcc
CFLAGS= -m32 -c -fpic #
SRC = memory.c bits.c


all: $(TARGET)

shlib: $(SRC)
	$(CC) $(CFLAGS) memory.c
	$(CC) $(CFLAGS) bits.c
	$(CC) -m32 -shared -o libmemory.so memory.o bits.o
	cd test; make
	
# let's clean up the mess
clean:
	rm -rf *.dSYM $() *.o *.so
	cd test; rm -rf *.o *.d $(SUBTARGET)
