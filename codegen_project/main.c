#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "ll.h"
#include "semanal.h"
#include "codegen.h"

int main(int argc, const char* argv[])
{
	if(argc > 3 || argc < 2)
	{
		printf("Usage: ./codegen <input file> <output file>\n");
		return 1;
	}
	const char* inputFilename = argv[1];
	const char* outputFilename = (argc == 2) ? "out.s" : argv[2];
  //Starting things up
	if(startLexer(inputFilename) != 0) return 1;
  if(startParser() != 0) return 1;
  startSemAnal();
  if(startCodeGen(outputFilename) != 0) return 1;

  //parse and check for semantic correctness
  AST* ast = parse();
  if(ast == NULL) return 1;//failed syntactic analysis
  if(semanal(ast) != 0) return 1;//failed semantic analysis

  //do code generation
  gencode(ast);

  //semanal may need a stop funciton to clear up any memory
  //parser doesnt need a stop function
  if(stopLexer() != 0) return 1;
	return 0;
}
