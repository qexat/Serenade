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

#include "ffi_binding.h"

#include "parser.h"

#include <stdlib.h>
#include <string.h>

#ifdef __MINGW32__
#else
#include <dlfcn.h>
#endif

#include <ffi.h>

ffi_cif cif;

struct ffi_info {
	void* ptr;
	ffi_cif cif;
	ffi_type** argtypes;
};

struct sn_generic* ffi_symbol_handler(struct sn_interpreter* sn, int args, struct sn_generic** gens) {
	struct sn_generic* gen = malloc(sizeof(struct sn_generic));
	gen->type = SN_TYPE_VOID;
	void* ptr = NULL;
	if(args == 3 && gens[1]->type == SN_TYPE_PTR && gens[2]->type == SN_TYPE_STRING) {
		char* sym = malloc(gens[2]->string_length + 1);
		memcpy(sym, gens[2]->string, gens[2]->string_length);
		sym[gens[2]->string_length] = 0;
		ptr = dlsym(gens[1]->ptr, sym);
		free(sym);
	}
	if(ptr != NULL) {
		gen->type = SN_TYPE_PTR;
		gen->ptr = ptr;
	}
	return gen;
}

struct sn_generic* ffi_function_handler(struct sn_interpreter* sn, int args, struct sn_generic** gens) {
	struct sn_generic* gen = malloc(sizeof(struct sn_generic));
	gen->type = SN_TYPE_VOID;
	if(args > 1){
		if(gens[1]->type == SN_TYPE_PTR){
			struct ffi_info* info = malloc(sizeof(struct ffi_info));
			int i;
			for(i = 2; i < args; i++){
			}
		}
	}
	return gen;
}

struct sn_generic* ffi_load_handler(struct sn_interpreter* sn, int args, struct sn_generic** gens) {
	struct sn_generic* gen = malloc(sizeof(struct sn_generic));
	gen->type = SN_TYPE_VOID;
	void* lib = NULL;
	if(args == 2 && gens[1]->type == SN_TYPE_STRING) {
		char* path = malloc(gens[1]->string_length + 1);
		memcpy(path, gens[1]->string, gens[1]->string_length);
		path[gens[1]->string_length] = 0;
		lib = dlopen(path, RTLD_LAZY);
		free(path);
	}
	if(lib != NULL) {
		gen->type = SN_TYPE_PTR;
		gen->ptr = lib;
	}
	return gen;
}

void ffi_init(struct sn_interpreter* sn) {
	struct sn_generic* gen = malloc(sizeof(struct sn_generic));
	gen->type = SN_TYPE_DOUBLE;
	gen->number = 1;
	sn_set_variable(sn, "ffi-loaded", gen);
	sn_set_handler(sn, "ffi-symbol", ffi_symbol_handler);
	sn_set_handler(sn, "ffi-load", ffi_load_handler);
	sn_set_handler(sn, "ffi-function", ffi_function_handler);
}
