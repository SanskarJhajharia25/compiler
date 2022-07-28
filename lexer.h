#include "lexerDef.h"

tokenInfo getNextToken(twinBuffer B);
int lookup(char *lexeme);
void fillBuffer(twinBuffer B);
void readChar(twinBuffer B);
void runLexer(char *filename);
void lexerInit(char *filename);
void removeComments(char *testcaseFile, char *cleanFile);
void printLexer(char *filename);