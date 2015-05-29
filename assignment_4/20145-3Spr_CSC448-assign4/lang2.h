/*-------------------------------------------------------------------------*
 *---									---*
 *---		lang2.h							---*
 *---									---*
 *---	    This file declares classes and variables that tie the	---*
 *---	tokenizer with the parser for a simple interpreted language	---*
 *---	inspired by C.							---*
 *---									---*
 *---	----	----	----	----	----	----	----	----	---*
 *---									---*
 *---	Version 1.2		2015 May 22		Joseph Phillips	---*
 *---									---*
 *-------------------------------------------------------------------------*/


//--			Common inclusions:				--//

#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>
#include	<errno.h>
#include	<string>
#include	"SymbolTable.h"


//--		Declaration of constants and global vars:		--//
//  PURPOSE:  To tell the maximum allowed length of C-strings.
#define	LINE_LEN	256

//  PURPOSE:  To point to a list of all variables that have been declared.
extern	VarEntry*	varList;

//  PURPOSE:  To point to the symbol table (necessary *only* during parsing,
//	not during execution).
extern	SymbolTable*	symTabPtr;

//  PURPOSE:  To point to the parse tree created by the parser.
#include		"Statement.h"
extern	Statement*	resultPtr;

//  PURPOSE:  To tell the FILE* object from which 'yylex()' should read.
extern	FILE*		yyin;

//  PURPOSE:  To show the error message pointed to by 'cPtr'.  Returns '0'.
extern	int		yyerror		(const char *cPtr);

//  PURPOSE:  To return the integer that encodes the next read token, or '0'
//	if there are no more tokens to read.
extern	int		yylex		();

//  PURPOSE:  To attempt to parse the input.  No parameters.  Returns '0'.
extern	int		yyparse		();

#define	MIN(x,y)	(((x)<(y)) ? (x) : (y))

// #include	<stdlib.h>
// #include	<stdio.h>
// #include	<string.h>
// #include	<errno.h>
// #include	<string>
// #include	"VarStore.h"

// #define	LINE_LEN	256

// extern	VarStore	varStore;
// extern	char*		textPtr;
// extern	char*		textEndPtr;

// #include	"Statement.h"

// extern	Statement*	resultPtr;

// extern	FILE*		yyin;
// extern	int		yyerror		(const char *s);
// extern	int		yylex		();
// extern	int		yyparse		();

// #define	MIN(x,y)	(((x)<(y)) ? (x) : (y))
