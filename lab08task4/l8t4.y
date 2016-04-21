/**
 * Name: Mattias Huber
 * Lab/task: lab 08 Task 4
 * Date: 03/20/2016
 **/
 
/*

s-expressions calculator

program ::= program s-expr

s-expr ::= number | ( func s-expr ) | ( func s-expr s-expr )

number ::= [ + | - ] digit+ [ . digit+ ] 

digit ::= 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9

*/

%{
#include "l8t4.h"
%}

%union
{
   double dval;
   char *sval;
   struct ast_node *astNode;
};

%token <sval> FUNC
%token <dval> NUMBER
%token LPAREN RPAREN EOL QUIT

%type <astNode> s_expr

%%

program:/* empty */ { 
                       printf("> ");
                    }
        | program s_expr EOL {
                              // printf("yacc: program expr\n"); 
                              double returnvalue=eval($2);
                              printvalue(returnvalue);
                              freeNode($2);
                              printf("\n> "); 
                           }
        ;

s_expr:
        NUMBER { 
                  //printf("yacc: NUMBER%lf", $1); 
                  $$ = number($1); 
               }
        | LPAREN FUNC s_expr RPAREN { 
                                     // printf("yacc: LPAREN FUNC expr RPAREN\n"); 
                                     $$ = function($2, $3, 0);  
                                     //printf("%s(%lf)", $2, $3);
                                  }
        | LPAREN FUNC s_expr s_expr RPAREN {
                                          // printf("LPAREN FUNC expr expr RPAREN\n"); 
                                          // $$ = calc($2, $3, $4); 
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
%%

