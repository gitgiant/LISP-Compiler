#!/bin/sh
bison -y -d l8t10.y
flex l8t10.l
gcc -std=c99 -D_GNU_SOURCE=1 -D_POSIX_SOURCE=1 l8t10.c lex.yy.c y.tab.c -o l8t10 -g -lm
gdb l8t10

