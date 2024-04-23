# $Id$

CC = cc
CFLAGS = -std=c99
LDFLAGS =
LIBS =
SUFFIX =

.include "platform.mk"

.PHONY: all ./Serenade ./Tool replace

all: ./Tool ./Serenade

./Tool::
	$(MAKE) -C ./Tool CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LIBS)" SUFFIX="$(SUFFIX)"

./Serenade::
	$(MAKE) -C ./Serenade CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LIBS)" SUFFIX="$(SUFFIX)"

clean:
	$(MAKE) -C ./Tool clean
	$(MAKE) -C ./Serenade clean

FILES = `find . -name "*.c" -or -name "*.h"`

replace:
	for i in $(FILES); do \
		echo -n "$$i ... "; \
		perl replace.pl < $$i > $$i.new; \
		mv $$i.new $$i; \
		echo "done"; \
	done
