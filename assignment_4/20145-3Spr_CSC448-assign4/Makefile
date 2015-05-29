# Makefile for lang0
lang0	: lang0.tab.o lang0.o
	  g++ -o $@ lang0.tab.o lang0.o

lang1	: lang1.tab.o lang0.o
	  g++ -o $@ lang1.tab.o lang0.o


lang0.o	 : lang0.h lang0.c lang0.tab.h VarStore.h Statement.h
	  g++ -c lang0.c -g


lang0.tab.o : lang0.h lang0.tab.c lang0.tab.h VarStore.h Statement.h
	  g++ -c lang0.tab.c -g

lang1.tab.o : lang0.h lang1.tab.c lang1.tab.h VarStore.h Statement.h
	  g++ -c lang1.tab.c -g


lang0.c	 : lang0.lex
	  flex -o$@ lang0.lex


lang0.tab.c : lang0.y
	  bison -d lang0.y --debug --verbose

lang0.tab.h : lang0.y
	  bison -d lang0.y --debug --verbose

lang1.tab.c : lang1.y
	  bison -d lang1.y --debug --verbose

lang1.tab.h : lang1.y
	  bison -d lang1.y --debug --verbose
