/*-------------------------------------------------------------------------*
 *---                 ---*
 *---   aCCompiler.cpp            ---*
 *---                 ---*
 *---     This file defines a C-ish C++ implementation of Cytron's  ---*
 *--- recursive-descent parser for his "ac" language that translates  ---*
 *--- arithematic expressions into RPN suitable for use by Unix's ---*
 *--- "dc" arbitrary precision calculating utility.     ---*
 *---                 ---*
 *---                 ---*
 *--- ----  ----  ----  ----  ----  ----  ----  ----  ---*
 *---                 ---*
 *--- Version 1.0   2015 April 5    Joseph Phillips ---*
 *---                 ---*
 *-------------------------------------------------------------------------*/


/*---*
 *---*    Compile with:
 *---*    $ g++ acCompiler.cpp -o acCompiler
 *---*/

/*---*
 *---*    Header inclusions:
 *---*/

#include    <cstdlib>
#include    <cstdio>
#include    <cstring>
#include    <cctype>

#include    <iostream>
#include    <string>


char errorMessage[256];

/*---*
 *---*    Ordinal enumerations:
 *---*/

//  PURPOSE:  To represent the types of variables.
typedef enum    {
        NULL_DC_TYPE,
        INTEGER_DC_TYPE,
        FLOAT_POINT_DC_TYPE
      }
      dcType_t;


//  PURPOSE:  To identify read symbols.
typedef enum    {
        END_OF_FILE_SYMBOL,
        INT_SYMBOL,
        FLOAT_SYMBOL,
        ADD_SYMBOL,
        SUBTRACT_SYMBOL,
        ID_SYMBOL,
        INT_DECLARE_SYMBOL,
        FLOAT_DECLARE_SYMBOL,
        PRINT_SYMBOL,
        ASSIGN_SYMBOL,
        STATEMENT_LIST_SYMBOL,
        TYPE_CONVERT_SYMBOL
      }
      symbol_t;



/*---*
 *---*    Data-structures that hold per-node parsed information:
 *---*/

struct  Symbol;

//  PURPOSE:  To hold the knowledge needed for an Expression (+/-).
struct  Expression
{
  Symbol*   lhsPtr_;
  Symbol*   rhsPtr_;
};


//  PURPOSE:  To hold the knowledge needed for an assignment.
struct  Assignment
{
  Symbol*   varPtr_;
  Symbol*   lhsPtr_;
  Symbol*   rhsPtr_;
};


//  PURPOSE:  To hold the knowledge needed for a sequence of statements.
struct  Statement
{
  Symbol*   firstPtr_;
  Symbol*   restPtr_;
};


//  PURPOSE:  To hold the knowledge that could be at any node.
struct  Symbol
{
  symbol_t    symbol_;
  union
  {
    int     integer_;
    float   floatPt_;
    char    varName_;
    Expression    expression_;
    Assignment    assignment_;
    Statement   statement_;
  }       value_;
};


//  PURPOSE:  To represent a special 'Symbol' that means the end of the input
//  stream.
Symbol      endSymbol;


/*---*
 *---*    Helper classes:
 *---*/

/*  PURPOSE:  To implement an interface that manages the character source.
 */
class InputCharStream
{
  //  I.  Member vars:
  //  PURPOSE:  To hold the input.
  std::string   input_;

  //  PURPOSE:  To hold where the cursor is.
  int     index_;

  //  II.  Disallowed auto-generated methods:
  
  //  III.  Protected methods:
protected :

public:
  //  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
  //  PURPOSE:  To
  InputCharStream (std::string& newInput
        ) :
      input_(newInput),
      index_(0)
      { }

  //  V.  Accessors:

  //  VI.  Mutators:

  //  VII.  Methods that do main and misc work of class:
  //  PURPOSE:  To return the current char, or '\0' if there are no more.
  //  No parameters.
  char    peek  ()
        const
      throw()
      { return
        ( (index_ >= input_.length())
          ? '\0' : input_[index_]
        );
      }

  //  PURPOSE:  To return 'true' if at eof-of-input, or 'false' otherwise.
  bool    isAtEnd ()
        const
      throw()
      { return(index_ >= input_.length()); }

  //  PURPOSE:  To advance to the next char (if not already at end).  No
  //  parameters.  No return value.
  void    advance ()
        throw()
      {
        if  (index_ < input_.length())  index_++;
      }

};



//  PURPOSE:  To implement an interface that gathers characters into lexemes.
class TokenStream
{
  //  I.  Member vars:
  //  PURPOSE:  To hold the source of the character input.
  InputCharStream&  inputCharStream_;

  //  PURPOSE:  To hold the lastest lexeme parsed.
  Symbol*     lastParsedPtr_;

  //  II.  Disallowed auto-generated methods:
  //  No default constructor:
  TokenStream   ();

  //  No copy constructor:
  TokenStream   (const TokenStream&
      );

  //  No copy assignment op:
  TokenStream&    operator=
      (const TokenStream&
      );

protected :
  //  III.  Protected methods:
  //  PURPOSE:  To return a pointer representing a scanned number, either
  //  an integer or floating point.  No parameters.
  Symbol*     scanDigits
      ()
      throw()
      {
        std::string lex("");

        while  ( isdigit(inputCharStream_.peek()) )
        {
          lex += inputCharStream_.peek();
          inputCharStream_.advance();
        }

        Symbol* symbolPtr;

        if  (inputCharStream_.peek() != '.')
        {
          symbolPtr   = (Symbol*)
                  malloc(sizeof(Symbol));
          symbolPtr->symbol_  = INT_SYMBOL;
          symbolPtr->value_.integer_
            = strtol(lex.c_str(),NULL,10);
        }
        else
        {
          lex += inputCharStream_.peek();
          inputCharStream_.advance();
          while  ( isdigit(inputCharStream_.peek()) )
          {
            lex += inputCharStream_.peek();
            inputCharStream_.advance();
          }

          symbolPtr   = (Symbol*)
                  malloc(sizeof(Symbol));
          symbolPtr->symbol_  = FLOAT_SYMBOL;
          symbolPtr->value_.floatPt_
            = strtod(lex.c_str(),NULL);
        }

        return(symbolPtr);
      }


  //  PURPOSE:  To return a pointer representing a scanned Symbol, or to return
  //  '&endSymbol' if the '*this' is at the end-of-input.  No parameters.
  Symbol* scanner ()
      throw(const char*)
        {
        while  ( isspace(inputCharStream_.peek()) )
          inputCharStream_.advance();

        if  ( inputCharStream_.isAtEnd() )
          return( &endSymbol );

        if  ( isdigit(inputCharStream_.peek()) )
          return( scanDigits() );

        char      ch    = inputCharStream_.peek();
        
        Symbol*   symbolPtr = (Symbol*)
                    malloc(sizeof(Symbol));

        inputCharStream_.advance();

        switch  (ch)
        {
        case '=' :
          symbolPtr->symbol_  = ASSIGN_SYMBOL;
          break;

        case '+' :
          symbolPtr->symbol_  = ADD_SYMBOL;
          break;

        case '-' :
          symbolPtr->symbol_  = SUBTRACT_SYMBOL;
          break;

        case 'p' :
          symbolPtr->symbol_  = PRINT_SYMBOL;
          break;

        case 'i' :
          // inputCharStream_.advance();
          // char ch1 = inputCharStream_.peek();
          // if(ch1=='n') printf("hahahhaha\n"); 
          symbolPtr->symbol_  = INT_DECLARE_SYMBOL;
          break;

        case 'f' :
          symbolPtr->symbol_  = FLOAT_DECLARE_SYMBOL;
          break;

        default :
          if  ( islower(ch) )
          {
            symbolPtr->symbol_  = ID_SYMBOL;
            symbolPtr->value_.varName_= ch;
            break;
          }

          throw "Unexpected character in input";
        }

        return(symbolPtr);
      }

public :
  //  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
  //  PURPOSE:  To initialize '*this'
  TokenStream   (InputCharStream& newInputCharStream
        )
      throw(const char*) :
      inputCharStream_(newInputCharStream),
      lastParsedPtr_(NULL)
      {
        advance();
      }

  //  V.  Accessors:

  //  VI.  Mutators:

  //  VII.  Methods that do main and misc work of class:
  //  PURPOSE:  To return the 'symbol_t' of the 'Symbol' instance that is next
  //  in the symbol stream.  No parameters.
  symbol_t    peek  ()
        throw(const char*)
      {
        if  (lastParsedPtr_ == NULL)
          lastParsedPtr_  = scanner();

        return(lastParsedPtr_->symbol_);
      }

  //  PURPOSE:  To return the pointer to the old Symbol at that was at the
  //  front of the symbol stream, and then to internally advance to the next
  //  Symbol instance (if not already at the end).  No parameters.
  Symbol* advance ()
        throw(const char*)
      {
        Symbol* toReturn  = lastParsedPtr_;

        lastParsedPtr_  = scanner();
        return(toReturn);
      }

};


//  PURPOSE:  To implement a class that holds the types of the symbols (well,
//  variables) that occur in the parsed program.
class SymbolTable
{
  //  0.  Internal constants and types:
  static
  const
  int     MAX_NUM_VARS  = 26*2;

  //  I.  Member vars:
  //  PURPOSE:  To hold the types for all possible vars.
  dcType_t    typeArray_[MAX_NUM_VARS];

  //  II.  Disallowed auto-generated methods:
  //  No copy constructor:
  SymbolTable   (const SymbolTable&
        );

  //  No copy assignment op:
  SymbolTable&    operator=
      (const SymbolTable&
        );

  //  III.  Protected methods:
protected :
  //  PURPOSE:  To return the index in 'typeArray_[]' for variable with name
  //  'varName'.
  static
  int   getIndex(char varName
        )
      {
        return( isupper(varName)
            ? (varName - 'A')
          : ((varName - 'a') + 26)
          );
      }

public:
  //  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
  //  PURPOSE:  To initialize '*this' table to hold no symbols.  No parameters.
  //  No return value.
  SymbolTable   ()
      throw()
        {
        for  (int i = 0;  i < MAX_NUM_VARS;  i++)
          typeArray_[i] = NULL_DC_TYPE;
      }

  ~SymbolTable    ()
      { }

  //  V.  Accessors:

  //  VI.  Mutators:

  //  VII.  Methods that do main and misc work of class:
  //  PURPOSE:  To store the type of the variable named 'varName' as being
  //  'varType', assuming there is no type already recorded.  No return
  //  value.
  void    enter (dcType_t varType,
         char   varName
        )
      throw(const char*)
      {
        int index = getIndex(varName);

        if  (typeArray_[index] != NULL_DC_TYPE) {
          snprintf(errorMessage,256,"Attempt to redefine symbol %c", varName);
          throw errorMessage;
          //throw "Attempt to redefine symbol";
        }
          
        typeArray_[index] = varType;
      }

  //  PURPOSE:  To return the type that is recorded for the variable named
  //  'varName'.
  dcType_t  lookUp  (char   varName
        )
      throw(const char*)
      {
        return(typeArray_[getIndex(varName)]);
      }

};


/*---*
 *---*    Global singletons:
 *---*/

//  PURPOSE:  To holds the types of the symbols (well, variables) that occur
//  in the parsed program.
SymbolTable   symbolTable;



/*---*
 *---*    Functions used directly to parse:
 *---*/

//  PURPOSE:  To ensure that a 'Symbol' instance of type 'expectedSymbol'
//  occurs next in the token stream 'tokenStream'.  Returns pointer to
//  symbol after successfully checked.
Symbol* expect    (TokenStream& tokenStream,
       symbol_t expectedSymbol
      )
      throw(const char*)
{
  Symbol* symbolPtr = tokenStream.advance();

  if  (symbolPtr->symbol_ != expectedSymbol)
    throw "Unexpected symbol";

  return(symbolPtr);
}


//  PURPOSE:  To parse a single variable declaration from 'tokenStream'.
//  Returns 'NULL' as no 'Symbol' instances need to be created to declare
//  a variable.
Symbol* parseDeclare  (TokenStream& tokenStream
      )
{
  Symbol* declarePtr;
  Symbol* varPtr;
  dcType_t  varType;

  if  (tokenStream.peek() == FLOAT_DECLARE_SYMBOL)
  {
    declarePtr  = expect(tokenStream,FLOAT_DECLARE_SYMBOL);
    varPtr  = expect(tokenStream,ID_SYMBOL);
    varType = FLOAT_POINT_DC_TYPE;
  }
  else
  if  (tokenStream.peek() == INT_DECLARE_SYMBOL)
  {
    declarePtr  = expect(tokenStream,INT_DECLARE_SYMBOL);
    varPtr  = expect(tokenStream,ID_SYMBOL);
    varType = INTEGER_DC_TYPE;
  }
  else
    throw "expected float or int declaration";

  symbolTable.enter(varType,varPtr->value_.varName_);
  free(varPtr);
  free(declarePtr);

  return(NULL);
}


//  PURPOSE:  To parse zero or more variable declarations from 'tokenStream'.
//  Returns 'NULL' as no 'Symbol' instances need to be created to declare
//  a variable.
Symbol* parseDeclares (TokenStream& tokenStream
      )
{
  if  ( (tokenStream.peek() == FLOAT_DECLARE_SYMBOL) ||
        (tokenStream.peek() == INT_DECLARE_SYMBOL)
      )
  {
    parseDeclare(tokenStream);
    parseDeclares(tokenStream);
  }
  else
  if  ( (tokenStream.peek() == ID_SYMBOL) ||
  (tokenStream.peek() == PRINT_SYMBOL)  ||
  (tokenStream.peek() == END_OF_FILE_SYMBOL)
      )
  {
    //  lambda production
  }
  else
    throw "expected floatdcl, intdcl, id, print, or eof";

  return(NULL);
}


//  PURPOSE:  To parse and return a pointer to a 'Symbol' that represents a
//  value from 'tokenStream'.
Symbol* parseValue  (TokenStream& tokenStream
      )
{
  Symbol* symbolPtr;

  if  (tokenStream.peek() == ID_SYMBOL)
    symbolPtr = expect(tokenStream,ID_SYMBOL);
  else
  if  (tokenStream.peek() == INT_SYMBOL)
    symbolPtr = expect(tokenStream,INT_SYMBOL);
  else
  if  (tokenStream.peek() == FLOAT_SYMBOL)
    symbolPtr = expect(tokenStream,FLOAT_SYMBOL);
  else
    throw "expected id, inum, or fnum";

  return(symbolPtr);
}


//  PURPOSE:  To parse and return a pointer to a 'Symbol' that represents an
//  expression from 'tokenStream'.
Symbol* parseExpression (TokenStream& tokenStream
      )
{
  Symbol* symbolPtr;

  if  (tokenStream.peek() == ADD_SYMBOL)
  {
    symbolPtr       = expect(tokenStream,ADD_SYMBOL);
    symbolPtr->value_.expression_.lhsPtr_ = parseValue(tokenStream);
    symbolPtr->value_.expression_.rhsPtr_ = parseExpression(tokenStream);
  }
  else
  if  (tokenStream.peek() == SUBTRACT_SYMBOL)
  {
    symbolPtr       = expect(tokenStream,SUBTRACT_SYMBOL);
    symbolPtr->value_.expression_.lhsPtr_ = parseValue(tokenStream);
    symbolPtr->value_.expression_.rhsPtr_ = parseExpression(tokenStream);
  }
  else
  if  ( (tokenStream.peek() == ID_SYMBOL) ||
  (tokenStream.peek() == PRINT_SYMBOL)  ||
  (tokenStream.peek() == END_OF_FILE_SYMBOL)
      )
  {
    // Do nothing for lambda-production
    symbolPtr         = NULL;
  }
  else
    throw "expected plus, minus, id, print, or eof";

  return(symbolPtr);
}


//  PURPOSE:  To parse and return a pointer to a 'Symbol' that represents a
//  statement from 'tokenStream'.
Symbol* parseStatement  (TokenStream& tokenStream
      )
{
  Symbol* symbolPtr;

  if  (tokenStream.peek() == ID_SYMBOL)
  {
    Symbol* varPtr  = expect(tokenStream,ID_SYMBOL);

    symbolPtr   = expect(tokenStream,ASSIGN_SYMBOL);
    symbolPtr->value_.assignment_.varPtr_ = varPtr;
    symbolPtr->value_.assignment_.lhsPtr_ = parseValue(tokenStream);
    symbolPtr->value_.assignment_.rhsPtr_ = parseExpression(tokenStream);
  }
  else
  if  (tokenStream.peek() == PRINT_SYMBOL)
  {
    symbolPtr         = expect(tokenStream,PRINT_SYMBOL);
    symbolPtr->value_.expression_.lhsPtr_ = expect(tokenStream,ID_SYMBOL);
  }
  else
    throw "expected id or print";

  return(symbolPtr);
}


//  PURPOSE:  To parse and return a pointer to a 'Symbol' that represents zero
//  or more statements from 'tokenStream'.
Symbol* parseStatements (TokenStream& tokenStream
      )
{
  Symbol* symbolPtr;

  if  (tokenStream.peek() == ID_SYMBOL || tokenStream.peek() == PRINT_SYMBOL)
  {
    symbolPtr = (Symbol*)malloc(sizeof(Symbol));

    symbolPtr->symbol_      = STATEMENT_LIST_SYMBOL;
    symbolPtr->value_.statement_.firstPtr_
          = parseStatement(tokenStream);
    symbolPtr->value_.statement_.restPtr_
          = parseStatements(tokenStream);
  }
  else
  if (tokenStream.peek() == END_OF_FILE_SYMBOL)
  {
    // Do nothing for lambda-production
    symbolPtr = &endSymbol;
  }
  else
    throw "expected id, print, or eof";

  return(symbolPtr);
}


//  PURPOSE:  To parse and return a pointer to a 'Symbol' that a whole program
//  from 'tokenStream'.
Symbol* parseProg (TokenStream& tokenStream
      )
{
  Symbol* symbolPtr;

  if  ( (tokenStream.peek() == FLOAT_DECLARE_SYMBOL)  ||
  (tokenStream.peek() == INT_DECLARE_SYMBOL)  ||
  (tokenStream.peek() == ID_SYMBOL)   ||
  (tokenStream.peek() == PRINT_SYMBOL)    ||
  (tokenStream.peek() == END_OF_FILE_SYMBOL)
      )
  {
        parseDeclares(tokenStream);
  symbolPtr = parseStatements(tokenStream);
        expect(tokenStream,END_OF_FILE_SYMBOL);
  }
  else
    throw "expected floatdcl, intdcl, id, print, or eof";

  return(symbolPtr);
}



/*---*
 *---*    Functions used to process the parse tree after parsing:
 *---*/

//  PURPOSE:  To return the more general type among 'first' or 'second'.
dcType_t
  generalize  (dcType_t first,
       dcType_t second
      )
      throw()
{
  return( ( (first == FLOAT_POINT_DC_TYPE) || (second == FLOAT_POINT_DC_TYPE) )
    ? FLOAT_POINT_DC_TYPE
    : INTEGER_DC_TYPE
  );
}


dcType_t
  getType   (Symbol*  symbolPtr
      )
      throw(const char*);


//  PURPOSE:  To return a node that casts '*symbolPtr' to type 'desiredType',
//  if it is needed.  Just returns 'symbolPtr' if not needed.
Symbol* convert   (Symbol*  symbolPtr,
       dcType_t desiredType
      )
      throw(const char*)
{
  if  ( (getType(symbolPtr) == FLOAT_POINT_DC_TYPE) &&
  (desiredType      == INTEGER_DC_TYPE)
      ){
      snprintf(errorMessage,256,"Type mismatch %u", symbolPtr->symbol_);
      throw errorMessage;
      //throw "Type mismatch"
  }
    ;

  Symbol* toReturn  = symbolPtr;

  if  ( (getType(symbolPtr) == INTEGER_DC_TYPE) &&
  (desiredType      == FLOAT_POINT_DC_TYPE)
      )
  {
    toReturn         = (Symbol*)malloc(sizeof(Symbol));
    toReturn->symbol_      = TYPE_CONVERT_SYMBOL;
    toReturn->value_.expression_.lhsPtr_ = symbolPtr;
  }

  return(toReturn);
}


//  PURPOSE:  To return the type of the value or expression represented by
//  'symbolPtr'.  Checks and adds conversion Symbol instances as needed. 
dcType_t
  getType   (Symbol*  symbolPtr
      )
      throw(const char*)
{
  if  (symbolPtr == NULL)
    return(NULL_DC_TYPE);

  switch  (symbolPtr->symbol_)
  {
  case INT_SYMBOL :
    return(INTEGER_DC_TYPE);

  case FLOAT_SYMBOL :
  case TYPE_CONVERT_SYMBOL :
    return(FLOAT_POINT_DC_TYPE);

  case ID_SYMBOL :
    return(symbolTable.lookUp(symbolPtr->value_.varName_));

  case ADD_SYMBOL :
  case SUBTRACT_SYMBOL :
    {
      dcType_t  newType =
          generalize(getType(symbolPtr->value_.expression_.lhsPtr_),
         getType(symbolPtr->value_.expression_.rhsPtr_)
        );

      symbolPtr->value_.expression_.lhsPtr_
      = convert(symbolPtr->value_.expression_.lhsPtr_,
          newType
         );
      symbolPtr->value_.expression_.rhsPtr_
      = convert(symbolPtr->value_.expression_.rhsPtr_,
          newType
         );
      return(newType);
    }

  case ASSIGN_SYMBOL :
  case PRINT_SYMBOL :
  case STATEMENT_LIST_SYMBOL :
    return(NULL_DC_TYPE);
  }
}


//  PURPOSE:  To check the consistency (e.g. between expected and given type)
//  at '*symbolPtr' and below.  No return value.
void  checkConsistency(Symbol*  symbolPtr
      )
      throw(const char*)
{
  if  (symbolPtr == NULL)
    return;

  switch  (symbolPtr->symbol_)
  {
  case END_OF_FILE_SYMBOL :
    break;

  case INT_SYMBOL :
  case FLOAT_SYMBOL :
  case ID_SYMBOL :
    break;

  case ADD_SYMBOL :
  case SUBTRACT_SYMBOL :
    getType(symbolPtr);
    break;

  case PRINT_SYMBOL :
    break;

  case ASSIGN_SYMBOL :
    symbolPtr->value_.assignment_.lhsPtr_
      = convert(symbolPtr->value_.assignment_.lhsPtr_,
          getType(symbolPtr->value_.assignment_.varPtr_)
         );
    break;

  case STATEMENT_LIST_SYMBOL :
    checkConsistency(symbolPtr->value_.statement_.firstPtr_);
    checkConsistency(symbolPtr->value_.statement_.restPtr_);
    break;
  }

}


//  PURPOSE:  To output the 'dc' code that implements the program at
//  '*symbolPtr'.  No return value.
void  outputForDC (Symbol*  symbolPtr
      )
{
  if  (symbolPtr == NULL)
    return;

  switch  (symbolPtr->symbol_)
  {
  case INT_SYMBOL :
    std::cout << symbolPtr->value_.integer_ << std::endl;
    break;

  case FLOAT_SYMBOL :
    std::cout << symbolPtr->value_.floatPt_ << std::endl;
    break;

  case ID_SYMBOL :
    std::cout << "l" << symbolPtr->value_.varName_ << std::endl;
    break;

  case ADD_SYMBOL :
    outputForDC(symbolPtr->value_.expression_.rhsPtr_);
    outputForDC(symbolPtr->value_.expression_.lhsPtr_);
    std::cout << "+" << std::endl;
    break;

  case SUBTRACT_SYMBOL :
    outputForDC(symbolPtr->value_.expression_.rhsPtr_);
    outputForDC(symbolPtr->value_.expression_.lhsPtr_);
    std::cout << "-" << std::endl;
    break;

  case PRINT_SYMBOL :
    std::cout << "l" << symbolPtr->value_.expression_.lhsPtr_->value_.varName_
        << std::endl;
    std::cout << "p" << std::endl;
    std::cout << "si" << std::endl;
    break;

  case ASSIGN_SYMBOL :
    outputForDC(symbolPtr->value_.assignment_.lhsPtr_);
    outputForDC(symbolPtr->value_.assignment_.rhsPtr_);
    std::cout << "s" << symbolPtr->value_.assignment_.varPtr_->value_.varName_
        << std::endl;
    std::cout << "0 k" << std::endl;
    break;

  case STATEMENT_LIST_SYMBOL :
    outputForDC(symbolPtr->value_.statement_.firstPtr_);
    outputForDC(symbolPtr->value_.statement_.restPtr_);
    break;

  case TYPE_CONVERT_SYMBOL :
    outputForDC(symbolPtr->value_.expression_.lhsPtr_);
    std::cout << "5 k" << std::endl;
    break;
  }

}


//  PURPOSE:  To recursively 'free()' the memory at 'symbolPtr' and below.
//  No return value.
void  recursivelyFree (Symbol*  symbolPtr
      )
{
  if  ( (symbolPtr == NULL)  ||  (symbolPtr == &endSymbol) )
    return;

  switch  (symbolPtr->symbol_)
  {
  case INT_SYMBOL :
  case FLOAT_SYMBOL :
  case ID_SYMBOL :
    break;

  case ADD_SYMBOL :
  case SUBTRACT_SYMBOL :
    recursivelyFree(symbolPtr->value_.expression_.lhsPtr_);
    recursivelyFree(symbolPtr->value_.expression_.rhsPtr_);
    break;

  case PRINT_SYMBOL :
    recursivelyFree(symbolPtr->value_.expression_.lhsPtr_);
    break;

  case ASSIGN_SYMBOL :
    recursivelyFree(symbolPtr->value_.assignment_.rhsPtr_);
    recursivelyFree(symbolPtr->value_.assignment_.lhsPtr_);
    recursivelyFree(symbolPtr->value_.assignment_.varPtr_);
    break;

  case STATEMENT_LIST_SYMBOL :
    recursivelyFree(symbolPtr->value_.statement_.restPtr_);
    recursivelyFree(symbolPtr->value_.statement_.firstPtr_);
    break;
  }

  free(symbolPtr);
}



/*---*
 *---*    Functions used to interact with the user:
 *---*/

//  PURPOSE:  To return keyboard input from the user.  No parameters.
std::string
  userStdInInput  ()
{
  std::string input;

  std::cout << "Get me an AC program to translate!" << std::endl;
  std::cout << "(e.g.  f b i a a = 5 b = a + 3.2 p b)" << std::endl;
  std::cout << "AC program to translate to DC? ";
  std::getline(std::cin,input);
  return(input);
}


//  PURPOSE:  To run the AC to DC conversion program.  Uses 'argv[1]' as input
//  if 'argc' > 1.  Returns 'EXIT_SUCCESS' on success or 'EXIT_FAILURE'
//  otherwise.
int main    (int    argc,
       char*    argv[]
      )
{
  endSymbol.symbol_   = END_OF_FILE_SYMBOL;
  Symbol* symbolPtr = NULL;
  int   status    = EXIT_SUCCESS;
  std::string input   = (argc > 1)
          ? std::string(argv[1])
          : userStdInInput();
  InputCharStream
    inputCharStream(input);
  TokenStream tokenStream(inputCharStream);

  try
  {
    symbolPtr = parseProg(tokenStream);
    checkConsistency(symbolPtr);
    outputForDC(symbolPtr);
    recursivelyFree(symbolPtr);
  }
  catch  (const char* cPtr
   )
  {
    std::cerr << cPtr << '\n';
    status  = EXIT_FAILURE;
  }

  return(status);
}