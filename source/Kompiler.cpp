#include <iostream>
#include "Kompiler.hpp"

void Kompiler::process(std::istream &stream)
{
  test.emplaceToken("BigToken", 1);
  test.emplaceToken("SmallToken", 2);
  test.emplaceToken("Yes", 3);
  std::istreambuf_iterator<char> start(stream);

  while (start != std::istreambuf_iterator<char>{})
    {
      std::istreambuf_iterator<char> best;
      auto tokenRange(test.getRootTokenRange());
      auto bestToken{tokenRange.begin};

      for (auto it(start); !tokenRange.isEmpty() && it != std::istreambuf_iterator<char>{}; ++it)
	{
	  tokenRange = tokenRange[*it];
	  if (tokenRange.hasExactMatch())
	    {
	      best = it;
	      bestToken = tokenRange.begin;
	    }
	}
      if (best == std::istreambuf_iterator<char>{})
	{
	  // TODO: no token matched
	  std::cerr << "No token match found." << std::endl;
	  return ;
	}
      std::cout << "Token id : " << bestToken->second << std::endl;
      start = best;
    }
    
}
