#!/bin/sh
yacc -y -d l8t1.y
flex l8t1.l
gcc -std=c99 -D_GNU_SOURCE=1 -D_POSIX_SOURCE=1 l8t1.c lex.yy.c y.tab.c -o l8t1 -g -lm

