%{
//  lang1.y
//  $ bison --verbose -d --debug lang1.y
//  $ gcc lang1.tab.c -c
#include  "lang1.h"
%}

%union
{
  Statement*	statePtr_;
  char*		charPtr_;
  double	const_;
}

%start			expr
%token			BEGIN_P END_P END
%token			PRINT READ IF WHILE DO
%nonassoc		EQUAL LOGIC_AND LESS GREATER PLUS MINUS STAR SLASH
%nonassoc		UMINUS
%token			ELSE
%token			BEGIN_C END_C
%token	<charPtr_>	VARIABLE
%token	<const_>	NUMBER
%type	<statePtr_>	expr
%type	<statePtr_>	list
%nonassoc		ERROR

%%

list	: list expr
	  {
	    ((BlockStatement*)$1)->add($2);
	    $$ = $1;
	  }
	|
	  {
	    // lambda production
	    $$ = new BlockStatement;
	  };

expr	: BEGIN_P PRINT expr END_P
	  { 
	    resultPtr = $$ = new PrintStatement($3);
	  }
	| BEGIN_P READ VARIABLE END_P
	  {
	    resultPtr = $$ = new ReadStatement($3);
	  }
	| BEGIN_P IF expr expr END_P
	  {
	    resultPtr = $$ = new IfStatement($3,$4);
	  }
	| BEGIN_P IF expr expr expr END_P
	  {
	    resultPtr = $$ = new IfStatement($3,$4,$5);
	  }
	| BEGIN_P WHILE expr expr END_P
	  {
	    resultPtr = $$ = new WhileStatement($3,$4);
	  }
	| BEGIN_P DO list END_P
	  {
	    resultPtr = $$ = $3;
	  }
	| BEGIN_P EQUAL VARIABLE expr END_P
	  {
	    $$ = new AssignStatement($3,$4);
	  }
	| BEGIN_P LESS expr expr END_P
	  {
	    $$ = new BinaryOpStatement($3,'<',$4);
	  }
	| BEGIN_P GREATER expr expr END_P
	  {
	    $$ = new BinaryOpStatement($4,'<',$3);
	  }
	| BEGIN_P PLUS expr expr END_P
	  {
	    $$ = new BinaryOpStatement($3,'+',$4);
	  }
	| BEGIN_P MINUS expr expr END_P
	  {
	    $$ = new BinaryOpStatement($3,'-',$4);
	  }
	| BEGIN_P STAR expr expr END_P
	  {
	    $$ = new BinaryOpStatement($3,'*',$4);
	  }
	| BEGIN_P SLASH expr expr END_P
	  {
	    $$ = new BinaryOpStatement($3,'/',$4);
	  }
	| BEGIN_P LOGIC_AND expr expr END_P
	  {
	    $$ = new BinaryOpStatement($3,'&',$4);
	  }
	| NUMBER
	  {
	    $$ = new ConstantStatement($1);
	  }
	| VARIABLE
	  {
	    $$ = new RValVarNameStatement($1);
	  };

%%


//  {  while (i < 11) { sum = sum + i; i = i + 1; } print sum; }
