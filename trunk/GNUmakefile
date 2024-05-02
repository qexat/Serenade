# $Id$

CPP = cpp
CC = cc

GREP = grep
ifeq ($(shell uname -s),SunOS)
GREP = ggrep
CC = gcc
endif

CFLAGS = -std=c99 -fPIC -g `$(CPP) -DCFLAGS cmdline | $(GREP) -vE "^\#" | tr '\\n' ' '`
LDFLAGS =
LIBS = `$(CPP) -DLIBS cmdline | $(GREP) -vE "^\#" | tr '\\n' ' '`
EXTRA_OBJS = `$(CPP) -DOBJS cmdline | $(GREP) -vE "^\#" | tr '\\n' ' '`
SUFFIX =
SUFFIX_SO = .so
PREFIX_SO = lib

ifneq ("$(PLATFORM)", "")
include "platforms/${PLATFORM}.mk"
endif

.PHONY: all ./Serenade ./Tool replace format

all: ./Tool ./config.h ./Serenade

./Tool::
	$(MAKE) -C ./Tool CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LIBS)" SUFFIX="$(SUFFIX)"

./config.h:
	if [ ! -e ./Tool/configgen ]; then $(MAKE) ./Tool ; fi
	./Tool/configgen $@

./Serenade::
	$(MAKE) -C ./Serenade CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LIBS)" SUFFIX="$(SUFFIX)" EXTRA_OBJS="$(EXTRA_OBJS)" SUFFIX_SO="$(SUFFIX_SO)" PREFIX_SO="$(PREFIX_SO)"

clean:
	$(MAKE) -C ./Tool clean
	$(MAKE) -C ./Serenade clean SUFFIX_SO="$(SUFFIX_SO)" PREFIX_SO="$(PREFIX_SO)"
	rm -f config.h

FILES = `find . -name "*.c" -or -name "*.h"`

replace:
	for i in $(FILES); do \
		echo -n "$$i ... "; \
		perl replace.pl < $$i > $$i.new; \
		mv $$i.new $$i; \
		echo "done"; \
	done

format:
	clang-format -i $(FILES)
