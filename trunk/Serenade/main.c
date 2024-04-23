/* $Id$ */
/* --- START LICENSE --- */
/* -------------------------------------------------------------------------- */
/*                                                Serenade is a Lisp Dialect  */
/* -------------------------------------------------------------------------- */
/* Copyright (c) 2024 Nishi.                                                  */
/* Redistribution and use in source and binary forms, with or without modific */
/* ation, are permitted provided that the following conditions are met:       */
/*     1. Redistributions of source code must retain the above copyright noti */
/* ce, this list of conditions and the following disclaimer.                  */
/*     2. Redistributions in binary form must reproduce the above copyright n */
/* otice, this list of conditions and the following disclaimer in the documen */
/* tation and/or other materials provided with the distribution.              */
/*     3. Neither the name of the copyright holder nor the names of its contr */
/* ibutors may be used to endorse or promote products derived from this softw */
/* are without specific prior written permission.                             */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS */
/* " AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, TH */
/* E IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPO */
/* SE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS  */
/* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CON */
/* SEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITU */
/* TE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPT */
/* ION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, S */
/* TRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN AN */
/* Y WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY  */
/* OF SUCH DAMAGE.                                                            */
/* -------------------------------------------------------------------------- */
/* --- END LICENSE --- */

#include "../config.h"
#include "parser.h"
#include "serenade.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char** argv) {
	int i;
	bool loaded = false;
	for(i = 1; i < argc; i++) {
		if(argv[i][0] == '-') {
			if(strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-V") == 0) {
				printf("Serenade LISP %s\n", SERENADE_VERSION);
				printf("Support: %s\n", SUPPORT);
				printf("Stack size: %d\n", STACK_SIZE);
				return 1;
			} else {
				fprintf(stderr, "%s: %s: invalid option\n", argv[0], argv[i]);
				return 1;
			}
		} else {
			/* file input */
			loaded = true;
			struct stat s;
			if(stat(argv[i], &s) == 0) {
				char* str = malloc(s.st_size);
				FILE* f = fopen(argv[i], "rb");
				if(f == NULL) {
					fprintf(stderr, "%s: %s: fopen fail\n", argv[0], argv[i]);
					free(str);
					return 1;
				}
				fread(str, 1, s.st_size, f);
				struct sn_generic** t = sn_parse(str, s.st_size);
				if(t != NULL) {
					int j;
					for(j = 0; t[j] != NULL; j++) {
						sn_generic_free(t[j]);
					}
					free(t);
				}
				free(str);
			} else {
				fprintf(stderr, "%s: %s: stat fail\n", argv[0], argv[i]);
				return 1;
			}
		}
	}
#ifdef HAS_REPL_SUPPORT
	if(!loaded) {
		printf("Welcome to Serenade LISP %s\n", SERENADE_VERSION);
		printf("Support: %s\n", SUPPORT);
		printf("Stack size: %d\n", STACK_SIZE);
	}
	return 0;
#else
	if(!loaded) {
		fprintf(stderr, "usage: %s [options] input\n", argv[0]);
		return 1;
	}
#endif
}
