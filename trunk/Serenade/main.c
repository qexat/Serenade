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
#include "interpreter.h"
#include "parser.h"
#include "run.h"
#include "serenade.h"
#include "util.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef HAS_READLINE_SUPPORT
#include <readline/readline.h>
#include <readline/history.h>
#endif

extern bool is_repl;

int main(int argc, char** argv) {
	int i;
	bool loaded = false;
	for(i = 1; i < argc; i++) {
		if(argv[i][0] == '-') {
			if(strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-V") == 0) {
				printf("Serenade LISP %s\n", SERENADE_VERSION);
				printf("Support: %s\n", SUPPORT);
				printf("Parser stack size: %d\n", PARSER_STACK_SIZE);
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
				struct sn_interpreter* sn = sn_create_interpreter();
				sn_stdlib_init(sn);
				sn_ffi_init(sn);
				int r = sn_eval(sn, str, s.st_size);
				sn_interpreter_free(sn);
				free(str);
				return r;
			} else {
				fprintf(stderr, "%s: %s: stat fail\n", argv[0], argv[i]);
				return 1;
			}
		}
	}
#ifdef HAS_REPL_SUPPORT
	if(!loaded) {
		is_repl = true;
		printf("  ---+-------+---    oooo                                            8       \n");
		printf("  ---+-------+---   8    8                                           8       \n");
		printf("     |       |      8       oooo  8 oooo   oooo  8 oooo   oooo   oooo8  oooo \n");
		printf("     |       |       oooo  8    8 8o    o 8    8 8o    8      8 8    8 8    8\n");
		printf("     |       |           8 8oooo8 8       8oooo8 8     8  oooo8 8    8 8oooo8\n");
		printf("     |       |      9    8 8      8       8      8     8 8    8 8    8 8     \n");
		printf("     |       |       oooo   oooo  8        oooo  8     8  oooo8  oooo8  oooo \n");
		printf("\n");
		printf("Welcome to Serenade LISP %s <http://nishi.boats/serenade>\n", SERENADE_VERSION);
		printf("\n");
		printf("Copyright (c) Nishi 2024\n");
		printf("\n");
		printf("Support: %s\n", SUPPORT);
		printf("Parser stack size: %d\n", PARSER_STACK_SIZE);
		struct sn_interpreter* sn = sn_create_interpreter();
		sn_stdlib_init(sn);
		sn_ffi_init(sn);
		char cbuf[2];
		cbuf[0] = '\n';
		cbuf[1] = 0;
		char* str = malloc(1);
		str[0] = 0;
		int br = 0;
#ifdef HAS_READLINE_SUPPORT
		char* line = NULL;
#endif
		while(1) {
#ifdef HAS_READLINE_SUPPORT
			line = readline("> ");
			if(line == NULL){
				free(line);
				continue;
			}
			int i;
			for(i = 0; line[i] != 0; i++){
				if(line[i] == '('){
					br++;
				}else if(line[i] == ')'){
					br--;
				}
			}
			if(strcmp(line, ":quit") == 0){
				free(line);
				break;
			}
			char* tmp = str;
			str = sn_strcat(tmp, line);
			free(tmp);
			if(br == 0){
				sn_eval(sn, str, strlen(str));
				add_history(str);
				free(str);
				str = malloc(1);
				str[0] = 0;
			}
			free(line);
#else
			if(cbuf[0] == '\n') {
				if(strcmp(str, ":quit") == 0){
					break;
				}
				if(br == 0 && strlen(str) > 0) {
					sn_eval(sn, str, strlen(str));
					free(str);
					str = malloc(1);
					str[0] = 0;
					printf("\n");
				} else if(br > 0) {
					char* tmp = str;
					str = sn_strcat(tmp, cbuf);
					free(tmp);
				}
				printf("> ");
				fflush(stdout);
			}
			int l = fread(cbuf, 1, 1, stdin);
			if(l <= 0) break;
			if(cbuf[0] == '(') {
				br++;
			} else if(cbuf[0] == ')') {
				br--;
			}
			if(cbuf[0] != '\r' && cbuf[0] != '\n') {
				char* tmp = str;
				str = sn_strcat(tmp, cbuf);
				free(tmp);
			}
#endif
		}
		free(str);
		sn_interpreter_free(sn);
	}
	return 0;
#else
	if(!loaded) {
		fprintf(stderr, "usage: %s [options] input\n", argv[0]);
		return 1;
	}
#endif
}
