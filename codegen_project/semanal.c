#include "semanal.h"

Scope* global = NULL;
Scope* curr_scope = NULL;

void startSemAnal()
{
  global = (Scope*)malloc(sizeof(struct scope));
  memset(global, 0, sizeof(struct scope));
  global->higherScope = NULL;
  global->lowerScope = NULL;
  curr_scope = global;
  Sembol* input_func = build_sembol(INT, FUNC, "input");
  Sembol* output_func = build_sembol(VOID, FUNC, "output");
  add_parameter(output_func, INT);
  add_sembol(input_func);
  add_sembol(output_func);
  //print_scope(curr_scope);
}

Sembol* build_sembol(Type type, Vorf vorf, char* name)
{
  Sembol* newSembol = (Sembol*)malloc(sizeof(struct sembol));
  memset(newSembol, 0, sizeof(struct sembol));
  newSembol->type = type;
  newSembol->vorf = vorf;
  int nameLen = strlen(name) + 1;
  newSembol->name = (char*)malloc(nameLen);
  memset(newSembol->name, 0, nameLen);
  memcpy(newSembol->name, name, nameLen - 1);
  return newSembol;
}

void print_sembol(Sembol* s)
{
  printf("Sembol Name: %s\n", s->name);
}

void print_scope(Scope* sc)
{
  Scope* scope = sc;
  while(scope != NULL)
  {
    printf("~~~~~\n");
    int i = 0;
    for(i = 0; i < scope->sembolNum; i++)
    {
      print_sembol(*(scope->sembols + i));
    }
    scope = scope->higherScope;
  }
  printf("~~~~~\n");
}

void add_sembol(Sembol* s)
{
  //printf("~~~ ADDING TO  SCOPE ~~~\n");
  //printf("%s\n", s->name);
  //printf("Scope: %p\n", curr_scope);
  //printf("Old Scope Sembol Num: %i\n", curr_scope->sembolNum);
  if(curr_scope->sembolNum == 0)
  {
    curr_scope->sembols = (Sembol**)malloc(sizeof(void*));
    *(curr_scope->sembols) = s;
    curr_scope->sembolNum++;
  }else{
    //if operandNum = n the nth index of the array will be free
    Sembol** newSembols = (Sembol**)malloc(sizeof(void*) * (curr_scope->sembolNum + 1));
    memset(newSembols, 0, sizeof(void*) * (curr_scope->sembolNum + 1));
    memcpy(newSembols, curr_scope->sembols, sizeof(void*) * curr_scope->sembolNum);
    free(curr_scope->sembols);
    curr_scope->sembols = newSembols;
    *(curr_scope->sembols + curr_scope->sembolNum) = s;
    curr_scope->sembolNum++;
  }
  //printf("New Scope Sembol Num: %i\n", curr_scope->sembolNum);
  //printf("~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

void add_parameter(Sembol* s, Type t)
{
  if(s->param_num == 0)
  {
    s->param_types = (Type*)malloc(sizeof(enum type));
    *(s->param_types) = t;
    s->param_num++;
  }
  else
  {
    Type* newtypes = (Type*)malloc(sizeof(enum type) * (s->param_num + 1));
    memset(newtypes, 0, sizeof(enum type) * (s->param_num + 1));
    memcpy(newtypes, s->param_types, sizeof(enum type) * s->param_num);
    free(s->param_types);
    s->param_types = newtypes;
    *(s->param_types + s->param_num) = t;
    s->param_num++;
  }
}

Sembol* find_sembol(Sembol* s)
{
  Scope* scope = curr_scope;
  while(scope != NULL)
  {
    //printf("~~~ CHECKING SCOPE ~~~\n");
    //printf("%s\n", s->name);
    //printf("Scope: %p\n", scope);
    //print_scope(scope);
    //printf("~~~~~~~~~~~~~~~~~~~~~~\n");
    int i = 0;
    for(i = 0; i < scope->sembolNum; i++)
    {
      //printf("%s =?= %s\n", (*(scope->sembols + i))->name, s->name);
      if(strcmp((*(scope->sembols + i))->name, s->name) == 0) return *(scope->sembols + i);
    }
    scope = scope->higherScope;
  }
  //printf("%s NOT FOUND IN HIGHER SCOPES\n", s->name);
  return NULL;
}

//creates new scope and sets curr_scope to it
void enter_scope()
{
  if(curr_scope->lowerScope == NULL)
  {
    //printf("~~~ ENTERING SCOPE ~~~\n");
    Scope* newScope = (Scope*)malloc(sizeof(struct scope));
    memset(newScope, 0, sizeof(struct scope));
    newScope->higherScope = curr_scope;
    curr_scope->lowerScope = newScope;
    //printf("New Scope:            %p\n", newScope);
    //printf("Curr Scope:           %p\n", curr_scope);
    //printf("New Higher Scope:     %p\n", newScope->higherScope);
    //printf("Curr New Lower Scope: %p\n", curr_scope->lowerScope);
    curr_scope = curr_scope->lowerScope;
    //printf("New Curr Scope:       %p\n", curr_scope);
    //printf("~~~~~~~~~~~~~~~~~~~~~~\n");
  }else{
    curr_scope = curr_scope->lowerScope;
  }
}

int check_scope(Sembol* s)
{
  int i = 0;
  for(i = 0; i < curr_scope->sembolNum; i++)
  {
    if(strcmp((*(curr_scope->sembols + i))->name, s->name) == 0) return 1;
  }
  return 0;
}

void save_scope()//exits scope but doesnt delete it
{
  curr_scope = curr_scope->higherScope;
}

void exit_scope()
{
  //printf("~~~ EXITING SCOPE ~~~\n");
  //printf("Exiting Scope:        %p\n", curr_scope);
  //printf("New Scope:            %p\n", curr_scope->higherScope);
  curr_scope = curr_scope->higherScope;
  //free(curr_scope->lowerScope);

  //printf("New Curr Scope:       %p\n", curr_scope);
  curr_scope->lowerScope = NULL;
  //printf("New Curr Lower Scope: %p\n", curr_scope->lowerScope);
 // printf("~~~~~~~~~~~~~~~~~~~~~~\n");
}

//returns the type of the ast given to it
//if there is a return statement in the ast
//this will return the type of the return statement or
//an error if something caused an error or void if no return statements and no errors
Type decTypeCheck(AST* ast)
{
  //check if the variables are defined in the Scope
  //check if funcitons are defined in the scope
  //check if main is the last declaration
  //variables can't have a void type declaration
  //array name is invalid unless it has [] except in function calls
  //make sure number of parameters in a call is the same as number of parameters in funciton declaration
  //check for redeclaration's of functions
  //check the return value of functions
  if(ast->astoken->sym == NTS_FUN_DEC)
  {

    //first 2 operands should be type and name
    Type func_type = (strcmp((*(ast->operands))->astoken->lexeme, "void") == 0) ? VOID : INT;
    char* func_name = (*(ast->operands + 1))->astoken->lexeme;
    Sembol* func_dec = build_sembol(func_type, FUNC, func_name);
    AST* param_list = *(ast->operands + 2);

    //check if the function actually has parameters
    if(param_list->operandNum != 0)
    {
      //yes add the parameters
      int i = 0;
      for(i = 0; i < param_list->operandNum; i++)
      {
        Type t = (strcmp((*((*(param_list->operands + i))->operands))->astoken->lexeme, "void") == 0) ? VOID : INT;
        //check if we have an array being passed in ie the operandNum == 3 (type, name, brackets)
        if((*(param_list->operands + i))->operandNum == 3) t = ARRVAR;
        //error if t is void
        if(t == VOID) return ERR;
        add_parameter(func_dec, t);
      }
    }
    //printf("already declared? ");
    //check if function has already been declared
    if(check_scope(func_dec) != 0) return ERR;
    //printf("no\n");
    //it has not so add it to the curr_scope
    add_sembol(func_dec);
    //move down scope and check the contents of the function
    enter_scope();
    //add paramters to scope
    int i = 0;
    for(i = 0; i < param_list->operandNum; i++)
    {
      Type t = (strcmp((*((*(param_list->operands + i))->operands))->astoken->lexeme, "void") == 0) ? VOID : INT;
      char* param_name = (*((*(param_list->operands + i))->operands + 1))->astoken->lexeme;
      //check if we have an array being passed in ie the operandNum == 3 (type, name, brackets)
      if((*(param_list->operands + i))->operandNum == 3) t = ARR; //this isnt arrvar cause we want to make sure we actually index into the passed in arrvar
      //error if t is void
      //printf("parameter type error 190?\n");
      if(t == VOID) return ERR;//shouldnt happen but just in case
      //printf("no\n");
      //just in case check if parameter exists in scope itshouldnt happen though
      Sembol* param = build_sembol(t, VAR, param_name);
      //printf("parameter redefined?\n");
      if(check_scope(param) != 0) return ERR;//parameter redefined
      //printf("no\n");
      add_sembol(param);
    }
    save_scope();
    //print_scope(curr_scope);
    //go into funciton statment
    Type cmpd_result = decTypeCheck(*(ast->operands + 3));
    //cmpd_result will be the type of the return statement
    //check the return statement value(s);
    //printf("compound statement error?\n");
    if(cmpd_result == ERR) return ERR; //error in the compound statement
    //printf("no\n");
    //if(cmpd_result == VOID) return ERR; //no return statement? or the return is just void
    //printf("wrong return type error?\n");
    if(cmpd_result != func_type) return ERR;
    //printf("no\n");
    //return
    return func_type;
  }
  else if(ast->astoken->sym == NTS_VAR_DEC)
  {
    //printf("var dec\n");
    //printAST(ast, 0);
    //make sure it has am int type
    //check if its an arr or not
    //chekc if it has been declared in this scope
    Type var_type = (strcmp((*(ast->operands))->astoken->lexeme, "void") == 0) ? VOID : INT;
    if(var_type == VOID) return ERR;
    char* var_name = (*(ast->operands + 1))->astoken->lexeme;
    //printf("%s\n", var_name);
    int arr_size = 0;
    //if arr the num operands is 3
    if(ast->operandNum == 3)
    {
      var_type = ARR;
      char *end;
      arr_size = strtol((*(ast->operands + 2))->astoken->lexeme, &end, 10);
    }
    Sembol* var = build_sembol(var_type, VAR, var_name);
    if(arr_size > 0) var->param_num = arr_size;
    //check if this has been declared in this scope
    if(check_scope(var) != 0) return ERR;
    add_sembol(var);
    //printf("var dec success\n");
    return var_type;
  }
  else if(ast->astoken->sym == NTS_CALL)
  {
    //printf("call\n");
    //check if funciton is declared
    char* call_name = (*(ast->operands))->astoken->lexeme;
    AST* args_list = *(ast->operands + 1);
    Sembol* call_func = build_sembol(UNKNOWN, FUNC, call_name);//we dont have information about the type of the funciton at this point
    Sembol* dec_func = find_sembol(call_func);
    if(dec_func == NULL) return ERR;//function not defined
    //check if dec_func is a funciton
    if(dec_func->vorf != FUNC) return ERR;//not a function the id is a variable
    //check if num of parameters matches
    if(dec_func->param_num != args_list->operandNum) return ERR;//not enough parameters
    //check if parameter types match
    int i = 0;
    for(i = 0; i < args_list->operandNum; i++)
    {
      Type arg_type = decTypeCheck(*(args_list->operands+i));
      //printf("arg_type: %i = %i or %i or %i or %i\n", arg_type, BOOL, INT, VARTYPE, ARRVAR);
      //printf("param_type: %i\n", *(dec_func->param_types + i));
      if((arg_type == BOOL ? INT : (arg_type == VARTYPE ? INT : arg_type)) != *(dec_func->param_types + i)) return ERR;//arg type does not match parameter type
    }
    //here the function exists all of the args exist and match the parameter types
    //so we have a valid function call
    //return func_type
    //printf("call success\n");
    return dec_func->type;
  }
  else if(ast->astoken->sym == NTS_VAR)
  {
    //printf("var\n");
    //check if variable is declared
    char* var_name = (*(ast->operands))->astoken->lexeme;
    //printf("%s\n", var_name);
    Sembol* var = build_sembol(UNKNOWN, VAR, var_name);
    Sembol* dec_var = find_sembol(var);
    if(dec_var == NULL) return ERR; //variable not declared
    //printf("dec success\n");
    if(dec_var->vorf == FUNC) return ERR; //not a variable but a funciton
    //printf("func success\n");
    //check if array needs the brackets
    if(dec_var->type != ARR && ast->operandNum > 1) return ERR;//variable cant be accessed as an array
    //printf("var success\n");

    if(dec_var->type == ARR && ast->operandNum == 1) return ARRVAR;
    //printf("not arr var success\n");
    //at this point we have a declared variable that is being accessed correctly
    //if we have an arr check the type of the index bit
    if(dec_var->type == ARR)
    {
      AST* index_bit = *(ast->operands + 1);
      Type index_type = decTypeCheck(index_bit);
      //printf("%i = %i\n", index_type, ERR);
      if(!(index_type == INT || index_type == VARTYPE)) return ERR;
      //printf("index success\n");
    }
    //return var type
    //here we will always have a valid array indexing which is an INT or we will have a regular variable which is always an INT
    //so to distinguish between int literals and assignable variable we set type to VAR
    //printf("var type success\n");
    return VARTYPE;
  }
  else if(ast->astoken->sym == TS_ASS_OP)
  {
    //printf("ass op\n");
    //check that left hand side is a variable
    AST* lhs = *(ast->operands);
    AST* rhs = *(ast->operands + 1);
    //i think this is already checked in the syntax
    if(lhs->astoken->sym != NTS_VAR) return ERR; //cannot assign to lhs
    Type lhs_type = decTypeCheck(lhs);
    Type rhs_type = decTypeCheck(rhs);
    if(lhs_type == ERR) return ERR;
    if(rhs_type == ERR) return ERR;
    //check that lhs isnt an arr
    //check that the type of the right hand side matches the lhs ie is an int
    if(lhs_type != VARTYPE) return ERR; //can't assign to lhs
    if(rhs_type != INT && rhs_type != BOOL && rhs_type != VARTYPE) return ERR; //rhs_type doesn't match lhs_type
    //we have a correct ass statment here
    return VOID;
  }
  else if(ast->astoken->sym == TS_NUM)
  {
    //printf("num\n");
    //int literal
    return INT;
  }
  else if(ast->astoken->sym == TS_ADD_OP || ast->astoken->sym == TS_MIN_OP ||
          ast->astoken->sym == TS_MUL_OP || ast->astoken->sym == TS_DIV_OP)
  {
    //printf("arit op\n");
    AST* lhs = *(ast->operands);
    AST* rhs = *(ast->operands + 1);
    Type lhs_type = decTypeCheck(lhs);
    Type rhs_type = decTypeCheck(rhs);
    if(lhs_type == ERR) return ERR;
    if(rhs_type == ERR) return ERR;
    if(!((lhs_type == INT || lhs_type == BOOL || lhs_type == VARTYPE) &&
         (rhs_type == INT || rhs_type == BOOL || rhs_type == VARTYPE))) return ERR; //operator doesn't make sense for type
    //here we have valid use of the operators
    return INT;
  }
  else if(ast->astoken->sym == TS_LT_OP || ast->astoken->sym == TS_LTE_OP ||
          ast->astoken->sym == TS_GT_OP || ast->astoken->sym == TS_GTE_OP ||
          ast->astoken->sym == TS_EQ_OP || ast->astoken->sym == TS_NEQ_OP)
  {
    //printf("relop\n");
    AST* lhs = *(ast->operands);
    AST* rhs = *(ast->operands + 1);
    Type lhs_type = decTypeCheck(lhs);
    Type rhs_type = decTypeCheck(rhs);
    if(lhs_type == ERR) return ERR;
    if(rhs_type == ERR) return ERR;
    if(!((lhs_type == VARTYPE || lhs_type == INT || lhs_type == BOOL) &&
         (rhs_type == VARTYPE || rhs_type == INT || rhs_type == BOOL))) return ERR; //operator doesn't make sense for type
    //here we have valid use of the operators
    return BOOL;
  }
  else if(ast->astoken->sym == NTS_CMPD_STMT)
  {
    //printf("cmpd_stmt\n");
    Type stmt_type = UNKNOWN;
    enter_scope();
    int i = 0;
    Type* stmt_types = (Type*)malloc(sizeof(enum type)*(ast->operandNum));
    memset(stmt_types, 0, sizeof(enum type)*(ast->operandNum));
    Symbol* stmt_syms = (Symbol*)malloc(sizeof(enum symbol)*(ast->operandNum));
    memset(stmt_syms, 0, sizeof(enum symbol)*(ast->operandNum));
    for(i = 0; i < ast->operandNum; i++)
    {
      //check the typing of everything in this cmpd_stmt
      *(stmt_syms + i) = (*(ast->operands + i))->astoken->sym;
      *(stmt_types + i) = decTypeCheck(*(ast->operands + i));
      //we can't have an ARRVAR statements
      if(*(stmt_types + i) == ARRVAR) *(stmt_types + i) = ERR;
    }
    exit_scope();
    //go through make sure all return stmts are the same if not then we have an error
    //if any of the types are an err then stmt_type is an err
    Type ret_stmt_type = UNKNOWN;
    Type sel_stmt_type = UNKNOWN;
    for(i = ast->operandNum - 1; i >= 0; i--)//might be good to check this backwards actually
    {
      Symbol s = *(stmt_syms + i);
      Type t = *(stmt_types + i);
      if(s == TS_NUM) return ERR;
      //check if only certain branches of a sel_stmt return something and if the next code then returns said thing
      if(s == NTS_SEL_STMT &&
        (t == RETINT_ELSE || t == RETVOID_ELSE || t == ELSE_RETINT || t == ELSE_RETVOID)
        && ret_stmt_type == UNKNOWN) return ERR;
      if(s == NTS_SEL_STMT && (t == RETINT_ELSE || t == RETVOID_ELSE)) t = t - (RETINT_ELSE - RETINT);//changes RETINT_ELSE -> RETINT and RETVOID_ELSE -> RETVOID
      if(s == NTS_SEL_STMT && (t == ELSE_RETINT || t == ELSE_RETVOID)) t = t - (ELSE_RETINT - RETINT);//changes ELSE_RETINT -> RETINT and ELSE_RETVOID -> RETVOID
      //check the return statement
      if(s == NTS_RET_STMT && ret_stmt_type == UNKNOWN && t != ERR) ret_stmt_type = t - (RETINT - INT); //turns retint->int and retvoid->void
      if(s == NTS_RET_STMT && ret_stmt_type != (t - (RETINT - INT))) return ERR;
      if(t == ERR) return ERR;
    }
    if(ret_stmt_type == UNKNOWN && stmt_type != ERR) stmt_type = VOID;
    if(ret_stmt_type != UNKNOWN && stmt_type != ERR) stmt_type = ret_stmt_type;
    //printf("%i = %i\n", ERR, stmt_type);
    return stmt_type;
  }
  else if(ast->astoken->sym == NTS_ITER_STMT)
  {
    //printf("iter\n");
    AST* exp = *(ast->operands);
    Type exp_type = decTypeCheck(exp);
    if(exp_type != INT && exp_type != BOOL) return ERR; //expression wrong type
    AST* stmt = *(ast->operands + 1);
    Type stmt_type = decTypeCheck(stmt);
    return stmt_type;
  }
  else if(ast->astoken->sym == NTS_SEL_STMT)
  {
    //printf("sel\n");
    AST* exp = *(ast->operands);
    Type exp_type = decTypeCheck(exp);
    if(exp_type != INT && exp_type != BOOL) return ERR; //expression wrong type
    AST* stmt = *(ast->operands + 1);
    Type stmt_type = decTypeCheck(stmt);
    //check if there is an else stmt
    if(ast->operandNum > 2)
    {
      AST* else_stmt = *(ast->operands + 2);
      Type else_stmt_type = decTypeCheck(else_stmt);
      //how to do this????
      if(else_stmt_type == ERR) return ERR;
      if(stmt_type == RETINT  && (else_stmt_type == INT || else_stmt_type == VOID)) return RETINT_ELSE;
      if(stmt_type == RETVOID && (else_stmt_type == INT || else_stmt_type == VOID)) return RETVOID_ELSE;
      if((stmt_type == INT || stmt_type == VOID) && else_stmt_type == RETVOID) return ELSE_RETVOID;
      if((stmt_type == INT || stmt_type == VOID) && else_stmt_type == RETINT ) return ELSE_RETINT;
      if(else_stmt_type == stmt_type) return stmt_type;
      //statements are different and they both have return statements
      if((stmt_type == RETINT || stmt_type == RETVOID) && else_stmt_type != stmt_type) return ERR;
      //no return statements in the sel_stmt
      return stmt_type;
    }
    else
    {
      //if no else stmt then just return the type of the regular stmt
      return stmt_type;
    }
  }
  else if(ast->astoken->sym == NTS_RET_STMT)
  {
    //printf("ret>>>");
    if(ast->operandNum == 0) return RETVOID;
    AST* exp = *(ast->operands);
    Type t = decTypeCheck(exp);
    if(t == INT || t == BOOL || t == VARTYPE || t == ARR)
    {
      t = RETINT;
    //}else if(t == VOID){ //this causes test-sema-typecheck-returnstmt-void-in-void to fail
                           //since "return void;" is bad "return;" is correct
    //  t = RETVOID;
    }else{
      //printf("MMMMMM >>>> %i = %i\n", t, ERR);
      t = ERR;
    }
    return t;
  }
  else
  {
    //printf("other\n");
    Type t = UNKNOWN;
    int i = 0;
    for(i = 0; i < ast->operandNum; i++)
    {
      AST* a = *(ast->operands + i);
      Type a_type = decTypeCheck(a);
      if(a_type == ERR)
      {
        t = a_type;
        break;
      }
    }
    if(t == UNKNOWN) t = VOID;
    //printf("type: %i = %i\n", t, ERR);
    return t;
  }
}

int semanal(AST* ast)
{
  //this ast will be the program ast
  //printf("starting sem anal\n");
  if(ast->operandNum == 0) return 1; //no declarations
  //printf("checking types and declarations and such...\n");
  Type program_type = decTypeCheck(ast);
  //printf("finished dec type check\n");
  if(program_type == ERR) return 1;
  int i = 0;
  //check that the main function is the last function declaration
  AST* main_dec = *(ast->operands + (ast->operandNum - 1));
  //since we did the decTypeCheck on the whole ast then main will return void;
  //check if fun_dec
  //check type, name, and params
  //printf("main last declaration\n");
  if(main_dec->astoken->sym != NTS_FUN_DEC) return 1;//last declaration is not a function
  AST* main_dec_type = *(main_dec->operands);
  AST* main_dec_name = *(main_dec->operands + 1);
  AST* main_dec_params = *(main_dec->operands + 2);
  if(strcmp(main_dec_type->astoken->lexeme, "void") != 0) return 1;//last declaration does not return void
  if(strcmp(main_dec_name->astoken->lexeme, "main") != 0) return 1;//last declaration is not main
  if(main_dec_params->operandNum != 0) return 1;//last declaration takes more than zero parameters
  //printf("done\n");
  //so I think we should be all good at this point
  return 0;
}
