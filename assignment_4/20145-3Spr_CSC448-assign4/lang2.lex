%{
//  lang1.lex
//  unix> flex -o lang1.c lang1.lex
//  unix> gcc lang1.c -c
//  unix> gcc -o lang1 lang1.tab.o lang1.o

#include  "lang2.h"
#include  "lang2.tab.h"

#undef    YY_INPUT
#define   YY_INPUT(buffer,result,maxSize)   \
    { result = ourInput(buffer,maxSize); }

extern
int   ourInput(char* buffer, int maxSize);

%}

%%

[ \t\n]     { /* ignore whitespace */ }
(\-|\+)?[0-9]+|([0-9]*\.[0-9]+) {
        yylval.const_ = strtod(yytext,NULL);
        return(NUMBER);
      }
print     { return(PRINT); }
println   { return(PRINTLN); }
declare  { return(DECLARE); }
read      { return(READ); }
while     { return(WHILE); }
do      { return(DO); }
if      { return(IF); }
else      { return(ELSE); }
[a-zA-Z_][a-zA-Z_0-9]*  {
        yylval.charPtr_ = strdup(yytext);
        return(VARIABLE);
      } 
\"([^\\\"]|\\.)*\" { return(STRING);}
\+      { return(PLUS); }
\-      { return(MINUS); }
\*      { return(STAR); }
\/      { return(SLASH); }
\&      { return(LOGIC_AND); }
\(      { return(BEGIN_P); }
\)      { return(END_P); }
\{      { return(BEGIN_C); }
\}      { return(END_C); }
\<      { return(LESS); }
\>      { return(GREATER); }
=     { return(EQUAL); }
;     { return(END); }
.     {
        printf("What's '%c'?\n",yytext[0]);
        return(ERROR);
      }

%%


//  PURPOSE:  To hold the input file from which to read the program (if it is
//  not 'stdin').
FILE*   filePtr   = NULL;

//  PURPOSE:  To point to the beginning of the input yet to read (if being
//  typed from the command line).
char*   textPtr   = NULL;

//  PURPOSE:  To point to the end of all input (if being typed from the command
//  line).
char*   textEndPtr  = NULL;

//  PURPOSE:  To point to a list of all variables that have been declared.
VarEntry* varList   = NULL;

//  PURPOSE:  To point to the symbol table (necessary *only* during parsing,
//  not during execution).
SymbolTable*  symTabPtr = NULL;

//  PURPOSE:  To point to the parse tree created by the parser.
Statement*  resultPtr = NULL;


//  PURPOSE:  To return 1 if the tokenizer should quit after EOF is reached.
//  Returns 0 otherwise.  No parameters.
int yywrap  ()
{
  return(1);
}


//  PURPOSE:  To show the error message pointed to by 'cPtr'.  Returns '0'.
int yyerror (const char *cPtr)
{
  printf("%s, sorry!\n",cPtr);
  return(0);
}


//  PURPOSE:  To get up to 'maxSize' more characters of input and put them
//  into 'buffer'.   Returns how many characters were obtained.
int   ourInput  (char*       buffer,
  int       maxSize
  )
{
  unsigned int  n;

  if  (filePtr == NULL)
  {
    n = MIN(maxSize,textEndPtr - textPtr);

    if  (n > 0)
    {
      memcpy(buffer,textPtr,n);
      textPtr += n;
    }
  }
  else
  {
    errno = 0;

    while  ( (n = fread(buffer,1,maxSize,yyin)) == 0 && ferror(yyin))
    {
      if  (errno != EINTR)
      {
        fprintf(stderr,"Reading file failed: %s\n",strerror(errno));
        exit(EXIT_FAILURE);
      }

      errno = 0;
      clearerr(yyin);
    }

  }

  return(n);
}


//  PURPOSE:  To parse and execute the program whose filename is given on the
//  command line at 'argv[1]' (if 'argc' >= 1), or that the user types in.
//  Returns 'EXIT_SUCCESS' on success or 'EXIT_FAILURE' otherwise.
int       main    (int    argc,
  char*    argv[]
  )
{
//  I.  Application validity check:

//  II.  Parse and execute program:
//  II.A.  Get program source:
  char  line[LINE_LEN];

  if  ( (argc >= 2)  &&  ( (filePtr = fopen(argv[1],"r")) != NULL ) )
  {
    yyin  = filePtr;
  }
  else
  {
    printf("Please enter an expression: ");
    textPtr = fgets(line,LINE_LEN,stdin);
    textEndPtr  = textPtr + strlen(textPtr);
  }

//  II.B.  Attempt to parse program:
  try
  {
    symTabPtr = new SymbolTable;
    yyparse();
    delete(symTabPtr);
  }
  catch  (std::string*  errStrPtr)
  {
    fprintf(stderr,"Error: %s.\n",errStrPtr->c_str());
    delete(errStrPtr);
  }

//  II.C.  Execute program (if present):
  if  (resultPtr != NULL)
  {
    double d = resultPtr->eval();
    delete(resultPtr);
  }

//  II.D. Clean up:
//  II.D.1.  Get rid of variables:
  delete(varList);

//  II.D.2.  Close file (if open):
  if  (filePtr != NULL)
    fclose(filePtr);

  fflush(stdout);

//  III.  Finished:
  return(EXIT_SUCCESS);
}


