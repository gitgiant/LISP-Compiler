/**
 * Name: Mattias Huber
 * Lab/task: lab 08 Task 2
 * Date: 03/17/2016
 **/
 
/*

s-expressions calculator

program ::= program s-expr

s-expr ::= number | ( func s-expr ) | ( func s-expr s-expr )

number ::= [ + | - ] digit+ [ . digit+ ] 

digit ::= 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9

*/

%{
#include "l8t2.h"
%}

%union
{
   double dval;
   char *sval;
}

%token <sval> FUNC
%token <dval> NUMBER
%token LPAREN RPAREN EOL QUIT

%type <dval> s_expr

%%

program:/* empty */ { 
                       printf("> ");
                    }
        | program s_expr EOL {
                              // printf("> "); 
                              // printf("yacc: program expr\n"); 
                              printf("%lf\n> ", $2); 
                           }
        ;

s_expr:
        NUMBER { 
                  // printf("yacc: NUMBER\n"); 
                  $$ = $1; 
               }
        | LPAREN FUNC s_expr RPAREN { 
                                     // printf("yacc: LPAREN FUNC expr RPAREN\n"); 
                                     $$ = calc($2, $3, 0);  
                                     // printf(" result = %lf", $$);
                                  }
        | LPAREN FUNC s_expr s_expr RPAREN {
                                          // printf("LPAREN FUNC expr expr RPAREN\n"); 
                                          $$ = calc($2, $3, $4); 
                                          // printf("%lf", $$);
                                       }
        | QUIT { 
                  //printf("QUIT\n"); 
                  exit(0);
               }
        
        | error { 
                        printf("error\n"); 
                        printf("> ");
                    }

        ;
%%

