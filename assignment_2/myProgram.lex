%{
	// Yuancheng Zhang
	// CSC448 Assignment_2_Q1
	// Compile with:
	// $ flex ­-o myProgram.c myProgram.lex 
	// $ gcc myProgram.c ­-o myProgram
	///*L?'(\\.|[^\\'])+'			{ printf("CONSTANT\n"); }*/
	/*Any ADD, MULTIPLY or POWER lexeme when preceded by single quote (')	IDENTIFIER
Optional +/-, followed by [0-9]+	CONSTANT
Optional +/-, followed by [0-9]+ \ [0-9]+	CONSTANT*/

%}
%%
\n					// Ingore newlines
\t 					// Ingore newlines
' ' 					// Ingore newlines
,							{ printf("COMMA\n"); }
;							{ printf("SEMICOLON\n"); }
~							{ printf("SIDE\n"); }
[a-zA-Z_][a-zA-Z0-9_]*		{ printf("IDENTIFIER\n"); }
@ [a-zA-Z0-9_]*				{ printf("VARIABLE\n"); }
\? [a-zA-Z0-9]*				{ printf("PREDICATE\n"); }
"\` [.]* \`"					{ printf("CONSTANT\n");	 }
"("							{ printf("BEGIN_PAREN\n"); }
")"							{ printf("END_PAREN\n"); }
"(*"							{ printf("BEGIN_VALUE\n"); }
"*)"							{ printf("END_VALUE\n"); }
"["							{ printf("BEGIN_LIST\n"); }
"]"							{ printf("END_LIST\n"); }
"[*"							{ printf("BEGIN_IMPLICIT\n"); }
"*]"							{ printf("END_IMPLICIT\n"); }
"[**"							{ printf("BEGIN_NODE_LIST\n"); }
"**]"							{ printf("END_NODE_LIST\n"); }
"{"							{ printf("BEGIN_BAG\n"); }
"}"							{ printf("END_BAG\n"); }
"{*"							{ printf("BEGIN_EXPLICIT\n"); }
"*}"							{ printf("END_EXPLICIT\n"); }
\"+\" [a-zA-Z_]*				{ printf("ADD\n"); }
"-" [a-zA-Z_]*				{ printf("ADD\n"); }
"*"							{ printf("MULTIPLY\n"); }
"/"							{ printf("MULTIPLY\n"); }
"**"							{ printf("POWER\n"); }
^[-+]?[0-9]*\\.?[0-9]+$	{ printf("CONSTANT\n"); }
":="							{ printf("ASSIGN\n"); }
":-"							{ printf("IS_IMPLIED_BY\n"); }
"^"							{ printf("CARET\n"); }
"^^"							{ printf("DOUBLE_CARET\n"); }
"->"							{ printf("ARROW\n"); }
"&&"							{ printf("AND\n"); }
"||"							{ printf("OR\n"); }
"|"							{ printf("VERTICAL_BAR\n"); }
"!"							{ printf("UNARY\n"); }
"!=ref"						{ printf("EQUALITY_COMPARE\n"); }
"=ref"						{ printf("EQUALITY_COMPARE\n"); }
"!=num"						{ printf("EQUALITY_COMPARE\n"); }
"=num"						{ printf("EQUALITY_COMPARE\n"); }
"=="						{ printf("EQUALITY_COMPARE\n"); }
"!="						{ printf("EQUALITY_COMPARE\n"); }
">"							{ printf("LESSER_GREATER_COMPARE\n"); }
">="						{ printf("LESSER_GREATER_COMPARE\n"); }
"<"							{ printf("LESSER_GREATER_COMPARE\n"); }
"<="						{ printf("LESSER_GREATER_COMPARE\n"); }
">>"						{ printf("LESSER_GREATER_COMPARE\n"); }
"<<"							{ printf("LESSER_GREATER_COMPARE\n"); }
"sub"							{ printf("SUB\n"); }
"final"						{ printf("FINAL\n"); }
"single"						{ printf("SINGLE\n"); }
"noMore"						{ printf("NOMORE\n"); }
"AnonFncTpl"					{ printf("LAMBDATEMPLATES\n"); }
"not"							{ printf("NOT\n"); }
"-unc"						{ printf("UNSPECIFIC_UNCAT\n"); }
"-uncA"						{ printf("UNCAT\n"); }
"-uncZ"						{ printf("UNCAT\n"); }
"+con"						{ printf("CONCAT\n"); }
"e"							{ printf("NAMED_CONSTANT\n"); }
"pi"							{ printf("NAMED_CONSTANT\n"); }
"+infinity"					{ printf("NAMED_CONSTANT\n"); }
"-infinity"					{ printf("NAMED_CONSTANT\n"); }
"sin"							{ printf("NAMED_CONSTANT\n"); }
"cos"							{ printf("NAMED_CONSTANT\n"); }
"tan"							{ printf("NAMED_CONSTANT\n"); }
"asin"						{ printf("NAMED_CONSTANT\n"); }
"acos"						{ printf("NAMED_CONSTANT\n"); }
"atan"						{ printf("NAMED_CONSTANT\n"); }
"+ ("							{ printf("NAMED_CONSTANT\n"); }
"- ("							{ printf("NAMED_CONSTANT\n"); }
.							{ /* ignore bad characters */ }
%%

int     yywrap  ()	{ return(1); }

int     main    ()
{
	yylex();
	return(0); 
}