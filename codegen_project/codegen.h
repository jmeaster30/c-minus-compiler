#ifndef __CODEGEN_H__
#define __CODEGEN_H__

#include "semanal.h"

typedef struct frame
{
  int varnum;
  char** varnames; //this will be the same size as offsets to the index of some var name corresponds to where it is on the stack
  int* varoffs; //this is the offset from thr frame pointer
  int* varsizes; //size of each variable
  char* vartypes; //type of each variable (0 int 1 arr)
}Frame;

//clears all the memory pointed to by f
void clearFrame();
//add vname to the frame and calculates the offset based on the size
void addToFrame(char* vname, int size, char type);
//gets offset of closest definition of vname if null the variable is a global variable
int getOffset(char* vname);

int startCodeGen(const char* outputFile);
int stopCodeGen();

int gencode(AST* ast);

#endif
