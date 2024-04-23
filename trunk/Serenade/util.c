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

#include "util.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char* sn_strcat(const char* str1, const char* str2){
	char* str = malloc(strlen(str1) + strlen(str2) + 1);
	memcpy(str, str1, strlen(str1));
	memcpy(str + strlen(str1), str2, strlen(str2));
	str[strlen(str1) + strlen(str2)] = 0;
	return str;
}

char* sn_strdup(const char* str1){
	char* str = malloc(strlen(str1) + 1);
	memcpy(str, str1, strlen(str1));
	str[strlen(str1)] = 0;
	return str;
}

void _sn_print_generic(struct sn_generic* gen, int n){
	int i;
	for(i = 0; i < n; i++) fprintf(stderr, "    ");
	if(gen->type == SN_TYPE_TREE){
		fprintf(stderr, "%s\n", gen->tree->op->name);
		if(gen->tree->args != NULL){
			for(i = 0; gen->tree->args[i] != NULL; i++){
				_sn_print_generic(gen->tree->args[i], n + 1);
			}
		}
	}else if(gen->type == SN_TYPE_DOUBLE){
		fprintf(stderr, "%f\n", gen->number);
	}else if(gen->type == SN_TYPE_STRING){
		fwrite(gen->string, 1, gen->string_length, stderr);
		fprintf(stderr, "\n");
	}
}

void sn_print_generic(struct sn_generic* gen){
	_sn_print_generic(gen, 0);
}
