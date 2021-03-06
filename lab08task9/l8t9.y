/**
 * Name: Mattias Huber
 * Lab/task: lab 08 Task 9
 * Date: 04/14/2016
 **/
 
/*

program ::= | program s_expr EOL
s_expr ::= number
| symbol
| ( func s_expr )
| ( func s_expr s_expr )
| ( ( let let_list ) s_expr )
let_list ::= let_elem | let_list let_elem
let_elem ::= ( symbol s_expr )
symbol ::= letter+
letter ::= [a-zA-Z]
func ::= neg|abs|exp|sqrt|add|sub|mult|div|remainder|log|pow|max|min|exp2|cbrt|hypot
number ::= [+|-]?digit+[.number+]
digit ::= [0-9]

*/

%{
#include "l8t9.h"
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
%type <astNode> let_list
%type <astNode> let_elem

%%

program:/* empty */ { 
                       printf("> ");
                    }
        | program s_expr EOL {
                              eval($2);
                              //printf("Program output: \n"); 
                              //printvalue(eval($2));
                              //freeNode($2);
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
            //static scoping: there will be multiple symbol tables associated with the root of the expression within which they are defined

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

        | QUIT { 
                  //printf("QUIT\n"); 
                  exit(0);
               }
        | error { 
                        printf("error\n"); 
                        //printf("> ");
                }
        ;
 
let_list: 
            let_elem{
            $$ = $1;
            }
            |   let_list let_elem  {
                //$1 pointer to list so far $2 new node, return node of head of list
                $$ = symbolTableInsert($1, $2);
            }  
let_elem:   LPAREN INTEGER SYMBOL s_expr RPAREN{
                $$ = createLetElem($2, $3, $4);
            }
            | LPAREN REAL SYMBOL s_expr RPAREN{
                $$ = createLetElem($2, $3, $4);
            }
            | LPAREN SYMBOL s_expr RPAREN{
                $$ = createLetElem(NULL, $2, $3);
            }
%%