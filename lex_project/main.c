#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_BUFFER_SIZE 255
#define NUM_OF_KEYWORDS 6
#define MAX_KEYWORD_LENGTH 6

typedef enum state
{
	NONE,
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

const char* keywords[NUM_OF_KEYWORDS] = {"else", "if", "int", "return", "void", "while"};

//unclosed comment error
Token* commentError = NULL;
//unrecognized symbol error
Token* symbolError = NULL;

void setCommentError(int lineNumber)
{
	free(commentError);
	commentError = (Token*)malloc(sizeof(struct token));
	memset(commentError, 0, sizeof(struct token));
	commentError->lineNumber = lineNumber;
	commentError->tokenType = ERROR;
	commentError->lexeme = "/*";
}

void setSymbolError(int lineNumber, char* symbol)
{
	free(symbolError);
	symbolError = (Token*)malloc(sizeof(struct token));
	memset(symbolError, 0, sizeof(struct token));
	symbolError->lineNumber = lineNumber;
	symbolError->tokenType = ERROR;
	symbolError->lexeme = (char*)malloc(strlen(symbol) + 1);
	symbolError->lexeme[0] = symbol[0];
	symbolError->lexeme[1] = '\0';
}

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
int tokenizeLine(char* line, int lineNumber, int* isComment, FILE* outputFile);

//debugging functions
void printInfo(char* lexeme, char c, State s)
{
	printf("LEXEME: %s\n", lexeme);
	printf("CURRENT CHAR: ");
	if(c == '\n') printf("\\n\n");
	if(c == '\t') printf("\\t\n");
	printf("STATE: ");
	switch(s)
	{
		case NONE:
			printf("NONE\n");
			break;
		case ID:
			printf("ID\n");
			break;
		case NUM:
			printf("NUM\n");
			break;
		case IDKEY:
			printf("IDKEY\n");
			break;
		case KEY:
			printf("KEY\n");
			break;
		case SYM:
			printf("SYM\n");
			break;
		case ERROR:
			printf("ERROR\n");
			break;
	}
}

void printToken(int lineNumber, State s, char* lexeme)
{
	printf("(%i,", lineNumber);
	switch(s)
	{
		case ID:
			printf("ID,");
			break;
		case NUM:
			printf("NUM,");
			break;
		case KEY:
			printf("KEY,");
			break;
		case SYM:
			printf("SYM,");
			break;
		case ERROR:
			printf("ERROR,");
			break;
	}
	printf("\"%s\")\n", lexeme);
}

int main(int argc, const char* argv[])
{
	if(argc > 3 || argc < 2)
	{
		printf("Usage: ./lex <input file> <output file>\n");
		return 1;
	}
	const char* inputFilename = argv[1];
	const char* outputFilename = (argc == 2) ? "out.lex" : argv[2];
	
	FILE* inputFile = fopen(inputFilename, "r");
	FILE* outputFile = fopen(outputFilename, "w+");
	
	//0 is false 1 is true
	int isComment = 0;
	int lineNumber = 0;
	
	char line[LINE_BUFFER_SIZE];
	while(fgets(line, LINE_BUFFER_SIZE, inputFile) != NULL)
	{
		lineNumber++;
		if(tokenizeLine(line, lineNumber, &isComment, outputFile) != 0)//we have an unrecognized symbol error
		{
			//printf("SYMBOL ERROR");
			//printToken(symbolError->lineNumber, symbolError->tokenType, symbolError->lexeme);
			outputToken(symbolError->lineNumber, symbolError->tokenType, symbolError->lexeme, outputFile);
			break;
		}
	}
	if(isComment == 1)//then we have a unclosed comment error
	{
		//printToken(commentError->lineNumber, commentError->tokenType, commentError->lexeme);
		outputToken(commentError->lineNumber, commentError->tokenType, commentError->lexeme, outputFile);
	}
	
	if(inputFile != NULL) 
		if(fclose(inputFile) == EOF) printf("Problem closing the input file.\n");
	if(outputFile != NULL)
		if(fclose(outputFile) == EOF) printf("Problem closign the output file.\n");
	return 0;
}

int isDigit(char c)
{
	int result = 0;
	if(c <= 57 && c >= 48) result = 1;
	return result;
}

int isLetter(char c)
{
	int result = 0;
	if((c <= 122 && c >= 97) || (c <= 90 && c >= 65)) result = 1;
	return result;
}

int isWhitespace(char c)
{
	int result = 0;
	if(c == ' ' || c == '\n' || c == '\t') result = 1;
	return result;
}

int isSymbolPrefix(char c)
{
	int result = 0;
	if(c == '+' || c == '-' ||
	   c == '*' || c == '/' ||
	   c == '<' || c == '>' ||
	   c == '!' || c == '=' ||
	   c == ';' || c == ',' ||
	   c == '(' || c == ')' ||
	   c == '[' || c == ']' ||
	   c == '{' || c == '}') result = 1;
	return result;
}

int isSymbol(char c1, char c2)//c1 will always be a symbol prefix
{
	int result = 0;
	if((c1 == '!' || c1 == '<' || c1 == '>' || c1 == '=') && c2 == '=') result = 1;
	if(c1 == '/' && c2 == '*') result = 2;
	return result;
}

int isKeyword(char* str, int length)
{
	int result = 0;
	int i = 0;
	for(i = 0; i < NUM_OF_KEYWORDS; i++)
	{
		if(strcmp(str, keywords[i]) == 0)
		{
			result = 1;
			break;
		}
	}
	return result;
}

int outputToken(int lineNumber, State s, char* lexeme, FILE* outputFile)
{
	int error = 0;
	
	fprintf(outputFile, "(%i,", lineNumber);
	switch(s)
	{
		case ID:
			fprintf(outputFile, "ID,");
			break;
		case NUM:
			fprintf(outputFile, "NUM,");
			break;
		case KEY:
			fprintf(outputFile, "KEY,");
			break;
		case SYM:
			fprintf(outputFile, "SYM,");
			break;
		case ERROR:
			fprintf(outputFile, "ERROR,");
			break;
	}
	fprintf(outputFile, "\"%s\")\n", lexeme);
	
	return error;
}

int tokenizeLine(char* line, int lineNumber, int* isComment, FILE* outputFile)
{
	//0 means continue otherwise there is a problem and we stop
	int error = 0;
	
	char lexeme[LINE_BUFFER_SIZE];
	//make sure lexeme is empty
	memset(lexeme, 0, LINE_BUFFER_SIZE);
	
	char c = line[0];
	State s = NONE;
	
	int cIndex = 0;
	int lexemeEnd = 0;
	//lexemeEnd is always the space after the last character in the lexeme string
	//so lexemeEnd tells us how many characters are in the lexeme string
	
	while(c != '\0')
	{
		//if we are in a comment do stuff
		if(*isComment == 1)
		{
			if(c == '*' && lexemeEnd == 0)
			{
				lexeme[lexemeEnd++] = c;
				c = line[++cIndex];
				continue;
			}
			else if(c == '*')//lexemeEnd != 0
			{
				//dont bother clearing lexeme and adding the char
				c = line[++cIndex];
				continue;
			}
			else if(c == '/' && lexemeEnd != 0)//we found end comment
			{
				memset(lexeme, 0, LINE_BUFFER_SIZE);
				lexemeEnd = 0;
				s = NONE;
				c = line[++cIndex];
				*isComment = 0;
				continue;
			}
			else
			{
				memset(lexeme, 0, LINE_BUFFER_SIZE);
				lexemeEnd = 0;
				s = NONE;
				c = line[++cIndex];
				continue;
			}
			
		}
		//else do this other stuff
		//check if we have a recognized symbol if not output lexeme and the throw error
		if(!(isDigit(c) || isLetter(c) || isWhitespace(c) || isSymbolPrefix(c)))
		{
			//set error
			char sym[2];
			sym[0] = c;
			sym[1] = '\0';
			setSymbolError(lineNumber, sym);
			error = 1;
			break;//break out of the while loop
		}
		//here we know we have a recognized symbol
		int result = -1; // this is for the symbol case
		switch(s)
		{
			case NONE:
				if(isDigit(c))
				{
					s = NUM;
					lexeme[lexemeEnd++] = c;
				}
				else if(isLetter(c))
				{
					s = IDKEY;
					lexeme[lexemeEnd++] = c;
				}
				else if(isSymbolPrefix(c))
				{
					s = SYM;
					lexeme[lexemeEnd++] = c;
				}
				//else if(isWhitespace(c))
				//{
				//	s = NONE;
				//}
				break;
			case IDKEY:
				if(isDigit(c))
				{
					s = ID;
					lexeme[lexemeEnd++] = c;
				}
				else if(isLetter(c))
				{
					//add char to lexeme
					lexeme[lexemeEnd++] = c;
					//check if its a keyword or if lexemeEnd is greater than 6
					if(lexemeEnd <= MAX_KEYWORD_LENGTH)
					{
						if(isKeyword(lexeme, lexemeEnd))
						{
							s = KEY;
						}
					}
					else//no keywords have a length greater than MAX_KEYWORD_LENGTH so if the lexeme is greater than that it must be an ID
					{
						s = ID;
					}
				}
				else//we had a non digit and non letter character so we output the token
				{
					//this lexeme will always be an ID
					s = ID;
					//printToken(lineNumber, s, lexeme);
					outputToken(lineNumber, s, lexeme, outputFile);
					memset(lexeme, 0, LINE_BUFFER_SIZE);
					lexemeEnd = 0;
					cIndex--;
					s = NONE;
				}
				break;
			case ID:
				if(isDigit(c) || isLetter(c))//still an id
				{
					lexeme[lexemeEnd++] = c;
				}
				else//if its not a digit and its not a letter then the id is finished
				{
					//output token
					//printToken(lineNumber, s, lexeme);
					outputToken(lineNumber, s, lexeme, outputFile);
					memset(lexeme, 0, LINE_BUFFER_SIZE);
					lexemeEnd = 0;
					cIndex--;
					s = NONE;
				}
				break;
			case NUM:
				if(isDigit(c))//still a number
				{
					lexeme[lexemeEnd++] = c;
				}
				else//num ended output the token
				{
					//printToken(lineNumber, s, lexeme);
					outputToken(lineNumber, s, lexeme, outputFile);
					memset(lexeme, 0, LINE_BUFFER_SIZE);
					lexemeEnd = 0;
					cIndex--;
					s = NONE;
				}
				break;
			case KEY:
				//any char should make it not a keyword so output the token
				if(isLetter(c) || isDigit(c))
				{
					lexeme[lexemeEnd++] = c;
					s = ID;
				}
				else
				{
					//printToken(lineNumber, s, lexeme);
					outputToken(lineNumber, s, lexeme, outputFile);
					memset(lexeme, 0, LINE_BUFFER_SIZE);
					lexemeEnd = 0;
					cIndex--;
					s = NONE;
				}
				break;
			case SYM:
				result = isSymbol(lexeme[0], c);
				//printf(">>>>%c<<<<<\n", c);
				if(result == 0)//we have an invalid symbol combo
				{
					//printf("here result 0\n");
					//we know lexeme is of length 1
					if(lexeme[0] == '!')
					{
						//printf("lexeme !\n");
						//set error
						char sym[2];
						sym[0] = lexeme[0];
						sym[1] = '\0';
						setSymbolError(lineNumber, sym);
						error = 1;
						//clear lexeme
						memset(lexeme, 0, LINE_BUFFER_SIZE);
						lexemeEnd = 0;
						s = NONE;
						return error;//break out of the while loop
					}
					else
					{
						//output lexeme
						//printToken(lineNumber, s, lexeme);
						outputToken(lineNumber, s, lexeme, outputFile);
						memset(lexeme, 0, LINE_BUFFER_SIZE);
						lexemeEnd = 0;
						cIndex--;
						//set state to none;
						s = NONE;
					}
				}
				else if(result == 1) //we have a valid symbol
				{
					//add c to lexeme
					lexeme[lexemeEnd++] = c;
					//output lexeme
					//printToken(lineNumber, s, lexeme);
					outputToken(lineNumber, s, lexeme, outputFile);
					memset(lexeme, 0, LINE_BUFFER_SIZE);
					lexemeEnd = 0;
					s = NONE;
				}
				else if(result == 2) // we have a comment
				{
					*isComment = 1;
					setCommentError(lineNumber);
					//clear lexeme
					memset(lexeme, 0, LINE_BUFFER_SIZE);
					lexemeEnd = 0;
					s = NONE;
					//printToken(lineNumber, ERROR, "/*");
				}
				break;
			default:
				break;
		}
		//debugging
		//printInfo(lexeme, c, s);
		
		c = line[++cIndex];
	}
	//we can end up with some token in the lexeme if we dont get a newline in the line argument
	//so we need to check the state one last time and output the lexeme
	if(lexemeEnd != 0)//if we still have stuff in the lexeme
	{
		switch(s)
		{
			case NONE:
				//if its none then the lexeme is empty
				break;
			case IDKEY:
				s = ID;//a lexeme with the idkey token type will always be an id at this point
				break;
			case ID:
			case NUM:
			case KEY:
			case SYM:
			case ERROR:
				break;
			default:
				//wont end up here
				break;
		}
		//output token
		//printToken(lineNumber, s, lexeme);
		outputToken(lineNumber, s, lexeme, outputFile);
	}
	
	return error;
}