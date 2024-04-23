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
#include "util.h"
#include "../config.h"

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

struct sn_generic* sn_expr_parse(char* data, unsigned long long size){
	int i;
	int br = 0;
	bool dq = false;
	int* br_stack = malloc(sizeof(*br_stack) * STACK_SIZE);
	char** op_stack = malloc(sizeof(*op_stack) * STACK_SIZE);
	struct sn_generic** gn_stack = malloc(sizeof(*gn_stack) * STACK_SIZE);
	for(i = 0; i < STACK_SIZE; i++){
		br_stack[i] = 0;
		op_stack[i] = NULL;
		gn_stack[i] = NULL;
	}
	for(i = 0; i < size; i++){
		char c = data[i];
		if(c == '"'){
			dq = !dq;
		}else if(dq){
		}else if(c == '('){
			br++;
			gn_stack[br - 1] = malloc(sizeof(*gn_stack));
			gn_stack[br - 1]->type = SN_TYPE_TREE;
			gn_stack[br - 1]->tree = malloc(sizeof(*gn_stack[br - 1]->tree));
			gn_stack[br - 1]->tree->op = malloc(sizeof(*gn_stack[br - 1]->tree->op));
			gn_stack[br - 1]->tree->op->type = SN_TYPE_FUNCTION;
			gn_stack[br - 1]->tree->args = malloc(sizeof(*gn_stack[br - 1]->tree->args));
			gn_stack[br - 1]->tree->args[0] = NULL;
			op_stack[br - 1] = malloc(1);
			op_stack[br - 1][0] = 0;
		}else if(c == ')'){
			if(br == 1){
				gn_stack[br - 1]->tree->op->name = sn_strdup(op_stack[br - 1]);
			}
			br_stack[br - 1] = 0;
			if(br_stack[br - 2] > 0){
				int j;
				struct sn_generic** old_args = gn_stack[br - 2]->tree->args;
				for(j = 0; old_args[j] != NULL; j++);
				gn_stack[br - 2]->tree->args = malloc(sizeof(*gn_stack[br - 2]->tree->args) * (j + 2));
				for(j = 0; old_args[j] != NULL; j++){
					gn_stack[br - 2]->tree->args[j] = old_args[j];
				}
				gn_stack[br - 2]->tree->args[j] = gn_stack[br - 1];
				gn_stack[br - 2]->tree->args[j + 1] = NULL;
				free(old_args);
			}
			br--;
		}else if(br > 0){
			if(c == ' '){
				br_stack[br - 1]++;
			}else if(br_stack[br - 1] == 0){
				char cbuf[2];
				cbuf[0] = c;
				cbuf[1] = 0;
				char* tmp = op_stack[br - 1];
				op_stack[br - 1] = sn_strcat(tmp, cbuf);
				free(tmp);
			}
		}
	}
	struct sn_generic* gen = gn_stack[0];
	free(gn_stack);
	free(br_stack);
	free(op_stack);
	return gen;
}

struct sn_generic** sn_parse(char* data, unsigned long long size){
	int br = 0;
	int i;
	int start = 0;
	bool dq = false;
	for(i = 0; i < size; i++){
		char c = data[i];
		if(c == '"'){
			dq = !dq;
		}else if(dq){
		}else if(c == '('){
			if(br == 0){
				start = i;
			}
			br++;
		}else if(c == ')'){
			br--;
			if(br == 0){
				char* d = malloc(i - start + 1);
				memcpy(d, data + start, i - start + 1);
				struct sn_generic* gen = sn_expr_parse(d, i - start + 1);
				if(gen != NULL){
					sn_generic_free(gen);
				}
				free(d);
			}
		}
	}
	return NULL;
}

void sn_generic_free(struct sn_generic* g){
	if(g->type == SN_TYPE_STRING){
		free(g->string);
	}else if(g->type == SN_TYPE_TREE){
		sn_tree_free(g->tree);
	}
	free(g);
}

void sn_tree_free(struct sn_tree* t){
	if(t->op != NULL){
		sn_generic_free(t->op);
	}
	if(t->args != NULL){
		int i;
		for(i = 0; t->args[i] != NULL; i++){
			sn_generic_free(t->args[i]);
		}
		free(t->args);
	}
}
