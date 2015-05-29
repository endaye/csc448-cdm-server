/*-------------------------------------------------------------------------*
 *---									---*
 *---		SymbolTable.h						---*
 *---									---*
 *---	    This file declares (and, well, also defines) classes	---*
 *---	necessary to define a symbol table for an *interpreted*		---*
 *---	language inspired from the chapter on compiler symbol tables	---*
 *---	as described by Charles Fisher, Ron Cytron, Richard LeBlanc Jr.	---*
 *---	"Crafting a Compiler", Addison Wesley, 2010.  Specifically, it	---*
 *---	declares (and, well, also defines) classes:			---*
 *---		VarEntry						---*
 *---		SymbolTable						---*
 *---									---*
 *---	----	----	----	----	----	----	----	----	---*
 *---									---*
 *---	Version 1.0		2015 May 22		Joseph Phillips	---*
 *---									---*
 *-------------------------------------------------------------------------*/

#include	<map>
#include	<vector>
#include	<string>


//  PURPOSE:  To hold data on one particular Variable.
 class		VarEntry
 {
  //  I.  Member vars:
  //  PURPOSE:  To hold the name of '*this' VarEntry.
  std::string			name_;

  //  PURPOSE:  To hold the scope depth of '*this' VarEntry.
  int				depth_;

  //  PURPOSE:  To hold the address of next VarEntry from a previous depth
  //  	that shares the same name as '*this'.
  VarEntry*			prevOfSameNamePtr_;

  //  PURPOSE:  To hold the address of next VarEntry at the same depth as
  //  	'*this'.
  VarEntry*			nextOnSameLevelPtr_;

  //  PURPOSE:  To hold the address of next VarEntry, of any level.
  VarEntry*			nextPtr_;

  //  PURPOSE:  To hold the value for '*this' VarEntry.
  double			value_;


  //  II.  Disallowed auto-generated methods:
  //  No default constructor:
  VarEntry			();

  //  No copy constructor:
  VarEntry			(VarEntry&);

  //  No copy assignment op:
  VarEntry&	operator=	(VarEntry&);

  protected :
  //  III.  Protected methods:

  public :
  //  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
  //  PURPOSE:  To create a new 'VarEntry' instance with name 'newName' and at
  //	depth 'newDepth'.  Also inserts '*this' in the linked list of
  //	existing VarEntry instances with the same name at lower depths pointed
  //	to by 'newPrevOfSameNamePtr', in the linked list of VarEntry instances
  //	at the same depth pointed to by 'newNextOnSameLevelPtr', and in the
  //	linked list of all VarEntry instances pointed to by 'newNextPtr'.
  VarEntry			(const std::string&	newName,
   int   			newDepth,
   VarEntry*		newPrevOfSameNamePtr,
   VarEntry*		newNextOnSameLevelPtr,
   VarEntry*		newNextPtr
   )
  throw() :
  name_(newName),
  depth_(newDepth),
  prevOfSameNamePtr_(newPrevOfSameNamePtr),
  nextOnSameLevelPtr_(newNextOnSameLevelPtr),
  nextPtr_(newNextPtr),
  value_(0.0)
  { }

  //  PURPOSE:  To release resources.  No parameters.  No return value.
  ~VarEntry			()
  {
    delete(nextPtr_);
  }

  //  V.  Accessors:
  //  PURPOSE:  To return the name of '*this' VarEntry.  No parameters.
  const std::string&
  getName		()
  const
  throw()
  { return(name_); }

  //  PURPOSE:  To return the scope depth of '*this' VarEntry.  No parameters.
  int		getDepth	()
  const
  throw()
  { return(depth_); }

  //  PURPOSE:  To return the address of the previous VarEntry of a lower
  //	scope depth with the same name as '*this' VarEntry, or NULL if one
  //	does not exist.  No parameters.
  VarEntry*	getPrevOfSameNamePtr
  ()
  const
  throw()
  { return(prevOfSameNamePtr_); }

  //  PURPOSE:  To return the address of the next VarEntry at the same
  //	scope depth as '*this' VarEntry.  No parameters.
  VarEntry*	getNextOnSameLevelPtr
  ()
  const
  throw()
  { return(nextOnSameLevelPtr_); }

  //  PURPOSE:  To return the value of '*this' VarEntry.  No parameters.
  double	getValue	()
  const
  throw()
  { return(value_); }

  //  VI.  Mutators:
  //  PURPOSE:  To set the value of '*this' VarEntry to 'value'.  No return
  //	value.
  void		setValue	(double		value
    )
  throw()
  { value_ = value; }

  //  VII.  Methods that do main and misc work of class:
};


//  PURPOSE:  To hold data on the variables as they are parsed.  NOTE:  It
//	purposefully does not hold the completed list of all created variables.
//	That is *not* its job.
class		SymbolTable
{
  //  I.  Member vars:
  //  PURPOSE:  To map from variable names to the address of the VarEntry
  //	instance at the highest depth that has that name. 
  std::map<std::string,VarEntry*>
  nameToEntryMap_;

  //  PURPOSE:  To hold a starting pointer to a linked list (maintained by
  //	member var 'nextOnSameLevelPtr_') of VarEntry instances at the
  //	depth indexed by the corresponding std::vector index.
  std::vector<VarEntry*>	scopeStack_;

  //  PURPOSE:  To keep track of the current scope depth.
  int				depth_;

  //  II.  Disallowed auto-generated methods:
  //  No copy constructor:
  SymbolTable			(const SymbolTable&);

  //  No copy assignment op:
  SymbolTable&	operator=	(const SymbolTable&);

  protected :
  //  III.  Protected methods:

  public :
  //  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
  //  PURPOSE:  To initialize '*this' SymbolTable to be empty.  No parameters.
  //	No return value.
  SymbolTable			() :
  depth_(0)
  { scopeStack_.push_back(NULL); }

  //  PURPOSE:  To release resources.  No parameters.  No return value.
  ~SymbolTable			()
  { }

  //  V.  Accessors:

  //  VI.  Mutators:

  //  VII.  Methods that do main and misc work of class:
  //  PURPOSE:  To record that a new scope depth has been entered.  No
  //	parameters.  No return value.  Throws ptr to 'std::string' instance
  //	that describes an error if an error is detected.
  void		openScope	()
  throw(std::string*)
  {
    //  I.  Application validity check:

    //  II.  Enter new, deeper, scope:
    //  II.A.  Update 'depth_':
    depth_++;

    //  II.B.  Update 'scopeStack_' to have a new, blank entry at 'depth_':
    if  (depth_ >= scopeStack_.size())
      scopeStack_.push_back(NULL);
    else
      scopeStack_[depth_]	= NULL;

    //  III.  Finished:
  }

  //  PURPOSE:  To record that the compiler has left a scope depth.  No
  //	parameters.  No return value.  Throws ptr to 'std::string' instance
  //	that describes an error if an error is detected.
  void		closeScope	()
  throw(std::string*)
  {
    //  I.  Application validity check:
    if  (depth_ == 0)
      throw new std::string("Attempt to close too many scopes");

    //  II.  Leave scope:
    //  II.A.  Update 'nameToEntryMap_':
    //  II.A.1.  Each iteration removes a VarEntry instance at scope depth
    //		 'depth_' from 'nameToEntryMap_':
    VarEntry*	run;

    for  (run  = scopeStack_[depth_];
     run != NULL;
     run  = run->getNextOnSameLevelPtr()
     )
      if  (run->getPrevOfSameNamePtr() == NULL)
      {
	//  If there is no prior variable at a lower depth with the same name
	//  then erase name from 'nameToEntryMap_':
        nameToEntryMap_.erase(run->getName());
      }
      else
      {
	//  If there is a prior variable at a lower depth with the same name
	//  then replace the name in 'nameToEntryMap_' with the prior variable:
       nameToEntryMap_[run->getName()]	= run->getPrevOfSameNamePtr();
     }

    //  II.B.  Update 'scopeStack_[depth_]':
     scopeStack_[depth_]	= NULL;

    //  II.C.  Update 'depth_':
     depth_--;

    //  III.  Finished:
   }

  //  PURPOSE:  To note that a variable with name 'name' has newly been
  //	declared at the current scope depth.  The new VarEntry instance
  //	will be put at the head of 'varList'.  Throws ptr to 'std::string'
  //	instance that describes an error if an error is detected.
   void		declare		(const std::string&	name,
     VarEntry*&		varList
     )
   throw(std::string*)
   {
    //  I.  Application validity check:
    //  I.A.  Complain if an entry for 'name' already exists at the current
    //	      scope depth:
    std::map<std::string,VarEntry*>::iterator
    iter		= nameToEntryMap_.find(name);
    VarEntry*	prevSameNamePtr	= (iter == nameToEntryMap_.end())
    ? NULL
    : iter->second;

    if  ( (prevSameNamePtr		!= NULL	 ) &&
     (prevSameNamePtr->getDepth()	== depth_)
     )
      throw new std::string(std::string("Attempt to redeclare ") + name);

    //  II.  Declare variable:
    scopeStack_[depth_]		=  //  II.D.  Update 'scopeStack_' with
    nameToEntryMap_[name]	=  //  II.C.  Update 'nameToEntryMap_' with
    varList			=  //  II.B.  Update 'varList' with
				   //  II.A.  the new VarEntry instance
    new VarEntry
    (name,
      depth_,
      prevSameNamePtr,
      scopeStack_[depth_],
      varList
      );

    //  III.  Finished:
  }

  //  PURPOSE:  To retrieve the address of the VarEntry instance with the
  //	highest depth scope (innermost scope) with name 'name'.  Throws
  //	ptr to 'std::string' instance that describes an error if an error
  //	is detected.
  VarEntry*	retrieve	(const std::string&	name
    )
  throw(std::string*)
  {
    //  I.  Application validity check:
    //  I.A.  Ensure there is an existing entry for 'name':
    std::map<std::string,VarEntry*>::iterator
    iter		= nameToEntryMap_.find(name);

    if  (iter == nameToEntryMap_.end())
      throw new std::string(name + std::string(" is undeclared"));

    //  II.  Return value:
    return(iter->second);
  }

};