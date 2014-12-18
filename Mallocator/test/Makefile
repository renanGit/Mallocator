TARGET= source
CC= gcc
CFLAGS= -m32 #-ggdb -Wall#-I/proj3 
LDFLAGS = -m32 -lmemory -lpthread -lm#-m32 
fPATH = /home/username/Desktop/proj3

# here we list all source files
SRC = testcases.c
OBJS = $(SRC:.c=.o)
DEPS = $(OBJS:.o=.d)


all: $(TARGET)

source: testcases.c memory.h
	$(CC) $(CFLAGS) testcases.c -L $(fPATH) -Wl,-rpath=$(fPATH) $(LDFLAGS) -o testcases
%.o: %.c
	$(CC) $(CFLAGS) -c -MMD $< -o $@

-include $(DEPS)
