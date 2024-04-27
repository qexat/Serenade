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
#include "util.h"

#include <stdlib.h>
#include <string.h>

#ifdef __MINGW32__
#include <libloaderapi.h>
#else
#include <dlfcn.h>
#endif

#include <ffi.h>

ffi_cif cif;

struct ffi_info {
	void* ptr;
	int argc;
	char** argtypes;
	ffi_cif cif;
	ffi_type** args;
};

struct sn_generic* ffi_symbol_handler(struct sn_interpreter* sn, int args, struct sn_generic** gens) {
	struct sn_generic* gen = malloc(sizeof(struct sn_generic));
	gen->type = SN_TYPE_VOID;
	void* ptr = NULL;
	if(args == 3 && gens[1]->type == SN_TYPE_PTR && gens[2]->type == SN_TYPE_STRING) {
		char* sym = malloc(gens[2]->string_length + 1);
		memcpy(sym, gens[2]->string, gens[2]->string_length);
		sym[gens[2]->string_length] = 0;
#ifdef __MINGW32__
		ptr = GetProcAddress(gens[1]->ptr, sym);
#else
		ptr = dlsym(gens[1]->ptr, sym);
#endif
		free(sym);
	}
	if(ptr != NULL) {
		gen->type = SN_TYPE_PTR;
		gen->ptr = ptr;
	}
	return gen;
}

struct sn_generic* function_caller_handler(struct sn_interpreter* sn, int args, struct sn_generic** gens) {
	struct sn_generic* gen = malloc(sizeof(struct sn_generic));
	gen->type = SN_TYPE_VOID;
	struct ffi_info* info = (struct ffi_info*)gens[0]->ptr;
	void** fargs = malloc(sizeof(void*) * (info->argc));
	if(info->argc > 0) {
		int i;
		for(i = 0; i < info->argc; i++) {
			void* ptr = NULL;
			if(strcmp(info->argtypes[i + 1], "integer") == 0) {
				int* data = malloc(sizeof(int));
				*data = gens[i + 1]->number;
				ptr = data;
			} else if(strcmp(info->argtypes[i + 1], "double") == 0) {
				double* data = malloc(sizeof(double));
				*data = gens[i + 1]->number;
				ptr = data;
			} else if(strcmp(info->argtypes[i + 1], "float") == 0) {
				float* data = malloc(sizeof(float));
				*data = gens[i + 1]->number;
				ptr = data;
			} else if(strcmp(info->argtypes[i + 1], "byte") == 0) {
				char* data = malloc(sizeof(char));
				*data = gens[i + 1]->number;
				ptr = data;
			} else if(strcmp(info->argtypes[i + 1], "string") == 0) {
				char** data = malloc(sizeof(char*));
				*data = malloc(gens[i + 1]->string_length + 1);
				memcpy(*data, gens[i + 1]->string, gens[i + 1]->string_length);
				(*data)[gens[i + 1]->string_length] = 0;
				ptr = data;
			} else if(strcmp(info->argtypes[i + 1], "pointer") == 0) {
				ptr = &gens[i + 1]->ptr;
			}
			fargs[i] = ptr;
		}
	}
	void* result;
	ffi_call(&info->cif, FFI_FN(info->ptr), &result, fargs);
	if(strcmp(info->argtypes[0], "pointer") == 0) {
		gen->type = SN_TYPE_PTR;
		gen->ptr = result;
	} else if(strcmp(info->argtypes[0], "double") == 0) {
		gen->type = SN_TYPE_DOUBLE;
		gen->number = *(double*)&result;
	} else if(strcmp(info->argtypes[0], "float") == 0) {
		gen->type = SN_TYPE_DOUBLE;
		gen->number = *(float*)&result;
	} else if(strcmp(info->argtypes[0], "integer") == 0) {
		gen->type = SN_TYPE_DOUBLE;
		gen->number = *(int*)&result;
	} else if(strcmp(info->argtypes[0], "byte") == 0) {
		gen->type = SN_TYPE_DOUBLE;
		gen->number = *(char*)&result;
	} else if(strcmp(info->argtypes[0], "string") == 0) {
		gen->type = SN_TYPE_STRING;
		gen->string = sn_strdup(*(char**)&result);
		gen->string_length = strlen(gen->string);
	}

	int i;
	if(fargs != NULL){
		for(i = 0; i < info->argc; i++) {
			if(strcmp(info->argtypes[i + 1], "string") == 0) {
				free(*((void**)fargs[i]));
			}
			if(strcmp(info->argtypes[i + 1], "pointer") != 0) free(fargs[i]);
		}
		free(fargs);
	}
	return gen;
}

struct sn_generic* ffi_function_handler(struct sn_interpreter* sn, int args, struct sn_generic** gens) {
	struct sn_generic* gen = malloc(sizeof(struct sn_generic));
	gen->type = SN_TYPE_VOID;
	if(args > 2) {
		if(gens[1]->type == SN_TYPE_PTR && gens[2]->type == SN_TYPE_STRING) {
			struct ffi_info* info = malloc(sizeof(struct ffi_info));
			int i;
			gen->type = SN_TYPE_FUNCTION;
			gen->handler = function_caller_handler;
			gen->name = NULL;

			info->ptr = gens[1]->ptr;
			info->argc = args - 3;
			info->args = malloc(sizeof(ffi_type*) * (args - 2));
			info->argtypes = malloc(sizeof(char*) * (args - 2));

			ffi_type* ret = &ffi_type_void;

			for(i = 2; i < args; i++) {
				ffi_type* assign = &ffi_type_void;
				char* typ = malloc(gens[i]->string_length + 1);
				typ[gens[i]->string_length] = 0;
				memcpy(typ, gens[i]->string, gens[i]->string_length);

				if(strcmp(typ, "void") == 0) {
					assign = &ffi_type_void;
				} else if(strcmp(typ, "pointer") == 0) {
					assign = &ffi_type_pointer;
				} else if(strcmp(typ, "integer") == 0) {
					assign = &ffi_type_sint;
				} else if(strcmp(typ, "double") == 0) {
					assign = &ffi_type_double;
				} else if(strcmp(typ, "string") == 0) {
					assign = &ffi_type_pointer;
				} else if(strcmp(typ, "byte") == 0) {
					assign = &ffi_type_schar;
				} else if(strcmp(typ, "float") == 0) {
					assign = &ffi_type_float;
				}

				info->argtypes[i - 2] = typ;

				if(i == 2) {
					ret = assign;
				} else {
					info->args[i - 3] = assign;
				}
			}

			ffi_prep_cif(&info->cif, FFI_DEFAULT_ABI, args - 3, ret, info->args);
			gen->ptr = info;
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
#ifdef __MINGW32__
		lib = LoadLibraryA(path);
#else
		lib = dlopen(path, RTLD_LAZY);
#endif
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
