# $Id$

SERENADE_OBJS = main.o parser.o util.o run.o interpreter.o

.PHONY: all clean
.SUFFIXES: .o .c

all: ./serenade$(SUFFIX)

./serenade$(SUFFIX): $(SERENADE_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(SERENADE_OBJS) $(LIBS)

.c.o: ../config.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o ./serenade ./serenade.exe
