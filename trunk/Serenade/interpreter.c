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

#include "interpreter.h"

#include "../config.h"
#include "run.h"
#include "util.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct sn_generic* math_handler(struct sn_interpreter* sn, int args, struct sn_generic** gens) {
	struct sn_generic* gen = malloc(sizeof(struct sn_generic));
	gen->type = SN_TYPE_DOUBLE;
	gen->number = 0;
	if(strcmp(gens[0]->name, "*") == 0) gen->number = 1;
	int i;
	for(i = 1; i < args; i++) {
		if(gens[i]->type == SN_TYPE_DOUBLE) {
			if(strcmp(gens[0]->name, "*") == 0) {
				gen->number *= gens[i]->number;
			} else if(strcmp(gens[0]->name, "+") == 0) {
				gen->number += gens[i]->number;
			} else if(strcmp(gens[0]->name, "-") == 0) {
				if(i == 1) {
					gen->number += gens[i]->number;
				} else {
					gen->number -= gens[i]->number;
				}
			} else if(strcmp(gens[0]->name, "/") == 0) {
				if(i == 1) {
					gen->number += gens[i]->number;
				} else {
					gen->number /= gens[i]->number;
				}
			}
		}
	}
	return gen;
}

struct sn_generic* print_handler(struct sn_interpreter* sn, int args, struct sn_generic** gens) {
	struct sn_generic* gen = malloc(sizeof(struct sn_generic));
	int i;
	for(i = 1; i < args; i++) {
		if(i > 1) printf(" ");
		fflush(stdout);
		sn_print_to(stdout, gens[i]);
		fflush(stdout);
	}
	printf("\n");
	gen->type = SN_TYPE_VOID;
	return gen;
}

struct sn_generic* defvar_handler(struct sn_interpreter* sn, int args, struct sn_generic** gens) {
	struct sn_generic* gen = malloc(sizeof(struct sn_generic));
	int i;
	if(args > 1) {
		for(i = 1; i < args; i += 2) {
			if(gens[i + 1] == NULL) break;
			if(gens[i]->type == SN_TYPE_STRING) {
				char* str = malloc(gens[i]->string_length + 1);
				gens[i + 1]->used = true;
				memcpy(str, gens[i]->string, gens[i]->string_length);
				str[gens[i]->string_length] = 0;
				sn_set_variable(sn, str, gens[i + 1]);
				free(str);
			}
		}
	}
	gen->type = SN_TYPE_VOID;
	return gen;
}

struct sn_generic* eval_handler(struct sn_interpreter* sn, int args, struct sn_generic** gens) {
	int i;
	int result = 0;
	int r = 0;
	for(i = 1; i < args; i++) {
		if(gens[i]->type == SN_TYPE_STRING) {
			r = sn_eval(sn, gens[i]->string, gens[i]->string_length);
			if(r != 0) result = 1;
		}
	}
	struct sn_generic* gen = malloc(sizeof(struct sn_generic));
	gen->type = SN_TYPE_DOUBLE;
	gen->number = result;
	return gen;
}

void sn_stdlib_init(struct sn_interpreter* sn) {
	sn_set_handler(sn, "+", math_handler);
	sn_set_handler(sn, "-", math_handler);
	sn_set_handler(sn, "*", math_handler);
	sn_set_handler(sn, "/", math_handler);
	sn_set_handler(sn, "print", print_handler);
	sn_set_handler(sn, "eval", eval_handler);
	sn_set_handler(sn, "define-var", defvar_handler);
}

void sn_ffi_init(struct sn_interpreter* sn) {
#ifdef HAS_FFI_SUPPORT
	ffi_init(sn);
#endif
}

struct sn_interpreter* sn_create_interpreter(void) {
	struct sn_interpreter* sn = malloc(sizeof(struct sn_interpreter));
	sn->variables = malloc(sizeof(struct sn_interpreter_kv*));
	sn->variables[0] = NULL;
	sn->generics = malloc(sizeof(struct sn_generic**));
	sn->generics[0] = NULL;

	return sn;
}

void sn_interpreter_free(struct sn_interpreter* sn) {
	int i;
	for(i = 0; sn->variables[i] != NULL; i++) {
		free(sn->variables[i]->key);
		if(sn->variables[i]->value != NULL) sn_generic_free(sn->variables[i]->value);
		free(sn->variables[i]);
	}
	free(sn->variables);
	for(i = 0; sn->generics[i] != NULL; i++) {
		int j;
		for(j = 0; sn->generics[i][j] != NULL; j++) sn_generic_free(sn->generics[i][j]);
		free(sn->generics[i]);
	}
	free(sn->generics);
	free(sn);
}

struct sn_interpreter_kv* sn_set_variable(struct sn_interpreter* sn, const char* name, struct sn_generic* gen) {
	int i;
	bool replaced = false;
	for(i = 0; sn->variables[i] != NULL; i++) {
		if(strcmp(sn->variables[i]->key, name) == 0) {
			sn->variables[i]->value = gen;
			replaced = true;
			return sn->variables[i];
		}
	}
	if(!replaced) {
		struct sn_interpreter_kv** oldvariables = sn->variables;
		for(i = 0; oldvariables[i] != NULL; i++)
			;
		sn->variables = malloc(sizeof(struct sn_interpreter_kv*) * (i + 2));
		for(i = 0; oldvariables[i] != NULL; i++) {
			sn->variables[i] = oldvariables[i];
		}
		sn->variables[i] = malloc(sizeof(struct sn_generic));
		sn->variables[i]->key = sn_strdup(name);
		sn->variables[i]->value = gen;
		sn->variables[i]->handler = NULL;
		if(gen->type == SN_TYPE_FUNCTION) sn->variables[i]->handler = gen->handler;
		sn->variables[i + 1] = NULL;
		return sn->variables[i];
	}
}

struct sn_interpreter_kv* sn_set_handler(struct sn_interpreter* sn, const char* name, struct sn_generic* (*handler)(struct sn_interpreter* sn, int, struct sn_generic**)) {
	int i;
	bool replaced = false;
	for(i = 0; sn->variables[i] != NULL; i++) {
		if(strcmp(sn->variables[i]->key, name) == 0) {
			sn->variables[i]->handler = handler;
			replaced = true;
			return sn->variables[i];
		}
	}
	if(!replaced) {
		struct sn_interpreter_kv** oldvariables = sn->variables;
		for(i = 0; oldvariables[i] != NULL; i++)
			;
		sn->variables = malloc(sizeof(struct sn_interpreter_kv*) * (i + 2));
		for(i = 0; oldvariables[i] != NULL; i++) {
			sn->variables[i] = oldvariables[i];
		}
		sn->variables[i] = malloc(sizeof(struct sn_generic));
		sn->variables[i]->key = sn_strdup(name);
		sn->variables[i]->value = NULL;
		sn->variables[i]->handler = handler;
		sn->variables[i + 1] = NULL;
		return sn->variables[i];
	}
}

int sn_eval(struct sn_interpreter* sn, char* data, unsigned long long len) {
	struct sn_generic** gens = sn_parse(data, len);
	int r = 0;
	if(gens != NULL) {
		int i;
		for(i = 0; gens[i] != NULL; i++) {
			if(r == 0) {
				sn->local_variables = NULL;
				if(sn_run(sn, gens[i]) != 0) {
					r = 1;
				}
			}
		}
		int j;
		struct sn_generic*** oldgens = sn->generics;
		for(j = 0; oldgens[j] != NULL; j++)
			;
		sn->generics = malloc(sizeof(struct sn_generic**) * (j + 2));
		for(j = 0; oldgens[j] != NULL; j++) {
			sn->generics[j] = oldgens[j];
		}
		sn->generics[j] = gens;
		sn->generics[j + 1] = NULL;
		free(oldgens);
	}
	return r;
}
