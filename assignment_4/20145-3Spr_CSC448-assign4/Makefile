#--------------------------------------------------------------------------#
#---									---#
#---		Makefile						---#
#---									---#
#---	    This file defines a Makefile for Lex, Yacc and C files that	---#
#---	implement a simple interpreted language inspired by C.		---#
#---									---#
#---	----	----	----	----	----	----	----	----	---#
#---									---#
#---	Version 1.2		2015 May 22		Joseph Phillips	---#
#---									---#
#--------------------------------------------------------------------------#

# Makefile for lang2
lang2	: lang2.tab.o lang2.o
	  g++ -o $@ lang2.tab.o lang2.o


lang2.o	 : lang2.h lang2.c lang2.tab.h SymbolTable.h Statement.h
	  g++ -c lang2.c -g


lang2.tab.o : lang2.h lang2.tab.c lang2.tab.h SymbolTable.h Statement.h
	  g++ -c lang2.tab.c -g


lang2.c	 : lang2.lex
	  flex -o$@ lang2.lex


lang2.tab.c : lang2.y
	  bison -d lang2.y --debug --verbose

lang2.tab.h : lang2.y
	  bison -d lang2.y --debug --verbose

# bison -d lang2.y --debug --verbose
# g++ -c lang2.tab.c -g
# flex -olang2.c lang2.lex
# g++ -c lang2.c -g
# g++ -o lang2 lang2.tab.o lang2.o