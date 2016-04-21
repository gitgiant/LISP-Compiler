/**
 * Name: Mattias Huber
 * Lab/task: lab 08 Task 6
 * Date: 04/07/2016
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
#include "l8t6.h"
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
%token LPAREN RPAREN EOL QUIT LET 

%type <astNode> s_expr
%type <astNode> let_list
%type <astNode> let_elem

%%

program:/* empty */ { 
                       printf("> ");
                    }
        | program s_expr EOL {
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
        | LPAREN FUNC s_expr RPAREN { 
                                    // printf("yacc: LPAREN FUNC expr RPAREN\n"); 
                                    $$ = function($2, $3, 0);  
                                    //printf("%s(%lf)", $2, $3);
                                    }
        | LPAREN FUNC s_expr s_expr RPAREN {
                                    // printf("LPAREN FUNC expr expr RPAREN\n"); 
                                    $$ = function($2, $3, $4);
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