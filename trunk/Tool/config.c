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
	char* argv2 = argv[2] == NULL ? "" : argv[2];
	if(strcmp(argv[1], "cflags") == 0) {
		if(strcmp(argv2, "NetBSD") == 0) {
			printf("-I/usr/pkg/include ");
		}
		printf("\n");
	} else if(strcmp(argv[1], "libs") == 0) {
		if(strcmp(argv2, "NetBSD") == 0) {
			printf("-L/usr/pkg/lib -Wl,-R/usr/pkg/lib ");
		}
#ifdef HAS_FFI_SUPPORT
		printf("-lffi ");
#endif
#ifdef HAS_READLINE_SUPPORT
		printf("-lreadline ");
		if(strcmp(argv2, "SunOS") == 0) {
			printf("-ltermlib ");
		}
#endif
#if defined(HAS_FFI_SUPPORT) || defined(HAS_BINMODULE_SUPPORT)
		if(strcmp(argv2, "NetBSD") != 0 && strcmp(argv2, "Windows") != 0) {
			printf("-ldl ");
		}
#endif
		printf("\n");
	} else if(strcmp(argv[1], "objs") == 0) {
#ifdef HAS_FFI_SUPPORT
		printf("ffi_binding.o ");
#endif
#ifdef HAS_BINMODULE_SUPPORT
		printf("binmodule.o ");
#endif
		printf("\n");
	}
}
