 # $Id$

CONFIGGEN_OBJS = configgen.o

.PHONY: all clean
.SUFFIXES: .o .c

all: ./configgen

./configgen: $(CONFIGGEN_OBJS)
	cc -o $@ $(CONFIGGEN_OBJS)

.c.o:
	cc -c -o $@ $<

clean:
	rm -f *.o ./configgen ./configgen.exe
