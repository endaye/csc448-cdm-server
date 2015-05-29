#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>
#include	<errno.h>
#include	<string>
#include	"VarStore.h"

#define	LINE_LEN	256

extern	VarStore	varStore;
extern	char*		textPtr;
extern	char*		textEndPtr;

#include	"Statement.h"

extern	Statement*	resultPtr;

extern	FILE*		yyin;
extern	int		yyerror		(const char *s);
extern	int		yylex		();
extern	int		yyparse		();

#define	MIN(x,y)	(((x)<(y)) ? (x) : (y))
