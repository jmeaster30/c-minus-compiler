#include "codegen.h"

FILE* output = NULL;
Frame* main_frame = NULL;

int functionNum = 0;
int stackSize = 0;
int arrnum = 0;
int branchnum = 0;
int iternum = 0;


//initializes code gen stuff
int startCodeGen(const char* outputFile)
{
  output = fopen(outputFile, "w+");
  if(output == NULL) return 1;
  //create global frame
  Frame* nf = (Frame*)malloc(sizeof(struct frame));
  memset(nf, 0, sizeof(struct frame));
  main_frame = nf;
  return 0;
}

int stopCodeGen()
{
  fclose(output);
  return 0;
}

//shrinkby is the number of variables we want to get rid of
void shrinkFrame(int shrinkby)
{
  if(shrinkby <= 0) return;
  //realloc the arrays
  int newsize = main_frame->varnum - shrinkby;
  main_frame->varnames = realloc(main_frame->varnames, newsize * sizeof(void*));
  main_frame->varoffs = realloc(main_frame->varoffs, newsize * sizeof(int));
  main_frame->varsizes = realloc(main_frame->varsizes, newsize * sizeof(int));
  main_frame->vartypes = realloc(main_frame->vartypes, newsize * sizeof(int));
  main_frame->varnum = newsize;
}

void addToFrame(char* vname, int size, char type)
{
  //get last offset and add 4bytes * last size
  //if curr_frame->varoffs == NULL we are adding the first element so offset is 4
  int offset = stackSize - 4;
  //copy vname
  int vnamelen = strlen(vname);
  char* vnameCopy = (char*)malloc(vnamelen + 1);
  memset(vnameCopy, 0, vnamelen);
  memcpy(vnameCopy, vname, vnamelen);
  //expand varnames and offsets
  if(main_frame->varnum == 0)
  {
    main_frame->varnames = (char**)malloc(sizeof(void*));
    *(main_frame->varnames) = vnameCopy;

    main_frame->varoffs = (int*)malloc(sizeof(int));
    *(main_frame->varoffs) = offset;

    main_frame->varsizes = (int*)malloc(sizeof(int));
    *(main_frame->varsizes) = size;

    main_frame->vartypes = (char*)malloc(sizeof(char));
    *(main_frame->vartypes) = type;

    main_frame->varnum++;
  }else{
    //if operandNum = n the nth index of the array will be free
    char** newnames = (char**)malloc(sizeof(void*) * (main_frame->varnum + 1));
    memset(newnames, 0, sizeof(void*) * (main_frame->varnum + 1));
    memcpy(newnames, main_frame->varnames, sizeof(void*) * main_frame->varnum);
    //free(main_frame->varnames);
    main_frame->varnames = newnames;
    *(main_frame->varnames + main_frame->varnum) = vnameCopy;

    int* newoffs = (int*)malloc(sizeof(int) * (main_frame->varnum + 1));
    memset(newoffs, 0, sizeof(int) * (main_frame->varnum + 1));
    memcpy(newoffs, main_frame->varoffs, sizeof(int) * main_frame->varnum);
    //free(main_frame->varoffs);
    main_frame->varoffs = newoffs;
    *(main_frame->varoffs + main_frame->varnum) = offset;

    int* newsizes = (int*)malloc(sizeof(int) * (main_frame->varnum + 1));
    memset(newsizes, 0, sizeof(int) * (main_frame->varnum + 1));
    memcpy(newsizes, main_frame->varsizes, sizeof(int) * main_frame->varnum);
    //free(main_frame->varsizes);
    main_frame->varsizes = newsizes;
    *(main_frame->varsizes + main_frame->varnum) = size;

    char* newtypes = (char*)malloc(sizeof(char) * (main_frame->varnum + 1));
    memset(newtypes, 0, sizeof(char) * (main_frame->varnum + 1));
    memcpy(newtypes, main_frame->vartypes, sizeof(char) * main_frame->varnum);
    //free(main_frame->vartypes);
    main_frame->vartypes = newtypes;
    *(main_frame->vartypes + main_frame->varnum) = type;

    main_frame->varnum++;
  }
}

int getIndex(char* vname)
{
  //printf("getting index\n");
  if(main_frame == NULL) return -1;
  if(main_frame->varnames == NULL) return -1;
  if(main_frame->varnum == 0) return -1;
  //printf("good to start\n");
  int i = main_frame->varnum - 1;
  for(i = main_frame->varnum - 1; i >= 0 ; i--)
  {
    //printf("looping\n");
    if(strcmp(*((main_frame->varnames) + i), vname) == 0)
    {
      //printf("done\n");
      break;
    }
  }
  //printf("got index\n");
  return i;
}

//this gives the offset from the bottom of the stack
//so no negative values unless the variable is global
int getOffset(char* vname)
{
  int i = 0;
  for(i = main_frame->varnum - 1; i >= 0 ; i--)
    if(strcmp(*(main_frame->varnames + i), vname) == 0) break;
  if(i < 0)
  {
    return -1;
  }
  return *(main_frame->varoffs + i);
}

char getType(char* vname)
{
  int i = 0;
  for(i = main_frame->varnum - 1; i >= 0 ; i--)
    if(strcmp(*(main_frame->varnames + i), vname) == 0) break;
  if(i < 0)
  {
    return -1;
  }
  return *(main_frame->vartypes + i);
}


//code gen stuff
int cgen(AST* ast);

int cg_prog(AST* ast)
{
  //printf("start program\n");
  //add in data segment
  //printf("data seg\n");
  fprintf(output, ".data\n");
  fprintf(output, ".align 2\n");
  int i = 0;
  for(i = 0; i < ast->operandNum; i++)
  {
    AST* op = *(ast->operands + i);
    if(op->astoken->sym == NTS_VAR_DEC)
    {
      //printf("var_dec: %s", (*(op->operands + 1))->astoken->lexeme);
      //we cant use cg_vardec here since this is a static global variable
      fprintf(output, "%s: .space", (*(op->operands + 1))->astoken->lexeme);    //////WE NEED TO ADD THIS STUFF TO A SCOPE or frame??
      //if operandNum = 3 then we have an array otherwise we have a regular int
      int space = 1;
      if(op->operandNum == 3)
      {
        char* end;
        int arrsize = strtol((*(ast->operands + 2))->astoken->lexeme, &end, 10);
        space = arrsize;
        addToFrame((*(op->operands + 1))->astoken->lexeme, arrsize, '\1');
      }else{
        addToFrame((*(op->operands + 1))->astoken->lexeme, 4, '\0');
      }
      fprintf(output, " %d\n", 4 * space);
      //printf("\n");
    }
  }
  //printf("text seg\n");
  //add in text segment
  fprintf(output, "\n.text\n");
  //add in the rest of the functions
  for(i = 0; i < ast->operandNum; i++)
  {
    AST* op = *(ast->operands + i);
    if(op->astoken->sym == NTS_FUN_DEC)
    {
      stackSize += 4;
      //printf("fun_dec: %s %s", (*(op->operands))->astoken->lexeme, (*(op->operands + 1))->astoken->lexeme);
      fprintf(output, "%s: \n", (*(op->operands + 1))->astoken->lexeme);
      //the call will set up the new (asm)frame and we will have memory (with $fp -> $sp) [old frame pointer][p1][p2]...[pn][ra]
      AST* params = *(op->operands + 2);
      //save param num
      int paramnum = params->operandNum;
      //add parameters to (c)frame
      int j = 0;
      for(j = 0; j < paramnum; j++)
      {
        AST* param = *(params->operands + j);
        stackSize += 4;
        addToFrame((*(param->operands + 1))->astoken->lexeme, 1, '\0');
      }
      //stack size should return to 0 at the end;
      //this stack size increment is to find the argumetns from the call
      //printf(" %i\n", params->operandNum);
      //printf("local dec\n");
      AST* body = *(op->operands + 3);
      //count local declarations
      int locdecnum = 0;
      for(j = 0; j < body->operandNum; j++)
        if((*(body->operands + j))->astoken->sym != NTS_VAR_DEC) break;
      locdecnum = j;
      //add local declarations
      stackSize -= 4;
      fprintf(output, "addiu $sp, $sp, -%d\n", 4 + locdecnum * 4);
      fprintf(output, "sw $ra, 0($sp)\n");
      for(j = 0; j < locdecnum; j++)
      {
        AST* locdec = *(body->operands + j);
        int size = 4;
        char type = '\0';
        if(locdec->operandNum == 3)
        {
          char* end;
          int index = 4 * strtol((*(locdec->operands + 2))->astoken->lexeme, &end, 10);
          fprintf(output, "li $a0, %d\n", index);
          fprintf(output, "li $v0, 9 #syscall 9 (sbrk)\n");
          fprintf(output, "syscall\n");
          fprintf(output, "add $a0, $v0, $0\n");
          fprintf(output, "sw $a0, %d($sp)\n", 4 * j + 4);
          type = '\1';
          //size = arraysize;<-this might always be 4 since we are just keeping track of an address rather than the whole array
        }
        //add new var to frame
        stackSize += 4;
        addToFrame((*(locdec->operands + 1))->astoken->lexeme, size, type);
      }
      stackSize += 4;
      //generate the function body
      //printf("statments\n");
      functionNum = i + 1;
      for(; j < body->operandNum; j++)
        cgen(*(body->operands + j));
      //printf("done with the body\n");
      //here our stack should not have grown at all
      fprintf(output, ".RET%d:\n", functionNum);
      //return address will be at the top of the stack
      fprintf(output, "lw $ra 0($sp)\n");
      //reset frame pointer and stack pointer
      stackSize -= locdecnum * 4;
      fprintf(output, "addiu $sp $sp %d\n", 4 + locdecnum * 4);
      //lose local vars
      shrinkFrame(locdecnum);
      //lose the parameters
      shrinkFrame(paramnum);//removes the parameters that were added in the funciton call
      //shrink stack size
      stackSize -= 4 * paramnum;
      fprintf(output, "jr $ra\n");
      //will reduce stacksize back in the call
      //printf("done fun_dec: %s\n", (*(op->operands + 1))->astoken->lexeme);
    }
  }
  fprintf(output, "\n.text\n");
  fprintf(output, ".globl __crt_out_of_bounds\n");
  fprintf(output, ".globl output\n");
  fprintf(output, ".globl input\n\n");
  fprintf(output, "__crt_out_of_bounds: \n"
                  "li $v0, 10 # exit\n"
                  "syscall\n");
  fprintf(output, "output: \n"
                  "li $v0, 1 # print_int\n"
                  "syscall\n"
                  "li $a0, 0x0a\n"
                  "li $v0, 11 #print_char\n"
                  "syscall\n"
                  "jr $ra\n");
  fprintf(output, "input: \n"
                  "li $v0, 5 #read_int\n"
                  "syscall\n"
                  "move $a0, $v0\n"
                  "jr $ra\n");
  return 0;
}

int cg_var_dec(AST* ast)
{
  //printf("start var_dec %s\n", (*(ast->operands + 1))->astoken->lexeme);
  //gaenerate variable
  int size = 4;
  char type = '\0';
  if(ast->operandNum == 3)
  {
    //allocate enough heap memory
    //a0 will be heap address
    //we know that *(ast->operands + 2)->astoken->lexeme is always a numberhere
    char* end;
    int index = strtol((*(ast->operands + 2))->astoken->lexeme, &end, 10);
    int arraysize = index * 4;
    fprintf(output, "addiu $a0 $0 %d\n", arraysize);
    fprintf(output, "addiu $v0 $0 9 #syscall 9 (sbrk)\n");
    fprintf(output, "syscall\n");
    //address is in $v0 so move it to $a0
    fprintf(output, "add $a0 $v0 $0\n");
    type = '\1';
    //size = arraysize;<-this might always be 4 since we are just keeping track of an address rather than the whole array
  }
  else
  {

    type = '\0';
  }
  //pop $ra
  fprintf(output, "lw $ra 0($sp)\n");
  //push new var
  //fprintf(output, "sw $a0 0($s0)\n");
  //push $ra back on
  fprintf(output, "addiu $sp $sp -4\n");
  fprintf(output, "sw $t1 0($s0)\n");
  //add new var to frame
  stackSize += 4;
  addToFrame((*(ast->operands + 1))->astoken->lexeme, size, type);
  //printf("done var_dec %s\n", (*(ast->operands + 1))->astoken->lexeme);
  return 0;
}

//(asm) ends with the address of the variable in t1 and the value in a0
//the variable in t1 should have no negative side effects since we are always going to assume it is empty
//except in the case of ts_ass_op since we will be using this function exactly for the value in t1
int cg_var(AST* ast)
{
  //printf("start var %s\n", (*(ast->operands))->astoken->lexeme);
  //printAST(ast, 0);
  char* name = (*(ast->operands))->astoken->lexeme;
  //printf(">>%s\n", name);
  int varindex = getIndex(name);
  //printf("%d\n", varindex);
  if(varindex < 0) return -1;//something went wrong
  int varoff = *(main_frame->varoffs + varindex);
  //printf("%d\n", varoff);
  int vartype = *(main_frame->vartypes + varindex);
  //printf("%d\n", vartype);
  //printf("stack size: %d\n", stackSize);
  //so a0 will always have the address of the variable in it
  if(ast->operandNum == 2)
  {
    //printf("heheh %d\n", ast->operandNum);
    cgen(*(ast->operands + 1));
    //so now we have the index in $a0
    //check if out of bounds
    arrnum += 1;
    fprintf(output, "bgezal $a0, .ERR_%d\n", arrnum);
    fprintf(output, "j __crt_out_of_bounds\n");
    fprintf(output, ".ERR_%d:\n", arrnum);
    fprintf(output, "sll $a0, $a0, 2\n");
    //load address into t1
    if(varoff >= 0){
      fprintf(output, "addi $t1, $sp, %d\n", stackSize - varoff);
      //add a0 and t1 to get the offset address
      fprintf(output, "add $t1, $t1, $a0\n");
      //t1 has the address of the word I want
      fprintf(output, "lw $a0, 4($t1)\n");
    }else{
      fprintf(output, "la $t1, %s\n", name);
      //add a0 and t1 to get the offset address
      fprintf(output, "add $t1, $t1, $a0\n");
      //t1 has the address of the word I want
      fprintf(output, "lw $a0, 0($t1)\n");
    }
  }
  else//we have a variable or just a reference to an array
  {
    //load address into t1
    if(varoff >= 0){
      fprintf(output, "addi $t1, $sp, %d\n", stackSize - varoff);
      if(vartype == 0){
        fprintf(output, "lw $a0, 4($t1)\n");
      }else{
        fprintf(output, "add $a0 $t1 $0\n");
      }
    }else{
      fprintf(output, "la $t1, %s\n", name);
      if(vartype == 0){
        fprintf(output, "lw $a0, 0($t1)\n");
      }else{
        fprintf(output, "add $a0 $t1 $0\n");
      }
    }
  }
  //printf("done var %s\n", (*(ast->operands))->astoken->lexeme);
  return 0;
}

int cg_call(AST* ast)
{
  //printf("start call %s\n", (*(ast->operands))->astoken->lexeme);
  //save parameters
  AST* args = *(ast->operands + 1);
  fprintf(output, "addiu $sp $sp -%d\n", 4 * args->operandNum);
  int i = 0;
  for(i = 0; i < args->operandNum; i++)
  {
    //printf("start arg:\n");
    cgen(*(args->operands + i));
    fprintf(output, "sw $a0 %d($sp)\n", 4 * i);
    //printf("end arg:\n");
  }
  //jump
  fprintf(output, "jal %s\n", (*(ast->operands))->astoken->lexeme);
  fprintf(output, "addiu $sp, $sp %d\n", 4 * args->operandNum);
  //printf("end call %s\n", (*(ast->operands))->astoken->lexeme);
  return 0;
}

int cg_sel(AST* ast)
{
  //generate code for EXPRESSION
  AST* exp = *(ast->operands);
  AST* ifbranch = *(ast->operands + 1);
  AST* elsebranch = (ast->operandNum == 3) ? *(ast->operands + 2) : NULL;
  branchnum++;
  int thisnum = branchnum;
  cgen(exp);
  fprintf(output, "bne $a0 $0 true_branch_%d\n", thisnum);//$a0 will be 1 or a nonzero integer if the exp was evaluated to tru
  //else branch here
  cgen(elsebranch);//will just return if NULL
  fprintf(output, "b end_if_%d\n", thisnum);
  fprintf(output, "true_branch_%d:\n", thisnum);
  cgen(ifbranch);
  fprintf(output, "end_if_%d:\n", thisnum);
  return 0;
}

int cg_iter(AST* ast)
{
  AST* exp = *(ast->operands);
  AST* body = *(ast->operands + 1);
  iternum++;
  int thisnum = iternum;
  fprintf(output, "iter_start_%d:\n", thisnum);
  cgen(exp);
  fprintf(output, "beq $a0 $0 iter_end_%d\n", thisnum);
  cgen(body);
  fprintf(output, "j iter_start_%d\n", thisnum);
  fprintf(output, "iter_end_%d:\n", thisnum);
  return 0;
}

int cg_ret(AST* ast)
{
  //printf("ret stmt\n");
  if(ast->operandNum != 0)
  {
    AST* exp = *(ast->operands);
    //get result of exp in $a0
    cgen(exp);
    //result will be in $a0
  }
  fprintf(output, "j .RET%d\n", functionNum);
  //printf("done ret stmt\n");
  return 0;
}

int cg_cmpd(AST* ast)
{
  //printf("start cmpd stmt\n");
  int prevScopeEnd = main_frame->varnum;
  //this section is copied from the function section
  //count local declarations
  int j = 0;
  int locdecnum = 0;
  for(j = 0; j < ast->operandNum; j++)
    if((*(ast->operands + j))->astoken->sym != NTS_VAR_DEC) break;
  locdecnum = j;
  //add local declarations
  stackSize -= 4;
  fprintf(output, "addiu $sp, $sp, -%d\n", 4 + locdecnum * 4);
  fprintf(output, "sw $ra, 0($sp)\n");
  for(j = 0; j < locdecnum; j++)
  {
    AST* locdec = *(ast->operands + j);
    int size = 4;
    char type = '\0';
    if(locdec->operandNum == 3)
    {
      char* end;
      int index = 4 * strtol((*(locdec->operands + 2))->astoken->lexeme, &end, 10);
      fprintf(output, "li $a0, %d\n", index);
      fprintf(output, "li $v0, 9 #syscall 9 (sbrk)\n");
      fprintf(output, "syscall\n");
      fprintf(output, "add $a0, $v0, $0\n");
      fprintf(output, "sw $a0, %d($sp)\n", 4 * j + 4);
      type = '\1';
      //size = arraysize;<-this might always be 4 since we are just keeping track of an address rather than the whole array
    }
    //add new var to frame
    stackSize += 4;
    addToFrame((*(locdec->operands + 1))->astoken->lexeme, size, type);
  }
  stackSize += 4;
  //printf("statments\n");
  for(; j < ast->operandNum; j++)
    cgen(*(ast->operands + j));
  //printf("done with statements\n");
  //the stack size will be the same as before all of these statements
  //reset stack pointer
  stackSize -= locdecnum * 4;
  fprintf(output, "addiu $sp $sp %d\n", 4 + locdecnum * 4);
  //lose local vars
  shrinkFrame(locdecnum);
  //if(main_frame->varnum == prevScopeEnd) //printf(":)\n");
  return 0;
}

//$a0 and $t1 can be thrown out
int cg_ass(AST* ast)
{
  //printf("start ass op\n");
  AST* lhs = *(ast->operands);
  AST* rhs = *(ast->operands + 1);
  cgen(lhs);
  //get address to variable in memory instead of value
  //save $t1 on stack
  fprintf(output, "addiu $sp $sp -4\n");
  stackSize += 4;
  fprintf(output, "sw $t1 0($sp)\n");
  cgen(rhs);
  //so we should have a fully calulated value in $a0
  //load the address of the variable back into t1 and sw
  fprintf(output, "lw $t1 0($sp)\n");
  fprintf(output, "addiu $sp $sp 4\n");
  stackSize -= 4;
  fprintf(output, "sw $a0 0($t1)\n");
  //printf("done ass op\n");
  return 0;
}

//(asm) result is in a0
int cg_op(AST* ast)
{
  //printf("start op %s\n", ast->astoken->lexeme);
  AST* lhs = *(ast->operands);
  AST* rhs = *(ast->operands + 1);
  cgen(lhs);
  fprintf(output, "addiu $sp $sp -4\n");
  stackSize += 4;
  fprintf(output, "sw $a0 0($sp)\n");
  cgen(rhs);
  fprintf(output, "lw $t1 0($sp)\n");
  //lhs is in $t1 rhs is in $a0
  switch(ast->astoken->sym)
  {
    case TS_ADD_OP:
      fprintf(output, "add $a0 $t1 $a0\n");
      break;
    case TS_MIN_OP:
      fprintf(output, "sub $a0 $t1 $a0\n");
      break;
    case TS_MUL_OP:
      fprintf(output, "mult $t1 $a0\n");
      fprintf(output, "mflo $a0\n");
      break;
    case TS_DIV_OP:
      fprintf(output, "div $t1 $a0\n");
      fprintf(output, "mflo $a0\n");
      break;
    case TS_LT_OP:
      fprintf(output, "slt $a0 $t1 $a0\n");
      break;
    case TS_LTE_OP:
      fprintf(output, "sle $a0 $t1 $a0\n");
      break;
    case TS_GT_OP:
      fprintf(output, "sgt $a0 $t1 $a0\n");
      break;
    case TS_GTE_OP:
      fprintf(output, "sge $a0 $t1 $a0\n");
      break;
    case TS_EQ_OP:
      fprintf(output, "seq $a0 $t1 $a0\n");
      break;
    case TS_NEQ_OP:
      fprintf(output, "sne $a0 $t1 $a0\n");
      break;
    default:
      break;
  }

  stackSize -= 4;
  fprintf(output, "addiu $sp $sp 4\n");
  //printf("end op %s\n", ast->astoken->lexeme);
  return 0;
}


int cg_num(AST* ast)
{
  //printf("NUM: %s\n", ast->astoken->lexeme);
  fprintf(output, "li $a0 %s\n", ast->astoken->lexeme);
  return 0;
}

int cg_id(AST* ast)
{
  char* name = ast->astoken->lexeme;
  //printf("ID: %s\n", name);
  int voffset = getOffset(name);
  if(voffset < 0)
  {
    //printf("OFFSET: global\n");
    //grab globally defined variable
    fprintf(output, "la $a0, %s\n", name);
    return 0;
  }
  int soff = stackSize - voffset;
  //printf("OFFSET: (%d)\n", voffset);
  fprintf(output, "la $a0, %d($sp)\n", soff);
  //printf("ID DONE\n");
  return 0;
}

int cgen(AST* ast)
{
  if(ast == NULL) return 0;
  int i = 0;//for loop counter
  switch(ast->astoken->sym)
  {
    case NTS_PROG:
      cg_prog(ast);
      break;
    case TS_ADD_OP:
    case TS_MIN_OP:
    case TS_MUL_OP:
    case TS_DIV_OP:
    case TS_LT_OP:
    case TS_LTE_OP:
    case TS_GT_OP:
    case TS_GTE_OP:
    case TS_EQ_OP:
    case TS_NEQ_OP:
      cg_op(ast);
      break;
    case NTS_VAR_DEC:
      cg_var_dec(ast);
      break;
    //case NTS_FUN_DEC:
    //  cg_fun_dec(ast);
    //  break;
    case NTS_ITER_STMT:
      cg_iter(ast);
      break;
    case NTS_SEL_STMT:
      cg_sel(ast);
      break;
    case NTS_RET_STMT:
      cg_ret(ast);
      break;
    case NTS_CMPD_STMT:
      cg_cmpd(ast);
      break;
    case NTS_VAR:
      cg_var(ast);
      break;
    case TS_ASS_OP:
      cg_ass(ast);
      break;
    case TS_NUM:
      cg_num(ast);
      break;
    case TS_ID:
      cg_id(ast);
      break;
    case NTS_CALL:
      cg_call(ast);
      break;
    default:
      //printf("here->%s\n", ast->astoken->lexeme);
      //for(i = 0; i < ast->operandNum; i++)
      //{
      //  cgen(*(ast->operands + i));
      //}
      break;
  }
  return 0;
}

int gencode(AST* ast)
{
  //printf("start code gen\n");
  cgen(ast);
  //printf("end code gen\n");
  return 0;
}
