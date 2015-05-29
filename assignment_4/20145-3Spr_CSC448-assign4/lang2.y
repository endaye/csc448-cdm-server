%{
//  lang2.y
//  $ bison --verbose -d --debug lang2.y
//  $ gcc lang2.tab.c -c
#include  "lang2.h"
%}

%union
{
  Statement*	statePtr_;
  char*		charPtr_;
  double	const_;
}

%start			state
%token			BEGIN_P END_P END
%token			PRINT READ IF WHILE DO
%nonassoc		IF_SIMPLE
%nonassoc		ELSE
%right			EQUAL
%left			LOGIC_AND
%left			LESS GREATER
%left			PLUS MINUS
%left			STAR SLASH
%nonassoc		UMINUS
%token			BEGIN_C END_C
%token	<charPtr_>	VARIABLE
%token	<const_>	NUMBER
%type	<statePtr_>	expr
%type	<statePtr_>	state
%type	<statePtr_>	list
%nonassoc		ERROR

%%

list	: list state
	  {
	    ((BlockStatement*)$1)->add($2);
	    $$ = $1;
	  }
	|
	  {
	    // lambda production
	    $$ = new BlockStatement;
	  };

state	: PRINT expr END
	  { 
	    resultPtr = $$ = new PrintStatement($2);
	  }
	| READ VARIABLE END
	  {
	    resultPtr = $$ = new ReadStatement($2);
	  }
	| IF BEGIN_P expr END_P state			%prec IF_SIMPLE
	  {
	    resultPtr = $$ = new IfStatement($3,$5);
	  }
	| IF BEGIN_P expr END_P state ELSE state	%prec ELSE
	  {
	    resultPtr = $$ = new IfStatement($3,$5,$7);
	  }
	| WHILE BEGIN_P expr END_P state
	  {
	    resultPtr = $$ = new WhileStatement($3,$5);
	  }
	| BEGIN_C list END_C
	  {
	    resultPtr = $$ = $2;
	  }
	| expr END
	  {
	    resultPtr = $$ = $1;
	  };

expr	: VARIABLE EQUAL expr
	  {
	    $$ = new AssignStatement($1,$3);
	  }
	| expr LESS expr
	  {
	    $$ = new BinaryOpStatement($1,'<',$3);
	  }
	| expr GREATER expr
	  {
	    $$ = new BinaryOpStatement($3,'<',$1);
	  }
	| expr PLUS expr
	  {
	    $$ = new BinaryOpStatement($1,'+',$3);
	  }
	| expr MINUS expr
	  {
	    $$ = new BinaryOpStatement($1,'-',$3);
	  }
	| expr STAR expr
	  {
	    $$ = new BinaryOpStatement($1,'*',$3);
	  }
	| expr SLASH expr
	  {
	    $$ = new BinaryOpStatement($1,'/',$3);
	  }
	| expr LOGIC_AND expr
	  {
	    $$ = new BinaryOpStatement($1,'&',$3);
	  }
	| BEGIN_P expr END_P
	  {
	    $$ = $2;
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
