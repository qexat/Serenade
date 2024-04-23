# $Id$

CC = cc
CFLAGS = -std=c99
LDFLAGS =
LIBS =
SUFFIX =

.include "platform.mk"

.PHONY: all ./Serenade

all: ./Tool ./Serenade

./Tool::
	$(MAKE) -C ./Tool CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LIBS)" SUFFIX="$(SUFFIX)"

./Serenade::
	$(MAKE) -C ./Serenade CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LIBS)" SUFFIX="$(SUFFIX)"

clean:
	$(MAKE) -C ./Tool clean
	$(MAKE) -C ./Serenade clean
