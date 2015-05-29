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
  char*		varNamePtr_;
  Statement*	exprPtr_;
public :
  AssignStatement	(char*		varNamePtr,
			 Statement*	exprPtr
			)
		{
		  varNamePtr_	= varNamePtr;
		  exprPtr_	= exprPtr;
		}

  ~AssignStatement	()
		{
		  delete(exprPtr_);
		  free(varNamePtr_);
		}

  double eval		()
		{
		  double	value = exprPtr_->eval();

		  varStore.assign(varNamePtr_,value);
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
  char*		varNamePtr_;
public :
  RValVarNameStatement	(char*	varNamePtr
  			)
		{
		  varNamePtr_ = varNamePtr;
		}

  ~RValVarNameStatement	()
  		{
		  free(varNamePtr_);
		}

  double eval		()
  	 	{
		  return(varStore.retrieve(varNamePtr_));
		}
};


class	PrintStatement : public Statement
{
  Statement*		exprPtr_;
public :
  PrintStatement	(Statement*	exprPtr
  			)
		{
		  exprPtr_ = exprPtr;
		}

  ~PrintStatement	()
  		{
		  delete(exprPtr_);
		}

  double eval		()
  	 	{
		  double	result	= exprPtr_->eval();

		  printf("%g\n",result);
		  return(result);
		}
};


class	ReadStatement : public Statement
{
  char*		varNamePtr_;
public :
  ReadStatement	(char*	varNamePtr
  			)
		{
		  varNamePtr_ = varNamePtr;
		}

  ~ReadStatement	()
  		{
		  free(varNamePtr_);
		}

  double eval		()
  	 	{
		  char		text[256];
		  double	result;

		  fgets(text,256,stdin);
		  result	= strtod(text,NULL);
		  varStore.assign(varNamePtr_,result);
		  return(result);
		}
};
