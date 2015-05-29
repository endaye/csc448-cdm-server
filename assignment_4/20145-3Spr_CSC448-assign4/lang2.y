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
	%token			PRINT PRINTLN READ IF WHILE DO DECLARE
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
	%token	<charPtr_>	STRING
	%type	<statePtr_>	expr
	%type	<statePtr_>	state
	%type	<statePtr_>	list
	%nonassoc		ERROR

	%%

	list	: list state
	{
		//For the list -> list state rule: make it add($2) only if $2 != NULL.	
		if($2 != NULL) {
			((BlockStatement*)$1)->add($2);
		}
		$$ = $1;
	}
	| 
	{
		// lambda production
		$$ = new BlockStatement;
	};

	state	: PRINT expr END
	{ 
		resultPtr = $$ = new PrintStatement($2,false);
	}
	| PRINT STRING END
	{
		resultPtr = $$ = new PrintStatement($2,false);
	}
	| PRINTLN expr END
	{
		resultPtr = $$ = new PrintStatement($2,true);
	}
	| PRINTLN STRING END
	{
		resultPtr = $$ = new PrintStatement($2,true);
	}
	| READ VARIABLE END
	{
		resultPtr = $$ = new ReadStatement(symTabPtr->retrieve($2));
		free($2);
	}
	| DECLARE VARIABLE END
	{ 
		/* Please add a rule state -> DECLARE VARIABLE END. 
		The code for the rule should:
		Do symTabPtr->declare($2,varList); 
		to tell the symbol table about the variable.
		free() the variable name after declaring it.
		Set $$ to NULL*/
		symTabPtr->declare($2,varList);
		free($2);
		$$ = NULL;
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
		$$ = new AssignStatement(symTabPtr->retrieve($1),$3);
		free($1);
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
		$$ = new RValVarNameStatement(symTabPtr->retrieve($1));
		free($1);
	};

	%%

