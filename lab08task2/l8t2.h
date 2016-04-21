/**
 * Name: Mattias Huber
 * Lab/task: lab 08 Task 1
 * Date: 03/17/2016
 **/

#ifndef __l8t2_h_
#define __l8t2_h_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include "y.tab.h"

int yyparse(void);
int yylex(void);
void yyerror(char *);

// function to calculate expressions
double calc(char *, double, double);

#endif
