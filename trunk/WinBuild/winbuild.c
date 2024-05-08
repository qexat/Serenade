/* $Id$ */
#include "dirent.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
#include <io.h>
#define access _access
#else
#include <unistd.h>
#endif

char* dynstrdup(const char* str1) {
	char* str = malloc(strlen(str1) + 1);
	memcpy(str, str1, strlen(str1));
	str[strlen(str1)] = 0;
	return str;
}

char* dynstrcat(const char* str1, const char* str2) {
	char* str = malloc(strlen(str1) + strlen(str2) + 1);
	memcpy(str, str1, strlen(str1));
	memcpy(str + strlen(str1), str2, strlen(str2));
	str[strlen(str1) + strlen(str2)] = 0;
	return str;
}

char* dynstrcat3(const char* str1, const char* str2, const char* str3) {
	char* tmp = dynstrcat(str1, str2);
	char* str = dynstrcat(tmp, str3);
	free(tmp);
	return str;
}

int main() {
#ifdef _MSC_VER
	if(access("..\\Tool", 0) != 0) {
#else
	if(access("..\\Tool", F_OK) != 0) {
#endif
		fprintf(stderr, "Run me from the WinBuild directory\n");
		return 1;
	}
#ifdef _MSC_VER
	if(access("..\\Serenade", 0) != 0) {
#else
	if(access("..\\Serenade", F_OK) != 0) {
#endif
		fprintf(stderr, "Run me from the WinBuild directory\n");
		return 1;
	}
	printf("WinBuild - Serenade build script for MSVC %d\n", _MSC_FULL_VER);
	system("cl /nologo /Fe:..\\Tool\\configgen.exe ..\\Tool\\configgen.c");
	FILE* f = fopen("..\\config.h", "r");
	if(f != NULL) {
		printf("config.h already exists, remove it to generate it again.\n");
		fclose(f);
	} else {
		system("..\\Tool\\configgen.exe ..\\config.h");
	}
	system("cl /nologo /Fe:..\\Tool\\config.exe ..\\Tool\\config.c");
	system("echo main.o >buildobjs");
	system("..\\Tool\\config.exe reqobjs Windows >>buildobjs");
	system("..\\Tool\\config.exe objs Windows >>buildobjs");
	f = fopen("buildobjs", "r");
	if(f != NULL) {
		chdir("..\\Serenade");
		char* cbuf = malloc(2);
		cbuf[1] = 0;
		char* buf = malloc(1);
		char* objs = malloc(1);
		objs[0] = 0;
		buf[0] = 0;
		while(1) {
			fread(cbuf, 1, 1, f);
			if(feof(f)) break;
			if(cbuf[0] == '\n' || cbuf[0] == '\r' || cbuf[0] == ' ') {
				if(strlen(buf) > 0) {
					char* objstmp = objs;
					objs = dynstrcat3(objstmp, " ", buf);
					char* cfile = dynstrdup(buf);
					cfile[strlen(cfile) - 1] = 'c';
					char* argtmp1 = dynstrcat("cl /nologo /c /D _AMD64_ /Fo:", buf);
					char* argtmp2 = dynstrcat(argtmp1, " ");
					char* arg = dynstrcat(argtmp2, cfile);
					system(arg);
					free(arg);
					free(argtmp1);
					free(argtmp2);
					free(cfile);
				}
				free(buf);
				buf = malloc(1);
				buf[0] = 0;
			} else {
				char* tmp = buf;
				buf = dynstrcat(tmp, cbuf);
				free(tmp);
			}
		}
		free(buf);

		char* cmd = dynstrcat("cl /nologo /Fe:serenade.exe ", objs);
		system(cmd);
		free(cmd);

		free(objs);

		chdir("WinBuild");
	}
	fclose(f);
}
