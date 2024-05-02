 # $Id$

CONFIGGEN_OBJS = configgen.o

.PHONY: all clean
.SUFFIXES: .o .c

all: ./configgen

./configgen: $(CONFIGGEN_OBJS)
	$(CC) -o $@ $(CONFIGGEN_OBJS)

.c.o:
	$(CC) -c -o $@ $<

clean:
	rm -f *.o ./configgen ./configgen.exe
