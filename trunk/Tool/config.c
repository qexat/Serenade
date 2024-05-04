/* $Id$ */
#define COMMANDLINE
#include "../config.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
	if(argc == 1) {
		fprintf(stderr, "insufficient arguments\n");
		return 1;
	}
	if(strcmp(argv[1], "cflags") == 0) {
		printf(
#ifdef __NetBSD__
		    "-I/usr/pkg/include "
#endif
		    "\n");
	} else if(strcmp(argv[1], "libs") == 0) {
		printf(
#ifdef __NetBSD__
		    "-Wl,-R/usr/pkg/lib -L/usr/pkg/lib "
#endif
#ifdef HAS_FFI_SUPPORT
		    "-lffi "
#endif
#ifdef HAS_READLINE_SUPPORT
		    "-lreadline "
#endif
#if defined(HAS_FFI_SUPPORT) || defined(HAS_BINMODULE_SUPPORT)
#ifdef __linux__
		    "-ldl "
#endif
#endif
		    "\n");
	} else if(strcmp(argv[1], "objs") == 0) {
		printf(
#ifdef HAS_FFI_SUPPORT
		    "ffi_binding.o "
#endif
#ifdef HAS_BINMODULE_SUPPORT
		    "binmodule.o "
#endif
		    "\n");
	}
}
