#!/bin/sh
bison -y -d l8t7.y
flex l8t7.l
gcc -std=c99 -D_GNU_SOURCE=1 -D_POSIX_SOURCE=1 l8t7.c lex.yy.c y.tab.c -o l8t7 -g -lm

