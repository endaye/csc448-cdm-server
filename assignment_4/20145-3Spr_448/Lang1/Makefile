# Makefile for lang1
lang1	: lang1.tab.o lang1.o
	  g++ -o $@ lang1.tab.o lang1.o


lang1.o	 : lang1.h lang1.c lang1.tab.h VarStore.h Statement.h
	  g++ -c lang1.c -g


lang1.tab.o : lang1.h lang1.tab.c lang1.tab.h VarStore.h Statement.h
	  g++ -c lang1.tab.c -g


lang1.c	 : lang1.lex
	  flex -o$@ lang1.lex


lang1.tab.c : lang1.y
	  bison -d lang1.y --debug --verbose

lang1.tab.h : lang1.y
	  bison -d lang1.y --debug --verbose
