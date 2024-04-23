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

#include "parser.h"
#include "../config.h"
#include "util.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/*
struct sn_generic {
	int type;
	double number;
	char* string;
	unsigned long long string_length;
	void* ptr;
	struct sn_tree* tree;
};
*/

void push_stack_generic(struct sn_generic* gen, struct sn_generic* pushthis) {
	if(gen->type == SN_TYPE_TREE) {
		struct sn_generic** old_gen = gen->tree->args;
		int i;
		for(i = 0; old_gen[i] != NULL; i++)
			;
		gen->tree->args = malloc(sizeof(struct sn_generic*) * (i + 2));
		for(i = 0; old_gen[i] != NULL; i++) {
			gen->tree->args[i] = old_gen[i];
		}

		gen->tree->args[i] = pushthis;
		gen->tree->args[i + 1] = NULL;

		free(old_gen);
	}
}

void push_stack(struct sn_generic* gen, char* buf, int mode) {
	struct sn_generic* newgen = malloc(sizeof(struct sn_generic));
	newgen->type = mode;
	if(mode == SN_TYPE_STRING || mode == SN_TYPE_FUNCTION) {
		newgen->string = sn_strdup(buf);
		newgen->string_length = strlen(buf);
	} else if(mode == SN_TYPE_DOUBLE) {
		newgen->number = atof(buf);
	}
	push_stack_generic(gen, newgen);
}

struct sn_generic* sn_expr_parse(char* data, unsigned long long size) {
	int i;
	int br = 0;
	bool dq = false;
	struct sn_generic** gn_stack = malloc(sizeof(*gn_stack) * STACK_SIZE);
	int* index_stack = malloc(sizeof(int) * STACK_SIZE);
	char* argbuf = malloc(1);
	argbuf[0] = 0;
	int argbufmode = SN_TYPE_VOID;
	for(i = 0; i < size; i++) {
		char c = data[i];
		if(c == '"') {
			dq = !dq;
		} else if(dq) {
			char cbuf[2] = {c, 0};

			char* tmp = argbuf;
			argbuf = sn_strcat(tmp, cbuf);
			free(tmp);
			argbufmode = SN_TYPE_STRING;
		} else if(c == '(') {
			gn_stack[br] = malloc(sizeof(struct sn_generic));
			gn_stack[br]->type = SN_TYPE_TREE;
			gn_stack[br]->tree = malloc(sizeof(struct sn_tree));
			gn_stack[br]->tree->args = malloc(sizeof(struct sn_generic*));
			gn_stack[br]->tree->args[0] = NULL;
			index_stack[br] = 0;
			br++;
		} else if(c == ')') {
			if(strlen(argbuf) > 0) {
				push_stack(gn_stack[br - 1], argbuf, argbufmode);
				index_stack[br - 1]++;
			}
			free(argbuf);
			argbuf = malloc(1);
			argbuf[0] = 0;
			argbufmode = SN_TYPE_VOID;
			if(br > 1) {
				push_stack_generic(gn_stack[br - 2], gn_stack[br - 1]);
				index_stack[br - 2]++;
			}
			br--;
		} else {
			if(c == ' ') {
				if(strlen(argbuf) > 0) {
					push_stack(gn_stack[br - 1], argbuf, argbufmode);
					index_stack[br - 1]++;
				}
				free(argbuf);
				argbuf = malloc(1);
				argbuf[0] = 0;
				argbufmode = SN_TYPE_VOID;
			} else {
				char cbuf[2] = {c, 0};

				char* tmp = argbuf;
				argbuf = sn_strcat(tmp, cbuf);
				free(tmp);

				if(argbufmode == SN_TYPE_VOID) {
					if(c == '.' || (c >= '0' && c <= '9')) {
						argbufmode = SN_TYPE_DOUBLE;
					} else {
						argbufmode = SN_TYPE_FUNCTION;
					}
				}
			}
		}
	}

	free(argbuf);

	struct sn_generic* gen = gn_stack[0];
	free(gn_stack);
	free(index_stack);

	return gen;
}

struct sn_generic** sn_parse(char* data, unsigned long long size) {
	int br = 0;
	int i;
	int start = 0;
	bool dq = false;
	for(i = 0; i < size; i++) {
		char c = data[i];
		if(c == '"') {
			dq = !dq;
		} else if(dq) {
		} else if(c == '(') {
			if(br == 0) {
				start = i;
			}
			br++;
		} else if(c == ')') {
			br--;
			if(br == 0) {
				char* d = malloc(i - start + 1);
				memcpy(d, data + start, i - start + 1);
				struct sn_generic* gen = sn_expr_parse(d, i - start + 1);
				if(gen != NULL) {
					sn_print_generic(gen);
					sn_generic_free(gen);
				}
				free(d);
			}
		}
	}
	return NULL;
}

void sn_generic_free(struct sn_generic* g) {
	if(g->type == SN_TYPE_STRING) {
		free(g->string);
	} else if(g->type == SN_TYPE_TREE) {
		sn_tree_free(g->tree);
	}
	free(g);
}

void sn_tree_free(struct sn_tree* t) {
	if(t->args != NULL) {
		int i;
		for(i = 0; t->args[i] != NULL; i++) {
			sn_generic_free(t->args[i]);
		}
		free(t->args);
	}
}
