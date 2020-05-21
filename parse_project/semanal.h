#ifndef __SEMANAL_H__
#define __SEMANAL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ll.h"

typedef enum type
{
  UNKNOWN,
  INT,
  VOID,
  ARR,//always an int
  ARRVAR, //non indexed arr
  VARTYPE,//assignable type
  BOOL,
  RETINT,
  RETVOID,
  RETINT_ELSE,//if returns something but else doesnt
  RETVOID_ELSE,//if returns something but else doesnt
  ELSE_RETINT,//if doesnt return anything but else does
  ELSE_RETVOID,//if doesnt return anything but else does
  ERR
}Type;

typedef enum vorf
{
  FUNC,
  VAR
}Vorf;

typedef struct sembol
{
  Type type;
  Vorf vorf;
  Type* param_types;
  int param_num;//arr size if arr
  char* name;
}Sembol;

typedef struct scope Scope;

struct scope
{
  Sembol** sembols;
  int sembolNum;
  Scope* lowerScope;
  Scope* higherScope;
};

Sembol* build_sembol(Type type, Vorf vorf, char* name);
void add_sembol(Sembol* s);
Sembol* find_sembol(Sembol* s);
void enter_scope();
int check_scope(Sembol* s);
void exit_scope();

void add_parameter(Sembol* s, Type t);

void startSemAnal();
int semanal(AST* ast);

#endif
