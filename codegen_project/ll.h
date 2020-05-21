#ifndef __LL_H__
#define __LL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum symbol {
  //Terminal Symbols
  TS_ID,         // identifier token
  TS_NUM,        // number token
  TS_ADD_OP,     // +
  TS_MIN_OP,     // -
  TS_MUL_OP,     // *
  TS_DIV_OP,     // /
  TS_LT_OP,      // <
  TS_LTE_OP,     // <=
  TS_GT_OP,      // >
  TS_GTE_OP,     // >=
  TS_EQ_OP,      // ==
  TS_NEQ_OP,     // !=
  TS_ASS_OP,     // =
  TS_SC,         // ;
  TS_C,          // ,
  TS_L_PAREN,    // (
  TS_R_PAREN,    // )
  TS_L_SQB,      // [
  TS_R_SQB,      // ]
  TS_L_CB,       // {
  TS_R_CB,       // }
  TS_ELSE,       // else
  TS_IF,         // if
  TS_INT,        // int
  TS_RETURN,     // return
  TS_WHILE,      // while
  TS_VOID,       // void
  TS_EOS,        // $ <-- end of sequence
  TS_INVALID,    // parser defaulted

  //Non-Terminal Symbols
  NTS_PROG,      // program
  NTS_DEC_LST,   // declaration list
  NTS_DEC_LST_1,
  NTS_DEC,       // declaration
  NTS_DEC_1,
  NTS_VAR_DEC,   // variable declaration
  NTS_VAR_DEC_1,
  NTS_VAR_DEC_2,
  NTS_TYPE_SPEC, // type specifier
  NTS_FUN_DEC,   // function declaration //not used i guess
  NTS_PARAMS,    // parameters
  NTS_PARAMS_2,
  NTS_PARAM_LST, // parameter list
  NTS_PARAM_LST_1,
  NTS_PARAM,     // parameter
  NTS_PARAM_1,
  NTS_PARAM_2,
  NTS_CMPD_STMT, // compound statement
  NTS_LOC_DEC,   // local declaration
  NTS_LOC_DEC_1,
  NTS_STMT_LST,  // statement list
  NTS_STMT_LST_1,
  NTS_STMT,      // statement
  NTS_EXP_STMT,  // expression statement
  NTS_SEL_STMT,  // selection statement
  NTS_SEL_STMT_1,
  NTS_ITER_STMT, // iteration statement
  NTS_RET_STMT,  // return statement
  NTS_RET_STMT_1,
  NTS_EXP,       // expression
  NTS_EXP_1,
  NTS_VAR,       // variable
  //NTS_VAR_1,
  NTS_SIMP_EXP,  // simple expression
  NTS_SIMP_EXP_1,
  NTS_SIMP_EXP_2,
  NTS_RELOP,     // relational operator
  NTS_ADD_EXP,   // additive expression
  NTS_ADD_EXP_1,
  NTS_ADD_EXP_2,
  NTS_ADD_EXP_3,
  NTS_ADDOP,     // additive operation
  NTS_TERM,      // term
  NTS_TERM_1,
  NTS_TERM_2,
  NTS_TERM_3,
  NTS_MULOP,     // multiplicative operation
  NTS_FACTOR,    // factor
  NTS_FACTOR_1,
  NTS_FACTOR_2,
  NTS_CALL,      // funciton call
  NTS_ARGS,      // arguments
  NTS_ARG_LST,   // argument list
  NTS_ARG_LST_1,
  //Error symbol
  NONE          // No Symbol
}Symbol;

typedef struct astoken
{
  Symbol sym; //Token Symbol (Terminal/Non-Terminal)
  char* lexeme; //Value of the symbol (= lexeme of token from lexer)
}ASToken;

//AST Struct and helper functions
typedef struct ast
{
  ASToken* astoken; //symbol for operator
  int operandNum; //number of operands
  struct ast** operands;//pointer to operands array
}AST;

AST* buildTerminalAST(ASToken* astoken);
AST* buildAST(ASToken* astoken);
ASToken* buildASToken(Symbol sym, char* lexeme);
void addOperand(AST* ast, AST* operand);

AST* fixTree(AST* ast);

int startParser();
AST* parse();

int validParseTree(AST* ast);

void printAST(AST* ast, int level);
void fprintAST(FILE* file, AST* ast, int level);

/*
 *  Symbol Stack Data Structure
 *  -implements a basic stack for the enum symbol
 *  -uses a linked list sort of structure
 */
typedef struct stack{
  ASToken astoken;
  struct stack* next;
}Stack;

//Initializes the stack with symbol sym
Stack* initStack(ASToken* astoken);
//Pushes the symbol sym on the stack
void push(Stack* head, ASToken* astoken);
//Pops the top symbol off the stack
ASToken* pop(Stack* head);
//Shows value of the top symbol on the stack
ASToken* peek(Stack* head);
/*
 * End Symbol Stack Implementation
 */

//LL(1) functions

AST* nts_prog();
AST* nts_dec_lst();
AST* nts_dec_lst_1();
AST* nts_dec();
AST* nts_dec_1();
AST* nts_var_dec();
AST* nts_var_dec_1();
AST* nts_var_dec_2();
AST* nts_type_spec();
AST* nts_params();
AST* nts_params_2();
AST* nts_param_lst();
AST* nts_param_lst_1();
AST* nts_param();
AST* nts_param_1();
AST* nts_param_2();
AST* nts_cmpd_stmt();
AST* nts_loc_dec();
AST* nts_loc_dec_1();
AST* nts_stmt_lst();
AST* nts_stmt_lst_1();
AST* nts_stmt();
AST* nts_exp_stmt();
AST* nts_sel_stmt();
AST* nts_sel_stmt_1();
AST* nts_sel_stmt_2();
AST* nts_iter_stmt();
AST* nts_ret_stmt();
AST* nts_ret_stmt_1();
AST* nts_exp();
AST* nts_exp_1();
AST* nts_simp_exp();
AST* nts_simp_exp_1();
AST* nts_simp_exp_2();
AST* nts_relop();
AST* nts_add_exp();
AST* nts_add_exp_1();
AST* nts_add_exp_2();
AST* nts_add_exp_3();
AST* nts_addop();
AST* nts_term();
AST* nts_term_1();
AST* nts_term_2();
AST* nts_term_3();
AST* nts_mulop();
AST* nts_factor();
AST* nts_factor_1();
AST* nts_factor_2();
AST* nts_call();
AST* nts_args();
AST* nts_arg_lst();
AST* nts_arg_lst_1();

#endif /*__LL_H__*/
