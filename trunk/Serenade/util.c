/* $Id$ */
/* --- START LICENSE --- */
/* --- END LICENSE --- */

#include "util.h"

#include <string.h>
#include <stdlib.h>

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
