#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "ll.h"
#include "semanal.h"

int main(int argc, const char* argv[])
{
	if(argc > 3 || argc < 2)
	{
		printf("Usage: ./parse <input file> <output file>\n");
		return 1;
	}
	const char* inputFilename = argv[1];
	const char* outputFilename = (argc == 2) ? "out.ast" : argv[2];
  //printf("STARTING...\n");
	if(startLexer(inputFilename, outputFilename) != 0) return 1;
  if(startParser() != 0) return 1;
  //printf("START DOING PARSER THINGS\n");
  //do parser stuff
  AST* ast = parse();
  //printAST(ast, 0);
  //printf("Fix Addop Order~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  ast = fixAddopOrder(ast);
  //printAST(ast, 0);
  //printf("Fix Mulop Order~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  ast = fixMulopOrder(ast);
  //
  if(validParseTree(ast) == 0)
  {
    //printf("valid tree...\n");
    startSemAnal();
    //printf("starting semantic analysis.\n");
    //printAST(ast, 0);
    if(semanal(ast) == 0)
    {
      //printf("success\n");
      fprintAST(getOutputFile(), ast, 0);
    }
    //printf("finishing\n");
  }
  //semanal may need a stop funciton to clear up any memory
  //parser doesnt need a stop function
  //if(stopLexer() != 0) return 1;
	return 0;
}
