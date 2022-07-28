#include "lexer.h"

char *tokens[59] = {"", "TK_ASSIGNOP", "TK_COMMENT", "TK_FIELDID", "TK_ID", "TK_NUM", "TK_RNUM", "TK_ERROR", "TK_FUNID", "TK_RUID", "TK_WITH", "TK_PARAMETERS", "TK_END", "TK_WHILE", "TK_UNION", "TK_ENDUNION", "TK_DEFINETYPE", "TK_AS", "TK_TYPE", "TK_MAIN", "TK_GLOBAL", "TK_PARAMETER", "TK_LIST", "TK_SQL", "TK_SQR", "TK_INPUT", "TK_OUTPUT", "TK_INT", "TK_REAL", "TK_COMMA", "TK_SEM", "TK_COLON", "TK_DOT", "TK_ENDWHILE", "TK_OP", "TK_CL", "TK_IF", "TK_THEN", "TK_ENDIF", "TK_READ", "TK_WRITE", "TK_RETURN", "TK_PLUS", "TK_MINUS", "TK_MUL", "TK_DIV", "TK_CALL", "TK_RECORD", "TK_ENDRECORD", "TK_ELSE", "TK_AND", "TK_OR", "TK_NOT", "TK_LT", "TK_LE", "TK_EQ", "TK_GT", "TK_GE", "TK_NE"};
char ch;
char lexeme[MAX_COMMENT_SIZE+1];

int linecount;
int skip;
int final;
int state;
int error;
int i;

tokenInfo tk;
twinBuffer tb;
FILE *fp;

void fillBuffer(twinBuffer B)
{
	int len;
	if (B->active == 1)
	{
		len = fread(B->buff1, 1, BUFFER_SIZE, fp);
		if (len < BUFFER_SIZE)
			B->buff1[len] = EOF;
	}
	else
	{
		len = fread(B->buff2, 1, BUFFER_SIZE, fp);
		if (len < BUFFER_SIZE)
			B->buff2[len] = EOF;
	}
}

void readChar(twinBuffer B)
{
	if (skip)
	{
		skip = 0;
		return;
	}

	ch = B->active ? B->buff2[B->pos] : B->buff1[B->pos];
	B->pos++;

	if (B->pos == BUFFER_SIZE / 2)
		fillBuffer(B);

	else if (B->pos == BUFFER_SIZE)
	{
		B->pos = 0;
		B->active = 1 - B->active;
	}
}

tokenInfo getNextToken(twinBuffer B)
{
	do
	{
		readChar(B);
		lexeme[i++] = ch;

		switch (state)
		{
		case 0:
			switch (ch)
			{
			case '\n':
				linecount++;

			case ' ':
			case '\r':
			case '\t':
				i = 0;

			case EOF:
				break;

			case 'a':
			case 'e' ... 'z':
				state = 1;
				break;

			case 'b' ... 'd':
				state = 2;
				break;

			case '0' ... '9':
				state = 5;
				break;

			case '_':
				state = 12;
				break;

			case '#':
				state = 15;
				break;

			case '@':
				state = 17;
				break;

			case '&':
				state = 19;
				break;

			case '!':
				state = 21;
				break;

			case '=':
				state = 22;
				break;

			case '>':
				state = 23;
				break;

			case '<':
				state = 24;
				break;

			case '~':
				final = 1;
				tk.type = TK_NOT;
				break;

			case ',':
				final = 1;
				tk.type = TK_COMMA;
				break;

			case ')':
				final = 1;
				tk.type = TK_CL;
				break;

			case '(':
				final = 1;
				tk.type = TK_OP;
				break;

			case ';':
				final = 1;
				tk.type = TK_SEM;
				break;

			case ':':
				final = 1;
				tk.type = TK_COLON;
				break;

			case '.':
				final = 1;
				tk.type = TK_DOT;
				break;

			case ']':
				final = 1;
				tk.type = TK_SQR;
				break;

			case '[':
				final = 1;
				tk.type = TK_SQL;
				break;

			case '/':
				final = 1;
				tk.type = TK_DIV;
				break;

			case '*':
				final = 1;
				tk.type = TK_MUL;
				break;

			case '-':
				final = 1;
				tk.type = TK_MINUS;
				break;

			case '+':
				final = 1;
				tk.type = TK_PLUS;
				break;

			case '%':
				readChar(B);
				while (ch != EOF && ch != '\n')
					readChar(B);
				skip = 1;
				tk.type = TK_COMMENT;
				break;

			default:
				error = 1;
				break;
			}
			break;

		case 1:
			switch (ch)
			{
			case 'a' ... 'z':
				break;

			default:
				skip = 1;
				lexeme[i - 1] = '\0';
				tk.type = lookup(lexeme);
				break;
			}
			break;

		case 2:
			switch (ch)
			{
			case 'a' ... 'z':
				state = 1;
				break;

			case '2' ... '7':
				state = 3;
				break;

			default:
				tk.type = TK_ID;
				skip = 1;
				break;
			}
			break;

		case 3:
			switch (ch)
			{
			case '2' ... '7':
				state = 4;
				break;

			case 'b' ... 'd':
				break;

			default:
				tk.type = TK_ID;
				skip = 1;
				break;
			}
			break;

		case 4:
			switch (ch)
			{
			case '2' ... '7':
				break;

			default:
				tk.type = TK_ID;
				skip = 1;
				break;
			}
			break;

		case 5:
			switch (ch)
			{
			case '0' ... '9':
				break;

			case '.':
				state = 6;
				break;

			default:
				tk.type = TK_NUM;
				skip = 1;
				break;
			}
			break;

		case 6:
			switch (ch)
			{
			case '0' ... '9':
				state = 7;
				break;

			default:
				error = 1;
				break;
			}
			break;

		case 7:
			switch (ch)
			{
			case '0' ... '9':
				state = 8;
				break;

			default:
				error = 1;
				break;
			}
			break;

		case 8:
			switch (ch)
			{
			case 'E':
				state = 9;
				break;

			default:
				tk.type = TK_RNUM;
				skip = 1;
				break;
			}
			break;

		case 9:
			switch (ch)
			{
			case '+':
			case '-':
				state = 10;
				break;

			case '0' ... '9':
				state = 11;
				break;

			default:
				error = 1;
				break;
			}
			break;

		case 10:
			switch (ch)
			{
			case '0' ... '9':
				state = 11;
				break;

			default:
				error = 1;
				break;
			}
			break;

		case 11:
			switch (ch)
			{
			case '0' ... '9':
				final = 1;
				tk.type = TK_RNUM;
				break;

			default:
				error = 1;
				break;
			}
			break;

		case 12:
			switch (ch)
			{
			case 'a' ... 'l':
			case 'n' ... 'z':
			case 'A' ... 'Z':
				state = 13;
				break;

			case 'm':
				state = 27;
				break;

			default:
				error = 1;
				break;
			}
			break;

		case 13:
			switch (ch)
			{
			case 'a' ... 'z':
			case 'A' ... 'Z':
				break;

			case '0' ... '9':
				state = 14;
				break;

			default:
				tk.type = TK_FUNID;
				skip = 1;
				break;
			}
			break;

		case 14:
			switch (ch)
			{
			case '0' ... '9':
				break;

			default:
				tk.type = TK_FUNID;
				skip = 1;
				break;
			}
			break;

		case 15:
			switch (ch)
			{
			case 'a' ... 'z':
				state = 16;
				break;

			default:
				error = 1;
				break;
			}
			break;

		case 16:
			switch (ch)
			{
			case 'a' ... 'z':
				break;

			default:
				tk.type = TK_RUID;
				skip = 1;
				break;
			}
			break;

		case 17:
			switch (ch)
			{
			case '@':
				state = 18;
				break;

			default:
				error = 1;
				break;
			}
			break;

		case 18:
			switch (ch)
			{
			case '@':
				final = 1;
				tk.type = TK_OR;
				break;

			default:
				error = 1;
				break;
			}
			break;

		case 19:
			switch (ch)
			{
			case '&':
				state = 20;
				break;

			default:
				error = 1;
				break;
			}
			break;

		case 20:
			switch (ch)
			{
			case '&':
				final = 1;
				tk.type = TK_AND;
				break;

			default:
				error = 1;
				break;
			}
			break;

		case 21:
			switch (ch)
			{
			case '=':
				final = 1;
				tk.type = TK_NE;
				break;

			default:
				error = 1;
				break;
			}
			break;

		case 22:
			switch (ch)
			{
			case '=':
				final = 1;
				tk.type = TK_EQ;
				break;

			default:
				error = 1;
				break;
			}
			break;

		case 23:
			switch (ch)
			{
			case '=':
				final = 1;
				tk.type = TK_GE;
				break;

			default:
				skip = 1;
				break;
			}
			break;

		case 24:
			switch (ch)
			{
			case '=':
				final = 1;
				tk.type = TK_LE;
				break;

			case '-':
				state = 25;
				break;

			default:
				tk.type = TK_LT;
				skip = 1;
				break;
			}
			break;

		case 25:
			switch (ch)
			{
			case '-':
				state = 26;
				break;

			default:
				error = 1;
				break;
			}
			break;

		case 26:
			switch (ch)
			{
			case '-':
				final = 1;
				tk.type = TK_ASSIGNOP;
				break;

			default:
				error = 1;
				break;
			}
			break;

		case 27:
			switch (ch)
			{
			case 'a':
				state = 28;
				break;

			case 'b' ... 'z':
			case 'A' ... 'Z':
				state = 13;
				break;

			default:
				skip = 1;
				tk.type = TK_FUNID;
			}
			break;

		case 28:
			switch (ch)
			{
			case 'i':
				state = 29;
				break;

			case 'a' ... 'h':
			case 'j' ... 'z':
			case 'A' ... 'Z':
				state = 13;
				break;

			default:
				skip = 1;
				tk.type = TK_FUNID;
			}
			break;

		case 29:
			switch (ch)
			{
			case 'n':
				state = 30;
				break;

			case 'a' ... 'm':
			case 'o' ... 'z':
			case 'A' ... 'Z':
				state = 13;
				break;

			default:
				skip = 1;
				tk.type = TK_FUNID;
			}
			break;

		case 30:
			switch (ch)
			{
			case 'a' ... 'z':
			case 'A' ... 'Z':
				state = 13;
				break;

			default:
				skip = 1;
				tk.type = TK_MAIN;
			}
			break;
		}

		if (error)
		{
			final = 1;
			tk.type = TK_ERROR;
			if (i > 1)
				skip = 1;
		}

		if (skip)
		{
			final = 1;
			i--;
		}

		if (final)
		{
			if (i > MAX_LEXEME_SIZE && tk.type != TK_COMMENT)
			{
				tk.type = TK_ERROR;
				strcpy(tk.lex, "Max size exceeded");
			}
			else
			{
				lexeme[i] = '\0';
				strcpy(tk.lex, lexeme);
			}
			tk.line_number = linecount;
		}
	} while (ch != EOF && final != 1);

	final = 0;
	state = 0;
	error = 0;
	i = 0;

	return tk;
}

int lookup(char *lexeme)
{
	if (!strcmp(lexeme, "with"))
		return TK_WITH;
	if (!strcmp(lexeme, "parameters"))
		return TK_PARAMETERS;
	if (!strcmp(lexeme, "end"))
		return TK_END;
	if (!strcmp(lexeme, "while"))
		return TK_WHILE;
	if (!strcmp(lexeme, "union"))
		return TK_UNION;
	if (!strcmp(lexeme, "endunion"))
		return TK_ENDUNION;
	if (!strcmp(lexeme, "definetype"))
		return TK_DEFINETYPE;
	if (!strcmp(lexeme, "as"))
		return TK_AS;
	if (!strcmp(lexeme, "type"))
		return TK_TYPE;
	if (!strcmp(lexeme, "global"))
		return TK_GLOBAL;
	if (!strcmp(lexeme, "parameter"))
		return TK_PARAMETER;
	if (!strcmp(lexeme, "list"))
		return TK_LIST;
	if (!strcmp(lexeme, "input"))
		return TK_INPUT;
	if (!strcmp(lexeme, "output"))
		return TK_OUTPUT;
	if (!strcmp(lexeme, "int"))
		return TK_INT;
	if (!strcmp(lexeme, "real"))
		return TK_REAL;
	if (!strcmp(lexeme, "endwhile"))
		return TK_ENDWHILE;
	if (!strcmp(lexeme, "if"))
		return TK_IF;
	if (!strcmp(lexeme, "then"))
		return TK_THEN;
	if (!strcmp(lexeme, "endif"))
		return TK_ENDIF;
	if (!strcmp(lexeme, "read"))
		return TK_READ;
	if (!strcmp(lexeme, "write"))
		return TK_WRITE;
	if (!strcmp(lexeme, "return"))
		return TK_RETURN;
	if (!strcmp(lexeme, "call"))
		return TK_CALL;
	if (!strcmp(lexeme, "record"))
		return TK_RECORD;
	if (!strcmp(lexeme, "endrecord"))
		return TK_ENDRECORD;
	if (!strcmp(lexeme, "else"))
		return TK_ELSE;
	return TK_FIELDID;
}

void lexerInit(char *filename)
{
	tb = (twinBuffer)malloc(sizeof(struct twinBuffer));
	tb->active = 0;
	tb->pos = 0;
	linecount = 1;
	skip = 0;
	final = 0;
	state = 0;
	error = 0;
	i = 0;
	ch = '\0';

	fp = fopen(filename, "r");
	int len = fread(tb->buff1, 1, BUFFER_SIZE, fp);
	if (len < BUFFER_SIZE)
		tb->buff1[len] = EOF;
}

void removeComments(char *testcaseFile, char *cleanFile)
{
	FILE *fp2 = fopen(cleanFile, "w");
	lexerInit(testcaseFile);
	while (ch != EOF)
	{
		readChar(tb);
		if (ch == '%')
			while (ch != EOF && ch != '\n')
				readChar(tb);

		if (ch != EOF)
		{
			fprintf(fp2, "%c", ch);
			printf("%c", ch);
		}
	}
	fclose(fp2);
	fclose(fp);
	free(tb);
}

void runLexer(char *filename)
{
	FILE *fp1 = fopen("tokenfile", "w");
	lexerInit(filename);
	while (ch != EOF)
	{
		tk = getNextToken(tb);
		if (tk.type != TK_COMMENT)
			fwrite(&tk, sizeof(tokenInfo), 1, fp1);
	}
}

void printLexer(char *filename)
{
	lexerInit(filename);
	printf("| %-20s | %-30s | %-4s |\n", "TOKEN", "LEXEME", "LINE");
	printf("----------------------------------------------------------------\n");
	while (ch != EOF)
	{
		tk = getNextToken(tb);
		printf("| %-20s | %-30s | %-4d |\n", tokens[tk.type], tk.lex, tk.line_number);
	}
	fclose(fp);
	free(tb);
}