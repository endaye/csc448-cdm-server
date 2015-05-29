%{
//  lang2.lex
//  unix> flex -o lang2.c lang2.lex
//  unix> gcc lang2.c -c
//  unix> gcc -o lang2 lang2.tab.o lang2.o

#include	"lang2.h"
#include	"lang2.tab.h"

#undef		YY_INPUT
#define		YY_INPUT(buffer,result,maxSize)		\
		{ result = ourInput(buffer,maxSize); }

extern
int		ourInput(char* buffer, int maxSize);

%}

%%

[ \t\n]			{ /* ignore whitespace */ }
(\-|\+)?[0-9]+|([0-9]*\.[0-9]+)	{
			  yylval.const_	= strtod(yytext,NULL);
			  return(NUMBER);
			}
print			{ return(PRINT); }
read			{ return(READ); }
while			{ return(WHILE); }
do			{ return(DO); }
if			{ return(IF); }
else			{ return(ELSE); }
[a-zA-Z_][a-zA-Z_0-9]*	{
			  yylval.charPtr_ = strdup(yytext);
			  return(VARIABLE);
			} 
\+			{ return(PLUS); }
\-			{ return(MINUS); }
\*			{ return(STAR); }
\/			{ return(SLASH); }
\&			{ return(LOGIC_AND); }
\(			{ return(BEGIN_P); }
\)			{ return(END_P); }
\{			{ return(BEGIN_C); }
\}			{ return(END_C); }
\<			{ return(LESS); }
\>			{ return(GREATER); }
=			{ return(EQUAL); }
;			{ return(END); }
.			{
			  printf("What's '%c'?\n",yytext[0]);
			  return(ERROR);
			}

%%

int	yywrap	()
{
  return(1);
}



FILE*		filePtr		= NULL;
char*		textPtr		= NULL;
char*		textEndPtr	= NULL;
VarStore	varStore;
Statement*	resultPtr;

int	yyerror	(const char *cPtr)
{
  printf("%s, sorry!\n",cPtr);
  return(0);
}


int	ourInput(char* buffer, int maxSize)
{
  unsigned int	n;

  if  (filePtr == NULL)
  {
    n	= MIN(maxSize,textEndPtr - textPtr);

    if  (n > 0)
    {
      memcpy(buffer,textPtr,n);
      textPtr	+= n;
    }
  }
  else
  {
    errno	= 0;

    while  ( (n = fread(buffer,1,maxSize,yyin)) == 0 && ferror(yyin))
    {
      if  (errno != EINTR)
      {
        fprintf(stderr,"Reading file failed: %s\n",strerror(errno));
	exit(EXIT_FAILURE);
      }

      errno	= 0;
      clearerr(yyin);
    }

  }

  return(n);
}


int	main	(int argc, char* argv[])
{
  char	line[LINE_LEN];

  if  ( (argc >= 2)  &&  ( (filePtr = fopen(argv[1],"r")) != NULL ) )
  {
    yyin	= filePtr;
   }
  else
  {
    printf("Please enter an expression: ");
    textPtr	= fgets(line,LINE_LEN,stdin);
    textEndPtr	= textPtr + strlen(textPtr);
   }

  yyparse();

  if  (resultPtr != NULL)
  {
    double d = resultPtr->eval();
//  printf("%g\n",d);
    delete(resultPtr);
  }

  if  (filePtr != NULL)
    fclose(filePtr);

  fflush(stdout);
  return(EXIT_SUCCESS);
}
