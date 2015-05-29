/*-------------------------------------------------------------------------*
 *---									---*
 *---		Statement.h						---*
 *---									---*
 *---	    This file declares (and, well, also defines) classes	---*
 *---	of a hierarchy necessary to implement the work of an		---*
 *---	*interpreted* language inspired by C.				---*
 *---									---*
 *---	----	----	----	----	----	----	----	----	---*
 *---									---*
 *---	Version 1.0		2015 May 22		Joseph Phillips	---*
 *---									---*
 *-------------------------------------------------------------------------*/

#include	"vector"

class	Statement
{
public :
  Statement	()	{ }

  virtual
  ~Statement	()	{ }

  virtual
  double eval	()	= 0;
};


class	IfStatement : public Statement
{
  Statement*	condPtr_;
  Statement*	thenPtr_;
  Statement*	elsePtr_;
public :
  IfStatement	(Statement*	condPtr,
		 Statement*	thenPtr,
		 Statement*	elsePtr	= NULL
		)
		{
		  condPtr_	= condPtr;
		  thenPtr_	= thenPtr;
		  elsePtr_	= elsePtr;
		}

  ~IfStatement	()
		{
		  delete(elsePtr_);
		  delete(thenPtr_);
		  delete(condPtr_);
		}

  double eval	()
		{
		  return( (condPtr_->eval() != 0.0)
			  ? thenPtr_->eval()
			  : ( (elsePtr_ == NULL)
			      ? 0.0
			      : elsePtr_->eval()
			    )
			);
		}
};


class WhileStatement : public Statement
{
  Statement*	condPtr_;
  Statement*	blockPtr_;
public :
  WhileStatement	(Statement*	condPtr,
			 Statement*	blockPtr
			)
		{
		  condPtr_	= condPtr;
		  blockPtr_	= blockPtr;
		}

  ~WhileStatement	()
		{
		  delete(blockPtr_);
		  delete(condPtr_);
		}

  double eval	()
 		{
		  double	result	= 0.0;

		  while  (condPtr_->eval() != 0.0)
		    result = blockPtr_->eval();

		  return(result);
		}
};


class BlockStatement : public Statement
{
  std::vector<Statement*>	list_;
public :
  BlockStatement	()	{ }

  ~BlockStatement	()
		{
		  for  (int i = 0; i < list_.size();  i++)
		    delete(list_[i]);
		}

  void	add		(Statement*	statePtr
			)
		{
		  list_.push_back(statePtr);
		}

  double eval		()
		{
		  double	result	= 0.0;

		  for  (int i = 0;  i < list_.size();  i++)
		    result = list_[i]->eval();

		  return(result);
		}
};


class AssignStatement : public Statement
{
  VarEntry*	varPtr_;
  Statement*	exprPtr_;
public :
  AssignStatement	(VarEntry*	varPtr,
			 Statement*	exprPtr
			)
		{
		  varPtr_	= varPtr;
		  exprPtr_	= exprPtr;
		}

  ~AssignStatement	()
		{
		  delete(exprPtr_);
		}

  double eval		()
		{
		  double	value = exprPtr_->eval();

		  varPtr_->setValue(value);
		  return(value);
		}
};


class BinaryOpStatement : public Statement
{
  Statement*	lhsPtr_;
  char		op_;
  Statement*	rhsPtr_;
public :
  BinaryOpStatement	(Statement*	lhsPtr,
			 char		op,
			 Statement*	rhsPtr
			)
		{
		  lhsPtr_	= lhsPtr;
		  op_		= op;
		  rhsPtr_	= rhsPtr;
		}

  ~BinaryOpStatement	()
		{
		  delete(rhsPtr_);
		  delete(lhsPtr_);
		}

  double eval		()
		{
		  double	result;
		  double	lhs	= lhsPtr_->eval();
		  double	rhs	= rhsPtr_->eval();

		  switch  (op_)
		  {
		  case '+' :	result	= lhs + rhs; break;
		  case '-' :	result	= lhs - rhs; break;
		  case '*' :	result	= lhs * rhs; break;
		  case '/' :	result	= lhs / rhs; break;
		  case '<' :	result	= (lhs < rhs); break;
		  case '&' :	result	= (lhs && rhs); break;
		  }

		  return(result);
		}
};


class	ConstantStatement : public Statement
{
  double	const_;
public :
  ConstantStatement	(double	constant
  			)
		{
		  const_ = constant;
		}

  double eval		()
  	 	{
		  return(const_);
		}
};


class	RValVarNameStatement : public Statement
{
  VarEntry*	varPtr_;
public :
  RValVarNameStatement	(VarEntry*	varPtr
  			)
		{
		  varPtr_ = varPtr;
		}

  ~RValVarNameStatement	()	{ }

  double eval		()
  	 	{
		  return(varPtr_->getValue());
		}
};


class	PrintStatement : public Statement
{
  Statement*		exprPtr_;
  char*			charPtr_;
  bool			shouldAddNewline_;
public :
  PrintStatement	(Statement*	exprPtr,
  			 bool		newShouldAddNewline
  			)
		{
		  exprPtr_		= exprPtr;
		  charPtr_		= NULL;
		  shouldAddNewline_	= newShouldAddNewline;
		}

  PrintStatement	(char*	charPtr,
  			 bool	newShouldAddNewline
  			)
		{
		  exprPtr_		= NULL;
		  charPtr_		= charPtr;
		  shouldAddNewline_	= newShouldAddNewline;
		}

  ~PrintStatement	()
  		{
		  delete(exprPtr_);
		  if  (charPtr_ != NULL)  free(charPtr_);
		}

  double eval		()
  	 	{
		  if  (exprPtr_ == NULL)
		  {
		    printf(charPtr_);
		    if  (shouldAddNewline_)  putchar('\n');
		    return(0.0);
		  }

		  double	result	= exprPtr_->eval();

		  printf("%g",result);
		  if  (shouldAddNewline_)  putchar('\n');
		  return(result);
		}
};


class	ReadStatement : public Statement
{
  VarEntry*	varPtr_;
public :
  ReadStatement	(VarEntry*	varPtr
  		)
		{
		  varPtr_ = varPtr;
		}

  ~ReadStatement	()
  		{ }

  double eval		()
  	 	{
		  char		text[256];
		  double	result;

		  fgets(text,256,stdin);
		  result	= strtod(text,NULL);
		  varPtr_->setValue(result);
		  return(result);
		}
};
Test programs:

//  'bad0.prog'
//  This should fail: "Attempt to close too many scopes."

}

//  'bad1.prog'
//  This should fail: "Attempt to redeclare d."
{
  declare	d;
  declare	e;
  declare	d;
}


//  'bad2.prog'
//  This should fail: "c is undeclared."
{
  declare a;
  declare b;

  c = 10;
}


//  'good0.prog'
//  This program tests scopes:
{
  declare	a;

  a		= 10;
  print	  "Outer a = ";
  println a;

  {
    declare	a;

    a		= 20;
    print   "Inner a = ";
    println a;
  }

  print	  "Outer a = ";
  println a;
}


//  'good1.prog'
{
  //  I.  Get first number:
  declare	first;
  print "Hello, please enter a number: ";
  read first;

  //  II.  Get second number:
  declare	second;
  second = first - 1;

  while  (second < first)
  {
    print "Please enter a number greater than ";
    print first;
    print ": ";
    read second;
  }

  //  III.  Compute sum from from '
  declare	run;
  declare	sum;

  run	= first;
  sum	= 0;

  while  (run < second)
  {
    sum = sum + run;
    run = run + 1;
  }

  sum	= sum + run;

  //  IV.  Print sum:
  print   "The sum from ";
  print   first;
  print   " to ";
  print   second;
  print   " is ";
  println sum;
}

// #include	"vector"

// class	Statement
// {
// public :
//   Statement	()	{ }

//   virtual
//   ~Statement	()	{ }

//   virtual
//   double eval	()	= 0;
// };


// class	IfStatement : public Statement
// {
//   Statement*	condPtr_;
//   Statement*	thenPtr_;
//   Statement*	elsePtr_;
// public :
//   IfStatement	(Statement*	condPtr,
// 		 Statement*	thenPtr,
// 		 Statement*	elsePtr	= NULL
// 		)
// 		{
// 		  condPtr_	= condPtr;
// 		  thenPtr_	= thenPtr;
// 		  elsePtr_	= elsePtr;
// 		}

//   ~IfStatement	()
// 		{
// 		  delete(elsePtr_);
// 		  delete(thenPtr_);
// 		  delete(condPtr_);
// 		}

//   double eval	()
// 		{
// 		  return( (condPtr_->eval() != 0.0)
// 			  ? thenPtr_->eval()
// 			  : ( (elsePtr_ == NULL)
// 			      ? 0.0
// 			      : elsePtr_->eval()
// 			    )
// 			);
// 		}
// };


// class WhileStatement : public Statement
// {
//   Statement*	condPtr_;
//   Statement*	blockPtr_;
// public :
//   WhileStatement	(Statement*	condPtr,
// 			 Statement*	blockPtr
// 			)
// 		{
// 		  condPtr_	= condPtr;
// 		  blockPtr_	= blockPtr;
// 		}

//   ~WhileStatement	()
// 		{
// 		  delete(blockPtr_);
// 		  delete(condPtr_);
// 		}

//   double eval	()
//  		{
// 		  double	result	= 0.0;

// 		  while  (condPtr_->eval() != 0.0)
// 		    result = blockPtr_->eval();

// 		  return(result);
// 		}
// };


// class BlockStatement : public Statement
// {
//   std::vector<Statement*>	list_;
// public :
//   BlockStatement	()	{ }

//   ~BlockStatement	()
// 		{
// 		  for  (int i = 0; i < list_.size();  i++)
// 		    delete(list_[i]);
// 		}

//   void	add		(Statement*	statePtr
// 			)
// 		{
// 		  list_.push_back(statePtr);
// 		}

//   double eval		()
// 		{
// 		  double	result	= 0.0;

// 		  for  (int i = 0;  i < list_.size();  i++)
// 		    result = list_[i]->eval();

// 		  return(result);
// 		}
// };


// class AssignStatement : public Statement
// {
//   char*		varNamePtr_;
//   Statement*	exprPtr_;
// public :
//   AssignStatement	(char*		varNamePtr,
// 			 Statement*	exprPtr
// 			)
// 		{
// 		  varNamePtr_	= varNamePtr;
// 		  exprPtr_	= exprPtr;
// 		}

//   ~AssignStatement	()
// 		{
// 		  delete(exprPtr_);
// 		  free(varNamePtr_);
// 		}

//   double eval		()
// 		{
// 		  double	value = exprPtr_->eval();

// 		  varStore.assign(varNamePtr_,value);
// 		  return(value);
// 		}
// };


// class BinaryOpStatement : public Statement
// {
//   Statement*	lhsPtr_;
//   char		op_;
//   Statement*	rhsPtr_;
// public :
//   BinaryOpStatement	(Statement*	lhsPtr,
// 			 char		op,
// 			 Statement*	rhsPtr
// 			)
// 		{
// 		  lhsPtr_	= lhsPtr;
// 		  op_		= op;
// 		  rhsPtr_	= rhsPtr;
// 		}

//   ~BinaryOpStatement	()
// 		{
// 		  delete(rhsPtr_);
// 		  delete(lhsPtr_);
// 		}

//   double eval		()
// 		{
// 		  double	result;
// 		  double	lhs	= lhsPtr_->eval();
// 		  double	rhs	= rhsPtr_->eval();

// 		  switch  (op_)
// 		  {
// 		  case '+' :	result	= lhs + rhs; break;
// 		  case '-' :	result	= lhs - rhs; break;
// 		  case '*' :	result	= lhs * rhs; break;
// 		  case '/' :	result	= lhs / rhs; break;
// 		  case '<' :	result	= (lhs < rhs); break;
// 		  case '&' :	result	= (lhs && rhs); break;
// 		  }

// 		  return(result);
// 		}
// };


// class	ConstantStatement : public Statement
// {
//   double	const_;
// public :
//   ConstantStatement	(double	constant
//   			)
// 		{
// 		  const_ = constant;
// 		}

//   double eval		()
//   	 	{
// 		  return(const_);
// 		}
// };


// class	RValVarNameStatement : public Statement
// {
//   char*		varNamePtr_;
// public :
//   RValVarNameStatement	(char*	varNamePtr
//   			)
// 		{
// 		  varNamePtr_ = varNamePtr;
// 		}

//   ~RValVarNameStatement	()
//   		{
// 		  free(varNamePtr_);
// 		}

//   double eval		()
//   	 	{
// 		  return(varStore.retrieve(varNamePtr_));
// 		}
// };


// class	PrintStatement : public Statement
// {
//   Statement*		exprPtr_;
// public :
//   PrintStatement	(Statement*	exprPtr
//   			)
// 		{
// 		  exprPtr_ = exprPtr;
// 		}

//   ~PrintStatement	()
//   		{
// 		  delete(exprPtr_);
// 		}

//   double eval		()
//   	 	{
// 		  double	result	= exprPtr_->eval();

// 		  printf("%g\n",result);
// 		  return(result);
// 		}
// };


// class	ReadStatement : public Statement
// {
//   char*		varNamePtr_;
// public :
//   ReadStatement	(char*	varNamePtr
//   			)
// 		{
// 		  varNamePtr_ = varNamePtr;
// 		}

//   ~ReadStatement	()
//   		{
// 		  free(varNamePtr_);
// 		}

//   double eval		()
//   	 	{
// 		  char		text[256];
// 		  double	result;

// 		  fgets(text,256,stdin);
// 		  result	= strtod(text,NULL);
// 		  varStore.assign(varNamePtr_,result);
// 		  return(result);
// 		}
// };
