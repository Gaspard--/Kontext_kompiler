#include <iostream>
#include "Kompiler.hpp"
#include "Lexer.hpp"

void Kompiler::createDefaultDefinitions() 
{
  prefixes["ID_DEBUG"].addFunc({{}, [](Value const &, DefinedValue const &val)
				      -> std::variant<DefinedValue, std::pair<Value, UnaryOperator>>
				      {
					std::cout << "Applying prefix!\n";
					return val;
				      }});
  postfixes["ID_DEBUG"].addFunc({{}, [](Value const &, DefinedValue const &val)
				       -> std::variant<DefinedValue, std::pair<Value, UnaryOperator>>
				       {
					 std::cout << "Applying postfix!\n";
					 return val;
				       }});
  postfixes["asInt"].addFunc({{PropertyList::getPrimitiveProperty<std::shared_ptr<Token>>()},
	[](Value const &, DefinedValue const &val)
	  -> std::variant<DefinedValue, std::pair<Value, UnaryOperator>>
	  {
	    auto const &token(std::get<std::shared_ptr<Token>>(val.value[0]));
	    long unsigned int ret(0u);

	    // TODO: wait for <char_conv> or do something serious
	    for (auto it(token->content.begin()); it != token->content.end(); ++it)
	      {
		ret *= 10;
		ret += *it - '0';
	      }

	    return makePrimitiveDefinedValue(ret);
	  }});
  prefixes["INT_ONLY"].addFunc({{PropertyList::getPrimitiveProperty<long unsigned int>()},
	[](Value const &, DefinedValue const &val)
	  -> std::variant<DefinedValue, std::pair<Value, UnaryOperator>>
	  {
	    std::cout << "Applying INT_ONLY prefix!\n";
	      
	    return val;
	  }});
  postfixes["+"].addFunc({{PropertyList::getPrimitiveProperty<long unsigned int>()},
	[](Value const &, DefinedValue const &val)
	  -> std::variant<DefinedValue, std::pair<Value, UnaryOperator>>
	  {
	    UnaryOperator add;

	    add.addFunc({{PropertyList::getPrimitiveProperty<long unsigned int>()},
		  [](Value const &left, DefinedValue const &right)
		    -> std::variant<DefinedValue, std::pair<Value, UnaryOperator>>
		    {
		      return makePrimitiveDefinedValue(std::get<long unsigned int>(left[0]) + std::get<long unsigned int>(right.value[0]));
		    }});
	    return std::pair<Value, UnaryOperator>{val.value, add};
	  }});
}

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

 std::cout << "> ";
 while (begin != std::istreambuf_iterator<char>{})
    {
      std::string line{};

      for (; !isEndOfLine(*begin); ++begin)
	line += *begin;
      parseLine(line);
      for (; isEndOfLine(*begin); ++begin)
	std::cout << "> ";
    }
}
