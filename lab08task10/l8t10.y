/**
 * Name: Mattias Huber
 * Lab/task: lab 08 Task 10
 * Date: 04/14/2016
 **/
 
/*

program ::= | program s_expr EOL
s_expr ::=
( cond s_expr s_expr s_expr )
| ( ( let let_list ) s_expr )
| ( func s-expr_list )
| ( symbol s-expr_list )
| symbol
| number
s-expr_list ::= | s-expr_list s-expr
let_list ::= let_elem | let_list let_elem
let_elem ::= ( symbol s-expr ) | ( type symbol s-expr ) | ( symbol ( arg_list ) s-expr )
type ::= integer | real
arg_list ::= symbol arg_list |
symbol ::= letter+
letter ::= [a-zA-Z]
func ::= neg|abs|exp|sqrt|add|sub|mult|div|remainder|log|pow|max|min|exp2|cbrt|hypot|rand|read|print|equal|smaller|larger
number ::= [+|-]?digit+[.number+]
digit ::= [0-9]

*/

%{
#include "l8t10.h"
%}

%union
{
    double dval;
    char *sval;
    struct ast_node *astNode;
};

%token <sval> FUNC
%token <sval> SYMBOL
%token <sval> INTEGER
%token <sval> REAL
%token <dval> NUMBER
%token LPAREN RPAREN EOL QUIT LET COND

%type <astNode> s_expr
%type <astNode> s_expr_list
%type <astNode> let_list
%type <astNode> let_elem
%type <astNode> arg_list

%%

program:/* empty */ { 
                       printf("> ");
                    }
        | program s_expr EOL {
                              eval($2);
                              //printf("Program output: \n"); 
                              printvalue(eval($2));
                              freeNode($2);
                              printf("\n> "); 
                           }
        ;

s_expr:
        NUMBER { 
                  //printf("yacc: NUMBER%lf", $1); 
                  $$ = number($1); 
               }
        | LPAREN LPAREN LET let_list RPAREN s_expr RPAREN
        {
            $$=createScope($4, $6);
        }
        | SYMBOL {
                    $$ = createSymbol($1);
                }
        | LPAREN FUNC RPAREN {
                                    $$ = function($2, 0, 0);
                                    }
        | LPAREN FUNC s_expr RPAREN { 
                                    // printf("yacc: LPAREN FUNC expr RPAREN\n"); 
                                    $$ = function($2, $3, 0);  
                                    //printf("%s(%lf)", $2, $3);
                                    }
        | LPAREN FUNC s_expr s_expr RPAREN {
                                    // printf("LPAREN FUNC expr expr RPAREN\n"); 
                                    $$ = function($2, $3, $4);
                                    }
        | LPAREN COND s_expr s_expr s_expr RPAREN {
                                    $$ = conditional($3, $4, $5);
                                    }
        | LPAREN FUNC s_expr_list RPAREN {
                                    //$$ = function($2, $3);
        }
        | LPAREN SYMBOL s_expr_list RPAREN {
                                    $$ = createFunctionNode($2, $3);
        }                   
        | QUIT { 
                  //printf("QUIT\n"); 
                  exit(0);
               }
        | error { 
                        printf("error\n"); 
                        //printf("> ");
                }
        ;
s_expr_list:{ 
            //nothing needed?
            $$ = NULL;
            }
            |
            s_expr_list s_expr {
                    $$ = s_exprTableInsert($1, $2);
                }
            ;
let_list: 
            let_elem{
            $$ = $1;
            }
            | let_list let_elem  {
                //$1 pointer to list so far $2 new node, return node of head of list
                $$ = symbolTableInsert($1, $2);
            }  
            ;
let_elem:   LPAREN INTEGER SYMBOL s_expr RPAREN{
                $$ = createLetElem($2, $3, $4);
            }
            | LPAREN REAL SYMBOL s_expr RPAREN{
                $$ = createLetElem($2, $3, $4);
            }
            | LPAREN SYMBOL s_expr RPAREN{
                $$ = createLetElem(NULL, $2, $3); //for letElem without datatype
            }
            | LPAREN SYMBOL LPAREN arg_list RPAREN s_expr RPAREN {
                $$ = createFunction($2, $4, $6);
            }
            ;
arg_list:   { 
            $$ = NULL;
            }
            | SYMBOL arg_list {
            $$ = createArgList($1, $2);
            } 
            ;
%%