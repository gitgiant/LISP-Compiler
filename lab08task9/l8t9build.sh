#!/bin/sh
bison -y -d l8t9.y
flex l8t9.l
gcc -std=c99 -D_GNU_SOURCE=1 -D_POSIX_SOURCE=1 l8t9.c lex.yy.c y.tab.c -o l8t9 -g -lm

