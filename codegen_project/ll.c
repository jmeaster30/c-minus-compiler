#include "ll.h"
#include "lexer.h"

//Stack* asTokenStack = NULL;
Token* nextToken = NULL;

int startParser()
{
  //printf("Starting Parser...\n");
  nextToken = (Token*)malloc(sizeof(struct token));
  if(getNextToken(nextToken) != 0)
  {
    //printf("First Token Failed...\n");
    return 1;
  }
  //printf("Parser Started...\n");
  return 0;
}

int validParseTree(AST* ast)
{
  if(ast->astoken->sym == TS_INVALID) return -1;
  int i = 0;
  for(i = 0; i < ast->operandNum; i++)
  {
    if(validParseTree(*(ast->operands + i)) != 0) return -1;
  }
  return 0;
}

void printAST(AST* ast, int level)
{
  int i = 0;
  for(i = 0; i < level; i++) printf(" ");
  printf("[%s", ast->astoken->lexeme);
  if(ast->operandNum != 0) printf("\n");
  for(i = 0; i < ast->operandNum; i++)
  {
    printf(" ");
    printAST(*(ast->operands + i), level + 1);
  }
  if(ast->operandNum != 0)
    for(i = 0; i < level; i++) printf(" ");
  printf("]\n");
}

void fprintAST(FILE* file, AST* ast, int level)
{
  int i = 0;
  for(i = 0; i < level; i++) fprintf(file, " ");
  fprintf(file, "[%s", ast->astoken->lexeme);
  if(ast->operandNum != 0) fprintf(file, "\n");
  for(i = 0; i < ast->operandNum; i++)
  {
    //printf(" ");
    fprintAST(file, *(ast->operands + i), level + 1);
  }
  if(ast->operandNum != 0)
    for(i = 0; i < level; i++) fprintf(file, " ");
  fprintf(file, "]\n");
}

AST* buildTerminalAST(ASToken* astoken)
{
  AST* newAST = (AST*)malloc(sizeof(struct ast));
  memset(newAST, 0, sizeof(struct ast));
  newAST->astoken = astoken;
  return newAST;
}

AST* buildAST(ASToken* astoken)
{
  AST* newAST = (AST*)malloc(sizeof(struct ast));
  memset(newAST, 0, sizeof(struct ast));
  newAST->astoken = astoken;
  return newAST;
}

void addOperand(AST* ast, AST* operand)
{
  if(ast->operandNum == 0)
  {
    ast->operands = (struct ast**)malloc(sizeof(void*));
    *(ast->operands) = operand;
    ast->operandNum++;
  }else{
    //if operandNum = n the nth index of the array will be free
    AST** newOperands = (AST**)malloc(sizeof(void*) * (ast->operandNum + 1));
    memset(newOperands, 0, sizeof(void*) * (ast->operandNum + 1));
    memcpy(newOperands, ast->operands, sizeof(void*) * ast->operandNum);
    free(ast->operands);
    ast->operands = newOperands;
    *(ast->operands + ast->operandNum) = operand;
    ast->operandNum++;
  }
}

AST* fixAddopOrder(AST* ast)
{
  if(ast->astoken->sym == TS_ADD_OP || ast->astoken->sym == TS_MIN_OP)
  {
    if(ast->operandNum < 2) return ast;
    //otherwise there are 2 operands since it is a +/- operator
    AST* next_node = *(ast->operands + 1);
    if(next_node->astoken->sym != TS_ADD_OP && next_node->astoken->sym != TS_MIN_OP) return ast; //nothing out of order
    AST* next_node_op = *(next_node->operands);
    AST* newNode = buildAST(ast->astoken);
    addOperand(newNode, *(ast->operands));
    addOperand(newNode, next_node_op);

    AST* newHead = buildAST(next_node->astoken);
    addOperand(newHead, newNode);
    addOperand(newHead, *(next_node->operands + 1));
    return fixAddopOrder(newHead);
  }
  else
  {
    AST* newast = buildAST(ast->astoken);
    int i = 0;
    for(i = 0; i < ast->operandNum; i++)
    {
      addOperand(newast, fixAddopOrder(*(ast->operands + i)));
    }
    return newast;
  }
}

AST* fixMulopOrder(AST* ast)
{
  if(ast->astoken->sym == TS_MUL_OP || ast->astoken->sym == TS_DIV_OP)
  {
    if(ast->operandNum < 2) return ast;
    //otherwise there are 2 operands since it is a *// operator
    AST* next_node = *(ast->operands + 1);
    if(next_node->astoken->sym != TS_MUL_OP && next_node->astoken->sym != TS_DIV_OP) return ast; //nothing out of order
    AST* next_node_op = *(next_node->operands);
    AST* newNode = buildAST(ast->astoken);
    addOperand(newNode, *(ast->operands));
    addOperand(newNode, next_node_op);

    AST* newHead = buildAST(next_node->astoken);
    addOperand(newHead, newNode);
    addOperand(newHead, *(next_node->operands + 1));
    return fixMulopOrder(newHead);
  }
  else
  {
    AST* newast = buildAST(ast->astoken);
    int i = 0;
    for(i = 0; i < ast->operandNum; i++)
    {
      addOperand(newast, fixMulopOrder(*(ast->operands + i)));
    }
    return newast;
  }
}

AST* fixTree(AST* ast){ return fixMulopOrder(fixAddopOrder(ast)); }

ASToken* buildASToken(Symbol sym, char* lexeme)
{
  int lexemeLength = strlen(lexeme) + 1;
  char* lexemecpy = (char*)malloc(lexemeLength);
  memset(lexemecpy, 0, lexemeLength);
  memcpy(lexemecpy, lexeme, lexemeLength - 1); //the -1 makes sure the ending character is zero
  ASToken* astoken = (ASToken*)malloc(sizeof(struct astoken));
  astoken->sym = sym;
  astoken->lexeme = lexemecpy;
  return astoken;
}

int consumeToken()
{
  if(getNextToken(nextToken) != 0)
  {
    //i don't need to catch anything here ill check if the token is null or not in each function
  }
  return 0;
}

AST* parse()
{
  AST* prog = fixTree(nts_prog());
  if(validParseTree(prog) != 0) return NULL;//failed syntactic analysis
  return prog;
}

//LL(1) functions
AST* nts_prog()
{
  if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 1"));
  //printf("NTS_PROG >> ");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //if next is void or into
  if(strcmp(nextToken->lexeme, "void") == 0 ||
     strcmp(nextToken->lexeme, "int") == 0)
  {
    //go to nts_dec_lst
    AST* ast = buildAST(buildASToken(NTS_PROG, "program"));
    AST* dec_lst = nts_dec_lst();
    //add dec list to prog ast
    int i = 0;
    for(i = 0; i < dec_lst->operandNum; i++)
    {
      addOperand(ast, *(dec_lst->operands + i));
    }
    return ast;
  }
  else
  {
    //there is an error
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 2"));
  }
}
AST* nts_dec_lst()
{
  if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 3"));
  //printf("NTS_DEC_LST >> ");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //if next is void or into
  if(strcmp(nextToken->lexeme, "void") == 0 ||
     strcmp(nextToken->lexeme, "int") == 0)
  {
    AST* dec_lst = buildAST(buildASToken(NTS_DEC_LST, "declaration-list"));
    //dec
    AST* dec = nts_dec();
    //dec lst
    AST* dec_lst_1 = nts_dec_lst_1();
    //add dec and dec_lst_1 to dec_lst and return
    addOperand(dec_lst, dec);
    int i = 0;
    for(i = 0; i < dec_lst_1->operandNum; i++)
    {
      addOperand(dec_lst, *(dec_lst_1->operands + i));
    }
    return dec_lst;
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 4"));
  }
}
AST* nts_dec_lst_1()
{
  if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 5"));
  //printf("NTS_DEC_LST_1 >> ");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //if next tokenis null then we have hit the EOF
  if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_EOS, ""));

  //if next is void or into
  if(strcmp(nextToken->lexeme, "void") == 0 ||
     strcmp(nextToken->lexeme, "int") == 0)
  {
    AST* new_dec_lst_1 = buildAST(buildASToken(NTS_DEC_LST_1, "declaration-list-1"));
    //dec
    AST* dec = nts_dec();
    //dec lst
    AST* dec_lst_1 = nts_dec_lst_1();
    //add dec and dec_lst_1 to dec_lst and return
    addOperand(new_dec_lst_1, dec);
    int i = 0;
    for(i = 0; i < dec_lst_1->operandNum; i++)
    {
      addOperand(new_dec_lst_1, *(dec_lst_1->operands + i));
    }
    return new_dec_lst_1;
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 6"));
  }
}
AST* nts_dec()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 7"));
  //printf("NTS_DEC >> ");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //if next is void or int
  if(strcmp(nextToken->lexeme, "void") == 0 ||
     strcmp(nextToken->lexeme, "int") == 0)
  {
    //build declaration later since we don't exactly know which type it is var/func
    AST* type_spec = nts_type_spec();//consumes token
    //if next token is an id consume it
    if(nextToken->tokenType != ID) return buildAST(buildASToken(TS_INVALID, "parser defaulted 8")); //WE HAVE AN ERROR
    AST* id = buildAST(buildASToken(TS_ID, nextToken->lexeme));
    //consume token
    consumeToken();
    //dec 2
    AST* dec_1 = nts_dec_1();
    //build our dec ast
    AST* dec = buildAST(buildASToken(TS_INVALID, "parser defaulted 9"));
    if(dec_1->astoken->sym == TS_SC)//var-dec
    {
      dec = buildAST(buildASToken(NTS_VAR_DEC, "var-declaration"));
      addOperand(dec, type_spec);
      addOperand(dec, id);
    }
    else if(dec_1->astoken->sym == TS_NUM)//var_dec array
    {
      dec = buildAST(buildASToken(NTS_VAR_DEC, "var-declaration"));
      addOperand(dec, type_spec);
      addOperand(dec, id);
      addOperand(dec, dec_1);
    }
    else if(dec_1->astoken->sym == NTS_FUN_DEC)
    {
      dec = buildAST(buildASToken(NTS_FUN_DEC, "fun-declaration"));
      addOperand(dec, type_spec);
      addOperand(dec, id);
      //add the operands of nts_dec_1
      int i = 0;
      for(i = 0; i < dec_1->operandNum; i++)
      {
        addOperand(dec, *(dec_1->operands + i));
      }
    }
    return dec;
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 10"));
  }
}
AST* nts_dec_1()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 11"));
  //printf("dec_1");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  if(strcmp(nextToken->lexeme, "[") == 0)
  {
    //var-dec
    //consume '['
    consumeToken();
    if(nextToken->tokenType != NUM) return buildAST(buildASToken(TS_INVALID, "parser defaulted 12")); //WE HAVE AN ERROR
    //build 'num' ast
    AST* num = buildAST(buildASToken(TS_NUM, nextToken->lexeme));
    consumeToken();
    //consume ']'
    if(strcmp(nextToken->lexeme, "]") != 0) return buildAST(buildASToken(TS_INVALID, "parser defaulted 13"));
    consumeToken();
    //consume ';'
    if(strcmp(nextToken->lexeme, ";") != 0) return buildAST(buildASToken(TS_INVALID, "parser defaulted 14"));
    consumeToken();
    //so we matched everything we needed for this var-declaration
    return num;
  }
  else if(strcmp(nextToken->lexeme, ";") == 0)
  {
    //var-dec
    //consume semi colon
    consumeToken();
    return buildAST(buildASToken(TS_SC, ";"));
  }
  else if(strcmp(nextToken->lexeme, "(") == 0)
  {
    //fun-dec
    // ( NTS_PARAMS ) NTS_CMPD_STMT
    consumeToken();//consumes (
    AST* params = nts_params();
    //should have consumes all tokens up to ) at this point
    if(strcmp(nextToken->lexeme, ")") != 0) return buildAST(buildASToken(TS_INVALID, "parser defaulted 15"));
    consumeToken();//consumes )
    AST* cmpd_stmt = nts_cmpd_stmt();
    //build fun-dec
    AST* func = buildAST(buildASToken(NTS_FUN_DEC, "fun-declaration"));
    addOperand(func, params);
    addOperand(func, cmpd_stmt);
    return func;
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 16"));
  }
}
AST* nts_var_dec()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 17"));
  //printf("var_dec");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  if(strcmp(nextToken->lexeme, "void") == 0 ||
     strcmp(nextToken->lexeme, "int") == 0)
  {
    //build declaration later since we don't exactly know which type it is var/func
    AST* type_spec = nts_type_spec();
    //if next token is an id consume it
    if(nextToken->tokenType != ID) return buildAST(buildASToken(TS_INVALID, "parser defaulted 18")); //WE HAVE AN ERROR
    AST* id = buildAST(buildASToken(TS_ID, nextToken->lexeme));
    //consume token
    consumeToken();
    //dec 2
    AST* dec_1 = nts_var_dec_1();
    //build our dec ast
    AST* dec = buildAST(buildASToken(TS_INVALID, "parser defaulted 19"));
    if(dec_1->astoken->sym == NONE)//var-dec
    {
      dec = buildAST(buildASToken(NTS_VAR_DEC, "var-declaration"));
      addOperand(dec, type_spec);
      addOperand(dec, id);
    }
    else if(dec_1->astoken->sym == TS_NUM)//var_dec array
    {
      dec = buildAST(buildASToken(NTS_VAR_DEC, "var-declaration"));
      addOperand(dec, type_spec);
      addOperand(dec, id);
      addOperand(dec, dec_1);
    }
    return dec;
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 20"));
  }
}
AST* nts_var_dec_1()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 21"));
  //printf("var_dec_1");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  if(strcmp(nextToken->lexeme, "[") == 0)
  {
    //var-dec
    //consume '['
    consumeToken();
    if(nextToken->tokenType != NUM) return buildAST(buildASToken(TS_INVALID, "parser defaulted 22")); //WE HAVE AN ERROR
    //build 'num' ast
    AST* num = buildAST(buildASToken(TS_NUM, nextToken->lexeme));
    consumeToken();
    //consume ']'
    if(strcmp(nextToken->lexeme, "]") != 0) return buildAST(buildASToken(TS_INVALID, "parser defaulted 23"));
    consumeToken();
    //consume ';'
    if(strcmp(nextToken->lexeme, ";") != 0) return buildAST(buildASToken(TS_INVALID, "parser defaulted 24"));
    consumeToken();
    //so we matched everything we needed for this var-declaration
    return num;
  }
  else if(strcmp(nextToken->lexeme, ";") == 0)
  {
    //var-dec
    //consume semi colon I dont need to return the semicolon
    consumeToken();
    return buildAST(buildASToken(NONE, ";"));
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 25"));
  }
}
AST* nts_type_spec()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 26"));
  //printf("NTS_TYPE_SPEC >> ");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  if(strcmp(nextToken->lexeme, "void") == 0)
  {
    AST* v = buildAST(buildASToken(TS_VOID, "void"));
    consumeToken();
    return v;
  }
  else if(strcmp(nextToken->lexeme, "int") == 0)
  {
    AST* i = buildAST(buildASToken(TS_INT, "int"));
    consumeToken();
    return i;
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 27"));
  }
}
AST* nts_params()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 28"));
  //printf("params");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //int
  //params -> int param_list_1
  if(strcmp(nextToken->lexeme, "int") == 0)
  {
    AST* type = buildAST(buildASToken(TS_INT, "int"));
    //consume
    consumeToken();//consumes int
    if(nextToken->tokenType != ID) return buildAST(buildASToken(TS_INVALID, "parser defaulted 29"));
    AST* p_list_1 = nts_param_lst_1(); //this will have the id as the first operand and the rest of the params as the next operands
    //grab first operand
    AST* inc_param = *(p_list_1->operands); //might have multiple operands to it cause of id[] for example
    AST* param = buildAST(buildASToken(NTS_PARAM, "param"));
    addOperand(param, type);
    int i = 0;
    for(i = 0; i < inc_param->operandNum; i++)//wont have more than 2 iterations
    {
      addOperand(param, *(inc_param->operands + i));
    }
    //build params ast
    AST* params = buildAST(buildASToken(NTS_PARAMS, "params"));
    addOperand(params, param);
    //get rest of param list
    for(i = 1; i < p_list_1->operandNum; i++)//skip first operand as explained above
    {
      addOperand(params, *(p_list_1->operands + i));
    }
    return params;
  }
  //void
  //params -> void params_2
  else if(strcmp(nextToken->lexeme, "void") == 0)
  {
    AST* type = buildAST(buildASToken(TS_VOID, "void"));
    //consume
    consumeToken();
    if(nextToken->tokenType != ID &&
       strcmp(nextToken->lexeme, ")") != 0) return buildAST(buildASToken(TS_INVALID, "parser defaulted 30"));
    AST* params_2 = nts_params_2();
    AST* params = buildAST(buildASToken(NTS_PARAMS, "params"));
    if(params_2->astoken->sym != NONE)//if params_2 didn't return empty then it will return param_list_1
    {
      //first operand will be id of first param the rest will be fully constructed param asts
      AST* inc_param = *(params_2->operands); //might have multiple operands to it cause of id[] for example
      AST* param = buildAST(buildASToken(NTS_PARAM, "param"));
      addOperand(param, type);
      int i = 0;
      for(i = 0; i < inc_param->operandNum; i++)//wont have more than 2 iterations
      {
        addOperand(param, *(inc_param->operands + i));
      }
      //add param to params
      addOperand(params, param);
      //get rest of param list
      for(i = 1; i < params_2->operandNum; i++)//skip first operand as explained above
      {
        addOperand(params, *(params_2->operands + i));
      }
    }
    return params;
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 31"));
  }
}
AST* nts_params_2()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 32"));
  //printf("params_2");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //)
  //-> empty
  if(strcmp(nextToken->lexeme, ")") == 0)
  {
    return buildAST(buildASToken(NONE, ";"));
  }
  //id
  //-> param_list_1
  else if(nextToken->tokenType == ID)
  {
    AST* p_list_1 = nts_param_lst_1();
    return p_list_1;
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 33"));
  }
}
AST* nts_param_lst()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 34"));
  //printf("param_lst");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //,
  //-> , param param_list
  if(strcmp(nextToken->lexeme, ",") == 0)
  {
    //consume comma
    consumeToken();
    AST* param = nts_param();
    AST* param_lst = nts_param_lst();
    AST* result = buildAST(buildASToken(NTS_PARAMS, "params"));
    addOperand(result, param);
    int i = 0;
    for(i = 0; i < param_lst->operandNum; i++)
    {
      addOperand(result, *(param_lst->operands + i));
    }
    return result;
  }
  //)
  //-> EMPTY
  else if(strcmp(nextToken->lexeme, ")") == 0)
  {
    return buildAST(buildASToken(NONE, ";"));
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 35"));
  }
}
AST* nts_param_lst_1()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 36"));
  //printf("param_lst_1");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //id
  //-> param_2 param_list
  if(nextToken->tokenType == ID)
  {
    AST* param2 = nts_param_2(); //id + the end it of a parameter
    AST* param_lst = nts_param_lst();
    AST* result = buildAST(buildASToken(NTS_PARAMS, "params"));
    addOperand(result, param2);
    int i = 0;
    for(i = 0; i < param_lst->operandNum; i++)
    {
      addOperand(result, *(param_lst->operands + i));
    }
    return result;
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 37"));
  }
}
AST* nts_param()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 38"));
  //printf("param");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //void | int
  //-> type_spec id param_1
  if(strcmp(nextToken->lexeme, "void") == 0 ||
     strcmp(nextToken->lexeme, "int") == 0)
  {
    AST* type = nts_type_spec();
    if(nextToken->tokenType != ID) return buildAST(buildASToken(TS_INVALID, "parser defaulted 39"));
    AST* id = buildAST(buildASToken(TS_ID, nextToken->lexeme));
    consumeToken();//consumes id
    AST* pm_1 = nts_param_1();
    AST* param = buildAST(buildASToken(NTS_PARAM, "param"));
    addOperand(param, type);
    addOperand(param, id);
    if(pm_1->astoken->sym != NONE) addOperand(param, pm_1);
    return param;
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 40"));
  }
}
AST* nts_param_1()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 41"));
  //printf("param_1");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //,
  //-> empty
  if(strcmp(nextToken->lexeme, ",") == 0)
  {
    return buildAST(buildASToken(NONE, ";"));
  }
  //[
  //-> [ ]
  else if(strcmp(nextToken->lexeme, "[") == 0)
  {
    consumeToken();
    if(strcmp(nextToken->lexeme, "]") != 0) return buildAST(buildASToken(TS_INVALID, "parser defaulted 42"));
    consumeToken();
    return buildAST(buildASToken(TS_L_SQB, "\\[\\]"));
  }
  //)
  //-> )
  else if(strcmp(nextToken->lexeme, ")") == 0)
  {
    //just consume don't need to return the terminal
    //consumeToken(); //we dont want to consume here ugh
    return buildAST(buildASToken(NONE, ";"));
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 43"));
  }
}
AST* nts_param_2()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 44"));
  //printf("param_2");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //id
  //-> id param_1
  if(nextToken->tokenType == ID)
  {
    AST* param_no_type = buildAST(buildASToken(NTS_PARAM, "param"));
    AST* pid = buildAST(buildASToken(TS_ID, nextToken->lexeme));
    addOperand(param_no_type, pid);
    consumeToken();
    AST* end = nts_param_1();
    if(end->astoken->sym != NONE)//then we add the []
    {
      //then we have end as a TS_L_SQB "\[\]"
      addOperand(param_no_type, end);
    }
    return param_no_type;
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 45"));
  }
}
AST* nts_cmpd_stmt()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 46"));
  //printf("cmpd_stmt");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  if(strcmp(nextToken->lexeme,"{") == 0)
  {
    AST* cmpd_stmt = buildAST(buildASToken(NTS_CMPD_STMT, "compound-stmt"));
    consumeToken(); //consume {
    AST* loc_dec = nts_loc_dec();//returns a list of declarations
    AST* stmt_list = nts_stmt_lst();//returns a list of statements
    int i = 0;
    //add all of the local declarations
    for(i = 0; i < loc_dec->operandNum; i++)
    {
      addOperand(cmpd_stmt, *(loc_dec->operands + i));
    }
    //add all of the statements
    for(i = 0; i < stmt_list->operandNum; i++)
    {
      addOperand(cmpd_stmt, *(stmt_list->operands + i));
    }
    //if(loc_dec->operandNum == 0 && stmt_list->operandNum == 0) addOperand(cmpd_stmt, buildAST(buildASToken(NONE, ";")));
    if(strcmp(nextToken->lexeme, "}") != 0) return buildAST(buildASToken(TS_INVALID, "parser defaulted 47"));
    consumeToken(); //consume }
    return cmpd_stmt;
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 48"));
  }
}
AST* nts_loc_dec()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 49"));
  //printf("loc_dec");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //num | ( | id | ; | return | while | if | } | { | void | int
  //-> loc_dec_1
  if(nextToken->tokenType == NUM || nextToken->tokenType == ID ||
     (nextToken->tokenType == KEY && strcmp(nextToken->lexeme, "else") != 0) ||
     strcmp(nextToken->lexeme, "(") == 0 || strcmp(nextToken->lexeme, ";") == 0 ||
     strcmp(nextToken->lexeme, "}") == 0 || strcmp(nextToken->lexeme, "{") == 0)
  {
    AST* loc_dec = nts_loc_dec_1();
    return loc_dec;
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 50"));
  }
}
AST* nts_loc_dec_1()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 51"));
  //printf("loc_dec_1");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //num | ( | id | ; | return | while | if | } | {
  //-> empty
  if(nextToken->tokenType == NUM || nextToken->tokenType == ID ||
     strcmp(nextToken->lexeme, "return") == 0 || strcmp(nextToken->lexeme, "while") == 0 ||
     strcmp(nextToken->lexeme, "if") == 0 || strcmp(nextToken->lexeme, "(") == 0 ||
     strcmp(nextToken->lexeme, ";") == 0 || strcmp(nextToken->lexeme, "}") == 0 ||
     strcmp(nextToken->lexeme, "{") == 0)
  {
    return buildAST(buildASToken(NONE, ";"));
  }
  //void | int
  //-> nts_var_dec loc_dec_1
  else if(strcmp(nextToken->lexeme, "void") == 0 || strcmp(nextToken->lexeme, "int") == 0)
  {
    AST* var_dec = nts_var_dec();
    AST* loc_dec_1 = nts_loc_dec_1();
    AST* list = buildAST(buildASToken(NTS_LOC_DEC, "local-declarations"));
    addOperand(list, var_dec);
    int i = 0;
    for(i = 0; i < loc_dec_1->operandNum; i++)
    {
      addOperand(list, *(loc_dec_1->operands + i));
    }
    return list;
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 52"));
  }
}
AST* nts_stmt_lst()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 53"));
  //printf("stmt_lst>>");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //num | ( | id | ; | return | while | if | } | {
  //-> stmt_lst_1
  if(nextToken->tokenType == NUM || nextToken->tokenType == ID ||
     strcmp(nextToken->lexeme, "(") == 0 || strcmp(nextToken->lexeme, ";") == 0 ||
     strcmp(nextToken->lexeme, "return") == 0 || strcmp(nextToken->lexeme, "while") == 0 ||
     strcmp(nextToken->lexeme, "if") == 0 || strcmp(nextToken->lexeme, "}") == 0 ||
     strcmp(nextToken->lexeme, "{") == 0)
  {
    AST* stmt_lst = nts_stmt_lst_1();
    return stmt_lst;
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 54"));
  }
}
AST* nts_stmt_lst_1()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 55"));
  //printf("stmt_lst_1");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //num | ( | id | ; | return | while | if | {
  //-> stmt stmt_lst_1
  if(nextToken->tokenType == NUM || nextToken->tokenType == ID ||
     strcmp(nextToken->lexeme, "(") == 0 || strcmp(nextToken->lexeme, ";") == 0 ||
     strcmp(nextToken->lexeme, "return") == 0 || strcmp(nextToken->lexeme, "while") == 0 ||
     strcmp(nextToken->lexeme, "if") == 0 || strcmp(nextToken->lexeme, "{") == 0)
  {
    AST* stmt = nts_stmt();
    AST* stmt_lst_1 = nts_stmt_lst_1();
    AST* stmt_lst = buildAST(buildASToken(NTS_STMT_LST, "statement-list"));
    if(stmt->astoken->sym != NONE) addOperand(stmt_lst, stmt); //dont add empty statements
    int i = 0;
    for(i = 0; i < stmt_lst_1->operandNum; i++)
    {
      addOperand(stmt_lst, *(stmt_lst_1->operands + i));
    }
    return stmt_lst;
  }
  //}
  //->empty
  else if(strcmp(nextToken->lexeme, "}") == 0)
  {
    return buildAST(buildASToken(NONE, ";"));
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 56"));
  }
}
AST* nts_stmt()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 57"));
  //printf("stmt");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //num | ( | id | ;
  //->exp_stmt
  if(nextToken->tokenType == NUM || nextToken->tokenType == ID ||
     strcmp(nextToken->lexeme, "(") == 0 || strcmp(nextToken->lexeme, ";") == 0)
  {
    AST* exp_stmt = nts_exp_stmt();
    return exp_stmt;
  }
  //return
  //-> return_stmt
  else if(strcmp(nextToken->lexeme, "return") == 0)
  {
    AST* ret_stmt = nts_ret_stmt();
    return ret_stmt;
  }
  //while
  //-> iter_stmt
  else if(strcmp(nextToken->lexeme, "while") == 0)
  {
    AST* iter_stmt = nts_iter_stmt();
    return iter_stmt;
  }
  //if
  //-> sel_stmt
  else if(strcmp(nextToken->lexeme, "if") == 0)
  {
    AST* sel_stmt = nts_sel_stmt();
    return sel_stmt;
  }
  //{
  //-> cmpd_stmt
  else if(strcmp(nextToken->lexeme, "{") == 0)
  {
    AST* cmpd_stmt = nts_cmpd_stmt();
    return cmpd_stmt;
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 1-1"));
  }
}
AST* nts_exp_stmt()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 58"));
  //printf("exp_stmt");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //num | ( | id
  //->exp ;
  if(nextToken->tokenType == NUM || nextToken->tokenType == ID || strcmp(nextToken->lexeme, "(") == 0)
  {
    AST* exp = nts_exp();
    //AST* exp_stmt = buildAST(buildASToken(NTS_EXP_STMT, "expression-stmt"));
    //addOperand(exp_stmt, exp);
    //consume semicolon
    if(strcmp(nextToken->lexeme, ";") != 0) return buildAST(buildASToken(TS_INVALID, "parser defaulted 59"));
    consumeToken();
    return exp;
  }
  //;
  //->;
  else if(strcmp(nextToken->lexeme, ";") == 0)
  {
    consumeToken(); // consume ;
    return buildAST(buildASToken(TS_SC, ";"));
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 1-2"));
  }
}
AST* nts_sel_stmt()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 60"));
  //printf("sel_stmt");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //if
  //-> if ( expression ) sel_stmt_1
  if(strcmp(nextToken->lexeme, "if") == 0)
  {
    //consume if
    consumeToken();
    //consume (
    if(strcmp(nextToken->lexeme, "(") !=  0) return buildAST(buildASToken(TS_INVALID, "parser defaulted 61"));
    consumeToken();
    //EXPRESSION
    AST* exp = nts_exp();
    //consume )
    if(strcmp(nextToken->lexeme, ")") != 0) return buildAST(buildASToken(TS_INVALID, "parser defaulted 62"));
    consumeToken();
    //sel_stmt_1
    AST* sel_stmt_1 = nts_sel_stmt_1();
    AST* sel_stmt = buildAST(buildASToken(NTS_SEL_STMT, "selection-stmt"));
    addOperand(sel_stmt, exp);
    int i = 0;
    for(i = 0; i < sel_stmt_1->operandNum; i++)
    {
      addOperand(sel_stmt, *(sel_stmt_1->operands + i));
    }
    return sel_stmt;
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 1-3"));
  }
}
AST* nts_sel_stmt_1()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 63"));
  //printf("sel_stmt_1");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //num | ( | id | ; | return | while | if | {
  //-> stmt
  if(nextToken->tokenType == NUM || nextToken->tokenType == ID ||
     strcmp(nextToken->lexeme, "(") == 0 || strcmp(nextToken->lexeme, ";") == 0 ||
     strcmp(nextToken->lexeme, "return") == 0 || strcmp(nextToken->lexeme, "while") == 0 ||
     strcmp(nextToken->lexeme, "if") == 0 || strcmp(nextToken->lexeme, "{") == 0)
  {
    AST* stmt = nts_stmt();
    AST* sel_stmt_2 = nts_sel_stmt_2();
    //check if sel_stmt_2 has a statement or not
    AST* sel_stmt_lst = buildAST(buildASToken(NTS_STMT_LST, "statement-list"));
    addOperand(sel_stmt_lst, stmt);
    if(sel_stmt_2->astoken->sym != NONE) addOperand(sel_stmt_lst, sel_stmt_2);
    return sel_stmt_lst;
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 1-4"));
  }
}
AST* nts_sel_stmt_2()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 64"));
  //printf("sel_stmt_2");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //num | ( | id | ; | return | while | if | } | {
  //->empty
  if(nextToken->tokenType == NUM || nextToken->tokenType == ID ||
     strcmp(nextToken->lexeme, "(") == 0 || strcmp(nextToken->lexeme, ";") == 0 ||
     strcmp(nextToken->lexeme, "return") == 0 || strcmp(nextToken->lexeme, "while") == 0 ||
     strcmp(nextToken->lexeme, "if") == 0 || strcmp(nextToken->lexeme, "{") == 0 ||
     strcmp(nextToken->lexeme, "}") == 0)
  {
    return buildAST(buildASToken(NONE, ";"));//buildAST(buildASToken(TS_SC, ";"));
  }
  //else
  //-> else stmt
  else if(strcmp(nextToken->lexeme, "else") == 0)
  {
    consumeToken();//consume the else token
    AST* else_stmt = nts_stmt();
    //if(else_stmt->astoken->sym == NONE) else_stmt = buildAST(buildASToken(NONE, ";"));
    return else_stmt;
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 65"));
  }
}
AST* nts_iter_stmt()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 66"));
  //printf("iter_stmt");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //while
  //-> while ( EXPRESSION ) STATEMENT
  if(strcmp(nextToken->lexeme, "while") == 0)
  {
    consumeToken();//consume while
    if(strcmp(nextToken->lexeme, "(") != 0) return buildAST(buildASToken(TS_INVALID, "parser defaulted 67"));
    consumeToken();//consume (
    AST* exp = nts_exp();
    if(strcmp(nextToken->lexeme, ")") != 0) return buildAST(buildASToken(TS_INVALID, "parser defaulted 68"));
    consumeToken();//consume )
    AST* stmt = nts_stmt();
    AST* iter_stmt = buildAST(buildASToken(NTS_ITER_STMT, "iteration-stmt"));
    addOperand(iter_stmt, exp);
    addOperand(iter_stmt, stmt);
    return iter_stmt;
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 1-5"));
  }
}
AST* nts_ret_stmt()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 69"));
  //printf("ret_stmt");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //return
  //-> return ret_stmt_1
  if(strcmp(nextToken->lexeme, "return") == 0)
  {
    consumeToken();//consume return
    AST* ret_stmt = buildAST(buildASToken(NTS_RET_STMT, "return-stmt"));
    AST* ret_stmt_1 = nts_ret_stmt_1();
    if(ret_stmt_1->astoken->sym != NONE) addOperand(ret_stmt, ret_stmt_1);
    return ret_stmt;
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 1-6"));
  }
}
AST* nts_ret_stmt_1()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 70"));
  //printf("ret_stmt_1");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //num | ( | id
  //-> EXPRESSION ;
  if(nextToken->tokenType == NUM || nextToken->tokenType == ID ||
     strcmp(nextToken->lexeme, "(") == 0)
  {
    AST* exp = nts_exp();
    consumeToken(); // consume ;
    return exp;
  }
  //;
  //-> ;
  else if(strcmp(nextToken->lexeme, ";") == 0)
  {
    consumeToken();//consume ;
    return buildAST(buildASToken(NONE, ";"));
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 1-7"));
  }
}
AST* nts_exp()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 71"));
  //printf("exp");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //num | (
  //-> simp_ex
  if(nextToken->tokenType == NUM ||
     strcmp(nextToken->lexeme, "(") == 0)
  {
    AST* simp_exp = nts_simp_exp_1();
    return simp_exp;
  }
  //id
  //-> id exp1
  else if(nextToken->tokenType == ID)
  {
    //may need to do some assembling here
    AST* eid = buildAST(buildASToken(TS_ID, nextToken->lexeme));
    consumeToken();
    AST* exp1 = nts_exp_1();
    //if exp1 is a ts_ass_op then id is a var
    //if(exp1->astoken->sym == TS_ASS_OP && )
    //{
    //  AST* var = buildAST(buildASToken(NTS_VAR, "var"));
    //  addOperand(var, eid);
    //  AST* exp = buildAST(exp1->astoken);
    ///  addOperand(exp, var);
    //  addOperand(exp, *(exp1->operands));
    //  return exp;
    //}
    //else//otherwise we have to do a lot more work
    //{
      //find the factor_2 node will either be NONE, NUM, or ARGS
    if(exp1->astoken->sym == NTS_ARGS)
    {
      AST* call = buildAST(buildASToken(NTS_CALL, "call"));
      addOperand(call, eid);
      addOperand(call, exp1);
      return call;
    }
    else if(exp1->astoken->sym == NONE)
    {
      AST* var = buildAST(buildASToken(NTS_VAR, "var"));
      addOperand(var, eid);
      return var;
    }
    else if(exp1->astoken->sym == TS_L_SQB)
    {
      AST* var = buildAST(buildASToken(NTS_VAR, "var"));
      addOperand(var, eid);
      addOperand(var, *(exp1->operands));
      return var;
    }
    else
    {
      //the incomplete factor will always be the first operand
      //the furthest you have to parse is relop -> addop -> mulop
      AST* curr_node = exp1;
      AST* par_node = NULL;
      while(curr_node->astoken->sym != NONE && curr_node->astoken->sym != TS_L_SQB &&
            curr_node->astoken->sym != NTS_ARGS)
      {
        par_node = curr_node;
        curr_node = *(curr_node->operands);
      }
      if(curr_node->astoken->sym == NTS_ARGS)
      {
        AST* call = buildAST(buildASToken(NTS_CALL, "call"));
        addOperand(call, eid);
        addOperand(call, curr_node);
        *(par_node->operands) = call;
      }
      else if(curr_node->astoken->sym == NONE)
      {
        AST* var = buildAST(buildASToken(NTS_VAR, "var"));
        addOperand(var, eid);
        *(par_node->operands) = var;
      }
      else if(curr_node->astoken->sym == TS_L_SQB)
      {
        AST* var = buildAST(buildASToken(NTS_VAR, "var"));
        addOperand(var, eid);
        addOperand(var, *(curr_node->operands));
        *(par_node->operands) = var;
      }
      return exp1;
      //}
    }
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 1-8"));
  }
}
AST* nts_exp_1()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 72"));
  //printf("exp_1");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //, | [ | ) | ( | / | * | - | + | != | == | >= | > | < | <= | ;
  //-> simple_expression
  if(nextToken->tokenType == SYM &&
     (strcmp(nextToken->lexeme, "{") != 0 && strcmp(nextToken->lexeme, "}") != 0 &&
      strcmp(nextToken->lexeme, "]") != 0 && strcmp(nextToken->lexeme, "=") != 0))
  {
    AST* simp_exp = nts_simp_exp();
    return simp_exp;
  }
  //=
  //-> = expression
  else if(strcmp(nextToken->lexeme, "=") == 0)
  {
    AST* assop = buildAST(buildASToken(TS_ASS_OP, "="));
    consumeToken();
    AST* empty = buildAST(buildASToken(NONE, ";"));
    addOperand(assop, empty);
    AST* exp = nts_exp();
    addOperand(assop, exp);
    return assop;
  }
  else if(strcmp(nextToken->lexeme, "]") == 0)
  {
    return buildAST(buildASToken(NONE, ";"));
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 1-9"));
  }
}
AST* nts_simp_exp()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 73"));
  //printf("simp_exp");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //, | [ | ) | ( | / | * | - | + | != | == | >= | > | < | <= | ;
  //-> add_ex_2 simp_exp_2
  if(nextToken->tokenType == SYM &&
     (strcmp(nextToken->lexeme, "{") != 0 && strcmp(nextToken->lexeme, "}") != 0 &&
      strcmp(nextToken->lexeme, "]") != 0 && strcmp(nextToken->lexeme, "=") != 0))
  {
    AST* add_exp_2 = nts_add_exp_2();
    AST* simp_exp_2 = nts_simp_exp_2();
    if(simp_exp_2->astoken->sym == TS_ASS_OP)
    {
      AST* assop = buildAST(simp_exp_2->astoken);
      addOperand(assop, add_exp_2);
      addOperand(assop, *(simp_exp_2->operands));
      return assop;
    }
    else if(simp_exp_2->astoken->sym != NONE)
    {
      AST* relop = buildAST(simp_exp_2->astoken);
      addOperand(relop, add_exp_2);
      //simp_exp_2 should have an operand but just incase it doesnt
      if(simp_exp_2->operandNum != 0) addOperand(relop, *(simp_exp_2->operands));
      return relop;
    }
    else
    {
      return add_exp_2;
    }
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 1-0"));
  }
}
AST* nts_simp_exp_1()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 74"));
  //printf("simp_exp_1");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //num | (
  //-> add_exp_3 simp_ex_2
  if(nextToken->tokenType == NUM || strcmp(nextToken->lexeme, "(") == 0)
  {
    AST* add_exp = nts_add_exp_3();
    //if simp_ex_2 != NONE then its a relop
    AST* simp_exp_2 = nts_simp_exp_2();
    if(simp_exp_2->astoken->sym != NONE)
    {
      AST* relop = buildAST(simp_exp_2->astoken);
      addOperand(relop, add_exp);
      //simp_exp_2 will either have 1 or 0 operands so check if it has one
      if(simp_exp_2->operandNum != 0) addOperand(relop, *(simp_exp_2->operands));
      return relop;
    }
    else
    {
      return add_exp;
    }
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 1-10"));
  }
}
AST* nts_simp_exp_2()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 75"));
  //printf("simp_exp_2");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //, | ) | ;
  //-> empty
  if(strcmp(nextToken->lexeme, ",") == 0 || strcmp(nextToken->lexeme, ")") == 0 ||
     strcmp(nextToken->lexeme, ";") == 0 || strcmp(nextToken->lexeme, "]") == 0)
  {
    //dont consume
    return buildAST(buildASToken(NONE, ";"));//buildAST(buildASToken(TS_SC, ";"));
  }
  //!= | == | >= | > | < | <=
  //-> relop add_exp
  else if(strcmp(nextToken->lexeme, "!=") == 0 || strcmp(nextToken->lexeme, "==") == 0 ||
          strcmp(nextToken->lexeme, ">=") == 0 || strcmp(nextToken->lexeme, ">") == 0 ||
          strcmp(nextToken->lexeme, "<") == 0 || strcmp(nextToken->lexeme, "<=") == 0)
  {
    AST* relop = nts_relop();
    AST* add_exp = nts_add_exp();
    addOperand(relop, add_exp);
    return relop;
  }
  //=
  //-> = expression
  else if(strcmp(nextToken->lexeme, "=") == 0)
  {
    AST* assop = buildAST(buildASToken(TS_ASS_OP, "="));
    consumeToken(); //consume =
    AST* exp = nts_exp();
    addOperand(assop, exp);
    return assop;
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 1-11"));
  }
}
AST* nts_relop()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 76"));
  //printf("relop");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //!= | == | >= | > | < | <=
  if(strcmp(nextToken->lexeme, "!=") == 0)
  {
    //consumeToken return ts
    consumeToken();
    return buildAST(buildASToken(TS_NEQ_OP, "!="));
  }
  else if(strcmp(nextToken->lexeme, "==") == 0)
  {
    consumeToken();
    return buildAST(buildASToken(TS_EQ_OP, "=="));
  }
  else if(strcmp(nextToken->lexeme, ">=") == 0)
  {
    //consumeToken return ts
    consumeToken();
    return buildAST(buildASToken(TS_GTE_OP, ">="));
  }
  else if(strcmp(nextToken->lexeme, ">") == 0)
  {
    consumeToken();
    return buildAST(buildASToken(TS_GT_OP, ">"));
  }
  else if(strcmp(nextToken->lexeme, "<") == 0)
  {
    //consumeToken return ts
    consumeToken();
    return buildAST(buildASToken(TS_LT_OP, "<"));
  }
  else if(strcmp(nextToken->lexeme, "<=") == 0)
  {
    consumeToken();
    return buildAST(buildASToken(TS_LTE_OP, "<="));
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 77"));
  }
}
AST* nts_add_exp()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 78"));
  //printf("add_exp");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //( | num
  //-> term3 add_exp_1
  if(nextToken->tokenType == NUM || strcmp(nextToken->lexeme, "(") == 0)
  {
    //need to do some building HERE
    AST* term = nts_term_3();
    AST* add_exp_1 = nts_add_exp_1();
    if(add_exp_1->astoken->sym != NONE)
    {
      AST* add_exp = buildAST(add_exp_1->astoken);
      addOperand(add_exp, term);
      addOperand(add_exp, *(add_exp_1->operands));
      return add_exp;
    }
    else
    {
      return term;
    }
  }
  //id
  //-> id term2 add_exp_1
  else if(nextToken->tokenType == ID)
  {
    AST* id = buildAST(buildASToken(TS_ID, nextToken->lexeme));
    consumeToken();//consume id
    AST* term_2 = nts_term_2();
    //term_2 is either a mulop with a factor_2 as the first operand or it id a nts_factor_2
    //put id into term_2
    if(term_2->astoken->sym == TS_MUL_OP || term_2->astoken->sym == TS_DIV_OP)
    {
      AST* factor_2 = *(term_2->operands);
      AST* new_factor_2 = buildAST(buildASToken(TS_INVALID, "parser defaulted 79"));
      if(factor_2->astoken->sym == NONE)
      {
        new_factor_2 = buildAST(buildASToken(NTS_VAR, "var"));
        addOperand(new_factor_2, id);
      }
      //if factor 2 is a num then it is a var[exp]
      else if(factor_2->astoken->sym == TS_L_SQB)
      {
        new_factor_2 = buildAST(buildASToken(NTS_VAR, "var"));
        addOperand(new_factor_2, id);
        addOperand(new_factor_2, *(factor_2->operands));
      }
      //if factor 2 is args then it is a call
      else if(factor_2->astoken->sym == NTS_ARGS)
      {
        new_factor_2 = buildAST(buildASToken(NTS_CALL, "call"));
        addOperand(new_factor_2, id);
        addOperand(new_factor_2, factor_2);
      }
      *(term_2->operands) = new_factor_2;
    }
    else//factor_2
    {
      AST* new_term_2 = buildAST(buildASToken(TS_INVALID, "parser defaulted 80"));
      if(term_2->astoken->sym == NONE)
      {
        new_term_2 = buildAST(buildASToken(NTS_VAR, "var"));
        addOperand(new_term_2, id);
      }
      //if factor 2 is a num then it is a var[num]
      else if(term_2->astoken->sym == TS_L_SQB)
      {
        new_term_2 = buildAST(buildASToken(NTS_VAR, "var"));
        addOperand(new_term_2, id);
        addOperand(new_term_2, *(term_2->operands));
      }
      //if factor 2 is args then it is a call
      else if(term_2->astoken->sym == NTS_ARGS)
      {
        new_term_2 = buildAST(buildASToken(NTS_CALL, "call"));
        addOperand(new_term_2, id);
        addOperand(new_term_2, term_2);
      }
      term_2 = new_term_2;
    }
    AST* add_exp_1 = nts_add_exp_1();
    //create new add_exp
    if(add_exp_1->astoken->sym == NONE) return term_2;
    AST* add_exp = buildAST(add_exp_1->astoken);
    addOperand(add_exp, term_2);
    int i = 0;
    for(i = 0; i > add_exp_1->operandNum; i++)
    {
      addOperand(add_exp, *(add_exp_1->operands + i));
    }
    return add_exp;
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 81"));
  }
}
AST* nts_add_exp_1()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 82"));
  //printf("add_exp_1");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //, | ) | != | == | >= | > | < | <= | ;
  //->EMPTY
  if(strcmp(nextToken->lexeme, ",") == 0 || strcmp(nextToken->lexeme, ")") == 0 ||
     strcmp(nextToken->lexeme, ";") == 0 || strcmp(nextToken->lexeme, "!=") == 0 ||
     strcmp(nextToken->lexeme, "==") == 0 || strcmp(nextToken->lexeme, ">=") == 0 ||
     strcmp(nextToken->lexeme, ">") == 0 || strcmp(nextToken->lexeme, "<") == 0 ||
     strcmp(nextToken->lexeme, "<=") == 0 || strcmp(nextToken->lexeme, "=") == 0 ||
     strcmp(nextToken->lexeme, "]") == 0)
  {
    return buildAST(buildASToken(NONE, ";"));//buildAST(buildASToken(TS_SC, ";"));
  }
  // + | -
  //-> addop term add_exp_1
  else if(strcmp(nextToken->lexeme, "+") == 0 || strcmp(nextToken->lexeme, "-") == 0)
  {
    AST* addop = nts_addop();
    AST* term = nts_term();
    AST* add_exp_1 = nts_add_exp_1();
    if(add_exp_1->astoken->sym != NONE)
    {
      AST* add_exp = buildAST(add_exp_1->astoken);
      addOperand(add_exp, term);
      addOperand(add_exp, *(add_exp_1->operands));
      addOperand(addop, add_exp);
    }
    else
    {
      addOperand(addop, term);
    }
    return addop;
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 83"));
  }
}
AST* nts_add_exp_2()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 84"));
  //printf("add_exp_2");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //, | [ | ) | ( | / | * | - | + | != | == | >= | > | < | <= | ;
  //-> term_2 add_exp_1
  if(nextToken->tokenType == SYM &&
     (strcmp(nextToken->lexeme, "{") != 0 && strcmp(nextToken->lexeme, "}") != 0 &&
      strcmp(nextToken->lexeme, "]") != 0 && strcmp(nextToken->lexeme, "=") != 0))
  {
    AST* term_2 = nts_term_2();
    AST* add_exp_1 = nts_add_exp_1();
    if(add_exp_1->astoken->sym != NONE)
    {
      AST* add_exp = buildAST(add_exp_1->astoken);
      addOperand(add_exp, term_2);
      int i = 0;
      for(i = 0; i < add_exp_1->operandNum; i++)
      {
        addOperand(add_exp, *(add_exp_1->operands + i));
      }
      return add_exp;
    }
    else
    {
      return term_2;
    }
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 85"));
  }
}
AST* nts_add_exp_3()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 86"));
  //printf("add_exp_3");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //num | (
  //-> term3 add_exp_1
  if(nextToken->tokenType == NUM || strcmp(nextToken->lexeme, "(") == 0)
  {
    AST* term_3 = nts_term_3();
    AST* add_exp_1 = nts_add_exp_1();
    if(add_exp_1->astoken->sym != NONE)
    {
      AST* add_exp = buildAST(add_exp_1->astoken);
      addOperand(add_exp, term_3);
      int i = 0;
      for(i = 0; i < add_exp_1->operandNum; i++)
      {
        addOperand(add_exp, *(add_exp_1->operands + i));
      }
      return add_exp;
    }
    else
    {
      return term_3;
    }
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 87"));
  }
}
AST* nts_addop()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 88"));
  //printf("addop");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //+ | -
  if(strcmp(nextToken->lexeme, "+") == 0)
  {
    //consumeToken return ts
    consumeToken();
    return buildAST(buildASToken(TS_ADD_OP, "+"));
  }
  else if(strcmp(nextToken->lexeme, "-") == 0)
  {
    consumeToken();
    return buildAST(buildASToken(TS_MIN_OP, "-"));
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 89"));
  }
}
AST* nts_term()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 90"));
  //printf("term");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //num | (
  //-> factor_1 term_1
  if(nextToken->tokenType == NUM || strcmp(nextToken->lexeme, "(") == 0)
  {
    AST* factor_1 = nts_factor_1();
    AST* term_1 = nts_term_1();
    //term1 is either empty or a TS_MUL_OP
    if(term_1->astoken->sym != NONE)
    {
      AST* mulop = buildAST(term_1->astoken);
      addOperand(mulop, factor_1);
      int i = 0;
      for(i = 0; i < term_1->operandNum; i++)
      {
        addOperand(mulop, *(term_1->operands + i));
      }
      return mulop;
    }
    else
    {
      return factor_1;
    }
  }
  //id
  //-> id factor_2 term_1
  else if(nextToken->tokenType == ID)
  {
    AST* id = buildAST(buildASToken(TS_ID, nextToken->lexeme));
    consumeToken();//consume id
    AST* factor_2 = nts_factor_2();
    AST* term_1 = nts_term_1();
    //combine id and factor_2;
    AST* factor = buildAST(buildASToken(TS_INVALID, "parser defaulted 91"));
    if(factor_2->astoken->sym == NONE)
    {
      factor = buildAST(buildASToken(NTS_VAR, "var"));
      addOperand(factor, id);
    }
    else if(factor_2->astoken->sym == TS_L_SQB)
    {
      factor = buildAST(buildASToken(NTS_VAR, "var"));
      addOperand(factor, id);
      addOperand(factor, *(factor_2->operands));
    }
    else if(factor_2->astoken->sym == NTS_ARGS)
    {
      factor = buildAST(buildASToken(NTS_CALL, "call"));
      addOperand(factor, id);
      addOperand(factor, factor_2);
    }
    //term_1 is either empty or a mulop
    if(term_1->astoken->sym != NONE)
    {
      AST* mulop = buildAST(term_1->astoken);
      addOperand(mulop, factor);
      int i = 0;
      for(i = 0; i < term_1->operandNum; i++)
      {
        addOperand(mulop, *(term_1->operands + i));
      }
      return mulop;
    }
    else
    {
      return factor;
    }
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 92"));
  }
}
AST* nts_term_1()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 93"));
  //printf("term_1>>");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //, | ) | - | + | != | == | >= | > | < | <= | ;
  //-> empty
  if(strcmp(nextToken->lexeme, ",") == 0 || strcmp(nextToken->lexeme, ")") == 0 ||
     strcmp(nextToken->lexeme, ";") == 0 || strcmp(nextToken->lexeme, "!=") == 0 ||
     strcmp(nextToken->lexeme, "==") == 0 || strcmp(nextToken->lexeme, ">=") == 0 ||
     strcmp(nextToken->lexeme, ">") == 0 || strcmp(nextToken->lexeme, "<") == 0 ||
     strcmp(nextToken->lexeme, "<=") == 0 || strcmp(nextToken->lexeme, "+") == 0 ||
     strcmp(nextToken->lexeme, "-") == 0 || strcmp(nextToken->lexeme, "=") == 0 ||
     strcmp(nextToken->lexeme, "]") == 0)
  {
    return buildAST(buildASToken(NONE, ";"));//buildAST(buildASToken(TS_SC, ";"));
  }
  // * | /
  //-> mulop factor term_1
  else if(strcmp(nextToken->lexeme, "*") == 0 || strcmp(nextToken->lexeme, "/") == 0)
  {
    //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
    AST* mulop = nts_mulop();
    AST* factor = nts_factor();
    AST* term_1 = nts_term_1();
    if(term_1->astoken->sym != NONE)
    {
      AST* term = buildAST(term_1->astoken);
      addOperand(term, factor);
      if(term_1->operandNum != 0) addOperand(term, *(term_1->operands));
      addOperand(mulop, term);
    }
    else
    {
      addOperand(mulop, factor);
    }
    return mulop;
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 94"));
  }
}
AST* nts_term_2()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 95"));
  //printf("term_2");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //, | [ | ) | ( | / | * | - | + | != | == | >= | > | < | <= | ;
  //-> factor_2 term_1
  if(nextToken->tokenType == SYM &&
     (strcmp(nextToken->lexeme, "{") != 0 && strcmp(nextToken->lexeme, "}") != 0 &&
      strcmp(nextToken->lexeme, "]") != 0 && strcmp(nextToken->lexeme, "=") != 0))
  {
    AST* factor_2 = nts_factor_2();
    AST* term_1 = nts_term_1();
    if(term_1->astoken->sym != NONE)
    {
      AST* mulop = buildAST(term_1->astoken);
      addOperand(mulop, factor_2);
      //term_1 will alwasy have one operand but chek if it has none just in case
      if(term_1->operandNum != 0) addOperand(mulop, *(term_1->operands));
      return mulop;
    }
    else
    {
      return factor_2;
    }
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 96"));
  }
}
AST* nts_term_3()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 97"));
  //printf("term_3");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //( | num
  //-> factor_1 term_1
  if(nextToken->tokenType == NUM | strcmp(nextToken->lexeme, "(") == 0)
  {
    AST* factor_1 = nts_factor_1();
    AST* term_1 = nts_term_1();
    if(term_1->astoken->sym != NONE)
    {
      AST* mulop = buildAST(term_1->astoken);
      addOperand(mulop, factor_1);
      //term_1 will alwasy have one operand but chek if it has none just in case
      if(term_1->operandNum != 0) addOperand(mulop, *(term_1->operands));
      return mulop;
    }
    else
    {
      return factor_1;
    }
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 98"));
  }
}
AST* nts_mulop()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 99"));
  //printf("mulop>>");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
 // * | /
  if(strcmp(nextToken->lexeme, "*") == 0)
  {
    //consumeToken return ts
    consumeToken();
    return buildAST(buildASToken(TS_MUL_OP, "*"));
  }
  else if(strcmp(nextToken->lexeme, "/") == 0)
  {
    consumeToken();
    return buildAST(buildASToken(TS_DIV_OP, "/"));
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 100"));
  }
}
AST* nts_factor()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 101"));
  //printf("factor>>");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //num
  //->num
  if(nextToken->tokenType == NUM)
  {
    //printf("We gpt a num here 1847\n");
    //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
    AST* num = buildAST(buildASToken(TS_NUM, nextToken->lexeme));
    consumeToken(); // consume num
    return num;
  }
  //(
  //->( expression )
  else if(strcmp(nextToken->lexeme, "(") == 0)
  {
    consumeToken();//consume (
    AST* exp = nts_exp();
    if(strcmp(nextToken->lexeme, ")") != 0) return buildAST(buildASToken(TS_INVALID, "parser defaulted 102"));
    consumeToken(); //consume )
    return exp;
  }
  //id
  //-> id factor_2
  else if(nextToken->tokenType == ID)
  {
    AST* id = buildAST(buildASToken(TS_ID, nextToken->lexeme));
    consumeToken();//consume id
    AST* factor_2 = nts_factor_2();
    //if factor 2 is empty then it is a var
    if(factor_2->astoken->sym == NONE)
    {
      AST* var = buildAST(buildASToken(NTS_VAR, "var"));
      addOperand(var, id);
      return var;
    }
    //if factor 2 is a num then it is a var[num]
    else if(factor_2->astoken->sym == TS_NUM)
    {
      AST* var = buildAST(buildASToken(NTS_VAR, "var"));
      addOperand(var, id);
      addOperand(var, factor_2);
      return var;
    }
    //if factor 2 is args then it is a call
    else if(factor_2->astoken->sym == NTS_ARGS)
    {
      AST* call = buildAST(buildASToken(NTS_CALL, "call"));
      addOperand(call, id);
      addOperand(call, factor_2);
      return call;
    }
    else//error
    {
      AST* err = buildAST(buildASToken(NTS_VAR, "var"));
      addOperand(err, id);
      addOperand(err, factor_2);
      return err;
    }
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 103"));
  }
}
AST* nts_factor_1()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 104"));
  //printf("factor_1");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //num
  //-> num
  if(nextToken->tokenType == NUM)
  {
    //printf("We gpt a num here 1916\n");
    //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
    AST* num = buildAST(buildASToken(TS_NUM, nextToken->lexeme));
    consumeToken();
    return num;
  }
  //(
  //-> ( expression )
  else if(strcmp(nextToken->lexeme, "(") == 0)
  {
    consumeToken();//consume (
    AST* exp = nts_exp();
    if(strcmp(nextToken->lexeme, ")") != 0) return buildAST(buildASToken(TS_INVALID, "parser defaulted 105"));
    consumeToken(); //consume )
    return exp;
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 106"));
  }
}
AST* nts_factor_2()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 107"));
  //printf("factor_2");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  // , | ) | / | * | - | + | != | == | >= | > | < | <= | ;
  //-> empty
  if(strcmp(nextToken->lexeme, ",") == 0 || strcmp(nextToken->lexeme, ")") == 0 ||
     strcmp(nextToken->lexeme, ";") == 0 || strcmp(nextToken->lexeme, "!=") == 0 ||
     strcmp(nextToken->lexeme, "==") == 0 || strcmp(nextToken->lexeme, ">=") == 0 ||
     strcmp(nextToken->lexeme, ">") == 0 || strcmp(nextToken->lexeme, "<") == 0 ||
     strcmp(nextToken->lexeme, "<=") == 0 || strcmp(nextToken->lexeme, "+") == 0 ||
     strcmp(nextToken->lexeme, "-") == 0 || strcmp(nextToken->lexeme, "*") == 0 ||
     strcmp(nextToken->lexeme, "/") == 0)
  {
    return buildAST(buildASToken(NONE, ";"));//buildAST(buildASToken(TS_SC, ";"));
  }
  //[
  //-> [ EXPRESSION ]
  else if(strcmp(nextToken->lexeme, "[") == 0)
  {
    consumeToken(); //consume [
    //if(nextToken->tokenType != NUM) return buildAST(buildASToken(TS_INVALID, "parser defaulted 108"));
    AST* sqb = buildAST(buildASToken(TS_L_SQB, "["));
    AST* exp = nts_exp();
    addOperand(sqb, exp);
    if(strcmp(nextToken->lexeme, "]") != 0) return buildAST(buildASToken(TS_INVALID, "parser defaulted 109"));
    consumeToken();//consume ]
    return sqb;
  }
  //(
  //-> ( args )
  else if(strcmp(nextToken->lexeme, "(") == 0)
  {
    consumeToken(); //consume (
    AST* args = nts_args();
    if(strcmp(nextToken->lexeme, ")") != 0) return buildAST(buildASToken(TS_INVALID, "parser defaulted 110"));
    consumeToken();//consume )
    return args;
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 111"));
  }
}
//AST* nts_call()
//{
//
//}
AST* nts_args()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 112"));
  //printf("args");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //num | id | (
  //->arg_list
  if(nextToken->tokenType == NUM || nextToken->tokenType == ID ||
     strcmp(nextToken->lexeme, "(") == 0)
  {
    AST* arg_list = nts_arg_lst();
    AST* args = buildAST(buildASToken(NTS_ARGS, "args"));
    int i = 0;
    for(i = 0; i < arg_list->operandNum; i++)
    {
      addOperand(args, *(arg_list->operands + i));
    }
    return args;
  }
  // )
  //->empty
  else if(strcmp(nextToken->lexeme, ")") == 0)
  {
    //returns an empty args object so we can tell it is a call later on
    return buildAST(buildASToken(NTS_ARGS, "args"));
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 113"));
  }
}
AST* nts_arg_lst()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 114"));
  //printf("arg_list");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //num | id | (
  //-> expression arg_list_1
  if(nextToken->tokenType == NUM || nextToken->tokenType == ID ||
     strcmp(nextToken->lexeme, "(") == 0)
  {
    AST* exp = nts_exp();
    AST* arg_list_1 = nts_arg_lst_1();
    AST* arg_list = buildAST(buildASToken(NTS_ARG_LST, "arg-list"));
    addOperand(arg_list, exp);
    int i = 0;
    for(i = 0; i < arg_list_1->operandNum; i++)
    {
      addOperand(arg_list, *(arg_list_1->operands + i));
    }
    return arg_list;
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 115"));
  }
}
AST* nts_arg_lst_1()
{
	if(nextToken->lexeme == NULL) return buildAST(buildASToken(TS_INVALID, "parser defaulted 116"));
  //printf("arg_lst_1");
  //printToken(nextToken->lineNumber, nextToken->tokenType, nextToken->lexeme);
  //printf("\n");
  //)
  //-> empty
  if(strcmp(nextToken->lexeme, ")") == 0)
  {
    return buildAST(buildASToken(NONE, ";"));
  }
  //,
  //-> , expression arg_list_1
  else if(strcmp(nextToken->lexeme, ",") == 0)
  {
    consumeToken(); //consume ,
    AST* exp = nts_exp();
    AST* arg_list_1 = nts_arg_lst_1();
    AST* arg_list =  buildAST(buildASToken(NTS_ARG_LST, "arg-list"));
    addOperand(arg_list, exp);
    int i = 0;
    for(i = 0; i < arg_list_1->operandNum; i++)
    {
      addOperand(arg_list, *(arg_list_1->operands + i));
    }
    return arg_list;
  }
  else
  {
    return buildAST(buildASToken(TS_INVALID, "parser defaulted 117"));
  }
}
