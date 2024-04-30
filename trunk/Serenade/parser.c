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

struct sn_generic* sn_generic_dup(struct sn_generic* g) {
	if(g == NULL) return NULL;
	struct sn_generic* r = malloc(sizeof(struct sn_generic));
	r->type = g->type;
	if(r->type == SN_TYPE_STRING) {
		r->string = malloc(g->string_length);
		r->string_length = g->string_length;
		memcpy(r->string, g->string, r->string_length);
	} else if(r->type == SN_TYPE_DOUBLE) {
		r->number = g->number;
	} else if(r->type == SN_TYPE_FUNCTION) {
		r->name = sn_strdup(g->name);
	}
	r->ptr = g->ptr;

	return r;
}

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
	if(mode == SN_TYPE_STRING) {
		newgen->string = sn_strdup(buf);
		newgen->string_length = strlen(buf);
	} else if(mode == SN_TYPE_FUNCTION || mode == SN_TYPE_VARIABLE) {
		newgen->name = sn_strdup(buf);
	} else if(mode == SN_TYPE_DOUBLE) {
		newgen->number = atof(buf);
	}
	push_stack_generic(gen, newgen);
}

struct sn_generic* sn_expr_parse(char* data, unsigned long long size) {
	int i;
	int br = 0;
	bool dq = false;
	struct sn_generic** gn_stack = malloc(sizeof(*gn_stack) * PARSER_STACK_SIZE);
	int* index_stack = malloc(sizeof(int) * PARSER_STACK_SIZE);
	char* argbuf = malloc(1);
	argbuf[0] = 0;
	gn_stack[0] = NULL;
	int argbufmode = SN_TYPE_VOID;
	for(i = 0; i < size; i++) {
		char c = data[i];
		if(c == '\r') {
		} else if(c == '"') {
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
			gn_stack[br]->used = false;
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
			if(c == ' ' || c == '\n' || c == '\t') {
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
					} else if(index_stack[br - 1] == 0) {
						argbufmode = SN_TYPE_FUNCTION;
					} else {
						argbufmode = SN_TYPE_VARIABLE;
					}
				}
			}
		}
	}

	free(argbuf);

	struct sn_generic* gen = gn_stack[0];
	free(index_stack);

	return gen;
}

struct sn_generic** sn_parse(char* data, unsigned long long size) {
	int br = 0;
	int i;
	int start = 0;
	bool dq = false;
	struct sn_generic** gens = malloc(sizeof(struct sn_generic*));
	gens[0] = NULL;
	for(i = 0; i < size; i++) {
		char c = data[i];
		if(c == ';') {
			for(; i < size && data[i] != '\n'; i++)
				;
		} else if(c == '"') {
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
					int j;
					struct sn_generic** old_gens = gens;
					for(j = 0; old_gens[j] != NULL; j++)
						;
					gens = malloc(sizeof(struct sn_generic*) * (j + 2));
					for(j = 0; old_gens[j] != NULL; j++) {
						gens[j] = old_gens[j];
					}
					gens[j] = gen;
					gens[j + 1] = NULL;
				}
				free(d);
			} else if(br < 0) {
				return gens;
			}
		}
	}
	return gens;
}

void sn_generic_free(struct sn_interpreter* sn, struct sn_generic* g) {
	printf("freed type %d @ %x \n:", g->type, g);
	sn_print_to(stdout, g);
	printf("\n");
	if(g->type == SN_TYPE_STRING) {
		free(g->string);
	} else if(g->type == SN_TYPE_TREE) {
		sn_tree_free(sn, g->tree);
	} else if(g->type == SN_TYPE_FUNCTION || g->type == SN_TYPE_VARIABLE) {
		free(g->name);
	}
	free(g);
	if(sn == NULL) return;
	int i;
	for(i = 0; sn->generics[i] != NULL; i++);
	struct sn_generic*** gens = malloc(sizeof(struct sn_generic**) * (i + 1));
	for(i = 0; sn->generics[i] != NULL; i++) gens[i] = sn->generics[i];
	gens[i] = NULL;
	for(i = 0; sn->generics[i] != NULL; i++){
		int j;
		for(j = 0; sn->generics[i][j] != NULL; j++){
			struct sn_generic** oldgens = gens[i];
			int k;
			int count = 0;
			int matched = 0;
			for(k = 0; oldgens[k] != NULL; k++){
				if(oldgens[k] != g){
					count++;
				}else{
					matched++;
				}
			}
			if(matched == 0) continue;
			gens[i] = malloc(sizeof(struct sn_generic*) * (count + 1));
			count = 0;
			for(k = 0; oldgens[k] != NULL; k++){
				if(oldgens[k] != g){
					gens[i][count] = oldgens[k];	
					count++;
				}
			}
			gens[i][count] = NULL;
			free(oldgens[k]);
		}
	}
	free(sn->generics);
	sn->generics = gens;

	if(sn->variables != NULL){
		for(i = 0; sn->variables[i] != NULL; i++);
		struct sn_interpreter_kv** kvs = malloc(sizeof(struct sn_interpreter_kv*) * (i + 1));
		for(i = 0; sn->variables[i] != NULL; i++) kvs[i] = sn->variables[i];
		kvs[i] = NULL;
		for(i = 0; sn->variables[i] != NULL; i++){
			struct sn_interpreter_kv** oldkvs = kvs;
			int j;
			int count = 0;
			int matched = 0;
			for(j = 0; oldkvs[j] != NULL; j++) count++;
			kvs = malloc(sizeof(struct sn_interpreter_kv*) * (count + 1));
			count = 0;
			for(j = 0; oldkvs[j] != NULL; j++){
				if(oldkvs[j] == (void*)1) continue;
				kvs[count] = oldkvs[j];
				if(oldkvs[j]->value == g){
					kvs[count]->value = NULL;
				}
				count++;
			}
			kvs[count] = NULL;
			free(oldkvs);
		}
		sn->variables = kvs;
	}
}

void sn_tree_free(struct sn_interpreter* sn, struct sn_tree* t) {
	if(t->args != NULL) {
		int i;
		for(i = 0; t->args[i] != NULL; i++) {
			sn_generic_free(sn, t->args[i]);
		}
		free(t->args);
	}
}
