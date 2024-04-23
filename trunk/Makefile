# $Id$

CC = cc
CFLAGS = -std=c99 -g
LDFLAGS =
LIBS =
SUFFIX =

.if "$(PLATFORM)" != ""
.include "platforms/${PLATFORM}.mk"
.else
.include "platform.mk"
.endif

.PHONY: all ./Serenade ./Tool replace format

all: ./Tool ./config.h ./Serenade

./Tool::
	$(MAKE) -C ./Tool CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LIBS)" SUFFIX="$(SUFFIX)"

./config.h:
	if [ ! -e ./Tool/configgen ]; then $(MAKE) ./Tool ; fi
	./Tool/configgen $@

./Serenade::
	$(MAKE) -C ./Serenade CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LIBS)" SUFFIX="$(SUFFIX)"

clean:
	$(MAKE) -C ./Tool clean
	$(MAKE) -C ./Serenade clean
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
