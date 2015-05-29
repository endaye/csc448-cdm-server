/*-------------------------------------------------------------------------*
 *---									---*
 *---		llParserMaker.cpp					---*
 *---									---*
 *---	    This file defines classes, methods and functions that	---*
 *---	implement the unenhanced llParser as defined by Fischer, Cytron	---*
 *---	and LeBlanc in Crafting a Compiler.  				---*
 *---									---*
 *---									---*
 *---	----	----	----	----	----	----	----	----	---*
 *---									---*
 *---	Version 1.0		2015 April 28		Joseph Phillips	---*
 *---									---*
 *-------------------------------------------------------------------------*/


//	----	----	----	----	----	----	----	----	//
//									//
//			Header file inclusions:				//
//									//
//	----	----	----	----	----	----	----	----	//

#include	<cstdlib>
#include	<cstdio>
#include	<cstring>
#include	<string>
#include	<iostream>
#include	<map>
#include	<vector>
#include	<set>


//	----	----	----	----	----	----	----	----	//
//									//
//			Definitions of constants:			//
//									//
//	----	----	----	----	----	----	----	----	//

//  PURPOSE:  To signify the end-of-input terminal.
#define		END_SYMBOL		"$"

//  PURPOSE:  To signify the integer constant terminal.
#define		INTEGER_CONST_SYMBOL	"#i"

//  PURPOSE:  To signify the floating point constant terminal.
#define		FLOAT_CONST_SYMBOL	"#f"

//  PURPOSE:  To represent the "beginning of a set" character.
const char	SET_BEGIN_CHAR		= '{';

//  PURPOSE:  To represent the character that separates two set members.
const char	SET_SEPARATOR_CHAR	= ',';

//  PURPOSE:  To represent the "end of a set" character.
const char	SET_END_CHAR		= '}';

//  PURPOSE:  To represent the arrow that separates the LHS of a production
//	from its RHS.
const char	PROD_ARROW_CHAR		= '-';

//  PURPOSE:  To tell how big 'text[]' is.
const int	TEXT_LEN		= 256;


//	----	----	----	----	----	----	----	----	//
//									//
//			    Definitions of types:			//
//									//
//	----	----	----	----	----	----	----	----	//

//  PURPOSE:  To define an abbreviation for "unsigned integer".
typedef		unsigned int		uInt;


//	----	----	----	----	----	----	----	----	//
//									//
//		    Definitions of global variables:			//
//									//
//	----	----	----	----	----	----	----	----	//

//  PURPOSE:  To hold C-string text needed for a short term basis, but
//	that must persist between function calls.
char  		text[TEXT_LEN];


//	----	----	----	----	----	----	----	----	//
//									//
//		Definitions of simple global functions:			//
//									//
//	----	----	----	----	----	----	----	----	//

//  PURPOSE:  To return 'true' if 'c' is a character used to define a Grammar,
//	or 'false' otherwise.
bool		isMetaChar	(char	c
                         )
throw()
{
    return( (c == SET_BEGIN_CHAR) || (c == SET_SEPARATOR_CHAR) ||
           (c == SET_END_CHAR) || (c == PROD_ARROW_CHAR)
           );
}


//	----	----	----	----	----	----	----	----	//
//									//
//    Definitions of classes that implement the components of Grammar:	//
//									//
//	----	----	----	----	----	----	----	----	//

class		NonTerminal;

class		Grammar;

//  PURPOSE:  To implement a set of 'NonTerminal*' pointers to which items
//	can be added, and that we would be guarannteed would be iterated
//	over.  This differs from the set<> template in that the set<>
//	template might add an item to a region of the set<> that already
//	has been iterated over, so an iterator might completely skip.
class		SafeSet
{
    //  I.  Member vars:
    //  PURPOSE:  To hold the items in '*this' set.
    std::vector<NonTerminal*>
    itemVect_;
    
    //  II.  Disallowed auto-generated methods:
    //  No copy constructor:
    SafeSet		(const SafeSet&
                 );
    
    //  No copy assignment op:
    SafeSet&		operator=
    (const SafeSet&
  			);
    protected :
    //  III.  Protected methods:
    
    public :
    //  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
    //  PURPOSE:  To initialize '*this' as an empty set.  No parameters.  No
    //	return value.
    SafeSet		()
    { }
    
    //  PURPOSE:  To release resources.  No parameters.  No return value.
    ~SafeSet		()
    { }
    
    //  V.  Accessors:
    //  PURPOSE:  To return the number of items in '*this'.  No parameters.
    uInt		getSize	()
    const
    throw()
    { return(itemVect_.size()); }
    
    //  PURPOSE:  To return the 'i'-th item.
    NonTerminal*	get	(uInt	i
                         )
    const
    throw()
    { return(itemVect_[i]); }
    
    //  VI.  Mutators:
    
    //  VII.  Methods that do main and misc work of class:
    //  PURPOSE:  To insert 'item' in '*this' only if it is not already present.
    //	No return value.
    void		insert	(NonTerminal*	item
                         )
    {
        //  I.  Application validity check:
        
        //  II.  Consider inserting 'item':
        //	II.A.  See if 'item' is already present:
        for  (int i = 0;  i < getSize();  i++)
            if  (itemVect_[i] == item)
                return;
        
        //  II.B.  If get here the 'item' is not present: insert it:
        itemVect_.push_back(item);
        
        //  III.  Finished:
    }
    
};


//  PURPOSE:  To represent a Symbol: both Terminal instances and NonTerminal
//	instances.  Serves as the abstract base class of both Terminal and
//	NonTerminal.
class		Symbol
{
    //  I.  Member vars:
    //  PURPOSE:  To hold the global count of the next Symbol identity integer.
    static
    int			nextId_;
    
    //  PURPOSE:  To hold the identity integer of '*this' Symbol.
    int			id_;
    
    //  PURPOSE:  To hold the name text of '*this' Symbol.
    std::string		name_;
    
    //  II.  Disallowed auto-generated methods:
    //  No default constructor:
    Symbol	 	();
    
    //  No copy constructor:
    Symbol 		(const Symbol&
                 );
    
    //  No copy assignment op:
    Symbol&	 	operator=
    (const Symbol&
     );
    
    protected :
    //  III.  Protected methods:
    
    public :
    //  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
    //  PURPOSE:  To create a new symbol with name 'newName'.  No return value.
    Symbol 		(const std::string&	newName
                 )
    throw() :
    id_(nextId_++),
    name_(newName)
    { }
    
    //  PURPOSE:  To release resources.  No parameters.  No return value.
    virtual
    ~Symbol 		()
    throw();
    
    //  V.  Accessors:
    //  PURPOSE:  To return the 'id' integer of '*this'.  No parameters.
    int		getId	()
    const
    throw()
    { return(id_); }
    
    //  PURPOSE:  To return the 'name' of '*this'.  No parameters.
    const std::string&
    getName	()
    const
    throw()
    { return(name_); }
    
    //  PURPOSE:  To return 'true' if '*this' can derive the empty string, or
    //	'false' otherwise.
    virtual
    bool		canDeriveEmpty
    ()
    const
    throw()
    = 0;
    
    //  VI.  Mutators:
    
    //  VII.  Methods that do main and misc work of this class.
    //  PURPOSE:  To return 'true' if '*this' is a terminal symbol, or 'false'
    //	otherwise.
    virtual
    bool		getIsTerminal
    ()
    const
    throw()
    { return(false); }
    
    //  PURPOSE:  To return 'true' if '*this' is a non-terminal symbol, or 'false'
    //	otherwise.
    virtual
    bool		getIsNonTerminal
    ()
    const
    throw()
    { return(false); }
    
    //  PURPOSE:  To return a 'std::string' representation of '*this'.  Prints
    //	'canDeriveEmpty()' value also if 'shouldComposeCanDerive' is 'true'.
    std::string	toString(bool	shouldComposeCanDerive	= false
                         )
    const
    throw()
    {
        //  I.  Application validity check:
        
        //  II.  Compose string:
        std::string	toReturn	= getName();
        
        if  (shouldComposeCanDerive)
        {
            if  (toReturn.length() >= 8)
                toReturn += "\t";
            else
                toReturn += "\t\t";
            
            if  (canDeriveEmpty())
                toReturn += "true";
            else
                toReturn += "false";
            
        }
        
        //  III.  Finished:
        return(toReturn);
    }
    
};


//  PURPOSE:  To hold the global count of the next Symbol identity integer.
int		Symbol::nextId_		= 0;


//  PURPOSE:  To release resources.  No parameter.  No return value.
//	NOTE: Does nothing.  Exists as a properly defined method to satisfy
//	the linker.
Symbol::~Symbol	() throw()		{ }


//  PURPOSE:  To represent Terminal symbols.
class		Terminal : public Symbol
{
    //  I.  Member vars:
    
    //  II.  Disallowed auto-generated methods:
    //  No default constructor:
    Terminal	 	();
    
    //  No copy constructor:
    Terminal 		(const Terminal&
                     );
    
    //  No copy assignment op:
    Terminal&	 	operator=
    (const Terminal&
     );
    
    protected :
    //  III.  Protected methods:
    
    public :
    //  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
    //  PURPOSE:  To create a new symbol with name 'newName'.  No return value.
    Terminal 		(const std::string&	newName
                     )
    throw() :
    Symbol(newName)
    { }
    
    //  V.  Accessors:
    
    //  VI.  Mutators:
    
    //  VII.  Methods that do main and misc work of this class.
    //  PURPOSE:  To return 'true' if '*this' is a terminal symbol, or 'false'
    //	otherwise.
    bool		getIsTerminal
    ()
    const
    throw()
    { return(true); }
    
    //  PURPOSE:  To return 'true' if '*this' can derive the empty string, or
    //	'false' otherwise.
    bool		canDeriveEmpty
    ()
    const
    throw()
    { return(false); }
    
};


//  PURPOSE:  To represent NonTerminal symbols.
class		NonTerminal : public Symbol
{
    //  I.  Member vars:
    //  PURPOSE:  To be 'true' if it has been shown that '*this' derives empty
    //	in its grammar, or 'false' otherwise.
    bool			canDeriveEmpty_;
    
    //  II.  Disallowed auto-generated methods:
    //  No default constructor:
    NonTerminal	 	();
    
    //  No copy constructor:
    NonTerminal 		(const NonTerminal&
                         );
    
    //  No copy assignment op:
    NonTerminal&	 	operator=
    (const NonTerminal&
     );
    
    protected :
    //  III.  Protected methods:
    
    public :
    //  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
    //  PURPOSE:  To create a new symbol with name 'newName'.  No return value.
    NonTerminal 		(const std::string&	newName
                         )
    throw() :
    Symbol(newName),
    canDeriveEmpty_(false)
    { }
    
    //  V.  Accessors:
    //  PURPOSE:  To return 'true' if '*this' can derive the empty string, or
    //	'false' otherwise.
    bool		canDeriveEmpty
    ()
    const
    throw()
    { return(canDeriveEmpty_); }
    
    //  VI.  Mutators:
    //  PURPOSE:  To note that '*this' NonTerminal can derive the empty string.
    void		setCanDeriveEmpty
    ()
    throw()
    { canDeriveEmpty_ = true; }
    
    //  VII.  Methods that do main and misc work of this class.
    //  PURPOSE:  To return 'true' if '*this' is a non-terminal symbol, or
    //	'false' otherwise.
    bool		getIsNonTerminal
    ()
    const
    throw()
    { return(true); }
    
};


//  PURPOSE:  To keep track of the defined Terminal and NonTerminal symbols
//	of a Grammar.
class		SymbolTable
{
    //  I.  Member vars:
    //  PURPOSE:  To map from the names of Terminals to pointers to them.
    std::map<std::string,Terminal*>
    nameToTerminalPtrMap_;
    
    //  PURPOSE:  To map from the names of NonTerminals to pointers to them.
    std::map<std::string,NonTerminal*>
    nameToNonTerminalPtrMap_;
    
    //  II.  Disallowed auto-created methods:
    //  No copy constructor:
    SymbolTable		(const SymbolTable&
                     );
    
    //  No copy assignment op:
    SymbolTable&		operator=
    (const SymbolTable&
     );
    
    protected :
    //  III.  Protected methods:
    
    public :
    //  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
    //  PURPOSE:  To initialize '*this' store.  No parameters.  No return value.
    SymbolTable		()
    throw(const char*)
    {
        registerTerminal(END_SYMBOL);
        registerTerminal(INTEGER_CONST_SYMBOL);
        registerTerminal(FLOAT_CONST_SYMBOL);
    }
    
    //  PURPOSE;  To release resources.  No parameters.  No return value.
    ~SymbolTable		()
    throw()
    {
        //  I.  Application validity check:
        
        //  II.  Release resources:
        //  II.A.  Release NonTerminal instances:
        for  (std::map<std::string,NonTerminal*>::iterator
              iter  = nameToNonTerminalPtrMap_.begin();
              iter != nameToNonTerminalPtrMap_.end();
              iter++
              )
        {
            delete(iter->second);
            iter->second	= NULL;
        }
        
        //  II.B.  Release Terminal instances:
        for  (std::map<std::string,Terminal*>::iterator
              iter  = nameToTerminalPtrMap_.begin();
              iter != nameToTerminalPtrMap_.end();
              iter++
              )
        {
            delete(iter->second);
            iter->second	= NULL;
        }
        
        //  III.  Finished:
    }
    
    //  V.  Accessors:
    //  PURPOSE:  To return the number of Terminal instances in '*this' store.
    //	No parameters.
    uInt		getNumTerminals
    ()
    const
    throw()
    { return(nameToTerminalPtrMap_.size()); }
    
    //  PURPOSE:  To return the number of NonTerminal instances in '*this' store.
    //	No parameters.
    uInt		getNumNonTerminals
    ()
    const
    throw()
    { return(nameToNonTerminalPtrMap_.size()); }
    
    //  PURPOSE:  To return the number of symbols in '*this' store.  No
    //	parameters.
    uInt		getNumSymbols
    ()
    const
    throw()
    { return(   nameToTerminalPtrMap_.size()  +
             nameToNonTerminalPtrMap_.size()
             );
    }
    
    //  PURPOSE:  To return a pointer to the Symbol with identity integer
    //	'id', or 'NULL' if no such Symbol has been found.
    Symbol*	getSymbol
    (int	i
     )
    throw()
    {
        //  I.  Application validity check:
        
        //  II.  Return value:
        for  (std::map<std::string,Terminal*>::iterator
              iter  = nameToTerminalPtrMap_.begin();
              iter != nameToTerminalPtrMap_.end();
              iter++
              )
        {
            if  (iter->second->getId() == i)
                return(iter->second);
        }
        
        for  (std::map<std::string,NonTerminal*>::iterator
              iter  = nameToNonTerminalPtrMap_.begin();
              iter != nameToNonTerminalPtrMap_.end();
              iter++
              )
        {
            if  (iter->second->getId() == i)
                return(iter->second);
        }
        
        //  III.  Finished:
        return(NULL);
    }
    
    //  VI.  Mutators:
    
    //  VII.  Methods that do main and misc work of this class.
    //  PURPOSE:  To return a pointer to a Terminal instance named 'name' (if
    //	one exists) or to return NULL otherwise.
    const Terminal*
    findTerminal
    (const std::string&	name
     )
    throw()
    {
        //  I.  Application validity check:
        
        //  II.  Look for 'name':
        std::map<std::string,Terminal*>::iterator
        termIter	= nameToTerminalPtrMap_.find(name);
        
        if  (termIter != nameToTerminalPtrMap_.end())
            return(termIter->second);
        
        //  III.  Finished:
        return(NULL);
    }
    
    //  PURPOSE:  To return a pointer to a Symbol instance named 'name' (if
    //	one exists) or to return NULL otherwise.
    Symbol*	find	(const std::string&	name
                     )
    throw()
    {
        //  I.  Application validity check:
        
        //  II.  Look for 'name':
        //  II.A.  Look among terminals:
        std::map<std::string,Terminal*>::iterator
        termIter	= nameToTerminalPtrMap_.find(name);
        
        if  (termIter != nameToTerminalPtrMap_.end())
            return(termIter->second);
        
        //  II.B.  Look among non-terminals:
        std::map<std::string,NonTerminal*>::iterator
        nonTermIter	= nameToNonTerminalPtrMap_.find(name);
        
        if  (nonTermIter != nameToNonTerminalPtrMap_.end())
            return(nonTermIter->second);
        
        //  III.  Finished:
        return(NULL);
    }
    
    //  PURPOSE:  To register a new terminal named 'name'.  No return value.
    //	Throws exception if 'name' already in use.
    void		registerTerminal
    (const std::string&	name
     )
    throw(const char*)
    {
        //  I.  Application validity check:
        if  (find(name) != NULL)
        {
            snprintf(text,TEXT_LEN,"Attempted redefinition of symbol %s",
                     name.c_str()
                     );
            throw text;
        }
        
        //  II.  Create and register Terminal:
        nameToTerminalPtrMap_[name]  = new Terminal(name);
        
        //  III.  Finished:
    }
    
    //  PURPOSE:  To register a new non-terminal named 'name'.  No return value.
    //	Throws exception if 'name' already in use.
    void		registerNonTerminal
    (const std::string&	name
     )
    throw(const char*)
    {
        //  I.  Application validity check:
        if  (find(name) != NULL)
        {
            snprintf(text,TEXT_LEN,"Attempted redefinition of symbol %s",
                     name.c_str()
                     );
            throw text;
        }
        
        //  II.  Create and register Terminal:
        nameToNonTerminalPtrMap_[name]	= new NonTerminal(name);
        
        //  III.  Finished:
    }
    
    
    //  PURPOSE:  To return a pointer to a newly-created NonTerminal instance.
    //	No parameters.
    NonTerminal*	newNonTerminal
    ()
    throw()
    {
        //  I.  Application validity check:
        
        //  II.  Create new NonTerminal instance:
        //  II.A.  Each iteration tries another name to see if it is available:
        static
        int		nonTermIndex		= 0;
        char	nonTermName[TEXT_LEN];
        
        do
        {
            snprintf(nonTermName,TEXT_LEN,"NT%d",nonTermIndex);
            nonTermIndex++;
        }
        while  (find(nonTermName) != NULL);
        
        //  II.B.  Create and register new 'NonTerminal' instance:
        std::string		name(nonTermName);
        NonTerminal*	nonTermPtr	= new NonTerminal(name);
        
        nameToNonTerminalPtrMap_[name]	= nonTermPtr;
        
        //  III.  Finished:
        return(nonTermPtr);
    }
    
};


//  PURPOSE:  To represent a production in a Grammar.
class		Production
{
    //  I.  Member vars:
    //  PURPOSE:  To hold the global count of the next Production identity
    //	integer.
    static
    int			nextId_;
    
    //  PURPOSE:  To hold the identity integer of '*this' Production.
    int			id_;
    
    //  PURPOSE:  To point to the NonTerminal on the left-handside of '*this'
    //	production.
    NonTerminal*		lhsPtr_;
    
    //  PURPOSE:  To hold pointers to Symbols on the right-handside of '*this'
    //	production.
    std::vector<Symbol*>	rhs_;
    
    //  PURPOSE:  To be 'true' if it has been shown that '*this' derives empty in
    //	its grammar, or 'false' otherwise.
    bool			canDeriveEmpty_;
    
    //  PURPOSE:  To hold the number of RHS symbols that cannot derive empty.
    int			nonEmptySymbolCount_;
    
    //  II.  Disallowed auto-generated methods:
    //  No default constructor:
    Production		();
    
    //  No copy constructor:
    Production		(const Production&
                     );
    
    //  No copy assignment:
    Production&		operator=
    (const Production&
     );
    
    protected :
    //  III.  Protected methods:
    
    public :
    //  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
    //  PURPOSE:  To initialize '*this' Production to have left-handside
    //	NonTerminal 'newLhsPtr' and right-handside Symbol instances as given
    //	in 'newRhs'.  No return value.
    Production		(NonTerminal*			newLhsPtr,
                     const std::vector<Symbol*>&	newRhs
                     )
    throw() :
    id_(nextId_++),
    lhsPtr_(newLhsPtr),
    canDeriveEmpty_(false),
    nonEmptySymbolCount_(0)
    {
        for (int i = 0;  i < newRhs.size();  i++)
        {
            rhs_.push_back(newRhs[i]);
        }
    }
    
    //  PURPOSE:  To release resources.  No parameters.  No return value.
    ~Production		()
    throw()
    { }
    
    //  V.  Accessors:
    //  PURPOSE:  To return the identity number of '*this' Production.  No
    //	parameters.
    int		getId	()
    const
    throw()
    { return(id_); }
    
    //  PURPOSE:  To return a pointer to the NonTerminal on the left-handside.
    NonTerminal*	getLhsPtr
    ()
    const
    throw()
    { return(lhsPtr_); }
    
    //  PURPOSE:  To return a pointer to the NonTerminal on the left-handside.
    const std::vector<Symbol*>&
    getRhs	()
    const
    throw()
    { return(rhs_); }
    
    //  PURPOSE:  To return the length of the right-handside.
    uInt		getRhsLength
    ()
    const
    throw()
    { return(rhs_.size()); }
    
    //  PURPOSE:  To return a pointer to the 'i'-th Symbol on the right-handside.
    Symbol*	getRhsSymbol
    (uInt	i
     )
    const
    throw()
    { return(rhs_[i]); }
    
    //  PURPOSE:  To return 'true' if '*this' can derive the empty string, or
    //	'false' otherwise.
    bool		canDeriveEmpty
    ()
    const
    throw()
    { return(canDeriveEmpty_); }
    
    //  VI.  Mutators:
    //  PURPOSE:  To add 'symbolPtr' at the end of 'rhs_'.  No return value.
    void		addSymbolAtEnd
    (Symbol*	symbolPtr
     )
    throw()
    { rhs_.push_back(symbolPtr); }
    
    //  PURPOSE:  To compute an upperbound for 'nonEmptySymbolCount_'.  No
    //	parameters.  No return value.
    void		initNonEmptySymbolCount
    ()
    throw()
    { nonEmptySymbolCount_ = getRhsLength(); }
    
    //  PURPOSE:  To decrement 'nonEmptySymbolCount_' by one.  No parameters.
    //	No return value.
    void		decNonEmptySymbolCount
    ()
    throw()
    { nonEmptySymbolCount_--; }
    
    
    //  VII.  Methods that do main and misc work of this class:
    //  PURPOSE:  To create and return a std::set<> of Terminal instances that
    //	may begin expansions of '*this' Production in Grammar 'grammar'.
    std::set<Terminal*>
    createPredictedTerminalSet
    (Grammar&	grammar)
    const
    throw();
    
    //  PURPOSE:  To see if '*this' can now derive the empty string.  If so,
    //	adds the lhs NonTerminal to 'toDoSet'.  No return value.
    void		checkForEmpty
    (SafeSet&		toDoSet
     )
    throw()
    {
        //  I.  Application validity check:
        
        //  II.  Do check:
        if  (nonEmptySymbolCount_ == 0)
        {
            canDeriveEmpty_	= true;
            
            if  ( !lhsPtr_->canDeriveEmpty() )
            {
                lhsPtr_->setCanDeriveEmpty();
                toDoSet.insert(lhsPtr_);
            }
            
        }
        
        //  III.  Finished:
    }
    
    //  PURPOSE:  To return 'true' of '*this' Production instance has a
    //	right-hand-side that begins with the symbols in 'symbolList', or
    //	to return 'false' otherwise.
    bool		doesRHSBeginWith
    (const std::vector<Symbol*>&	symbolList
     )
    const
    throw()
    {
        //  I.  Application validity check:
        
        //  II.  Do comparison:
        //  II.A.  Check lengths:
        if  (symbolList.size() > getRhsLength())
            return(false);
        
        //  II.B.  Compare Symbol instances:
        for  (uInt i = 0;  i < symbolList.size();  i++)
            if  (symbolList[i] != getRhsSymbol(i))
                return(false);
        
        //  III.  Finished:
        return(true);
    }
    
    //  PURPOSE:  To return a 'std::vector<>' containing pointers to the Symbol
    //	instances that appear in the right-hand-side at or after position
    //	'index'.
    std::vector<Symbol*>
    symbolsAfter
    (uInt	index
     )
    const
    throw(const char*)
    {
        //  I.  Application validity check:
        if  (index > getRhsLength())
            throw "index too big in Production::symbolsAfter()";
        
        //  II.  Gather pointers:
        std::vector<Symbol*>	toReturn;
        
        for  (uInt i = index;  i < getRhsLength();  i++)
            toReturn.push_back(getRhsSymbol(i));
            
            //  III.  Finished:
            return(toReturn);
    }
    
    //  PURPOSE:  To return a 'std::string' representation of '*this'.  No
    //	parameters.
    std::string	toString(bool	shouldComposeCanDerive	= false
                         )
    const
    throw()
    {
        //  I.  Application validity check:
        
        //  II.  Compose string:
        std::string	toReturn	 = getLhsPtr()->getName();
        
        toReturn			+= " ";
        toReturn			+= PROD_ARROW_CHAR;
        toReturn			+= " ";
        
        for  (uInt i = 0;  i < rhs_.size();  i++)
        {
            toReturn			+= rhs_[i]->getName();
            toReturn			+= " ";
        }
        
        if  (shouldComposeCanDerive)
        {
            if  (toReturn.length() >= 40)
                toReturn += " ";
            else
                if  (toReturn.length() >= 32)
                    toReturn += "\t";
                else
                    if  (toReturn.length() >= 24)
                        toReturn += "\t\t";
                    else
                        if  (toReturn.length() >= 16)
                            toReturn += "\t\t\t";
                        else
                            if  (toReturn.length() >= 8)
                                toReturn += "\t\t\t\t";
                            else
                                toReturn += "\t\t\t\t\t";
            
            if  (canDeriveEmpty())
                toReturn += "true";
            else
                toReturn += "false";
        }
        
        //  III.  Finished:
        return(toReturn);
    }
    
};

//  PURPOSE:  To hold the global count of the next Production identity
//	integer.
int		Production::nextId_	= 0;


//  PURPOSE:  To return 'true' if 'lhs' is less than 'rhs', or 'false'
//	otherwise.
bool		operator<	(const std::vector<Symbol*>&	lhs,
                         const std::vector<Symbol*>&	rhs
                         )
{
    //  I.  Application validity check:
    
    //  II.  Do comparison:
    size_t	limit	= lhs.size();
    
    if  (limit > rhs.size())
        limit = rhs.size();
    
    for  (uInt i = 0;  i < limit;   i++)
    {
        if  (lhs[i]->getId() > rhs[i]->getId())
            return(false);
        
        if  (lhs[i]->getId() < rhs[i]->getId())
            return(true);
    }
    
    //  III.  Matched up to this point:
    return( lhs.size() < rhs.size() );
}


//	----	----	----	----	----	----	----	----	//
//									//
//	Definitions of classes that help the Grammar tokenize input:	//
//									//
//	----	----	----	----	----	----	----	----	//

//  PURPOSE:  To implement an interface that manages the character source.
class		InputCharStream
{
    //  I.  Member vars:
    //  PURPOSE:  To hold the input.
    std::string		input_;
    
    //  PURPOSE:  To hold where the cursor is.
    int			index_;
    
    //  II.  Disallowed auto-generated methods:
    //  No default constructor.
    InputCharStream	();
    
    //  No copy constructor.
    InputCharStream	(const InputCharStream&
                     );
    
    //  No copy assignment operator.
    InputCharStream&	operator=
    (const InputCharStream&
     );
    
    protected :
    //  III.  Protected methods:
    
public:
    //  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
    //  PURPOSE:  To initialize '*this' to read the characters of 'newInput'.
    InputCharStream	(const std::string&	newInput
                     ) :
    input_(newInput),
    index_(0)
    { }
    
    //  PURPOSE: To release resources.  No parameters.  No return value.
    ~InputCharStream	()
    throw()
    { }
    
    //  V.  Accessors:
    
    //  VI.  Mutators:
    
    //  VII.  Methods that do main and misc work of class:
    //  PURPOSE:  To return the current char, or '\0' if there are no more.
    //	No parameters.
    char		peek	()
    const
    throw()
    { return
        ( (index_ >= input_.length())
         ? '\0' : input_[index_]
         );
    }
    
    //  PURPOSE:  To return 'true' if at eof-of-input, or 'false' otherwise.
    bool		isAtEnd	()
    const
    throw()
    { return(index_ >= input_.length()); }
    
    //  PURPOSE:  To advance to the next char (if not already at end).  No
    //	parameters.  No return value.
    void		advance	()
    throw()
    {
        if  (index_ < input_.length())  index_++;
    }
    
};


//  PURPOSE:  To implement an interface that gathers characters into lexemes.
class		TokenStream
{
    //  I.  Member vars:
    //  PURPOSE:  To refer to the object that knows which Terminals have been
    //	defined.
    SymbolTable&		symbolTable_;
    
    //  PURPOSE:  To hold the source of the character input.
    InputCharStream&	inputCharStream_;
    
    //  PURPOSE:  To hold the lastest lexeme parsed.
    const Terminal*   	lastParsedPtr_;
    
    //  PURPOSE:  To hold the pointer to the Terminal that represents the
    //  	end-of-input.
    const Terminal*	endOfInputTerminalPtr_;
    
    //  PURPOSE:  To hold the pointer to the Terminal that represents an
    //  	integer constant.
    const Terminal*	integerConstTerminalPtr_;
    
    //  PURPOSE:  To hold the pointer to the Terminal that represents a floating
    //  	point constant.
    const Terminal*	floatPtConstTerminalPtr_;
    
    //  II.  Disallowed auto-generated methods:
    //  No default constructor:
    TokenStream		();
    
    //  No copy constructor:
    TokenStream		(const TokenStream&
                     );
    
    //  No copy assignment op:
    TokenStream&		operator=
    (const TokenStream&
     );
    
    protected :
    //  III.  Protected methods:
    //  PURPOSE:  To return a pointer representing a scanned number, either
    //	an integer or floating point.  No parameters.
    const Terminal*
    scanDigits
    ()
    throw()
    {
        //  I.  Application validity check:
        
        //  II.  To read a number from 'inputCharStream_':
        //  II.A.  Initialize object into which to put characters of number:
        std::string	lex("");
        
        //  II.B.  Gather digits:
        while  ( isdigit(inputCharStream_.peek()) )
        {
            lex += inputCharStream_.peek();
            inputCharStream_.advance();
        }
        
        //  II.C.  Distinguish between integers and floating point constants:
        const Terminal*	symbolPtr;
        
        if  (inputCharStream_.peek() != '.')
        {
            //  II.C.1.  If don't have decimal point the have an integer const,
            //  	   Stop reading:
            symbolPtr	= integerConstTerminalPtr_;
        }
        else
        {
            //  II.C.2.  If do have decimal point the have a floating point const.
            //  	   Continue reading optional digits after decimal point:
            lex += inputCharStream_.peek();
            inputCharStream_.advance();
            
            while  ( isdigit(inputCharStream_.peek()) )
            {
                lex += inputCharStream_.peek();
                inputCharStream_.advance();
            }
            
            symbolPtr	= floatPtConstTerminalPtr_;
        }
        
        //  III.  Finished:
        return(symbolPtr);
    }
    
    
    //  PURPOSE:  To return a pointer representing a scanned Symbol, or to return
    //	'endOfInputTerminalPtr_' if the '*this' is at the end-of-input.  No
    //	parameters.
    const Terminal*	scanner	()
    throw(const char*)
    {
        //  I.  Application validity check:
        
        //  II.  Attempt to get next terminal:
        //	II.A.  Fast-forward past spaces:
        while  ( isspace(inputCharStream_.peek()) )
            inputCharStream_.advance();
            
            //  II.B.  Return 'endOfInputTerminalPtr_' if at end of input:
            if  ( inputCharStream_.isAtEnd() )
                return(endOfInputTerminalPtr_);
        
        //  II.C.  If see a digit then have a number.  Handle with more
        //		specialized 'scanDigits()':
        if  ( isdigit(inputCharStream_.peek()) )
            return( scanDigits() );
        
        //  II.D.  Try to interpret as an ordinary Terminal:
        //  II.D.1.  Gather chars into 'lexeme':
        std::string	lexeme;
        
        lexeme += inputCharStream_.peek();
        
        for  ( inputCharStream_.advance();
              !inputCharStream_.isAtEnd() && !isspace(inputCharStream_.peek());
              inputCharStream_.advance()
              )
            lexeme += inputCharStream_.peek();
            
            //	II.D.2.  Look up 'lexeme' in symbol table:
            const Terminal*	symPtr	= symbolTable_.findTerminal(lexeme);
            
            //  II.D.3.  Complain if not found:
            if  (symPtr == NULL)
            {
                snprintf(text,TEXT_LEN,"Undeclared symbol %s in input.",lexeme.c_str());
                throw text;
            }
        
        //  III.  Finished:
        return(symPtr);
    }
    
    public :
    //  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
    //  PURPOSE:  To initialize '*this' to read characters from
    //	'newInputCharStream' and turn them into Symbol instances as defined by
    //	'newSymbolTable'.  No return value.
    TokenStream		(SymbolTable&		newSymbolTable,
                     InputCharStream&	newInputCharStream
                     )
    throw(const char*) :
    symbolTable_(newSymbolTable),
    inputCharStream_(newInputCharStream),
    lastParsedPtr_(NULL),
    endOfInputTerminalPtr_
				(symbolTable_.findTerminal(END_SYMBOL)),
    integerConstTerminalPtr_
				(symbolTable_.findTerminal
                 (INTEGER_CONST_SYMBOL)
                 ),
    floatPtConstTerminalPtr_
				(symbolTable_.findTerminal
                 (FLOAT_CONST_SYMBOL)
                 )
    {
        advance();
    }
    
    //  PURPOSE:  To release resources.  No parameters.  No return value.
    ~TokenStream		()
    throw()
    { }
    
    //  V.  Accessors:
    
    //  VI.  Mutators:
    
    //  VII.  Methods that do main and misc work of class:
    //  PURPOSE:  To return the 'symbol_t' of the 'Symbol' instance that is next
    //	in the symbol stream.  No parameters.
    const Terminal*
    peek	()
    throw(const char*)
    {
        if  (lastParsedPtr_ == NULL)
            lastParsedPtr_	= scanner();
            
            return(lastParsedPtr_);
    }
    
    //  PURPOSE:  To return the pointer to the old Symbol at that was at the
    //	front of the symbol stream, and then to internally advance to the next
    //	Symbol instance (if not already at the end).  No parameters.
    const Terminal*
    advance	()
    throw(const char*)
    {
        const Terminal* toReturn	= lastParsedPtr_;
        
        lastParsedPtr_		= scanner();
        return(toReturn);
    }
    
};


//	----	----	----	----	----	----	----	----	//
//									//
//    Definitions of class(es) and functions that do high-level work:	//
//									//
//	----	----	----	----	----	----	----	----	//

//  PURPOSE:  To represent a Grammar.
class		Grammar
{
    //  I.  Member vars:
    //  PURPOSE:  To hold the Symbols of '*this' Grammar.
    SymbolTable		symbolTable_;
    
    //  PURPOSE:  To point the starting non-terminal of '*this' Grammar.
    const NonTerminal*	startSymbolPtr_;
    
    //  PURPOSE:  To hold pointers to the productions in '*this' Grammar.
    std::vector<Production*>
    productionPtrVect_;
    
    //  PURPOSE:  To hold the 2-D table of (Symbol x Terminal) -> Production*.
    Production***		llTableMetaHandle_;
    
    //  II.  Disallowed auto-generated methods:
    //  No copy constructor:
    Grammar		(const Grammar&
                 );
    
    //  No copy assignment:
    Grammar&		operator=
    (const Grammar&
     );
    
    protected :
    //  III.  Protected methods:
    //  PURPOSE:  To parse the terminals from 'descriptionStr' starting at
    //	position 'index'.  Advances 'index' to just past the terminals.
    void		parseTerminals
    (int&			index,
     const std::string&	descriptionStr
     )
    throw(const char*)
    {
        //  I.  Application validity check:
        
        //  II.  Parse terminals:
        //	II.A.  Fast-forward past spaces to SET_BEGIN_CHAR:
        for  ( ;  index < descriptionStr.length();  index++ )
            if  ( !isspace(descriptionStr[index]) )
                break;
        
        if  (index >= descriptionStr.length())
        {
            snprintf(text,TEXT_LEN,"Expected %c at %d instead of end",
                     SET_BEGIN_CHAR,index
                     );
            throw text;
        }
        
        if  (descriptionStr[index] != SET_BEGIN_CHAR)
        {
            snprintf(text,TEXT_LEN,"Expected %c at %d instead of %c",
                     SET_BEGIN_CHAR,index,descriptionStr[index]
                     );
            throw text;
        }
        
        //	II.B.  Fast-forward past spaces to either SET_END_CHAR or terminal(s):
        for  (index++;  index < descriptionStr.length();  index++ )
            if  ( !isspace(descriptionStr[index]) )
                break;
        
        while  ( isspace(descriptionStr[index]) )
            index++;
        
        //  II.C.  Read terminals:
        if  (descriptionStr[index] != SET_END_CHAR)
        {
            char		c;
            
            do
            {
                std::string	lexeme;
                
                c		= descriptionStr[index];
                
                if  ( isMetaChar(c) )
                {
                    snprintf(text,TEXT_LEN,"%c is an illegal symbol at %d",c,index);
                    throw text;
                }
                
                lexeme	+= descriptionStr[index];
                
                for  (index++;  index < descriptionStr.length();  index++)
                {
                    c	= descriptionStr[index];
                    
                    if  ( isMetaChar(c) || isspace(c) )
                        break;
                    
                    lexeme	+= c;
                }
                
                symbolTable_.registerTerminal(lexeme);
                
                while  ( (index < descriptionStr.length()) && isspace(c) )
                {
                    index++;
                    c = descriptionStr[index];
                }
                
                if  (index >= descriptionStr.length())
                {
                    snprintf(text,TEXT_LEN,"Expected %c or %c at %d instead of end",
                             SET_SEPARATOR_CHAR,SET_END_CHAR,index
                             );
                    throw text;
                }
                else
                    if  (c == SET_SEPARATOR_CHAR)
                        index++;
                    else
                        if  (c != SET_END_CHAR)
                        {
                            snprintf(text,TEXT_LEN,"Expected %c or %c at %d instead of end",
                                     SET_SEPARATOR_CHAR,SET_END_CHAR,index
                                     );
                            throw text;
                        }
                
            }
            while  (c != SET_END_CHAR);
            
        }
        
        //  II.D.  Inc. 'index' past 'SET_END_CHAR':
        if  (index < descriptionStr.length())
            index++;
        
        //  III. Finished:
    }
    
    
    //  PURPOSE:  To parse the terminals from 'descriptionStr' starting at
    //	position 'index'.  Advances 'index' to just past the non-terminals.
    void		parseNonTerminals
    (int&			index,
     const std::string&	descriptionStr
     )
    throw(const char*)
    {
        //  I.  Application validity check:
        
        //  II.  Parse non-terminals:
        //	II.A.  Fast-forward past spaces to SET_SEPARATOR_CHAR:
        for  ( ;  index < descriptionStr.length();  index++ )
            if  ( !isspace(descriptionStr[index]) )
                break;
        
        if  (index >= descriptionStr.length())
        {
            snprintf(text,TEXT_LEN,"Expected %c at %d instead of end",
                     SET_SEPARATOR_CHAR,index
                     );
            throw text;
        }
        
        if  (descriptionStr[index] != SET_SEPARATOR_CHAR)
        {
            snprintf(text,TEXT_LEN,"Expected %c at %d instead of %c",
                     SET_SEPARATOR_CHAR,index,descriptionStr[index]
                     );
            throw text;
        }
        
        index++;
        
        //	II.B.  Fast-forward past spaces to SET_BEGIN_CHAR:
        for  ( ;  index < descriptionStr.length();  index++ )
            if  ( !isspace(descriptionStr[index]) )
                break;
        
        if  (index >= descriptionStr.length())
        {
            snprintf(text,TEXT_LEN,"Expected %c at %d instead of end",
                     SET_BEGIN_CHAR,index
                     );
            throw text;
        }
        
        if  (descriptionStr[index] != SET_BEGIN_CHAR)
        {
            snprintf(text,TEXT_LEN,"Expected %c at %d instead of %c",
                     SET_BEGIN_CHAR,index,descriptionStr[index]
                     );
            throw text;
        }
        
        //	II.C.  Fast-forward past spaces to either SET_END_CHAR
        //	       or non-terminal(s):
        for  (index++;  index < descriptionStr.length();  index++ )
            if  ( !isspace(descriptionStr[index]) )
                break;
        
        while  ( isspace(descriptionStr[index]) )
            index++;
        
        //  II.D.  Read non-terminals:
        if  (descriptionStr[index] != SET_END_CHAR)
        {
            char		c;
            
            do
            {
                std::string	lexeme;
                
                c		= descriptionStr[index];
                
                if  ( isMetaChar(c) )
                {
                    snprintf(text,TEXT_LEN,"%c is an illegal symbol at %d",c,index);
                    throw text;
                }
                
                lexeme	+= descriptionStr[index];
                
                for  (index++;  index < descriptionStr.length();  index++)
                {
                    c	= descriptionStr[index];
                    
                    if  ( isMetaChar(c) || isspace(c) )
                        break;
                    
                    lexeme	+= c;
                }
                
                symbolTable_.registerNonTerminal(lexeme);
                
                while  ( (index < descriptionStr.length()) && isspace(c) )
                {
                    index++;
                    c = descriptionStr[index];
                }
                
                if  (index >= descriptionStr.length())
                {
                    snprintf(text,TEXT_LEN,"Expected %c or %c at %d instead of end",
                             SET_SEPARATOR_CHAR,SET_END_CHAR,index
                             );
                    throw text;
                }
                else
                    if  (c == SET_SEPARATOR_CHAR)
                        index++;
                    else
                        if  (c != SET_END_CHAR)
                        {
                            snprintf(text,TEXT_LEN,"Expected %c or %c at %d instead of end",
                                     SET_SEPARATOR_CHAR,SET_END_CHAR,index
                                     );
                            throw text;
                        }
                
            }
            while  (c != SET_END_CHAR);
            
        }
        
        //  II.E.  Inc. 'index' past 'SET_END_CHAR':
        if  (index < descriptionStr.length())
            index++;
        
        //  III. Finished:
    }
    
    
    void		parseStartSymbol
    (int&			index,
     const std::string&	descriptionStr
     )
    throw(const char*)
    {
        //  I.  Application validity check:
        
        //  II.  Parse non-terminals:
        //	II.A.  Fast-forward past spaces to SET_SEPARATOR_CHAR:
        for  ( ;  index < descriptionStr.length();  index++ )
            if  ( !isspace(descriptionStr[index]) )
                break;
        
        if  (index >= descriptionStr.length())
        {
            snprintf(text,TEXT_LEN,"Expected %c at %d instead of end",
                     SET_SEPARATOR_CHAR,index
                     );
            throw text;
        }
        
        if  (descriptionStr[index] != SET_SEPARATOR_CHAR)
        {
            snprintf(text,TEXT_LEN,"Expected %c at %d instead of %c",
                     SET_SEPARATOR_CHAR,index,descriptionStr[index]
                     );
            throw text;
        }
        
        index++;
        
        //	II.B.  Fast-forward past spaces to start-symbol:
        for  ( ;  index < descriptionStr.length();  index++ )
            if  ( !isspace(descriptionStr[index]) )
                break;
        
        if  (index >= descriptionStr.length())
        {
            snprintf(text,TEXT_LEN,"Expected %c at %d instead of end",
                     SET_BEGIN_CHAR,index
                     );
            throw text;
        }
        
        std::string	lexeme;
        char	c;
        
        for  ( c = descriptionStr[index];
              (index < descriptionStr.length()) &&
              (c != SET_BEGIN_CHAR) && (c != SET_END_CHAR) &&
              (c != SET_SEPARATOR_CHAR);
              index++, c = descriptionStr[index]
              )
            lexeme += c;
        
        if  (lexeme.length() == 0)
        {
            snprintf(text,TEXT_LEN,
                     "Expected starting non-terminal at %d instead of %c",
                     index,c
                     );
            throw text;
        }
        
        Symbol*	symPtr	= symbolTable_.find(lexeme);
        
        if  (symPtr == NULL)
        {
            snprintf(text,TEXT_LEN,"%s is an unknown non-terminal",lexeme.c_str());
            throw text;
        }
        
        if  ( !symPtr->getIsNonTerminal() )
        {
            snprintf(text,TEXT_LEN,"%s is not a non-terminal",lexeme.c_str());
            throw text;
        }
        
        //  III.  Finished:
        startSymbolPtr_	= (NonTerminal*)symPtr;
    }
    
    
    void		parseProductions
    (int&			index,
     const std::string&	descriptionStr
     )
    throw(const char*)
    {
        //	II.A.  Fast-forward past spaces to SET_SEPARATOR_CHAR:
        for  ( ;  index < descriptionStr.length();  index++ )
            if  ( !isspace(descriptionStr[index]) )
                break;
        
        if  (index >= descriptionStr.length())
        {
            snprintf(text,TEXT_LEN,"Expected %c at %d instead of end",
                     SET_SEPARATOR_CHAR,index
                     );
            throw text;
        }
        
        if  (descriptionStr[index] != SET_SEPARATOR_CHAR)
        {
            snprintf(text,TEXT_LEN,"Expected %c at %d instead of %c",
                     SET_SEPARATOR_CHAR,index,descriptionStr[index]
                     );
            throw text;
        }
        
        index++;
        
        //	II.B.  Fast-forward past spaces to SET_BEGIN_CHAR:
        for  ( ;  index < descriptionStr.length();  index++ )
            if  ( !isspace(descriptionStr[index]) )
                break;
        
        if  (index >= descriptionStr.length())
        {
            snprintf(text,TEXT_LEN,"Expected %c at %d instead of end",
                     SET_BEGIN_CHAR,index
                     );
            throw text;
        }
        
        if  (descriptionStr[index] != SET_BEGIN_CHAR)
        {
            snprintf(text,TEXT_LEN,"Expected %c at %d instead of %c",
                     SET_BEGIN_CHAR,index,descriptionStr[index]
                     );
            throw text;
        }
        
        //	II.C.  Fast-forward past spaces to either SET_END_CHAR
        //	       or non-terminal(s):
        for  (index++;  index < descriptionStr.length();  index++ )
            if  ( !isspace(descriptionStr[index]) )
                break;
        
        while  ( isspace(descriptionStr[index]) )
            index++;
        
        //  II.D.  Read productions:
        if  (descriptionStr[index] != SET_END_CHAR)
        {
            char		c;
            
            do
            {
                Symbol*		symPtr;
                NonTerminal*	lhsPtr;
                std::string	lexeme;
                
                //  II.D.1.  Read rhs non-terminal:
                for  (c	= descriptionStr[index];
                      index < descriptionStr.length();
                      index++, c = descriptionStr[index]
                      )
                {
                    if  ( !isspace(c) )
                        break;
                }
                
                if  ( isMetaChar(c) )
                {
                    snprintf(text,TEXT_LEN,"%c is an illegal symbol at %d",c,index);
                    throw text;
                }
                
                lexeme	+= descriptionStr[index];
                
                for  (index++;  index < descriptionStr.length();  index++)
                {
                    c	= descriptionStr[index];
                    
                    if  ( isMetaChar(c) || isspace(c) )
                        break;
                    
                    lexeme	+= c;
                }
                
                symPtr = symbolTable_.find(lexeme);
                
                if  (symPtr == NULL)
                {
                    snprintf(text,TEXT_LEN,
                             "%s is an unknown non-terminal",lexeme.c_str()
                             );
                    throw text;
                }
                
                if  (!symPtr->getIsNonTerminal() )
                {
                    snprintf(text,TEXT_LEN,"%s is not a non-terminal",lexeme.c_str());
                    throw text;
                }
                
                lhsPtr	= (NonTerminal*)symPtr;
                
                //  II.D.2.  Read production arrow:
                while  ( (index < descriptionStr.length()) && isspace(c) )
                {
                    index++;
                    c = descriptionStr[index];
                }
                
                if  (index >= descriptionStr.length())
                {
                    snprintf(text,TEXT_LEN,"Expected %c at %d instead of end",
                             PROD_ARROW_CHAR,index
                             );
                    throw text;
                }
                else
                    if  (c != PROD_ARROW_CHAR)
                    {
                        snprintf(text,TEXT_LEN,"Expected %c at %d instead of %c",
                                 PROD_ARROW_CHAR,index,c
                                 );
                        throw text;
                    }
                
                index++;
                
                //  II.D.3.  Read rhs:
                std::vector<Symbol*>	rhsVect;
                
                while  (true)
                {
                    
                    for  (;  index < descriptionStr.length();  index++)
                    {
                        c = descriptionStr[index];
                        
                        if  (!isspace(c) )
                            break;
                    }
                    
                    if  ( (c == SET_END_CHAR) || (c == SET_SEPARATOR_CHAR) )
                        break;
                    
                    lexeme.clear();
                    lexeme += c;
                    
                    for  (index++;  index < descriptionStr.length();  index++)
                    {
                        c = descriptionStr[index];
                        
                        if  ( isspace(c) || isMetaChar(c) )
                            break;
                        
                        lexeme += c;
                    }
                    
                    symPtr	= symbolTable_.find(lexeme);
                    
                    if  (symPtr == NULL)
                    {
                        snprintf(text,TEXT_LEN,"Symbol %s undeclared",lexeme.c_str());
                        throw text;
                    }
                    
                    rhsVect.push_back(symPtr);
                }
                
                productionPtrVect_.push_back(new Production(lhsPtr,rhsVect));
                
                if  (c == SET_SEPARATOR_CHAR)
                    index++;
            }
            while  (c != SET_END_CHAR);
            
        }
        
        //  II.E.  Inc. 'index' past 'SET_END_CHAR':
        if  (index < descriptionStr.length())
            index++;
        
        //  III. Finished:
    }
    
    
    //  PURPOSE:  To add
    void		addEndTerms
    ()
    throw()
    {
        Symbol*	endSymPtr	= symbolTable_.find(END_SYMBOL);
        
        for  (uInt pi = 0;  pi < getNumProductions();  pi++)
        {
            Production*	prodPtr	= productionPtrVect_[pi];
            
            if  ( (prodPtr->getLhsPtr() == getStartSymbolPtr())
                 &&
                 ( (prodPtr->getRhsLength() == 0)	||
                  (prodPtr->getRhsSymbol(prodPtr->getRhsLength()-1) != endSymPtr)
                  )
                 )
                prodPtr->addSymbolAtEnd(endSymPtr);
                }
    }
    
    
    //  PURPOSE:  To compute and return the longest common prefix of
    //	right-hand-side symbols held in common by two or more Production
    //	instances for NonTerminal instance '*nonTermPtr'.  Prefix returned as
    //	std::vector<Symbol*>.  Returns empty vector if no such common vector
    //	exists.
    std::vector<Symbol*>
    longestCommonPrefix
    (const NonTerminal*	nonTermPtr
     )
    throw(const char*)
    {
        //  I.  Application validity check:
        if  (nonTermPtr == NULL)
            throw "NULL ptr to Grammar::longestCommonPrefix()";
        
        //  II.  Compute longest common prefix:
        //  II.A.  Gather those Productions for 'nonTermPtr' in 'localProdVect':
        std::vector<Production*>	localProdVect;
        
        for  (int i = 0;  i < productionPtrVect_.size();  i++)
            if  (productionPtrVect_[i]->getLhsPtr() == nonTermPtr)
                localProdVect.push_back(productionPtrVect_[i]);
                
                //  II.B.  Sort 'localProdVect':
                for  (int i = 0;  i < localProdVect.size()-1;  i++)
                {
                    int	smallestIndex	= i;
                    
                    for  (int j = i+1;  j < localProdVect.size();  j++)
                        if  (localProdVect[smallestIndex]->getRhs() >
                             localProdVect[j]->getRhs()
                             )
                            smallestIndex	= j;
                    
                    if  (smallestIndex != i)
                    {
                        Production*	pPtr		= localProdVect[smallestIndex];
                        
                        localProdVect[smallestIndex]	= localProdVect[i];
                        localProdVect[i]		= pPtr;
                    }
                    
                }
        
        //  II.C.  Look for common prefix:
        uInt	longestPrefixLen	= 0;
        uInt	longestPrefixInd	= 0;
        
        for  (int i = 0;  i < localProdVect.size()-1;  i++)
        {
            uInt	len;
            uInt	limit	= localProdVect[i]->getRhs().size();
            
            if  (limit > localProdVect[i+1]->getRhs().size())
                limit  = localProdVect[i+1]->getRhs().size();
                
                for  (len = 0;  len < limit;  len++)
                    if  (localProdVect[i+0]->getRhs()[len] !=
                         localProdVect[i+1]->getRhs()[len]
                         )
                        break;
            
            if  (len > longestPrefixLen)
            {
                longestPrefixInd	= i;
                longestPrefixLen	= len;
            }
            
        }
        
        //  III.  Finished:
        std::vector<Symbol*>	toReturn;
        
        for  (uInt i = 0;  i < longestPrefixLen;  i++)
        {
            toReturn.push_back(localProdVect[longestPrefixInd]->getRhs()[i]);
        }
        return(toReturn);
    }
    
    
    //  PURPOSE:  To transform '*this' Grammar by factoring common prefixes
    //	from Production instances and putting them in their own, new
    //	Production instance.  No parameters.  No return value.
    void		factorCommonPrefixes
    ()
    throw(const char*)
    {
        //  I.  Application validity check:
        
        //  II.  Check each NonTerminal instance:
        
        //  II.A.  Each iteration considers one Symbol:
        for  (uInt i = 0;  i < getNumSymbols();  i++)
        {
            //  II.A.1.  Get a pointer to the current Symbol:
            Symbol*	symbolPtr	= getSymbolPtr(i);
            
            //  II.A.2.  Ignore '*symbolPtr' if it is not a NonTerminal instance:
            if  ( !symbolPtr->getIsNonTerminal() )
                continue;
            
            NonTerminal*	nonTermPtr	= (NonTerminal*)symbolPtr;
            
            //  II.A.3.  :
            std::vector<Symbol*>	prefix;
            
            //  YOUR CODE HERE!
            //  The outer loop is their foreach A in NonTerminal loop.
            //  'nonTermPtr' points to the current 'NonTerminal' instance
            /*
             procedure FACTOR( ) 
                foreach A ∈ N do
                    α ← LongestCommonPrefix(ProductionsFor(A))
                    while |α| > 0 do
                        V ← new NonTerminal ( )
                        Productions ← Productions ∪ { A → αV }
                        foreach p ∈ ProductionsFor(A) | RHS(p) = αβp do
                            Productions ← Productions − { p }
                            Productions ← Productions ∪ { V → βp }
                        α ← LongestCommonPrefix(ProductionsFor(A))
             ￼end
             A: symbolPtr
             α: prefix
             */
            
            //α ← LongestCommonPrefix(ProductionsFor(A))
            prefix  =   longestCommonPrefix(nonTermPtr);
            
            //while |α| > 0 do
            while(prefix.size() > 0) {
                
                //V ← new NonTerminal ( )
                NonTerminal*    v   =   (NonTerminal*)newNonTerminal();
                
                //Productions ← Productions ∪ { A → αV }
                std::vector<Symbol*>	alphaV  =   prefix;
                alphaV.push_back(v);
                Production* prodAlphaV = new Production(nonTermPtr,    alphaV);
                addProduction(prodAlphaV);
                
                //foreach p ∈ ProductionsFor(A) | RHS(p) = αβp do
                Production* prodForA    =   getProductionPtr(i);
                std::vector<Production*>    toRemove;
                for (uInt pi = 0;    pi < getNumProductions();  pi++)   {
                    Production* prodPtr =   getProductionPtr(pi);
                    if  (prodPtr   ==  prodForA   ||
                         prodPtr->doesRHSBeginWith(prefix))
                    {
                        //Productions ← Productions − { p }
                        toRemove.push_back(prodPtr);
                        //Productions ← Productions ∪ { V → βp }
                        std::vector<Symbol*>    betaP    =   prodPtr->getRhs();
                        
                        if(prefix.size() > prodPtr->getRhs().size())
                        {
                            throw "error";
                        }
                        betaP.erase(betaP.begin(),betaP.begin()+prefix.size());
                    
                        Production* prodBetaP = new Production(v,    betaP);
                        addProduction(prodBetaP);
                    }
                    
                }
                
                for  (uInt pi = 0;  pi < toRemove.size();  pi++)
                {
                    removeProduction(toRemove[pi]);
                }
                
                //α ← LongestCommonPrefix(ProductionsFor(A))
                prefix = longestCommonPrefix(nonTermPtr);
                
             
            }
            
            
            //  III.  Finished:
        }
        
    }
    
    
    //  PURPOSE:  To compute the derives empty member var for NonTerminal
    //	instances and productions.
    void		computeDerivesEmpty
    ()
    throw()
    {
        //  I.  Application validity check:
        
        //  II.
        for  (int pi = 0;  pi < getNumProductions();  pi++)
            productionPtrVect_[pi]->initNonEmptySymbolCount();
            
            SafeSet		toDoSet;
        
        for  (int pi = 0;  pi < getNumProductions();  pi++)
            productionPtrVect_[pi]->checkForEmpty(toDoSet);
            
            for  (int si = 0;  si < toDoSet.getSize();  si++)
            {
                NonTerminal*	nonTermPtr	= toDoSet.get(si);
                int		i;
                
                for  (int pi = 0;  pi < getNumProductions();  pi++)
                {
                    Production*	prodPtr	= productionPtrVect_[pi];
                    
                    for  (i = 0;  i < prodPtr->getRhsLength();  i++)
                        if  (nonTermPtr == prodPtr->getRhsSymbol(i))
                            break;
                    
                    if  (i >= prodPtr->getRhsLength())
                        continue;
                    
                    prodPtr->decNonEmptySymbolCount();
                    prodPtr->checkForEmpty(toDoSet);
                }
            }
    }
    
    //  PURPOSE:  To construct the table 'llTableMetaHandle_'.  No parameters.
    //	No return value.
    void		buildTable
    ()
    throw(const char*)
    {
        //  I.  Application validity check:
        
        //  II.  Construct 'llTableMetaHandle_':
        //	II.A.  Allocate memory (calloc() automatically fills mem with NULLs):
        uInt	numTerminals	= symbolTable_.getNumTerminals();
        
        llTableMetaHandle_		= (Production***)
        calloc
        (symbolTable_.getNumSymbols(),
         sizeof(Production**)
         );
        
        for  (uInt si = 0;  si < symbolTable_.getNumSymbols();  si++)
            llTableMetaHandle_[si]	= (Production**)
            calloc
            (numTerminals,
             sizeof(Production*)
             );
            
            //  II.B.  Build table:
            //  II.B.1.  Each iteration iterates over a Symbol:
            for  (uInt si = 0;  si < symbolTable_.getNumSymbols();  si++)
            {
                //  II.B.1.a.  Get the current symbol:
                const Symbol*	symPtr	= symbolTable_.getSymbol(si);
                
                //  II.B.1.b.  Ignore Symbol instances that are Terminal instances:
                if  (symPtr->getIsTerminal())
                    continue;
                
                //  II.B.1.c.  If get here then '*symPtr' is a NonTerminal:
                //  II.B.1.c.I.  Each iteration sees if a production expands the
                //  	       NonTerminal '*symPtr':
                for  (uInt pInd = 0;  pInd < getNumProductions();  pInd++)
                {
                    //  II.B.1.c.I.A.  Get current production:
                    Production*	prodPtr	= productionPtrVect_[pInd];
                    
                    //  II.B.1.c.I.B.  Ignore if does not expand 'symPtr':
                    if  (prodPtr->getLhsPtr() != symPtr)
                        continue;
                    
                    //  II.B.1.c.I.C.  '*prodPtr' does expand '*symPtr'.  Get the set of
                    //  		   Terminal instances that an expansion of '*symPtr'
                    //		   via '*prodPtr' could begin with:
                    int			prLhsId	= prodPtr->getLhsPtr()->getId();
                    std::set<Terminal*>	set	= prodPtr->
                    createPredictedTerminalSet
                    (*this);
                    
                    //  II.B.1.c.I.D.  Update table to note that when NonTerminal
                    //  		   '*symPtr' appears on the top of the stack, and
                    //		   any one of the Terminal instances in 'set'
                    //		   appear in the input stream, the production
                    //		   '*prodPtr' ought to be done:
                    for  (std::set<Terminal*>::iterator
                          iter	 = set.begin();
                          iter	!= set.end();
                          iter++
                          )
                    {
                        const Terminal*	termPtr	= (*iter);
                        int			termId	= termPtr->getId();
                        
                        //  II.B.1.c.I.D.1.  Complain if this table entry is already taken:
                        if  ( llTableMetaHandle_[prLhsId][termId] != NULL )
                        {
                            snprintf(text,TEXT_LEN,
                                     "Ambiguous action on %s/%s: both "
                                     "productions \"%s\" and \"%s\" apply.",
                                     prodPtr->getLhsPtr()->getName().c_str(),
                                     termPtr->getName().c_str(),
                                     llTableMetaHandle_[prLhsId][termId]->toString().c_str(),
                                     prodPtr->toString().c_str()
                                     );
                            throw text;
                        }
                        
                        //  II.B.1.c.I.D.2.  Update table:
                        llTableMetaHandle_[prLhsId][termId]	= prodPtr;
                        printTable();
                    }
                    
                }
                
            }
        
        //  III.  Finished:
    }
    
    public :
    //  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
    //  PURPOSE:  To initialize '*this' Grammar from its description in
    //		'descriptionStr'.  No return value.
    Grammar		(const std::string&		descriptionStr
                 )
    throw(const char*) :
    startSymbolPtr_(NULL),
    llTableMetaHandle_(NULL)
    {
        int	index	= 0;
        
        parseTerminals(index,descriptionStr);
        parseNonTerminals(index,descriptionStr);
        parseStartSymbol(index,descriptionStr);
        parseProductions(index,descriptionStr);
        factorCommonPrefixes();
        addEndTerms();
        computeDerivesEmpty();
        printStatus();
        buildTable();
    }
    
    //  PURPOSE:  To release resources.  No parameters.  No return value.
    ~Grammar		()
    throw()
    {
        //  I.  Application validity check:
        
        //  II.  Release resources:
        for  (uInt i = 0;  i < symbolTable_.getNumSymbols();  i++)
            free(llTableMetaHandle_[i]);
            
            free(llTableMetaHandle_);
            
            //  III.  Finished:
            }
    
    //  V.  Accessors:
    //  PURPOSE:  To return a pointer to the starting symbol.  No parameters.
    const NonTerminal*
    getStartSymbolPtr
    ()
    const
    throw()
    { return(startSymbolPtr_); }
    
    //  PURPOSE:  To return the number of Symbols in '*this' Grammar.  No
    //	parameters.
    uInt		getNumSymbols
    ()
    const
    throw()
    { return(symbolTable_.getNumSymbols()); }
    
    //  PURPOSE:  To return a pointer to the 'i'-th of Symbol in '*this' Grammar.
    //	No parameters.
    Symbol*	getSymbolPtr
    (uInt	i
     )
    throw()
    { return(symbolTable_.getSymbol(i)); }
    
    //  PURPOSE:  To return the number of Production instances in '*this'
    //	Grammar.  No parameters.
    uInt		getNumProductions
    ()
    const
    throw()
    { return( productionPtrVect_.size() ); }
    
    //  PURPOSE:  To return a pointer to the 'i'-th Production instance.
    Production*	getProductionPtr
    (uInt	i
     )
    const
    throw()
    { return(productionPtrVect_[i]); }
    
    //  VI.  Mutators:
    //  PURPOSE:  To return a pointer to a newly-created NonTerminal instance.
    //	No parameters.
    NonTerminal*	newNonTerminal
    ()
    throw()
    {
        //  I.  Application validity check:
        
        //  II.  Create and return pointer to new NonTerminal instance.
        return(symbolTable_.newNonTerminal());
    }
    
    
    //  PURPOSE:  To add the production pointed to by 'prodPtr' to '*this'
    //	Grammar.  No return value.
    void		addProduction
    (Production*	prodPtr
     )
    throw(const char*)
    {
        //  I.  Application validity check:
        if  (prodPtr == NULL)
            throw "NULL ptr to Grammar::addProduction()";
        
        for  (uInt i = 0;  i < productionPtrVect_.size();  i++)
            if  (productionPtrVect_[i] == prodPtr)
                throw "Attempt to redundantly add existing production to Grammar";
        
        //  II.  Add 'prodPtr' to 'productionPtrVect_':
        productionPtrVect_.push_back(prodPtr);
        
        //  III.  Finished:
    }
    
    
    //  PURPOSE:  To remove the production pointed to by 'prodPtr' from '*this'
    //	Grammar.  No return value.
    void		removeProduction
    (Production*	prodPtr
     )
    throw(const char*)
    {
        //  I.  Application validity check:
        if  (prodPtr == NULL)
            throw "NULL ptr to Grammar::removeProduction()";
        
        //  II.  Add 'prodPtr' to 'productionPtrVect_':
        for  (std::vector<Production*>::iterator
              iter  = productionPtrVect_.begin();
              iter != productionPtrVect_.end();
              iter++
              )
            if  (*iter == prodPtr)
            {
                productionPtrVect_.erase(iter);
                return;
            }
        
        //  III.  Finished (not found):
        throw "Grammar::removeProduction() attempted"
        " to remove non-existing Production";
    }
    
    
    //  VII.  Methods that do main and misc work of this class:
    //  PURPOSE:  To print the status of symbols and productions after all
    //  	info on them has been read or computed, as a debugging exercise.  No
    //	parameters.  No return value.
    void		printStatus
    ()
    throw()
    {
        //  I.  Application validity check:
        
        //  II.  Print status:
        //  II.A.  Print Symbol status:
        std::cout << "Symbols:" << std::endl;
        
        for  (int i = 0;  i < symbolTable_.getNumSymbols();  i++)
        std::cout << symbolTable_.getSymbol(i)->toString(true) << std::endl;
        
        //  II.B.  Print Production status:
        std::cout << std::endl << "Productions:" << std::endl;
        
        for  (int i = 0;  i < getNumProductions();  i++)
        std::cout << getProductionPtr(i)->toString(true) << std::endl;
        
        std::cout << std::endl;
        
        //  III.  Finished:
        }
        
        //  PURPOSE:  To print the Symbol x Terminal -> Production table.  No
        //	parameters.  No return value.
        void		printTable
        ()
        throw()
        {
            //  I.  Application validity check:
            
            //  II.  Print the table:
            //	II.A.  Print top row heading:
            std::cout << '\t';
            
            for  (uInt i = 0;  i < symbolTable_.getNumTerminals();  i++)
                std::cout << symbolTable_.getSymbol(i)->getName() << '\t';
            
            std::cout << std::endl;
            
            //  II.B.  Print table:
            //	II.B.1.  Each iteration prints one row:
            for  (uInt i0 = 0;  i0 < symbolTable_.getNumSymbols();  i0++)
            {
                std::cout << symbolTable_.getSymbol(i0)->getName() << '\t';
                
                for  (uInt i1 = 0;  i1 < symbolTable_.getNumTerminals();  i1++)
                    if  (llTableMetaHandle_[i0][i1] == NULL)
                        std::cout << "--\t";
                    else
                        std::cout << 'p' << llTableMetaHandle_[i0][i1]->getId() << '\t';
                
                std::cout << std::endl;
            }
            
            //  II.C.  Print ending separatory row:
            std::cout << std::endl;
            
            //  III.  Finished:
        }
        
        
        //  PURPOSE:  To attempt to parse the text in 'toParseStr' according to
        //	'*this' Grammar.  No return value.
        void		parse	(const std::string&	toParseStr
                             )
        throw(const char*)
        {
            //  I.  Application validity check:
            
            //  II.  Attempt to parse:
            const int		MAX_STACK_SIZE	= 100;
            
            InputCharStream	inputStream(toParseStr);
            TokenStream		tokenStream(symbolTable_,inputStream);
            const Symbol*	stack[MAX_STACK_SIZE];
            int			stackInd	= -1;
            bool		accepted	= false;
            
            stack[++stackInd]	= getStartSymbolPtr();
            
            while  (!accepted)
            {
                std::cout << "Stack:";
                
                for  (int i = 0;  i < stackInd;  i++)
                    std::cout << " " << stack[i]->getName();
                
                std::cout << " (" << stack[stackInd]->getName() << ")"
                << " / Input: " << tokenStream.peek()->getName()
                << "\n";
                
                if  (stack[stackInd]->getIsTerminal())
                {
                    if  (stack[stackInd] != tokenStream.peek())
                    {
                        snprintf(text,TEXT_LEN,
                                 "Syntax error: Expected %s, found %s",
                                 stack[stackInd]->getName().c_str(),
                                 tokenStream.peek()->getName().c_str()
                                 );
                        throw text;
                    }
                    
                    for  (int i = 0;  i < stackInd;  i++)
                        std::cout << '\t';
                    
                    std::cout << "Matched\n";
                    
                    tokenStream.advance();
                    
                    if  (stack[stackInd] == symbolTable_.find(END_SYMBOL))
                        accepted = true;
                    
                    stackInd--;
                }
                else
                {
                    Production**	handle	= llTableMetaHandle_[stack[stackInd]->getId()];
                    Production*	prodPtr	= handle[tokenStream.peek()->getId()];
                    
                    if  (prodPtr == NULL)
                    {
                        snprintf(text,TEXT_LEN,
                                 "Syntax error: no productions applicable"
                                 " expected symbol %s/found symbol %s",
                                 stack[stackInd]->getName().c_str(),
                                 tokenStream.peek()->getName().c_str()
                                 );
                        throw text;
                    }
                    
                    for  (int i = 0;  i < stackInd;  i++)
                        std::cout << '\t';
                    
                    std::cout << "Popping " << stack[stackInd]->getName() << std::endl;
                    
                    for  (int i = 0;  i < stackInd;  i++)
                        std::cout << '\t';
                    
                    std::cout << "Invoking " << prodPtr->toString() << std::endl;
                    
                    stackInd--;
                    
                    for  (int i = prodPtr->getRhsLength()-1;  i >= 0;  i--)
                    {
                        if  (stackInd >= MAX_STACK_SIZE)
                            throw "Text is too complex to parse";
                        
                        stack[++stackInd]	= prodPtr->getRhsSymbol(i);
                    }
                    
                }
                
            }
            
            //  III.  Finished:
            std::cout << "Parse completed." << std::endl;
        }
        
        };
        
        
        //  PURPOSE:  To print out the members of set 'termSet'.  No return value.
        void	      	printSet(std::set<Terminal*>&	termSet
                                 )
        throw()
        {
            //  I.  Application validity check:
            
            //  II.  Print members:
            for  (std::set<Terminal*>::iterator
                  iter  = termSet.begin();
                  iter != termSet.end();
                  iter++
                  )
                std::cout << (*iter)->toString() << std::endl;
            
            //  III.  Finished:
        }
        
        
        //  PURPOSE:  To copy the items in 'second' into 'first'.  No return value.
        void		unionSecondIntoFirst
        (std::set<Terminal*>&		first,
         const std::set<Terminal*>&	second
         )
        throw()
        {
            //  I.  Application validity check:
            
            //  II.  Copy items from 'second' into 'first':
            for  (std::set<Terminal*>::iterator
                  iter  = second.begin();
                  iter != second.end();
                  iter++
                  )
                first.insert(*iter);
            
            //  III.  Finished:
        }
        
        
        //  PURPOSE:  To recursively create and return a set of pointers to all Symbol
        //	instances that can appear at the beginning of any string to which the
        //	Symbol instances of 'symbolList' may derive according to 'grammar'.
        //	'haveVisitedMap' keeps track of the Terminal instances that have
        //	already been considered.
        std::set<Terminal*>
        internalFirst
        (const std::vector<Symbol*>&	symbolList,
         Grammar&			grammar,
         std::map<Symbol*,bool>&	haveVisitedMap
         )
        throw()
        {
            //  I.  Application validity check:
            
            //  II.  Create and return set:
            std::set<Terminal*>	toReturn;
            
            //  II.A.  Case 1: Handle when 'symbolList' is empty:
            if  (symbolList.size() == 0)
                return(toReturn);
            
            //  II.B.  Case 2: Handle when 'symbolList' begins with a Terminal:
            Symbol*	firstSymbolPtr	= symbolList[0];
            
            if  ( firstSymbolPtr->getIsTerminal() )
            {
                toReturn.insert((Terminal*)firstSymbolPtr);
                return(toReturn);
            }
            
            //  II.C.  Case 3: Handle when 'symbolList' begins with a NonTerminal:
            if  ( !haveVisitedMap[firstSymbolPtr] )
            {
                //  II.C.1.  Note that now have considered NonTerminal '*firstSymbolPtr':
                haveVisitedMap[firstSymbolPtr]	= true;
                
                //  II.C.2.  Find productions that derive '*firstSymbolPtr' to see
                //		 to what they derive:
                //	II.C.2.a.  Each iteration considers one production:
                for  (uInt pi = 0;  pi < grammar.getNumProductions();  pi++)
                {
                    //  II.C.2.a.I.  Get the Production:
                    const Production*	prodPtr = grammar.getProductionPtr(pi);
                    
                    //  II.C.2.a.II.  If it does derive '*firstSymbolPtr' then union its
                    //  		first Terminal instances (determined by its RHS) into
                    //		'toReturn':
                    if  (prodPtr->getLhsPtr() == firstSymbolPtr)
                        unionSecondIntoFirst
                        (toReturn,
                         internalFirst(prodPtr->getRhs(),grammar,haveVisitedMap)
                         );
                }
                
                //  II.C.2.b.  If '*firstSymbolPtr' can derive the empty string then
                //		   add into 'toReturn' those Terminal instances that
                //		   can follow 'firstSymbolPtr' in 'symbolList':
                if  (firstSymbolPtr->canDeriveEmpty())
                {
                    //  II.C.2.b.I.  Build a string of Symbols that is 'symbolList', but
                    //  	       with 'firstSymbolPtr' removed:
                    std::vector<Symbol*>	subsequentList;
                    
                    for  (uInt si = 1;  si < symbolList.size();  si++)
                        subsequentList.push_back(symbolList[si]);
                    
                    //  II.C.2.b.II.  Add into 'toReturn' those Terminal instance that
                    //  		may start its derivations into Terminal instances:
                    unionSecondIntoFirst
                    (toReturn,
                     internalFirst(subsequentList,grammar,haveVisitedMap)
                     );
                }
                
            }
            
            //  III.  Finished:
            return(toReturn);
        }
        
        
        //  PURPOSE:  To create and return a set of pointers to all Symbol instances
        //	that can appear at the beginning of any string to which the Symbol
        //	instances of 'symbolList' may derive according to 'grammar'.
        std::set<Terminal*>
        first	(const std::vector<Symbol*>&	symbolList,
                 Grammar&			grammar
                 )
        throw()
        {
            //  I.  Application validity check:
            
            //  II.  Create and return set:
            //  II.A.  Create and initialize a datastructure that keeps track of the
            //  	     NonTerminal instances that have not yet been visited:
            std::map<Symbol*,bool>	haveVisitedMap;
            
            for  (uInt i = 0;  i < grammar.getNumSymbols();  i++)
                haveVisitedMap[grammar.getSymbolPtr(i)] = false;
            
            //  II.B.  Create and return set:
            return( internalFirst(symbolList,grammar,haveVisitedMap) );
        }
        
        
        //  PURPOSE:  To return 'true' if all of the Symbol instances in 'str' can
        //	derive the empty string, or 'false' otherwise.
        bool		doAllDeriveEmpty
        (std::vector<Symbol*>&	str
         )
        throw()
        {
            //  I.  Application validity check:
            
            //  II.  See if all Symbol instances in 'str' can derive to empty string:
            //  II.A.  Each iteration checks one Symbol instance:
            for  (uInt i = 0;  i < str.size();  i++)
                if  ( !str[i]->canDeriveEmpty() )
                    return(false);
            
            //  III.  Finished:
            return(true);
        }
        
        
        std::set<Terminal*>
        internalFollowTerminalSet
        (NonTerminal*			nonTermPtr,
         Grammar&			grammar,
         std::map<Symbol*,bool>&	haveVisitedMap
         )
        throw()
        {
            //  I.  Application validity check:
            
            //  II.  :
            std::set<Terminal*>	toReturn;
            
            if  ( !haveVisitedMap[nonTermPtr] )
            {
                haveVisitedMap[nonTermPtr]	= true;
                
                for  (uInt pi = 0;  pi < grammar.getNumProductions();  pi++)
                {
                    uInt		ri;
                    const Production*	prodPtr = grammar.getProductionPtr(pi);
                    
                    for  (ri = 0;  ri < prodPtr->getRhsLength();  ri++)
                        if  (nonTermPtr == prodPtr->getRhsSymbol(ri))
                            break;
                    
                    if  (ri >= prodPtr->getRhsLength())
                        continue;
                    
                    std::vector<Symbol*>	tail;
                    
                    for  (uInt i = ri + 1;  i < prodPtr->getRhsLength();  i++)
                        tail.push_back(prodPtr->getRhsSymbol(i));
                    
                    unionSecondIntoFirst(toReturn,first(tail,grammar));
                    
                    if  ( doAllDeriveEmpty(tail) )
                    {
                        const NonTerminal* lhs	= prodPtr->getLhsPtr();
                        
                        unionSecondIntoFirst
                        (toReturn,
                         internalFollowTerminalSet
                         (prodPtr->getLhsPtr(),
                          grammar,
                          haveVisitedMap
                          )
                         );
                    }
                    
                }
                
            }
            
            //  III.  Finished:
            return(toReturn);
        }
        
        
        //  PURPOSE:  To create and return the set of NonTerminal instances that can
        //	follow '*nonTermPtr' in 'grammar'.
        std::set<Terminal*>
        createFollowTerminalSet
        (NonTerminal*		nonTermPtr,
         Grammar&		grammar
         )
        throw()
        {
            //  I.  Application validity check:
            
            //  II.  Create and initialize a datastructure that keeps track of the
            //  	   NonTerminal instances that have not yet been visited:
            std::map<Symbol*,bool>	haveVisitedMap;
            
            for  (uInt i = 0;  i < grammar.getNumSymbols();  i++)
                haveVisitedMap[grammar.getSymbolPtr(i)] = false;
            
            //  III.  Finished:
            return( internalFollowTerminalSet(nonTermPtr,grammar,haveVisitedMap) );
        }
        
        
        //  PURPOSE:  To create and return a std::set<> of Terminal instances that
        //	may begin expansions of '*this' Production in Grammar 'grammar'.
        std::set<Terminal*>
        Production::createPredictedTerminalSet
        (Grammar&	grammar
         )
        const
        throw()
        {
            //  I.  Application validity check:
            
            //  II.  Compute set of Terminal instances:
            //  II.A.  Obtain the Terminal instances that can be at the beginning of
            //	     'rhs_'.
            std::set<Terminal*>	toReturn= first(getRhs(),grammar);
            
            //  II.B.  If 'rhs_' can derive the empty string, then also include the
            //  	     Terminal instances that can follow the LHS in other productions:
            if  ( canDeriveEmpty() )
            {
                std::set<Terminal*>	temp	= createFollowTerminalSet(getLhsPtr(),grammar);
                
                unionSecondIntoFirst(toReturn,temp);
            }
            
            //  III.  Finished:
            std::cout << "The predicted term set of " << toString()
            << " is:" << std::endl;
            printSet(toReturn);
            
            return(toReturn);
        }
        
        
        //  PURPOSE:  To ask the user for the definition of a grammar, and then a
        //	sentence to parse.  Then attempts to parse the sentence according to
        //	the LL(1) rules for the parser.  Ignores command line parameters.
        int		main	()
        {
            //  I.  Application validity check:
            
            //  II.  Do program:
            //  II.A.  Get grammar:
            std::string		grammarStr;
            int			status	= EXIT_SUCCESS;
            
            std::cout
            << "Please enter the grammar string like:\n"
            << "{a,b},{S,A,B},S,{S-A B, A-a, A-, B-b, B-}\n"
            << "{+,*},{S,E,T},S,{S - E,E - E + T,E - T,T - T * #i,T - #i}\n"
            << "{a,b,+},{S,Exprs,E,F},S,{S-Exprs,Exprs-E a,Exprs-F b,E-E + #i,E-#i,F-F + #i,F-#i}\n"
            << "(No need to define terminals $=end, #i=int const, #f=float const)\n"
            << "Your grammar: ";
            std::getline(std::cin,grammarStr);
            
            try
            {
                std::string	sentenceStr;
                Grammar	grammar(grammarStr);
                
                std::cout << "Please enter text to parse: ";
                std::getline(std::cin,sentenceStr);
                
                grammar.parse(sentenceStr);
            }
            catch  (const char*	errCPtr)
            {
                std::cerr << errCPtr << std::endl;
                status	= EXIT_FAILURE;
            }
            
            //  III.  Finished:
            return(status);
        }
        
        
        
        
        // Grammar #1:
        // {a,b},{S,A,B},S,{S-a B, B - b}
        
        // Grammar #2:
        // {a,b},{S,A,B},S,{S-}
        
        // Grammar #3:
        // {a,b},{S,A},S,{S-A,A-}
        
        // Grammar #4:
        // {a,b},{S,A},S,{S-a A,A-b,A-}
        
        // Grammar #5:
        // {a,b},{S,A,B},S,{S-A B, A-a, A-, B-b, B-}
        // Legal strings: a, b, a b, (empty)
        
        // Grammar #6: ("AC" grammar)
        // {float,int,id,=,print,+},{Prog,Dcls,Dcl,Stmts,Stmt,Expr,Val},Prog,{Prog - Dcls Stmts,Dcls - Dcl Dcls, Dcls - ,Dcl - float id, Dcl - int id, Stmts - Stmt Stmts, Stmts - , Stmt - id = Val Expr, Stmt - print id, Expr - + Val Expr, Expr - , Val - id, Val - #i , Val - #f}
        // Example input: float id int id id = 5 id = id + 5.5 print id
        
        // Grammar #7: Two rules with common preface
        // {if,then,else,endif,var,;,+},{Stmt,StmtList,Expr},StmtList,{Stmt - if Expr then StmtList endif, Stmt - if Expr then StmtList else StmtList endif, StmtList - StmtList ; Stmt, StmtList - Stmt, Expr - var + Expr, Expr - var}
        // Grammar #7B: Two rules with common preface
        //  {if,then,else,endif,var,+},{S,Stmt,Expr},S,{S - Stmt, Stmt - if Expr then Stmt endif, Stmt - if Expr then Stmt else Stmt endif, Stmt - Expr, Expr - var + Expr, Expr - var}
        // Strings to test with Grammar #7B:
        // "if var then var + var endif"
        // "if var + var then var else var + var endif"
        // "if var + var then var else if var + var then var else var endif endif"
        // "if var + var then if var then var else var endif else if var + var then var else var endif endif"
