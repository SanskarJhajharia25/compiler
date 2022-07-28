#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenDef.h"

#define BUFFER_SIZE 50
#define MAX_LEXEME_SIZE 25
#define MAX_COMMENT_SIZE 2048

typedef struct tokenInfo {
	int type;
	char lex[MAX_LEXEME_SIZE+1];
	int line_number;
} tokenInfo;

typedef struct twinBuffer{
	char buff1[BUFFER_SIZE];
	char buff2[BUFFER_SIZE];
	int active;
	int pos;
} * twinBuffer;
