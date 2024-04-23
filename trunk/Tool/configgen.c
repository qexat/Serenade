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

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

char choice[256];

char* asks[] = {"repl", "y", "HAS_REPL_SUPPORT", "Do you want the REPL support?",

		"ffi",	"n", "HAS_FFI_SUPPORT",	 "Do you want the FFI support?",

		NULL};

void show_dialog(int n) {
	fprintf(stderr, "[default is %c] %s ", asks[n * 4 + 1][0], asks[n * 4 + 3]);
	fflush(stderr);
}

int main(int argc, char** argv) {
	FILE* out = stdout;
	bool load = false;
	if(argv[1] != NULL) {
		out = fopen(argv[1], "w");
		if(out != NULL) {
			load = true;
		} else {
			fprintf(stderr, "%s: %s: couldn't open the file\n", argv[0], argv[1]);
			return 1;
		}
	}
	fprintf(out, "/* Autogenerated config */\n");
	char c;
	char oldc;
	bool nl = false;
	int n = 0;
	show_dialog(n);
	while(true) {
		oldc = c;
		if(fread(&c, 1, 1, stdin) <= 0) break;
		if(c == '\n') {
			char ch = asks[n * 4 + 1][0];
			if(nl) {
				ch = tolower(oldc);
			}
			choice[n] = ch;
			fprintf(out, "#%s %s\n", ch == 'y' ? "define" : "undef", asks[n * 4 + 2]);
			n++;
			nl = false;
			if(asks[n * 4 + 1] == NULL) break;
			show_dialog(n);
		} else {
			nl = true;
		}
	}
	int stack_size = 1024;
	fprintf(stderr, "[recommended: 1024] Parser stack size? ");
	fflush(stderr);
	scanf("%d", &stack_size);
	fprintf(out, "#define PARSER_STACK_SIZE %d\n", stack_size);
	fprintf(out, "#define SUPPORT \"");
	for(n = 0; asks[n * 4] != NULL; n++) {
		if(n > 0) fprintf(out, " ");
		fprintf(out, "%c%s", choice[n] == 'y' ? '+' : '-', asks[n * 4]);
	}
	fprintf(out, "\"\n");
	if(load) fclose(out);
}
