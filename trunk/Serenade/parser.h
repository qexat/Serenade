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

#ifndef __SERENADE_PARSER_H__
#define __SERENADE_PARSER_H__

#include <stdbool.h>

#include "interpreter.h"

enum types { SN_TYPE_DOUBLE = 0, SN_TYPE_STRING, SN_TYPE_TREE, SN_TYPE_PTR, SN_TYPE_FUNCTION, SN_TYPE_VOID, SN_TYPE_VARIABLE };

struct sn_interpreter;

struct sn_generic {
	int type;
	double number;
	char* string;
	unsigned long long string_length;
	void* ptr;
	void* funcptr;
	char* name;
	struct sn_tree* tree;
	bool used;
	struct sn_generic* (*handler)(struct sn_interpreter* sn, int, struct sn_generic**);
};

struct sn_tree {
	struct sn_generic** args;
};

struct sn_generic** sn_parse(char* data, unsigned long long size);
void sn_generic_free(struct sn_generic* g);
struct sn_generic* sn_generic_dup(struct sn_generic* g);
void sn_tree_free(struct sn_tree* t);

#endif
