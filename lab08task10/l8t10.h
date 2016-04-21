/**
 * Name: Mattias Huber
 * Lab/task: lab 08 Task 8
 * Date: 04/14/2016
 **/
#ifndef __l8t10_h_
#define __l8t10_h_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include "y.tab.h"

int yyparse(void);
int yylex(void);
void yyerror(char *);

typedef enum { NUM_TYPE=0, FUNC_TYPE, SYMBOL_TYPE, SYMBOL_TABLE_TYPE, CONDITION_TYPE, CUSTOM_FUNCTION_TYPE} AST_NODE_TYPE;

//consider making symbol table struct? currently uses linked list

typedef struct
{
    long longValue;
    double value;
} NUMBER_AST_NODE;

typedef struct
{
    char *name;
    struct ast_node *op1;
    struct ast_node *op2;
} FUNCTION_AST_NODE;

typedef struct
{
    char *symbolName;
} SYMBOL_AST_NODE;

typedef struct
{
    struct ast_node *conditionNode;
    struct ast_node *s_expr1;
    struct ast_node *s_expr2;
}CONDITION_AST_NODE;

typedef struct
{
    char *symbolName;
    char *type;
    struct ast_node *symbolValue; 
    struct ast_node *nextSymbol;
} SYMBOL_TABLE_AST_NODE;

typedef struct
{
    char *functionName;
    struct ast_node *arg_list;
} CUSTOM_FUNCTION_NODE;

typedef struct ast_node
{
    AST_NODE_TYPE type;
    struct ast_node *scope;
    struct ast_node *parent;
    char *datatype;
    union
    {
        NUMBER_AST_NODE number;
        FUNCTION_AST_NODE function;
        SYMBOL_AST_NODE symbol;
        SYMBOL_TABLE_AST_NODE symbolTable;
        CONDITION_AST_NODE condition;
        CUSTOM_FUNCTION_NODE customFunction;
    } data;
} AST_NODE;

AST_NODE *number(double value);

AST_NODE *function(char *funcName, AST_NODE *op1, AST_NODE *op2);

AST_NODE *createSymbol(char *symbolName);

AST_NODE *symbolLookup(AST_NODE *parentNode, char *searchName);

AST_NODE *createScope(AST_NODE *rootSymbol, AST_NODE *s_expr);

AST_NODE *createLetElem(char *letElemType, char *symbolName, AST_NODE *s_expr);

//creates function with s_expr as value, arg_list as s_expr->scope, in let statement
AST_NODE *createFunction(char *myFunc, AST_NODE *arg_list, AST_NODE *s_expr);

AST_NODE *createArgList(char *insert, AST_NODE *argList);

AST_NODE *symbolTableInsert(AST_NODE *rootNode, AST_NODE *insertSymbol);

AST_NODE *s_exprTableInsert(AST_NODE *rootNode, AST_NODE *insertSymbol);

//creates the CUSTOM_FUNCTION_NODE for evaluation in AST, not in let statement
AST_NODE *createFunctionNode(char *functionName, AST_NODE *s_expr_list);

AST_NODE *functionLookup(char *searchName, AST_NODE *parentNode);

AST_NODE *conditional(AST_NODE *condition, AST_NODE *s_expr1, AST_NODE *s_expr2);

void freeNode(AST_NODE *p);

void traverseSymbolTable();

double eval(AST_NODE *ast);

long longEval(AST_NODE *ast);

void printvalue(double value);

#endif
