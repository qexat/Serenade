# $Id$

SERENADE_OBJS = main.o parser.o util.o run.o interpreter.o $(EXTRA_OBJS)

.PHONY: all clean
.SUFFIXES: .o .c

all: ./serenade$(SUFFIX) ./$(PREFIX_SO)serenade$(SUFFIX_SO)

./serenade$(SUFFIX): $(SERENADE_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(SERENADE_OBJS) $(LIBS)

./$(PREFIX_SO)serenade$(SUFFIX_SO): $(SERENADE_OBJS)
	$(CC) $(LDFLAGS) -shared -o $@ $(SERENADE_OBJS) $(LIBS)

.c.o: ../config.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o ./serenade ./serenade.exe ./*.so ./*.dll
