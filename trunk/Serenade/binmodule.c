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

#include "binmodule.h"

#include "interpreter.h"
#include "util.h"

#if defined(__MINGW32__) || defined(_MSC_VER)
#include <libloaderapi.h>
#else
#include <dlfcn.h>
#endif

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct sn_generic* binmodule_load_handler(struct sn_interpreter* sn, int argc, struct sn_generic** args) {
	struct sn_generic* gen = malloc(sizeof(struct sn_generic));
	gen->type = SN_TYPE_DOUBLE;
	gen->number = -1;
	void* lib = NULL;
	char* name = sn_strdup("binmodule");
	if((argc == 3 || argc == 2) && args[1]->type == SN_TYPE_STRING) {
		if((argc == 3 ? (args[2]->type == SN_TYPE_STRING) : true)) {
			char* path = malloc(args[1]->string_length + 1);
			memcpy(path, args[1]->string, args[1]->string_length);
			path[args[1]->string_length] = 0;
#if defined(__MINGW32__) || defined(_MSC_VER)
			lib = LoadLibraryA(path);
#else
			lib = dlopen(path, RTLD_LAZY);
#endif
			free(path);
			if(argc == 3) {
				free(name);
				name = malloc(args[2]->string_length + 1);
				memcpy(name, args[2]->string, args[2]->string_length);
				name[args[2]->string_length] = 0;
			}
		}
	}
	if(lib != NULL) {
		struct sn_binmodule_config* conf = malloc(sizeof(struct sn_binmodule_config));
		conf->interpreter = sn;
		conf->set_variable = sn_set_variable;
		conf->set_handler = sn_set_handler;
		char* symbol_name = sn_strcat(name, "_init");
		int (*loadfunc)(struct sn_binmodule_config*);
#if defined(__MINGW32__) || defined(_MSC_VER)
		loadfunc = (int (*)(struct sn_binmodule_config*))GetProcAddress(lib, symbol_name);
#else
		loadfunc = (int (*)(struct sn_binmodule_config*))dlsym(lib, symbol_name);
#endif
		if(loadfunc == NULL) {
			gen->number = -1;
		} else {
			gen->number = loadfunc(conf);
		}
		free(symbol_name);
		free(conf);
	}
	free(name);
	return gen;
}

void sn_binmodule_init(struct sn_interpreter* sn) {
	struct sn_generic* gen = malloc(sizeof(struct sn_generic));
	gen->type = SN_TYPE_DOUBLE;
	gen->number = 1;
	sn_set_variable(sn, "binmodule-loaded", gen);
	sn_set_handler(sn, "binmodule-load", binmodule_load_handler);
}
