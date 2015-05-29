#include	<map>

typedef	std::map<std::string,double> 	MapType;

class	VarStore
{
  MapType	map_;

public :
  VarStore	()
		{ }

  ~VarStore	()
		{ }

  void	assign	(const char*	varNamePtr,
		 double		value
		)
  {
    map_[std::string(varNamePtr)]	= value;
  }

  double
	retrieve(const char*	varNamePtr
		)
  {
    MapType::iterator	iter	= map_.find(std::string(varNamePtr));

    return( (iter == map_.end()) ? 0 : iter->second );
  }

};
