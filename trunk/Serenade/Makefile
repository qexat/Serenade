# $Id$

SERENADE_OBJS = parser.o util.o run.o interpreter.o $(EXTRA_OBJS)

.PHONY: all clean
.SUFFIXES: .o .c

all: ./serenade$(SUFFIX) ./$(PREFIX_SO)serenade$(SUFFIX_SO)

./serenade$(SUFFIX): main.o $(SERENADE_OBJS)
	$(CC) $(LDFLAGS) -o $@ main.o $(SERENADE_OBJS) $(LIBS)

./$(PREFIX_SO)serenade$(SUFFIX_SO): $(SERENADE_OBJS)
	$(CC) $(LDFLAGS) -shared -o $@ $(SERENADE_OBJS) $(LIBS)

.c.o: ../config.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o ./serenade ./serenade.exe ./*.so ./*.dll
