#!/bin/sh
yacc -y -d l8t2.y
flex l8t2.l
gcc -std=c99 -D_GNU_SOURCE=1 -D_POSIX_SOURCE=1 l8t2.c lex.yy.c y.tab.c -o l8t2 -g -lm

