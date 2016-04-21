#!/bin/sh
bison -y -d l8t5.y
flex l8t5.l
gcc -std=c99 -D_GNU_SOURCE=1 -D_POSIX_SOURCE=1 l8t5.c lex.yy.c y.tab.c -o l8t5 -g -lm

