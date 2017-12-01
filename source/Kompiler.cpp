#include <iostream>
#include "Kompiler.hpp"
#include "Lexer.hpp"

static DefinedValue idDebugPrefix(Kompiler &, Value &&, DefinedValue &&val)
{
  std::cout << "Applying prefix!\n";
  return std::move(val);
}

static DefinedValue idDebugPostfix(Kompiler &, Value &&, DefinedValue &&val)
{
  std::cout << "Applying prefix!\n";
  return std::move(val);
}

static DefinedValue asInt(Kompiler &, Value &&, DefinedValue &&val)
{
  auto const &token(std::get<std::unique_ptr<Token>>(val.value[0]));
  long unsigned int ret(0u);

  // TODO: wait for <char_conv> or do something serious
  for (auto it(token->content.begin()); it != token->content.end(); ++it)
    {
      ret *= 10;
      ret += *it - '0';
    }

  return makePrimitiveDefinedValue(ret);
}

static DefinedValue intOnly(Kompiler &, Value &&, DefinedValue &&val)
{
  std::cout << "Applying INT_ONLY!\n";
  return std::move(val);
}

void Kompiler::createDefaultDefinitions() 
{
  prefixes["ID_DEBUG"].addFunc<&idDebugPrefix>({});
  postfixes["ID_DEBUG"].addFunc<&idDebugPostfix>({});
  prefixes["asInt"].addFunc<&asInt>({PropertyList::getPrimitiveProperty<std::unique_ptr<Token>>()});
  prefixes["INT_ONLY"].addFunc<&intOnly>({PropertyList::getPrimitiveProperty<long unsigned int>()});
  postfixes["+"].addFunc({{PropertyList::getPrimitiveProperty<long unsigned int>()},
	[](Kompiler &, Value &&, DefinedValue &&val)
	  -> std::variant<DefinedValue, std::pair<Value, UnaryOperator>>
	  {
	    UnaryOperator add;

	    add.addFunc({{PropertyList::getPrimitiveProperty<long unsigned int>()},
		  [](Kompiler &, Value &&left, DefinedValue &&right)
		    -> std::variant<DefinedValue, std::pair<Value, UnaryOperator>>
		    {
		      return {std::move(makePrimitiveDefinedValue(std::get<long unsigned int>(left[0]) + std::get<long unsigned int>(right.value[0])))};
		    }});
	    return {std::move(std::pair<Value, UnaryOperator>{std::move(val.value), add})};
	  }});

  postfixes["="].addFunc({{PropertyList::getPrimitiveProperty<std::unique_ptr<Token>>()},
  	[](Kompiler &, Value &&, DefinedValue &&val)
  	  -> std::variant<DefinedValue, std::pair<Value, UnaryOperator>>
  	  {
  	    UnaryOperator set;

  	    set.addFunc({{},
  		  [](Kompiler &kompiler, Value &&left, DefinedValue &&right)
  		    -> std::variant<DefinedValue, std::pair<Value, UnaryOperator>>
  		    {
  		      kompiler.values.emplace(std::get<std::unique_ptr<Token>>(left[0])->content, copy(right));
  		      return {std::move(right)};
  		    }});
  	    return {std::move(std::pair<Value, UnaryOperator>{std::move(val.value), set})};
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
