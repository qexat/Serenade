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

#include "run.h"

#include "util.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct sn_generic* _sn_run(struct sn_interpreter* sn, struct sn_generic* gen) {
	if(gen->type == SN_TYPE_TREE) {
		struct sn_generic* op = gen->tree->args[0];
		if(op->type == SN_TYPE_TREE) {
			op = _sn_run(sn, op);
		}
		int j;
		for(j = 0; gen->tree->args[j]; j++)
			;
		struct sn_generic** args = malloc(sizeof(struct sn_generic*) * (j));
		int argc = j;
		for(j = 0; gen->tree->args[j]; j++) {
			args[j] = _sn_run(sn, gen->tree->args[j]);
		}

		struct sn_generic* r = malloc(sizeof(struct sn_generic));
		r->type = SN_TYPE_VOID;

		if(op->type != SN_TYPE_FUNCTION) {
			fprintf(stderr, "Cannot call non-function (%d)\n", op->type);
			free(args);
			return NULL;
		} else {
			for(j = 0; sn->variables[j] != NULL; j++) {
				if(strcmp(sn->variables[j]->key, op->name) == 0) {
					struct sn_generic* op_result = NULL;
					if(sn->variables[j]->handler != NULL) {
						op_result = sn->variables[j]->handler(sn, argc, args);
					} else {
						struct sn_generic* build = malloc(sizeof(struct sn_generic));
						build->tree = malloc(sizeof(struct sn_tree));
						build->tree->args = args;
						build->tree->args[0] = sn->variables[j]->value;
						op_result = _sn_run(sn, build);
						free(build->tree);
					}
					if(op_result != NULL) {
						free(r);
						r = op_result;
					}
					break;
				}
			}
		}
		free(args);
		return r;
	} else {
		return gen;
	}
	return NULL;
}

int sn_run(struct sn_interpreter* sn, struct sn_generic* gen) {
	struct sn_generic* rgen = _sn_run(sn, gen);
	if(rgen == NULL) return 1;
	return 0;
}
