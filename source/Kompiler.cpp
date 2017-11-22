#include <iostream>
#include "Kompiler.hpp"

void Kompiler::process(std::istream &stream)
{
  auto isEndOfLine([](char c)
		   {
		     return c == '\n' || c == ';';
		   });
  /**
   * Note that this iterator is single pass (this explains the code below)
   */
 std::istreambuf_iterator<char> begin(stream);

 while (begin != std::istreambuf_iterator<char>{})
    {
      std::string line{};

      for (; !isEndOfLine(*begin); ++begin)
	line += *begin;
      parseLine(line);
      for (; isEndOfLine(*begin); ++begin);
    }
}
