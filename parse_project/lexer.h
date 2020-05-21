#ifndef __LEXER_H__
#define __LEXER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_BUFFER_SIZE 255
#define NUM_OF_KEYWORDS 6
#define MAX_KEYWORD_LENGTH 6

typedef enum state
{
	EMPTY,
	ID,
	NUM,
	KEY,
	IDKEY,
	SYM,
	ERROR
}State;

typedef struct token
{
	int lineNumber;
	State tokenType;
	char* lexeme;
}Token;

int startLexer(const char* inputFilename, const char* outputFilename);
int stopLexer();

FILE* getOutputFile();

void printToken(int lineNumber, State s, char* lexeme);

//checks if the str is one of the keywords
int isKeyword(char* str, int length);
//returns 1 if c is a digit otherwise returns 0
int isDigit(char c);
//returns 1 if c is a letter otherwise returns 0
int isLetter(char c);
//returns 1 if c is a whitespace otherwise returns 0
int isWhitespace(char c);
//returns 1 if c is a prefix to any symbol otherwise returns 0
int isSymbolPrefix(char c);
//returns 1 if c1 + c2 a valid symbol
//will return 2 if c1 + c2 is the start of a comment
//otherwise will return 0
int isSymbol(char c1, char c2);

int outputToken(int lineNumber, State s, char* lexeme, FILE* outputFile);
int tokenizeLine(char* line, int lineNumber, int* isComment, int* numAddedTokens);

int getNextToken(Token* token);

#endif /*__LEXER_H__*/
