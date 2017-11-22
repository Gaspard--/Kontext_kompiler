#include <iostream>
#include <fstream>

#include "Kompiler.hpp"

int main(int ac, char **argv)
{
  if (ac > 2)
    {
      std::cerr << "usage: " << argv[0] << " mainFile" << std::endl
		<< "Expected main file." << std::endl;
      return (1);
    }
  Kompiler kompiler;

  try {
    if (ac == 2)
      {
	std::string file(argv[1]);
	std::ifstream stream(file);

	if (!stream)
	  {
	    std::cerr << "usage: " << argv[0] << " mainFile" << std::endl
		      << "Invalid file " << file << "." << std::endl;
	    return (1);
	  }
	kompiler.process(stream);
      }
    else
      kompiler.process(std::cin);
  } catch (std::runtime_error const &e) {
    std::cerr << e.what() << std::endl;
    return (1);
  }
}
