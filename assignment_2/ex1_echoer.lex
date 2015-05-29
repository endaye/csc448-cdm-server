%{
%}
%%
\n	printf("\n");
.	printf("%c",yytext[0]);
%%

int	yywrap	()	{	return(1);	}

int	main	()
{
	yylex();
	return(0);
}

